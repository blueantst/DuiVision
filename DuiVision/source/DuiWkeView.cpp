#include "StdAfx.h"
#include "DuiWkeView.h"
#include <Imm.h>

#ifdef USE_WKE_CONTROL

#pragma comment(lib, "imm32.lib")		// 自动链接imm库
#pragma message("Automatically linking with imm32.lib")

//#pragma comment(lib, "../DuiVision/third-part/wke/wke.lib")		// 自动链接wke库
//#pragma message("Automatically linking with wke.lib")

static bool g_wkeInited = false;	// wke库是否已经初始化
static CPtrList	g_duiWkeViews;		// Wke视图对象列表

/////////////////////////////////////////////////////////////////////////////////////
// CDuiWkeView

CDuiWkeView::CDuiWkeView(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBase(hWnd, pDuiObject)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_OldWndProc = ::DefWindowProc;
	m_bCreated = false;
	m_bCreating = false;
	m_bDelayCreate = false;
	m_bTransparent = false;
	m_pWebView = NULL;
	m_strUrl = L"";
	m_strHtml = L"";

	CDuiWkeView::WkeInit();
	g_duiWkeViews.AddTail(this);
}

CDuiWkeView::~CDuiWkeView()
{
	if(m_hNativeWnd != NULL)
	{
		::DestroyWindow(m_hNativeWnd);
	}

	// 删除wke对象列表中的项
	int count = g_duiWkeViews.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_duiWkeViews.FindIndex(i);
		CDuiWkeView* pDuiWkeView = (CDuiWkeView*)g_duiWkeViews.GetAt(pos);
		if(pDuiWkeView == this) 
		{
			g_duiWkeViews.RemoveAt(pos);
			return;
		}
	}
}

// wke库初始化
void CDuiWkeView::WkeInit()
{
	if(g_wkeInited)
	{
		return;
	}

    wkeInit();

	g_wkeInited = true;
}

// wke库释放
void CDuiWkeView::WkeShutdown()
{
	wkeShutdown();
}

// 根据回调指针获取wke对象指针
CDuiWkeView* CDuiWkeView::GetWkeViewByClientHandler(const wkeClientHandler* pWkeClientHandler)
{
	int count = g_duiWkeViews.GetCount();
	for(int i = 0; i < count; i ++)
	{
		POSITION pos = g_duiWkeViews.FindIndex(i);
		CDuiWkeView* pDuiWkeView = (CDuiWkeView*)g_duiWkeViews.GetAt(pos);
		if(pWkeClientHandler == pDuiWkeView->GetWkeClientHandler())
		{
			return pDuiWkeView;
		}
	}
	return NULL;
}

// 获取wke视图指针
wkeWebView CDuiWkeView::GetWkeWebView()
{
	return m_pWebView;
}

// 获取wke回调指针
wkeClientHandler* CDuiWkeView::GetWkeClientHandler()
{
	return &m_wkeHander;
}

// 设置控件中的Windows原生控件是否可见的状态
void CDuiWkeView::SetControlWndVisible(BOOL bIsVisible)
{
	if( m_hNativeWnd != NULL )
	{
		::ShowWindow(m_hNativeWnd, bIsVisible ? SW_SHOW : SW_HIDE);
	}
}

// 设置控件的位置
void CDuiWkeView::SetControlRect(CRect rc) 
{
	m_rc = rc;

	if( !m_bCreated )
	{
		CreateControl();
	}

    if(m_hNativeWnd && ::IsWindow(m_hNativeWnd))
	{
		::MoveWindow(m_hNativeWnd, m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top, TRUE);
    }
}

// 设置当前焦点控件
void CDuiWkeView::SetCurrentFocusControl(BOOL bFocus) 
{
	// 暂时不能设置,否则会导致tab切换到edit控件时候异常
	/*CDlgBase* pDlg = GetParentDialog(FALSE);
	if(pDlg)
	{
		pDlg->SetFocusControl(bFocus ? this : NULL);
	}*/
}

