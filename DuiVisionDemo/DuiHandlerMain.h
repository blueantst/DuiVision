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

	// 皮肤选择消息处理
	LRESULT OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);

	// 进程间消息处理
	LRESULT	OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);

	// DUI消息处理
	LRESULT OnDuiMsgTrayIconDClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgNotifyButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgNotifyButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgNotifyButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMsgBoxButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMsgBoxButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMsgBoxButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgTimerButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgTimerButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgXmlDlgButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgHideTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgShowTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgDelWebTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgLoadWebTabButton(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMenuButton1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMenuButton2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMenuButton3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgListCtrl1Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgListCtrl2Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgTreeCtrlClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMenuOption(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgOptionDlgOK(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnShowFlash1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnShowFlash2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnShowFlash3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnShowFlash4(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	CDlgBase* m_pDlg;
	UINT m_uTimerAni;	// 动画定时器
	int m_nAniIndex;	// 动画索引

	// 消息处理定义
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerMain)
		DUI_CONTROL_ID_MESSAGE(APP_IPC, OnDuiMsgInterprocess)
		DUI_CONTROL_NAME_MESSAGE(NAME_SKIN_WND, OnDuiMsgSkin)
		DUI_CONTROL_NAMEMSG_MESSAGE(NAME_TRAY_ICON, MSG_TRAY_DBCLICK, OnDuiMsgTrayIconDClick)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"notify_button_1", BUTTOM_UP, OnDuiMsgNotifyButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"notify_button_2", BUTTOM_UP, OnDuiMsgNotifyButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"notify_button_3", BUTTOM_UP, OnDuiMsgNotifyButton3)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"msgbox_button_1", BUTTOM_UP, OnDuiMsgMsgBoxButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"msgbox_button_2", BUTTOM_UP, OnDuiMsgMsgBoxButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"msgbox_button_3", BUTTOM_UP, OnDuiMsgMsgBoxButton3)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"timer_button_1", BUTTOM_UP, OnDuiMsgTimerButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"timer_button_2", BUTTOM_UP, OnDuiMsgTimerButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"xml_content_dlg_button", BUTTOM_UP, OnDuiMsgXmlDlgButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"button_hide_tab", BUTTOM_UP, OnDuiMsgHideTabButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"button_show_tab", BUTTOM_UP, OnDuiMsgShowTabButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"button_del_tab_web", BUTTOM_UP, OnDuiMsgDelWebTabButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"button_load_tab_web", BUTTOM_UP, OnDuiMsgLoadWebTabButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"menu_1", BUTTOM_UP, OnDuiMsgMenuButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"menu_2", BUTTOM_UP, OnDuiMsgMenuButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"menu_3", BUTTOM_UP, OnDuiMsgMenuButton3)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"listctrl_1", BUTTOM_DOWN, OnDuiMsgListCtrl1Click)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"listctrl_2", BUTTOM_DOWN, OnDuiMsgListCtrl2Click)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"treectrl_1", BUTTOM_DOWN, OnDuiMsgTreeCtrlClick)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"item_setup", BUTTOM_UP, OnDuiMsgMenuOption)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"config.button.ok", BUTTOM_UP, OnDuiMsgOptionDlgOK)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"textbtn_showflash_1", BUTTOM_UP, OnDuiMsgBtnShowFlash1)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"textbtn_showflash_2", BUTTOM_UP, OnDuiMsgBtnShowFlash2)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"textbtn_showflash_3", BUTTOM_UP, OnDuiMsgBtnShowFlash3)
		DUI_CONTROL_NAMEMSG_MESSAGE(L"textbtn_showflash_4", BUTTOM_UP, OnDuiMsgBtnShowFlash4)
	DUI_DECLARE_MESSAGE_END()
};
