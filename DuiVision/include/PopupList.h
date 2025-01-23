#pragma once
#include "DlgPopup.h"

#include <vector>
using namespace std;

// 文字显示模式
enum enumPopupTextMode
{
	enPopupTextAuto = 0,			// 自动显示
	enPopupTextHoverEnlarge,		// 鼠标热点行的高度放大
	enPopupTextFixedHeight,			// 每一行的高度固定，只显示名字
	enPopupTextLeftRight			// 每一行的高度固定，左侧显示名字，右侧显示描述
};

// 列表项定义
struct EditListItem
{
	UINT		nResourceID;	// ID
	CString		strImageFile;	// 图片文件
	Image*		pImage;			// 图片指针
	CSize		sizeImage;		// 图片大小
	CString		strName;		// 显示名
	CString		strDesc;		// 描述
	CString		strValue;		// 值
	CRect		rcItem;			// 位置
	Color		clrText;		// 文字颜色
	Color		clrDesc;		// 描述文字颜色
};

class CPopupList :  public CDlgPopup
{
	static LPCTSTR GetClassName() { return _T("popuplist");}

public:
	CPopupList(void);
	virtual ~CPopupList(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

	void SetWidth(int nWidth) { m_nWidth = nWidth; }
	void SetHeight(int nHeight) { m_nHeight = nHeight; }
	void SetFont(CString strFont, int nFontWidth, FontStyle fontStyle);
	void SetTitleFont(CString strFont, int nFontWidth, FontStyle fontStyle);
	void SetHoverColor(Color clrHover);
	void SetRowHeight(int nRowHeight) { m_nRowHeight = nRowHeight; }
	void SetTextMode(enumPopupTextMode enTextMode) { m_enTextMode = enTextMode; }

	bool GetItemDesc(UINT nItem, CString &strDesc);
	bool GetItemName(UINT nItem, CString &strName);
	bool GetItemValue(UINT nItem, CString &strValue);
	bool GetItemImage(UINT nItem, UINT &nResourceID, CString &strImageFile);
	int AddItem(CString strName, CString strDesc, CString strValue, int nResourceID,
		CString strImageFile = _T(""), Color clrText = Color(255, 0, 20, 35), Color clrDesc = Color(255, 255, 255, 255));
	bool DeleteItem(UINT nItem);

	void SetCurItem(UINT nItem);
	void SetCurItem(CString strValue);

	virtual void InitUI(CRect rcClient);

	void SetItemPoint();

	HRESULT OnAttributeImageScrollV(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeFontTitle(const CString& strValue, BOOL bLoading);

	virtual BOOL OnMouseMove(CPoint point);
	virtual BOOL OnLButtonDown(CPoint point);
	virtual BOOL OnLButtonUp(CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void DrawWindow(CDC &dc, CRect rcClient);
	virtual void DrawWindowEx(CDC &dc, CRect rcClient);

public:
	CRect					m_rcClose;			// 列表项的关闭区域
	enumButtonState			m_buttonState;
	vector<EditListItem>	m_vecItem;			// 下拉项列表
	int						m_nHoverItem;		// 当前选择的列表项索引
	int						m_nWidth;			// 列表宽度
	int						m_nHeight;			// 列表高度

	CString					m_strFont;			// 文字字体
	int						m_nFontWidth;		// 文字字体宽度
	FontStyle				m_fontStyle;		// 文字字体Style
	CString					m_strFontTitle;		// 标题字体
	int						m_nFontTitleWidth;	// 标题字体宽度
	FontStyle				m_fontTitleStyle;	// 标题字体Style

	Color					m_clrHover;			// 选择的列表项背景颜色(鼠标移动到行)
	BOOL					m_bSingleLine;		// 是否单行的列表
	enumPopupTextMode		m_enTextMode;		// 文字显示模式
	int						m_nVirtualHeight;	// 整体的高度
	CControlBaseFont*		m_pControScrollV;	// 垂直滚动条
	int						m_nScrollWidth;		// 滚动条宽度
	int						m_nRowHeight;		// 行高度
	int						m_nVirtualTop;		// 当前虚拟显示的顶部位置

	DUI_IMAGE_ATTRIBUTE_DEFINE(Head);			// 定义列表项左侧图片
	DUI_IMAGE_ATTRIBUTE_DEFINE_DPI(Close);		// 定义列表项删除图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("width"), m_nWidth, FALSE)
		DUI_INT_ATTRIBUTE_DPI(_T("height"), m_nHeight, FALSE)
		DUI_CUSTOM_ATTRIBUTE("img-scroll", OnAttributeImageScrollV)
		DUI_INT_ATTRIBUTE("scroll-width", m_nScrollWidth, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("font-title"), OnAttributeFontTitle)
		DUI_INT_ATTRIBUTE_DPI(_T("row-height"), m_nRowHeight, FALSE)
		DUI_ENUM_ATTRIBUTE(_T("text-mode"), enumPopupTextMode, TRUE)
			DUI_ENUM_VALUE(_T("auto"), enPopupTextAuto)
			DUI_ENUM_VALUE(_T("hover-enlarge"), enPopupTextHoverEnlarge)
			DUI_ENUM_VALUE(_T("fixed-height"), enPopupTextFixedHeight)
			DUI_ENUM_VALUE(_T("left-right"), enPopupTextLeftRight)
		DUI_ENUM_END(m_enTextMode)
    DUI_DECLARE_ATTRIBUTES_END()
	
protected:
	int GetItemsOffset();//列表项相对显示区顶部的偏移量
	DECLARE_DYNAMIC(CPopupList)
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
