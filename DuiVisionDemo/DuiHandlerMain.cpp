#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"


#define XML_ABOUT_DLG _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<dlg name=\"dlg_about\" title=\"MsgBox\" width=\"450\" height=\"230\" appwin=\"0\" >\
	<base>\
		<imgbtn name=\"button.close\" pos=\"-45,0,-0,29\" skin=\"IDB_BT_CLOSE\" shortcut=\"ESC\"/>\
		<text name=\"title\" crtext=\"FFFFFF\" crmark=\"800000\" font=\"big\"\
				pos=\"10,5,200,25\" title=\"����[APP_NAME]\" mask=\"[APP_NAME]\" response=\"0\" />\
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
		<text crtext=\"000000\" pos=\"170,65,-25,85\" title=\"��Ȩ����2013-2014\" />\
		<linkbtn name=\"linkbtn1\" crtext=\"800000\"\
			pos=\"170,100,-25,130\" title=\"���������\" href=\"http://www.blueantstudio.net\" />\
		<text crtext=\"000080\" pos=\"170,140,-25,160\" title=\"�˶Ի����ɶ����XML���ݴ���\" />\
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

// ��ʼ��
void CDuiHandlerMain::OnInit()
{
	// ��ʼ������ͼ��
	DuiSystem::Instance()->InitTray();
	// ��tabҳ3ע���¼��������
	//DuiSystem::RegisterHandler(m_pDlg, new CDuiHandlerTab3(), _T("tab3"));
	// ������ҳע���¼��������
	//CDuiHandlerTools* pDuiHandlerTools = new CDuiHandlerTools();
	//pDuiHandlerTools->SetDialog(m_pDlg);
	//DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerTools, _T("tab.tools"));
	//pDuiHandlerTools->OnInit();
	// ����������ʱ��
	m_uTimerAni = DuiSystem::AddDuiTimer(500);

	// ��ʼ��ԭ���ؼ�
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
		pWndList->InsertColumn(0, L"��1", LVCFMT_CENTER, 50);
		pWndList->InsertColumn(1, L"��2", LVCFMT_CENTER, 150);
		pWndList->InsertColumn(2, L"��3", LVCFMT_CENTER, 100);
		pWndList->SetItemHeight(22);
		int nItem = pWndList->AppendTitle("row1", Color(80,80,80), LISTITEM_EXPANDABLE|LISTITEM_BOLD);
		pWndList->AppendSubItem(nItem, "test1", SUBITEM_LINK);
		pNativeWnd->SetNativeWnd(pWndList);*/
	}

	// ��ʾ�б�ؼ�ͨ��API����ָ�����д��ڿɼ���Χ
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_1"));
	if(pListCtrl)
	{
		pListCtrl->EnsureVisible(10, TRUE);
	}

	// ��ʾ���ؼ�ͨ��API�����ӿؼ�
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	if(pGridCtrl)
	{
		CLinkButton* pControl = (CLinkButton*)DuiSystem::CreateControlByName(_T("linkbtn"), NULL, NULL);
		if(pControl)
		{
			pControl->SetName(_T("grid1_updateinfo_link"));
			pControl->SetPosStr(_T("100, 3, -10, 25"));
			pControl->SetTitle(_T("��������"));
			pControl->SetLink(_T("http://www.blueantstudio.net"));
			pGridCtrl->AddSubItemControl(1, 0, pControl);
		}
	}

	// ��ʾͨ��API���gridctrl��
	if(pGridCtrl)
	{
		for(int i=0; i<10; i++)
		{
			CString strId;
			strId.Format(_T("id_%d"), i);
			int nRow = pGridCtrl->InsertRow(-1,	// ����������,-1��ʾ��ӵ����
				strId,							// ��id�ַ���
				-1,								// �����ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
				Color(0, 0, 0, 0),				// ��������ɫ,ȫ0��ʾĬ��(��ʹ����������ɫ,ʹ�ñ��ȫ����ɫ)
				_T("skins\\icon\\scriptnet.png"),	// ������ͼƬ�ļ�
				-1,								// ���Ҳ�ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
				_T(""),							// ���Ҳ��ͼƬ�ļ�
				(i % 2));						// �����ļ���״̬(-1��ʾ����ʾ����)
			CString strText;
			strText.Format(_T("������%d"), i);
			CString strContent;
			strContent.Format(_T("����˵��%d"), i);
			pGridCtrl->SetSubItem(nRow, 0, strText, strContent, TRUE);
			pGridCtrl->SetSubItem(nRow, 1, _T("��װ��1.0.0.1"), _T("���£�2.3.0.1"));
			pGridCtrl->SetSubItem(nRow, 2, _T("11M"));
			pGridCtrl->SetSubItem(nRow, 3, _T("��װ"));
		}
	}

	// ��ʾ���ؼ�ͨ��API�����ӿؼ�
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	if(pTreeCtrl)
	{
		HTREEITEM hNode = pTreeCtrl->GetNodeById(_T("1-0"));
		CLinkButton* pControl = (CLinkButton*)DuiSystem::CreateControlByName(_T("linkbtn"), NULL, NULL);
		if(pControl)
		{
			pControl->SetName(_T("tree1_updateinfo_link"));
			pControl->SetPosStr(_T("10, 5, -10, 25"));
			pControl->SetTitle(_T("��������"));
			pControl->SetLink(_T("http://www.blueantstudio.net"));
			pTreeCtrl->AddSubItemControl(hNode, 2, pControl);
		}
	}

	// ��ʾ���ؼ�ָ��ĳ���ڵ��ڿɼ���Χ��
	pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	if(pTreeCtrl)
	{
		HTREEITEM hNode = pTreeCtrl->GetNodeById(_T("2-7-1-1-1"));
		pTreeCtrl->EnsureVisible(hNode, TRUE);
	}

	// ��ʾ��div�ж�̬����ӿؼ�(��ӵ������ؼ��ı༭��ҳ��)
	CControlBase* pDiv = (CControlBase*)GetControl(_T("layout-1"));
	if(pDiv)
	{
		CDuiEdit* pControlEdit = static_cast<CDuiEdit*>(DuiSystem::CreateControlByName(_T("edit"), m_pDlg->GetSafeHwnd(), pDiv));
		if(pControlEdit)
		{
			pDiv->AddControl(pControlEdit);
			pControlEdit->SetName(_T("btnName"));
			pControlEdit->SetTitle(_T("��̬��ӵı༭��ؼ�"));
			pControlEdit->SetPosStr(_T("50,220,250,250"));
			pControlEdit->OnAttributeSkin(_T("skin:IDB_EDIT"), FALSE);
			pControlEdit->OnAttributeLeftImage(_T("skin:IDB_COMPUTER"), FALSE);
			pControlEdit->OnPositionChange();
		}
	}
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

