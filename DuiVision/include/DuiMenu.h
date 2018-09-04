#pragma once
#include "DlgPopup.h"

#include <vector>
using namespace std;

// 菜单项预设置信息
struct MenuItemValue
{
	//UINT		uID;			// 菜单项ID
	CString		strName;		// 菜单项名字
	CString		strTitle;		// 菜单项标题
	BOOL		bVisible;		// 是否可见
	BOOL		bDisable;		// 是否禁用
	int			nCheck;			// 是否选择
};

class CMenuItem;

class CDuiMenu : public CDlgPopup
{
public:
	static LPCTSTR GetClassName() { return _T("menu");}
	virtual BOOL IsClass(LPCTSTR lpszName)
	{
		if(_tcscmp(GetClassName(), lpszName)  == 0) return TRUE;
		return __super::IsClass(lpszName);
	}

public:
	CDuiMenu(CString strFont = DuiSystem::GetDefaultFont(), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular);
	virtual ~CDuiMenu(void);

	// 创建菜单窗口
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID, UINT nResourceID, int nFrameSize = 4, int nMinWidth = 113, enumBackMode enBackMode = enBMFrame);
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID, CString strImage, int nFrameSize = 4, int nMinWidth = 113, enumBackMode enBackMode = enBMFrame);
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID);

	// 添加菜单项和分隔线
	int AddMenu(CString strText, UINT uMenuID, int nResourceID = -1, BOOL bSelect = false, int nIndex = -1);
	int AddMenu(CString strText, UINT uMenuID, CString strImage, BOOL bSelect = false, int nIndex = -1);
	int AddSeparator(int nIndex = -1);

	// 预设值菜单项的属性
	void SetItemTitle(CString strName, CString strTitle);
	void SetItemVisible(CString strName, BOOL bVisible);
	void SetItemDisable(CString strName, BOOL bDisable);
	void SetItemCheck(CString strName, int nCheck);

	// 设置菜单项位置
	void SetMenuPoint();

	// 获取父菜单对象
	CDuiMenu* GetParentMenu();
	// 获取当前激活菜单项对象
	CMenuItem* GetHoverMenuItem();
	// 获取当前处于活动状态的子菜单项(根据鼠标位置查找)
	CMenuItem* GetMenuItemWithPoint(CPoint point);

	// 加载菜单
	BOOL LoadSubMenu(DuiXmlNode pXmlElem, CString strSubItemName);
	BOOL LoadXmlNode(DuiXmlNode pXmlElem, CString strXmlFile = _T(""));
	BOOL LoadXmlFile(CString strFileName, CString strSubItemName = _T(""));
	BOOL LoadXmlFile(CString strFileName, CWnd *pParent, CPoint point, UINT uMessageID, CString strSubItemName = _T(""));

	virtual void DrawWindowEx(CDC &dc, CRect rcClient);
	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	virtual void InitUI(CRect rcClient);

	// 消息响应
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnNcCloseWindow();

public:
	CString		m_strTitle;				// 标题
	CString		m_strFont;				// 菜单字体
	int			m_nFontWidth;			// 字体宽度
	FontStyle	m_fontStyle;			// 字体风格
	UINT		m_uAlignment;			// 水平对齐方式
	UINT		m_uVAlignment;			// 垂直对齐方式

	int			m_nLeft;				// 左边间隔
	int			m_nHeight;				// 菜单项默认高度
	int			m_nWidth;				// 菜单项最大宽度
	int			m_nFrameWidth;			// 菜单项距离边框的宽度
	int			m_nTopHeight;			// 菜单项顶部距离边框的高度
	int			m_nBottomHeight;		// 菜单项底部距离边框的高度
	int			m_nSeparatorHeight;		// 分隔线高度
	CPoint		m_point;				// 菜单位置
	CWnd*		m_pParent;				// 父窗口

	Color		m_clrRowHover;			// 行背景颜色(鼠标移动到菜单项)

	vector<MenuItemValue> m_vecMenuItemValue;	// 菜单项预设置信息

	DUI_IMAGE_ATTRIBUTE_DEFINE(RowHover);	// 定义行背景图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(PopupArrow);	// 定义弹出菜单箭头图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_TSTRING_ATTRIBUTE(_T("title"), m_strTitle, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("font"), m_strFont, FALSE)
		DUI_INT_ATTRIBUTE(_T("fontwidth"), m_nFontWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("item-height"), m_nHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("frame-width"), m_nFrameWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("top-height"), m_nTopHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("bottom-height"), m_nBottomHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("left"), m_nLeft, FALSE)
		DUI_INT_ATTRIBUTE(_T("sep-height"), m_nSeparatorHeight, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crrowhover"), m_clrRowHover, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("img-rowhover"), OnAttributeImageRowHover)
		DUI_CUSTOM_ATTRIBUTE(_T("img-popuparrow"), OnAttributeImagePopupArrow)
	DUI_DECLARE_ATTRIBUTES_END()
};
