#pragma once
#include "ControlBase.h"
#include "Panel.h"

// Layout模式
enum LAYOUT_TYPE
{
	LAYOUT_TYPE_HORIZONTAL = 0,			// 水平方向
	LAYOUT_TYPE_VERTICAL,				// 垂直方向
};

// Layout信息结构定义
struct LayoutItemInfo
{
	BOOL			bVisible;	// div是否可见
	DUI_POSITION_ITEM	pos;	// 位置信息
	int					nMinPos;	// 位置最小值
	int					nMaxPos;	// 位置最大值
	int					nPos;			// 实际的位置值
	CDuiPanel*		pControlPanel;	// Layout对应的div容器控件
	CRect			rcSplit;			// 分割线区域
	CRect			rcThumb;		// 滑块区域
};

class CDuiLayout : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiLayout, _T("layout"))
public:
	CDuiLayout(HWND hWnd, CDuiObject* pDuiObject);
	CDuiLayout(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress = 0,
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiLayout(void);

	LayoutItemInfo* GetLayoutItemInfo(int nItem);
	int GetLayoutPos(LayoutItemInfo* pLayoutInfo);
	int SetSplitPos(int nSplitItem, int nSplitPos);

	int MoveSplitPos(int nSplitItem, CPoint point);
	
	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	virtual void SetControlRect(CRect rc);

	virtual void SetControlVisible(BOOL bIsVisible);
	virtual void SetControlWndVisible(BOOL bIsVisible);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	
public:
	LAYOUT_TYPE		m_nLayoutType;			// Layout类型
	vector<LayoutItemInfo>		m_vecItemInfo;	// Layout信息列表
	BOOL					m_bInit;				// 是否初始化完成
	enumButtonState m_enButtonState;	// 鼠标状态
	BOOL			m_bHover;			// 是否鼠标热点状态
	int					m_nHoverSplit;	// 鼠标拖动的分隔线索引
	int				m_nSplitWidth;		// 分隔线宽度
	BOOL		m_bSplitImm;			// 鼠标移动分割线是否立即生效
	//int				m_nThumbWidth;		// 滑块宽度
	//int				m_nThumbHeight;		// 滑块高度
	//int				m_nThumbPos;		// 滑块顶端位置

	DUI_IMAGE_ATTRIBUTE_DEFINE(Split);	// 定义Split图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(Thumb);	// 定义前景图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_ENUM_ATTRIBUTE(_T("layout-type"), LAYOUT_TYPE, TRUE)
            DUI_ENUM_VALUE(_T("horizontal"), LAYOUT_TYPE_HORIZONTAL)
            DUI_ENUM_VALUE(_T("vertical"), LAYOUT_TYPE_VERTICAL)
        DUI_ENUM_END(m_nLayoutType)
		DUI_CUSTOM_ATTRIBUTE(_T("img-split"), OnAttributeImageSplit)
		DUI_CUSTOM_ATTRIBUTE(_T("img-thumb"), OnAttributeImageThumb)
		DUI_INT_ATTRIBUTE(_T("split-width"), m_nSplitWidth, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("split-imm"), m_bSplitImm, TRUE)
		//DUI_INT_ATTRIBUTE(_T("thumb-width"), m_nThumbWidth, FALSE)
		//DUI_INT_ATTRIBUTE(_T("thumb-height"), m_nThumbHeight, FALSE)
		//DUI_INT_ATTRIBUTE(_T("thumb-pos"), m_nThumbPos, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};