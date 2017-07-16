#include "StdAfx.h"
#include <afxinet.h>
#include "BrowserOper.h"
#include "prtGlobal.h"


#import "msxml3.dll" named_guids
using namespace MSXML2;


static void EnumForm(IHTMLDocument2 * pIHTMLDocument2);
static void EnumFrame(IHTMLDocument2 * pIHTMLDocument2);

/*临时变量，为简化代码*/
CString g_tmp_webTitle;
CString g_tmp_formTitle;

FIELD_INFO_T g_tmp_fields[ELM_MAX];
int g_tmp_fields_cnt;
int (*g_field_hdl_func)(LPCTSTR lpName, LPCTSTR lpType, LPCTSTR lpValue) = NULL;
/*每次刷新的表单字段个数*/
int g_tmp_wr_fields_cnt = 0;

char* g_form_type_desc[] = {
	"other",
	"text",
	"textarea",
	"checkbox",
	"radio",
	"select",
	"hidden",
};


CBrowserOper::CBrowserOper(void)
{
}

CBrowserOper::~CBrowserOper(void)
{
}

int CBrowserOper::getFormTypeByName(char *formtype)
{
	if (strcmp(formtype, "text") == 0)
	{
		return FORM_TEXT;
	}
	else if (strcmp(formtype, "textarea") == 0)
	{
		return FORM_TEXT_AREA;
	}
	else if (strcmp(formtype, "select") == 0)
	{
		return FORM_LIST;
	}
	else if (strcmp(formtype, "checkbox") == 0)
	{
		return FORM_CHK_BOX;
	}
	else if (strcmp(formtype, "radio") == 0)
	{
		return FORM_RADIO;
	}
	else if (strcmp(formtype, "hidden") == 0)
	{
		return FORM_HIDDEN;
	}

	/*其他类型不处理*/
	return FORM_UNUSED;
}

int CBrowserOper::formReadHandle(CComDispatchDriver &spInputElement,
						LPCTSTR lpName, LPCTSTR lpType, LPCTSTR lpValue)
{
	PRT_LOG_INFO("read [%s, %s = %s]", lpName, lpType, lpValue);  

	if (g_tmp_fields_cnt >= ELM_MAX)
	{
		/*已经满了，不再填充*/
		return 0;
	}

	g_tmp_fields[g_tmp_fields_cnt].formType = getFormTypeByName((char*)lpType);
	if (FORM_UNUSED == g_tmp_fields[g_tmp_fields_cnt].formType)
	{
		/*其他类型不处理*/
		return 0;
	}

	strncpy(g_tmp_fields[g_tmp_fields_cnt].formName, lpName, MAX_FIELD_FORM_NAME);
	g_tmp_fields[g_tmp_fields_cnt].formName[MAX_FIELD_FORM_NAME - 1] = 0;
	strncpy(g_tmp_fields[g_tmp_fields_cnt].value, lpValue, MAX_FIELD_VALUE_LEN);
	g_tmp_fields[g_tmp_fields_cnt].value[MAX_FIELD_VALUE_LEN - 1] = 0;
	g_tmp_fields_cnt++;

	return 1;
}

