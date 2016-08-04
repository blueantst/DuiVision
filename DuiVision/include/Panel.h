// Panel�ؼ����˿ؼ���һ���ؼ�����
#pragma once

#include "ControlBase.h"
#include "vcicomm.h"
#include "IDuiHostWnd.h"

using namespace  std;

interface IDuiPluginPanel;
class CDuiPanel : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiPanel, _T("div"))

public:
	CDuiPanel(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiPanel(void);

	void	SetXmlFile(CString strXmlFile) {m_strXmlFile = strXmlFile;}

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadXmlFile(CString strFileName);
	HRESULT OnAttributeXml(const CString& strValue, BOOL bLoading);

	HRESULT OnAttributePlugin(const CString& strValue, BOOL bLoading);

	void SetVirtualHeight(int nHeight) { m_nVirtualHeight = nHeight; }
	void SetVirtualWidth(int nWidth) { m_nVirtualWidth = nWidth; }

	void SetEnableScroll(BOOL bEnableScroll) { m_bEnableScroll = bEnableScroll; }
	BOOL GetEnableScroll() { return m_bEnableScroll; }

	virtual void SetControlVisible(BOOL bIsVisible);
	virtual void SetControlHide(BOOL bIsHide);
	virtual void SetControlDisable(BOOL bIsDisable);
	virtual BOOL SetControlFocus(BOOL bFocus);

	// ��Ϣ��Ӧ
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// ���ݿؼ��������ؼ�ʵ��
	CControlBase* _CreateControlByName(LPCTSTR lpszName);
	void SetIDuiHostWnd(IDuiHostWnd* pIDuiHostWnd) { m_pIDuiHostWnd = pIDuiHostWnd; }
	IDuiHostWnd* GetIDuiHostWnd() { return m_pIDuiHostWnd; }

	HRESULT OnAttributeImageScrollV(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeImageScrollH(const CString& strValue, BOOL bLoading);

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	virtual BOOL DrawSubControls(CDC &dc, CRect rcUpdate);
	virtual void SetUpdate(BOOL bUpdate, COLORREF clr = 0);

	virtual BOOL OnMousePointChange(CPoint& point);
	virtual BOOL OnCheckMouseResponse(UINT nFlags, CPoint point);
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL OnControlTimer();

	virtual void InitUI(CRect rcClient, DuiXmlNode pNode);

protected:
	// �����Ĳ���������ڹ��ܽӿ�
    BEGIN_INTERFACE_PART(DuiPanel, IDuiHostWnd)
		// ƽ̨����
		STDMETHOD_( CString , GetAppName) ();				// ��ȡӦ�ó�����
		STDMETHOD_( CString , GetPlatPath) ();				// ��ȡƽ̨·��
		STDMETHOD_( CString , GetPlatVersion) ();			// ��ȡƽ̨�汾
		STDMETHOD_( int , GetCurrentLanguage) ();			// ��ȡ��ǰ����
		STDMETHOD_( CString , GetPlatRegistry) ();			// ��ȡƽ̨ע����·��
		STDMETHOD_( CString , GetPlatCopyRight) ();			// ��ȡƽ̨��Ȩ�ַ���
		STDMETHOD_( CString , GetPlatHomeURL) ();			// ��ȡ��ҳURL
		STDMETHOD_( CString , GetPlatDownloadURL) ();		// ��ȡ����URL
		STDMETHOD_( int  , SendMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// ������Ϣ
		STDMETHOD_( int  , ProcessMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// ƽ̨����Ϣ����
		STDMETHOD_( int  , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam);	// ����ƽ̨����
		STDMETHOD_( BOOL , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult);	// ����ƽ̨����

		// DuiVisionϵͳ����
		STDMETHOD_( int  , GetAppID) ();	// ��ȡDuiVisionӦ��ID

		// ���ڲ���
		STDMETHOD_( BOOL , GetWindowBkInfo) (int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile);	// ��ȡ���ڱ�����Ϣ
		STDMETHOD_( BOOL , SetWindowBkInfo) (int nType, int nIDResource, COLORREF clr, LPCTSTR lpszImgFile);	// ���ô��ڱ�����Ϣ
		STDMETHOD_( void  , ClientToScreen) (LPPOINT lpPoint);	// ����ת��Ϊ��Ļ����
		STDMETHOD_( HWND  , GetPaintHWnd) ();	// ��ȡ�������ڵľ��

		// Tooltip����
		STDMETHOD_( void  , SetTooltip) (int nCtrlID, LPCTSTR lpszTooltip, CRect rect, int nTipWidth);	// ����Tooltip
		STDMETHOD_( void , ClearTooltip) ();			// ���Tooltip
		STDMETHOD_( void , SetTooltipCtrlID) (int nTooltipCtrlID);	// ���õ�ǰTooltip�ؼ�ID
		STDMETHOD_( int , GetTooltipCtrlID) ();		// ��ȡ��ǰTooltip�ؼ�ID
	END_INTERFACE_PART(DuiPanel)
	EXPORT_INTERFACE_PART(DuiPanel)

public:
	ULONG			m_ulRefCount;			// ��Ƕ�ӿ���ʹ�õ����ü���,vcicommʹ��
	BOOL				m_bInit;					// �Ƿ��ʼ�����
	CString				m_strXmlFile;				// XML�ļ���
	int					m_nVirtualHeight;			// Panel����ĸ߶�
	int					m_nVirtualWidth;			// Panel����Ŀ��
	CControlBaseFont*	m_pControScrollV;			// ��ֱ������
	CControlBaseFont*	m_pControScrollH;			// ˮƽ������
	int					m_nScrollWidth;				// ���������
	int					m_nVirtualTop;				// ��ǰ������ʾ�Ķ���λ��
	int					m_nVirtualLeft;				// ��ǰ������ʾ�Ķ���λ��
	BOOL				m_bEnableScroll;			// �Ƿ��������

	HINSTANCE			m_hPluginHandle;			// �����������̬��ľ��
	CString				m_strPluginFile;			// �������ļ���
	IDuiPluginPanel*	m_pDuiPluginObject;			// ����������
	IDuiHostWnd*	m_pIDuiHostWnd;		// ����������ڵĹ��ܽӿ�

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("img-scroll", OnAttributeImageScrollV)
		DUI_CUSTOM_ATTRIBUTE("img-scrollh", OnAttributeImageScrollH)
		DUI_INT_ATTRIBUTE("scroll-width", m_nScrollWidth, FALSE)
		DUI_CUSTOM_ATTRIBUTE("xml", OnAttributeXml)
#ifdef _DEBUG
		DUI_CUSTOM_ATTRIBUTE("plugin-debug", OnAttributePlugin)
#else
		DUI_CUSTOM_ATTRIBUTE("plugin", OnAttributePlugin)
#endif
		DUI_INT_ATTRIBUTE("scroll", m_bEnableScroll, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
