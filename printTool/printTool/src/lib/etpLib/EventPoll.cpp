
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#endif

#include "etpLib.h"
#include "EventPoll.h"


CEventPoll::CEventPoll() {
	// TODO Auto-generated constructor stub

#ifndef _WIN32
	m_epfd = -1;
#endif

	m_isShutDown = false;

	m_listen_thrd_pool = NULL;
	m_io_thrd_pool = NULL;
	m_evt_thrd_pool = NULL;
	m_time_thrd_pool = NULL;

#ifndef _WIN32
	pthread_mutexattr_t mux_attr;
	memset(&mux_attr, 0, sizeof(mux_attr));
	pthread_mutexattr_settype(&mux_attr, PTHREAD_MUTEX_RECURSIVE);
	MUTEX_SETUP_ATTR(m_job_lock, &mux_attr);
#else
	MUTEX_SETUP_ATTR(m_job_lock, NULL);
#endif

	INIT_LIST_HEAD(&m_io_jobs);
	INIT_LIST_HEAD(&m_time_jobs);
}

CEventPoll::~CEventPoll() {
	// TODO Auto-generated destructor stub
	MUTEX_CLEANUP(m_job_lock);
}

void CEventPoll::time_job_walk(unsigned long long nowtime)
{
	TIME_NODE_T *tmp_obj = NULL;
	struct list_head *pos = NULL, *next = NULL;

	MUTEX_LOCK(m_job_lock);

	list_for_each_safe(pos, next, &this->m_time_jobs)
	{
		tmp_obj = (TIME_NODE_T*)pos;
		if (tmp_obj->expire_time <= nowtime)
		{
#if 1
			CEvtTask *timeTask = new CEvtTask(tmp_obj->job_func,
									tmp_obj->param1, tmp_obj->param2, tmp_obj->param3);
			if (m_time_thrd_pool == NULL)
			{
				m_evt_thrd_pool->Execute(timeTask);
			}
			else
			{
				m_time_thrd_pool->Execute(timeTask);
			}
#else
			tmp_obj->job_func(tmp_obj->param1, tmp_obj->param2);
#endif
			if (tmp_obj->isOnce)
			{
				list_del(pos);
				delete tmp_obj;
			}
		}
	}

	/*maybe del timer job in  job_func, so can't set expire_time after call job_func*/
	list_for_each_safe(pos, next, &this->m_time_jobs)
	{
		tmp_obj = (TIME_NODE_T*)pos;
		if (tmp_obj->expire_time <= nowtime)
		{
			tmp_obj->expire_time = nowtime + tmp_obj->time_value;
		}
	}

	MUTEX_UNLOCK(m_job_lock);
}

