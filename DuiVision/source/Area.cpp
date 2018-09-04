#include "StdAfx.h"
#include "Area.h"

CArea::CArea(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBase(hWnd, pDuiObject)
{
	m_clr = RGB(255, 255, 255);
	m_nTransparentType = TRANSPARENT_VERTICAL;
	m_nBeginTransparent = 50;
	m_nEndTransparent = 50;
}

CArea::CArea(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nBeginTransparent/* = 50*/,int nEndTransparent/* = 50*/, 
			 COLORREF clr/* = RGB(255, 255, 255)*/, BOOL bIsVisible/* = TRUE*/)
			: CControlBase(hWnd, pDuiObject, uControlID, rc, bIsVisible, FALSE, FALSE)
{
	m_clr = clr;
	m_nTransparentType = TRANSPARENT_VERTICAL;
	m_nBeginTransparent = nBeginTransparent;
	m_nEndTransparent = nEndTransparent;
}

CArea::~CArea(void)
{
}

void CArea::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight);

		CRect  rcTemp(0, 0, nWidth, nHeight);

		CDC TempDC;
		TempDC.CreateCompatibleDC(&dc);

		CBitmap memBmpTemp;
		memBmpTemp.CreateCompatibleBitmap(&dc, nWidth, nHeight);
		CBitmap *pOldmap =  TempDC.SelectObject(&memBmpTemp);

		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);
		TempDC.FillSolidRect(&rcTemp, m_clr);		

		if(m_nTransparentType == TRANSPARENT_VERTICAL)
		{
			DrawVerticalTransition(m_memDC, TempDC, rcTemp, rcTemp, m_nBeginTransparent, m_nEndTransparent);
		}else
		if(m_nTransparentType == TRANSPARENT_HORIZONTAL)
		{
			DrawHorizontalTransition(m_memDC, TempDC, rcTemp, rcTemp, m_nBeginTransparent, m_nEndTransparent);
		}
		
		TempDC.SelectObject(pOldmap);
		memBmpTemp.DeleteObject();
		TempDC.DeleteDC();
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}