#ifndef __DUIWKEVIEW_H__
#define __DUIWKEVIEW_H__

#pragma once

#include "../third-part/wke/wke.h"

// GDI��ʽ����Ⱦ��
class CRenderGDI
{
public:
    CRenderGDI()
        :m_hView(NULL)
        ,m_hDC(NULL)
        ,m_hBitmap(NULL)
        ,m_pixels(NULL)
        ,m_width(0)
        ,m_height(0)
    {}

    ~CRenderGDI()
    {
        if (m_hDC)
            DeleteDC(m_hDC);

        if (m_hBitmap)
            DeleteObject(m_hBitmap);
    }

    virtual bool init(HWND hView)
    {
        m_hView = hView;
        m_hDC = CreateCompatibleDC(0);

        RECT rect;
        GetClientRect(hView, &rect);
        resize(rect.right, rect.bottom);
        return true;
    }

    virtual void destroy()
    {
        delete this;
    }

    virtual void resize(unsigned int w, unsigned int h)
    {
        if (m_width == w && m_height == h)
            return;

        m_width = w;
        m_height = h;
        m_pixels = NULL;
    }

    virtual void render(wkeWebView webView)
    {
        if (webView->isDirty())
        {
            if (m_pixels == NULL)
                createBitmap();

            webView->paint(m_pixels, 0);

            HDC hDC = GetDC(m_hView);
            BitBlt(hDC, 0, 0, m_width, m_height, m_hDC, 0, 0, SRCCOPY);
            ReleaseDC(m_hView, hDC);
        }
    }

    void createBitmap()
    {
        BITMAPINFO bi;
        memset(&bi, 0, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth         = int(m_width);
        bi.bmiHeader.biHeight        = -int(m_height);
        bi.bmiHeader.biPlanes        = 1;
        bi.bmiHeader.biBitCount      = 32;
        bi.bmiHeader.biCompression   = BI_RGB;

        HBITMAP hbmp = ::CreateDIBSection(0, &bi, DIB_RGB_COLORS, &m_pixels, NULL, 0);

        SelectObject(m_hDC, hbmp);

        if (m_hBitmap)
            DeleteObject(m_hBitmap);

        m_hBitmap = hbmp;
    }

private:
    HWND m_hView;
    HBITMAP m_hBitmap;
    HDC m_hDC;
    unsigned int m_width;
    unsigned int m_height;
    void* m_pixels;
};

/////////////////////////////////////////////////////////////////////////////////////
// CDuiWkeView - wke�ؼ���

#define WKE_EVENT_URLCHANGED	100	// URL����¼�
#define WKE_EVENT_TITLECHANGED	101	// ��ҳ�������¼�

class CDuiWkeView : public CControlBase ,protected wkeBufHandler
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiWkeView, _T("wkeview"))

public:
    CDuiWkeView(HWND hWnd, CDuiObject* pDuiObject);
    virtual ~CDuiWkeView();

	static void WkeInit();
	static void WkeShutdown();

	static CDuiWkeView* GetWkeViewByClientHandler(const wkeClientHandler* pWkeClientHandler);
	wkeWebView GetWkeWebView();
	wkeClientHandler* GetWkeClientHandler();

    bool IsDelayCreate() const;
    void SetDelayCreate(bool bDelayCreate = true);

	bool RegisterWindowClass();
    bool CreateControl();

	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlRect(CRect rc);

	void SetCurrentFocusControl(BOOL bFocus);

	HRESULT OnAttributeDelayCreate(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeUrl(const CString& strValue, BOOL bLoading);

	CString getTitle();
	CString getURL();
	void setURL(CString strUrl);

	virtual void Navigate(CString strUrl);
	virtual void loadHTML(CString strHtml);
	virtual void loadFile(CString strFile);
	virtual void stopLoading();

	virtual bool canGoBack();
	virtual bool goBack();
	virtual bool canGoForward();
	virtual bool goForward();

protected:
    void ReleaseControl();

	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	virtual	BOOL OnControlTimer();
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	LRESULT OnWmKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void onBufUpdated (const HDC hdc,int x, int y, int cx, int cy);

	static LRESULT CALLBACK __WebViewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT WebViewWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static void onURLChanged(const struct _wkeClientHandler* clientHandler, const wkeString URL);
	static void onTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString title);

protected:
	WNDPROC					m_OldWndProc;	// ����ɵĴ����¼�������
    bool					m_bCreated;		// �Ƿ��Ѵ���
	bool					m_bCreating;	// �Ƿ����ڴ���
    bool					m_bDelayCreate;	// �Ƿ��ӳٴ����ؼ�
	bool					m_bTransparent;	// �Ƿ񱳾�͸��
	wkeWebView				m_pWebView;		// wke��ͼ����ָ��
	wkeClientHandler		m_wkeHander;	// wke��ҳ����ı��URL�ı�Ļص�
	CRenderGDI				m_render;		// wke��Ⱦ����
	CString					m_strUrl;		// URL
	CString					m_strHtml;		// Html����

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("delaycreate"), OnAttributeDelayCreate)
		DUI_CUSTOM_ATTRIBUTE(_T("url"), OnAttributeUrl)
		DUI_BOOL_ATTRIBUTE(_T("transparent"), m_bTransparent, TRUE)
	DUI_DECLARE_ATTRIBUTES_END()
};

#endif // __DUIWKEVIEW_H__
