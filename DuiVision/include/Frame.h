// 边框控件，在区域周围画出边框，可以设置区域的渐变透明度，不能响应鼠标
#pragma once
#include "ControlBase.h"

class CFrame : public CControlBase
{
	DUIOBJ_DECLARE_CLASS_NAME(CFrame, _T("frame"))
public:
	CFrame(HWND hWnd, CDuiObject* pDuiObject);
	CFrame(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nBeginTransparent = 50, int nEndTransparent = 50, 
		COLORREF clr = RGB(255, 255, 255), BOOL bIsVisible = TRUE);
	virtual ~CFrame(void);

protected:
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	int		m_nBeginTransparent;
	int		m_nEndTransparent;
	COLORREF m_clr;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_RGBCOLOR_ATTRIBUTE("color", m_clr, FALSE)
		DUI_INT_ATTRIBUTE("begin-transparent", m_nBeginTransparent, FALSE)
		DUI_INT_ATTRIBUTE("end-transparent", m_nEndTransparent, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};
