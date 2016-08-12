#include "StdAfx.h"
#include "CheckButton.h"

CCheckButton::CCheckButton(HWND hWnd, CDuiObject* pDuiObject)
						   : CControlBaseFont(hWnd, pDuiObject)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_bIsFocus = FALSE;
	m_bShowFocus = TRUE;
	m_enButtonState = enBSNormal;
	m_pImage = NULL;
	m_bDown = false;
	m_bMouseDown = false;
	m_uVAlignment = VAlign_Middle;
	m_clrText = Color(254, 0, 0, 0);
	SetBitmapCount(6);
}

CCheckButton::CCheckButton(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle/*= TEXT("")*/, BOOL bIsVisible/* = TRUE*/, 
						   BOOL bIsDisable/* = FALSE*/ ,BOOL bIsPressDown/* = FALSE*/)
						   : CControlBaseFont(hWnd, pDuiObject, uControlID, rc, strTitle, bIsVisible, bIsDisable)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_bIsFocus = FALSE;
	m_bShowFocus = TRUE;
	m_enButtonState = enBSNormal;
	m_pImage = NULL;
	m_bDown = false;
	m_bMouseDown = false;
	m_uVAlignment = VAlign_Middle;
	m_clrText = Color(254, 0, 0, 0);
	SetBitmapCount(6);
}

CCheckButton::~CCheckButton(void)
{
}

BOOL CCheckButton::GetCheck()
{
	return m_bDown;
}

BOOL CCheckButton::SetCheck(BOOL bCheck)
{
	if (m_bIsDisable)
	{
		return m_bDown;
	}

	BOOL bDown = m_bDown;
	m_bDown = bCheck;
	if(m_bDown)
	{
		m_enButtonState = enBSDown;
	}
	else
	{
		 m_enButtonState = enBSNormal;
	}
	if(m_bDown != bDown)
	{
		UpdateControl();
	}
	return m_bDown;
}

// 从XML设置check属性
HRESULT CCheckButton::OnAttributeCheck(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	BOOL bCheck = (strValue == "true");
	SetCheck(bCheck);

	return bLoading?S_FALSE:S_OK;
}

// 设置控件的焦点
BOOL CCheckButton::SetControlFocus(BOOL bFocus)
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

BOOL CCheckButton::OnControlMouseMove(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable && !m_bMouseDown)
	{
		if(m_rc.PtInRect(point))
		{
			if(m_bDown)
			{
				m_enButtonState = enBSHoverDown;
			}
			else
			{
				m_enButtonState = enBSHover;
			}
		}
		else
		{
			if(m_bDown)
			{
				m_enButtonState = enBSDown;
			}
			else
			{
				m_enButtonState = enBSNormal;
			}
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CCheckButton::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_bMouseDown = true;
			if(m_bDown)
			{
				m_enButtonState = enBSHoverDown;
				// 设置为当前处于激活状态
				m_bIsFocus = TRUE;
			}
			else
			{
				m_enButtonState = enBSHover;
			}
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CCheckButton::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			if(m_bMouseDown)
			{
				m_bDown = !m_bDown;
				SendMessage(MSG_BUTTON_UP, 0, 0);
			}
			if(m_bDown)
			{
				m_enButtonState = enBSHoverDown;
			}
			else
			{
				m_enButtonState = enBSHover;
			}			
		}
		else
		{
			if(m_bDown)
			{
				m_enButtonState = enBSDown;
			}
			else
			{
				m_enButtonState = enBSNormal;
			}			
		}
	}
	m_bMouseDown = false;
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

// 键盘事件处理
BOOL CCheckButton::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 处理快捷键
	if((m_nShortcutKey != 0) && (nChar == m_nShortcutKey) && (nFlags == m_nShortcutFlag))
	{
		SetCheck(!GetCheck());
		SendMessage(MSG_BUTTON_CHECK, 0, GetCheck());
		return true;
	}

	// 如果当前处于焦点状态,用空格键可以切换check
	if(m_bIsFocus && (nChar == VK_SPACE) && (nFlags == 0))
	{
		SetCheck(!GetCheck());
		SendMessage(MSG_BUTTON_CHECK, 0, GetCheck());
		return true;
	}

	return false;
}

void  CCheckButton::SetControlDisable(BOOL bIsDisable)
{
	if(m_bIsDisable != bIsDisable)
	{
		m_bIsDisable = bIsDisable;
		if(bIsDisable)
		{
			if(m_bDown)
			{
				m_enButtonState = enBSDisableDown;
			}
			else
			{
				m_enButtonState = enBSDisable;
			}
		}
		else
		{
			if(m_bDown)
			{
				m_enButtonState = enBSDown;
			}
			else
			{
				m_enButtonState = enBSNormal;
			}
		}
		::InvalidateRect(m_hWnd, &m_rc, true);
	}
}

void CCheckButton::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth * 6, nHeight);

		Graphics graphics(m_memDC);
		CRect  rcTemp(0, 0, nWidth, nHeight);

		for(int i = 0; i < 6; i++)
		{
			m_memDC.BitBlt(i * nWidth, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

			graphics.DrawImage(m_pImage, Rect(rcTemp.left, rcTemp.top + (nHeight - m_sizeImage.cy) / 2,   m_sizeImage.cx, m_sizeImage.cy),
				i * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);

			rcTemp.OffsetRect(nWidth, 0);
		}
		
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
			CPoint point = GetOriginPoint(nWidth - m_sizeImage.cx - 3, nHeight, size.Width, size.Height,
							GetGDIAlignment(m_uAlignment), GetGDIVAlignment(m_uVAlignment));

			for(int i = 0; i < 6; i++)
			{
				SolidBrush solidBrush(enBSDisable == i ? Color(128, 128, 128) : m_clrText);

				RectF rect((Gdiplus::REAL)(m_sizeImage.cx + 3 + point.x + i * nWidth), (Gdiplus::REAL)point.y, (Gdiplus::REAL)(nWidth - m_sizeImage.cx - 3 - point.x), (Gdiplus::REAL)size.Height);
				BSTR bsTitle = m_strTitle.AllocSysString();
				graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrush);
				::SysFreeString(bsTitle);

				// 画焦点框(虚线框)
				if(m_bIsFocus && m_bShowFocus)
				{
					Pen pen(Color(128, 128, 128), 1);
					pen.SetDashStyle(DashStyleDot);
					RectF rectFocus((Gdiplus::REAL)(point.x + i * nWidth), (Gdiplus::REAL)point.y, (Gdiplus::REAL)(m_sizeImage.cx + 6 + size.Width), (Gdiplus::REAL)size.Height);
					graphics.DrawRectangle(&pen, rectFocus);
				}
			}
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, m_enButtonState * nWidth, 0, SRCCOPY);
}