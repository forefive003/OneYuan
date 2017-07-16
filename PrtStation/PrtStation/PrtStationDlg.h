
// PrtStationDlg.h : 头文件
//

#pragma once

#include "afxcmn.h"
#include "XPButton.h"
#include "XPListCtrl.h"
#include "afxwin.h"
#include "SplashCtrl.h"

// CPrtStationDlg 对话框
class CPrtStationDlg : public CDialogEx
{
// 构造
public:
	CPrtStationDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PRTSTATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	BOOL m_splashTimeUp; /*是否flash定时到*/
	BOOL m_initializeOk; /*是否初始化完成*/
	BOOL m_splashCompleted; /*是否flash完成*/

	HICON m_hIcon;
	CStatusBarCtrl  m_StatusBar;
	CSplashCtrl splashDlg;
	void StartProxyDlg();

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam,LPARAM lParam); 
	afx_msg LRESULT OnUpdateTaskList(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUpdateStatusBar(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnPopupUserDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPopupWarnDlg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CXPButton m_butReg;
	CXPButton m_butPrtParam;
	CXPButton m_butCardParam;
	CXPButton m_butTest;
	CXPButton m_btnAuto;
	CXPButton m_btnProxy;

	CXPListCtrl m_listCtrlTask;

	afx_msg void OnIdrMenu();
	afx_msg void OnMenuReg();
	afx_msg void OnPrinttest();
	afx_msg void OnBnClickedOk();

	afx_msg void OnMenuQuit();
	afx_msg void OnMenuHide();
	afx_msg void OnPrtparam();
	afx_msg void OnPrintParamClick();
	afx_msg void OnBnClickedButReg();
	afx_msg void OnBnClickedButPrtParam();
	afx_msg void OnBnClickedButCardParam();
	afx_msg void OnBnClickedButTest();
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();

	afx_msg void OnBnClickedCheckAutoStart();
	CButton m_chkAutoStart;
	CButton m_chkStopGetTask;
	afx_msg void OnBnClickedCheckAutoStart2();
	afx_msg void OnBnClickedButAuto();
	
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_edtSpareSD;
	CEdit m_edtPrtCardCnt;
	CStatic m_stcSouhou;
	afx_msg void OnBnClickedButProxy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void display();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
