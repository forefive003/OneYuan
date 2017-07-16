

#include "afxdisp.h"
#include "prtCommon.h"
#include "prtGlobal.h"
#include "serverComm.h"


#if 0
#import "msxml3.dll" named_guids

#import "C:\Program Files (x86)\Common Files\MSSoap\Binaries\mssoap30.dll" \
            exclude("IStream", "IErrorInfo", "ISequentialStream", "_LARGE_INTEGER", \
                    "_ULARGE_INTEGER", "tagSTATSTG", "_FILETIME")
#else
#import "..\\Release\\lib\\soap\\msxml4.dll"

#import "..\\Release\\lib\\soap\\mssoap30.dll" \
            exclude("IStream", "IErrorInfo", "ISequentialStream", "_LARGE_INTEGER", \
                    "_ULARGE_INTEGER", "tagSTATSTG", "_FILETIME")
#endif

using namespace MSSOAPLib30; 

static int _parseTaskXml(const char *xmlStr,
		int *taskSessionId, char **taskData, int *taskDataLen,
		char *taskSerialNo)
{
	char taskXmlFile[MAX_PATH + 1] = {0};
	SNPRINTF(taskXmlFile, MAX_PATH, "%s/taskData.xml", g_logDir);

	FILE* pTmp = fopen(taskXmlFile, "w+");
	if (NULL == pTmp)
	{
		PRT_LOG_ERROR("fail to create %s.", taskXmlFile);
		return ET_ERROR;
	}

	fputs(xmlStr, pTmp);
	fclose(pTmp);

	int iRet = OK;

	IXMLDOMDocumentPtr m_plDomDocument;
	HRESULT hr = m_plDomDocument.CreateInstance("Msxml2.DOMDocument.4.0");
	if(FAILED(hr))
	{
		_com_error er(hr);
		PRT_LOG_ERROR("初始化失败：%s", er.ErrorMessage());
		return ET_ERROR;
	}

	IXMLDOMElementPtr XMLROOT;
	IXMLDOMElementPtr XMLELEMENT;
	IXMLDOMNodeListPtr XMLNODES; //某个节点的所以字节点
	IXMLDOMNamedNodeMapPtr XMLNODEATTS;//某个节点的所有属性;
	IXMLDOMNodePtr XMLNODE;

	BSTR nodeName;
	BSTR text;
	_bstr_t bNodeName;
	_bstr_t bText;

	BSTR taskDataText;
	_bstr_t bTaskDataText;

	char *taskDataStr = NULL;
	int taskDataStrLen = 0;

	_variant_t varXml(taskXmlFile);

	VARIANT_BOOL varOut;
	m_plDomDocument->load(varXml, &varOut);
	m_plDomDocument->get_documentElement(&XMLROOT);//获得根节点;

	if (XMLROOT == NULL)
	{
		PRT_LOG_ERROR("解析xml文件失败.");
		iRet = ET_ERROR;
		m_plDomDocument.Release();
		return iRet;
	}

	XMLROOT->get_childNodes(&XMLNODES);//获得根节点的所有子节点;
	long XMLNODESNUM, ATTSNUM;
	XMLNODES->get_length(&XMLNODESNUM);//获得所有子节点的个数;

	if (XMLNODESNUM != 4)
	{
		PRT_LOG_ERROR("属性个数错误：%d", XMLNODESNUM);
		iRet = ET_ERROR;
		goto parseEnd;
	}

	char *chNodeName = NULL;
	char *chText = NULL;

	int index = 0;

	XMLNODES->get_item(index,&XMLNODE);//获得某个子节点;
	XMLNODE->get_attributes(&XMLNODEATTS);//获得某个节点的所有属性;
	XMLNODEATTS->get_length(&ATTSNUM);//获得所有属性的个数;

	XMLNODE->get_nodeName(&nodeName);
	XMLNODE->get_text(&text);

	bNodeName = nodeName;
	chNodeName = bNodeName;
	bText = text;
	chText = bText;

	if (strncmp(bNodeName, "strTaskID", strlen("strTaskID")) != 0)
	{
		PRT_LOG_ERROR("invalue node name：%s", bNodeName);
		iRet = ET_ERROR;
		goto parseEnd;
	}
	*taskSessionId = atoi(chText);

	XMLNODE.Release();
	XMLNODEATTS.Release();

	index = 1;

	XMLNODES->get_item(index,&XMLNODE);//获得某个子节点;
	XMLNODE->get_attributes(&XMLNODEATTS);//获得某个节点的所有属性;
	XMLNODEATTS->get_length(&ATTSNUM);//获得所有属性的个数;

	XMLNODE->get_nodeName(&nodeName);
	XMLNODE->get_text(&taskDataText);

	bNodeName = nodeName;
	chNodeName = bNodeName;
	bTaskDataText = taskDataText;
	taskDataStr = bTaskDataText;

	if (strncmp(bNodeName, "strlsh", strlen("strlsh")) != 0)
	{
		PRT_LOG_ERROR("invalue node name：%s", bNodeName);
		iRet = ET_ERROR;
		goto parseEnd;
	}

	strncpy(taskSerialNo, taskDataStr, 8);

	XMLNODE.Release();
	XMLNODEATTS.Release();


	index = 2;

	XMLNODES->get_item(index,&XMLNODE);//获得某个子节点;
	XMLNODE->get_attributes(&XMLNODEATTS);//获得某个节点的所有属性;
	XMLNODEATTS->get_length(&ATTSNUM);//获得所有属性的个数;

	XMLNODE->get_nodeName(&nodeName);
	XMLNODE->get_text(&taskDataText);

	bNodeName = nodeName;
	chNodeName = bNodeName;
	bTaskDataText = taskDataText;
	taskDataStr = bTaskDataText;

	if (strncmp(bNodeName, "strTaskData", strlen("strTaskData")) != 0)
	{
		PRT_LOG_ERROR("invalue node name：%s", bNodeName);
		iRet = ET_ERROR;
		goto parseEnd;
	}

	XMLNODE.Release();
	XMLNODEATTS.Release();

	index = 3;

	XMLNODES->get_item(index,&XMLNODE);//获得某个子节点;
	XMLNODE->get_attributes(&XMLNODEATTS);//获得某个节点的所有属性;
	XMLNODEATTS->get_length(&ATTSNUM);//获得所有属性的个数;

	XMLNODE->get_nodeName(&nodeName);
	XMLNODE->get_text(&text);

	bNodeName = nodeName;
	chNodeName = bNodeName;
	bText = text;
	chText = bText;

	if (strncmp(bNodeName, "strTaskLen", strlen("strTaskLen")) != 0)
	{
		PRT_LOG_ERROR("invalue node name：%s", bNodeName);
		iRet = ET_ERROR;
		goto parseEnd;
	}
	taskDataStrLen = atoi(chText);
	PRT_LOG_INFO("get taskData, type %d, len %d.", *taskSessionId, taskDataStrLen);

	*taskData = (char*)malloc(taskDataStrLen+1);
	if (*taskData == NULL)
	{
		PRT_LOG_ERROR("malloc failed, %d", taskDataStrLen+1);
		iRet = ET_ERROR;
		goto parseEnd;
	}

	strncpy(*taskData, taskDataStr, taskDataStrLen);
	*taskDataLen = taskDataStrLen+1;

	XMLNODE.Release();
	XMLNODEATTS.Release();

parseEnd:
	XMLNODES.Release();
	XMLROOT.Release();

	m_plDomDocument.Release();

	return iRet;
}
void WriteLogStr(CString log, int lastErr = 0)
{
	USES_CONVERSION;
	

	COleDateTime timeNow;						
	timeNow = COleDateTime::GetCurrentTime();
	CString  curTime = timeNow.Format("%Y-%m-%d %H:%M:%S");

	CString fileName = CString(_T("C:\\DataCardTrace\\Gy_GetData")) + curTime + CString(_T(".txt"));

	FILE* hfile = fopen(fileName.GetBuffer(0), "a+");
	if (hfile)
	{
		fseek(hfile, 0, SEEK_END);
		fprintf(hfile, "%s\n", log);
		fclose(hfile);
	}
	
	//char ctime[64]; memset(ctime, 0, 64);

	//sprintf_s(ctime, (_T("%02dH-%02dM-%02dS-%03dMs")), sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

	//CString  curTime = CString(ctime);

	//memset(ctime, 0, 64);
	//sprintf_s(ctime, (_T("Gy_GetData %dY-%dM-%dD")), sysTime.wYear, sysTime.wMonth, sysTime.wDay);

	//CString  fileName = CString(ctime);;

	//fileName = CString(CString(_T("C:\\DataCardTrace\\"))) + fileName + CString(CString(_T(".log")));

	//FILE* hfile = NULL;
	//fopen_s(&hfile, fileName.GetBuffer(0), CString(_T("at+,ccs=UTF-8")));
	//if (hfile)
	//{
	//	fseek(hfile, 0, SEEK_END);
	//	_fprintf_p(hfile, CString("%s: %s\n"), curTime, log);
	//	fclose(hfile);
	//}
};

