#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"

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
	CRegistryUtil reg(HKEY_CURRENT_USER);
	CString strStartOpen = reg.GetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY, REG_EXPLORER_STARTOPEN);
	if(strStartOpen == L"last")	// 打开上次退出时未关闭的页面
	{
		// 获取保存的浏览器页签URL
		for(int i=1; i<=20; i++)
		{
			CString strUrlKey;
			strUrlKey.Format(L"url%d", i);
			CString strUrl = reg.GetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY_URL, strUrlKey);
			if(!strUrl.IsEmpty())
			{
				m_asUrl.Add(strUrl);
			}
		}

		// 创建页面
		for(int i=0; i<m_asUrl.GetSize(); i++)
		{
			InsertExplorerTab(1, L"", m_asUrl[i]);
		}
	}

	// 启动动画定时器
	m_uTimerAni = DuiSystem::AddDuiTimer(500);
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
	return TRUE;
}

// 获取Tab页中的IE浏览器控件
CDuiWebBrowserCtrl* CDuiHandlerMain::GetTabWebIEControl(CControlBase* pTabCtrl)
{
	if(pTabCtrl == NULL)
	{
		return NULL;
	}

	vector<CControlBase*>* paControls = pTabCtrl->GetControls();
	if(paControls)
	{
		for (size_t i = 0; i < paControls->size(); i++)
		{
			CControlBase* pControl = paControls->at(i);
			if(pControl && pControl->IsClass(CDuiWebBrowserCtrl::GetClassName()))
			{
				return (CDuiWebBrowserCtrl*)pControl;
			}
		}
	}

	return NULL;
}

// 获取Tab页中的wke浏览器控件
CDuiWkeView* CDuiHandlerMain::GetTabWebWkeControl(CControlBase* pTabCtrl)
{
	if(pTabCtrl == NULL)
	{
		return NULL;
	}

	vector<CControlBase*>* paControls = pTabCtrl->GetControls();
	if(paControls)
	{
		for (size_t i = 0; i < paControls->size(); i++)
		{
			CControlBase* pControl = paControls->at(i);
			if(pControl && pControl->IsClass(CDuiWkeView::GetClassName()))
			{
				return (CDuiWkeView*)pControl;
			}
		}
	}

	return NULL;
}

// 获取Tab页中某个控件ID对应的Tab页信息
TabItemInfo* CDuiHandlerMain::GetTabInfoByWebCtrlId(UINT uID)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		int nCount = pTabCtrl->GetItemCount();
		for(int i=0; i<nCount; i++)
		{
			TabItemInfo* pTabInfo = pTabCtrl->GetItemInfo(i);
			if(pTabInfo && pTabInfo->pControl)
			{
				CDuiWebBrowserCtrl* pWebControl = (CDuiWebBrowserCtrl*)GetTabWebIEControl(pTabInfo->pControl);
				if(pWebControl && (pWebControl->GetID() == uID))
				{
					return pTabInfo;
				}else
				{
					CDuiWkeView* pWebControl = (CDuiWkeView*)GetTabWebWkeControl(pTabInfo->pControl);
					if(pWebControl && (pWebControl->GetID() == uID))
					{
						return pTabInfo;
					}
				}
			}
		}
	}

	return NULL;
}

// 获取当前页面的IE浏览器控件
CDuiWebBrowserCtrl* CDuiHandlerMain::GetCurTabWebIEControl()
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		int nItem = pTabCtrl->GetSelectItem();
		TabItemInfo* pTabInfo = pTabCtrl->GetItemInfo(nItem);
		if(pTabInfo && pTabInfo->pControl)
		{
			CDuiWebBrowserCtrl* pWebControl = (CDuiWebBrowserCtrl*)GetTabWebIEControl(pTabInfo->pControl);
			return pWebControl;
		}
	}

	return NULL;
}

// 获取当前页面的wke浏览器控件
CDuiWkeView* CDuiHandlerMain::GetCurTabWebWkeControl()
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		int nItem = pTabCtrl->GetSelectItem();
		TabItemInfo* pTabInfo = pTabCtrl->GetItemInfo(nItem);
		if(pTabInfo && pTabInfo->pControl)
		{
			CDuiWkeView* pWebControl = (CDuiWkeView*)GetTabWebWkeControl(pTabInfo->pControl);
			return pWebControl;
		}
	}

	return NULL;
}

