#include "StdAfx.h"
#include "MenuItem.h"

CMenuItem::CMenuItem(HWND hWnd, CDuiObject* pDuiObject)
						   : CControlBaseFont(hWnd, pDuiObject)
{
	m_enButtonState = enBSNormal;
	m_bDown = false;
	m_bMouseDown = false;
	m_uVAlignment = VAlign_Middle;
	m_bSelect = FALSE;
	m_bIsSeparator = FALSE;
	m_bIsPopup = FALSE;
	m_pPopupMenu = NULL;
	m_strMenuXml = _T("");

	m_nLeft = 30;
	m_nFrameWidth = 0;

	m_clrHover = Color(254, 71, 156, 235);	// ����ƶ�������ʾ�ı���ɫ
	m_pImageHover = NULL;
	m_sizeHover = CSize(0, 0);
	m_pImagePopupArrow = NULL;
	m_sizePopupArrow = CSize(0, 0);
}

CMenuItem::CMenuItem(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle/*= TEXT("")*/, int nLeft/* = 30*/, BOOL bSelect/* = false*/,
					 BOOL bIsVisible/* = TRUE*/,  BOOL bIsDisable/* = FALSE*/ ,BOOL bIsPressDown/* = FALSE*/)
						   : CControlBaseFont(hWnd, pDuiObject, uControlID, rc, strTitle, bIsVisible, bIsDisable)
{
	m_enButtonState = enBSNormal;
	m_bDown = false;
	m_bMouseDown = false;
	m_uVAlignment = VAlign_Middle;
	m_bSelect = bSelect;
	m_bIsPopup = FALSE;
	m_pPopupMenu = NULL;
	m_strMenuXml = _T("");

	m_nLeft = nLeft;
	m_nFrameWidth = 0;

	m_strGroupName = _T("");
	m_strValue = _T("");

	m_clrHover = Color(254, 71, 156, 235);	// ����ƶ�������ʾ�ı���ɫ
	m_pImageHover = NULL;
	m_sizeHover = CSize(0, 0);
	m_pImagePopupArrow = NULL;
	m_sizePopupArrow = CSize(0, 0);
}

CMenuItem::~CMenuItem(void)
{
	if((m_pPopupMenu != NULL) && IsWindow(m_pPopupMenu->GetSafeHwnd()))
	{
		m_pPopupMenu->CloseWindow();
	}
}

// ���ؼ���XML�ڵ㺯��,�ж��Ƿ����Ӳ˵�
BOOL CMenuItem::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	BOOL bRet = __super::Load(pXmlElem);

	// �ж��Ƿ��ж����Ӳ˵�
	if(pXmlElem && (pXmlElem.first_child() != NULL))
	{
		m_bIsPopup = TRUE;
	}

	// �����Ƕ�ײ˵�(��menu����),��ͨ�����ø��˵���Load������Ƕ�ײ˵���Ϊƽ���˵�����
	CDuiMenu* pParentMenu = GetParentMenu();
	if(pParentMenu && !m_strMenuXml.IsEmpty())
	{
		pParentMenu->LoadXmlFile(m_strMenuXml);
		// �����Ƕ�ײ˵�,�򷵻�FALSE,�����Ͳ��ᴴ���˲˵���,ֻ�ᴴ��Ƕ�ײ˵��ж���Ĳ˵���
		return FALSE;
	}

	return bRet;
}

// ��ȡ���˵�����
CDuiMenu* CMenuItem::GetParentMenu()
{
	CDuiObject* pParentObj = GetParent();
	while((pParentObj != NULL) && (!pParentObj->IsClass(_T("menu"))))
	{
		if(pParentObj->IsClass(_T("popup")))
		{
			pParentObj = ((CDlgPopup*)pParentObj)->GetParent();
		}else
		if(pParentObj->IsClass(_T("dlg")))
		{
			pParentObj = ((CDlgBase*)pParentObj)->GetParent();
		}else
		{
			pParentObj = ((CControlBase*)pParentObj)->GetParent();
		}
	}
	if((pParentObj != NULL) && pParentObj->IsClass(_T("menu")))
	{
		return (CDuiMenu*)pParentObj;
	}

	return NULL;
}

