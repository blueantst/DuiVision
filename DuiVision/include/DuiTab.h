// Tab页控件
#pragma once
#include "ControlBase.h"

// Tab页信息结构定义
struct TabItemInfo
{
	BOOL			bVisible;	// 是否可见
	Image*			pImage;		// 图片
	CSize			sizeImage;	// 图片大小
	int				nImageCount;// 图片个数(可以是1或3,如果是1,则hover图片使用tabctrl设置的)
	int				nImageIndex;// 图片索引,如果pImage为空,则使用此索引到tabctrl中查找图片
	CRect			rc;			// 页面包含的位置
	CString			strText;	// 文字
	CString			strAction;	// 动作属性
	UINT			nItemID;	// ID
	CString			strName;	// 名字
	BOOL			bOutLink;	// 是否外部链接页面
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
	void SetItemVisible(int nItem, BOOL bVisible);
	void SetItemVisible(CString strTabName, BOOL bVisible);
	BOOL GetItemVisible(int nItem);
	BOOL GetItemVisible(CString strTabName);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadTabXml(CString strFileName);
	
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

	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);			// 定义分隔图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(Hover);				// 定义热点图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("img-sep"), OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-hover"), OnAttributeImageHover)
		DUI_INT_ATTRIBUTE(_T("item-width"), m_nTabItemWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-height"), m_nTabCtrlHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("animate"), m_bAnimateChangeTab, FALSE)
		DUI_INT_ATTRIBUTE(_T("animate-count"), m_nAnimateCount, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};