int CBrowserOper::formWriteHandle(CComDispatchDriver &spInputElement,
						LPCTSTR lpName, LPCTSTR lpType, LPCTSTR lpValue)
{
	PRT_LOG_INFO("get [%s, %s = %s] before write", lpName, lpType, lpValue); 

	CString tmpStr;

	if (0 == strncmp(lpName, "dto['aac002_2']", MAX_FIELD_FORM_NAME)
				&& 0 == strncmp(lpType, "text", 32))
	{
		/*填充身份证值*/
		CComVariant vSetStatus(g_tmp_fields[ELM_IDNUM].value);	
		spInputElement.PutPropertyByName(L"value",&vSetStatus);
		return 0;
	}

	int isHidden = 0;
	int index = this->getIndexByFormname((LPTSTR)lpName, (LPTSTR)lpType, &isHidden);
	if (index < 0 || index >= ELM_MAX)
	{
		return 0;
	}
	
	/*找到hiden字段*/
	if (isHidden == 1)
	{
		if (index == ELM_SEX)
		{
			if (0 == strncmp(g_tmp_fields[index].value, "男", MAX_FIELD_VALUE_LEN))
			{
				tmpStr = "1";
			}
			else if (0 == strncmp(g_tmp_fields[index].value, "女", MAX_FIELD_VALUE_LEN))
			{
				tmpStr = "2";
			}
			else
			{
				tmpStr = "9";
			}

			PRT_LOG_INFO("write sex [%s, %s = %s]", lpName, lpType, (LPTSTR)(LPCTSTR)tmpStr);  
			CComVariant vSetStatus(tmpStr);	
			spInputElement.PutPropertyByName(L"value",&vSetStatus);
		}
		if (index == ELM_MINZU)
		{
			char *code = g_mingzuMgr.getCodeByName(g_tmp_fields[index].value);
			tmpStr.Format("%s", code);

			PRT_LOG_INFO("write mingzu [%s, %s = %s]", lpName, lpType, (LPTSTR)(LPCTSTR)tmpStr);  
			CComVariant vSetStatus(tmpStr);	
			spInputElement.PutPropertyByName(L"value",&vSetStatus);
		}
		else if (index == ELM_BANK_CODE)
		{
			char *code = g_bankMgr.getCodeByName(g_tmp_fields[index].value);
			tmpStr.Format("%s", code);

			PRT_LOG_INFO("write bank code [%s, %s = %s]", lpName, lpType, (LPTSTR)(LPCTSTR)tmpStr);  
			CComVariant vSetStatus(tmpStr);	
			spInputElement.PutPropertyByName(L"value",&vSetStatus);
		}
		else if (index == ELM_BANK_PLACE)
		{
			char *code = g_bankWangdianMgr.getCodeByName(g_tmp_fields[index].value);
			tmpStr.Format("%s", code);

			PRT_LOG_INFO("write bank place [%s, %s = %s]", lpName, lpType, (LPTSTR)(LPCTSTR)tmpStr);  
			CComVariant vSetStatus(tmpStr);	
			spInputElement.PutPropertyByName(L"value",&vSetStatus);		
		}
		else if (index == ELM_SHEBAO_PLACE)
		{
			char *code = g_placeMgr.getCodeByName(g_tmp_fields[index].value);
			tmpStr.Format("%s", code);

			PRT_LOG_INFO("write shebao place [%s, %s = %s]", lpName, lpType, (LPTSTR)(LPCTSTR)tmpStr);  
			CComVariant vSetStatus(tmpStr);	
			spInputElement.PutPropertyByName(L"value",&vSetStatus);	
		}
		else if (index == ELM_IDTYPE)
		{
			tmpStr.Format("%s", g_tmp_fields[index].value);

			PRT_LOG_INFO("write idtype [%s, %s = %s]", lpName, lpType, (LPTSTR)(LPCTSTR)tmpStr);  
			CComVariant vSetStatus(tmpStr);	
			spInputElement.PutPropertyByName(L"value",&vSetStatus);	
		}
		else if (index == ELM_XINZ_QH)
		{
			char *code = g_placeMgr.getCodeByName(g_tmp_fields[index].value);
			tmpStr.Format("%s", code);

			PRT_LOG_INFO("write xinzheng quhua id [%s, %s = %s]", lpName, lpType, (LPTSTR)(LPCTSTR)tmpStr);
			CComVariant vSetStatus(tmpStr);
			spInputElement.PutPropertyByName(L"value", &vSetStatus);
		}

		return 1;
	}
	
	/*字段格式转换*/	
	if (index == ELM_IDTYPE)
	{
		char *name = g_zengjianMgr.getNameByCode(g_tmp_fields[index].value);
		tmpStr.Format("%s", name);

		PRT_LOG_INFO("write idtype [%s, %s = %s]", lpName, lpType, (LPTSTR)(LPCTSTR)tmpStr);  
		CComVariant vSetStatus(tmpStr);	
		spInputElement.PutPropertyByName(L"value",&vSetStatus);	
	}
	else if (index == ELM_IDNUM)
	{
		/*身份证号去掉最后一个字符，让客户填入，网页才能自动获取*/
		int len = strlen(g_tmp_fields[index].value);
		g_tmp_fields[index].value[len - 1] = 0;
		tmpStr.Format("%s", g_tmp_fields[index].value);
	}
	else if (index == ELM_EXPIRE_TIME)
	{
		if (strlen(g_tmp_fields[index].value) >= 8)
		{
			char dataStr[16] = {0};
			tmpStr.Format("%c%c%c%c%c%c%c%c", g_tmp_fields[index].value[0],
				g_tmp_fields[index].value[1],
				g_tmp_fields[index].value[2],
				g_tmp_fields[index].value[3],
				g_tmp_fields[index].value[4],
				g_tmp_fields[index].value[5],
				g_tmp_fields[index].value[6],
				g_tmp_fields[index].value[7]);
		}
		else
		{
			tmpStr = "";
		}
	}
	else if (index == ELM_BIRTHDAY)
	{
		if (strlen(g_tmp_fields[index].value) >= 8)
		{
			char dataStr[16] = {0};
			tmpStr.Format("%c%c%c%c-%c%c-%c%c", g_tmp_fields[index].value[0],
				g_tmp_fields[index].value[1],
				g_tmp_fields[index].value[2],
				g_tmp_fields[index].value[3],
				g_tmp_fields[index].value[4],
				g_tmp_fields[index].value[5],
				g_tmp_fields[index].value[6],
				g_tmp_fields[index].value[7]);
		}
		else
		{
			tmpStr = "";
		}
	}
	else 
	{
		tmpStr.Format("%s", g_tmp_fields[index].value);
	}

	PRT_LOG_INFO("write [%s, %s = %s], value %s", lpName, lpType, 
			(LPTSTR)(LPCTSTR)tmpStr, g_tmp_fields[index].value);  

	/*填充值*/
	CComVariant vSetStatus(tmpStr);	
	spInputElement.PutPropertyByName(L"value",&vSetStatus);
	g_tmp_wr_fields_cnt++;

	return 1;
}

