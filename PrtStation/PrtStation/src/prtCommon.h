/*
 * prtCommon.h
 *
 *  Created on: 2015年7月4日
 *      Author: abcht
 */

#ifndef PRTCOMMON_H_
#define PRTCOMMON_H_

#include <afx.h>
#include "etpCommontype.h"


#define DEV_IP_MAX_LEN  48



#define  OWNER_TEXT_MAX_LEN  256
#define  OWNER_PHOTO_MAX_LEN   0x200000

typedef struct
{
	char ownerName[OWNER_TEXT_MAX_LEN + 1];
	char ownerCardNum[OWNER_TEXT_MAX_LEN + 1];

	char ownerSex[OWNER_TEXT_MAX_LEN + 1];
	char cardLaunchTime[OWNER_TEXT_MAX_LEN + 1];
	char cardLaunchPlace[OWNER_TEXT_MAX_LEN + 1];

	char ownerIdentityNum[OWNER_TEXT_MAX_LEN + 1];
	char ownerPhoto[OWNER_PHOTO_MAX_LEN + 1];
	unsigned int  ownerPhotoLen;
}PRT_INFO_T;



#define  OUTPUT_ICSTR_MAX_LEN  8096
#define  OUTPUT_FORMAT_MAX_LEN  8096

#define  RESULT_OK_STR_LEN  512
#define  RESULT_FAIL_STR_LEN   512
#define  SEND_FAIL_STR_LEN  256


/*
 *
0000000100  数据来源，必填
0000000200  PIN码， 配置就传，没配置不传
0000000300  社保sam卡槽位， 如果是加密机模式， 就不传
0000000400  打印机sam卡槽位
0000000500  社保卡加密类型   1：PSAM卡类型， 2：加密机
0000000600  加密机IP端口，加密机模式才传，格式为1.1.1.1@8080
0000000700  加密机用户名密码, 加密机模式才传， 格式为user@password
0000000800  读写器类型，1 是HD100，2，是明华读写器，3，是龙杰读写器，4，是ACR读写器。。。
0000000900  用户卡类型： 1 大唐；2 华大; 3 华宏
0000000A00  银行卡号
0000000B00  为1时，必需读出银行卡号，否则不进行发卡。其它或无此节时，不作控制。
0000000C00  为1时，ATR中城市代码必需与数据源一至。其它或无此节时，不作控制
0000000D00  为1时，不论卡内有无密钥，强制写密钥。其它或无此节时，自动检测卡内有无密钥，已有密钥则不写密钥，只写数据。
0000000E00  加密因子卡类型, 1：版本1(蒋)， 2：版本2。
*/

#define ATTR_DATA_SOURCE_DESC  		"0000000100"   /*已在报文中包含*/
#define ATTR_PIN_CODE_DESC  		"0000000200"   /*16位*/
#define ATTR_SHEBAO_SAM_SLOT_DESC  	"0000000300"   /*1-4*/
#define ATTR_PRINTER_SAM_SLOT_DESC  "0000000400"  /*1-4*/
#define ATTR_SHEBAO_JIAMI_TYPE_DESC  	"0000000500"  /*社保卡加密类型   1：PSAM卡类型， 2：加密机*/
#define ATTR_JIAMIJI_IPADDR_DESC  	"0000000600"  /*加密机IP端口，加密机模式才传，格式为1.1.1.1@8080*/
#define ATTR_JIAMIJI_USER_DESC  	"0000000700"  /*加密机用户名密码, 加密机模式才传， 格式为user@password*/
#define ATTR_CARD_READER_DESC  		"0000000800"  /*读写器类型，1 是HD100，2，是明华读写器，3，是龙杰读写器，4，是ACR读写器*/
#define ATTR_USER_CARD_TYPE_DESC  			"0000000900"  /*用户卡类型： 1 大唐；2 华大; 3 华宏*/

#define ATTR_BANK_CODE_DESC  			"0000000A00"  /*有值表示调试模式， 否则为正式写IC卡模式*/

#define ATTR_WR_MATCH_BANK_DESC  			"0000000B00"  /*有值表示调试模式， 否则为正式写IC卡模式*/
#define ATTR_WR_MATCH_ATR_DESC  			"0000000C00"  /*有值表示调试模式， 否则为正式写IC卡模式*/
#define ATTR_WR_FORCE_MIYAO_DESC  			"0000000D00"  /*有值表示调试模式， 否则为正式写IC卡模式*/

#define ATTR_PRINTER_SAM_TYPE_DESC  			"0000000E00"  /*加密因子卡类型, 1：版本1(蒋)， 2：版本2。*/


enum
{
	MSG_PRINT_TASK = 1,
};

typedef struct
{
	int type;
	int len;
}MSG_HDR_T;



enum
{
	ERR_DATA_INVALID = 0,
	ERR_TASK_FULL,
	ERR_ADD_TASK_ERR,

	ERR_PRINTER_CARD_READER,
	ERR_PRINTER_NOT_READY,

	ERR_PRINTER_PHOTO_PARSE_FAIL,
	ERR_PRINTER_PHOTO_PRINT_FAIL,
	ERR_PRINTER_LOAD_DLL_FAIL,
	ERR_PRINTER_CANT_GET_INFO,
	ERR_PRINTER_IN_ABNORMAL,
	ERR_PRINTER_LAST_TASK_ABNORMAL,

	ERR_PRINTER_WR_IC_ERROR,
	ERR_PRINTER_RET_DATA_INVALID,

	ERR_TYPE_MAX
};

extern const char* g_errCodeDesc[];


static const char* getErrCodeDesc(int errCode)
{
	return g_errCodeDesc[errCode];
}

#endif /* PRTCOMMON_H_ */
