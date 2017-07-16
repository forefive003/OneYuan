// CCardParamDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PrtStation.h"
#include "CardParamDlg.h"
#include "afxdialogex.h"

#include "prtGlobal.h"

// CCardParamDlg 对话框

IMPLEMENT_DYNAMIC(CCardParamDlg, CDialogEx)

CCardParamDlg::CCardParamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCardParamDlg::IDD, pParent)
{

}

CCardParamDlg::~CCardParamDlg()
{
}

void CCardParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_XM, m_xmChkBox);
	DDX_Control(pDX, IDC_EDIT_XM_X, m_xmPosXEdit);
	DDX_Control(pDX, IDC_SPIN_XM_X, m_xmPosXSpin);
	DDX_Control(pDX, IDC_EDIT_XM_Y, m_xmPosYEdit);
	DDX_Control(pDX, IDC_SPIN_XM_Y, m_xmPosYSpin);

	DDX_Control(pDX, IDC_CHECK_XB, m_xbChkBox);
	DDX_Control(pDX, IDC_EDIT_XB_X, m_xbPosXEdit);
	DDX_Control(pDX, IDC_SPIN_XB_X, m_xbPosXSpin);
	DDX_Control(pDX, IDC_EDIT_XB_Y, m_xbPosYEdit);
	DDX_Control(pDX, IDC_SPIN_XB_Y, m_xbPosYSpin);

	DDX_Control(pDX, IDC_CHECK_KH, m_khChkBox);
	DDX_Control(pDX, IDC_EDIT_KH_X, m_khPosXEdit);
	DDX_Control(pDX, IDC_SPIN_KH_X, m_khPosXSpin);
	DDX_Control(pDX, IDC_EDIT_KH_Y, m_khPosYEdit);
	DDX_Control(pDX, IDC_SPIN_KH_Y, m_khPosYSpin);

	DDX_Control(pDX, IDC_CHECK_BZH, m_bzhChkBox);
	DDX_Control(pDX, IDC_EDIT_BZH_X, m_bzhPosXEdit);
	DDX_Control(pDX, IDC_SPIN_BZH_X, m_bzhPosXSpin);
	DDX_Control(pDX, IDC_EDIT_BZH_Y, m_bzhPosYEdit);
	DDX_Control(pDX, IDC_SPIN_BZH_Y, m_bzhPosYSpin);

	DDX_Control(pDX, IDC_CHECK_ZP, m_zpChkBox);
	DDX_Control(pDX, IDC_EDIT_ZP_X, m_zpPosXEdit);
	DDX_Control(pDX, IDC_SPIN_ZP_X, m_zpPosXSpin);
	DDX_Control(pDX, IDC_EDIT_ZP_Y, m_zpPosYEdit);
	DDX_Control(pDX, IDC_SPIN_ZP_Y, m_zpPosYSpin);

	DDX_Control(pDX, IDC_CHECK_FKRQ, m_fkrqChkBox);
	DDX_Control(pDX, IDC_EDIT_FKRQ_X, m_fkrqPosXEdit);
	DDX_Control(pDX, IDC_SPIN_FKRQ_X, m_fkrqPosXSpin);
	DDX_Control(pDX, IDC_EDIT_FKRQ_Y, m_fkrqPosYEdit);
	DDX_Control(pDX, IDC_SPIN_FKRQ_Y, m_fkrqPosYSpin);
	DDX_Control(pDX, IDC_CHECK_XM2, m_xmChkStatic);
	DDX_Control(pDX, IDC_CHECK_XB2, m_xbChkStatic);
	DDX_Control(pDX, IDC_CHECK_KH2, m_khChkStatic);
	DDX_Control(pDX, IDC_CHECK_BZH2, m_bzhChkStatic);
	DDX_Control(pDX, IDC_CHECK_FKRQ2, m_fkrqChkStatic);
	DDX_Control(pDX, IDC_CHECK_FKD, m_fkdChkBox);
	DDX_Control(pDX, IDC_EDIT_FKD_X, m_fkdPosXEdit);
	DDX_Control(pDX, IDC_SPIN_FKD_X, m_fkdPosXSpin);
	DDX_Control(pDX, IDC_EDIT_FKD_Y, m_fkdPosYEdit);
	DDX_Control(pDX, IDC_SPIN_FKD_Y, m_fkdPosYSpin);
	DDX_Control(pDX, IDC_CHECK_FKD2, m_fkdChkStatic);
}


