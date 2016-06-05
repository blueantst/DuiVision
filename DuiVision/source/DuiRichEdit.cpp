#include "stdafx.h"
#include "DuiRichEdit.h"

#include <Imm.h>

#pragma comment(lib, "imm32.lib")		// 自动链接imm库
#pragma message("Automatically linking with imm32.lib")

// These constants are for backward compatibility. They are the 
// sizes used for initialization and reset in RichEdit 1.0

const LONG cInitTextMax = (32 * 1024) - 1;

EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

#include <textserv.h>

HRESULT FnCreateTextServices(
						IUnknown *punkOuter,
						ITextHost *pITextHost,
						IUnknown **ppUnk);

class CTxtWinHost : public ITextHost
{
public:
    CTxtWinHost();
    BOOL Init(CDuiRichEdit *re , const CREATESTRUCT *pcs);
    virtual ~CTxtWinHost();

    ITextServices* GetTextServices(void) { return pserv; }
    void SetClientRect(RECT *prc);
    RECT* GetClientRect() { return &rcClient; }
    BOOL GetWordWrap(void) { return fWordWrap; }
    void SetWordWrap(BOOL fWordWrap);
    BOOL GetReadOnly();
    void SetReadOnly(BOOL fReadOnly);
    void SetFont(HFONT hFont);
    void SetColor(COLORREF color);
    SIZEL* GetExtent();
    void SetExtent(SIZEL *psizelExtent);
    void LimitText(LONG nChars);
    BOOL IsCaptured();

    BOOL GetAllowBeep();
    void SetAllowBeep(BOOL fAllowBeep);
    WORD GetDefaultAlign();
    void SetDefaultAlign(WORD wNewAlign);
    BOOL GetRichTextFlag();
    void SetRichTextFlag(BOOL fNew);
    LONG GetDefaultLeftIndent();
    void SetDefaultLeftIndent(LONG lNewIndent);
    BOOL SetSaveSelection(BOOL fSaveSelection);
    HRESULT OnTxInPlaceDeactivate();
    HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
    BOOL GetActiveState(void) { return fInplaceActive; }
    BOOL DoSetCursor(RECT *prc, POINT *pt);
    void SetTransparent(BOOL fTransparent);
    void GetControlRect(LPRECT prc);
    LONG SetAccelPos(LONG laccelpos);
    WCHAR SetPasswordChar(WCHAR chPasswordChar);
    void SetDisabled(BOOL fOn);
    LONG SetSelBarWidth(LONG lSelBarWidth);
    BOOL GetTimerState();

    void SetCharFormat(CHARFORMAT2W &c);
    void SetParaFormat(PARAFORMAT2 &p);

    // -----------------------------
    //	IUnknown interface
    // -----------------------------
    virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG _stdcall AddRef(void);
    virtual ULONG _stdcall Release(void);

    // -----------------------------
    //	ITextHost interface
    // -----------------------------
    virtual HDC TxGetDC();
    virtual INT TxReleaseDC(HDC hdc);
    virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
    virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
    virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
    virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
    virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
    virtual void TxViewChange(BOOL fUpdate);
    virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    virtual BOOL TxShowCaret(BOOL fShow);
    virtual BOOL TxSetCaretPos(INT x, INT y);
    virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
    virtual void TxKillTimer(UINT idTimer);
    virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
    virtual void TxSetCapture(BOOL fCapture);
    virtual void TxSetFocus();
    virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
    virtual BOOL TxScreenToClient (LPPOINT lppt);
    virtual BOOL TxClientToScreen (LPPOINT lppt);
    virtual HRESULT TxActivate( LONG * plOldState );
    virtual HRESULT TxDeactivate( LONG lNewState );
    virtual HRESULT TxGetClientRect(LPRECT prc);
    virtual HRESULT TxGetViewInset(LPRECT prc);
    virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
    virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
    virtual COLORREF TxGetSysColor(int nIndex);
    virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
    virtual HRESULT TxGetMaxLength(DWORD *plength);
    virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
    virtual HRESULT TxGetPasswordChar(TCHAR *pch);
    virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
    virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
    virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
    virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
    virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
    virtual HRESULT TxNotify(DWORD iNotify, void *pv);
    virtual HIMC TxImmGetContext(void);
    virtual void TxImmReleaseContext(HIMC himc);
    virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

private:
    CDuiRichEdit*	m_re;
    ULONG			cRefs;					// Reference Count
    ITextServices*	pserv;					// pointer to Text Services object
    // Properties
    DWORD			dwStyle;				// style bits

    unsigned		fEnableAutoWordSel	:1;	// enable Word style auto word selection?
    unsigned		fWordWrap			:1;	// Whether control should word wrap
    unsigned		fAllowBeep			:1;	// Whether beep is allowed
    unsigned		fRich				:1;	// Whether control is rich text
    unsigned		fSaveSelection		:1;	// Whether to save the selection when inactive
    unsigned		fInplaceActive		:1; // Whether control is inplace active
    unsigned		fTransparent		:1; // Whether control is transparent
    unsigned		fTimer				:1;	// A timer is set
    unsigned		fCaptured           :1;

    LONG			lSelBarWidth;			// Width of the selection bar
    LONG  			cchTextMost;			// maximum text size
    DWORD			dwEventMask;			// DoEvent mask to pass on to parent window
    LONG			icf;
    LONG			ipf;
    RECT			rcClient;				// Client Rect for this control
    SIZEL			sizelExtent;			// Extent array
    CHARFORMAT2W	cf;						// Default character format
    PARAFORMAT2		pf;					    // Default paragraph format
    LONG			laccelpos;				// Accelerator position
    WCHAR			chPasswordChar;		    // Password character
};

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
    return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
    return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

HRESULT InitDefaultCharFormat(CDuiRichEdit* re, CHARFORMAT2W* pcf, HFONT hfont) 
{
    memset(pcf, 0, sizeof(CHARFORMAT2W));
    LOGFONT lf;
    if( !hfont )
	{
		hfont = re->GetDefaultFont();
	}
    ::GetObject(hfont, sizeof(LOGFONT), &lf);

    Color color = re->GetTextColor();
    pcf->cbSize = sizeof(CHARFORMAT2W);
    pcf->crTextColor = color.ToCOLORREF();
	HDC hDCScreen = ::GetDC(NULL);
    LONG yPixPerInch = GetDeviceCaps(hDCScreen, LOGPIXELSY);
    pcf->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    pcf->yOffset = 0;
    pcf->dwEffects = 0;
    pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
    if(lf.lfWeight >= FW_BOLD)
        pcf->dwEffects |= CFE_BOLD;
    if(lf.lfItalic)
        pcf->dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline)
        pcf->dwEffects |= CFE_UNDERLINE;
    pcf->bCharSet = lf.lfCharSet;
    pcf->bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
    _tcscpy(pcf->szFaceName, lf.lfFaceName);
#else
    //need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE) ;
#endif

    return S_OK;
}

HRESULT InitDefaultParaFormat(CDuiRichEdit* re, PARAFORMAT2* ppf) 
{	
    memset(ppf, 0, sizeof(PARAFORMAT2));
    ppf->cbSize = sizeof(PARAFORMAT2);
    ppf->dwMask = PFM_ALL;
    ppf->wAlignment = PFA_LEFT;
    ppf->cTabCount = 1;
    ppf->rgxTabs[0] = lDefaultTab;

    return S_OK;
}

HRESULT CreateHost(CDuiRichEdit *re, const CREATESTRUCT *pcs, CTxtWinHost **pptec)
{
    HRESULT hr = E_FAIL;
    //GdiSetBatchLimit(1);

    CTxtWinHost *phost = new CTxtWinHost();
    if(phost)
    {
        if (phost->Init(re, pcs))
        {
            *pptec = phost;
            hr = S_OK;
        }
    }

    if (FAILED(hr))
    {
        delete phost;
    }

    return TRUE;
}

CTxtWinHost::CTxtWinHost() : m_re(NULL)
{
    ::ZeroMemory(&cRefs, sizeof(CTxtWinHost) - offsetof(CTxtWinHost, cRefs));
    cchTextMost = cInitTextMax;
    laccelpos = -1;
}

CTxtWinHost::~CTxtWinHost()
{
    pserv->OnTxInPlaceDeactivate();
    pserv->Release();
}

////////////////////// Create/Init/Destruct Commands ///////////////////////

BOOL CTxtWinHost::Init(CDuiRichEdit *re, const CREATESTRUCT *pcs)
{
    IUnknown *pUnk;
    HRESULT hr;

    m_re = re;
    // Initialize Reference count
    cRefs = 1;

    // Create and cache CHARFORMAT for this control
    if(FAILED(InitDefaultCharFormat(re, &cf, NULL)))
        goto err;

    // Create and cache PARAFORMAT for this control
    if(FAILED(InitDefaultParaFormat(re, &pf)))
        goto err;

    // edit controls created without a window are multiline by default
    // so that paragraph formats can be
    dwStyle = ES_MULTILINE;

    // edit controls are rich by default
    fRich = re->IsRich();

    cchTextMost = re->GetLimitText();

    if (pcs )
    {
        dwStyle = pcs->style;

        if ( !(dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)) )
        {
            fWordWrap = TRUE;
        }
    }

    if( !(dwStyle & ES_LEFT) )
    {
        if(dwStyle & ES_CENTER)
            pf.wAlignment = PFA_CENTER;
        else if(dwStyle & ES_RIGHT)
            pf.wAlignment = PFA_RIGHT;
    }

    fInplaceActive = TRUE;

	CDuiRichEdit::InitTextService();
	hr = FnCreateTextServices(NULL, this, &pUnk);
	if(hr == S_FALSE)
	{
		goto err;
	}

    hr = pUnk->QueryInterface(IID_ITextServices,(void **)&pserv);

    // Whether the previous call succeeded or failed we are done
    // with the private interface.
    pUnk->Release();

    if(FAILED(hr))
    {
        goto err;
    }

    // Set window text
    if(pcs && pcs->lpszName)
    {
#ifdef _UNICODE		
        if(FAILED(pserv->TxSetText((TCHAR *)pcs->lpszName)))
            goto err;
#else
        size_t iLen = _tcslen(pcs->lpszName);
        LPWSTR lpText = new WCHAR[iLen + 1];
        ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, pcs->lpszName, -1, (LPWSTR)lpText, iLen) ;
        if(FAILED(pserv->TxSetText((LPWSTR)lpText))) {
            delete[] lpText;
            goto err;
        }
        delete[] lpText;
