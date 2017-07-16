#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CFromInfoDlg 对话框

class CFromInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFromInfoDlg)

public:
	CFromInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFromInfoDlg();

// 对话框数据
	enum { IDD = IDD_DLG_FORM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtWebTitle;
	CEdit m_edtFormTitle;
	afx_msg void OnBnClickedOk();
	virtual INT_PTR DoModal();
	CListCtrl m_listFormFileds;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
};
