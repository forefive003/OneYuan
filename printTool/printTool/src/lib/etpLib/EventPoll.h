/*
 * EventPoll.h
 *
 *  Created on: 2015骞�4鏈�25鏃�
 *      Author: cht
 */

#ifndef CEVTPOLL_H_
#define CEVTPOLL_H_

#include "list.h"
#include "Thread.h"
#include "ThreadPool.h"


enum
{
	RECV_OK = 0,
	RECV_ERROR,
	RECV_TIMEOUT,
};


#define IO_FLAG_READ 0x01
#define IO_FLAG_WRITE 0X02


#define BIGPATHBUFLEN (1024)

#define EVENTMAX 1024
#define EPOLL_TIMEOUT 1000  /*1S*/
#define THRD_AGE_TIME 10


typedef struct
{
	struct list_head node;

	io_hdl_func  read_func;
	io_hdl_func  write_func;
	int  fd;
	bool is_async; /*if Asynchronous*/
	void *param1;
	int now_event;
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
	bool isOnce;
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

class CIoTask : public Runnable
{
public:
	CIoTask(io_hdl_func io_func, int  fd, void* param1)
	{
		m_fd = fd;
		m_param1 = param1;
		m_io_func = io_func;
	}
	~CIoTask(void)
	{

	}

	void Run()
	{
		m_io_func(m_fd, m_param1);
	}

private:
	int  m_fd;
	void* m_param1;
	io_hdl_func m_io_func;
};


class CEventPoll {
public:
	CEventPoll();
	virtual ~CEventPoll();

	bool init();

	bool init_loop_thrds(unsigned int start_core, unsigned int thrd_cnt);
	bool init_evt_thrds(unsigned int start_core, unsigned int thrd_cnt);
	bool init_io_thrds(unsigned int start_core, unsigned int thrd_cnt);
	bool init_time_thrds(unsigned int start_core, unsigned int thrd_cnt);

	unsigned int io_task_cnt();
	unsigned int time_task_cnt();
	unsigned int evt_task_cnt();

	bool start();
	void stop();
	void set_stop_flag();

	bool add_read_job(io_hdl_func read_func, int  fd, void* param1, bool is_async);
	bool del_read_job(int  fd);

	bool add_write_job(io_hdl_func write_func, int  fd, void* param1, bool is_async);
	bool del_write_job(int  fd);

	bool add_evt_job(evt_hdl_func evt_func, void* param1, void* param2, void* param3);

	bool add_time_job(expire_hdl_func expire_func,
					void* param1, void* param2, void* param3,
					unsigned int time_value,
					bool isOnce);
	bool del_time_job(expire_hdl_func expire_func, void* param1);

private:
	void time_job_walk(unsigned long long nowtime);
	TIME_NODE_T* find_time_jobs(expire_hdl_func expire_func, void* param1);
	IO_NODE_T* find_io_jobs(int fd);

	bool is_io_read(int event);
	bool is_io_write(int event);

	bool add_io_job(io_hdl_func read_func,
						int  fd, void* param1, int event, bool is_async);
	bool del_io_job(int fd, int event);

#ifndef _WIN32
	MUTEX_TYPE m_ep_lock;
	int  m_epfd;
#endif

	volatile int m_isShutDown;

	static void loop_handle(void *arg, void *unused);
	static void evt_poll_timer_hdl(void *param1, void *param2, void *param3);

	MUTEX_TYPE m_job_lock;
	struct list_head m_io_jobs;
	struct list_head m_time_jobs;
	unsigned long long m_latest_expire_time;

	CThreadPool *m_listen_thrd_pool;
	CThreadPool *m_time_thrd_pool;
	CThreadPool *m_io_thrd_pool;
	CThreadPool *m_evt_thrd_pool;
};

#endif /* CRSCANEVT_H_ */