int wsGetTask(int *taskSessionId, char **taskData, int *taskDataLen,
			char *taskSerialNo)
{
	char tmp[64] = {0};
	char weburl[128] = {0};

	int ret = OK;

	PRT_REG_CFG_T *registerCfg = registerCfgGet();
	char *jiamiyinzi = jiamiyinziGet();

	HRESULT hr = CoInitialize(NULL);

	try
	{
		ISoapSerializerPtr Serializer;
		ISoapReaderPtr Reader;
		ISoapConnectorPtr Connector;
		// Connect to the service.
		Connector.CreateInstance(__uuidof(HttpConnector30));

		SNPRINTF(weburl, sizeof(weburl) - 1, "http://%s:%u%s",
					registerCfg->serverAddrDesc, registerCfg->serverPort,
					registerCfg->serverRegUrl);

		Connector->Property["EndPointURL"] = weburl;
		Connector->Connect();

		// Begin the message.
		Connector->Property["SoapAction"] = "";
		Connector->Property["Timeout"] = "5000";
		Connector->BeginMessage();

		// Create the SoapSerializer object.
		Serializer.CreateInstance(__uuidof(SoapSerializer30));

		// Connect the serializer object to the input stream of the connector object.
		Serializer->Init(_variant_t((IUnknown*)Connector->InputStream));

		// Build the SOAP Message.
		Serializer->StartEnvelope("","http://schemas.xmlsoap.org/soap/encoding/","");
		Serializer->StartBody("");
		Serializer->StartElement("getTask","http://service.com","","");  //这是本地的Web Services,实际中要指定命名空间

		Serializer->StartElement("encryptIndex","","","");
		Serializer->WriteString(jiamiyinzi);
		Serializer->EndElement();

		Serializer->EndElement();
		Serializer->EndBody();
		Serializer->EndEnvelope();

		// Send the message to the XML Web service.
		Connector->EndMessage();

		// Read the response.
		Reader.CreateInstance(__uuidof(SoapReader30));

		// Connect the reader to the output stream of the connector object.
		Reader->Load(_variant_t((IUnknown*)Connector->OutputStream), "");
		// Display the result.
		if (strncmp((const char*)Reader->RpcResult->text, "no task", strlen("no task")) == 0)
		{
			PRT_LOG_DEBUG("no task when get task.");
			ret = SERVER_NO_TASK;
		}
		else
		{
			PRT_LOG_DEBUG("获取到任务数据:%s", (const char*)Reader->RpcResult->text);
			
			if(OK != _parseTaskXml((const char*)Reader->RpcResult->text,
								taskSessionId, taskData, taskDataLen, taskSerialNo))
			{
				ret = ET_ERROR;
			}
		}
	}
	catch(_com_error &e)
	{
		PRT_LOG_ERROR("获取任务数据异常: %s", e.ErrorMessage());
		ret = ET_ERROR;
	}

	CoUninitialize();

	return ret;
}