void CBrowserOper::EnumFrame( IHTMLDocument2 * pIHTMLDocument2)  
{  
	if ( !pIHTMLDocument2 ) return; 

	HRESULT hr;  
	CComPtr< IHTMLFramesCollection2 > spFramesCollection2;  
	pIHTMLDocument2->get_frames( &spFramesCollection2 ); //取得框架frame的集合  

	long nFrameCount=0;    //取得子框架个数  
	hr = spFramesCollection2->get_length( &nFrameCount );  
	if ( FAILED ( hr ) || 0 == nFrameCount ) return;  

	for(long i=0; i<nFrameCount; i++)  
	{  
		CComVariant vDispWin2;  //取得子框架的自动化接口  
		hr = spFramesCollection2->item( &CComVariant(i), &vDispWin2 ); 
		if ( FAILED ( hr ) ) continue;  

		CComQIPtr< IHTMLWindow2 > spWin2 = vDispWin2.pdispVal;  
		if( !spWin2 ) continue; //取得子框架的 IHTMLWindow2 接口  
 
		CComPtr < IHTMLDocument2 > spDoc2;  
		spWin2->get_document( &spDoc2 ); //取得字框架的 IHTMLDocument2 接口  
  
		EnumForm( spDoc2 );   //递归枚举当前子框架 IHTMLDocument2 上的表单form  
	} 
}  
 