#endif
    }

    return TRUE;

err:
    return FALSE;
}

/////////////////////////////////  IUnknown ////////////////////////////////


HRESULT CTxtWinHost::QueryInterface(REFIID riid, void **ppvObject)
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) 
        || IsEqualIID(riid, IID_ITextHost)) 
    {
        AddRef();
        *ppvObject = (ITextHost *) this;
        hr = S_OK;
    }

    return hr;
}

ULONG CTxtWinHost::AddRef(void)
{
    return ++cRefs;
}

ULONG CTxtWinHost::Release(void)
{
    ULONG c_Refs = --cRefs;

    if (c_Refs == 0)
    {
        delete this;
    }

    return c_Refs;
}

/////////////////////////////////  Far East Support  //////////////////////////////////////

HIMC CTxtWinHost::TxImmGetContext(void)
{
    //return NULL;
	return ImmGetContext(m_re->GetHWND());
}

void CTxtWinHost::TxImmReleaseContext(HIMC himc)
{
    //::ImmReleaseContext( hwnd, himc );
	ImmReleaseContext(m_re->GetHWND(), himc);
}

//////////////////////////// ITextHost Interface  ////////////////////////////

HDC CTxtWinHost::TxGetDC()
{
	/*CDlgBase* pDlg = m_re->GetParentDialog();
	if(pDlg)
	{
		return ::GetDC(pDlg->m_hWnd);
	}
    return NULL;*/
	return ::GetDC(NULL);
}

int CTxtWinHost::TxReleaseDC(HDC hdc)
{
    //return 1;
	return ::ReleaseDC(NULL,hdc);
}

BOOL CTxtWinHost::TxShowScrollBar(INT fnBar, BOOL fShow)
{
    /*CScrollBarUI* pVerticalScrollBar = m_re->GetVerticalScrollBar();
    CScrollBarUI* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if( fnBar == SB_VERT && pVerticalScrollBar ) {
        pVerticalScrollBar->SetVisible(fShow == TRUE);
    }
    else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
        pHorizontalScrollBar->SetVisible(fShow == TRUE);
    }
    else if( fnBar == SB_BOTH ) {
        if( pVerticalScrollBar ) pVerticalScrollBar->SetVisible(fShow == TRUE);
        if( pHorizontalScrollBar ) pHorizontalScrollBar->SetVisible(fShow == TRUE);
    }*/
    return TRUE;
}

BOOL CTxtWinHost::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
    /*if( fuSBFlags == SB_VERT ) {
        m_re->EnableScrollBar(true, m_re->GetHorizontalScrollBar() != NULL);
        m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }
    else if( fuSBFlags == SB_HORZ ) {
        m_re->EnableScrollBar(m_re->GetVerticalScrollBar() != NULL, true);
        m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }
    else if( fuSBFlags == SB_BOTH ) {
        m_re->EnableScrollBar(true, true);
        m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
        m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }*/
    return TRUE;
}

BOOL CTxtWinHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
    /*CScrollBarUI* pVerticalScrollBar = m_re->GetVerticalScrollBar();
    CScrollBarUI* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if( fnBar == SB_VERT && pVerticalScrollBar ) {
        if( nMaxPos - nMinPos - rcClient.bottom + rcClient.top <= 0 ) {
            pVerticalScrollBar->SetVisible(false);
        }
        else {
            pVerticalScrollBar->SetVisible(true);
            pVerticalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.bottom + rcClient.top);
        }
    }
    else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
        if( nMaxPos - nMinPos - rcClient.right + rcClient.left <= 0 ) {
            pHorizontalScrollBar->SetVisible(false);
        }
        else {
            pHorizontalScrollBar->SetVisible(true);
            pHorizontalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.right + rcClient.left);
        }   
    }*/
    return TRUE;
}

BOOL CTxtWinHost::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
{
    /*CScrollBarUI* pVerticalScrollBar = m_re->GetVerticalScrollBar();
    CScrollBarUI* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if( fnBar == SB_VERT && pVerticalScrollBar ) {
        pVerticalScrollBar->SetScrollPos(nPos);
    }
    else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
        pHorizontalScrollBar->SetScrollPos(nPos);
    }*/
    return TRUE;
}

void CTxtWinHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
    if( prc == NULL ) {
        m_re->InvalidateRect(&rcClient);
        return;
    }
    RECT rc = *prc;
    m_re->InvalidateRect(&rc);
}

void CTxtWinHost::TxViewChange(BOOL fUpdate) 
{
    if( m_re->OnTxViewChanged() ) m_re->UpdateControl(true);
}

BOOL CTxtWinHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
    return ::CreateCaret(m_re->GetHWND(), hbmp, xWidth, yHeight);
}

BOOL CTxtWinHost::TxShowCaret(BOOL fShow)
{
    if(fShow)
        return ::ShowCaret(m_re->GetHWND());
    else
        return ::HideCaret(m_re->GetHWND());
}

BOOL CTxtWinHost::TxSetCaretPos(INT x, INT y)
{
    return ::SetCaretPos(x, y);
}

BOOL CTxtWinHost::TxSetTimer(UINT idTimer, UINT uTimeout)
{
    fTimer = TRUE;
    //return m_re->SetTimer(m_re, idTimer, uTimeout) == TRUE;
	return FALSE;
}

void CTxtWinHost::TxKillTimer(UINT idTimer)
{
    //m_re->KillTimer(m_re, idTimer);
    fTimer = FALSE;
}

void CTxtWinHost::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
{
    return;
}

void CTxtWinHost::TxSetCapture(BOOL fCapture)
{
    if (fCapture)
		m_re->OnFocus(TRUE);
    else
		m_re->OnFocus(FALSE);
    fCaptured = fCapture;
}

void CTxtWinHost::TxSetFocus()
{
    m_re->OnFocus(TRUE);
}

void CTxtWinHost::TxSetCursor(HCURSOR hcur,	BOOL fText)
{
    ::SetCursor(hcur);
}

BOOL CTxtWinHost::TxScreenToClient(LPPOINT lppt)
{
    return ::ScreenToClient(m_re->GetHWND(), lppt);	
}

BOOL CTxtWinHost::TxClientToScreen(LPPOINT lppt)
{
    return ::ClientToScreen(m_re->GetHWND(), lppt);
}

HRESULT CTxtWinHost::TxActivate(LONG *plOldState)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxDeactivate(LONG lNewState)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxGetClientRect(LPRECT prc)
{
    *prc = rcClient;
    GetControlRect(prc);
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetViewInset(LPRECT prc) 
{
    prc->left = prc->right = prc->top = prc->bottom = 0;
    return NOERROR;	
}

HRESULT CTxtWinHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
    *ppCF = &cf;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetParaFormat(const PARAFORMAT **ppPF)
{
    *ppPF = &pf;
    return NOERROR;
}

COLORREF CTxtWinHost::TxGetSysColor(int nIndex) 
{
    return ::GetSysColor(nIndex);
}

HRESULT CTxtWinHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    *pstyle = !fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetMaxLength(DWORD *pLength)
{
    *pLength = cchTextMost;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetScrollBars(DWORD *pdwScrollBar)
{
    *pdwScrollBar =  dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
        ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return NOERROR;
}

HRESULT CTxtWinHost::TxGetPasswordChar(TCHAR *pch)
{
#ifdef _UNICODE
    *pch = chPasswordChar;
#else
    ::WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetAcceleratorPos(LONG *pcp)
{
    *pcp = laccelpos;
    return S_OK;
} 										   

HRESULT CTxtWinHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
{
    return S_OK;
}

HRESULT CTxtWinHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
{
    DWORD dwProperties = 0;

    if (fRich)
    {
        dwProperties = TXTBIT_RICHTEXT;
    }

    if (dwStyle & ES_MULTILINE)
    {
        dwProperties |= TXTBIT_MULTILINE;
    }

    if (dwStyle & ES_READONLY)
    {
        dwProperties |= TXTBIT_READONLY;
    }

    if (dwStyle & ES_PASSWORD)
    {
        dwProperties |= TXTBIT_USEPASSWORD;
    }

    if (!(dwStyle & ES_NOHIDESEL))
    {
        dwProperties |= TXTBIT_HIDESELECTION;
    }

    if (fEnableAutoWordSel)
    {
        dwProperties |= TXTBIT_AUTOWORDSEL;
    }

    if (fWordWrap)
    {
        dwProperties |= TXTBIT_WORDWRAP;
    }

    if (fAllowBeep)
    {
        dwProperties |= TXTBIT_ALLOWBEEP;
    }

    if (fSaveSelection)
    {
        dwProperties |= TXTBIT_SAVESELECTION;
    }

    *pdwBits = dwProperties & dwMask; 
    return NOERROR;
}


HRESULT CTxtWinHost::TxNotify(DWORD iNotify, void *pv)
{
    if( iNotify == EN_REQUESTRESIZE ) {
        RECT rc;
        REQRESIZE *preqsz = (REQRESIZE *)pv;
        GetControlRect(&rc);
        rc.bottom = rc.top + preqsz->rc.bottom;
        rc.right  = rc.left + preqsz->rc.right;
        SetClientRect(&rc);
    }
    m_re->OnTxNotify(iNotify, pv);
    return S_OK;
}

HRESULT CTxtWinHost::TxGetExtent(LPSIZEL lpExtent)
{
    *lpExtent = sizelExtent;
    return S_OK;
}

HRESULT	CTxtWinHost::TxGetSelectionBarWidth (LONG *plSelBarWidth)
{
    *plSelBarWidth = lSelBarWidth;
    return S_OK;
}

void CTxtWinHost::SetWordWrap(BOOL fWordWrap)
{
    fWordWrap = fWordWrap;
    pserv->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
}

BOOL CTxtWinHost::GetReadOnly()
{
    return (dwStyle & ES_READONLY) != 0;
}

void CTxtWinHost::SetReadOnly(BOOL fReadOnly)
{
    if (fReadOnly)
    {
        dwStyle |= ES_READONLY;
    }
    else
    {
        dwStyle &= ~ES_READONLY;
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, 
        fReadOnly ? TXTBIT_READONLY : 0);
}

void CTxtWinHost::SetFont(HFONT hFont) 
{
    if( hFont == NULL ) return;
    LOGFONT lf;
    ::GetObject(hFont, sizeof(LOGFONT), &lf);
	HDC hDCScreen = ::GetDC(NULL);
    LONG yPixPerInch = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
    cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    if(lf.lfWeight >= FW_BOLD)
        cf.dwEffects |= CFE_BOLD;
    if(lf.lfItalic)
        cf.dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline)
        cf.dwEffects |= CFE_UNDERLINE;
    cf.bCharSet = lf.lfCharSet;
    cf.bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
    _tcscpy(cf.szFaceName, lf.lfFaceName);
#else
    //need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, cf.szFaceName, LF_FACESIZE) ;
#endif

    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
        TXTBIT_CHARFORMATCHANGE);
}

void CTxtWinHost::SetColor(COLORREF color)
{
    cf.crTextColor = color;
    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
        TXTBIT_CHARFORMATCHANGE);
}