// 从XML设置DelayCreate属性
HRESULT CDuiWkeView::OnAttributeDelayCreate(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	SetDelayCreate(strValue == _T("true"));

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置url属性
HRESULT CDuiWkeView::OnAttributeUrl(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_strUrl = strValue;
	if(m_pWebView != NULL)
	{
		m_pWebView->loadURL(m_strUrl);
	}

	return bLoading?S_FALSE:S_OK;
}

bool CDuiWkeView::IsDelayCreate() const
{
    return m_bDelayCreate;
}

// 设置延迟加载属性
void CDuiWkeView::SetDelayCreate(bool bDelayCreate)
{
    if(m_bDelayCreate == bDelayCreate)
	{
		return;
	}
	if(!bDelayCreate)
	{
		CreateControl();
	}
	m_bDelayCreate = bDelayCreate;
}

const LPCWSTR wkeWebViewClassName = L"class::wkeWebView";	// wke窗口类名

// 注册窗口类
bool CDuiWkeView::RegisterWindowClass()
{
    WNDCLASS wc = { 0 };
    wc.style = CS_DBLCLKS;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = CDuiWkeView::__WebViewWndProc;
    wc.hInstance = DuiSystem::Instance()->GetInstance();
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = wkeWebViewClassName;
    ATOM ret = ::RegisterClass(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

// 创建wke控件
bool CDuiWkeView::CreateControl()
{
	if(m_bCreating)
	{
		return false;
	}

    m_bCreated = false;
	m_bCreating = true;

	// 设置wke事件回调函数
	m_wkeHander.onTitleChanged = onTitleChanged;
	m_wkeHander.onURLChanged = onURLChanged;

	// 创建wke视图,并加载url
	m_pWebView = wkeCreateWebView();
	m_pWebView->setTransparent(m_bTransparent);
	m_pWebView->setClientHandler(&m_wkeHander);
	m_pWebView->setBufHandler(this);
	m_pWebView->loadURL(m_strUrl);

	// 注册窗口类
	RegisterWindowClass();

	// 创建用于显示wke视图的窗口
	HWND hWnd = ::CreateWindow(wkeWebViewClassName, 0, 
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, 0, 0, 
        m_hWnd, 
        0, 
        DuiSystem::Instance()->GetInstance(), this);

	// 设置窗口句柄
	if(hWnd && ::IsWindow(hWnd))
	{
		ReleaseControl();
		m_hNativeWnd = hWnd;
		//OnAttributePosChange(GetPosStr(), FALSE);
	}

	// 初始化wke渲染对象,将渲染对象关联到视图窗口
	m_render.init(m_hNativeWnd);

	m_bCreated = true;
	m_bCreating = false;

    return true;
}

// 释放原生控件
void CDuiWkeView::ReleaseControl()
{
	// 如果是焦点控件,则将当前焦点控件设置为空
	if(IsFocusControl())
	{
		SetCurrentFocusControl(NULL);
	}
	// 删除宿主窗口
	if(m_hNativeWnd && ::IsWindow(m_hNativeWnd))
	{
		DestroyWindow(m_hNativeWnd);
	}
    m_hNativeWnd = NULL;
}

// 控件画图函数
void CDuiWkeView::DrawControl(CDC &dc, CRect rcUpdate)
{
	if( !::IntersectRect(&rcUpdate, &rcUpdate, &m_rc) ) return;

	if(m_hNativeWnd && ::IsWindow(m_hNativeWnd) && m_pWebView)
    {
		// 强制wke视图刷新
		m_pWebView->setDirty(true);
		// wke渲染
		m_render.render(m_pWebView);
    }
}

// 控件的30毫秒定时器
BOOL CDuiWkeView::OnControlTimer()
{
	if(!m_bIsVisible || !m_bCreated)
	{
		return FALSE;
	}

	// 给wke窗口发送定时器消息,用于刷新页面
	::SendMessage(m_hNativeWnd, WM_TIMER, 0, 0);

	return TRUE;
}

// 键盘事件处理
BOOL CDuiWkeView::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 如果当前处于焦点状态,则处理回车键和ESC键
	if(IsFocusControl() && ((nChar == VK_RETURN) || (nChar == VK_ESCAPE)) && (nFlags == 0) && m_pWebView)
	{
		m_pWebView->keyDown(nChar, nFlags, false);
		m_render.render(m_pWebView);
		return true;
	}

	return false;
}

void CDuiWkeView::onBufUpdated( const HDC hdc,int x, int y, int cx, int cy )
{
	RECT rcClient;
	GetClientRect(m_hWnd,&rcClient);
	RECT rcInvalid = {rcClient.left + x, rcClient.top + y, rcClient.left + x + cx, rcClient.top + y + cy};
	::InvalidateRect(m_hWnd, &rcInvalid ,TRUE);
}

// 窗口消息处理函数
LRESULT CALLBACK CDuiWkeView::__WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CDuiWkeView* pThis = NULL;
    if( message == WM_NCCREATE )
	{
		// 窗口创建时候保存当前对象指针(创建时候通过参数传入的)
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CDuiWkeView*>(lpcs->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
    }else
	{
		// 通过窗口保存的指针获取对象指针
        pThis = reinterpret_cast<CDuiWkeView*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if( message == WM_DESTROY && pThis != NULL )
		{
			// 窗口销毁
            //LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, message, wParam, lParam);
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, 0L);
			pThis->ReleaseControl();
            return 0;//lRes;
        }
    }

	// 如果获取到对象指针,则调用wke消息处理函数,否则调用默认窗口消息处理函数
    if( pThis != NULL )
	{
        return pThis->WebViewWindowProc(hWnd, message, wParam, lParam);
    }else
	{
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// wke窗口的windows消息处理
LRESULT CDuiWkeView::WebViewWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( !GetRresponse() )
	{
		return 0;
	}

	// 对于键盘消息,如果此控件不是焦点控件,则退出
	if( message >= WM_KEYFIRST && message <= WM_KEYLAST )
	{
        if( !IsFocusControl() ) return 0;
    }
	
    bool handled = true;
	switch (message)
	{
    case WM_COMMAND:
		::SendMessage(m_hWnd, message, wParam, lParam);
        return 0;
/*
	case WM_PAINT:
        if (m_pWebView)
        {
			PAINTSTRUCT ps = { 0 };
			HDC hDcPaint = ::BeginPaint(hWnd, &ps);

			RECT rcClip;	
			::GetClipBox(hDcPaint,&rcClip);	

			RECT rcClient;
			::GetClientRect(hWnd, &rcClient);
			
			RECT rcInvalid;
			::IntersectRect(&rcInvalid, &rcClip,&rcClient);

			m_pWebView->paint(hDcPaint,rcInvalid.left,rcInvalid.top,
				rcInvalid.right - rcInvalid.left, rcInvalid.bottom - rcInvalid.top,
				rcInvalid.left-rcClient.left,
				rcInvalid.top-rcClient.top,true);

			::EndPaint(hWnd, &ps);
        }
        break;
*/
    case WM_SIZE:
        if (m_pWebView)
        {
            m_pWebView->resize(LOWORD(lParam), HIWORD(lParam));
            m_render.resize(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_KEYDOWN:
        {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            //flags = HIWORD(lParam);

			handled = m_pWebView->keyDown(virtualKeyCode, flags, false);
			m_render.render(m_pWebView);

			/*// 下面的转换不起作用
			MSG msg;
			msg.hwnd = hWnd;
			msg.message = message;
			msg.wParam = wParam;
			msg.lParam = lParam;
			::TranslateMessage(&msg);
			if((msg.message == WM_CHAR) || (msg.message == WM_IME_CHAR))
			{
				::DispatchMessage(&msg);
			}*/
        }
        break;

    case WM_KEYUP:
        {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            //flags = HIWORD(lParam);

            handled = m_pWebView->keyUp(virtualKeyCode, flags, false);
			m_render.render(m_pWebView);
        }
        break;

    case WM_CHAR:
	case WM_IME_CHAR:
        {
            unsigned int charCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            //flags = HIWORD(lParam);

            handled = m_pWebView->keyPress(charCode, flags, false);
			m_render.render(m_pWebView);
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:

    case WM_MOUSEMOVE:
        {
            if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN)
            {
                SetFocus(hWnd);
                SetCapture(hWnd);
            }
            else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP)
            {
                ReleaseCapture();
            }

            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            unsigned int flags = 0;

            if (wParam & MK_CONTROL)
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            //flags = wParam;

            handled = m_pWebView->mouseEvent(message, x, y, flags);
			m_render.render(m_pWebView);
        }
        break;

    case WM_CONTEXTMENU:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);

            if (pt.x != -1 && pt.y != -1)
                ScreenToClient(hWnd, &pt);

            unsigned int flags = 0;

            if (wParam & MK_CONTROL)
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            handled = m_pWebView->contextMenuEvent(pt.x, pt.y, flags);
			m_render.render(m_pWebView);
        }
        break;

    case WM_MOUSEWHEEL:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(hWnd, &pt);

            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            unsigned int flags = 0;

            if (wParam & MK_CONTROL)
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            //flags = wParam;

            handled = m_pWebView->mouseWheel(pt.x, pt.y, delta, flags);
			m_render.render(m_pWebView);
        }
        break;

    case WM_SETFOCUS:
        m_pWebView->focus();
		m_render.render(m_pWebView);
		// 设置对话框的焦点控件为此控件
		SetCurrentFocusControl(TRUE);
        break;

    case WM_KILLFOCUS:
        m_pWebView->unfocus();
		m_render.render(m_pWebView);
		// 清除对话框的焦点控件
		//SetCurrentFocusControl(FALSE);
        break;

	case WM_TIMER:
		m_render.render(m_pWebView);
        break;

    case WM_IME_STARTCOMPOSITION:
        {
            wkeRect caret = m_pWebView->getCaret();

            CANDIDATEFORM form;
            form.dwIndex = 0;
            form.dwStyle = CFS_EXCLUDE;
            form.ptCurrentPos.x = caret.x;
            form.ptCurrentPos.y = caret.y + caret.h;
            form.rcArea.top = caret.y;
            form.rcArea.bottom = caret.y + caret.h;
            form.rcArea.left = caret.x;
            form.rcArea.right = caret.x + caret.w;

            HIMC hIMC = ImmGetContext(hWnd);
            ImmSetCandidateWindow(hIMC, &form);
            ImmReleaseContext(hWnd, hIMC);
        }
        break;

    default:
        handled = false;
        break;
	}
    
    if (!handled)
        return DefWindowProc(hWnd, message, wParam, lParam);

    return 0;
}

