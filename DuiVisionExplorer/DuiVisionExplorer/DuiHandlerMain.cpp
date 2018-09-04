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

// ��ʼ��
void CDuiHandlerMain::OnInit()
{
	CRegistryUtil reg(HKEY_CURRENT_USER);
	CString strStartOpen = reg.GetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY, REG_EXPLORER_STARTOPEN);
	if(strStartOpen == L"last")	// ���ϴ��˳�ʱδ�رյ�ҳ��
	{
		// ��ȡ����������ҳǩURL
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

		// ����ҳ��
		for(int i=0; i<m_asUrl.GetSize(); i++)
		{
			InsertExplorerTab(1, L"", m_asUrl[i]);
		}
	}

	// ����������ʱ��
	m_uTimerAni = DuiSystem::AddDuiTimer(500);
}

// Ƥ����Ϣ����(ʵ��Ƥ���ı���ͻ�ȡ)
LRESULT CDuiHandlerMain::OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(Msg == MSG_GET_SKIN_TYPE)	// ��ȡSkin����
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		int nBkType = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE);
		*(int*)wParam = nBkType;
		return TRUE;
	}else
	if(Msg == MSG_GET_SKIN_VALUE)	// ��ȡSkinֵ
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
	if(Msg == MSG_SET_SKIN_VALUE)	// ����Skinֵ
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


// DUI��ʱ���¼�����
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}

// ���̼���Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// �����в���,���Զ������в������д���,Ҳ����ֱ����ʾ������
	DUI_INTERPROCESS_MSG* pInterMsg = (DUI_INTERPROCESS_MSG*)lParam;
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, L"ִ���������в���:" + strCmd);
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

// ��ȡTabҳ�е�IE������ؼ�
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

// ��ȡTabҳ�е�wke������ؼ�
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

// ��ȡTabҳ��ĳ���ؼ�ID��Ӧ��Tabҳ��Ϣ
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

// ��ȡ��ǰҳ���IE������ؼ�
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

// ��ȡ��ǰҳ���wke������ؼ�
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

// ���������ҳ��
void CDuiHandlerMain::InsertExplorerTab(int nIndex, CString strTitle, CString strUrl)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		// ���������
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
		// ����div,������tabҳ
		//pControlPanel->SetName(L"");
		pTabCtrl->AddControl(pControlPanel);
		pControlPanel->SetTitle(strTitle);
		pControlPanel->SetEnableScroll(FALSE);
		pControlPanel->SetPosStr(L"0,27,-0,-0");
		pControlPanel->OnPositionChange();
		CString strId;
		strId.Format(L"url-%d", nIndex);
		pTabCtrl->InsertItem(nIndex, 5000+nIndex, strId, strTitle, L"", 0, pControlPanel);

		// ����������ؼ�
		CDuiWebBrowserCtrl* pControlWeb = (CDuiWebBrowserCtrl*)DuiSystem::CreateControlByName(L"webbrowser", m_pDlg->GetSafeHwnd(), pControlPanel);
		pControlPanel->AddControl(pControlWeb);
		pControlWeb->SetDelayCreate(true);
		pControlWeb->Navigate(strUrl);
		pControlWeb->SetPosStr(L"0,47,-1,-1");
		
		pControlWeb->OnPositionChange();
	}
*/
}

// ����򿪵�����ҳ��URL��ע���
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
	// ����ע����е�URL�б�,����¼20��URL
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

// ��ʾϵͳ���öԻ���˵���Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMenuOption(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_option"), m_pDlg, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	// ���������
	CRegistryUtil reg(HKEY_CURRENT_USER);
	CString strWebType = reg.GetStringValue(NULL, REG_EXPLORER_SUBKEY, REG_EXPLORER_WEBTYPE);
	BOOL bWebTypeIE = (strWebType != L"wke");
	pDlg->SetControlValue(L"option.webtype.ie", L"check", bWebTypeIE ? L"true" : L"false");
	pDlg->SetControlValue(L"option.webtype.wke", L"check", !bWebTypeIE ? L"true" : L"false");

	// ����ʱ�򿪵�ҳ��
	CString strStartOpen = reg.GetStringValue(NULL, REG_EXPLORER_SUBKEY, REG_EXPLORER_STARTOPEN);
	pDlg->SetControlValue(L"option.startopen.last", L"check", (strStartOpen == L"last") ? L"true" : L"false");
	pDlg->SetControlValue(L"option.startopen.home", L"check", (strStartOpen == L"home") ? L"true" : L"false");
	pDlg->SetControlValue(L"option.startopen.none", L"check", (strStartOpen == L"none") ? L"true" : L"false");

	// Ĭ����������
	CString strSearchEngineType = reg.GetStringValue(NULL, REG_EXPLORER_SUBKEY, REG_EXPLORER_SEARCHTYPE);
	pDlg->SetControlValue(L"option.search.engine", L"value", strSearchEngineType);

	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	return TRUE;
}