// ��ʾ�����˵�
void CMenuItem::ShowPopupMenu()
{
	// ����Ѿ���ʾ���Ӳ˵�,��ֱ���˳�
	if(m_pPopupMenu)
	{
		return;
	}

	if(m_bIsPopup)
	{
		m_pPopupMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		m_pPopupMenu->SetAutoClose(FALSE);
		m_pPopupMenu->SetParent(this);
		m_pPopupMenu->m_clrRowHover = m_clrHover;	// ���ò˵��˵��ı���ɫ
		CPoint point;
		CRect rc = GetRect();
		point.SetPoint(rc.left + rc.Width(), rc.top);
		int nMenuWidth = rc.Width();

		CDlgBase* pParentDlg = GetParentDialog();
		// ����˵������XML�ļ�,��ʹ�ô˲˵�����XML�ļ������Ӳ˵�
		// ������Ҹ��˵�����,�ҵ���Ӧ��XML�ļ���,ʹ�ô�XML�ļ��������Ӳ˵�
		CString strXmlFile = _T("");		
		CDuiMenu* pParentMenu = GetParentMenu();
		if(pParentMenu)
		{
			if(!m_strMenuXml.IsEmpty())
			{
				// ʹ�ò˵�����XML�ļ�
				strXmlFile = m_strMenuXml;
			}else
			{
				// XML�ļ�����Ϊ���˵������XML�ļ�
				strXmlFile = pParentMenu->GetXmlFile();
			}
			// ���뽫���˵�������Զ��رչص�,�����Ӳ˵���ʾʱ��,���˵�ʧȥ����,���Զ�����
			pParentMenu->SetAutoClose(FALSE);
			// ת���Ӳ˵�������
			::ClientToScreen(pParentMenu->GetSafeHwnd(), &point);

			// �����˵���Ԥ��ֵ�˵����б���ӵ��Ӳ˵���
			for (size_t i = 0; i < pParentMenu->m_vecMenuItemValue.size(); i++)
			{
				MenuItemValue& itemValue = pParentMenu->m_vecMenuItemValue.at(i);
				m_pPopupMenu->m_vecMenuItemValue.push_back(itemValue);
			}
		}

		if(!strXmlFile.IsEmpty())
		{
			BOOL bSucc = m_pPopupMenu->LoadXmlFile(strXmlFile, pParentDlg, point, WM_DUI_MENU, GetName());
			if(bSucc)	// ���ص����˵��ɹ�����ʾ�����˵�
			{
				// ����˵���λ�ò���ʾ
				CRect rc;
				m_pPopupMenu->GetWindowRect(&rc);
				// ���������Ļ�Ҳ෶Χ,��˵����������ƶ�һЩ,�ƶ�����ǰ�˵������
				int nScreenWidth= GetSystemMetrics(SM_CXSCREEN);
				if(rc.right > nScreenWidth)
				{
					//rc.OffsetRect(nScreenWidth - rc.right -10, 0);	// �ƶ�����Ļ���Ҳ�
					rc.OffsetRect(-(nMenuWidth + rc.Width()), 0);	// �ƶ�����ǰ�˵����
				}
				int nScreenHeight= GetSystemMetrics(SM_CYSCREEN);
				if(rc.bottom > nScreenHeight)
				{
					rc.OffsetRect(0, -(rc.Height() - m_rc.Height()));	// �߶ȳ�����Ļ���Ϊ�¶��뷽ʽ
				}
				m_pPopupMenu->MoveWindow(rc);
				m_pPopupMenu->ShowWindow(SW_SHOW);
				m_pPopupMenu->SetAutoClose(TRUE);
			}else
			{
				// �����˵�����ʧ��,ɾ���˵�����
				delete m_pPopupMenu;
				m_pPopupMenu = NULL;
			}
		}
	}
}

BOOL CMenuItem::GetCheck()
{
	return m_bDown;
}

BOOL CMenuItem::SetCheck(BOOL bCheck)
{
	if(!m_bSelect) return false;

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
		// ��������ǿ�,˵����radiobutton��,��ˢ��ͬһ������������radiobtn
		if(!m_strGroupName.IsEmpty())
		{
			ResetGroupCheck();
		}
		UpdateControl();
	}
	return m_bDown;
}

// ���ÿؼ���ѡ��״̬,����ȥ���������ؼ���״̬
void CMenuItem::SetControlCheck(BOOL bCheck)
{
	m_bDown = bCheck;
	m_enButtonState = bCheck ? enBSDown : enBSNormal;
	UpdateControl();
}

