#pragma once
#include "ControlBase.h"

// 进度文字显示模式
enum enumProgTextMode
{
	enTitleAuto = 0,			// 显示标题,并自动判断是否百分比显示
	enTitlePercent,				// 显示标题,并转换为百分比显示数值
	enTitleNumberUse,			// 显示标题,并显示进度数值
	enTitleNumberAll,			// 显示标题,并显示进度数值和总数
	enTitleOnly					// 只显示标题
};

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
	enumProgTextMode m_enTextMode;		// 文字显示模式

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
		DUI_ENUM_ATTRIBUTE(_T("text-mode"), enumProgTextMode, TRUE)
			DUI_ENUM_VALUE(_T("auto"), enTitleAuto)
			DUI_ENUM_VALUE(_T("percent"), enTitlePercent)
			DUI_ENUM_VALUE(_T("number-use"), enTitleNumberUse)
			DUI_ENUM_VALUE(_T("number-all"), enTitleNumberAll)
			DUI_ENUM_VALUE(_T("title-only"), enTitleOnly)
		DUI_ENUM_END(m_enTextMode)
	DUI_DECLARE_ATTRIBUTES_END()
};