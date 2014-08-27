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
	int GetProgress() { return m_nProgress; };
	BOOL SetRun(BOOL bRun, int nIndex = -1);
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	HRESULT OnAttributeRun(const CString& strValue, BOOL bLoading);
	
public:
	//过程索引
	int				m_nIndex;			// 当前计数(=m_nProgress)
	int				m_nMaxIndex;		// 最大计数(100)
	int				m_nCount;			// 定时多少次计数增加一次
	
	int				m_nProgress;		// 当前进度(0-100)

	int				m_nHeadLength;		// 进度条图片头部长度

	DUI_IMAGE_ATTRIBUTE_DEFINE(BackGround);	// 定义背景图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(ForeGround);	// 定义前景图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("value"), m_nProgress, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("run"), OnAttributeRun)
		DUI_CUSTOM_ATTRIBUTE(_T("img-back"), OnAttributeImageBackGround)
		DUI_CUSTOM_ATTRIBUTE(_T("img-fore"), OnAttributeImageForeGround)
		DUI_INT_ATTRIBUTE(_T("head-len"), m_nHeadLength, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};