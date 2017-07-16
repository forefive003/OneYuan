/*
 * ept.h
 *
 *  Created on: 2015年6月3日
 *      Author: cht
 */

#ifndef EPT_MAIN_H_
#define EPT_MAIN_H_


#include "commontype.h"

#ifdef __cplusplus
extern "C"
{
#endif

enum
{
	ETP_LOG_ERROR = 1,
	ETP_LOG_INFO,
	ETP_LOG_DEBUG,
};

typedef void (*log_func)(const char* format, ...);

extern int g_etp_log_level;


#ifdef WIN32
/*mfc下发现不支持可变参数宏???, 所以改成这种方式*/
extern log_func g_log_dbg_func;
extern log_func g_log_err_func;
extern log_func g_log_info_func;


extern void etpset_log_func(log_func log_dbg_func,
		log_func log_err_func,
		log_func log_info_func);

#define EL_LOG_DEBUG g_log_dbg_func
#define EL_LOG_ERROR g_log_err_func
#define EL_LOG_INFO g_log_info_func
#else

extern log_func g_log_func;
void etpset_log_func(log_func  logFunc);

#define EL_LOG_DEBUG(format, ...) \
	if (g_etp_log_level >= ETP_LOG_DEBUG) \
		g_log_func("<%lu>:[dbg]"format"\t%s-%d\n", time(NULL), ##__VA_ARGS__,  __FUNCTION__, __LINE__);

#define EL_LOG_ERROR(format, ...) \
	if (g_etp_log_level >= ETP_LOG_ERROR) \
		g_log_func("<%lu>:[err]"format"\t%s-%d\n", time(NULL), ##__VA_ARGS__,  __FUNCTION__, __LINE__);

#define EL_LOG_INFO(format, ...) \
	if (g_etp_log_level >= ETP_LOG_INFO) \
		g_log_func("<%lu>:[info]"format"\t%s-%d\n", time(NULL), ##__VA_ARGS__,  __FUNCTION__, __LINE__);
#endif


typedef void (*io_hdl_func)(int  fd, void* param1);
typedef void (*evt_hdl_func)(void* param1, void* param2, void* param3);
typedef void (*expire_hdl_func)(void* param1, void* param2, void* param3);

void etp_set_log_level(int level);

int etp_sock_set_block(int fd);
int etp_sock_set_unblock(int fd);
unsigned int etp_sock_safe_recv(int fd, char *buf, size_t len, bool *is_expired);
int etp_sock_safe_send(int fd, char *buf, size_t len, bool *is_expired);
int etp_sock_send_line(int fd, const char *format, ...);
int etp_sock_recv_line(int fd, char *buf, size_t bufsiz, int eof_ok);



bool etp_init_loop_thrds(void *eptObj, unsigned int start_core, unsigned int thrd_cnt);
bool etp_init_evt_thrds(void *eptObj, unsigned int start_core, unsigned int thrd_cnt);
bool etp_init_io_thrds(void *eptObj, unsigned int start_core, unsigned int thrd_cnt);
bool etp_init_time_thrds(void *eptObj, unsigned int start_core, unsigned int thrd_cnt);

unsigned int etp_io_jobs_cnt(void *eptObj);
unsigned int etp_time_jobs_cnt(void *eptObj);
unsigned int etp_evt_jobs_cnt(void *eptObj);

void* etp_init();
bool etp_start(void *eptObj);
void etp_stop(void *eptObj);
void etp_set_stop_flag(void *eptObj);

bool etp_add_read_job(void *eptObj, io_hdl_func read_func, int  fd, void* param1, bool is_async);
bool etp_del_read_job(void *eptObj, int  fd);

bool etp_add_write_job(void *eptObj, io_hdl_func write_func, int  fd, void* param1, bool is_async);
bool etp_del_write_job(void *eptObj, int  fd);

bool etp_add_evt_job(void *eptObj,evt_hdl_func evt_func, void* param1, void* param2, void *param3);

bool etp_add_time_job(void *eptObj, expire_hdl_func expire_func,
				void* param1, void* param2,
				unsigned int time_value,
				bool isOnce);
bool etp_del_time_job(void *eptObj, expire_hdl_func expire_func, void* param1);

#ifdef __cplusplus
}
#endif


#endif /* EPT_H_ */