// 创建浏览器页面
void CDuiHandlerMain::InsertExplorerTab(int nIndex, CString strTitle, CString strUrl)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		// 浏览器类型
		CRegistryUtil reg(HKEY_CURRENT_USER);
		CString strWebType = reg.GetStringValue(NULL, REG_EXPLORER_SUBKEY, REG_EXPLORER_WEBTYPE);
		BOOL bWebTypeIE = (strWebType != L"wke");

		if(bWebTypeIE)
		{
			pTabCtrl->LoadTabXml(L"tab_webie");
			int nCount = pTabCtrl->GetItemCount();
			pTabCtrl->SetSelectItem(nCount-1);
			TabItemInfo* pTabInfo = pTabCtrl->GetItemInfo(nCount - 1);
			if(pTabInfo && pTabInfo->pControl)
			{
				pTabInfo->strText = strTitle;
				pTabInfo->nImageIndex = 0;
				CDuiWebBrowserCtrl* pWebControl = (CDuiWebBrowserCtrl*)GetTabWebIEControl(pTabInfo->pControl);
				if(pWebControl)
				{
					pWebControl->Navigate(strUrl);
				}
			}
		}else
		{
			pTabCtrl->LoadTabXml(L"tab_webwke");
			int nCount = pTabCtrl->GetItemCount();
			pTabCtrl->SetSelectItem(nCount-1);
			TabItemInfo* pTabInfo = pTabCtrl->GetItemInfo(nCount - 1);
			if(pTabInfo && pTabInfo->pControl)
			{
				pTabInfo->strText = strTitle;
				pTabInfo->nImageIndex = 0;
				CDuiWkeView* pWebControl = (CDuiWkeView*)GetTabWebWkeControl(pTabInfo->pControl);
				if(pWebControl)
				{
					pWebControl->Navigate(strUrl);
				}
			}
		}
	}
/*
	CDuiPanel* pControlPanel = (CDuiPanel*)DuiSystem::CreateControlByName(L"div", m_pDlg->GetSafeHwnd(), m_pDuiObject);
	if(pControlPanel)
	{
		// 创建div,并插入tab页
		//pControlPanel->SetName(L"");
		pTabCtrl->AddControl(pControlPanel);
		pControlPanel->SetTitle(strTitle);
		pControlPanel->SetEnableScroll(FALSE);
		pControlPanel->SetPosStr(L"0,27,-0,-0");
		pControlPanel->OnPositionChange();
		CString strId;
		strId.Format(L"url-%d", nIndex);
		pTabCtrl->InsertItem(nIndex, 5000+nIndex, strId, strTitle, L"", 0, pControlPanel);

		// 创建浏览器控件
		CDuiWebBrowserCtrl* pControlWeb = (CDuiWebBrowserCtrl*)DuiSystem::CreateControlByName(L"webbrowser", m_pDlg->GetSafeHwnd(), pControlPanel);
		pControlPanel->AddControl(pControlWeb);
		pControlWeb->SetDelayCreate(true);
		pControlWeb->Navigate(strUrl);
		pControlWeb->SetPosStr(L"0,47,-1,-1");
		
		pControlWeb->OnPositionChange();
	}
*/
}

// 保存打开的所有页面URL到注册表
void CDuiHandlerMain::SaveExplorerUrls()
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl == NULL)
	{
		return;
	}

	m_asUrl.RemoveAll();
	int nCount = pTabCtrl->GetItemCount();
	for(int i=0; i<nCount; i++)
	{
		TabItemInfo* pTabInfo = pTabCtrl->GetItemInfo(i);
		if(pTabInfo && pTabInfo->pControl)
		{
			CDuiWebBrowserCtrl* pWebControlIE = (CDuiWebBrowserCtrl*)GetTabWebIEControl(pTabInfo->pControl);
			CDuiWkeView* pWebControlWke = (CDuiWkeView*)GetTabWebWkeControl(pTabInfo->pControl);
			CString strUrl = L"";
			if(pWebControlIE != NULL)
			{
				strUrl = pWebControlIE->getURL();
			}else
			if(pWebControlWke != NULL)
			{
				strUrl = pWebControlWke->getURL();
			}

			if(!strUrl.IsEmpty())
			{
				m_asUrl.Add(strUrl);
			}
		}
	}

	CRegistryUtil reg(HKEY_CURRENT_USER);
	// 更新注册表中的URL列表,最多纪录20个URL
	for(int i=1; i<=20; i++)
	{
		CString strUrlKey;
		strUrlKey.Format(L"url%d", i);
		if(m_asUrl.GetSize() >= i)
		{
			reg.SetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY_URL, strUrlKey, m_asUrl[i-1]);
		}else
		{
			reg.SetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY_URL, strUrlKey, L"");
		}
	}
}

