#include "stdafx.h"

#include "prtCommon.h"
#include "prtGlobal.h"
#include "serverComm.h"
#include "photoConvert.h"

#include "etpLib.h"
#include "PrtTaskHdl.h"
#include "jiamijiProxy.h"

const char* g_taskStatusDesc[PRT_TASK_ST_MAX] =
{
	"无效状态",
	"初始化",

	"正在获取job",
	"正在写IC",
	"写IC结束",
	"正在打印",
	"打印结束",

	//"正在注销job",

	"正在发送响应",
	"任务结束"
};

PrtTaskHdl::PrtTaskHdl()
{
	MUTEX_SETUP(m_prtTasksLock);
	m_curTaskingCnt = 0;
}


PrtTaskHdl::~PrtTaskHdl()
{
	MUTEX_CLEANUP(m_prtTasksLock);
}


PrtTaskHdl* PrtTaskHdl::instance()
{
	static PrtTaskHdl *prtTaskHdl = NULL;

	if (prtTaskHdl == NULL)
	{
		prtTaskHdl = new PrtTaskHdl();
	}
	return prtTaskHdl;
}

void PrtTaskHdl::addTask(PRT_TASK_T* taskInfo)
{
	MUTEX_LOCK(m_prtTasksLock);
	m_prtTasks.push_back(taskInfo);
	m_curTaskingCnt++;

	/*如果数量操作了最大值,删除最早的*/
	if (m_prtTasks.size() > PRT_MAX_TASK_CNT)
	{
		PRT_TASK_T *tmpTaskInfo = m_prtTasks.front();
		if (isTaskHdlEnd(tmpTaskInfo))
		{
			/*已结结束则删除*/
			m_prtTasks.pop_front();
			/*释放空间*/
			PRT_LOG_INFO("task queue full, del %s.", tmpTaskInfo->outPrtInfo.ownerName);
			free(tmpTaskInfo);
		}
	}

	MUTEX_UNLOCK(m_prtTasksLock);

	/*通知主窗口更新*/
	::SendMessage(g_parentWnd, WM_UPDATE_TASK_LIST, NULL, NULL);
}

void PrtTaskHdl::delTask(PRT_TASK_T* taskInfo)
{
	if (taskInfo->status != PRT_TASK_END)
	{
		PRT_LOG_ERROR("task not end, now %d when del.", taskInfo->status);
		setTaskStatus(taskInfo, PRT_TASK_END);
	}

	MUTEX_LOCK(m_prtTasksLock);
	m_prtTasks.remove(taskInfo);
	MUTEX_UNLOCK(m_prtTasksLock);

	/*销毁空间*/
	free(taskInfo);

	/*通知主窗口更新*/
	::SendMessage(g_parentWnd, WM_UPDATE_TASK_LIST, NULL, NULL);
}

PRT_TASK_T* PrtTaskHdl::getTask(char* timeBuf, char* ownerName)
{
	PRT_TASK_T *taskInfo = NULL, *ret = NULL;

	PrintTasksItr itr = m_prtTasks.begin();	
	while (itr != g_taskHdl->m_prtTasks.end())
	{
		taskInfo = *itr;
		if (strncmp(taskInfo->timeBuf, timeBuf, sizeof(taskInfo->timeBuf)) == 0
			&& strncmp(taskInfo->outPrtInfo.ownerName, ownerName, OWNER_TEXT_MAX_LEN) == 0)
		{
			ret = taskInfo;
			break;
		}

		itr++;
	}

	return ret;
}

int PrtTaskHdl::photoToFile(char *photoData, int photoLen, char *filename)
{
	CFile ImageFile;
	if (0 != ImageFile.Open(filename, CFile::modeCreate | CFile::modeWrite))
	{
		unsigned int dstLen = 0;
		char *imageBin = (char*)malloc(OWNER_PHOTO_MAX_LEN);
		if (NULL == imageBin)
		{
			PRT_LOG_ERROR("malloc failed.");
			return ET_ERROR;
		}

		char *imageBin2 = (char*)malloc(OWNER_PHOTO_MAX_LEN);
		if (NULL == imageBin2)
		{
			free(imageBin);
			PRT_LOG_ERROR("malloc failed.");
			return ET_ERROR;
		}

		memcpy(imageBin2, photoData, photoLen);
		if (NULL == base64decode(imageBin2, photoLen, imageBin, OWNER_PHOTO_MAX_LEN, &dstLen))
		{
			free(imageBin);
			free(imageBin2);
			PRT_LOG_ERROR("decode failed.");
			return ET_ERROR;
		}

		ImageFile.Write(imageBin, dstLen);
		ImageFile.Flush();
		ImageFile.Close();
		free(imageBin);
		free(imageBin2);
	}
	else
	{
		PRT_LOG_ERROR("create file failed.");
		return ET_ERROR;
	}

	return OK;
}

