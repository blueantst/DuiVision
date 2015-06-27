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

// 图片属性的实现
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
		// 设置值超过最大值,自动更改为最大值
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

	// 计算滑块的位置
	int nPos = (int)__max(m_rc.Width() * m_nProgress / m_nMaxProgress - m_nThumbWidth / 2, 0);
	nPos = (int)__min(nPos, m_rc.Width() - m_nThumbWidth);
	m_rcThumb = CRect(nPos, 0, nPos + m_nThumbWidth, m_nThumbHeight);
	m_rcThumb.OffsetRect(m_rc.left, m_rc.top);
}

// 移动滑块位置
int CDuiSlider::MoveThumbPos(CPoint point)
{
	int nProgress = m_nMaxProgress * (point.x - m_rc.left) / m_rc.Width();
	nProgress = __max(0, nProgress);
	nProgress = __min(m_nMaxProgress, nProgress);
	if(nProgress != m_nProgress)
	{
		m_nProgress = nProgress;
		// 计算滑块的位置
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

// 鼠标移动
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
			// 鼠标按着滚动块,拖动的情况下,移动滚动块范围
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

// 鼠标左键按下
BOOL CDuiSlider::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rcThumb.PtInRect(point))
		{
			// 如果在滚动块内,则记录鼠标位置
			m_enButtonState = enBSDown;
			MoveThumbPos(point);
		}
		else
		{
			// 不在滚动块内,则移动滚动块
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

// 鼠标左键放开
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

		// 画4个状态的内存图片
		for(int i = 0; i < 4; i++)
		{
			// 复制背景
			m_memDC.BitBlt(0, i * nHeight, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

			int nPosY = i * nHeight;

			if(m_pImageForeGround != NULL)	// 使用背景和前景图片画进度条
			{
				if(m_pImageBackGround != NULL)	// 画背景
				{
					DrawImageFrameMID(graphics, m_pImageBackGround, CRect(0, nPosY + m_nThumbTop, nWidth, nPosY + m_nThumbTop + m_nSliderHeight),
						0, 0, m_sizeBackGround.cx, m_sizeBackGround.cy,
						m_nHeadLength, 0, m_nHeadLength, 0);
				}

				if(m_nProgress != 0)	// 画前景
				{
					DrawImageFrameMID(graphics, m_pImageForeGround, CRect(0, nPosY + m_nThumbTop, nWidth * m_nProgress / m_nMaxProgress, nPosY + m_nThumbTop + m_nSliderHeight),
						0, 0, m_sizeForeGround.cx, m_sizeForeGround.cy,
						m_nHeadLength, 0, m_nHeadLength, 0);
				}
			}else
			if(m_pImage != NULL)	// 使用单张图片画进度条
			{
				DrawImageFrame(graphics, m_pImage, CRect(0, nPosY + m_nThumbTop, nWidth, nPosY + m_nThumbTop + m_nSliderHeight), 
					0, 0, m_sizeImage.cx, m_sizeImage.cy, 2);

				if(m_nProgress != 0)
				{
					DrawImageFrame(graphics, m_pImage, CRect(0, nPosY + m_nThumbTop, nWidth * m_nProgress / m_nMaxProgress, nPosY + m_nThumbTop + m_nSliderHeight), 
						m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, 2);
				}
			}

			// 画滑块
			if(m_pImageThumb != NULL)
			{
				// 计算滑块的位置
				int nPos = (int)__max(m_rc.Width() * m_nProgress / m_nMaxProgress - m_nThumbWidth / 2, 0);
				nPos = (int)__min(nPos, m_rc.Width() - m_nThumbWidth);
				Rect rect(nPos, nPosY, m_nThumbWidth, m_nThumbHeight);
				graphics.DrawImage(m_pImageThumb, rect, i * m_sizeThumb.cx, 0, m_sizeThumb.cx, m_sizeThumb.cy, UnitPixel);
			}

			// 画进度文字
			if(m_bShowText)
			{
				BSTR bsFont = m_strFont.AllocSysString();
				FontFamily fontFamily(bsFont);
				Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
				::SysFreeString(bsFont);

				SolidBrush solidBrush(m_clrText);
				graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
				// 设置水平和垂直对齐方式
				DUI_STRING_ALIGN_DEFINE();
				strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);

				CString strText;
				// 只有最大值设置为100情况下才会显示百分号
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