// URL变更的回调函数
void CDuiWkeView::onURLChanged(const struct _wkeClientHandler* clientHandler, const wkeString URL)
{
	// 查找对应的控件对象
	CDuiWkeView* pDuiWkeView = CDuiWkeView::GetWkeViewByClientHandler(clientHandler);
	if(pDuiWkeView)
	{
		pDuiWkeView->setURL(wkeToStringW(URL));
		pDuiWkeView->SendMessage(MSG_CONTROL_EVENT, (WPARAM)WKE_EVENT_URLCHANGED, (LPARAM)wkeToStringW(URL));
	}
}

// 页面标题变更的回调函数
void CDuiWkeView::onTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString title)
{
	// 查找对应的控件对象
	CDuiWkeView* pDuiWkeView = CDuiWkeView::GetWkeViewByClientHandler(clientHandler);
	if(pDuiWkeView)
	{
		pDuiWkeView->SendMessage(MSG_CONTROL_EVENT, (WPARAM)WKE_EVENT_TITLECHANGED, (LPARAM)wkeToStringW(title));
	}
}

// 获取网页标题
CString CDuiWkeView::getTitle()
{
	if(m_pWebView)
	{
		return m_pWebView->titleW();
	}

	return L"";
}

// 获取网页URL
CString CDuiWkeView::getURL()
{
	return m_strUrl;
}