SIZEL* CTxtWinHost::GetExtent() 
{
    return &sizelExtent;
}

void CTxtWinHost::SetExtent(SIZEL *psizelExtent) 
{ 
    sizelExtent = *psizelExtent; 
    pserv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
}

void CTxtWinHost::LimitText(LONG nChars)
{
    cchTextMost = nChars;
    if( cchTextMost <= 0 ) cchTextMost = cInitTextMax;
    pserv->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

BOOL CTxtWinHost::IsCaptured()
{
    return fCaptured;
}

BOOL CTxtWinHost::GetAllowBeep()
{
    return fAllowBeep;
}

void CTxtWinHost::SetAllowBeep(BOOL fAllowBeep)
{
    fAllowBeep = fAllowBeep;

    pserv->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, 
        fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

WORD CTxtWinHost::GetDefaultAlign()
{
    return pf.wAlignment;
}

void CTxtWinHost::SetDefaultAlign(WORD wNewAlign)
{
    pf.wAlignment = wNewAlign;

    // Notify control of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL CTxtWinHost::GetRichTextFlag()
{
    return fRich;
}

void CTxtWinHost::SetRichTextFlag(BOOL fNew)
{
    fRich = fNew;

    pserv->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
        fNew ? TXTBIT_RICHTEXT : 0);
}

LONG CTxtWinHost::GetDefaultLeftIndent()
{
    return pf.dxOffset;
}

void CTxtWinHost::SetDefaultLeftIndent(LONG lNewIndent)
{
    pf.dxOffset = lNewIndent;

    pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void CTxtWinHost::SetClientRect(RECT *prc) 
{
    rcClient = *prc;

	HDC hDCScreen = ::GetDC(NULL);
    LONG xPerInch = ::GetDeviceCaps(hDCScreen, LOGPIXELSX); 
    LONG yPerInch =	::GetDeviceCaps(hDCScreen, LOGPIXELSY); 
    sizelExtent.cx = DXtoHimetricX(rcClient.right - rcClient.left, xPerInch);
    sizelExtent.cy = DYtoHimetricY(rcClient.bottom - rcClient.top, yPerInch);

    pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

BOOL CTxtWinHost::SetSaveSelection(BOOL f_SaveSelection)
{
    BOOL fResult = f_SaveSelection;

    fSaveSelection = f_SaveSelection;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, 
        fSaveSelection ? TXTBIT_SAVESELECTION : 0);

    return fResult;		
}

HRESULT	CTxtWinHost::OnTxInPlaceDeactivate()
{
    HRESULT hr = pserv->OnTxInPlaceDeactivate();

    if (SUCCEEDED(hr))
    {
        fInplaceActive = FALSE;
    }

    return hr;
}

HRESULT	CTxtWinHost::OnTxInPlaceActivate(LPCRECT prcClient)
{
    fInplaceActive = TRUE;

    HRESULT hr = pserv->OnTxInPlaceActivate(prcClient);

    if (FAILED(hr))
    {
        fInplaceActive = FALSE;
    }

    return hr;
}

BOOL CTxtWinHost::DoSetCursor(RECT *prc, POINT *pt)
{
    RECT rc = prc ? *prc : rcClient;

    // Is this in our rectangle?
    if (PtInRect(&rc, *pt))
    {
        RECT *prcClient = (!fInplaceActive || prc) ? &rc : NULL;
		pserv->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL,  ::GetDC(m_re->GetHWND()),
            NULL, prcClient, pt->x, pt->y);

        return TRUE;
    }

    return FALSE;
}

void CTxtWinHost::GetControlRect(LPRECT prc)
{
    prc->top = rcClient.top;
    prc->bottom = rcClient.bottom;
    prc->left = rcClient.left;
    prc->right = rcClient.right;
}

void CTxtWinHost::SetTransparent(BOOL f_Transparent)
{
    fTransparent = f_Transparent;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG CTxtWinHost::SetAccelPos(LONG l_accelpos)
{
    LONG laccelposOld = l_accelpos;

    laccelpos = l_accelpos;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

    return laccelposOld;
}

WCHAR CTxtWinHost::SetPasswordChar(WCHAR ch_PasswordChar)
{
    WCHAR chOldPasswordChar = chPasswordChar;

    chPasswordChar = ch_PasswordChar;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
        (chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

    return chOldPasswordChar;
}

void CTxtWinHost::SetDisabled(BOOL fOn)
{
    cf.dwMask	 |= CFM_COLOR | CFM_DISABLED;
    cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

    if( !fOn )
    {
        cf.dwEffects &= ~CFE_DISABLED;
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
        TXTBIT_CHARFORMATCHANGE);
}

LONG CTxtWinHost::SetSelBarWidth(LONG l_SelBarWidth)
{
    LONG lOldSelBarWidth = lSelBarWidth;

    lSelBarWidth = l_SelBarWidth;

    if (lSelBarWidth)
    {
        dwStyle |= ES_SELECTIONBAR;
    }
    else
    {
        dwStyle &= (~ES_SELECTIONBAR);
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

    return lOldSelBarWidth;
}

BOOL CTxtWinHost::GetTimerState()
{
    return fTimer;
}

void CTxtWinHost::SetCharFormat(CHARFORMAT2W &c)
{
    cf = c;
}

void CTxtWinHost::SetParaFormat(PARAFORMAT2 &p)
{
    pf = p;
}


/////////////////////////////////////////////////////////////////////////////////////
// CDuiRichEdit
static HINSTANCE	g_hrichedit = NULL;
static PCreateTextServices	g_funCreateTextServices = NULL;

CDuiRichEdit::CDuiRichEdit(HWND hWnd, CDuiObject* pDuiObject)
	: CDuiPanel(hWnd, pDuiObject)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_bDown = false;
	m_bDownTemp = false;
	m_buttonState = enBSNormal;
	m_EditState = enBSNormal;
	m_pLeftImage = NULL;
	m_pSmallImage = NULL;
	SetBitmapCount(4);
	m_sizeLeftImage.SetSize(0,0);
	m_sizeSmallImage.SetSize(0,0);

	m_strTitle = _T("");

	m_pTxtWinHost = NULL;
	m_bVScrollBarFixing = false;
	m_bPassWord = false;
	m_bMultiLine = false;
	m_bWantReturn = true;
	m_bWantCtrlReturn = true;
	m_bHScrollBar = false;
	m_bVScrollBar = false;
	m_bAutoHScroll = false;
	m_bAutoVScroll = false;
	m_bRich = true;
	m_bReadOnly = false;
	m_bNumber = false;
	m_bWordWrap = false;
	m_strFile = _T("");
    m_iLimitText = cInitTextMax;
	m_nStartChar = -1;
	m_nEndChar = -1;
	m_lTwhStyle = ES_MULTILINE;
	m_hFont = NULL;
	m_bInited = false;
	m_chLeadByte = 0;
#ifndef _UNICODE
	m_fAccumulateDBC =true;
#else
	m_fAccumulateDBC= false;
#endif
}

CDuiRichEdit::~CDuiRichEdit()
{
    if( m_pTxtWinHost )
	{
		m_pTxtWinHost->GetTextServices()->OnTxInPlaceDeactivate();
        m_pTxtWinHost->Release();
		m_pTxtWinHost = NULL;
    }

	if(m_pLeftImage != NULL)
	{
		delete m_pLeftImage;
		m_pLeftImage = NULL;
	}

	if(m_pSmallImage != NULL)
	{
		delete m_pSmallImage;
		m_pSmallImage = NULL;
	}
}

// 初始化richedit库
BOOL CDuiRichEdit::InitTextService()
{
	if(g_funCreateTextServices != NULL)
	{
		return TRUE;
	}

	// 加载richedit动态库
	g_hrichedit = LoadLibrary(_T("msftedit.dll"));	// msftedit.dll是richedit的4.0版本
	if(g_hrichedit == NULL)
	{
		g_hrichedit = LoadLibrary(_T("riched20.dll"));
	}
	if (g_hrichedit)
	{
		g_funCreateTextServices = (PCreateTextServices)GetProcAddress(g_hrichedit, "CreateTextServices");
	}

	return (g_funCreateTextServices != NULL);
}

// 释放richedit库
void CDuiRichEdit::ReleaseTextService()
{
	if(g_hrichedit != NULL)
	{
		FreeLibrary(g_hrichedit);
		g_hrichedit = NULL;
	}
	g_funCreateTextServices = NULL;
}

// 创建richedit文字服务
HRESULT FnCreateTextServices(IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk)
{
	if(g_funCreateTextServices)
	{
		return g_funCreateTextServices(punkOuter, pITextHost, ppUnk);
	}

	return S_FALSE;
}

bool CDuiRichEdit::SetLeftBitmap(UINT nResourceID, CString strType)
{
	if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, m_pLeftImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight()) >= 3)
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetWidth() / 4, m_pLeftImage->GetHeight());
			m_nLeftImageCount = 4;
		}else
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetHeight(), m_pLeftImage->GetHeight());
			m_nLeftImageCount = m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight();
		}
		return true;
	}
	return false;
}

