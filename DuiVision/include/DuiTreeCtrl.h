// 树控件
#pragma once

#include "Panel.h"

using namespace  std;

// 列信息
struct TreeColumnInfo
{
	CRect	rcHeader;		// 列头位置信息
	CString strTitle;		// 标题
	Color	clrText;		// 文字颜色
	int		nWidth;			// 列宽度
};

// 单元格信息
struct TreeItemInfo
{
	CRect	rcItem;			// 位置信息
	CString strTitle;		// 文字
	CString strContent;		// 内容
	int		nImageIndex;	// 图片索引
	Image * pImage;			// 图片对象
	CSize	sizeImage;		// 图片大小
	CSize	sizeImageDpi;	// 图片大小(DPI适配的)
	int		nImageCount;	// 图片个数
	Color	clrText;		// 文字颜色
	CString strLink;		// 链接的文字
	CString strLinkAction;	// 链接的动作
	BOOL	bNeedTitleTip;	// 是否需要显示title tip(title实际宽度大于显示宽度)
	BOOL	bNeedContentTip;// 是否需要显示content tip(content实际宽度大于显示宽度)
	BOOL	bUseTitleFont;	// 是否使用标题字体显示标题
	BOOL	bShowCollapse;	// 是否在此单元格显示节点缩放图片
	vector<CControlBase *>	vecControl;// 控件列表
};

#define HDUITREEITEM	int

// 树节点信息
struct TreeNodeInfo
{
	HDUITREEITEM hParentNode;	// 父节点句柄
	HDUITREEITEM hNode;		// 节点句柄
	CRect	rcRow;			// 行位置信息
	CString	strId;			// ID
	int		nCheck;			// 检查框状态(-1表示不显示)
	CRect	rcCheck;		// 检查框位置信息
	int		nImageIndex;	// 图片索引
	Image * pImage;			// 图片对象
	CSize	sizeImage;		// 图片大小
	CSize	sizeImageDpi;	// 图片大小(DPI适配的)
	int		nRightImageIndex;// 右边图片索引
	Image * pRightImage;	// 右边图片对象
	CSize	sizeRightImage;	// 右边图片大小
	CSize	sizeRightImageDpi;	// 右边图片大小(DPI适配的)
	BOOL	bRowColor;		// 使用行定义的文字颜色
	Color	clrText;		// 行文字颜色
	BOOL	bRowBackColor;		// 使用行定义的背景颜色
	Color	clrBack;		// 行背景颜色
	int		nHoverItem;		// 当前热点列
	BOOL	bCollapse;		// 是否折叠
	BOOL	bHide;			// 是否隐藏
	DWORD   dwData;     // 关联用户数据
	vector<TreeItemInfo> vecItemInfo;
};

class CDuiTreeCtrl : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiTreeCtrl, _T("treectrl"))

