#pragma once
#include "ControlBase.h"

// Í¼Æ¬×Ö·û´®¿Ø¼þ
class CDuiImageString : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiImageString, _T("imgstr"))
public:
	CDuiImageString(HWND hWnd, CDuiObject* pDuiObject);
	CDuiImageString(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nNumber, 
		UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	CDuiImageString(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, 
		UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiImageString(void);

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