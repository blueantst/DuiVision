#include "StdAfx.h"
#include "WndBase.h"
#include "DuiActiveX.h"
#include "exdispid.h"
#include <comdef.h>
#include <mshtmhst.h>	// IDocHostUIHandler使用

/////////////////////////////////////////////////////////////////////////////////////
//

class CActiveXCtrl;

/////////////////////////////////////////////////////////////////////////////////////
// CActiveXWnd

class CActiveXWnd : public CWindowWnd
{
public:
    HWND Init(CActiveXCtrl* pOwner, HWND hWndParent);

    virtual LPCTSTR GetWindowClassName() const;
    virtual void OnFinalMessage(HWND hWnd);

protected:
    void DoVerb(LONG iVerb);

	virtual LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    CActiveXCtrl* m_pOwner;
};


/////////////////////////////////////////////////////////////////////////////////////
// CActiveXEnum

class CActiveXEnum : public IEnumUnknown
{
public:
    CActiveXEnum(IUnknown* pUnk) : m_pUnk(pUnk), m_dwRef(1), m_iPos(0)
    {
        m_pUnk->AddRef();
    }
    ~CActiveXEnum()
    {
        m_pUnk->Release();
    }

    LONG m_iPos;
    ULONG m_dwRef;
    IUnknown* m_pUnk;

    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        LONG lRef = --m_dwRef;
        if( lRef == 0 ) delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if( riid == IID_IUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
        else if( riid == IID_IEnumUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
        if( *ppvObject != NULL ) AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }
    STDMETHOD(Next)(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
    {
        if( pceltFetched != NULL ) *pceltFetched = 0;
        if( ++m_iPos > 1 ) return S_FALSE;
        *rgelt = m_pUnk;
        (*rgelt)->AddRef();
        if( pceltFetched != NULL ) *pceltFetched = 1;
        return S_OK;
    }
    STDMETHOD(Skip)(ULONG celt)
    {
        m_iPos += celt;
        return S_OK;
    }
    STDMETHOD(Reset)(void)
    {
        m_iPos = 0;
        return S_OK;
    }
    STDMETHOD(Clone)(IEnumUnknown **ppenum)
    {
        return E_NOTIMPL;
    }
};


/////////////////////////////////////////////////////////////////////////////////////
// CActiveXFrameWnd

class CActiveXFrameWnd : public IOleInPlaceFrame
{
public:
    CActiveXFrameWnd(CDuiActiveX* pOwner)
		: m_dwRef(1), m_pOwner(pOwner), m_pActiveObject(NULL)
    {
    }
    ~CActiveXFrameWnd()
    {
        if( m_pActiveObject != NULL ) m_pActiveObject->Release();
    }

    ULONG						m_dwRef;
    CDuiActiveX*				m_pOwner;
    IOleInPlaceActiveObject*	m_pActiveObject;

    // IUnknown
    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        ULONG lRef = --m_dwRef;
        if( lRef == 0 ) delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if( riid == IID_IUnknown ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if( riid == IID_IOleWindow ) *ppvObject = static_cast<IOleWindow*>(this);
        else if( riid == IID_IOleInPlaceFrame ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if( riid == IID_IOleInPlaceUIWindow ) *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);
        if( *ppvObject != NULL ) AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }  
    // IOleInPlaceFrameWindow
    STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
    {
        return S_OK;
    }
    STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
    {
        return S_OK;
    }
    STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
    {
        return S_OK;
    }
    STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
    {
        return S_OK;
    }
    STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
    {
        return S_FALSE;
    }
    // IOleWindow
    STDMETHOD(GetWindow)(HWND* phwnd)
    {
        if( m_pOwner == NULL ) return E_UNEXPECTED;
        *phwnd = m_pOwner->GetPaintHWnd();
        return S_OK;
    }
    STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
    {
        return S_OK;
    }
    // IOleInPlaceUIWindow
    STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
    {
        return S_OK;
    }
    STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return INPLACE_E_NOTOOLSPACE;
    }
    STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
    {
        if( pActiveObject != NULL ) pActiveObject->AddRef();
        if( m_pActiveObject != NULL ) m_pActiveObject->Release();
        m_pActiveObject = pActiveObject;
        return S_OK;
    }
};

/////////////////////////////////////////////////////////////////////////////
// CActiveXCommandHandler -- activex控件的命令处理类,用于控件和宿主程序的交互调用

// 定义DISPID
#define DISPID_EX_HANDLER	1

class CActiveXCommandHandler : public IDispatch
{
	long		refcount;
	_bstr_t		m_bstrParam;	// 参数名
public:

	CActiveXCommandHandler() {refcount = 0;}
	//virtual ~CActiveXCommandHandler() {}

	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (iid == IID_IDispatch)
		{
			*ppvObj = (IDispatch*)this;
			AddRef();
		}else
		{
			return E_NOINTERFACE;
			//return E_NOTIMPL;
		}
		return S_OK;
	}

	STDMETHOD_(ULONG,AddRef)()
	{
		InterlockedIncrement(&refcount);
		return refcount;
	}

	STDMETHOD_(ULONG,Release)()
	{
		InterlockedDecrement(&refcount);
		if (refcount == 0)
			delete this;
		return refcount;
	}

	STDMETHOD(GetTypeInfoCount)(/* [out] */ UINT *pctinfo)
	{
		return S_OK;
	}

	STDMETHOD(GetTypeInfo)(
      /* [in] */ UINT iTInfo,
      /* [in] */ LCID lcid,
      /* [out] */ ITypeInfo **ppTInfo)
	{
		return S_OK;
	}

	STDMETHOD(GetIDsOfNames)( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId)
	{
		m_bstrParam = *rgszNames;
		*rgDispId = DISPID_EX_HANDLER;
		return S_OK;
	}

	STDMETHOD(Invoke)( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr)
	{
		if(dispIdMember==DISPID_EX_HANDLER)
		{
			if(wFlags == DISPATCH_PROPERTYGET)	// 获取属性
			{
				/*IInterp* pInterp = GetCurrentInterp();
				if(pInterp)
				{
					CString strValue = pInterp->GetVar(m_bstrParam);
					COleVariant* pRetVal = (COleVariant*)pVarResult;
					*pRetVal = strValue;
				}*/
			}else
			if(wFlags == DISPATCH_PROPERTYPUT)	// 设置属性
			{
				/*IInterp* pInterp = GetCurrentInterp();
				if(pInterp)
				{
					if(pDispParams->cArgs > 0)
					{
						COleVariant* pRetVal = (COleVariant*)(pDispParams->rgvarg);
						CString str(pRetVal->bstrVal);
						pInterp->SetVar(m_bstrParam, str);
					}
				}*/
			}else
			if((wFlags & DISPATCH_METHOD) != 0)	// 调用方法
			{
				/*IInterp* pInterp = GetCurrentInterp();
				if(pInterp)
				{
					CStringArray asParam;
					// 说明:参数数组pDispParams为倒序,且命名参数在前面,此处省略命名参数
					int nArgs = pDispParams->cArgs + pDispParams->cNamedArgs;
					int nNamedArgs = pDispParams->cNamedArgs;
					for(int i = nArgs-1; i >= nNamedArgs; i--)
					{
						COleVariant* pRetVal = (COleVariant*)(&(pDispParams->rgvarg[i]));
						CString str(pRetVal->bstrVal);
						asParam.Add(str);
					}
					CString strRes = "";
					if(pInterp->CallMethod(m_bstrParam, asParam))
					{
						strRes = pInterp->GetResult();
					}else
					{
						strRes = pInterp->GetResult();
					}
					if((wFlags & DISPATCH_PROPERTYGET) != 0)
					{
						// 可以获取返回值
						COleVariant* pRetVal = (COleVariant*)pVarResult;
						*pRetVal = strRes;
					}
				}*/
			}

              //MessageBox(0,"Hello World","Hello",0); //place your code here
			  //frmweb->Edit1->Text="Hello World(这也可以？)";
		}

		return S_OK;
	}
	
};

/////////////////////////////////////////////////////////////////////////////////////
// CActiveXCtrl

