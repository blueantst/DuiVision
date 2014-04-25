// 图片控件
#pragma once
#include "ControlBase.h"

// 图片显示模式
enum enumShowMode
{
	enSMNormal = 0,			// 正常
	enSMTile,				// 平铺
	enSMExtrude,			// 拉伸
	enSMFrame,				// 边框
	enSMMID					// 九宫格边框
};

class CDuiPicture : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiPicture, "img")
public:
	CDuiPicture(HWND hWnd, CDuiObject* pDuiObject);
	CDuiPicture(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, 
		BOOL bIsVisible = TRUE);
	virtual ~CDuiPicture(void);

	BOOL SetShowMode(enumShowMode enShowMode, int nFrameSize = 2);
	BOOL SetShowModeMID(int nWLT, int nHLT, int nWRB, int nHRB);

protected:
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

public:
	enumShowMode	m_enShowMode;	// 显示模式
	int				m_nFrameSize;	// 边框大小(enSMFrame模式)
	int				m_nWLT;			// 左上角宽度(九宫格模式)
	int				m_nHLT;			// 左上角高度(九宫格模式)
	int				m_nWRB;			// 右下角宽度(九宫格模式)
	int				m_nHRB;			// 右下角高度(九宫格模式)

	DUI_DECLARE_ATTRIBUTES_BEGIN()
 		DUI_INT_ATTRIBUTE("framesize", m_nFrameSize, FALSE)
		DUI_INT_ATTRIBUTE("width-lt", m_nWLT, FALSE)
		DUI_INT_ATTRIBUTE("height-lt", m_nHLT, FALSE)
		DUI_INT_ATTRIBUTE("width-rb", m_nWRB, FALSE)
		DUI_INT_ATTRIBUTE("height-rb", m_nHRB, FALSE)
		DUI_ENUM_ATTRIBUTE("mode", enumShowMode, TRUE)
            DUI_ENUM_VALUE("normal", enSMNormal)
            DUI_ENUM_VALUE("tile", enSMTile)
            DUI_ENUM_VALUE("extrude", enSMExtrude)
			DUI_ENUM_VALUE("frame", enSMFrame)
			DUI_ENUM_VALUE("mid", enSMMID)
        DUI_ENUM_END(m_enShowMode)
    DUI_DECLARE_ATTRIBUTES_END()
};