#ifndef _WIN32
void CEventPoll::loop_handle(void *arg, void *unused)
{
	CEventPoll *pollObj = (CEventPoll*)arg;

	struct epoll_event events[EVENTMAX];
	int fd_num = 0;
	int i = 0;

	pollObj->m_latest_expire_time = time(NULL);

	sigset_t block_set;
	sigemptyset(&block_set);
	sigfillset(&block_set);
	sigdelset(&block_set,SIGSEGV);
	sigdelset(&block_set,SIGBUS);
	sigdelset(&block_set,SIGPIPE);

	if( pthread_sigmask(SIG_BLOCK, &block_set,NULL) != 0 )
	{
		char err_buf[64] = {0};
		EL_LOG_ERROR("pthread_sigmask error %s\n", str_error_s(err_buf, sizeof(err_buf), errno));
		pthread_exit((void*)-1);
	}

	EL_LOG_INFO("evt loop thread started.");

	while(false == pollObj->m_isShutDown)
	{
		bzero(events, sizeof(events));

		MUTEX_LOCK(pollObj->m_ep_lock);
		fd_num = epoll_wait(pollObj->m_epfd, events, EVENTMAX, EPOLL_TIMEOUT);
		MUTEX_UNLOCK(pollObj->m_ep_lock);
		if (fd_num < 0)
		{
			char err_buf[64] = {0};

			if (EINTR == errno || errno == EAGAIN)
			{
				EL_LOG_INFO("select returned, continue, %s.",
						str_error_s(err_buf, sizeof(err_buf), errno));
				continue;
			}

			EL_LOG_INFO("epoll_wait failed, %s.",
									str_error_s(err_buf, sizeof(err_buf), errno));
			break;
		}
		else if (fd_num > 0)
		{
			for (i = 0; i < fd_num; i++)
			{
				if (events[i].events & EPOLLIN)
				{
					/*find read fd and read*/
					IO_NODE_T *job_node = (IO_NODE_T*)events[i].data.ptr;
					if (NULL == job_node)
					{
						EL_LOG_ERROR("EPOLLIN happen but no job node.");
					}
					else
					{
						if (job_node->is_async)
						{
							CIoTask *recvTask = new CIoTask(job_node->read_func,
											job_node->fd, job_node->param1);
							if (NULL == recvTask)
							{
								EL_LOG_ERROR("no memory when malloc.");
							}
							else
							{
								pollObj->m_io_thrd_pool->Execute(recvTask);
							}
						}
						else
						{
							job_node->read_func(job_node->fd, job_node->param1);
						}
					}
				}
				else if (events[i].events & EPOLLOUT)
				{
					/*find write fd and write, del EPOLLOUT flag, otherwise loop*/
					IO_NODE_T *job_node = (IO_NODE_T*)events[i].data.ptr;
					if (NULL == job_node)
					{
						EL_LOG_ERROR("EPOLLOUT happen but no job node.");
					}
					else
					{
						EL_LOG_DEBUG("write event coming");
						if (job_node->is_async)
						{
							CIoTask *sendTask = new CIoTask(job_node->write_func,
														job_node->fd, job_node->param1);
							if (NULL == sendTask)
							{
								EL_LOG_ERROR("no memory when malloc.");
							}
							else
							{
								pollObj->m_io_thrd_pool->Execute(sendTask);
							}
						}
						else
						{
							job_node->write_func(job_node->fd, job_node->param1);
						}
					}
				}
				else
				{
					EL_LOG_ERROR("epoll_wait unexpect event.");
				}
			}
		}

		/*do timeout jobs*/
		time_t nowtime = time(NULL);
		unsigned int eclapse_time = nowtime - pollObj->m_latest_expire_time;
		if (eclapse_time >= 1)
		{
			pollObj->m_latest_expire_time = nowtime;
			pollObj->time_job_walk(nowtime);
		}
	}

	EL_LOG_INFO("event pool listen thread exit succuss!");
}
#else
void CEventPoll::loop_handle(void *arg, void *unused)
{
	CEventPoll *pollObj = (CEventPoll*)arg;

	int fd_num = 0;
	int i = 0;

	IO_NODE_T *job_node = NULL;
	struct list_head *pos = NULL, *next = NULL;

	pollObj->m_latest_expire_time = time(NULL);

	EL_LOG_INFO("evt loop thread started.");

	while(false == pollObj->m_isShutDown)
	{
		fd_set rset;
		fd_set wset;
		FD_ZERO(&rset);
		FD_ZERO(&wset);

		int maxFd = 0;

		MUTEX_LOCK(pollObj->m_job_lock);
		list_for_each(pos, &pollObj->m_io_jobs)
		{
			job_node = (IO_NODE_T*)pos;

			maxFd = max(maxFd, job_node->fd);

			if (pollObj->is_io_read(job_node->now_event))
			{
				FD_SET(job_node->fd, &rset);
			}
			else if (pollObj->is_io_write(job_node->now_event))
			{
				FD_SET(job_node->fd, &wset);
			}
			else
			{
				EL_LOG_INFO("fd event invalid %d.", job_node->now_event);
			}
		}
		MUTEX_UNLOCK(pollObj->m_job_lock);

		struct timeval tv;
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		fd_num = select(maxFd + 1, &rset, &wset, NULL, &tv);
		if (fd_num < 0)
		{
			char err_buf[64] = {0};

			if (EINTR == errno || errno == EAGAIN)
			{
				EL_LOG_INFO("select returned, continue, %s.",
						str_error_s(err_buf, sizeof(err_buf), errno));
				continue;
			}

			EL_LOG_INFO("epoll_wait failed, %s.",
									str_error_s(err_buf, sizeof(err_buf), errno));
			break;
		}
		else if (fd_num > 0)
		{
			MUTEX_LOCK(pollObj->m_job_lock);
			list_for_each_safe(pos, next, &pollObj->m_io_jobs)
			{
				job_node = (IO_NODE_T*)pos;

				if (pollObj->is_io_read(job_node->now_event))
				{
					if(FD_ISSET(job_node->fd, &rset))
					{
						if (job_node->is_async)
						{
							CIoTask *recvTask = new CIoTask(job_node->read_func,
											job_node->fd, job_node->param1);
							if (NULL == recvTask)
							{
								EL_LOG_ERROR("no memory when malloc.");
							}
							else
							{
								pollObj->m_io_thrd_pool->Execute(recvTask);
							}
						}
						else
						{
							job_node->read_func(job_node->fd, job_node->param1);
						}
					}
				}
				else if (pollObj->is_io_write(job_node->now_event))
				{
					if(FD_ISSET(job_node->fd, &wset))
					{
						EL_LOG_DEBUG("write event coming");
						if (job_node->is_async)
						{
							CIoTask *sendTask = new CIoTask(job_node->write_func,
														job_node->fd, job_node->param1);
							if (NULL == sendTask)
							{
								EL_LOG_ERROR("no memory when malloc.");
							}
							else
							{
								pollObj->m_io_thrd_pool->Execute(sendTask);
							}
						}
						else
						{
							job_node->write_func(job_node->fd, job_node->param1);
						}
					}
				}
			}
			MUTEX_UNLOCK(pollObj->m_job_lock);
		}

		/*do timeout jobs*/
		time_t nowtime = time(NULL);
		unsigned long long eclapse_time = nowtime - pollObj->m_latest_expire_time;
		if (eclapse_time >= 1)
		{
			pollObj->m_latest_expire_time = nowtime;
			pollObj->time_job_walk(nowtime);
		}
	}

	EL_LOG_INFO("event pool listen thread exit succuss!");
}
#endif

