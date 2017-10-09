// PrintInfoDbg.cpp : 实现文件
//

#include "stdafx.h"
#include "PrtStation.h"
#include "PrintInfoDbg.h"
#include "afxdialogex.h"

#include "etpLib.h"
#include "prtGlobal.h"
#include "serverComm.h"
#include "printComm.h"
#include "photoConvert.h"

// CPrintInfoDbg 对话框

IMPLEMENT_DYNAMIC(CPrintInfoDbg, CDialogEx)

CPrintInfoDbg::CPrintInfoDbg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPrintInfoDbg::IDD, pParent)
{

}

CPrintInfoDbg::~CPrintInfoDbg()
{
}

void CPrintInfoDbg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SEX, m_cmb_sex);
}


BEGIN_MESSAGE_MAP(CPrintInfoDbg, CDialogEx)
	ON_BN_CLICKED(ID_PRINT_OK, &CPrintInfoDbg::OnBnClickedPrintOk)
	ON_BN_CLICKED(ID_PRINTCANCEL, &CPrintInfoDbg::OnBnClickedPrintcancel)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_PHOTO_SEL, &CPrintInfoDbg::OnBnClickedBtnPhotoSel)
	ON_BN_CLICKED(IDC_BUT_PRT_IC_CHK, &CPrintInfoDbg::OnBnClickedButPrtIcChk)
	ON_BN_CLICKED(IDC_BUT_PRT_RESET, &CPrintInfoDbg::OnBnClickedButPrtReset)
	ON_BN_CLICKED(IDC_BUT_PRT_CLEAR, &CPrintInfoDbg::OnBnClickedButPrtClear)
END_MESSAGE_MAP()


// CPrintInfoDbg 消息处理程序

