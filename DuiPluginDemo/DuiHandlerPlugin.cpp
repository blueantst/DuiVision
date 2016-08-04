#include "StdAfx.h"
#include "DuiHandlerPlugin.h"

#define XML_ABOUT_DLG _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<dlg name=\"dlg_about\" title=\"MsgBox\" width=\"450\" height=\"230\" appwin=\"0\" >\
	<base>\
		<imgbtn name=\"button.close\" pos=\"-45,0,-0,29\" skin=\"IDB_BT_CLOSE\" shortcut=\"ESC\"/>\
		<text name=\"title\" crtext=\"FFFFFF\" crmark=\"800000\" font=\"big\"\
				pos=\"10,5,200,25\" title=\"关于[APP_NAME]-插件\" mask=\"[APP_NAME]\" response=\"0\" />\
	</base>\
	<body>\
		<area name=\"area-1\" pos=\"0,0,-0,40\" begin-transparent=\"100\" end-transparent=\"30\" />\
		<area name=\"area-2\" pos=\"0,40,-0,-0\" begin-transparent=\"30\" end-transparent=\"30\" />\
		<area name=\"area-3\" pos=\"0,-37,-0,-36\" begin-transparent=\"70\" end-transparent=\"70\" />\
		<area name=\"area-4\" pos=\"0,-36,-0,-0\" begin-transparent=\"88\" end-transparent=\"88\" />\
		\
		<img name=\"icon\" pos=\"25,45\" width=\"128\" height=\"128\" image=\"skins\\scriptnet.jpg\" mode=\"normal\" framesize=\"0\" response=\"0\" />\
		\
		<text crtext=\"000000\" pos=\"170,45,-25,65\" title=\"[APP_NAME] [APP_VER]\" />\
		<text crtext=\"000000\" pos=\"170,65,-25,85\" title=\"版权所有2013-2014\" />\
		<linkbtn name=\"linkbtn1\" crtext=\"800000\"\
			pos=\"170,100,-25,130\" title=\"点击打开链接\" href=\"http://www.blueantstudio.net\" />\
		<text crtext=\"000080\" pos=\"170,140,-25,160\" title=\"此对话框由定义的XML内容创建\" />\
		<button name=\"button.ok\" skin=\"IDB_BT_DEFAULT\" title=\"[OK]\" pos=\"-100,-30,-20,-6\" />\
	</body>\
</dlg>");

//////////////////////////////////////////////////////////////
// CDuiHandlerPlugin

CDuiHandlerPlugin::CDuiHandlerPlugin(void) : CDuiHandler()
{
	m_pPanel = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerPlugin::~CDuiHandlerPlugin(void)
{
}

// 初始化
void CDuiHandlerPlugin::OnInit()
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiHandlerPlugin::OnInit"));

	// 启动动画定时器
	m_uTimerAni = DuiSystem::AddDuiTimer(500);

	// 初始化原生控件
	CDuiNativeWnd* pNativeWnd = (CDuiNativeWnd*)GetControl(_T("nativewnd_1"));
	/*if(pNativeWnd)
	{
		CEdit* pEdit = new CEdit;
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_AUTOVSCROLL;
		pEdit->Create(dwStyle, CRect(0,0,0,0), CWnd::FromHandle(m_pPanel->GetHWND()), 1111);
		pNativeWnd->SetNativeWnd(pEdit);
		DuiSystem::LogEvent(LOG_LEVEL_DEBUG, L"CDuiHandlerPlugin::OnInit create native edit control");
	}*/
}

// DUI定时器事件处理
void CDuiHandlerPlugin::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
		CDuiProgress* pProgress2 = (CDuiProgress*)GetControl(_T("progress_2"));
		if(pProgress2)
		{
			pProgress2->SetProgress(m_nAniIndex*10);
		}

		m_nAniIndex++;
		if(m_nAniIndex > 10)
		{
			m_nAniIndex = 0;
		}
	}else
	if(_T("timer_notify") == strTimerName)
	{
		// 定时显示提示框
		DuiSystem::AddDuiNotifyMsgTask(_T("这是由定时器触发的提示框！\n触发周期：20秒\n此窗口5秒钟后自动关闭..."), MB_ICONINFORMATION | 2, _T(""), 5000);
	}
}

// 显示提示窗口1消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgNotifyButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("提示信息1！\n添加提示任务，窗口大小设置为400*250，此窗口5秒钟后自动关闭..."), MB_ICONINFORMATION | 2, _T("自定义标题"), 5000, 400, 250);
	return TRUE;
}

// 显示提示窗口2消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgNotifyButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("提示信息2！\n添加提示任务，窗口大小设置为300*200，此窗口3秒钟后自动关闭..."), MB_ICONINFORMATION | 2, _T(""), 3000, 300, 200);
	return TRUE;
}

// 显示提示窗口3消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgNotifyButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 自定义的Notify提示窗口
	DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));
	// 设置标题和内容
	DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.title"), _T("自定义提示窗口"));
	DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.text"), _T("非任务方式提示窗口，不自动关闭，大小为250*300，显示确定按钮，................."));
	// 设置图标
	DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_INFO"));
	// 设置类型,5种类型,需要把不用的类型隐藏掉
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.0"), TRUE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.1"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.2"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.3"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.4"), FALSE);
	// 设置大小
	DuiSystem::SetNotifyMsgBoxSize(250, 300);
	// 显示窗口
	DuiSystem::ShowNotifyMsgBox();
	return TRUE;
}

