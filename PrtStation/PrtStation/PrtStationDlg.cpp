
// PrtStationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PrtStation.h"
#include "PrtStationDlg.h"
#include "afxdialogex.h"


#include "ExcelLoadDlg.h"
#include "prtGlobal.h"

#include "UserInfoDlg.h"

//#include "xpsPrint.h"
#include "printComm.h"
#include "CardParamDlg.h"
#include "PrintParamDlg.h"
#include "PrintInfoDbg.h"
#include "PrinterRegDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
// CPrtStationDlg 对话框




CPrtStationDlg::CPrtStationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPrtStationDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
}

void CPrtStationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUT_REG, m_butReg);
	DDX_Control(pDX, IDC_BUT_PRT_PARAM, m_butPrtParam);
	DDX_Control(pDX, IDC_BUT_CARD_PARAM, m_butCardParam);
	DDX_Control(pDX, IDC_BUT_TEST, m_butTest);
	DDX_Control(pDX, IDC_LIST1, m_listCtrlTask);
	DDX_Control(pDX, IDC_CHECK_AUTO_START, m_chkAutoStart);
	DDX_Control(pDX, IDC_CHECK_AUTO_START2, m_chkStopGetTask);
	DDX_Control(pDX, IDC_BUT_AUTO, m_btnAuto);
	DDX_Control(pDX, IDC_EDIT_SD_CNT, m_edtSpareSD);
	DDX_Control(pDX, IDC_EDIT_CARD_CNT, m_edtPrtCardCnt);
	DDX_Control(pDX, IDC_STATIC_SOUHOU, m_stcSouhou);
	DDX_Control(pDX, IDC_BUT_PROXY, m_btnProxy);
}

BEGIN_MESSAGE_MAP(CPrtStationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_MENU_REG, &CPrtStationDlg::OnMenuReg)
	ON_COMMAND(ID_PRINTTEST, &CPrtStationDlg::OnPrinttest)
	
	ON_COMMAND(ID_MENU_QUIT, &CPrtStationDlg::OnMenuQuit)
	ON_COMMAND(ID_MENU_HIDE, &CPrtStationDlg::OnMenuHide)
	ON_COMMAND(ID_PRTPARAM, &CPrtStationDlg::OnPrtparam)
	ON_COMMAND(ID_32781, &CPrtStationDlg::OnPrintParamClick)
	ON_BN_CLICKED(IDC_BUT_REG, &CPrtStationDlg::OnBnClickedButReg)
	ON_BN_CLICKED(IDC_BUT_PRT_PARAM, &CPrtStationDlg::OnBnClickedButPrtParam)
	ON_BN_CLICKED(IDC_BUT_CARD_PARAM, &CPrtStationDlg::OnBnClickedButCardParam)
	ON_BN_CLICKED(IDC_BUT_TEST, &CPrtStationDlg::OnBnClickedButTest)

	ON_MESSAGE(WM_TRAY_NOTIFY, OnTrayNotify)
	ON_MESSAGE(WM_UPDATE_TASK_LIST, OnUpdateTaskList)
	ON_MESSAGE(WM_UPDATE_STATUS_BAR, OnUpdateStatusBar)
	ON_MESSAGE(WM_POPUP_USER_DLG, OnPopupUserDlg)
	ON_MESSAGE(WM_POPUP_WARNING_DLG, OnPopupWarnDlg)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_AUTO_START, &CPrtStationDlg::OnBnClickedCheckAutoStart)
	ON_BN_CLICKED(IDC_CHECK_AUTO_START2, &CPrtStationDlg::OnBnClickedCheckAutoStart2)
	ON_BN_CLICKED(IDC_BUT_AUTO, &CPrtStationDlg::OnBnClickedButAuto)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CPrtStationDlg::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUT_PROXY, &CPrtStationDlg::OnBnClickedButProxy)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPrtStationDlg 消息处理程序

