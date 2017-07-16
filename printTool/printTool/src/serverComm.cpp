
#include "serverComm.h"

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

char* msgTaskDataBuild(CARD_INFO_T *cardInfo, int *len)
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
	taskData = (char*)malloc(totalLen);
	if (NULL == taskData)
	{
		return NULL;
	}
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



static unsigned char* base64=(unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

char* base64encode(char *src,unsigned int srclen, unsigned int *dstlen)
{
     unsigned int n,buflen,i,j;
     char *dst;

     buflen=n=srclen;

     *dstlen = buflen/3*4 + 4;

     dst=(char*)malloc(*dstlen);
     if (NULL == dst)
     {
    	 return NULL;
     }

     memset(dst,0,*dstlen);

     for(i=0,j=0;i<=srclen-3;i+=3,j+=4) {
         dst[j]=(src[i]&0xFC)>>2;
         dst[j+1]=((src[i]&0x03)<<4) + ((src[i+1]&0xF0)>>4);
         dst[j+2]=((src[i+1]&0x0F)<<2) + ((src[i+2]&0xC0)>>6);
         dst[j+3]=src[i+2]&0x3F;
     }
     if(n%3==1) {
         dst[j]=(src[i]&0xFC)>>2;
         dst[j+1]=((src[i]&0x03)<<4);
         dst[j+2]= 64;
         dst[j+3]= 64;
         j+=4;
     }
     else if(n%3==2) {
         dst[j]=(src[i]&0xFC)>>2;
         dst[j+1]=((src[i]&0x03)<<4)+((src[i+1]&0xF0)>>4);
         dst[j+2]=((src[i+1]&0x0F)<<2);
         dst[j+3]= 64;
         j+=4;
     }

     for(i=0;i<j;i++) /* map 6 bit value to base64 ASCII character */
         dst[i]=base64[(int)dst[i]];

     return dst;
}

char* base64decode(char *src, unsigned int srclen,
		char *dst, unsigned int dstMaxLen,
		unsigned int* dstlen)
{
     unsigned int n,i,j,pad;
     unsigned char *p;

     *dstlen=0;
     pad=0;
     n = srclen;

     while(n>0&&src[n-1]== '=') {
         src[n-1]=0;
         pad++;
         n--;
     }

     for(i=0;i<srclen;i++)   { /* map base64 ASCII character to 6 bit value */
         p=(unsigned char *)strchr((const char *)base64,(int)src[i]);
         if(!p)
              break;
         src[i] = p-(unsigned char *)base64;
     }

     unsigned int needLen = srclen*3/4+1;
     if (dstMaxLen < needLen)
     {
    	 return NULL;
     }

     memset(dst,0,srclen*3/4+1);
     for(i=0,j=0; i<srclen; i+=4,j+=3) {
         dst[j]=(src[i]<<2) + ((src[i+1]&0x30)>>4);
         dst[j+1]=((src[i+1]&0x0F)<<4) + ((src[i+2]&0x3C)>>2);
         dst[j+2]=((src[i+2]&0x03)<<6) + src[i+3];
         *dstlen += 3;
     }
     *dstlen -= pad;

     return dst;
}
