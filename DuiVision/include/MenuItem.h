#pragma once
#include "ControlBase.h"

class CMenuEx;

class CMenuItem : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CMenuItem, "menuitem")
public:
	CMenuItem(HWND hWnd, CDuiObject* pDuiObject);
	CMenuItem(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle= TEXT(""), int nLeft = 30, BOOL bSelect = false, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual ~CMenuItem(void);

	// 检查框状态
	BOOL GetCheck();
	BOOL SetCheck(BOOL bCheck);

	// 是否分隔线
	BOOL IsSeparator() { return m_bIsSeparator; }
	// 是否弹出菜单
	BOOL IsPopup() { return m_bIsPopup; }
	// 是否处于活动状态(鼠标在此菜单上)
	BOOL IsHover() { return (m_enButtonState == enBSHover); }

	// 获取父菜单对象
	CMenuEx* GetParentMenu();
	// 设置菜单项的嵌套XML文件名
	void SetMenuXml(CString strMenuXml) { m_strMenuXml = strMenuXml; }
	// 获取菜单项的嵌套XML文件名
	CString GetMenuXml() { return m_strMenuXml; }
	// 显示弹出菜单
	void ShowPopupMenu();
	// 获取弹出菜单
	CMenuEx* GetPopupMenu() { return m_pPopupMenu; }
	// 设置弹出菜单
	void SetPopupMenu(CMenuEx* pPopupMenu) { m_pPopupMenu = pPopupMenu; }
	// 设置菜单项和边框之间的距离
	void SetFrameWidth(int nFrameWidth) { m_nFrameWidth = nFrameWidth; }

	HRESULT OnAttributeCheck(const CStringA& strValue, BOOL bLoading);

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual	void SetControlDisable(BOOL bIsDisable);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	enumButtonState		m_enButtonState;
	BOOL				m_bDown;			// 如果是检查框，表示是否选择
	BOOL				m_bMouseDown;
	BOOL				m_bSelect;			// 是否选择,检查框必须是选择的
	BOOL				m_bIsSeparator;		// 是否分隔线
	BOOL				m_bIsPopup;			// 是否弹出菜单
	CMenuEx*			m_pPopupMenu;		// 弹出菜单的对象指针
	CString				m_strMenuXml;		// 嵌套菜单的XML文件
	int					m_nLeft;			// 菜单项文字的左边距
	int					m_nFrameWidth;		// 菜单项距离边框的宽度

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("select", m_bSelect, FALSE)
		DUI_INT_ATTRIBUTE("separator", m_bIsSeparator, FALSE)
		DUI_TSTRING_ATTRIBUTE("menu", m_strMenuXml, FALSE)
		DUI_CUSTOM_ATTRIBUTE("check", OnAttributeCheck)
	DUI_DECLARE_ATTRIBUTES_END()
};