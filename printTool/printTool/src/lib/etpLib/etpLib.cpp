
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

#include "Thread.h"
#include "etpLib.h"
#include "EventPoll.h"

int g_io_timeout = 3;
int g_etp_log_level = ETP_LOG_INFO;

static void etp_log_print_ap(const char* format, va_list ap)
{
	vprintf(format, ap);
	printf("\n");
}

static void etp_log_print(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	etp_log_print_ap(format, ap);
	va_end(ap);
}

static void etp_log_debug_print(const char* format, ...)
{
	if (g_etp_log_level < ETP_LOG_DEBUG)
	{
		return;
	}

	va_list ap;
	va_start(ap, format);
	etp_log_print_ap(format, ap);
	va_end(ap);
}

static void etp_log_info_print(const char* format, ...)
{
	if (g_etp_log_level < ETP_LOG_INFO)
	{
		return;
	}

	va_list ap;
	va_start(ap, format);
	etp_log_print_ap(format, ap);
	va_end(ap);
}

static void etp_log_error_print(const char* format, ...)
{
	if (g_etp_log_level < ETP_LOG_ERROR)
	{
		return;
	}

	va_list ap;
	va_start(ap, format);
	etp_log_print_ap(format, ap);
	va_end(ap);
}

log_func g_log_func = etp_log_print;
log_func g_log_dbg_func = etp_log_debug_print;
log_func g_log_info_func = etp_log_info_print;
log_func g_log_err_func = etp_log_error_print;

void etp_set_log_level(int level)
{
	g_etp_log_level = level;
}

void etpset_log_func(log_func  logFunc)
{
	g_log_func = logFunc;
}

void etpset_log_func(log_func log_dbg_func,
				log_func log_err_func,
				log_func log_info_func)
{
	g_log_dbg_func = log_dbg_func;
	g_log_info_func = log_info_func;
	g_log_err_func = log_err_func;
}

int etp_sock_set_block(int fd)
{
#ifndef _WIN32
	int flags = fcntl(fd,F_GETFL,0);
	flags &= ~O_NONBLOCK;
	fcntl(fd,F_SETFL,flags);
#else
	u_long iMode = 0;  //non-blocking mode is enabled.
	ioctlsocket(fd, FIONBIO, &iMode);
#endif
	return OK;
}

int etp_sock_set_unblock(int fd)
{
#ifndef _WIN32
	int flags = fcntl(fd,F_GETFL,0);
	flags |= O_NONBLOCK;
	fcntl(fd,F_SETFL,flags);
#else
	u_long iMode = 1;  //non-blocking mode is enabled.
	ioctlsocket(fd, FIONBIO, &iMode);
#endif
	return OK;
}

unsigned int etp_sock_safe_recv(int fd, char *buf, size_t len, bool *is_expired)
{
	size_t got = 0;
	char err_buf[64] = {0};

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
				str_error_s(err_buf, 32, errno);
				EL_LOG_ERROR("safe_read select failed [%s]",err_buf);
				break;
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
				if (errno == EINTR)
					continue;

				str_error_s(err_buf, 32, errno);
				EL_LOG_ERROR("safe_read failed to read %ld bytes, %s",
						(long)len, err_buf);
				break;
			}

			if ((got += (size_t)n) == len)
				break;
		}
	}

	return got;
}

int etp_sock_safe_send(int fd, char *buf, size_t len, bool *is_expired)
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
			str_error_s(err_buf, 32, errno);

			EL_LOG_ERROR("safe_write failed to write %ld bytes, %s.",
				(long)len, err_buf);
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
				str_error_s(err_buf, 32, errno);

				EL_LOG_ERROR("safe_write select failed [%s]", err_buf);
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
					str_error_s(err_buf, 32, errno);

					EL_LOG_ERROR("safe_write failed to write %ld bytes, %s.",
						(long)len, err_buf);
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

