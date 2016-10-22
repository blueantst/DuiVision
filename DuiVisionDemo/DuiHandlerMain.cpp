#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"


#define XML_ABOUT_DLG _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<dlg name=\"dlg_about\" title=\"MsgBox\" width=\"450\" height=\"230\" appwin=\"0\" >\
	<base>\
		<imgbtn name=\"button.close\" pos=\"-45,0,-0,29\" skin=\"IDB_BT_CLOSE\" shortcut=\"ESC\"/>\
		<text name=\"title\" crtext=\"FFFFFF\" crmark=\"800000\" font=\"big\"\
				pos=\"10,5,200,25\" title=\"关于[APP_NAME]\" mask=\"[APP_NAME]\" response=\"0\" />\
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
// CDuiHandlerMain

CDuiHandlerMain::CDuiHandlerMain(void) : CDuiHandler()
{
	m_pDlg = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
	m_bMouseDownImageNormal = FALSE;
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

	// 初始化原生控件
	CDuiNativeWnd* pNativeWnd = (CDuiNativeWnd*)GetControl(_T("nativewnd_1"));
	if(pNativeWnd)
	{
		CEdit* pEdit = new CEdit;
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL;
		pEdit->Create(dwStyle, CRect(0,0,0,0), pNativeWnd->GetPaintWnd(), 1111);
		pNativeWnd->SetNativeWnd(pEdit);

		/*CListViewCtrlEx* pWndList = new CListViewCtrlEx;
		DWORD dwStyle = WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_OWNERDRAWFIXED;
		pWndList->Create(pNativeWnd->GetPaintHWnd(), NULL, NULL,
			dwStyle, 0, 1112, NULL);
		pWndList->SetObserverWindow(pNativeWnd->GetPaintHWnd());
		pWndList->InsertColumn(0, L"列1", LVCFMT_CENTER, 50);
		pWndList->InsertColumn(1, L"列2", LVCFMT_CENTER, 150);
		pWndList->InsertColumn(2, L"列3", LVCFMT_CENTER, 100);
		pWndList->SetItemHeight(22);
		int nItem = pWndList->AppendTitle("row1", Color(80,80,80), LISTITEM_EXPANDABLE|LISTITEM_BOLD);
		pWndList->AppendSubItem(nItem, "test1", SUBITEM_LINK);
		pNativeWnd->SetNativeWnd(pWndList);*/
	}

	// 演示列表控件通过API设置指定的行处于可见范围
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_1"));
	if(pListCtrl)
	{
		pListCtrl->EnsureVisible(10, TRUE);
	}

	// 演示表格控件通过API增加子控件
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	if(pGridCtrl)
	{
		// 添加链接按钮
		CLinkButton* pControl = (CLinkButton*)DuiSystem::CreateControlByName(_T("linkbtn"), NULL, NULL);
		if(pControl)
		{
			pControl->SetName(_T("grid1_updateinfo_link"));
			pControl->SetPosStr(_T("100, 3, -10, 25"));
			pControl->SetTitle(_T("更新内容"));
			pControl->SetLink(_T("http://www.blueantstudio.net"));
			pGridCtrl->AddSubItemControl(1, 0, pControl);
		}

		// 添加输入框
		CDuiEdit* pControlEdit = (CDuiEdit*)DuiSystem::CreateControlByName(_T("edit"), m_pDlg->GetSafeHwnd(), NULL);
		if(pControlEdit)
		{
			pControlEdit->SetName(_T("edit_gridctrl_btnName"));
			pControlEdit->SetTitle(_T("100"));
			pControlEdit->SetPosStr(_T("220, 10, -20, 40"));
			pControlEdit->OnAttributeSkin(_T("skin:IDB_EDIT"), TRUE);
			pGridCtrl->AddSubItemControl(4, 0, pControlEdit);
		}
	}

	// 演示通过API添加gridctrl行
	if(pGridCtrl)
	{
		for(int i=0; i<10; i++)
		{
			CString strId;
			strId.Format(_T("id_%d"), i);
			int nRow = pGridCtrl->InsertRow(-1,	// 插入的行序号,-1表示添加到最后
				strId,							// 行id字符串
				-1,								// 行左侧图片(索引图片方式,无索引图片填-1)
				Color(0, 0, 0, 0),				// 行文字颜色,全0表示默认(不使用行文字颜色,使用表格全局颜色)
				_T("skins\\icon\\scriptnet.png"),	// 行左侧的图片文件
				-1,								// 行右侧图片(索引图片方式,无索引图片填-1)
				_T(""),							// 行右侧的图片文件
				(i % 2));						// 行左侧的检查框状态(-1表示不显示检查框)
			CString strText;
			strText.Format(_T("程序名%d"), i);
			CString strContent;
			strContent.Format(_T("程序说明%d"), i);
			pGridCtrl->SetSubItem(nRow, 0, strText, strContent, TRUE);
			pGridCtrl->SetSubItem(nRow, 1, _T("已装：1.0.0.1"), _T("最新：2.3.0.1"));
			pGridCtrl->SetSubItem(nRow, 2, _T("11M"));
			pGridCtrl->SetSubItem(nRow, 3, _T("安装"));
		}
	}

	// 演示树控件通过API增加子控件
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	if(pTreeCtrl)
	{
		HTREEITEM hNode = pTreeCtrl->GetNodeById(_T("1-0"));
		CLinkButton* pControl = (CLinkButton*)DuiSystem::CreateControlByName(_T("linkbtn"), NULL, NULL);
		if(pControl)
		{
			pControl->SetName(_T("tree1_updateinfo_link"));
			pControl->SetPosStr(_T("10, 5, -10, 25"));
			pControl->SetTitle(_T("更新内容"));
			pControl->SetLink(_T("http://www.blueantstudio.net"));
			pTreeCtrl->AddSubItemControl(hNode, 2, pControl);
		}
	}

	// 演示树控件指定某个节点在可见范围内
	pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	if(pTreeCtrl)
	{
		HTREEITEM hNode = pTreeCtrl->GetNodeById(_T("2-7-1-1-1"));
		pTreeCtrl->EnsureVisible(hNode, TRUE);
	}

	// 演示在div中动态添加子控件(添加到基础控件的编辑框页面)
	CControlBase* pDiv = (CControlBase*)GetControl(_T("layout-1"));
	if(pDiv)
	{
		CDuiEdit* pControlEdit = static_cast<CDuiEdit*>(DuiSystem::CreateControlByName(_T("edit"), m_pDlg->GetSafeHwnd(), pDiv));
		if(pControlEdit)
		{
			pDiv->AddControl(pControlEdit);
			pControlEdit->SetName(_T("btnName"));
			pControlEdit->SetTitle(_T("动态添加的编辑框控件"));
			pControlEdit->SetPosStr(_T("50,220,250,250"));
			pControlEdit->OnAttributeSkin(_T("skin:IDB_EDIT"), FALSE);
			pControlEdit->OnAttributeLeftImage(_T("skin:IDB_COMPUTER"), FALSE);
			pControlEdit->OnPositionChange();
		}
	}
}

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