BOOL CPrtStationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	
	
	
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	/*显示flash 对话框,然后隐藏自己, 启动一个3秒定时器，3秒到后,如果对话框已经初始化完毕,显示主对话框；
	否则等待对话框初始化完成*/
	splashDlg.ShowSplashWindow();
	/*其中： 
    WS_EX_APPWINDOW属性的作用是当一个顶层窗口可见时，强制其出现在任务栏上。将其移出。
　　WS_EX_TOOLWINDOW属性的作用是创建一个象浮动工具栏一样的窗口，该窗口不会出现在任务栏上，并且按 "ALT+TAB"　时也不会出现。将其增加。*/
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	this->m_splashTimeUp = FALSE;
	this->m_initializeOk = FALSE;
	this->m_splashCompleted = FALSE;
	SetTimer(2, 6000, NULL);

	LONG lStyle; 
	lStyle = GetWindowLong(m_listCtrlTask.m_hWnd, GWL_STYLE);//获取当前窗口style 
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位 
	lStyle |= LVS_REPORT; //设置style 
	SetWindowLong(m_listCtrlTask.m_hWnd, GWL_STYLE, lStyle);//设置style 
	DWORD dwStyle = m_listCtrlTask.GetExtendedStyle(); 
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl） 
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl） 
	m_listCtrlTask.SetExtendedStyle(dwStyle); //设置扩展风格 

	CRect rectList;
	m_listCtrlTask.GetClientRect(rectList); //获得当前客户区信息
	m_listCtrlTask.InsertColumn( 0, "时间", LVCFMT_LEFT, rectList.Width()/5);//插入列
	m_listCtrlTask.InsertColumn( 1, "姓名", LVCFMT_LEFT, rectList.Width()/6);
	m_listCtrlTask.InsertColumn( 2, "状态", LVCFMT_LEFT, rectList.Width()/6);
	m_listCtrlTask.InsertColumn( 3, "结果", LVCFMT_LEFT, rectList.Width()/2);


	m_StatusBar.Create(WS_CHILD|WS_VISIBLE|SBT_OWNERDRAW, CRect(0,0,0,0), this, 0);
    CRect rect;
    m_StatusBar.GetClientRect(&rect);
    int a[4]= {rect.left+60, rect.left+160, rect.right-100, rect.right};
    m_StatusBar.SetParts(4,a);

	//设置状态栏文本
	//m_StatusBar.SetText(_T("当前状态"), 0, 0);
	//m_StatusBar.SetText(_T("运行中"), 1, 0);

#if 0
	CMenu m_menu;
	m_menu.LoadMenu(IDR_MENU);
	SetMenu(&m_menu);
