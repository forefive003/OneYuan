// PrintParamDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PrtStation.h"
#include "PrintParamDlg.h"
#include "afxdialogex.h"

#include "prtGlobal.h"

// CPrintParamDlg 对话框

IMPLEMENT_DYNAMIC(CPrintParamDlg, CDialogEx)

CPrintParamDlg::CPrintParamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPrintParamDlg::IDD, pParent)
{

}

CPrintParamDlg::~CPrintParamDlg()
{
}

void CPrintParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_IP_JIMIJI, m_JimijiIp);
	DDX_Control(pDX, IDC_COMB_SHEBAOSAM, m_listShebaokaSlot);
	DDX_Control(pDX, IDC_CHK_PIN, m_chkUsePin);

	DDX_Control(pDX, IDC_COM_PRINTER_RO, m_listPrtRO);
	DDX_Control(pDX, IDC_COMB_JIMIKA_RO, m_listJimikaRO);
	DDX_Control(pDX, IDC_COMB_CARDRDER, m_listCardReader);
	DDX_Control(pDX, IDC_CHK_IC_DEBUG, m_chkIcDebug);
	DDX_Control(pDX, IDC_COMB_JIAMI_TYPE, m_listJiamiType);
	DDX_Control(pDX, IDC_EDIT_STATION_NUM, m_edtStationNum);
	DDX_Control(pDX, IDC_EDIT_SERIAL_NUM, m_edtSerialNum);

	DDX_Control(pDX, IDC_CHK_MATCH_BANK, m_chkMatchBank);
	DDX_Control(pDX, IDC_CHK_MATCH_ATR, m_chkMatchAtr);
	DDX_Control(pDX, IDC_CHK_FORCE_MIYAO, m_chkForceMiyao);
	DDX_Control(pDX, IDC_COMB_PRT_JIMIKA_TYPE, m_listPrtJiamiType);
	DDX_Control(pDX, IDC_COMB_USERKA_TYPE, m_listUserKaType);
	DDX_Control(pDX, IDC_COMB_PRTTYPE, m_listDoType);
	DDX_Control(pDX, IDC_EDIT_TISHI_THRD, m_edtTishiThrd);
	DDX_Control(pDX, IDC_EDIT_STOP_THRD, m_edtStopThrd);
	DDX_Control(pDX, IDC_COM_DLLTYPE2, m_cmbDllType);
}