bool CDuiRichEdit::SetLeftBitmap(CString strImage)
{
	if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, m_pLeftImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight()) >= 3)
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetWidth() / 4, m_pLeftImage->GetHeight());
			m_nLeftImageCount = 4;
		}else
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetHeight(), m_pLeftImage->GetHeight());
			m_nLeftImageCount = m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight();
		}
		return true;
	}
	return false;
}

// 从XML设置图片信息属性
HRESULT CDuiRichEdit::OnAttributeLeftImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// 加载图片文件
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!SetLeftBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = _ttoi(strSkin);
		if(!SetLeftBitmap(nResourceID, TEXT("PNG")))
		{
			if(!SetLeftBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

bool CDuiRichEdit::SetSmallBitmap(UINT nResourceID, CString strType)
{
	if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, m_pSmallImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight()) >= 3)
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetWidth() / 4, m_pSmallImage->GetHeight());
			m_nSmallImageCount = 4;
			m_bIsSmallButton = TRUE;
		}else
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetHeight(), m_pSmallImage->GetHeight());
			m_nSmallImageCount = m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight();
			m_bIsSmallButton = FALSE;
		}
		return true;
	}
	return false;
}

bool CDuiRichEdit::SetSmallBitmap(CString strImage)
{
	if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, m_pSmallImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight()) >= 3)
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetWidth() / 4, m_pSmallImage->GetHeight());
			m_nSmallImageCount = 4;
			m_bIsSmallButton = TRUE;
		}else
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetHeight(), m_pSmallImage->GetHeight());
			m_nSmallImageCount = m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight();
			m_bIsSmallButton = FALSE;
		}
		return true;
	}
	return false;
}

// 从XML设置图片信息属性
HRESULT CDuiRichEdit::OnAttributeSmallImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// 加载图片文件
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!SetSmallBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = _ttoi(strSkin);
		if(!SetSmallBitmap(nResourceID, TEXT("PNG")))
		{
			if(!SetSmallBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

BOOL CDuiRichEdit::IsWantReturn()
{
    return m_bWantReturn;
}

void CDuiRichEdit::SetWantReturn(BOOL bWantReturn)
{
    m_bWantReturn = bWantReturn;
}

BOOL CDuiRichEdit::IsWantCtrlReturn()
{
    return m_bWantCtrlReturn;
}

void CDuiRichEdit::SetWantCtrlReturn(BOOL bWantCtrlReturn)
{
    m_bWantCtrlReturn = bWantCtrlReturn;
}

BOOL CDuiRichEdit::IsRich()
{
    return m_bRich;
}

void CDuiRichEdit::SetRich(BOOL bRich)
{
    m_bRich = bRich;
    if( m_pTxtWinHost ) m_pTxtWinHost->SetRichTextFlag(bRich);
}

BOOL CDuiRichEdit::IsReadOnly()
{
    return m_bReadOnly;
}

void CDuiRichEdit::SetReadOnly(BOOL bReadOnly)
{
    m_bReadOnly = bReadOnly;
    if( m_pTxtWinHost ) m_pTxtWinHost->SetReadOnly(bReadOnly);
}

BOOL CDuiRichEdit::GetWordWrap()
{
    return m_bWordWrap;
}

void CDuiRichEdit::SetWordWrap(BOOL bWordWrap)
{
    m_bWordWrap = bWordWrap;
    if( m_pTxtWinHost ) m_pTxtWinHost->SetWordWrap(bWordWrap);
}

HFONT CDuiRichEdit::GetDefaultFont()
{
	bool bBold = ((m_fontStyle & FontStyleBold) != 0);
	bool bUnderline = ((m_fontStyle & FontStyleUnderline) != 0);
	bool bItalic = ((m_fontStyle & FontStyleItalic) != 0);

	LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    _tcsncpy(lf.lfFaceName, m_strFont, LF_FACESIZE);
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -m_nFontWidth;
    if( bBold ) lf.lfWeight += FW_BOLD;
    if( bUnderline ) lf.lfUnderline = TRUE;
    if( bItalic ) lf.lfItalic = TRUE;
    HFONT hFont = ::CreateFontIndirect(&lf);
	return hFont;
}

void CDuiRichEdit::SetFont()
{
	bool bBold = ((m_fontStyle & FontStyleBold) != 0);
	bool bUnderline = ((m_fontStyle & FontStyleUnderline) != 0);
	bool bItalic = ((m_fontStyle & FontStyleItalic) != 0);
	SetFont(m_strFont, m_nFontWidth, bBold, bUnderline, bItalic);
}

void CDuiRichEdit::SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
    if( m_pTxtWinHost )
	{
        LOGFONT lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        _tcsncpy(lf.lfFaceName, pStrFontName, LF_FACESIZE);
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -nSize;
        if( bBold ) lf.lfWeight += FW_BOLD;
        if( bUnderline ) lf.lfUnderline = TRUE;
        if( bItalic ) lf.lfItalic = TRUE;
        HFONT hFont = ::CreateFontIndirect(&lf);
        if( hFont == NULL ) return;
        m_pTxtWinHost->SetFont(hFont);
        ::DeleteObject(hFont);
    }
}

LONG CDuiRichEdit::GetWinStyle()
{
    return m_lTwhStyle;
}

void CDuiRichEdit::SetWinStyle(LONG lStyle)
{
    m_lTwhStyle = lStyle;
}

Color CDuiRichEdit::GetTextColor()
{
    return m_clrText;
}

void CDuiRichEdit::SetTextColor(Color clrText)
{
    m_clrText = clrText;
    if( m_pTxtWinHost ) {
        m_pTxtWinHost->SetColor(clrText.ToCOLORREF());
    }
}

int CDuiRichEdit::GetLimitText()
{
    return m_iLimitText;
}

void CDuiRichEdit::SetLimitText(int iChars)
{
    m_iLimitText = iChars;
    if( m_pTxtWinHost )
	{
        m_pTxtWinHost->LimitText(m_iLimitText);
    }
}

long CDuiRichEdit::GetTextLength(DWORD dwFlags) const
{
    GETTEXTLENGTHEX textLenEx;
    textLenEx.flags = dwFlags;
#ifdef _UNICODE
    textLenEx.codepage = 1200;
#else
    textLenEx.codepage = CP_ACP;
#endif
    LRESULT lResult;
    TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
    return (long)lResult;
}

CString CDuiRichEdit::GetText() const
{
    long lLen = GetTextLength(GTL_DEFAULT);
    LPTSTR lpText = NULL;
    GETTEXTEX gt;
    gt.flags = GT_DEFAULT;
#ifdef _UNICODE
    gt.cb = sizeof(TCHAR) * (lLen + 1) ;
    gt.codepage = 1200;
    lpText = new TCHAR[lLen + 1];
    ::ZeroMemory(lpText, (lLen + 1) * sizeof(TCHAR));
#else
    gt.cb = sizeof(TCHAR) * lLen * 2 + 1;
    gt.codepage = CP_ACP;
    lpText = new TCHAR[lLen * 2 + 1];
    ::ZeroMemory(lpText, (lLen * 2 + 1) * sizeof(TCHAR));
#endif
    gt.lpDefaultChar = NULL;
    gt.lpUsedDefChar = NULL;
    TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)lpText, 0);
    CString sText(lpText);
    delete[] lpText;
    return sText;
}

void CDuiRichEdit::SetText(LPCTSTR pstrText)
{
    m_strTitle = pstrText;
    if( !m_pTxtWinHost ) return;
    SetSel(0, -1);
    ReplaceSel(pstrText, FALSE);
}

// 从文件中读取rtf内容
bool CDuiRichEdit::SetFile(LPCTSTR pstrFile)
{
    m_strFile = pstrFile;
    if( !m_pTxtWinHost ) return false;

	SetSel(-1, -1);
    ReplaceSel(_T(""), FALSE);

	BYTE* pData = NULL;
	if(DuiSystem::Instance()->LoadFileToBuffer(pstrFile, pData))
	{
		SetSel(0, -1);
		ReplaceSel((LPCTSTR)pData, FALSE);
		delete pData;
		return true;
	}
	
	return false;
}

bool CDuiRichEdit::GetModify() const
{ 
    if( !m_pTxtWinHost ) return false;
    LRESULT lResult;
    TxSendMessage(EM_GETMODIFY, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

void CDuiRichEdit::SetModify(bool bModified) const
{ 
    TxSendMessage(EM_SETMODIFY, bModified, 0, 0);
}

void CDuiRichEdit::GetSel(CHARRANGE &cr) const
{ 
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
}

void CDuiRichEdit::GetSel(long& nStartChar, long& nEndChar) const
{
    CHARRANGE cr;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
    nStartChar = cr.cpMin;
    nEndChar = cr.cpMax;
}

int CDuiRichEdit::SetSel(CHARRANGE &cr)
{ 
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (int)lResult;
}

int CDuiRichEdit::SetSel(long nStartChar, long nEndChar)
{
    CHARRANGE cr;
    cr.cpMin = nStartChar;
    cr.cpMax = nEndChar;
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (int)lResult;
}

void CDuiRichEdit::ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo)
{
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
}

void CDuiRichEdit::ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo)
{
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
}

CString CDuiRichEdit::GetSelText() const
{
    if( !m_pTxtWinHost ) return _T("");
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
    LPWSTR lpText = NULL;
    lpText = new WCHAR[cr.cpMax - cr.cpMin + 1];
    ::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(WCHAR));
    TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpText, 0);
    CString sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

int CDuiRichEdit::SetSelAll()
{
    return SetSel(0, -1);
}

int CDuiRichEdit::SetSelNone()
{
    return SetSel(-1, 0);
}