#endif

	/*初始化*/
	if( OK != prtGlobalInit())
	{
		MessageBox(_T("初始化失败，请查看日志！"), _T("错误"));
		CDialogEx::OnCancel();
		return FALSE;
	}

	HKEY hKey;
	LONG ret;
	CString strRegPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

	if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		//////////////////////枚举注册表的子键/////////////////////////////
		DWORD dwIndex = 0;                    //子键的索引序号
		TCHAR szSubKey[MAX_PATH] = _T("");    //接收子键名称的缓冲区
		DWORD dwNameLen = MAX_PATH;

		ret = RegEnumValue(hKey, dwIndex, szSubKey, &dwNameLen, NULL, NULL, NULL, NULL);
		while(ERROR_SUCCESS == ret)
		{
			//开始枚举注册表的所有子键信息
			if (strncmp(szSubKey, "PrtStation", MAX_PATH) == 0)
			{
				m_chkAutoStart.SetCheck(1);
				break;
			}
			memset(szSubKey, 0, sizeof(szSubKey));
			dwNameLen = MAX_PATH;
			++dwIndex;

			ret = RegEnumValue(hKey, dwIndex, szSubKey, &dwNameLen, NULL, NULL, NULL, NULL);
		}

		RegCloseKey(hKey);
	}

	
	NOTIFYICONDATA nc; 
	nc.cbSize = sizeof(NOTIFYICONDATA); 
	nc.hIcon = AfxGetApp()->LoadIcon(IDI_ICON2); 
	nc.hWnd = m_hWnd; 
	strcpy(nc.szTip,APP_NAME); 
	nc.uCallbackMessage = WM_TRAY_NOTIFY; 
	nc.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP; 
	nc.uID = IDI_ICON2; 
	Shell_NotifyIcon(NIM_ADD,&nc); 

	/*更新色带和任务个数*/
	char strtmp[16] = {0};
	SNPRINTF(strtmp, 16, "%d", g_prtService->m_printerSdPercent);
	m_edtSpareSD.SetWindowText(strtmp);
	SNPRINTF(strtmp, 16, "%d", g_prtService->m_printerPrtedCnt);
	m_edtPrtCardCnt.SetWindowText(strtmp);

	/*更新售后显示*/
	char sales_info[256] = {0};
	int pos = 0;
	for (int ii = 0; ii < g_sales_cnt; ii++)
	{
		if (g_sales_phone[0] == 0)  break;
		if (pos >= 256) break;

		int sret = SNPRINTF(sales_info + pos, 256-pos, "%s\n", g_sales_phone[ii]);
		if (sret == -1)
		{
			break;
		}

		pos += sret;
	}
	m_stcSouhou.SetWindowText(_T(sales_info));

#if 0
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	wp.flags = WPF_RESTORETOMAXIMIZED;
	wp.showCmd = SW_HIDE;
	SetWindowPlacement(&wp);
#endif

	/*设置初始化完成*/
	this->m_initializeOk = TRUE;
	::SendMessage(this->m_hWnd, WM_NCPAINT, 0, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPrtStationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPrtStationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();

#if 0
		CPaintDC dc(this);
		 CBitmap   bitmap;
		 bitmap.LoadBitmap(IDB_BITMAP1);    //这个IDB_BITMAP1要自己添加
		 CBrush   brush;
		 brush.CreatePatternBrush(&bitmap);
		 CBrush*   pOldBrush   =   dc.SelectObject(&brush);

		 CRect rect;
		 GetClientRect(&rect);
		 dc.Rectangle(0,0,rect.Width(), rect.Height());   // 这些参数可以调整图片添加位置和大小
		 dc.SelectObject(pOldBrush);
#else
		CPaintDC dc(this);
		CRect rect;
		GetClientRect(&rect);

		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap bmpBackground;
		bmpBackground.LoadBitmap(IDB_BITMAP3);
		BITMAP bitmap;

		bmpBackground.GetBitmap(&bitmap);               
		CBitmap   *pbmpOld=dcMem.SelectObject(&bmpBackground);
		dc.StretchBlt(0,0,rect.Width(),100,&dcMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);


#if 0
		CDC dcMem1;
		dcMem1.CreateCompatibleDC(&dc);
		CBitmap bmpBackground1;
		bmpBackground1.LoadBitmap(IDB_BITMAP3);
		BITMAP bitmap1;

		bmpBackground1.GetBitmap(&bitmap1);               
		CBitmap   *pbmpOld1=dcMem1.SelectObject(&bmpBackground1);
		dc.StretchBlt(0,100,rect.Width(),rect.Height()-100,&dcMem1,0,0,bitmap1.bmWidth,bitmap1.bmHeight,SRCCOPY);
#else
		CRect rect1(0,100,rect.Width(),rect.Height());
		CBrush brBk;
		brBk.CreateSolidBrush(RGB(0,128,128));

		dc.FillRect(rect1,&brBk);
#endif

#endif
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPrtStationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPrtStationDlg::OnMenuReg()
{
	// TODO: 在此添加命令处理程序代码
	PrinterRegDlg prterRegDbg;
	prterRegDbg.DoModal();
}


void CPrtStationDlg::OnPrinttest()
{
	// TODO: 在此添加命令处理程序代码
	CPrintInfoDbg prtInfoDbg;
	prtInfoDbg.DoModal();
}


void CPrtStationDlg::OnPrtparam()
{
	// TODO: 在此添加命令处理程序代码
	CCardParamDlg cardParamDlg;
	cardParamDlg.DoModal();
}


void CPrtStationDlg::OnMenuQuit()
{
	// TODO: 在此添加命令处理程序代码
	CDialog::OnCancel();
}


void CPrtStationDlg::OnMenuHide()
{
	// TODO: 在此添加命令处理程序代码
}


void CPrtStationDlg::OnPrintParamClick()
{
	// TODO: 在此添加命令处理程序代码
	CPrintParamDlg prtParamDbg;
	prtParamDbg.DoModal();
}



void CPrtStationDlg::OnBnClickedButReg()
{
	// TODO: 在此添加控件通知处理程序代码
	PrinterRegDlg prterRegDbg;
	prterRegDbg.DoModal();
}


void CPrtStationDlg::OnBnClickedButPrtParam()
{
	// TODO: 在此添加控件通知处理程序代码
	CPrintParamDlg prtParamDbg;
	prtParamDbg.DoModal();
	
}


void CPrtStationDlg::OnBnClickedButCardParam()
{
	// TODO: 在此添加控件通知处理程序代码
	CCardParamDlg cardParamDbg;
	cardParamDbg.DoModal();
}


void CPrtStationDlg::OnBnClickedButTest()
{
	// TODO: 在此添加控件通知处理程序代码
	CPrintInfoDbg prtInfoDbg;
	prtInfoDbg.DoModal();
}


BOOL CPrtStationDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (0)
	{
		NOTIFYICONDATA data;
		memset(&data, 0, sizeof(data));
		data.hWnd = m_hWnd;
		data.uID = IDI_ICON2;
	}
	NOTIFYICONDATA nc;
	nc.cbSize = sizeof(NOTIFYICONDATA);
	nc.hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
	nc.hWnd = m_hWnd;
	strcpy(nc.szTip, APP_NAME);
	nc.uCallbackMessage = WM_TRAY_NOTIFY;
	nc.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nc.uID = IDI_ICON2;


	Shell_NotifyIcon(NIM_DELETE, &nc);

	return CDialogEx::DestroyWindow();
}