int wsSendRegInfo()
{
#if 0
	const char endpoint[] = "http://52.74.250.165:8080/axis2/services/testweb";

	testwebSoap11BindingProxy ClientSoap;					//XXXXSoapProxy 就是你的类名

	_ns1__add addReq;
	_ns1__addResponse addResp;

	addReq.a = &regInfo->serverPort;
	addReq.b = &regInfo->localPort;
	if (SOAP_OK == ClientSoap.add(endpoint, NULL, &addReq, addResp))
	{
		CString str;
		str.Format(_T("%d + %d = %d"), *(addReq.a), *(addReq.b), *(addResp.return_));
		MessageBox(NULL, str, "成功", MB_OK);

		*ret = ERROR;
		return;
	}
#endif
	char tmp[64] = {0};
	char weburl[128] = {0};

	int ret = OK;

	PRT_REG_CFG_T *registerCfg = registerCfgGet();
	char *jiamiyinzi = jiamiyinziGet();
	PRT_PRINTER_CFG_T *printerCfg = printerCfgGet();

	HRESULT hr = CoInitialize(NULL);

	try
	{
		ISoapSerializerPtr Serializer;
		ISoapReaderPtr Reader;
		ISoapConnectorPtr Connector;
		// Connect to the service.
		Connector.CreateInstance(__uuidof(HttpConnector30)); 

		SNPRINTF(weburl, sizeof(weburl) - 1, "http://%s:%u%s",
					registerCfg->serverAddrDesc, registerCfg->serverPort,
					registerCfg->serverRegUrl);

		Connector->Property["EndPointURL"] = weburl;
		Connector->Connect();

		// Begin the message.
		Connector->Property["SoapAction"] = "";
		/*超时10秒*/
		Connector->Property["Timeout"] = "10000";
		Connector->BeginMessage();

		// Create the SoapSerializer object.
		Serializer.CreateInstance(__uuidof(SoapSerializer30));

		// Connect the serializer object to the input stream of the connector object.
		Serializer->Init(_variant_t((IUnknown*)Connector->InputStream));
		
		// Build the SOAP Message.
		Serializer->StartEnvelope("","http://schemas.xmlsoap.org/soap/encoding/","");
		Serializer->StartBody("");
		Serializer->StartElement("cardDeviceRegister","http://service.com","","");  //这是本地的Web Services,实际中要指定命名空间
   
		Serializer->StartElement("workStationIP","","","");
		Serializer->WriteString(registerCfg->localAddrDesc);
		Serializer->EndElement();

		Serializer->StartElement("workStationPort","","","");
		SNPRINTF(tmp, 47, "%d", registerCfg->localPort);
		Serializer->WriteString(tmp);
		Serializer->EndElement();

		Serializer->StartElement("cardDeviceName","","","");
		Serializer->WriteString(printerCfg->printer);
		Serializer->EndElement();

		Serializer->StartElement("encryptIndex","","","");
		Serializer->WriteString(jiamiyinzi);
		Serializer->EndElement();

		Serializer->EndElement();
		Serializer->EndBody();
		Serializer->EndEnvelope();
   
		// Send the message to the XML Web service.
		Connector->EndMessage();     

		// Read the response.
		Reader.CreateInstance(__uuidof(SoapReader30));

		// Connect the reader to the output stream of the connector object.
		Reader->Load(_variant_t((IUnknown*)Connector->OutputStream), "");
				
		// Display the result.
		if (strncmp((const char*)Reader->RpcResult->text, "0", 1) == 0)
		{
			PRT_LOG_INFO("注册到%s成功", weburl);

			MessageBox(NULL, _T("注册成功"), "成功", MB_OK);
		}
		else
		{
			PRT_LOG_INFO("注册到%s失败: %s", weburl, (const char*)Reader->RpcResult->text);

			MessageBox(NULL, _T("注册失败"), "失败", MB_OK);
			ret = ET_ERROR;
		}
	}
	catch(_com_error &e)
	{
		PRT_LOG_INFO("注册到%s异常: %s", weburl, e.ErrorMessage());
		MessageBox(NULL, _T("注册异常"), "失败", MB_OK);

		ret = ET_ERROR;
	}

	CoUninitialize();

	return ret;
}