// ϵͳ���öԻ����ȷ�ϰ�ť��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgOptionDlgOK(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = GetControlDialog(uID);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	CRegistryUtil reg(HKEY_CURRENT_USER);

	// ���������
	CDuiRadioButton* pControlWebTypeIE = static_cast<CDuiRadioButton*>(pDlg->GetControl(L"option.webtype.ie"));
	CString strWebType = pControlWebTypeIE->GetGroupValue();
	reg.SetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY, REG_EXPLORER_WEBTYPE, strWebType);

	// ����ʱ�򿪵�ҳ��
	CDuiRadioButton* pControlStartOpenLast = static_cast<CDuiRadioButton*>(pDlg->GetControl(L"option.startopen.last"));
	CString strStartOpen = pControlStartOpenLast->GetGroupValue();
	reg.SetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY, REG_EXPLORER_STARTOPEN, strStartOpen);

	// Ĭ����������
	CDuiComboBox* pSearchEngineCtrl = static_cast<CDuiComboBox*>(pDlg->GetControl(L"option.search.engine"));
	CString strSearchEngineType = pSearchEngineCtrl->GetComboValue();
	reg.SetStringValue(HKEY_CURRENT_USER, REG_EXPLORER_SUBKEY, REG_EXPLORER_SEARCHTYPE, strSearchEngineType);

	pDlg->DoOK();
	return TRUE;
}

// ��������һҳ
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

// ��������һҳ
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

// ˢ��webҳ��
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

// ��������ҳ
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

// URL�����ļ����¼�����(������س��¼�)
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
		// ���url����ؼ����ǽ���ؼ�,���˳�
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
		// ���û�д������������,��Ҫ�ȴ���
		InsertExplorerTab(-1, L"", L"");
		pWebControlIE = GetCurTabWebIEControl();
		pWebControlWke = GetCurTabWebWkeControl();
	}

	// �����������url
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

// URL�������¼�����(Ŀǰ������URL�����������б�ѡ���¼�)
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
		// ���û�д������������,��Ҫ�ȴ���
		InsertExplorerTab(-1, L"", L"");
		pWebControlIE = GetCurTabWebIEControl();
		pWebControlWke = GetCurTabWebWkeControl();
	}

	// �����������url
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

// ���������ļ����¼�����(������س��¼�)
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

	// ��ȡ��������������
	CString strSearch = pSearchEditCtrl->GetEditText();
	if(strSearch.IsEmpty())
	{
		return FALSE;
	}

	// ��ȡ������ؼ�
	CDuiWebBrowserCtrl* pWebControlIE = NULL;
	CDuiWkeView* pWebControlWke = NULL;
	if((pWebControlIE == NULL) && (pWebControlWke == NULL))
	{
		// ���û�д������������,��Ҫ�ȴ���
		InsertExplorerTab(-1, L"", L"");
		pWebControlIE = GetCurTabWebIEControl();
		pWebControlWke = GetCurTabWebWkeControl();
	}

	// ��ȡĬ����������
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

	// �����������url
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

// �½�ҳ��
LRESULT CDuiHandlerMain::OnDuiMsgButtonNewTab(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// �½�webҳ��
	InsertExplorerTab(-1, L"", L"");

	return TRUE;
}

// �л�ҳ��
LRESULT CDuiHandlerMain::OnDuiMsgTabCtrlSwitch(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
	if((pTabCtrl == NULL) || (pUrlCtrl == NULL))
	{
		return FALSE;
	}

	int nTabItem = wParam;

	// ˢ��������url
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

	// ����ҳ����ؽ�����
	CDuiProgress* pProgress = (CDuiProgress*)GetControl(_T("progress.webload"));
	if(pProgress != NULL)
	{
		pProgress->SetVisible(FALSE);
	}

	return TRUE;
}

// �ر�ҳ��
LRESULT CDuiHandlerMain::OnDuiMsgButtonCloseTab(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		// wParam������ʾtabҳ����
		pTabCtrl->DeleteItem(wParam);
		// URL��¼д��ע���
		SaveExplorerUrls();
	}

	return TRUE;
}

#define MAX_URL 256
// IE�����ҳ��ı������¼�
LRESULT CDuiHandlerMain::OnDuiMsgWebIETitleChange(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ��ȡҳ�����
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

	// �ҶԶ�Ӧ��������ؼ�,�����ö�Ӧ��tabҳǩ����
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	TabItemInfo* pTabInfo = GetTabInfoByWebCtrlId(uID);
	if(pTabInfo && pTabCtrl)
	{
		pTabInfo->strText = strTitle;
		pTabCtrl->RefreshItems();
	}

	return TRUE;
}