// ˢ�¸��ؼ���������ͬһ�����RadioButton�ؼ���״̬
BOOL CMenuItem::ResetGroupCheck()
{
	CDuiObject* pParentObj = GetParent();
	if(pParentObj == NULL)
	{
		return FALSE;
	}

	vector<CControlBase*>* pvecControl = NULL;
	if(pParentObj->IsClass(_T("dlg")))
	{
		CDlgBase* pDlg = static_cast<CDlgBase*>(pParentObj);
		pvecControl = pDlg->GetControls();
	}else
	if(pParentObj->IsClass(_T("popup")))
	{
		CDlgPopup* pDlg = static_cast<CDlgPopup*>(pParentObj);
		pvecControl = pDlg->GetControls();
	}else
	{
		CControlBase* pControlBase = static_cast<CControlBase*>(pParentObj);
		pvecControl = pControlBase->GetControls();
	}

	if(pvecControl == NULL)
	{
		return FALSE;
	}

	for(int i=0; i<(int)pvecControl->size(); i++)
	{
		CControlBase* pControlBase = pvecControl->at(i);
		if(pControlBase->IsClass(CMenuItem::GetClassName()) && pControlBase->GetVisible() && !pControlBase->GetDisable())
		{
			CMenuItem* pControl = static_cast<CMenuItem*>(pControlBase);
			if((pControl->GetGroupName() == m_strGroupName) && (pControl != this))
			{
				// ���ÿؼ�״̬
				pControl->SetControlCheck(FALSE);
			}
		}
	}

	return TRUE;
}

// ��ȡ����Radio��ѡ��Ŀؼ���ֵ
CString CMenuItem::GetGroupValue()
{
	CDuiObject* pParentObj = GetParent();
	if(pParentObj == NULL)
	{
		return _T("");
	}

	vector<CControlBase*>* pvecControl = NULL;
	if(pParentObj->IsClass(_T("dlg")))
	{
		CDlgBase* pDlg = static_cast<CDlgBase*>(pParentObj);
		pvecControl = pDlg->GetControls();
	}else
	if(pParentObj->IsClass(_T("popup")))
	{
		CDlgPopup* pDlg = static_cast<CDlgPopup*>(pParentObj);
		pvecControl = pDlg->GetControls();
	}else
	{
		CControlBase* pControlBase = static_cast<CControlBase*>(pParentObj);
		pvecControl = pControlBase->GetControls();
	}

	if(pvecControl == NULL)
	{
		return _T("");
	}

	for(int i=0; i<(int)pvecControl->size(); i++)
	{
		CControlBase* pControlBase = pvecControl->at(i);
		if(pControlBase->IsClass(CMenuItem::GetClassName()) && pControlBase->GetVisible() && !pControlBase->GetDisable())
		{
			CMenuItem* pControl = static_cast<CMenuItem*>(pControlBase);
			if((pControl->GetGroupName() == m_strGroupName) && pControl->GetCheck())
			{
				return pControl->GetValue();
			}
		}
	}

	return _T("");
}

// ��XML���ü�������
HRESULT CMenuItem::OnAttributeCheck(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	SetCheck(strValue == _T("true"));

	return bLoading?S_FALSE:S_OK;
}

// ����ƶ��¼�����
BOOL CMenuItem::OnControlMouseMove(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable && !m_bMouseDown)
	{
		if(m_rc.PtInRect(point))
		{
			if(m_bSelect)
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
				m_enButtonState = enBSHover;
				// ������Ӳ˵�,����ص����˵�
				if(m_bIsPopup)
				{
					ShowPopupMenu();
				}
			}
		}
		else
		{
			// ������ڵ����˵�,������겻�ڵ����˵���Χ��,��رյ����˵�
			if(m_bIsPopup && (m_pPopupMenu != NULL))
			{
				// ��鸸�˵��ĸ����Ӳ˵�,����굱ǰ�Ƿ��������˵�������
				CDuiMenu* pParentMenu = GetParentMenu();
				//CMenuItem* pHoverItem = pParentMenu->GetHoverMenuItem();
				CMenuItem* pHoverItem = pParentMenu->GetMenuItemWithPoint(point);
				if((pHoverItem != NULL) && (pHoverItem != this))
				{
					// ������������ƽ���Ĳ˵���,��ر��Ӳ˵�
					if(IsWindow(m_pPopupMenu->GetSafeHwnd()))
					{
						m_pPopupMenu->CloseWindow();
					}
					m_pPopupMenu = NULL;
				
					// ���˵��������û��Զ��ر�
					CDuiMenu* pParentMenu = GetParentMenu();
					if(pParentMenu)
					{
						pParentMenu->SetAutoClose(TRUE);
						pParentMenu->SetForegroundWindow();
					}
				}
			}

			if(m_bSelect)
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

// �����������¼�����
BOOL CMenuItem::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_bMouseDown = true;
			if(m_bSelect)
			{
				BOOL bDown = m_bDown;
				if(m_bDown)
				{
					m_enButtonState = enBSHoverDown;					
				}
				else
				{
					m_enButtonState = enBSHover;
				}				
				SendMessage(MSG_BUTTON_CHECK, bDown, true);
			}
			else
			{
				m_enButtonState = enBSDown;
				SendMessage(MSG_BUTTON_DOWN, false, false);
			}
		}
	}
	
	if(buttonState != m_enButtonState)
	{
		//UpdateControl();
		return true;
	}
	return false;
}

