// Ìî³äµÄ¾ØÐÎ¿Ø¼þ
#pragma once
#include "ControlBase.h"

class CRectangle : public CControlBase
{
	DUIOBJ_DECLARE_CLASS_NAME(CRectangle, _T("rect"))
public:
	CRectangle(HWND hWnd, CDuiObject* pDuiObject);
	CRectangle(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, 
		Color clr = Color(254, 255, 255, 255), BOOL bIsVisible = TRUE);
	virtual ~CRectangle(void);

protected:
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	int			m_nFrameWidth;			// ±ß¿ò¿í¶È
	int			m_nDashStyle;			// ±ß¿òÀàÐÍ
	Color		m_clr;					// Ìî³äÑÕÉ«
	Color		m_clrFrame;				// ±ß¿òÑÕÉ«

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_COLOR_ATTRIBUTE("color", m_clr, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crframe"), m_clrFrame, FALSE)
		DUI_ENUM_ATTRIBUTE(_T("dashstyle"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("solid"), DashStyleSolid)
            DUI_ENUM_VALUE(_T("dash"), DashStyleDash)
            DUI_ENUM_VALUE(_T("dot"), DashStyleDot)
			DUI_ENUM_VALUE(_T("dashdot"), DashStyleDashDot)
            DUI_ENUM_VALUE(_T("dotdot"), DashStyleDashDotDot)
            DUI_ENUM_VALUE(_T("custom"), DashStyleCustom)
        DUI_ENUM_END(m_nDashStyle)
		DUI_INT_ATTRIBUTE("frame-width", m_nFrameWidth, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};
