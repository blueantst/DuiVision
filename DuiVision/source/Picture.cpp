#include "StdAfx.h"
#include "Picture.h"
#include "math.h"

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

// ������ʾģʽ
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

// ������ʾģʽΪ�Ź���ģʽ
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
		if(m_enShowMode == enSMNormal)	// ��ͨ����ͼƬ
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
		if(m_enShowMode == enSMTile)	// ƽ��
		{
			int nHCount = nWidth / m_sizeImage.cx;
			int nVCount = nHeight / m_sizeImage.cy;
			for(int i=0; i<=nHCount; i++)
			{
				for(int j=0; j<=nVCount; j++)
				{
					int nTileWidth = nWidth - i*m_sizeImage.cx;		// ʣ��Ŀ��
					int nTileHeight = nHeight - j*m_sizeImage.cy;	// ʣ��ĸ߶�
					if((nTileWidth < 0) || (nTileHeight < 0))
					{
						continue;
					}
					BOOL bHTile = (nTileWidth < m_sizeImage.cx);	// �Ƿ�С��һ��ͼƬ���
					BOOL bVTile = (nTileHeight < m_sizeImage.cy);	// �Ƿ�С��һ��ͼƬ�߶�
					Rect rect(i*m_sizeImage.cx, j*m_sizeImage.cy,
						bHTile ? nTileWidth : m_sizeImage.cx, bVTile ? nTileHeight : m_sizeImage.cy);
					graphics.DrawImage(m_pImage, rect,
						0, 0, bHTile ? nTileWidth : m_sizeImage.cx, bVTile ? nTileHeight : m_sizeImage.cy, UnitPixel);
				}
			}
		}else
		if(m_enShowMode == enSMExtrude || m_enShowMode == enCycle)	// ����
		{
			Rect rect(0, 0, nWidth, nHeight);
			graphics.DrawImage(m_pImage, rect, 0, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
		}else
		if(m_enShowMode == enSMFrame)	// �߿�
		{
			CRect  rcTemp(0, 0, nWidth, nHeight);
			DrawImageFrame(graphics, m_pImage, rcTemp, 0, 0, m_sizeImage.cx, m_sizeImage.cy, m_nFrameSize);
		}else
		if(m_enShowMode == enSMMID)	// �Ź���߿�
		{
			CRect  rcTemp(0, 0, nWidth, nHeight);
			DrawImageFrameMID(graphics, m_pImage, rcTemp, 0, 0, m_sizeImage.cx, m_sizeImage.cy,
				m_nWLT, m_nHLT, m_nWRB, m_nHRB);
		}
	}
	if (m_enShowMode == enCycle)
	{
 		Graphics graphics(dc.m_hDC);
		Pen pen(Color(225,225,225),1);
		graphics.SetSmoothingMode(SmoothingModeHighQuality);
		//���òü�Բ
		GraphicsPath graphicspath;
		if (graphicspath.GetLastStatus() != Ok)
			return;
		graphicspath.AddEllipse(m_rc.left-1, m_rc.top-1, nWidth+1, nHeight+1);
		graphics.SetClip(&graphicspath, CombineModeReplace);

		//����ͼ��
		graphics.DrawImage(m_pImage, m_rc.left,m_rc.top,nWidth,nHeight);

		//graphics.DrawEllipse(&pen,m_rc.left,m_rc.top, nWidth, nHeight);
	}else
		dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);	
}