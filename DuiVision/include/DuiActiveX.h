#ifndef __DUIACTIVEX_H__
#define __DUIACTIVEX_H__

#pragma once

#include "WndBase.h"
#include "../activex/duicomcli.h"
#include "../activex/flash10t.tlh"
#include "../activex/wmp.tlh"

struct IOleObject;

/////////////////////////////////////////////////////////////////////////////////////
// CSafeRelease

class CActiveXCtrl;

template< class T >
class CSafeRelease
{
public:
    CSafeRelease(T* p) : m_p(p) { };
    ~CSafeRelease() { if( m_p != NULL ) m_p->Release(); };
    T* Detach() { T* t = m_p; m_p = NULL; return t; };
    T* m_p;
};

/////////////////////////////////////////////////////////////////////////////////////
// CDuiActiveX

class CDuiActiveX : public CControlBase
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiActiveX, "activex")

    friend CActiveXCtrl;
public:
    CDuiActiveX(HWND hWnd, CDuiObject* pDuiObject);
    virtual ~CDuiActiveX();

    HWND GetHostWindow() const;
	HWND GetPaintWindow();

    bool IsDelayCreate() const;
    void SetDelayCreate(bool bDelayCreate = true);

    bool CreateControl(const CLSID clsid);
    bool CreateControl(LPCTSTR pstrCLSID);
    HRESULT GetControl(const IID iid, LPVOID* ppRet);
	CLSID GetClisd() const;
    CString GetModuleName() const;
    void SetModuleName(LPCTSTR pstrText);
	CString ParseFilePath(CString strUrl);

	bool GetShowContentMenu() { return m_bShowContentMenu; }

	virtual void OnAxInit();
	virtual void OnAxActivate(IUnknown *pUnknwn);
	virtual void OnAxCreateCtrl();
	virtual void OnAxInitFinish();
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlRect(CRect rc);
	virtual HRESULT Navigate(CString strUrl);
    void DoPaint(HDC hDC, const RECT& rcPaint);

	HRESULT OnAttributeCLSID(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeDelayCreate(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeUrl(const CStringA& strValue, BOOL bLoading);

    LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
    void ReleaseControl();
    bool DoCreateControl();

	virtual void DrawControl(CDC &dc, CRect rcUpdate);

protected:
    CLSID					m_clsid;
    CString					m_strModuleName;
	CString					m_strUrl;
    bool					m_bCreated;
    bool					m_bDelayCreate;
	bool					m_bShowContentMenu;	// 是否显示右键菜单
    IOleObject*				m_pUnk;
	IConnectionPoint*		m_pCP;		// 连接点指针
	DWORD                   m_dwEventCookie;
    CActiveXCtrl*			m_pControl;	// ActiveX控件指针
    HWND					m_hwndHost;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("clsid", OnAttributeCLSID)
		DUI_TSTRING_ATTRIBUTE("modulename", m_strModuleName, FALSE)
		DUI_CUSTOM_ATTRIBUTE("url", OnAttributeUrl)
		DUI_CUSTOM_ATTRIBUTE("delaycreate", OnAttributeDelayCreate)
		DUI_INT_ATTRIBUTE("show-contextmenu", m_bShowContentMenu, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};

/////////////////////////////////////////////////////////////////////////////////////
// CDuiWebBrowserCtrl

class CDuiWebBrowserCtrl : public CDuiActiveX
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiWebBrowserCtrl, "webbrowser")

public:
    CDuiWebBrowserCtrl(HWND hWnd, CDuiObject* pDuiObject);
    virtual ~CDuiWebBrowserCtrl();

	virtual void OnAxInit();
	virtual void OnAxActivate(IUnknown *pUnknwn);
	virtual void OnAxCreateCtrl();
	virtual void OnAxInitFinish();
	virtual HRESULT Navigate(CString strUrl);

protected:
	HRESULT InitEvents();

protected:
	DUI_DECLARE_ATTRIBUTES_BEGIN()
	DUI_DECLARE_ATTRIBUTES_END()
};

/////////////////////////////////////////////////////////////////////////////////////
// CDuiFlashCtrl

class CDuiFlashCtrl : public CDuiActiveX
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiFlashCtrl, "flash")

public:
    CDuiFlashCtrl(HWND hWnd, CDuiObject* pDuiObject);
    virtual ~CDuiFlashCtrl();

	virtual void OnAxInit();
	virtual void OnAxActivate(IUnknown *pUnknwn);
	virtual void OnAxInitFinish();
	virtual HRESULT Navigate(CString strUrl);

protected:
    CDuiComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> flash_;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
	DUI_DECLARE_ATTRIBUTES_END()
};


/////////////////////////////////////////////////////////////////////////////////////
// CDuiMediaPlayer

class CDuiMediaPlayer : public CDuiActiveX
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiMediaPlayer, "mediaplayer")

public:
    CDuiMediaPlayer(HWND hWnd, CDuiObject* pDuiObject);
    virtual ~CDuiMediaPlayer();

	virtual void OnAxInit();
	virtual void OnAxActivate(IUnknown *pUnknwn);
	virtual void OnAxInitFinish();
	virtual HRESULT Navigate(CString strUrl);

protected:
    CDuiComQIPtr<WMPLib::IWMPPlayer4> wmp_;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
	DUI_DECLARE_ATTRIBUTES_END()
};

#endif // __DUIACTIVEX_H__
