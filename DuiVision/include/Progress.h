#pragma once
#include "ControlBase.h"


class CDuiProgress : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiProgress, "progress")
public:
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject);
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress = 0,
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiProgress(void);

	int SetProgress(int nProgress);
	int GetProgress() { return m_nProgress; };
	BOOL SetRun(BOOL bRun, int nIndex = -1);

	HRESULT OnAttributeRun(const CStringA& strValue, BOOL bLoading);
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	
public:
	//过程索引
	int m_nIndex;		// 当前计数(=m_nProgress)
	int m_nMaxIndex;// 最大计数(100)
	int m_nCount;		// 定时多少次计数增加一次
	
	int m_nProgress;	// 当前进度(0-100)

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("value", m_nProgress, FALSE)
		DUI_CUSTOM_ATTRIBUTE("run", OnAttributeRun)
	DUI_DECLARE_ATTRIBUTES_END()
};