int etp_sock_send_line(int fd, const char *format, ...)
{
	va_list ap;
	char buf[BIGPATHBUFLEN];
	int len;
	bool is_send_timeout =false;

	va_start(ap, format);
	len = VSNPRINTF(buf, BIGPATHBUFLEN, format, ap);
	va_end(ap);

	if (len < 0 || len > BIGPATHBUFLEN)
	{
		return ET_ERROR;
	}

	return (len == etp_sock_safe_send(fd, buf, len, &is_send_timeout)) ? OK : ET_ERROR;
}

/* Read a line of up to bufsiz-1 characters into buf.  Strips
 * the (required) trailing newline and all carriage returns.
 * Returns 1 for success; 0 for I/O error or truncation. */
int etp_sock_recv_line(int fd, char *buf, size_t bufsiz, int eof_ok)
{
	bool is_rcv_timeout = false;

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

bool etp_init_loop_thrds(void *eptObj,
		unsigned int start_core, unsigned int thrd_cnt)
{
	return ((CEventPoll*)eptObj)->init_loop_thrds(start_core, thrd_cnt);
}

bool etp_init_evt_thrds(void *eptObj,
		unsigned int start_core, unsigned int thrd_cnt)
{
	return ((CEventPoll*)eptObj)->init_evt_thrds(start_core, thrd_cnt);
}

bool etp_init_io_thrds(void *eptObj,
		unsigned int start_core, unsigned int thrd_cnt)
{
	return ((CEventPoll*)eptObj)->init_io_thrds(start_core, thrd_cnt);
}

bool etp_init_time_thrds(void *eptObj,
		unsigned int start_core, unsigned int thrd_cnt)
{
	return ((CEventPoll*)eptObj)->init_time_thrds(start_core, thrd_cnt);
}

unsigned int etp_io_jobs_cnt(void *eptObj)
{
	return ((CEventPoll*)eptObj)->io_task_cnt();
}

unsigned int etp_time_jobs_cnt(void *eptObj)
{
	return ((CEventPoll*)eptObj)->time_task_cnt();
}

unsigned int etp_evt_jobs_cnt(void *eptObj)
{
	return ((CEventPoll*)eptObj)->evt_task_cnt();
}

void* etp_init()
{
	CEventPoll *eptObj = new CEventPoll;
	if (NULL == eptObj)
	{
		EL_LOG_ERROR("new EventPoll failed.");
		return NULL;
	}

	eptObj->init();
	return eptObj;
}

bool etp_start(void *eptObj)
{
	return ((CEventPoll*)eptObj)->start();
}

void etp_stop(void *eptObj)
{
	((CEventPoll*)eptObj)->stop();
}

void etp_set_stop_flag(void *eptObj)
{
	((CEventPoll*)eptObj)->set_stop_flag();
}

bool etp_add_read_job(void *eptObj,
		io_hdl_func read_func, int  fd, void* param1, bool is_async)
{
	return ((CEventPoll*)eptObj)->add_read_job(read_func, fd, param1, is_async);
}

bool etp_del_read_job(void *eptObj, int  fd)
{
	return ((CEventPoll*)eptObj)->del_read_job(fd);
}

bool etp_add_write_job(void *eptObj,
		io_hdl_func write_func, int  fd, void* param1, bool is_async)
{
	return ((CEventPoll*)eptObj)->add_write_job(write_func, fd, param1, is_async);
}

bool etp_del_write_job(void *eptObj,int  fd)
{
	return ((CEventPoll*)eptObj)->del_write_job(fd);
}

bool etp_add_evt_job(void *eptObj,
		evt_hdl_func evt_func, void* param1, void* param2, void *param3)
{
	return ((CEventPoll*)eptObj)->add_evt_job(evt_func, param1, param2, param3);
}

bool etp_add_time_job(void *eptObj,
				expire_hdl_func expire_func,
				void* param1, void* param2, void* param3,
				unsigned int time_value,
				bool isOnce)
{
	return ((CEventPoll*)eptObj)->add_time_job(expire_func, param1, param2, param3,
								time_value, isOnce);
}

bool etp_del_time_job(void *eptObj,
		expire_hdl_func expire_func, void* param1)
{
	return ((CEventPoll*)eptObj)->del_time_job(expire_func, param1);
}

