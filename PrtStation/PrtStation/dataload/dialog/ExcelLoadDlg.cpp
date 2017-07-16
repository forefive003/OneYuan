// ExcelLoadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PrtStation.h"
#include "ExcelLoadDlg.h"
#include "afxdialogex.h"

#include "FromInfoDlg.h"
#include "prtGlobal.h"

// CExcelLoadDlg 对话框

IMPLEMENT_DYNAMIC(CExcelLoadDlg, CDialogEx)

CExcelLoadDlg::CExcelLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExcelLoadDlg::IDD, pParent)
{

}

CExcelLoadDlg::~CExcelLoadDlg()
{
}

void CExcelLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_edtExcelFile);
	DDX_Control(pDX, IDC_LIST_INFO, m_listElment);
	DDX_Control(pDX, ID_BTN_LAST, m_btnPrev);
	DDX_Control(pDX, ID_BTN_NEXT, m_btnNext);
	DDX_Control(pDX, IDC_STC_TOTAL, m_stcTotal);
	DDX_Control(pDX, IDC_STC_CUR, m_stcCur);
}


BEGIN_MESSAGE_MAP(CExcelLoadDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_FILESEL, &CExcelLoadDlg::OnBnClickedBtnFilesel)
	ON_BN_CLICKED(ID_BTN_LAST, &CExcelLoadDlg::OnBnClickedBtnLast)
	ON_BN_CLICKED(ID_BTN_NEXT, &CExcelLoadDlg::OnBnClickedBtnNext)
	ON_BN_CLICKED(ID_BTN_LOAD, &CExcelLoadDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_FILELOAD, &CExcelLoadDlg::OnBnClickedBtnFileload)
	ON_BN_CLICKED(ID_BTN_SCANFORM, &CExcelLoadDlg::OnBnClickedBtnScanform)
END_MESSAGE_MAP()


// CExcelLoadDlg 消息处理程序


void CExcelLoadDlg::OnBnClickedBtnFilesel()
{
	// TODO: 在此添加控件通知处理程序代码
	
	 // 设置过滤器   
    TCHAR szFilter[] = _T("excel文件(*.xls)|*.xls");   
    // 构造打开文件对话框   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // 显示打开文件对话框   
    if (IDOK == fileDlg.DoModal())   
    {   
        // 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
        strFilePath = fileDlg.GetPathName(); 
		m_edtExcelFile.SetWindowText(strFilePath);

		/*自动导出*/
		this->OnBnClickedBtnFileload();
    }   
}

void CExcelLoadDlg::setListColumnText()
{
	int nRow = 0;

	m_listElment.DeleteAllItems();
	for (int i = g_browserOper.m_fieldCnt - 1; i >= 0; i--)
	{
		nRow = m_listElment.InsertItem(0, g_browserOper.m_fields[i].name);//插入行
		m_listElment.SetItemText(nRow, 1, g_excelOper->m_curRowData[i]);			
	}
}

void CExcelLoadDlg::setListColumnCaptain()
{
	int nRow = 0;

	m_listElment.DeleteAllItems();
	for (int i = g_browserOper.m_fieldCnt - 1; i >= 0; i--)
	{
		nRow = m_listElment.InsertItem(0, g_browserOper.m_fields[i].name);//插入行
		m_listElment.SetItemText(nRow, 1, "");			
	}
}


void CExcelLoadDlg::OnBnClickedBtnFileload()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFilePath; 
	m_edtExcelFile.GetWindowText(strFilePath);

	/*先销毁之前的*/
	if (g_excelOper != NULL)
	{
		delete g_excelOper;
		g_excelOper = NULL;
	}

	g_excelOper = new CExcelOper;
	if(1 != g_excelOper->init(strFilePath))
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("初始化Excel文件失败!"), _T("错误"));
		return;
	}

	int colNum = g_excelOper->getColNum();
	int rowNum = g_excelOper->getRowNum();

	/*行数判断*/
	if (rowNum == 0)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("Excel文件无标题!"), _T("错误"));
		return;
	}

	/*字段个数比较*/
	if (colNum != g_browserOper.m_fieldCnt)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		CString tmpStr;
		tmpStr.Format(_T("Excel文件列数(%d)与配置文件中字段个数(%d)不相符!"), colNum, g_browserOper.m_fieldCnt);
		MessageBox(tmpStr, _T("错误"));
		return;
	}

	/*获取列名*/
	if (1 != g_excelOper->getFirstRow())
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("Excel文件获取标题失败!"), _T("错误"));
		return;
	}

	/*比较配置文件*/
	for (int i = 0; i < colNum; i++)
	{
		if (0 != strcmp(((LPSTR)(LPCTSTR)g_excelOper->m_curRowData[i]), g_browserOper.m_fields[i].name))
		{
			CString tmpStr;
			tmpStr.Format(_T("Excel文件第%d列(%s)与配置文件中字段描述(%s)不相符!"), i+1, 
				(LPSTR)(LPCTSTR)(g_excelOper->m_curRowData[i]),
				g_browserOper.m_fields[i].name);
			MessageBox(tmpStr, _T("错误"));

			delete g_excelOper;
			g_excelOper = NULL;
			return;
		}
	}

	if (rowNum == 1)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("Excel文件无数据!"), _T("提示"));
		return;
	}

	/*获取第一行数据*/
	int ret = g_excelOper->getNextRow();
	if (GET_ROW_BOTTOM == ret)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("已到Excel文件尾!"), _T("提示"));
		return;
	}
	else if (1 != ret)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("Excel文件获取数据失败!"), _T("错误"));
		return;
	}

	CString tmpStr1;
	tmpStr1.Format("总共 %u 行", rowNum-1);
	this->m_stcTotal.SetWindowText(tmpStr1);

	tmpStr1.Format("当前第 1 行");
	this->m_stcCur.SetWindowText(tmpStr1);
	this->m_btnPrev.EnableWindow(false);
	if (rowNum == 2)
	{
		/*只有一行数据*/
		this->m_btnNext.EnableWindow(false);
	}
	else
	{
		this->m_btnNext.EnableWindow(true);
	}

	this->setListColumnText();
}

