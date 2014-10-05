// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI事件处理类
class CDuiHandlerMain : public CDuiHandler
{
public:
	CDuiHandlerMain(void);
	virtual ~CDuiHandlerMain(void);

	void SetDialog(CDlgBase* pDlg) { m_pDlg = pDlg; };

	virtual void OnInit();

[!if OPTION_CHECK_SKINWND]
	// 皮肤选择消息处理
	LRESULT OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
[!endif]

	// 进程间消息处理
	LRESULT	OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);

[!if OPTION_CHECK_TRAYICON]
	// 托盘双击消息处理
	LRESULT OnDuiMsgTrayIconDClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
[!endif]
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	CDlgBase* m_pDlg;
	UINT m_uTimerAni;	// 动画定时器
	int m_nAniIndex;	// 动画索引

	// 消息处理定义
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerMain)
		DUI_CONTROL_ID_MESSAGE(APP_IPC, OnDuiMsgInterprocess)
[!if OPTION_CHECK_SKINWND]
		DUI_CONTROL_NAME_MESSAGE(NAME_SKIN_WND, OnDuiMsgSkin)
[!endif]
[!if OPTION_CHECK_TRAYICON]
		DUI_CONTROL_NAMEMSG_MESSAGE(NAME_TRAY_ICON, MSG_TRAY_DBCLICK, OnDuiMsgTrayIconDClick)
[!endif]
	DUI_DECLARE_MESSAGE_END()
};
