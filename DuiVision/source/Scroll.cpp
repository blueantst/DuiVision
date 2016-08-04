#include "StdAfx.h"
#include "Scroll.h"

#define	BT_UP		1
#define	BT_DOWN		2
#define	BT_LEFT		3
#define	BT_RIGHT	4

CDuiScroll::CDuiScroll(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBaseFont(hWnd, pDuiObject)
{
	m_nScrollType = SCROLL_TYPE_VERTICAL;
	m_rcBlock.SetRectEmpty();

	m_bShowScroll = false;
	m_bHover = false;
	m_enButtonState = enBSNormal;
	m_bAutoCalcRange = FALSE;
	m_nArrowLen = 0;

	// Ĭ��ֵ
	m_nMaxRange = 400;
	m_nCurrentPos = 0;
	m_nPageRange = 0;
	m_nRowRange = 10;
}

CDuiScroll::CDuiScroll(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible/* = TRUE*/)
			: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, TEXT(""), bIsVisible, FALSE)
{
	m_nScrollType = SCROLL_TYPE_VERTICAL;
	m_rcBlock.SetRectEmpty();

	m_bShowScroll = false;
	m_bHover = false;
	m_enButtonState = enBSNormal;
	m_bAutoCalcRange = FALSE;
	m_nArrowLen = 0;

	// Ĭ��ֵ
	m_nMaxRange = 400;
	m_nCurrentPos = 0;
	m_nPageRange = 0;
	m_nRowRange = 10;
}

CDuiScroll::~CDuiScroll(void)
{
}

// ��XML��������
HRESULT CDuiScroll::OnAttributeMaxRange(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_nMaxRange = _ttoi(strValue);
	SetRange();

	return bLoading?S_FALSE:S_OK;
}

// ��XML��������
HRESULT CDuiScroll::OnAttributeCurrentPos(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_nCurrentPos = _ttoi(strValue);
	SetRange();

	return bLoading?S_FALSE:S_OK;
}

// ��XML��������
HRESULT CDuiScroll::OnAttributePageRange(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_nPageRange = _ttoi(strValue);
	SetRange();

	return bLoading?S_FALSE:S_OK;
}

// ��XML��������
HRESULT CDuiScroll::OnAttributeRowRange(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_nRowRange = _ttoi(strValue);
	SetRange();

	return bLoading?S_FALSE:S_OK;
}

// ���ù�������ǰλ��
BOOL CDuiScroll::SetScrollCurrentPos(int nCurrentPos)
{
	m_nCurrentPos = nCurrentPos;
	SetRange();
	return TRUE;
}

// ���ù��������ֵ
BOOL CDuiScroll::SetScrollMaxRange(int nMaxRange)
{
	m_nMaxRange = nMaxRange;
	if(m_bAutoCalcRange)
	{
		// ������Զ��������ҳ��Χ,������ҳ��Χ,�����¼���
		m_nPageRange = 0;
	}
	SetRange();
	return TRUE;
}

// ���ù������к�ҳ��������
BOOL CDuiScroll::SetScrollSize(int nPageRange, int nRowRange)
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

BOOL CDuiScroll::SetScrollInfo(int nMaxRange, int nCurrentPos, int nPageRange, int nRowRange)
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

// ����ָ��������
BOOL CDuiScroll::ScrollRow(int nRow)
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

void CDuiScroll::SetControlRect(CRect rc)
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
			}else
			if(BT_DOWN == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.top = m_rc.bottom - m_nArrowLen;
			}else
			if (BT_LEFT == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.right = m_rc.left + m_nArrowLen;
			}else
			if(BT_RIGHT == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.left = m_rc.right - m_nArrowLen;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}
}

// ��Ϣ��Ӧ
LRESULT CDuiScroll::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == BT_UP) && (MSG_BUTTON_UP == lParam))
	{
		MoveRange(-m_nRowRange);
	}else
	if((uID == BT_DOWN) && (MSG_BUTTON_UP == lParam))
	{
		MoveRange(m_nRowRange);
	}else
	if((uID == BT_LEFT) && (MSG_BUTTON_UP == lParam))
	{
		MoveRange(-m_nRowRange);
	}else
	if((uID == BT_RIGHT) && (MSG_BUTTON_UP == lParam))
	{
		MoveRange(m_nRowRange);
	}
	return 0L; 
}

void CDuiScroll::DrawControl(CDC &dc, CRect rcUpdate)
{
}

int CDuiScroll::MoveRange(int nMove)
{
	return 0;
}

int CDuiScroll::SetRange()
{
	return 0;
}

