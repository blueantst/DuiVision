// Tab页控件
#pragma once
#include "ControlBase.h"

// Tab页签图片显示模式
enum enumTabImageMode
{
	enTIMNormal = 0,			// 正常
	enTIMExtrude,				// 拉伸
	enTIMMID					// 九宫格边框
};

// Tab页信息结构定义
struct TabItemInfo
{
	BOOL			bVisible;	// tab页签是否可见
	Image*			pImage;		// tab页签图片
	CSize			sizeImage;	// tab页签图片大小
	int				nImageCount;// tab页签图片个数(可以是1或3,如果是1,则hover图片使用tabctrl设置的)
	int				nImageIndex;// tab页签图片索引,如果pImage为空,则使用此索引到tabctrl中查找图片
	CRect			rc;			// tab页签部分的位置
	CString			strText;	// tab页签文字
	CString			strAction;	// 动作属性
	UINT			nItemID;	// ID
	CString			strName;	// 名字
	BOOL			bOutLink;	// 是否外部链接页面
	CRect			rcButton;	// tab页签的按钮区域
	enumButtonState	buttonState;// Tab页签按钮的状态
	CControlBase*	pControl;	// Tab页对应的子页面容器控件
};


class CDuiTabCtrl : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiTabCtrl, _T("tabctrl"))
public:
	CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject);
	CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle= TEXT(""), BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual~CDuiTabCtrl(void);

	BOOL InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, UINT nResourceID, CControlBase* pControl, int nImageCount = -1, BOOL bOutLink = FALSE, int nItemWidth = 0, CString strType= TEXT("PNG"));
	BOOL InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, CString strImage, CControlBase* pControl, int nImageCount = -1, BOOL bOutLink = FALSE, int nItemWidth = 0);
	BOOL InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, int nImageIndex, CControlBase* pControl, BOOL bOutLink = FALSE, int nItemWidth = 0);

	int  GetItemIndex(CString strTabName);
	TabItemInfo* GetItemInfo(int nItem);
	void RefreshItems();
	void DeleteItem(int nItem);
	void DeleteItem(CString strTabName);
	int  SetSelectItem(int nItem);
	int  GetSelectItem();
	void SetItemVisible(int nItem, BOOL bVisible);
	void SetItemVisible(CString strTabName, BOOL bVisible);
	BOOL GetItemVisible(int nItem);
	BOOL GetItemVisible(CString strTabName);

	virtual void SetControlRect(CRect rc);
	virtual void SetControlVisible(BOOL bIsVisible);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadTabXml(CString strFileName);

	BOOL PtInTabButton(CPoint point, TabItemInfo& itemInfo);

	HRESULT OnAttributeTabBtnPosChange(const CString& strValue, BOOL bLoading);
	
protected:
	// 根据控件名创建控件实例
	CControlBase* _CreateControlByName(LPCTSTR lpszName);

	virtual BOOL OnCheckMouseResponse(UINT nFlags, CPoint point);
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	virtual	BOOL OnControlTimer();
	virtual BOOL DrawSubControls(CDC &dc, CRect rcUpdate);

	BOOL InsertItem(int nItem, TabItemInfo &itemInfo);
	
public:
	vector<TabItemInfo>		m_vecItemInfo;			// Tab页信息列表
	vector<CRect>			m_vecRcSeperator;		// tab位置列表

	Color					m_clrText;				// 文字颜色

	int						m_nTabItemWidth;		// 每个Tab页的宽度
	int						m_nTabCtrlHeight;		// TabCtrl部分的高度

	BOOL					m_bInit;				// 是否初始化完成

	int						m_nHoverItem;			// 当前显示的页面索引
	int						m_nDownItem;			// 当前点击的页面索引
	BOOL					m_bAnimateChangeTab;	// 是否支持切换tab页的动画
	int						m_nOldItem;				// 切换前的页面索引
	int						m_nAnimateCount;		// 切换动画的帧数
	int						m_nCurXPos;				// 切换过程中当前的横坐标位置
	int						m_nTabLeftPading;		// Tab页签左侧的空白宽度
	DUIDLG_POSITION			m_posTabBtn;			// Tab页签的内部按钮位置信息

	enumTabImageMode		m_enTabImageMode;		// Tab页签图片的显示模式(普通、拉伸、九宫格)
	int						m_nWLT;					// 左上角宽度(Tab页签图片的九宫格模式)
	int						m_nHLT;					// 左上角高度(Tab页签图片的九宫格模式)
	int						m_nWRB;					// 右下角宽度(Tab页签图片的九宫格模式)
	int						m_nHRB;					// 右下角高度(Tab页签图片的九宫格模式)

	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);			// 定义分隔图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(Hover);				// 定义热点图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(TabBtn);				// 定义tab页签按钮图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("img-sep"), OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-hover"), OnAttributeImageHover)
		DUI_CUSTOM_ATTRIBUTE(_T("img-tabbtn"), OnAttributeImageTabBtn)
		DUI_CUSTOM_ATTRIBUTE(_T("tabbtnpos"), OnAttributeTabBtnPosChange)
		DUI_INT_ATTRIBUTE(_T("item-width"), m_nTabItemWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-height"), m_nTabCtrlHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-left-pading"), m_nTabLeftPading, FALSE)
		DUI_INT_ATTRIBUTE(_T("animate"), m_bAnimateChangeTab, FALSE)
		DUI_INT_ATTRIBUTE(_T("animate-count"), m_nAnimateCount, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-lt"), m_nWLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-lt"), m_nHLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-rb"), m_nWRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-rb"), m_nHRB, FALSE)
		DUI_ENUM_ATTRIBUTE(_T("tabimg-mode"), enumTabImageMode, TRUE)
            DUI_ENUM_VALUE(_T("normal"), enTIMNormal)
            DUI_ENUM_VALUE(_T("extrude"), enTIMExtrude)
			DUI_ENUM_VALUE(_T("mid"), enTIMMID)
        DUI_ENUM_END(m_enTabImageMode)
    DUI_DECLARE_ATTRIBUTES_END()
};