bool CDuiRichEdit::GetZoom(int& nNum, int& nDen) const
{
    LRESULT lResult;
    TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CDuiRichEdit::SetZoom(int nNum, int nDen)
{
    if (nNum < 0 || nNum > 64) return false;
    if (nDen < 0 || nDen > 64) return false;
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, nNum, nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CDuiRichEdit::SetZoomOff()
{
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

WORD CDuiRichEdit::GetSelectionType() const
{
    LRESULT lResult;
    TxSendMessage(EM_SELECTIONTYPE, 0, 0, &lResult);
    return (WORD)lResult;
}

bool CDuiRichEdit::GetAutoURLDetect() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETAUTOURLDETECT, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CDuiRichEdit::SetAutoURLDetect(bool bAutoDetect)
{
    LRESULT lResult;
    TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0, &lResult);
    return (BOOL)lResult == FALSE;
}

DWORD CDuiRichEdit::GetEventMask() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);
    return (DWORD)lResult;
}

DWORD CDuiRichEdit::SetEventMask(DWORD dwEventMask)
{
    LRESULT lResult;
    TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lResult);
    return (DWORD)lResult;
}

CString CDuiRichEdit::GetTextRange(long nStartChar, long nEndChar) const
{
    TEXTRANGEW tr = { 0 };
    tr.chrg.cpMin = nStartChar;
    tr.chrg.cpMax = nEndChar;
    LPWSTR lpText = NULL;
    lpText = new WCHAR[nEndChar - nStartChar + 1];
    ::ZeroMemory(lpText, (nEndChar - nStartChar + 1) * sizeof(WCHAR));
    tr.lpstrText = lpText;
    TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);
    CString sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

void CDuiRichEdit::HideSelection(bool bHide, bool bChangeStyle)
{
    TxSendMessage(EM_HIDESELECTION, bHide, bChangeStyle, 0);
}

void CDuiRichEdit::ScrollCaret()
{
    TxSendMessage(EM_SCROLLCARET, 0, 0, 0);
}

int CDuiRichEdit::InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo)
{
    int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

int CDuiRichEdit::AppendText(LPCTSTR lpstrText, bool bCanUndo)
{
    int nRet = SetSel(-1, -1);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

DWORD CDuiRichEdit::GetDefaultCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool CDuiRichEdit::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
    if( !m_pTxtWinHost ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    if( (BOOL)lResult == TRUE ) {
        CHARFORMAT2W cfw;
        cfw.cbSize = sizeof(CHARFORMAT2W);
        TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cfw, 0);
        m_pTxtWinHost->SetCharFormat(cfw);
        return true;
    }
    return false;
}

DWORD CDuiRichEdit::GetSelectionCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool CDuiRichEdit::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
    if( !m_pTxtWinHost ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CDuiRichEdit::SetWordCharFormat(CHARFORMAT2 &cf)
{
    if( !m_pTxtWinHost ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION|SCF_WORD, (LPARAM)&cf, &lResult); 
    return (BOOL)lResult == TRUE;
}

DWORD CDuiRichEdit::GetParaFormat(PARAFORMAT2 &pf) const
{
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    return (DWORD)lResult;
}

bool CDuiRichEdit::SetParaFormat(PARAFORMAT2 &pf)
{
    if( !m_pTxtWinHost ) return false;
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    if( (BOOL)lResult == TRUE )
	{
        m_pTxtWinHost->SetParaFormat(pf);
        return true;
    }
    return false;
}

bool CDuiRichEdit::Redo()
{ 
    if( !m_pTxtWinHost ) return false;
    LRESULT lResult;
    TxSendMessage(EM_REDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

bool CDuiRichEdit::Undo()
{ 
    if( !m_pTxtWinHost ) return false;
    LRESULT lResult;
    TxSendMessage(EM_UNDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

void CDuiRichEdit::Clear()
{ 
    TxSendMessage(WM_CLEAR, 0, 0, 0); 
}

void CDuiRichEdit::Copy()
{ 
    TxSendMessage(WM_COPY, 0, 0, 0); 
}

void CDuiRichEdit::Cut()
{ 
    TxSendMessage(WM_CUT, 0, 0, 0); 
}

void CDuiRichEdit::Paste()
{ 
    TxSendMessage(WM_PASTE, 0, 0, 0); 
}

int CDuiRichEdit::GetLineCount() const
{ 
    if( !m_pTxtWinHost ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);
    return (int)lResult; 
}

CString CDuiRichEdit::GetLine(int nIndex, int nMaxLength) const
{
    LPWSTR lpText = NULL;
    lpText = new WCHAR[nMaxLength + 1];
    ::ZeroMemory(lpText, (nMaxLength + 1) * sizeof(WCHAR));
    *(LPWORD)lpText = (WORD)nMaxLength;
    TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpText, 0);
    CString sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

int CDuiRichEdit::LineIndex(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINEINDEX, nLine, 0, &lResult);
    return (int)lResult;
}

int CDuiRichEdit::LineLength(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINELENGTH, nLine, 0, &lResult);
    return (int)lResult;
}

bool CDuiRichEdit::LineScroll(int nLines, int nChars)
{
    LRESULT lResult;
    TxSendMessage(EM_LINESCROLL, nChars, nLines, &lResult);
    return (BOOL)lResult == TRUE;
}

CPoint CDuiRichEdit::GetCharPos(long lChar) const
{ 
    CPoint pt; 
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar, 0); 
    return pt;
}

long CDuiRichEdit::LineFromChar(long nIndex) const
{ 
    if( !m_pTxtWinHost ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);
    return (long)lResult;
}

CPoint CDuiRichEdit::PosFromChar(UINT nChar) const
{ 
    POINTL pt; 
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar, 0); 
    return CPoint(pt.x, pt.y); 
}

int CDuiRichEdit::CharFromPos(CPoint pt) const
{ 
    POINTL ptl = {pt.x, pt.y}; 
    if( !m_pTxtWinHost ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lResult);
    return (int)lResult; 
}

void CDuiRichEdit::EmptyUndoBuffer()
{ 
    TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, 0); 
}

UINT CDuiRichEdit::SetUndoLimit(UINT nLimit)
{ 
    if( !m_pTxtWinHost ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_SETUNDOLIMIT, (WPARAM) nLimit, 0, &lResult);
    return (UINT)lResult; 
}

long CDuiRichEdit::StreamIn(int nFormat, EDITSTREAM &es)
{ 
    if( !m_pTxtWinHost ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult;
}

long CDuiRichEdit::StreamOut(int nFormat, EDITSTREAM &es)
{ 
    if( !m_pTxtWinHost ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult; 
}

// 初始化richedit控件
void CDuiRichEdit::DoInit()
{
	if(m_bInited)
		return;

	// 初始化窗口风格
	if(m_bVScrollBar)
	{
		m_lTwhStyle |= ES_DISABLENOSCROLL | WS_VSCROLL;
	}
	if(m_bHScrollBar)
	{
		m_lTwhStyle |= ES_DISABLENOSCROLL | WS_HSCROLL;
	}
	if(m_bAutoVScroll)
	{
		m_lTwhStyle |= ES_AUTOVSCROLL;
	}
	if(m_bAutoHScroll)
	{
		m_lTwhStyle |= ES_AUTOHSCROLL;
	}
	if(!m_bMultiLine)
	{
		m_lTwhStyle &= ~ES_MULTILINE;
	}
	if(m_bReadOnly)
	{
		m_lTwhStyle |= ES_READONLY;
	}
	if(m_bPassWord)
	{
		m_lTwhStyle |= ES_PASSWORD;
	}
	if(m_uAlignment == Align_Left)
	{
		m_lTwhStyle &= ~(ES_CENTER | ES_RIGHT);
		m_lTwhStyle |= ES_LEFT;
	}else
	if(m_uAlignment == Align_Center)
	{
		m_lTwhStyle &= ~(ES_LEFT | ES_RIGHT);
		m_lTwhStyle |= ES_CENTER;
	}else
	if(m_uAlignment == Align_Right)
	{
		m_lTwhStyle &= ~(ES_LEFT | ES_CENTER);
		m_lTwhStyle |= ES_RIGHT;
	}

    CREATESTRUCT cs;
    cs.style = m_lTwhStyle;
    cs.x = m_rc.left;
    cs.y = m_rc.top;
    cs.cx = m_rc.Width();
    cs.cy = m_rc.Height();
    cs.lpszName = m_strTitle;
    CreateHost(this, &cs, &m_pTxtWinHost);
    if( m_pTxtWinHost )
	{
        m_pTxtWinHost->SetTransparent(TRUE);
        LRESULT lResult;
		m_pTxtWinHost->GetTextServices()->TxSendMessage(EM_GETLANGOPTIONS, 0, 0, &lResult);
		DWORD dw = lResult;
		dw |= IMF_AUTOKEYBOARD | IMF_DUALFONT;
		dw &= ~IMF_AUTOFONT;
        m_pTxtWinHost->GetTextServices()->TxSendMessage(EM_SETLANGOPTIONS, 0, dw, &lResult);
        m_pTxtWinHost->OnTxInPlaceActivate(NULL);
		m_pTxtWinHost->SetRichTextFlag(m_bRich);
		m_pTxtWinHost->SetWordWrap(m_bWordWrap);
		m_pTxtWinHost->SetColor(m_clrText.ToCOLORREF());
		SetLimitText(m_iLimitText);
		SetFont();
		// 加载rtf文件
		if(!m_strFile.IsEmpty())
		{
			CString strFile = _T("");
			if(GetFileAttributes(DuiSystem::GetSkinPath() + strFile) != 0xFFFFFFFF)	// 从exe路径开始查找
			{
				strFile = DuiSystem::GetSkinPath() + m_strFile;
			}else
			if(GetFileAttributes(strFile) != 0xFFFFFFFF)	// 绝对路径查找
			{
				strFile = m_strFile;
			}
			if(!strFile.IsEmpty())
			{
				SetFile(strFile);
			}
		}
		SetSel(m_nStartChar, m_nEndChar);

		m_bInited= true;
    }
}

HRESULT CDuiRichEdit::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
{
    if( m_pTxtWinHost )
	{
        if( msg == WM_KEYDOWN && TCHAR(wparam) == VK_RETURN )
		{
            if( !m_bWantReturn || (::GetKeyState(VK_CONTROL) < 0 && !m_bWantCtrlReturn) )
			{
                return S_OK;
            }
			return m_pTxtWinHost->GetTextServices()->TxSendMessage(WM_CHAR, wparam, lparam, plresult);
        }
        return m_pTxtWinHost->GetTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
    }
    return S_FALSE;
}

IDropTarget* CDuiRichEdit::GetTxDropTarget()
{
    IDropTarget *pdt = NULL;
    if( m_pTxtWinHost->GetTextServices()->TxGetDropTarget(&pdt) == NOERROR ) return pdt;
    return NULL;
}

bool CDuiRichEdit::OnTxViewChanged()
{
    return true;
}

bool CDuiRichEdit::SetDropAcceptFile(bool bAccept) 
{
	LRESULT lResult;
	TxSendMessage(EM_SETEVENTMASK, 0,ENM_DROPFILES|ENM_LINK, // ENM_CHANGE| ENM_CORRECTTEXT | ENM_DRAGDROPDONE | ENM_DROPFILES | ENM_IMECHANGE | ENM_LINK | ENM_OBJECTPOSITIONS | ENM_PROTECTED | ENM_REQUESTRESIZE | ENM_SCROLL | ENM_SELCHANGE | ENM_UPDATE,
		&lResult);
	return (BOOL)lResult == FALSE;
}

void CDuiRichEdit::OnTxNotify(DWORD iNotify, void *pv)
{
	switch(iNotify)
	{ 
	case EN_DROPFILES:   
	case EN_MSGFILTER:   
	case EN_OLEOPFAILED:   
	case EN_PROTECTED:   
	case EN_SAVECLIPBOARD:   
	case EN_SELCHANGE:   
	case EN_STOPNOUNDO:   
	case EN_LINK:   
	case EN_OBJECTPOSITIONS:   
	case EN_DRAGDROPDONE:   
		{
			if(pv)  // Fill out NMHDR portion of pv   
			{   
				LONG nId =  GetWindowLong(this->GetHWND(), GWL_ID);   
				NMHDR  *phdr = (NMHDR *)pv;   
				phdr->hwndFrom = this->GetHWND();   
				phdr->idFrom = nId;   
				phdr->code = iNotify;  

				if(::SendMessage(this->GetHWND(), WM_NOTIFY, (WPARAM) nId, (LPARAM) pv))   
				{   
					//hr = S_FALSE;   
				}   
			}    
		}
		break;
	}
}

void CDuiRichEdit::LineUp()
{
    TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
}

void CDuiRichEdit::LineDown()
{
    int iPos = 0;
//    if( m_pTxtWinHost && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
//        iPos = m_pVerticalScrollBar->GetScrollPos();
    TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);
/*    if( m_pTxtWinHost && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() )
	{
        if( m_pVerticalScrollBar->GetScrollPos() <= iPos )
            m_pVerticalScrollBar->SetScrollPos(m_pVerticalScrollBar->GetScrollRange());
    }*/
}

void CDuiRichEdit::PageUp()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEUP, 0L, 0);
}

