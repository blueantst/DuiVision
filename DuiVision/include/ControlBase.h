#pragma once

#include "DuiObject.h"

#include <vector>

using namespace  std;

class CDlgBase;
class CDlgPopup;
interface IDuiHostWnd;

// 按钮状态
enum enumButtonState
{
	enBSNormal = 0,
	enBSHover,
	enBSDown,
	enBSDisable,
	enBSHoverDown,	
	enBSDisableDown
};

// 位置的计算方式,普通、以中心为0计算、按照百分比计算
enum PIT{PIT_NORMAL=0,PIT_CENTER,PIT_PERCENT};

// 位置信息中每个点的信息
struct DUI_POSITION_ITEM
{
	PIT pit;			// 位置计算方式	
    BOOL bMinus;		// 是否负值
    float  nPos;		// 位置信息
};

// 位置信息结构
struct DUI_POSITION
{
    int nCount;
    union
    {
        struct
        {
            DUI_POSITION_ITEM Left;
            DUI_POSITION_ITEM Top;
            DUI_POSITION_ITEM Right;
            DUI_POSITION_ITEM Bottom;
        };
        DUI_POSITION_ITEM Item[4];
    };
};

// 透明渐变类型
enum TRANSPARENT_TYPE
{
	TRANSPARENT_HORIZONTAL = 0,	// 水平方向渐变
	TRANSPARENT_VERTICAL,				// 垂直方向渐变
};

// 菜单ID定义
#define	WM_DUI_MENU		(WM_USER + 20)	

class CControlBase : public CDuiObject
{
public:
	CControlBase(HWND hWnd, CDuiObject* pDuiObject) ;
	CControlBase(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE , BOOL bRresponse = TRUE) ;
	virtual ~CControlBase(void);

	// 对象和句柄
	virtual CDuiObject* GetParent() { return m_pParentDuiObject; }
	void SetParent(CDuiObject* pParent) { m_pParentDuiObject = pParent; }
	HWND GetHWND() { return m_hWnd; }
	virtual void SetHWND(HWND hWnd);
	HWND GetNativeHWnd() { return m_hwndHost; }
	HWND GetPaintHWnd();

	void TestMainThread();
	void Draw(CDC &dc, CRect rcUpdate);
	virtual void DrawControl(CDC &dc, CRect rcUpdate) = 0;
	virtual BOOL DrawSubControls(CDC &dc, CRect rcUpdate);
	virtual BOOL IsDraw(CPoint point) { return FALSE; }
	virtual void SetUpdate(BOOL bUpdate, COLORREF clr = 0);
	BOOL GetUpdate() { return m_bUpdate; };
	void UpdateMemDC(CDC &dc, int nWidth, int nHeight);
	void UpdateAnimateDC(CDC &dc, int nWidth, int nHeight);
	void ClientToScreen(LPPOINT lpPoint);