// 托盘双击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgTrayIconDClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::ShowDuiDialog(_T("dlg_login"), NULL);
	return TRUE;
}

// 托盘左键单击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgTrayIconLButtonDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//DuiSystem::ShowDuiDialog(_T("dlg_about"), NULL);
	return TRUE;
}

// 显示提示窗口1消息处理
LRESULT CDuiHandlerMain::OnDuiMsgNotifyButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("提示信息1！\n添加提示任务，窗口大小设置为400*250，此窗口5秒钟后自动关闭..."), MB_ICONINFORMATION | 2, _T("自定义标题"), 5000, 400, 250);
	return TRUE;
}

// 显示提示窗口2消息处理
LRESULT CDuiHandlerMain::OnDuiMsgNotifyButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("提示信息2！\n添加提示任务，窗口大小设置为300*200，此窗口3秒钟后自动关闭..."), MB_ICONINFORMATION | 2, _T(""), 3000, 300, 200);
	return TRUE;
}

// 显示提示窗口3消息处理
LRESULT CDuiHandlerMain::OnDuiMsgNotifyButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
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
LRESULT CDuiHandlerMain::OnDuiMsgMsgBoxButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("演示对话框1：默认的对话框\n1.标题使用程序默认标题\n2.按钮使用默认的确定(MB_OK)按钮\n3.图标使用默认的信息图标"));
	return TRUE;
}

// 显示信息对话框2消息处理
LRESULT CDuiHandlerMain::OnDuiMsgMsgBoxButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("演示对话框2：\n1.标题使用自定义标题\n2.按钮使用确定取消(MB_OKCANCEL)按钮\n3.图标使用错误图标"), _T("自定义标题"), MB_OKCANCEL|MB_ICONERROR);
	return TRUE;
}

