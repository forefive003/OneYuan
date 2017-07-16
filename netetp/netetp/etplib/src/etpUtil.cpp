/*
 * util.c
 *
 *  Created on: 2015年6月27日
 *      Author: abcht
 */



#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#endif

#include "etpLib.h"

DLL_API int CreatDir(char *pDir)
{
	int i = 0;
	int iRet;
	int iLen;
	char* pszDir;

	if(NULL == pDir)
	{
		return 0;
	}

	pszDir = STRDUP(pDir);
	iLen = strlen(pszDir);

	if (pszDir[0] == '/')
	{
		/*linux下第一个为/符号*/
		i = 1;
	}
	else
	{
		i = 0;
	}

	// 创建中间目录
	for (;i < iLen;i ++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			pszDir[i] = '\0';

			//如果不存在,创建
			iRet = ACCESS(pszDir,0);
			if (iRet != 0)
			{
				iRet = MKDIR(pszDir);
				if (iRet != 0)
				{
					char errBuf[64] = {0};
					printf("mkdir %s failed, error %s.\n", pszDir,
								str_error_s(errBuf, 64, errno));
					return -1;
				}
			}
			//支持linux,将所有\换成/
			pszDir[i] = '/';
		}
	}

	iRet = MKDIR(pszDir);
	free(pszDir);
	return iRet;
}


