// Panel控件，此控件是一个控件容器
#pragma once

#include "Panel.h"

using namespace  std;

// 列表行信息
struct ListRowInfo
{
	CRect	rcRow;			// 行位置信息
	CString	strId;			// ID
	CString strTitle;		// 标题
	CString strContent;		// 内容
	CString	strTime;		// 时间
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
	CString strLink1;		// 链接1的文字
	CString strLinkAction1;	// 链接1的动作
	CRect	rcLink1;		// 链接1位置信息
	CString strLink2;		// 链接2的文字
	CString strLinkAction2;	// 链接2的动作
	CRect	rcLink2;		// 链接2位置信息
	int		nHoverLink;		// 当前处于热点状态的链接索引
	BOOL	bNeedTitleTip;	// 是否需要显示title tip(title实际宽度大于显示宽度)
	BOOL	bNeedContentTip;// 是否需要显示content tip(content实际宽度大于显示宽度)
};

class CDuiListCtrl : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiListCtrl, "listctrl")

public:
	CDuiListCtrl(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiListCtrl(void);

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);

	BOOL InsertItem(int nItem, CString strId, CString strTitle, CString strContent, CString strTime = _T(""),
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""),
		int nRightImageIndex = -1, CString strRightImage = _T(""),
		CString strLink1 = _T(""), CString strLinkAction1 = _T(""),
		CString strLink2 = _T(""), CString strLinkAction2 = _T(""),
		int nCheck = -1);
	BOOL InsertItem(int nItem, CString strTitle, int nCheck = -1, Color clrText = Color(0, 0, 0, 0), int nImageIndex = -1,
		CString strLink1 = _T(""), CString strLinkAction1 = _T(""),
		CString strLink2 = _T(""), CString strLinkAction2 = _T(""));
	BOOL InsertItem(int nItem, ListRowInfo &rowInfo);
	BOOL DeleteItem(int nItem);
	int  GetItemCount() { return m_vecRowInfo.size(); }
	ListRowInfo* GetItemInfo(int nRow);
	void SetRowColor(int nRow, Color clrText);
	void ClearItems();

	BOOL SetSeperator(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetSeperator(CString strImage = TEXT(""));
	BOOL SetCheckBoxImage(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetCheckBoxImage(CString strImage = TEXT(""));

	BOOL PtInRow(CPoint point, ListRowInfo& rowInfo);
	BOOL PtInRowCheck(CPoint point, ListRowInfo& rowInfo);
	int  PtInRowLink(CPoint point, ListRowInfo& rowInfo);

	void SetRowTooltip(int nRow, CString strTooltip);
	void ClearRowTooltip();

protected:
	vector<ListRowInfo> m_vecRowInfo;

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);

	HRESULT OnAttributeFontTitle(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageSeperator(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageCheckBox(const CStringA& strValue, BOOL bLoading);

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
	int					m_nRowHeight;		// 行高度
	int					m_nBkTransparent;	// 背景透明度
	BOOL				m_bSingleLine;		// 显示单行文字
	BOOL				m_bTextWrap;		// 文字是否换行

	Image*				m_pImageSeperator;	// 行分隔线图片
	CSize				m_sizeSeperator;	// 行分隔线大小
	Image*				m_pImageCheckBox;	// 检查框图片
	CSize				m_sizeCheckBox;		// 检查框图片大小

	int					m_nHoverRow;		// 当前鼠标移动的行索引
	int					m_nDownRow;			// 当前点击的行索引

	int					m_nFirstViewRow;	// 当前显示区的第一行的序号
	int					m_nLastViewRow;		// 当前显示区的最后一行的序号
	int					m_nVirtualTop;		// 当前滚动条位置对应的虚拟的top位置

	BOOL				m_bRowTooltip;		// 是否显示行的Tooltip(title部分)
	int					m_nTipRow;			// 当前tip行
	int					m_nTipVirtualTop;	// 当前tip行的虚拟Top

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
		DUI_INT_ATTRIBUTE("wrap", m_bTextWrap, FALSE)
		DUI_INT_ATTRIBUTE("bk-transparent", m_nBkTransparent, FALSE)
		DUI_INT_ATTRIBUTE("row-tip", m_bRowTooltip, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
