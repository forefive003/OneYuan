#pragma once

#include <list>
#include "etpLib.h"

#define PRT_MAX_TASK_CNT  16 /*队列中最多缓存16个用户数据*/

#define  FLAG_PRINT 0x01
#define  FLAG_WRITE_IC 0x02
#define  FLAG_SEND_RET 0X04

#define  TASK_RESULT_DESC_LEN 512

enum
{
	PRT_TASK_INVALID = 0,
	PRT_TASK_INIT = 1,

	PRT_TASK_GET_JOB,
	PRT_TASK_WRITE_IC,
	PRT_TASK_WRITE_IC_END,

	PRT_TASK_PRINT,
	PRT_TASK_PRINT_END,

	//PRT_TASK_END_JOB,

	PRT_TASK_SEND_RET,

	PRT_TASK_END,

	PRT_TASK_ST_MAX
};

typedef enum
{
	ST_INIT = 0,
	ST_SUCCSS,
	ST_FAILED,
}OP_ST_E;


typedef struct
{
	int taskSessionId;

	char timeBuf[64];
	int flag;
	int operFlag;

	int status;

	PRT_INFO_T outPrtInfo;

	OP_ST_E outIcSt;
	char outIcStr[OUTPUT_ICSTR_MAX_LEN];
	char outIcFmtStr[OUTPUT_FORMAT_MAX_LEN];

	OP_ST_E printSt;
	char resultOkStr[RESULT_OK_STR_LEN]; /*卡识别码|ATR值|银行卡号*/
	char resultFailStr[RESULT_FAIL_STR_LEN];/*错误码|错误描述*/

	OP_ST_E retSt;
	char sendResultStr[SEND_FAIL_STR_LEN];

	BOOL endByErrors; /*是否发生了错误而终止*/
	BOOL isOpering; /*是否正在处理*/

	BOOL isTest; /*是否是测试*/
}PRT_TASK_T;


typedef std::list<PRT_TASK_T*> PrintTasks;
typedef PrintTasks::iterator PrintTasksItr;
typedef PrintTasks::reverse_iterator PrintTasksRItr; /*反向遍历迭代器*/

class PrtTaskHdl
{
public:
	PrtTaskHdl();
	virtual ~PrtTaskHdl();

	static PrtTaskHdl* instance();

	int dataParse(char *taskData, int len,
		PRT_INFO_T *outPrintInfo,
		char *outIcStr, int *outIcStrLen,
		char *outFmtStr, int *outFmtStrLen,
		char *taskSerialNo,
		char *parseFailReason);

	void dataExpand(char *outIcStr, int *outIcStrLen,
		char *outFmtStr, int *outFmtStrLen);

public:
	void addTask(PRT_TASK_T* taskInfo);
	void delTask(PRT_TASK_T* taskInfo);
	void setTaskStatus(PRT_TASK_T* taskInfo, int status);
	PRT_TASK_T* getTask(char* timeBuf, char* ownerName);

	void sendTaskResult(PRT_TASK_T* taskInfo);
	BOOL isTaskHdlEnd(PRT_TASK_T* taskInfo);
	void getTaskResultDesc(PRT_TASK_T* taskInfo, char *desc);
	int getTaskOperFlag(PRT_TASK_T* taskInfo);

	int getTaskingCnt();
	BOOL isTaskingFull();

	int photoToFile(char *photoData, int photoLen, char *filename);

public:
	MUTEX_TYPE m_prtTasksLock;
	PrintTasks m_prtTasks;

	int m_curTaskingCnt;
};


extern const char* g_taskStatusDesc[];