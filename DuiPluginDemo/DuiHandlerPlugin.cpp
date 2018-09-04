#include "StdAfx.h"
#include "DuiHandlerPlugin.h"

#define XML_ABOUT_DLG _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<dlg name=\"dlg_about\" title=\"MsgBox\" width=\"450\" height=\"230\" appwin=\"0\" >\
	<base>\
		<imgbtn name=\"button.close\" pos=\"-45,0,-0,29\" skin=\"IDB_BT_CLOSE\" shortcut=\"ESC\"/>\
		<text name=\"title\" crtext=\"FFFFFF\" crmark=\"800000\" font=\"big\"\
				pos=\"10,5,200,25\" title=\"����[APP_NAME]-���\" mask=\"[APP_NAME]\" response=\"0\" />\
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

// ��ʼ��
void CDuiHandlerPlugin::OnInit()
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiHandlerPlugin::OnInit"));

	// ����������ʱ��
	m_uTimerAni = DuiSystem::AddDuiTimer(500);

	// ��ʼ��ԭ���ؼ�
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

// DUI��ʱ���¼�����
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
		// ��ʱ��ʾ��ʾ��
		DuiSystem::AddDuiNotifyMsgTask(_T("�����ɶ�ʱ����������ʾ��\n�������ڣ�20��\n�˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 5000);
	}
}

// ��ʾ��ʾ����1��Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgNotifyButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ1��\n�����ʾ���񣬴��ڴ�С����Ϊ400*250���˴���5���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T("�Զ������"), 5000, 400, 250);
	return TRUE;
}

// ��ʾ��ʾ����2��Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgNotifyButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiNotifyMsgTask(_T("��ʾ��Ϣ2��\n�����ʾ���񣬴��ڴ�С����Ϊ300*200���˴���3���Ӻ��Զ��ر�..."), MB_ICONINFORMATION | 2, _T(""), 3000, 300, 200);
	return TRUE;
}

// ��ʾ��ʾ����3��Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgNotifyButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
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
LRESULT CDuiHandlerPlugin::OnDuiMsgMsgBoxButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���1��Ĭ�ϵĶԻ���\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ��Ĭ�ϵ�ȷ��(MB_OK)��ť\n3.ͼ��ʹ��Ĭ�ϵ���Ϣͼ��"));
	return TRUE;
}

// ��ʾ��Ϣ�Ի���2��Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgMsgBoxButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���2��\n1.����ʹ���Զ������\n2.��ťʹ��ȷ��ȡ��(MB_OKCANCEL)��ť\n3.ͼ��ʹ�ô���ͼ��"), _T("�Զ������"), MB_OKCANCEL|MB_ICONERROR);
	return TRUE;
}

// ��ʾ��Ϣ�Ի���3��Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgMsgBoxButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ�Ի���3��\n1.����ʹ�ó���Ĭ�ϱ���\n2.��ťʹ���Ƿ�ȡ��(MB_YESNOCANCEL)��ť\n3.ͼ��ʹ�ü��ͼ��\n4.�Զ���Ի���Ĵ�С"), _T(""), MB_YESNOCANCEL, 370, 200);
	return TRUE;
}

// ������ʾ��ʱ����Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgTimerButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::AddDuiTimer(20000, _T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ������������ʱ����20�룡"));
	return TRUE;
}

// ֹͣ��ʾ��ʱ����Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgTimerButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::RemoveDuiTimer(_T("timer_notify"));
	DuiSystem::DuiMessageBox(NULL, _T("��ʾ��ʱ����ֹͣ��"));
	return TRUE;
}

// ��ʾXML�Ի�����Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgXmlDlgButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ��ʾͨ�������XML���ݼ��ش���
	CString strDlgXmlContent = XML_ABOUT_DLG;
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(strDlgXmlContent, NULL, _T(""), TRUE, 0, TRUE);
	if(pDlg != NULL)
	{
		int nResponse = pDlg->DoModal();
		DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	}
	return TRUE;
}

// �˵�1��Ϣ����
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
	// ��ʾ����ڲ˵�����ʱ����Ĳ˵������ʾ���⡢�ɼ��ԡ��Ƿ���á��Ƿ�ѡ�������
	// �����ڵ���LoadXmlFile֮ǰͨ���˵���������������Ӧ�˵��������
	pDuiMenu->SetItemTitle(_T("item_login"), _T("��֤--�޸�Title"));
	pDuiMenu->SetItemCheck(_T("item_setup"), 0);
	pDuiMenu->SetItemVisible(_T("item_help"), FALSE);
	pDuiMenu->SetItemDisable(_T("item_about"), TRUE);
	if(pDuiMenu->LoadXmlFile(_T("menu_tray"), NULL, point, WM_DUI_MENU))
	{
		pDuiMenu->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

// �˵�2��Ϣ����
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

// �б�ؼ������Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgListCtrl1Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ������б�ؼ�ĳһ��,��ʾ���е���ϸ��Ϣ,���������wParam��ʾ�ؼ��к�
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

// �б�ؼ������Ϣ����
LRESULT CDuiHandlerPlugin::OnDuiMsgListCtrl2Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ������б�ؼ�ĳһ��
	// ���������wParam��ʾ�ؼ��к�,lParam��ʾ��������ӵ�����(����1��2�ֱ�Ϊ0��1)
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(m_pPanel && pRowInfo)
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
LRESULT CDuiHandlerPlugin::OnDuiMsgListCtrl2DblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ˫�����б�ؼ�ĳһ��,���������wParam��ʾ�ؼ��к�
	CDuiListCtrl* pListCtrl = (CDuiListCtrl*)GetControl(_T("listctrl_2"));
	ListRowInfo* pRowInfo = pListCtrl->GetItemInfo(wParam);
	if(m_pPanel && pRowInfo)
	{
		CString strMsg;
		strMsg.Format(_T("���˫�����б��е���\n�кţ�%d\n�����ݣ�%s"), wParam, pRowInfo->strTitle);
		DuiSystem::DuiMessageBox(NULL, strMsg);
	}
	return TRUE;
}
