/*
 * EventPoll.h
 *
 *  Created on: 2015骞�4鏈�25鏃�
 *      Author: cht
 */

#ifndef CEVTPOLL_H_
#define CEVTPOLL_H_

#include <set>
#include "list.h"
#include "Thread.h"
#include "ThreadPool.h"


enum
{
	RECV_OK = 0,
	RECV_ERROR,
	RECV_TIMEOUT,
};


#define BIGPATHBUFLEN (1024)

#define EVENTMAX 1024

#ifdef _WIN32
#define EPOLL_TIMEOUT 1  /*1S*/

#define IO_FLAG_READ 0x01
#define IO_FLAG_WRITE 0X02

#else
#define EPOLL_TIMEOUT 1000  /*1S*/
#endif

#define THRD_AGE_TIME 10


typedef struct
{
	struct list_head node;

	accept_hdl_func accept_func;

	read_hdl_func  read_func;
	write_hdl_func  write_func;
	int  fd;
	void *param1;
	int now_event;

	int bufferSize;
	CThreadPool *thrdPool;
	struct sockaddr cliAddr;
	BOOL isTcp;

	BOOL isWriting;
	BOOL isDeleting;
}IO_NODE_T;


typedef struct
{
	struct list_head node;

	expire_hdl_func  job_func;
	void *param1;
	void *param2;
	void *param3;

	unsigned long long time_value;
	unsigned long long expire_time;
	BOOL isOnce;

	CThreadPool *thrdPool;
}TIME_NODE_T;

class CEvtTask : public Runnable
{
public:
	CEvtTask(evt_hdl_func evt_func,	void* param1,void* param2, void* param3)
	{
		m_evt_func = evt_func;
		m_param1 = param1;
		m_param2 = param2;
		m_param3 = param3;
	}
	~CEvtTask(void)
	{

	}

	void Run()
	{
		m_evt_func(m_param1, m_param2, m_param3);
	}

private:
	evt_hdl_func m_evt_func;
	void* m_param1;
	void* m_param2;
	void* m_param3;
};

class CAcptTask : public Runnable
{
public:
	CAcptTask(accept_hdl_func io_func, int  fd, void* param1)
	{
		m_fd = fd;
		m_param1 = param1;
		m_io_func = io_func;
	}
	~CAcptTask(void)
	{

	}

	void Run()
	{
		m_io_func(m_fd, m_param1);
	}

private:
	int  m_fd;
	void* m_param1;
	accept_hdl_func m_io_func;
};


class CReadTask : public Runnable
{
public:
	CReadTask(read_hdl_func io_func, int  fd, void* param1,
				struct sockaddr *cliAddr,
				char *recvBuf, int recvLen)
	{
		m_fd = fd;
		m_param1 = param1;
		m_io_func = io_func;

		memcpy(&m_cliAddr, cliAddr, sizeof(struct sockaddr));
		m_recvBuf = recvBuf;
		m_recvLen = recvLen;
	}
	~CReadTask(void)
	{

	}

	void Run()
	{
		m_io_func(m_fd, m_param1, &m_cliAddr, m_recvBuf, m_recvLen);
		free(m_recvBuf);
	}

private:
	int  m_fd;
	void* m_param1;
	read_hdl_func m_io_func;

	struct sockaddr m_cliAddr;
	char *m_recvBuf;
	int m_recvLen;
};


class CWriteTask : public Runnable
{
public:
	CWriteTask(write_hdl_func io_func, int  fd, void* param1)
	{
		m_fd = fd;
		m_param1 = param1;
		m_io_func = io_func;
	}
	~CWriteTask(void)
	{

	}

	void Run()
	{
		m_io_func(m_fd, m_param1);
	}

private:
	int  m_fd;
	void* m_param1;
	write_hdl_func m_io_func;
};


class CEventPoll {
public:
	CEventPoll();
	virtual ~CEventPoll();

	BOOL init();

	BOOL init_listen_thrds(unsigned int max_thrd_cnt,
			unsigned int start_core, unsigned int core_cnt);

	void* init_evt_thrds(unsigned int max_thrd_cnt,
						unsigned int start_core,
						unsigned int core_cnt,
						thrd_init_func init_func,
						thrd_exit_func exit_func);
	void free_evt_thrds(void* thrdPool);
	unsigned int evt_task_cnt(void *thrdPool);

	BOOL start();
	void stop();
	void set_stop_flag();

	BOOL add_listen_job(accept_hdl_func acpt_func,
								int fd, void* param1);
	BOOL del_listen_job(int fd);


	BOOL add_read_job(void* thrdPool,
					read_hdl_func read_func,
					int fd, void* param1,
					struct sockaddr *cliAddr,
					int bufferSize,
					BOOL isTcp);
	BOOL del_read_job(int  fd);


	BOOL add_write_job(void* thrdPool, write_hdl_func write_func,
				int fd, void* param1);
	BOOL del_write_job(int  fd);


	BOOL add_evt_job(void *thrdPool, evt_hdl_func evt_func,
				void* param1, void* param2, void* param3);


	BOOL add_time_job(void *thrdPool, expire_hdl_func expire_func,
					void* param1, void* param2, void* param3,
					unsigned int time_value,
					BOOL isOnce);
	BOOL del_time_job(expire_hdl_func expire_func, void* param1);

private:
	void time_job_walk(unsigned long long nowtime);
	TIME_NODE_T* find_time_jobs(expire_hdl_func expire_func, void* param1);
	IO_NODE_T* find_io_jobs(int fd);

	BOOL is_io_read(int event);
	BOOL is_io_write(int event);

	BOOL add_io_job(void* thrdPool, void* io_func,
						int  fd, void* param1,
						struct sockaddr *cliAddr,
						int event,
						int bufferSize,
						BOOL isTcp = true);
	BOOL del_io_job(int fd, int event);

	void set_io_writing(IO_NODE_T *jobNode);

	static void loop_handle(void *arg, void *unused);
	static void evt_poll_timer_hdl(void *param1, void *param2, void *param3);

	void _add_del_io_job(IO_NODE_T *jobNode);
	void _do_del_io_jobs();
private:
	struct list_head m_del_io_jobs;
	MUTEX_TYPE m_del_job_lock;

#ifndef _WIN32
	MUTEX_TYPE m_ep_lock;
	int  m_epfd;
#endif

	volatile int m_isShutDown;

	MUTEX_TYPE m_job_lock;
	struct list_head m_io_jobs;
	struct list_head m_time_jobs;
	unsigned long long m_latest_expire_time;

	CThreadPool *m_listen_thrd_pool;

	typedef std::set<CThreadPool *> ThrdPoolSet;
	typedef ThrdPoolSet::iterator ThrdPoolSetIter;
	ThrdPoolSet m_evt_thrd_pools;
};

#endif /* CRSCANEVT_H_ */
