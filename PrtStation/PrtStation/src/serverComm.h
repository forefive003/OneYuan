
#ifndef _SERVER_COMM_H
#define _SERVER_COMM_H

enum
{
	TASK_FAILED = 0,
	TASK_OK = 1,

	TASK_BUSY  = 2,
	TASK_RECVED = 3,
};

#define SERVER_NO_TASK  -2


#define GET_REG_INFO_WEBURL(webUrl, len, srvAddr, srvPort)\
		SNPRINTF(webUrl, len, "http://%s:%u/CardPrintService/services/CardPrint",\
							srvAddr, srvPort);


#define GET_PRT_RET_WEBURL(webUrl, len, srvAddr, srvPort)\
		SNPRINTF(webUrl, len, "http://%s:%u/CardPrintService/services/CardPrint",\
							srvAddr, srvPort);


extern int wsGetTask(int *taskSessionId, char **taskData, int *taskDataLen,
		char *taskSerialNo);

extern int wsSendRegInfo();
extern int wsSendPrtResult(BOOL isOK, char *failReason,
				char *cardId, char *atrValue, char *bankCode,
				char *resultOutStr, int taskSessionId);

#endif
