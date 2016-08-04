#include "stdafx.h"
#include "CDuiPlugin.h"
#include "DuiHandlerPlugin.h"

/////////////////////////////////////////////////////////////////////////////
// ��ȡƽ̨·��
/////////////////////////////////////////////////////////////////////////////
CString GetPlatRootPath()
{
	//��ȡƽ̨·��
	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	::GetModuleFileName(NULL, szFullPath, MAX_PATH);
#ifdef _UNICODE
	_wsplitpath(szFullPath, szdrive, szdir, NULL, NULL);
#else
	_splitpath(szFullPath, szdrive, szdir, NULL, NULL);
#endif
	CString szPath;
	szPath.Format(_T("%s%s"), szdrive, szdir);
	szPath = szPath.Left(szPath.GetLength() - 1);
	int nPos = szPath.ReverseFind('\\');
	if(nPos >= 0)
	{
		szPath = szPath.Left(nPos+1);
	}

	return szPath;
}

/////////////////////////////////////////////////////////////////////////////
// ��ȡ���·��
/////////////////////////////////////////////////////////////////////////////
CString GetPlugInRootPath()
{
	//��ȡƽ̨·��
	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	::GetModuleFileName(GetDllInstance(), szFullPath, MAX_PATH);
#ifdef _UNICODE
	_wsplitpath(szFullPath, szdrive, szdir, NULL, NULL);
#else
	_splitpath(szFullPath, szdrive, szdir, NULL, NULL);
#endif
	CString szPath;
	szPath.Format(_T("%s%s"), szdrive, szdir);

	return szPath;
}

//////////////////////////////////////////////////////////////////////////
// ����ʵ��
//////////////////////////////////////////////////////////////////////////
CDuiPlugin::CDuiPlugin()
{
	m_pDuiPanel = NULL;
}

