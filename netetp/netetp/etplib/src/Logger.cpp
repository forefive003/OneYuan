#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <dirent.h>
#include <sys/time.h>
#endif

#include "etpLib.h"

#define MAX_SIZE_KB (8 * 1024) //8MB的日志大小

#define MAX_LINE_LEN    2048
#define QUEUE_LEN 512 //申请512个空间的队列

typedef struct log_q {
      char data[QUEUE_LEN + 1][MAX_LINE_LEN + 1];//浪费一个节点f 用来区分'满'和'空'
      int front, rear;

      MUTEX_TYPE lock;

#ifndef _WIN32
		pthread_mutex_t job_lock;
		pthread_cond_t job_notify;
#else
		HANDLE event;
#endif
} log_queue;


#ifdef _WIN32
static HANDLE g_thd_handle;
static unsigned int g_thd_ID;
#else
static pthread_t g_thd_handle;
#endif

static BOOL g_thd_run = FALSE;

static unsigned int g_cur_files_cnt = 0;
static log_queue g_log_queue;

static unsigned int g_logfile_max_kb = MAX_SIZE_KB;
static unsigned int g_logfile_max_cnt = 8;
static unsigned int g_isAsyncWrite = FALSE;

static char g_log_path[MAX_NAME_LEN + 1] = {0};
static char g_log_name[MAX_NAME_LEN + 1] = {0};
static int g_log_level = L_INFO;


// default_size : The size of Linked Queue by deault
static inline void init_queue(log_queue *lq){
      lq->front = lq->rear = 0;

      MUTEX_SETUP(lq->lock);

#ifndef _WIN32
	pthread_mutex_init(&lq->job_lock, NULL);
	pthread_cond_init(&lq->job_notify, NULL);
#else
	lq->event = CreateEvent(NULL,FALSE,FALSE,NULL);
#endif
}

static inline void free_queue(log_queue *lq)
{
	MUTEX_CLEANUP(lq->lock);
}

static inline void queue_in(log_queue *lq, char *data){
      if(lq->front == (lq->rear + 1) % (QUEUE_LEN + 1)){
    	    static int count = 1;
    	    count++;
    	    if (count % 10000)
    	    {
    	    	printf("log queue is full\n");
    	    }
            return;
      }
      lq->rear = (lq->rear + 1) % (QUEUE_LEN + 1);

      memset(lq->data[lq->rear], 0, MAX_LINE_LEN);
      strncpy(lq->data[lq->rear], data, MAX_LINE_LEN);
}

static inline int queue_out(log_queue *lq){
    int index = 0;
	if(lq->front == lq->rear){
		return -1;
	}

    index = lq->front;
    lq->front = (lq->front + 1) % (QUEUE_LEN + 1);
    return index + 1;
}

static unsigned long get_cur_logfile_size(char *logName)
{
    unsigned long filesize = 0;
    char temp[128] = {0};
	SNPRINTF(temp, 127, "%s/%s.log", g_log_path, logName);

    struct stat statbuff;
    if(stat(temp, &statbuff) < 0)
    {
        return filesize;
    }else
    {
        filesize = statbuff.st_size;
    }

    return filesize;
}

static void file_size_check(char *logName){
    unsigned long size = get_cur_logfile_size(logName);

    if((size >> 10) >= g_logfile_max_kb)
    {
        char oldFile[128] = {0}, newFile[128] = {0};
        SNPRINTF(oldFile, 127, "%s/%s.log", g_log_path, g_log_name);

        if (g_cur_files_cnt >= g_logfile_max_cnt)
        {
        	g_cur_files_cnt = 0;
        }

		SNPRINTF(newFile, 127, "%s.%d", oldFile, g_cur_files_cnt+1);

        rename(oldFile, newFile);//重命名

        g_cur_files_cnt += 1;
#if 0
        FILE *pfd = fopen(oldFile, "a");
		if (NULL == pfd)
		{
			fprintf(stderr, "open %s failed.\n", oldFile);
		}
		else
		{
			chmod(oldFile, S_IRWXU | S_IRWXG | S_IRWXO);
			fclose(pfd);
		}
#endif
    }
}

