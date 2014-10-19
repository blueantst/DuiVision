#pragma once
#include "ControlBase.h"


class CDuiProgress : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiProgress, _T("progress"))
public:
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject);
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress = 0,
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiProgress(void);

	int SetProgress(int nProgress);
	int GetProgress() { return m_nProgress; }
	void SetMaxProgress(int nMaxProgress) { m_nMaxProgress = nMaxProgress; }
	int GetMaxProgress() { return m_nMaxProgress; }
	BOOL SetRun(BOOL bRun, int nIndex = -1);
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	HRESULT OnAttributeRun(const CString& strValue, BOOL bLoading);
	
public:
	//过程索引
	int				m_nProgress;		// 当前进度(0-100)
	int				m_nMaxProgress;		// 最大进度值(默认100)
	int				m_nCount;			// 定时器次数计数
	int				m_nTimerCount;		// 定时多少次进度增加一次

	int				m_nHeadLength;		// 进度条图片头部长度

	Color			m_clrText;			// 进度文字颜色
	BOOL			m_bShowText;		// 是否显示进度文字

	DUI_IMAGE_ATTRIBUTE_DEFINE(BackGround);	// 定义背景图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(ForeGround);	// 定义前景图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("value"), m_nProgress, FALSE)
		DUI_INT_ATTRIBUTE(_T("max-value"), m_nMaxProgress, FALSE)
		DUI_INT_ATTRIBUTE(_T("timer-count"), m_nTimerCount, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("run"), OnAttributeRun)
		DUI_CUSTOM_ATTRIBUTE(_T("img-back"), OnAttributeImageBackGround)
		DUI_CUSTOM_ATTRIBUTE(_T("img-fore"), OnAttributeImageForeGround)
		DUI_INT_ATTRIBUTE(_T("head-len"), m_nHeadLength, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("show-text"), m_bShowText, TRUE)
	DUI_DECLARE_ATTRIBUTES_END()
};