BEGIN_MESSAGE_MAP(CCardParamDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCardParamDlg::OnBnClickedOk)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CHECK_XM, &CCardParamDlg::OnBnClickedCheckXm)
	ON_BN_CLICKED(IDC_CHECK_XB, &CCardParamDlg::OnBnClickedCheckXb)
	ON_BN_CLICKED(IDC_CHECK_KH, &CCardParamDlg::OnBnClickedCheckKh)
	ON_BN_CLICKED(IDC_CHECK_BZH, &CCardParamDlg::OnBnClickedCheckBzh)
	ON_BN_CLICKED(IDC_CHECK_ZP, &CCardParamDlg::OnBnClickedCheckZp)
	ON_BN_CLICKED(IDC_CHECK_FKRQ, &CCardParamDlg::OnBnClickedCheckFkrq)
	ON_BN_CLICKED(IDC_CHECK_FKD, &CCardParamDlg::OnBnClickedCheckFkd)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_XM_X, &CCardParamDlg::OnDeltaposSpinXmX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_XM_Y, &CCardParamDlg::OnDeltaposSpinXmY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_XB_X, &CCardParamDlg::OnDeltaposSpinXbX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_XB_Y, &CCardParamDlg::OnDeltaposSpinXbY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KH_X, &CCardParamDlg::OnDeltaposSpinKhX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_KH_Y, &CCardParamDlg::OnDeltaposSpinKhY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BZH_X, &CCardParamDlg::OnDeltaposSpinBzhX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BZH_Y, &CCardParamDlg::OnDeltaposSpinBzhY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ZP_X, &CCardParamDlg::OnDeltaposSpinZpX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ZP_Y, &CCardParamDlg::OnDeltaposSpinZpY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FKRQ_X, &CCardParamDlg::OnDeltaposSpinFkrqX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FKRQ_Y, &CCardParamDlg::OnDeltaposSpinFkrqY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FKD_X, &CCardParamDlg::OnDeltaposSpinFkdX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FKD_Y, &CCardParamDlg::OnDeltaposSpinFkdY)
END_MESSAGE_MAP()


// CCardParamDlg 消息处理程序


void CCardParamDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTmp;
	PRT_TEMP_CFG_T posInfo;
	memset(&posInfo, 0, sizeof(posInfo));

	posInfo.name.isPrint = m_xmChkBox.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_xmPosXEdit.GetWindowText(strTmp);
	posInfo.name.posX = _ttof(strTmp);
	m_xmPosYEdit.GetWindowText(strTmp);
	posInfo.name.posY = _ttof(strTmp);
	posInfo.name.isPrintStatic = m_xmChkStatic.GetCheck() == BST_CHECKED ? TRUE : FALSE;

	posInfo.sex.isPrint = m_xbChkBox.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_xbPosXEdit.GetWindowText(strTmp);
	posInfo.sex.posX = _ttof(strTmp);
	m_xbPosYEdit.GetWindowText(strTmp);
	posInfo.sex.posY = _ttof(strTmp);
	posInfo.sex.isPrintStatic = m_xbChkStatic.GetCheck() == BST_CHECKED ? TRUE : FALSE;

	posInfo.cardNo.isPrint = m_khChkBox.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_khPosXEdit.GetWindowText(strTmp);
	posInfo.cardNo.posX = _ttof(strTmp);
	m_khPosYEdit.GetWindowText(strTmp);
	posInfo.cardNo.posY = _ttof(strTmp);
	posInfo.cardNo.isPrintStatic = m_khChkStatic.GetCheck() == BST_CHECKED ? TRUE : FALSE;

	posInfo.personId.isPrint = m_bzhChkBox.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_bzhPosXEdit.GetWindowText(strTmp);
	posInfo.personId.posX = _ttof(strTmp);
	m_bzhPosYEdit.GetWindowText(strTmp);
	posInfo.personId.posY = _ttof(strTmp);
	posInfo.personId.isPrintStatic = m_bzhChkStatic.GetCheck() == BST_CHECKED ? TRUE : FALSE;

	posInfo.photo.isPrint = m_zpChkBox.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_zpPosXEdit.GetWindowText(strTmp);
	posInfo.photo.posX = _ttof(strTmp);
	m_zpPosYEdit.GetWindowText(strTmp);
	posInfo.photo.posY = _ttof(strTmp);

	posInfo.launchTime.isPrint = m_fkrqChkBox.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_fkrqPosXEdit.GetWindowText(strTmp);
	posInfo.launchTime.posX = _ttof(strTmp);
	m_fkrqPosYEdit.GetWindowText(strTmp);
	posInfo.launchTime.posY = _ttof(strTmp);
	posInfo.launchTime.isPrintStatic = m_fkrqChkStatic.GetCheck() == BST_CHECKED ? TRUE : FALSE;

	posInfo.launchPlace.isPrint = m_fkdChkBox.GetCheck() == BST_CHECKED ? TRUE : FALSE;
	m_fkdPosXEdit.GetWindowText(strTmp);
	posInfo.launchPlace.posX = _ttof(strTmp);
	m_fkdPosYEdit.GetWindowText(strTmp);
	posInfo.launchPlace.posY = _ttof(strTmp);
	posInfo.launchPlace.isPrintStatic = m_fkdChkStatic.GetCheck() == BST_CHECKED ? TRUE : FALSE;

	/*写配置*/
	templateCfgSave(&posInfo);

	MessageBox("设置成功!", "提示", MB_OK);
	CDialogEx::OnOK();
}


