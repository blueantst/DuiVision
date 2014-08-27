#pragma once
#include "ControlBase.h"

class CHideButton : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CHideButton, _T("hidebtn"))
public:
	CHideButton(HWND hWnd, CDuiObject* pDuiObject);
	CHideButton(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTip, CString strTitle, Color clrTip = Color(56, 56, 56),
		Color clrTextNormal = Color(39, 134, 230) ,Color clrTextHover = Color(93, 169, 242),
		Color clrTextDown = Color(21, 112, 235) ,Color clrTextDisable =  Color(128, 128, 128),
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CHideButton(void);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	Color m_clrTextNormal;
	Color m_clrTextHover;
	Color m_clrTextDown;
	Color m_clrTextDisable;
	Color m_clrTip;
	CRect m_rcText;
	enumButtonState m_enButtonState;
	BOOL m_bDown;
	CString m_strTip;
	BOOL m_bShowButton;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrTextNormal, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crhover"), m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crpush"), m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crdisable"), m_clrTextDisable, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtip"), m_clrTip, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("text"), m_strTip, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};