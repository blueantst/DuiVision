#pragma once
#include "ControlBase.h"

class CDuiText : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiText, _T("text"))
public:
	CDuiText(HWND hWnd, CDuiObject* pDuiObject);
	CDuiText(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, 
		Color clr = Color(254, 0, 0, 0), CString strFont = DuiSystem::GetDefaultFont(), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular, BOOL bIsVisible = TRUE);
	virtual ~CDuiText(void);

	void SetMarkText(CString strMark);
	void SetTitleMarkText(CString strTitle, CString strMark, int nStart = 0);
	void SetMarkText(CString strMark, Color clrMark = Color(237, 128, 27), int nStart = 0);
	void SetMarkText(CString strTitle, CString strMark, Color clrMark = Color(237, 128, 27), int nStart = 0);

	void SetTextColor(Color clrText) { m_clrText = clrText; };

	int GetVirtualHeight();

	HRESULT OnAttributeBackColor(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeTextHoverColor(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeTextShadowColor(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeImageScroll(const CString& strValue, BOOL bLoading);

	// 消息响应
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	CControlBaseFont*	m_pControScrollV;	// 垂直滚动条
	BOOL				m_bScrollV;			// 是否启用垂直滚动条
	int					m_nScrollWidth;		// 滚动条宽度
	BOOL				m_bEnableHover;		// 是否允许鼠标移动时候变色
	BOOL				m_bEnableShadow;	// 是否允许画阴影
	bool				m_bBack;			// 是否填充背景
	Color				m_clrBack;			// 背景色
	Color				m_clrText;			// 文字颜色
	Color				m_clrTextHover;		// 热点文字颜色
	Color				m_clrTextShadow;	// 阴影文字颜色
	Color				m_clrMark;			// 标记颜色
	CString				m_strMark;			// 标记文字
	int					m_nStart;			// 标记起始位置
	enumButtonState		m_enButtonState;	// 状态
	int					m_nVirtualHeight;	// 显示全部文字需要的整体的高度
	int					m_nBkTransparent;	// 背景透明度

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("img-scroll"), OnAttributeImageScroll)
		DUI_INT_ATTRIBUTE(_T("scroll-width"), m_nScrollWidth, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("crhover"), OnAttributeTextHoverColor)
		DUI_CUSTOM_ATTRIBUTE(_T("crshadow"), OnAttributeTextShadowColor)
		DUI_CUSTOM_ATTRIBUTE(_T("crback"), OnAttributeBackColor)
		DUI_COLOR_ATTRIBUTE(_T("crmark"), m_clrMark, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("mask"), m_strMark, FALSE)
		DUI_INT_ATTRIBUTE(_T("bk-transparent"), m_nBkTransparent, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};