//////////////////////////////////////////////////////////////////
// CDuiScrollVertical- ��ֱ���������
CDuiScrollVertical::CDuiScrollVertical(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiScroll(hWnd, pDuiObject)
{
	m_nScrollType = SCROLL_TYPE_VERTICAL;

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

	m_nDownTop = -1;

	SetRange();
}

CDuiScrollVertical::CDuiScrollVertical(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible/* = TRUE*/)
			: CDuiScroll(hWnd, pDuiObject, uControlID, rc, bIsVisible)
{
	m_nScrollType = SCROLL_TYPE_VERTICAL;
	
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

	m_nDownTop = -1;

	SetRange();
}

CDuiScrollVertical::~CDuiScrollVertical(void)
{
}

BOOL CDuiScrollVertical::SetBitmap(UINT nResourceID, CString strType)
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

BOOL CDuiScrollVertical::SetBitmap(CString strImage)
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

// ��XML�����ϼ�ͷͼƬ��Ϣ����
HRESULT CDuiScrollVertical::OnAttributeUpImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
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
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _ttoi(strSkin);
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

// ��XML�����¼�ͷͼƬ��Ϣ����
HRESULT CDuiScrollVertical::OnAttributeDownImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
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
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _ttoi(strSkin);
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

// �ƶ�������
int CDuiScrollVertical::MoveRange(int nMove)
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
				// ���������Ĺ�����̫С,������һ�������鳤�ȵ�����ֵ
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

// ����λ����Ϣ,�Լ��Ƿ���ʾ������
int CDuiScrollVertical::SetRange()
{
	if(m_nPageRange == 0)
	{
		// ���û������ҳ������Χ,������Ϊ�������߶�
		m_nPageRange = m_rc.Height();
		// ����ܸ߶ȴ���ҳ������Χ��10��,������ҳ������ΧΪ�ܸ߶ȵ�1/10,�������̫��
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
		// ���������Ĺ�����̫С,������һ�������鳤�ȵ�����ֵ
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

// ����ƶ�
BOOL  CDuiScrollVertical::OnControlMouseMove(UINT nFlags, CPoint point)
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
			// ��갴�Ź�����,�϶��������,�ƶ������鷶Χ
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

// ����������
BOOL CDuiScrollVertical::OnControlLButtonDown(UINT nFlags, CPoint point)
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
			// ����ڹ�������,���¼���λ��
			m_enButtonState = enBSDown;
			m_nDownTop = point.y;
		}
		else
		{
			// ���ڹ�������,���ƶ�������
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

// �������ſ�
BOOL  CDuiScrollVertical::OnControlLButtonUp(UINT nFlags, CPoint point)
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

void CDuiScrollVertical::DrawControl(CDC &dc, CRect rcUpdate)
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
			//����������
			if(m_bHover)
			{
				rcTemp.SetRect(0, m_nArrowLen, nWidth, nHeight -  m_nArrowLen);
				DrawRange(m_memDC, rcTemp, 0);
			}

			//������
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

// ������������,nXPos��ʾͼƬ����ƫ��
void CDuiScrollVertical::DrawRange(CDC &dc, CRect rc, int nXPos)
{
	Graphics graphics(dc);

	// ������ԭʼͼƬ�����Ϊ3��,�ֱ��ǹ�������/��/����������
	CRect rcTemp;
	rcTemp.SetRect(rc.left, rc.top, rc.left + rc.Width(), rc.top + m_sizeImage.cy);
	DrawImageFrame(graphics, m_pImage, rcTemp, nXPos, 0, m_sizeImage.cx, m_sizeImage.cy, 2);

	rcTemp.SetRect(rc.left, rc.top + m_sizeImage.cy, rc.left + rc.Width(), rc.bottom - m_sizeImage.cy);
	graphics.DrawImage(m_pImage, Rect(rcTemp.left, rcTemp.top, rcTemp.Width(), rcTemp.Height()), 
		nXPos, m_sizeImage.cy, m_sizeImage.cx,m_sizeImage.cy , UnitPixel);

	rcTemp.SetRect(rc.left, rc.bottom - m_sizeImage.cy, rc.left + rc.Width(), rc.bottom);
	DrawImageFrame(graphics, m_pImage, rcTemp, nXPos, m_sizeImage.cy * 2, m_sizeImage.cx, m_sizeImage.cy, 2);
}


//////////////////////////////////////////////////////////////////
// CDuiScrollHorizontal- ˮƽ���������
CDuiScrollHorizontal::CDuiScrollHorizontal(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiScroll(hWnd, pDuiObject)
{
	m_nScrollType = SCROLL_TYPE_VERTICAL;

	CRect rcButton = CRect(0,0,0,0);
	rcButton.right = rcButton.left + m_nArrowLen;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CImageButton(hWnd, this, BT_UP, rcButton);
	pControlBase->SetTabStop(FALSE);
	m_pControlUpImage = pControlBase;
 	m_vecControl.push_back(pControlBase);
 
 	rcButton = CRect(0,0,0,0);
 	rcButton.left = rcButton.right - m_nArrowLen;
 	pControlBase = new CImageButton(hWnd, this, BT_DOWN, rcButton);
	pControlBase->SetTabStop(FALSE);
	m_pControlDownImage = pControlBase;
 	m_vecControl.push_back(pControlBase);

	m_nDownLeft = -1;

	SetRange();
}

CDuiScrollHorizontal::CDuiScrollHorizontal(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible/* = TRUE*/)
			: CDuiScroll(hWnd, pDuiObject, uControlID, rc, bIsVisible)
{
	m_nScrollType = SCROLL_TYPE_VERTICAL;
	
	CRect rcButton = rc;
	rcButton.right = rcButton.left + m_nArrowLen;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CImageButton(hWnd, this, BT_UP, rcButton);
	pControlBase->SetTabStop(FALSE);
 	m_vecControl.push_back(pControlBase);
 
 	rcButton = rc;
 	rcButton.left = rcButton.right - m_nArrowLen;
 	pControlBase = new CImageButton(hWnd, this, BT_DOWN, rcButton);
	pControlBase->SetTabStop(FALSE);
 	m_vecControl.push_back(pControlBase);

	m_nDownLeft = -1;

	SetRange();
}

CDuiScrollHorizontal::~CDuiScrollHorizontal(void)
{
}

BOOL CDuiScrollHorizontal::SetBitmap(UINT nResourceID, CString strType)
{
	if(m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, m_pImage))
	{
		m_sizeImage.SetSize(m_pImage->GetWidth() / 3, m_pImage->GetHeight() / 4);
		return true;
	}
	return false;
}

BOOL CDuiScrollHorizontal::SetBitmap(CString strImage)
{
	if(m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, m_pImage))
	{
		m_sizeImage.SetSize(m_pImage->GetWidth() / 3, m_pImage->GetHeight() / 4);
		return true;
	}
	return false;
}

// ��XML�������ͷͼƬ��Ϣ����
HRESULT CDuiScrollHorizontal::OnAttributeLeftImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!((CImageButton*)m_pControlLeftImage)->SetBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _ttoi(strSkin);
		if(!((CImageButton*)m_pControlLeftImage)->SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!((CImageButton*)m_pControlLeftImage)->SetBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	m_nArrowLen = 20;

	return bLoading?S_FALSE:S_OK;
}

// ��XML�����Ҽ�ͷͼƬ��Ϣ����
HRESULT CDuiScrollHorizontal::OnAttributeRightImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!((CImageButton*)m_pControlRightImage)->SetBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _ttoi(strSkin);
		if(!((CImageButton*)m_pControlRightImage)->SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!((CImageButton*)m_pControlRightImage)->SetBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	m_nArrowLen = 20;

	return bLoading?S_FALSE:S_OK;
}

// �ƶ�������
int CDuiScrollHorizontal::MoveRange(int nMove)
{
	if(nMove != 0)
	{
		CRect rcOldBlock = m_rcBlock;
		int nOldnMove = nMove;
		
		if(nMove < 0)
		{
			if(m_rcBlock.left + nMove < m_rc.left + m_nArrowLen)
			{
				nMove = (m_rc.left + m_nArrowLen) - m_rcBlock.left;
			}
		}
		else
		{
			if(m_rcBlock.right + nMove > m_rc.right - m_nArrowLen)
			{
				nMove = m_rc.right - m_nArrowLen - m_rcBlock.right;
			}
		}
		
		if(nMove != 0)
		{
			m_rcBlock.OffsetRect(nMove, 0);

			int nRangeWidth = m_rc.Width() - m_nArrowLen*2;
			int nBlockWidth = (int)__max(m_nArrowLen/2, ((double)m_nPageRange / m_nMaxRange) * nRangeWidth);
			if(nBlockWidth < 100)
			{
				// ���������Ĺ�����̫С,������һ�������鳤�ȵ�����ֵ
				nBlockWidth = (int)__min(100, nRangeWidth-50);
			}
			m_nCurrentPos = (m_rcBlock.left - m_rc.left - m_nArrowLen)  * m_nMaxRange / (nRangeWidth - nBlockWidth);

			UpdateControl(true);

			SendMessage(MSG_SCROLL_CHANGE, m_nCurrentPos, 0);
		}

		return nMove;
	}

	return nMove;
}

// ����λ����Ϣ,�Լ��Ƿ���ʾ������
int CDuiScrollHorizontal::SetRange()
{
	if(m_nPageRange == 0)
	{
		// ���û������ҳ������Χ,������Ϊ���������
		m_nPageRange = m_rc.Width();
		// ����ܸ߶ȴ���ҳ������Χ��10��,������ҳ������ΧΪ�ܸ߶ȵ�1/10,�������̫��
		if(m_nMaxRange > (m_nPageRange * 10))
		{
			m_nPageRange = m_nMaxRange / 10;
		}
		m_bAutoCalcRange = TRUE;
	}
	int nRangeWidth = m_rc.Width() - m_nArrowLen*2;
	int nBlockWidth = (int)__max(m_nArrowLen/2, ((double)m_nPageRange / m_nMaxRange) * nRangeWidth);
	if(nBlockWidth < 100)
	{
		// ���������Ĺ�����̫С,������һ�������鳤�ȵ�����ֵ
		nBlockWidth = (int)__min(100, nRangeWidth-50);
	}
	int nRangeLeft = (int)__min(m_rc.right - m_nArrowLen - m_nArrowLen/2, __max(m_rc.left + m_nArrowLen, m_rc.left + m_nArrowLen + ((double)m_nCurrentPos / m_nMaxRange) * (nRangeWidth - nBlockWidth)));

	m_rcBlock.SetRect(nRangeLeft, m_rc.top, nRangeLeft + nBlockWidth, m_rc.bottom);

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

// ����ƶ�
BOOL  CDuiScrollHorizontal::OnControlMouseMove(UINT nFlags, CPoint point)
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
			// ��갴�Ź�����,�϶��������,�ƶ������鷶Χ
			m_nDownLeft += MoveRange(point.x - m_nDownLeft);
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

// ����������
BOOL CDuiScrollHorizontal::OnControlLButtonDown(UINT nFlags, CPoint point)
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
			// ����ڹ�������,���¼���λ��
			m_enButtonState = enBSDown;
			m_nDownLeft = point.x;
		}
		else
		{
			// ���ڹ�������,���ƶ�������
			int nRangeWidth = m_rc.Width() - m_nArrowLen*2;
			int nMove = m_nPageRange * nRangeWidth / m_nMaxRange;

			return MoveRange(point.x < m_rcBlock.left ? -nMove : nMove);
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

// �������ſ�
BOOL  CDuiScrollHorizontal::OnControlLButtonUp(UINT nFlags, CPoint point)
{	
	if(!m_bShowScroll)
	{
		return FALSE;
	}

	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{		
		m_nDownLeft = -1;
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

void CDuiScrollHorizontal::DrawControl(CDC &dc, CRect rcUpdate)
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
			//����������
			if(m_bHover)
			{
				rcTemp.SetRect(m_nArrowLen, 0, nWidth - m_nArrowLen, nHeight);
				DrawRange(m_memDC, rcTemp, 0);
			}

			//������
			rcTemp.SetRect(m_rcBlock.left - m_rc.left, m_rcBlock.top - m_rc.top,m_rcBlock.left - m_rc.left +  m_rcBlock.Width(), m_rcBlock.top - m_rc.top + m_rcBlock.Height());
			int nYPos = m_sizeImage.cy;
			if(m_enButtonState == enBSHover)
			{
				nYPos = m_sizeImage.cy * 2;
			}
			else if(m_enButtonState == enBSDown)
			{
				nYPos = m_sizeImage.cy * 3;
			}
			DrawRange(m_memDC, rcTemp, nYPos);
		}
	}	

	dc.BitBlt(m_rc.left, m_rc.top, nWidth, nHeight, &m_memDC, 0, 0, SRCCOPY);
}

// ������������,nYPos��ʾͼƬ����ƫ��
void CDuiScrollHorizontal::DrawRange(CDC &dc, CRect rc, int nYPos)
{
	Graphics graphics(dc);

	// ������ԭʼͼƬ�����Ϊ3��,�ֱ��ǹ�������/��/����������
	CRect rcTemp;
	rcTemp.SetRect(rc.left, rc.top, rc.left + m_sizeImage.cx,rc.top + rc.Height());
	DrawImageFrame(graphics, m_pImage, rcTemp, 0, nYPos, m_sizeImage.cx, m_sizeImage.cy, 2);

	rcTemp.SetRect(rc.left + m_sizeImage.cx, rc.top, rc.right - m_sizeImage.cx, rc.top + rc.Height());
	graphics.DrawImage(m_pImage, Rect(rcTemp.left, rcTemp.top, rcTemp.Width(), rcTemp.Height()), 
		m_sizeImage.cx, nYPos, m_sizeImage.cx, m_sizeImage.cy , UnitPixel);

	rcTemp.SetRect(rc.right - m_sizeImage.cx, rc.top, rc.right, rc.top + rc.Height());
	DrawImageFrame(graphics, m_pImage, rcTemp, m_sizeImage.cx * 2, nYPos, m_sizeImage.cx, m_sizeImage.cy, 2);
}