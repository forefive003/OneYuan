
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "etpLib.h"
#include "Thread.h"
#include "EventPoll.h"

int g_io_timeout = 3;
static CEventPoll *g_eptObj = NULL;

DLL_API BOOL etp_sock_set_block(int fd)
{
#ifndef _WIN32
	int flags = fcntl(fd,F_GETFL,0);
	flags &= ~O_NONBLOCK;
	fcntl(fd,F_SETFL,flags);
#else
	u_long iMode = 0;  //non-blocking mode is enabled.
	ioctlsocket(fd, FIONBIO, &iMode);
#endif
	return true;
}

DLL_API BOOL etp_sock_set_unblock(int fd)
{
#ifndef _WIN32
	int flags = fcntl(fd,F_GETFL,0);
	flags |= O_NONBLOCK;
	fcntl(fd,F_SETFL,flags);
#else
	u_long iMode = 1;  //non-blocking mode is enabled.
	ioctlsocket(fd, FIONBIO, &iMode);
#endif
	return true;
}

DLL_API int etp_sock_safe_recv(int fd, char *buf, size_t len, BOOL *is_expired)
{
	size_t got = 0;
	char err_buf[64] = {0};

	int first = recv(fd, buf, len, 0);
	if (first < 0)
	{
		if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
		{
			return ETP_RCV_NODATA;
		}

		EL_LOG_ERROR("read failed [%s]", str_error_s(err_buf, 32, errno));
		return ETP_RCV_ERROR;
	}
	else if (first == 0)
	{
		EL_LOG_DEBUG("safe_read peer closed.");
		return 0;
	}

	if ((got = (size_t)first) == len)
	{
		return got;
	}

	while (1)
	{
		struct timeval tv;
		fd_set r_fds, e_fds;
		int cnt;

		FD_ZERO(&r_fds);
		FD_SET(fd, &r_fds);

		FD_ZERO(&e_fds);
		FD_SET(fd, &e_fds);

		tv.tv_sec = g_io_timeout;
		tv.tv_usec = 0;

		cnt = select(fd+1, &r_fds, NULL, &e_fds, &tv);
		if (cnt < 0)
		{
			if (cnt < 0 && errno == EBADF)
			{
				EL_LOG_ERROR("safe_read select failed [%s]", str_error_s(err_buf, 32, errno));
				return -1;
			}

			continue;
		}
		else if (0 == cnt)
		{
			/*timeout, not recved*/
			EL_LOG_INFO("safe_read select timeout [%d]",g_io_timeout);

			*is_expired = true;
			break;
		}

		if (FD_ISSET(fd, &r_fds))
		{
			int n = recv(fd, buf + got, len - got, 0);
			if (n == 0)
			{
				EL_LOG_DEBUG("safe_read peer closed.");
				got = 0;
				break;
			}

			if (n < 0)
			{
				if (errno == EINTR && errno == EWOULDBLOCK && errno == EAGAIN)
					continue;

				EL_LOG_ERROR("safe_read failed to read %ld bytes, %s",
						(long)len, str_error_s(err_buf, 32, errno));
				return ETP_RCV_ERROR;
			}

			if ((got += (size_t)n) == len)
				break;
		}
	}

	return got;
}

