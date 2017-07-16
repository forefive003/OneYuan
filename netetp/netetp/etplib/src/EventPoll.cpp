
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

#ifndef _WIN32
	pthread_mutexattr_t mux_attr;
	memset(&mux_attr, 0, sizeof(mux_attr));
	pthread_mutexattr_settype(&mux_attr, PTHREAD_MUTEX_RECURSIVE);
	MUTEX_SETUP_ATTR(m_job_lock, &mux_attr);

	memset(&mux_attr, 0, sizeof(mux_attr));
	pthread_mutexattr_settype(&mux_attr, PTHREAD_MUTEX_RECURSIVE);
	MUTEX_SETUP_ATTR(m_del_job_lock, &mux_attr);
#else
///TODO:设置递归锁
	MUTEX_SETUP_ATTR(m_job_lock, NULL);
	MUTEX_SETUP_ATTR(m_del_job_lock, NULL);
#endif

	INIT_LIST_HEAD(&m_del_io_jobs);

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
			if (tmp_obj->thrdPool != NULL)
			{
				CEvtTask *timeTask = new CEvtTask(tmp_obj->job_func,
													tmp_obj->param1, tmp_obj->param2, tmp_obj->param3);
				tmp_obj->thrdPool->Execute(timeTask);
			}
			else
			{
				tmp_obj->job_func(tmp_obj->param1, tmp_obj->param2, tmp_obj->param3);
			}

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

	EL_LOG_INFO("evt loop started.");

	while(false == pollObj->m_isShutDown)
	{
		bzero(events, sizeof(events));

		/*删除待删除节点*/
		pollObj->_do_del_io_jobs();

		/*防止惊觉*/
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
					IO_NODE_T *job_node = (IO_NODE_T*)events[i].data.ptr;
					if (NULL == job_node)
					{
						EL_LOG_ERROR("EPOLLIN happen but no job node.");
					}
					else
					{
						if (job_node->bufferSize == 0)
						{
							if (job_node->thrdPool)
							{
								CAcptTask *acptTask = new CAcptTask(job_node->accept_func,
											job_node->fd, job_node->param1);
								if (NULL == acptTask)
								{
									EL_LOG_ERROR("no memory when malloc.");
								}
								else
								{
									job_node->thrdPool->Execute(acptTask);
								}
							}
							else
							{
								job_node->accept_func(job_node->fd, job_node->param1);
							}
						}
						else
						{
							BOOL recvEnd = FALSE;
							while(!recvEnd)
							{
								if (job_node->isDeleting)
								{
									EL_LOG_INFO("fd %d is deleting, not read.", job_node->fd);
									break;
								}

								/*分配空间，接受数据*/
								char *recvBuf = (char*)malloc(job_node->bufferSize);
								if (NULL == recvBuf)
								{
									EL_LOG_ERROR("no memory when malloc.");
									/*下次再接受,这里最好使进程退出*/
									assert(0);
									break;
								}
								memset(recvBuf, 0, job_node->bufferSize);

								int recvLen = 0;
								struct sockaddr *cliAddr = NULL;

								if (job_node->isTcp)
								{
									cliAddr = &job_node->cliAddr;
									recvLen = recv(job_node->fd, recvBuf, job_node->bufferSize, 0);
								}
								else
								{
									struct sockaddr cliAddrTmp;
									memset((void*)&cliAddrTmp, 0, sizeof(cliAddrTmp));

									int clientAddrLen = sizeof(cliAddrTmp);

									recvLen = recvfrom(job_node->fd, recvBuf, job_node->bufferSize, 0,
												&cliAddrTmp, (socklen_t*)&clientAddrLen);

									cliAddr = &cliAddrTmp;
								}

								if (recvLen < 0)
								{
									if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
									{
										/*下次再接受*/
										recvEnd = TRUE;
										free(recvBuf);
										break;
									}

									char err_buf[64] = {0};
									EL_LOG_ERROR("read failed [%s]", str_error_s(err_buf, 32, errno));

									/*通知关闭*/
									recvLen = 0;
									recvEnd = TRUE;
								}
								else if (recvLen == 0)
								{
									/*通知关闭*/
									recvEnd = TRUE;
								}

//								EL_LOG_DEBUG("recv len %d.", recvLen);

								if (job_node->thrdPool)
								{
									CReadTask *recvTask = new CReadTask(job_node->read_func,
														job_node->fd, job_node->param1,
														cliAddr, recvBuf, recvLen);
									if (NULL == recvTask)
									{
										EL_LOG_ERROR("no memory when malloc.");
										assert(0);
									}
									else
									{
										job_node->thrdPool->Execute(recvTask);
									}
								}
								else
								{
									job_node->read_func(job_node->fd, job_node->param1, cliAddr, recvBuf, recvLen);
									free(recvBuf);
								}
							}
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
						if (job_node->isDeleting)
						{
							EL_LOG_INFO("fd %d is deleting, not write.", job_node->fd);
							break;
						}

						//设置正在写
						pollObj->set_io_writing(job_node);

						EL_LOG_DEBUG("write event coming");
						if (job_node->thrdPool)
						{
							CWriteTask *sendTask = new CWriteTask(job_node->write_func,
														job_node->fd, job_node->param1);
							if (NULL == sendTask)
							{
								EL_LOG_ERROR("no memory when malloc.");
							}
							else
							{
								job_node->thrdPool->Execute(sendTask);
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

	EL_LOG_INFO("evt loop listen thread exit succuss!");
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

	SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
				if (FALSE == job_node->isWriting)
				{
					FD_SET(job_node->fd, &wset);
				}
			}
			else
			{
				EL_LOG_INFO("fd event invalid %d.", job_node->now_event);
			}
		}

		//windows中必须有存在的fd
		if (maxFd == 0)
		{
			FD_SET(hSocket, &rset);
			maxFd = hSocket;
		}
		MUTEX_UNLOCK(pollObj->m_job_lock);

		/*删除待删除节点*/
		pollObj->_do_del_io_jobs();

		struct timeval tv;
		tv.tv_sec = EPOLL_TIMEOUT;
		tv.tv_usec = 0;
		fd_num = select(maxFd + 1, &rset, &wset, NULL, &tv);
		if (fd_num < 0)
		{
			DWORD dwError = WSAGetLastError();
			if (EINTR == dwError || dwError == EAGAIN)
			{
				EL_LOG_INFO("select returned, continue, %d.", dwError);
				continue;
			}

			EL_LOG_INFO("epoll_wait failed, %d.", dwError);
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
						if (job_node->bufferSize == 0)
						{
							if (job_node->thrdPool)
							{
								CAcptTask *acptTask = new CAcptTask(job_node->accept_func,
											job_node->fd, job_node->param1);
								if (NULL == acptTask)
								{
									EL_LOG_ERROR("no memory when malloc.");
								}
								else
								{
									job_node->thrdPool->Execute(acptTask);
								}
							}
							else
							{
								job_node->accept_func(job_node->fd, job_node->param1);
							}
						}
						else
						{
							BOOL recvEnd = FALSE;
							while(!recvEnd)
							{
								/*分配空间，接受数据*/
								char *recvBuf = (char*)malloc(job_node->bufferSize);
								if (NULL == recvBuf)
								{
									EL_LOG_ERROR("no memory when malloc.");
									/*下次再接受,这里最好使进程退出*/
									assert(0);
									break;
								}
								memset(recvBuf, 0, job_node->bufferSize);

								int recvLen = 0;
								struct sockaddr *cliAddr = NULL;

								if (job_node->isTcp)
								{
									cliAddr = &job_node->cliAddr;
									recvLen = recv(job_node->fd, recvBuf, job_node->bufferSize, 0);
								}
								else
								{
									struct sockaddr cliAddrTmp;
									memset((void*)&cliAddrTmp, 0, sizeof(cliAddrTmp));

									int clientAddrLen = sizeof(cliAddrTmp);

									recvLen = recvfrom(job_node->fd, recvBuf, job_node->bufferSize, 0,
												&cliAddrTmp, (socklen_t*)&clientAddrLen);

									cliAddr = &cliAddrTmp;
								}

								if (recvLen < 0)
								{
									DWORD dwError = WSAGetLastError();

									if (dwError == WSAEINTR || dwError == WSAEWOULDBLOCK)
									{
										/*下次再接受*/
										recvEnd = TRUE;
										break;
									}

									char err_buf[64] = {0};
									EL_LOG_ERROR("read failed [%d]", dwError);

									/*通知关闭*/
									recvLen = 0;

									recvEnd = TRUE;
								}
								else if (recvLen == 0)
								{
									recvEnd = TRUE;
								}

								if (job_node->thrdPool)
								{
									CReadTask *recvTask = new CReadTask(job_node->read_func,
														job_node->fd, job_node->param1,
														cliAddr, recvBuf, recvLen);
									if (NULL == recvTask)
									{
										EL_LOG_ERROR("no memory when malloc.");
										assert(0);
									}
									else
									{
										job_node->thrdPool->Execute(recvTask);
									}
								}
								else
								{
									job_node->read_func(job_node->fd, job_node->param1, cliAddr, recvBuf, recvLen);
								}
							}
						}
					}
				}
				else if (pollObj->is_io_write(job_node->now_event))
				{
					if(FD_ISSET(job_node->fd, &wset))
					{
						//设置正在写
						pollObj->set_io_writing(job_node);

						EL_LOG_DEBUG("write event coming");
						if (job_node->thrdPool)
						{
							CWriteTask *sendTask = new CWriteTask(job_node->write_func,
														job_node->fd, job_node->param1);
							if (NULL == sendTask)
							{
								EL_LOG_ERROR("no memory when malloc.");
							}
							else
							{
								job_node->thrdPool->Execute(sendTask);
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

	closesocket(hSocket);
	EL_LOG_INFO("evt listen thread exit succuss!");
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

BOOL CEventPoll::is_io_read(int event)
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

BOOL CEventPoll::is_io_write(int event)
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

void CEventPoll::set_io_writing(IO_NODE_T *jobNode)
{
#ifndef _WIN32
	char err_buf[64] = {0};

	/*modify*/
	struct epoll_event ev;

	memset(&ev, 0, sizeof(ev));
	ev.events = jobNode->now_event;
	ev.data.ptr = (void*)jobNode;
	if(epoll_ctl(m_epfd, EPOLL_CTL_MOD, jobNode->fd, &ev) != 0)
	{
		EL_LOG_ERROR("EPOLL_CTL_MOD failed, %s.", str_error_s(err_buf, sizeof(err_buf), errno));
	}
	else
	{
		EL_LOG_DEBUG("pause write event from io job, fd %d.", jobNode->fd);
	}
#else
	jobNode->isWriting = TRUE;
#endif
}

void CEventPoll::_add_del_io_job(IO_NODE_T *job_node)
{
	MUTEX_LOCK(m_del_job_lock);
	job_node->isDeleting = TRUE;
	list_add(&job_node->node, &this->m_del_io_jobs);
	MUTEX_UNLOCK(m_del_job_lock);
}

void CEventPoll::_do_del_io_jobs()
{
	IO_NODE_T *tmp_obj = NULL;
	struct list_head *pos = NULL, *next = NULL;

	MUTEX_LOCK(m_del_job_lock);
	list_for_each_safe(pos, next, &this->m_del_io_jobs)
	{
		tmp_obj = (IO_NODE_T *)pos;

		list_del(&tmp_obj->node);
		EL_LOG_DEBUG("del io job, fd %d.", tmp_obj->fd);

		//尽快检测出错误，全部清0
		memset(tmp_obj, 0, sizeof(*tmp_obj));
		delete tmp_obj;
	}
	MUTEX_UNLOCK(m_del_job_lock);
}

BOOL CEventPoll::add_io_job(void* thrdPool, void* io_func,
						int  fd, void* param1,
						struct sockaddr *cliAddr,
						int event,
						int bufferSize,
						BOOL isTcp)
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
			if (bufferSize == 0)
			{
				job_node->accept_func = (accept_hdl_func)io_func;
				job_node->bufferSize = 0;

				EL_LOG_DEBUG("add new liten job, fd %d.", fd);
			}
			else
			{
				job_node->read_func = (read_hdl_func)io_func;
				job_node->bufferSize = bufferSize;
				job_node->isTcp = isTcp;

				if (NULL != cliAddr)
				{
					memcpy((void*)&job_node->cliAddr, (void*)cliAddr, sizeof(struct sockaddr));
				}

				if (isTcp)
				{
					EL_LOG_DEBUG("add new tcp read job, fd %d.", fd);
				}
				else
				{
					EL_LOG_DEBUG("add new udp read job, fd %d.", fd);
				}
			}
		}
		else
		{
			job_node->write_func = (write_hdl_func)io_func;
			job_node->isWriting = FALSE;
		}

		job_node->fd = fd;
		job_node->param1 = param1;
		job_node->now_event = event;
		job_node->thrdPool = (CThreadPool*)thrdPool;
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

	if ((void*)(job_node->thrdPool) != thrdPool)
	{
		MUTEX_UNLOCK(m_job_lock);
		EL_LOG_ERROR("thrd pool not same.");
		return false;
	}

	if (is_io_read(event))
	{
		if (bufferSize == 0)
		{
			MUTEX_UNLOCK(m_job_lock);
			EL_LOG_ERROR("fd %d already has a listen job.", fd);
			return false;
		}
		else
		{
			job_node->read_func = (read_hdl_func)io_func;
			job_node->bufferSize = bufferSize;
			job_node->isTcp = isTcp;

			if (NULL != cliAddr)
			{
				memcpy((void*)&job_node->cliAddr, (void*)cliAddr, sizeof(struct sockaddr));
			}
		}
	}
	else
	{
		job_node->write_func = (write_hdl_func)io_func;
		job_node->isWriting = FALSE;
	}
	job_node->now_event |= event;

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

BOOL CEventPoll::del_io_job(int fd, int event)
{
	char err_buf[64] = {0};

	IO_NODE_T *tmp_obj = NULL;
	BOOL ret = true;

	MUTEX_LOCK(m_job_lock);

	tmp_obj = find_io_jobs(fd);
	if (NULL == tmp_obj)
	{
		MUTEX_UNLOCK(m_job_lock);
		return false;
	}

	/*如果已经存在,屏蔽 */
	if (tmp_obj->now_event & event)
	{
		tmp_obj->now_event &= (~event);
	}

	if (false == is_io_write(tmp_obj->now_event)
			&& false == is_io_read(tmp_obj->now_event))
	{
		/*从链表删除*/
		list_del(&tmp_obj->node);
		/*加入待删除链表,在监听链表中删除*/
		this->_add_del_io_job(tmp_obj);
		EL_LOG_DEBUG("add fd %d to deling jobs.", fd);

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

	MUTEX_UNLOCK(m_job_lock);

	return ret;
}


BOOL CEventPoll::add_listen_job(accept_hdl_func acpt_func,
							int fd, void* param1)
{
#ifndef _WIN32
	return this->add_io_job(NULL, (void*)acpt_func, fd, param1, NULL, EPOLLIN | EPOLLET, 0);
#else
	return this->add_io_job(NULL, (void*)acpt_func, fd, param1, NULL, IO_FLAG_READ, 0);
#endif
}

BOOL CEventPoll::del_listen_job(int fd)
{
#ifndef _WIN32
	return this->del_io_job(fd, EPOLLIN);
#else
	return this->del_io_job(fd, IO_FLAG_READ);
#endif
}


BOOL CEventPoll::add_read_job(void* thrdPool,
							read_hdl_func read_func,
							int fd, void* param1,
							struct sockaddr *cliAddr,
							int bufferSize,
							BOOL isTcp)
{
#ifndef _WIN32
	return this->add_io_job(thrdPool, (void*)read_func, fd, param1, cliAddr, EPOLLIN | EPOLLET, bufferSize, isTcp);
#else
	return this->add_io_job(thrdPool, (void*)read_func, fd, param1, cliAddr, IO_FLAG_READ, bufferSize, isTcp);
#endif
}

BOOL CEventPoll::del_read_job(int fd)
{
#ifndef _WIN32
	return this->del_io_job(fd, EPOLLIN);
#else
	return this->del_io_job(fd, IO_FLAG_READ);
#endif
}

BOOL CEventPoll::add_write_job(void* thrdPool, write_hdl_func write_func,
									int fd, void* param1)
{
#ifndef _WIN32
	return this->add_io_job(thrdPool, (void*)write_func, fd, param1, NULL, EPOLLOUT | EPOLLET, 0);
#else
	return this->add_io_job(thrdPool, (void*)write_func, fd, param1, NULL, IO_FLAG_WRITE, 0);
#endif
}

BOOL CEventPoll::del_write_job(int fd)
{
#ifndef _WIN32
	return this->del_io_job(fd, EPOLLOUT);
#else
	return this->del_io_job(fd, IO_FLAG_WRITE);
#endif
}

BOOL CEventPoll::add_evt_job(void *thrdPool, evt_hdl_func evt_func,
					void* param1, void* param2, void* param3)
{
	/*throw to the thread pool*/
	CEvtTask *evtTask = new CEvtTask(evt_func, param1, param2, param3);
	if (NULL == evtTask)
	{
		EL_LOG_ERROR("no memory when malloc evtTask.");
		return false;
	}
	return ((CThreadPool*)thrdPool)->Execute(evtTask);
}

BOOL CEventPoll::add_time_job(void *thrdPool, expire_hdl_func expire_func,
						void* param1, void* param2, void* param3,
						unsigned int time_value,
						BOOL isOnce)
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
	job_node->thrdPool = (CThreadPool*)thrdPool;

	MUTEX_LOCK(m_job_lock);
	list_add(&job_node->node, &this->m_time_jobs);
	MUTEX_UNLOCK(m_job_lock);

	EL_LOG_DEBUG("add a new timer.");
	return true;
}

BOOL CEventPoll::del_time_job(expire_hdl_func expire_func, void* param1)
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

unsigned int CEventPoll::evt_task_cnt(void *thrdPool)
{
	return ((CThreadPool*)thrdPool)->GetTaskSize();
}

BOOL  CEventPoll::init()
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

BOOL  CEventPoll::init_listen_thrds(unsigned int max_thrd_cnt,
		unsigned int start_core, unsigned int core_cnt)
{
	if (1 > max_thrd_cnt)
	{
		EL_LOG_ERROR("thrd cnt %d invalid.", max_thrd_cnt);
		return false;
	}

	m_listen_thrd_pool = new CThreadPool();
	if (NULL == m_listen_thrd_pool)
	{
		EL_LOG_ERROR("no memory when malloc.");
		return false;
	}

	if (false == m_listen_thrd_pool->Init(1,max_thrd_cnt,
						"listenThrds", start_core, core_cnt))
	{
		EL_LOG_ERROR("init listen thread pool failed.");
		return false;
	}

	return true;
}

void* CEventPoll::init_evt_thrds(unsigned int max_thrd_cnt,
				unsigned int start_core,
				unsigned int core_cnt,
				thrd_init_func init_func,
				thrd_exit_func exit_func)
{
	if (1 > max_thrd_cnt)
	{
		EL_LOG_ERROR("thrd cnt %d invalid.", max_thrd_cnt);
		return NULL;
	}

	CThreadPool *newPool = new CThreadPool();
	if (NULL == newPool)
	{
		EL_LOG_ERROR("no memory when malloc.");
		return NULL;
	}

	if (false == newPool->Init(1,max_thrd_cnt,
				"eventThrds", start_core, core_cnt, init_func, exit_func))
	{
		EL_LOG_ERROR("init event thread pool failed.");
		delete newPool;
		return NULL;
	}

	m_evt_thrd_pools.insert(newPool);
	return (void*)newPool;
}

void CEventPoll::free_evt_thrds(void* thrdPool)
{
	if (NULL != thrdPool)
	{
		((CThreadPool*)thrdPool)->Terminate();
		delete ((CThreadPool*)thrdPool);

		m_evt_thrd_pools.erase(((CThreadPool*)thrdPool));		
	}
}

void CEventPoll::evt_poll_timer_hdl(void *param1, void *param2, void* param3)
{
	CEventPoll *evtObj = (CEventPoll*)param1;
	time_t nowTime = time(NULL);
	unsigned long long agedTime = nowTime - THRD_AGE_TIME;

	if (NULL != evtObj->m_listen_thrd_pool)
	{
//		evtObj->m_io_thrd_pool->print_info();
		evtObj->m_listen_thrd_pool->aged_worker(agedTime);
	}

	CThreadPool* thrdPool = NULL;
	ThrdPoolSetIter itr = evtObj->m_evt_thrd_pools.begin();
	while(itr != evtObj->m_evt_thrd_pools.end())
	{
		thrdPool = *itr;
//		thrdPool->print_info();
		thrdPool->aged_worker(agedTime);

		itr++;
	}
}

BOOL CEventPoll::start()
{
	m_isShutDown = false;

	add_time_job(NULL, CEventPoll::evt_poll_timer_hdl,
					(void*)this, NULL, NULL,
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
	/*设置停止标志*/
	this->set_stop_flag();

	EL_LOG_INFO("etp pool will exit.");

	del_time_job(CEventPoll::evt_poll_timer_hdl, (void*)this);

	if (NULL != m_listen_thrd_pool)
	{
		m_listen_thrd_pool->Terminate();
		delete m_listen_thrd_pool;
		m_listen_thrd_pool = NULL;
	}

	//if (m_evt_thrd_pools.size() != 0)
	//{
	//	EL_LOG_ERROR("event thread pool not empty.");
	//}

	EL_LOG_INFO("evt pool exit succ.");

#ifndef _WIN32
	close(m_epfd);
	MUTEX_CLEANUP(m_ep_lock);
#endif
}

void CEventPoll::set_stop_flag()
{
	m_isShutDown = true;
}

