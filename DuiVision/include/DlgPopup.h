#pragma once

#include <vector>
#include "DuiVision.h"

using namespace  std;

// 消息定义
#define	MSG_CLOSE	1
#define	MSG_USER	2

// 背景模式
enum enumBackMode
{
	enBMImage = 0,			// 正常
	enBMFrame,				// 边框
	enBMMID					// 九宫格模式的边框
};

// CDlgPopup

class CDlgPopup : public CWnd, public CTimer, public CDuiObject
{
public:
	static LPCSTR GetClassName() { return "popup";}
	virtual BOOL IsClass(LPCSTR lpszName)
	{
		if(strcmp(GetClassName(), lpszName)  == 0) return TRUE;
		return __super::IsClass(lpszName);
	}

public:
	BOOL			m_bInit;
	BOOL			m_bTracking;
	BOOL			m_bIsLButtonDown;	
	BOOL			m_bIsLButtonDblClk;
	BOOL			m_bIsSetCapture;

protected:
	CDuiObject*		m_pParentDuiObject;			// 父控件对象

	Image*			m_pImage;
	CSize			m_sizeBKImage;
	CPoint			m_point;
	CSize			m_size;
	UINT			m_uMessageID;

	enumBackMode	m_enBackMode;				// 背景模式
	BOOL			m_bBackAlpha;				// 背景是否使用Alpha通道
	int				m_nBackTranslucent;			// 背景透明度
	int				m_nFrameSize;				// 边框大小
	int				m_nFrameWLT;				// 边框左上角宽度(九宫格模式)
	int				m_nFrameHLT;				// 边框左上角高度(九宫格模式)
	int				m_nFrameWRB;				// 边框右下角宽度(九宫格模式)
	int				m_nFrameHRB;				// 边框右下角高度(九宫格模式)
	BOOL			m_bInitFinish;
	BOOL			m_bAutoClose;				// 是否自动关闭

	BOOL			m_bImageUseECM;				// 是否使用图片自身的颜色管理信息

	BOOL			m_bNCActive;
	vector<CControlBase*>	m_vecControl;		// 窗口控件
	vector<CControlBase*>	m_vecArea;			// 域(不影响鼠标事件)
	CControlBase*	m_pControl;
	CControlBase*	m_pFocusControl;

	UINT			m_uTimerAnimation;			// 动画定时器

	CString			m_strXmlFile;				// XML文件名
	CString			m_strXmlContent;			// XML内容

	DECLARE_DYNAMIC(CDlgPopup)

public:
	CDlgPopup();
	virtual ~CDlgPopup();

	void SetParent(CDuiObject* pParent) { m_pParentDuiObject = pParent; }
	virtual CDuiObject* GetParent() { return m_pParentDuiObject; }

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadXmlFile(CString strFileName);
	BOOL LoadXmlContent(CString strXmlContent);
	CString GetXmlFile() { return m_strXmlFile; }

	void SetAutoClose(BOOL bAutoClose) { m_bAutoClose = bAutoClose; }
	BOOL IsAutoClose() { return m_bAutoClose; }

	BOOL UseImageECM() { return m_bImageUseECM; }

	virtual void InitUI(CRect rcClient, TiXmlElement* pNode);

	void SetBackMode(enumBackMode enBackMode);
	void SetFrameSize(int nFrameSize);
	void SetBackBitmap(UINT nResourceID);
	void SetBackBitmap(CString strImage);
	BOOL Create(CWnd *pParent, CRect rc, UINT uMessageID, UINT nResourceID, enumBackMode enBackMode = enBMFrame, int nFrameSize = 4);
	BOOL Create(CWnd *pParent, CRect rc, UINT uMessageID, CString strImage, enumBackMode enBackMode = enBMFrame, int nFrameSize = 4);
	BOOL Create(CWnd *pParent, CRect rc, UINT uMessageID);
	void CloseWindow();

	// 根据控件名创建控件实例
	CControlBase* _CreateControlByName(LPCSTR lpszName);

	void DrawWindow();	
	void DrawWindow(CDC &dc);
	void DrawWindow(CDC *pDC);
	virtual void DrawWindow(CDC &dc, CRect rcClient){};
	virtual void DrawWindowEx(CDC &dc, CRect rcClient){};
	virtual void InitUI(CRect rcClient) {};
	virtual BOOL OnMouseMove(CPoint point){ return false;};
	virtual BOOL OnLButtonDown(CPoint point){ return false;};
	virtual BOOL OnLButtonUp(CPoint point){ return false;};

	HRESULT OnAttributeBkMode(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeBkImage(const CStringA& strValue, BOOL bLoading);

	// 定时器消息
	virtual void OnTimer(UINT uTimerID);

	// 消息响应
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnNcCloseWindow();

	// 发送通知消息
	BOOL SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam);
	BOOL PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam);
	BOOL SendMessageToParent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	CControlBase *GetControl(UINT uControlID);
	CControlBase *GetControl(CString strControlName);

	vector<CControlBase*>* GetControls() { return &m_vecControl; }

	// 移动控件
	virtual CControlBase * SetControlRect(UINT uControlID, CRect rc);
	// 移动控件
	virtual CControlBase * SetControlRect(CControlBase *pControlBase, CRect rc);
	// 显示控件
	virtual CControlBase * SetControlVisible(UINT uControlID, BOOL bVisible);
	// 显示控件
	virtual CControlBase * SetControlVisible(CControlBase *pControlBase, BOOL bVisible);
	// 禁用控件
	virtual CControlBase * SetControlDisable(UINT uControlID, BOOL bDisable);
	// 禁用控件
	virtual CControlBase * SetControlDisable(CControlBase *pControlBase, BOOL bDisable);
	// 更新选中
	void UpdateHover();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg BOOL OnEraseBkgnd (CDC* pDC);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();()
public:
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("bkmode", OnAttributeBkMode)
		DUI_CUSTOM_ATTRIBUTE("bkimg", OnAttributeBkImage)
		DUI_INT_ATTRIBUTE("bkalpha", m_bBackAlpha, TRUE)
		DUI_INT_ATTRIBUTE("translucent", m_nBackTranslucent, TRUE)
		DUI_INT_ATTRIBUTE("width", m_size.cx, FALSE)
		DUI_INT_ATTRIBUTE("height", m_size.cy, FALSE)
		DUI_INT_ATTRIBUTE("framesize", m_nFrameSize, FALSE)
		DUI_INT_ATTRIBUTE("width-lt", m_nFrameWLT, FALSE)
		DUI_INT_ATTRIBUTE("height-lt", m_nFrameHLT, FALSE)
		DUI_INT_ATTRIBUTE("width-rb", m_nFrameWRB, FALSE)
		DUI_INT_ATTRIBUTE("height-rb", m_nFrameHRB, FALSE)
		DUI_INT_ATTRIBUTE("img-ecm", m_bImageUseECM, TRUE)
		DUI_INT_ATTRIBUTE("autoclose", m_bAutoClose, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};