class CActiveXCtrl :
    public IOleClientSite,
    public IOleInPlaceSiteWindowless,
    public IOleControlSite,
    public IObjectWithSite,
    public IOleContainer,
	public IDocHostUIHandler
{
    friend CDuiActiveX;
    friend CActiveXWnd;
public:
    CActiveXCtrl();
    ~CActiveXCtrl();

public:
    // IUnknown
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

    // IObjectWithSite
    STDMETHOD(SetSite)(IUnknown *pUnkSite);
    STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);

    // IOleClientSite
    STDMETHOD(SaveObject)(void);       
    STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
    STDMETHOD(GetContainer)(IOleContainer** ppContainer);        
    STDMETHOD(ShowObject)(void);        
    STDMETHOD(OnShowWindow)(BOOL fShow);        
    STDMETHOD(RequestNewObjectLayout)(void);

    // IOleInPlaceSiteWindowless
    STDMETHOD(CanWindowlessActivate)(void);
    STDMETHOD(GetCapture)(void);
    STDMETHOD(SetCapture)(BOOL fCapture);
    STDMETHOD(GetFocus)(void);
    STDMETHOD(SetFocus)(BOOL fFocus);
    STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
    STDMETHOD(ReleaseDC)(HDC hDC);
    STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
    STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
    STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
    STDMETHOD(AdjustRect)(LPRECT prc);
    STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

    // IOleInPlaceSiteEx
    STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);        
    STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);       
    STDMETHOD(RequestUIActivate)(void);

    // IOleInPlaceSite
    STDMETHOD(CanInPlaceActivate)(void);       
    STDMETHOD(OnInPlaceActivate)(void);        
    STDMETHOD(OnUIActivate)(void);
    STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
    STDMETHOD(Scroll)(SIZE scrollExtant);
    STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
    STDMETHOD(OnInPlaceDeactivate)(void);
    STDMETHOD(DiscardUndoState)( void);
    STDMETHOD(DeactivateAndUndo)( void);
    STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

    // IOleWindow
    STDMETHOD(GetWindow)(HWND* phwnd);
    STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

    // IOleControlSite
    STDMETHOD(OnControlInfoChanged)(void);      
    STDMETHOD(LockInPlaceActive)(BOOL fLock);       
    STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);        
    STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);       
    STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
    STDMETHOD(OnFocus)(BOOL fGotFocus);
    STDMETHOD(ShowPropertyFrame)(void);

    // IOleContainer
    STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
    STDMETHOD(LockContainer)(BOOL fLock);

    // IParseDisplayName
    STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);
