#pragma once
#include "ControlBase.h"


class CDuiSlider : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiSlider, _T("slider"))
public:
	CDuiSlider(HWND hWnd, CDuiObject* pDuiObject);
	CDuiSlider(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress = 0,
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiSlider(void);

	int SetProgress(int nProgress);
	int GetProgress() { return m_nProgress; }
	void SetMaxProgress(int nMaxProgress) { m_nMaxProgress = nMaxProgress; }
	int GetMaxProgress() { return m_nMaxProgress; }

	int MoveThumbPos(CPoint point);
	
	virtual void SetControlRect(CRect rc);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	
public:
	enumButtonState m_enButtonState;	// 鼠标状态
	BOOL			m_bHover;			// 是否鼠标热点状态
	int				m_nProgress;		// 当前进度(0-100)
	int				m_nMaxProgress;		// 最大进度值(默认100)
	int				m_nSliderHeight;	// 背景条高度
	int				m_nThumbWidth;		// 滑块宽度
	int				m_nThumbHeight;		// 滑块高度
	int				m_nThumbTop;		// 滑块顶端位置
	int				m_nHeadLength;		// 进度条图片头部长度
	CRect			m_rcThumb;			// 滑块区域

	Color			m_clrText;			// 进度文字颜色
	BOOL			m_bShowText;		// 是否显示进度文字

	DUI_IMAGE_ATTRIBUTE_DEFINE(BackGround);	// 定义背景图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(ForeGround);	// 定义前景图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(Thumb);	// 定义滑块图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("value"), m_nProgress, FALSE)
		DUI_INT_ATTRIBUTE(_T("max-value"), m_nMaxProgress, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("img-back"), OnAttributeImageBackGround)
		DUI_CUSTOM_ATTRIBUTE(_T("img-fore"), OnAttributeImageForeGround)
		DUI_CUSTOM_ATTRIBUTE(_T("img-thumb"), OnAttributeImageThumb)
		DUI_INT_ATTRIBUTE(_T("slider-height"), m_nSliderHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("thumb-width"), m_nThumbWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("thumb-height"), m_nThumbHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("thumb-top"), m_nThumbTop, FALSE)
		DUI_INT_ATTRIBUTE(_T("head-len"), m_nHeadLength, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("show-text"), m_bShowText, TRUE)
	DUI_DECLARE_ATTRIBUTES_END()
};