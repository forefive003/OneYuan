
#ifndef _SERVER_COM_H
#define _SERVER_COM_H

#include <afx.h>
#include "commontype.h"

#define XPS_TEST_PATH	"./test"
#define XPS_TEST_PHOTO_FILE   "./test/example.jpg"

#define  PRINT_LOG_FILE   "./runlog"


#define  CARD_ID_CODE_MAX_LEN 	32
#define  CARD_TYPE_MAX_LEN 	6
#define  STANDARD_VERSION_MAX_LEN 	4
#define  ORG_NUM_MAX_LEN 	24
#define  CARD_LAUCHTIME_MAX_LEN 	8
#define  CARD_VALIDTIME_MAX_LEN 	8

#define  OWNER_CARD_NUM_MAX_LEN 	9
#define  OWNER_IDENTITY_NUM_MAX_LEN  18
#define  OWNER_NAME_MAX_LEN  30
#define  OWNER_SEX_MAX_LEN 1
#define  OWNER_VOLK_MAX_LEN 2
#define  OWNER_BIRTHPLACE_MAX_LEN  6
#define  OWNER_BIRTHDAY_MAX_LEN  8
#define  OWNER_RESIDENCE_TYPE_MAX_LEN 6
#define  OWNER_RESIDENCE_PLACE_MAX_LEN 80
#define  OWNER_PHONE_MAX_LEN  15
#define  OWNER_HOUSEHOLD_MAX_LEN  80
#define  OWNER_NATION_MAX_LEN 3
#define  OWNER_POLITICAL_MAX_LEN  2
#define  OWNER_POSTAL_CODE_MAX_LEN  6
#define  OWNER_CITY_CODE_MAX_LEN 6
#define  OWNER_MEDICAL_PLACE_MAX_LEN  6
#define  OWNER_PHOTO_MAX_LEN   0x2000


#define DATA_SOURCE_LEN 6
#define BANK_CODE_LEN 19
#define JOB_MAX_LEN  7

#define STR_TASK_DATA_NO_PHOTO_LEN  ( (DATA_SOURCE_LEN +1)\
					+ (OWNER_MEDICAL_PLACE_MAX_LEN + 1)\
					+ (CARD_ID_CODE_MAX_LEN + 1)\
					+ (CARD_TYPE_MAX_LEN + 1)\
					+ (STANDARD_VERSION_MAX_LEN + 1)\
					+ (ORG_NUM_MAX_LEN + 1)\
					+ (CARD_LAUCHTIME_MAX_LEN + 1)\
					+ (CARD_VALIDTIME_MAX_LEN + 1)\
					+ (OWNER_CARD_NUM_MAX_LEN + 1)\
					+ (OWNER_IDENTITY_NUM_MAX_LEN + 1)\
					+ (OWNER_NAME_MAX_LEN + 1)\
					+ (OWNER_NAME_MAX_LEN + 1)\
					+ (OWNER_SEX_MAX_LEN + 1)\
					+ (OWNER_VOLK_MAX_LEN + 1)\
					+ (OWNER_BIRTHPLACE_MAX_LEN + 1)\
					+ (OWNER_BIRTHDAY_MAX_LEN + 1)\
					+ (BANK_CODE_LEN + 1)\
					+ (OWNER_RESIDENCE_PLACE_MAX_LEN + 1)\
					+ (OWNER_RESIDENCE_TYPE_MAX_LEN + 1)\
					+ (OWNER_PHONE_MAX_LEN + 1)\
					+ (OWNER_HOUSEHOLD_MAX_LEN + 1)\
					+ (JOB_MAX_LEN + 1)\
					+ (OWNER_NATION_MAX_LEN + 1)\
					)
					
	

typedef struct
{
	char dataSource[DATA_SOURCE_LEN + 1];
	char ownerMedicalPlace[OWNER_MEDICAL_PLACE_MAX_LEN + 1];

	char cardIdCode[CARD_ID_CODE_MAX_LEN + 1];
	char cardType[CARD_TYPE_MAX_LEN + 1];

	char standardVersion[STANDARD_VERSION_MAX_LEN + 1];
	char orginazeNum[ORG_NUM_MAX_LEN + 1];
	char cardLaunchTime[CARD_LAUCHTIME_MAX_LEN + 1];
	char cardValidTime[CARD_VALIDTIME_MAX_LEN + 1];

	char ownerCardNum[OWNER_CARD_NUM_MAX_LEN + 1];
	char ownerIdentityNum[OWNER_IDENTITY_NUM_MAX_LEN + 1];
	char ownerName[OWNER_NAME_MAX_LEN + 1];
	char ownerNameOther[OWNER_NAME_MAX_LEN + 1];

	char ownerSex[OWNER_SEX_MAX_LEN + 1];
	char ownerVolk[OWNER_VOLK_MAX_LEN + 1];
	char ownerBirthPlace[OWNER_BIRTHPLACE_MAX_LEN + 1];
	char ownerBirthDay[OWNER_BIRTHDAY_MAX_LEN + 1];
	
	char ownerBankCode[BANK_CODE_LEN + 1];

	char ownerResidencePlace[OWNER_RESIDENCE_PLACE_MAX_LEN + 1];
	char ownerResidenceType[OWNER_RESIDENCE_TYPE_MAX_LEN + 1];
	
	char ownerHousehold[OWNER_HOUSEHOLD_MAX_LEN + 1];
	char ownerPhone[OWNER_PHONE_MAX_LEN + 1];
	
	char ownerJob[JOB_MAX_LEN + 1];
	char ownerNation[OWNER_NATION_MAX_LEN + 1];
		
	char ownerPhoto[OWNER_PHOTO_MAX_LEN + 1];
	unsigned int  ownerPhotoLen;
}CARD_INFO_T;


enum
{
	MSG_PRINT_TASK = 1,
};

typedef struct
{
	int type;
	int len;
}MSG_HDR_T;



extern char* msgTaskDataBuild(CARD_INFO_T *cardInfo, int *len);

extern char* base64decode(char *src, unsigned int srclen,
		char *dst, unsigned int dstMaxLen,
		unsigned int* dstlen);
extern char* base64encode(char *src,unsigned int srclen, unsigned int *dstlen);

#endif