/*
	//DWebBrowserEvents2
    //IDispatch methods
    STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo);
    STDMETHOD(GetTypeInfo)(UINT itinfo,LCID lcid,ITypeInfo FAR* FAR* pptinfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid,OLECHAR FAR* FAR* rgszNames,UINT cNames,LCID lcid, DISPID FAR* rgdispid);
    STDMETHOD(Invoke)(DISPID dispidMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,EXCEPINFO FAR* pexcepinfo,UINT FAR* puArgErr);
*/
	// IDocHostUIHandler
	STDMETHOD(ShowContextMenu)(/* [in] */ DWORD dwID,
            /* [in] */ POINT __RPC_FAR *ppt,
            /* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
            /* [in] */ IDispatch __RPC_FAR *pdispReserved);
	STDMETHOD(GetHostInfo)( 
            /* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo);
	STDMETHOD(ShowUI)( 
            /* [in] */ DWORD dwID,
            /* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
            /* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
            /* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
            /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc);
	STDMETHOD(HideUI)(void);
	STDMETHOD(UpdateUI)(void);
	STDMETHOD(EnableModeless)(/* [in] */ BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)(/* [in] */ BOOL fEnable);
	STDMETHOD(OnFrameWindowActivate)(/* [in] */ BOOL fEnable);
	STDMETHOD(ResizeBorder)( 
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
            /* [in] */ BOOL fRameWindow);
	STDMETHOD(TranslateAccelerator)( 
            /* [in] */ LPMSG lpMsg,
            /* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
            /* [in] */ DWORD nCmdID);
	STDMETHOD(GetOptionKeyPath)( 
            /* [out] */ LPOLESTR __RPC_FAR *pchKey,
            /* [in] */ DWORD dw);
	STDMETHOD(GetDropTarget)(
            /* [in] */ IDropTarget __RPC_FAR *pDropTarget,
            /* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget);
    STDMETHOD(GetExternal)( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
    STDMETHOD(TranslateUrl)( 
            /* [in] */ DWORD dwTranslate,
            /* [in] */ OLECHAR __RPC_FAR *pchURLIn,
            /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
    STDMETHOD(FilterDataObject)( 
            /* [in] */ IDataObject __RPC_FAR *pDO,
            /* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet);

protected:
    HRESULT CreateActiveXWnd();

protected:
    LONG			m_dwRef;
    CDuiActiveX*	m_pOwner;
    CActiveXWnd*	m_pWindow;
    IUnknown*		m_pUnkSite;
    IViewObject*	m_pViewObject;
    IOleInPlaceObjectWindowless* m_pInPlaceObject;
    bool			m_bLocked;
    bool			m_bFocused;
    bool			m_bCaptured;
    bool			m_bUIActivated;
    bool			m_bInPlaceActive;
    bool			m_bWindowless;
};

CActiveXCtrl::CActiveXCtrl() : 
	m_dwRef(1), 
	m_pOwner(NULL), 
	m_pWindow(NULL),
	m_pUnkSite(NULL), 
	m_pViewObject(NULL),
	m_pInPlaceObject(NULL),
	m_bLocked(false), 
	m_bFocused(false),
	m_bCaptured(false),
	m_bWindowless(false),
	m_bUIActivated(false),
	m_bInPlaceActive(false)
{
}

CActiveXCtrl::~CActiveXCtrl()
{
    if( m_pWindow != NULL )
	{
        ::DestroyWindow(*m_pWindow);
        delete m_pWindow;
    }
    if( m_pUnkSite != NULL ) m_pUnkSite->Release();
    if( m_pViewObject != NULL ) m_pViewObject->Release();
    if( m_pInPlaceObject != NULL ) m_pInPlaceObject->Release();
}

STDMETHODIMP CActiveXCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
    *ppvObject = NULL;
    if( riid == IID_IUnknown )                       *ppvObject = static_cast<IOleWindow*>(this);
    else if( riid == IID_IOleClientSite )            *ppvObject = static_cast<IOleClientSite*>(this);
    else if( riid == IID_IOleInPlaceSiteWindowless ) *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
    else if( riid == IID_IOleInPlaceSiteEx )         *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
    else if( riid == IID_IOleInPlaceSite )           *ppvObject = static_cast<IOleInPlaceSite*>(this);
    else if( riid == IID_IOleWindow )                *ppvObject = static_cast<IOleWindow*>(this);
    else if( riid == IID_IOleControlSite )           *ppvObject = static_cast<IOleControlSite*>(this);
    else if( riid == IID_IOleContainer )             *ppvObject = static_cast<IOleContainer*>(this);
    else if( riid == IID_IObjectWithSite )           *ppvObject = static_cast<IObjectWithSite*>(this);
	else if( riid == IID_IDocHostUIHandler )         *ppvObject = static_cast<IDocHostUIHandler*>(this);
    if( *ppvObject != NULL ) AddRef();
    return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG) CActiveXCtrl::AddRef()
{
    return ++m_dwRef;
}

STDMETHODIMP_(ULONG) CActiveXCtrl::Release()
{
    LONG lRef = --m_dwRef;
    if( lRef == 0 ) delete this;
    return lRef;
}

STDMETHODIMP CActiveXCtrl::SetSite(IUnknown *pUnkSite)
{
    TRACE(_T("AX: CActiveXCtrl::SetSite\n"));
    if( m_pUnkSite != NULL )
	{
        m_pUnkSite->Release();
        m_pUnkSite = NULL;
    }
    if( pUnkSite != NULL )
	{
        m_pUnkSite = pUnkSite;
        m_pUnkSite->AddRef();
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetSite(REFIID riid, LPVOID* ppvSite)
{
    TRACE(_T("AX: CActiveXCtrl::GetSite\n"));
    if( ppvSite == NULL ) return E_POINTER;
    *ppvSite = NULL;
    if( m_pUnkSite == NULL ) return E_FAIL;
    return m_pUnkSite->QueryInterface(riid, ppvSite);
}

STDMETHODIMP CActiveXCtrl::SaveObject(void)
{
    TRACE(_T("AX: CActiveXCtrl::SaveObject\n"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
{
    TRACE(_T("AX: CActiveXCtrl::GetMoniker\n"));
    if( ppmk != NULL ) *ppmk = NULL;
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetContainer(IOleContainer** ppContainer)
{
    TRACE(_T("AX: CActiveXCtrl::GetContainer\n"));
    if( ppContainer == NULL ) return E_POINTER;
    *ppContainer = NULL;
    HRESULT Hr = E_NOTIMPL;
    if( m_pUnkSite != NULL )
	{
		Hr = m_pUnkSite->QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
	}
    if( FAILED(Hr) )
	{
		Hr = QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
	}
    return Hr;
}

STDMETHODIMP CActiveXCtrl::ShowObject(void)
{
    TRACE(_T("AX: CActiveXCtrl::ShowObject\n"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    HDC hDC = ::GetDC(m_pOwner->m_hwndHost);
    if( hDC == NULL ) return E_FAIL;
    if( m_pViewObject != NULL )
	{
		m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &m_pOwner->m_rc, (RECTL*) &m_pOwner->m_rc, NULL, NULL);
	}
    ::ReleaseDC(m_pOwner->m_hwndHost, hDC);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnShowWindow(BOOL fShow)
{
    TRACE(_T("AX: CActiveXCtrl::OnShowWindow\n"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::RequestNewObjectLayout(void)
{
    TRACE(_T("AX: CActiveXCtrl::RequestNewObjectLayout\n"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::CanWindowlessActivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::CanWindowlessActivate\n"));
    return S_OK;  // Yes, we can!!
}

STDMETHODIMP CActiveXCtrl::GetCapture(void)
{
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if(!m_pOwner->IsControlVisible())
	{
		return S_FALSE;
	}

    TRACE(_T("AX: CActiveXCtrl::GetCapture\n"));
    return m_bCaptured ? S_OK : S_FALSE;
}

STDMETHODIMP CActiveXCtrl::SetCapture(BOOL fCapture)
{
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if(!m_pOwner->IsControlVisible())
	{
		return S_FALSE;
	}

    TRACE(_T("AX: CActiveXCtrl::SetCapture\n"));
    m_bCaptured = (fCapture == TRUE);
    if( fCapture ) ::SetCapture(m_pOwner->m_hwndHost); else ::ReleaseCapture();
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetFocus(void)
{
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if(!m_pOwner->IsControlVisible())
	{
		return S_FALSE;
	}

    TRACE(_T("AX: CActiveXCtrl::GetFocus\n"));
    return m_bFocused ? S_OK : S_FALSE;
}

STDMETHODIMP CActiveXCtrl::SetFocus(BOOL fFocus)
{
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if(!m_pOwner->IsControlVisible())
	{
		return S_FALSE;
	}

    TRACE(_T("AX: CActiveXCtrl::SetFocus\n"));
    if( fFocus ) m_pOwner->SetControlFocus(fFocus);
    m_bFocused = (fFocus == TRUE);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
{
	if( phDC == NULL ) return E_POINTER;
    if( m_pOwner == NULL ) return E_UNEXPECTED;
	if(!m_pOwner->IsControlVisible())
	{
		return S_FALSE;
	}

    TRACE(_T("AX: CActiveXCtrl::GetDC\n"));
	*phDC = ::GetDC(m_pOwner->m_hwndHost);
    if( (grfFlags & OLEDC_PAINTBKGND) != 0 )
	{
        CRect rcItem = m_pOwner->GetRect();
        if( !m_bWindowless ) rcItem.MoveToXY(0, 0);
        ::FillRect(*phDC, &rcItem, (HBRUSH) (COLOR_WINDOW + 1));
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ReleaseDC(HDC hDC)
{
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if(!m_pOwner->IsControlVisible())
	{
		return S_FALSE;
	}

    TRACE(_T("AX: CActiveXCtrl::ReleaseDC\n"));
    ::ReleaseDC(m_pOwner->m_hwndHost, hDC);
    return S_OK;
}

// 控件的刷新界面操作
STDMETHODIMP CActiveXCtrl::InvalidateRect(LPCRECT pRect, BOOL fErase)
{
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if(!m_pOwner->IsControlVisible())
	{
		return S_FALSE;
	}

    TRACE(_T("AX: CActiveXCtrl::InvalidateRect\n"));
    if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
	//return ::InvalidateRect(m_pOwner->m_hwndHost, pRect, fErase) ? S_OK : E_FAIL;
	// 调用对应DUI控件的刷新函数
	m_pOwner->UpdateControl();
	return S_OK;
}

STDMETHODIMP CActiveXCtrl::InvalidateRgn(HRGN hRGN, BOOL fErase)
{
	if( m_pOwner == NULL ) return E_UNEXPECTED;
	if(!m_pOwner->IsControlVisible())
	{
		return S_FALSE;
	}

    TRACE(_T("AX: CActiveXCtrl::InvalidateRgn\n"));
    return ::InvalidateRgn(m_pOwner->m_hwndHost, hRGN, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP CActiveXCtrl::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
{
    TRACE(_T("AX: CActiveXCtrl::ScrollRect\n"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::AdjustRect(LPRECT prc)
{
    TRACE(_T("AX: CActiveXCtrl::AdjustRect\n"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{
    TRACE(_T("AX: CActiveXCtrl::OnDefWindowMessage\n"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *plResult = ::DefWindowProc(m_pOwner->m_hwndHost, msg, wParam, lParam);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceActivateEx(BOOL* pfNoRedraw, DWORD dwFlags)        
{
    TRACE(_T("AX: CActiveXCtrl::OnInPlaceActivateEx\n"));
    ASSERT(m_pInPlaceObject==NULL);
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
    ::OleLockRunning(m_pOwner->m_pUnk, TRUE, FALSE);
    HWND hWndFrame = m_pOwner->GetPaintHWnd();
    HRESULT Hr = E_FAIL;
    if( (dwFlags & ACTIVATE_WINDOWLESS) != 0 )
	{
		// 创建无窗口实例
        m_bWindowless = true;
        Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*) &m_pInPlaceObject);
        m_pOwner->m_hwndHost = hWndFrame;
        //m_pOwner->GetManager()->AddMessageFilter(m_pOwner);
		Hr = CreateActiveXWnd();
        if( FAILED(Hr) ) return Hr;
    }
    if( FAILED(Hr) )
	{
		// 创建窗口
        m_bWindowless = false;
        Hr = CreateActiveXWnd();
        if( FAILED(Hr) ) return Hr;
        Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObject, (LPVOID*) &m_pInPlaceObject);
    }
    if( m_pInPlaceObject != NULL )
	{
        CRect rcItem = m_pOwner->m_rc;
        if( !m_bWindowless ) rcItem.MoveToXY(0, 0);
        m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
    }
    m_bInPlaceActive = SUCCEEDED(Hr);
    return Hr;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivateEx(BOOL fNoRedraw)       
{
    TRACE(_T("AX: CActiveXCtrl::OnInPlaceDeactivateEx\n"));
    m_bInPlaceActive = false;
    if( m_pInPlaceObject != NULL )
	{
        m_pInPlaceObject->Release();
        m_pInPlaceObject = NULL;
    }
    if( m_pWindow != NULL )
	{
        ::DestroyWindow(*m_pWindow);
        delete m_pWindow;
        m_pWindow = NULL;
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::RequestUIActivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::RequestUIActivate\n"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::CanInPlaceActivate(void)       
{
    TRACE(_T("AX: CActiveXCtrl::CanInPlaceActivate\n"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceActivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::OnInPlaceActivate\n"));
    BOOL bDummy = FALSE;
    return OnInPlaceActivateEx(&bDummy, 0);
}

STDMETHODIMP CActiveXCtrl::OnUIActivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::OnUIActivate\n"));
    m_bUIActivated = true;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    TRACE(_T("AX: CActiveXCtrl::GetWindowContext\n"));
    if( ppDoc == NULL ) return E_POINTER;
    if( ppFrame == NULL ) return E_POINTER;
    if( lprcPosRect == NULL ) return E_POINTER;
    if( lprcClipRect == NULL ) return E_POINTER;

	CRect rcItem = m_pOwner->GetRect(); 
	memcpy(lprcPosRect, &rcItem, sizeof(rcItem));  
	memcpy(lprcClipRect, &rcItem, sizeof(rcItem));  
	
    *ppFrame = new CActiveXFrameWnd(m_pOwner);
    *ppDoc = NULL;
    ACCEL ac = { 0 };
    HACCEL hac = ::CreateAcceleratorTable(&ac, 1);
    lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
    lpFrameInfo->fMDIApp = FALSE;
    lpFrameInfo->hwndFrame = m_pOwner->GetPaintHWnd();
    lpFrameInfo->haccel = hac;
    lpFrameInfo->cAccelEntries = 1;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::Scroll(SIZE scrollExtant)
{
    TRACE(_T("AX: CActiveXCtrl::Scroll\n"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::OnUIDeactivate(BOOL fUndoable)
{
    TRACE(_T("AX: CActiveXCtrl::OnUIDeactivate\n"));
    m_bUIActivated = false;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::OnInPlaceDeactivate\n"));
    return OnInPlaceDeactivateEx(TRUE);
}

STDMETHODIMP CActiveXCtrl::DiscardUndoState(void)
{
    TRACE(_T("AX: CActiveXCtrl::DiscardUndoState\n"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::DeactivateAndUndo(void)
{
    TRACE(_T("AX: CActiveXCtrl::DeactivateAndUndo\n"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::OnPosRectChange(LPCRECT lprcPosRect)
{
    TRACE(_T("AX: CActiveXCtrl::OnPosRectChange\n"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetWindow(HWND* phwnd)
{
    if( m_pOwner == NULL ) return E_UNEXPECTED;

	if(!m_pOwner->IsControlVisible())
	{
		return S_FALSE;
	}

	TRACE(_T("AX: CActiveXCtrl::GetWindow\n"));

    if( m_pOwner->m_hwndHost == NULL ) CreateActiveXWnd();
    if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
    *phwnd = m_pOwner->m_hwndHost;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ContextSensitiveHelp(BOOL fEnterMode)
{
    TRACE(_T("AX: CActiveXCtrl::ContextSensitiveHelp\n"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnControlInfoChanged(void)      
{
    TRACE(_T("AX: CActiveXCtrl::OnControlInfoChanged\n"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::LockInPlaceActive(BOOL fLock)       
{
    TRACE(_T("AX: CActiveXCtrl::LockInPlaceActive\n"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetExtendedControl(IDispatch** ppDisp)        
{
    TRACE(_T("AX: CActiveXCtrl::GetExtendedControl\n"));
    if( ppDisp == NULL ) return E_POINTER;   
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
    return m_pOwner->m_pUnk->QueryInterface(IID_IDispatch, (LPVOID*) ppDisp);
}

STDMETHODIMP CActiveXCtrl::TransformCoords(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags)       
{
    TRACE(_T("AX: CActiveXCtrl::TransformCoords\n"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
{
    TRACE(_T("AX: CActiveXCtrl::TranslateAccelerator\n"));
    return S_FALSE;
}

STDMETHODIMP CActiveXCtrl::OnFocus(BOOL fGotFocus)
{
    TRACE(_T("AX: CActiveXCtrl::OnFocus\n"));
    m_bFocused = (fGotFocus == TRUE);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ShowPropertyFrame(void)
{
    TRACE(_T("AX: CActiveXCtrl::ShowPropertyFrame\n"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::EnumObjects(DWORD grfFlags, IEnumUnknown** ppenum)
{
    TRACE(_T("AX: CActiveXCtrl::EnumObjects\n"));
    if( ppenum == NULL ) return E_POINTER;
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *ppenum = new CActiveXEnum(m_pOwner->m_pUnk);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::LockContainer(BOOL fLock)
{
    TRACE(_T("AX: CActiveXCtrl::LockContainer\n"));
    m_bLocked = fLock != FALSE;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut)
{
    TRACE(_T("AX: CActiveXCtrl::ParseDisplayName\n"));
    return E_NOTIMPL;
}

STDMETHODIMP  CActiveXCtrl::GetHostInfo( DOCHOSTUIINFO* pInfo )
{
	TRACE(_T("AX: CActiveXCtrl::GetHostInfo\n"));
	// 显示为新式用户界面
	//pInfo->dwFlags = DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE |DOCHOSTUIFLAG_FLAT_SCROLLBAR|0x40000;
	pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER;	// 去除3D边框
	if(m_pOwner && !m_pOwner->GetShowScroll())
	{
		pInfo->dwFlags |= DOCHOSTUIFLAG_SCROLL_NO;	// 不显示滚动条
	}
    pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
    return S_OK;
}

STDMETHODIMP  CActiveXCtrl::ShowUI(
				DWORD /*dwID*/, 
				IOleInPlaceActiveObject * /*pActiveObject*/,
				IOleCommandTarget * /*pCommandTarget*/,
				IOleInPlaceFrame * /*pFrame*/,
				IOleInPlaceUIWindow * /*pDoc*/)
{
	TRACE(_T("AX: CActiveXCtrl::ShowUI\n"));
    return S_OK;
}

STDMETHODIMP  CActiveXCtrl::HideUI(void)
{
	TRACE(_T("AX: CActiveXCtrl::HideUI\n"));
    return S_OK;
}

STDMETHODIMP  CActiveXCtrl::UpdateUI(void)
{
	TRACE(_T("AX: CActiveXCtrl::UpdateUI\n"));
	return S_OK;
}

STDMETHODIMP  CActiveXCtrl::EnableModeless(BOOL /*fEnable*/)
{
	TRACE(_T("AX: CActiveXCtrl::EnableModeless\n"));
    return E_NOTIMPL;
}

STDMETHODIMP  CActiveXCtrl::OnDocWindowActivate(BOOL /*fActivate*/)
{
	TRACE(_T("AX: CActiveXCtrl::OnDocWindowActivate\n"));
    return E_NOTIMPL;
}

STDMETHODIMP  CActiveXCtrl::OnFrameWindowActivate(BOOL /*fActivate*/)
{
	TRACE(_T("AX: CActiveXCtrl::OnFrameWindowActivate\n"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::ResizeBorder(
				LPCRECT /*prcBorder*/, 
				IOleInPlaceUIWindow* /*pUIWindow*/,
				BOOL /*fRameWindow*/)
{
	TRACE(_T("AX: CActiveXCtrl::ResizeBorder\n"));
    return E_NOTIMPL;
}

STDMETHODIMP  CActiveXCtrl::ShowContextMenu(
				DWORD /*dwID*/, 
				POINT* /*pptPosition*/,
				IUnknown* /*pCommandTarget*/,
				IDispatch* /*pDispatchObjectHit*/)
{
	TRACE(_T("AX: CActiveXCtrl::ShowContextMenu\n"));
	if(m_pOwner)
	{
		return m_pOwner->GetShowContentMenu() ? S_FALSE : S_OK;
	}
	return E_NOTIMPL;
}

STDMETHODIMP  CActiveXCtrl::TranslateAccelerator(LPMSG /*lpMsg*/,
            /* [in] */ const GUID __RPC_FAR* /*pguidCmdGroup*/,
            /* [in] */ DWORD /*nCmdID*/)
{
	TRACE(_T("AX: CActiveXCtrl::TranslateAccelerator\n"));
    return S_FALSE;
}

STDMETHODIMP  CActiveXCtrl::GetOptionKeyPath(BSTR* /*pbstrKey*/, DWORD)
{
	TRACE(_T("AX: CActiveXCtrl::GetOptionKeyPath\n"));
	return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetDropTarget( 
            /* [in] */ IDropTarget __RPC_FAR* /*pDropTarget*/,
            /* [out] */ IDropTarget __RPC_FAR *__RPC_FAR* /*ppDropTarget*/)
{
	TRACE(_T("AX: CActiveXCtrl::GetDropTarget\n"));
	return E_NOTIMPL;
}

// 获取扩展的命令处理对象,用于控件和宿主程序的交互,调用宿主程序的函数
STDMETHODIMP CActiveXCtrl::GetExternal( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR* ppDispatch)
{
	TRACE(_T("AX: CActiveXCtrl::GetExternal\n"));
	// 创建扩展命令处理器对象
	CActiveXCommandHandler* pHandler = new CActiveXCommandHandler();
	pHandler->QueryInterface(IID_IDispatch, (void**)ppDispatch);
	return S_OK;
	//return E_NOTIMPL;
}
        
STDMETHODIMP CActiveXCtrl::TranslateUrl( 
            /* [in] */ DWORD /*dwTranslate*/,
            /* [in] */ OLECHAR __RPC_FAR* /*pchURLIn*/,
            /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR* /*ppchURLOut*/)
{
	TRACE(_T("AX: CActiveXCtrl::TranslateUrl\n"));
    return E_NOTIMPL;
}
        
STDMETHODIMP CActiveXCtrl::FilterDataObject( 
            /* [in] */ IDataObject __RPC_FAR* /*pDO*/,
            /* [out] */ IDataObject __RPC_FAR *__RPC_FAR* /*ppDORet*/)
{
	TRACE(_T("AX: CActiveXCtrl::FilterDataObject\n"));
    return E_NOTIMPL;
}

// 创建ActiveX窗口
HRESULT CActiveXCtrl::CreateActiveXWnd()
{
    if( m_pWindow != NULL )
	{
		return S_OK;
	}
    m_pWindow = new CActiveXWnd;
    if( m_pWindow == NULL )
	{
		return E_OUTOFMEMORY;
	}
    m_pOwner->m_hwndHost = m_pWindow->Init(this, m_pOwner->GetPaintHWnd());
    return S_OK;
}


class CWebBrowserCtrl :
    public CActiveXCtrl,
	public DWebBrowserEvents2
{
    friend CDuiActiveX;
    friend CActiveXWnd;
public:
    CWebBrowserCtrl();
    ~CWebBrowserCtrl();

public:
    // IUnknown
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

	//DWebBrowserEvents2
    //IDispatch methods
    STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo);
    STDMETHOD(GetTypeInfo)(UINT itinfo,LCID lcid,ITypeInfo FAR* FAR* pptinfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid,OLECHAR FAR* FAR* rgszNames,UINT cNames,LCID lcid, DISPID FAR* rgdispid);
    STDMETHOD(Invoke)(DISPID dispidMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,EXCEPINFO FAR* pexcepinfo,UINT FAR* puArgErr);
};


/////////////////////////////////////////////////////////////////
// 浏览器控件
CWebBrowserCtrl::CWebBrowserCtrl() : 
	CActiveXCtrl()
{
}

CWebBrowserCtrl::~CWebBrowserCtrl()
{
}

STDMETHODIMP_(ULONG) CWebBrowserCtrl::AddRef()
{
    return ++m_dwRef;
}

STDMETHODIMP_(ULONG) CWebBrowserCtrl::Release()
{
    LONG lRef = --m_dwRef;
    if( lRef == 0 ) delete this;
    return lRef;
}

STDMETHODIMP CWebBrowserCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	CActiveXCtrl::QueryInterface(riid, ppvObject);
	if(*ppvObject)
	{
		return S_OK;
	}

	if ((riid == DIID_DWebBrowserEvents2) || (riid == IID_IDispatch))	*ppvObject = static_cast<DWebBrowserEvents2*>(this);
    if( *ppvObject != NULL ) CActiveXCtrl::AddRef();
    return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

//---------------------------------------------------------------------------------
//Description:
// DWebBrowserEvents and IDispatch methods
//
//----------------------------------------------------------------------------------
STDMETHODIMP CWebBrowserCtrl::GetTypeInfoCount(UINT FAR* pctinfo)          
{
	TRACE(_T("AX: CWebBrowserCtrl::GetTypeInfoCount\n"));
	return E_NOTIMPL;
}


//---------------------------------------------------------------------------------
//Description:
// DWebBrowserEvents and IDispatch methods
//
//----------------------------------------------------------------------------------
STDMETHODIMP CWebBrowserCtrl::GetTypeInfo(UINT itinfo,LCID lcid,ITypeInfo FAR* FAR* pptinfo)   
{
	TRACE(_T("AX: CWebBrowserCtrl::GetTypeInfo\n"));
	return E_NOTIMPL;
}


//---------------------------------------------------------------------------------
//Description:
// DWebBrowserEvents and IDispatch methods
//
//----------------------------------------------------------------------------------
STDMETHODIMP CWebBrowserCtrl::GetIDsOfNames(REFIID riid,OLECHAR FAR* FAR* rgszNames,UINT cNames,LCID lcid, DISPID FAR* rgdispid)                  
{
	TRACE(_T("AX: CWebBrowserCtrl::GetIDsOfNames\n"));
	return E_NOTIMPL;
}


//---------------------------------------------------------------------------------
//Description:
// DWebBrowserEvents and IDispatch methods
//
//----------------------------------------------------------------------------------
STDMETHODIMP CWebBrowserCtrl::Invoke(DISPID dispidMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,EXCEPINFO FAR* pexcepinfo,UINT FAR* puArgErr)    
{
	TRACE(_T("AX: CWebBrowserCtrl::Invoke\n"));
	// 发送Invoke消息到DUI消息
	if(((CDuiWebBrowserCtrl*)m_pOwner)->IsDuiMsgInvoke())
	{
		return m_pOwner->SendMessage(dispidMember, (WPARAM)pdispparams, (LPARAM)pvarResult);
	}

	switch (dispidMember)
	{
		// The parameter for this DISPID:
		// [0]: Document title - VT_BSTR
		case DISPID_TITLECHANGE:	// web页面标题变更
		{
			if (pdispparams && pdispparams->rgvarg[0].vt == VT_BSTR)
			{         
				//WCHAR szTitle[MAX_URL];
				//int len = wcslen(pdispparams->rgvarg[0].bstrVal);               
				//wcsncpy(szTitle, pdispparams->rgvarg[0].bstrVal, MAX_URL - 5);
				//if(len > MAX_URL - 5)
				//{
				//	wcscat(szTitle, L"...");
				//}

				//OnTitleChange(szTitle);
			}
			return S_OK;
		}

		// The user has told Internet Explorer to close.
		case DISPID_ONQUIT:
		{
			//OnQuit();
			return S_OK;
		}

		// The parameters for this DISPID:
		// [0]: New status bar text - VT_BSTR
		case DISPID_STATUSTEXTCHANGE:	// 状态栏文字变更
		{
			if (pdispparams && pdispparams->rgvarg[0].vt == VT_BSTR)
			{         
				//WCHAR szStatus[MAX_URL];
				//int len = wcslen(pdispparams->rgvarg[0].bstrVal);               
				//wcsncpy(szStatus, pdispparams->rgvarg[0].bstrVal, MAX_URL - 5);
				//if(len > MAX_URL - 5)
				//{
				//	wcscat(szStatus, L"...");
				//}

				//OnStatusTextChange(szStatus);
			}
			return S_OK;
		}

		// The parameters for this DISPID:
		// [0]: Maximum progress - VT_I4				-- 最大进度值
		// [1]: Amount of total progress - VT_I4		-- 当前进度值
		case DISPID_PROGRESSCHANGE:	// 页面加载进度变更
		{
			//OnProgressChange(pdispparams->rgvarg[1].lVal,pdispparams->rgvarg[0].lVal);
			return S_OK;
		}

		// The parameter for this DISPID:
		// [0]: Name of property that changed - VT_BSTR
		case DISPID_PROPERTYCHANGE:	// 属性变更
		{
			if ((pdispparams->cArgs > 0) && (pdispparams->rgvarg[0].vt == VT_BSTR))
			{
#ifdef _UNICODE
				CString strProp = OLE2T(pdispparams->rgvarg[0].bstrVal);
#else
				CString strProp = COLE2T(pdispparams->rgvarg[0].bstrVal);
#endif
			}
			return S_OK;
		}

		case DISPID_FILEDOWNLOAD:	// 文件下载
		{   
			BOOL bCancel = FALSE;
			//OnFileDownload(&bCancel);
			if(bCancel == TRUE)
			{
				*pdispparams->rgvarg[0].pboolVal = VARIANT_TRUE;
			}
			else
			{
				*pdispparams->rgvarg[0].pboolVal = VARIANT_FALSE;
			}
			return S_OK;
		}

		case DISPID_DOWNLOADBEGIN:	// 开始下载
		{
			//OnDownloadBegin();
			return S_OK;
		}

		case DISPID_DOWNLOADCOMPLETE:	// 下载完成
		{
			//OnDownloadComplete();
			return S_OK;
		}

		// The parameters for this DISPID:
		// [0]: Enabled state - VT_BOOL
		// [1]: Command identifier - VT_I4
		case DISPID_COMMANDSTATECHANGE:
		{
			return S_OK;
		}

		// The parameters for this DISPID are as follows:
		// [0]: Cancel flag  - VT_BYREF|VT_BOOL
		// [1]: HTTP headers - VT_BYREF|VT_VARIANT
		// [2]: Address of HTTP POST data  - VT_BYREF|VT_VARIANT
		// [3]: Target frame name - VT_BYREF|VT_VARIANT
		// [4]: Option flags - VT_BYREF|VT_VARIANT
		// [5]: URL to navigate to - VT_BYREF|VT_VARIANT
		// [6]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event
		case DISPID_BEFORENAVIGATE2:	// 导航到指定URL之前
		{
			if ((pdispparams->cArgs >= 5) && (pdispparams->rgvarg[5].vt == (VT_BYREF|VT_VARIANT)))
			{
				CComVariant varURL(*pdispparams->rgvarg[5].pvarVal);
				varURL.ChangeType(VT_BSTR);
#ifdef _UNICODE
				CString strUrl = OLE2T(varURL.bstrVal);
#else
				CString strUrl = COLE2T(varURL.bstrVal);
#endif
				//DuiSystem::DuiMessageBox(NULL, strUrl);
			}
			return S_OK;
		}

		// The parameters for this DISPID:
		// [0]: URL navigated to - VT_BYREF|VT_VARIANT
		// [1]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event.
		case DISPID_NAVIGATECOMPLETE2:	// 导航完成
		{
			if (pdispparams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT))
			{
				CComVariant varURL(*pdispparams->rgvarg[0].pvarVal);
				varURL.ChangeType(VT_BSTR);
#ifdef _UNICODE
				CString strUrl = OLE2T(varURL.bstrVal);
#else
				CString strUrl = COLE2T(varURL.bstrVal);
#endif
			}
			return S_OK;
		}

		case DISPID_DOCUMENTCOMPLETE:	// 页面加载完成
		{
			return S_OK;
		}

		// The parameters for this DISPID are as follows:
		// [0]: Cancel flag - VT_BYREF|VT_BOOL
		// [1]: HTTP headers - VT_BYREF|VT_VARIANT
		// [2]: Address of HTTP POST data - VT_BYREF|VT_VARIANT 
		// [3]: Target frame name - VT_BYREF|VT_VARIANT 
		// [4]: Option flags - VT_BYREF|VT_VARIANT
		// [5]: URL to navigate to - VT_BYREF|VT_VARIANT
		// [6]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event.
		// 实际情况:DISPID_NEWWINDOW2只有两个参数,后面5个参数都没有
		case DISPID_NEWWINDOW2:	// 新建窗口
		{
			return S_OK;
		}

		// The parameters for this DISPID are as follows:
		//0 : bstrUrl         弹出窗口的URL
		//1 : bstrContext     所在网页的URL 
		//2 : dwFlags  
		//3 : *bCancel        是否允许打开该窗口
		//4 : **pDisp
		case DISPID_NEWWINDOW3:	// 新建窗口
		{
			return S_OK;
		}

		default:
			return E_NOTIMPL;
	}

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////////////
// CActiveXWnd

HWND CActiveXWnd::Init(CActiveXCtrl* pOwner, HWND hWndParent)
{
    m_pOwner = pOwner;
    UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    Create(hWndParent, _T("DuiActiveX"), uStyle, 0L, 0,0,0,0, NULL);
    return m_hWnd;
}

LPCTSTR CActiveXWnd::GetWindowClassName() const
{
    return _T("ActiveXWnd");
}

void CActiveXWnd::OnFinalMessage(HWND hWnd)
{
    //delete this; // 这里不需要清理，CDuiActiveX会清理的
}

void CActiveXWnd::DoVerb(LONG iVerb)
{
    if( m_pOwner == NULL )
	{
		return;
	}
    if( m_pOwner->m_pOwner == NULL )
	{
		return;
	}
    IOleObject* pUnk = NULL;
    m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if( pUnk == NULL )
	{
		return;
	}
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    IOleClientSite* pOleClientSite = NULL;
    m_pOwner->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
    pUnk->DoVerb(iVerb, NULL, pOleClientSite, 0, m_hWnd, &m_pOwner->m_pOwner->GetRect());
}

LRESULT CActiveXWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if( m_pOwner->m_pViewObject == NULL )
	{
		bHandled = FALSE;
	}
    return 1;
}

LRESULT CActiveXWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    IOleObject* pUnk = NULL;
    m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if( pUnk == NULL )
	{
		return 0;
	}
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    DWORD dwMiscStatus = 0;
    pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    if( (dwMiscStatus & OLEMISC_NOUIACTIVATE) != 0 )
	{
		return 0;
	}
    if( !m_pOwner->m_bInPlaceActive )
	{
		DoVerb(OLEIVERB_INPLACEACTIVATE);
	}
    bHandled = FALSE;
    return 0;
}

LRESULT CActiveXWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_pOwner->m_bFocused = true;
    if( !m_pOwner->m_bUIActivated ) DoVerb(OLEIVERB_UIACTIVATE);
    return 0;
}

LRESULT CActiveXWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_pOwner->m_bFocused = false;
    return 0;
}

