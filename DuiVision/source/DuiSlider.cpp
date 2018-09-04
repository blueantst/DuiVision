#include "StdAfx.h"
#include "DuiSlider.h"

CDuiSlider::CDuiSlider(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBaseFont(hWnd, pDuiObject)
{
	m_bHover = false;
	m_enButtonState = enBSNormal;

	m_bRunTime = false;
	m_nMaxProgress = 100;

	SetBitmapCount(2);

	m_pImageBackGround = NULL;
	m_sizeBackGround = CSize(0, 0);
	m_pImageForeGround = NULL;
	m_sizeForeGround = CSize(0, 0);
	m_pImageThumb = NULL;
	m_sizeThumb = CSize(0, 0);
	m_nSliderHeight = 0;
	m_nThumbWidth = 0;
	m_nThumbHeight = 0;
	m_nThumbTop = 0;
	m_nHeadLength = 0;
	m_rcThumb = CRect(0, 0, 0, 0);

	m_clrText = Color(254, 128, 128, 128);
	m_uAlignment = Align_Center;
	m_uVAlignment = VAlign_Middle;
	m_bShowText = FALSE;

	m_nProgress = 0;
	SetProgress(0);
}

CDuiSlider::CDuiSlider(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress/* = 0*/, 
					 BOOL bIsVisible/* = TRUE*/, BOOL bIsDisable/* = FALSE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, TEXT(""), bIsVisible, bIsDisable)
{
	m_bHover = false;
	m_enButtonState = enBSNormal;

	m_bRunTime = false;
	m_nMaxProgress = 100;

	SetBitmapCount(2);

	m_pImageBackGround = NULL;
	m_sizeBackGround = CSize(0, 0);
	m_pImageForeGround = NULL;
	m_sizeForeGround = CSize(0, 0);
	m_pImageThumb = NULL;
	m_sizeThumb = CSize(0, 0);
	m_nSliderHeight = 0;
	m_nThumbWidth = 0;
	m_nThumbHeight = 0;
	m_nThumbTop = 0;
	m_nHeadLength = 0;
	m_rcThumb = CRect(0, 0, 0, 0);

	m_clrText = Color(254, 128, 128, 128);
	m_uAlignment = Align_Center;
	m_uVAlignment = VAlign_Middle;
	m_bShowText = FALSE;

	m_nProgress = 0;
	SetProgress(nProgress);
}

CDuiSlider::~CDuiSlider(void)
{
	if(m_pImageBackGround != NULL)
	{
		delete m_pImageBackGround;
		m_pImageBackGround = NULL;
	}
	if(m_pImageForeGround != NULL)
	{
		delete m_pImageForeGround;
		m_pImageForeGround = NULL;
	}
	if(m_pImageThumb != NULL)
	{
		delete m_pImageThumb;
		m_pImageThumb = NULL;
	}
}

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiSlider, BackGround, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiSlider, ForeGround, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiSlider, Thumb, 4)

int CDuiSlider::SetProgress(int nProgress)
{
	if(GetDisable())
	{
		return m_nProgress;
	}

	if(nProgress > m_nMaxProgress)
	{
		// ����ֵ�������ֵ,�Զ�����Ϊ���ֵ
		nProgress = m_nMaxProgress;
	}

	if(nProgress >= 0 && nProgress <= m_nMaxProgress && m_nProgress != nProgress)
	{
		m_nProgress = nProgress;
		if(GetVisible())
		{
			UpdateControl(true);
		}
	}
	return m_nProgress;
}

void CDuiSlider::SetControlRect(CRect rc)
{
	m_rc = rc;
	if(m_nSliderHeight == 0)
	{
		m_nSliderHeight = m_rc.Height() - m_nThumbTop;
	}
	if(m_nThumbWidth == 0)
	{
		m_nThumbWidth = m_sizeThumb.cx;
	}
	if(m_nThumbHeight == 0)
	{
		m_nThumbHeight = m_sizeThumb.cy;
	}

	// ���㻬���λ��
	int nPos = (int)__max(m_rc.Width() * m_nProgress / m_nMaxProgress - m_nThumbWidth / 2, 0);
	nPos = (int)__min(nPos, m_rc.Width() - m_nThumbWidth);
	m_rcThumb = CRect(nPos, 0, nPos + m_nThumbWidth, m_nThumbHeight);
	m_rcThumb.OffsetRect(m_rc.left, m_rc.top);
}

// �ƶ�����λ��
int CDuiSlider::MoveThumbPos(CPoint point)
{
	int nProgress = m_nMaxProgress * (point.x - m_rc.left) / m_rc.Width();
	nProgress = __max(0, nProgress);
	nProgress = __min(m_nMaxProgress, nProgress);
	if(nProgress != m_nProgress)
	{
		m_nProgress = nProgress;
		// ���㻬���λ��
		int nPos = (int)__max(m_rc.Width() * m_nProgress / m_nMaxProgress - m_nThumbWidth / 2, 0);
		nPos = (int)__min(nPos, m_rc.Width() - m_nThumbWidth);
		m_rcThumb = CRect(nPos, 0, nPos + m_nThumbWidth, m_nThumbHeight);
		m_rcThumb.OffsetRect(m_rc.left, m_rc.top);
		UpdateControl(TRUE);

		SendMessage(MSG_SCROLL_CHANGE, m_nProgress, 0);

		return nProgress;
	}

	return nProgress;
}

// ����ƶ�
BOOL  CDuiSlider::OnControlMouseMove(UINT nFlags, CPoint point)
{		
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
					if(m_rcThumb.PtInRect(point))
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
			MoveThumbPos(point);
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
BOOL CDuiSlider::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rcThumb.PtInRect(point))
		{
			// ����ڹ�������,���¼���λ��
			m_enButtonState = enBSDown;
			MoveThumbPos(point);
		}
		else
		{
			// ���ڹ�������,���ƶ�������
			MoveThumbPos(point);
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
BOOL  CDuiSlider::OnControlLButtonUp(UINT nFlags, CPoint point)
{	
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{		
		if(m_rcThumb.PtInRect(point))
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

void CDuiSlider::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight * 4);

		Graphics graphics(m_memDC);

		// ��4��״̬���ڴ�ͼƬ
		for(int i = 0; i < 4; i++)
		{
			// ���Ʊ���
			m_memDC.BitBlt(0, i * nHeight, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

			int nPosY = i * nHeight;

			if(m_pImageForeGround != NULL)	// ʹ�ñ�����ǰ��ͼƬ��������
			{
				if(m_pImageBackGround != NULL)	// ������
				{
					DrawImageFrameMID(graphics, m_pImageBackGround, CRect(0, nPosY + m_nThumbTop, nWidth, nPosY + m_nThumbTop + m_nSliderHeight),
						0, 0, m_sizeBackGround.cx, m_sizeBackGround.cy,
						m_nHeadLength, 0, m_nHeadLength, 0);
				}

				if(m_nProgress != 0)	// ��ǰ��
				{
					DrawImageFrameMID(graphics, m_pImageForeGround, CRect(0, nPosY + m_nThumbTop, nWidth * m_nProgress / m_nMaxProgress, nPosY + m_nThumbTop + m_nSliderHeight),
						0, 0, m_sizeForeGround.cx, m_sizeForeGround.cy,
						m_nHeadLength, 0, m_nHeadLength, 0);
				}
			}else
			if(m_pImage != NULL)	// ʹ�õ���ͼƬ��������
			{
				DrawImageFrame(graphics, m_pImage, CRect(0, nPosY + m_nThumbTop, nWidth, nPosY + m_nThumbTop + m_nSliderHeight), 
					0, 0, m_sizeImage.cx, m_sizeImage.cy, 2);

				if(m_nProgress != 0)
				{
					DrawImageFrame(graphics, m_pImage, CRect(0, nPosY + m_nThumbTop, nWidth * m_nProgress / m_nMaxProgress, nPosY + m_nThumbTop + m_nSliderHeight), 
						m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, 2);
				}
			}

			// ������
			if(m_pImageThumb != NULL)
			{
				// ���㻬���λ��
				int nPos = (int)__max(m_rc.Width() * m_nProgress / m_nMaxProgress - m_nThumbWidth / 2, 0);
				nPos = (int)__min(nPos, m_rc.Width() - m_nThumbWidth);
				Rect rect(nPos, nPosY, m_nThumbWidth, m_nThumbHeight);
				graphics.DrawImage(m_pImageThumb, rect, i * m_sizeThumb.cx, 0, m_sizeThumb.cx, m_sizeThumb.cy, UnitPixel);
			}

			// ����������
			if(m_bShowText)
			{
				BSTR bsFont = m_strFont.AllocSysString();
				FontFamily fontFamily(bsFont);
				Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
				::SysFreeString(bsFont);

				SolidBrush solidBrush(m_clrText);
				graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
				// ����ˮƽ�ʹ�ֱ���뷽ʽ
				DUI_STRING_ALIGN_DEFINE();
				strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);

				CString strText;
				// ֻ�����ֵ����Ϊ100����²Ż���ʾ�ٷֺ�
				strText.Format(_T("%s%d%s"), m_strTitle, m_nProgress, (m_nMaxProgress == 100) ? _T("%") : _T(""));
				BSTR bsTitle = strText.AllocSysString();
				RectF rect((Gdiplus::REAL)(0), (Gdiplus::REAL)nPosY, (Gdiplus::REAL)nWidth, (Gdiplus::REAL)nHeight);
				graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrush);
				::SysFreeString(bsTitle);
			}
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, m_enButtonState * nHeight, SRCCOPY);
}