#ifndef __DUIACTIVEX_H__
#define __DUIACTIVEX_H__

#pragma once

#include "WndBase.h"
#include "../activex/duicomcli.h"
#include "../activex/flash10t.tlh"
#include "../activex/wmp.tlh"
#include <exdisp.h>

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
	DUIOBJ_DECLARE_CLASS_NAME(CDuiActiveX, _T("activex"))

    friend CActiveXCtrl;
public:
    CDuiActiveX(HWND hWnd, CDuiObject* pDuiObject);
    virtual ~CDuiActiveX();

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
	bool GetShowScroll() { return m_bShowScroll; }

	virtual void OnAxInit();
	virtual void OnAxActivate(IUnknown *pUnknwn);
	virtual void OnAxCreateCtrl();
	virtual void OnAxInitFinish();
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlRect(CRect rc);
	virtual HRESULT Navigate(CString strUrl);

	HRESULT OnAttributeCLSID(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeDelayCreate(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeUrl(const CString& strValue, BOOL bLoading);

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
    bool					m_bDelayCreate;		// 是否延迟创建
	bool					m_bShowContentMenu;	// 是否显示右键菜单
	bool					m_bShowScroll;		// 是否显示滚动条
    IOleObject*				m_pUnk;
	IConnectionPoint*		m_pCP;				// 连接点指针
	DWORD                   m_dwEventCookie;
    CActiveXCtrl*			m_pControl;			// ActiveX控件指针

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("clsid"), OnAttributeCLSID)
		DUI_TSTRING_ATTRIBUTE(_T("modulename"), m_strModuleName, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("url"), OnAttributeUrl)
		DUI_CUSTOM_ATTRIBUTE(_T("delaycreate"), OnAttributeDelayCreate)
		DUI_BOOL_ATTRIBUTE(_T("show-contextmenu"), m_bShowContentMenu, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("show-scroll"), m_bShowScroll, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};

/////////////////////////////////////////////////////////////////////////////////////
// CDuiWebBrowserCtrl

class CDuiWebBrowserCtrl : public CDuiActiveX
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiWebBrowserCtrl, _T("webbrowser"))

public:
    CDuiWebBrowserCtrl(HWND hWnd, CDuiObject* pDuiObject);
    virtual ~CDuiWebBrowserCtrl();

	BOOL IsDuiMsgInvoke() {return m_bDuiMsgInvoke; }
	virtual BOOL OnControlSetDuiMsg(LPCTSTR lpszDuiMsg);

	virtual void OnAxInit();
	virtual void OnAxActivate(IUnknown *pUnknwn);
	virtual void OnAxCreateCtrl();
	virtual void OnAxInitFinish();

	IWebBrowser2* GetIWebBrowser2();
	CString getURL();

	virtual HRESULT Navigate(CString strUrl);
	HRESULT GoBack();
	HRESULT GoForward();
	HRESULT Refresh();
	HRESULT Stop();
	HRESULT GetBusy(BOOL& bBusy);

protected:
	HRESULT InitEvents();

	BOOL					m_bDuiMsgInvoke;	// 是否发送浏览器Invoke消息到DUI消息

protected:
	DUI_DECLARE_ATTRIBUTES_BEGIN()
	DUI_DECLARE_ATTRIBUTES_END()
};

/////////////////////////////////////////////////////////////////////////////////////
// CDuiFlashCtrl

class CDuiFlashCtrl : public CDuiActiveX
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiFlashCtrl, _T("flash"))

public:
	// 重载new操作符，判定如果flash没有安装，返回null
	void * operator new(size_t sz);

    CDuiFlashCtrl(HWND hWnd, CDuiObject* pDuiObject);
    virtual ~CDuiFlashCtrl();

	virtual void OnAxInit();
	virtual void OnAxActivate(IUnknown *pUnknwn);
	virtual void OnAxInitFinish();

	virtual HRESULT Navigate(CString strUrl);
	HRESULT PutFlashVars(CString strVars);

	HRESULT OnAttributeVars(const CString& strValue, BOOL bLoading);

	// 检测是否安装flash
	static bool isExistFlashActiveX();
protected:
    CDuiComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> flash_;
	bool		m_bTransparent;	// 是否创建背景透明的flash
	CString		m_strVars;			// flash参数

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_BOOL_ATTRIBUTE(_T("transparent"), m_bTransparent, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("vars"), m_strVars, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};


/////////////////////////////////////////////////////////////////////////////////////
// CDuiMediaPlayer

class CDuiMediaPlayer : public CDuiActiveX
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiMediaPlayer, _T("mediaplayer"))

public:
    CDuiMediaPlayer(HWND hWnd, CDuiObject* pDuiObject);
    virtual ~CDuiMediaPlayer();

	virtual void OnAxInit();
	virtual void OnAxActivate(IUnknown *pUnknwn);
	virtual void OnAxInitFinish();
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual HRESULT Navigate(CString strUrl);

protected:
    CDuiComQIPtr<WMPLib::IWMPPlayer4> wmp_;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
	DUI_DECLARE_ATTRIBUTES_END()
};

#endif // __DUIACTIVEX_H__