LRESULT CActiveXWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    PAINTSTRUCT ps = { 0 };
    ::BeginPaint(m_hWnd, &ps);
    ::EndPaint(m_hWnd, &ps);
    return 1;
}


/////////////////////////////////////////////////////////////////////////////////////
// CDuiActiveX

CDuiActiveX::CDuiActiveX(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBase(hWnd, pDuiObject)
{
    m_clsid = IID_NULL;
	m_pUnk = NULL;
	m_pCP = NULL;
	m_pControl = NULL;
	m_bCreated = false;
	m_bDelayCreate = false;
	m_bShowContentMenu = true;
	m_bShowScroll = true;
	m_strModuleName = _T("");
	m_strUrl = _T("");
}

CDuiActiveX::~CDuiActiveX()
{
    ReleaseControl();
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

// ActiveX控件初始化
void CDuiActiveX::OnAxInit()
{
}

// ActiveX控件激活
void CDuiActiveX::OnAxActivate(IUnknown *pUnknwn)
{
}

// ActiveX控件创建控件对象
void CDuiActiveX::OnAxCreateCtrl()
{
	m_pControl = new CActiveXCtrl();
}

// ActiveX控件初始化完成
void CDuiActiveX::OnAxInitFinish()
{
}

// 设置控件中的Windows原生控件是否可见的状态
void CDuiActiveX::SetControlWndVisible(BOOL bIsVisible)
{
	if( m_hwndHost != NULL )
	{
		::ShowWindow(m_hwndHost, bIsVisible ? SW_SHOW : SW_HIDE);
	}
}

void CDuiActiveX::SetControlRect(CRect rc) 
{
	m_rc = rc;

	if( !m_bCreated ) DoCreateControl();

    if( m_pUnk == NULL ) return;
    if( m_pControl == NULL ) return;

    SIZEL hmSize = { 0 };
    SIZEL pxSize = { 0 };
    pxSize.cx = m_rc.right - m_rc.left;
    pxSize.cy = m_rc.bottom - m_rc.top;
    PixelToHiMetric(&pxSize, &hmSize);

    if( m_pUnk != NULL )
	{
        m_pUnk->SetExtent(DVASPECT_CONTENT, &hmSize);
    }
    if( m_pControl->m_pInPlaceObject != NULL )
	{
        CRect rcItem = m_rc;
        if( !m_pControl->m_bWindowless ) rcItem.MoveToXY(0, 0);
        m_pControl->m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
    }
    if( !m_pControl->m_bWindowless )
	{
        ASSERT(m_pControl->m_pWindow);
        ::MoveWindow(*m_pControl->m_pWindow, m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top, TRUE);
    }
}

// 从XML设置CLSID属性
HRESULT CDuiActiveX::OnAttributeCLSID(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	CreateControl(strValue);

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置DelayCreate属性
HRESULT CDuiActiveX::OnAttributeDelayCreate(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	SetDelayCreate(strValue == _T("true"));

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置url属性
HRESULT CDuiActiveX::OnAttributeUrl(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_strUrl = strValue;
	if(m_pControl != NULL)
	{
		Navigate(m_strUrl);
	}

	return bLoading?S_FALSE:S_OK;
}

LRESULT CDuiActiveX::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if( m_pControl == NULL )
	{
		return 0;
	}
    ASSERT(m_pControl->m_bWindowless);
    if( !m_pControl->m_bInPlaceActive )
	{
		return 0;
	}
    if( m_pControl->m_pInPlaceObject == NULL )
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
        DWORD dwHitResult = m_pControl->m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if( dwHitResult == HITRESULT_OUTSIDE && m_pControl->m_pViewObject != NULL )
		{
            IViewObjectEx* pViewEx = NULL;
            m_pControl->m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*) &pViewEx);
            if( pViewEx != NULL )
			{
                POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                pViewEx->QueryHitPoint(DVASPECT_CONTENT, &m_rc, ptMouse, 0, &dwHitResult);
                pViewEx->Release();
            }
        }
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
    HRESULT Hr = m_pControl->m_pInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lResult);
    if( Hr == S_OK ) bHandled = bWasHandled;
    return lResult;
}