BEGIN_MESSAGE_MAP(CPrintParamDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPrintParamDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_PIN, &CPrintParamDlg::OnBnClickedChkPin)
	ON_CBN_SELCHANGE(IDC_COMB_JIAMI_TYPE, &CPrintParamDlg::OnCbnSelchangeCombJiamiType)
	ON_BN_CLICKED(IDCANCEL, &CPrintParamDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPrintParamDlg 消息处理程序


void CPrintParamDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTmp;
	PRT_PRINTER_CFG_T printerParam;
	CEdit*  pEdit = NULL;

	if (g_prtService->isTaskHdling())
	{
		MessageBox(_T("当前有打印任务，不能提交！"), _T("提示"));
		return;
	}

	PRT_PRINTER_CFG_T* oldPrinterCfg = printerCfgGet();
	memcpy(&printerParam, oldPrinterCfg, sizeof(PRT_PRINTER_CFG_T));

	m_JimijiIp.GetWindowText(strTmp);
	strncpy(&printerParam.jimijiAddrDesc[0], strTmp.GetBuffer(0), DEV_IP_MAX_LEN);
	printerParam.jimijiAddr = inet_addr(printerParam.jimijiAddrDesc);
	printerParam.jimijiAddr = ntohl(printerParam.jimijiAddr );

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//获取相应的编辑框ID
	pEdit->GetWindowText(strTmp); //设置默认显示的内容
	printerParam.jimijiPort = _tcstoul(strTmp, NULL, 10);

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//获取相应的编辑框ID
	pEdit->GetWindowText(strTmp); //设置默认显示的内容
	strncpy(&printerParam.jimijiUserName[0], strTmp.GetBuffer(0), JIMIJI_USER_MAX_LEN);

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//获取相应的编辑框ID
	pEdit->GetWindowText(strTmp); //设置默认显示的内容
	strncpy(&printerParam.jimijiPasswd[0], strTmp.GetBuffer(0), JIMIJI_PASSWD_MAX_LEN);

	printerParam.shebaoSamType = m_listJiamiType.GetCurSel() + 1;
	printerParam.shebaoSamSlot = m_listShebaokaSlot.GetCurSel() + 1;

	printerParam.jiamiSamType = m_listPrtJiamiType.GetCurSel() + 1;

	printerParam.userkaType = m_listUserKaType.GetCurSel() + 1;


	if (m_chkUsePin.GetCheck() == BST_CHECKED)
	{
		printerParam.isUsePin = 1;
	}
	else
	{
		printerParam.isUsePin = 0;
	}
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//获取相应的编辑框ID
	pEdit->GetWindowText(strTmp);
	strncpy(&printerParam.pinCode[0], strTmp.GetBuffer(0), PIN_CODE_MAX_LEN);

	/*读卡器*/
	printerParam.cardReaderType = m_listCardReader.GetCurSel() + 1;
	/*执行方式*/
	printerParam.printDoType = m_listDoType.GetCurSel() + 1;

	/*是否模拟写卡*/
	if (m_chkIcDebug.GetCheck() == BST_CHECKED)
	{
		printerParam.isNotWrIc = 1;
	}
	else
	{
		printerParam.isNotWrIc = 0;
	}

	if (m_chkMatchBank.GetCheck() == BST_CHECKED)
	{
		printerParam.isWrMatchBank = 1;
	}
	else
	{
		printerParam.isWrMatchBank = 0;
	}

	if (m_chkMatchAtr.GetCheck() == BST_CHECKED)
	{
		printerParam.isWrMatchAtr = 1;
	}
	else
	{
		printerParam.isWrMatchAtr = 0;
	}

	if (m_chkForceMiyao.GetCheck() == BST_CHECKED)
	{
		printerParam.isWrForceMiyao = 1;
	}
	else
	{
		printerParam.isWrForceMiyao = 0;
	}

	//CString strTmp;
	m_edtTishiThrd.GetWindowText(strTmp);
	printerParam.tishiThrd = atoi(strTmp);
	m_edtStopThrd.GetWindowText(strTmp);
	printerParam.stopThrd = atoi(strTmp);

	/*写配置*/
	if (OK != printerCfgSave(&printerParam))
	{
		return;
	}
	
	/*保存当前类型*/
	int ii = m_cmbDllType.GetCurSel();
	/*重新加载配置*/
	prtSetWrDllType(g_writeIcDlls[ii]);

	MessageBox("设置成功!", "提示", MB_OK);
	CDialogEx::OnOK();
}


