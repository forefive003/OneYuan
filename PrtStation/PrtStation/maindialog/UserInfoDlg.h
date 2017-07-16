#pragma once

#include "prtCommon.h"
#include "PrtTaskHdl.h"
#include "afxwin.h"

#include "XPEdit.h"

// UserInfoDlg dialog

class UserInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(UserInfoDlg)

public:
	UserInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~UserInfoDlg();

	void setTaskInfo(char *strTime, char *strName);

// Dialog Data
	enum { IDD = IDD_USERINFO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk1();
	virtual BOOL OnInitDialog();

private:
	char m_strTimeBuf[64];
	char m_strOwnerName[OWNER_TEXT_MAX_LEN + 1];

	CEdit m_edtPlace;
	CEdit m_edtIder;
	CEdit m_edtCardNo;
	CEdit m_edtName;

	XPEdit m_edtTaskStatus;
	XPEdit m_edtPrtStatus;
	XPEdit m_edtWrStatus;

	CImage	m_image;
public:
	afx_msg void OnPaint();
private:
	CButton m_btnOk;
public:
	afx_msg void OnBnClickedCancel1();
	CEdit m_edtTime;
	virtual void PostNcDestroy();
	virtual void OnCancel();
	virtual void OnOK();
};
