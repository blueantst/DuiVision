#include "StdAfx.h"
#include "Frame.h"

CDuiFrame::CDuiFrame(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBase(hWnd, pDuiObject)
{
	m_clr = RGB(255, 255, 255);
	m_clrFrame = RGB(0, 0, 0);
	m_nFrameWidth = 1;
	m_nDashStyle = DashStyleSolid;
	m_nBeginTransparent = 50;
	m_nEndTransparent = 50;
}

CDuiFrame::CDuiFrame(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nBeginTransparent/* = 50*/,int nEndTransparent/* = 50*/, 
			 COLORREF clr/* = RGB(255, 255, 255)*/, BOOL bIsVisible/* = TRUE*/)
			: CControlBase(hWnd, pDuiObject, uControlID, rc, bIsVisible, FALSE, FALSE)
{
	m_clr = clr;
	m_clrFrame = RGB(0, 0, 0);
	m_nFrameWidth = 1;
	m_nDashStyle = DashStyleSolid;
	m_nBeginTransparent = nBeginTransparent;
	m_nEndTransparent = nEndTransparent;
}

CDuiFrame::~CDuiFrame(void)
{
}

void CDuiFrame::DrawControl(CDC &dc, CRect rcUpdate)
{
	CRect rc = m_rc;
	if(!m_bUpdate)
	{
		UpdateMemDC(dc, rc.Width(), rc.Height());

		Graphics graphics(m_memDC);

		m_memDC.SetBkMode(TRANSPARENT);

		// 画边框
		Pen pen(m_clrFrame, (Gdiplus::REAL)m_nFrameWidth);
		pen.SetDashStyle((Gdiplus::DashStyle)m_nDashStyle);
		RectF rectFrame((Gdiplus::REAL)(0), (Gdiplus::REAL)(0), (Gdiplus::REAL)(rc.Width()), (Gdiplus::REAL)(rc.Height()));
		graphics.DrawRectangle(&pen, rectFrame);

		// 计算内容部分大小
		//rc.OffsetRect(m_nFrameWidth, m_nFrameWidth);
		//rc.DeflateRect(m_nFrameWidth*2, m_nFrameWidth*2);

		//SolidBrush brush(m_clr);
		//graphics.FillRectangle(&brush, m_nFrameWidth, m_nFrameWidth, rc.Width()-2*m_nFrameWidth, rc.Height()-2*m_nFrameWidth);
/*
		m_memDC.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, m_rc.left+m_nFrameWidth, m_rc.top+m_nFrameWidth, WHITENESS);	// 画白色背景
		DrawVerticalTransition(m_memDC, dc, CRect(m_nFrameWidth, m_nFrameWidth, rc.Width(), rc.Height()),	// 背景透明度
				m_rc, m_nBeginTransparent, m_nEndTransparent);
*/
/*
		// 计算内容部分大小
		rc.OffsetRect(m_nFrameWidth, m_nFrameWidth);
		rc.DeflateRect(m_nFrameWidth*2, m_nFrameWidth*2);

		// 画内容部分,支持渐变填充
		CDC TempDC;
		TempDC.CreateCompatibleDC(&dc);

		CBitmap memBmpTemp;
		memBmpTemp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
		CBitmap *pOldmap = TempDC.SelectObject(&memBmpTemp);

		m_memDC.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dc, m_rc.left+m_nFrameWidth, m_rc.top+m_nFrameWidth, SRCCOPY);
		TempDC.FillSolidRect(&rc, m_clr);		

		DrawVerticalTransition(m_memDC, TempDC, rc, rc, m_nBeginTransparent, m_nEndTransparent);
		
		TempDC.SelectObject(pOldmap);
		memBmpTemp.DeleteObject();
		TempDC.DeleteDC();*/
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}