
#ifndef EPT_MAIN_H_
#define EPT_MAIN_H_

#include <string.h>
#include "etpCommontype.h"

#ifdef _WIN32
#include <windows.h>
#include "stdafx.h"
#include <direct.h>
#include <io.h>
#ifdef ETP_DLL_EXPORT
#define DLL_API __declspec(dllexport)
#elif defined(ETP_USE_DLL)
#define DLL_API __declspec(dllimport)
#elif defined(ETP_USE_LIB)
#define DLL_API 
#else
#define DLL_API 
#endif

#else
#include <unistd.h>
#include <sys/stat.h>
#define DLL_API extern
#endif


#ifdef __cplusplus
extern "C"
{
#endif


#ifdef _WIN32

//#define EINVAL  WSAEINVAL
//#define EAGAIN  WSAEWOULDBLOCK
//#define EINTR   WSAEINTR
//#define EWOULDBLOCK   WSAEWOULDBLOCK
//#define EACCES   WSAEACCES


#define MUTEX_TYPE            CRITICAL_SECTION
#define MUTEX_SETUP(x)        InitializeCriticalSection(&(x))
#define MUTEX_SETUP_ATTR(x, attr)        InitializeCriticalSection(&(x))
#define MUTEX_CLEANUP(x)      DeleteCriticalSection(&(x))
#define MUTEX_LOCK(x)         EnterCriticalSection(&(x))
#define MUTEX_UNLOCK(x)       LeaveCriticalSection(&(x))
#define THREAD_ID             GetCurrentThreadId()

#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#define STRDUP(a) _strdup(a)


#define VSNPRINTF(buf, len, format, ap)\
		vsnprintf_s(buf, len, _TRUNCATE, format, ap);

#define SNPRINTF(buf, len, format, ...)\
		_snprintf_s(buf, len, _TRUNCATE, format, ##__VA_ARGS__);

typedef int socklen_t;


static inline void sleep_s(int second)
{
	Sleep(second*1000);
}

static inline char* str_error_s(char *err_buf, unsigned int len, int errnum)
{
	SNPRINTF(err_buf, len, "%d", errnum);
	return err_buf;
}

#else

#define MUTEX_TYPE            pthread_mutex_t
#define MUTEX_SETUP(x)        pthread_mutex_init(&(x), NULL)
#define MUTEX_SETUP_ATTR(x, attr)  pthread_mutex_init(&(x), attr)
#define MUTEX_CLEANUP(x)      pthread_mutex_destroy(&(x))
#define MUTEX_LOCK(x)         pthread_mutex_lock(&(x))
#define MUTEX_UNLOCK(x)       pthread_mutex_unlock(&(x))
#define THREAD_ID             pthread_self()

#define ACCESS access
#define MKDIR(a) mkdir((a),0777)
#define STRDUP(a) strdup(a)

#define VSNPRINTF(buf, len, format,ap)\
		vsnprintf(buf, len, format, ap);

#define SNPRINTF(buf, len, format, ...)\
		snprintf(buf, len, format, ##__VA_ARGS__);


static inline void sleep_s(int second)
{
	sleep(second);
}

static inline char* str_error_s(char *err_buf, unsigned int len, int errnum)
{
	return strerror_r(errnum, err_buf, len);
}
#endif



#define MAX_NAME_LEN 256

enum
{
	L_DEBUG = 0,
	L_INFO,
	L_WARN,
	L_ERROR,

	L_MAX,
};

#define EL_LOG_DEBUG(format, ...) \
		logger_write(L_DEBUG, format, ##__VA_ARGS__);

#define EL_LOG_ERROR(format, ...) \
		logger_write(L_ERROR, format, ##__VA_ARGS__);

#define EL_LOG_WARN(format, ...) \
		logger_write(L_WARN, format, ##__VA_ARGS__);

#define EL_LOG_INFO(format, ...) \
		logger_write(L_INFO, format, ##__VA_ARGS__);


DLL_API int loggger_init(char *log_path, char *mod_name,
					unsigned int maxfilekb, unsigned int maxfilecnt,
					BOOL isAsynWr);
DLL_API void loggger_exit();
DLL_API void logger_write(int level, const char *format, ...);
DLL_API void logger_set_level(int level);


DLL_API int CreatDir(char *pDir);

enum
{
	ETP_RCV_ERROR = -1,
	ETP_RCV_NODATA = -2,
};


typedef void (*accept_hdl_func)(int  fd, void* param1);
typedef void (*read_hdl_func)(int  fd, void* param1,
		struct sockaddr *cliAddr,
		char *recvBuf, int recvLen);
typedef void (*write_hdl_func)(int  fd, void* param1);

typedef void (*evt_hdl_func)(void* param1, void* param2, void* param3);
typedef void (*expire_hdl_func)(void* param1, void* param2, void* param3);

typedef int (*thrd_init_func)();
typedef void (*thrd_exit_func)();

DLL_API BOOL etp_sock_set_block(int fd);
DLL_API BOOL etp_sock_set_unblock(int fd);
DLL_API int etp_sock_safe_recv(int fd, char *buf, size_t len, BOOL *is_expired);
DLL_API int etp_sock_safe_send(int fd, char *buf, size_t len, BOOL *is_expired);
DLL_API int etp_sock_send_line(int fd, const char *format, ...);
DLL_API int etp_sock_recv_line(int fd, char *buf, size_t bufsiz, int eof_ok);



DLL_API BOOL etp_init_listen_thrds(unsigned int max_thrd_cnt,
					unsigned int start_core,
					unsigned int core_cnt);
DLL_API void* etp_init_evt_thrds(unsigned int max_thrd_cnt,
		unsigned int start_core,
		unsigned int core_cnt,
		thrd_init_func init_func,
		thrd_exit_func exit_func);
DLL_API void etp_free_evt_thrds(void* thrdPool);
DLL_API unsigned int etp_evt_jobs_cnt(void* thrdPool);


DLL_API BOOL etp_init();
DLL_API void etp_free();

DLL_API BOOL etp_start();
DLL_API void etp_stop();
DLL_API void etp_set_stop_flag();


DLL_API BOOL etp_add_listen_job(accept_hdl_func acpt_func, int fd, void* param1);
DLL_API BOOL etp_del_listen_job(int  fd);

DLL_API BOOL etp_add_read_job(void *thrdPool, read_hdl_func read_func,
				int  fd, void* param1,
				struct sockaddr *cliAddr,
				int bufferSize,
				BOOL isTcp = true);
DLL_API BOOL etp_del_read_job(int  fd);

DLL_API BOOL etp_add_write_job(void *thrdPool, write_hdl_func write_func,
				int  fd, void* param1);
DLL_API BOOL etp_del_write_job(int  fd);

DLL_API BOOL etp_add_evt_job(void *thrdPool,
		evt_hdl_func evt_func, void* param1, void* param2, void *param3);

DLL_API BOOL etp_add_time_job(void *thrdPool,
				expire_hdl_func expire_func,
				void* param1, void* param2, void* param3,
				unsigned int time_value,
				BOOL isOnce);
DLL_API BOOL etp_del_time_job(expire_hdl_func expire_func, void* param1);

#ifdef __cplusplus
}
#endif


#endif /* EPT_H_ */