TIME_NODE_T* CEventPoll::find_time_jobs(expire_hdl_func expire_func,
									void* param1)
{
	TIME_NODE_T *tmp_obj = NULL;
	struct list_head *pos = NULL;

	list_for_each(pos, &this->m_time_jobs)
	{
		tmp_obj = (TIME_NODE_T*)pos;
		if (expire_func == tmp_obj->job_func
				&& param1 == tmp_obj->param1)
		{
			return tmp_obj;
		}
	}

	return NULL;
}

IO_NODE_T* CEventPoll::find_io_jobs(int fd)
{
	IO_NODE_T *tmp_obj = NULL;
	struct list_head *pos = NULL;

	list_for_each(pos, &this->m_io_jobs)
	{
		tmp_obj = (IO_NODE_T*)pos;
		if (fd == tmp_obj->fd)
		{
			return tmp_obj;
		}
	}

	return NULL;
}

bool CEventPoll::is_io_read(int event)
{
#ifndef _WIN32
	if (!(event & EPOLLIN)) return false;

	return true;
#else
	if (event & IO_FLAG_READ) {
		return true;
	}
	return false;
#endif
}

bool CEventPoll::is_io_write(int event)
{
#ifndef _WIN32
	if (!(event & EPOLLOUT)) return false;

	return true;
#else
	if (event & IO_FLAG_WRITE) {
		return true;
	}
	return false;
#endif
}