// 显示信息对话框3消息处理
LRESULT CDuiHandlerMain::OnDuiMsgMsgBoxButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("演示对话框3：\n1.标题使用程序默认标题\n2.按钮使用是否取消(MB_YESNOCANCEL)按钮\n3.图标使用检查图标\n4.自定义对话框的大小"), _T(""), MB_YESNOCANCEL, 370, 200);
	return TRUE;
}

// 启动提示定时器消息处理
LRESULT CDuiHandlerMain::OnDuiMsgTimerButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiTimer(20000, _T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("提示定时器已启动，定时周期20秒！"));
	return TRUE;
}

// 停止提示定时器消息处理
LRESULT CDuiHandlerMain::OnDuiMsgTimerButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::RemoveDuiTimer(_T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("提示定时器已停止！"));
	return TRUE;
}

// 显示XML对话框消息处理
LRESULT CDuiHandlerMain::OnDuiMsgXmlDlgButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 演示通过定义的XML内容加载窗口
	CString strDlgXmlContent = XML_ABOUT_DLG;
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(strDlgXmlContent, GetControlDialog(uID), _T(""), TRUE, 0, TRUE);
	if(pDlg != NULL)
	{
		int nResponse = pDlg->DoModal();
		DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	}
	return TRUE;
}

// 隐藏主tab页中的某个页面消息处理
LRESULT CDuiHandlerMain::OnDuiMsgHideTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->SetItemVisible(7, FALSE);
		//pTabCtrl->DeleteItem(6);
		//pTabCtrl->DeleteItem(L"tab_6");
	}
	return TRUE;
}

// 显示主tab页中的某个页面消息处理
LRESULT CDuiHandlerMain::OnDuiMsgShowTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->SetItemVisible(7, TRUE);
	}
	return TRUE;
}

// 删除Web页面Tab页消息处理
LRESULT CDuiHandlerMain::OnDuiMsgDelWebTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->DeleteItem(_T("tab.activex"));
	}
	return TRUE;
}

// 加载Web页面Tab页消息处理
LRESULT CDuiHandlerMain::OnDuiMsgLoadWebTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)(pDlg->GetControl(_T("tabctrl.main")));
	if(pTabCtrl)
	{
		pTabCtrl->LoadTabXml(_T("duivision\\tab_ext_web.xml"));
	}
	return TRUE;
}

