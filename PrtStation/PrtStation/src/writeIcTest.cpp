

#include <afx.h>
#include "etpCommontype.h"
#include "prtGlobal.h"

#define  CARD_ID_CODE_MAX_LEN 	32
#define  CARD_TYPE_MAX_LEN 	6
#define  STANDARD_VERSION_MAX_LEN 	4
#define  ORG_NUM_MAX_LEN 	24
#define  CARD_LAUCHTIME_MAX_LEN 	8
#define  CARD_VALIDTIME_MAX_LEN 	8

#define  OWNER_CARD_NUM_MAX_LEN 	9
#define  OWNER_IDENTITY_NUM_MAX_LEN  18
#define  OWNER_NAME_MAX_LEN  30
#define  OWNER_SEX_MAX_LEN 2
#define  OWNER_VOLK_MAX_LEN 2
#define  OWNER_BIRTHPLACE_MAX_LEN  6
#define  OWNER_BIRTHDAY_MAX_LEN  8
#define  OWNER_RESIDENCE_TYPE_MAX_LEN 2
#define  OWNER_RESIDENCE_PLACE_MAX_LEN 80
#define  OWNER_PHONE_MAX_LEN  15
#define  OWNER_HOUSEHOLD_MAX_LEN  80
#define  OWNER_NATION_MAX_LEN 3
#define  OWNER_POLITICAL_MAX_LEN  2
#define  OWNER_POSTAL_CODE_MAX_LEN  6
#define  OWNER_CITY_CODE_MAX_LEN 6
#define  OWNER_MEDICAL_PLACE_MAX_LEN  6


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
	
	char ownerJob[JOB_MAX_LEN];
	char ownerNation[OWNER_NATION_MAX_LEN + 1];
		
	char ownerPhoto[OWNER_PHOTO_MAX_LEN + 1];
	unsigned int  ownerPhotoLen;
}CARD_INFO_T;




#define ELM_BUILD(src, len, data, pos) \
do{\
	int srcLen = strlen(src);\
	if (srcLen != 0){\
		strncpy(&(data)[pos], src, srcLen);\
		pos += srcLen;\
	}\
	(data)[pos] = '|';\
	pos += 1;\
}while(0)

static char* msgTaskDataBuild(char *outStr, CARD_INFO_T *cardInfo, int *len)
{
	char *taskData = NULL;
	int pos = 0;

	unsigned int baseLen = 0;
	char* imgBase64 = base64encode(cardInfo->ownerPhoto, cardInfo->ownerPhotoLen, &baseLen);
	if (NULL == imgBase64)
	{
		MessageBox(NULL, "base64 ¼ÓÃÜÊ§°Ü£¡", "´íÎó", MB_OK);
		return NULL;
	}

	int totalLen = STR_TASK_DATA_NO_PHOTO_LEN + baseLen;
	taskData = outStr;

	memset(taskData, 0, totalLen);

	ELM_BUILD(cardInfo->dataSource, DATA_SOURCE_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerMedicalPlace, OWNER_MEDICAL_PLACE_MAX_LEN, taskData, pos);
	
	ELM_BUILD(cardInfo->cardIdCode, CARD_ID_CODE_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->cardType, CARD_TYPE_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->standardVersion, STANDARD_VERSION_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->orginazeNum, ORG_NUM_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->cardLaunchTime, CARD_LAUCHTIME_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->cardValidTime, CARD_VALIDTIME_MAX_LEN, taskData, pos);

	ELM_BUILD(cardInfo->ownerCardNum, OWNER_CARD_NUM_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerIdentityNum, OWNER_IDENTITY_NUM_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerName, OWNER_NAME_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerNameOther, OWNER_NAME_MAX_LEN, taskData, pos);

	ELM_BUILD(cardInfo->ownerSex, OWNER_SEX_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerVolk, OWNER_VOLK_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerBirthPlace, OWNER_BIRTHPLACE_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerBirthDay, OWNER_BIRTHDAY_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerBankCode, BANK_CODE_LEN, taskData, pos);

	
	ELM_BUILD(cardInfo->ownerResidencePlace, OWNER_RESIDENCE_PLACE_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerResidenceType, OWNER_RESIDENCE_TYPE_MAX_LEN, taskData, pos);
	
	ELM_BUILD(cardInfo->ownerHousehold, OWNER_HOUSEHOLD_MAX_LEN, taskData, pos);

	ELM_BUILD(cardInfo->ownerPhone, OWNER_PHONE_MAX_LEN, taskData, pos);
	ELM_BUILD(cardInfo->ownerJob, JOB_MAX_LEN, taskData, pos);

	ELM_BUILD(cardInfo->ownerNation, OWNER_NATION_MAX_LEN, taskData, pos);	
		
	memcpy(&taskData[pos], imgBase64, baseLen);
	pos += baseLen;

	*len = pos;
	return taskData;
}

