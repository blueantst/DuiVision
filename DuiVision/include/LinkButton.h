#pragma once
#include "ControlBase.h"

class CLinkButton : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CLinkButton, _T("linkbtn"))
public:
	CLinkButton(HWND hWnd, CDuiObject* pDuiObject);
	CLinkButton(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, CString strLink,
		Color clrTextNormal = Color(39, 134, 230) ,Color clrTextHover = Color(21, 112, 235),
		Color clrTextDown = Color(21, 112, 235) ,Color clrTextDisable =  Color(128, 128, 128),
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CLinkButton(void);

	void SetLink(CString strLink) { m_strLink = strLink; }
	CString GetLink() { return m_strLink; }

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
	CString m_strLink;
	CRect m_rcText;
	enumButtonState m_enButtonState;
	BOOL m_bDown;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrTextNormal, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crhover"), m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crpush"), m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crdisable"), m_clrTextDisable, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("href"), m_strLink, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};