#ifdef _WIN32
static unsigned int get_cur_logfile_cnt(char *logName){
	WIN32_FIND_DATA ffd;
	char szDir[MAX_NAME_LEN + 1];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError=0;

	SNPRINTF(szDir, MAX_NAME_LEN, "%s\\*.*", g_log_path);//这里一定要指明通配符，不然不会读取所有文件和目录

	hFind = FindFirstFile(szDir, &ffd);

	int count = 0;
	// List all the files in the directory with some info about them.
	do
	{
	   if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	   {
		   if(strstr(ffd.cFileName, logName) != NULL)
		   {
			   count++;
		   }
	   }
	}
	while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

    return count;
}

unsigned int WINAPI async_fputs(void * arg)
{
	int index = -1;

	char fileName[128] = {0};
	SNPRINTF(fileName, 127, "%s/%s.log", g_log_path, g_log_name);

	file_size_check(g_log_name);

	int count = 0;

	while(g_thd_run)
	{
		//读取队列首中的串, 并写入日志文件
        /*wait*/
		DWORD waittime = 2000;
		DWORD dReturn = WaitForSingleObject(g_log_queue.event, waittime);

		while(-1 != (index = queue_out(&g_log_queue)))
		{
			FILE *file = NULL;
			if((file = fopen(fileName, "a+")) == NULL)
			{
				//返回值不可能为NULL
				printf("[error]open log %s file failed.\n", fileName);
				continue;
			}

			fputs((g_log_queue.data)[index], file);
			fclose(file);

			count++;
			if(count % 100 == 0)
			{
				file_size_check(g_log_name);
			}
		}
	}
	return 1;
}
#else
static unsigned int get_cur_logfile_cnt(char *logName){
    DIR *dir = NULL;
    struct dirent *dptr = NULL;
    if(!(dir = opendir(g_log_path))){
        printf("[error]open dir %s failed", g_log_path);
        exit(-1);
    }

    int count = 0;
    while(NULL != (dptr = readdir(dir))){
        if(dptr->d_type != DT_DIR){
        	if(strstr(dptr->d_name, logName) != NULL)
			{
        		count++;
			}
        }
    }
    return count;
}

static void *async_fputs(void *arg)
{
	int index = -1;

	char fileName[128] = {0};
	SNPRINTF(fileName, 127, "%s/%s.log", g_log_path, g_log_name);

	file_size_check(g_log_name);

	int count = 0;

	while(g_thd_run)
	{
		//读取队列首中的串, 并写入日志文件
        /*wait*/
        struct timespec beattime;
		struct timeval now;

		gettimeofday(&now, NULL);
		beattime.tv_sec = now.tv_sec + 2;
		beattime.tv_nsec = now.tv_usec * 1000;

		pthread_mutex_lock(&g_log_queue.job_lock);
		pthread_cond_timedwait(&g_log_queue.job_notify, &g_log_queue.job_lock, &beattime);
		pthread_mutex_unlock(&g_log_queue.job_lock);

		while(-1 != (index = queue_out(&g_log_queue)))
		{
			FILE *file = NULL;
			if((file = fopen(fileName, "a+")) == NULL)
			{
				//返回值不可能为NULL
				printf("[error]open log %s file failed.\n", fileName);
				continue;
			}

			fputs((g_log_queue.data)[index], file);
			fclose(file);

			count++;
			if(count % 100 == 0)
			{
				file_size_check(g_log_name);
			}
		}
	}
	pthread_exit((void *)1);
}
#endif

