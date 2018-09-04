#include "StdAfx.h"
#include "LinkButton.h"

CLinkButton::CLinkButton(HWND hWnd, CDuiObject* pDuiObject)
									: CControlBaseFont(hWnd, pDuiObject)
{
	m_enButtonState = enBSNormal;
	m_clrTextNormal = Color(39, 134, 230);
	m_clrTextHover = Color(21, 112, 235);
	m_clrTextDown = Color(21, 112, 235);
	m_clrTextDisable = Color(128, 128, 128);
	m_strLink = _T("");
	m_bDown = FALSE;
}

CLinkButton::CLinkButton(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, CString strLink,
									Color clrTextNormal ,Color clrTextHover, Color clrTextDown ,Color clrTextDisable,
									BOOL bIsVisible, BOOL bIsDisable)
									: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, strTitle, bIsVisible, bIsDisable)
{
	m_enButtonState = enBSNormal;
	m_clrTextNormal = clrTextNormal;
	m_clrTextHover = clrTextHover;
	m_clrTextDown = clrTextDown;
	m_clrTextDisable = clrTextDisable;
	m_strLink = strLink;
	m_bDown = FALSE;
}

CLinkButton::~CLinkButton(void)
{
}

BOOL CLinkButton::OnControlMouseMove(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable && m_enButtonState != enBSDown)
	{
		if(m_rcText.PtInRect(point))
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

BOOL CLinkButton::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rcText.PtInRect(point))
		{
			m_enButtonState = enBSDown;
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

BOOL CLinkButton::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	bool bSend = false;
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rcText.PtInRect(point))
		{
			if(m_enButtonState == enBSDown)
			{
				bSend = true;
				if(!m_strLink.IsEmpty())
				{
					ShellExecute(NULL, TEXT("open"), m_strLink, NULL,NULL,SW_NORMAL);
				}
			}
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

		if(bSend)
		{
			SendMessage(MSG_BUTTON_UP, 0, 0);
		}
		return true;
	}
	return false;
}

void CLinkButton::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{		
		UpdateMemDC(dc, nWidth * 4, nHeight);

		for(int i = 0; i < 4; i++)
		{
			m_memDC.BitBlt(i * nWidth, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);
		}
		
		Color clrText[4] = {m_clrTextNormal, m_clrTextHover, m_clrTextDown, m_clrTextDisable};

		Graphics graphics(m_memDC);
		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
		::SysFreeString(bsFont);

		StringFormat strFormat;
		strFormat.SetAlignment(StringAlignmentNear);
		strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
		Size sizeText = GetTextBounds(font, strFormat, m_strTitle);
		CPoint point = GetOriginPoint(nWidth, nHeight, sizeText.Width, sizeText.Height,
						GetGDIAlignment(m_uAlignment), GetGDIVAlignment(m_uVAlignment));
		
		m_rcText.SetRect(m_rc.left, m_rc.top + point.y, m_rc.left + sizeText.Width, m_rc.top + point.y + sizeText.Height);

		for(int i = 0; i < 4; i++)
		{
			SolidBrush solidBrush(clrText[i]);	
			RectF rect((Gdiplus::REAL)(i * nWidth), (Gdiplus::REAL)point.y, (Gdiplus::REAL)nWidth, (Gdiplus::REAL)sizeText.Height);
			BSTR bsTitle = m_strTitle.AllocSysString();
			graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrush);
			::SysFreeString(bsTitle);
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, m_enButtonState * nWidth, 0, SRCCOPY);
}