bool CDuiActiveX::IsDelayCreate() const
{
    return m_bDelayCreate;
}

void CDuiActiveX::SetDelayCreate(bool bDelayCreate)
{
    if( m_bDelayCreate == bDelayCreate ) return;
    if( bDelayCreate == false ) {
        if( m_bCreated == false && m_clsid != IID_NULL ) DoCreateControl();
    }
    m_bDelayCreate = bDelayCreate;
}

bool CDuiActiveX::CreateControl(LPCTSTR pstrCLSID)
{
    CLSID clsid = { 0 };
    OLECHAR szCLSID[100] = { 0 };
#ifndef _UNICODE
    ::MultiByteToWideChar(::GetACP(), 0, pstrCLSID, -1, szCLSID, sizeof(szCLSID) - 1);
#else
    _tcsncpy(szCLSID, pstrCLSID, 99);
#endif
    if( pstrCLSID[0] == '{' )
	{
		::CLSIDFromString(szCLSID, &clsid);
	}else
	{
		::CLSIDFromProgID(szCLSID, &clsid);
	}
    return CreateControl(clsid);
}

bool CDuiActiveX::CreateControl(const CLSID clsid)
{
    ASSERT(clsid!=IID_NULL);
    if( clsid == IID_NULL )
	{
		return false;
	}
    m_bCreated = false;
    m_clsid = clsid;
    if( !m_bDelayCreate )
	{
		DoCreateControl();
	}
    return true;
}

