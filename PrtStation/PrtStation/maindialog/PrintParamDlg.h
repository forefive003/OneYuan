#pragma once
#include "afxwin.h"


// CPrintParamDlg 对话框

class CPrintParamDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPrintParamDlg)

public:
	CPrintParamDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPrintParamDlg();

// 对话框数据
	enum { IDD = IDD_DLG_PRINTER_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	CComboBox m_listShebaokaSlot;

	CIPAddressCtrl m_JimijiIp;
	virtual BOOL OnInitDialog();
	CButton m_chkUsePin;
	afx_msg void OnBnClickedChkPin();
	CComboBox m_listPrtRO;
	CComboBox m_listJimikaRO;
	CComboBox m_listCardReader;
	CButton m_chkIcDebug;
	CComboBox m_listJiamiType;
	afx_msg void OnCbnSelchangeCombJiamiType();
	CEdit m_edtStationNum;
	CEdit m_edtSerialNum;
	CButton m_chkMatchBank;
	CButton m_chkMatchAtr;
	CButton m_chkForceMiyao;
	afx_msg void OnBnClickedCancel();
	CComboBox m_listPrtJiamiType;
	CComboBox m_listUserKaType;
	CComboBox m_listDoType;
	CEdit m_edtTishiThrd;
	CEdit m_edtStopThrd;
	CComboBox m_cmbDllType;
};
