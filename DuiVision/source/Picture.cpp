#include "StdAfx.h"
#include "Picture.h"

CDuiPicture::CDuiPicture(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBaseFont(hWnd, pDuiObject)
{
	m_enShowMode = enSMNormal;
	m_nFrameSize = 3;
	m_nWLT = 0;
	m_nHLT = 0;
	m_nWRB = 0;
	m_nHRB = 0;

	SetBitmapCount(1);
}

CDuiPicture::CDuiPicture(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc,
				   BOOL bIsVisible/* = TRUE*/)
			: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, TEXT(""), bIsVisible, FALSE)
{
	m_enShowMode = enSMNormal;
	m_nFrameSize = 3;
	m_nWLT = 0;
	m_nHLT = 0;
	m_nWRB = 0;
	m_nHRB = 0;

	SetBitmapCount(1);
}

CDuiPicture::~CDuiPicture(void)
{
}

// 设置显示模式
BOOL CDuiPicture::SetShowMode(enumShowMode enShowMode, int nFrameSize)
{
	if(enShowMode != m_enShowMode || m_enShowMode == enSMFrame && nFrameSize != m_nFrameSize)
	{
		m_nFrameSize = nFrameSize;
		m_enShowMode = enShowMode;
		UpdateControl(true);
		return true;
	}
	return false;
}

// 设置显示模式为九宫格模式
BOOL CDuiPicture::SetShowModeMID(int nWLT, int nHLT, int nWRB, int nHRB)
{
	m_enShowMode = enSMMID;
	m_nWLT = nWLT;
	m_nHLT = nHLT;
	m_nWRB = nWRB;
	m_nHRB = nHRB;
	UpdateControl(true);
	return true;
}

void CDuiPicture::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight);

		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

		Graphics graphics(m_memDC);
		if(m_enShowMode == enSMNormal)	// 普通单张图片
		{
			Rect rect;
			if(m_uAlignment == Align_Left)
			{
				rect.X = 0;
			}
			else if(m_uAlignment == Align_Right)
			{
				rect.X = nWidth - m_sizeImage.cx;
			}
			else
			{
				rect.X = (nWidth - m_sizeImage.cx) / 2;
			}

			if(m_uVAlignment == VAlign_Top)
			{
				rect.Y = 0;
			}
			else if(m_uVAlignment == VAlign_Bottom)
			{
				rect.Y = (nHeight - m_sizeImage.cy);
			}
			else
			{
				rect.Y = (nHeight - m_sizeImage.cy) / 2;
			}

			rect.Width = m_sizeImage.cx;
			rect.Height = m_sizeImage.cy;

			graphics.DrawImage(m_pImage, rect, 0, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
		}else
		if(m_enShowMode == enSMTile)	// 平铺
		{
			int nHCount = nWidth / m_sizeImage.cx;
			int nVCount = nHeight / m_sizeImage.cy;
			for(int i=0; i<=nHCount; i++)
			{
				for(int j=0; j<=nVCount; j++)
				{
					int nTileWidth = nWidth - i*m_sizeImage.cx;		// 剩余的宽度
					int nTileHeight = nHeight - j*m_sizeImage.cy;	// 剩余的高度
					if((nTileWidth < 0) || (nTileHeight < 0))
					{
						continue;
					}
					BOOL bHTile = (nTileWidth < m_sizeImage.cx);	// 是否小于一幅图片宽度
					BOOL bVTile = (nTileHeight < m_sizeImage.cy);	// 是否小于一幅图片高度
					Rect rect(i*m_sizeImage.cx, j*m_sizeImage.cy,
						bHTile ? nTileWidth : m_sizeImage.cx, bVTile ? nTileHeight : m_sizeImage.cy);
					graphics.DrawImage(m_pImage, rect,
						0, 0, bHTile ? nTileWidth : m_sizeImage.cx, bVTile ? nTileHeight : m_sizeImage.cy, UnitPixel);
				}
			}
		}else
		if(m_enShowMode == enSMExtrude)	// 拉伸
		{
			Rect rect(0, 0, nWidth, nHeight);
			graphics.DrawImage(m_pImage, rect, 0, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
		}else
		if(m_enShowMode == enSMFrame)	// 边框
		{
			CRect  rcTemp(0, 0, nWidth, nHeight);
			DrawImageFrame(graphics, m_pImage, rcTemp, 0, 0, m_sizeImage.cx, m_sizeImage.cy, m_nFrameSize);
		}else
		if(m_enShowMode == enSMMID)	// 九宫格边框
		{
			CRect  rcTemp(0, 0, nWidth, nHeight);
			DrawImageFrameMID(graphics, m_pImage, rcTemp, 0, 0, m_sizeImage.cx, m_sizeImage.cy,
				m_nWLT, m_nHLT, m_nWRB, m_nHRB);
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}