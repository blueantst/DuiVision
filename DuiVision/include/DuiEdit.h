#pragma once
#include "ControlBase.h"

#define					BUTTOM				1
#define					EDIT				2

class CDuiEdit : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiEdit, "edit")
public:
	CDuiEdit(HWND hWnd, CDuiObject* pDuiObject);
	CDuiEdit(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle= TEXT(""), BOOL bPassWord = FALSE, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual ~CDuiEdit(void);

	// 设置左侧小图片
	bool SetLeftBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	bool SetLeftBitmap(CString strImage);
	HRESULT OnAttributeLeftImage(const CStringA& strValue, BOOL bLoading);

	// 设置右侧小图片
	bool SetSmallBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	bool SetSmallBitmap(CString strImage);
	HRESULT OnAttributeSmallImage(const CStringA& strValue, BOOL bLoading);

	virtual void SetControlRect(CRect rc);
	virtual BOOL SetControlFocus(BOOL bFocus);
	virtual BOOL IsDraw(CPoint point) ;
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlTitle(CString strTitle);
	BOOL GetLButtonDown();
	BOOL GetLButtonUp();
	CString GetEditText();

	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual	void SetControlDisable(BOOL bIsDisable);

	void ShowEdit();
	void HideEdit();
	
public:
	Image*			m_pLeftImage;		// 左侧小图片
	CSize			m_sizeLeftImage;	// 左侧小图片大小
	int				m_nLeftImageCount;	// 左侧小图片的个数

	Image*			m_pSmallImage;		// 右侧小图片
	CSize			m_sizeSmallImage;	// 右侧小图片大小
	BOOL			m_bIsSmallButton;	// 右侧小图片是否作为按钮来处理
	int				m_nSmallImageCount;	// 右侧小图片的个数

	bool			m_bPassWord;		// 密码输入
	bool			m_bMultiLine;		// 多行
	bool			m_bAutoHScroll;		// 水平滚动
	bool			m_bAutoVScroll;		// 垂直滚动
	bool			m_bNumber;			// 只能输入数字
	bool			m_bReadOnly;		// 只读
	int				m_nMaxChar;			// 最大字符数

	bool			m_bDown;
	bool			m_bDownTemp;
	enumButtonState m_buttonState;
	enumButtonState m_EditState;
	CRect			m_rcText;			// 文字部分位置

	Color			m_clrText;			// 文字颜色
	Color			m_clrTooltip;		// 提示信息颜色
	CFont			m_fontTemp;			// 输入框的字体
	CEdit*			m_pEdit;			// Windows原生输入控件

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("password", m_bPassWord, FALSE)
		DUI_INT_ATTRIBUTE("multiline", m_bMultiLine, FALSE)
		DUI_INT_ATTRIBUTE("autohscroll", m_bAutoHScroll, FALSE)
		DUI_INT_ATTRIBUTE("autovscroll", m_bAutoVScroll, FALSE)
		DUI_INT_ATTRIBUTE("number", m_bNumber, FALSE)
		DUI_INT_ATTRIBUTE("readonly", m_bReadOnly, FALSE)
		DUI_INT_ATTRIBUTE("maxchar", m_nMaxChar, FALSE)
		DUI_CUSTOM_ATTRIBUTE("left-image", OnAttributeLeftImage)
		DUI_CUSTOM_ATTRIBUTE("small-image", OnAttributeSmallImage)
		DUI_COLOR_ATTRIBUTE("crtext", m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE("crtip", m_clrTooltip, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};