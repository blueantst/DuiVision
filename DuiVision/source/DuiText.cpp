#include "StdAfx.h"
#include "DuiText.h"

#define	SCROLL_V	1	// 滚动条控件ID

CDuiText::CDuiText(HWND hWnd, CDuiObject* pDuiObject)
		: CControlBaseFont(hWnd, pDuiObject)
{
	m_nScrollWidth = 8;

	CRect rcScroll = CRect(0,0,0,0);
	rcScroll.top;
	rcScroll.left = rcScroll.right - m_nScrollWidth;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CDuiScrollVertical(hWnd, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(pControlBase);
	m_pControScrollV = (CControlBaseFont*)pControlBase;
	m_bScrollV = FALSE;

	m_nBkTransparent = 0;

	m_enButtonState = enBSNormal;
	m_bEnableHover = FALSE;
	m_bEnableShadow = FALSE;
	m_bBack = false;
	m_clrBack = Color(0, 128, 0);
	m_clrText = Color(254, 0, 0, 0);
	m_clrTextHover = Color(128, 80, 80);
	m_clrTextShadow = Color(80, 80, 80);
	m_nStart = 0;
	m_clrMark = Color(237, 128, 27);

	m_nVirtualHeight = 0;

	SetBitmapCount(1);
}

CDuiText::CDuiText(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString m_strTitle, Color clr/* = Color(0, 0, 0)*/,
				CString strFont, int nFontWidth/* = 12*/, FontStyle fontStyle/* = FontStyleRegular*/, BOOL bIsVisible/* = TRUE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, m_strTitle, bIsVisible, FALSE, FALSE, strFont, nFontWidth, fontStyle)
{
	m_nScrollWidth = 8;

	CRect rcScroll = CRect(0,0,0,0);
	rcScroll.top;
	rcScroll.left = rcScroll.right - m_nScrollWidth;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CDuiScrollVertical(hWnd, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(pControlBase);
	m_pControScrollV = (CControlBaseFont*)pControlBase;
	m_bScrollV = FALSE;

	m_nBkTransparent = 0;

	m_enButtonState = enBSNormal;
	m_bEnableHover = FALSE;
	m_bEnableShadow = FALSE;
	m_bBack = false;
	m_clrBack = Color(0, 128, 0);
	m_clrText = clr;
	m_clrTextHover = Color(128, 80, 80);
	m_clrTextShadow = Color(80, 80, 80);
	m_nStart = 0;
	m_clrMark = Color(237, 128, 27);

	SetBitmapCount(1);
}

CDuiText::~CDuiText(void)
{

}

void CDuiText::SetMarkText(CString strMark)
{
	if(m_strMark != strMark)
	{
		m_strMark = strMark;
		UpdateControl(true);
	}
}

void CDuiText::SetTitleMarkText(CString strTitle, CString strMark, int nStart)
{
	if(m_strTitle != strTitle || m_strMark != strMark)
	{
		m_strTitle = strTitle;
		m_strMark = strMark;
		UpdateControl(true);
	}
}

void CDuiText::SetMarkText(CString strMark, Color clrMark, int nStart)
{
	if(m_strMark != strMark || *((DWORD *)&m_clrMark) != *((DWORD *)&clrMark) || m_nStart != nStart)
	{
		m_strMark = strMark;
		m_clrMark = clrMark ;
		m_nStart = nStart;
		UpdateControl(true);
	}
}

void CDuiText::SetMarkText(CString strTitle, CString strMark, Color clrMark, int nStart)
{
	if(m_strTitle != strTitle || m_strMark != strMark || *((DWORD *)&m_clrMark) != *((DWORD *)&clrMark) || m_nStart != nStart)
	{
		m_strTitle = strTitle;
		m_strMark = strMark;
		m_clrMark = clrMark ;
		m_nStart = nStart;
		UpdateControl(true);
	}
}

// 从XML设置背景颜色属性
HRESULT CDuiText::OnAttributeBackColor(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_clrBack = CDuiObject::StringToColor(strValue);
	m_bBack = true;

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置热点颜色属性
HRESULT CDuiText::OnAttributeTextHoverColor(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_clrTextHover = CDuiObject::StringToColor(strValue);
	m_bEnableHover = TRUE;

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置阴影颜色属性
HRESULT CDuiText::OnAttributeTextShadowColor(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_clrTextShadow = CDuiObject::StringToColor(strValue);
	m_bEnableShadow = TRUE;

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置图片信息属性
HRESULT CDuiText::OnAttributeImageScroll(const CString& strValue, BOOL bLoading)
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
		if(!m_pControScrollV->SetBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = _ttoi(strSkin);
		if(!m_pControScrollV->SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!m_pControScrollV->SetBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	m_bScrollV = TRUE;

	return bLoading?S_FALSE:S_OK;
}

void CDuiText::SetControlRect(CRect rc)
{
	m_rc = rc;

	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			UINT uControlID = pControlBase->GetControlID();
			if((SCROLL_V == uControlID) && m_bScrollV)
			{
				rcTemp = m_rc;
				rcTemp.left = rcTemp.right - m_nScrollWidth;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}
}

BOOL CDuiText::OnControlMouseMove(UINT nFlags, CPoint point)
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
	}

	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CDuiText::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_enButtonState = enBSDown;
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

BOOL CDuiText::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			if(m_enButtonState == enBSDown)
			{
				//ShellExecute(NULL, TEXT("open"), m_strLink, NULL,NULL,SW_NORMAL); 
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
		return true;
	}
	return false;
}

// 滚动事件处理
BOOL CDuiText::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(!m_pControScrollV->GetVisible())
	{
		return false;
	}

	// 更新滚动条,并刷新界面
	CDuiScrollVertical* pScroll = (CDuiScrollVertical*)m_pControScrollV;
	if(pScroll->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
	}

	return true;
}

// 消息响应
LRESULT CDuiText::OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OnMessage(uID, uMsg, wParam, lParam);
	return 0L; 
}

// 消息响应
LRESULT CDuiText::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// 如果是滚动条的位置变更事件,则刷新显示
		UpdateControl(true);
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

// 计算显示的字符串总高度应该是多高
int CDuiText::GetVirtualHeight()
{
	BSTR bsFont = m_strFont.AllocSysString();
	FontFamily fontFamily(bsFont);
	Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
	::SysFreeString(bsFont);

	// 设置水平和垂直对齐方式
	DUI_STRING_ALIGN_DEFINE();
	strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);

	int nWidth = m_rc.Width();
	if(m_bScrollV)
	{
		nWidth -= 8;
	}
	
	Size size = GetTextBounds(font, strFormat, nWidth, m_strTitle);

	// 滚动条只有在需要的总高度大于文本框的高度时候才会显示
	m_pControScrollV->SetVisible(size.Height > m_rc.Height());
	((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange(size.Height);

	if(size.Height < m_rc.Height())
	{
		return m_rc.Height();
	}
	return size.Height;
}

void CDuiText::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();
	
	// 计算显示位置
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// 当前top位置
	int nMaxRange = pScrollV->GetScrollMaxRange();
	int nVirtualTop = 0;	// 当前显示的是虚拟图片中什么位置开始的图片
	m_nVirtualHeight = GetVirtualHeight();
	if(m_nVirtualHeight > m_rc.Height())
	{
		nVirtualTop = (nMaxRange > 0) ? nCurPos*(m_nVirtualHeight-m_rc.Height())/nMaxRange : 0;
	}else
	{
		nVirtualTop = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, m_nVirtualHeight);

		Graphics graphics(m_memDC);
		
		if(m_bBack)
		{
			SolidBrush brush(m_clrBack);
			graphics.FillRectangle(&brush, 0, 0, nWidth, nHeight);
		}
		else
		{
			//m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);
			m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, WHITENESS);	// 画白色背景
			DrawVerticalTransition(m_memDC, dc, CRect(0, 0+nVirtualTop, nWidth, nHeight+nVirtualTop),	// 背景透明度
					m_rc, m_nBkTransparent, m_nBkTransparent);
		}
		
		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		SolidBrush solidBrush(m_clrText);
		
		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// 设置水平和垂直对齐方式
		DUI_STRING_ALIGN_DEFINE();

		//strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
		strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		//strFormat.SetTrimming(StringTrimmingEllipsisWord);	// 以单词为单位去尾,略去部分使用省略号

		int nXPos = 0;
		if(m_pImage != NULL)
		{
			graphics.DrawImage(m_pImage, Rect(0, (nHeight - m_sizeImage.cy) / 2, m_sizeImage.cx, m_sizeImage.cy),
				0, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
			nXPos += m_sizeImage.cx + 5;
		}
		
		int nStart = m_strTitle.Find(m_strMark, m_nStart);
		if(m_strMark.IsEmpty() || (nStart == -1))
		{
			int nTextWidth = nWidth - nXPos;
			if(m_bScrollV)
			{
				nTextWidth -= m_nScrollWidth;
			}

			// 先画阴影
			if(m_bEnableShadow)
			{
				RectF rectShadow((Gdiplus::REAL)(nXPos  + 1), (Gdiplus::REAL)1, (Gdiplus::REAL)nTextWidth, (Gdiplus::REAL)nHeight);
				SolidBrush solidBrushS(m_clrTextShadow);
				BSTR bsTitle = m_strTitle.AllocSysString();
				graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rectShadow, &strFormat, &solidBrushS);
				::SysFreeString(bsTitle);
			}

			// 再画正常的文字
			RectF rect((Gdiplus::REAL)(nXPos), (Gdiplus::REAL)0, (Gdiplus::REAL)nTextWidth, (Gdiplus::REAL)nHeight);
			if((m_enButtonState == enBSHover) && m_bEnableHover)
			{
				SolidBrush solidBrushH(m_clrTextHover);
				BSTR bsTitle = m_strTitle.AllocSysString();
				graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrushH);
				::SysFreeString(bsTitle);
			}else
			{
				BSTR bsTitle = m_strTitle.AllocSysString();
				graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrush);
				::SysFreeString(bsTitle);
			}
		}
		else
		{
			SolidBrush solidBrushM(m_clrMark);
			SolidBrush solidBrushS(m_clrTextShadow);

			CString srtL = m_strTitle.Left(nStart);
			CString srtR  = m_strTitle.Right(m_strTitle.GetLength() - m_strMark.GetLength() - nStart);
			Size sizeL = GetTextBounds(font, strFormat, srtL);
			Size sizeM = GetTextBounds(font, strFormat, m_strMark);
			Size sizeR = GetTextBounds(font, strFormat, srtR);

			if(m_bEnableShadow)
			{
				BSTR bsL = srtL.AllocSysString();
				graphics.DrawString(bsL, (INT)wcslen(bsL), &font, 
					PointF((Gdiplus::REAL)(nXPos + 1), (Gdiplus::REAL)1), &strFormat, &solidBrushS);
				::SysFreeString(bsL);
				BSTR bsMark = m_strMark.AllocSysString();
				graphics.DrawString(bsMark, (INT)wcslen(bsMark), &font, 
					PointF((Gdiplus::REAL)(nXPos + sizeL.Width + 2 + 1), (Gdiplus::REAL)1), &strFormat, &solidBrushS);
				::SysFreeString(bsMark);
			}
			BSTR bsL = srtL.AllocSysString();
			graphics.DrawString(bsL, (INT)wcslen(bsL), &font, 
				PointF((Gdiplus::REAL)(nXPos), (Gdiplus::REAL)0), &strFormat, &solidBrush);
			::SysFreeString(bsL);
			BSTR bsMark = m_strMark.AllocSysString();
			graphics.DrawString(bsMark, (INT)wcslen(bsMark), &font, 
				PointF((Gdiplus::REAL)(nXPos + sizeL.Width + 2), (Gdiplus::REAL)0), &strFormat, &solidBrushM);
			::SysFreeString(bsMark);

			if(m_bEnableShadow)
			{
				RectF rect((Gdiplus::REAL)(nXPos + sizeL.Width + sizeM.Width + 4 + 1), (Gdiplus::REAL)(1), (Gdiplus::REAL)(nWidth - (nXPos + sizeL.Width + sizeM.Width + 4)), (Gdiplus::REAL)nHeight);
				BSTR bsR = srtR.AllocSysString();
				graphics.DrawString(bsR, (INT)wcslen(bsR), &font, 
					PointF((Gdiplus::REAL)(nXPos + sizeL.Width + sizeM.Width + 4), (Gdiplus::REAL)0), &strFormat, &solidBrushS);
				::SysFreeString(bsR);
			}
			//RectF rect(nXPos + point.x + sizeL.Width + sizeM.Width + 4, point.y, nWidth - (nXPos + sizeL.Width + sizeM.Width + 4 + point.x), size.Height);
			RectF rect((Gdiplus::REAL)(nXPos + sizeL.Width + sizeM.Width + 4), (Gdiplus::REAL)0, (Gdiplus::REAL)(nWidth - (nXPos + sizeL.Width + sizeM.Width + 4)), (Gdiplus::REAL)nHeight);
			BSTR bsR = srtR.AllocSysString();
			graphics.DrawString(bsR, (INT)wcslen(bsR), &font, 
				PointF((Gdiplus::REAL)(nXPos + sizeL.Width + sizeM.Width + 4), (Gdiplus::REAL)0), &strFormat, &solidBrush);
			::SysFreeString(bsR);
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, nVirtualTop, SRCCOPY);
}