int CPrintInfoDbg::getTestPrintInfo(PRT_INFO_T *cardInfo)
{
	CEdit*  pEdit = NULL;
	CString name;
	CString cardID;
	CString cardNum;
	CString cardLaunchTime;
	CString cardLaunchPlace;
	CString sex;

	memset(cardInfo, 0, sizeof(PRT_INFO_T));

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_NAME);
	pEdit->GetWindowText(name);
	if (name.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("姓名长度太长！！！"), _T("非法参数"));
		return ET_ERROR;
	}

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_ID);
	pEdit->GetWindowText(cardID);
	if (cardID.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("社保卡ID长度太长！！！"), _T("非法参数"));
		return ET_ERROR;
	}

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_NUM);
	pEdit->GetWindowText(cardNum);
	if (cardNum.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("社保号码长度太长！！！"), _T("非法参数"));
		return ET_ERROR;
	}
	
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_LAUNCHPLACE);
	pEdit->GetWindowText(cardLaunchTime);
	if (cardLaunchTime.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("发卡日期长度太长！！！"), _T("非法参数"));
		return ET_ERROR;
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LAUNCHPLACE2);
	pEdit->GetWindowText(cardLaunchPlace);
	if (cardLaunchPlace.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("发卡地长度太长！！！"), _T("非法参数"));
		return ET_ERROR;
	}

	m_cmb_sex.GetWindowTextA(sex);
	
	CFile ImageFile;
	char * szImageBin = NULL;
	if (0 == ImageFile.Open(g_testPhotoDir, CFile::modeRead))
	{
		free(cardInfo);
		MessageBox(_T("打开测试文件失败！！！"), _T("错误"));
		return ET_ERROR;
	}

	unsigned int iSrcLen = (int)ImageFile.GetLength();

	szImageBin = (char*)malloc(OWNER_PHOTO_MAX_LEN);
	if (NULL == szImageBin)
	{
		free(cardInfo);
		ImageFile.Close();
		MessageBox(_T("内存不足！！！"), _T("错误"));
		return ET_ERROR;
	}
	ImageFile.Read((void*)szImageBin,iSrcLen);
	ImageFile.Close();


	PHOTO_TYPE_E photoType = g_photoConvert.getPhotoType(szImageBin, iSrcLen);
	if (PHOTO_INVALID == photoType)
	{
		free(cardInfo);
		free(szImageBin);
		MessageBox(_T("photo type invalid！！！"), _T("错误"));
		return ET_ERROR;
	}
	else if (PHOTO_JPG != photoType)
	{
		if(OK != g_photoConvert.convertToJpg(szImageBin, iSrcLen,
			szImageBin, OWNER_PHOTO_MAX_LEN, &iSrcLen))
		{
			free(cardInfo);
			free(szImageBin);
			MessageBox(_T("photo convert failed！！！"), _T("错误"));
			return ET_ERROR;
		}
	}

	unsigned int baseLen = 0;
	char* imgBase64 = base64encode(szImageBin, iSrcLen, &baseLen);
	if (NULL == imgBase64)
	{
		free(cardInfo);
		free(szImageBin);
		MessageBox(_T("base64 加密失败！！！"), _T("错误"));
		return ET_ERROR;
	}
	free(szImageBin);

	if (baseLen > OWNER_PHOTO_MAX_LEN)
	{
		free(imgBase64);

		CString strTmp;
		strTmp.Format("照片太大，超过 %lu KB！！！", OWNER_PHOTO_MAX_LEN/1024);
		MessageBox(_T(strTmp), _T("错误"));
		return ET_ERROR;
	}

	memcpy(cardInfo->ownerPhoto, imgBase64, baseLen);
	cardInfo->ownerPhotoLen = baseLen;

	free(imgBase64);


	int fmtCnt = 0;
	PRT_DATA_FMT_T *dataFmt = NULL;
	dataFmt = icDataFmtInfoGet(&fmtCnt);

	PRT_TEMPLATE_T *prtFormat = NULL;
	prtFormat = prtTemplateGet();

	int i = 0;
	for (i = 0; i<fmtCnt; i++)
	{
		if (dataFmt[i].isPrint)
		{
			if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerNameAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerNameAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->ownerName, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName, name);
				}
				else
				{
					strncpy(cardInfo->ownerName, name, OWNER_TEXT_MAX_LEN);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerIdentityNumAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerIdentityNumAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->ownerIdentityNum, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName, cardID);
				}
				else
				{
					strncpy(cardInfo->ownerIdentityNum, cardID, OWNER_TEXT_MAX_LEN);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerCardNumAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerCardNumAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->ownerCardNum, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName, cardNum);
				}
				else
				{
					strncpy(cardInfo->ownerCardNum, cardNum, OWNER_TEXT_MAX_LEN);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerSexAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerSexAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->ownerSex, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName,
															sex);
				}
				else
				{
					SNPRINTF(cardInfo->ownerSex, OWNER_TEXT_MAX_LEN,
															"%s",
															sex);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->cardLaunchTimeAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->cardLaunchTimeAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->cardLaunchTime, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName, cardLaunchTime);
				}
				else
				{
					SNPRINTF(cardInfo->cardLaunchTime, OWNER_TEXT_MAX_LEN,
						"%s", cardLaunchTime);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->cardLaunchPlaceAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->cardLaunchPlaceAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->cardLaunchPlace, OWNER_TEXT_MAX_LEN,
						"%s %s",
						dataFmt[i].printName, cardLaunchPlace);
				}
				else
				{
					SNPRINTF(cardInfo->cardLaunchPlace, OWNER_TEXT_MAX_LEN,
						"%s", cardLaunchPlace);
				}
			}
		}
	}
	return OK;
}