LRESULT  CPrtStationDlg::OnTrayNotify(WPARAM wParam,LPARAM lParam) 
{ 
	if (lParam == WM_LBUTTONUP)
	{
		this->ShowWindow(SW_SHOWNORMAL);
		this->SetActiveWindow();
		this->SetForegroundWindow();
		this->BringWindowToTop();
	}
	else if (lParam == WM_RBUTTONUP)
	{
		LPPOINT lpoint = new tagPOINT;
        ::GetCursorPos(lpoint);                    // 得到鼠标位置
        CMenu menu;
        menu.CreatePopupMenu();                    // 声明一个弹出式菜单
        menu.AppendMenu(MF_STRING, ID_APP_EXIT, _T("关闭"));
        menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x ,lpoint->y, this);
        HMENU hmenu = menu.Detach(); 
        menu.DestroyMenu();
        delete lpoint;
	}

	return 0;
}

LRESULT  CPrtStationDlg::OnUpdateStatusBar(WPARAM wParam,LPARAM lParam)
{
	int index = (int)wParam;
	char *desc = (char*)lParam;

	m_StatusBar.SetText(_T(desc),index, 0);
	return 0;
}

LRESULT  CPrtStationDlg::OnUpdateTaskList(WPARAM wParam,LPARAM lParam)
{
	for(int i=m_listCtrlTask.GetItemCount()-1; i>=0;i--)
	{
		m_listCtrlTask.DeleteItem(i);
	}

	MUTEX_LOCK(g_taskHdl->m_prtTasksLock);

	PrintTasksRItr itr = g_taskHdl->m_prtTasks.rbegin();
	PRT_TASK_T *taskInfo = NULL;
	int nRow = 0, nRowRet = 0;

	char desc[TASK_RESULT_DESC_LEN] = { 0 };

	while (itr != g_taskHdl->m_prtTasks.rend())
	{
		taskInfo = *itr;

		/*需要在listCtrl属性中去掉自动排序功能*/
		nRowRet = m_listCtrlTask.InsertItem(nRow, taskInfo->timeBuf);//插入行

		m_listCtrlTask.SetItemText(nRowRet, 1, taskInfo->outPrtInfo.ownerName);//名字
		m_listCtrlTask.SetItemText(nRowRet, 2, g_taskStatusDesc[taskInfo->status]);//状态

		g_taskHdl->getTaskResultDesc(taskInfo, desc);
		m_listCtrlTask.SetItemText(nRowRet, 3, desc);//结果
		
		itr++;
		nRow++;
	}

	MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);

	/*更新色带和任务个数*/
	char strtmp[16] = {0};
	SNPRINTF(strtmp, 16, "%d", g_prtService->m_printerSdPercent);
	m_edtSpareSD.SetWindowText(strtmp);
	SNPRINTF(strtmp, 16, "%d", g_prtService->m_printerPrtedCnt);
	m_edtPrtCardCnt.SetWindowText(strtmp);

	UpdateData(0);
	return 0;
}


