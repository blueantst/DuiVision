#include "StdAfx.h"
#include "DuiWkeView.h"
#include <Imm.h>

#pragma comment(lib, "imm32.lib")		// �Զ�����imm��
#pragma message("Automatically linking with imm32.lib")

//#pragma comment(lib, "../DuiVision/third-part/wke/wke.lib")		// �Զ�����wke��
//#pragma message("Automatically linking with wke.lib")

static bool g_wkeInited = false;	// wke���Ƿ��Ѿ���ʼ��
static CPtrList	g_duiWkeViews;		// Wke��ͼ�����б�

/////////////////////////////////////////////////////////////////////////////////////
// CDuiWkeView

CDuiWkeView::CDuiWkeView(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBase(hWnd, pDuiObject)
{
	m_bTabStop = TRUE;	// ������Ӧtab��
	m_OldWndProc = ::DefWindowProc;
	m_bCreated = false;
	m_bCreating = false;
	m_bDelayCreate = false;
	m_bTransparent = false;
	m_pWebView = NULL;
	m_strUrl = _T("");
	m_strHtml = _T("");

	CDuiWkeView::WkeInit();
	g_duiWkeViews.AddTail(this);
}

CDuiWkeView::~CDuiWkeView()
{
	if(m_hwndHost != NULL)
	{
		::DestroyWindow(m_hwndHost);
	}

	// ɾ��wke�����б��е���
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

// wke���ʼ��
void CDuiWkeView::WkeInit()
{
	if(g_wkeInited)
	{
		return;
	}

    wkeInit();

	g_wkeInited = true;
}

// wke���ͷ�
void CDuiWkeView::WkeShutdown()
{
	wkeShutdown();
}

// ���ݻص�ָ���ȡwke����ָ��
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

// ��ȡwke��ͼָ��
wkeWebView CDuiWkeView::GetWkeWebView()
{
	return m_pWebView;
}

// ��ȡwke�ص�ָ��
wkeClientHandler* CDuiWkeView::GetWkeClientHandler()
{
	return &m_wkeHander;
}

// ���ÿؼ��е�Windowsԭ���ؼ��Ƿ�ɼ���״̬
void CDuiWkeView::SetControlWndVisible(BOOL bIsVisible)
{
	if( m_hwndHost != NULL )
	{
		::ShowWindow(m_hwndHost, bIsVisible ? SW_SHOW : SW_HIDE);
	}
}

// ���ÿؼ���λ��
void CDuiWkeView::SetControlRect(CRect rc) 
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

// ���õ�ǰ����ؼ�
void CDuiWkeView::SetCurrentFocusControl(BOOL bFocus) 
{
	CDlgBase* pDlg = GetParentDialog(FALSE);
	if(pDlg)
	{
		pDlg->SetFocusControl(bFocus ? this : NULL);
	}
}

// ��XML����DelayCreate����
HRESULT CDuiWkeView::OnAttributeDelayCreate(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	SetDelayCreate(strValue == _T("true"));

	return bLoading?S_FALSE:S_OK;
}

// ��XML����url����
HRESULT CDuiWkeView::OnAttributeUrl(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_strUrl = strValue;
	Navigate(m_strUrl);

	return bLoading?S_FALSE:S_OK;
}

bool CDuiWkeView::IsDelayCreate() const
{
    return m_bDelayCreate;
}

// �����ӳټ�������
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

const LPCTSTR wkeWebViewClassName = _T("class::wkeWebView");	// wke��������

// ע�ᴰ����
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

// ����wke�ؼ�
bool CDuiWkeView::CreateControl()
{
	if(m_bCreating)
	{
		return false;
	}

    m_bCreated = false;
	m_bCreating = true;

	// ����wke�¼��ص�����
	m_wkeHander.onTitleChanged = onTitleChanged;
	m_wkeHander.onURLChanged = onURLChanged;

	// ����wke��ͼ,������url��html����
	m_pWebView = wkeCreateWebView();
	m_pWebView->setTransparent(m_bTransparent);
	m_pWebView->setClientHandler(&m_wkeHander);
	m_pWebView->setBufHandler(this);
	if(!m_strUrl.IsEmpty())
	{
		Navigate(m_strUrl);
	}else
	if(!m_strHtml.IsEmpty())
	{
		loadHTML(m_strHtml);
	}

	// ע�ᴰ����
	RegisterWindowClass();

	// ����������ʾwke��ͼ�Ĵ���
	HWND hWnd = ::CreateWindow(wkeWebViewClassName, 0, 
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, 0, 0, 
        m_hWnd, 
        0, 
        DuiSystem::Instance()->GetInstance(), this);

	// ���ô��ھ��
	if(hWnd && ::IsWindow(hWnd))
	{
		ReleaseControl();
		m_hwndHost = hWnd;
	}

	// ��ʼ��wke��Ⱦ����,����Ⱦ�����������ͼ����
	m_render.init(m_hwndHost);

	m_bCreated = true;
	m_bCreating = false;

    return true;
}

// �ͷſؼ�
void CDuiWkeView::ReleaseControl()
{
	// ����ǽ���ؼ�,�򽫵�ǰ����ؼ�����Ϊ��
	if(IsFocusControl())
	{
		SetCurrentFocusControl(NULL);
	}
	// ɾ����������
	if(m_hwndHost && ::IsWindow(m_hwndHost))
	{
		DestroyWindow(m_hwndHost);
	}
    m_hwndHost = NULL;
}

// �ؼ���ͼ����
void CDuiWkeView::DrawControl(CDC &dc, CRect rcUpdate)
{
	if( !::IntersectRect(&rcUpdate, &rcUpdate, &m_rc) ) return;

	if(m_hwndHost && ::IsWindow(m_hwndHost) && m_pWebView)
    {
		// ǿ��wke��ͼˢ��
		m_pWebView->setDirty(true);
		// wke��Ⱦ
		m_render.render(m_pWebView);
    }
}

// �ؼ���30���붨ʱ��
BOOL CDuiWkeView::OnControlTimer()
{
	if(!m_bIsVisible || !m_bCreated)
	{
		return FALSE;
	}

	// ��wke���ڷ��Ͷ�ʱ����Ϣ,����ˢ��ҳ��
	::SendMessage(m_hwndHost, WM_TIMER, 0, 0);

	return TRUE;
}

// �����¼�����
BOOL CDuiWkeView::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// �����ǰ���ڽ���״̬,����س�����ESC��
	if(IsFocusControl() && ((nChar == VK_RETURN) || (nChar == VK_ESCAPE)) && (nFlags == 0) && m_pWebView)
	{
		//m_pWebView->keyDown(nChar, nFlags, false);
		m_pWebView->keyPress(nChar, nFlags, false);
		m_render.render(m_pWebView);
		return true;
	}

	return false;
}

// ����WM_KEYDOWN��Ϣ
LRESULT CDuiWkeView::OnWmKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool handled = true;
	unsigned int virtualKeyCode = wParam;
    unsigned int flags = 0;
	bool bShiftState = false;
    if (HIWORD(lParam) & KF_REPEAT)
        flags |= WKE_REPEAT;
    if (HIWORD(lParam) & KF_EXTENDED)
        flags |= WKE_EXTENDED;
	if((GetKeyState(VK_SHIFT) & 0x8000) || (VK_SHIFT == wParam))
		bShiftState = true;
	HKL hKL = GetKeyboardLayout(0);
	int i = LOWORD(hKL);
	TCHAR buffer[255];   
	memset(buffer,0,255 * sizeof(TCHAR));  
	if( (i == 0x0804) && (ImmIsIME(hKL)) )
	{
		ImmGetDescription(hKL,buffer,255);
		if(buffer[0] != NULL)
		{
			if(( wParam & 0x80 )  |  (wParam >= 0x70  && wParam <= 0x7F  ))
			{
				if( !IsFocusControl() ) return 0;
			}	
		}
	}	
	if(wParam >= 0x70  && wParam <= 0x7F  )
	{
		handled = m_pWebView->keyDown(virtualKeyCode, flags, false);
		m_render.render(m_pWebView);
		if( wParam == 0x74 )
		{
			MSG msg;
			msg.hwnd = hWnd;
			msg.message = WM_CHAR;
			msg.wParam = virtualKeyCode;
			msg.lParam = lParam;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);			 
		}
		if( !IsFocusControl() ) return 0;
	}

    handled = m_pWebView->keyDown(virtualKeyCode, flags, false);
	m_render.render(m_pWebView);
	
	if(!bShiftState)
	{
		if( !GetKeyState(VK_CAPITAL)  )
		{
			if((wParam >= 'A') && (wParam <= 'Z'))
				virtualKeyCode = tolower(wParam);
		}
		if(wParam != VK_SHIFT)
		{
			if( wParam > 0x80 )
				return 0;
		}
	}else
	{	
		if(wParam != VK_SHIFT)
		{
			//wParam = wParam & 0x7F;//`~
			//virtualKeyCode = wParam;
			if(wParam == '1' || wParam == '3' || wParam == '4' || wParam == '5' )
				virtualKeyCode = wParam - 0x10;
			else if(wParam == '2')
				virtualKeyCode = '@';
			else if(wParam == '6')
				virtualKeyCode = '^';
			else if(wParam == '7')
				virtualKeyCode = '&';
			else if( wParam == '8')
				virtualKeyCode = '*' ;
			else if( wParam == '9')
				virtualKeyCode = '(';
			else if(wParam == '0')
				virtualKeyCode = ')';
			else if(wParam == 45)
				virtualKeyCode = '_';
			else if(wParam == '=')
				virtualKeyCode = '+';
			else if(wParam == '[')
				virtualKeyCode = '{';
			else if(wParam == ']')
				virtualKeyCode = '}';
			else if(wParam == ';')
				virtualKeyCode = ':';
			else if(wParam == '\'')
				virtualKeyCode = '"';
			else if(wParam == '\\')
				virtualKeyCode = '|';
			else if(wParam == ',')
				virtualKeyCode = '<';
			else if(wParam == '.')
				virtualKeyCode = '>';
			else if(wParam == '/')
				virtualKeyCode = '?';
			else if(wParam == '`')
				virtualKeyCode = '~';
			else if(wParam > 0x80)
				return 0;
		}
	}	
	MSG msg;
	msg.hwnd = hWnd;
	msg.message = WM_CHAR;
	msg.wParam = virtualKeyCode;
	msg.lParam = lParam;
	::TranslateMessage(&msg);
	::DispatchMessage(&msg);

	return 0;
}