// 显示系统设置对话框菜单消息处理
LRESULT CDuiHandlerMain::OnDuiMsgMenuOption(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_option"), m_pDlg, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	// 浏览器类型
	CRegistryUtil reg(HKEY_CURRENT_USER);
	CString strWebType = reg.GetStringValue(NULL, REG_EXPLORER_SUBKEY, REG_EXPLORER_WEBTYPE);
	BOOL bWebTypeIE = (strWebType != L"wke");
	pDlg->SetControlValue(L"option.webtype.ie", L"check", bWebTypeIE ? L"true" : L"false");
	pDlg->SetControlValue(L"option.webtype.wke", L"check", !bWebTypeIE ? L"true" : L"false");

	// 启动时打开的页面
	CString strStartOpen = reg.GetStringValue(NULL, REG_EXPLORER_SUBKEY, REG_EXPLORER_STARTOPEN);
	pDlg->SetControlValue(L"option.startopen.last", L"check", (strStartOpen == L"last") ? L"true" : L"false");
	pDlg->SetControlValue(L"option.startopen.home", L"check", (strStartOpen == L"home") ? L"true" : L"false");
	pDlg->SetControlValue(L"option.startopen.none", L"check", (strStartOpen == L"none") ? L"true" : L"false");

	// 默认搜索引擎
	CString strSearchEngineType = reg.GetStringValue(NULL, REG_EXPLORER_SUBKEY, REG_EXPLORER_SEARCHTYPE);
	pDlg->SetControlValue(L"option.search.engine", L"value", strSearchEngineType);

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

	CRegistryUtil reg(HKEY_CURRENT_USER);

	// 浏览器类型
	CDuiRadioButton* pControlWebTypeIE = static_cast<CDuiRadioButton*>(pDlg->GetControl(L"option.webtype.ie"));
	CString strWebType = pControlWebTypeIE->GetGroupValue();
	reg.SetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY, REG_EXPLORER_WEBTYPE, strWebType);

	// 启动时打开的页面
	CDuiRadioButton* pControlStartOpenLast = static_cast<CDuiRadioButton*>(pDlg->GetControl(L"option.startopen.last"));
	CString strStartOpen = pControlStartOpenLast->GetGroupValue();
	reg.SetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY, REG_EXPLORER_STARTOPEN, strStartOpen);

	// 默认搜索引擎
	CDuiComboBox* pSearchEngineCtrl = static_cast<CDuiComboBox*>(pDlg->GetControl(L"option.search.engine"));
	CString strSearchEngineType = pSearchEngineCtrl->GetComboValue();
	reg.SetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY, REG_EXPLORER_SEARCHTYPE, strSearchEngineType);

	pDlg->DoOK();
	return TRUE;
}

// 导航到上一页
LRESULT CDuiHandlerMain::OnDuiMsgButtonGoBack(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
	CDuiWkeView* pWebControlWke = GetCurTabWebWkeControl();
	if(pWebControlIE)
	{
		pWebControlIE->GoBack();
	}else
	if(pWebControlWke)
	{
		pWebControlWke->goBack();
	}

	return TRUE;
}

// 导航到下一页
LRESULT CDuiHandlerMain::OnDuiMsgButtonGoForward(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
	CDuiWkeView* pWebControlWke = GetCurTabWebWkeControl();
	if(pWebControlIE)
	{
		pWebControlIE->GoForward();
	}else
	if(pWebControlWke)
	{
		pWebControlWke->goForward();
	}

	return TRUE;
}

// 刷新web页面
LRESULT CDuiHandlerMain::OnDuiMsgButtonRefresh(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
	CDuiWkeView* pWebControlWke = GetCurTabWebWkeControl();
	if(pWebControlIE)
	{
		pWebControlIE->Refresh();
	}else
	if(pWebControlWke)
	{
		//pWebControlWke->();
	}

	return TRUE;
}

