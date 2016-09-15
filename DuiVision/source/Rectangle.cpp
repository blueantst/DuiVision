#include "StdAfx.h"
#include "Rectangle.h"

CRectangle::CRectangle(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBase(hWnd, pDuiObject)
{
	m_clr = Color(254, 255, 255, 255);
	m_clrFrame = RGB(0, 0, 0);
	m_nFrameWidth = 1;
	m_nDashStyle = DashStyleSolid;
	m_nTransparentType = TRANSPARENT_VERTICAL;
	m_nBeginTransparent = 100;
	m_nEndTransparent = 100;
}

CRectangle::CRectangle(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, 
			 Color clr/* = Color(254, 255, 255, 255)*/, BOOL bIsVisible/* = TRUE*/)
			: CControlBase(hWnd, pDuiObject, uControlID, rc, bIsVisible, FALSE, FALSE)
{
	m_clr = clr;
	m_clrFrame = RGB(0, 0, 0);
	m_nFrameWidth = 1;
	m_nDashStyle = DashStyleSolid;
	m_nTransparentType = TRANSPARENT_VERTICAL;
	m_nBeginTransparent = 100;
	m_nEndTransparent = 100;
}

CRectangle::~CRectangle(void)
{
}

void CRectangle::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight);

		m_memDC.SetBkMode(TRANSPARENT);
		
		Graphics graphics(m_memDC);
		// 定义填充颜色
		graphics.Clear(m_clr);

		// 定义边框画笔
		Pen pen(m_clrFrame, (Gdiplus::REAL)m_nFrameWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)m_nDashStyle);

		RectF rect((Gdiplus::REAL)(m_nFrameWidth), (Gdiplus::REAL)(m_nFrameWidth), (Gdiplus::REAL)(nWidth-m_nFrameWidth*2), (Gdiplus::REAL)(nHeight-m_nFrameWidth*2));
		graphics.DrawRectangle(&pen, rect);

		if(m_nTransparentType == TRANSPARENT_VERTICAL)
		{
			DrawVerticalTransition(m_memDC, dc, CRect(m_nFrameWidth, m_nFrameWidth, nWidth, nHeight),	// 垂直背景透明度
				m_rc, m_nBeginTransparent, m_nEndTransparent);
		}else
		if(m_nTransparentType == TRANSPARENT_HORIZONTAL)
		{
			DrawHorizontalTransition(m_memDC, dc, CRect(m_nFrameWidth, m_nFrameWidth, nWidth, nHeight),	// 水平背景透明度
				m_rc, m_nBeginTransparent, m_nEndTransparent);
		}
	}

	dc.BitBlt(m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}