LRESULT  CPrtStationDlg::OnPopupWarnDlg(WPARAM wParam, LPARAM lParam)
{
	int warn_type = (int)wParam;
	if (WARN_PRINT_FAILED == warn_type)
	{
		MessageBox(_T("任务打印失败！"), _T("错误"), MB_OK);
	}
	else if (WARN_WRITE_FAILED == warn_type)
	{
		MessageBox(_T("任务写卡失败！"), _T("错误"), MB_OK);
	}
	else if (WARN_PARSE_FAILED == warn_type)
	{
		MessageBox(_T("任务数据解析失败！"), _T("错误"), MB_OK);
	}
	else if (WARN_NEED_CLEAR_PRINTER == warn_type)
	{
		MessageBox(_T("打印张数接近极限，请及时用清洁卡清洁！"), _T("提示"), MB_OK);
	}
	else if (WARN_NEED_CLEAR_PRINTER_END == warn_type)
	{
		MessageBox(_T("目前已经是制卡极限，请清洁以后再制卡！"), _T("提示"), MB_OK);
	}
	
	return 0;
}

LRESULT  CPrtStationDlg::OnPopupUserDlg(WPARAM wParam, LPARAM lParam)
{
	PRT_TASK_T *taskInfo = (PRT_TASK_T*)lParam;

	UserInfoDlg *userDlg = new UserInfoDlg();
	userDlg->setTaskInfo(taskInfo->timeBuf, taskInfo->outPrtInfo.ownerName);
	if (TRUE == userDlg->Create(IDD_USERINFO_DLG))
	{
		userDlg->ShowWindow(TRUE);
		userDlg->CenterWindow();
	}
	else
	{
		userDlg->DestroyWindow();
	}
	return 0;
}


void CPrtStationDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(nType == SIZE_MINIMIZED)  
    {  
        ShowWindow(SW_HIDE);// 当最小化市，隐藏主窗口
    }
}


void CPrtStationDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
}


void CPrtStationDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (MessageBox("确实要退出吗？", "提示", MB_OKCANCEL|MB_ICONWARNING) == IDOK)
	{
		CDialog::OnClose();
	}
}