void CDuiActiveX::ReleaseControl()
{
	if(m_pCP)
	{
		m_pCP->Unadvise(m_dwEventCookie);
		m_pCP->Release();
	}
	m_pCP = NULL;

    m_hwndHost = NULL;
    if( m_pUnk != NULL )
	{
        IObjectWithSite* pSite = NULL;
        m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
        if( pSite != NULL )
		{
            pSite->SetSite(NULL);
            pSite->Release();
        }
        m_pUnk->Close(OLECLOSE_NOSAVE);
        m_pUnk->SetClientSite(NULL);
        m_pUnk->Release(); 
        m_pUnk = NULL;
    }
    if( m_pControl != NULL )
	{
        m_pControl->m_pOwner = NULL;
        m_pControl->Release();
        m_pControl = NULL;
    }
    //m_pManager->RemoveMessageFilter(this);
}

typedef HRESULT (__stdcall *DllGetClassObjectFunc)(REFCLSID rclsid, REFIID riid, LPVOID* ppv); 

// 创建ActiveX控件
bool CDuiActiveX::DoCreateControl()
{
    ReleaseControl();
    // At this point we'll create the ActiveX control
    m_bCreated = true;
    IOleControl* pOleControl = NULL;

	// 控件初始化
	OnAxInit();

    HRESULT Hr = -1;
    if( !m_strModuleName.IsEmpty() )
	{
        HMODULE hModule = ::LoadLibrary((LPCTSTR)m_strModuleName);
        if( hModule != NULL )
		{
            IClassFactory* aClassFactory = NULL;
            DllGetClassObjectFunc aDllGetClassObjectFunc = (DllGetClassObjectFunc)::GetProcAddress(hModule, "DllGetClassObject");
            Hr = aDllGetClassObjectFunc(m_clsid, IID_IClassFactory, (LPVOID*)&aClassFactory);
            if( SUCCEEDED(Hr) )
			{
                Hr = aClassFactory->CreateInstance(NULL, IID_IOleObject, (LPVOID*)&pOleControl);
            }
            aClassFactory->Release();
        }
    }
    if( FAILED(Hr) )
	{
        Hr = ::CoCreateInstance(m_clsid, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*)&pOleControl);
		if( FAILED(Hr) )
		{
			LPOLESTR lpwClsid = NULL;
			Hr = StringFromCLSID(m_clsid, &lpwClsid);
			if (SUCCEEDED(Hr))
			{
				USES_CONVERSION;
				LPCTSTR lpszClsid = OLE2T(lpwClsid);
				DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("CoCreateInstance %s failed"), lpszClsid);
				IMalloc * pMalloc = NULL;
				Hr = ::CoGetMalloc(1, &pMalloc);	// 取得 IMalloc
				if (SUCCEEDED(Hr))
				{
					pMalloc->Free(lpwClsid);		// 释放ProgID内存
					pMalloc->Release();				// 释放IMalloc
				}
			}
		}
		// 控件激活
		OnAxActivate(pOleControl);
    }
    //ASSERT(SUCCEEDED(Hr));
    if( FAILED(Hr) ) return false;
    pOleControl->QueryInterface(IID_IOleObject, (LPVOID*) &m_pUnk);
    pOleControl->Release();
    if( m_pUnk == NULL ) return false;

    // Create the host too
	OnAxCreateCtrl();
	if(m_pControl != NULL)
	{
		m_pControl->m_pOwner = this;
	}

    // More control creation stuff
    DWORD dwMiscStatus = 0;
    m_pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    IOleClientSite* pOleClientSite = NULL;
    m_pControl->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;

    // Initialize control,设置用户site
    if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) != 0 ) m_pUnk->SetClientSite(pOleClientSite);
    IPersistStreamInit* pPersistStreamInit = NULL;
    m_pUnk->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersistStreamInit);
    if( pPersistStreamInit != NULL )
	{
        Hr = pPersistStreamInit->InitNew();
        pPersistStreamInit->Release();
    }
    if( FAILED(Hr) ) return false;
    if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) == 0 ) m_pUnk->SetClientSite(pOleClientSite);

    // Grab the view...
    Hr = m_pUnk->QueryInterface(IID_IViewObjectEx, (LPVOID*) &m_pControl->m_pViewObject);
    if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject2, (LPVOID*) &m_pControl->m_pViewObject);
    if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject, (LPVOID*) &m_pControl->m_pViewObject);

    // Activate and done...,激活浏览器控件
    m_pUnk->SetHostNames(OLESTR("UIActiveX"), NULL);
    //if( m_pManager != NULL ) m_pManager->SendNotify((CControlUI*)this, _T("showactivex"), 0, 0, false);
    if( (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0 )
	{
        Hr = m_pUnk->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pOleClientSite, 0, GetPaintHWnd(), &m_rc);
        //::RedrawWindow(GetPaintHWnd(), &m_rcItem, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
    }
    IObjectWithSite* pSite = NULL;
    m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
    if( pSite != NULL )
	{
        pSite->SetSite(static_cast<IOleClientSite*>(m_pControl));
        pSite->Release();
    }

	// 控件初始化完成
	OnAxInitFinish();

	// 设置初始的URL,导航到URL
	if(!m_strUrl.IsEmpty())
	{
		Navigate(m_strUrl);
	}

    return SUCCEEDED(Hr);
}