// ����˫����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTrayIconDClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::ShowDuiDialog(_T("dlg_login"), NULL);
	return TRUE;
}

// �������������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTrayIconLButtonDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//DuiSystem::ShowDuiDialog(_T("dlg_about"), NULL);
	return TRUE;
}

// ��ʾ��ʾ����1��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgNotifyButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ1��\n�����ʾ���񣬴��ڴ�С����Ϊ400*250���˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T("�Զ������"), 5000, 400, 250);
	return TRUE;
}

// ��ʾ��ʾ����2��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgNotifyButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ2��\n�����ʾ���񣬴��ڴ�С����Ϊ300*200���˴���3���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 3000, 300, 200);
	return TRUE;
}

// ��ʾ��ʾ����3��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgNotifyButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// �Զ����Notify��ʾ����
	DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));
	// ���ñ��������
	DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.title"), _T("�Զ�����ʾ����"));
	DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.text"), _T("������ʽ��ʾ���ڣ����Զ��رգ���СΪ250*300����ʾȷ����ť��................."));
	// ����ͼ��
	DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_INFO"));
	// ��������,5������,��Ҫ�Ѳ��õ��������ص�
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.0"), TRUE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.1"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.2"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.3"), FALSE);
	DuiSystem::SetNotifyMsgBoxControlVisible(_T("notify.type.4"), FALSE);
	// ���ô�С
	DuiSystem::SetNotifyMsgBoxSize(250, 300);
	// ��ʾ����
	DuiSystem::ShowNotifyMsgBox();
	return TRUE;
}