DLL_API int etp_sock_safe_send(int fd, char *buf, size_t len, BOOL *is_expired)
{
	int n;
	char err_buf[64] = {0};

	struct timeval tv;
	fd_set w_fds;
	int cnt;
	int send_len = (int)len;

	n = send(fd, buf, len, 0);
	if ((size_t)n == len)
	{
		return send_len;
	}

	if (n < 0)
	{
		if (errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
		{
			EL_LOG_ERROR("safe_write failed to write %ld bytes, %s.",
				(long)len, str_error_s(err_buf, 32, errno));
			return 0;
		}
	}

	buf += n;
	len -= n;

	while (len)
	{
		FD_ZERO(&w_fds);
		FD_SET(fd, &w_fds);

		tv.tv_sec = g_io_timeout;
		tv.tv_usec = 0;

		cnt = select(fd + 1, NULL, &w_fds, NULL, &tv);
		if (cnt < 0)
		{
			if (cnt < 0 && errno == EBADF)
			{
				EL_LOG_ERROR("safe_write select failed [%s]", str_error_s(err_buf, 32, errno));
				return 0;
			}

			continue;
		}
		else if (0 == cnt)
		{
			/*timeout, not recved*/
			*is_expired = true;
			break;
		}

		if (FD_ISSET(fd, &w_fds))
		{
			n = send(fd, buf, len, 0);
			if (n < 0)
			{
				if (errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
				{
					EL_LOG_ERROR("safe_write failed to write %ld bytes, %s.",
						(long)len, str_error_s(err_buf, 32, errno));
					return 0;
				}
				else
				{
					continue;
				}
			}

			buf += n;
			len -= n;
		}
	}

	return send_len;
}

DLL_API int etp_sock_send_line(int fd, const char *format, ...)
{
	va_list ap;
	char buf[BIGPATHBUFLEN];
	int len;
	BOOL is_send_timeout =false;

	va_start(ap, format);
	len = VSNPRINTF(buf, BIGPATHBUFLEN, format, ap);
	va_end(ap);

	if (len < 0 || len > BIGPATHBUFLEN)
	{
		return ET_ERROR;
	}

	return (len == etp_sock_safe_send(fd, buf, len, &is_send_timeout)) ? 0 : ET_ERROR;
}

/* Read a line of up to bufsiz-1 characters into buf.  Strips
 * the (required) trailing newline and all carriage returns.
 * Returns 1 for success; 0 for I/O error or truncation. */
DLL_API int etp_sock_recv_line(int fd, char *buf, size_t bufsiz, int eof_ok)
{
	BOOL is_rcv_timeout = false;

	bufsiz--; /* leave room for the null */
	while (bufsiz > 0)
	{
		if (etp_sock_safe_recv(fd, buf, 1, &is_rcv_timeout) != 1)
		{
			if (is_rcv_timeout)
			{
				return RECV_TIMEOUT;
			}

			return RECV_ERROR;
		}

		if (*buf == '\0')
			return RECV_OK;

		if (*buf == '\n')
			break;

		if (*buf != '\r')
		{
			buf++;
			bufsiz--;
		}
	}

	*buf = '\0';

	return (bufsiz > 0 ? RECV_OK : RECV_ERROR);
}

DLL_API BOOL etp_init_listen_thrds(unsigned int max_thrd_cnt,
					unsigned int start_core,
					unsigned int core_cnt)
{
	return g_eptObj->init_listen_thrds(max_thrd_cnt, start_core, core_cnt);
}

DLL_API void* etp_init_evt_thrds(unsigned int max_thrd_cnt,
				unsigned int start_core,
				unsigned int core_cnt,
				thrd_init_func init_func,
				thrd_exit_func exit_func)
{
	return g_eptObj->init_evt_thrds(max_thrd_cnt, start_core, core_cnt, init_func, exit_func);
}

DLL_API void etp_free_evt_thrds(void* thrdPool)
{
	return g_eptObj->free_evt_thrds(thrdPool);
}

DLL_API unsigned int etp_evt_jobs_cnt(void* thrdPool)
{
	return g_eptObj->evt_task_cnt(thrdPool);
}

DLL_API BOOL etp_init()
{
#ifdef _WIN32
	char err_buf[64] = {0};

	WSADATA  Ws;
	if (WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		EL_LOG_ERROR("Init Windows Socket Failed, %s!",
					str_error_s(err_buf, sizeof(err_buf), errno));
		return FALSE;
	}
#endif

	g_eptObj = new CEventPoll;
	if (NULL == g_eptObj)
	{
		EL_LOG_ERROR("new EventPoll failed.");
		return FALSE;
	}

	g_eptObj->init();
	return TRUE;
}

DLL_API void etp_free()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

DLL_API BOOL etp_start()
{
	return g_eptObj->start();
}

DLL_API void etp_stop()
{
	g_eptObj->stop();
}

DLL_API void etp_set_stop_flag()
{
	g_eptObj->set_stop_flag();
}


DLL_API BOOL etp_add_listen_job(accept_hdl_func acpt_func, int fd, void* param1)
{
	return g_eptObj->add_listen_job(acpt_func, fd, param1);
}

DLL_API BOOL etp_del_listen_job(int  fd)
{
	return g_eptObj->del_listen_job(fd);
}


DLL_API BOOL etp_add_read_job(void *thrdPool, read_hdl_func read_func,
					int fd, void* param1,
					struct sockaddr *cliAddr,
					int bufferSize,
					BOOL isTcp)
{
	if (bufferSize <= 0)
	{
		return false;
	}

	return g_eptObj->add_read_job(thrdPool, read_func, fd, param1, cliAddr, bufferSize, isTcp);
}

DLL_API BOOL etp_del_read_job(int  fd)
{
	return g_eptObj->del_read_job(fd);
}

DLL_API BOOL etp_add_write_job(void *thrdPool, write_hdl_func write_func,
					int  fd, void* param1)
{
	return g_eptObj->add_write_job(thrdPool, write_func, fd, param1);
}

DLL_API BOOL etp_del_write_job(int  fd)
{
	return g_eptObj->del_write_job(fd);
}

DLL_API BOOL etp_add_evt_job(void *thrdPool,
		evt_hdl_func evt_func, void* param1, void* param2, void *param3)
{
	return g_eptObj->add_evt_job(thrdPool, evt_func, param1, param2, param3);
}

DLL_API BOOL etp_add_time_job(void *thrdPool,
				expire_hdl_func expire_func,
				void* param1, void* param2, void* param3,
				unsigned int time_value,
				BOOL isOnce)
{
	return g_eptObj->add_time_job(thrdPool, expire_func,
								param1, param2, param3,
								time_value, isOnce);
}

DLL_API BOOL etp_del_time_job(expire_hdl_func expire_func, void* param1)
{
	return g_eptObj->del_time_job(expire_func, param1);
}

