
#ifndef COMMONTYPE_H_
#define COMMONTYPE_H_

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>


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

//#ifndef ERROR
//#define ERROR -1
//#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//#define inline __inline


#ifdef __cplusplus
}
#endif

#endif /* COMMONTYPE_H_ */