// 菜单1消息处理
LRESULT CDuiHandlerMain::OnDuiMsgMenuButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_1"));
	CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
	pDuiMenu->SetParent(pButton);
	CPoint point;
	CRect rc = pButton->GetRect();
	point.SetPoint(rc.left, rc.bottom);
	if(pDlg != NULL)
	{
		pDlg->ClientToScreen(&point);
	}
	// 演示如何在菜单加载时候更改菜单项的显示标题、可见性、是否禁用、是否选择等属性
	// 必须在调用LoadXmlFile之前通过菜单项名字来设置相应菜单项的属性
	pDuiMenu->SetItemTitle(_T("item_login"), _T("认证--修改Title"));
	pDuiMenu->SetItemCheck(_T("item_setup"), 0);
	pDuiMenu->SetItemVisible(_T("item_help"), FALSE);
	pDuiMenu->SetItemDisable(_T("item_about"), TRUE);
	if(pDuiMenu->LoadXmlFile(_T("menu_tray"), pDlg, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// 菜单2消息处理
LRESULT CDuiHandlerMain::OnDuiMsgMenuButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiButton* pButton = (CDuiButton*)GetControl(_T("menu_2"));
	CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
	pDuiMenu->SetParent(pButton);
	CPoint point;
	CRect rc = pButton->GetRect();
	point.SetPoint(rc.left, rc.bottom);
	if(pDlg != NULL)
	{
		pDlg->ClientToScreen(&point);
	}
	if(pDuiMenu->LoadXmlFile(_T("duivision\\menu_2.xml"), pDlg, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// 列表控件点击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgListCtrl1Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 点击了列表控件某一行,显示此行的详细信息,传入参数中wParam表示控件行号
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_1"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(pDlg && pRowInfo)
	{
		CDlgPopup* pDlgPopup = new CDlgPopup;
		CRect rc = pListCtrl->GetRect();
		rc.OffsetRect(50, 20);
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
	return TRUE;
}

// 列表控件点击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgListCtrl2Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 点击了列表控件某一行
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
		}
	}

	return TRUE;
}

// 列表控件双击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgListCtrl2DblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 双击了列表控件某一行,传入参数中wParam表示控件行号
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(pDlg && pRowInfo)
	{
		CString strMsg;
		strMsg.Format(_T("鼠标双击了列表中的行\n行号：%d\n行内容：%s"), wParam, pRowInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}

// 表格控件双击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgGridCtrlDblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 双击了表格控件某一行,传入参数中wParam表示控件行号
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	GridItemInfo* pItemInfo = pGridCtrl->GetItemInfo(wParam, lParam);
	if(pItemInfo)
	{
		CString strMsg;
		strMsg.Format(_T("鼠标双击了表格中的单元格\n行号：%d\n列号：%d\n行内容：%s"), wParam, lParam, pItemInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}

// 表格控件的删除按钮子控件点击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgGridCtrlDelBtnClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 点击了表格控件的删除行按钮子控件
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	pGridCtrl->DeleteRow(3);
	
	return TRUE;
}

// 拖拽文件到表格控件消息处理
LRESULT CDuiHandlerMain::OnDuiMsgGridCtrlDropFile(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 拖拽消息的wParam表格鼠标位置，lParam表示当前拖拽的文件全路径名
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	CPoint* pptDropFile = (CPoint*)wParam;
	CString* pstrDropFile = (CString*)lParam;
	// 截取文件名
	CString strFileName = *pstrDropFile;
	int nPos = strFileName.ReverseFind(_T('\\'));
	strFileName.Delete(0, nPos+1);
	// 在表格中插入一行文件信息
	if(pGridCtrl)
	{
		CString strId = *pstrDropFile;
		int nRow = pGridCtrl->InsertRow(-1,	// 插入的行序号,-1表示添加到最后
			strId,							// 行id字符串
			-1,								// 行左侧图片(索引图片方式,无索引图片填-1)
			Color(0, 0, 0, 0),				// 行文字颜色,全0表示默认(不使用行文字颜色,使用表格全局颜色)
			_T("skins\\icon\\NewIcons005.png"),	// 行左侧的图片文件
			-1,								// 行右侧图片(索引图片方式,无索引图片填-1)
			_T(""),							// 行右侧的图片文件
			0);
		pGridCtrl->SetSubItem(nRow, 0, strFileName, *pstrDropFile, TRUE);
		pGridCtrl->SetSubItem(nRow, 1, _T("文件"));
	}

	return TRUE;
}

// 树控件点击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgTreeCtrlClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	HTREEITEM hNode = (HTREEITEM)wParam;	// 树节点句柄	
	int nItem = lParam;	// 点击的树控件的第几列
	if(nItem == 0)
	{
		// 如果点击第一列,则展开或收缩此节点
		pTreeCtrl->ToggleNode(hNode);
	}
	return TRUE;
}

// 树控件双击消息处理
LRESULT CDuiHandlerMain::OnDuiMsgTreeCtrlDblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 双击了树控件某一节点,传入参数中wParam表示控件节点索引
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	HTREEITEM hNode = (HTREEITEM)wParam;	// 树节点句柄
	int nItem = lParam;	// 点击的树控件的第几列
	TreeItemInfo* pItemInfo = pTreeCtrl->GetItemInfo(hNode, nItem);
	if(pItemInfo)
	{
		CString strMsg;
		strMsg.Format(_T("鼠标双击了树节点\n节点内容：%s"), pItemInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}

// 显示系统设置对话框菜单消息处理
LRESULT CDuiHandlerMain::OnDuiMsgMenuOption(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_option"), m_pDlg, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	// IP地址
	pDlg->SetControlValue(_T("config.server.ip"), _T("title"), _T("192.168.1.1"));

	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	return TRUE;
}

// 系统设置对话框的确认按钮消息处理
LRESULT CDuiHandlerMain::OnDuiMsgOptionDlgOK(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	// IP地址
	CString strServerAddr = _T("");
	CDuiEdit* pControlIp = static_cast<CDuiEdit*>(pDlg->GetControl(_T("config.server.ip")));
	if(pControlIp)
	{
		strServerAddr = pControlIp->GetEditText();
		if(strServerAddr == _T(""))
		{
			// 地址不能为空提示
			return TRUE;
		}
	}

	pDlg->DoOK();
	return TRUE;
}

// 显示Flash1按钮消息处理
LRESULT CDuiHandlerMain::OnDuiMsgBtnShowFlash1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(_T("flash/afternoon.swf"));
	}
	return TRUE;
}

// 显示Flash2按钮消息处理
LRESULT CDuiHandlerMain::OnDuiMsgBtnShowFlash2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(_T("flash/morning.swf"));
	}
	return TRUE;
}

// 显示Flash3按钮消息处理
LRESULT CDuiHandlerMain::OnDuiMsgBtnShowFlash3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(_T("flash/night.swf"));
	}
	return TRUE;
}