// 设置网页URL
void CDuiWkeView::setURL(CString strUrl)
{
	m_strUrl = strUrl;
}

// URL导航
void CDuiWkeView::Navigate(CString strUrl)
{
	if(m_pWebView)
	{
		m_pWebView->loadURL(strUrl);
		m_render.render(m_pWebView);
	}
}

// 加载页面内容
void CDuiWkeView::loadHTML(CString strHtml)
{
	if(m_pWebView)
	{
		m_pWebView->loadHTML(strHtml);
		m_render.render(m_pWebView);
	}
}

// 加载本地文件
void CDuiWkeView::loadFile(CString strFile)
{
	if(m_pWebView)
	{
		m_pWebView->loadFile(strFile);
		m_render.render(m_pWebView);
	}
}

// 停止加载页面
void CDuiWkeView::stopLoading()
{
	if(m_pWebView)
	{
		m_pWebView->stopLoading();
		m_render.render(m_pWebView);
	}
}

// 是否可以回退
bool CDuiWkeView::canGoBack()
{
	if(m_pWebView)
	{
		return m_pWebView->canGoBack();
	}
	return false;
}

// 回退
bool CDuiWkeView::goBack()
{
	if(m_pWebView)
	{
		return m_pWebView->goBack();
	}
	return false;
}

// 是否可以前进
bool CDuiWkeView::canGoForward()
{
	if(m_pWebView)
	{
		return m_pWebView->canGoForward();
	}
	return false;
}

// 前进
bool CDuiWkeView::goForward()
{
	if(m_pWebView)
	{
		return m_pWebView->goForward();
	}
	return false;
}

#endif // USE_WKE_CONTROL