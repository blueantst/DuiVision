#include "StdAfx.h"
#include "Scroll.h"

#define	BT_UP		1
#define	BT_DOWN		2

CScrollV::CScrollV(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBaseFont(hWnd, pDuiObject)
{
	m_rcBlock.SetRectEmpty();

	m_bShowScroll = false;
	m_bHover = false;
	m_enButtonState = enBSNormal;
	m_nDownTop = -1;
	m_bAutoCalcRange = FALSE;

	m_nArrowLen = 0;
	
	CRect rcButton = CRect(0,0,0,0);
	rcButton.bottom = rcButton.top + m_nArrowLen;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CImageButton(hWnd, this, BT_UP, rcButton);
	pControlBase->SetTabStop(FALSE);
	m_pControlUpImage = pControlBase;
 	m_vecControl.push_back(pControlBase);
 
 	rcButton = CRect(0,0,0,0);
 	rcButton.top = rcButton.bottom - m_nArrowLen;
 	pControlBase = new CImageButton(hWnd, this, BT_DOWN, rcButton);
	pControlBase->SetTabStop(FALSE);
	m_pControlDownImage = pControlBase;
 	m_vecControl.push_back(pControlBase);

	// 默认值
	m_nMaxRange = 400;
	m_nCurrentPos = 0;
	m_nPageRange = 0;
	m_nRowRange = 10;
	SetRange();
}

CScrollV::CScrollV(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible/* = TRUE*/)
			: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, TEXT(""), bIsVisible, FALSE)
{
	m_rcBlock.SetRectEmpty();

	m_bShowScroll = false;
	m_bHover = false;
	m_enButtonState = enBSNormal;
	m_nDownTop = -1;
	m_bAutoCalcRange = FALSE;

	m_nArrowLen = 0;
	
	CRect rcButton = rc;
	rcButton.bottom = rc.top + m_nArrowLen;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CImageButton(hWnd, this, BT_UP, rcButton);
	pControlBase->SetTabStop(FALSE);
 	m_vecControl.push_back(pControlBase);
 
 	rcButton = rc;
 	rcButton.top = rc.bottom - m_nArrowLen;
 	pControlBase = new CImageButton(hWnd, this, BT_DOWN, rcButton);
	pControlBase->SetTabStop(FALSE);
 	m_vecControl.push_back(pControlBase);

	// 默认值
	m_nMaxRange = 400;
	m_nCurrentPos = 0;
	m_nPageRange = 0;
	m_nRowRange = 10;
	SetRange();
}

CScrollV::~CScrollV(void)
{
}

BOOL CScrollV::SetBitmap(UINT nResourceID, CString strType)
{
	if(m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, m_pImage))
	{
		m_sizeImage.SetSize(m_pImage->GetWidth() / 4, m_pImage->GetHeight() / 3);
		return true;
	}
	return false;
}

BOOL CScrollV::SetBitmap(CString strImage)
{
	if(m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, m_pImage))
	{
		m_sizeImage.SetSize(m_pImage->GetWidth() / 4, m_pImage->GetHeight() / 3);
		return true;
	}
	return false;
}

