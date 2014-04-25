#include "StdAfx.h"
#include "Rectangle.h"

CRectangle::CRectangle(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBase(hWnd, pDuiObject)
{
	m_clr = Color(254, 255, 255, 255);
}

CRectangle::CRectangle(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, 
			 Color clr/* = Color(254, 255, 255, 255)*/, BOOL bIsVisible/* = TRUE*/)
			: CControlBase(hWnd, pDuiObject, uControlID, rc, bIsVisible, FALSE, FALSE)
{
	m_clr = clr;
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

		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);
		
		Graphics graphics(m_memDC);
		SolidBrush brush(m_clr);
		graphics.FillRectangle(&brush, 0, 0, nWidth, nHeight);
	}

	dc.BitBlt(m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}