DLL_API int loggger_init(char *log_path, char *mod_name,
		unsigned int maxfilekb, unsigned int maxfilecnt,
		BOOL isAsynWr)
{
	strncpy(g_log_path, log_path, MAX_NAME_LEN);
	strncpy(g_log_name, mod_name, MAX_NAME_LEN);
	g_logfile_max_kb = maxfilekb;
	g_logfile_max_cnt = maxfilecnt;
	g_isAsyncWrite = isAsynWr;

	struct stat s;
	if(-1 == stat(g_log_path, &s))
	{
		CreatDir(g_log_path);
	}

	g_cur_files_cnt = get_cur_logfile_cnt(g_log_name);

	if (g_isAsyncWrite)
	{
		init_queue(&g_log_queue);

		g_thd_run = TRUE;
	#ifndef _WIN32
		pthread_create(&g_thd_handle, NULL, async_fputs, NULL);
	#else
		g_thd_handle = (HANDLE)_beginthreadex(NULL, 0,
								async_fputs, NULL,
								0, &g_thd_ID);
	#endif
	}

    return 0;
}

DLL_API void loggger_exit()
{
	if (g_isAsyncWrite)
	{
		g_thd_run = FALSE;

	#ifndef _WIN32
		pthread_join(g_thd_handle, NULL);
	#else
		unsigned long ExitCode = 0;
		if(::TerminateThread(g_thd_handle, ExitCode))
		{
			::CloseHandle(g_thd_handle);
			return;
		}
	#endif

		free_queue(&g_log_queue);
	}
}

DLL_API void logger_write(int level, const char *format, ...)
{// 一整天过去之后怎么轮转日志???
    char src[MAX_LINE_LEN + 1] = {'\0'};
    char dateformat[64] = {'\0'};
    char prefixstr[64] = {0};

    if (level < g_log_level)
    {
    	return;
    }

    va_list argp;

    time_t timer;
	struct tm *now = NULL;
    time(&timer);
    now = localtime(&timer);
    sprintf(dateformat, "[%04d-%02d-%02d %02d:%02d:%02d]", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

    va_start(argp, format);
    VSNPRINTF(src, MAX_LINE_LEN, format, argp);
    va_end(argp);

    //往 线程维护的循环队列 里面插入要写入的串
    switch(level){
        case L_ERROR://error级别
        	SNPRINTF(prefixstr, sizeof(prefixstr), "%s [ERR]:", dateformat);
        	break;
        case L_DEBUG://debug级别
        	SNPRINTF(prefixstr, sizeof(prefixstr), "%s [DBG]:", dateformat);
        	break;
        case L_WARN://warn级别
        	SNPRINTF(prefixstr, sizeof(prefixstr), "%s [WARN]:", dateformat);
        	break;
        case L_INFO://info级别
        	SNPRINTF(prefixstr, sizeof(prefixstr), "%s [INFO]:", dateformat);
            break;
        default:
        	SNPRINTF(prefixstr, sizeof(prefixstr), "%s [DEF]:", dateformat);
            break;
    }

    if (g_isAsyncWrite)
    {
		MUTEX_LOCK(g_log_queue.lock);
		queue_in(&g_log_queue, prefixstr);
		queue_in(&g_log_queue, src);
		queue_in(&g_log_queue, (char*)"\n");
		MUTEX_UNLOCK(g_log_queue.lock);

#ifndef _WIN32
		pthread_mutex_lock(&g_log_queue.job_lock);
		pthread_cond_broadcast(&g_log_queue.job_notify);
		pthread_mutex_unlock(&g_log_queue.job_lock);
#else
		SetEvent(g_log_queue.event);
#endif
    }
    else
    {
    	char fileName[128] = {0};
		SNPRINTF(fileName, 127, "%s/%s.log", g_log_path, g_log_name);

    	file_size_check(g_log_name);

		FILE *file = NULL;
		if((file = fopen(fileName, "a+")) == NULL)
		{
			//返回值不可能为NULL
			printf("[error]open log %s file failed.\n", fileName);
			return;
		}

		fputs(prefixstr, file);
		fputs(src, file);
		fputs((char*)"\n", file);
		fclose(file);
    }
}

DLL_API void logger_set_level(int level)
{
	g_log_level = level;
}