CDuiPlugin::~CDuiPlugin()
{
	// ɾ��DuiPanel����
	if(m_pDuiPanel)
	{
		delete m_pDuiPanel;
		m_pDuiPanel = NULL;
	}

	// ������в�������Ѿ��ͷ���,��DuiVision������DuiSystem�����е���Դ���ͷ�
	IPlatUI* pIPlatUI = (IPlatUI*)getIPlatUI();
	if(pIPlatUI)
	{
		CPtrArray auVciObj;
		int nObjCount = pIPlatUI->GetVciObjects("org.vci.duivision", auVciObj);
		if(nObjCount == 1)
		{
			// ��Script.NETƽ̨��ȡDuiVision�������ʵ���ĸ���,���ֻ��һ��,˵�������һ��ʵ��,��Ҫִ��DuiSystem���ͷ�
			DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiPlugin release, release DuiSystem instance"));
			DuiSystem::Release();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// ��װ�ص�����
/////////////////////////////////////////////////////////////////////////////
int CDuiPlugin::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
{
	CVisualComponent::InstallHandler(nEventID, lpHandler, dwRefData);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ж�ػص�����
/////////////////////////////////////////////////////////////////////////////
int CDuiPlugin::UninstallHandler(DWORD nEventID, LPVOID lpHandler)
{

	CVisualComponent::UninstallHandler(nEventID, lpHandler);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// ��Ϣ����
/////////////////////////////////////////////////////////////////////////////
int CDuiPlugin::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	return 0;
}

// ����xml�ļ�
BOOL CDuiPlugin::LoadXmlFile(CString strXmlFile)
{
	if(m_pDuiPanel == NULL)
	{
		return FALSE;
	}

	DuiXmlDocument xmlDoc;
	DuiXmlNode pDlgElem;
	DuiXmlNode pDlgBodyElem;
	DuiXmlNode pDlgBaseElem;
	DuiXmlNode pPopupElem;
	DuiXmlNode pMenuElem;

	if(!DuiSystem::Instance()->LoadXmlFile(xmlDoc, strXmlFile))
	{
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("CDuiPlugin::LoadXmlFile %s failed"), strXmlFile);
		return FALSE;
	}

	BOOL bRet = FALSE;
	// ������Ϊ�Ի�����м���
	pDlgElem = xmlDoc.child(_T("dlg"));//RootElement();
	if(pDlgElem)
	{
		pDlgBaseElem = pDlgElem.child(_T("base"));
		if(pDlgBaseElem)
		{
			pDlgBaseElem.set_name(_T("div"));
			bRet = m_pDuiPanel->Load(pDlgBaseElem);
		}

		pDlgBodyElem = pDlgElem.child(_T("body"));
		if(pDlgBodyElem)
		{
			pDlgBodyElem.set_name(_T("div"));
			bRet = m_pDuiPanel->Load(pDlgBodyElem);
		}
	}

	// ������Ϊpopup���м���
	if(!bRet)
	{
		pPopupElem = xmlDoc.child(_T("popup"));
		if(pPopupElem)
		{
			pPopupElem.set_name(_T("div"));
			bRet = m_pDuiPanel->Load(pPopupElem);
		}
	}

	// ������Ϊmenu���м���
	if(!bRet)
	{
		pMenuElem = xmlDoc.child(_T("menu"));
		if(pMenuElem)
		{
			pMenuElem.set_name(_T("div"));
			bRet = m_pDuiPanel->Load(pMenuElem);
		}
	}

	return bRet;
}

//{{VCI_IMPLEMENT_BEGIN
int CDuiPlugin::OnInit(UINT nIDTemplate, HWND hWnd, LPCTSTR lpszName, CRect rc, IDuiHostWnd* pIDuiHostWnd)
{
	// ����DuiSystem����һ��Panel�ؼ�����,������xml�ļ�
	TRACE(_T("CDuiPlugin::OnInit, name=%s, rc=%d,%d,%d,%d\n"), lpszName, rc.left, rc.top, rc.right, rc.bottom);
	USES_CONVERSION;
	
	// ���DuiSystem������û�д���,����Ҫ��ʼ��DuiVision��
	if(!DuiSystem::HaveInstance())
	{
		// ����DuiVision��ĸ�Ŀ¼
		DuiSystem::SetRootPath(GetPlugInRootPath());

		// ��ʼ��DUI��
		DWORD dwLangID = 0;
		UINT uAppID = 0;
		if(pIDuiHostWnd)
		{
			dwLangID = (DWORD)pIDuiHostWnd->GetCurrentLanguage();
			uAppID = (UINT)pIDuiHostWnd->GetAppID();
		}
		new DuiSystem(NULL, dwLangID, _T(""), uAppID, nIDTemplate, _T(""));

		DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiPlugin::OnInit, create DuiSystem instance, root path is %s"), GetPlugInRootPath());
	}

	// ���������xml�ļ������ǿ�,������ļ�
	CString strXmlFile = lpszName;
	if(!strXmlFile.IsEmpty())
	{
		// ����xml
		m_pDuiPanel = (CDuiPanel*)DuiSystem::CreateControlByName(_T("div"), hWnd, NULL);
		if(m_pDuiPanel)
		{
			// ����panel�ؼ����������ڹ��ܽӿ�ָ��
			m_pDuiPanel->SetIDuiHostWnd(pIDuiHostWnd);

			// �������panel����ע���¼��������
			CDuiHandlerPlugin* pHandler = new CDuiHandlerPlugin();
			pHandler->SetDuiPanel(m_pDuiPanel);
			DuiSystem::RegisterHandler(m_pDuiPanel, pHandler);
			// ���ز�������ļ�
			BOOL bRet = m_pDuiPanel->LoadXmlFile(lpszName);
			if(!bRet)
			{
				// �����Ϊdiv����ʧ��(xml�ļ����ӽڵ㲻��div�ڵ�),�����xml�ļ��Ƿ������Ϊ�Ի������
				bRet = LoadXmlFile(lpszName);
			}
			DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("CDuiPlugin::OnInit load %s %s"), lpszName, bRet ? _T("succ") : _T("fail"));
		}
	}

	return 0;
}

int CDuiPlugin::SetRect(CRect rc)
{
	TRACE(_T("CDuiPlugin::SetRect(%d,%d,%d,%d)\n"), rc.left, rc.top, rc.right, rc.bottom);
	if(m_pDuiPanel)
	{
		CString strPos;
		strPos.Format(_T("%d,%d,%d,%d"), rc.left, rc.top,rc.right, rc.bottom);
		m_pDuiPanel->SetPosStr(strPos);
		m_pDuiPanel->OnPositionChange();
	}

	return 0;
}

int CDuiPlugin::SetVisible(BOOL bIsVisible)
{
	TRACE(_T("CDuiPlugin::SetVisible(%d)\n"), bIsVisible);
	if(m_pDuiPanel)
	{
		m_pDuiPanel->SetVisible(bIsVisible);
	}

	return 0;
}

int CDuiPlugin::SetDisable(BOOL bIsDisable)
{
	TRACE(_T("CDuiPlugin::SetDisable(%d)\n"), bIsDisable);
	if(m_pDuiPanel)
	{
		m_pDuiPanel->SetDisable(bIsDisable);
	}

	return 0;
}

int CDuiPlugin::SetFocus(BOOL bFocus)
{
	TRACE(_T("CDuiPlugin::SetFocus(%d)\n"), bFocus);
	if(m_pDuiPanel)
	{
		m_pDuiPanel->OnFocus(bFocus);
	}

	return 0;
}

int CDuiPlugin::DrawControl(CDC &dc, CRect rcUpdate)
{
	//TRACE("CDuiPlugin::DrawControl(%d,%d,%d,%d)(%d,%d)\n", rcUpdate.left, rcUpdate.top, rcUpdate.right, rcUpdate.bottom);
	if(m_pDuiPanel)
	{
		m_pDuiPanel->Draw(dc, rcUpdate);
	}

	return 0;
}

int CDuiPlugin::OnTimer(UINT uTimerID, LPCTSTR lpszTimerName)
{
	if(m_pDuiPanel)
	{
		m_pDuiPanel->OnTimer();
	}
	return 0;
}

int CDuiPlugin::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnMessage(uID, uMsg, wParam, lParam);
	}
	return 0;
}