// �������ſ��¼�����
BOOL CMenuItem::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	bool bSend = false;
	BOOL bbDown = FALSE;
	bool bSelect = false;
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			if(m_bSelect)
			{
				if(m_bDown)
				{
					m_enButtonState = enBSHoverDown;
				}
				else
				{
					m_enButtonState = enBSHover;
				}
				if(m_bMouseDown)
				{
					// ��������ǿ�,˵����radiobutton��,��ˢ��ͬһ������������radiobtn
					if(!m_strGroupName.IsEmpty())
					{
						m_bDown = true;
						ResetGroupCheck();
					}else
					{
						m_bDown = !m_bDown;
					}
					bSend = false;
					bbDown = m_bDown;
					bSelect = true;
				}				
			}
			else
			{
				m_enButtonState = enBSHover;
				bSend = true;
				
			}
		}
		else
		{
			// ������ڵ����˵�,������겻�ڵ����˵���Χ��,��رո��˵�
			if(m_bIsPopup)
			{
				// ���˵��������û��Զ��ر�
				CDuiMenu* pParentMenu = GetParentMenu();
				if(pParentMenu)
				{
					pParentMenu->SetAutoClose(TRUE);
					pParentMenu->SetForegroundWindow();
				}
			}

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

		if(bSend)
		{
			SendMessage(MSG_BUTTON_UP, bbDown, bSelect);
		}
		return true;
	}
	return false;
}