// 导航到首页
LRESULT CDuiHandlerMain::OnDuiMsgButtonHome(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
	CDuiWkeView* pWebControlWke = GetCurTabWebWkeControl();
	if(pWebControlIE)
	{
		pWebControlIE->Navigate(L"http://www.blueantstudio.net");
	}else
	if(pWebControlWke)
	{
		pWebControlWke->Navigate(L"http://www.blueantstudio.net");
	}

	return TRUE;
}

// URL输入框的键盘事件处理(仅处理回车事件)
LRESULT CDuiHandlerMain::OnDuiMsgComboUrlKeyDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(wParam != VK_RETURN)
	{
		return FALSE;
	}

	CString strUrl = L"";
	CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
	if(pUrlCtrl != NULL)
	{
		// 如果url输入控件不是焦点控件,则退出
		if(!pUrlCtrl->IsFocusControl())
		{
			return FALSE;
		}
		strUrl = pUrlCtrl->GetEditText();
	}

	CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
	CDuiWkeView* pWebControlWke = GetCurTabWebWkeControl();
	if((pWebControlIE == NULL) && (pWebControlWke == NULL))
	{
		// 如果没有创建浏览器窗口,需要先创建
		InsertExplorerTab(-1, L"", L"");
		pWebControlIE = GetCurTabWebIEControl();
		pWebControlWke = GetCurTabWebWkeControl();
	}

	// 导航到输入的url
	if(pWebControlIE)
	{
		pWebControlIE->Navigate(strUrl);
	}else
	if(pWebControlWke)
	{
		pWebControlWke->Navigate(strUrl);
	}

	return TRUE;
}

// URL输入框的事件处理(目前仅处理URL输入框的下拉列表选择事件)
LRESULT CDuiHandlerMain::OnDuiMsgComboUrl(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CString strUrl = L"";
	CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
	if(pUrlCtrl != NULL)
	{
		strUrl = pUrlCtrl->GetEditText();
	}

	CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
	CDuiWkeView* pWebControlWke = GetCurTabWebWkeControl();
	if((pWebControlIE == NULL) && (pWebControlWke == NULL))
	{
		// 如果没有创建浏览器窗口,需要先创建
		InsertExplorerTab(-1, L"", L"");
		pWebControlIE = GetCurTabWebIEControl();
		pWebControlWke = GetCurTabWebWkeControl();
	}

	// 导航到输入的url
	if(pWebControlIE)
	{
		pWebControlIE->Navigate(strUrl);
	}else
	if(pWebControlWke)
	{
		pWebControlWke->Navigate(strUrl);
	}

	return TRUE;
}

// 搜索输入框的键盘事件处理(仅处理回车事件)
LRESULT CDuiHandlerMain::OnDuiMsgEditSearchKeyDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(wParam != VK_RETURN)
	{
		return FALSE;
	}

	CDuiEdit* pSearchEditCtrl = (CDuiEdit*)GetControl(_T("edit.search"));
	if((pSearchEditCtrl == NULL) || !pSearchEditCtrl->IsFocusControl())
	{
		return FALSE;
	}

	// 获取搜索框输入内容
	CString strSearch = pSearchEditCtrl->GetEditText();
	if(strSearch.IsEmpty())
	{
		return FALSE;
	}

	// 获取浏览器控件
	CDuiWebBrowserCtrl* pWebControlIE = NULL;
	CDuiWkeView* pWebControlWke = NULL;
	if((pWebControlIE == NULL) && (pWebControlWke == NULL))
	{
		// 如果没有创建浏览器窗口,需要先创建
		InsertExplorerTab(-1, L"", L"");
		pWebControlIE = GetCurTabWebIEControl();
		pWebControlWke = GetCurTabWebWkeControl();
	}

	// 获取默认搜索引擎
	CRegistryUtil reg(HKEY_CURRENT_USER);
	CString strSearchEngineType = reg.GetStringValue(NULL, REG_EXPLORER_SUBKEY, REG_EXPLORER_SEARCHTYPE);
	CString strSearchUrl;
	if(strSearchEngineType == L"haosou")
	{
		strSearchUrl.Format(L"http://www.haosou.com/s?ie=utf-8&src=duivision_explorer&q=%s", strSearch);
	}else
	if(strSearchEngineType == L"baidu")
	{
		strSearchUrl.Format(L"http://www.baidu.com/s?wd=%s&ie={utf-8}", strSearch);
	}else
	if(strSearchEngineType == L"google")
	{
		strSearchUrl.Format(L"{google:baseURL}search?client=duivision_explorer&ie={utf-8}&q=%s&{google:RLZ}{google:originalQueryForSuggestion}{google:assistedQueryStats}{google:searchFieldtrialParameter}{google:bookmarkBarPinned}{google:searchClient}{google:instantExtendedEnabledParameter}{google:omniboxStartMarginParameter}", strSearch);
	}else
	{
		return FALSE;
	}

	// 导航到输入的url
	if(pWebControlIE)
	{
		pWebControlIE->Navigate(strSearchUrl);
	}else
	if(pWebControlWke)
	{
		pWebControlWke->Navigate(strSearchUrl);
	}

	return TRUE;
}

