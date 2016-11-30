// 表格控件
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
	UINT	uAlignment;		// 水平对齐方式
	UINT	uVAlignment;	// 垂直对齐方式
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
	vector<CControlBase *>	vecControl;// 控件列表
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
	BOOL	bRowColor;		// 使用行定义的文字颜色
	Color	clrText;		// 行文字颜色
	BOOL	bRowBackColor;	// 使用行定义的背景颜色
	Color	clrBack;		// 行背景颜色
	int		nHoverItem;		// 当前热点列
	vector<GridItemInfo> vecItemInfo;
};

// 自定义表格排序函数的定义(比较两个参数的大小,小于0表示参数1小于参数2,大于0表示参数1大于参数2)
typedef int (CALLBACK *PFN_GRIDCTRL_COMPARE)(GridItemInfo* pItem1, GridItemInfo* pItem2);

class CDuiGridCtrl : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiGridCtrl, _T("gridctrl"))

public:
	CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiGridCtrl(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

	BOOL InsertColumn(int nColumn, CString strTitle, int nWidth = -1, Color clrText = Color(0, 0, 0, 0),
		UINT uAlignment = 0xFFFFUL, UINT uVAlignment = 0xFFFFUL);
	int GetColumnCount() { return (int)m_vecColumnInfo.size(); }
	int SetColumnWidth(int nColumn, int nWidth, int nWidthNextColumn = -1);
	void MoveColumnSplit(int nColumn, int nPos);
	int GetTotalColumnWidth();
	int InsertRow(int nRow, CString strId,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""),
		int nRightImageIndex = -1, CString strRightImage = _T(""),
		int nCheck = -1, Color clrBack = Color(0, 0, 0, 0));
	int InsertRow(int nRow, GridRowInfo &rowInfo);
	BOOL SetSubItem(int nRow, int nItem, CString strTitle, CString strContent = _T(""), BOOL bUseTitleFont = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemLink(int nRow, int nItem, CString strLink, CString strLinkAction = _T(""),
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL AddSubItemControl(int nRow, int nItem, CControlBase* pControl);
	BOOL DeleteSubItemControl(CControlBase* pControl);
	BOOL DeleteSubItemControl(CString strControlName, UINT uControlID = ID_NULL);
	BOOL DeleteRow(int nRow);
	void CalcRowsPos();
	void CalcColumnsPos();
	BOOL EnsureVisible(int nRow, BOOL bPartialOK);
	int  GetRowCount() { return m_vecRowInfo.size(); }
	GridRowInfo* GetRowInfo(int nRow);
	GridItemInfo* GetItemInfo(int nRow, int nItem);
	CString GetItemText(int nRow, int nItem);
	void SetRowColor(int nRow, Color clrText);
	void SetRowBackColor(int nRow, Color clrBack);
	void SetRowCheck(int nRow, int nCheck);
	int  GetRowCheck(int nRow);
	void ClearItems();

	BOOL PtInRow(CPoint point, GridRowInfo& rowInfo);
	BOOL PtInRowCheck(CPoint point, GridRowInfo& rowInfo);
	int  PtInRowItem(CPoint point, GridRowInfo& rowInfo);

	void SetGridTooltip(int nRow, int nItem, CString strTooltip);
	void ClearGridTooltip();

	// 表格排序
	void SetHeaderSort(BOOL bSortOnClick = TRUE)  { m_bSortOnClick = bSortOnClick;    }
    BOOL GetHeaderSort() const                    { return m_bSortOnClick;            }
	void SetSortColumn(int nCol);
    int  GetSortColumn() const                    { return m_nSortColumn;             }
    void SetSortAscending(BOOL bAscending)        { m_bAscending = bAscending;        }
    BOOL GetSortAscending() const                 { return m_bAscending;              }

	BOOL SortTextItems(int nCol, BOOL bAscending);
	BOOL SortTextItems(int nCol, BOOL bAscending, int low, int high);
    BOOL SortItems(PFN_GRIDCTRL_COMPARE pfnCompare, int nCol, BOOL bAscending);
	BOOL SortItems(PFN_GRIDCTRL_COMPARE pfnCompare, int nCol, BOOL bAscending, int low, int high);
	void SetSortCompareFunction(PFN_GRIDCTRL_COMPARE pfnCompare) { m_pfnCompare = pfnCompare; }

protected:
	vector<GridColumnInfo> m_vecColumnInfo;
	vector<GridRowInfo> m_vecRowInfo;

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	HRESULT OnAttributeFontTitle(const CString& strValue, BOOL bLoading);

	// 消息响应
	virtual LRESULT OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	CString				m_strFontTitle;		// 标题字体
	int					m_nFontTitleWidth;	// 标题字体宽度
	FontStyle			m_fontTitleStyle;	// 标题字体Style
	UINT				m_uAlignmentHeader;	// 标题文字水平对齐方式
	UINT				m_uVAlignmentHeader;// 标题文字垂直对齐方式
	Color				m_clrHeader;		// 标题行文字颜色
	Color				m_clrText;			// 文字颜色
	Color				m_clrTextHover;		// 文字颜色(鼠标移动)
	Color				m_clrTextDown;		// 文字颜色(鼠标按下)
	Color				m_clrTitle;			// 标题颜色
	Color				m_clrSeperator;		// 分割线颜色
	Color				m_clrRowHover;		// 行背景颜色(鼠标移动到行)
	Color				m_clrRowCurrent;	// 行背景颜色(当前行)
	int					m_nLeftPos;			// 左侧起始位置
	int					m_nRowHeight;		// 行高度
	int					m_nHeaderHeight;	// 标题行高度
	int					m_nBkTransparent;	// 背景透明度
	BOOL				m_bSingleLine;		// 显示单行文字
	BOOL				m_bTextWrap;		// 文字是否换行
	BOOL				m_bShowColumnSeperator;	// 是否显示内容部分的列分隔线

	int					m_nHoverRow;		// 当前鼠标移动的行索引
	int					m_nDownRow;			// 当前点击的行索引
	BOOL				m_bEnableDownRow;	// 允许显示当前点击行

	int					m_nFirstViewRow;	// 当前显示区的第一行的序号
	int					m_nLastViewRow;		// 当前显示区的最后一行的序号
	int					m_nVirtualTop;		// 当前滚动条位置对应的虚拟的top位置
	int					m_nVirtualLeft;		// 当前滚动条位置对应的虚拟的left位置

	BOOL				m_bGridTooltip;		// 是否显示单元格的Tooltip
	int					m_nTipRow;			// 当前tip行
	int					m_nTipItem;			// 当前tip列
	int					m_nTipVirtualTop;	// 当前tip行的虚拟Top

	BOOL				m_bEnableModifyColumn;	// 是否允许修改列宽度
	enumButtonState		m_enButtonState;	// 鼠标状态
	BOOL				m_bHoverSplitColumn;	// 是否鼠标热点状态(列分隔线)
	int					m_nHoverSplitColumn;	// 鼠标拖动的列分隔线索引

	// sorting
	BOOL				m_bSortOnClick;		// 点击列标题时,是否进行排序
    int					m_bAscending;		// 是否升序排序
    int					m_nSortColumn;		// 排序的列
	PFN_GRIDCTRL_COMPARE m_pfnCompare;		// 自定义表格排序函数

	DUI_IMAGE_ATTRIBUTE_DEFINE(Header);		// 定义标题行背景图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(HeaderSort);		// 定义标题行排序状态图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(ColumnSeperator);	// 定义列分隔线图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);	// 定义行分隔线图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(CheckBox);	// 定义检查框图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_ENUM_ATTRIBUTE(_T("valign-header"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("top"), VAlign_Top)
            DUI_ENUM_VALUE(_T("middle"), VAlign_Middle)
            DUI_ENUM_VALUE(_T("bottom"), VAlign_Bottom)
        DUI_ENUM_END(m_uVAlignmentHeader)
        DUI_ENUM_ATTRIBUTE(_T("align-header"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("left"), Align_Left)
            DUI_ENUM_VALUE(_T("center"), Align_Center)
            DUI_ENUM_VALUE(_T("right"), Align_Right)
        DUI_ENUM_END(m_uAlignmentHeader)
		DUI_CUSTOM_ATTRIBUTE(_T("img-header"), OnAttributeImageHeader)
		DUI_CUSTOM_ATTRIBUTE(_T("img-header-sort"), OnAttributeImageHeaderSort)
		DUI_CUSTOM_ATTRIBUTE(_T("img-colsep"), OnAttributeImageColumnSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-sep"), OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-check"), OnAttributeImageCheckBox)
		DUI_CUSTOM_ATTRIBUTE(_T("font-title"), OnAttributeFontTitle)
		DUI_COLOR_ATTRIBUTE(_T("crheader"), m_clrHeader, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crhover"), m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crpush"), m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtitle"), m_clrTitle, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crsep"), m_clrSeperator, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crrowhover"), m_clrRowHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crrowcurrent"), m_clrRowCurrent, FALSE)
		DUI_INT_ATTRIBUTE(_T("row-height"), m_nRowHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("header-height"), m_nHeaderHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("left-pos"), m_nLeftPos, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("wrap"), m_bTextWrap, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("down-row"), m_bEnableDownRow, FALSE)
		DUI_INT_ATTRIBUTE(_T("bk-transparent"), m_nBkTransparent, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("grid-tip"), m_bGridTooltip, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("column-sep"), m_bShowColumnSeperator, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("modify-column-width"), m_bEnableModifyColumn, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("sort-click"), m_bSortOnClick, TRUE)
    DUI_DECLARE_ATTRIBUTES_END()
};