void CPrintInfoDbg::OnBnClickedPrintOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_taskHdl->isTaskingFull())
	{
		MessageBox(_T("当前打印队列满，不能提交！"), _T("提示"));
		return;
	}

	PRT_TASK_T *taskInfo = (PRT_TASK_T*)malloc(sizeof(PRT_TASK_T));
	if (NULL == taskInfo)
	{
		MessageBox(_T("内存不够！！！"), _T("错误"));
		return;
	}

	memset(taskInfo, 0, sizeof(PRT_TASK_T));

	time_t nowTime = time(NULL); //获取目前秒时间
	tm* local; //本地时间
	local = localtime(&nowTime); //转为本地时间
	strftime(taskInfo->timeBuf, sizeof(taskInfo->timeBuf) - 1, "%Y-%m-%d %H:%M:%S", local);

	CButton *pBut = NULL;
	pBut = (CButton*)GetDlgItem(IDC_CHECK_PRT);
	if (pBut->GetCheck() == BST_CHECKED)
	{
		taskInfo->flag |= FLAG_PRINT;
	}

	pBut = (CButton*)GetDlgItem(IDC_CHECK_WR);
	if (pBut->GetCheck() == BST_CHECKED)
	{
		taskInfo->flag |= FLAG_WRITE_IC;

		extern int gzzTaskDataBuild(char *outIcStr);
		if (OK != gzzTaskDataBuild(taskInfo->outIcStr))
		{
			free(taskInfo);
			MessageBox(_T("构造IC数据失败！！！"), _T("错误"));
			return;
		}
	}

	if(OK != this->getTestPrintInfo(&taskInfo->outPrtInfo))
	{
		free(taskInfo);
		return;
	}

	/*进入初始状态*/
	taskInfo->isTest = TRUE;
	g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_INIT);
	if(OK != g_prtService->printTaskHdl(taskInfo))
	{
		/*设置Init的时候已经加入队列,不能自己释放*/
		//free(taskInfo);
	}

	CDialog::OnOK();
}


void CPrintInfoDbg::OnBnClickedPrintcancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}



BOOL CPrintInfoDbg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_cmb_sex.InsertString(0, _T("男"));
	m_cmb_sex.InsertString(1, _T("女"));
	m_cmb_sex.SetCurSel(0);

	CEdit*  pEdit=(CEdit*)GetDlgItem(IDC_EDIT_NAME);//获取相应的编辑框ID
	pEdit->SetWindowText(_T("张三")); //设置默认显示的内容

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_ID);//获取相应的编辑框ID
	pEdit->SetWindowText(_T("123456789012345678")); //设置默认显示的内容

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_NUM);//获取相应的编辑框ID
	pEdit->SetWindowText(_T("123456789")); //设置默认显示的内容

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LAUNCHPLACE);//获取相应的编辑框ID
	pEdit->SetWindowText(_T("2017年7月")); //设置默认显示的内容

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LAUNCHPLACE2);//获取相应的编辑框ID
	pEdit->SetWindowText(_T("四川成都")); //设置默认显示的内容

	//根据路径载入图片
	HRESULT hResult = m_image.Load(_T(g_testPhotoDir));
	if (FAILED(hResult)) 
	{
		MessageBox(_T("加载测试图片失败！"), _T("加载失败"));
		return FALSE;
	}

	CButton *pBut = NULL;
	pBut = (CButton*)GetDlgItem(IDC_CHECK_PRT);
	pBut->SetCheck(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CPrintInfoDbg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码

	if (m_image.IsNull()) 
	{
		return;
	}

	
	//int cx, cy;
	//获取图片的宽 高度
	//cx	= m_image.GetWidth();
	//cy	= m_image.GetHeight();

	CWnd *pWnd = NULL;
	CRect	rect;
	pWnd = GetDlgItem(IDC_STATIC_PHOTO);//获取控件句柄

	//获取Picture Control控件的大小
	//pWnd->GetWindowRect(&rect);
	//将客户区选中到控件表示的矩形区域内
	//ScreenToClient(&rect);

	//窗口移动到控件表示的区域
	//pWnd->MoveWindow(rect.left, rect.top, cx, cy, TRUE);

	pWnd->GetClientRect(&rect);

	CDC *pDc = NULL;
	pDc	= pWnd->GetDC();//获取picture的DC
	m_image.Draw(pDc->m_hDC, rect);//将图片绘制到picture表示的区域内
	ReleaseDC(pDc);
	// 不为绘图消息调用 CDialogEx::OnPaint()
}