void CBrowserOper::EnumForm( IHTMLDocument2 * pIHTMLDocument2 )  
{  
	if( !pIHTMLDocument2 ) return;  
  
	EnumFrame( pIHTMLDocument2 ); //递归枚举当前 IHTMLDocument2 上的子框架frame
  
	HRESULT hr;  
	//CComBSTR bstrTitle;  
	//pIHTMLDocument2->get_title( &bstrTitle ); //取得文档标题  
  
	USES_CONVERSION;  
	//PRT_LOG_INFO("====================");  
	//PRT_LOG_INFO("开始枚举 %s 的表单", OLE2CT( bstrTitle ));
  
	CComQIPtr< IHTMLElementCollection > spElementCollection;  
	hr = pIHTMLDocument2->get_forms( &spElementCollection ); //取得表单集合  
	if ( FAILED( hr ) )  
	{  
		PRT_LOG_ERROR("获取表单的集合 IHTMLElementCollection 错误");  
		return;  
	}  
	
	long nFormCount=0;    //取得表单数目  
	hr = spElementCollection->get_length( &nFormCount ); 
	if ( FAILED( hr ) )  
	{  
		PRT_LOG_ERROR("获取表单数目错误");  
		return;  
	}  
	
	if (nFormCount == 0)
	{
		return;
	}

	CComBSTR bstrTitle;  
	pIHTMLDocument2->get_title( &bstrTitle ); //取得文档标题 
	PRT_LOG_INFO(">>>>>>>遍历表单 %s :", OLE2CT( bstrTitle ));

	for(long i=0; i<nFormCount; i++)  
	{  
		IDispatch *pDisp = NULL; //取得第 i 项表单  
		hr = spElementCollection->item( CComVariant( i ), CComVariant(), &pDisp );  
		if ( FAILED( hr ) )  continue;  

		CComQIPtr< IHTMLFormElement > spFormElement = pDisp;  
		pDisp->Release();  
 
		long nElemCount=0;   //取得表单中 域 的数目  
		hr = spFormElement->get_length( &nElemCount );  

		if ( FAILED( hr ) )  continue;  
  
		for(long j=0; j<nElemCount; j++)  
		{  
			CComDispatchDriver spInputElement; //取得第 j 项表单域  
			hr = spFormElement->item( CComVariant( j ), CComVariant(), &spInputElement );  
			if ( FAILED( hr ) ) continue;  
			
			CComVariant vName,vVal,vType;  //取得表单域的 名，值，类型					
			hr = spInputElement.GetPropertyByName( L"name", &vName );  
			if( FAILED( hr ) ) continue;  

			hr = spInputElement.GetPropertyByName( L"value", &vVal );  
			if( FAILED( hr ) ) continue;  

			hr = spInputElement.GetPropertyByName( L"type", &vType );  
			if( FAILED( hr ) ) continue;    

			LPCTSTR lpName = vName.bstrVal? OLE2CT( vName.bstrVal ) : _T("NULL"); //未知域名 
			LPCTSTR lpVal  = vVal.bstrVal?  OLE2CT( vVal.bstrVal  ) : _T("NULL"); //空值，未输入  
			LPCTSTR lpType = vType.bstrVal? OLE2CT( vType.bstrVal ) : _T("NULL"); //未知类型  
   
			if (this->m_curOperType == 0)
			{
				this->formReadHandle(spInputElement, lpName, lpType, lpVal);
			}
			else
			{
				this->formWriteHandle(spInputElement, lpName, lpType, lpVal);
			}
#if 0
			VARIANT vardisp;     
            vardisp.vt=VT_DISPATCH;     
            vardisp.pdispVal=pDisp;

			HRESULT hr = pIHTMLDocument2->put_onclick(vardisp);
            if (FAILED(hr))  
            {  
                continue;  
            }  

			IHTMLElement* pElem = NULL;  
			pDisp->QueryInterface(IID_IHTMLElement, (void**)&pElem);  
#endif	  

#if 0
			HRESULT hr = objDocument->put_onclick(vardisp);     
            if (FAILED(hr))  
            {  
                pElem->click();  
                continue;  
            }  

			IHTMLInputButtonElement *spInputText;  
			HRESULT rsc = pDisp->QueryInterface(IID_IHTMLInputButtonElement, (void**)&spInputText);  
			if (FAILED(rsc))  
			{  
				continue;  
			}  
			IHTMLElement* pElem = NULL;  
			spInputText->QueryInterface(IID_IHTMLElement, (void**)&pElem);  
			pElem->click();  
#endif
	    //提交表单
	    //pForm->submit(); 
		}
	}  
}  

