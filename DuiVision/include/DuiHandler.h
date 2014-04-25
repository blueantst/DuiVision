// DuiVision message handler base class
#pragma once

class CDuiObject;

// DUI事件处理类
class CDuiHandler
{
public:
	CDuiHandler(void);
	virtual ~CDuiHandler(void);

	void SetDuiObject(CDuiObject* pDuiObject);
	CControlBase* GetControl(UINT uControlID);
	CControlBase* GetControl(CString strControlName);
	CDlgBase* GetControlDialog(UINT uControlID);

	void SetVisible(CString strControlName, BOOL bIsVisible);
	void SetDisable(CString strControlName, BOOL bIsDisable);
	void SetTitle(CString strControlName, CString strTitle);
	CString GetTitle(CString strControlName);

	virtual void OnInit();
	virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

protected:
	CDuiObject*	m_pDuiObject;			// DUI对象
};
