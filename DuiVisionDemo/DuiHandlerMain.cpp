#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"


#define XML_ABOUT_DLG L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<dlg name=\"dlg_about\" title=\"MsgBox\" width=\"450\" height=\"230\" appwin=\"0\" >\
	<base>\
		\<imgbtn name=\"button.close\" pos=\"-45,0,-0,29\" skin=\"IDB_BT_CLOSE\"/>\
		<text name=\"title\" crtext=\"FFFFFF\" crmark=\"800000\" font=\"big\"\
				pos=\"10,5,200,25\" title=\"关于[APP_NAME]\" mask=\"[APP_NAME]\" response=\"0\"\ />\
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
		\
		<button name=\"button.ok\" skin=\"IDB_BT_DEFAULT\" title=\"[OK]\" pos=\"-100,-30,-20,-6\" />\
	</body>\
</dlg>";


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
	// 初始化托盘图标
	DuiSystem::Instance()->InitTray();
	// 将tab页3注册事件处理对象
	//DuiSystem::RegisterHandler(m_pDlg, new CDuiHandlerTab3(), _T("tab3"));
	// 将工具页注册事件处理对象
	//CDuiHandlerTools* pDuiHandlerTools = new CDuiHandlerTools();
	//pDuiHandlerTools->SetDialog(m_pDlg);
	//DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerTools, _T("tab.tools"));
	//pDuiHandlerTools->OnInit();
	// 启动动画定时器
	m_uTimerAni = DuiSystem::AddDuiTimer(500);
}

// 皮肤消息处理(实现皮肤的保存和获取)
BOOL CDuiHandlerMain::ProcessSkinMsg(UINT Msg, WPARAM wParam, LPARAM lParam)
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