BOOL CCardParamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// TODO:  在此添加额外的初始化
	CString   strValue;

	PRT_TEMP_CFG_T* posInfo = templateCfgGet();

	m_xmChkBox.SetCheck(posInfo->name.isPrint);
	m_xmChkStatic.SetCheck(posInfo->name.isPrintStatic);

	//buddy方式不精确，手动改变edit的值，不能自适应
	//m_xmPosXSpin.SetBuddy((CWnd *)&m_xmPosXEdit);
	//m_xmPosXSpin.SetRange(0,1000);
	//m_xmPosXSpin.SetBase(10);
	//m_xmPosXSpin.SetPos(int(posInfo->name.posX * 10));
	//strValue.Format( _T("%3.1f "), posInfo->name.posX);
	//m_xmPosXEdit.SetWindowText(strValue);
	m_xmPosXEdit.SetDelta(1);
	m_xmPosXEdit.SetRange(0, 1000);
	m_xmPosXEdit.SetDecimal(int(posInfo->name.posX * 10));
	m_xmPosYEdit.SetDelta(1);
	m_xmPosYEdit.SetRange(0, 1000);
	m_xmPosYEdit.SetDecimal(int(posInfo->name.posY * 10));
	if (!posInfo->name.isPrint)
	{
		m_xmPosXSpin.EnableWindow(FALSE);
		m_xmPosYSpin.EnableWindow(FALSE);
		m_xmPosXEdit.EnableWindow(FALSE);
		m_xmPosYEdit.EnableWindow(FALSE);
		m_xmChkStatic.EnableWindow(FALSE);
	}




	m_xbChkBox.SetCheck(posInfo->sex.isPrint);
	m_xbChkStatic.SetCheck(posInfo->sex.isPrintStatic);

	m_xbPosXEdit.SetDelta(1);
	m_xbPosXEdit.SetRange(0, 1000);
	m_xbPosXEdit.SetDecimal(int(posInfo->sex.posX * 10));		
	m_xbPosYEdit.SetDelta(1);
	m_xbPosYEdit.SetRange(0, 1000);
	m_xbPosYEdit.SetDecimal(int(posInfo->sex.posY * 10));
	if (!posInfo->sex.isPrint)
	{
		m_xbPosXSpin.EnableWindow(FALSE);
		m_xbPosYSpin.EnableWindow(FALSE);
		m_xbPosXEdit.EnableWindow(FALSE);
		m_xbPosYEdit.EnableWindow(FALSE);
		m_xbChkStatic.EnableWindow(FALSE);
	}




	m_khChkBox.SetCheck(posInfo->cardNo.isPrint);
	m_khChkStatic.SetCheck(posInfo->cardNo.isPrintStatic);

	m_khPosXEdit.SetDelta(1);
	m_khPosXEdit.SetRange(0, 1000);
	m_khPosXEdit.SetDecimal(int(posInfo->cardNo.posX * 10));
	m_khPosYEdit.SetDelta(1);
	m_khPosYEdit.SetRange(0, 1000);
	m_khPosYEdit.SetDecimal(int(posInfo->cardNo.posY * 10));
	if (!posInfo->cardNo.isPrint)
	{
		m_khPosXSpin.EnableWindow(FALSE);
		m_khPosYSpin.EnableWindow(FALSE);
		m_khPosXEdit.EnableWindow(FALSE);
		m_khPosYEdit.EnableWindow(FALSE);
		m_khChkStatic.EnableWindow(FALSE);
	}



	m_bzhChkBox.SetCheck(posInfo->personId.isPrint);
	m_bzhChkStatic.SetCheck(posInfo->personId.isPrintStatic);

	m_bzhPosXEdit.SetDelta(1);
	m_bzhPosXEdit.SetRange(0, 1000);
	m_bzhPosXEdit.SetDecimal(int(posInfo->personId.posX * 10));
	m_bzhPosYEdit.SetDelta(1);
	m_bzhPosYEdit.SetRange(0, 1000);
	m_bzhPosYEdit.SetDecimal(int(posInfo->personId.posY * 10));
	if (!posInfo->personId.isPrint)
	{
		m_bzhPosXSpin.EnableWindow(FALSE);
		m_bzhPosYSpin.EnableWindow(FALSE);
		m_bzhPosXEdit.EnableWindow(FALSE);
		m_bzhPosYEdit.EnableWindow(FALSE);
		m_bzhChkStatic.EnableWindow(FALSE);
	}




	m_zpChkBox.SetCheck(posInfo->photo.isPrint);

	m_zpPosXEdit.SetDelta(1);
	m_zpPosXEdit.SetRange(0, 1000);
	m_zpPosXEdit.SetDecimal(int(posInfo->photo.posX * 10));
	m_zpPosYEdit.SetDelta(1);
	m_zpPosYEdit.SetRange(0, 1000);
	m_zpPosYEdit.SetDecimal(int(posInfo->photo.posY * 10));
	if (!posInfo->photo.isPrint)
	{
		m_zpPosXSpin.EnableWindow(FALSE);
		m_zpPosYSpin.EnableWindow(FALSE);
		m_zpPosXEdit.EnableWindow(FALSE);
		m_zpPosYEdit.EnableWindow(FALSE);
	}




	m_fkrqChkBox.SetCheck(posInfo->launchTime.isPrint);
	m_fkrqChkStatic.SetCheck(posInfo->launchTime.isPrintStatic);

	m_fkrqPosXEdit.SetDelta(1);
	m_fkrqPosXEdit.SetRange(0, 1000);
	m_fkrqPosXEdit.SetDecimal(int(posInfo->launchTime.posX * 10));
	m_fkrqPosYEdit.SetDelta(1);
	m_fkrqPosYEdit.SetRange(0, 1000);
	m_fkrqPosYEdit.SetDecimal(int(posInfo->launchTime.posY * 10));
	if (!posInfo->launchTime.isPrint)
	{
		m_fkrqPosXSpin.EnableWindow(FALSE);
		m_fkrqPosYSpin.EnableWindow(FALSE);
		m_fkrqPosXEdit.EnableWindow(FALSE);
		m_fkrqPosYEdit.EnableWindow(FALSE);
		m_fkrqChkStatic.EnableWindow(FALSE);
	}


	m_fkdChkBox.SetCheck(posInfo->launchPlace.isPrint);
	m_fkdChkStatic.SetCheck(posInfo->launchPlace.isPrintStatic);

	m_fkdPosXEdit.SetDelta(1);
	m_fkdPosXEdit.SetRange(0, 1000);
	m_fkdPosXEdit.SetDecimal(int(posInfo->launchPlace.posX * 10));
	m_fkdPosYEdit.SetDelta(1);
	m_fkdPosYEdit.SetRange(0, 1000);
	m_fkdPosYEdit.SetDecimal(int(posInfo->launchPlace.posY * 10));
	if (!posInfo->launchPlace.isPrint)
	{
		m_fkdPosXSpin.EnableWindow(FALSE);
		m_fkdPosYSpin.EnableWindow(FALSE);
		m_fkdPosXEdit.EnableWindow(FALSE);
		m_fkdPosYEdit.EnableWindow(FALSE);
		m_fkdChkStatic.EnableWindow(FALSE);
	}


	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CCardParamDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nSBCode == SB_ENDSCROLL)
	{
	    return;
	}

	//下面的方法不精确，手动改变edit的值，不能自适应
	//CString   strValue;
	//strValue.Format( _T("%3.1f "),   (double)nPos/10);
	//((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(strValue);
	
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CCardParamDlg::OnBnClickedCheckXm()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_xmChkBox.GetCheck() == BST_CHECKED)
	{
		m_xmPosXSpin.EnableWindow(TRUE);
		m_xmPosYSpin.EnableWindow(TRUE);
		m_xmPosXEdit.EnableWindow(TRUE);
		m_xmPosYEdit.EnableWindow(TRUE);
		m_xmChkStatic.EnableWindow(TRUE);
	}
	else
	{
		m_xmPosXSpin.EnableWindow(FALSE);
		m_xmPosYSpin.EnableWindow(FALSE);
		m_xmPosXEdit.EnableWindow(FALSE);
		m_xmPosYEdit.EnableWindow(FALSE);
		m_xmChkStatic.EnableWindow(FALSE);
	}
}


