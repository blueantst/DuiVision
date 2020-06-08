#pragma once
#include "ControlBase.h"

// 按钮显示模式
enum enumButtonShowMode
{
	enBSMFrame = 0,			// 边框
	enBSMExtrude			// 拉伸
};

class CDuiButton : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiButton, _T("button"))
public:
	CDuiButton(HWND hWnd, CDuiObject* pDuiObject);
	CDuiButton(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle = TEXT(""), BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual ~CDuiButton(void);

	void SetMaxIndex(int nMaxIndex) { m_nMaxIndex = nMaxIndex; }
	void SetShowMode(enumButtonShowMode enButtonShowMode);
	void SetShowFocus(BOOL bShowFocus);

	virtual BOOL SetControlFocus(BOOL bFocus);

protected:

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual	void SetControlDisable(BOOL bIsDisable);
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	enumButtonShowMode	m_enButtonShowMode;	// 按钮的背景图片显示模式
	enumButtonState		m_enButtonState;	// 按钮状态
	Color				m_clrText;			// 文字颜色
	int					m_nIndex;			// 渐变效果的过程索引
	int					m_nMaxIndex;		// 渐变效果的最大索引
	BOOL				m_bTimer;			// 是否启用渐变效果定时器
	BOOL				m_bIsFocus;			// 当前是否处于焦点状态
	BOOL				m_bShowFocus;		// 是否显示焦点框

	DUI_IMAGE_ATTRIBUTE_DEFINE_DPI(Btn);		// 定义按钮图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_INT_ATTRIBUTE(_T("animate"), m_bTimer, TRUE)
		DUI_INT_ATTRIBUTE(_T("maxindex"), m_nMaxIndex, TRUE)
		DUI_CUSTOM_ATTRIBUTE(_T("img-btn"), OnAttributeImageBtn)
		DUI_BOOL_ATTRIBUTE(_T("showfocus"), m_bShowFocus, FALSE)
		DUI_ENUM_ATTRIBUTE(_T("showmode"), enumButtonShowMode, TRUE)
			DUI_ENUM_VALUE(_T("extrude"), enBSMExtrude)
			DUI_ENUM_VALUE(_T("frame"), enBSMFrame)
		DUI_ENUM_END(m_enButtonShowMode)
    DUI_DECLARE_ATTRIBUTES_END()
};

// 图片按钮,功能和基础的按钮完全相同,只是为了兼容之前的控件名字
class CImageButton : public CDuiButton
{
	DUIOBJ_DECLARE_CLASS_NAME(CImageButton, _T("imgbtn"))
public:
	CImageButton(HWND hWnd, CDuiObject* pDuiObject);
	CImageButton(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bAnimation = true, CString strTitle = TEXT(""), BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	~CImageButton(void) {};
};