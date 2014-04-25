#include "StdAfx.h"
#include "Frame.h"

CFrame::CFrame(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBase(hWnd, pDuiObject)
{
	m_clr = RGB(255, 255, 255);
	m_nBeginTransparent = 50;
	m_nEndTransparent = 50;
}

CFrame::CFrame(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nBeginTransparent/* = 50*/,int nEndTransparent/* = 50*/, 
			 COLORREF clr/* = RGB(255, 255, 255)*/, BOOL bIsVisible/* = TRUE*/)
			: CControlBase(hWnd, pDuiObject, uControlID, rc, bIsVisible, FALSE, FALSE)
{
	m_clr = clr;
	m_nBeginTransparent = nBeginTransparent;
	m_nEndTransparent = nEndTransparent;
}

CFrame::~CFrame(void)
{
}

void CFrame::DrawControl(CDC &dc, CRect rcUpdate)
{
	CRect rc = m_rc;
	if(!m_bUpdate)
	{
		UpdateMemDC(dc, rc.Width(), rc.Height());

		m_memDC.FillSolidRect(&rc, RGB(0, 0, 0));

		rc.left++;
		rc.top++;
		rc.right--;
		rc.bottom--;
		m_memDC.FillSolidRect(&rc, RGB(200, 200, 200));
	}

	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.BlendOp= AC_SRC_OVER;
	blend.SourceConstantAlpha = 128; // 透明度	

	// 左
	rc.SetRect(m_rc.left, m_rc.top + 2, m_rc.left + 2, m_rc.bottom - 2);
	rc = rc & rcUpdate;
	if(!rc.IsRectEmpty())
	{
		dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(), &m_memDC, rc.left, rc.top, rc.Width(), rc.Height(), blend);
	}

	// 上
	rc.SetRect(m_rc.left + 2, m_rc.top, m_rc.right - 2, m_rc.top + 2);
	rc = rc & rcUpdate;
	if(!rc.IsRectEmpty())
	{
		dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(), &m_memDC, rc.left, rc.top, rc.Width(), rc.Height(), blend);
	}

	// 右
	rc.SetRect(m_rc.right - 2, m_rc.top + 2, m_rc.right, m_rc.bottom - 2);
	rc = rc & rcUpdate;
	if(!rc.IsRectEmpty())
	{
		dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(), &m_memDC, rc.left, rc.top, rc.Width(), rc.Height(), blend);
	}

	// 下
	rc.SetRect(m_rc.left + 2, m_rc.bottom - 2, m_rc.right - 2, m_rc.bottom);
	rc = rc & rcUpdate;
	if(!rc.IsRectEmpty())
	{
		dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(), &m_memDC, rc.left, rc.top, rc.Width(), rc.Height(), blend);
	}

	blend.SourceConstantAlpha = 104; // 透明度	
	dc.AlphaBlend(m_rc.left, m_rc.top + 1, 1, 1, &m_memDC, 0, 0, 1, 1, blend);
	dc.AlphaBlend(m_rc.left + 1, m_rc.top, 1, 1, &m_memDC, 0, 0, 1, 1, blend);

	dc.AlphaBlend(m_rc.right - 2, m_rc.top, 1, 1, &m_memDC, 0, 0, 1, 1, blend);
	dc.AlphaBlend(m_rc.right - 1, m_rc.top + 1, 1, 1, &m_memDC, 0, 0, 1, 1, blend);

	dc.AlphaBlend(m_rc.right - 1, m_rc.bottom - 2, 1, 1, &m_memDC, 0, 0, 1, 1, blend);
	dc.AlphaBlend(m_rc.right - 2, m_rc.bottom - 1, 1, 1, &m_memDC, 0, 0, 1, 1, blend);

	dc.AlphaBlend(m_rc.left + 1, m_rc.bottom - 1, 1, 1, &m_memDC, 0, 0, 1, 1, blend);
	dc.AlphaBlend(m_rc.left, m_rc.bottom - 2, 1, 1, &m_memDC, 0, 0, 1, 1, blend);

	dc.AlphaBlend(m_rc.left + 1, m_rc.top + 1, 1, 1, &m_memDC, 1, 1, 1, 1, blend);
	dc.AlphaBlend(m_rc.right - 2, m_rc.top + 1, 1, 1, &m_memDC, 1, 1, 1, 1, blend);
	dc.AlphaBlend(m_rc.right - 2, m_rc.bottom - 2, 1, 1, &m_memDC, 1, 1, 1, 1, blend);
	dc.AlphaBlend(m_rc.left + 1, m_rc.bottom - 2, 1, 1, &m_memDC, 1, 1, 1, 1, blend);
}