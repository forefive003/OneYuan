#ifndef _REG_CONFIG_H
#define _REG_CONFIG_H

#define DEF_SERVER_PORT  8903
#define DEF_LOCAL_PORT  6832

#define REG_URL_MAX_LEN  128
#define RESULT_URL_MAX_LEN  128

typedef struct
{
	int serverAddr;
	char serverAddrDesc[DEV_IP_MAX_LEN + 1];
	int serverPort;
	char serverRegUrl[REG_URL_MAX_LEN + 1];
	char serverResultUrl[RESULT_URL_MAX_LEN + 1];

	int localAddr;
	char localAddrDesc[DEV_IP_MAX_LEN + 1];
	int localPort;

	int max_cnt_per_time;
}PRT_REG_CFG_T;

PRT_REG_CFG_T* registerCfgGet(void);
int registerCfgSave(PRT_REG_CFG_T* regInfo);
int registerCfgInit(char *stationName);

int registerCfgRead(char *stationName, PRT_REG_CFG_T *registerCfg);
#endif