// ��ʾ��Ϣ�Ի���1��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMsgBoxButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���1��Ĭ�ϵĶԻ���\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ��Ĭ�ϵ�ȷ��(MB_OK)��ť\n3.ͼ��ʹ��Ĭ�ϵ���Ϣͼ��"));
	return TRUE;
}

// ��ʾ��Ϣ�Ի���2��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMsgBoxButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���2��\n1.����ʹ���Զ������\n2.��ťʹ��ȷ��ȡ��(MB_OKCANCEL)��ť\n3.ͼ��ʹ�ô���ͼ��"), _T("�Զ������"), MB_OKCANCEL|MB_ICONERROR);
	return TRUE;
}

// ��ʾ��Ϣ�Ի���3��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMsgBoxButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���3��\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ���Ƿ�ȡ��(MB_YESNOCANCEL)��ť\n3.ͼ��ʹ�ü��ͼ��\n4.�Զ���Ի���Ĵ�С"), _T(""), MB_YESNOCANCEL, 370, 200);
	return TRUE;
}

// ������ʾ��ʱ����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTimerButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiTimer(20000, _T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ������������ʱ����20�룡"));
	return TRUE;
}

// ֹͣ��ʾ��ʱ����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTimerButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::RemoveDuiTimer(_T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ����ֹͣ��"));
	return TRUE;
}

// ��ʾXML�Ի�����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgXmlDlgButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ��ʾͨ�������XML���ݼ��ش���
	CString strDlgXmlContent = XML_ABOUT_DLG;
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(strDlgXmlContent, GetControlDialog(uID), _T(""), TRUE, 0, TRUE);
	if(pDlg != NULL)
	{
		int nResponse = pDlg->DoModal();
		DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	}
	return TRUE;
}

// ������tabҳ�е�ĳ��ҳ����Ϣ����
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

// ��ʾ��tabҳ�е�ĳ��ҳ����Ϣ����
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

// ɾ��Webҳ��Tabҳ��Ϣ����
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

// ����Webҳ��Tabҳ��Ϣ����
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

