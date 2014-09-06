#include "StdAfx.h"
#include "WndBase.h"

#ifdef _DEBUG
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

/////////////////////////////////////////////////////////////////////////////////////
// Windows窗口类

CWindowWnd::CWindowWnd()
	: m_hWnd(NULL), m_OldWndProc(::DefWindowProc), m_bSubclassed(false)
{
}

HWND CWindowWnd::GetHWND() const 
{ 
    return m_hWnd; 
}

UINT CWindowWnd::GetClassStyle() const
{
    return 0;
}

LPCTSTR CWindowWnd::GetWindowClassName() const
{
    return NULL;
}

LPCTSTR CWindowWnd::GetSuperClassName() const
{
    return NULL;
}

CWindowWnd::operator HWND() const
{
    return m_hWnd;
}

HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
    return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
    if( GetSuperClassName() != NULL && !RegisterSuperclass() ) return NULL;
    if( GetSuperClassName() == NULL && !RegisterWindowClass() ) return NULL;
	m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, DuiSystem::Instance()->GetInstance(), this);
    ASSERT(m_hWnd!=NULL);
    return m_hWnd;
}

// 子类化
HWND CWindowWnd::Subclass(HWND hWnd)
{
    ASSERT(::IsWindow(hWnd));
    ASSERT(m_hWnd==NULL);
	m_OldWndProc = SubclassWindow(hWnd, __WndProc);
    if( m_OldWndProc == NULL ) return NULL;
    m_bSubclassed = true;
    m_hWnd = hWnd;
    return m_hWnd;
}

// 取消子类化
void CWindowWnd::Unsubclass()
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    if( !m_bSubclassed ) return;
    SubclassWindow(m_hWnd, m_OldWndProc);
    m_OldWndProc = ::DefWindowProc;
    m_bSubclassed = false;
}

void CWindowWnd::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    ::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
}

// 显示模态窗口
bool CWindowWnd::ShowModal()
{
    ASSERT(::IsWindow(m_hWnd));
    HWND hWndParent = GetWindowOwner(m_hWnd);
    ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
    ::EnableWindow(hWndParent, FALSE);
    MSG msg = { 0 };
    while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) )
	{
        if( msg.message == WM_CLOSE && msg.hwnd == m_hWnd )
		{
            ::EnableWindow(hWndParent, TRUE);
            ::SetFocus(hWndParent);
        }
        if( !::TranslateMessage(&msg) )
		{
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        if( msg.message == WM_QUIT ) break;
    }
    ::EnableWindow(hWndParent, TRUE);
    ::SetFocus(hWndParent);
    if( msg.message == WM_QUIT )
	{
		::PostQuitMessage(msg.wParam);
	}
    return true;
}

void CWindowWnd::Destory()
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    DestroyWindow(m_hWnd);
}

void CWindowWnd::Close()
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    PostMessage(WM_CLOSE);
}

