// 边框控件，在区域周围画出边框，可以设置区域的渐变透明度，不能响应鼠标
#pragma once
#include "ControlBase.h"

class CDuiFrame : public CControlBase
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiFrame, _T("frame"))
public:
	CDuiFrame(HWND hWnd, CDuiObject* pDuiObject);
	CDuiFrame(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nBeginTransparent = 50, int nEndTransparent = 50, 
		COLORREF clr = RGB(255, 255, 255), BOOL bIsVisible = TRUE);
	virtual ~CDuiFrame(void);

protected:
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	int			m_nBeginTransparent;	// 填充部分起始透明度
	int			m_nEndTransparent;		// 填充部分终止透明度
	int			m_nFrameWidth;			// 边框宽度
	int			m_nDashStyle;			// 边框类型
	Color		m_clr;					// 填充颜色
	Color		m_clrFrame;				// 边框颜色

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
		DUI_INT_ATTRIBUTE("begin-transparent", m_nBeginTransparent, FALSE)
		DUI_INT_ATTRIBUTE("end-transparent", m_nEndTransparent, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};
