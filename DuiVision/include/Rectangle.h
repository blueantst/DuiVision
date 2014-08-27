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
	Color m_clr;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_COLOR_ATTRIBUTE(_T("color"), m_clr, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};