	// 位置信息
	virtual void SetRect(CRect rc);
	virtual void SetControlRect(CRect rc) { m_rc = rc; };
	virtual CRect GetRect() { return m_rc;};
	virtual void OnPositionChange();	// 位置刷新
	virtual void InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);

	void SetPosStr(CString strPos) { m_strPos = strPos; };
	CString GetPosStr() { return m_strPos;};
	int PositionItem2Value( const DUI_POSITION_ITEM &pos ,int nMin, int nMax);
	LPCTSTR ParsePosition(LPCTSTR pszPos,DUI_POSITION_ITEM &pos);
	BOOL SetPositionWithParent(CRect rectParent);
	HRESULT OnAttributePosChange(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeWidth(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeHeight(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeMenuPosChange(const CString& strValue, BOOL bLoading);

	HRESULT OnAttributeShortcut(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeDisable(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeCursor(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeSendDuiMsg(const CString& strValue, BOOL bLoading);
	
	// 属性
	void SetVisible(BOOL bIsVisible);
	virtual void SetControlVisible(BOOL bIsVisible) { m_bIsVisible = bIsVisible; }
	BOOL GetVisible() { return IsControlVisible(); }
	virtual BOOL IsControlVisible();
	void SetHide(BOOL bIsHide);
	virtual void SetControlHide(BOOL bIsHide) { m_bIsHide = bIsHide; }
	virtual void SetControlWndVisible(BOOL bIsVisible) { };	// 设置控件中的Windows原生控件是否可见的状态
	void SetDisable(BOOL bIsDisable);
	virtual	void SetControlDisable(BOOL bIsDisable) { m_bIsDisable = bIsDisable; }
	BOOL GetDisable() { return m_bIsDisable; }
	void SetRresponse(BOOL bRresponse) { m_bRresponse = bRresponse; }
	BOOL GetRresponse() { return m_bRresponse; }
	void SetTabStop(BOOL bTabStop) { m_bTabStop = bTabStop; }
	BOOL IsTabStop() { return m_bTabStop; }
	void SetTooltip(CString strTooltip) { m_strTooltip = strTooltip; }
	CString GetTooltip() { return m_strTooltip; }
	void SetTipWidth(int nTipWidth) { m_nTipWidth = nTipWidth; }
	void SetAction(CString strAction) { m_strAction = strAction; }
	CString GetAction() { return m_strAction; }
	BOOL UseImageECM() { return m_bImageUseECM; }
	void SetDragEnable(BOOL bDragEnable) { m_bDragEnable = bDragEnable; }
	BOOL GetDragEnable() { return m_bDragEnable; }
	void SetDropFileEnable(BOOL bDropFileEnable) { m_bDropFileEnable = bDropFileEnable; }
	BOOL GetDropFileEnable() { return m_bDropFileEnable; }
	void SetShotcutKey(UINT nShortcutKey, UINT nShortcutFlag);

	virtual	BOOL PtInRect(CPoint point);	// 判断坐标是否在控件范围内
	UINT GetControlID() { return m_uID; }	// 控件ID就是DUI对象的ID
	void UpdateControl(BOOL bUpdate = false, BOOL bVisible = false);
	void UpdateControl(CRect rc, BOOL bVisible = false, BOOL bUpdate = false);
	BOOL GetDblClk() { return m_bDblClk; }

	// 焦点
	virtual	BOOL SetWindowFocus();				// 设置窗口焦点
	BOOL OnFocus(BOOL bFocus);
	virtual	BOOL SetControlFocus(BOOL bFocus);		// 设置控件的焦点
	BOOL IsFocusControl();
	CControlBase* GetFocusControl(CControlBase* pFocusControl);
	CControlBase* GetPrevFocusableControl(CControlBase* pFocusControl);
	CControlBase* GetNextFocusableControl(CControlBase* pFocusControl);

	// Tooltip
	void SetTooltip(CControlBase* pControl, CString strTooltip, CRect rect, BOOL bControlWidth, int nTipWidth = 0);
	void ClearTooltip();
	void SetTooltipCtrlID(int nTooltipCtrlID);
	int GetTooltipCtrlID();

	// 鼠标键盘消息
	BOOL OnMouseMove(UINT nFlags, CPoint point);
	BOOL OnLButtonDown(UINT nFlags, CPoint point);
	BOOL OnLButtonUp(UINT nFlags, CPoint point);
	BOOL OnLButtonDblClk(UINT nFlags, CPoint point);
	BOOL OnScroll(BOOL bVertical, UINT nFlags, CPoint point);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnRButtonDown(UINT nFlags, CPoint point);
	BOOL OnRButtonUp(UINT nFlags, CPoint point);
	BOOL OnRButtonDblClk(UINT nFlags, CPoint point);

	BOOL OnTimer();

	virtual BOOL OnMousePointChange(CPoint& point) { return FALSE; }
	virtual BOOL OnCheckMouseResponse(UINT nFlags, CPoint point) { return TRUE; }	// 判断鼠标是否在控件可响应的区域
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlSetCursor(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlLButtonDblClk(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnControlKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnControlSetDuiMsg(LPCTSTR lpszDuiMsg);
	virtual BOOL OnControlRButtonDown(UINT nFlags, CPoint point){ return FALSE; };
	virtual BOOL OnControlRButtonUp(UINT nFlags, CPoint point){ return FALSE; };
	virtual BOOL OnControlRButtonDblClk(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlDropFile(CPoint point, CString strFileName);

	virtual	BOOL OnControlTimer() { return FALSE; }

	// 控件和子控件
	CControlBase *GetControl(UINT uControlID);
	CControlBase *GetControl(CString strControlName);
	void AddControl(CControlBase* pControl);
	BOOL RemoveControl(CControlBase* pControl);
	BOOL RemoveControl(CString strControlName, UINT uControlID);
	void RemoveControls(CString strClassName);
	void RemoveControls();
	CDlgBase* GetParentDialog(BOOL bEnablePopup = TRUE);
	IDuiHostWnd* GetParentIDuiHostWnd();
	vector<CControlBase*>* GetControls() { return &m_vecControl; }

	// 消息处理
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CallDuiHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// 发送通知消息
	LRESULT SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam);

	// 打开弹出对话框
	void OpenDlgPopup(CDlgPopup *pWndPopup, CRect rc, UINT uMessageID, BOOL bShow=TRUE);
	// 关闭弹出对话框
	void CloseDlgPopup();

protected:
	CDuiObject*				m_pParentDuiObject;	// 父控件对象
	HWND					m_hWnd;				// 父窗口的Windows窗口句柄(一般是对话框的窗口句柄)
	HWND					m_hwndHost;			// 原生窗口句柄(用于保存原生控件的宿主窗口)
	
	CString					m_strPos;			// 位置(XML定义中的字符串)
	BOOL					m_bIsVisible;		// 是否可见
	BOOL					m_bIsHide;			// 是否处于隐藏状态(tab页切换时候会自动隐藏子控件)
	BOOL					m_bIsDisable;		// 是否可用
	BOOL					m_bRresponse;		// 是否可以响应鼠标事件
	BOOL					m_bTabStop;			// 是否可以响应Tab键
	BOOL					m_bIsSelect;		// 是否允许选中
	BOOL					m_bDblClk;			// 是否可以响应双击事件
	BOOL					m_bMouseDown;		// 是否鼠标按下
	BOOL					m_bDragEnable;		// 是否允许鼠标拖动控件
	BOOL					m_bDropFileEnable;	// 是否允许鼠标拖拽文件到此控件

	UINT					m_nShortcutKey;		// 快捷键按键
	UINT					m_nShortcutFlag;	// 快捷键扫描码

	BOOL					m_bUpdate;			// 更新贴图

	BOOL					m_bIsRun;			// 是否有执行功能
	BOOL					m_bRunTime;			// 是否正在执行定时器

	BOOL					m_bImageUseECM;		// 是否使用图片自身的颜色管理信息

	vector<CControlBase *>	m_vecControl;		// 控件列表
	vector<CControlBase *>	m_vecArea;			// 区域列表(不影响鼠标事件)
	CControlBase*			m_pControl;			// 当前控件

	// 内存DC
	int						m_nDCWidth;			// 内存DC宽度
	int						m_nDCHeight;		// 内存DC高度
	CBitmap					m_bitmap;			// 内存DC的位图对象
	CDC						m_memDC;			// 内存DC
	CBitmap*				m_pOldBitmap;		// 保存内存DC旧的位图对象

	// 动画DC
	int						m_nAniDCWidth;		// 动画DC宽度
	int						m_nAniDCHeight;		// 动画DC高度
	CBitmap					m_aniBitmap;		// 动画DC的位图对象
	CDC						m_animateDC;		// 动画DC
	CBitmap*				m_pOldAniBitmap;	// 保存动画DC旧的位图对象

	int						m_nWidth;			// 控件宽度
	int						m_nHeight;			// 控件高度

	DUI_POSITION			m_posMenu;			// 弹出菜单的位置信息

	CString					m_strTooltip;		// 控件的Tooltip
	int						m_nTipWidth;		// Tooltip的宽度
	CString					m_strAction;		// 控件执行的动作
	BOOL					m_bTaskMsg;			// 调用事件响应函数时候采用任务方式

	CDlgPopup*				m_pWndPopup;		// 保存的弹出框指针

	HCURSOR					m_hCursor;			// 控件的鼠标光标
	
	CPoint					m_ptLastMousePosition;	// 控件上一次的鼠标移动位置

	BOOL					m_bDuiMsgMouseMove;	// 是否发送鼠标移动DUI消息
	BOOL					m_bDuiMsgMouseLDown;// 是否发送鼠标左键按下DUI消息
	BOOL					m_bDuiMsgMouseLUp;	// 是否发送鼠标左键放开DUI消息
	BOOL					m_bDuiMsgMouseLDblClk;// 是否发送鼠标左键双击DUI消息
	BOOL					m_bDuiMsgMouseRDown;// 是否发送鼠标右键按下DUI消息
	BOOL					m_bDuiMsgMouseRUp;	// 是否发送鼠标右键放开DUI消息
	BOOL					m_bDuiMsgMouseRDblClk;// 是否发送鼠标右键双击DUI消息
	BOOL					m_bDuiMsgKeyDown;	// 是否发送键盘按下DUI消息
	BOOL					m_bDuiMsgKeyUp;		// 是否发送键盘放开DUI消息
	BOOL					m_bDuiMsgFocusChange;// 是否发送控件焦点变化的DUI消息
	BOOL					m_bMouseLeave;		// 鼠标是否已经离开控件

	DUI_DECLARE_ATTRIBUTES_BEGIN()
        //DUI_STYLE_ATTRIBUTE("class", m_style, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("show"), m_bIsVisible, TRUE)
		DUI_CUSTOM_ATTRIBUTE(_T("disable"), OnAttributeDisable)
		DUI_BOOL_ATTRIBUTE(_T("response"), m_bRresponse, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("tabstop"), m_bTabStop, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("drag"), m_bDragEnable, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("dropfile"), m_bDropFileEnable, TRUE)
		DUI_CUSTOM_ATTRIBUTE(_T("pos"), OnAttributePosChange)
		DUI_CUSTOM_ATTRIBUTE(_T("width"), OnAttributeWidth)
		DUI_CUSTOM_ATTRIBUTE(_T("height"), OnAttributeHeight)
		DUI_CUSTOM_ATTRIBUTE(_T("menupos"), OnAttributeMenuPosChange)
		DUI_TSTRING_ATTRIBUTE(_T("action"), m_strAction, FALSE)
		DUI_INT_ATTRIBUTE(_T("taskmsg"), m_bTaskMsg, TRUE)
		DUI_INT_ATTRIBUTE(_T("img-ecm"), m_bImageUseECM, TRUE)
		DUI_TSTRING_ATTRIBUTE(_T("tip"), m_strTooltip, FALSE)
		DUI_INT_ATTRIBUTE(_T("tip-width"), m_nTipWidth, TRUE)
		DUI_CUSTOM_ATTRIBUTE(_T("shortcut"), OnAttributeShortcut)
		DUI_CUSTOM_ATTRIBUTE(_T("cursor"), OnAttributeCursor)
		DUI_CUSTOM_ATTRIBUTE(_T("duimsg"), OnAttributeSendDuiMsg)
    DUI_DECLARE_ATTRIBUTES_END()
};

// DUI字体信息结构
struct DuiFontInfo
{
	CString strName;	// 字体对象名
	CString strFont;	// 字体名
	int nFontWidth;		// 字体大小
	FontStyle fontStyle;// 字体风格
	CString strOS;		// 操作系统
};

// 位置和对齐方式
enum {
    // Specify by "width" attribute
    SizeX_Mask          = 0x0007UL, 
    SizeX_Specify       = 0x0001UL, // width > 0
    SizeX_FitContent    = 0x0002UL, // width <= 0
    SizeX_FitParent     = 0x0004UL, // width = "full" default

    // Specify by "height" attribute
    SizeY_Mask          = 0x0070UL, 
    SizeY_Specify       = 0x0010UL, // height > 0
    SizeY_FitContent    = 0x0020UL, // height <= 0 default
    SizeY_FitParent     = 0x0040UL, // height = "full" default
    // Specify by "float" attribute

    Position_Mask       = 0x0300UL, 
    Position_Relative   = 0x0100UL, // float = 0 default
    Position_Float      = 0x0200UL, // float = 1

    // Specify by "valign" and "align" attribute, only using at float = 1 or panel control (Vert-Align)
    Align_Mask          = 0xF000UL, 
    VAlign_Top          = 0x0000UL, // valign = top
    VAlign_Middle       = 0x1000UL, // valign = middle
    VAlign_Bottom       = 0x2000UL, // valign = bottom
    Align_Left          = 0x0000UL, // align = left
    Align_Center        = 0x4000UL, // align = center
    Align_Right         = 0x8000UL, // align = right
};

// 具有文字的控件基类
class CControlBaseFont : public CControlBase
{
public:
	CControlBaseFont(HWND hWnd, CDuiObject* pDuiObject);
	CControlBaseFont(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE , BOOL bRresponse = TRUE,
		CString strFont = _T(""), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular) ;
	virtual ~CControlBaseFont(void);

	void SetTitle(CString strTitle);
	virtual void SetControlTitle(CString strTitle) { m_strTitle = strTitle; }
	virtual CString GetTitle() { return m_strTitle; }

	// 对齐方式
	virtual void SetAlignment(UINT uAlignment);
	virtual void SetVAlignment(UINT uVAlignment);
	virtual void SetAlignment(UINT uAlignment, UINT uVAlignment);

	UINT GetGDIAlignment(UINT uAlignment);
	UINT GetGDIVAlignment(UINT uVAlignment);

	// 字体和图片属性
	virtual void SetFont(CString strFont = _T(""), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular);
	virtual BOOL SetBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	virtual BOOL SetBitmap(CString strImage);
	void SetBitmapCount(int nCount);
	BOOL SetImage(CString strImageA);
	HRESULT OnAttributeFont(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeSkin(const CString& strValue, BOOL bLoading);

protected:
	CString					m_strTitle;			// 标题
	CString					m_strFont;			// 字体
	int						m_nFontWidth;		// 字体宽度
	FontStyle				m_fontStyle;		// 字体Style
	UINT					m_uAlignment;		// 水平对齐方式
	UINT					m_uVAlignment;		// 垂直对齐方式

	Image*					m_pImage;			// 图片
	CSize					m_sizeImage;		// 图片大小
	int						m_nImagePicCount;	// Image中默认包含的图片个数

	DUI_DECLARE_ATTRIBUTES_BEGIN()
        DUI_TSTRING_ATTRIBUTE(_T("title"), m_strTitle, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("font"), OnAttributeFont)
		DUI_TSTRING_ATTRIBUTE(_T("fontname"), m_strFont, FALSE)
		DUI_INT_ATTRIBUTE(_T("fontwidth"), m_nFontWidth, FALSE)
		DUI_ENUM_ATTRIBUTE(_T("valign"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("top"), VAlign_Top)
            DUI_ENUM_VALUE(_T("middle"), VAlign_Middle)
            DUI_ENUM_VALUE(_T("bottom"), VAlign_Bottom)
        DUI_ENUM_END(m_uVAlignment)
        DUI_ENUM_ATTRIBUTE(_T("align"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("left"), Align_Left)
            DUI_ENUM_VALUE(_T("center"), Align_Center)
            DUI_ENUM_VALUE(_T("right"), Align_Right)
        DUI_ENUM_END(m_uAlignment)
		DUI_CUSTOM_ATTRIBUTE(_T("image"), OnAttributeImage)
		DUI_CUSTOM_ATTRIBUTE(_T("skin"), OnAttributeSkin)
		DUI_INT_ATTRIBUTE(_T("img-count"), m_nImagePicCount, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
