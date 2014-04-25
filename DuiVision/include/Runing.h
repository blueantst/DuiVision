// 动画图片，多图片类，可以通过多个图片生成动画，或者显现指定的图片
#pragma once
#include "ControlBase.h"


class CRuning : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CRuning, "animateimg")
public:
	CRuning(HWND hWnd, CDuiObject* pDuiObject);
	CRuning(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, 
		UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CRuning(void);
	
	HRESULT OnAttributeMaxIndex(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeRun(const CStringA& strValue, BOOL bLoading);
	BOOL SetRun(BOOL bRun, int nIndex = -1);
	void SetTimerCount(int nTimerCount) { m_nTimerCount = nTimerCount; }
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	
public:
	//过程索引
	int m_nIndex;
	int m_nMaxIndex;
	int m_nCount;
	int m_nTimerCount;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("index", m_nIndex, FALSE)
		DUI_CUSTOM_ATTRIBUTE("maxindex", OnAttributeMaxIndex)
		DUI_INT_ATTRIBUTE("timer-count", m_nTimerCount, FALSE)
		DUI_CUSTOM_ATTRIBUTE("run", OnAttributeRun)
	DUI_DECLARE_ATTRIBUTES_END()
};