void CCardParamDlg::OnBnClickedCheckXb()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_xbChkBox.GetCheck() == BST_CHECKED)
	{
		m_xbPosXSpin.EnableWindow(TRUE);
		m_xbPosYSpin.EnableWindow(TRUE);
		m_xbPosXEdit.EnableWindow(TRUE);
		m_xbPosYEdit.EnableWindow(TRUE);
		m_xbChkStatic.EnableWindow(TRUE);
	}
	else
	{
		m_xbPosXSpin.EnableWindow(FALSE);
		m_xbPosYSpin.EnableWindow(FALSE);
		m_xbPosXEdit.EnableWindow(FALSE);
		m_xbPosYEdit.EnableWindow(FALSE);
		m_xbChkStatic.EnableWindow(FALSE);
	}
}


void CCardParamDlg::OnBnClickedCheckKh()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_khChkBox.GetCheck() == BST_CHECKED)
	{
		m_khPosXSpin.EnableWindow(TRUE);
		m_khPosYSpin.EnableWindow(TRUE);
		m_khPosXEdit.EnableWindow(TRUE);
		m_khPosYEdit.EnableWindow(TRUE);
		m_khChkStatic.EnableWindow(TRUE);
	}
	else
	{
		m_khPosXSpin.EnableWindow(FALSE);
		m_khPosYSpin.EnableWindow(FALSE);
		m_khPosXEdit.EnableWindow(FALSE);
		m_khPosYEdit.EnableWindow(FALSE);
		m_khChkStatic.EnableWindow(FALSE);
	}
}


