// UserInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrtStation.h"
#include "UserInfoDlg.h"
#include "afxdialogex.h"

#include "prtGlobal.h"
#include "PrtTaskHdl.h"

// UserInfoDlg dialog

IMPLEMENT_DYNAMIC(UserInfoDlg, CDialogEx)

UserInfoDlg::UserInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(UserInfoDlg::IDD, pParent)
{
	memset(m_strTimeBuf, 0, sizeof(m_strTimeBuf));
	memset(m_strOwnerName, 0, sizeof(m_strOwnerName));
}

UserInfoDlg::~UserInfoDlg()
{
}

void UserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PLACE2, m_edtPlace);
	DDX_Control(pDX, IDC_EDIT_NUM1, m_edtIder);
	DDX_Control(pDX, IDC_EDIT_ID1, m_edtCardNo);
	DDX_Control(pDX, IDC_EDIT_NAME1, m_edtName);
	DDX_Control(pDX, IDC_EDIT_ST_TASK, m_edtTaskStatus);
	DDX_Control(pDX, IDC_EDIT_ST_PRT, m_edtPrtStatus);
	DDX_Control(pDX, IDC_EDIT_ST_WR, m_edtWrStatus);
	DDX_Control(pDX, IDOK1, m_btnOk);
	DDX_Control(pDX, IDC_EDIT_TIME, m_edtTime);
}


BEGIN_MESSAGE_MAP(UserInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK1, &UserInfoDlg::OnBnClickedOk1)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDCANCEL1, &UserInfoDlg::OnBnClickedCancel1)
END_MESSAGE_MAP()


// UserInfoDlg message handlers
void UserInfoDlg::setTaskInfo(char *strTime, char *strName)
{
	strncpy(m_strTimeBuf, strTime, 63);
	strncpy(m_strOwnerName, strName, OWNER_TEXT_MAX_LEN);
}


BOOL UserInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	/*根据内容展示*/
	PRT_TASK_T *taskInfo = NULL;

	MUTEX_LOCK(g_taskHdl->m_prtTasksLock);
	taskInfo = g_taskHdl->getTask(m_strTimeBuf, m_strOwnerName);
	if (NULL == taskInfo)
	{
		MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);
		MessageBox("数据已经不存在,请检查是否完成!", "提示", MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	m_edtName.SetWindowText(taskInfo->outPrtInfo.ownerName); 
	m_edtCardNo.SetWindowText(taskInfo->outPrtInfo.ownerCardNum);
	m_edtIder.SetWindowText(taskInfo->outPrtInfo.ownerIdentityNum);
	m_edtPlace.SetWindowText(taskInfo->outPrtInfo.cardLaunchPlace);
	m_edtTime.SetWindowText(taskInfo->timeBuf);

	if (g_taskHdl->isTaskHdlEnd(taskInfo))
	{
		if (taskInfo->endByErrors)
		{
			m_edtTaskStatus.SetWindowText("异常结束");
		}
		else
		{
			m_edtTaskStatus.SetWindowText("正常结束");
		}
	}
	else
	{
		m_edtTaskStatus.SetWindowText("未完成");
	}

	if (taskInfo->printSt == ST_INIT)
	{
		m_edtPrtStatus.SetWindowText("未执行");
	}
	else if (taskInfo->printSt == ST_SUCCSS)
	{
		m_edtPrtStatus.SetWindowText("正常结束");
	}
	else if (taskInfo->printSt == ST_FAILED)
	{
		m_edtPrtStatus.SetWindowText("异常结束");
	}

	if (taskInfo->outIcSt == ST_INIT)
	{
		m_edtWrStatus.SetWindowText("未执行");
	}
	else if (taskInfo->outIcSt == ST_SUCCSS)
	{
		m_edtWrStatus.SetWindowText("正常结束");
	}
	else if (taskInfo->outIcSt == ST_FAILED)
	{
		m_edtWrStatus.SetWindowText("结束");
	}

	int operFlag = g_taskHdl->getTaskOperFlag(taskInfo);
	if (0 == operFlag)
	{
		m_btnOk.EnableWindow(FALSE);
	}
	else if (FLAG_PRINT == operFlag)
	{
		m_btnOk.SetWindowText("打卡");
	}
	else if (FLAG_WRITE_IC == operFlag)
	{
		m_btnOk.SetWindowText("写卡");
	}

	if (OK != g_taskHdl->photoToFile(taskInfo->outPrtInfo.ownerPhoto, taskInfo->outPrtInfo.ownerPhotoLen, g_testUserDir))
	{
		MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);
		MessageBox("图片解码失败,请检查是否完成!", "提示", MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);

	//根据路径载入图片
	HRESULT hResult = m_image.Load(_T(g_testUserDir));
	if (FAILED(hResult))
	{
		MessageBox(_T("加载测试图片失败！"), _T("加载失败"));
		return FALSE;
	}

	return TRUE;
}



void UserInfoDlg::OnBnClickedOk1()
{
	// TODO: Add your control notification handler code here
	PRT_TASK_T *taskInfo = NULL;

	MUTEX_LOCK(g_taskHdl->m_prtTasksLock);
	taskInfo = g_taskHdl->getTask(m_strTimeBuf, m_strOwnerName);
	if (NULL == taskInfo)
	{
		MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);
		MessageBox("数据已经不存在,请检查是否完成!", "提示", MB_OK);
		return;
	}

	taskInfo->operFlag = g_taskHdl->getTaskOperFlag(taskInfo);
	if (0 == taskInfo->operFlag)
	{
		MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);
		MessageBox("数据已经处理完成,无需再处理!", "提示", MB_OK);
		return;
	}
	
	/*执行任务*/
	g_prtService->printTaskHdl(taskInfo);

	MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);

	/*关闭自己并销毁*/
	this->OnOK();
	return;
}


void UserInfoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages


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
	pWnd = GetDlgItem(IDC_STATIC_PHOTO1);//获取控件句柄

	//获取Picture Control控件的大小
	//pWnd->GetWindowRect(&rect);
	//将客户区选中到控件表示的矩形区域内
	//ScreenToClient(&rect);

	//窗口移动到控件表示的区域
	//pWnd->MoveWindow(rect.left, rect.top, cx, cy, TRUE);

	pWnd->GetClientRect(&rect);

	CDC *pDc = NULL;
	pDc = pWnd->GetDC();//获取picture的DC
	m_image.Draw(pDc->m_hDC, rect);//将图片绘制到picture表示的区域内
	ReleaseDC(pDc);
}


void UserInfoDlg::OnBnClickedCancel1()
{
	// TODO: Add your control notification handler code here
	//不能用父类的OnCancel， 会调用到模式对话框的EndDialog()中
	this->OnCancel();
}


void UserInfoDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CDialogEx::PostNcDestroy();
}


void UserInfoDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogEx::OnCancel();
	DestroyWindow();
}


void UserInfoDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogEx::OnOK();
	DestroyWindow();
}
