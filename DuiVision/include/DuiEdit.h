#pragma once
#include "ControlBase.h"

// edit控件中的子控件类型
#define	CONTROL_BUTTON		1	// 编辑框中的按钮区域
#define	CONTROL_EDIT		2	// 编辑框中非按钮区域

class CDuiEdit : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiEdit, _T("edit"))
public:
	CDuiEdit(HWND hWnd, CDuiObject* pDuiObject);
	CDuiEdit(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle= TEXT(""), BOOL bPassWord = FALSE, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual ~CDuiEdit(void);

	// 设置左侧小图片
	bool SetLeftBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	bool SetLeftBitmap(CString strImage);
	HRESULT OnAttributeLeftImage(const CString& strValue, BOOL bLoading);

	// 设置右侧小图片
	bool SetSmallBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	bool SetSmallBitmap(CString strImage);
	HRESULT OnAttributeSmallImage(const CString& strValue, BOOL bLoading);

	void SetBackColor(Color clrBack);
	HRESULT OnAttributeBackColor(const CString& strValue, BOOL bLoading);

	virtual void SetControlRect(CRect rc);
	virtual BOOL SetControlFocus(BOOL bFocus);
	virtual BOOL IsDraw(CPoint point) ;
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlTitle(CString strTitle);
	BOOL GetLButtonDown();
	BOOL GetLButtonUp();
	CString GetEditText();

	BOOL IsReadOnly() { return m_bReadOnly; }
	void SetReadOnly(BOOL bReadOnly) { m_bReadOnly = bReadOnly; }

	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual	void SetControlDisable(BOOL bIsDisable);

	void CreateEditFont();
	void ShowEdit();
	void HideEdit();
	void DeleteEdit();
	
public:
	Image*			m_pLeftImage;		// 左侧小图片
	CSize			m_sizeLeftImage;	// 左侧小图片大小
	int				m_nLeftImageCount;	// 左侧小图片的个数

	Image*			m_pSmallImage;		// 右侧小图片
	CSize			m_sizeSmallImage;	// 右侧小图片大小
	BOOL			m_bIsSmallButton;	// 右侧小图片是否作为按钮来处理
	int				m_nSmallImageCount;	// 右侧小图片的个数

	BOOL			m_bPassWord;		// 密码输入
	BOOL			m_bMultiLine;		// 多行
	BOOL			m_bWantReturn;		// 是否允许回车换行
	BOOL			m_bAutoHScroll;		// 允许水平滚动
	BOOL			m_bAutoVScroll;		// 允许垂直滚动
	BOOL			m_bShowHScroll;		// 显示水平滚动条
	BOOL			m_bShowVScroll;		// 显示垂直滚动条
	BOOL			m_bNumber;			// 只能输入数字
	BOOL			m_bReadOnly;		// 只读
	int				m_nMaxChar;			// 最大字符数

	bool			m_bDown;
	bool			m_bDownTemp;
	enumButtonState m_buttonState;
	enumButtonState m_EditState;
	CRect			m_rcText;			// 文字部分位置

	bool			m_bBack;			// 是否填充背景
	Color			m_clrBack;			// 背景色
	Color			m_clrText;			// 文字颜色
	Color			m_clrTooltip;		// 提示信息颜色
	CFont			m_fontTemp;			// 输入框的字体
	CEdit*			m_pEdit;			// Windows原生输入控件

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_BOOL_ATTRIBUTE(_T("password"), m_bPassWord, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("multiline"), m_bMultiLine, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("wantreturn"), m_bWantReturn, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("autohscroll"), m_bAutoHScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("autovscroll"), m_bAutoVScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("hscroll"), m_bShowHScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("vscroll"), m_bShowVScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("number"), m_bNumber, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("readonly"), m_bReadOnly, FALSE)
		DUI_INT_ATTRIBUTE(_T("maxchar"), m_nMaxChar, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("left-image"), OnAttributeLeftImage)
		DUI_CUSTOM_ATTRIBUTE(_T("small-image"), OnAttributeSmallImage)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("crback"), OnAttributeBackColor)
		DUI_COLOR_ATTRIBUTE(_T("crtip"), m_clrTooltip, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};