void PrtTaskHdl::setTaskStatus(PRT_TASK_T* taskInfo, int status)
{
	if (status == taskInfo->status)
	{
		return;
	}

	taskInfo->status = status;
	PRT_LOG_INFO("print task status of user %s set to %s, %d",
		taskInfo->outPrtInfo.ownerName, g_taskStatusDesc[status], status);

	if (taskInfo->status == PRT_TASK_INIT)
	{
		if (taskInfo->isTest)
		{
			/*如果是测试,直接覆盖*/
			taskInfo->operFlag = taskInfo->flag;
		}
		else
		{
			PRT_PRINTER_CFG_T *printerParam = printerCfgGet();
			if (printerParam->printDoType == DO_PRT_AND_WR_IC)
			{
				taskInfo->operFlag = FLAG_PRINT | FLAG_WRITE_IC;
				PRT_LOG_INFO("set to print and write ic");
			}
			else if (printerParam->printDoType == DO_PRT_STEP1)
			{
				taskInfo->operFlag = FLAG_PRINT;
				PRT_LOG_INFO("set to print firstly");
			}
			else if (printerParam->printDoType == DO_WR_IC_STEP1)
			{
				taskInfo->operFlag = FLAG_WRITE_IC;
				PRT_LOG_INFO("set to write ic firstly");
			}
		}

		if (!(taskInfo->flag & FLAG_PRINT))
		{
			if (taskInfo->operFlag & FLAG_PRINT)
			{
				/*不打印*/
				taskInfo->operFlag &= (~FLAG_PRINT);
				PRT_LOG_INFO("unset print");
			}
		}

		if (!(taskInfo->flag & FLAG_WRITE_IC))
		{
			if (taskInfo->operFlag & FLAG_WRITE_IC)
			{
				/*不写卡*/
				taskInfo->operFlag &= (~FLAG_WRITE_IC);
				PRT_LOG_INFO("unset write ic");
			}
		}

		/*添加到任务队列中*/
		addTask(taskInfo);
	}
	else if (taskInfo->status == PRT_TASK_END)
	{
		/*如果任务结束,当前任务减一*/
		MUTEX_LOCK(m_prtTasksLock);
		if (m_curTaskingCnt == 0)
		{
			PRT_LOG_ERROR("cur tasking count not valid.");
		}
		else
		{
			m_curTaskingCnt--;
		}
		MUTEX_UNLOCK(m_prtTasksLock);
	}
	
	/*通知主窗口更新*/
	::SendMessage(g_parentWnd, WM_UPDATE_TASK_LIST, NULL, NULL);
}

int PrtTaskHdl::getTaskOperFlag(PRT_TASK_T* taskInfo)
{
	if (isTaskHdlEnd(taskInfo))
	{
		return 0;
	}

	if ((taskInfo->flag & FLAG_PRINT) && (taskInfo->printSt == ST_INIT))
	{
		return FLAG_PRINT;
	}

	if ((taskInfo->flag & FLAG_WRITE_IC) && (taskInfo->outIcSt == ST_INIT))
	{
		return FLAG_WRITE_IC;
	}

	return 0;
}

int PrtTaskHdl::getTaskingCnt()
{
	int size = 0;

	MUTEX_LOCK(m_prtTasksLock);
	size = m_curTaskingCnt;
	MUTEX_UNLOCK(m_prtTasksLock);

	return size;
}