// ���ÿؼ���������
void  CMenuItem::SetControlDisable(BOOL bIsDisable)
{
	if(m_bIsDisable != bIsDisable)
	{
		m_bIsDisable = bIsDisable;
		if(bIsDisable)
		{
			if(m_bDown && m_bSelect)
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

// ���ؼ�
void CMenuItem::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		int nImageCount = m_bSelect ? 6 : 4;
		if(m_bIsSeparator)
		{
			nImageCount = 1;
		}
		UpdateMemDC(dc, nWidth * nImageCount, nHeight);

		// ˢ��ͼƬ�Ĵ�С(��Ϊm_bSelectѡ���п�����SetBitmap֮���б仯)
		if(m_pImage != NULL)
		{
			if(m_bIsPopup && (m_pImagePopupArrow == NULL))
			{
				// �����˵��ļ�ͷ,�̶�������СͼƬ
				m_sizeImage.SetSize(m_pImage->GetWidth() / 2, m_pImage->GetHeight());
			}else
			if(m_bIsSeparator)
			{
				// �ָ���,ֻ��һ��ͼƬ
				m_sizeImage.SetSize(m_pImage->GetWidth(), m_pImage->GetHeight());
			}else
			if(m_bSelect)
			{
				// checkbox��radiobutton,�̶�Ϊ6��СͼƬ
				m_sizeImage.SetSize(m_pImage->GetWidth() / 6, m_pImage->GetHeight());
			}else
			{
				// �������õ�СͼƬ��������
				m_sizeImage.SetSize(m_pImage->GetWidth() / m_nImagePicCount, m_pImage->GetHeight());
			}
		}

		Graphics graphics(m_memDC);
		CRect  rcTemp(0, 0, nWidth, nHeight);

		for(int i = 0; i < nImageCount; i++)
		{
			m_memDC.BitBlt(i * nWidth, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

			if(enBSHover == i || (enBSDown == i && !m_bSelect) || enBSHoverDown == i)
			{
				// ���˵����
				if(m_pImageHover != NULL)
				{
					// ʹ������ģʽ���Ի�ͼ
					graphics.DrawImage(m_pImageHover, RectF((Gdiplus::REAL)(i * nWidth+m_nFrameWidth), 0, (Gdiplus::REAL)(nWidth-m_nFrameWidth*2), (Gdiplus::REAL)nHeight),
							0, 0, (Gdiplus::REAL)m_sizeHover.cx, (Gdiplus::REAL)m_sizeHover.cy, UnitPixel);
				}else
				{
					// ʹ����ɫ���
					SolidBrush brush(m_clrHover);//Color(254, 71, 156, 235));
					graphics.FillRectangle(&brush, i * nWidth+m_nFrameWidth, 0, nWidth-m_nFrameWidth*2, nHeight);
				}
			}

			// ���˵���ͼƬ
			if(m_pImage != NULL)
			{
				if(m_bIsSeparator)
				{
					// ����Ƿָ���,�����ƽ�̷�ʽ��ͼ
					TextureBrush tileBrush(m_pImage, WrapModeTile);
					graphics.FillRectangle(&tileBrush, RectF((Gdiplus::REAL)rcTemp.left, (Gdiplus::REAL)(rcTemp.top + (nHeight - m_sizeImage.cy) / 2), (Gdiplus::REAL)(nWidth-m_nFrameWidth*2), (Gdiplus::REAL)m_sizeImage.cy));
				}else
				if(m_bIsPopup && (m_pImagePopupArrow == NULL))
				{
					// ����ǵ����˵�,����û�����ò˵��ļ�ͷͼƬ,���ò˵�ͼƬ��Ϊ�Ҳ�ļ�ͷͼƬ
					graphics.DrawImage(m_pImage, Rect(rcTemp.right - m_sizeImage.cx - 6, rcTemp.top + (nHeight - m_sizeImage.cy) / 2, m_sizeImage.cx, m_sizeImage.cy),
						(i % 2) * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
				}else
				if(m_bSelect)
				{
					// checkbox��radiobutton
					graphics.DrawImage(m_pImage, Rect(rcTemp.left + (m_nLeft - m_sizeImage.cx) / 2, rcTemp.top + (nHeight - m_sizeImage.cy) / 2, m_sizeImage.cx, m_sizeImage.cy),
						i * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
				}else
				{
					// ��ͨ�˵����ͼƬ,���СͼƬ��������,��ʹ�õ�һ��СͼƬ
					graphics.DrawImage(m_pImage, Rect(rcTemp.left + (m_nLeft - m_sizeImage.cx) / 2, rcTemp.top + (nHeight - m_sizeImage.cy) / 2, m_sizeImage.cx, m_sizeImage.cy),
						((m_nImagePicCount-1 < i) ? 0 : i) * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
				}
			}

			// ����ǵ����˵�,���������˲˵��ļ�ͷͼƬ,���Ҳ�ļ�ͷͼƬ
			if(m_bIsPopup && (m_pImagePopupArrow != NULL))
			{
				graphics.DrawImage(m_pImagePopupArrow, Rect(rcTemp.right - m_sizePopupArrow.cx - 6, rcTemp.top + (nHeight - m_sizePopupArrow.cy) / 2, m_sizePopupArrow.cx, m_sizePopupArrow.cy),
					(i % 2) * m_sizePopupArrow.cx, 0, m_sizePopupArrow.cx, m_sizePopupArrow.cy, UnitPixel);
			}

			rcTemp.OffsetRect(nWidth, 0);
		}
		
		// ���˵�������
		if(!m_strTitle.IsEmpty())
		{
			m_memDC.SetBkMode(TRANSPARENT);

			rcTemp.SetRect(0, 0, nWidth, nHeight);

			BSTR bsFont = m_strFont.AllocSysString();
			FontFamily fontFamily(bsFont);
			Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
			graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
			::SysFreeString(bsFont);

			// ���ò˵����ֵ�ˮƽ�ʹ�ֱ���뷽ʽ,Ĭ����ˮƽ���������,��ֱ�����м����
			DUI_STRING_ALIGN_DEFINE();
			strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);

			for(int i = 0; i < nImageCount; i++)
			{
				SolidBrush solidBrush(enBSDisable == i ? Color(254, 128, 128, 128) : (enBSHover == i || (enBSDown == i && !m_bSelect) || enBSHoverDown == i ? Color(254, 255, 255, 255) : Color(254, 56, 56, 56)));

				RectF rect((Gdiplus::REAL)(m_nLeft + i * nWidth), (Gdiplus::REAL)0, (Gdiplus::REAL)(nWidth - m_nLeft), (Gdiplus::REAL)nHeight);
				BSTR bsTitle = m_strTitle.AllocSysString();
				graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrush);
				::SysFreeString(bsTitle);
			}
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, m_enButtonState * nWidth, 0, SRCCOPY);
}