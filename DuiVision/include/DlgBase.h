

#ifndef __DLG_BASE_H__
#define __DLG_BASE_H__
#include <vector>
#include "DuiVision.h"
#include "WndShadow.h"

using namespace  std;

class  CDlgBase : public CDialog, public CTimer, public CDuiObject
{
public:
	static LPCTSTR GetClassName() { return _T("dlg");}
	virtual BOOL IsClass(LPCTSTR lpszName)
	{
		if(_tcscmp(GetClassName(), lpszName)  == 0) return TRUE;
		return __super::IsClass(lpszName);
	}

	DECLARE_DYNAMIC(CDlgBase)

	// 对话框控件预设置结构
	struct CONTROL_VALUE
	{
		CString strName;	// 控件对象名
		CString strType;	// 控件类型
		CString strValue;	// 设置的值
	};

public:
	UINT			m_nIDTemplate;				// 对话框资源ID
	BOOL			m_bInit;					// 是否初始化完成
	CString			m_strXmlFile;				// 窗口的XML文件名
	CString			m_strXmlContent;			// 窗口的XML内容

protected:
	CDuiObject*		m_pParentDuiObject;			// 父控件对象

	DWORD			m_nMainThreadId;			// 主线程ID

	UINT			m_uTimerAnimation;			// 动画定时器
	UINT			m_uTimerAutoClose;			// 用于窗口自动关闭的定时器ID

	BOOL			m_bChangeSize;				// 改变窗口大小
	CSize			m_MinSize;					// 窗口限定最小大小
	CRgn			m_Rgn;						// 不规则窗口区域
	BOOL			m_bTopMost;					// 窗口总在最前面

	CString			m_strTitle;					// 窗口标题
	CFont			m_TitleFont;				// 绘制标题栏的字体
	HICON			m_hIcon;					// 窗口图标句柄
	BOOL			m_bAppWin;					// 是否主窗口(在任务栏会显示出此窗口)

	CDlgPopup		*m_pWndPopup;				// 保存的弹出框指针

	COLORREF		m_clrBK;					// 背景颜色,由背景图片计算出的平均色,当背景图片不够大时可以达到渐变色效果
	CBitmap			m_BKImage;					// 框架背景图片对象
	CSize			m_sizeBKImage;				// 背景图片大小
	CString			m_strFramePicture;			// 背景图片
	CDC				m_MemBKDC;					// 背景dc(由背景图片或背景颜色生成)
	CBitmap			*m_pOldMemBK;
	CBitmap			m_MemBK;
	BOOL			m_bDrawImage;				// 图片或纯色背景
	int				m_nFrameSize;				// 边框大小
	int				m_nFrameWLT;				// 边框左上角宽度(九宫格模式)
	int				m_nFrameHLT;				// 边框左上角高度(九宫格模式)
	int				m_nFrameWRB;				// 边框右下角宽度(九宫格模式)
	int				m_nFrameHRB;				// 边框右下角高度(九宫格模式)

	CString			m_strBkImg;					// 背景图片
	COLORREF		m_crlBack;					// 背景颜色
	COLORREF		m_crlBackTransParent;		// 背景透明颜色
	int				m_nBackTranslucent;			// 背景透明度

	int				m_nFrameTopBottomSpace;		// 鼠标拖动改变窗口大小的区域宽度
	int				m_nFrameLeftRightSpace;		// 鼠标拖动改变窗口大小的区域高度

	int				m_nOverRegioX;				// 过度的大小
	int				m_nOverRegioY;				// 过度的大小
	BOOL			m_bNCActive;

	BOOL			m_bTracking;
	BOOL			m_bIsLButtonDown;	
	BOOL			m_bIsLButtonDblClk;
	BOOL			m_bIsRButtonDown;
	BOOL			m_bIsRButtonDblClk;
	BOOL			m_bIsSetCapture;

	BOOL			m_bAutoClose;				// 窗口自动关闭标志
	BOOL			m_bAutoHide;				// 窗口自动隐藏标志
	UINT			m_uAutoCloseDelayTime;		// 窗口自动关闭的延迟时间

	BOOL			m_bImageUseECM;				// 是否使用图片自身的颜色管理信息

	CToolTipCtrl	m_wndToolTip;				// Tooltip
	int				m_nTooltipCtrlID;			// 当前Tooltip显示的控件ID

	CDuiHandler*	m_pTrayHandler;				// 托盘图标和菜单的事件处理对象
	CString			m_strTrayMenuXml;			// 托盘菜单的XML定义文件

