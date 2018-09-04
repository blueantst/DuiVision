#include "StdAfx.h"
#include "DuiButton.h"

CDuiButton::CDuiButton(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBaseFont(hWnd, pDuiObject)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_bIsFocus = FALSE;
	m_bShowFocus = TRUE;
	m_enButtonState = enBSNormal;
	m_uAlignment = Align_Center;
	m_uVAlignment = VAlign_Middle;
	m_clrText = Color(255, 0, 0, 0);
	m_bTimer = FALSE;
	m_nIndex = 0;
	m_nMaxIndex = 0;
	m_pImageBtn = NULL;
	m_sizeBtn = CSize(0, 0);
}

CDuiButton::CDuiButton(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle/*= TEXT("")*/, BOOL bIsVisible/* = TRUE*/, 
						   BOOL bIsDisable/* = FALSE*/ ,BOOL bIsPressDown/* = FALSE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, strTitle, bIsVisible, bIsDisable)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_bIsFocus = FALSE;
	m_bShowFocus = TRUE;
	m_enButtonState = enBSNormal;
	m_uAlignment = Align_Center;
	m_uVAlignment = VAlign_Middle;
	m_clrText = Color(225, 64, 64, 64);
	m_bTimer = FALSE;
	m_nIndex = 0;
	m_nMaxIndex = 0;
	m_pImageBtn = NULL;
	m_sizeBtn = CSize(0, 0);
}

CDuiButton::~CDuiButton(void)
{
	if(m_pImageBtn != NULL)
	{
		delete m_pImageBtn;
		m_pImageBtn = NULL;
	}
}

// 图片属性的实现
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiButton, Btn, 1)

// 设置控件的焦点
BOOL CDuiButton::SetControlFocus(BOOL bFocus)
{
	__super::SetControlFocus(bFocus);

	if(!GetVisible() || GetDisable())
	{
		return FALSE;
	}

	m_bIsFocus = bFocus;
	UpdateControl(true);

	return TRUE;
}

BOOL CDuiButton::OnControlMouseMove(UINT nFlags, CPoint point)
{	
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable && m_enButtonState != enBSDown)
	{
		if(m_rc.PtInRect(point))
		{
			m_enButtonState = enBSHover;
		}
		else
		{
			m_enButtonState = enBSNormal;
		}

		if (buttonState != m_enButtonState)
		{
			if(m_bTimer && !m_bIsDisable && m_bIsVisible && (0 == m_nIndex || m_nMaxIndex == m_nIndex) && (enBSNormal == buttonState || enBSHover == buttonState))
			{
				if(0 == m_nIndex)
				{
					m_nIndex = m_nMaxIndex / 2;
				}
				//启动过渡			
				m_bRunTime = true;
			}

			UpdateControl();
			return true;
		}
	}

	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CDuiButton::OnControlLButtonDown(UINT nFlags, CPoint point)
{	
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_enButtonState = enBSDown;
			if(m_bTimer)
			{
				m_bRunTime = false;
				m_nIndex = m_nMaxIndex;
			}
			// 设置为当前处于激活状态
			m_bIsFocus = TRUE;
			SendMessage(MSG_BUTTON_DOWN, 0, 0);
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CDuiButton::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	bool bSend = false;
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			if(m_enButtonState == enBSDown)
			{
				bSend = true;
			}
			m_enButtonState = enBSHover;
		}
		else
		{
			m_enButtonState = enBSNormal;
			if(m_bTimer)
			{
				m_bRunTime = true;
				m_nIndex = m_nMaxIndex;
			}
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();

		if(bSend)
		{
			SendMessage(MSG_BUTTON_UP, 0, 0);
		}
		return true;
	}
	return false;
}

// 键盘事件处理
BOOL CDuiButton::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 如果当前处于焦点状态,用空格键可以模拟点击按钮
	if(m_bIsFocus && (nChar == VK_SPACE) && (nFlags == 0))
	{
		SendMessage(MSG_BUTTON_DOWN, 0, 0);
		SendMessage(MSG_BUTTON_UP, 0, 0);
		return true;
	}

	return __super::OnControlKeyDown(nChar, nRepCnt, nFlags);
}

void CDuiButton::SetControlDisable(BOOL bIsDisable)
{
	if(m_bIsDisable != bIsDisable)
	{
		m_bIsDisable = bIsDisable;
		if(bIsDisable)
		{
			m_enButtonState = enBSDisable;
		}
		else
		{
			m_enButtonState = enBSNormal;
		}

		UpdateControl();
	}
}

BOOL CDuiButton::OnControlTimer()
{
	if(!m_bRunTime)
	{
		return FALSE;
	}

	if(enBSNormal == m_enButtonState)
	{
		m_nIndex--;
		if(m_nIndex < 0)
		{
			m_nIndex = 0;
		}
	}
	else if(enBSHover == m_enButtonState)
	{
		m_nIndex++;
		if(m_nIndex > m_nMaxIndex)
		{
			m_nIndex = m_nMaxIndex;
		}
	}
	if(0 == m_nIndex || m_nIndex == m_nMaxIndex)
	{
		m_bRunTime = false;
	}

	UpdateControl();

	return true;
}

void CDuiButton::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();
	
	if(!m_bUpdate)
	{		
		UpdateMemDC(dc, nWidth * (4 + m_nMaxIndex), nHeight);

		ColorMatrix matrix;
		ImageAttributes imageAttr;

		ZeroMemory(&matrix, sizeof(matrix));
		for(int i = 0; i < 5; i++)
		{
			matrix.m[i][i] = 1.0f;
		}

		Graphics graphics(m_memDC);
		CRect  rcTemp(0, 0, nWidth, nHeight);
		
		m_memDC.SetBkMode(TRANSPARENT);

		// 画基础状态
		for(int i = 0; i < 4; i++)
		{
			m_memDC.BitBlt(i * nWidth, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);
			
			DrawImageFrame(graphics, m_pImage, rcTemp, i * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, 4);

			// 画焦点框(虚线框)
			if(m_bIsFocus && m_bShowFocus)
			{
				Pen pen(Color(128, 128, 128), 1);
				pen.SetDashStyle(DashStyleDot);
				RectF rectFocus((Gdiplus::REAL)(rcTemp.left+3), (Gdiplus::REAL)(rcTemp.top+3), (Gdiplus::REAL)(rcTemp.Width()-7), (Gdiplus::REAL)(rcTemp.Height()-7));
				graphics.DrawRectangle(&pen, rectFocus);
			}

			rcTemp.OffsetRect(nWidth, 0);
		}

		// 画渐变效果状态
		for (int i = 4; i < 4 + m_nMaxIndex; i++)
		{
			m_memDC.BitBlt(i * nWidth, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

			matrix.m[3][3] = 1 - ((float)(i - 4 + 1)) / m_nMaxIndex;
			imageAttr.SetColorMatrix(&matrix);
			graphics.DrawImage(m_pImage, Rect(rcTemp.left ,rcTemp.top, rcTemp.Width(), rcTemp.Height()), 
				m_sizeImage.cx * enBSNormal, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel ,&imageAttr);

			matrix.m[3][3] = ((float)(i - 4 + 1)) / m_nMaxIndex;
			imageAttr.SetColorMatrix(&matrix);
			graphics.DrawImage(m_pImage, Rect(rcTemp.left ,rcTemp.top, rcTemp.Width(), rcTemp.Height()), 
				m_sizeImage.cx * enBSHover, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel ,&imageAttr);

			rcTemp.OffsetRect(nWidth, 0);
		}

		// 画按钮图片
		if(m_pImageBtn != NULL)
		{
			int nX = (nWidth - m_sizeBtn.cx) / 2;
			int nY = (nHeight - m_sizeBtn.cy - (m_strTitle.IsEmpty() ? 0 : 20)) / 2;
			for(int i = 0; i < 4 + m_nMaxIndex; i++)
			{
				graphics.DrawImage(m_pImageBtn, Rect(nX + (enBSDown == i ? 1 : 0), nY + (enBSDown == i ? 1 : 0), m_sizeBtn.cx, m_sizeBtn.cy),
					0, 0, m_sizeBtn.cx, m_sizeBtn.cy, UnitPixel);
				nX += nWidth;
			}
		}

		// 画文字
		if(!m_strTitle.IsEmpty())
		{
			m_memDC.SetBkMode(TRANSPARENT);

			rcTemp.SetRect(0, 0, nWidth, nHeight);

			BSTR bsFont = m_strFont.AllocSysString();
			FontFamily fontFamily(bsFont);
			Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
			graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
			::SysFreeString(bsFont);

			StringFormat strFormat;
			strFormat.SetAlignment(StringAlignmentNear);
			strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
			Size size = GetTextBounds(font, strFormat, m_strTitle);
			CPoint point = GetOriginPoint(nWidth, nHeight, size.Width, size.Height,
							GetGDIAlignment(m_uAlignment), GetGDIVAlignment(m_uVAlignment));
			if(m_pImageBtn != NULL)
			{
				// 如果有按钮图片,则文字位置向下移
				point.y += (m_sizeBtn.cy / 2);
			}

			for(int i = 0; i < 4 + m_nMaxIndex; i++)
			{
				SolidBrush solidBrush(enBSDisable == i ? Color(128, 128, 128) : m_clrText);

				RectF rect((Gdiplus::REAL)(point.x + i * nWidth + (enBSDown == i ? 1 : 0)), (Gdiplus::REAL)(point.y + (enBSDown == i ? 1 : 0)), (Gdiplus::REAL)(nWidth - point.x), (Gdiplus::REAL)size.Height);
				BSTR bsTitle = m_strTitle.AllocSysString();
				graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrush);
				::SysFreeString(bsTitle);
			}
		}
	}

	if(m_nIndex != 0 && m_bRunTime && m_bTimer)
	{
		dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, (4 + m_nIndex - 1) * nWidth, 0, SRCCOPY);
	}
	else
	{
		dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, m_enButtonState * nWidth, 0, SRCCOPY);
	}
}

// 图片按钮控件类(和基础按钮类完全相同,只是默认会启用渐变效果定时器)
CImageButton::CImageButton(HWND hWnd, CDuiObject* pDuiObject)
	: CDuiButton(hWnd, pDuiObject)
{
	m_enButtonState = enBSNormal;
	m_bTimer = true;
	m_nIndex = 0;
	m_nMaxIndex = 10;
}

CImageButton::CImageButton(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bAnimation/* = true*/, CString strTitle/*= TEXT("")*/, BOOL bIsVisible/* = TRUE*/, 
						   BOOL bIsDisable/* = FALSE*/ ,BOOL bIsPressDown/* = FALSE*/)
	: CDuiButton(hWnd, pDuiObject, uControlID, rc, strTitle, bIsVisible, bIsDisable)
{
	m_enButtonState = enBSNormal;
	m_bTimer = bAnimation;
	m_nIndex = 0;
	m_nMaxIndex = 10;
}