bool CEventPoll::add_io_job(io_hdl_func io_func,
								int fd, void* param1, int event,
								bool is_async)
{
	char err_buf[64] = {0};

	IO_NODE_T *job_node = NULL;

	MUTEX_LOCK(m_job_lock);
	job_node = find_io_jobs(fd);
	if (NULL == job_node)
	{
		job_node = new IO_NODE_T;
		if (NULL == job_node)
		{
			MUTEX_UNLOCK(m_job_lock);
			EL_LOG_ERROR("no memory when malloc.");
			return false;
		}

		memset(job_node, 0, sizeof(IO_NODE_T));
		if (is_io_read(event))
		{
			job_node->read_func = io_func;
		}
		else
		{
			job_node->write_func = io_func;
		}
		job_node->fd = fd;
		job_node->param1 = param1;
		job_node->now_event = event;
		job_node->is_async = is_async;
		list_add(&job_node->node, &this->m_io_jobs);

#ifndef _WIN32
		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		ev.events = job_node->now_event;
		ev.data.ptr = (void*)job_node;
		if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev) != 0)
		{
			MUTEX_UNLOCK(m_job_lock);

			list_del(&job_node->node);
			delete job_node;
			EL_LOG_ERROR("epoll_ctl add failed, fd %d, is read %d, %s.", fd,
					is_io_read(event), str_error_s(err_buf, sizeof(err_buf), errno));
			return false;
		}
#endif

		MUTEX_UNLOCK(m_job_lock);

		EL_LOG_DEBUG("add new io job, fd %d.", fd);
		return true;
	}

	job_node->now_event |= event;
	if (is_io_read(event))
	{
		job_node->read_func = io_func;
	}
	else
	{
		job_node->write_func = io_func;
	}

#ifndef _WIN32
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = job_node->now_event;
	ev.data.ptr = (void*)job_node;
	if(epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &ev) != 0)
	{
		MUTEX_UNLOCK(m_job_lock);

		EL_LOG_ERROR("epoll_ctl mod failed, fd %d, is read %d, %s.",
							fd,
							is_io_read(event),
							str_error_s(err_buf, sizeof(err_buf), errno));
		return false;
	}
#endif

	MUTEX_UNLOCK(m_job_lock);

	return true;
}

bool CEventPoll::del_io_job(int fd, int event)
{
	char err_buf[64] = {0};

	IO_NODE_T *tmp_obj = NULL;
	bool ret = true;

	MUTEX_LOCK(m_job_lock);

	tmp_obj = find_io_jobs(fd);
	if (NULL == tmp_obj)
	{
		MUTEX_UNLOCK(m_job_lock);
		return false;
	}

	if (tmp_obj->now_event & event)
	{
		tmp_obj->now_event &= (~event);
		if (false == is_io_write(tmp_obj->now_event)
				&& false == is_io_read(tmp_obj->now_event))
		{
			list_del(&tmp_obj->node);
			delete tmp_obj;

#ifndef _WIN32
			/*delete from epoll fd*/
			if(epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, NULL) != 0)
			{
				EL_LOG_ERROR("EPOLL_CTL_DEL failed, %s.", str_error_s(err_buf, sizeof(err_buf), errno));
				ret = false;
			}
#endif

			EL_LOG_DEBUG("del io job, fd %d.", fd);
		}
		else
		{
			if (is_io_read(event))
			{
				tmp_obj->read_func = NULL;
			}
			else
			{
				tmp_obj->write_func = NULL;
			}

#ifndef _WIN32
			/*modify*/
			struct epoll_event ev;

			memset(&ev, 0, sizeof(ev));
			ev.events = tmp_obj->now_event;
			ev.data.ptr = (void*)tmp_obj;
			if(epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &ev) != 0)
			{
				EL_LOG_ERROR("EPOLL_CTL_MOD failed, %s.", str_error_s(err_buf, sizeof(err_buf), errno));
				ret = false;
			}
#endif
			EL_LOG_DEBUG("del %s from io job, fd %d.",
						is_io_read(event) ? "read" : "write", fd);
		}
	}

	MUTEX_UNLOCK(m_job_lock);

	return ret;
}


bool CEventPoll::add_read_job(io_hdl_func read_func,
									int fd, void* param1, bool is_async)
{
#ifndef _WIN32
	return this->add_io_job(read_func, fd, param1, EPOLLIN | EPOLLET, is_async);
#else
	return this->add_io_job(read_func, fd, param1, IO_FLAG_READ, is_async);
#endif
}

bool CEventPoll::del_read_job(int fd)
{
#ifndef _WIN32
	return this->del_io_job(fd, EPOLLIN);
#else
	return this->del_io_job(fd, IO_FLAG_READ);
#endif
}