	vector<CControlBase*>	m_vecControl;		// 用户添加的窗口控件
	vector<CControlBase*>	m_vecArea;			// 用户添加的区域(不影响鼠标事件)
	CControlBase*	m_pControl;				// 当前活动控件对象
	CControlBase*	m_pFocusControl;			// 当前焦点的控件对象

	vector<CONTROL_VALUE>	m_vecControlValue;	// 控件预设置信息

	// 窗口阴影
	CWndShadow		m_Shadow;					// 阴影对象
	int				m_nShadowWLT;				// 阴影图片左上角宽度(九宫格模式)
	int				m_nShadowHLT;				// 阴影图片左上角高度(九宫格模式)
	int				m_nShadowWRB;				// 阴影图片右下角宽度(九宫格模式)
	int				m_nShadowHRB;				// 阴影图片右下角高度(九宫格模式)
	int				m_nShadowSize;				// 阴影宽度(算法阴影)

private:
	vector<CControlBase*>	m_vecBaseControl;	// 窗口自身用到的一些默认控件
	vector<CControlBase*>	m_vecBaseArea;		// 窗口默认区域

public:
	CDlgBase(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CDlgBase();

	// 获取窗口模板ID
	UINT GetIDTemplate() { return m_nIDTemplate; }

	void SetMinSize(int iWidth, int iHeight);	// 设置最小窗口大小
	CSize GetMinSize();

	void SetParent(CDuiObject* pParent) { m_pParentDuiObject = pParent; }
	virtual CDuiObject* GetParent() { return m_pParentDuiObject; }

	void TestMainThread();	// 测试是否在主线程

	// 是否使用用户自定义ECM
	BOOL UseImageECM() { return m_bImageUseECM; }

	// 设置窗口的xml文件
	void SetXmlFile(CString strXmlFile) {m_strXmlFile = strXmlFile;}
	// 设置窗口的xml内容
	void SetXmlContent(CString strXmlContent) {m_strXmlContent = strXmlContent;}

	// 设置托盘菜单的事件处理对象
	void SetTrayHandler(CDuiHandler* pDuiHandler) { m_pTrayHandler = pDuiHandler; }
	// 设置托盘菜单xml定义文件
	void SetTrayMenuXml(CString strMenuXml) { m_strTrayMenuXml = strMenuXml; }

	// 获取控件
	CControlBase* GetControl(UINT uControlID);
	CControlBase* GetControl(CString strControlName);
	CControlBase* GetBaseControl(UINT uControlID);
	CControlBase* GetBaseControl(CString strControlName);
	vector<CControlBase*>* GetControls() { return &m_vecControl; }

	// 焦点控件相关函数
	void SetFocusControl(CControlBase* pFocusControl);
	void SetFocusControlPtr(CControlBase* pFocusControl) { m_pFocusControl = pFocusControl; }	// 设置焦点控件指针
	CControlBase* GetFocusControl();
	CControlBase* GetPrevFocusableControl();
	CControlBase* GetNextFocusableControl();

	// 确认和放弃
	void DoOK() { PostMessage(WM_USER_CLOSEWND, IDOK, 0); }
	void DoCancel() { PostMessage(WM_USER_CLOSEWND, IDCANCEL, 0); }
	void DoClose() { PostMessage(WM_USER_CLOSEWND, IDCANCEL, 0); }
	void DoYes() { PostMessage(WM_USER_CLOSEWND, IDYES, 0); }
	void DoNo() { PostMessage(WM_USER_CLOSEWND, IDNO, 0); }

	// 移动控件
	virtual CControlBase* SetControlRect(UINT uControlID, CRect rc);
	virtual CControlBase* SetControlRect(CControlBase *pControlBase, CRect rc);
	// 显示控件
	virtual CControlBase* SetControlVisible(UINT uControlID, BOOL bVisible);
	virtual CControlBase* SetControlVisible(CControlBase *pControlBase, BOOL bVisible);
	// 禁用控件
	virtual CControlBase* SetControlDisable(UINT uControlID, BOOL bDisable);
	virtual CControlBase* SetControlDisable(CControlBase *pControlBase, BOOL bDisable);

	// 设置resize属性
	HRESULT OnAttributeResize(const CString& strValue, BOOL bLoading);

	// 打开弹出对话框
	void OpenDlgPopup(CDlgPopup *pWndPopup, CRect rc, UINT uMessageID, BOOL bShow=TRUE);
	// 关闭弹出对话框
	void CloseDlgPopup();

	// 根据控件名创建控件实例
	CControlBase* _CreateControlByName(LPCTSTR lpszName);

	// 设置不规则窗体区域
	void SetupRegion(int border_offset[], int nSize);
	// 画窗口背景和控件
	void DrawBackgroundAndControls(CDC &dc, const CRect &rcClient, const CRect &rcUpdate);
	
	// 初始化窗口背景皮肤(加载到背景内存dc)
	void InitWindowBkSkin();
	// 加载窗口背景图片
	void LoadBackgroundImage(UINT nIDResource, CString strType = TEXT("PNG"));
	void LoadBackgroundImage(CString strFileName);
	// 设置窗口背景透明度
	void SetupBackTranslucent();
	// 设置不规则窗体区域
	void SetupRegion(int nSize);
	// 画背景图片
	void DrawBackground(CBitmap &bitBackground);
	// 画背景颜色
	void DrawBackground(COLORREF clr);
	// 画控件
	virtual void DrawControl(CDC &dc, const CRect &rcClient);	
	// 重置控件
	virtual void ResetControl();
	// 更新鼠标所在区域
	void UpdateHover();

	// 设置Tooltip
	void SetTooltip(CControlBase* pControl, CString strTooltip, CRect rect, BOOL bControlWidth = FALSE, int nTipWidth = 0);
	// 清除Tooltip
	void ClearTooltip();
	// 设置当前tooltip控件ID
	void SetTooltipCtrlID(int nTooltipCtrlID) { m_nTooltipCtrlID = nTooltipCtrlID; }
	// 获取当前tooltip控件ID
	int  GetTooltipCtrlID() { return m_nTooltipCtrlID; }

	void InitBaseUI(CRect rcClient, DuiXmlNode pNode);
	virtual void InitUI(CRect rcClient, DuiXmlNode pNode);
	virtual void OnSize(CRect rcClient);

	// 设置控件的预制值
	void SetControlValue(CString strName, CString strType, CString strValue);
	void InitDialogValue();
	void InitControlValue();

	// 设置窗口的自动关闭定时器
	void SetAutoCloseTimer(UINT uDelayTime, BOOL bHideWnd = FALSE);

	// 定时器消息
	virtual void OnTimer(UINT uTimerID);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	// 消息响应
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void OnClose();
	virtual void OnMinimize();
	virtual BOOL OnMaximize();
	virtual void OnSkin();

	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd (CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	afx_msg LRESULT OnUserCloseWindow(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageSkin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageUITask(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnSystemTrayIcon(WPARAM wParam, LPARAM lParam);

	// 跨进程通知消息
	LRESULT OnCheckItsMe(WPARAM wParam, LPARAM lParam);

	LRESULT OnMessageButtonMin(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtonMax(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtonClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtonSkin(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();

	DUI_IMAGE_ATTRIBUTE_DEFINE(Shadow);			// 定义阴影图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("appwin"), m_bAppWin, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("title"), m_strTitle, FALSE)
		DUI_INT_ATTRIBUTE(_T("width"), m_MinSize.cx, FALSE)
		DUI_INT_ATTRIBUTE(_T("height"), m_MinSize.cy, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("resize"), OnAttributeResize)
		DUI_TSTRING_ATTRIBUTE(_T("frame"), m_strFramePicture, FALSE)
		DUI_INT_ATTRIBUTE(_T("framesize"), m_nFrameSize, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-lt"), m_nFrameWLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-lt"), m_nFrameHLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-rb"), m_nFrameWRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-rb"), m_nFrameHRB, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("bkimg"), m_strBkImg, FALSE)
		DUI_RGBCOLOR_ATTRIBUTE(_T("crbk"), m_crlBack, FALSE)
		DUI_RGBCOLOR_ATTRIBUTE(_T("crtransparent"), m_crlBackTransParent, FALSE)
		DUI_INT_ATTRIBUTE(_T("img-ecm"), m_bImageUseECM, TRUE)
		DUI_INT_ATTRIBUTE(_T("translucent"), m_nBackTranslucent, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("img-shadow"), OnAttributeImageShadow)
		DUI_INT_ATTRIBUTE(_T("shadow-wlt"), m_nShadowWLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("shadow-hlt"), m_nShadowHLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("shadow-wrb"), m_nShadowWRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("shadow-hrb"), m_nShadowHRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("shadow-size"), m_nShadowSize, FALSE)
		DUI_INT_ATTRIBUTE(_T("topmost"), m_bTopMost, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};

#endif __DLG_BASE_H__