// 新建页面
LRESULT CDuiHandlerMain::OnDuiMsgButtonNewTab(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 新建web页面
	InsertExplorerTab(-1, L"", L"");

	return TRUE;
}

// 切换页面
LRESULT CDuiHandlerMain::OnDuiMsgTabCtrlSwitch(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
	if((pTabCtrl == NULL) || (pUrlCtrl == NULL))
	{
		return FALSE;
	}

	int nTabItem = wParam;

	// 刷新输入框的url
	CString strUrl = L"";
	CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
	CDuiWkeView* pWebControlWke = GetCurTabWebWkeControl();
	if(pWebControlIE)
	{
		strUrl = pWebControlIE->getURL();
		pUrlCtrl->SetTitle(strUrl);
	}else
	if(pWebControlWke)
	{
		strUrl = pWebControlWke->getURL();
		pUrlCtrl->SetTitle(strUrl);
	}

	// 隐藏页面加载进度条
	CDuiProgress* pProgress = (CDuiProgress*)GetControl(_T("progress.webload"));
	if(pProgress != NULL)
	{
		pProgress->SetVisible(FALSE);
	}

	return TRUE;
}

// 关闭页面
LRESULT CDuiHandlerMain::OnDuiMsgButtonCloseTab(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		// wParam参数表示tab页索引
		pTabCtrl->DeleteItem(wParam);
		// URL纪录写入注册表
		SaveExplorerUrls();
	}

	return TRUE;
}

#define MAX_URL 256
// IE浏览器页面的标题变更事件
LRESULT CDuiHandlerMain::OnDuiMsgWebIETitleChange(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 获取页面标题
	CString strTitle = L"";
	DISPPARAMS FAR* pdispparams = (DISPPARAMS FAR*)wParam;
	if(pdispparams && pdispparams->rgvarg[0].vt == VT_BSTR)
	{
		WCHAR szTitle[MAX_URL];
		int len = wcslen(pdispparams->rgvarg[0].bstrVal);               
		wcsncpy(szTitle, pdispparams->rgvarg[0].bstrVal, MAX_URL - 5);
		if(len > MAX_URL - 5)
		{
			wcscat(szTitle, L"...");
		}
		strTitle = szTitle;
	}

	// 找对对应的浏览器控件,并设置对应的tab页签文字
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	TabItemInfo* pTabInfo = GetTabInfoByWebCtrlId(uID);
	if(pTabInfo && pTabCtrl)
	{
		pTabInfo->strText = strTitle;
		pTabCtrl->RefreshItems();
	}

	return TRUE;
}

// IE浏览器导航完成事件
LRESULT CDuiHandlerMain::OnDuiMsgWebIENavigateComplete(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	TabItemInfo* pTabInfo = GetTabInfoByWebCtrlId(uID);
	if(pTabInfo == NULL)
	{
		return FALSE;
	}

	// 获取URL
	DISPPARAMS FAR* pDispParams = (DISPPARAMS FAR*)wParam;
	if(pDispParams && (pDispParams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT)))
	{
		// VT_BYREF|VT_VARIANT类型字符串用下面的方法转换
		CComVariant varURL(*pDispParams->rgvarg[0].pvarVal);
		varURL.ChangeType(VT_BSTR);
		CString strUrl = OLE2T(varURL.bstrVal);

		// 找对对应的浏览器控件,如果是当前页面,则刷新URL输入框内容
		CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
		if(pUrlCtrl && pTabCtrl && pTabInfo->pControl)
		{
			CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
			if((pWebControlIE != NULL) && ((CDuiWebBrowserCtrl*)GetTabWebIEControl(pTabInfo->pControl) == pWebControlIE))
			{
				pUrlCtrl->SetTitle(strUrl);
				// URL纪录写入注册表
				SaveExplorerUrls();
			}
		}
	}

	return TRUE;
}