public:
	CDuiTreeCtrl(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiTreeCtrl(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadNode(HDUITREEITEM hParentNode, DuiXmlNode pXmlElem);

	BOOL InsertColumn(int nColumn, CString strTitle, int nWidth = -1, Color clrText = Color(0, 0, 0, 0));
	int GetColumnCount() { return (int)m_vecColumnInfo.size(); }
	int SetColumnWidth(int nColumn, int nWidth, int nWidthNextColumn = -1);
	void MoveColumnSplit(int nColumn, int nPos);
	int GetTotalColumnWidth();
	void CalcColumnsPos();
	HDUITREEITEM InsertNode(HDUITREEITEM hParentNode, CString strId, CString strTitle, BOOL bCollapse = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""),
		int nRightImageIndex = -1, CString strRightImage = _T(""),
		int nCheck = -1, Color clrBack = Color(0, 0, 0, 0));
	HDUITREEITEM InsertNode(HDUITREEITEM hParentNode, TreeNodeInfo &nodeInfo);
	BOOL SetSubItem(HDUITREEITEM hNode, int nItem, CString strTitle, CString strContent = _T(""), BOOL bUseTitleFont = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemLink(HDUITREEITEM hNode, int nItem, CString strLink, CString strLinkAction = _T(""),
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemCollapse(HDUITREEITEM hNode, int nItem, CString strImage = _T(""), int nImageCount = 0);
	BOOL AddSubItemControl(HDUITREEITEM hNode, int nItem, CControlBase* pControl);
	BOOL DeleteSubItemControl(CControlBase* pControl);
	BOOL DeleteSubItemControl(CString strControlName, UINT uControlID = ID_NULL);
	BOOL DeleteNode(HDUITREEITEM hNode);
	int  GetNodeCount() { return m_vecRowInfo.size(); }
	int  GetNodeRow(HDUITREEITEM hNode);
	int  GetNodeRowById(CString strNodeId);
	int	 GetNodeLastChildRow(HDUITREEITEM hNode);
	HDUITREEITEM  GetCurrentNode();
	BOOL HaveChildNode(HDUITREEITEM hNode);
	HDUITREEITEM GetParentNode(HDUITREEITEM hNode);
	HDUITREEITEM GetChildNode(HDUITREEITEM hNode);
	HDUITREEITEM GetNextSiblingNode(HDUITREEITEM hNode);
	HDUITREEITEM GetPrevSiblingNode(HDUITREEITEM hNode);
	int  GetChildNodeCount(HDUITREEITEM hNode);
	int  GetNodeLevel(HDUITREEITEM hNode);
	HDUITREEITEM GetNodeById(CString strId);
	TreeNodeInfo* GetNodeInfo(HDUITREEITEM hNode);
	TreeItemInfo* GetItemInfo(HDUITREEITEM hNode, int nItem);
	void SetItemInfo(HDUITREEITEM hNode, int nItem, TreeItemInfo* pItemInfo);
	void SetNodeColor(HDUITREEITEM hNode, Color clrText);
	void SetNodeBackColor(HDUITREEITEM hNode, Color clrBack);
	void ToggleNode(HDUITREEITEM hNode);
	void ExpandNode(HDUITREEITEM hNode, BOOL bExpand);
	void SetNodeCheck(HDUITREEITEM hNode, int nCheck);
	int  GetNodeCheck(HDUITREEITEM hNode);
	void SetNodeData(HDUITREEITEM hNode, DWORD dwData);
	DWORD GetNodeData(HDUITREEITEM hNode);
	void ClearNodes();
	void HideChildNodes(HDUITREEITEM hItem);
	void RefreshNodeRows();
	BOOL EnsureVisible(HDUITREEITEM hNode, BOOL bPartialOK);

	BOOL PtInRow(CPoint point, TreeNodeInfo& rowInfo);
	BOOL PtInRowCheck(CPoint point, TreeNodeInfo& rowInfo);
	BOOL PtInRowCollapse(CPoint point, TreeNodeInfo& rowInfo);
	int  PtInRowItem(CPoint point, TreeNodeInfo& rowInfo);

	void SetGridTooltip(HDUITREEITEM hNode, int nItem, CString strTooltip);
	void ClearGridTooltip();

protected:
	vector<TreeColumnInfo> m_vecColumnInfo;
	vector<TreeNodeInfo> m_vecRowInfo;

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// 消息响应
	virtual LRESULT OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	HRESULT OnAttributeFontTitle(const CString& strValue, BOOL bLoading);

public:
	CString				m_strFontTitle;		// 标题字体
	int					m_nFontTitleWidth;	// 标题字体宽度
	FontStyle			m_fontTitleStyle;	// 标题字体Style
	Color				m_clrText;			// 文字颜色
	Color				m_clrTextHover;		// 文字颜色(鼠标移动)
	Color				m_clrTextDown;		// 文字颜色(鼠标按下)
	Color				m_clrTitle;			// 标题颜色
	Color				m_clrSeperator;		// 分割线颜色
	Color				m_clrRowHover;		// 行背景颜色(鼠标移动到行)
	Color				m_clrRowCurrent;	// 行背景颜色(当前行)
	int					m_nLeftPos;			// 左侧起始位置
	int					m_nRowHeight;		// 行高度
	int					m_nBkTransparent;	// 背景透明度
	BOOL				m_bSingleLine;		// 显示单行文字
	BOOL				m_bTextWrap;		// 文字是否换行

	int					m_nHoverRow;		// 当前鼠标移动的行索引
	int					m_nDownRow;			// 当前点击的行索引
	BOOL				m_bEnableDownRow;	// 允许显示当前点击行

	int					m_nFirstViewRow;	// 当前显示区的第一行的序号
	int					m_nVirtualTop;		// 当前滚动条位置对应的虚拟的top位置
	int					m_nVirtualLeft;		// 当前滚动条位置对应的虚拟的left位置
	int					m_nVisibleRowCount;	// 当前可显示的行数(非折叠行)

	BOOL				m_bGridTooltip;		// 是否显示单元格的Tooltip
	int					m_nTipNode;			// 当前tip节点
	int					m_nTipItem;			// 当前tip列
	int					m_nTipVirtualTop;	// 当前tip行的虚拟Top

	HDUITREEITEM			m_nNodeIndex;		// 句柄递增变量

	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);	// 定义行分隔线图片
	DUI_IMAGE_ATTRIBUTE_DEFINE_DPI(CheckBox);	// 定义检查框图片
	DUI_IMAGE_ATTRIBUTE_DEFINE_DPI(Collapse);	// 定义行缩放图片
	DUI_IMAGE_ATTRIBUTE_DEFINE_DPI(Toggle);		// 定义树节点收缩图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("img-sep"), OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-check"), OnAttributeImageCheckBox)
		DUI_CUSTOM_ATTRIBUTE(_T("img-collapse"), OnAttributeImageCollapse)
		DUI_CUSTOM_ATTRIBUTE(_T("img-toggle"), OnAttributeImageToggle)
		DUI_CUSTOM_ATTRIBUTE(_T("font-title"), OnAttributeFontTitle)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crhover"), m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crpush"), m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtitle"), m_clrTitle, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crsep"), m_clrSeperator, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crrowhover"), m_clrRowHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crrowcurrent"), m_clrRowCurrent, FALSE)
		DUI_INT_ATTRIBUTE_DPI(_T("row-height"), m_nRowHeight, FALSE)
		DUI_INT_ATTRIBUTE_DPI(_T("left-pos"), m_nLeftPos, FALSE)
		DUI_INT_ATTRIBUTE(_T("wrap"), m_bTextWrap, FALSE)
		DUI_INT_ATTRIBUTE(_T("down-row"), m_bEnableDownRow, FALSE)
		DUI_INT_ATTRIBUTE(_T("bk-transparent"), m_nBkTransparent, FALSE)
		DUI_INT_ATTRIBUTE(_T("grid-tip"), m_bGridTooltip, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
