#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CExcelLoadDlg 对话框

class CExcelLoadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExcelLoadDlg)

public:
	CExcelLoadDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExcelLoadDlg();

// 对话框数据
	enum { IDD = IDD_DLG_EXCELLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtExcelFile;
	afx_msg void OnBnClickedBtnFilesel();
	afx_msg void OnBnClickedBtnLast();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnLoad();
	//afx_msg void OnBnClickedCancel();
	CListCtrl m_listElment;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnFileload();
	afx_msg void OnBnClickedBtnScanform();

private:
	void setListColumnText();
	void setListColumnCaptain();
public:
	CButton m_btnPrev;
	CButton m_btnNext;
	CStatic m_stcTotal;
	CStatic m_stcCur;
};