BOOL CPrintParamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString str;
	PRT_PRINTER_CFG_T *printerParam = printerCfgGet();

	CEdit*  pEdit = NULL;
	
	for (int i = 0; i<g_dll_cnt; i++)
	{
		m_cmbDllType.InsertString(i, (LPCTSTR)(g_writeIcDlls[i])); //插入一行
		if (strncmp(g_writeIcDllTypes, g_writeIcDlls[i], sizeof(g_writeIcDllTypes)) == 0)
		{
			m_cmbDllType.SetCurSel(i);
		}
	}

	m_listJimikaRO.InsertString(0, _T("卡槽一"));
	m_listJimikaRO.InsertString(1, _T("卡槽二"));
	m_listJimikaRO.InsertString(2, _T("卡槽三"));
	m_listJimikaRO.InsertString(3, _T("卡槽四"));
	if (printerParam->jiamiSamSlot > 0)
	{
		m_listJimikaRO.SetCurSel(printerParam->jiamiSamSlot - 1);
	}
	else
	{
		m_listJimikaRO.SetCurSel(2);
	}
	m_listJimikaRO.EnableWindow(FALSE);

	m_listPrtRO.InsertString(0, (LPCTSTR)(printerParam->printer));
	m_listPrtRO.SetCurSel(0);
	m_listPrtRO.EnableWindow(FALSE);



	m_listShebaokaSlot.InsertString(0, _T("卡槽一"));
	m_listShebaokaSlot.InsertString(1, _T("卡槽二"));
	m_listShebaokaSlot.InsertString(2, _T("卡槽三"));
	m_listShebaokaSlot.InsertString(3, _T("卡槽四"));
	if (printerParam->shebaoSamSlot > 0)
	{
		m_listShebaokaSlot.SetCurSel(printerParam->shebaoSamSlot - 1);
	}
	else
	{
		m_listShebaokaSlot.SetCurSel(2);
	}

	str.Format(_T("%x"), prtGetStationNum());
	m_edtStationNum.SetWindowText(str);

	str.Format(_T("%x"), prtGetCurSerailNum());
	m_edtSerialNum.SetWindowText(str);

	m_JimijiIp.SetWindowText(printerParam->jimijiAddrDesc);
	str.Format(_T("%d"), printerParam->jimijiPort);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//获取相应的编辑框ID
	pEdit->SetWindowText(str); //设置默认显示的内容

	str.Format(_T("%s"), printerParam->jimijiUserName);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//获取相应的编辑框ID
	pEdit->SetWindowText(str); //设置默认显示的内容

	str.Format(_T("%s"), printerParam->jimijiPasswd);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//获取相应的编辑框ID
	pEdit->SetWindowText(str); //设置默认显示的内容

	m_listJiamiType.InsertString(0, _T("PSAM卡方式"));
	m_listJiamiType.InsertString(1, _T("加密机方式"));

	if (printerParam->shebaoSamType == SHEBAO_JIAMI_TYPE_PSAM
			|| printerParam->shebaoSamType == SHEBAO_JIAMI_TYPE_JIAMIJI)
	{
		m_listJiamiType.SetCurSel(printerParam->shebaoSamType - 1);
	}
	else
	{
		m_listJiamiType.SetCurSel(1);
	}

	if (printerParam->shebaoSamType == SHEBAO_JIAMI_TYPE_JIAMIJI)
	{
		m_listShebaokaSlot.EnableWindow(FALSE);
	}
	else
	{
		m_JimijiIp.EnableWindow(FALSE);

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//获取相应的编辑框ID
		pEdit->EnableWindow(FALSE); //设置默认显示的内容

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//获取相应的编辑框ID
		pEdit->EnableWindow(FALSE); //设置默认显示的内容

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//获取相应的编辑框ID
		pEdit->EnableWindow(FALSE); //设置默认显示的内容
	}


	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//获取相应的编辑框ID
	pEdit->SetWindowText(printerParam->pinCode); //设置默认显示的内容

	if (printerParam->isUsePin)
	{
		m_chkUsePin.SetCheck(1);
	}
	else
	{
		m_chkUsePin.SetCheck(0);

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//获取相应的编辑框ID
		pEdit->EnableWindow(FALSE); //设置默认显示的内容
	}

	m_listPrtJiamiType.InsertString(0, _T("版本1(蒋)"));
	m_listPrtJiamiType.InsertString(1, _T("版本2"));

	if (printerParam->jiamiSamType == PRT_JIAMI_TYPE_VER1
				|| printerParam->jiamiSamType == PRT_JIAMI_TYPE_VER2)
	{
		m_listPrtJiamiType.SetCurSel(printerParam->jiamiSamType - 1);
	}
	else
	{
		m_listPrtJiamiType.SetCurSel(1);
	}

	/*用户卡类型*/
	m_listUserKaType.InsertString(0, _T("大唐"));
	m_listUserKaType.InsertString(1, _T("华大"));
	m_listUserKaType.InsertString(2, _T("华大-杰德"));
	m_listUserKaType.InsertString(3, _T("华宏"));

	if (printerParam->userkaType == USERKA_TYPE_DATANG
				|| printerParam->userkaType == USERKA_TYPE_HUADA
				|| printerParam->userkaType == USERKA_TYPE_HUAHONG)
	{
		m_listUserKaType.SetCurSel(printerParam->userkaType - 1);
	}
	else
	{
		m_listUserKaType.SetCurSel(0);
	}

	/*读卡器*/
	m_listCardReader.InsertString(0, _T("HD100"));
	m_listCardReader.InsertString(1, _T("明华读写器"));
	m_listCardReader.InsertString(2, _T("龙杰读写器"));
	m_listCardReader.InsertString(3, _T("ACR读写器"));
	if (printerParam->cardReaderType > 0)
	{
		m_listCardReader.SetCurSel(printerParam->cardReaderType - 1);
	}
	else
	{
		m_listCardReader.SetCurSel(0);
	}

	/*执行方式*/
	/*
	DO_PRT_AND_WR_IC = 1,
	DO_PRT_STEP1 = 2,
	DO_WR_IC_STEP1 = 3,
	*/
	m_listDoType.InsertString(0, _T("自动打卡和写卡"));
	m_listDoType.InsertString(1, _T("自动打卡手动写卡"));
	m_listDoType.InsertString(2, _T("手动写卡自动打卡"));
	if (printerParam->printDoType > 0)
	{
		m_listDoType.SetCurSel(printerParam->printDoType - 1);
	}
	else
	{
		m_listDoType.SetCurSel(0);
	}

	/*是否模拟写卡*/
	if (printerParam->isNotWrIc)
	{
		m_chkIcDebug.SetCheck(1);
	}
	else
	{
		m_chkIcDebug.SetCheck(0);
	}

	if (printerParam->isWrMatchBank)
	{
		m_chkMatchBank.SetCheck(1);
	}
	else
	{
		m_chkMatchBank.SetCheck(0);
	}

	if (printerParam->isWrMatchAtr)
	{
		m_chkMatchAtr.SetCheck(1);
	}
	else
	{
		m_chkMatchAtr.SetCheck(0);
	}

	if (printerParam->isWrForceMiyao)
	{
		m_chkForceMiyao.SetCheck(1);
	}
	else
	{
		m_chkForceMiyao.SetCheck(0);
	}

	CString strTmp;
	strTmp.Format(_T("%d"), printerParam->tishiThrd);
	m_edtTishiThrd.SetWindowText(strTmp);
	strTmp.Format(_T("%d"), printerParam->stopThrd);
	m_edtStopThrd.SetWindowText(strTmp);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CPrintParamDlg::OnBnClickedChkPin()
{
	// TODO: 在此添加控件通知处理程序代码

	CEdit *pEdit = NULL;

	if (m_chkUsePin.GetCheck() == BST_CHECKED)
	{
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//获取相应的编辑框ID
		pEdit->EnableWindow(TRUE); //设置默认显示的内容
	}
	else
	{
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//获取相应的编辑框ID
		pEdit->EnableWindow(FALSE); //设置默认显示的内容
	}
}

