#include "StdAfx.h"
#include "SelectBox.h"

CSelectBox::CSelectBox(HWND hWnd, CDuiObject* pDuiObject)
: CControlBase(hWnd, pDuiObject)
{
	m_nXCount = 1;
	m_nYCount = 1;
	m_clrFrame = Color(254, 0, 0, 0);
	m_clrHover = Color(128, 0, 0, 0);
	m_clrSelect = Color(254, 255, 255, 255);

	m_nXHover = -1;
	m_nYHover = -1;
	m_nXSelect = -1;
	m_nYSelect = -1;

	m_bImage = FALSE;

	m_nSelectIndex = -1;
	m_clrCurSelect = Color(0, 0, 0, 0);
}

CSelectBox::CSelectBox(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nXCount, int nYCount, BOOL bImage,
					   Color clrFrame/* = Color(254, 0, 0, 0)*/, Color clrHover/* = Color(64, 128, 128, 128)*/, Color clrSelect/* = Color(254, 255, 255, 255)*/, BOOL bIsVisible/* = TRUE*/)
					   : CControlBase(hWnd, pDuiObject, uControlID, rc, bIsVisible)
{
	m_nXCount = nXCount;
	m_nYCount = nYCount;
	m_clrFrame = clrFrame;
	m_clrHover = clrHover;
	m_clrSelect = clrSelect;

	m_nXHover = -1;
	m_nYHover = -1;
	m_nXSelect = -1;
	m_nYSelect = -1;

	m_bImage = bImage;

	m_nSelectIndex = -1;
	m_clrCurSelect = Color(0, 0, 0, 0);
}

CSelectBox::~CSelectBox(void)
{
	for(size_t i = 0; i < m_vecpImage.size(); i++)
	{
		if(m_vecpImage[i] != NULL)
		{
			delete m_vecpImage[i];
			m_vecpImage[i] = NULL;
		}
	}
}

// 加载XML节点
BOOL CSelectBox::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

	// 使用XML节点初始化控件
	if(pXmlElem != NULL)
	{
		// 加载图片和颜色列表
		DuiXmlNode pControlElem;
		for (DuiXmlNode pControlElem = pXmlElem.child(_T("item")); pControlElem; pControlElem=pControlElem.next_sibling(_T("item")))
		{
			CString strImage = pControlElem.attribute(_T("image")).value();
			CString strColor = pControlElem.attribute(_T("color")).value();
			if(!strImage.IsEmpty())
			{
				if(strImage.Find(_T("skin:")) == 0)
				{
					strImage = DuiSystem::Instance()->GetSkin(strImage);
				}

				if(strImage.Find(_T(".")) != -1)	// 加载图片文件
				{
					CString strImgFile = strImage;
					SetBitmap(strImgFile);
				}else	// 加载图片资源
				{
					UINT nResourceID = _ttoi(strImage);
					if(!SetBitmap(nResourceID, -1, TEXT("PNG")))
					{
						SetBitmap(nResourceID, -1, TEXT("BMP"));
					}
				}
				m_bImage = TRUE;
			}else
			if(!strColor.IsEmpty())
			{
				Color color = CDuiObject::StringToColor(strColor);
				SetColor(color);
			}
		}
	}

	return TRUE;
}

BOOL CSelectBox::SetBitmap(UINT nResourceID, int nIndex, CString strType)
{
	if(nIndex >= 0 && nIndex < (int)m_vecpImage.size())
	{
		Image *&pImage = m_vecpImage[nIndex];
		if(pImage != NULL)
		{
			delete pImage;
			pImage = NULL;
		}

		if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, pImage))
		{
			m_vecsizeImage[nIndex].SetSize(pImage->GetWidth(), pImage->GetHeight());
			UpdateControl(true);
			return true;
		}
	}
	else if(nIndex == -1 && (int)m_vecpImage.size() < m_nXCount * m_nYCount)
	{
		Image *pImage = NULL;
		if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, pImage))
		{
			CSize sizeImage(pImage->GetWidth(), pImage->GetHeight());

			m_vecpImage.push_back(pImage);
			m_vecsizeImage.push_back(sizeImage);

			UpdateControl(true);
			return true;
		}
	}

	return false;
}

