#include "StdAfx.h"
#include "ExcelOper.h"

#include "prtGlobal.h"


CExcelOper::CExcelOper(void)
{
	m_isInited = 0;
}

CExcelOper::~CExcelOper(void)
{
	if (m_isInited == 1)
	{
		this->free();
	}
}

int CExcelOper::init(CString &strFilePath)
{	
	LPDISPATCH lpDisp;

	try
	{
		//定义变量
		COleVariant covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR);

		AfxOleInit();

		//创建Excel 服务器(启动Excel)
		if (!m_app.CreateDispatch(_T("Excel.Application"),NULL))
		{
			PRT_LOG_ERROR(_T("无法创建Excel应用"));
			return 0;
		}

		/*得到工作簿容器*/
		m_books = m_app.get_Workbooks();

		//打开Excel，其中pathname为Excel表的路径名
		lpDisp = m_books.Open(strFilePath,covOptional
					,covOptional,covOptional,covOptional,covOptional,covOptional,covOptional
					,covOptional,covOptional,covOptional
					,covOptional,covOptional,covOptional
					,covOptional);
		m_book.AttachDispatch(lpDisp);

		/*得到工作簿中的Sheet的容器*/
		m_sheets = m_book.get_Worksheets();
		/*打开一个Sheet*/
		m_sheet = m_sheets.get_Item(COleVariant((short)1));

		//读取已经使用区域的信息，包括已经使用的行数、列数、起始行、起始列
		m_range.AttachDispatch(m_sheet.get_UsedRange());
		m_range.AttachDispatch(m_range.get_Rows());

		//取得已经使用的行数
		m_RowNum = m_range.get_Count();         
		m_range.AttachDispatch(m_range.get_Columns());
		//取得已经使用的列数
		m_ColNum = m_range.get_Count();  

		int j = 0;
		CString elmStr1,elmStr2,elmStr3;
		COleVariant rValue1,rValue2,rValue3;
		for (j = 1; j<=m_RowNum; j++)
		{
			elmStr1.Format("A%u",j);			
			m_range = m_sheet.get_Range(COleVariant(elmStr1), COleVariant(elmStr1));
			////获得单元格的内容			
			rValue1 = COleVariant(m_range.get_Value2());
			if(rValue1.vt == VT_EMPTY)
			{
				break;
			}

			elmStr2.Format("B%u",j);
			m_range = m_sheet.get_Range(COleVariant(elmStr2), COleVariant(elmStr2));
			////获得单元格的内容			
			rValue2 = COleVariant(m_range.get_Value2());
			if(rValue2.vt == VT_EMPTY)
			{
				break;
			}

			elmStr3.Format("C%u",j);
			m_range = m_sheet.get_Range(COleVariant(elmStr3), COleVariant(elmStr3));
			////获得单元格的内容			
			rValue3 = COleVariant(m_range.get_Value2());
			if(rValue3.vt == VT_EMPTY)
			{
				break;
			}
		}
		
		PRT_LOG_INFO(_T("Excel文件: %d行%d列, 实际%d行数据"), m_RowNum, m_ColNum, j-1);	
		m_RowNum = j-1;
		/*excel是从第1行开始*/
		m_CurRow = 1;
	}
	catch(_com_error &e)
	{
		PRT_LOG_ERROR("初始化Excel文件异常: %s", e.ErrorMessage());
		this->free();
		return 0;
	}

	/*保存路径*/
	this->m_filePath = strFilePath;
	m_isInited = 1;
	return 1;
}

void CExcelOper::free()
{
	/*释放资源*/
	m_range.ReleaseDispatch();
	m_sheet.ReleaseDispatch();
	m_sheets.ReleaseDispatch();
	m_book.ReleaseDispatch();
	m_books.ReleaseDispatch();
	m_app.Quit();
	m_app.ReleaseDispatch();

	m_isInited = 0;
}

int CExcelOper::getColNum()
{
	return this->m_ColNum;
}

int CExcelOper::getRowNum()
{
	return this->m_RowNum;
}

int CExcelOper::getCurRowNum()
{
	return this->m_CurRow;
}

int CExcelOper::getRowData(int row)
{
	COleVariant vResult;
	int j = 0;

	/*先清空*/
	for (j = 0; j < ELM_MAX; j++)
	{
		this->m_curRowData[j].Empty();
	}

	PRT_LOG_INFO("get data from row %d", row);

	try
	{
		for(int j=1; j <= m_ColNum; j++)
		{
			//读取单元格的值
			m_range.AttachDispatch(m_sheet.get_Cells());
			m_range.AttachDispatch(m_range.get_Item(COleVariant((long)row), COleVariant((long)j)).pdispVal);
			vResult = m_range.get_Value2();

			if(vResult.vt == VT_BSTR)     //若是字符串
			{
				this->m_curRowData[j-1] = vResult.bstrVal;
				PRT_LOG_INFO("colume %d: type VT_BSTR, value %s", j, (LPSTR)(LPCTSTR)m_curRowData[j-1]);
			}
			else if (vResult.vt == VT_R8) //8字节的数字
			{
				this->m_curRowData[j-1].Format("%f",vResult.dblVal);
				PRT_LOG_INFO("colume %d: type VT_R8, value %s", j, (LPSTR)(LPCTSTR)m_curRowData[j-1]);
			}
			else if(vResult.vt == VT_DATE) //时间格式
			{
				CString stry,strm,strd;
				SYSTEMTIME st;

				VariantTimeToSystemTime(vResult.date, &st);
				stry.Format("%d",st.wYear);
				strm.Format("%d",st.wMonth);
				strd.Format("%d",st.wDay);
				this->m_curRowData[j-1] = stry+L"-"+strm+L"-"+strd;

				PRT_LOG_INFO("colume %d: type VT_DATE, value %s", j, (LPSTR)(LPCTSTR)m_curRowData[j-1]);
			}
			else if(vResult.vt == VT_EMPTY) //单元为空
			{
				this->m_curRowData[j-1]="";
				PRT_LOG_INFO("colume %d: type VT_EMPTY", j);
			}
			else if (vResult.vt ==VT_I4)
			{
				this->m_curRowData[j-1].Format(_T("%ld"), (int)vResult.lVal);
				PRT_LOG_INFO("colume %d: type VT_I4, value %s", j, (LPSTR)(LPCTSTR)m_curRowData[j-1]);
			}
			else
			{
				this->m_curRowData[j-1]="";
				PRT_LOG_ERROR("colume %d: type unKnown", j);
			}
		}
	}
	catch(_com_error &e)
	{
		PRT_LOG_ERROR("获取Excel文件数据异常: %s", e.ErrorMessage());
		return 0;
	}

	return 1;
}

int CExcelOper::getFirstRow()
{
	m_CurRow = 1;

	if (1 != this->getRowData(m_CurRow))
	{
		return GET_ROW_ERROR;
	}
	return 1;
}

int CExcelOper::getNextRow()
{
	if (m_CurRow > this->m_RowNum)
	{
		return GET_ROW_BOTTOM;
	}

	m_CurRow++;

	if (1 != this->getRowData(m_CurRow))
	{
		return GET_ROW_ERROR;
	}
	return 1;
}

int CExcelOper::getPrevRow()
{
	if (m_CurRow == 1)
	{
		return GET_ROW_TOP;
	}

	m_CurRow--;

	if (1 != this->getRowData(m_CurRow))
	{
		return GET_ROW_ERROR;
	}
	return 1;
}