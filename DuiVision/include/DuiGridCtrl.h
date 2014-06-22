// Panel控件，此控件是一个控件容器
#pragma once

#include "Panel.h"

using namespace  std;

// 列信息
struct GridColumnInfo
{
	CRect	rcHeader;		// 列头位置信息
	CString strTitle;		// 标题
	Color	clrText;		// 文字颜色
	int		nWidth;			// 列宽度
};

// 单元格信息
struct GridItemInfo
{
	CRect	rcItem;			// 位置信息
	CString strTitle;		// 文字
	CString strContent;		// 内容
	int		nImageIndex;	// 图片索引
	Image * pImage;			// 图片对象
	CSize	sizeImage;		// 图片大小
	Color	clrText;		// 文字颜色
	CString strLink;		// 链接的文字
	CString strLinkAction;	// 链接的动作
	BOOL	bNeedTitleTip;	// 是否需要显示title tip(title实际宽度大于显示宽度)
	BOOL	bNeedContentTip;// 是否需要显示content tip(content实际宽度大于显示宽度)
	BOOL	bUseTitleFont;	// 是否使用标题字体显示标题
};

// 行信息
struct GridRowInfo
{
	CRect	rcRow;			// 行位置信息
	CString	strId;			// ID
	int		nCheck;			// 检查框状态(-1表示不显示)
	CRect	rcCheck;		// 检查框位置信息
	int		nImageIndex;	// 图片索引
	Image * pImage;			// 图片对象
	CSize	sizeImage;		// 图片大小
	int		nRightImageIndex;// 右边图片索引
	Image * pRightImage;	// 右边图片对象
	CSize	sizeRightImage;	// 右边图片大小
	BOOL	bRowColor;		// 使用行定义的颜色
	Color	clrText;		// 行文字颜色
	int		nHoverItem;		// 当前热点列
	vector<GridItemInfo> vecItemInfo;
};

class CDuiGridCtrl : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiGridCtrl, "gridctrl")

public:
	CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiGridCtrl(void);

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);

	BOOL InsertColumn(int nColumn, CString strTitle, int nWidth = -1, Color clrText = Color(0, 0, 0, 0));
	BOOL InsertRow(int nItem, CString strId,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""),
		int nRightImageIndex = -1, CString strRightImage = _T(""),
		int nCheck = -1);
	BOOL InsertRow(int nItem, GridRowInfo &rowInfo);
	BOOL SetSubItem(int nRow, int nItem, CString strTitle, CString strContent = _T(""), BOOL bUseTitleFont = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemLink(int nRow, int nItem, CString strLink, CString strLinkAction = _T(""),
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL DeleteRow(int nItem);
	int  GetRowCount() { return m_vecRowInfo.size(); }
	GridRowInfo* GetRowInfo(int nRow);
	GridItemInfo* GetItemInfo(int nRow, int nItem);
	void SetRowColor(int nRow, Color clrText);
	void SetRowCheck(int nRow, int nCheck);
	int  GetRowCheck(int nRow);
	void ClearItems();

	BOOL PtInRow(CPoint point, GridRowInfo& rowInfo);
	BOOL PtInRowCheck(CPoint point, GridRowInfo& rowInfo);
	int  PtInRowItem(CPoint point, GridRowInfo& rowInfo);

	void SetGridTooltip(int nRow, int nItem, CString strTooltip);
	void ClearGridTooltip();

protected:
	vector<GridColumnInfo> m_vecColumnInfo;
	vector<GridRowInfo> m_vecRowInfo;

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);

	HRESULT OnAttributeFontTitle(const CStringA& strValue, BOOL bLoading);

	// 消息响应
	virtual LRESULT OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	CControlBase*		m_pControBkArea;	// 背景Area
	CString				m_strFontTitle;		// 标题字体
	int					m_nFontTitleWidth;	// 标题字体宽度
	FontStyle			m_fontTitleStyle;	// 标题字体Style
	Color				m_clrText;			// 文字颜色
	Color				m_clrTextHover;		// 文字颜色
	Color				m_clrTextDown;		// 文字颜色
	Color				m_clrTitle;			// 标题颜色
	Color				m_clrSeperator;		// 分割线颜色
	int					m_nLeftPos;			// 左侧起始位置
	int					m_nRowHeight;		// 行高度
	int					m_nHeaderHeight;	// 标题行高度
	int					m_nBkTransparent;	// 背景透明度
	BOOL				m_bSingleLine;		// 显示单行文字
	BOOL				m_bTextWrap;		// 文字是否换行

	int					m_nHoverRow;		// 当前鼠标移动的行索引
	int					m_nDownRow;			// 当前点击的行索引
	BOOL				m_bEnableDownRow;	// 允许显示当前点击行

	int					m_nFirstViewRow;	// 当前显示区的第一行的序号
	int					m_nLastViewRow;		// 当前显示区的最后一行的序号
	int					m_nVirtualTop;		// 当前滚动条位置对应的虚拟的top位置

	BOOL				m_bGridTooltip;		// 是否显示单元格的Tooltip
	int					m_nTipRow;			// 当前tip行
	int					m_nTipItem;			// 当前tip列
	int					m_nTipVirtualTop;	// 当前tip行的虚拟Top

	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);	// 定义行分隔线图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(CheckBox);	// 定义检查框图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("img-sep", OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE("img-check", OnAttributeImageCheckBox)
		DUI_CUSTOM_ATTRIBUTE("font-title", OnAttributeFontTitle)
		DUI_COLOR_ATTRIBUTE("crtext", m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE("crhover", m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE("crpush", m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE("crtitle", m_clrTitle, FALSE)
		DUI_COLOR_ATTRIBUTE("crsep", m_clrSeperator, FALSE)
		DUI_INT_ATTRIBUTE("row-height", m_nRowHeight, FALSE)
		DUI_INT_ATTRIBUTE("header-height", m_nHeaderHeight, FALSE)
		DUI_INT_ATTRIBUTE("left-pos", m_nLeftPos, FALSE)
		DUI_INT_ATTRIBUTE("wrap", m_bTextWrap, FALSE)
		DUI_INT_ATTRIBUTE("down-row", m_bEnableDownRow, FALSE)
		DUI_INT_ATTRIBUTE("bk-transparent", m_nBkTransparent, FALSE)
		DUI_INT_ATTRIBUTE("grid-tip", m_bGridTooltip, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
