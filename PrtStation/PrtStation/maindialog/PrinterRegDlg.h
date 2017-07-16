#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// PrinterRegDlg 对话框

class PrinterRegDlg : public CDialogEx
{
	DECLARE_DYNAMIC(PrinterRegDlg)

public:
	PrinterRegDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~PrinterRegDlg();

// 对话框数据
	enum { IDD = IDD_DLG_REG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CComboBox m_listPrt;
	CComboBox m_listJimika;
	afx_msg void OnBnClickedCancel();
	CIPAddressCtrl m_ServerIp;

	afx_msg void OnCbnSelchangeComPrinter();
	afx_msg void OnCbnSelchangeCombJimika();
	afx_msg void OnCbnSelchangeComStation();
	CComboBox m_combStations;

private:
	void init_show(PRT_REG_CFG_T *regInfo, PRT_PRINTER_CFG_T *printerCfg);
public:
	CComboBox m_combLocalIpaddr;
};