static int getTestCardInfo(CARD_INFO_T *cardInfo)
{
	CString name;
	CString cardID;
	CString cardNum;


	memset(cardInfo, 0, sizeof(CARD_INFO_T));
	
	strncpy(cardInfo->dataSource, "123456", DATA_SOURCE_LEN);
	strncpy(cardInfo->ownerMedicalPlace, "001", OWNER_MEDICAL_PLACE_MAX_LEN);
	strncpy(cardInfo->cardIdCode, "0102030405060708", CARD_ID_CODE_MAX_LEN);
	strncpy(cardInfo->cardType, "2", CARD_TYPE_MAX_LEN);
	strncpy(cardInfo->standardVersion, "0123", STANDARD_VERSION_MAX_LEN);
	strncpy(cardInfo->orginazeNum, "¸Ê×ÎÖÝ°¢°Ó", ORG_NUM_MAX_LEN);
	strncpy(cardInfo->cardLaunchTime, "0712", CARD_LAUCHTIME_MAX_LEN);
	strncpy(cardInfo->cardValidTime, "1713", CARD_VALIDTIME_MAX_LEN);
	strncpy(cardInfo->ownerCardNum, "12345678", OWNER_CARD_NUM_MAX_LEN);
	strncpy(cardInfo->ownerIdentityNum, "123456789012345678", OWNER_IDENTITY_NUM_MAX_LEN);
	strncpy(cardInfo->ownerName, "ÕÅÈý", OWNER_NAME_MAX_LEN);
	strncpy(cardInfo->ownerNameOther, "¹þ¹þºÇºÇºÙºÙ", OWNER_NAME_MAX_LEN);
	strncpy(cardInfo->ownerSex, "1", OWNER_SEX_MAX_LEN);
	strncpy(cardInfo->ownerVolk, "02", OWNER_VOLK_MAX_LEN);
	strncpy(cardInfo->ownerBirthPlace, "002", OWNER_BIRTHPLACE_MAX_LEN);
	strncpy(cardInfo->ownerBirthDay, "19910901", OWNER_BIRTHDAY_MAX_LEN);

	strncpy(cardInfo->ownerBankCode, "", BANK_CODE_LEN);
	strncpy(cardInfo->ownerResidencePlace, "³É¶¼ÊÐÎäºîÇø²âÊÔÓÃ", OWNER_RESIDENCE_PLACE_MAX_LEN);
	strncpy(cardInfo->ownerResidenceType, "023", OWNER_RESIDENCE_TYPE_MAX_LEN);

	strncpy(cardInfo->ownerHousehold, "ºþ±±Ê¡ÎäººÊÐ", BANK_CODE_LEN);
	strncpy(cardInfo->ownerPhone, "18782351982", OWNER_PHONE_MAX_LEN);
	strncpy(cardInfo->ownerJob, "Èí¼þ", JOB_MAX_LEN);
	strncpy(cardInfo->ownerNation, "003", OWNER_NATION_MAX_LEN);
	
	CFile ImageFile;
	char * szImageBin = NULL;
	if (0 == ImageFile.Open(g_testPhotoDir, CFile::modeRead))
	{
		free(cardInfo);
		MessageBox(NULL, _T("´ò¿ª²âÊÔÎÄ¼þÊ§°Ü£¡£¡£¡"), "´íÎó", MB_OK);
		return ET_ERROR;
	}

	unsigned int iSrcLen = (int)ImageFile.GetLength();

	szImageBin = (char*)malloc(iSrcLen);
	if (NULL == szImageBin)
	{
		free(cardInfo);
		ImageFile.Close();
		MessageBox(NULL, _T("ÄÚ´æ²»×ã£¡£¡£¡"), "´íÎó", MB_OK);
		return ET_ERROR;
	}
	ImageFile.Read((void*)szImageBin,iSrcLen);
	ImageFile.Close();

	unsigned int baseLen = 0;
	char* imgBase64 = base64encode(szImageBin, iSrcLen, &baseLen);
	if (NULL == imgBase64)
	{
		free(cardInfo);
		free(szImageBin);
		MessageBox(NULL, _T("base64 ¼ÓÃÜÊ§°Ü£¡£¡£¡"), "´íÎó", MB_OK);
		return ET_ERROR;
	}
	free(szImageBin);

	unsigned int dstLen = 0;
	if (NULL == base64decode(imgBase64, baseLen,
					cardInfo->ownerPhoto, OWNER_PHOTO_MAX_LEN, &dstLen))
	{
		free(cardInfo);
		free(imgBase64);
		MessageBox(NULL, _T("base64 ½âÃÜÊ§°Ü£¡£¡£¡"), "´íÎó", MB_OK);
		return ET_ERROR;
	}
	cardInfo->ownerPhotoLen = dstLen;
	free(imgBase64);

	return OK;
}


int gzzTaskDataBuild(char *outIcStr)
{
	CARD_INFO_T *cardInfo = (CARD_INFO_T*)malloc(sizeof(CARD_INFO_T));
	if (NULL == cardInfo)
	{
		return ET_ERROR;
	}

	memset(cardInfo, 0, sizeof(cardInfo));
	if(OK != getTestCardInfo(cardInfo))
	{
		free(cardInfo);
		return ET_ERROR;
	}

	int len = 0;
	char *buf = msgTaskDataBuild(outIcStr, cardInfo, &len);
	if(NULL == buf)
	{
		free(cardInfo);
		return ET_ERROR;
	}

	free(cardInfo);
	return OK;
}