void CDuiRichEdit::PageDown()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L, 0);
}

void CDuiRichEdit::HomeUp()
{
    TxSendMessage(WM_VSCROLL, SB_TOP, 0L, 0);
}

void CDuiRichEdit::EndDown()
{
    TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);
}

void CDuiRichEdit::LineLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);
}

void CDuiRichEdit::LineRight()
{
    TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);
}

void CDuiRichEdit::PageLeft()
{
    TxSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L, 0);
}

void CDuiRichEdit::PageRight()
{
    TxSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L, 0);
}

void CDuiRichEdit::HomeLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LEFT, 0L, 0);
}

void CDuiRichEdit::EndRight()
{
    TxSendMessage(WM_HSCROLL, SB_RIGHT, 0L, 0);
}

// 控件初始化,由Load函数加载完属性之后调用
BOOL CDuiRichEdit::OnInit()
{
	DoInit();
	if(m_pTxtWinHost)
	{
        m_pTxtWinHost->SetClientRect(&m_rcText);
	}
	return TRUE;
}

// 设置控件的位置信息
void CDuiRichEdit::SetControlRect(CRect rc) 
{
	m_rc = rc;

	// 去掉图片按钮部分
	m_rcText = m_rc;
	m_rcText.top += 4;
	m_rcText.left += (6 + m_sizeLeftImage.cx);
	m_rcText.bottom -= 4;
	m_rcText.right -= (3 + m_sizeSmallImage.cx);

	//if( !m_bInited ) DoInit();

	if( m_pTxtWinHost )
	{
        m_pTxtWinHost->SetClientRect(&m_rcText);
        /*if( bVScrollBarVisiable && (!m_pVerticalScrollBar->IsVisible() || m_bVScrollBarFixing) ) {
            LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            m_pTxtWinHost->GetTextServices()->TxGetNaturalSize(
                DVASPECT_CONTENT, 
                GetHWND(), 
                NULL,
                NULL,
                TXTNS_FITTOCONTENT,
                &szExtent,
                &lWidth,
                &lHeight);
            if( lHeight > rc.bottom - rc.top ) {
                m_pVerticalScrollBar->SetVisible(true);
                m_pVerticalScrollBar->SetScrollPos(0);
                m_bVScrollBarFixing = true;
            }
            else {
                if( m_bVScrollBarFixing ) {
                    m_pVerticalScrollBar->SetVisible(false);
                    m_bVScrollBarFixing = false;
                }
            }
        }*/
    }
}

BOOL CDuiRichEdit::IsDraw(CPoint point)
{
 	if(m_buttonState == enBSDown)
 	{
 		return false;
 	}
	CRect  rc;
	rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
	rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
	rc.right = rc.left + m_sizeSmallImage.cx;
	rc.bottom = rc.top + m_sizeSmallImage.cy;

	if(rc.PtInRect(point))
	{
		if(m_buttonState == enBSNormal)
			return true;
	}
	else
	{
		if(m_buttonState != enBSNormal)
			return true;
	}

	return false;
}

// 设置控件中的Windows原生控件是否可见的状态
void CDuiRichEdit::SetControlWndVisible(BOOL bIsVisible)
{
	if( m_pTxtWinHost != NULL )
	{
	}
}

// 设置控件的禁用状态
void CDuiRichEdit::SetControlDisable(BOOL bIsDisable)
{
	if(m_bIsDisable != bIsDisable)
	{
		m_bIsDisable = bIsDisable;
		if(bIsDisable)
		{
			m_EditState = enBSDisable;
			m_buttonState = enBSDisable;
		}
		else
		{
			if(m_bDown)
			{
				m_EditState = enBSDown;
				m_buttonState = enBSDown;
			}
			else
			{
				m_EditState = enBSNormal;
				m_buttonState = enBSNormal;
			}
		}
		::InvalidateRect(m_hWnd, &m_rc, true);

		if( m_pTxtWinHost )
		{
			m_pTxtWinHost->SetDisabled(bIsDisable);
		}
	}
}

// 设置控件title
void CDuiRichEdit::SetControlTitle(CString strTitle)
{
	__super::SetControlTitle(strTitle);

	// 设置richedit控件的内容
	SetText(strTitle);
}

// 设置控件的焦点
BOOL CDuiRichEdit::SetControlFocus(BOOL bFocus)
{
	__super::SetControlFocus(bFocus);

	enumButtonState buttonState = m_buttonState;
	enumButtonState editState = m_EditState;

	if(!bFocus)
	{
		m_bDown = false;
		m_buttonState = enBSNormal;
		m_EditState = enBSNormal;
	}else
	{
		m_bDown = true;
		m_buttonState = enBSDown;
		m_EditState = enBSDown;
	}

	if( m_pTxtWinHost )
	{
        m_pTxtWinHost->OnTxInPlaceActivate(NULL);
		m_pTxtWinHost->GetTextServices()->TxSendMessage(bFocus ? WM_SETFOCUS : WM_KILLFOCUS, 0, 0, 0);
    }

	bool bIsDraw = buttonState != m_buttonState || editState != m_EditState;
	if(bIsDraw)
	{
		::InvalidateRect(m_hWnd, &m_rc, true);
	}

	return bIsDraw;
}

BOOL CDuiRichEdit::OnControlMouseMove(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_buttonState;
	enumButtonState editState = m_EditState;
	if(!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_EditState = enBSHover;
			if(m_buttonState != enBSDown)
			{
				CRect  rc;
				rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
				rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
				rc.right = rc.left + m_sizeSmallImage.cx;
				rc.bottom = rc.top + m_sizeSmallImage.cy;

				if(rc.PtInRect(point))
				{
					m_buttonState = enBSHover;
				}
				else
				{
					m_buttonState = enBSNormal;
				}
			}
		}
		else
		{
			if(m_buttonState != enBSDown)
			{
				m_buttonState = enBSNormal;
				m_EditState = enBSNormal;
			}
		}
	}
	
	if(buttonState != m_buttonState || editState != m_EditState)
	{
		UpdateControl();
	}

	return buttonState != m_buttonState || editState != m_EditState;
}

BOOL CDuiRichEdit::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_buttonState;	
	enumButtonState editState = m_EditState;
	if(!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			CRect  rc;
			rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
			rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
			rc.right = rc.left + m_sizeSmallImage.cx;
			rc.bottom = rc.top + m_sizeSmallImage.cy;

			if(rc.PtInRect(point))	// 点击的是编辑框右部的小按钮图片
			{
				if(m_bIsSmallButton)	// 小图片作为按钮才进行处理
				{
					m_bDown = !m_bDown;
					m_bDownTemp = true;
					if(m_bDown)
					{
						m_buttonState = enBSDown;
					}
					else
					{				
						m_buttonState = enBSHover;
					}				
					SendMessage(MSG_CONTROL_BUTTON, CONTROL_BUTTON, MSG_BUTTON_DOWN);
				}
			}
			else
			{
				if(m_bDown)
				{
					m_bDown = false;
					m_buttonState = enBSHover;
				}
				
				SendMessage(MSG_CONTROL_BUTTON, CONTROL_EDIT, MSG_BUTTON_DOWN);
			}		
		}
		else
		{
			m_buttonState = enBSNormal;
			m_EditState = enBSNormal;
		}
	}

	if(buttonState != m_buttonState || editState != m_EditState)
	{
		UpdateControl();
	}
	
	return buttonState != m_buttonState || editState != m_EditState;
}

