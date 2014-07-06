#ifndef __UIACTIVEX_H__
#define __UIACTIVEX_H__

#pragma once

#include "WndBase.h"

struct IOleObject;

/////////////////////////////////////////////////////////////////////////////////////
//

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
//

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

	HRESULT Navigate(CString strUrl);

	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlRect(CRect rc);
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
    CLSID m_clsid;
    CString m_strModuleName;
	CString m_strUrl;
    bool m_bCreated;
    bool m_bDelayCreate;
    IOleObject* m_pUnk;
    CActiveXCtrl* m_pControl;
    HWND m_hwndHost;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("clsid", OnAttributeCLSID)
		DUI_TSTRING_ATTRIBUTE("modulename", m_strModuleName, FALSE)
		DUI_CUSTOM_ATTRIBUTE("url", OnAttributeUrl)
		DUI_CUSTOM_ATTRIBUTE("delaycreate", OnAttributeDelayCreate)
	DUI_DECLARE_ATTRIBUTES_END()
};


#endif // __UIACTIVEX_H__