BOOL PrtTaskHdl::isTaskingFull()
{
	int size = getTaskingCnt();
	PRT_REG_CFG_T *registerCfg = registerCfgGet();
	if (size >= registerCfg->max_cnt_per_time)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL PrtTaskHdl::isTaskHdlEnd(PRT_TASK_T* taskInfo)
{
	if (taskInfo->endByErrors)
	{
		return TRUE;
	}

	if ((taskInfo->flag & FLAG_PRINT) && (taskInfo->printSt == ST_INIT))
	{
		return FALSE;
	}

	if ((taskInfo->flag & FLAG_WRITE_IC) && (taskInfo->outIcSt == ST_INIT))
	{
		return FALSE;
	}

	/*所有操作完成*/
	return TRUE;
}

void PrtTaskHdl::sendTaskResult(PRT_TASK_T* taskInfo)
{
	if (!(taskInfo->flag & FLAG_SEND_RET))
	{
		PRT_LOG_INFO("no need to send ret to server.");
		return;
	}

	if (taskInfo->endByErrors == FALSE)
	{
		/*操作结果为成功*/
		/*NOTE:如果不写卡,resultOkStr会为空*/
		char resultOkStr[RESULT_OK_STR_LEN];
		strncpy(resultOkStr, taskInfo->resultOkStr, RESULT_OK_STR_LEN);
		resultOkStr[RESULT_OK_STR_LEN - 1] = 0;

		char *p[4];
		int i = 0;
		p[i] = strtok_my(resultOkStr, "|");
		int iRet = OK;

		while (p[i] != NULL)
		{
			i++;
			if (i > 3)
			{
				SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN - 1,
					"%s|too many split, %d.",
					getErrCodeDesc(ERR_PRINTER_RET_DATA_INVALID), i);

				PRT_LOG_ERROR("%s", taskInfo->resultFailStr);

				if (OK != wsSendPrtResult(TASK_FAILED, taskInfo->resultFailStr, "", "", "",
					taskInfo->sendResultStr, taskInfo->taskSessionId))
				{
					PRT_LOG_ERROR("发送响应失败, 返回 %s.", taskInfo->sendResultStr);
					taskInfo->retSt = ST_FAILED;
				}
				return;
			}

			p[i] = strtok_my(NULL, "|");
		}

		if (i != 3)
		{
			SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN - 1,
				"%s|too less split, %d.",
				getErrCodeDesc(ERR_PRINTER_RET_DATA_INVALID), i);

			PRT_LOG_ERROR("%s", taskInfo->resultFailStr);

			if (OK != wsSendPrtResult(TASK_FAILED, taskInfo->resultFailStr, "", "", "",
				taskInfo->sendResultStr, taskInfo->taskSessionId))
			{
				PRT_LOG_ERROR("发送响应失败, 返回 %s.", taskInfo->sendResultStr);
				taskInfo->retSt = ST_FAILED;
			}
			return;
		}

		PRT_LOG_INFO("send return ok str, %s, %s, %s.", p[0], p[1], p[2]);
		if (OK != wsSendPrtResult(TASK_OK, "", p[0], p[1], p[2],
			taskInfo->sendResultStr, taskInfo->taskSessionId))
		{
			PRT_LOG_ERROR("发送响应失败, 返回 %s.", taskInfo->sendResultStr);
			taskInfo->retSt = ST_FAILED;
			return;
		}

	}
	else
	{
		if (OK != wsSendPrtResult(TASK_FAILED, taskInfo->resultFailStr, "", "", "",
			taskInfo->sendResultStr, taskInfo->taskSessionId))
		{
			PRT_LOG_ERROR("发送响应失败, 返回 %s.", taskInfo->sendResultStr);
			taskInfo->retSt = ST_FAILED;
			return;
		}
	}

	SNPRINTF(taskInfo->sendResultStr, SEND_FAIL_STR_LEN - 1, "发送响应成功");
	taskInfo->retSt = ST_SUCCSS;
	PRT_LOG_INFO("%s.", taskInfo->sendResultStr);
	return;
}