int CBrowserOper::EnumBrowser(void)
{
	int ret = 1;

	::CoInitialize(NULL); //初始化 COM 公寓 

	CComPtr< IShellWindows > spShellWin;
	HRESULT hr = spShellWin.CoCreateInstance( CLSID_ShellWindows );  
	if ( FAILED ( hr ) )  
	{  
		PRT_LOG_ERROR("获取 IShellWindows 接口错误");  
		ret = 0; 
		goto end;
	}

	long nCount = 0;  // 取得浏览器实例个数(Explorer 和 IExplorer)  
	spShellWin->get_Count( &nCount );  

	PRT_LOG_INFO("running browser instance count %d.", nCount);  

	for(int i=0; i<nCount; i++)  
	{  
		CComPtr< IDispatch > spDispIE;  
		hr=spShellWin->Item(CComVariant( (long)i ), &spDispIE );  
		if ( FAILED ( hr ) ) continue;  

		CComQIPtr< IWebBrowser2 > spBrowser = spDispIE;  
		if ( !spBrowser )  continue;  
		//spDispIE.Release();

		CComPtr < IDispatch > spDispDoc;
		hr = spBrowser->get_Document( &spDispDoc ); 
		if ( FAILED ( hr ) ) continue;  

		CComQIPtr< IHTMLDocument2 > spDocument2 = spDispDoc;  
		if ( !spDocument2 )  continue;  

		// 程序运行到此，已经找到了 IHTMLDocument2 的接口指针  
		// 删除下行语句的注释，把浏览器的背景改变看看  
		//spDocument2->put_bgColor( CComVariant( "red" ) );    

		CComBSTR bstrTitle;  
		spDocument2->get_title( &bstrTitle ); //取得文档标题 
		
		USES_CONVERSION;   
		PRT_LOG_INFO("==========搜索到web实例 %s", OLE2CT( bstrTitle ));

		if (g_tmp_webTitle.IsEmpty())
		{
			//标题为空, 枚举所有的表单
			EnumForm( spDocument2 );  
		}
		else if (strcmp((LPSTR)(LPCTSTR)g_tmp_webTitle, OLE2CT( bstrTitle )) == 0)
		{
			//枚举指定标题的表单
			EnumForm( spDocument2 );  
		}		  
	} 

end:
	::CoUninitialize();  //释放 COM 公寓
	return ret;
}

int CBrowserOper::getIndexByFormname(char *formname, char *formtype, int *isHidden)
{
	for (int j = 0; j < this->m_fieldCnt; j++)
	{
		if (0 == strncmp(this->m_fields[j].formName, formname, MAX_FIELD_FORM_NAME)
			&& 0 == strncmp(g_form_type_desc[m_fields[j].formType], formtype, 32))
		{
			*isHidden = 0;
			return j;
		}

		if (this->m_fields[j].hasHiden)
		{
			if (0 == strncmp(this->m_fields[j].hidenFormName, formname, MAX_FIELD_FORM_NAME)
				&& 0 == strncmp(g_form_type_desc[m_fields[j].hidenformType], formtype, 32))
			{
				*isHidden = 1;
				return j;
			}

		}		
	}

	return -1;
}