// 从XML设置上箭头图片信息属性
HRESULT CScrollV::OnAttributeUpImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// 加载图片文件
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!((CImageButton*)m_pControlUpImage)->SetBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = _wtoi(strSkin);
		if(!((CImageButton*)m_pControlUpImage)->SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!((CImageButton*)m_pControlUpImage)->SetBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	m_nArrowLen = 20;

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置下箭头图片信息属性
HRESULT CScrollV::OnAttributeDownImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// 加载图片文件
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!((CImageButton*)m_pControlDownImage)->SetBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = _wtoi(strSkin);
		if(!((CImageButton*)m_pControlDownImage)->SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!((CImageButton*)m_pControlDownImage)->SetBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	m_nArrowLen = 20;

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置属性
HRESULT CScrollV::OnAttributeMaxRange(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_nMaxRange = _wtoi(strValue);
	SetRange();

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置属性
HRESULT CScrollV::OnAttributeCurrentPos(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_nCurrentPos = _wtoi(strValue);
	SetRange();

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置属性
HRESULT CScrollV::OnAttributePageRange(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_nPageRange = _wtoi(strValue);
	SetRange();

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置属性
HRESULT CScrollV::OnAttributeRowRange(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_nRowRange = _wtoi(strValue);
	SetRange();

	return bLoading?S_FALSE:S_OK;
}

// 设置滚动条当前位置
BOOL CScrollV::SetScrollCurrentPos(int nCurrentPos)
{
	m_nCurrentPos = nCurrentPos;
	SetRange();
	return TRUE;
}

// 设置滚动条最大值
BOOL CScrollV::SetScrollMaxRange(int nMaxRange)
{
	m_nMaxRange = nMaxRange;
	if(m_bAutoCalcRange)
	{
		// 如果是自动计算滚动页范围,则重置页范围,好重新计算
		m_nPageRange = 0;
	}
	SetRange();
	return TRUE;
}

// 设置滚动条行和页的像素数
BOOL CScrollV::SetScrollSize(int nPageRange, int nRowRange)
{
	m_nPageRange = nPageRange;
	if(m_nPageRange != 0)
	{
		m_bAutoCalcRange = FALSE;
	}
	if(nRowRange != 0)
	{
		m_nRowRange = nRowRange;
	}
	SetRange();
	return TRUE;
}

BOOL CScrollV::SetScrollInfo(int nMaxRange, int nCurrentPos, int nPageRange, int nRowRange)
{
	m_nMaxRange = nMaxRange;
	m_nCurrentPos = nCurrentPos;
	if(nPageRange != 0)
	{
		m_nPageRange = nPageRange;
		m_bAutoCalcRange = FALSE;
	}
	if(nRowRange != 0)
	{
		m_nRowRange = nRowRange;
	}
	SetRange();
	return TRUE;
}

// 滚动指定的行数
BOOL CScrollV::ScrollRow(int nRow)
{
	int nNewPos = m_nCurrentPos + nRow * m_nRowRange;
	if(nNewPos < 0)
	{
		nNewPos = 0;
	}else
	if(nNewPos > m_nMaxRange)
	{
		nNewPos = m_nMaxRange;
	}
	BOOL bRet = (nNewPos != m_nCurrentPos);
	m_nCurrentPos = nNewPos;
	SetRange();
	return bRet;
}

// 鼠标移动
BOOL  CScrollV::OnControlMouseMove(UINT nFlags, CPoint point)
{		
	if(!m_bShowScroll)
	{
		return FALSE;
	}
	enumButtonState buttonState = m_enButtonState;
	BOOL bHover = m_bHover;
	if (!m_bIsDisable)
	{
		if(m_enButtonState != enBSDown)
		{
			if(!m_bMouseDown)
			{
				if(m_rc.PtInRect(point))
				{
					m_bHover = true;
					if(m_rcBlock.PtInRect(point))
					{
						m_enButtonState = enBSHover;
					}
					else
					{
						m_enButtonState = enBSNormal;
					}
				}
				else
				{
					m_bHover = false;
					m_enButtonState = enBSNormal;
				}
			}
		}
		else
		{
			// 鼠标按着滚动块,拖动的情况下,移动滚动块范围
			m_nDownTop += MoveRange(point.y - m_nDownTop);
			return true;
		}
	}

	if(buttonState != m_enButtonState || !m_bUpdate || bHover != m_bHover)
	{
		SetUpdate(FALSE);
		UpdateControl();
		return true;
	}
	return false;
}

// 鼠标左键按下
BOOL CScrollV::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bShowScroll)
	{
		return FALSE;
	}

	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rcBlock.PtInRect(point))
		{
			// 如果在滚动块内,则记录鼠标位置
			m_enButtonState = enBSDown;
			m_nDownTop = point.y;
		}
		else
		{
			// 不在滚动块内,则移动滚动块
			int nRangeHeight = m_rc.Height() - m_nArrowLen*2;
			int nMove = m_nPageRange * nRangeHeight / m_nMaxRange;

			return MoveRange(point.y < m_rcBlock.top ? -nMove : nMove);
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

// 鼠标左键放开
BOOL  CScrollV::OnControlLButtonUp(UINT nFlags, CPoint point)
{	
	if(!m_bShowScroll)
	{
		return FALSE;
	}

	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{		
		m_nDownTop = -1;
		if(m_rcBlock.PtInRect(point))
		{
			m_enButtonState = enBSHover;
		}
		else
		{
			m_enButtonState = enBSNormal;
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

void CScrollV::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{		
		UpdateMemDC(dc, nWidth, nHeight);

		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);		
		
		if(m_bShowScroll)
		{
			CRect  rcTemp;
			//滚动条背景
			if(m_bHover)
			{
				rcTemp.SetRect(0, m_nArrowLen, nWidth, nHeight -  m_nArrowLen);
				DrawRange(m_memDC, rcTemp, 0);
			}

			//滚动块
			rcTemp.SetRect(m_rcBlock.left - m_rc.left, m_rcBlock.top - m_rc.top,m_rcBlock.left - m_rc.left +  m_rcBlock.Width(), m_rcBlock.top - m_rc.top + m_rcBlock.Height());
			int nXPos = m_sizeImage.cx;
			if(m_enButtonState == enBSHover)
			{
				nXPos = m_sizeImage.cx * 2;
			}
			else if(m_enButtonState == enBSDown)
			{
				nXPos = m_sizeImage.cx * 3;
			}
			DrawRange(m_memDC, rcTemp, nXPos);
		}
	}	

	dc.BitBlt(m_rc.left, m_rc.top, nWidth, nHeight, &m_memDC, 0, 0, SRCCOPY);
}

void CScrollV::DrawRange(CDC &dc, CRect rc, int nXPos)
{
	Graphics graphics(dc);

	CRect rcTemp;
	rcTemp.SetRect(rc.left, rc.top, rc.left + rc.Width(), rc.top + m_sizeImage.cy);
	DrawImageFrame(graphics, m_pImage, rcTemp, nXPos, 0, m_sizeImage.cx, m_sizeImage.cy, 2);

	rcTemp.SetRect(rc.left, rc.top + m_sizeImage.cy, rc.left + rc.Width(), rc.bottom - m_sizeImage.cy);
	graphics.DrawImage(m_pImage, Rect(rcTemp.left, rcTemp.top, rcTemp.Width(), rcTemp.Height()), 
		nXPos, m_sizeImage.cy, m_sizeImage.cx,m_sizeImage.cy , UnitPixel);

	rcTemp.SetRect(rc.left, rc.bottom - m_sizeImage.cy, rc.left + rc.Width(), rc.bottom);
	DrawImageFrame(graphics, m_pImage, rcTemp, nXPos, m_sizeImage.cy * 2, m_sizeImage.cx, m_sizeImage.cy, 2);
}

// 移动滚动块
int CScrollV::MoveRange(int nMove)
{
	if(nMove != 0)
	{
		CRect rcOldBlock = m_rcBlock;
		int nOldnMove = nMove;
		
		if(nMove < 0)
		{
			if(m_rcBlock.top + nMove < m_rc.top + m_nArrowLen)
			{
				nMove = (m_rc.top + m_nArrowLen) - m_rcBlock.top;
			}
		}
		else
		{
			if(m_rcBlock.bottom + nMove > m_rc.bottom - m_nArrowLen)
			{
				nMove = m_rc.bottom - m_nArrowLen - m_rcBlock.bottom;
			}
		}
		
		if(nMove != 0)
		{
			m_rcBlock.OffsetRect(0, nMove);

			int nRangeHeight = m_rc.Height() - m_nArrowLen*2;
			int nBlockHeight = (int)__max(m_nArrowLen/2, ((double)m_nPageRange / m_nMaxRange) * nRangeHeight);
			if(nBlockHeight < 100)
			{
				// 如果计算出的滚动快太小,则设置一个滚动块长度的下限值
				nBlockHeight = (int)__min(100, nRangeHeight-50);
			}
			m_nCurrentPos = (m_rcBlock.top - m_rc.top - m_nArrowLen)  * m_nMaxRange / (nRangeHeight - nBlockHeight);

			UpdateControl(true);

			SendMessage(MSG_SCROLL_CHANGE, m_nCurrentPos, 0);
		}

		return nMove;
	}

	return nMove;
}

// 计算位置信息,以及是否显示滚动条
int CScrollV::SetRange()
{
	if(m_nPageRange == 0)
	{
		// 如果没有设置页滚动范围,则设置为滚动条高度
		m_nPageRange = m_rc.Height();
		// 如果总高度大于页滚动范围的10倍,则设置页滚动范围为总高度的1/10,避免滚动太慢
		if(m_nMaxRange > (m_nPageRange * 10))
		{
			m_nPageRange = m_nMaxRange / 10;
		}
		m_bAutoCalcRange = TRUE;
	}
	int nRangeHeight = m_rc.Height() - m_nArrowLen*2;
	int nBlockHeight = (int)__max(m_nArrowLen/2, ((double)m_nPageRange / m_nMaxRange) * nRangeHeight);
	if(nBlockHeight < 100)
	{
		// 如果计算出的滚动快太小,则设置一个滚动块长度的下限值
		nBlockHeight = (int)__min(100, nRangeHeight-50);
	}
	int nRangeTop = (int)__min(m_rc.bottom - m_nArrowLen - m_nArrowLen/2, __max(m_rc.top + m_nArrowLen, m_rc.top + m_nArrowLen + ((double)m_nCurrentPos / m_nMaxRange) * (nRangeHeight - nBlockHeight)));

	m_rcBlock.SetRect(m_rc.left, nRangeTop, m_rc.right, nRangeTop + nBlockHeight);

	if(m_nPageRange >= m_nMaxRange)
	{
		m_bShowScroll = false;
	}
	else
	{
		m_bShowScroll = true;
	}

	return m_nCurrentPos;
}

void CScrollV::SetControlRect(CRect rc)
{
	m_rc = rc;
	SetRange();
	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetVisible(m_bShowScroll);

			UINT uControlID = pControlBase->GetControlID();
			if (BT_UP == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.bottom = m_rc.top + m_nArrowLen;
			}
			else if(BT_DOWN == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.top = m_rc.bottom - m_nArrowLen;
			}
			else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}
}

// 消息响应
LRESULT CScrollV::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(uID == BT_UP)
	{
		if(MSG_BUTTON_UP == lParam)
		{
			CRect rcUpdate;
			rcUpdate.SetRectEmpty();
			MoveRange(-m_nRowRange);
		}
	}
	else if (uID == BT_DOWN)
	{
		if(MSG_BUTTON_UP == lParam)
		{
			CRect rcUpdate;
			rcUpdate.SetRectEmpty();
			MoveRange(m_nRowRange);
		}
	}
	return 0L; 
}