/*
 * commontype.h
 *
 *  Created on: 2015Äê6ÔÂ19ÈÕ
 *      Author: cht
 */

#ifndef COMMONTYPE_H_
#define COMMONTYPE_H_

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>


#ifdef _WIN32
#include <windows.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

#define _COM_TYPES_

#ifndef _COM_TYPES_
typedef int 			BOOL;

typedef char            INT8;
typedef short           INT16;
typedef int             INT32;

typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned int    UINT32;

typedef long                INT64;
typedef unsigned long       UINT64;

#endif

#ifndef OK
#define OK 0
#endif

#ifndef ET_ERROR
#define ET_ERROR -1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define inline __inline

static inline char* str_error_s(char *err_buf, unsigned int len, int errnum)
{
#ifdef _WIN32
	strerror_s(err_buf, len, errnum);
	return err_buf;
#else
	return strerror_r(errnum, err_buf, len);
#endif
}

static inline void sleep_s(int second)
{
#ifdef _WIN32
	Sleep(second*1000);
#else
	sleep(second);
#endif
}

#ifdef _WIN32
#define MUTEX_TYPE            CRITICAL_SECTION
#define MUTEX_SETUP(x)        InitializeCriticalSection(&(x))
#define MUTEX_SETUP_ATTR(x, attr)        InitializeCriticalSection(&(x))
#define MUTEX_CLEANUP(x)      DeleteCriticalSection(&(x))
#define MUTEX_LOCK(x)         EnterCriticalSection(&(x))
#define MUTEX_UNLOCK(x)       LeaveCriticalSection(&(x))
#define THREAD_ID             GetCurrentThreadId()
#else
#define MUTEX_TYPE            pthread_mutex_t
#define MUTEX_SETUP(x)        pthread_mutex_init(&(x), NULL)
#define MUTEX_SETUP_ATTR(x, attr)  pthread_mutex_init(&(x), attr)
#define MUTEX_CLEANUP(x)      pthread_mutex_destroy(&(x))
#define MUTEX_LOCK(x)         pthread_mutex_lock(&(x))
#define MUTEX_UNLOCK(x)       pthread_mutex_unlock(&(x))
#define THREAD_ID             pthread_self()
#endif


#ifdef _WIN32
#define VSNPRINTF(buf, len, format, ap)\
		vsnprintf_s(buf, len, _TRUNCATE, format, ap);

#define SNPRINTF(buf, len, format, ...)\
		_snprintf_s(buf, len, format, ##__VA_ARGS__);
#else
#define VSNPRINTF(buf, len, format,ap)\
		vsnprintf(buf, BIGPATHBUFLEN, format, ap);

#define SNPRINTF(buf, len, format, ...)\
		snprintf_s(buf, len, format, ##__VA_ARGS__);
#endif


extern int CreatDir(char *pDir);


#ifdef __cplusplus
}
#endif

#endif /* COMMONTYPE_H_ */
