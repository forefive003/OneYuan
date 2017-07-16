// PrinterRegDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "afxdialogex.h"

#include "etpLib.h"
#include "prtGlobal.h"
#include "printComm.h"

#include "PrtStation.h"
#include "PrinterRegDlg.h"

// PrinterRegDlg 对话框

IMPLEMENT_DYNAMIC(PrinterRegDlg, CDialogEx)

PrinterRegDlg::PrinterRegDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(PrinterRegDlg::IDD, pParent)
{

}

PrinterRegDlg::~PrinterRegDlg()
{
}

void PrinterRegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COM_PRINTER, m_listPrt);
	DDX_Control(pDX, IDC_COMB_JIMIKA, m_listJimika);
	DDX_Control(pDX, IDC_IP_SERVER, m_ServerIp);

	DDX_Control(pDX, IDC_COM_STATION, m_combStations);
	DDX_Control(pDX, IDC_COM_LOCALIP, m_combLocalIpaddr);
}


BEGIN_MESSAGE_MAP(PrinterRegDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &PrinterRegDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &PrinterRegDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMB_JIMIKA, &PrinterRegDlg::OnCbnSelchangeCombJimika)
	ON_CBN_SELCHANGE(IDC_COM_STATION, &PrinterRegDlg::OnCbnSelchangeComStation)
END_MESSAGE_MAP()


void PrinterRegDlg::init_show(PRT_REG_CFG_T *regInfo, PRT_PRINTER_CFG_T *printerCfg)
{
	int count = 0;
	PRT_DEV_T *prtDev = NULL;
	prtDev = getWinPrinters(&count);
	if (NULL != prtDev)
	{
		m_listPrt.ResetContent();
		for(int i=0; i<count; i++)
		{
			m_listPrt.InsertString(i, (LPCTSTR)(prtDev[i].printerName)); //插入一行
		}
		m_listPrt.SetCurSel(0);
		
		for(int i=0; i<count; i++)
		{
			if (0 == strncmp(printerCfg->printer, (LPCTSTR)(prtDev[i].printerName), PRINTER_MAX_LEN))
			{
				m_listPrt.SetCurSel(i);
			}
		}

		free(prtDev);
	}
	else
	{
		MessageBox(_T("没有连接打印机！"), _T("提示"));
	}

	m_listJimika.ResetContent();
	m_listJimika.InsertString(0, _T("卡槽一"));
	m_listJimika.InsertString(1, _T("卡槽二"));
	m_listJimika.InsertString(2, _T("卡槽三"));
	m_listJimika.InsertString(3, _T("卡槽四"));
	if (printerCfg->jiamiSamSlot > 0)
	{
		m_listJimika.SetCurSel(printerCfg->jiamiSamSlot - 1);
	}
	else
	{
		m_listJimika.SetCurSel(2);
	}

	char jamiyinzi[PRINTER_JIMIKA_INFO_LEN + 1] = {0};
	char errData[RESULT_FAIL_STR_LEN + 1] = {0};

	CEdit*  pEdit = NULL;
	int ret = 0;
#if 1
	ret = XpsGetJamiYinzi(printerCfg->cardReaderType, printerCfg->jiamiSamSlot, jamiyinzi, errData);
	if (OK != ret)
	{
		MessageBox(_T(errData), _T("获取加密因子失败"));
	}
	else
	{
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//获取相应的编辑框ID
		pEdit->SetWindowText(jamiyinzi); //设置显示的内容
	}
#else
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//获取相应的编辑框ID
	pEdit->SetWindowText(regInfo->jimikaInfo); //设置默认显示的内容
#endif

	CString str;

	m_ServerIp.SetWindowText(regInfo->serverAddrDesc);
	str.Format(_T("%d"), regInfo->serverPort);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_SERVER);//获取相应的编辑框ID
	pEdit->SetWindowText(str); //设置默认显示的内容

	bool found = false;
	m_combLocalIpaddr.ResetContent();
	for (int ii = 0; ii < g_localip_cnt; ii++)
	{
		m_combLocalIpaddr.InsertString(ii, g_localip_desc[ii]);
		if (strncmp(regInfo->localAddrDesc, g_localip_desc[ii], 255) == 0)
		{
			m_combLocalIpaddr.SetCurSel(ii);
			found = true;
		}
	}
	if (found == false)
	{
		m_combLocalIpaddr.SetCurSel(0);
	}

	str.Format(_T("%d"), regInfo->localPort);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_LOCAL);//获取相应的编辑框ID
	pEdit->SetWindowText(str); //设置默认显示的内容
}