bool CEventPoll::add_write_job(io_hdl_func write_func,
									int fd, void* param1, bool is_async)
{
#ifndef _WIN32
	return this->add_io_job(write_func, fd, param1, EPOLLOUT | EPOLLET, is_async);
#else
	return this->add_io_job(write_func, fd, param1, IO_FLAG_WRITE, is_async);
#endif

}

bool CEventPoll::del_write_job(int fd)
{
#ifndef _WIN32
	return this->del_io_job(fd, EPOLLOUT);
#else
	return this->del_io_job(fd, IO_FLAG_WRITE);
#endif
}

bool CEventPoll::add_evt_job(evt_hdl_func evt_func,
					void* param1, void* param2, void* param3)
{
	/*throw to the thread pool*/
	CEvtTask *evtTask = new CEvtTask(evt_func, param1, param2, param3);
	if (NULL == evtTask)
	{
		EL_LOG_ERROR("no memory when malloc evtTask.");
		return false;
	}
	return m_evt_thrd_pool->Execute(evtTask);
}

bool CEventPoll::add_time_job(expire_hdl_func expire_func,
						void* param1, void* param2, void* param3,
						unsigned int time_value,
						bool isOnce)
{
	TIME_NODE_T *job_node = new TIME_NODE_T;
	if (NULL == job_node)
	{
		EL_LOG_ERROR("no memory when malloc.");
		return false;
	}
	job_node->job_func = expire_func;
	job_node->param1 = param1;
	job_node->param2 = param2;
	job_node->param3 = param3;
	job_node->time_value = time_value;
	job_node->expire_time = time(NULL) + time_value;
	job_node->isOnce = isOnce;

	MUTEX_LOCK(m_job_lock);
	list_add(&job_node->node, &this->m_time_jobs);
	MUTEX_UNLOCK(m_job_lock);

	EL_LOG_DEBUG("add a new timer.");
	return true;
}

bool CEventPoll::del_time_job(expire_hdl_func expire_func, void* param1)
{
	TIME_NODE_T *tmp_obj = NULL;

	MUTEX_LOCK(m_job_lock);
	tmp_obj = find_time_jobs(expire_func, param1);
	if (NULL == tmp_obj)
	{
		MUTEX_UNLOCK(m_job_lock);
		EL_LOG_ERROR("node not exist when del time job.");
		return false;
	}
	list_del(&tmp_obj->node);
	MUTEX_UNLOCK(m_job_lock);

	delete tmp_obj;
	return true;
}

unsigned int CEventPoll::io_task_cnt()
{
	return this->m_io_thrd_pool->GetTaskSize();
}

unsigned int CEventPoll::evt_task_cnt()
{
	return this->m_evt_thrd_pool->GetTaskSize();
}

unsigned int CEventPoll::time_task_cnt()
{
	return this->m_time_thrd_pool->GetTaskSize();
}

bool  CEventPoll::init()
{
#ifndef _WIN32
	m_epfd = epoll_create(1);
	if(m_epfd == -1)
	{
		EL_LOG_ERROR("epoll_create failed.");
		return false;
	}
	MUTEX_SETUP_ATTR(m_ep_lock, NULL);
#endif
	return true;
}

bool  CEventPoll::init_loop_thrds(unsigned int start_core, unsigned int thrd_cnt)
{
	if (1 > thrd_cnt)
	{
		EL_LOG_ERROR("thrd cnt %d invalid.", thrd_cnt);
		return false;
	}

	m_listen_thrd_pool = new CThreadPool();
	if (NULL == m_listen_thrd_pool)
	{
		EL_LOG_ERROR("no memory when malloc.");
		return false;
	}

	if (false == m_listen_thrd_pool->Init(1,1,
						"listenThrds", start_core, thrd_cnt))
	{
		EL_LOG_ERROR("init listen thread pool failed.");
		return false;
	}

	return true;
}

bool  CEventPoll::init_io_thrds(unsigned int start_core, unsigned int thrd_cnt)
{
	if (1 > thrd_cnt)
	{
		EL_LOG_ERROR("thrd cnt %d invalid.", thrd_cnt);
		return false;
	}

	m_io_thrd_pool = new CThreadPool();
	if (NULL == m_io_thrd_pool)
	{
		EL_LOG_ERROR("no memory when malloc.");
		return false;
	}

	if (false == m_io_thrd_pool->Init(1,1,
				"ioThrds", start_core, thrd_cnt))
	{
		EL_LOG_ERROR("init read thread pool failed.");
		return false;
	}

	return true;
}