// �˵�1��Ϣ����
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
	// ��ʾ����ڲ˵�����ʱ����Ĳ˵������ʾ���⡢�ɼ��ԡ��Ƿ���á��Ƿ�ѡ�������
	// �����ڵ���LoadXmlFile֮ǰͨ���˵���������������Ӧ�˵��������
	pDuiMenu->SetItemTitle(_T("item_login"), _T("��֤--�޸�Title"));
	pDuiMenu->SetItemCheck(_T("item_setup"), 0);
	pDuiMenu->SetItemVisible(_T("item_help"), FALSE);
	pDuiMenu->SetItemDisable(_T("item_about"), TRUE);
	if(pDuiMenu->LoadXmlFile(_T("menu_tray"), pDlg, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// �˵�2��Ϣ����
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

// �б�ؼ������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgListCtrl1Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ������б�ؼ�ĳһ��,��ʾ���е���ϸ��Ϣ,���������wParam��ʾ�ؼ��к�
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

// �б�ؼ������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgListCtrl2Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ������б�ؼ�ĳһ��
	// ���������wParam��ʾ�ؼ��к�,lParam��ʾ��������ӵ�����(����1��2�ֱ�Ϊ0��1)
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(pDlg && pRowInfo)
	{
		if(lParam != -1)
		{
			// ������е�ĳ������
			CString strMsg;
			strMsg.Format(_T("������б��е�����\n�����ݣ�%s\n���������ţ�%d"), pRowInfo->strTitle, lParam);
			DuiSystem::DuiMessageBox(NULL, strMsg);
		}
	}

	return TRUE;
}

// �б�ؼ�˫����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgListCtrl2DblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ˫�����б�ؼ�ĳһ��,���������wParam��ʾ�ؼ��к�
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(pDlg && pRowInfo)
	{
		CString strMsg;
		strMsg.Format(_T("���˫�����б��е���\n�кţ�%d\n�����ݣ�%s"), wParam, pRowInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}

// ���ؼ�˫����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgGridCtrlDblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ˫���˱��ؼ�ĳһ��,���������wParam��ʾ�ؼ��к�
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	GridItemInfo* pItemInfo = pGridCtrl->GetItemInfo(wParam, lParam);
	if(pItemInfo)
	{
		CString strMsg;
		strMsg.Format(_T("���˫���˱���еĵ�Ԫ��\n�кţ�%d\n�кţ�%d\n�����ݣ�%s"), wParam, lParam, pItemInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}

// ���ؼ���ɾ����ť�ӿؼ������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgGridCtrlDelBtnClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ����˱��ؼ���ɾ���а�ť�ӿؼ�
	CDlgBase* pDlg = GetControlDialog(uID);
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	pGridCtrl->DeleteRow(3);
	
	return TRUE;
}

// ��ק�ļ������ؼ���Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgGridCtrlDropFile(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ��ק��Ϣ��wParam������λ�ã�lParam��ʾ��ǰ��ק���ļ�ȫ·����
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_1"));
	CPoint* pptDropFile = (CPoint*)wParam;
	CString* pstrDropFile = (CString*)lParam;
	// ��ȡ�ļ���
	CString strFileName = *pstrDropFile;
	int nPos = strFileName.ReverseFind(_T('\\'));
	strFileName.Delete(0, nPos+1);
	// �ڱ���в���һ���ļ���Ϣ
	if(pGridCtrl)
	{
		CString strId = *pstrDropFile;
		int nRow = pGridCtrl->InsertRow(-1,	// ����������,-1��ʾ��ӵ����
			strId,							// ��id�ַ���
			-1,								// �����ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
			Color(0, 0, 0, 0),				// ��������ɫ,ȫ0��ʾĬ��(��ʹ����������ɫ,ʹ�ñ��ȫ����ɫ)
			_T("skins\\icon\\NewIcons005.png"),	// ������ͼƬ�ļ�
			-1,								// ���Ҳ�ͼƬ(����ͼƬ��ʽ,������ͼƬ��-1)
			_T(""),							// ���Ҳ��ͼƬ�ļ�
			0);
		pGridCtrl->SetSubItem(nRow, 0, strFileName, *pstrDropFile, TRUE);
		pGridCtrl->SetSubItem(nRow, 1, _T("�ļ�"));
	}

	return TRUE;
}

// ���ؼ������Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTreeCtrlClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	HTREEITEM hNode = (HTREEITEM)wParam;	// ���ڵ���	
	int nItem = lParam;	// ��������ؼ��ĵڼ���
	if(nItem == 0)
	{
		// ��������һ��,��չ���������˽ڵ�
		pTreeCtrl->ToggleNode(hNode);
	}
	return TRUE;
}

// ���ؼ�˫����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTreeCtrlDblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ˫�������ؼ�ĳһ�ڵ�,���������wParam��ʾ�ؼ��ڵ�����
	CDuiTreeCtrl* pTreeCtrl = (CDuiTreeCtrl*)GetControl(_T("treectrl_1"));
	HTREEITEM hNode = (HTREEITEM)wParam;	// ���ڵ���
	int nItem = lParam;	// ��������ؼ��ĵڼ���
	TreeItemInfo* pItemInfo = pTreeCtrl->GetItemInfo(hNode, nItem);
	if(pItemInfo)
	{
		CString strMsg;
		strMsg.Format(_T("���˫�������ڵ�\n�ڵ����ݣ�%s"), pItemInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}

// ��ʾϵͳ���öԻ���˵���Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgMenuOption(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_option"), m_pDlg, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	// IP��ַ
	pDlg->SetControlValue(_T("config.server.ip"), _T("title"), _T("192.168.1.1"));

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

	// IP��ַ
	CString strServerAddr = _T("");
	CDuiEdit* pControlIp = static_cast<CDuiEdit*>(pDlg->GetControl(_T("config.server.ip")));
	if(pControlIp)
	{
		strServerAddr = pControlIp->GetEditText();
		if(strServerAddr == _T(""))
		{
			// ��ַ����Ϊ����ʾ
			return TRUE;
		}
	}

	pDlg->DoOK();
	return TRUE;
}

