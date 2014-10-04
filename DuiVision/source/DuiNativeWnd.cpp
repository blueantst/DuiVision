#include "StdAfx.h"
#include "WndBase.h"
#include "DuiNativeWnd.h"


/////////////////////////////////////////////////////////////////////////////////////
// CDuiNativeWnd

CDuiNativeWnd::CDuiNativeWnd(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBase(hWnd, pDuiObject)
{
	m_hNativeWnd = NULL;
	m_pNativeWnd = NULL;
	m_bCreated = false;
	m_bDelayCreate = false;
}

CDuiNativeWnd::~CDuiNativeWnd()
{
    ReleaseControl();
}

HWND CDuiNativeWnd::GetNativeHWnd() const
{
    return m_hNativeWnd;
}

CWnd* CDuiNativeWnd::GetNativeWnd() const
{
    return m_pNativeWnd;
}

// ��ȡ�ؼ��ĸ����ھ��
HWND CDuiNativeWnd::GetPaintHWnd()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		return pDlg->GetSafeHwnd();
	}
    return NULL;
}

// ��ȡ�ؼ��ĸ����ڶ���
CWnd* CDuiNativeWnd::GetPaintWnd()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		return pDlg;
	}
    return NULL;
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

// ���ÿؼ��е�Windowsԭ���ؼ��Ƿ�ɼ���״̬
void CDuiNativeWnd::SetControlWndVisible(BOOL bIsVisible)
{
	if( m_hNativeWnd != NULL )
	{
		::ShowWindow(m_hNativeWnd, bIsVisible ? SW_SHOW : SW_HIDE);
	}
}

void CDuiNativeWnd::SetControlRect(CRect rc) 
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

// ��XML����DelayCreate����
HRESULT CDuiNativeWnd::OnAttributeDelayCreate(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	SetDelayCreate(strValue == _T("true"));

	return bLoading?S_FALSE:S_OK;
}

LRESULT CDuiNativeWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if((m_hNativeWnd == NULL) || !::IsWindow(m_hNativeWnd))
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

// �����ӳټ�������
void CDuiNativeWnd::SetDelayCreate(bool bDelayCreate)
{
    if( m_bDelayCreate == bDelayCreate ) return;
	CreateControl();
    m_bDelayCreate = bDelayCreate;
}

// ����ԭ���ؼ�����Ĵ��ھ��
bool CDuiNativeWnd::SetNativeHWnd(HWND hWnd)
{
	if(hWnd == NULL)
	{
		return false;
	}

	ReleaseControl();
	m_hNativeWnd = hWnd;
	bool bRet = false;
	if(m_hNativeWnd && ::IsWindow(m_hNativeWnd))
	{
		SetControlWndVisible(FALSE);
		OnAttributePosChange(GetPosStr(), FALSE);
		bRet = true;
	}

	return bRet;
}

// ����ԭ���ؼ�����ָ��
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

// ����ԭ���ؼ�
bool CDuiNativeWnd::CreateControl()
{
    m_bCreated = false;
    if( !m_bDelayCreate )
	{
		// ������Ϣ���¼�������,֪ͨ����ԭ���ؼ�
	}
    return true;
}

// �ͷ�ԭ���ؼ�
void CDuiNativeWnd::ReleaseControl()
{
	if(m_hNativeWnd && ::IsWindow(m_hNativeWnd))
	{
		DestroyWindow(m_hNativeWnd);
	}
    m_hNativeWnd = NULL;

	if(m_pNativeWnd != NULL)
	{
		delete m_pNativeWnd;
		m_pNativeWnd = NULL;
	}
}

// �ؼ���ͼ����
void CDuiNativeWnd::DrawControl(CDC &dc, CRect rcUpdate)
{
	if( !::IntersectRect(&rcUpdate, &rcUpdate, &m_rc) ) return;

	if(m_hNativeWnd && ::IsWindow(m_hNativeWnd))
    {
		// ����ԭ���ؼ��Ļ�ͼ(����WM_PAINT��Ϣ���ؼ�)
		//m_pWindowWnd->DrawControl(dc, rcUpdate);
    }
}