// IE�������������¼�
LRESULT CDuiHandlerMain::OnDuiMsgWebIENavigateComplete(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	TabItemInfo* pTabInfo = GetTabInfoByWebCtrlId(uID);
	if(pTabInfo == NULL)
	{
		return FALSE;
	}

	// ��ȡURL
	DISPPARAMS FAR* pDispParams = (DISPPARAMS FAR*)wParam;
	if(pDispParams && (pDispParams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT)))
	{
		// VT_BYREF|VT_VARIANT�����ַ���������ķ���ת��
		CComVariant varURL(*pDispParams->rgvarg[0].pvarVal);
		varURL.ChangeType(VT_BSTR);
		CString strUrl = OLE2T(varURL.bstrVal);

		// �ҶԶ�Ӧ��������ؼ�,����ǵ�ǰҳ��,��ˢ��URL���������
		CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
		if(pUrlCtrl && pTabCtrl && pTabInfo->pControl)
		{
			CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
			if((pWebControlIE != NULL) && ((CDuiWebBrowserCtrl*)GetTabWebIEControl(pTabInfo->pControl) == pWebControlIE))
			{
				pUrlCtrl->SetTitle(strUrl);
				// URL��¼д��ע���
				SaveExplorerUrls();
			}
		}
	}

	return TRUE;
}

// IE������´����¼�
LRESULT CDuiHandlerMain::OnDuiMsgWebIENewWindow(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 1.��ֹIE������Լ�����Ϊ,����pbCancel����ΪVARIANT_TRUE������ֹ
	DISPPARAMS FAR* pDispParams = (DISPPARAMS FAR*)wParam;
	VARIANT_BOOL* pbCancel = pDispParams->rgvarg[3].pboolVal;
	*pbCancel = VARIANT_TRUE;

	// 2.�½�һ��ҳ��,��������Ҫ��ת��ҳ��
	if(pDispParams && ((pDispParams->rgvarg[0].vt & VT_BSTR) == VT_BSTR))
	{
		// VT_BSTR�����ַ���������ķ���ת��
		WCHAR szUrl[MAX_URL];
		int len = wcslen(pDispParams->rgvarg[0].bstrVal);               
		wcsncpy(szUrl, pDispParams->rgvarg[0].bstrVal, MAX_URL - 5);
		InsertExplorerTab(-1, L"", szUrl);
	}

	return S_OK;
}

// IE�����ҳ����ؽ��ȸ����¼�
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

	// ������ǵ�ǰҳ�����Ϣ�򲻴���
	CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
	if((pWebControlIE == NULL) || ((CDuiWebBrowserCtrl*)GetTabWebIEControl(pTabInfo->pControl) != pWebControlIE))
	{
		return S_OK;
	}

	// ��ȡ�������ؼ�
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
		// ��ʾ������
		pProgress->SetMaxProgress(nMaxProgress);
		pProgress->SetProgress(nCurProgress);
		pProgress->SetVisible(TRUE);
	}else
	{
		// nCurProgressΪ-1��ʾ�Ѿ��������,��Ҫ���ؽ�����
		pProgress->SetVisible(FALSE);
	}

	return S_OK;
}

// wke������¼�
LRESULT CDuiHandlerMain::OnDuiMsgWebWkeEvent(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	TabItemInfo* pTabInfo = GetTabInfoByWebCtrlId(uID);
	if(pTabInfo == NULL)
	{
		return FALSE;
	}

	if(wParam == WKE_EVENT_TITLECHANGED)	// ҳ�������
	{
		// ��ȡҳ�����
		CString strTitle = (LPCTSTR)lParam;

		// �ҶԶ�Ӧ��������ؼ�,�����ö�Ӧ��tabҳǩ����
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
		if(pTabCtrl)
		{
			pTabInfo->strText = strTitle;
			pTabCtrl->RefreshItems();
		}
	}else
	if(wParam == WKE_EVENT_URLCHANGED)	// URL���
	{
		// ��ȡURL
		CString strUrl = (LPCTSTR)lParam;

		// �ҶԶ�Ӧ��������ؼ�,����ǵ�ǰҳ��,��ˢ��URL���������
		CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
		CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
		if(pUrlCtrl && pTabCtrl && pTabInfo->pControl)
		{
			CDuiWebBrowserCtrl* pWebControlIE = GetCurTabWebIEControl();
			CDuiWkeView* pWebControlWke = GetCurTabWebWkeControl();
			if((pWebControlWke != NULL) && ((CDuiWkeView*)GetTabWebWkeControl(pTabInfo->pControl) == pWebControlWke))
			{
				pUrlCtrl->SetTitle(strUrl);
				// URL��¼д��ע���
				SaveExplorerUrls();
			}
		}
	}

	return TRUE;
}