void CPrtStationDlg::OnBnClickedCheckAutoStart()
{
	// TODO: 在此添加控件通知处理程序代码
	HKEY hKey;
	CString strRegPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//找到系统的启动项
	if (m_chkAutoStart.GetCheck())
	{
		if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) //打开启动项
		{
			TCHAR szModule[MAX_PATH];
			GetModuleFileName(NULL, szModule, MAX_PATH);//得到本程序自身的全路径
			RegSetValueEx(hKey, _T("PrtStation"), 0, REG_SZ, (LPBYTE)szModule, (lstrlen(szModule) + 1)*sizeof(TCHAR));
			RegCloseKey(hKey); //关闭注册表

			AfxMessageBox(_T("设置开机自启动成功!"));
		}
		else
		{
			AfxMessageBox(_T("系统参数错误,不能随系统启动!"));
		}
	}
	else
	{
		if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			RegDeleteValue(hKey, _T("PrtStation"));
			RegCloseKey(hKey);

			AfxMessageBox(_T("取消开机自启动成功!"));
		}
		else
		{
			AfxMessageBox(_T("系统参数错误,取消开机自启动失败!"));
		}
	}
}


void CPrtStationDlg::OnBnClickedCheckAutoStart2()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_chkStopGetTask.GetCheck())
	{
		g_prtService->m_stopGetTask = TRUE;

		PRT_LOG_INFO("停止获取打印任务.");
	}
	else
	{
		g_prtService->m_stopGetTask = FALSE;
		PRT_LOG_INFO("启动获取打印任务.");
	}
}


void CPrtStationDlg::OnBnClickedButAuto()
{
	// TODO: 在此添加控件通知处理程序代码
	CExcelLoadDlg  autoDlg;
	autoDlg.DoModal();
}


void CPrtStationDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	/*销毁资源*/
	prtGlobalFree();
}


