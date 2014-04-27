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

	BOOL SetBackGroundImage(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetBackGroundImage(CString strImage = TEXT(""));
	BOOL SetForeGroundImage(UINT nResourceID = 0, CString strType= TEXT("PNG"));
	BOOL SetForeGroundImage(CString strImage = TEXT(""));
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	HRESULT OnAttributeRun(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageBackGround(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImageForeGround(const CStringA& strValue, BOOL bLoading);
	
public:
	//过程索引
	int				m_nIndex;			// 当前计数(=m_nProgress)
	int				m_nMaxIndex;		// 最大计数(100)
	int				m_nCount;			// 定时多少次计数增加一次
	
	int				m_nProgress;		// 当前进度(0-100)

	Image*			m_pImageBackGround;	// 背景图片
	CSize			m_sizeBackGround;	// 背景图片大小
	Image*			m_pImageForeGround;	// 前景图片
	CSize			m_sizeForeGround;	// 前景图片大小
	int				m_nHeadLength;		// 进度条图片头部长度

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("value", m_nProgress, FALSE)
		DUI_CUSTOM_ATTRIBUTE("run", OnAttributeRun)
		DUI_CUSTOM_ATTRIBUTE("img-back", OnAttributeImageBackGround)
		DUI_CUSTOM_ATTRIBUTE("img-fore", OnAttributeImageForeGround)
		DUI_INT_ATTRIBUTE("head-len", m_nHeadLength, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};