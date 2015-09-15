// Panel控件，此控件是一个控件容器
#pragma once

#include "ControlBase.h"

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
	virtual	void SetControlDisable(BOOL bIsDisable);
	virtual	BOOL SetControlFocus(BOOL bFocus);

	// 消息响应
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// 根据控件名创建控件实例
	CControlBase* _CreateControlByName(LPCTSTR lpszName);

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
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL OnControlTimer();

	virtual void InitUI(CRect rcClient, DuiXmlNode pNode);

public:
	BOOL				m_bInit;					// 是否初始化完成
	CString				m_strXmlFile;				// XML文件名
	int					m_nVirtualHeight;			// Panel整体的高度
	int					m_nVirtualWidth;			// Panel整体的宽度
	CControlBaseFont*	m_pControScrollV;			// 垂直滚动条
	CControlBaseFont*	m_pControScrollH;			// 水平滚动条
	int					m_nScrollWidth;				// 滚动条宽度
	int					m_nVirtualTop;				// 当前虚拟显示的顶部位置
	int					m_nVirtualLeft;				// 当前虚拟显示的顶部位置
	BOOL				m_bEnableScroll;			// 是否允许滚动

	HINSTANCE			m_hPluginHandle;			// 保存界面插件动态库的句柄
	CString				m_strPluginFile;			// 界面插件文件名
	IDuiPluginPanel*	m_pDuiPluginObject;			// 界面插件对象

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