// PrinterRegDlg 消息处理程序
BOOL PrinterRegDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	char *stationType = prtGetStationType();
	for(int i=0; i<g_station_cnt; i++)
	{
		m_combStations.InsertString(i, (LPCTSTR)(g_stations[i])); //插入一行
		if (strncmp(stationType, g_stations[i], MAX_STATION_NAME_LEN) == 0)
		{
			m_combStations.SetCurSel(i);
		}
	}
	

	PRT_REG_CFG_T *regInfo = registerCfgGet();
	PRT_PRINTER_CFG_T *printerCfg = printerCfgGet();

	this->init_show(regInfo, printerCfg);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void PrinterRegDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_prtService->isRegistering())
	{
		MessageBox(_T("当前正在注册中，不能重复提交！"), _T("提示"));
		return;
	}

	if (g_prtService->isTaskHdling())
	{
		MessageBox(_T("当前有打印任务，不能提交！"), _T("提示"));
		return;
	}

	/*保存当前类型*/
	int ii = m_combStations.GetCurSel();
	/*重新加载配置*/
	if (sysConfigReload(g_stations[ii]) != OK)
	{
		MessageBox(_T("重新加载配置失败,请检查所选站点的配置！"), _T("错误"));
		return;
	}

	PRT_REG_CFG_T *oldRegisterCfg = registerCfgGet();
	PRT_PRINTER_CFG_T *oldPrinterCfg = printerCfgGet();

	PRT_REG_CFG_T regInfo;
	memcpy(&regInfo, oldRegisterCfg, sizeof(PRT_REG_CFG_T));
	PRT_PRINTER_CFG_T printerCfg;
	memcpy(&printerCfg, oldPrinterCfg, sizeof(PRT_PRINTER_CFG_T));
	char jiamiyinzi[PRINTER_JIMIKA_INFO_LEN + 1] = {0};

	CString strTmp;   
	int i = m_listPrt.GetCurSel();
	m_listPrt.GetLBText(i, strTmp);
	strncpy(&printerCfg.printer[0], strTmp.GetBuffer(0), PRINTER_MAX_LEN);

	if (printerCfg.printer[0] == 0)
	{
		MessageBox(_T("打印机为空，不能注册！"), _T("提示"));
		return;
	}

	printerCfg.jiamiSamSlot = m_listJimika.GetCurSel() + 1;

	CEdit*  pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//获取相应的编辑框ID
	pEdit->GetWindowText(strTmp); //设置默认显示的内容
	strncpy(&jiamiyinzi[0], strTmp.GetBuffer(0), PRINTER_JIMIKA_INFO_LEN);

	m_ServerIp.GetWindowText(strTmp);
	strncpy(&regInfo.serverAddrDesc[0], strTmp.GetBuffer(0), DEV_IP_MAX_LEN);
	regInfo.serverAddr = inet_addr(regInfo.serverAddrDesc);
	regInfo.serverAddr = ntohl(regInfo.serverAddr );

	m_combLocalIpaddr.GetWindowText(strTmp);
	strncpy(&regInfo.localAddrDesc[0], strTmp.GetBuffer(0), DEV_IP_MAX_LEN);
	regInfo.localAddr = inet_addr(regInfo.localAddrDesc);
	regInfo.localAddr = ntohl(regInfo.localAddr );


	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_SERVER);//获取相应的编辑框ID
	pEdit->GetWindowText(strTmp); //设置默认显示的内容
	regInfo.serverPort = _tcstoul(strTmp, NULL, 10);

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_LOCAL);//获取相应的编辑框ID
	pEdit->GetWindowText(strTmp); //设置默认显示的内容
	regInfo.localPort = _tcstoul(strTmp, NULL, 10);

	/*先写配置*/
	registerCfgSave(&regInfo);
	printerCfgSave(&printerCfg);
	saveJiamiyinzi(jiamiyinzi);

	/*再注册*/	
	if (jiamiyinzi[0] == 0)
	{
		MessageBox(_T("加密因子为空,不能注册,请确认打印机是否已连接！"), _T("提示"));
	}
	else
	{
		g_prtService->registerTaskHdl();
	}
	CDialogEx::OnOK();
}


void PrinterRegDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}



void PrinterRegDlg::OnCbnSelchangeCombJimika()
{
	// TODO: 在此添加控件通知处理程序代码
	PRT_PRINTER_CFG_T *printerCfg = printerCfgGet();

	int iReadType = printerCfg->cardReaderType;
	int slot = m_listJimika.GetCurSel() + 1;	
	char jamiyinzi[PRINTER_JIMIKA_INFO_LEN + 1] = {0};
	char errData[RESULT_FAIL_STR_LEN + 1] = {0};

	CEdit*  pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//获取相应的编辑框ID
	pEdit->SetWindowText(jamiyinzi); //设置默认显示的内容

	UpdateData(0);

	int ret = 0;

	ret = XpsGetJamiYinzi(iReadType, slot, jamiyinzi, errData);
	if (OK != ret)
	{
		MessageBox(_T(errData), _T("获取加密因子失败"));
	}
	else
	{
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//获取相应的编辑框ID
		pEdit->SetWindowText(jamiyinzi); //设置默认显示的内容
	}
}


void PrinterRegDlg::OnCbnSelchangeComStation()
{
	// TODO: Add your control notification handler code here
	int ii = m_combStations.GetCurSel();

	PRT_PRINTER_CFG_T printerCfg;
	PRT_REG_CFG_T registerCfg;

	memset(&printerCfg, 0, sizeof(PRT_PRINTER_CFG_T));
	memset(&registerCfg, 0, sizeof(PRT_REG_CFG_T));

	if (OK != printerCfgRead(g_stations[ii], &printerCfg))
	{
		goto backSel;
	}
	if (OK != registerCfgRead(g_stations[ii], &registerCfg))
	{
		goto backSel;
	}

	this->init_show(&registerCfg, &printerCfg);
	UpdateData(0);
	return;

backSel:
	char *stationType = prtGetStationType();
	for(int i=0; i<g_station_cnt; i++)
	{
		if (strncmp(stationType, g_stations[i], MAX_STATION_NAME_LEN) == 0)
		{
			m_combStations.SetCurSel(i);
			break;
		}
	}
}
