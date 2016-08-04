#include "StdAfx.h"
#include "MFCDateTime.h"

////////////////////////////////////////////////
// CMFCMonthCalCtrl
CMFCMonthCalCtrl::CMFCMonthCalCtrl(CDuiObject* pParentObj, UINT msg, UINT id)
	: CMonthCalCtrl()
{
	m_DbClick = false;
	m_nMsgClick = msg;
	m_pParentObj = pParentObj;
	m_ControlID = id;
}

CMFCMonthCalCtrl::~CMFCMonthCalCtrl()
{ 
	m_pParentObj = NULL;
}

BEGIN_MESSAGE_MAP(CMFCMonthCalCtrl, CMonthCalCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()

LRESULT CMFCMonthCalCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{ 
	int x = 0, y = 0;
	POINT pt;
	switch (message)
	{
	case 129:		//<<<鼠标左单击
	case 132:		//<<<鼠标移动		
		break;
	case 533:		//<<<鼠标左双击
		m_DbClick = !m_DbClick;
		if (m_DbClick)
		{
			GetCursorPos(&pt);
			OnExLButtonDbClk(m_ControlID,pt.x, pt.y);
			TRACE("[pos: %d,%d],\n", pt.x, pt.y);
		}
	default:
		TRACE("[msg: %d]%d,%d\n", message, wParam, lParam);
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}

void CMFCMonthCalCtrl::OnExLButtonDbClk(UINT id, long x, long y)
{
	if (m_pParentObj != NULL)
	{
		CPoint pt(x, y);
		SendMessageToParent(m_nMsgClick, id, (LPARAM)&pt);
	}			
}

void CMFCMonthCalCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

// 发送通知消息给父控件
BOOL CMFCMonthCalCtrl::SendMessageToParent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pParentObj == NULL)
	{
		return FALSE;
	}

	if(m_pParentObj->IsClass(_T("dlg")))
	{
		return ((CDlgBase*)m_pParentObj)->OnMessage(m_ControlID, uMsg, wParam, lParam);
	}else
	if(m_pParentObj->IsClass(_T("popup")))
	{
		return ((CDlgPopup*)m_pParentObj)->OnMessage(m_ControlID, uMsg, wParam, lParam);
	}else
	{
		return ((CControlBase*)m_pParentObj)->OnMessage(m_ControlID, uMsg, wParam, lParam);
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////
// CMFCDateTime
CMFCDateTime::CMFCDateTime(HWND hWnd, CDuiObject* pDuiObject)
	: CDuiEdit(hWnd, pDuiObject)
{
	::GetLocalTime(&m_sysTime);
	m_bReadOnly = false;
	m_pMouthCalCtrl = NULL;
	m_rcMounth.left = 0;
	m_rcMounth.top = 0;
	m_rcMounth.bottom = 0;
	m_rcMounth.right = 0;
	m_MouthCalCtrlHeight = 170;
	m_MouthCalCtrlWidth = 190;
	m_IsShowMouthCalCtrl = false;
	m_bReadOnly = TRUE;	
	m_isDefaultToday = true;
}

CMFCDateTime::CMFCDateTime(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, BOOL bPassWord, BOOL bIsVisible, BOOL bIsDisable, BOOL bIsPressDown)
	: CDuiEdit(hWnd,pDuiObject,uControlID,rc,strTitle,bPassWord,bIsVisible,bIsDisable,bIsPressDown)
{
	::GetLocalTime(&m_sysTime);
	m_bReadOnly = false;	
	m_pMouthCalCtrl = NULL;
	m_rcMounth.left = 0;
	m_rcMounth.top = 0;
	m_rcMounth.bottom = 0;
	m_rcMounth.right = 0;
	m_MouthCalCtrlHeight = 170;
	m_MouthCalCtrlWidth = 190;
	m_IsShowMouthCalCtrl = false;
	m_bReadOnly = TRUE;	
	m_isDefaultToday = true;
}

CMFCDateTime::~CMFCDateTime()
{
	DeleteCMonthCalCtrl();
}

HFONT CMFCDateTime::GetDateTimeFont()
{
	return (HFONT)m_fontTemp.m_hObject;
}

void CMFCDateTime::ShowCMonthCalCtrl()
{
	if (m_bReadOnly || m_bIsDisable)
		return;

	if (m_pMouthCalCtrl == NULL)
	{
		TestMainThread();

		// 弹出控件选择时候发送MSG_CONTROL_BUTTON消息
		m_pMouthCalCtrl = new CMFCMonthCalCtrl(this, MSG_CONTROL_BUTTON, GetID());

		//DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | MCS_NOTODAY;
		DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER ;

		m_pMouthCalCtrl->Create(dwStyle, m_rcMounth, CWnd::FromHandle(m_hWnd), GetID());
		m_pMouthCalCtrl->ShowWindow(SW_SHOW);
		m_pMouthCalCtrl->GetWindowRect(m_rcMounth);

		m_IsShowMouthCalCtrl = true;
	}	
	else
	{
		m_pMouthCalCtrl->ShowWindow(SW_SHOW);
		m_IsShowMouthCalCtrl = true;
	}
}

void CMFCDateTime::HideCMonthCalCtrl()
{
	if (m_pMouthCalCtrl)
	{
		TestMainThread();
		if (::IsWindow(m_pMouthCalCtrl->GetSafeHwnd()))
		{
			m_pMouthCalCtrl->GetWindowText(m_strTitle);
		}
		m_pMouthCalCtrl->ShowWindow(SW_HIDE);
		m_IsShowMouthCalCtrl = false;
	}
}

void CMFCDateTime::DeleteCMonthCalCtrl()
{
	if (m_pMouthCalCtrl)
	{
		TestMainThread();
		if (::IsWindow(m_pMouthCalCtrl->GetSafeHwnd()))
		{
			m_pMouthCalCtrl->GetWindowText(m_strTitle);
		}
		delete m_pMouthCalCtrl;
		m_pMouthCalCtrl = NULL;
		m_IsShowMouthCalCtrl = false;
	}
}

SYSTEMTIME& CMFCDateTime::GetTime()
{
	return m_sysTime;	
}

void CMFCDateTime::SetTime(SYSTEMTIME* pst)
{
	m_sysTime = *pst;
	InvalidateRect(&this->GetRect());
}

void CMFCDateTime::SetControlRect(CRect rc)
{
	__super::SetControlRect(rc);

	m_rcMounth = m_rcText;
	m_rcMounth.top = m_rcText.bottom + 2;
	m_rcMounth.right = m_rcMounth.left + m_MouthCalCtrlWidth;
	m_rcMounth.bottom = m_rcMounth.bottom + m_MouthCalCtrlHeight;

	DeleteCMonthCalCtrl();
}

void CMFCDateTime::SetControlWndVisible(BOOL bIsVisible)
{
	__super::SetControlWndVisible(bIsVisible);
	if (bIsVisible)
	{
		//ShowCMonthCalCtrl();
	}
	else
	{
		HideCMonthCalCtrl();
	}
}

void CMFCDateTime::SetControlTitle(CString strTitle)
{
	__super::SetControlTitle(strTitle);
	if (m_pMouthCalCtrl)
		m_pMouthCalCtrl->SetWindowText(m_strTitle);
}

BOOL CMFCDateTime::SetControlFocus(BOOL bFocus)
{
	bool isShow = !m_IsShowMouthCalCtrl || (m_IsShowMouthCalCtrl && m_isVailidRect);

	__super::SetControlFocus(bFocus);

	if (isShow)
	{
		//ShowCMonthCalCtrl();	
	}
	else
	{
		HideCMonthCalCtrl();
	}

	return isShow;
}

// 弹出的时间选择控件的消息处理
LRESULT CMFCDateTime::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT res = __super::OnMessage(uID, uMsg, wParam, lParam);
	if((uID == GetID()) && (uMsg == MSG_CONTROL_BUTTON) && ((wParam == CONTROL_BUTTON) || (wParam == CONTROL_EDIT)))
	{
		// wParam为CONTROL_BUTTON或CONTROL_EDIT表示在编辑框的按钮或输入区域点击
		ShowCMonthCalCtrl();
	}else
	if((uID == GetID()) && (uMsg == MSG_CONTROL_BUTTON) && (wParam == GetID()))
	{
		// 只有MSG_CONTROL_BUTTON消息,并且wParam为控件ID才是时间控件的消息
		OnMonthCalCtrlSelect(0, *(CPoint*)lParam);
	}

	return res;
}

// 事件控件选择时候,刷新edit显示的文字
BOOL CMFCDateTime::OnMonthCalCtrlSelect(UINT nFlags, CPoint point)
{
	BOOL bResult = FALSE;
	if (m_rcMounth.PtInRect(point))
	{
		m_pMouthCalCtrl->GetCurSel(&m_sysTime);
		CString strDate;
		strDate.Format(_T("%4d-%02d-%02d"), m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay);
		SetControlTitle(strDate);
		bResult = TRUE;
	}	
	HideCMonthCalCtrl();
	return bResult;
}

BOOL CMFCDateTime::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	m_isVailidRect = m_rcMounth.PtInRect(point);
	return __super::OnControlLButtonDown(nFlags, point);
}

HRESULT CMFCDateTime::OnAttributeDateTimeValue(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty())
		return E_FAIL;
	int         nYear = 0;
	int         nMonth = 0;
	int         nDay = 0;
	int         nHour = 0;
	int         nSecond = 0;
	int         nMinute = 0;

	_stscanf_s(strValue, _T("%d-%d-%d %d:%d:%d"), &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);

	m_sysTime.wYear = nYear;
	m_sysTime.wMonth = nMonth;
	m_sysTime.wDay = nDay;
	m_sysTime.wHour = nHour;
	m_sysTime.wSecond = nSecond;
	m_sysTime.wMinute = nMinute;

	m_isDefaultToday = false;

	return bLoading ? S_FALSE : S_OK;
}