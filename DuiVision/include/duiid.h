// DuiVision id define
//

#define					WM_SKIN								(WM_USER + 1)	// 皮肤选择消息
#define					WM_MIAN_MENU						(WM_USER + 2)	// 主菜单消息
#define					WM_UI_TASK							(WM_USER + 3)	// 转UI线程的任务处理消息
#define					WM_SYSTEM_TRAYICON					(WM_USER + 4)	// Tray图标消息
#define					WM_USER_CLOSEWND					(WM_USER + 5)	// 自定义的窗口关闭消息

//定时器
#define					IDI_TIME_TEST						1

// 系统控件
#define					ID_NULL								0xFFFFFFFF	// 不存在的控件ID
#define					APP_IPC								100	// 用于进程间通知的IPC
#define					TRAY_ICON							800	// 托盘图标
#define					BT_MIN								900	// 最小化按钮
#define					BT_MAX								901	// 最大化按钮
#define					BT_CLOSE							902	// 关闭按钮
#define					FRAME_MAINWND						903	// 主窗口的透明度渐变层蒙板图片
#define					BT_SKIN								904	// 换肤按钮
#define					WND_SKIN							905	// 皮肤窗口
#define					AREA_CAPTION						906	// 窗口标题区域
#define					BT_OK								910	// 确定按钮
#define					BT_CANCEL							911	// 取消按钮
#define					BT_YES								912	// 是按钮
#define					BT_NO								913	// 否按钮

// 系统控件命名
#define					NAME_TRAY_ICON						_T("tray.icon")			// 托盘图标

#define					NAME_BT_MIN							_T("button.min")		// 最小化按钮
#define					NAME_BT_MAX							_T("button.max")		// 最大化按钮
#define					NAME_BT_CLOSE						_T("button.close")		// 关闭按钮
#define					NAME_BT_SKIN						_T("button.skin")		// 换肤按钮
#define					NAME_BT_SETUP						_T("button.setup")		// 设置按钮
#define					NAME_AREA_CAPTION					_T("win.caption")		// 窗口标题区域
#define					NAME_FRAME_MAINWND					_T("frame.mainwnd")		// 主窗口的透明度渐变层蒙板图片

#define					NAME_BT_OK							_T("button.ok")			// 确定按钮
#define					NAME_BT_CANCEL						_T("button.cancel")		// 取消按钮
#define					NAME_BT_YES							_T("button.yes")		// 是按钮
#define					NAME_BT_NO							_T("button.no")			// 否按钮

#define					NAME_SKIN_WND						_T("skin.wnd")			// Skin窗口
#define					NAME_BT_SKIN_CUSTOMIZE				_T("skin.button.customize")
#define					NAME_BT_SKIN_IMAGE					_T("skin.button.image")
#define					NAME_BT_SKIN_COLOR					_T("skin.button.color")
#define					NAME_SKIN_PIC_SELECT				_T("skin.pic.select")
#define					NAME_SKIN_BOX_COLOR					_T("skin.box.color")
#define					NAME_SKIN_BOX_IMAGE					_T("skin.box.image")

// 系统动作定义
#define					ACTION_CLOSE_WINDOW					_T("close-window:")		// 关闭窗口
#define					ACTION_HIDE_WINDOW					_T("hide-window")		// 隐藏窗口
#define					ACTION_SHOW_WINDOW					_T("show-window:")		// 显示窗口

// 系统消息定义
#define					MSG_TRAY_DBCLICK					1						// 托盘双击消息
#define					MSG_TRAY_LBUTTONDOWN				2						// 托盘左键单击消息

// 控件消息定义
#define					MSG_BUTTON_DOWN						1						// 鼠标或键盘在控件按下
#define					MSG_BUTTON_UP						2						// 鼠标或键盘在控件放开
#define					MSG_BUTTON_DBLCLK					3						// 鼠标在控件双击
#define					MSG_BUTTON_CHECK					4						// 检查框消息
#define					MSG_SCROLL_CHANGE					5						// 滚动条位置变更事件
#define					MSG_CONTROL_BUTTON					6						// 控件内的按钮点击事件
#define					MSG_MOUSE_MOVE						7						// 鼠标移动事件
#define					MSG_MOUSE_LEAVE						8						// 鼠标离开事件
#define					MSG_MOUSE_LDOWN						9						// 鼠标左键按下事件
#define					MSG_MOUSE_LUP						10						// 鼠标左键放开事件
#define					MSG_MOUSE_RDOWN						11						// 鼠标右键按下事件
#define					MSG_MOUSE_RUP						12						// 鼠标右键放开事件
#define					MSG_KEY_DOWN						13						// 键盘按下事件
#define					MSG_CONTROL_EVENT					14						// 控件的自定义事件
#define					MSG_MOUSE_RDBLCLK					15						// 鼠标右键双击
#define					MSG_CONTROL_SELECT					16						// 控件内的选择事件
#define					MSG_CONTROL_DELETE					17						// 控件内的删除事件
#define					MSG_DROP_FILE						18						// 拖拽文件事件
#define					MSG_FOCUS_CHANGE					19						// 控件的焦点状态变更事件
#define					MSG_KEY_UP							20						// 键盘放开事件


// 进程间消息内容的结构定义
struct DUI_INTERPROCESS_MSG
{
	TCHAR	wAppName[128];		// 应用名
	UINT	uControlID;			// 控件ID
	TCHAR	wControlName[64];	// 控件名字
	UINT	uMsg;				// 消息
	WPARAM	wParam;				// 参数1
	LPARAM	lParam;				// 参数2
	TCHAR	wInfo[10240];		// 信息字符串
	UINT	uDataLen;			// 扩展部分长度
};
