#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"
[!if TAB_CHECK_1]
#include "DuiHandler[!output TAB_CLASS_1].h"
[!else]
#include "DuiHandlerHome.h"
[!endif]
[!if TAB_CHECK_2]
#include "DuiHandler[!output TAB_CLASS_2].h"
[!endif]
[!if TAB_CHECK_3]
#include "DuiHandler[!output TAB_CLASS_3].h"
[!endif]
[!if TAB_CHECK_4]
#include "DuiHandler[!output TAB_CLASS_4].h"
[!endif]
[!if TAB_CHECK_5]
#include "DuiHandler[!output TAB_CLASS_5].h"
[!endif]
[!if TAB_CHECK_6]
#include "DuiHandler[!output TAB_CLASS_6].h"
[!endif]

//////////////////////////////////////////////////////////////
// CDuiHandlerMain

CDuiHandlerMain::CDuiHandlerMain(void) : CDuiHandler()
{
	m_pDlg = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerMain::~CDuiHandlerMain(void)
{
}

// 初始化
void CDuiHandlerMain::OnInit()
{
[!if OPTION_CHECK_TRAYICON]
	// 初始化托盘图标
	DuiSystem::Instance()->InitTray();
[!endif]
	
[!if TAB_CHECK_1]
	// 将tab页'[!output TAB_TITLE_1]'注册事件处理对象
	CDuiHandler[!output TAB_CLASS_1]* pDuiHandler[!output TAB_CLASS_1] = new CDuiHandler[!output TAB_CLASS_1]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_1], _T("tab.[!output TAB_CLASS_1]"));
	pDuiHandler[!output TAB_CLASS_1]->OnInit();
[!else]
	CDuiHandlerHome* pDuiHandlerHome = new CDuiHandlerHome();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerHome, _T("tab.Home"));
	pDuiHandlerHome->OnInit();
[!endif]
[!if TAB_CHECK_2]
	// 将tab页'[!output TAB_TITLE_2]'注册事件处理对象
	CDuiHandler[!output TAB_CLASS_2]* pDuiHandler[!output TAB_CLASS_2] = new CDuiHandler[!output TAB_CLASS_2]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_2], _T("tab.[!output TAB_CLASS_2]"));
	pDuiHandler[!output TAB_CLASS_2]->OnInit();
[!endif]
[!if TAB_CHECK_3]
	// 将tab页'[!output TAB_TITLE_3]'注册事件处理对象
	CDuiHandler[!output TAB_CLASS_3]* pDuiHandler[!output TAB_CLASS_3] = new CDuiHandler[!output TAB_CLASS_3]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_3], _T("tab.[!output TAB_CLASS_3]"));
	pDuiHandler[!output TAB_CLASS_3]->OnInit();
[!endif]
[!if TAB_CHECK_4]
	// 将tab页'[!output TAB_TITLE_4]'注册事件处理对象
	CDuiHandler[!output TAB_CLASS_4]* pDuiHandler[!output TAB_CLASS_4] = new CDuiHandler[!output TAB_CLASS_4]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_4], _T("tab.[!output TAB_CLASS_4]"));
	pDuiHandler[!output TAB_CLASS_4]->OnInit();
[!endif]
[!if TAB_CHECK_5]
	// 将tab页'[!output TAB_TITLE_5]'注册事件处理对象
	CDuiHandler[!output TAB_CLASS_5]* pDuiHandler[!output TAB_CLASS_5] = new CDuiHandler[!output TAB_CLASS_5]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_5], _T("tab.[!output TAB_CLASS_5]"));
	pDuiHandler[!output TAB_CLASS_5]->OnInit();
[!endif]
[!if TAB_CHECK_6]
	// 将tab页'[!output TAB_TITLE_6]'注册事件处理对象
	CDuiHandler[!output TAB_CLASS_6]* pDuiHandler[!output TAB_CLASS_6] = new CDuiHandler[!output TAB_CLASS_6]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_6], _T("tab.[!output TAB_CLASS_6]"));
	pDuiHandler[!output TAB_CLASS_6]->OnInit();
[!endif]

	// 启动动画定时器
	m_uTimerAni = DuiSystem::AddDuiTimer(500);
}

[!if OPTION_CHECK_SKINWND]
// 皮肤消息处理(实现皮肤的保存和获取)
LRESULT CDuiHandlerMain::OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(Msg == MSG_GET_SKIN_TYPE)	// 获取Skin类型
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		int nBkType = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE);
		*(int*)wParam = nBkType;
		return TRUE;
	}else
	if(Msg == MSG_GET_SKIN_VALUE)	// 获取Skin值
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			*(int*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES);
			return TRUE;
		}else
		if(wParam == BKTYPE_COLOR)
		{
			*(COLORREF*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR);
			return TRUE;
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			*(CString*)lParam = reg.GetStringValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE);
			return TRUE;
		}
	}else
	if(Msg == MSG_SET_SKIN_VALUE)	// 设置Skin值
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE, wParam);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES, lParam);
		}else
		if(wParam == BKTYPE_COLOR)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR, lParam);
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			CString* pstrImgFile = (CString*)lParam;
			reg.SetStringValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE, *pstrImgFile);
		}
		return TRUE;
	}
	return FALSE;
}
[!endif]

[!if OPTION_CHECK_TRAYICON]
// 托盘双击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgTrayIconDClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::ShowDuiDialog(_T("dlg_login"), NULL);
	return TRUE;
}
[!endif]

// DUI定时器事件处理
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}

// 进程间消息处理
LRESULT CDuiHandlerMain::OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 命令行参数,可以对命令行参数进行处理,也可以直接显示主窗口
	DUI_INTERPROCESS_MSG* pInterMsg = (DUI_INTERPROCESS_MSG*)lParam;
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, _T("执行了命令行参数:") + strCmd);
	}else
	{
		CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(_T("dlg_main"));
		if(pDlg)
		{
			pDlg->SetForegroundWindow();
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->ShowWindow(SW_SHOW);
			pDlg->BringWindowToTop();
		}
	}
	return TRUE;
}