void PrtTaskHdl::getTaskResultDesc(PRT_TASK_T* taskInfo, char *desc)
{
	memset(desc, 0, TASK_RESULT_DESC_LEN);

	if (isTaskHdlEnd(taskInfo))
	{
		/*先写卡再打卡, 会出现ok和failed里面都有数据,所以必须优先显示failed的东西*/
		if (taskInfo->resultFailStr[0] != 0)
		{
			strncpy(desc, taskInfo->resultFailStr, TASK_RESULT_DESC_LEN);
		}
		else if (taskInfo->resultOkStr[0] != 0)
		{
			strncpy(desc, taskInfo->resultOkStr, TASK_RESULT_DESC_LEN);
		}
		else if (taskInfo->endByErrors)
		{
			strncpy(desc, "发生其他错误", TASK_RESULT_DESC_LEN);
		}
		else
		{
			/*有可能只打印不打卡,resultOkStr里面不会有值*/
			strncpy(desc, "正常", TASK_RESULT_DESC_LEN);
		}
	}
	else
	{
		strncpy(desc, "执行中", TASK_RESULT_DESC_LEN);
	}

	desc[TASK_RESULT_DESC_LEN - 1] = 0;
}

int PrtTaskHdl::dataParse(char *taskData, int len,
	PRT_INFO_T *outPrintInfo,
	char *outIcStr, int *outIcStrLen,
	char *outFmtStr, int *outFmtStrLen,
	char *taskSerialNo,
	char *parseFailReason)
{
	int iRet = OK;
	char *placeName = NULL;

	int fmtCnt = 0;
	PRT_TEMPLATE_T *prtFormat = prtTemplateGet();
	PRT_DATA_FMT_T *dataFmt = icDataFmtInfoGet(&fmtCnt);

	const char *split = "|";
	char *p[DATA_ATTR_MAX_CNT];
	int plen[DATA_ATTR_MAX_CNT];
	int i = 0;
	p[i] = strtok_my(taskData, split);

	while (p[i] != NULL)
	{
		PRT_LOG_DEBUG("index %d, attr %s. value %s.", i, dataFmt[i].attrName, p[i]);

		plen[i] = strlen(p[i]);
		i++;

		if (i > fmtCnt)
		{
			SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
				"%s|too many split, %d.",
				getErrCodeDesc(ERR_DATA_INVALID), i);

			PRT_LOG_ERROR("%s", parseFailReason);
			iRet = ET_ERROR;
			goto parseEnd;
		}

		p[i] = strtok_my(NULL, split);
	}

	if (i != fmtCnt)
	{
		SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
			"%s|too less split, %d.",
			getErrCodeDesc(ERR_DATA_INVALID), i);

		PRT_LOG_ERROR("%s", parseFailReason);
		iRet = ET_ERROR;
		goto parseEnd;
	}

	char dataSource[7] = { 0 };
	int icStrLen = 0;
	int fmtStrLen = 0;

	for (i = 0; i<fmtCnt; i++)
	{
		if (dataFmt[i].isRequired)
		{
			if (plen[i] == 0)
			{
				SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
					"%s|parse failed, data %s has no value.",
					getErrCodeDesc(ERR_DATA_INVALID),
					dataFmt[i].attrName);

				PRT_LOG_ERROR("%s", parseFailReason);
				iRet = ET_ERROR;
				goto parseEnd;
			}
		}

		if (plen[i] != 0
			&& ((plen[i] > dataFmt[i].maxLen) || (plen[i] < dataFmt[i].minLen)))
		{
			SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
				"%s|parse failed, len(%d) of attr %s is invalid.",
				getErrCodeDesc(ERR_DATA_INVALID),
				plen[i],
				dataFmt[i].attrName);

			PRT_LOG_ERROR("%s", parseFailReason);
			iRet = ET_ERROR;
			goto parseEnd;
		}

		if (dataFmt[i].isPrint)
		{
			if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerNameAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerNameAttr.isPrintStatic)
				{
					SNPRINTF(outPrintInfo->ownerName, OWNER_TEXT_MAX_LEN,
						"%s %s",
						dataFmt[i].printName, p[i]);
				}
				else
				{
					strncpy(outPrintInfo->ownerName, p[i], OWNER_TEXT_MAX_LEN);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerIdentityNumAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerIdentityNumAttr.isPrintStatic)
				{
					SNPRINTF(outPrintInfo->ownerIdentityNum, OWNER_TEXT_MAX_LEN,
						"%s %s",
						dataFmt[i].printName, p[i]);
				}
				else
				{
					strncpy(outPrintInfo->ownerIdentityNum, p[i], OWNER_TEXT_MAX_LEN);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerCardNumAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerCardNumAttr.isPrintStatic)
				{
					SNPRINTF(outPrintInfo->ownerCardNum, OWNER_TEXT_MAX_LEN,
						"%s %s",
						dataFmt[i].printName, p[i]);
				}
				else
				{
					strncpy(outPrintInfo->ownerCardNum, p[i], OWNER_TEXT_MAX_LEN);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerSexAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerSexAttr.isPrintStatic)
				{
					SNPRINTF(outPrintInfo->ownerSex, OWNER_TEXT_MAX_LEN,
						"%s %s",
						dataFmt[i].printName,
						(atoi(p[i]) == 1 ? "男" : "女"));
				}
				else
				{
					SNPRINTF(outPrintInfo->ownerSex, OWNER_TEXT_MAX_LEN,
						"%s",
						(atoi(p[i]) == 1 ? "男" : "女"));
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->cardLaunchPlaceAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				/*获取发卡地地名*/
				placeName = g_placeMgr.getNameByCode(p[i]);
				if (NULL == placeName)
				{
					SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
						"%s|city code %s invalid.",
						getErrCodeDesc(ERR_DATA_INVALID),
						p[i]);

					PRT_LOG_ERROR("%s", parseFailReason);
					iRet = ET_ERROR;
					goto parseEnd;
				}

				if (prtFormat->cardLaunchPlaceAttr.isPrintStatic)
				{
					SNPRINTF(outPrintInfo->cardLaunchPlace, OWNER_TEXT_MAX_LEN,
						"%s %s",
						dataFmt[i].printName,
						placeName);
				}
				else
				{
					SNPRINTF(outPrintInfo->cardLaunchPlace, OWNER_TEXT_MAX_LEN,
						"%s",
						placeName);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->cardLaunchTimeAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (plen[i] != 8)
				{
					SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
						"%s|owner date len %d invalid.",
						getErrCodeDesc(ERR_DATA_INVALID),
						plen[i]);

					PRT_LOG_ERROR("%s", parseFailReason);
					iRet = ET_ERROR;
					goto parseEnd;
				}

				int year = 0, month = 0, day = 0;
				char tmp[6] = { 0 };
				memcpy(&tmp[0], &p[i][0], 4);
				year = atoi(tmp);

				memset(tmp, 0, sizeof(tmp));
				memcpy(&tmp[0], &p[i][4], 2);
				month = atoi(tmp);

				memset(tmp, 0, sizeof(tmp));
				memcpy(&tmp[0], &p[i][6], 2);
				day = atoi(tmp);

				if (prtFormat->cardLaunchTimeAttr.isPrintStatic)
				{
					SNPRINTF(outPrintInfo->cardLaunchTime, OWNER_TEXT_MAX_LEN,
						"%s %4d年%d月",
						dataFmt[i].printName,
						year, month);
				}
				else
				{
					SNPRINTF(outPrintInfo->cardLaunchTime, OWNER_TEXT_MAX_LEN,
						"%4d年%d月",
						year, month);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerPhotoAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (plen[i] > OWNER_PHOTO_MAX_LEN)
				{
					SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
						"%s|owner photo len %d invalid.",
						getErrCodeDesc(ERR_DATA_INVALID),
						plen[i]);

					PRT_LOG_ERROR("%s", parseFailReason);
					iRet = ET_ERROR;
					goto parseEnd;
				}

				unsigned int dstLen = 0;
				if (NULL == base64decode(p[i], plen[i],
					outPrintInfo->ownerPhoto, OWNER_PHOTO_MAX_LEN, &dstLen))
				{
					SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
						"base64 decode failed.");
					PRT_LOG_ERROR("%s", parseFailReason);
					iRet = ET_ERROR;
					goto parseEnd;
				}

				PHOTO_TYPE_E photoType = g_photoConvert.getPhotoType(outPrintInfo->ownerPhoto, dstLen);
				if (PHOTO_INVALID == photoType)
				{
					SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
						"photo type invalid.");
					PRT_LOG_ERROR("%s", parseFailReason);
					iRet = ET_ERROR;
					goto parseEnd;
				}
				else if (PHOTO_JPG != photoType)
				{
					if (OK != g_photoConvert.convertToJpg(outPrintInfo->ownerPhoto, dstLen,
						outPrintInfo->ownerPhoto, OWNER_PHOTO_MAX_LEN, &dstLen))
					{
						SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
							"photo convert failed.");
						PRT_LOG_ERROR("%s", parseFailReason);
						iRet = ET_ERROR;
						goto parseEnd;
					}
				}

				outPrintInfo->ownerPhotoLen = dstLen;

				/*再压缩成base64格式*/
				unsigned int baseLen = 0;
				char* imgBase64 = base64encode(outPrintInfo->ownerPhoto, outPrintInfo->ownerPhotoLen, &baseLen);
				if (NULL == imgBase64)
				{
					SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
						"base64 加密失败.");
					PRT_LOG_ERROR("%s", parseFailReason);
					iRet = ET_ERROR;
					goto parseEnd;
				}

				if (baseLen > OWNER_PHOTO_MAX_LEN)
				{
					free(imgBase64);

					SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
						"照片太大.");
					PRT_LOG_ERROR("%s", parseFailReason);
					iRet = ET_ERROR;
					goto parseEnd;
				}

				memcpy(outPrintInfo->ownerPhoto, imgBase64, baseLen);
				outPrintInfo->ownerPhotoLen = baseLen;

				free(imgBase64);
			}
		}

		if (0 == strncmp(dataFmt[i].attrName, "数据来源",
			DATA_ATTR_NAME_LEN))
		{
			memcpy(dataSource, p[i], 6);
		}

		if (dataFmt[i].isSendToIc)
		{
			if (0 == strncmp(dataFmt[i].attrName, "卡识别码",
				DATA_ATTR_NAME_LEN))
			{
				char cardIdCode[33] = { 0 };

				if (plen[i] == 32
					|| plen[i] == 24)
				{
					memcpy(cardIdCode, p[i], plen[i]);
					memset(&cardIdCode[24], 0x30, 8);
				}
				else if (plen[i] == 16)
				{
					memcpy(cardIdCode, p[i], plen[i]);

					/*生成并替换流水号*/
					memcpy(&cardIdCode[16], taskSerialNo, 8);
					memset(&cardIdCode[24], 0x30, 8);
				}
				else if (plen[i] == 0)
				{
					memcpy(&cardIdCode[0], dataSource, 6);
					memcpy(&cardIdCode[6], "D156000005", 10);

					/*生成并替换流水号*/
					memcpy(&cardIdCode[16], taskSerialNo, 8);
					memset(&cardIdCode[24], 0x30, 8);
				}
				else
				{
					SNPRINTF(parseFailReason, RESULT_FAIL_STR_LEN - 1,
						"%s|cardId Len(%d) invalid.",
						getErrCodeDesc(ERR_DATA_INVALID),
						plen[i]);

					PRT_LOG_ERROR("%s", parseFailReason);
					iRet = ET_ERROR;
					goto parseEnd;
				}

				PRT_LOG_DEBUG("cardIdCode : %s", cardIdCode);

				/*填充数据*/
				memcpy(&outIcStr[icStrLen], cardIdCode, 32);
				icStrLen += 32;
				outIcStr[icStrLen] = '|';
				icStrLen += 1;

				/*填充标识字段*/
				memcpy(&outFmtStr[fmtStrLen], dataFmt[i].positonInIc, DATA_ATTR_ICPOS_LEN);
				fmtStrLen += DATA_ATTR_ICPOS_LEN;
				outFmtStr[fmtStrLen] = '|';
				fmtStrLen += 1;
			}
			else
			{
				if (plen[i] != 0)
				{
					/*填充数据*/
					memcpy(&outIcStr[icStrLen], p[i], plen[i]);
					icStrLen += plen[i];
					outIcStr[icStrLen] = '|';
					icStrLen += 1;

					/*填充标识字段*/
					memcpy(&outFmtStr[fmtStrLen], dataFmt[i].positonInIc, DATA_ATTR_ICPOS_LEN);
					fmtStrLen += DATA_ATTR_ICPOS_LEN;
					outFmtStr[fmtStrLen] = '|';
					fmtStrLen += 1;
				}
				else
				{
					if (dataFmt[i].defaultVal[0] != 0)
					{
						PRT_LOG_INFO("attr %s is empty, use default: %s",
							dataFmt[i].attrName, dataFmt[i].defaultVal);

						/*填充数据*/
						int defLen = strlen(dataFmt[i].defaultVal);
						memcpy(&outIcStr[icStrLen], dataFmt[i].defaultVal, defLen);
						icStrLen += defLen;
						outIcStr[icStrLen] = '|';
						icStrLen += 1;

						/*填充标识字段*/
						memcpy(&outFmtStr[fmtStrLen], dataFmt[i].positonInIc, DATA_ATTR_ICPOS_LEN);
						fmtStrLen += DATA_ATTR_ICPOS_LEN;
						outFmtStr[fmtStrLen] = '|';
						fmtStrLen += 1;
					}
					else
					{
						PRT_LOG_DEBUG("attr %s is empty", dataFmt[i].attrName);
					}
				}
			}
		}
	}

	*outIcStrLen = icStrLen;
	*outFmtStrLen = fmtStrLen;