void CCardParamDlg::OnBnClickedCheckBzh()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bzhChkBox.GetCheck() == BST_CHECKED)
	{
		m_bzhPosXSpin.EnableWindow(TRUE);
		m_bzhPosYSpin.EnableWindow(TRUE);
		m_bzhPosXEdit.EnableWindow(TRUE);
		m_bzhPosYEdit.EnableWindow(TRUE);
		m_bzhChkStatic.EnableWindow(TRUE);
	}
	else
	{
		m_bzhPosXSpin.EnableWindow(FALSE);
		m_bzhPosYSpin.EnableWindow(FALSE);
		m_bzhPosXEdit.EnableWindow(FALSE);
		m_bzhPosYEdit.EnableWindow(FALSE);
		m_bzhChkStatic.EnableWindow(FALSE);
	}
}


void CCardParamDlg::OnBnClickedCheckZp()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_zpChkBox.GetCheck() == BST_CHECKED)
	{
		m_zpPosXSpin.EnableWindow(TRUE);
		m_zpPosYSpin.EnableWindow(TRUE);
		m_zpPosXEdit.EnableWindow(TRUE);
		m_zpPosYEdit.EnableWindow(TRUE);
	}
	else
	{
		m_zpPosXSpin.EnableWindow(FALSE);
		m_zpPosYSpin.EnableWindow(FALSE);
		m_zpPosXEdit.EnableWindow(FALSE);
		m_zpPosYEdit.EnableWindow(FALSE);
	}
}


void CCardParamDlg::OnBnClickedCheckFkrq()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_fkrqChkBox.GetCheck() == BST_CHECKED)
	{
		m_fkrqPosXSpin.EnableWindow(TRUE);
		m_fkrqPosYSpin.EnableWindow(TRUE);
		m_fkrqPosXEdit.EnableWindow(TRUE);
		m_fkrqPosYEdit.EnableWindow(TRUE);
		m_fkrqChkStatic.EnableWindow(TRUE);
	}
	else
	{
		m_fkrqPosXSpin.EnableWindow(FALSE);
		m_fkrqPosYSpin.EnableWindow(FALSE);
		m_fkrqPosXEdit.EnableWindow(FALSE);
		m_fkrqPosYEdit.EnableWindow(FALSE);
		m_fkrqChkStatic.EnableWindow(FALSE);
	}
}




void CCardParamDlg::OnBnClickedCheckFkd()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_fkdChkBox.GetCheck() == BST_CHECKED)
	{
		m_fkdPosXSpin.EnableWindow(TRUE);
		m_fkdPosYSpin.EnableWindow(TRUE);
		m_fkdPosXEdit.EnableWindow(TRUE);
		m_fkdPosYEdit.EnableWindow(TRUE);
		m_fkdChkStatic.EnableWindow(TRUE);
	}
	else
	{
		m_fkdPosXSpin.EnableWindow(FALSE);
		m_fkdPosYSpin.EnableWindow(FALSE);
		m_fkdPosXEdit.EnableWindow(FALSE);
		m_fkdPosYEdit.EnableWindow(FALSE);
		m_fkdChkStatic.EnableWindow(FALSE);
	}
}


void CCardParamDlg::OnDeltaposSpinXmX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_xmPosXEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_xmPosXEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinXmY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_xmPosYEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_xmPosYEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinXbX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_xbPosXEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_xbPosXEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinXbY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_xbPosYEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_xbPosYEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinKhX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_khPosXEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_khPosXEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinKhY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_khPosYEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_khPosYEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinBzhX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_bzhPosXEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_bzhPosXEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinBzhY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_bzhPosYEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_bzhPosYEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinZpX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_zpPosXEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_zpPosXEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinZpY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_zpPosYEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_zpPosYEdit.ChangeDecimal(1);	
	}
}


void CCardParamDlg::OnDeltaposSpinFkrqX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_fkrqPosXEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_fkrqPosXEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinFkrqY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_fkrqPosYEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_fkrqPosYEdit.ChangeDecimal(1);	
	}
}


void CCardParamDlg::OnDeltaposSpinFkdX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_fkdPosXEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_fkdPosXEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}


void CCardParamDlg::OnDeltaposSpinFkdY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMUpDown->iDelta > 0)
	{
		m_fkdPosYEdit.ChangeDecimal(-1);	
	}
	else
	{
		m_fkdPosYEdit.ChangeDecimal(1);	
	}

	UpdateData(FALSE);
}