void CPrintParamDlg::OnCbnSelchangeCombJiamiType()
{
	// TODO: 在此添加控件通知处理程序代码
	int type =  m_listJiamiType.GetCurSel() + 1;
	CEdit*  pEdit = NULL;

	if (type == SHEBAO_JIAMI_TYPE_JIAMIJI)
	{
		m_JimijiIp.EnableWindow(TRUE);
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//获取相应的编辑框ID
		pEdit->EnableWindow(TRUE); //设置默认显示的内容

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//获取相应的编辑框ID
		pEdit->EnableWindow(TRUE); //设置默认显示的内容

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//获取相应的编辑框ID
		pEdit->EnableWindow(TRUE); //设置默认显示的内容

		m_listShebaokaSlot.EnableWindow(FALSE);
	}
	else
	{
		m_JimijiIp.EnableWindow(FALSE);
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//获取相应的编辑框ID
		pEdit->EnableWindow(FALSE); //设置默认显示的内容

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//获取相应的编辑框ID
		pEdit->EnableWindow(FALSE); //设置默认显示的内容

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//获取相应的编辑框ID
		pEdit->EnableWindow(FALSE); //设置默认显示的内容

		m_listShebaokaSlot.EnableWindow(TRUE);
	}
}




void CPrintParamDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
