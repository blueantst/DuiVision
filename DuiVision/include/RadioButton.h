#pragma once
#include "ControlBase.h"

class CDuiRadioButton : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiRadioButton, "radiobtn")
public:
	CDuiRadioButton(HWND hWnd, CDuiObject* pDuiObject);
	CDuiRadioButton(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle= TEXT(""), BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	~CDuiRadioButton(void);

	CString GetGroupName() { return m_strGroupName; }
	void SetGroupName(CString strGroupName) { m_strGroupName = strGroupName; }
	CString GetValue() { return m_strValue; }
	CString GetGroupValue();

	BOOL GetCheck();
	BOOL SetCheck(BOOL bCheck);
	void SetControlCheck(BOOL bCheck);
	BOOL ResetGroupCheck();

	HRESULT OnAttributeCheck(const CStringA& strValue, BOOL bLoading);

	void SetTextColor(Color clrText) { m_clrText = clrText; };

	virtual BOOL SetControlFocus(BOOL bFocus);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual	void SetControlDisable(BOOL bIsDisable);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	
public:
	enumButtonState	m_enButtonState;
	BOOL			m_bDown;
	BOOL			m_bMouseDown;
	BOOL			m_bIsFocus;			// 当前是否处于焦点状态
	CString			m_strGroupName;		// Radio组名字
	CString			m_strValue;			// Radio值

	Color			m_clrText;			// 文字颜色

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("check", OnAttributeCheck)
		DUI_COLOR_ATTRIBUTE("crtext", m_clrText, FALSE)
		DUI_TSTRING_ATTRIBUTE("group", m_strGroupName, FALSE)
		DUI_TSTRING_ATTRIBUTE("value", m_strValue, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};