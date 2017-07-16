// FromInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PrtStation.h"
#include "FromInfoDlg.h"
#include "afxdialogex.h"

#include "prtGlobal.h"

// CFromInfoDlg 对话框

IMPLEMENT_DYNAMIC(CFromInfoDlg, CDialogEx)

CFromInfoDlg::CFromInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFromInfoDlg::IDD, pParent)
{

}

CFromInfoDlg::~CFromInfoDlg()
{
}

void CFromInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_WEB_TITLE, m_edtWebTitle);
	DDX_Control(pDX, IDC_EDIT_WEB_TITLE1, m_edtFormTitle);
	DDX_Control(pDX, IDC_LIST_FORM, m_listFormFileds);
}


BEGIN_MESSAGE_MAP(CFromInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CFromInfoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CFromInfoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CFromInfoDlg 消息处理程序


void CFromInfoDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString webTitle, formTitle;

	m_edtWebTitle.GetWindowText(webTitle);
	m_edtFormTitle.GetWindowText(formTitle);

	//if (webTitle.IsEmpty())
	//{
	//	MessageBox(_T("请指定web标题！"), _T("提示"));
	//	return;
	//}

	if( 1 != g_browserOper.formScan(webTitle, formTitle))
	{
		MessageBox(_T("获取表单信息失败,请检查是否在浏览器上打开！"), _T("提示"));
		return;
	}

	if (0 == g_tmp_fields_cnt)
	{
		MessageBox(_T("表单不存在,请检查是否在浏览器上打开！"), _T("提示"));
		return;
	}

	int nRow = 0;
	m_listFormFileds.DeleteAllItems();
	for (int i = 0; i < g_tmp_fields_cnt; i++)
	{
		nRow = m_listFormFileds.InsertItem(0, g_tmp_fields[i].formName);//插入行
		m_listFormFileds.SetItemText(nRow, 1, g_form_type_desc[g_tmp_fields[i].formType]);
		m_listFormFileds.SetItemText(nRow, 2, g_tmp_fields[i].value);//设置数据				
	}
	
	//CString tmpStr;
	//tmpStr.Format("获取到%d个字段信息", g_tmp_fields_cnt);
	//MessageBox(tmpStr, _T("提示"));
	//CDialogEx::OnOK();
}


INT_PTR CFromInfoDlg::DoModal()
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialogEx::DoModal();
}


BOOL CFromInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	LONG lStyle; 
	lStyle = GetWindowLong(m_listFormFileds.m_hWnd, GWL_STYLE);//获取当前窗口style 
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位 
	lStyle |= LVS_REPORT; //设置style 
	SetWindowLong(m_listFormFileds.m_hWnd, GWL_STYLE, lStyle);//设置style 
	DWORD dwStyle = m_listFormFileds.GetExtendedStyle(); 
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl） 
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl） 
	m_listFormFileds.SetExtendedStyle(dwStyle); //设置扩展风格 

	CRect rectList;
	m_listFormFileds.GetClientRect(rectList); //获得当前客户区信息
	m_listFormFileds.InsertColumn( 0, "字段名", LVCFMT_LEFT, rectList.Width()/4);//插入列
	m_listFormFileds.InsertColumn( 1, "类型", LVCFMT_LEFT, rectList.Width()/4);
	m_listFormFileds.InsertColumn( 2, "值", LVCFMT_LEFT, rectList.Width()/2);

	m_edtWebTitle.SetWindowText(g_browserOper.m_webTitle);
	m_edtFormTitle.SetWindowText(g_browserOper.m_formTitle);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CFromInfoDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
