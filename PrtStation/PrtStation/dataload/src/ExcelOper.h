#pragma once

#include "CApplication.h"
#include "CFont0.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"

#include "autoload.h"

enum
{
	GET_ROW_ERROR = 100,
	GET_ROW_BOTTOM,
	GET_ROW_TOP
};

class CExcelOper
{
public:
	CExcelOper(void);
	~CExcelOper(void);

public:
	int init(CString &strFilePath);
	void free();

	int getFirstRow();
	int getNextRow();
	int getPrevRow();

	int getColNum();
	int getRowNum();
	int getCurRowNum();

	/*当前行的数据*/
	CString m_curRowData[ELM_MAX];
	/*excel文件路径*/
	CString m_filePath;

private:
	int getRowData(int row);
	
	int m_isInited;

	CApplication m_app;
	CWorkbook m_book;
	CWorkbooks m_books;
	CWorksheet m_sheet;
	CWorksheets m_sheets;
	CRange m_range;

	//取得行数
	long m_RowNum;
	//取得列数
	long m_ColNum;         
	//取得已使用区域的起始行，从1开始
	long m_CurRow;	
};

