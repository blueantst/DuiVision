#pragma once
#include "ControlBase.h"

class CDuiRadioButton : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiRadioButton, _T("radiobtn"))
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

	HRESULT OnAttributeCheck(const CString& strValue, BOOL bLoading);

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
	BOOL			m_bShowFocus;		// 是否显示焦点框
	CString			m_strGroupName;		// Radio组名字
	CString			m_strValue;			// Radio值

	Color			m_clrText;			// 文字颜色

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("check"), OnAttributeCheck)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("group"), m_strGroupName, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("value"), m_strValue, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("showfocus"), m_bShowFocus, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};