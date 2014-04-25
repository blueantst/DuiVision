#include "StdAfx.h"
#include "Progress.h"

CDuiProgress::CDuiProgress(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBaseFont(hWnd, pDuiObject)
{
	m_bRunTime = false;
	m_nIndex = 0;
	m_nMaxIndex = 100;
	m_nCount = 0;

	SetBitmapCount(2);

	m_nProgress = 0;
	SetProgress(0);
}

CDuiProgress::CDuiProgress(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress/* = 0*/, 
					 BOOL bIsVisible/* = TRUE*/, BOOL bIsDisable/* = FALSE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, TEXT(""), bIsVisible, bIsDisable)
{
	m_bRunTime = false;
	m_nIndex = 0;
	m_nMaxIndex = 100;
	m_nCount = 0;

	SetBitmapCount(2);

	m_nProgress = 0;
	SetProgress(nProgress);
}

CDuiProgress::~CDuiProgress(void)
{
}

int CDuiProgress::SetProgress(int nProgress)
{
	int nOldProgress = m_nProgress;
	if(nProgress >= 0 && nProgress <= 100 && m_nProgress != nProgress)
	{
		m_nProgress = nProgress;
		UpdateControl(true);
	}
	return nOldProgress;
}

// 设置是否自动运行
BOOL CDuiProgress::SetRun(BOOL bRun, int nIndex/* = -1*/)
{
	m_bRunTime = bRun;
	if(nIndex != -1)
	{
		m_nIndex = nIndex;
		m_nProgress = nIndex;
	}
	UpdateControl();
	return TRUE;
}

// 从XML设置自动运行属性
HRESULT CDuiProgress::OnAttributeRun(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	BOOL bRun = (strValue == "true");
	SetRun(bRun);

	return bLoading?S_FALSE:S_OK;
}

BOOL CDuiProgress::OnControlTimer()
{
	if(!m_bRunTime || !m_bIsVisible)
	{
		return FALSE;
	}

	if(++m_nCount == 3)
	{
		m_nCount = 0;
		if(++m_nIndex >= m_nMaxIndex)
		{
			m_nIndex = 0;
		}
		m_nProgress = m_nIndex;
		UpdateControl();
		return true;
	}

	return false;
}

void CDuiProgress::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight);

		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

		Graphics graphics(m_memDC);		

		DrawImageFrame(graphics, m_pImage, CRect(0, 0, nWidth, nHeight), 
			0, 0, m_sizeImage.cx, m_sizeImage.cy, 2);

		if(m_nProgress != 0)
		{
			DrawImageFrame(graphics, m_pImage, CRect(0, 0, nWidth * m_nProgress / 100, nHeight), 
				m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, 2);
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}