int CDuiPlugin::OnMousePointChange(CPoint& point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnMousePointChange(point);
	}
	return 0;
}

int CDuiPlugin::OnCheckMouseResponse(UINT nFlags, CPoint point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnCheckMouseResponse(nFlags, point);
	}
	return 0;
}

int CDuiPlugin::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnMouseMove(nFlags, point);
	}
	return 0;
}

int CDuiPlugin::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnLButtonDown(nFlags, point);
	}
	return 0;
}

int CDuiPlugin::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnLButtonUp(nFlags, point);
	}
	return 0;
}

int CDuiPlugin::OnScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnScroll(bVertical, nFlags, point);
	}
	return 0;
}

int CDuiPlugin::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnKeyDown(nChar, nRepCnt, nFlags);
	}
	return 0;
}

int CDuiPlugin::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnLButtonDblClk(nFlags, point);
	}
	return 0;
}

int CDuiPlugin::SetUpdate(BOOL bUpdate, COLORREF clr)
{
	if(m_pDuiPanel)
	{
		m_pDuiPanel->SetUpdate(bUpdate, clr);
	}
	return 0;
}

int CDuiPlugin::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnRButtonDown(nFlags, point);
	}
	return 0;
}

int CDuiPlugin::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnRButtonUp(nFlags, point);
	}
	return 0;
}

int CDuiPlugin::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_pDuiPanel)
	{
		return m_pDuiPanel->OnRButtonDblClk(nFlags, point);
	}
	return 0;
}
//}}VCI_IMPLEMENT_END

//////////////////////////////////////////////////////////////////////////
// �ӿ�ʵ��
//////////////////////////////////////////////////////////////////////////
INTERFACE_IMPLEMENT(DuiPlugin)
CONTROL_INTERFACE_IMPLEMENT(DuiPlugin)

//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
// ��ȡ�ӿڵĶ�����Ϣ
LPVOID __stdcall CDuiPlugin::XDuiPlugin::GetExtraInfo(LPVOID lpRefData)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj;
}
//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

//{{VCI_INTERFACE_IMPLEMENT_BEGIN
int __stdcall CDuiPlugin::XDuiPlugin::OnInit(UINT nIDTemplate, HWND hWnd, LPCTSTR lpszName, CRect rc, IDuiHostWnd* pIDuiHostWnd)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnInit(nIDTemplate, hWnd, lpszName, rc, pIDuiHostWnd);
}

int __stdcall CDuiPlugin::XDuiPlugin::SetRect(CRect rc)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->SetRect(rc);
}

int __stdcall CDuiPlugin::XDuiPlugin::SetVisible(BOOL bIsVisible)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->SetVisible(bIsVisible);
}

int __stdcall CDuiPlugin::XDuiPlugin::SetDisable(BOOL bIsDisable)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->SetDisable(bIsDisable);
}

int __stdcall CDuiPlugin::XDuiPlugin::SetFocus(BOOL bFocus)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->SetFocus(bFocus);
}

int __stdcall CDuiPlugin::XDuiPlugin::DrawControl(CDC &dc, CRect rcUpdate)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->DrawControl(dc, rcUpdate);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnTimer(UINT uTimerID, LPCTSTR lpszTimerName)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnTimer(uTimerID, lpszTimerName);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnMessage(uID, uMsg, wParam, lParam);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnMousePointChange(CPoint& point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnMousePointChange(point);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnCheckMouseResponse(UINT nFlags, CPoint point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnCheckMouseResponse(nFlags, point);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnMouseMove(UINT nFlags, CPoint point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnMouseMove(nFlags, point);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnLButtonDown(nFlags, point);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnLButtonUp(nFlags, point);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnScroll(bVertical, nFlags, point);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnKeyDown(nChar, nRepCnt, nFlags);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnLButtonDblClk(nFlags, point);
}

int __stdcall CDuiPlugin::XDuiPlugin::SetUpdate(BOOL bUpdate, COLORREF clr)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->SetUpdate(bUpdate, clr);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnRButtonDown(UINT nFlags, CPoint point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnRButtonDown(nFlags, point);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnRButtonUp(nFlags, point);
}

int __stdcall CDuiPlugin::XDuiPlugin::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CDuiPlugin *pObj = GET_INTERFACE_OBJECT(DuiPlugin);
	return pObj->OnRButtonDblClk(nFlags, point);
}
//}}VCI_INTERFACE_IMPLEMENT_END

// ��ѯ�����Ƿ�֧��
extern "C" __declspec(dllexport)
int gQueryCommandIsSupport(LPCSTR lpcsCmd)
{
	//if(strcmp(lpcsCmd, "HELLO") == 0)
	//{
	//}
	return 0;
}
