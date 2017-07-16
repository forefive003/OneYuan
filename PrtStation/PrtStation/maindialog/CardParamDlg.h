#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "XPEdit.h"

// CCardParamDlg 对话框

class CCardParamDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCardParamDlg)

public:
	CCardParamDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCardParamDlg();

// 对话框数据
	enum { IDD = IDD_DLG_TEMP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	CButton m_xmChkBox;
	XPEdit m_xmPosXEdit;
	CSpinButtonCtrl m_xmPosXSpin;
	XPEdit m_xmPosYEdit;
	CSpinButtonCtrl m_xmPosYSpin;

	CButton m_xbChkBox;
	XPEdit m_xbPosXEdit;
	CSpinButtonCtrl m_xbPosXSpin;
	XPEdit m_xbPosYEdit;
	CSpinButtonCtrl m_xbPosYSpin;

	CButton m_khChkBox;
	XPEdit m_khPosXEdit;
	CSpinButtonCtrl m_khPosXSpin;
	XPEdit m_khPosYEdit;
	CSpinButtonCtrl m_khPosYSpin;

	CButton m_bzhChkBox;
	XPEdit m_bzhPosXEdit;
	CSpinButtonCtrl m_bzhPosXSpin;
	XPEdit m_bzhPosYEdit;
	CSpinButtonCtrl m_bzhPosYSpin;

	CButton m_zpChkBox;
	XPEdit m_zpPosXEdit;
	CSpinButtonCtrl m_zpPosXSpin;
	XPEdit m_zpPosYEdit;
	CSpinButtonCtrl m_zpPosYSpin;

	CButton m_fkrqChkBox;
	XPEdit m_fkrqPosXEdit;
	CSpinButtonCtrl m_fkrqPosXSpin;
	XPEdit m_fkrqPosYEdit;
	CSpinButtonCtrl m_fkrqPosYSpin;
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckXm();
	afx_msg void OnBnClickedCheckXb();
	afx_msg void OnBnClickedCheckKh();
	afx_msg void OnBnClickedCheckBzh();
	afx_msg void OnBnClickedCheckZp();
	afx_msg void OnBnClickedCheckFkrq();
	CButton m_xmChkStatic;
	CButton m_xbChkStatic;
	CButton m_khChkStatic;
	CButton m_bzhChkStatic;
	CButton m_fkrqChkStatic;
	CButton m_fkdChkBox;

	XPEdit m_fkdPosXEdit;
	CSpinButtonCtrl m_fkdPosXSpin;
	XPEdit m_fkdPosYEdit;
	CSpinButtonCtrl m_fkdPosYSpin;
	CButton m_fkdChkStatic;
	afx_msg void OnBnClickedCheckFkd();
	afx_msg void OnDeltaposSpinXmX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinXmY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinXbX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinXbY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinKhX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinKhY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBzhX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBzhY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinZpX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinZpY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinFkrqX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinFkrqY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinFkdX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinFkdY(NMHDR *pNMHDR, LRESULT *pResult);
};