BOOL CSelectBox::SetBitmap(CString strImage, int nIndex)
{
	if(nIndex >= 0 && nIndex < (int)m_vecpImage.size())
	{
		Image *&pImage = m_vecpImage[nIndex];
		if(pImage != NULL)
		{
			delete pImage;
			pImage = NULL;
		}

		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, pImage))
		{
			m_vecsizeImage[nIndex].SetSize(pImage->GetWidth(), pImage->GetHeight());
			UpdateControl(true);
			return true;
		}
	}
	else if(nIndex == -1 && (int)m_vecpImage.size() < m_nXCount * m_nYCount)
	{
		Image *pImage = NULL;
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, pImage))
		{
			CSize sizeImage(pImage->GetWidth(), pImage->GetHeight());

			m_vecpImage.push_back(pImage);
			m_vecsizeImage.push_back(sizeImage);

			UpdateControl(true);
			return true;
		}
	}

	return false;
}

BOOL CSelectBox::SetColor(Color clr, int nIndex/* = -1*/)
{
	if(nIndex >= 0 && nIndex < (int)m_vecclr.size())
	{
		m_vecclr[nIndex] = clr;
		UpdateControl(true);
		return true;
	}
	else if(nIndex == -1 && (int)m_vecclr.size() < m_nXCount * m_nYCount)
	{
		m_vecclr.push_back(clr);
		UpdateControl(true);
		return true;
	}

	return false;
}

BOOL CSelectBox::SetColor(Color clr[], int nColorCount)
{
	m_vecclr.clear();
	for(int i = 0; i < nColorCount; i++)
	{
		if(i >= m_nXCount * m_nYCount) break;

		m_vecclr.push_back(clr[i]);
	}

	m_bUpdate = false;
	return true;
}

void CSelectBox::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	int nItemWidth = nWidth / m_nXCount;
	int nItemHeight = nHeight / m_nYCount;
	int nXPos = (nWidth - nItemWidth * m_nXCount) / 2;		
	int nYPos = (nHeight - nItemHeight * m_nYCount) / 2;

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight * 3);

		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

		int nXPosTemp = nXPos;
		int nYPosTemp = nYPos;

		Graphics graphics(m_memDC);
		Pen pen(m_clrFrame, 1);

		for(int i = 0; i <= m_nYCount; i++)
		{
			graphics.DrawLine(&pen, nXPos, nYPosTemp, nXPos + nItemWidth * m_nXCount, nYPosTemp);
			nYPosTemp += nItemHeight;
		}

		for(int i = 0; i <= m_nXCount; i++)
		{
			graphics.DrawLine(&pen, nXPosTemp, nYPos, nXPosTemp, nYPos + nItemHeight * m_nYCount);
			nXPosTemp += nItemWidth;
		}

		if(m_bImage)
		{
			for(size_t i = 0; i < m_vecpImage.size(); i++)
			{
				if(m_vecpImage[i] != NULL)
				{
					graphics.DrawImage(m_vecpImage[i], Rect(nXPos + nItemWidth * (i % m_nXCount) + 1, nYPos + nItemHeight * (i / m_nXCount) + 1, nItemWidth - 1, nItemHeight - 1), 
						0, 0, m_vecsizeImage[i].cx, m_vecsizeImage[i].cy, UnitPixel);
				}
			}
		}
		else
		{
			for(size_t i = 0; i < m_vecclr.size(); i++)
			{
				SolidBrush brush(m_vecclr[i]);
				graphics.FillRectangle(&brush, nXPos + nItemWidth * (i % m_nXCount) + 1, nYPos + nItemHeight * (i / m_nXCount) + 1, nItemWidth - 1, nItemHeight - 1);
			}
		}

		m_memDC.BitBlt(0, nHeight, nWidth, nHeight, &m_memDC, 0, 0, SRCCOPY);
		m_memDC.BitBlt(0, nHeight * 2, nWidth, nHeight, &m_memDC, 0, 0, SRCCOPY);

		//选择
		SolidBrush brush(m_clrHover);
		nYPosTemp = nYPos + nHeight;
		if(m_bImage)
		{
			for(size_t i = 0; i < m_vecpImage.size(); i++)
			{			
				graphics.FillRectangle(&brush, nXPos + nItemWidth * (i % m_nXCount) + 1, nYPosTemp + nItemHeight * (i / m_nXCount) + 1, nItemWidth - 1, nItemHeight - 1);
			}
		}
		else
		{
			for(size_t i = 0; i < m_vecclr.size(); i++)
			{			
				graphics.FillRectangle(&brush, nXPos + nItemWidth * (i % m_nXCount) + 1, nYPosTemp + nItemHeight * (i / m_nXCount) + 1, nItemWidth - 1, nItemHeight - 1);
			}
		}

		int nLineWidth = m_bImage ? 2 : 1;
		//选中
		pen.SetColor(m_clrSelect);
		pen.SetWidth((Gdiplus::REAL)nLineWidth);

		nYPosTemp = nYPos + nHeight * 2;
		for(int i = 0; i < m_nYCount; i++)
		{
			nXPosTemp = nXPos;
			for(int j = 0; j < m_nXCount; j++)
			{				
				Rect rect(nXPosTemp + nLineWidth, nYPosTemp + nLineWidth, nItemWidth - 1 - nLineWidth, nItemHeight - 1 - nLineWidth);

				//绘制矩形
				graphics.DrawRectangles(&pen, &rect, 1);
				nXPosTemp += nItemWidth;
			}
			nYPosTemp += nItemHeight;
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width()+1, m_rc.Height()+1, &m_memDC, 0, 0, SRCCOPY);

	if(m_nXSelect != -1 && m_nYSelect != -1)
	{
		dc.BitBlt(m_rc.left + nXPos + m_nXSelect * nItemWidth, m_rc.top + nYPos + m_nYSelect * nItemHeight, nItemWidth, nItemHeight, &m_memDC, m_nXSelect * nItemWidth, nHeight * 2 + m_nYSelect * nItemHeight, SRCCOPY);
	}

	if(m_nXHover != -1 && m_nYHover != -1)
	{
		dc.BitBlt(m_rc.left + nXPos + m_nXHover * nItemWidth, m_rc.top + nYPos + m_nYHover * nItemHeight, nItemWidth, nItemHeight, &m_memDC, m_nXHover * nItemWidth, nHeight + m_nYHover * nItemHeight, SRCCOPY);
	}
}