parseEnd:
	return iRet;
}


void PrtTaskHdl::dataExpand(char *outIcStr, int *outIcStrLen,
	char *outFmtStr, int *outFmtStrLen)
{
	PRT_PRINTER_CFG_T *printerParam = printerCfgGet();

	CString strTmp;
	int strLen = 0;

	int outLen = *outIcStrLen;
	int outFmtLen = *outFmtStrLen;

	/*社保卡类型*/
	strTmp.Format("%d", printerParam->shebaoSamType);
	strLen = strTmp.GetLength();
	memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
	outLen += strLen;
	outIcStr[outLen] = '|';
	outLen += 1;

	memcpy(&outFmtStr[outFmtLen], ATTR_SHEBAO_JIAMI_TYPE_DESC, DATA_ATTR_ICPOS_LEN);
	outFmtLen += DATA_ATTR_ICPOS_LEN;
	outFmtStr[outFmtLen] = '|';
	outFmtLen += 1;



	if (printerParam->shebaoSamType == SHEBAO_JIAMI_TYPE_JIAMIJI)
	{
		/*加密机类型*/
		if (g_is_use_proxy)
		{
			strTmp.Format("%s:%d", "127.0.0.1", g_jiamijiProxyPort);
		}
		else
		{
			strTmp.Format("%s:%d", printerParam->jimijiAddrDesc, printerParam->jimijiPort);
		}
		
		strLen = strTmp.GetLength();
		memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
		outLen += strLen;
		outIcStr[outLen] = '|';
		outLen += 1;

		memcpy(&outFmtStr[outFmtLen], ATTR_JIAMIJI_IPADDR_DESC, DATA_ATTR_ICPOS_LEN);
		outFmtLen += DATA_ATTR_ICPOS_LEN;
		outFmtStr[outFmtLen] = '|';
		outFmtLen += 1;



		strTmp.Format("%s@%s", printerParam->jimijiUserName, printerParam->jimijiPasswd);
		strLen = strTmp.GetLength();
		memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
		outLen += strLen;
		outIcStr[outLen] = '|';
		outLen += 1;

		memcpy(&outFmtStr[outFmtLen], ATTR_JIAMIJI_USER_DESC, DATA_ATTR_ICPOS_LEN);
		outFmtLen += DATA_ATTR_ICPOS_LEN;
		outFmtStr[outFmtLen] = '|';
		outFmtLen += 1;
	}
	else
	{
		/*加密卡槽位*/
		strTmp.Format("%d", printerParam->shebaoSamSlot);
		strLen = strTmp.GetLength();
		memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
		outLen += strLen;
		outIcStr[outLen] = '|';
		outLen += 1;

		memcpy(&outFmtStr[outFmtLen], ATTR_SHEBAO_SAM_SLOT_DESC, DATA_ATTR_ICPOS_LEN);
		outFmtLen += DATA_ATTR_ICPOS_LEN;
		outFmtStr[outFmtLen] = '|';
		outFmtLen += 1;
	}

	/*打印机卡槽位*/
	strTmp.Format("%d", printerParam->jiamiSamSlot);
	strLen = strTmp.GetLength();
	memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
	outLen += strLen;
	outIcStr[outLen] = '|';
	outLen += 1;

	memcpy(&outFmtStr[outFmtLen], ATTR_PRINTER_SAM_SLOT_DESC, DATA_ATTR_ICPOS_LEN);
	outFmtLen += DATA_ATTR_ICPOS_LEN;
	outFmtStr[outFmtLen] = '|';
	outFmtLen += 1;



	/*打印机卡类型*/
	strTmp.Format("%d", printerParam->jiamiSamType);
	strLen = strTmp.GetLength();
	memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
	outLen += strLen;
	outIcStr[outLen] = '|';
	outLen += 1;

	memcpy(&outFmtStr[outFmtLen], ATTR_PRINTER_SAM_TYPE_DESC, DATA_ATTR_ICPOS_LEN);
	outFmtLen += DATA_ATTR_ICPOS_LEN;
	outFmtStr[outFmtLen] = '|';
	outFmtLen += 1;



	/*用户卡信息*/
	strTmp.Format("%d", printerParam->userkaType);
	strLen = strTmp.GetLength();
	memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
	outLen += strLen;
	outIcStr[outLen] = '|';
	outLen += 1;

	memcpy(&outFmtStr[outFmtLen], ATTR_USER_CARD_TYPE_DESC, DATA_ATTR_ICPOS_LEN);
	outFmtLen += DATA_ATTR_ICPOS_LEN;
	outFmtStr[outFmtLen] = '|';
	outFmtLen += 1;



	/*根据配置组装pin code*/
	if (printerParam->isUsePin)
	{
		PRT_LOG_INFO("use pin code: %s", printerParam->pinCode);

		strLen = strlen(printerParam->pinCode);
		memcpy(&outIcStr[outLen], printerParam->pinCode, strLen);
		outLen += strLen;
		outIcStr[outLen] = '|';
		outLen += 1;

		memcpy(&outFmtStr[outFmtLen], ATTR_PIN_CODE_DESC, DATA_ATTR_ICPOS_LEN);
		outFmtLen += DATA_ATTR_ICPOS_LEN;
		outFmtStr[outFmtLen] = '|';
		outFmtLen += 1;
	}

	/*读卡器类型*/
	strTmp.Format("%d", printerParam->cardReaderType);
	strLen = strTmp.GetLength();
	memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
	outLen += strLen;
	outIcStr[outLen] = '|';
	outLen += 1;

	memcpy(&outFmtStr[outFmtLen], ATTR_CARD_READER_DESC, DATA_ATTR_ICPOS_LEN);
	outFmtLen += DATA_ATTR_ICPOS_LEN;
	outFmtStr[outFmtLen] = '|';
	outFmtLen += 1;



	if (printerParam->isWrMatchBank)
	{
		PRT_LOG_INFO("match bask code");

		strTmp.Format("%d", printerParam->isWrMatchBank);
		strLen = strTmp.GetLength();
		memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
		outLen += strLen;
		outIcStr[outLen] = '|';
		outLen += 1;

		memcpy(&outFmtStr[outFmtLen], ATTR_WR_MATCH_BANK_DESC, DATA_ATTR_ICPOS_LEN);
		outFmtLen += DATA_ATTR_ICPOS_LEN;
		outFmtStr[outFmtLen] = '|';
		outFmtLen += 1;
	}

	if (printerParam->isWrMatchAtr)
	{
		PRT_LOG_INFO("match atr value");

		strTmp.Format("%d", printerParam->isWrMatchAtr);
		strLen = strTmp.GetLength();
		memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
		outLen += strLen;
		outIcStr[outLen] = '|';
		outLen += 1;

		memcpy(&outFmtStr[outFmtLen], ATTR_WR_MATCH_ATR_DESC, DATA_ATTR_ICPOS_LEN);
		outFmtLen += DATA_ATTR_ICPOS_LEN;
		outFmtStr[outFmtLen] = '|';
		outFmtLen += 1;
	}

	if (printerParam->isWrForceMiyao)
	{
		PRT_LOG_INFO("force write miyao");

		strTmp.Format("%d", printerParam->isWrForceMiyao);
		strLen = strTmp.GetLength();
		memcpy(&outIcStr[outLen], strTmp.GetBuffer(0), strLen);
		outLen += strLen;
		outIcStr[outLen] = '|';
		outLen += 1;

		memcpy(&outFmtStr[outFmtLen], ATTR_WR_FORCE_MIYAO_DESC, DATA_ATTR_ICPOS_LEN);
		outFmtLen += DATA_ATTR_ICPOS_LEN;
		outFmtStr[outFmtLen] = '|';
		outFmtLen += 1;
	}

	*outIcStrLen = outLen;
	*outFmtStrLen = outFmtLen;
}