// 居中
void CWindowWnd::CenterWindow()
{
    ASSERT(::IsWindow(m_hWnd));
    ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD)==0);
    RECT rcDlg = { 0 };
    ::GetWindowRect(m_hWnd, &rcDlg);
    RECT rcArea = { 0 };
    RECT rcCenter = { 0 };
    HWND hWndParent = ::GetParent(m_hWnd);
    HWND hWndCenter = ::GetWindowOwner(m_hWnd);
    ::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
    if( hWndCenter == NULL )
	{
		rcCenter = rcArea;
	}else
	{
		::GetWindowRect(hWndCenter, &rcCenter);
	}

    int DlgWidth = rcDlg.right - rcDlg.left;
    int DlgHeight = rcDlg.bottom - rcDlg.top;

    // Find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

    // The dialog is outside the screen, move it inside
    if( xLeft < rcArea.left )
	{
		xLeft = rcArea.left;
	}else
	if( xLeft + DlgWidth > rcArea.right )
	{
		xLeft = rcArea.right - DlgWidth;
	}
    if( yTop < rcArea.top )
	{
		yTop = rcArea.top;
	}else
	if( yTop + DlgHeight > rcArea.bottom )
	{
		yTop = rcArea.bottom - DlgHeight;
	}
    ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CWindowWnd::SetIcon(UINT nRes)
{
    HICON hIcon = (HICON)::LoadImage(DuiSystem::Instance()->GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    ASSERT(hIcon);
    ::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
    hIcon = (HICON)::LoadImage(DuiSystem::Instance()->GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    ASSERT(hIcon);
    ::SendMessage(m_hWnd, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
}

bool CWindowWnd::RegisterWindowClass()
{
    WNDCLASS wc = { 0 };
    wc.style = GetClassStyle();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = CWindowWnd::__WndProc;
    wc.hInstance = DuiSystem::Instance()->GetInstance();
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClass(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool CWindowWnd::RegisterSuperclass()
{
    // Get the class information from an existing
    // window so we can subclass it later on...
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    if( !::GetClassInfoEx(NULL, GetSuperClassName(), &wc) )
	{
        if( !::GetClassInfoEx(DuiSystem::Instance()->GetInstance(), GetSuperClassName(), &wc) )
		{
            ASSERT(!"Unable to locate window class");
            return NULL;
        }
    }
    m_OldWndProc = wc.lpfnWndProc;
    wc.lpfnWndProc = CWindowWnd::__ControlProc;
    wc.hInstance = DuiSystem::Instance()->GetInstance();
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClassEx(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CWindowWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowWnd* pThis = NULL;
    if( uMsg == WM_NCCREATE )
	{
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
        pThis->m_hWnd = hWnd;
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
    }else
	{
        pThis = reinterpret_cast<CWindowWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if( uMsg == WM_NCDESTROY && pThis != NULL )
		{
            LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
            ::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
            if( pThis->m_bSubclassed ) pThis->Unsubclass();
            pThis->m_hWnd = NULL;
            pThis->OnFinalMessage(hWnd);
            return lRes;
        }
    }
    if( pThis != NULL )
	{
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }else
	{
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK CWindowWnd::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowWnd* pThis = NULL;
    if( uMsg == WM_NCCREATE )
	{
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
        ::SetProp(hWnd, _T("WndX"), (HANDLE) pThis);
        pThis->m_hWnd = hWnd;
    }else
	{
        pThis = reinterpret_cast<CWindowWnd*>(::GetProp(hWnd, _T("WndX")));
        if( uMsg == WM_NCDESTROY && pThis != NULL )
		{
            LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
            if( pThis->m_bSubclassed ) pThis->Unsubclass();
            ::SetProp(hWnd, _T("WndX"), NULL);
            pThis->m_hWnd = NULL;
            pThis->OnFinalMessage(hWnd);
            return lRes;
        }
    }
    if( pThis != NULL )
	{
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }else
	{
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CWindowWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(m_hWnd));
    return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
} 

LRESULT CWindowWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(m_hWnd));
    return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void CWindowWnd::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
    ASSERT(::IsWindow(m_hWnd));
    RECT rc = { 0 };
    if( !::GetClientRect(m_hWnd, &rc) ) return;
    if( cx != -1 ) rc.right = cx;
    if( cy != -1 ) rc.bottom = cy;
    if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)) ) return;
    ::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

// 窗口消息处理函数
LRESULT CWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 首先对一些可能被重载的消息进行处理
	LRESULT lRes;
    BOOL bHandled = TRUE;
    switch( uMsg )
	{
    case WM_PAINT:         lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
    case WM_SETFOCUS:      lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_ERASEBKGND:    lRes = OnEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
    case WM_MOUSEACTIVATE: lRes = OnMouseActivate(uMsg, wParam, lParam, bHandled); break;
    default:
        bHandled = FALSE;
    }

	// 如果消息没有被处理,则调用Windows的窗口消息处理函数
    if( !bHandled )
	{
		return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
	}

    return lRes;
}

void CWindowWnd::OnFinalMessage(HWND /*hWnd*/)
{
}

LRESULT CWindowWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CWindowWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CWindowWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CWindowWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CWindowWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

// 控件画图函数
void CWindowWnd::DrawControl(CDC &dc, CRect rcUpdate)
{
}