int CBrowserOper::fmtConfigInit()
{
	int iRet = 1;

	CoInitialize(NULL);

	//AfxOleInit();

	MSXML2::IXMLDOMDocument2Ptr m_plDomDocument;
	HRESULT hr = m_plDomDocument.CreateInstance(MSXML2::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		PRT_LOG_ERROR("初始化失败：%s", er.ErrorMessage());
		return 0;
	}

	MSXML2::IXMLDOMElementPtr XMLROOT;
	MSXML2::IXMLDOMElementPtr XMLELEMENT;
	MSXML2::IXMLDOMNodeListPtr XMLNODES; //某个节点的所以字节点
	MSXML2::IXMLDOMNamedNodeMapPtr XMLNODEATTS;//某个节点的所有属性;
	MSXML2::IXMLDOMNodePtr XMLNODE_ELM;

	m_plDomDocument->load(g_configAutoFillDir);
	XMLROOT = m_plDomDocument->GetdocumentElement();//获得根节点;

	XMLROOT->get_childNodes(&XMLNODES);//获得根节点的所有子节点;
	long XMLNODESNUM, ATTSNUM;
	XMLNODES->get_length(&XMLNODESNUM);//获得所有子节点的个数;
		
	CString nodeNameStr;
	CString nodeValueStr;
	MSXML2::IXMLDOMNodePtr XMLNODE;

	if (XMLNODESNUM > (ELM_MAX + 1))
	{
		PRT_LOG_ERROR("属性个数太多：%d", XMLNODESNUM);
		iRet = ET_ERROR;
		goto parseEnd;
	}

	/*获取webtitles*/
	XMLNODES->get_item(0,&XMLNODE_ELM);//获得某个子节点;
	XMLNODE_ELM->get_attributes(&XMLNODEATTS);//获得某个节点的所有属性;
	XMLNODEATTS->get_length(&ATTSNUM);//获得所有属性的个数;

	for(int J=0;J<ATTSNUM;J++)
	{
		XMLNODEATTS->get_item(J,&XMLNODE);//获得某个属性;

		nodeNameStr = (char*)(_bstr_t)XMLNODE->nodeName;
		nodeValueStr = (char*)(_bstr_t)XMLNODE->text;

		if (strncmp((LPCTSTR)nodeNameStr, "webtitle", strlen("webtitle")) == 0)
		{
			this->m_webTitle = nodeValueStr;
		}
		else if (strncmp((LPCTSTR)nodeNameStr, "formtitle", strlen("formtitle")) == 0)
		{
			this->m_formTitle = nodeValueStr;
		}

		XMLNODE.Release();
	}

	XMLNODE_ELM.Release();
	XMLNODEATTS.Release();

	/*获取字段信息*/
	for(int I = 1; I < XMLNODESNUM; I++)
	{
		XMLNODES->get_item(I,&XMLNODE_ELM);//获得某个子节点;
		XMLNODE_ELM->get_attributes(&XMLNODEATTS);//获得某个节点的所有属性;
		XMLNODEATTS->get_length(&ATTSNUM);//获得所有属性的个数;

		int count = 0;
		FIELD_INFO_T field;
		memset(&field, 0, sizeof(field));

		for(int J=0;J<ATTSNUM;J++)
		{
			XMLNODEATTS->get_item(J,&XMLNODE);//获得某个属性;

			nodeNameStr = (char*)(_bstr_t)XMLNODE->nodeName;
			nodeValueStr = (char*)(_bstr_t)XMLNODE->text;

			if (strncmp((LPCTSTR)nodeNameStr, "name", strlen("name")) == 0)
			{
				strncpy(field.name, (LPCTSTR)nodeValueStr, MAX_FIELD_NAME);
				field.name[MAX_FIELD_NAME - 1] = 0;

				count++;
			}			
			else if (strncmp((LPCTSTR)nodeNameStr, "formtype", strlen("formtype")) == 0)
			{
				field.formType = getFormTypeByName((char*)(LPCTSTR)nodeValueStr);
				if (FORM_UNUSED == field.formType)
				{
					PRT_LOG_ERROR("formtype 无效的表单字段类型：%s", (LPCTSTR)nodeValueStr);
					/*其他类型不处理*/
					continue;
				}

				count++;
			}
			else if (strncmp((LPCTSTR)nodeNameStr, "formname", strlen("formname")) == 0)
			{
				strncpy(field.formName, (LPCTSTR)nodeValueStr, MAX_FIELD_FORM_NAME);
				field.formName[MAX_FIELD_FORM_NAME - 1] = 0;

				count++;
			}
			else if (strncmp((LPCTSTR)nodeNameStr, "hidenformtype", strlen("hidenformtype")) == 0)
			{
				field.hidenformType = getFormTypeByName((char*)(LPCTSTR)nodeValueStr);
				//if (FORM_UNUSED == field.hidenformType)
				//{
				//	PRT_LOG_ERROR("hidenformtype 无效的表单字段类型：%s", (LPCTSTR)nodeValueStr);
				//	/*其他类型不处理*/
				//	continue;
				//}

				count++;
			}
			else if (strncmp((LPCTSTR)nodeNameStr, "hidenformname", strlen("hidenformname")) == 0)
			{
				strncpy(field.hidenFormName, (LPCTSTR)nodeValueStr, MAX_FIELD_FORM_NAME);
				field.hidenFormName[MAX_FIELD_FORM_NAME - 1] = 0;

				count++;
			}
			else if (strncmp((LPCTSTR)nodeNameStr, "hashiden", strlen("hashiden")) == 0)
			{
				field.hasHiden = atoi( (LPCTSTR)nodeValueStr);

				count++;
			}

			XMLNODE.Release();
		}

		XMLNODE_ELM.Release();
		XMLNODEATTS.Release();

		if (count != 6)
		{
			PRT_LOG_ERROR("字段 %s 解析失败,属性个数不够：%d", field.name, count);
			iRet = ET_ERROR;
			goto parseEnd;
		}
		else
		{
			memcpy(&this->m_fields[I-1], &field, sizeof(field));
			PRT_LOG_INFO("add form field succ, name %s, formName %s, formType %d.", 
				field.name, field.formName, field.formType);
		}

		/*行数加一*/
		this->m_fieldCnt++;
	}

parseEnd:
	XMLNODES.Release();
	XMLROOT.Release();

	m_plDomDocument.Release();

	CoUninitialize();
	return iRet;
}