// 显示信息对话框1消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgMsgBoxButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("演示对话框1：默认的对话框\n1.标题使用程序默认标题\n2.按钮使用默认的确定(MB_OK)按钮\n3.图标使用默认的信息图标"));
	return TRUE;
}

// 显示信息对话框2消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgMsgBoxButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("演示对话框2：\n1.标题使用自定义标题\n2.按钮使用确定取消(MB_OKCANCEL)按钮\n3.图标使用错误图标"), _T("自定义标题"), MB_OKCANCEL|MB_ICONERROR);
	return TRUE;
}

// 显示信息对话框3消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgMsgBoxButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("演示对话框3：\n1.标题使用程序默认标题\n2.按钮使用是否取消(MB_YESNOCANCEL)按钮\n3.图标使用检查图标\n4.自定义对话框的大小"), _T(""), MB_YESNOCANCEL, 370, 200);
	return TRUE;
}

// 启动提示定时器消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgTimerButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiTimer(20000, _T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("提示定时器已启动，定时周期20秒！"));
	return TRUE;
}

// 停止提示定时器消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgTimerButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::RemoveDuiTimer(_T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("提示定时器已停止！"));
	return TRUE;
}

// 显示XML对话框消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgXmlDlgButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 演示通过定义的XML内容加载窗口
	CString strDlgXmlContent = XML_ABOUT_DLG;
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(strDlgXmlContent, NULL, _T(""), TRUE, 0, TRUE);
	if(pDlg != NULL)
	{
		int nResponse = pDlg->DoModal();
		DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	}
	return TRUE;
}

// 菜单1消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgMenuButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_1"));
	CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
	pDuiMenu->SetParent(pButton);
	CPoint point;
	CRect rc = pButton->GetRect();
	point.SetPoint(rc.left, rc.bottom);
	if(m_pPanel)
	{
		::ClientToScreen(m_pPanel->GetHWND(), &point);
	}
	// 演示如何在菜单加载时候更改菜单项的显示标题、可见性、是否禁用、是否选择等属性
	// 必须在调用LoadXmlFile之前通过菜单项名字来设置相应菜单项的属性
	pDuiMenu->SetItemTitle(_T("item_login"), _T("认证--修改Title"));
	pDuiMenu->SetItemCheck(_T("item_setup"), 0);
	pDuiMenu->SetItemVisible(_T("item_help"), FALSE);
	pDuiMenu->SetItemDisable(_T("item_about"), TRUE);
	if(pDuiMenu->LoadXmlFile(_T("menu_tray"), NULL, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// 菜单2消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgMenuButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_2"));
	CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
	pDuiMenu->SetParent(pButton);
	CPoint point;
	CRect rc = pButton->GetRect();
	point.SetPoint(rc.left, rc.bottom);
	if(m_pPanel)
	{
		::ClientToScreen(m_pPanel->GetHWND(), &point);
	}
	if(pDuiMenu->LoadXmlFile(_T("duivision\\menu_2.xml"), NULL, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// 列表控件点击消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgListCtrl1Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 点击了列表控件某一行,显示此行的详细信息,传入参数中wParam表示控件行号
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_1"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(m_pPanel && pRowInfo)
	{
		CDlgPopup* pDlgPopup = new CDlgPopup;
		CRect rc = pListCtrl->GetRect();
		rc.OffsetRect(50, 30);
		pDlgPopup->LoadXmlFile(_T("xml:dlg_notice"));
		CControlBaseFont* pControlTitle = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.title")));
		if(pControlTitle)
		{
			pControlTitle->SetTitle(pRowInfo->strTitle);
		}
		CControlBaseFont* pControlTime = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.time")));
		if(pControlTime)
		{
			pControlTime->SetTitle(pRowInfo->strTime);
		}
		CControlBaseFont* pControlContent = (CControlBaseFont*)(pDlgPopup->GetControl(_T("notice.content")));
		if(pControlContent)
		{
			//CString strTmp = L"www\nwww\nj";
			pControlContent->SetTitle(pRowInfo->strContent);
		}
		m_pPanel->OpenDlgPopup(pDlgPopup, rc, 0);
	}
	return TRUE;
}

// 列表控件点击消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgListCtrl2Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 点击了列表控件某一行
	// 传入参数中wParam表示控件行号,lParam表示点击的链接的索引(链接1和2分别为0和1)
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(m_pPanel && pRowInfo)
	{
		if(lParam != -1)
		{
			// 点击了行的某个链接
			CString strMsg;
			strMsg.Format(_T("点击了列表中的链接\n行内容：%s\n链接索引号：%d"), pRowInfo->strTitle, lParam);
			DuiSystem::DuiMessageBox(NULL, strMsg);
		}
	}

	return TRUE;
}

// 列表控件双击消息处理
LRESULT CDuiHandlerPlugin::OnDuiMsgListCtrl2DblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 双击了列表控件某一行,传入参数中wParam表示控件行号
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(m_pPanel && pRowInfo)
	{
		CString strMsg;
		strMsg.Format(_T("鼠标双击了列表中的行\n行号：%d\n行内容：%s"), wParam, pRowInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}