BOOL CDuiRichEdit::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_buttonState;
	enumButtonState editState = m_EditState;
	if(!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_EditState = enBSHover;

			CRect  rc;
			rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
			rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
			rc.right = rc.left + m_sizeSmallImage.cx;
			rc.bottom = rc.top + m_sizeSmallImage.cy;

			if(rc.PtInRect(point))
			{
				if(m_bIsSmallButton)	// 小图片作为按钮才进行处理
				{
					if(m_bDown)
					{
						m_buttonState = enBSDown;
					}
					else
					{
						m_buttonState = enBSHover;
					}	
					SendMessage(MSG_CONTROL_BUTTON, CONTROL_BUTTON, MSG_BUTTON_UP);
				}
			}
			else
			{
				if(m_bDown)
				{
					m_buttonState = enBSDown;
				}
				else
				{
					m_buttonState = enBSNormal;
				}	
				SendMessage(MSG_CONTROL_BUTTON, CONTROL_EDIT, MSG_BUTTON_UP);
			}			
		}
		else
		{
			if(!m_bDown)
			{
				m_buttonState = enBSNormal;
				m_EditState = enBSNormal;
			}
		}
	}
	
	m_bDownTemp = false;

	if(buttonState != m_buttonState || editState != m_EditState)
	{
		UpdateControl();
	}

	return buttonState != m_buttonState || editState != m_EditState;
}

// 键盘事件处理
BOOL CDuiRichEdit::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
/*	// 如果是回车键,则转换为字符事件传递给原生控件
	if((nChar == VK_RETURN) && m_pTxtWinHost)
	{
		m_pTxtWinHost->GetTextServices()->TxSendMessage(WM_CHAR, VK_RETURN, nFlags, NULL);
		return true;
	}

	switch(nChar)
    {
    case VK_RETURN:
        if(!(GetKeyState(VK_CONTROL) & 0x8000) && !m_bWantReturn)
            return false;
        break;

    case VK_TAB:
        if(!m_bTabStop && !(GetKeyState(VK_CONTROL) & 0x8000))
            return false;
        break;
    default:
        if(m_bNumber && !isdigit(nChar) && nChar!='-' && nChar!='.' && nChar!=',')
            return false;
#ifndef _UNICODE
        if(m_byDbcsLeadByte==0)
        {
            if(IsDBCSLeadByte(nChar))
            {
                m_byDbcsLeadByte=nChar;
                return false;
            }
        }else
        {
            nChar=MAKEWORD(nChar,m_byDbcsLeadByte);
            m_pTxtWinHost->GetTextServices()->TxSendMessage(WM_IME_CHAR,nChar,0,NULL);
            m_byDbcsLeadByte=0;
            return true;
        }
#endif//_UNICODE
        break;
    }
    m_pTxtWinHost->GetTextServices()->TxSendMessage(WM_CHAR, nChar, nFlags, NULL);

	return true;
*/
	if(!IsFocusControl())
	{
		return false;
	}

	bool handled = true;
	unsigned int virtualKeyCode = nChar;
    unsigned int flags = nFlags;
	bool bShiftState = false;
	if((GetKeyState(VK_SHIFT) & 0x8000) || (VK_SHIFT == nChar))
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
			if(( nChar & 0x80 )  |  (nChar >= 0x70  && nChar <= 0x7F  ))
			{
				//if( !IsFocusControl() ) return 0;
				return false;
			}	
		}
	}	
	if(nChar >= 0x70  && nChar <= 0x7F  )
	{
		LRESULT lResult = true;
		TxSendMessage(WM_KEYDOWN, virtualKeyCode, flags, &lResult);
		handled = true;
		if( nChar == 0x74 )
		{
			MSG msg;
			msg.hwnd = m_hWnd;
			msg.message = WM_CHAR;
			msg.wParam = virtualKeyCode;
			msg.lParam = nFlags;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);			 
		}
		//if( !IsFocusControl() ) return 0;
		return false;
	}

	LRESULT lResult = true;
    TxSendMessage(WM_KEYDOWN, virtualKeyCode, flags, &lResult);
	handled = true;
	
	if(!bShiftState)
	{
		if( !GetKeyState(VK_CAPITAL)  )
		{
			if((nChar >= 'A') && (nChar <= 'Z'))
				virtualKeyCode = tolower(nChar);
		}
		if(nChar != VK_SHIFT)
		{
			if( nChar > 0x80 )
				return 0;
		}
	}else
	{	
		if(nChar != VK_SHIFT)
		{
			//wParam = wParam & 0x7F;//`~
			//virtualKeyCode = wParam;
			if(nChar == '1' || nChar == '3' || nChar == '4' || nChar == '5' )
				virtualKeyCode = nChar - 0x10;
			else if(nChar == '2')
				virtualKeyCode = '@';
			else if(nChar == '6')
				virtualKeyCode = '^';
			else if(nChar == '7')
				virtualKeyCode = '&';
			else if(nChar == '8')
				virtualKeyCode = '*' ;
			else if(nChar == '9')
				virtualKeyCode = '(';
			else if(nChar == '0')
				virtualKeyCode = ')';
			else if(nChar == 45)
				virtualKeyCode = '_';
			else if(nChar == '=')
				virtualKeyCode = '+';
			else if(nChar == '[')
				virtualKeyCode = '{';
			else if(nChar == ']')
				virtualKeyCode = '}';
			else if(nChar == ';')
				virtualKeyCode = ':';
			else if(nChar == '\'')
				virtualKeyCode = '"';
			else if(nChar == '\\')
				virtualKeyCode = '|';
			else if(nChar == ',')
				virtualKeyCode = '<';
			else if(nChar == '.')
				virtualKeyCode = '>';
			else if(nChar == '/')
				virtualKeyCode = '?';
			else if(nChar == '`')
				virtualKeyCode = '~';
			else if(nChar > 0x80)
				return 0;
		}
	}

	if((nChar < '0') && (nChar != ' '))
	{
		return handled;
	}

	TxSendMessage(WM_CHAR, virtualKeyCode, flags, &lResult);
	handled = true;

	return handled;
}

