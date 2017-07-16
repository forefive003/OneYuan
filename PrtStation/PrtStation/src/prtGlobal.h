#ifndef _PRT_GLOBAL_H
#define _PRT_GLOBAL_H

#include <afx.h>
#include <stdarg.h>
#include "etpCommontype.h"

#include "prtCommon.h"
#include "PrtConfig.h"
#include "stationConfig.h"
#include "base64.h"

#include "PrtTaskHdl.h"
#include "PrtService.h"
#include "BrowserOper.h"
#include "ExcelOper.h"

//支持多种格式,必须自己解码
//#define SELF_DECODE_BASE64  1

#define XPS_TEST_PHOTO_FILE   "test\\example.jpg"
#define XPS_TEMP_PHOTO_FILE   "test\\temp.jpg"
#define XPS_USER_PHOTO_FILE   "test\\user.jpg"


#define  PRINT_LOG_DIR   "log"

extern char g_localModDir[MAX_PATH + 1];

extern char g_logDir[MAX_PATH + 1];

extern char g_testPhotoDir[MAX_PATH + 1];
extern char g_testTempDir[MAX_PATH + 1];
extern char g_testUserDir[MAX_PATH + 1];


typedef struct
{
	char printerName[32];
	char printerDriver[32];
}PRT_DEV_T;

extern PrtService *g_prtService;
extern PrtTaskHdl *g_taskHdl;
extern BOOL g_isTestPrinting;
extern CBrowserOper g_browserOper;
extern CExcelOper *g_excelOper;

extern HWND g_parentWnd;

#define PRT_LOG_DEBUG EL_LOG_DEBUG
#define PRT_LOG_ERROR EL_LOG_ERROR
#define PRT_LOG_INFO EL_LOG_INFO
#define PRT_LOG_WARN EL_LOG_WARN

#define  NET_STATUS_DESC_LEN 64
#define  TASK_STATUS_DESC_LEN 512

typedef struct
{
	BOOL isNew;
	char timeBuf[32];
	char name[32];
	char identity[32];
	char desc[TASK_STATUS_DESC_LEN + 1];
}PRT_TASK_LIST_ITEM_T;

extern void prtSetNetStatus(int index, char *netStatus);

extern PRT_DEV_T* getWinPrinters(int *count);
extern int prtGlobalInit();
extern void prtGlobalFree();

#endif
