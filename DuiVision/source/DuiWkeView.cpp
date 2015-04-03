#include "StdAfx.h"
#include "DuiWkeView.h"

#ifdef USE_WKE_CONTROL

#pragma comment(lib, "../DuiVision/third-part/wke/wke.lib")		// 自动链接wke库
#pragma message("Automatically linking with wke.lib")

static bool g_wkeInited = false;	// wke库是否已经初始化
static CPtrList	g_duiWkeViews;		// Wke视图对象列表

/////////////////////////////////////////////////////////////////////////////////////
// CDuiWkeView

CDuiWkeView::CDuiWkeView(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBase(hWnd, pDuiObject)
{
	m_hNativeWnd = NULL;
	m_OldWndProc = ::DefWindowProc;
	m_bCreated = false;
	m_bCreating = false;
	m_bDelayCreate = false;
	m_bTransparent = false;
	m_pWebView = NULL;
	m_strUrl = L"";

	CDuiWkeView::WkeInit();
	g_duiWkeViews.AddTail(this);
}

CDuiWkeView::~CDuiWkeView()
{
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

static void PixelToHiMetric(const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric)
{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
    int nPixelsPerInchX;    // Pixels per logical inch along width
    int nPixelsPerInchY;    // Pixels per logical inch along height
    HDC hDCScreen = ::GetDC(NULL);
    nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
    nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
    ::ReleaseDC(NULL, hDCScreen);
    lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, nPixelsPerInchX);
    lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, nPixelsPerInchY);
}

// 设置控件中的Windows原生控件是否可见的状态
void CDuiWkeView::SetControlWndVisible(BOOL bIsVisible)
{
	if( m_hNativeWnd != NULL )
	{
		::ShowWindow(m_hNativeWnd, bIsVisible ? SW_SHOW : SW_HIDE);
	}
}

void CDuiWkeView::SetControlRect(CRect rc) 
{
	m_rc = rc;

	if( !m_bCreated )
	{
		CreateControl();
	}

    SIZEL hmSize = { 0 };
    SIZEL pxSize = { 0 };
    pxSize.cx = m_rc.right - m_rc.left;
    pxSize.cy = m_rc.bottom - m_rc.top;
    PixelToHiMetric(&pxSize, &hmSize);

    if(m_hNativeWnd && ::IsWindow(m_hNativeWnd))
	{
		::MoveWindow(m_hNativeWnd, m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top, TRUE);
    }
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
    if( m_bDelayCreate == bDelayCreate ) return;
	CreateControl();
    m_bDelayCreate = bDelayCreate;
}

// 设置原生控件对象的窗口句柄
bool CDuiWkeView::SetNativeHWnd(HWND hWnd)
{
	if(hWnd == NULL)
	{
		return false;
	}

	ReleaseControl();
	m_hNativeWnd = hWnd;
	bool bRet = false;
	if(m_hNativeWnd && ::IsWindow(m_hNativeWnd) && m_pWebView)
	{
		SetControlWndVisible(FALSE);
		OnAttributePosChange(GetPosStr(), FALSE);
		bRet = true;
	}

	return bRet;
}

const LPCWSTR wkeWebViewClassName = L"class::wkeWebView";	// wke窗口类名

// 注册窗口类
bool CDuiWkeView::RegisterWindowClass()
{
    WNDCLASS wc = { 0 };
    wc.style = 0;
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
    if( !m_bDelayCreate )
	{
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
		DWORD dwExStyle = 0;
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		HWND hWnd = ::CreateWindowEx(dwExStyle, wkeWebViewClassName,
			L"wkeWebView", dwStyle,
			0, 0, 0, 0,
			m_hWnd, NULL,
			DuiSystem::Instance()->GetInstance(), this);

		// 设置窗口句柄
		SetNativeHWnd(hWnd);

		// 初始化wke渲染对象,将渲染对象关联到视图窗口
		m_render.init(m_hNativeWnd);

		m_bCreated = true;
		m_bCreating = false;
	}
    return true;
}

// 释放原生控件
void CDuiWkeView::ReleaseControl()
{
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
			//pThis->m_hNativeWnd = NULL;
			//pThis->m_render.destroy();
			//pThis->m_pWebView->destroy();
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
	if( !GetRresponse() ) //&& message >= WM_MOUSEFIRST && message <= WM_MOUSELAST )
	{
		return 0;
	}

    bool handled = true;
	switch (message)
	{
    case WM_COMMAND:
		::SendMessage(m_hWnd, message, wParam, lParam);
        return 0;

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
        }
        break;

    case WM_CHAR:
        {
            unsigned int charCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            //flags = HIWORD(lParam);

            handled = m_pWebView->keyPress(charCode, flags, false);
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
        }
        break;

    case WM_SETFOCUS:
        m_pWebView->focus();
        break;

    case WM_KILLFOCUS:
        m_pWebView->unfocus();
        break;
/*
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
*/
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
	}
}

// 加载页面内容
void CDuiWkeView::loadHTML(CString strHtml)
{
	if(m_pWebView)
	{
		m_pWebView->loadHTML(strHtml);
	}
}

// 加载本地文件
void CDuiWkeView::loadFile(CString strFile)
{
	if(m_pWebView)
	{
		m_pWebView->loadFile(strFile);
	}
}

// 停止加载页面
void CDuiWkeView::stopLoading()
{
	if(m_pWebView)
	{
		m_pWebView->stopLoading();
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