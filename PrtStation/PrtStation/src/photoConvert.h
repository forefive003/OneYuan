#ifndef _PHOTO_CONVERT_H
#define _PHOTO_CONVERT_H


typedef enum 
{
	PHOTO_INVALID = 0,
	PHOTO_JPG = 1,
	PHOTO_PNG,
	PHOTO_GIF,
	PHOTO_TIF,
	PHOTO_BMP,
	PHOTO_PSD,
	PHOTO_PCX
}PHOTO_TYPE_E;

class CPhotoCovert
{
public:
	CPhotoCovert(void){}
	virtual ~CPhotoCovert(void){}

private:
	UINT32 typeConvert(PHOTO_TYPE_E photoType);

public:
	int convertToJpg(char *photoData, unsigned int photoLen, 
		char *outBuf, unsigned int bufLen, 
		unsigned int *outLen);
	PHOTO_TYPE_E getPhotoType(char *photoData, int photoLen);
	const char* getPhotoTypeName(PHOTO_TYPE_E photoType);
};

extern class CPhotoCovert g_photoConvert;
#endif