bool  CEventPoll::init_time_thrds(unsigned int start_core, unsigned int thrd_cnt)
{
	if (1 > thrd_cnt)
	{
		EL_LOG_ERROR("thrd cnt %d invalid.", thrd_cnt);
		return false;
	}

	m_time_thrd_pool = new CThreadPool();
	if (NULL == m_time_thrd_pool)
	{
		EL_LOG_ERROR("no memory when malloc.");
		return false;
	}

	if (false == m_time_thrd_pool->Init(1,1,
				"timeThrds", start_core, thrd_cnt))
	{
		EL_LOG_ERROR("init time thread pool failed.");
		return false;
	}

	return true;
}

bool  CEventPoll::init_evt_thrds(unsigned int start_core, unsigned int thrd_cnt)
{
	if (1 > thrd_cnt)
	{
		EL_LOG_ERROR("thrd cnt %d invalid.", thrd_cnt);
		return false;
	}

	m_evt_thrd_pool = new CThreadPool();
	if (NULL == m_evt_thrd_pool)
	{
		EL_LOG_ERROR("no memory when malloc.");
		return false;
	}

	if (false == m_evt_thrd_pool->Init(1,1,
				"eventThrds", start_core, thrd_cnt))
	{
		EL_LOG_ERROR("init event thread pool failed.");
		return false;
	}

	return true;
}

void CEventPoll::evt_poll_timer_hdl(void *param1, void *param2, void* param3)
{
	CEventPoll *evtObj = (CEventPoll*)param1;
	time_t nowTime = time(NULL);
	unsigned long long agedTime = nowTime - THRD_AGE_TIME;

	if (NULL != evtObj->m_io_thrd_pool)
	{
		evtObj->m_io_thrd_pool->print_info();
		evtObj->m_io_thrd_pool->aged_worker(agedTime);
	}

	if (NULL != evtObj->m_evt_thrd_pool)
	{
		evtObj->m_evt_thrd_pool->print_info();
		evtObj->m_evt_thrd_pool->aged_worker(agedTime);
	}

	if (NULL != evtObj->m_time_thrd_pool)
	{
		evtObj->m_time_thrd_pool->print_info();
		evtObj->m_time_thrd_pool->aged_worker(agedTime);
	}
}

bool CEventPoll::start()
{
	m_isShutDown = false;

	add_time_job(CEventPoll::evt_poll_timer_hdl, (void*)this, NULL, NULL,
					30, false);

	if (NULL != m_listen_thrd_pool)
	{
		CEvtTask *listenTask = new CEvtTask((evt_hdl_func)loop_handle, (void*)this, NULL, NULL);
		m_listen_thrd_pool->Execute(listenTask);
	}
	else
	{
		loop_handle((void*)this, NULL);
	}

	return true;
}

void CEventPoll::stop()
{
	EL_LOG_INFO("event pool will exit.");

	del_time_job(CEventPoll::evt_poll_timer_hdl, (void*)this);

	if (NULL != m_listen_thrd_pool)
	{
		m_listen_thrd_pool->Terminate();
		delete m_listen_thrd_pool;
	}

	if (NULL != m_time_thrd_pool)
	{
		m_time_thrd_pool->Terminate();
		delete m_time_thrd_pool;
	}

	if (NULL != m_io_thrd_pool)
	{
		m_io_thrd_pool->Terminate();
		delete m_io_thrd_pool;
	}

	if (NULL != m_evt_thrd_pool)
	{
		m_evt_thrd_pool->Terminate();
		delete m_evt_thrd_pool;
	}

	EL_LOG_INFO("event pool exit succ.");

#ifndef _WIN32
	close(m_epfd);
	MUTEX_CLEANUP(m_ep_lock);
#endif
}

void CEventPoll::set_stop_flag()
{
	m_isShutDown = true;
}