// 导航到指定的URL
HRESULT CDuiActiveX::Navigate(CString strUrl)
{
	return S_FALSE;
}

// 根据IID获取ActiveX控件
HRESULT CDuiActiveX::GetControl(const IID iid, LPVOID* ppRet)
{
    ASSERT(ppRet!=NULL);
    ASSERT(*ppRet==NULL);
    if( ppRet == NULL )
	{
		return E_POINTER;
	}
    if( m_pUnk == NULL )
	{
		return E_PENDING;
	}
    return m_pUnk->QueryInterface(iid, (LPVOID*) ppRet);
}

CLSID CDuiActiveX::GetClisd() const
{
	return m_clsid;
}

CString CDuiActiveX::GetModuleName() const
{
    return m_strModuleName;
}

void CDuiActiveX::SetModuleName(LPCTSTR pstrText)
{
    m_strModuleName = pstrText;
}

// 控件画图函数
void CDuiActiveX::DrawControl(CDC &dc, CRect rcUpdate)
{
	if( !::IntersectRect(&rcUpdate, &rcUpdate, &m_rc) )
	{
		return;
	}

	if( m_pControl != NULL && m_pControl->m_bWindowless && m_pControl->m_pViewObject != NULL )
    {
		// 无窗口模式下调用ActiveX控件的View接口画图
        m_pControl->m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, dc.m_hDC, (RECTL*) &m_rc, (RECTL*) &m_rc, NULL, NULL); 
    }
}

// 文件路径解析
CString CDuiActiveX::ParseFilePath(CString strUrl)
{
	// 通过Skin读取
	CString strSkin = _T("");
	if(strUrl.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strUrl);
		if (strSkin.IsEmpty()) return FALSE;
	}else
	{
		strSkin = strUrl;
	}

	if(strSkin.Find(_T(".")) != -1)	// 加载文件
	{
		CString strFile = DuiSystem::GetSkinPath() + strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strFile = strSkin;
		}
		return strFile;
	}

	CString strFile = strSkin;
	return strFile;
}

/////////////////////////////////////////////////////////////////////////////////////
// CDuiWebBrowserCtrl

CDuiWebBrowserCtrl::CDuiWebBrowserCtrl(HWND hWnd, CDuiObject* pDuiObject)
	: CDuiActiveX(hWnd, pDuiObject)
{
	m_bDuiMsgInvoke = FALSE;
}

CDuiWebBrowserCtrl::~CDuiWebBrowserCtrl()
{
}

// ActiveX控件初始化
void CDuiWebBrowserCtrl::OnAxInit()
{
	::CLSIDFromString(L"{8856F961-340A-11D0-A96B-00C04FD705A2}", &m_clsid);	// IE浏览器控件ID
}

// ActiveX控件激活
void CDuiWebBrowserCtrl::OnAxActivate(IUnknown *pUnknwn)
{
}

// 设置控件的DUI消息发送标识
BOOL CDuiWebBrowserCtrl::OnControlSetDuiMsg(LPCTSTR lpszDuiMsg)
{
	if(__super::OnControlSetDuiMsg(lpszDuiMsg))
	{
		return TRUE;
	}

	CString strDuiMsg = lpszDuiMsg;
	if(strDuiMsg == _T("invoke"))	// 发送Invoke的DUI消息
	{
		m_bDuiMsgInvoke = TRUE;
		return TRUE;
	}

	return FALSE;
}

// ActiveX控件创建控件对象
void CDuiWebBrowserCtrl::OnAxCreateCtrl()
{
	m_pControl = new CWebBrowserCtrl();   
}

// ActiveX控件初始化完成
void CDuiWebBrowserCtrl::OnAxInitFinish()
{
	// 初始化浏览器事件处理
	InitEvents();
}

// 获取IWebBrowser2接口指针
IWebBrowser2* CDuiWebBrowserCtrl::GetIWebBrowser2()
{
	IWebBrowser2* pWebBrowser = NULL;
	GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	return pWebBrowser;
}

// 获取当前的URL
CString CDuiWebBrowserCtrl::getURL()
{
	IWebBrowser2* pIWebBrowser = GetIWebBrowser2();
	if(pIWebBrowser != NULL)
	{
		BSTR bstrURL;
		HRESULT hr = pIWebBrowser->get_LocationURL(&bstrURL);
		if(SUCCEEDED(hr))
		{
#ifdef _UNICODE
			return bstrURL;
#else
			_bstr_t tout(bstrURL, FALSE);
			CString out = tout;
			return out;
#endif
		}
	}
	return _T("");
}

// 导航到指定的URL
HRESULT CDuiWebBrowserCtrl::Navigate(CString strUrl)
{
	HRESULT hr = -1;
	IWebBrowser2* pWebBrowser = NULL;
	GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	if( pWebBrowser != NULL )
	{
		if(strUrl.Find(_T("file://")) == 0)
		{
			strUrl.Delete(0, 7);
			if(strUrl.Find(_T(":")) == -1)
			{
				strUrl = DuiSystem::GetSkinPath() + strUrl;
			}
		}
		BSTR bsUrl = strUrl.AllocSysString();
		hr = pWebBrowser->Navigate(bsUrl,NULL,NULL,NULL,NULL);
		pWebBrowser->Release();
		::SysFreeString(bsUrl);
	}

	return hr;
}