// DUI事件处理
LRESULT CDuiHandlerMain::OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiHandlerTest::OnDuiMessage:uID=%d, name=%s, msg=%d, wParam=%d, lParam=%d"),
						uID, strName, Msg, wParam, lParam);

	if(strName == NAME_SKIN_WND)	// 皮肤消息处理
	{
		return ProcessSkinMsg(Msg, wParam, lParam);
	}else
	if(uID == APP_IPC)	// 进程间消息
	{
		ProcessInterprocessMsg(Msg, wParam, (DUI_INTERPROCESS_MSG*)lParam);
	}else
	if((strName == NAME_TRAY_ICON) && (Msg == MSG_TRAY_DBCLICK))	// 托盘双击消息
	{
		DuiSystem::ShowDuiDialog(_T("dlg_login"), NULL);
	}else
	if((strName == _T("notify_button_1")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::AddDuiNotifyMsgTask(_T("提示信息1！\n添加提示任务，窗口大小设置为400*250，此窗口5秒钟后自动关闭..."), MB_ICONINFORMATION | 2, _T("自定义标题"), 5000, 400, 250);
	}else
	if((strName == _T("notify_button_2")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::AddDuiNotifyMsgTask(_T("提示信息2！\n添加提示任务，窗口大小设置为300*200，此窗口3秒钟后自动关闭..."), MB_ICONINFORMATION | 2, _T(""), 3000, 300, 200);
	}else
	if((strName == _T("notify_button_3")) && (Msg == BUTTOM_UP))	// 自定义的Notify提示窗口
	{
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
	}else
	if((strName == _T("msgbox_button_1")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::DuiMessageBox(NULL, _T("演示对话框1：默认的对话框\n1.标题使用程序默认标题\n2.按钮使用默认的确定(MB_OK)按钮\n3.图标使用默认的信息图标"));
	}else
	if((strName == _T("msgbox_button_2")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::DuiMessageBox(NULL, _T("演示对话框2：\n1.标题使用自定义标题\n2.按钮使用确定取消(MB_OKCANCEL)按钮\n3.图标使用错误图标"), _T("自定义标题"), MB_OKCANCEL|MB_ICONERROR);
	}else
	if((strName == _T("msgbox_button_3")) && (Msg == BUTTOM_UP))
	{
		DuiSystem::DuiMessageBox(NULL, _T("演示对话框2：\n1.标题使用程序默认标题\n2.按钮使用是否取消(MB_YESNOCANCEL)按钮\n3.图标使用检查图标\n4.自定义对话框的大小"), _T(""), MB_YESNOCANCEL, 370, 200);
	}else
	if((strName == _T("timer_button_1")) && (Msg == BUTTOM_UP))	// 启动提示定时器
	{
		DuiSystem::AddDuiTimer(20000, _T("timer_notify"));
		DuiSystem::DuiMessageBox(NULL, _T("提示定时器已启动，定时周期20秒！"));
	}else
	if((strName == _T("timer_button_2")) && (Msg == BUTTOM_UP))	// 停止提示定时器
	{
		DuiSystem::RemoveDuiTimer(_T("timer_notify"));
		DuiSystem::DuiMessageBox(NULL, _T("提示定时器已停止！"));
	}else
	if((strName == _T("xml_content_dlg_button")) && (Msg == BUTTOM_UP))	// XML内容对话框
	{
		// 演示通过定义的XML内容加载窗口
		CString strDlgXmlContent = XML_ABOUT_DLG;
		CDlgBase* pDlg = DuiSystem::CreateDuiDialog(strDlgXmlContent, GetControlDialog(uID), _T(""), TRUE, 0, TRUE);
		if(pDlg != NULL)
		{
			int nResponse = pDlg->DoModal();
			DuiSystem::Instance()->RemoveDuiDialog(pDlg);
		}
	}else
	if((strName == _T("button_hide_tab")) && (Msg == BUTTOM_UP))	// 隐藏主tab页中的某个页面
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
		if(pTabCtrl)
		{
			pTabCtrl->SetItemVisible(7, FALSE);
			//pTabCtrl->DeleteItem(6);
			//pTabCtrl->DeleteItem(L"tab_6");
		}
	}else
	if((strName == _T("button_show_tab")) && (Msg == BUTTOM_UP))	// 显示主tab页中的某个页面
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
		if(pTabCtrl)
		{
			pTabCtrl->SetItemVisible(7, TRUE);
		}
	}else
	if((strName == _T("pic_button_2")) && (Msg == BUTTOM_UP))	// 刷新Tab页
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
		if(pTabCtrl)
		{
			pTabCtrl->LoadTabXml(L"tab_ext.xml");
		}
	}else
	if((strName == _T("button_load_tab")) && (Msg == BUTTOM_UP))	// 加载Tab页XML
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
		if(pTabCtrl)
		{
			pTabCtrl->DeleteItem(L"tab_5");
			pTabCtrl->DeleteItem(L"tab_6");
			pTabCtrl->DeleteItem(L"tab_7");
			pTabCtrl->LoadTabXml(L"tab_ext.xml");
		}
	}else
	if((strName == _T("button_hide_tab_check2")) && (Msg == BUTTOM_UP))	// 隐藏tab页
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.check")));
		if(pTabCtrl)
		{
			pTabCtrl->SetItemVisible(1, FALSE);
		}
	}else
	if((strName == _T("button_show_tab_check2")) && (Msg == BUTTOM_UP))	// 显示tab页
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.check")));
		if(pTabCtrl)
		{
			pTabCtrl->SetItemVisible(1, TRUE);
		}
	}else
	if((strName == _T("menu_1")) && (Msg == BUTTOM_UP))	// 菜单演示按钮
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_1"));
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		pDuiMenu->SetParent(pButton);
		CPoint point;
		CRect rc = pButton->GetRect();
		point.SetPoint(rc.left + 75, rc.bottom);
		if(pDlg != NULL)
		{
			pDlg->ClientToScreen(&point);
		}
		// 演示如何在菜单加载时候更改菜单项的显示标题、可见性、是否禁用、是否选择等属性
		// 必须在调用LoadXmlFile之前通过菜单项名字来设置相应菜单项的属性
		pDuiMenu->SetItemTitle(L"item_login", L"认证--修改Title");
		pDuiMenu->SetItemCheck(L"item_setup", 0);
		pDuiMenu->SetItemVisible(L"item_help", FALSE);
		pDuiMenu->SetItemDisable(L"item_about", TRUE);
		pDuiMenu->LoadXmlFile(_T("menu_tray"), pDlg, point, WM_DUI_MENU);
		pDuiMenu->ShowWindow(SW_SHOW);
	}else
	if((strName == _T("menu_2")) && (Msg == BUTTOM_UP))	// 菜单演示按钮
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_2"));
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		pDuiMenu->SetParent(pButton);
		CPoint point;
		CRect rc = pButton->GetRect();
		point.SetPoint(rc.left + 75, rc.bottom);
		if(pDlg != NULL)
		{
			pDlg->ClientToScreen(&point);
		}
		pDuiMenu->LoadXmlFile(_T("duivision\\menu_2.xml"), pDlg, point, WM_DUI_MENU);
		pDuiMenu->ShowWindow(SW_SHOW);
	}else
	if((strName == _T("menu_3")) && (Msg == BUTTOM_UP))	// 菜单演示按钮
	{
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_3"));
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		pDuiMenu->SetParent(pButton);
		CPoint point;
		CRect rc = pButton->GetRect();
		point.SetPoint(rc.left + 75, rc.bottom);
		if(pDlg != NULL)
		{
			pDlg->ClientToScreen(&point);
		}
		pDuiMenu->LoadXmlFile(_T("duivision\\menu_3.xml"), pDlg, point, WM_DUI_MENU);
		pDuiMenu->ShowWindow(SW_SHOW);
	}else
	if((strName == _T("listctrl_1")) && (Msg == BUTTOM_DOWN))	// 列表控件点击
	{
		// 点击了列表控件某一行,显示此行的详细信息,传入参数中wParam表示控件行号
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_1"));
		ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
		if(pDlg && pRowInfo)
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
			pDlg->OpenDlgPopup(pDlgPopup, rc, 0);
		}
	}else
	if((strName == _T("listctrl_2")) && (Msg == BUTTOM_DOWN))
	{
		// 点击了列表控件某一行,显示此行的详细信息
		// 传入参数中wParam表示控件行号,lParam表示点击的链接的索引(链接1和2分别为0和1)
		CDlgBase* pDlg = GetControlDialog(uID);
		CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
		ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
		if(pDlg && pRowInfo)
		{
			if(lParam != -1)
			{
				// 点击了行的某个链接
				CString strMsg;
				strMsg.Format(_T("点击了列表中的链接\n行内容：%s\n链接索引号：%d"), pRowInfo->strTitle, lParam);
				DuiSystem::DuiMessageBox(NULL, strMsg);
			}else
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
					pControlContent->SetTitle(pRowInfo->strContent);
				}
				pDlg->OpenDlgPopup(pDlgPopup, rc, 0);
			}
		}
	}else
	if((strName == _T("treectrl_1")) && (Msg == BUTTOM_DOWN))
	{
		CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
		HTREEITEM hNode = (HTREEITEM)wParam;
		int nItem = lParam;
		if(nItem == 0)
		{
			pTreeCtrl->ToggleNode(hNode);
		}
	}else
	if((strName == _T("item_setup")) && (Msg == BUTTOM_UP))	// 系统设置菜单
	{
		OnOption();
	}else
	if((strName == _T("config.button.ok")) && (Msg == BUTTOM_UP))	// 系统设置对话框的确认按钮
	{
		// 点击确认按钮之后可以做相应的处理,例如决定是否关闭窗口,获取窗口中控件的值等
		CDlgBase* pDlg = GetControlDialog(uID);
		if(pDlg)
		{
			OnOptionOK(pDlg);
		}
	}

    return 0;
}