int wsSendPrtResult(BOOL isOK, char *failReason,
			char *cardId, char *atrValue, char *bankCode,
			char *resultOutStr, int taskSessionId)
{
	int iRet = OK;
	char tmp[64] = {0};
	char weburl[128] = {0};

	PRT_REG_CFG_T *registerCfg = registerCfgGet();
	
	HRESULT hr = CoInitialize(NULL);

	try
	{
		ISoapSerializerPtr Serializer;
		ISoapReaderPtr Reader;
		ISoapConnectorPtr Connector;
		// Connect to the service.
		Connector.CreateInstance(__uuidof(HttpConnector30));

		SNPRINTF(weburl, sizeof(weburl) - 1, "http://%s:%u%s",
							registerCfg->serverAddrDesc, registerCfg->serverPort,
							registerCfg->serverResultUrl);

		Connector->Property["EndPointURL"] = weburl;
		Connector->Connect();

		// Begin the message.
		Connector->Property["SoapAction"] = "";
		/*超时50秒*/
		Connector->Property["Timeout"] = "50000";
		Connector->BeginMessage();

		// Create the SoapSerializer object.
		Serializer.CreateInstance(__uuidof(SoapSerializer30));

		// Connect the serializer object to the input stream of the connector object.
		Serializer->Init(_variant_t((IUnknown*)Connector->InputStream));

		// Build the SOAP Message.
		Serializer->StartEnvelope("","http://schemas.xmlsoap.org/soap/encoding/","");
		Serializer->StartBody("");
		Serializer->StartElement("returnPrintResult","http://service.com","","");  //这是本地的Web Services,实际中要指定命名空间

		Serializer->StartElement("isOK","","","");
		SNPRINTF(tmp, sizeof(tmp) - 1, "%d", isOK);
		Serializer->WriteString(tmp);
		Serializer->EndElement();

		Serializer->StartElement("SBYY","","","");
		Serializer->WriteString(failReason);
		Serializer->EndElement();

		Serializer->StartElement("KSBM","","","");
		Serializer->WriteString(cardId);
		Serializer->EndElement();

		Serializer->StartElement("ATR","","","");
		Serializer->WriteString(atrValue);
		Serializer->EndElement();

		Serializer->StartElement("YHKH","","","");
		Serializer->WriteString(bankCode);
		Serializer->EndElement();

		Serializer->StartElement("taskSessionID","","","");
		SNPRINTF(tmp, sizeof(tmp) - 1, "%d", taskSessionId);
		Serializer->WriteString(tmp);
		Serializer->EndElement();

		Serializer->EndElement();
		Serializer->EndBody();
		Serializer->EndEnvelope();

		// Send the message to the XML Web service.
		Connector->EndMessage();

		// Read the response.
		Reader.CreateInstance(__uuidof(SoapReader30));

		// Connect the reader to the output stream of the connector object.
		Reader->Load(_variant_t((IUnknown*)Connector->OutputStream), "");

		// Display the result.
		if (strncmp((const char*)Reader->RpcResult->text, "0", 1) == 0)
		{
			//MessageBox(NULL, _T("注册成功！"), "成功", MB_OK);
		}
		else
		{
			SNPRINTF(resultOutStr, SEND_FAIL_STR_LEN-1, _T("%s"), (const char*)Reader->RpcResult->text);
		 	iRet = ET_ERROR;
		}
	}
	catch(_com_error &e)
	{
		SNPRINTF(resultOutStr, SEND_FAIL_STR_LEN-1, "%s", e.ErrorMessage());
		iRet = ET_ERROR;
	}

	CoUninitialize();

	return iRet;
}