HBRUSH CPrtStationDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	//CFont * cfont = new CFont;
	//cfont->CreateFont(16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, 0,
	//	ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH&FF_SWISS, "宋体");
		
	int idTmp = pWnd->GetDlgCtrlID();
	switch (idTmp)
	{
		case IDC_STATIC_1: //静态文本控件
		case IDC_STATIC:		
		{
			pDC->SetBkMode(TRANSPARENT);
			//pDC->SetBkColor(RGB(0, 128, 128));
			//pDC->SetTextColor(RGB(255,255,0)); //设置字体颜色
			//pWnd->SetFont(cfont); //设置字体
			//HBRUSH B = CreateSolidBrush(RGB(125, 125, 255)); //创建画刷
			//return (HBRUSH)B;
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		case IDC_STATIC_SOUHOU:
		case IDC_STATIC_BANBEN:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(RGB(0, 128, 128));
			pDC->SetTextColor(RGB(255,255,0)); //设置字体颜色
			//pWnd->SetFont(cfont); //设置字体
			//HBRUSH B = CreateSolidBrush(RGB(125, 125, 255)); //创建画刷
			//return (HBRUSH)B;
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		case IDC_CHECK_AUTO_START2: 
		case IDC_CHECK_AUTO_START:
		{
			pDC->SetBkMode(TRANSPARENT);
			//pDC->SetBkColor(RGB(0, 128, 128));
			// pDC->SetTextColor(RGB(255,255,0));
			//pWnd->SetFont(cfont);
			//HBRUSH B = CreateSolidBrush(RGB(125, 125, 255));
			//return (HBRUSH)B;
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
		case IDC_STATIC_GP:
		case IDC_STATIC_GP1:
		case IDC_STATIC_GP2:
		{
			//pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(RGB(0, 128, 128));
			pDC->SetTextColor(RGB(255,255,255));
			//HBRUSH B = CreateSolidBrush(RGB(125, 125, 255));
			//return (HBRUSH)B;
			return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
	}

	return hbr;
}


void CPrtStationDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	int col = 0;
	int row = 0;
	row = pNMItemActivate->iItem;
	if (row == -1)
	{
		/*没有数据*/
		return;
	}

	CString strTime = _T("");
	CString strName = _T("");
	strTime = m_listCtrlTask.GetItemText(row, 0);
	strName = m_listCtrlTask.GetItemText(row, 1);
	if (strTime.IsEmpty() || strName.IsEmpty())
	{
		MessageBox("所选行数据异常", "提示", MB_OK | MB_ICONWARNING);
		return;
	}

	UserInfoDlg *userDlg = new UserInfoDlg();
	userDlg->setTaskInfo(strTime.GetBuffer(), strName.GetBuffer());
	if (TRUE == userDlg->Create(IDD_USERINFO_DLG))
	{
		userDlg->ShowWindow(TRUE);
	}
	else
	{
		userDlg->DestroyWindow();
	}

	*pResult = 0;
}



void CPrtStationDlg::OnBnClickedButProxy()
{
	// TODO: Add your control notification handler code here
	HWND hWnd;
#if 0
	hWnd = ::FindWindow(NULL, _T("GoProxy"));
#else
	hWnd = get_proc_hwnd(_T("GoProxy.exe"));
#endif
	if (NULL != hWnd)
	{
#define WM_TOP_SHOW (WM_USER + 8121)
		::SendMessage(hWnd, WM_TOP_SHOW, (WPARAM)0, NULL);
		return;
	}

	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo; //This is an [in] parameter
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo; //Only compulsory field
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_SHOWNORMAL;

	char exefile[256] = { 0 };
	SNPRINTF(exefile, 256, "%s代理工具\\GoProxy.exe", g_localModDir);
	if (CreateProcess(exefile, NULL,
		NULL, NULL, FALSE, 0, NULL,
		NULL, &StartupInfo, &ProcessInfo))
	{
		//WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}
	else
	{
		MessageBox("The process could not be started...");
	}
}


void CPrtStationDlg::StartProxyDlg()
{
	/*弹出代理程序*/
	if (g_is_use_proxy)
	{
		HWND hWnd;
#if 0
		hWnd = ::FindWindow(NULL, _T("GoProxy"));
#else
		hWnd = get_proc_hwnd(_T("GoProxy.exe"));
#endif
		if (NULL == hWnd)
		{
			PROCESS_INFORMATION ProcessInfo;
			STARTUPINFO StartupInfo; //This is an [in] parameter
			ZeroMemory(&StartupInfo, sizeof(StartupInfo));
			StartupInfo.cb = sizeof StartupInfo; //Only compulsory field
			StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
			StartupInfo.wShowWindow = SW_MINIMIZE;

			/*最小化显示*/
			char exefile[256] = { 0 };
			SNPRINTF(exefile, 256, "%s代理工具\\GoProxy.exe", g_localModDir);
			if (CreateProcess(exefile, NULL,
				NULL, NULL, FALSE, 0, NULL,
				NULL, &StartupInfo, &ProcessInfo))
			{
				//WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
				CloseHandle(ProcessInfo.hThread);
				CloseHandle(ProcessInfo.hProcess);
			}
			else
			{
				MessageBox("The proxy process could not be started...");
			}
		}

#if 0
		hWnd = ::FindWindow(NULL, _T("GoProxy"));
#else
		hWnd = get_proc_hwnd(_T("GoProxy.exe"));
#endif
		if (NULL != hWnd)
		{
#define WM_TOP_SHOW (WM_USER + 8121)
			::SendMessage(hWnd, WM_TOP_SHOW, (WPARAM)1, NULL);
		}
	}
}

LRESULT CPrtStationDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_NCPAINT)
	{
		if (!this->m_splashCompleted)
		{
			if (this->m_splashTimeUp && this->m_initializeOk)
			{
				this->m_splashCompleted = TRUE;

				splashDlg.HideSplashWindow();				
				this->StartProxyDlg();
				ShowWindow(SW_SHOW);
			}
			else
			{
				ShowWindow(SW_HIDE);
			}
		}			
	}

	return CDialogEx::DefWindowProc(message, wParam, lParam);
}


void CPrtStationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	this->m_splashTimeUp = TRUE;
	::SendMessage(this->m_hWnd, WM_NCPAINT, 0, NULL);
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);

	KillTimer(2);
	CDialogEx::OnTimer(nIDEvent);
}