// DUI定时器事件处理
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
		/*CRuning* pRunning = (CRuning*)GetControl(_T("animateimg_scan"));
		if(pRunning)
		{
			pRunning->SetRun(FALSE, m_nAniIndex);
		}*/

		CDuiProgress* pProgress1 = (CDuiProgress*)GetControl(_T("progress_1"));
		if(pProgress1)
		{
			pProgress1->SetProgress(m_nAniIndex*10);
		}

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
		//DuiSystem::RemoveDuiTimer(_T("timer_notify"));
	}
}

// 进程间消息处理
void CDuiHandlerMain::ProcessInterprocessMsg(UINT Msg, WPARAM wParam, DUI_INTERPROCESS_MSG* pInterMsg)
{
	// 命令行参数,可以对命令行参数进行处理,也可以直接显示主窗口
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, L"执行了命令行参数:" + strCmd);
	}else
	{
		CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(L"dlg_main");
		if(pDlg)
		{
			pDlg->SetForegroundWindow();
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->ShowWindow(SW_SHOW);
			pDlg->BringWindowToTop();
		}
	}
}

// 显示系统设置对话框
void CDuiHandlerMain::OnOption()
{
	USES_CONVERSION;

	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_option"), m_pDlg, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return;
	}

	// IP地址
	pDlg->SetControlValue(L"config.server.ip", L"title", L"192.168.1.1");

	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
}

//系统设置对话框确认
void CDuiHandlerMain::OnOptionOK(CDlgBase* pDlg)
{
	USES_CONVERSION;

	// IP地址
	CString strServerAddr = L"";
	CDuiEdit* pControlIp = static_cast<CDuiEdit*>(pDlg->GetControl(L"config.server.ip"));
	if(pControlIp)
	{
		strServerAddr = pControlIp->GetEditText();
		if(strServerAddr == L"")
		{
			// 地址不能为空提示
			return;
		}
	}

	pDlg->DoOK();
}