void CDuiWkeView::onBufUpdated( const HDC hdc,int x, int y, int cx, int cy )
{
	RECT rcClient;
	GetClientRect(m_hWnd,&rcClient);
	RECT rcInvalid = {rcClient.left + x, rcClient.top + y, rcClient.left + x + cx, rcClient.top + y + cy};
	::InvalidateRect(m_hWnd, &rcInvalid ,TRUE);
}

// ������Ϣ������
LRESULT CALLBACK CDuiWkeView::__WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CDuiWkeView* pThis = NULL;
    if( message == WM_NCCREATE )
	{
		// ���ڴ���ʱ�򱣴浱ǰ����ָ��(����ʱ��ͨ�����������)
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CDuiWkeView*>(lpcs->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
    }else
	{
		// ͨ�����ڱ����ָ���ȡ����ָ��
        pThis = reinterpret_cast<CDuiWkeView*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if( message == WM_DESTROY && pThis != NULL )
		{
			// ��������
            //LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, message, wParam, lParam);
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, 0L);
			pThis->ReleaseControl();
            return 0;//lRes;
        }
    }

	// �����ȡ������ָ��,�����wke��Ϣ������,�������Ĭ�ϴ�����Ϣ������
    if( pThis != NULL )
	{
        return pThis->WebViewWindowProc(hWnd, message, wParam, lParam);
    }else
	{
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// wke���ڵ�windows��Ϣ����
LRESULT CDuiWkeView::WebViewWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( !GetRresponse() )
	{
		return 0;
	}

	// ���ڼ�����Ϣ,����˿ؼ����ǽ���ؼ�,���˳�
	if( message >= WM_KEYFIRST && message <= WM_KEYLAST )
	{
        //if( !IsFocusControl() ) return 0;
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
			OnWmKeyDown(hWnd, message, wParam, lParam);
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
		// ���öԻ���Ľ���ؼ�Ϊ�˿ؼ�
		SetCurrentFocusControl(TRUE);
        break;

    case WM_KILLFOCUS:
        m_pWebView->unfocus();
		m_render.render(m_pWebView);
		// ����Ի���Ľ���ؼ�
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

// URL����Ļص�����
void CDuiWkeView::onURLChanged(const struct _wkeClientHandler* clientHandler, const wkeString URL)
{
	// ���Ҷ�Ӧ�Ŀؼ�����
	CDuiWkeView* pDuiWkeView = CDuiWkeView::GetWkeViewByClientHandler(clientHandler);
	if(pDuiWkeView)
	{
#ifdef _UNICODE
		pDuiWkeView->setURL(wkeToStringW(URL));
		pDuiWkeView->SendMessage(MSG_CONTROL_EVENT, (WPARAM)WKE_EVENT_URLCHANGED, (LPARAM)wkeToStringW(URL));
#else
		const wchar_t *pWchar=wkeToStringW(URL);
		CStringA strA=CEncodingUtil::UnicodeToAnsi(pWchar);
		pDuiWkeView->setURL(strA);
		pDuiWkeView->SendMessage(MSG_CONTROL_EVENT, (WPARAM)WKE_EVENT_URLCHANGED, (LPARAM)(LPCTSTR)(strA));
#endif
	}
}

// ҳ��������Ļص�����
void CDuiWkeView::onTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString title)
{
	// ���Ҷ�Ӧ�Ŀؼ�����
	CDuiWkeView* pDuiWkeView = CDuiWkeView::GetWkeViewByClientHandler(clientHandler);
	if(pDuiWkeView)
	{
		pDuiWkeView->SendMessage(MSG_CONTROL_EVENT, (WPARAM)WKE_EVENT_TITLECHANGED, (LPARAM)wkeToStringW(title));
	}
}

// ��ȡ��ҳ����
CString CDuiWkeView::getTitle()
{
	if(m_pWebView)
	{
#ifdef _UNICODE
		return m_pWebView->titleW();
#else
		const wchar_t *pWstr=m_pWebView->titleW();
		return CEncodingUtil::UnicodeToAnsi(pWstr);
#endif
	}

	return _T("");
}

// ��ȡ��ҳURL
CString CDuiWkeView::getURL()
{
	return m_strUrl;
}

// ������ҳURL
void CDuiWkeView::setURL(CString strUrl)
{
	m_strUrl = strUrl;
}

// URL����
void CDuiWkeView::Navigate(CString strUrl)
{
	if(m_pWebView)
	{
		if(strUrl.Find(_T("file://")) == 0)
		{
			CString strFile = strUrl;
			strFile.Delete(0, 7);
			if(strFile.Find(_T(":")) == -1)
			{
				strFile = DuiSystem::GetSkinPath() + strFile;
			}
			m_pWebView->loadFile(strFile);
		}else
		{
			m_pWebView->loadURL(strUrl);
		}
		m_render.render(m_pWebView);
	}
}

// ����ҳ������
void CDuiWkeView::loadHTML(CString strHtml)
{
	if(m_pWebView)
	{
		m_pWebView->loadHTML(strHtml);
		m_render.render(m_pWebView);
	}
}

// ���ر����ļ�
void CDuiWkeView::loadFile(CString strFile)
{
	if(m_pWebView)
	{
		m_pWebView->loadFile(strFile);
		m_render.render(m_pWebView);
	}
}

// ֹͣ����ҳ��
void CDuiWkeView::stopLoading()
{
	if(m_pWebView)
	{
		m_pWebView->stopLoading();
		m_render.render(m_pWebView);
	}
}

// �Ƿ���Ի���
bool CDuiWkeView::canGoBack()
{
	if(m_pWebView)
	{
		return m_pWebView->canGoBack();
	}
	return false;
}

// ����
bool CDuiWkeView::goBack()
{
	if(m_pWebView)
	{
		return m_pWebView->goBack();
	}
	return false;
}

// �Ƿ����ǰ��
bool CDuiWkeView::canGoForward()
{
	if(m_pWebView)
	{
		return m_pWebView->canGoForward();
	}
	return false;
}

// ǰ��
bool CDuiWkeView::goForward()
{
	if(m_pWebView)
	{
		return m_pWebView->goForward();
	}
	return false;
}