// 导航到上一个URL
HRESULT CDuiWebBrowserCtrl::GoBack()
{
	HRESULT hr = -1;
	IWebBrowser2* pWebBrowser = NULL;
	GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	if( pWebBrowser != NULL )
	{
		hr = pWebBrowser->GoBack();
		pWebBrowser->Release();
	}

	return hr;
}

// 导航到下一个URL
HRESULT CDuiWebBrowserCtrl::GoForward()
{
	HRESULT hr = -1;
	IWebBrowser2* pWebBrowser = NULL;
	GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	if( pWebBrowser != NULL )
	{
		hr = pWebBrowser->GoForward();
		pWebBrowser->Release();
	}

	return hr;
}

// 刷新页面
HRESULT CDuiWebBrowserCtrl::Refresh()
{
	HRESULT hr = -1;
	IWebBrowser2* pWebBrowser = NULL;
	GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	if( pWebBrowser != NULL )
	{
		hr = pWebBrowser->Refresh();
		pWebBrowser->Release();
	}

	return hr;
}

// 停止加载页面
HRESULT CDuiWebBrowserCtrl::Stop()
{
	HRESULT hr = -1;
	IWebBrowser2* pWebBrowser = NULL;
	GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	if( pWebBrowser != NULL )
	{
		hr = pWebBrowser->Stop();
		pWebBrowser->Release();
	}

	return hr;
}

// 获取页面是否正在加载
HRESULT CDuiWebBrowserCtrl::GetBusy(BOOL& bBusy)
{
	HRESULT hr = -1;
	IWebBrowser2* pWebBrowser = NULL;
	GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	VARIANT_BOOL pBusy = VARIANT_FALSE;
	if( pWebBrowser != NULL )
	{
		hr = pWebBrowser->get_Busy(&pBusy);
		pWebBrowser->Release();
	}

	bBusy = (pBusy == VARIANT_TRUE);

	return hr;
}

// 初始化浏览器控件的事件处理
HRESULT CDuiWebBrowserCtrl::InitEvents()
{
	HRESULT                     hr;
	IConnectionPointContainer  *pCPCont = NULL;
	DWebBrowserEvents          *pEvents = NULL;

	IWebBrowser2* pWebBrowser = NULL;
	GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
	if(!pWebBrowser)
	{
		return S_FALSE;
	}

	hr = pWebBrowser->QueryInterface(IID_IConnectionPointContainer, (LPVOID *)&pCPCont);
	if(FAILED(hr))
	{
		return S_FALSE;
	}

	hr = pCPCont->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pCP);
	if(FAILED(hr))
	{
		m_pCP = NULL;
		goto Cleanup;
	}

	hr = m_pControl->QueryInterface(DIID_DWebBrowserEvents2, (LPVOID *)(&pEvents));
	if(FAILED(hr))
		goto Cleanup;
	hr = m_pCP->Advise(pEvents, &(m_dwEventCookie));
	if(FAILED(hr))
		goto Cleanup;

Cleanup:
	if(pCPCont)
		pCPCont->Release();
	if(pEvents)
		pEvents->Release();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////////////
// CDuiFlashCtrl
// 重载new操作符，判定如果flash没有安装，返回null
void* CDuiFlashCtrl::operator new(size_t sz)
{
	static bool bIsExistFlashActiveX = false;
	static bool bCheckedFlashActivex = false;
	if (!bCheckedFlashActivex)
	{
		bIsExistFlashActiveX = isExistFlashActiveX();
		bCheckedFlashActivex = true;
	}

	if (!bIsExistFlashActiveX)
		return NULL;
	return ::operator new(sz);
}

CDuiFlashCtrl::CDuiFlashCtrl(HWND hWnd, CDuiObject* pDuiObject)
	: CDuiActiveX(hWnd, pDuiObject)
{
	m_bTransparent = false;
	m_strVars = _T("");
}

CDuiFlashCtrl::~CDuiFlashCtrl()
{
}

// ActiveX控件初始化
void CDuiFlashCtrl::OnAxInit()
{
	m_clsid=__uuidof(ShockwaveFlashObjects::ShockwaveFlash);
}

// ActiveX控件激活
void CDuiFlashCtrl::OnAxActivate(IUnknown *pUnknwn)
{
	// 保存flash控件接口
	flash_ = pUnknwn;
	
	if(m_bTransparent)
	{
		// 创建透明无窗体的Flash控件,使用DUI控件的窗口句柄画图
		flash_->put_WMode(bstr_t(_T("transparent")));
	}

	flash_->DisableLocalSecurity();
	flash_->put_AllowScriptAccess(bstr_t(_T("always"))); 
}

// ActiveX控件初始化完成
void CDuiFlashCtrl::OnAxInitFinish()
{
}

// 导航到指定的URL
HRESULT CDuiFlashCtrl::Navigate(CString strUrl)
{
	HRESULT hr = S_OK;
	if(flash_)
	{
		if(!strUrl.IsEmpty())
		{
			m_strUrl = strUrl;
			flash_->put_Movie(bstr_t(ParseFilePath(m_strUrl)));
			if(!m_strVars.IsEmpty())
			{
				flash_->PutFlashVars(bstr_t(m_strVars));
			}
			flash_->Play();
		}
	}

	return hr;
}

// 设置Flash参数
HRESULT CDuiFlashCtrl::PutFlashVars(CString strVars)
{
	HRESULT hr = S_OK;
	if(flash_)
	{
		if(!strVars.IsEmpty())
		{
			flash_->PutFlashVars(bstr_t(strVars));
		}
	}

	return hr;
}

// 检测是否安装flash
bool CDuiFlashCtrl::isExistFlashActiveX()
{
	HKEY hKey = NULL;

	// 如果注册表中没有FlashPlayerX，则返回false
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Macromedia\\FlashPlayerActiveX"),0,KEY_READ,&hKey)!=ERROR_SUCCESS)
		return false;

	std::wstring strValueName;
	std::wstring strDataBuffer;
	strValueName.resize(1024);
	strDataBuffer.resize(1024);
	DWORD nValueNameBufferLength=1024, nValueType, nDataBudderSize=1024;

	int i=0;  
	while(RegEnumValueW(hKey,i++, (LPWSTR)strValueName.c_str(), &nValueNameBufferLength, NULL, &nValueType, (BYTE*)strDataBuffer.c_str(), &nDataBudderSize) != ERROR_NO_MORE_ITEMS)  
	{  
		std::wstring strName(strValueName.c_str());
		if (strName.compare(L"PlayerPath") == 0)
		{

			if( (_waccess(std::wstring(strDataBuffer.c_str()).c_str(), 0 )) == -1 )
				return false;
		}

		nDataBudderSize=1024;  
		nValueNameBufferLength=1024;  
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// CDuiMediaPlayer

CDuiMediaPlayer::CDuiMediaPlayer(HWND hWnd, CDuiObject* pDuiObject)
	: CDuiActiveX(hWnd, pDuiObject)
{
}

CDuiMediaPlayer::~CDuiMediaPlayer()
{
}

// ActiveX控件初始化
void CDuiMediaPlayer::OnAxInit()
{
	m_clsid=__uuidof(WMPLib::WindowsMediaPlayer);
}

// ActiveX控件激活
void CDuiMediaPlayer::OnAxActivate(IUnknown *pUnknwn)
{
	// 保存mediaplayer控件接口
	wmp_ = pUnknwn;
	if(wmp_)
	{
		// 设置为无窗口模式,使用DUI控件的窗口句柄画图
		//wmp_->put_windowlessVideo(VARIANT_TRUE);
		// 伸缩画面，使其适合播放窗口
		//wmp_->put_stretchToFit(TRUE);
		// 允许右键菜单
		//wmp_->PutenableContextMenu(TRUE);
	}
}

// ActiveX控件初始化完成
void CDuiMediaPlayer::OnAxInitFinish()
{
}

// 设置控件中的Windows原生控件是否可见的状态
void CDuiMediaPlayer::SetControlWndVisible(BOOL bIsVisible)
{
	if(( m_hwndHost != NULL ) && (wmp_ != NULL))
	{
		//::ShowWindow(m_hwndHost, bIsVisible ? SW_SHOW : SW_HIDE);
		//CString str = wmp_->GetuiMode();
		//TRACE("CDuiMediaPlayer::SetControlWndVisible %s\n", str);
		if(bIsVisible)
		{
			::ShowWindow(m_hwndHost, SW_SHOW);
			wmp_->PutuiMode("full");
		}else
		{
			wmp_->PutuiMode("Invisible");
		}
	}
}

// 导航到指定的URL
HRESULT CDuiMediaPlayer::Navigate(CString strUrl)
{
	HRESULT hr = S_OK;
	if(wmp_)
	{
		wmp_->close();
		if(!strUrl.IsEmpty())
		{
			m_strUrl = strUrl;
			wmp_->put_URL(bstr_t(ParseFilePath(m_strUrl)));
			//wmp_->openPlayer(bstr_t(ParseFilePath(m_strUrl)));	// 打开单独的播放窗口
		}
	}

	return hr;
}