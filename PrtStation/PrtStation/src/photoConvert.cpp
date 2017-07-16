
#include "prtGlobal.h"
#include "etpLib.h"

#include "ximage.h"
#include "photoConvert.h"

class CPhotoCovert g_photoConvert;


int CPhotoCovert::convertToJpg(char *photoData, unsigned int photoLen, 
	char *outBuf, unsigned int bufLen, 
	unsigned int *outLen)
{	
	PHOTO_TYPE_E photoType = getPhotoType(photoData, photoLen);
	if (PHOTO_INVALID == photoType)
	{
		return ET_ERROR;
	}
	else if (PHOTO_JPG == photoType)
	{
		return OK;
	}

	UINT32 cxtype = typeConvert(photoType);
	CxImage image((BYTE*)photoData, photoLen, cxtype);
	if (!image.IsValid())
	{
		PRT_LOG_ERROR("%s图片加载失败, %s.", getPhotoTypeName(photoType), image.GetLastError());
		return ET_ERROR;
	}

	if (!image.IsGrayScale())
		image.IncreaseBpp(24);
	image.SetJpegQuality(99);

	BYTE* pJpgDataTemp = NULL;	  
	int lJpgSize = 0;
	if (FALSE == image.Encode(pJpgDataTemp,lJpgSize,CXIMAGE_FORMAT_JPG)
		|| lJpgSize == 0)
	{
		PRT_LOG_ERROR("%s转换为JPG格式失败, %s.", getPhotoTypeName(photoType), image.GetLastError());
		return ET_ERROR;
	}

	if (lJpgSize > (int)bufLen)
	{
		PRT_LOG_ERROR("转换后的JPG格式文件太大 %u.", lJpgSize);
		return ET_ERROR;
	}

	memset(outBuf, 0, bufLen);
	memcpy(outBuf, pJpgDataTemp, lJpgSize);
	*outLen = (unsigned int)lJpgSize;

	free(pJpgDataTemp);
	PRT_LOG_INFO("%s转换为JPG格式成功.", getPhotoTypeName(photoType));
	return OK;
}

PHOTO_TYPE_E CPhotoCovert::getPhotoType(char *photoData, int photoLen)
{
	if (photoLen < 10)
	{
		return PHOTO_INVALID;
	}

	if ((BYTE)photoData[0] == 0xff &&
        (BYTE)photoData[1] == 0xd8 &&
        (BYTE)photoData[2] == 0xff)
    {
        return PHOTO_JPG;
    }

    if ((BYTE)photoData[0] == 0x89 &&
        (BYTE)photoData[1] == 0x50 &&
        (BYTE)photoData[2] == 0x4e &&
        (BYTE)photoData[3] == 0x47)
    {
        return PHOTO_PNG;
    }

    if ((BYTE)photoData[0] == 0x47 &&
        (BYTE)photoData[1] == 0x49 &&
        (BYTE)photoData[2] == 0x46 &&
        (BYTE)photoData[3] == 0x38)
    {
        return PHOTO_GIF;
    }

    if ((BYTE)photoData[0] == 0x49 &&
        (BYTE)photoData[1] == 0x49 &&
        (BYTE)photoData[2] == 0x2A &&
        (BYTE)photoData[3] == 0x00)
    {
        return PHOTO_TIF;
    }

    if ((BYTE)photoData[0] == 0x42 &&
        (BYTE)photoData[1] == 0x4D)
    {
        return PHOTO_BMP;
    }

	if ((BYTE)photoData[0] == 0x38 &&
        (BYTE)photoData[1] == 0x42 &&
        (BYTE)photoData[2] == 0x50 &&
        (BYTE)photoData[3] == 0x53)
    {
        return PHOTO_PSD;
    }

    if ((BYTE)photoData[0] == 0x0A &&
        (BYTE)photoData[1] == 0x05 &&
        (BYTE)photoData[2] == 0x01 &&
        (BYTE)photoData[3] == 0x08)
    {
        return PHOTO_PCX;
    }

	return PHOTO_INVALID;
}

const char* CPhotoCovert::getPhotoTypeName(PHOTO_TYPE_E photoType)
{
	switch(photoType)
	{
		case PHOTO_JPG:
			return "JPG";
			break;

		case PHOTO_PNG:
			return "PNG";
			break;

		case PHOTO_GIF:
			return "GIF";
			break;

		case PHOTO_TIF:
			return "TIF";
			break;

		case PHOTO_BMP:
			return "BMP";
			break;

		case PHOTO_PSD:
			return "PSD";
			break;

		case PHOTO_PCX:
			return "PCX";
			break;

		default:
			return "UNKNOWN";
			break;
	}

	return "UNKNOWN";
}


UINT32 CPhotoCovert::typeConvert(PHOTO_TYPE_E photoType)
{
	switch(photoType)
	{
		case PHOTO_JPG:
			return CXIMAGE_FORMAT_JPG;
			break;

		case PHOTO_PNG:
			return CXIMAGE_FORMAT_PNG;
			break;

		case PHOTO_GIF:
			return CXIMAGE_FORMAT_GIF;
			break;

		case PHOTO_TIF:
			return CXIMAGE_FORMAT_TIF;
			break;

		case PHOTO_BMP:
			return CXIMAGE_FORMAT_BMP;
			break;

		case PHOTO_PSD:
			return CXIMAGE_FORMAT_PSD;
			break;

		case PHOTO_PCX:
			return CXIMAGE_FORMAT_PCX;
			break;

		default:
			return CXIMAGE_FORMAT_UNKNOWN;
			break;
	}

	return CXIMAGE_FORMAT_UNKNOWN;
}