// IE浏览器新窗口事件
LRESULT CDuiHandlerMain::OnDuiMsgWebIENewWindow(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 1.阻止IE浏览器自己的行为,必须pbCancel设置为VARIANT_TRUE才能阻止
	DISPPARAMS FAR* pDispParams = (DISPPARAMS FAR*)wParam;
	VARIANT_BOOL* pbCancel = pDispParams->rgvarg[3].pboolVal;
	*pbCancel = VARIANT_TRUE;

	// 2.新建一个页面,导航到需要跳转的页面
	if(pDispParams && ((pDispParams->rgvarg[0].vt & VT_BSTR) == VT_BSTR))
	{
		// VT_BSTR类型字符串用下面的方法转换
		WCHAR szUrl[MAX_URL];
		int len = wcslen(pDispParams->rgvarg[0].bstrVal);               
		wcsncpy(szUrl, pDispParams->rgvarg[0].bstrVal, MAX_URL - 5);
		InsertExplorerTab(-1, L"", szUrl);
	}

	return S_OK;
}

// IE浏览器页面加载进度更新事件
LRESULT CDuiHandlerMain::OnDuiMsgWebIEProgressChange(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl == NULL)
	{
		return S_OK;
	}

	TabItemInfo* pTabInfo = GetTabInfoByWebCtrlId(uID);
	if(pTabInfo == NULL)
	{
		return S_OK;
	}

	// 如果不是当前页面的消息则不处理
	CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
	if((pWebControlIE == NULL) || ((CDuiWebBrowserCtrl*)GetTabWebIEControl(pTabInfo->pControl) != pWebControlIE))
	{
		return S_OK;
	}

	// 获取进度条控件
	CDuiProgress* pProgress = (CDuiProgress*)GetControl(_T("progress.webload"));
	if(pProgress == NULL)
	{
		return S_OK;
	}

	DISPPARAMS FAR* pDispParams = (DISPPARAMS FAR*)wParam;
	int nCurProgress = pDispParams->rgvarg[1].lVal;
	int nMaxProgress = pDispParams->rgvarg[0].lVal;

	if ((nMaxProgress > 0) && (nCurProgress >= 0))
	{
		// 显示进度条
		pProgress->SetMaxProgress(nMaxProgress);
		pProgress->SetProgress(nCurProgress);
		pProgress->SetVisible(TRUE);
	}else
	{
		// nCurProgress为-1表示已经加载完成,需要隐藏进度条
		pProgress->SetVisible(FALSE);
	}

	return S_OK;
}

// wke浏览器事件
LRESULT CDuiHandlerMain::OnDuiMsgWebWkeEvent(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	TabItemInfo* pTabInfo = GetTabInfoByWebCtrlId(uID);
	if(pTabInfo == NULL)
	{
		return FALSE;
	}

	if(wParam == WKE_EVENT_TITLECHANGED)	// 页面标题变更
	{
		// 获取页面标题
		CString strTitle = (LPCTSTR)lParam;

		// 找对对应的浏览器控件,并设置对应的tab页签文字
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
		if(pTabCtrl)
		{
			pTabInfo->strText = strTitle;
			pTabCtrl->RefreshItems();
		}
	}else
	if(wParam == WKE_EVENT_URLCHANGED)	// URL变更
	{
		// 获取URL
		CString strUrl = (LPCTSTR)lParam;

		// 找对对应的浏览器控件,如果是当前页面,则刷新URL输入框内容
		CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
		if(pUrlCtrl && pTabCtrl && pTabInfo->pControl)
		{
			CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
			CDuiWkeView* pWebControlWke = GetCurTabWebWkeControl();
			if((pWebControlWke != NULL) && ((CDuiWkeView*)GetTabWebWkeControl(pTabInfo->pControl) == pWebControlWke))
			{
				pUrlCtrl->SetTitle(strUrl);
				// URL纪录写入注册表
				SaveExplorerUrls();
			}
		}
	}

	return TRUE;
}
