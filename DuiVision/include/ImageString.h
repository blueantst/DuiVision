#pragma once
#include "ControlBase.h"


class CImageString : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CImageString, _T("imgstr"))
public:
	CImageString(HWND hWnd, CDuiObject* pDuiObject);
	CImageString(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nNumber, 
		UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	CImageString(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, 
		UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CImageString(void);

	void SetTitle(int nNumber);

	HRESULT OnAttributeMask(const CString& strValue, BOOL bLoading);

protected:
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	// »ñÈ¡Í¼Æ¬Ë÷Òý
	int GetImageIndex(int nIndex);

public:
	CString m_strMask;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("mask"), OnAttributeMask)
	DUI_DECLARE_ATTRIBUTES_END()
};