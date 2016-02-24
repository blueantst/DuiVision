#include "StdAfx.h"
#include "WndBase.h"
#include "DuiNativeWnd.h"


/////////////////////////////////////////////////////////////////////////////////////
// CDuiNativeWnd

CDuiNativeWnd::CDuiNativeWnd(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBase(hWnd, pDuiObject)
{
	m_pNativeWnd = NULL;
	m_bCreated = false;
	m_bDelayCreate = false;
}

CDuiNativeWnd::~CDuiNativeWnd()
{
    ReleaseControl();
}

CWnd* CDuiNativeWnd::GetNativeWnd() const
{
    return m_pNativeWnd;
}

// 获取控件的父窗口对象
CWnd* CDuiNativeWnd::GetPaintWnd()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		return pDlg;
	}
    return NULL;
}

// 设置控件中的Windows原生控件是否可见的状态
void CDuiNativeWnd::SetControlWndVisible(BOOL bIsVisible)
{
	if( m_hwndHost != NULL )
	{
		::ShowWindow(m_hwndHost, bIsVisible ? SW_SHOW : SW_HIDE);
	}
}

// 设置控件的位置信息
void CDuiNativeWnd::SetControlRect(CRect rc) 
{
	m_rc = rc;

	if( !m_bCreated )
	{
		CreateControl();
	}

    if(m_hwndHost && ::IsWindow(m_hwndHost))
	{
		::MoveWindow(m_hwndHost, m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top, TRUE);
    }
}

// 从XML设置DelayCreate属性
HRESULT CDuiNativeWnd::OnAttributeDelayCreate(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	SetDelayCreate(strValue == _T("true"));

	return bLoading?S_FALSE:S_OK;
}

LRESULT CDuiNativeWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if((m_hwndHost == NULL) || !::IsWindow(m_hwndHost))
	{
		return 0;
	}

    if( !GetRresponse() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST )
	{
		return 0;
	}

    bool bWasHandled = true;
    if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR )
	{
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = HITRESULT_HIT;
        if( dwHitResult != HITRESULT_HIT ) return 0;
        if( uMsg == WM_SETCURSOR ) bWasHandled = false;
    }else
	if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST )
	{
        // Keyboard messages just go when we have focus
        if( !IsFocusControl() ) return 0;
    }else
	{
        switch( uMsg )
		{
        case WM_HELP:
        case WM_CONTEXTMENU:
            bWasHandled = false;
            break;
        default:
            return 0;
        }
    }
    LRESULT lResult = 0;
    return lResult;
}

bool CDuiNativeWnd::IsDelayCreate() const
{
    return m_bDelayCreate;
}

// 设置延迟加载属性
void CDuiNativeWnd::SetDelayCreate(bool bDelayCreate)
{
    if( m_bDelayCreate == bDelayCreate ) return;
	CreateControl();
    m_bDelayCreate = bDelayCreate;
}

// 设置原生控件对象的窗口句柄
bool CDuiNativeWnd::SetNativeHWnd(HWND hWnd)
{
	if(hWnd == NULL)
	{
		return false;
	}

	ReleaseControl();
	m_hwndHost = hWnd;
	bool bRet = false;
	if(m_hwndHost && ::IsWindow(m_hwndHost))
	{
		SetControlWndVisible(FALSE);
		OnAttributePosChange(GetPosStr(), FALSE);
		bRet = true;
	}

	return bRet;
}

// 设置原生控件对象指针
bool CDuiNativeWnd::SetNativeWnd(CWnd* pWnd)
{
	if(pWnd == NULL)
	{
		return false;
	}

	bool bRet = SetNativeHWnd(pWnd->m_hWnd);
	if(bRet)
	{
		m_pNativeWnd = pWnd;
	}

	return bRet;
}

// 创建原生控件
bool CDuiNativeWnd::CreateControl()
{
    m_bCreated = false;
    if( !m_bDelayCreate )
	{
		// 发送消息给事件处理类,通知创建原生控件
	}
    return true;
}

// 释放原生控件
void CDuiNativeWnd::ReleaseControl()
{
	if(m_hwndHost && ::IsWindow(m_hwndHost))
	{
		DestroyWindow(m_hwndHost);
	}
    m_hwndHost = NULL;

	if(m_pNativeWnd != NULL)
	{
		delete m_pNativeWnd;
		m_pNativeWnd = NULL;
	}
}

// 键盘事件处理
BOOL CDuiNativeWnd::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 如果是回车键或ESC键,则转换为字符事件传递给原生控件
	if(((nChar == VK_RETURN) || (nChar == VK_ESCAPE)) && m_hwndHost && ::IsWindow(m_hwndHost))
	{
		if(::SendMessage(m_hwndHost, WM_CHAR, nChar, nFlags))
		{
			return true;
		}
	}

	return false;
}

// 控件画图函数
void CDuiNativeWnd::DrawControl(CDC &dc, CRect rcUpdate)
{
	if( !::IntersectRect(&rcUpdate, &rcUpdate, &m_rc) ) return;

	if(m_hwndHost && ::IsWindow(m_hwndHost))
    {
		// 调用原生控件的画图(发送WM_PAINT消息给控件)
		//m_pWindowWnd->DrawControl(dc, rcUpdate);
    }
}