// ��ʾFlash1��ť��Ϣ����
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

// ��ʾFlash2��ť��Ϣ����
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

// ��ʾFlash3��ť��Ϣ����
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

// ��ʾFlash4��ť��Ϣ����
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

// Tabҳǩ�رհ�ť��Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTabCtrlClose(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

// image-normal�ؼ���������������Ϣ����
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

// image-normal�ؼ����������ſ���Ϣ����
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

// image-normal�ؼ�������ƶ���Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgImageNormalMouseMove(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiPicture* pImgCtrl = (CDuiPicture*)GetControl(_T("image-normal"));
	if(pImgCtrl == NULL)
	{
		return FALSE;
	}

	// ����϶��ؼ�
	if(m_bMouseDownImageNormal)
	{
		CPoint ptNew = *((CPoint*)lParam);	// ��ǰ���λ��
		CControlBase* pParentCtrl = (CControlBase*)(pImgCtrl->GetParent());
		if(pParentCtrl)
		{
			CRect rcParent = pParentCtrl->GetRect();
			if(!rcParent.PtInRect(ptNew))
			{
				// ������λ���Ѿ����ڸ��ؼ���Χ�����˳�
				return FALSE;
			}
		}

		// ��ȡ�ؼ��ĵ�ǰλ�ú���굱ǰλ������һ��λ�õĲ�ֵ,���ؼ���ǰλ�ü������λ�õĲ�ֵ
		CRect rc = pImgCtrl->GetRect();
		CSize offset = ptNew - m_ptControlImageNormal;
		rc.OffsetRect(offset);
		pImgCtrl->SetRect(rc);

		// ˢ�������һ��λ�õı���
		m_ptControlImageNormal = *((CPoint*)lParam);

		// ˢ�¿ؼ�
		pImgCtrl->UpdateControl(true);
	}

	return TRUE;
}

// ����ͷ�ı��ؼ��Ҽ��ſ���Ϣ����,��ʾ�����Ҽ��˵�
LRESULT CDuiHandlerMain::OnDuiMsgGridCtrlHeaderRButtonUp(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ���ؼ�ĳһ��,���������wParam��ʾ����к�,lParam��ʾ�к�
	CDuiGridCtrl* pGridCtrl = (CDuiGridCtrl*)GetControl(_T("gridctrl_header"));
	GridItemInfo* pItemInfo = pGridCtrl->GetItemInfo(wParam, lParam);
	if(pItemInfo)
	{
		// ��ʾ�Ҽ��˵�
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		pDuiMenu->SetAutoClose(FALSE);
		pDuiMenu->SetParent(pGridCtrl);
		CPoint point;
		CRect rc = pItemInfo->rcItem;
		// ����˵�����ʾλ��
		point.SetPoint(rc.left + rc.Width() / 2, rc.bottom);

		CDlgBase* pParentDlg = GetControlDialog(uID);

		// ����ת��Ϊ��Ļ����
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
// DUI�¼�����
LRESULT CDuiHandlerMain::OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiHandlerMain::OnDuiMessage:uID=%d, name=%s, msg=%d, wParam=%d, lParam=%d"),
						uID, strName, Msg, wParam, lParam);

    return 0;
}
*/
// DUI��ʱ���¼�����
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
		// ��ʱ��ʾ��ʾ��
		DuiSystem::AddDuiNotifyMsgTask(_T("�����ɶ�ʱ����������ʾ��\n�������ڣ�20��\n�˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 5000);
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
		DuiSystem::DuiMessageBox(NULL, _T("ִ���������в���:") + strCmd);
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