void CPrintInfoDbg::OnBnClickedBtnPhotoSel()
{
	// TODO: 在此添加控件通知处理程序代码
	 // 设置过滤器   
	TCHAR szFilter[] = _T("照片文件(*.jpg;*.png;*.gif;*.bmp;*.pcx;*.tif;*.psd)|*.jpg;*.png;*.gif;*.bmp;*.pcx;*.tif;*.psd");
    // 构造打开文件对话框   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // 显示打开文件对话框   
    if (IDOK == fileDlg.DoModal())   
    {   
        // 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
        strFilePath = fileDlg.GetPathName(); 

		strncpy(g_testPhotoDir, _T(strFilePath), MAX_PATH);

		//根据路径载入图片
		HRESULT hResult = m_image.Load(_T(g_testPhotoDir));
		if (FAILED(hResult)) 
		{
			MessageBox(_T("加载照片失败！"), _T("加载失败"));
			return;
		}

		/*触发wm_paint消息*/
		this->Invalidate();
    }   
}


void CPrintInfoDbg::OnBnClickedButPrtIcChk()
{
	// TODO: Add your control notification handler code here
	if (g_taskHdl->getTaskingCnt() > 0)
	{
		MessageBox("有任务正在执行,请等待任务结束后再执行此操作", "提示", MB_OK | MB_ICONWARNING);
		return;
	}

	PRT_PRINTER_CFG_T *prtParam = printerCfgGet();
	char errData[512] = {0};
	if (OK != XpsCardGoOut(prtParam->printer, prtParam->cardReaderType, errData))
	{
		CString str;
		str.Format(_T("卡进位失败: %s"), errData);
		MessageBox(str, "提示", MB_OK | MB_ICONWARNING);
		return;
	}

	g_prtService->m_printerStopped = TRUE;

	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo; //This is an [in] parameter
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo; //Only compulsory field

	char exefile[256] = { 0 };
	SNPRINTF(exefile, 256, "%s读卡器卡座检测工具\\Psam卡编号-卡座检测工具.exe", g_localModDir);
	if (CreateProcess(exefile, NULL,
		NULL, NULL, FALSE, 0, NULL,
		NULL, &StartupInfo, &ProcessInfo))
	{
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}
	else
	{
		MessageBox("The process could not be started...");
	}

	g_prtService->m_printerStopped = FALSE;

	XpsCardGoBack();
}


void CPrintInfoDbg::OnBnClickedButPrtReset()
{
	// TODO: Add your control notification handler code here
	if (g_taskHdl->getTaskingCnt() > 0)
	{
		MessageBox("有任务正在执行,请等待任务结束后再执行此操作", "提示", MB_OK | MB_ICONWARNING);
		return;
	}

	g_prtService->m_printerStopped = TRUE;

	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo; //This is an [in] parameter
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo; //Only compulsory field

	char exefile[256] = { 0 };
	SNPRINTF(exefile, 256, "%s重置打印机工具\\打印机.exe", g_localModDir);
	if (CreateProcess(exefile, NULL,
		NULL, NULL, FALSE, 0, NULL,
		NULL, &StartupInfo, &ProcessInfo))
	{
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}
	else
	{
		MessageBox("The process could not be started...");
	}

	g_prtService->m_printerStopped = FALSE;
}


void CPrintInfoDbg::OnBnClickedButPrtClear()
{
	// TODO: Add your control notification handler code here
	if (MessageBox("请把设备的色带拿出来并且把清洁卡放在机器上，然后点击清洁.\n是否继续？", "提示", MB_OKCANCEL|MB_ICONWARNING) != IDOK)
	{
		return;
	}

	PRT_PRINTER_CFG_T *prtParam = printerCfgGet();
	int ret = XpsCleanPrinter(prtParam->printer);
	if (ret == 0)
	{
		MessageBox(_T("清洁打印机成功"));
	}
	else
	{
		MessageBox(_T("清洁打印机失败"));
	}
}
