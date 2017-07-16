
#ifndef _FORMAT_CONFIG_H
#define _FORMAT_CONFIG_H


#define DATA_ATTR_NAME_LEN  32
#define DATA_ATTR_ICPOS_LEN 10
#define DATA_ATTR_MAX_CNT  100

enum
{
	CODE_AN = 1,
	CODE_CN,
};

#define MAX_ATTR_CNT 10
#define DEF_VAL_MAX_LEN  128
#define DATA_PRINT_MAX_LEN 32

typedef struct
{
	char attrName[DATA_ATTR_NAME_LEN + 1];
	char positonInIc[DATA_ATTR_ICPOS_LEN + 1];
	bool isSendToIc;
	bool isPrint;
	char printName[DATA_PRINT_MAX_LEN + 1];
	bool isRequired; /*是否必须*/

	int codeType;
	int minLen;
	int maxLen;
	char defaultVal[DEF_VAL_MAX_LEN];
}PRT_DATA_FMT_T;

PRT_DATA_FMT_T* icDataFmtInfoGet(int *count);
int icDataFmtInit(void);

int icDataFmtValidChk(void);

#endif