int CBrowserOper::init()
{
	return fmtConfigInit();
}

int CBrowserOper::formScan(CString &webTitle, CString &formTitle)
{
	g_tmp_webTitle = webTitle;
	g_tmp_formTitle = formTitle;
	memset(g_tmp_fields, 0, sizeof(g_tmp_fields));
	g_tmp_fields_cnt = 0;

	/*设置读标志*/
	m_curOperType = 0;
	PRT_LOG_INFO("读取表单>>>");
	return EnumBrowser();
}

int CBrowserOper::formFieldFill(CString *strField, int size)
{
	g_tmp_webTitle = this->m_webTitle;
	g_tmp_formTitle = this->m_formTitle;

	g_tmp_fields_cnt = size;
	if (g_tmp_fields_cnt > ELM_MAX)
	{
		g_tmp_fields_cnt = ELM_MAX;
	}

	for (int j = 0; j < g_tmp_fields_cnt; j++)
	{
		strncpy(g_tmp_fields[j].value, (LPSTR)(LPCTSTR)strField[j], MAX_FIELD_VALUE_LEN);
		g_tmp_fields[j].value[MAX_FIELD_VALUE_LEN - 1] = 0;
	}

	/*设置写标志*/
	m_curOperType = 1;
	g_tmp_wr_fields_cnt = 0;
	PRT_LOG_INFO("填充表单>>>");
	return EnumBrowser();
}