// 显示Flash4按钮消息处理
LRESULT CDuiHandlerMain::OnDuiMsgBtnShowFlash4(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiFlashCtrl* pFlashCtrl = (CDuiFlashCtrl*)(pDlg->GetControl(_T("flash_ctrl_1")));
	if(pFlashCtrl)
	{
		pFlashCtrl->Navigate(_T("flash/noon.swf"));
	}
	return TRUE;
}

// Tab页签关闭按钮消息处理
LRESULT CDuiHandlerMain::OnDuiMsgTabCtrlClose(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

// image-normal控件的鼠标左键按下消息处理
LRESULT CDuiHandlerMain::OnDuiMsgImageNormalMouseLDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiPicture* pImgCtrl = (CDuiPicture*)GetControl(_T("image-normal"));
	if(pImgCtrl == NULL)
	{
		return FALSE;
	}

	m_bMouseDownImageNormal = TRUE;
	m_ptControlImageNormal = *((CPoint*)lParam);

	return TRUE;
}

// image-normal控件的鼠标左键放开消息处理
LRESULT CDuiHandlerMain::OnDuiMsgImageNormalMouseLUp(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiPicture* pImgCtrl = (CDuiPicture*)GetControl(_T("image-normal"));
	if(pImgCtrl == NULL)
	{
		return FALSE;
	}

	m_bMouseDownImageNormal = FALSE;

	return TRUE;
}

// image-normal控件的鼠标移动消息处理
LRESULT CDuiHandlerMain::OnDuiMsgImageNormalMouseMove(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiPicture* pImgCtrl = (CDuiPicture*)GetControl(_T("image-normal"));
	if(pImgCtrl == NULL)
	{
		return FALSE;
	}

	// 鼠标拖动控件
	if(m_bMouseDownImageNormal)
	{
		CPoint ptNew = *((CPoint*)lParam);	// 当前鼠标位置
		CControlBase* pParentCtrl = (CControlBase*)(pImgCtrl->GetParent());
		if(pParentCtrl)
		{
			CRect rcParent = pParentCtrl->GetRect();
			if(!rcParent.PtInRect(ptNew))
			{
				// 如果鼠标位置已经不在父控件范围内则退出
				return FALSE;
			}
		}

		// 获取控件的当前位置和鼠标当前位置与上一次位置的差值,将控件当前位置加上鼠标位置的差值
		CRect rc = pImgCtrl->GetRect();
		CSize offset = ptNew - m_ptControlImageNormal;
		rc.OffsetRect(offset);
		pImgCtrl->SetRect(rc);

		// 刷新鼠标上一次位置的变量
		m_ptControlImageNormal = *((CPoint*)lParam);

		// 刷新控件
		pImgCtrl->UpdateControl(true);
	}

	return TRUE;
}

// 带表头的表格控件右键放开消息处理,演示表格的右键菜单
LRESULT CDuiHandlerMain::OnDuiMsgGridCtrlHeaderRButtonUp(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 表格控件某一行,传入参数中wParam表示表格行号,lParam表示列号
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_header"));
	GridItemInfo* pItemInfo = pGridCtrl->GetItemInfo(wParam, lParam);
	if(pItemInfo)
	{
		// 显示右键菜单
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		pDuiMenu->SetAutoClose(FALSE);
		pDuiMenu->SetParent(pGridCtrl);
		CPoint point;
		CRect rc = pItemInfo->rcItem;
		// 计算菜单的显示位置
		point.SetPoint(rc.left + rc.Width() / 2, rc.bottom);

		CDlgBase* pParentDlg = GetControlDialog(uID);

		// 坐标转换为屏幕坐标
		pParentDlg->ClientToScreen(&point);

		CString strXmlFile = _T("duivision\\menu_tray.xml");
		if(pDuiMenu->LoadXmlFile(strXmlFile, pParentDlg, point, WM_DUI_MENU))
		{
			CRect rcMenu;
			pDuiMenu->GetWindowRect(&rcMenu);
			pDuiMenu->MoveWindow(rcMenu);

			pDuiMenu->ShowWindow(SW_SHOW);
			pDuiMenu->SetAutoClose(TRUE);
		}
	}
	return TRUE;
}

/*
// DUI事件处理
LRESULT CDuiHandlerMain::OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiHandlerMain::OnDuiMessage:uID=%d, name=%s, msg=%d, wParam=%d, lParam=%d"),
						uID, strName, Msg, wParam, lParam);

    return 0;
}
*/
// DUI定时器事件处理
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
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