void CDuiRichEdit::DrawControl(CDC &dc, CRect rcUpdate)
{
	// 画边框和按钮
	Graphics graphics(dc);

	DrawImageFrame(graphics, m_pImage, m_rc, m_EditState * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, 4);

	if(m_pLeftImage)
	{
		CRect  rc;
		rc.left = m_rc.left + 2;
		rc.top = m_rc.top + (m_rc.Height() - m_sizeLeftImage.cy) / 2;
		rc.right = rc.left + m_sizeLeftImage.cx;
		rc.bottom = rc.top + m_sizeLeftImage.cy;
		
		if(m_nLeftImageCount > m_buttonState)
		{
			graphics.DrawImage(m_pLeftImage, RectF((Gdiplus::REAL)rc.left , (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height()),
				(Gdiplus::REAL)(m_buttonState * m_sizeLeftImage.cx), 0, (Gdiplus::REAL)m_sizeLeftImage.cx, (Gdiplus::REAL)m_sizeLeftImage.cy, UnitPixel);
		}else
		{
			graphics.DrawImage(m_pLeftImage, RectF((Gdiplus::REAL)rc.left , (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height()),
				0, 0, (Gdiplus::REAL)m_sizeLeftImage.cx, (Gdiplus::REAL)m_sizeLeftImage.cy, UnitPixel);
		}
	}

	if(m_pSmallImage)
	{
		CRect  rc;
		rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
		rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
		rc.right = rc.left + m_sizeSmallImage.cx;
		rc.bottom = rc.top + m_sizeSmallImage.cy;
		
		if(m_nSmallImageCount > m_buttonState)
		{
			graphics.DrawImage(m_pSmallImage, RectF((Gdiplus::REAL)rc.left , (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height()),
				(Gdiplus::REAL)(m_buttonState * m_sizeSmallImage.cx), 0, (Gdiplus::REAL)m_sizeSmallImage.cx, (Gdiplus::REAL)m_sizeSmallImage.cy, UnitPixel);
		}else
		{
			graphics.DrawImage(m_pSmallImage, RectF((Gdiplus::REAL)rc.left , (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height()),
				0, 0, (Gdiplus::REAL)m_sizeSmallImage.cx, (Gdiplus::REAL)m_sizeSmallImage.cy, UnitPixel);
		}
	}

	// 显示richedit控件
	if( !::IntersectRect(&rcUpdate, &rcUpdate, &m_rc) ) return;

	if( m_pTxtWinHost ) {
        RECT rc;
        m_pTxtWinHost->GetControlRect(&rc);
        // Remember wparam is actually the hdc and lparam is the update
        // rect because this message has been preprocessed by the window.
        m_pTxtWinHost->GetTextServices()->TxDraw(
            DVASPECT_CONTENT,  		// Draw Aspect
            /*-1*/0,				// Lindex
            NULL,					// Info for drawing optimazation
            NULL,					// target device information
            dc.m_hDC,			    // Draw device HDC
            NULL, 				   	// Target device HDC
            (RECTL*)&rc,			// Bounding client rectangle
            NULL, 		            // Clipping rectangle for metafiles
            (RECT*)&rcUpdate,		// Update rectangle
            NULL, 	   				// Call back function
            NULL,					// Call back parameter
            0);				        // What view of the object
        /*if( m_bVScrollBarFixing ) {
            LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            m_pTxtWinHost->GetTextServices()->TxGetNaturalSize(
                DVASPECT_CONTENT, 
                GetHWND(), 
                NULL,
                NULL,
                TXTNS_FITTOCONTENT,
                &szExtent,
                &lWidth,
                &lHeight);
            if( lHeight <= rc.bottom - rc.top ) {
                NeedUpdate();
            }
        }*/
    }
}

/*
void CDuiRichEdit::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND )
	{
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CControlUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
    {
        if( m_pTxtWinHost && m_pTxtWinHost->DoSetCursor(NULL, &event.ptMouse) )
		{
            return;
        }
    }
    if( event.Type == UIEVENT_SETFOCUS )
	{
        if( m_pTxtWinHost )
		{
            m_pTxtWinHost->OnTxInPlaceActivate(NULL);
            m_pTxtWinHost->GetTextServices()->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
        }
		m_bFocused = true;
		Invalidate();
		return;
    }
    if( event.Type == UIEVENT_KILLFOCUS )
	{
        if( m_pTxtWinHost )
		{
            m_pTxtWinHost->OnTxInPlaceActivate(NULL);
            m_pTxtWinHost->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
        }
		m_bFocused = false;
		Invalidate();
		return;
    }
    if( event.Type == UIEVENT_TIMER )
	{
        if( m_pTxtWinHost )
		{
            m_pTxtWinHost->GetTextServices()->TxSendMessage(WM_TIMER, event.wParam, event.lParam, 0);
        } 
    }
    if( event.Type == UIEVENT_SCROLLWHEEL )
	{
        if( (event.wKeyState & MK_CONTROL) != 0  )
		{
            return;
        }
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK ) 
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE ) 
    {
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP ) 
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        return;
    }
    if( event.Type > UIEVENT__KEYBEGIN && event.Type < UIEVENT__KEYEND )
    {
        return;
    }
    CContainerUI::DoEvent(event);
}

SIZE CDuiRichEdit::EstimateSize(SIZE szAvailable)
{
    //return CSize(m_rcItem); // 这种方式在第一次设置大小之后就大小不变了
    return CContainerUI::EstimateSize(szAvailable);
}

void CDuiRichEdit::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);
    rc = m_rcItem;

    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;
    bool bVScrollBarVisiable = false;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        bVScrollBarVisiable = true;
        rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    }
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
        rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
    }

    if( m_pTxtWinHost ) {
        m_pTxtWinHost->SetClientRect(&rc);
        if( bVScrollBarVisiable && (!m_pVerticalScrollBar->IsVisible() || m_bVScrollBarFixing) ) {
            LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            m_pTxtWinHost->GetTextServices()->TxGetNaturalSize(
                DVASPECT_CONTENT, 
                GetHWND(), 
                NULL,
                NULL,
                TXTNS_FITTOCONTENT,
                &szExtent,
                &lWidth,
                &lHeight);
            if( lHeight > rc.bottom - rc.top ) {
                m_pVerticalScrollBar->SetVisible(true);
                m_pVerticalScrollBar->SetScrollPos(0);
                m_bVScrollBarFixing = true;
            }
            else {
                if( m_bVScrollBarFixing ) {
                    m_pVerticalScrollBar->SetVisible(false);
                    m_bVScrollBarFixing = false;
                }
            }
        }
    }

    if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
        RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom};
        m_pVerticalScrollBar->SetPos(rcScrollBarPos);
    }
    if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
        RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
        m_pHorizontalScrollBar->SetPos(rcScrollBarPos);
    }

    for( int it = 0; it < m_items.GetSize(); it++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) {
            SetFloatPos(it);
        }
        else {
            pControl->SetPos(rc); // 所有非float子控件放大到整个客户区
        }
    }
}

void CDuiRichEdit::DoPaint(HDC hDC, const RECT& rcPaint)
{
    RECT rcTemp = { 0 };
    if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

    CRenderClip clip;
    CRenderClip::GenerateClip(hDC, rcTemp, clip);
    CControlUI::DoPaint(hDC, rcPaint);

    if( m_pTxtWinHost ) {
        RECT rc;
        m_pTxtWinHost->GetControlRect(&rc);
        // Remember wparam is actually the hdc and lparam is the update
        // rect because this message has been preprocessed by the window.
        m_pTxtWinHost->GetTextServices()->TxDraw(
            DVASPECT_CONTENT,  		// Draw Aspect
            0,				// Lindex
            NULL,					// Info for drawing optimazation
            NULL,					// target device information
            hDC,			        // Draw device HDC
            NULL, 				   	// Target device HDC
            (RECTL*)&rc,			// Bounding client rectangle
            NULL, 		            // Clipping rectangle for metafiles
            (RECT*)&rcPaint,		// Update rectangle
            NULL, 	   				// Call back function
            NULL,					// Call back parameter
            0);				        // What view of the object
        if( m_bVScrollBarFixing ) {
            LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            m_pTxtWinHost->GetTextServices()->TxGetNaturalSize(
                DVASPECT_CONTENT, 
                GetHWND(), 
                NULL,
                NULL,
                TXTNS_FITTOCONTENT,
                &szExtent,
                &lWidth,
                &lHeight);
            if( lHeight <= rc.bottom - rc.top ) {
                NeedUpdate();
            }
        }
    }

    if( m_items.GetSize() > 0 ) {
        RECT rc = m_rcItem;
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;
        if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

        if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl ->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    pControl->DoPaint(hDC, rcPaint);
                }
            }
        }
        else {
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl ->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    CRenderClip::UseOldClipBegin(hDC, childClip);
                    pControl->DoPaint(hDC, rcPaint);
                    CRenderClip::UseOldClipEnd(hDC, childClip);
                }
                else {
                    if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
                    pControl->DoPaint(hDC, rcPaint);
                }
            }
        }
    }

    if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
        if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
            m_pVerticalScrollBar->DoPaint(hDC, rcPaint);
        }
    }

    if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
        if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
            m_pHorizontalScrollBar->DoPaint(hDC, rcPaint);
        }
    }
}
*/
LRESULT CDuiRichEdit::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    //if( !GetVisible() || !IsEnabled() ) return 0;
    if( !GetRresponse() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
    if( uMsg == WM_MOUSEWHEEL && (LOWORD(wParam) & MK_CONTROL) == 0 ) return 0;

	if (uMsg == WM_IME_COMPOSITION)
	{
		// 解决微软输入法位置异常的问题
		HIMC hIMC = ImmGetContext(GetHWND());
		if (hIMC) 
		{
			// Set composition window position near caret position
			POINT point;
			GetCaretPos(&point);

			COMPOSITIONFORM Composition;
			Composition.dwStyle = CFS_POINT;
			Composition.ptCurrentPos.x = point.x;
			Composition.ptCurrentPos.y = point.y;
			ImmSetCompositionWindow(hIMC, &Composition);

			ImmReleaseContext(GetHWND(),hIMC);
		}
		return 0;
	}

    bool bWasHandled = true;
    if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
        if( !m_pTxtWinHost->IsCaptured() ) {
            switch (uMsg) {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                {
                    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    /*CControlUI* pHover = GetManager()->FindControl(pt);
                    if(pHover != this) {
                        bWasHandled = false;
                        return 0;
                    }*/
                }
                break;
            }
        }
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = m_pTxtWinHost->IsCaptured() ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if( dwHitResult == HITRESULT_OUTSIDE ) {
            RECT rc;
            m_pTxtWinHost->GetControlRect(&rc);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if( uMsg == WM_MOUSEWHEEL ) ::ScreenToClient(GetHWND(), &pt);
            if( ::PtInRect(&rc, pt) && !IsFocusControl() ) dwHitResult = HITRESULT_HIT;
        }
        if( dwHitResult != HITRESULT_HIT ) return 0;
        if( uMsg == WM_SETCURSOR ) bWasHandled = false;
        else if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDOWN ) {
            OnFocus(TRUE);
        }
    }
#ifdef _UNICODE
    else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
#else
    else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR ) {
#endif
        if( !IsFocusControl() ) return 0;
    }
    else if( uMsg == WM_CONTEXTMENU ) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ::ScreenToClient(GetHWND(), &pt);
        /*CControlUI* pHover = GetManager()->FindControl(pt);
        if(pHover != this) {
            bWasHandled = false;
            return 0;
        }*/
    }
    else
    {
        switch( uMsg ) {
        case WM_HELP:
            bWasHandled = false;
            break;
        default:
            return 0;
        }
    }

	if(WM_CHAR == uMsg)
	{
#ifndef _UNICODE
		// check if we are waiting for 2 consecutive WM_CHAR messages
		if ( IsAccumulateDBCMode() )
		{
			if ( (GetKeyState(VK_KANA) & 0x1) )
			{
				// turn off accumulate mode
				SetAccumulateDBCMode ( false );
				m_chLeadByte = 0;
			}
			else
			{
				if ( !m_chLeadByte )
				{
					// This is the first WM_CHAR message, 
					// accumulate it if this is a LeadByte.  Otherwise, fall thru to
					// regular WM_CHAR processing.
					if ( IsDBCSLeadByte ( (BYTE)(WORD)wParam ) )
					{
						// save the Lead Byte and don't process this message
						m_chLeadByte = (WORD)wParam << 8 ;

						//TCHAR a = (WORD)wParam << 8 ;
						return 0;
					}
				}
				else
				{
					// This is the second WM_CHAR message,
					// combine the current byte with previous byte.
					// This DBC will be handled as WM_IME_CHAR.
					wParam |= m_chLeadByte;
					uMsg = WM_IME_CHAR;

					// setup to accumulate more WM_CHAR
					m_chLeadByte = 0; 
				}
			}
		}
#endif
	}

    LRESULT lResult = 0;
    HRESULT Hr = TxSendMessage(uMsg, wParam, lParam, &lResult);
    if( Hr == S_OK ) bHandled = bWasHandled;
    else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR )
        bHandled = bWasHandled;
    else if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) {
        if( m_pTxtWinHost->IsCaptured() ) bHandled = bWasHandled;
    }
    return lResult;
}

void CDuiRichEdit::SetAccumulateDBCMode( bool bDBCMode )
{
	m_fAccumulateDBC = bDBCMode;
}

bool CDuiRichEdit::IsAccumulateDBCMode()
{
	return m_fAccumulateDBC;
}
