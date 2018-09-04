#include "StdAfx.h"
#include "Progress.h"

CDuiProgress::CDuiProgress(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBaseFont(hWnd, pDuiObject)
{
	m_bRunTime = false;
	m_nMaxProgress = 100;
	m_nCount = 0;
	m_nTimerCount = 3;

	SetBitmapCount(2);

	m_pImageBackGround = NULL;
	m_sizeBackGround = CSize(0, 0);
	m_pImageForeGround = NULL;
	m_sizeForeGround = CSize(0, 0);
	m_nHeadLength = 0;

	m_clrText = Color(254, 128, 128, 128);
	m_uAlignment = Align_Center;
	m_uVAlignment = VAlign_Middle;
	m_bShowText = FALSE;

	m_nProgress = 0;
	SetProgress(0);
}

CDuiProgress::CDuiProgress(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress/* = 0*/, 
					 BOOL bIsVisible/* = TRUE*/, BOOL bIsDisable/* = FALSE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, TEXT(""), bIsVisible, bIsDisable)
{
	m_bRunTime = false;
	m_nMaxProgress = 100;
	m_nCount = 0;
	m_nTimerCount = 3;

	SetBitmapCount(2);

	m_pImageBackGround = NULL;
	m_sizeBackGround = CSize(0, 0);
	m_pImageForeGround = NULL;
	m_sizeForeGround = CSize(0, 0);
	m_nHeadLength = 0;

	m_clrText = Color(254, 128, 128, 128);
	m_uAlignment = Align_Center;
	m_uVAlignment = VAlign_Middle;
	m_bShowText = FALSE;

	m_nProgress = 0;
	SetProgress(nProgress);
}

CDuiProgress::~CDuiProgress(void)
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
}

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiProgress, BackGround, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiProgress, ForeGround, 1)

int CDuiProgress::SetProgress(int nProgress)
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

// �����Ƿ��Զ�����
BOOL CDuiProgress::SetRun(BOOL bRun, int nIndex/* = -1*/)
{
	if(GetDisable())
	{
		return m_bRunTime;
	}

	BOOL bOldRunTime = m_bRunTime;
	int nOldProgress = m_nProgress;
	m_bRunTime = bRun;

	if(nIndex >= 0 && nIndex <= m_nMaxProgress)
	{
		m_nProgress = nIndex;
	}

	if(GetVisible() && ((bOldRunTime != m_bRunTime) || (nOldProgress != m_nProgress)))
	{
		UpdateControl(true);
	}

	return m_bRunTime;
}

// ��XML�����Զ���������
HRESULT CDuiProgress::OnAttributeRun(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	BOOL bRun = (strValue == _T("true"));
	SetRun(bRun);

	return bLoading?S_FALSE:S_OK;
}

BOOL CDuiProgress::OnControlTimer()
{
	if(!m_bRunTime || !m_bIsVisible)
	{
		return FALSE;
	}

	if(++m_nCount >= m_nTimerCount)
	{
		m_nCount = 0;
		if(++m_nProgress >= m_nMaxProgress)
		{
			m_nProgress = 0;
		}

		if(GetVisible())
		{
			UpdateControl(true);
		}
		return true;
	}

	return false;
}

void CDuiProgress::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight);

		m_memDC.BitBlt(0, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

		Graphics graphics(m_memDC);

		if(m_pImageForeGround != NULL)	// ʹ�ñ�����ǰ��ͼƬ��������
		{
			if(m_pImageBackGround != NULL)	// ������
			{
				DrawImageFrameMID(graphics, m_pImageBackGround, CRect(0, 0, nWidth, nHeight),
					0, 0, m_sizeBackGround.cx, m_sizeBackGround.cy,
					m_nHeadLength, 0, m_nHeadLength, 0);
			}

			if(m_nProgress != 0)	// ��ǰ��
			{
				DrawImageFrameMID(graphics, m_pImageForeGround, CRect(0, 0, nWidth * m_nProgress / m_nMaxProgress, nHeight),
					0, 0, m_sizeForeGround.cx, m_sizeForeGround.cy,
					m_nHeadLength, 0, m_nHeadLength, 0);
			}
		}else
		if(m_pImage != NULL)	// ʹ�õ���ͼƬ��������
		{
			DrawImageFrame(graphics, m_pImage, CRect(0, 0, nWidth, nHeight), 
				0, 0, m_sizeImage.cx, m_sizeImage.cy, 2);

			if(m_nProgress != 0)
			{
				DrawImageFrame(graphics, m_pImage, CRect(0, 0, nWidth * m_nProgress / m_nMaxProgress, nHeight), 
					m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, 2);
			}
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
			RectF rect((Gdiplus::REAL)(0), (Gdiplus::REAL)0, (Gdiplus::REAL)nWidth, (Gdiplus::REAL)nHeight);
			graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrush);
			::SysFreeString(bsTitle);
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}