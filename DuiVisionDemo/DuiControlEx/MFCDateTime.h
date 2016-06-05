#pragma once
#include "DuiEdit.h"

class CDlgBase;
class CMFCDateTime;

enum DATETIME_VALUES
{
	YESTODAY,		//<<<昨天
	NOW,			//<<<今天
	TOMORROW		//<<<明天
};

class CMFCMonthCalCtrl : public CMonthCalCtrl
{
public:
	CMFCMonthCalCtrl(CDuiObject* pParentObj, UINT msg, UINT id);
	virtual ~CMFCMonthCalCtrl();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnExLButtonDbClk(UINT id, long x, long y);
	BOOL SendMessageToParent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
protected:
	bool m_DbClick;						// 鼠标双击
	UINT m_nMsgClick;				// 选择时间的事件
	CDuiObject* m_pParentObj;		// 父控件
	UINT m_ControlID;					// 父控件ID
};

// 时间选择控件
class CMFCDateTime : public CDuiEdit
{
	DUIOBJ_DECLARE_CLASS_NAME(CMFCDateTime, _T("mfcdatetime"))
public:
	CMFCDateTime(HWND hWnd, CDuiObject* pDuiObject);
	CMFCDateTime(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle = TEXT(""), BOOL bPassWord = FALSE, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual ~CMFCDateTime();

	SYSTEMTIME& GetTime();
	void SetTime(SYSTEMTIME* pst);

	virtual void SetControlRect(CRect rc);
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlTitle(CString strTitle);
	virtual BOOL SetControlFocus(BOOL bFocus);

	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HRESULT OnAttributeDateTimeValue(const CString& strValue, BOOL bLoading);

	BOOL OnMonthCalCtrlSelect(UINT nFlags, CPoint point);

protected:
	HFONT GetDateTimeFont();
	
	void ShowCMonthCalCtrl();
	void HideCMonthCalCtrl();
	void DeleteCMonthCalCtrl();

	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);

protected:
	SYSTEMTIME m_sysTime;
	int        m_nDTUpdateFlag;
	DATETIME_VALUES m_DateTime;		//<<<时间
	CMFCMonthCalCtrl* m_pMouthCalCtrl;
	CRect  m_rcMounth;
	int  m_MouthCalCtrlHeight;
	int  m_MouthCalCtrlWidth;
	BOOL m_IsShowMouthCalCtrl;
	BOOL m_isVailidRect;
	BOOL m_isDefaultToday;

protected:
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_ENUM_ATTRIBUTE(_T("value"), DATETIME_VALUES, TRUE)
			DUI_ENUM_VALUE(_T("yestoday"), YESTODAY)
			DUI_ENUM_VALUE(_T("now")	 , NOW)
			DUI_ENUM_VALUE(_T("tomorrow"), TOMORROW)
		DUI_ENUM_END(m_DateTime)
		DUI_CUSTOM_ATTRIBUTE(_T("txt"), OnAttributeDateTimeValue)
	DUI_DECLARE_ATTRIBUTES_END()
};
