// 区域控件，可以设置区域的渐变透明度，不能响应鼠标
#pragma once
#include "ControlBase.h"

class CArea : public CControlBase
{
	DUIOBJ_DECLARE_CLASS_NAME(CArea, _T("area"))
public:
	CArea(HWND hWnd, CDuiObject* pDuiObject);
	CArea(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nBeginTransparent = 50, int nEndTransparent = 50, 
		COLORREF clr = RGB(255, 255, 255), BOOL bIsVisible = TRUE);
	virtual ~CArea(void);

protected:
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	int		m_nTransparentType;			// 透明渐变类型
	int		m_nBeginTransparent;
	int		m_nEndTransparent;
	COLORREF m_clr;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_RGBCOLOR_ATTRIBUTE(_T("color"), m_clr, FALSE)
		DUI_ENUM_ATTRIBUTE(_T("transparent-type"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("horizontal"), TRANSPARENT_HORIZONTAL)
            DUI_ENUM_VALUE(_T("vertical"), TRANSPARENT_VERTICAL)
        DUI_ENUM_END(m_nTransparentType)
		DUI_INT_ATTRIBUTE(_T("begin-transparent"), m_nBeginTransparent, FALSE)
		DUI_INT_ATTRIBUTE(_T("end-transparent"), m_nEndTransparent, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};