BOOL CSelectBox::OnControlMouseMove(UINT nFlags, CPoint point)
{
	int nOldXHover = m_nXHover;
	int nOldYHover = m_nYHover;

	if (!m_bIsDisable)
	{
		int nWidth = m_rc.Width();
		int nHeight = m_rc.Height();
		int nItemWidth = nWidth / m_nXCount;
		int nItemHeight = nHeight / m_nYCount;
		int nXPos = (nWidth - nItemWidth * m_nXCount) / 2;		
		int nYPos = (nHeight - nItemHeight * m_nYCount) / 2;

		CRect rc = m_rc;
		rc.left += nXPos;
		rc.top += nYPos;
		rc.right = rc.left + nItemWidth * m_nXCount;
		rc.bottom = rc.top + nItemHeight * m_nYCount;

		if(rc.PtInRect(point))
		{
			m_nXHover = (point.x - rc.left) / nItemWidth;
			m_nYHover = (point.y - rc.top) / nItemHeight;

			if(m_nXHover == m_nXSelect && m_nYHover == m_nYSelect)
			{
				m_nXHover = -1;
				m_nYHover = -1;
			}
		}
		else
		{
			m_nXHover = -1;
			m_nYHover = -1;
		}
	}	

	if(nOldXHover != m_nXHover || nOldYHover != m_nYHover)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CSelectBox::OnControlLButtonDown(UINT nFlags, CPoint point)
{	
	int nOldXHover = m_nXHover;
	int nOldYHover = m_nYHover;
	int nOldXSelect = m_nXSelect;
	int nOldYSelect = m_nYSelect;

	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			if(m_nXHover != -1 && m_nYHover != -1)
			{
				m_nXSelect = m_nXHover;
				m_nYSelect = m_nYHover;
				m_nXHover = -1;
				m_nYHover = -1;
			}

			if(nOldXSelect != m_nXSelect || m_nYSelect != nOldYSelect)
			{
				SelectInfo selectInfo;
				if(m_bImage)
				{
					selectInfo.nType = 0;
					selectInfo.uIndex = m_nXCount * m_nYSelect + m_nXSelect;
					m_nSelectIndex = selectInfo.uIndex;
				}
				else
				{
					selectInfo.nType = 1;
					selectInfo.clr = m_vecclr[m_nXCount * m_nYSelect + m_nXSelect];
					m_clrCurSelect = selectInfo.clr;
				}
				SendMessage(MSG_BUTTON_DOWN, 0,(LPARAM)&selectInfo);
			}
		}
	}

	if(nOldXHover != m_nXHover || nOldYHover != m_nYHover || nOldXSelect != m_nXSelect ||  nOldYSelect != m_nYSelect)
	{
		UpdateControl();
		return true;
	}
	return false;
}