void CExcelLoadDlg::OnBnClickedBtnLast()
{
	// TODO: 在此添加控件通知处理程序代码
	if (NULL == g_excelOper)
	{
		MessageBox(_T("没有指定Excel文件!"), _T("提示"));
		return;
	}

	int ret = g_excelOper->getPrevRow();
	if (GET_ROW_TOP == ret)
	{
		MessageBox(_T("已到Excel文件头!"), _T("提示"));
		return;
	}
	else if (1 != ret)
	{
		MessageBox(_T("Excel文件获取数据失败!"), _T("错误"));
		return;
	}

	this->setListColumnText();

	int curRowNum = g_excelOper->getCurRowNum();
	CString tmpStr1;
	tmpStr1.Format("当前第 %u 行", curRowNum-1);
	this->m_stcCur.SetWindowText(tmpStr1);

	if (curRowNum == 2)
	{
		this->m_btnPrev.EnableWindow(false);
	}
	this->m_btnNext.EnableWindow(true);
}

void CExcelLoadDlg::OnBnClickedBtnNext()
{
	// TODO: 在此添加控件通知处理程序代码
	if (NULL == g_excelOper)
	{
		MessageBox(_T("没有指定Excel文件!"), _T("提示"));
		return;
	}

	int ret = g_excelOper->getNextRow();
	if (GET_ROW_BOTTOM == ret)
	{
		MessageBox(_T("已到Excel文件尾!"), _T("提示"));
		return;
	}
	else if (1 != ret)
	{
		MessageBox(_T("Excel文件获取数据失败!"), _T("错误"));
		return;
	}

	this->setListColumnText();

	int rowNum = g_excelOper->getRowNum();
	int curRowNum = g_excelOper->getCurRowNum();

	CString tmpStr1;
	tmpStr1.Format("当前第 %u 行", curRowNum-1);
	this->m_stcCur.SetWindowText(tmpStr1);

	if (rowNum == curRowNum)
	{
		this->m_btnNext.EnableWindow(false);
	}
	this->m_btnPrev.EnableWindow(true);
}

void CExcelLoadDlg::OnBnClickedBtnLoad()
{
	// TODO: 在此添加控件通知处理程序代码
	if (NULL == g_excelOper)
	{
		MessageBox(_T("没有指定Excel文件!"), _T("提示"));
		return;
	}

	int curRowNum = g_excelOper->getCurRowNum();
	int colNum = g_excelOper->getColNum();
	if (curRowNum > 1)
	{
		/*当前有数据*/
		g_browserOper.formFieldFill(g_excelOper->m_curRowData, colNum);
		if (g_tmp_wr_fields_cnt < colNum)
		{
			CString tmpStr1;
			tmpStr1.Format(_T("填充 %u 个字段,请确认网页上是否存在 %s 页面!"), 
					g_tmp_wr_fields_cnt, (LPCSTR)(LPCTSTR)g_tmp_webTitle);
			MessageBox(tmpStr1, _T("提示"));
		}
		else
		{
			MessageBox(_T("填充完成!"), _T("提示"));
		}
	}
}

void CExcelLoadDlg::OnBnClickedBtnScanform()
{
	// TODO: 在此添加控件通知处理程序代码
	CFromInfoDlg  formDlg;
	formDlg.DoModal();
}

BOOL CExcelLoadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	LONG lStyle; 
	lStyle = GetWindowLong(m_listElment.m_hWnd, GWL_STYLE);//获取当前窗口style 
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位 
	lStyle |= LVS_REPORT; //设置style 
	SetWindowLong(m_listElment.m_hWnd, GWL_STYLE, lStyle);//设置style 
	DWORD dwStyle = m_listElment.GetExtendedStyle(); 
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl） 
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl） 
	m_listElment.SetExtendedStyle(dwStyle); //设置扩展风格 

	CRect rectList;
	m_listElment.GetClientRect(rectList); //获得当前客户区信息
	m_listElment.InsertColumn( 0, "字段", LVCFMT_LEFT, rectList.Width()/4);//插入列
	m_listElment.InsertColumn( 1, "值", LVCFMT_LEFT, rectList.Width() * 3/4);
	
	if (g_excelOper != NULL)
	{
		/*之前打开过文档，直接导入数据*/
		m_edtExcelFile.SetWindowText(g_excelOper->m_filePath);
		this->setListColumnText();

		int curRowNum = g_excelOper->getCurRowNum();
		int rowNum = g_excelOper->getRowNum();

		CString tmpStr1;
		tmpStr1.Format("总共 %u 行", rowNum-1);
		this->m_stcTotal.SetWindowText(tmpStr1);

		tmpStr1.Format("当前第 %u 行", curRowNum-1);
		this->m_stcCur.SetWindowText(tmpStr1);

		if (curRowNum <= 2)
		{
			/*已经到行首*/
			this->m_btnPrev.EnableWindow(false);
		}

		if (curRowNum == rowNum)
		{
			/*已经到行尾*/
			this->m_btnNext.EnableWindow(false);
		}
	}
	else
	{
		/*没有打开，展示列表头*/
		this->setListColumnCaptain();
		this->m_btnPrev.EnableWindow(false);
		this->m_btnNext.EnableWindow(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
