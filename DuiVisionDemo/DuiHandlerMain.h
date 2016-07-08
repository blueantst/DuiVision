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
	LRESULT OnDuiMsgTrayIconLButtonDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
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
	LRESULT OnDuiMsgListCtrl1Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgListCtrl2Click(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgListCtrl2DblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgGridCtrlDblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgGridCtrlDelBtnClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgGridCtrlDropFile(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgTreeCtrlClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgTreeCtrlDblClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgMenuOption(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgOptionDlgOK(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnShowFlash1(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnShowFlash2(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnShowFlash3(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgBtnShowFlash4(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgTabCtrlClose(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgImageNormalMouseLDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgImageNormalMouseLUp(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgImageNormalMouseMove(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiMsgGridCtrlHeaderRButtonUp(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	CDlgBase*		m_pDlg;
	UINT			m_uTimerAni;	// 动画定时器
	int				m_nAniIndex;	// 动画索引

	BOOL			m_bMouseDownImageNormal;// image-normal控件是否鼠标按下
	CPoint			m_ptControlImageNormal;	// image-normal控件上一次的位置

	// 消息处理定义
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerMain)
		DUI_CONTROL_ID_MESSAGE(APP_IPC, OnDuiMsgInterprocess)
		DUI_CONTROL_NAME_MESSAGE(NAME_SKIN_WND, OnDuiMsgSkin)
		DUI_CONTROL_NAMEMSG_MESSAGE(NAME_TRAY_ICON, MSG_TRAY_DBCLICK, OnDuiMsgTrayIconDClick)
		DUI_CONTROL_NAMEMSG_MESSAGE(NAME_TRAY_ICON, MSG_TRAY_LBUTTONDOWN, OnDuiMsgTrayIconLButtonDown)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("notify_button_1"), MSG_BUTTON_UP, OnDuiMsgNotifyButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("notify_button_2"), MSG_BUTTON_UP, OnDuiMsgNotifyButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("notify_button_3"), MSG_BUTTON_UP, OnDuiMsgNotifyButton3)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("msgbox_button_1"), MSG_BUTTON_UP, OnDuiMsgMsgBoxButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("msgbox_button_2"), MSG_BUTTON_UP, OnDuiMsgMsgBoxButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("msgbox_button_3"), MSG_BUTTON_UP, OnDuiMsgMsgBoxButton3)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("timer_button_1"), MSG_BUTTON_UP, OnDuiMsgTimerButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("timer_button_2"), MSG_BUTTON_UP, OnDuiMsgTimerButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("xml_content_dlg_button"), MSG_BUTTON_UP, OnDuiMsgXmlDlgButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_hide_tab"), MSG_BUTTON_UP, OnDuiMsgHideTabButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_show_tab"), MSG_BUTTON_UP, OnDuiMsgShowTabButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_del_tab_web"), MSG_BUTTON_UP, OnDuiMsgDelWebTabButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("button_load_tab_web"), MSG_BUTTON_UP, OnDuiMsgLoadWebTabButton)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("menu_1"), MSG_BUTTON_UP, OnDuiMsgMenuButton1)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("menu_2"), MSG_BUTTON_UP, OnDuiMsgMenuButton2)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("listctrl_1"), MSG_BUTTON_DOWN, OnDuiMsgListCtrl1Click)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("listctrl_2"), MSG_BUTTON_DOWN, OnDuiMsgListCtrl2Click)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("listctrl_2"), MSG_BUTTON_DBLCLK, OnDuiMsgListCtrl2DblClick)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("gridctrl_1"), MSG_BUTTON_DBLCLK, OnDuiMsgGridCtrlDblClick)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("gridctrl_1"), MSG_DROP_FILE, OnDuiMsgGridCtrlDropFile)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("grid_btn_delete"), MSG_BUTTON_DOWN, OnDuiMsgGridCtrlDelBtnClick)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("treectrl_1"), MSG_BUTTON_DOWN, OnDuiMsgTreeCtrlClick)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("treectrl_1"), MSG_BUTTON_DBLCLK, OnDuiMsgTreeCtrlDblClick)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("item_setup"), MSG_BUTTON_UP, OnDuiMsgMenuOption)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("config.button.ok"), MSG_BUTTON_UP, OnDuiMsgOptionDlgOK)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("textbtn_showflash_1"), MSG_BUTTON_UP, OnDuiMsgBtnShowFlash1)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("textbtn_showflash_2"), MSG_BUTTON_UP, OnDuiMsgBtnShowFlash2)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("textbtn_showflash_3"), MSG_BUTTON_UP, OnDuiMsgBtnShowFlash3)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("textbtn_showflash_4"), MSG_BUTTON_UP, OnDuiMsgBtnShowFlash4)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("tabctrl.controls"), MSG_CONTROL_BUTTON, OnDuiMsgTabCtrlClose)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("image-normal"), MSG_MOUSE_LDOWN, OnDuiMsgImageNormalMouseLDown)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("image-normal"), MSG_MOUSE_LUP, OnDuiMsgImageNormalMouseLUp)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("image-normal"), MSG_MOUSE_MOVE, OnDuiMsgImageNormalMouseMove)
		DUI_CONTROL_NAMEMSG_MESSAGE(_T("gridctrl_header"), MSG_MOUSE_RUP, OnDuiMsgGridCtrlHeaderRButtonUp)
	DUI_DECLARE_MESSAGE_END()
};
