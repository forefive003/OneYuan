#pragma once
#include "afxwin.h"
#include "prtCommon.h"

// CPrintInfoDbg 对话框

class CPrintInfoDbg : public CDialogEx
{
	DECLARE_DYNAMIC(CPrintInfoDbg)

public:
	CPrintInfoDbg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPrintInfoDbg();

// 对话框数据
	enum { IDD = IDD_DLG_PRINT };

private:
	int getTestPrintInfo(PRT_INFO_T *cardInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedPrintOk();
	afx_msg void OnBnClickedPrintcancel();
	virtual BOOL OnInitDialog();
	CImage	m_image;
	afx_msg void OnPaint();
	CButton m_chkPrint;
	CButton m_chkWriteIc;
	afx_msg void OnBnClickedBtnPhotoSel();
	afx_msg void OnBnClickedButPrtIcChk();
	afx_msg void OnBnClickedButPrtReset();
	afx_msg void OnBnClickedButPrtClear();
};
