// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI事件处理类
class CDuiHandlerPlugin : public CDuiHandler
{
public:
	CDuiHandlerPlugin(void);
	virtual ~CDuiHandlerPlugin(void);

	void SetDuiPanel(CDuiPanel* pPanel) { m_pPanel = pPanel; }

	virtual void OnInit();

	// DUI消息处理
	LRESULT OnDuiMsgNotifyButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgNotifyButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgNotifyButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMsgBoxButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMsgBoxButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMsgBoxButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgTimerButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgTimerButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgXmlDlgButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMenuButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMenuButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgListCtrl1Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgListCtrl2Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgListCtrl2DblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	CDuiPanel*	m_pPanel;
	UINT m_uTimerAni;	// 动画定时器
	int m_nAniIndex;	// 动画索引

	// 消息处理定义
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerPlugin)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("notify_button_1"), MSG_BUTTON_UP, OnDuiMsgNotifyButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("notify_button_2"), MSG_BUTTON_UP, OnDuiMsgNotifyButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("notify_button_3"), MSG_BUTTON_UP, OnDuiMsgNotifyButton3)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("msgbox_button_1"), MSG_BUTTON_UP, OnDuiMsgMsgBoxButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("msgbox_button_2"), MSG_BUTTON_UP, OnDuiMsgMsgBoxButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("msgbox_button_3"), MSG_BUTTON_UP, OnDuiMsgMsgBoxButton3)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("timer_button_1"), MSG_BUTTON_UP, OnDuiMsgTimerButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("timer_button_2"), MSG_BUTTON_UP, OnDuiMsgTimerButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("xml_content_dlg_button"), MSG_BUTTON_UP, OnDuiMsgXmlDlgButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("menu_1"), MSG_BUTTON_UP, OnDuiMsgMenuButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("menu_2"), MSG_BUTTON_UP, OnDuiMsgMenuButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("listctrl_1"), MSG_BUTTON_DOWN, OnDuiMsgListCtrl1Click)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("listctrl_2"), MSG_BUTTON_DOWN, OnDuiMsgListCtrl2Click)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("listctrl_2"), MSG_BUTTON_DBLCLK, OnDuiMsgListCtrl2DblClick)
	DUI_DECLARE_MESSAGE_END()
};
