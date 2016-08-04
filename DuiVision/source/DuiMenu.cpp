#include "StdAfx.h"
#include "DuiMenu.h"

CDuiMenu::CDuiMenu(CString strFont/* = TEXT("����")*/, int nFontWidth/* = 12*/, FontStyle fontStyle/* = FontStyleRegular*/)
	: CDlgPopup()
{
	m_strFont = strFont;
	m_nFontWidth = nFontWidth;
	m_fontStyle = fontStyle;
	m_uAlignment = Align_Left;
	m_uVAlignment = VAlign_Top;

	m_nLeft = 30;
	m_nHeight = 30;
	m_nWidth = 113;
	m_nFrameWidth = 0;
	m_nTopHeight = 0;
	m_nBottomHeight = 0;
	m_nSeparatorHeight = 4;

	m_clrRowHover = Color(254, 71, 156, 235);	// ����ƶ�������ʾ�ı���ɫ
	m_pImageRowHover = NULL;
	m_sizeRowHover = CSize(0, 0);
	m_pImagePopupArrow = NULL;
	m_sizePopupArrow = CSize(0, 0);
}

CDuiMenu::~CDuiMenu(void)
{
	if(m_pImageRowHover != NULL)
	{
		delete m_pImageRowHover;
		m_pImageRowHover = NULL;
	}
	if(m_pImagePopupArrow != NULL)
	{
		delete m_pImagePopupArrow;
		m_pImagePopupArrow = NULL;
	}
}

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiMenu, RowHover, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiMenu, PopupArrow, 2)

BOOL CDuiMenu::Create(CWnd *pParent, CPoint point, UINT uMessageID, UINT nResourceID, int nFrameSize/* = 4*/, int nMinWidth/* = 112*/, enumBackMode enBackMode/* = enBMFrame*/)
{
	CRect rc(point.x, point.y, point.x + nMinWidth, point.y + nMinWidth);
	return CDlgPopup::Create(pParent, rc, uMessageID, nResourceID, enBackMode, nFrameSize);
}

BOOL CDuiMenu::Create(CWnd *pParent, CPoint point, UINT uMessageID, CString strImage, int nFrameSize/* = 4*/, int nMinWidth/* = 112*/, enumBackMode enBackMode/* = enBMFrame*/)
{
	CRect rc(point.x, point.y, point.x + nMinWidth, point.y + nMinWidth);
	return CDlgPopup::Create(pParent, rc, uMessageID, strImage, enBackMode, nFrameSize);
}

BOOL CDuiMenu::Create(CWnd *pParent, CPoint point, UINT uMessageID)
{
	int nMinWidth = m_nWidth;
	CRect rc(point.x, point.y, point.x + nMinWidth, point.y + nMinWidth);
	return CDlgPopup::Create(pParent, rc, uMessageID);
}

void CDuiMenu::DrawWindowEx(CDC &dc, CRect rcClient)
{
}

// ���ؼ���XML�ڵ㺯���������²��Menu item��Ϣ
BOOL CDuiMenu::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	SetRect(CRect(0, 0, m_nWidth, m_nHeight));

	__super::Load(pXmlElem, bLoadSubControl);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	if(!bLoadSubControl)
	{
		// �������ӿؼ�
		return TRUE;
	}

	// �˵����ڿ������Ϊpopup���ڵĿ��
	m_nWidth = m_size.cx;

	// ��������
	Create(m_pParent, m_point, m_uMessageID);
	
	// �����²��item�ڵ���Ϣ(��������¶�ʹ��DlgPopup��Load�ؼ���ʽ���ز˵���,����Ľ����Ƚ����õ�)
	int nIdIndex = 100;
	for (DuiXmlNode pItemElem = pXmlElem.child(_T("item")); pItemElem; pItemElem=pItemElem.next_sibling())
	{
		CString strId = pItemElem.attribute(_T("id")).value();
		int nId = nIdIndex;
		if(strId != _T(""))
		{
			nId = _ttoi(strId);
		}

		CString strType = pItemElem.attribute(_T("type")).value();
		CString strName = pItemElem.attribute(_T("name")).value();
		CString strImage = pItemElem.attribute(_T("image")).value();
		CString strTitle = pItemElem.attribute(_T("title")).value();
		
		if(strType == _T("separator"))
		{
			// �ָ���Ҳ������ͼƬ�ķ�ʽ
			AddSeparator();
			continue;
		}
		CString strTitleU = strTitle;
		if(strImage.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
		{
			CString strImgFile = strImage;
			AddMenu(strTitleU, nIdIndex, strImgFile);
		}else
		if(!strImage.IsEmpty())
		{
			UINT nResourceID = _ttoi(strImage);
			AddMenu(strTitleU, nIdIndex, nResourceID);
		}else
		{
			AddMenu(strTitleU, nIdIndex);
		}

		nIdIndex++;
	}

	// ˢ�¸��˵��ؼ���λ��
	SetMenuPoint();

	m_bInit = TRUE;

    return TRUE;
}

// ����ָ�����ֵĲ˵��ڵ�
BOOL CDuiMenu::LoadSubMenu(DuiXmlNode pXmlElem, CString strSubItemName)
{
	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	// �ݹ�����²�ڵ�,���Ƿ���ָ�����ֵĽڵ�
	for (DuiXmlNode pItemElem = pXmlElem.first_child(); pItemElem; pItemElem=pItemElem.next_sibling())
	{
		CString strName = pItemElem.attribute(_T("name")).value();
		if(strSubItemName == strName)
		{
			// �����Ӳ˵�
			return Load(pItemElem);
		}
		if(LoadSubMenu(pItemElem, strSubItemName))
		{
			// ����ݹ���سɹ��򷵻�,����������±�������
			return TRUE;
		}
	}
	return FALSE;
}

// ����XML�ڵ��ж���Ĳ˵��������ؼ�
BOOL CDuiMenu::LoadXmlNode(DuiXmlNode pXmlElem, CString strXmlFile)
{
	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	for (DuiXmlNode pControlElem = pXmlElem.first_child(); pControlElem; pControlElem=pControlElem.next_sibling())
	{
		CString strControlName = pControlElem.name();
			CControlBase* pControl = _CreateControlByName(strControlName);
			if(pControl)
			{
				if(pControl->Load(pControlElem))
				{
					// ���Load�ɹ�,����ӿؼ�
					if(pControl->IsClass(CArea::GetClassName()) || pControl->IsClass(CDuiFrame::GetClassName()))
					{
						// Area��Frame������Ӧ���,����ӵ�Area�б���
						m_vecArea.push_back(pControl);
					}else
					{
						m_vecControl.push_back(pControl);
					}
					// ����ǲ˵���ؼ�,�����ò˵���Ĳ˵�XML����
					if(pControl->IsClass(CMenuItem::GetClassName()))
					{
						((CMenuItem*)pControl)->SetMenuXml(strXmlFile);
					}
				}else
				{
					// ����ֱ��ɾ���ؼ�����ָ��
					delete pControl;
				}
			}
	}

	return TRUE;
}

// ����XML�ļ��ж���Ĳ˵�
BOOL CDuiMenu::LoadXmlFile(CString strFileName, CString strSubItemName)
{
	DuiXmlDocument xmlDoc;
	DuiXmlNode pDivElem;

	if(DuiSystem::Instance()->LoadXmlFile(xmlDoc, strFileName))
	{
		pDivElem = xmlDoc.child(GetClassName());
		if(pDivElem != NULL)
		{
			if(!strSubItemName.IsEmpty())
			{
				// ����һ��XML��root�ڵ������(�������ӿؼ�)
				Load(pDivElem, FALSE);
				// ָ�����Ӳ˵���,�����Ӳ˵������в��Ҳ�����
				LoadSubMenu(pDivElem, strSubItemName);
			}else
			{
				// ����menu�ڵ�(�������ýڵ�˵����XML�ļ�����)
				LoadXmlNode(pDivElem, strFileName);
			}
		}
		return TRUE;
	}

	return FALSE;
}

// ����XML�ļ�
BOOL CDuiMenu::LoadXmlFile(CString strFileName, CWnd *pParent, CPoint point, UINT uMessageID, CString strSubItemName)
{
	m_pParent = pParent;
	m_point = point;
	m_uMessageID = uMessageID;

	DuiXmlDocument xmlDoc;
	DuiXmlNode pDivElem;

	BOOL bRet = FALSE;
	if(DuiSystem::Instance()->LoadXmlFile(xmlDoc, strFileName))
	{
		m_strXmlFile = strFileName;
		pDivElem = xmlDoc.child(GetClassName());
		if(pDivElem)
		{
			if(!strSubItemName.IsEmpty())
			{
				// ����һ��XML��root�ڵ������(�������ӿؼ�)
				Load(pDivElem, FALSE);
				// ָ�����Ӳ˵���,�����Ӳ˵������в��Ҳ�����
				bRet = LoadSubMenu(pDivElem, strSubItemName);
			}else
			{
				// ����menu�ڵ�����
				bRet = Load(pDivElem);
			}
		}
	}

	return bRet;
}

// UI��ʼ��,�˺����ڴ��ڵ�OnCreate�����е���
void CDuiMenu::InitUI(CRect rcClient)
{
	// ����в˵����Ԥ����ֵ,��������Ӧ��ֵ���ؼ�
	if(m_vecMenuItemValue.size() > 0)
	{
		for (size_t i = 0; i < m_vecMenuItemValue.size(); i++)
		{
			MenuItemValue& itemValue = m_vecMenuItemValue.at(i);
			CControlBase* pControlBase = GetControl(itemValue.strName);
			if(pControlBase)
			{
				if(!itemValue.strTitle.IsEmpty())
				{
					((CControlBaseFont*)pControlBase)->SetTitle(itemValue.strTitle);
				}
				if(!itemValue.bVisible)
				{
					pControlBase->SetVisible(FALSE);
				}
				if(itemValue.bDisable)
				{
					pControlBase->SetDisable(TRUE);
				}
				if(itemValue.nCheck != -1)
				{
					((CMenuItem*)pControlBase)->SetCheck(itemValue.nCheck);
				}
			}
		}

		// ˢ�²˵���λ����Ϣ
		SetMenuPoint();
	}
}

// ��Ӳ˵���Ԥ������Ϣ(���ò˵������)
void CDuiMenu::SetItemTitle(CString strName, CString strTitle)
{
	MenuItemValue itemValue;
	itemValue.strName = strName;
	itemValue.strTitle = strTitle;
	itemValue.bVisible = TRUE;
	itemValue.bDisable = FALSE;
	itemValue.nCheck = -1;
	m_vecMenuItemValue.push_back(itemValue);
}

// ��Ӳ˵���Ԥ������Ϣ(���ò˵���ɼ���)
void CDuiMenu::SetItemVisible(CString strName, BOOL bVisible)
{
	MenuItemValue itemValue;
	itemValue.strName = strName;
	itemValue.strTitle = _T("");
	itemValue.bVisible = bVisible;
	itemValue.bDisable = FALSE;
	itemValue.nCheck = -1;
	m_vecMenuItemValue.push_back(itemValue);
}

// ��Ӳ˵���Ԥ������Ϣ(���ò˵����Ƿ����)
void CDuiMenu::SetItemDisable(CString strName, BOOL bDisable)
{
	MenuItemValue itemValue;
	itemValue.strName = strName;
	itemValue.strTitle = _T("");
	itemValue.bVisible = TRUE;
	itemValue.bDisable = bDisable;
	itemValue.nCheck = -1;
	m_vecMenuItemValue.push_back(itemValue);
}

// ��Ӳ˵���Ԥ������Ϣ(���ò˵����Ƿ�ѡ��)
void CDuiMenu::SetItemCheck(CString strName, int nCheck)
{
	MenuItemValue itemValue;
	itemValue.strName = strName;
	itemValue.strTitle = _T("");
	itemValue.bVisible = TRUE;
	itemValue.bDisable = FALSE;
	itemValue.nCheck = nCheck;
	m_vecMenuItemValue.push_back(itemValue);
}

int CDuiMenu::AddMenu(CString strText, UINT uMenuID, int nResourceID, BOOL bSelect, int nIndex)
{
	CControlBase * pControlBase = NULL;

	BSTR bsFont = m_strFont.AllocSysString();
	FontFamily fontFamily(bsFont);
	Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
	::SysFreeString(bsFont);

	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);
	strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
	Size size = GetTextBounds(font, strFormat, strText);

	if(size.Width > m_nWidth - m_nLeft - 4)
	{
		m_nWidth = size.Width + m_nLeft + 4;
	}

	pControlBase = new CMenuItem(GetSafeHwnd(),this, uMenuID, CRect(0, 0, 0, 0), strText, m_nLeft, bSelect);
	((CControlBaseFont*)pControlBase)->SetFont(m_strFont, m_nFontWidth, m_fontStyle);
	if(nResourceID != -1)
	{
		((CMenuItem *)pControlBase)->SetBitmap(nResourceID);
	}

	if(nIndex >= 0 && nIndex < (int)m_vecControl.size())
	{
		m_vecControl.insert(m_vecControl.begin() + nIndex, pControlBase);
	}
	else
	{
		m_vecControl.push_back(pControlBase);
	}

	SetMenuPoint();
	return m_vecControl.size();
}

int CDuiMenu::AddMenu(CString strText, UINT uMenuID, CString strImage, BOOL bSelect, int nIndex)
{
	CControlBase * pControlBase = NULL;

	BSTR bsFont = m_strFont.AllocSysString();
	FontFamily fontFamily(bsFont);
	Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
	::SysFreeString(bsFont);

	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);
	strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
	Size size = GetTextBounds(font, strFormat, strText);

	if(size.Width > m_nWidth - m_nLeft - 4)
	{
		m_nWidth = size.Width + m_nLeft + 4;
	}

	pControlBase = new CMenuItem(GetSafeHwnd(),this, uMenuID, CRect(0, 0, 0, 0), strText, m_nLeft, bSelect);
	((CControlBaseFont*)pControlBase)->SetFont(m_strFont, m_nFontWidth, m_fontStyle);
	if(!strImage.IsEmpty())
	{
		((CMenuItem *)pControlBase)->SetBitmap(strImage);
	}

	if(nIndex >= 0 && nIndex < (int)m_vecControl.size())
	{
		m_vecControl.insert(m_vecControl.begin() + nIndex, pControlBase);
	}
	else
	{
		m_vecControl.push_back(pControlBase);
	}

	SetMenuPoint();
	return m_vecControl.size();
}

// ��Ӳ˵��ָ�
int CDuiMenu::AddSeparator(int nIndex)
{
	// ����ʹ�þ��οؼ���Ҳ����ʹ��ͼƬ�ؼ�
	CControlBase * pControlBase = new CRectangle(GetSafeHwnd(),this, -1, CRect(0, 0, 0, 0), Color(254, 227, 229, 230));

	if(nIndex >= 0 && nIndex < (int)m_vecControl.size())
	{
		m_vecControl.insert(m_vecControl.begin() + nIndex, pControlBase);
	}
	else
	{
		m_vecControl.push_back(pControlBase);
	}

	SetMenuPoint();
	return m_vecControl.size();
}

// ���ò˵���λ��
void CDuiMenu::SetMenuPoint()
{
	int nXPos = 2;
	int nYPos = (m_nTopHeight != 0) ? m_nTopHeight : 2;
	CRect rc;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl[i];
		if(pControlBase == NULL)
		{
			continue;
		}
		if(pControlBase->IsClass(CMenuItem::GetClassName()))	// �����MenuItem���Ϳؼ�
		{
			CMenuItem* pMenuItem = (CMenuItem*)pControlBase;
			pMenuItem->SetFrameWidth(m_nFrameWidth);
			if(!pMenuItem->GetVisible())
			{
				// �˵���ɼ�
				rc.SetRect(0,0,0,0);
			}else
			if(pMenuItem->IsSeparator())
			{
				// �ָ���
				rc.SetRect(m_nLeft + 1, nYPos + 1, m_nWidth - 1, nYPos + 2);
				nYPos += 4;
			}else
			{
				// ��ͨ�˵���
				rc.SetRect(nXPos, nYPos, m_nWidth - 2, nYPos + m_nHeight);
				nYPos += m_nHeight;
				pMenuItem->m_nLeft = m_nLeft;	// ���ò˵�������������߾�
				// ���ò˵��������ƶ�ʱ��ı���
				if(m_pImageRowHover != NULL)
				{
					pMenuItem->m_pImageHover = m_pImageRowHover;
					pMenuItem->m_sizeHover = m_sizeRowHover;
				}else
				{
					pMenuItem->m_clrHover = m_clrRowHover;	// ���ò˵���ı���ɫ
				}
				// ���ò˵���ĵ����˵���ͷͼƬ
				if(m_pImagePopupArrow != NULL)
				{
					pMenuItem->m_pImagePopupArrow = m_pImagePopupArrow;
					pMenuItem->m_sizePopupArrow = m_sizePopupArrow;
				}
			}
			SetControlRect(pControlBase, rc);
		}else
		if(-1 == pControlBase->GetControlID())
		{
			rc.SetRect(m_nLeft + 4, nYPos + 1, m_nWidth - 9, nYPos + 2);
			nYPos += 4;
			SetControlRect(pControlBase, rc);
		}
	}
	nYPos += ((m_nBottomHeight != 0) ? m_nBottomHeight : 2);
	SetWindowPos(NULL, 0, 0, m_nWidth, nYPos, SWP_NOMOVE);
	SetRect(CRect(0, 0, m_nWidth, nYPos));	// ���ò˵����ڵĴ�С

	// ���÷ǲ˵���ؼ���λ��(�����ڸ߶ȼ������֮������)
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl[i];
		if(pControlBase == NULL)
		{
			continue;
		}
		if(pControlBase->IsClass(CMenuItem::GetClassName()))
		{
			continue;
		}else
		if(-1 == pControlBase->GetControlID())
		{
			continue;
		}else
		{
			pControlBase->OnAttributePosChange(pControlBase->GetPosStr(), FALSE);
		}
	}

	InvalidateRect(NULL);
}

// ��ȡ���˵�����
CDuiMenu* CDuiMenu::GetParentMenu()
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

// ��ȡ��ǰ���ڻ״̬���Ӳ˵���
CMenuItem* CDuiMenu::GetHoverMenuItem()
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl[i];
		if(pControlBase->IsClass(CMenuItem::GetClassName()))	// �����MenuItem���Ϳؼ�
		{
			CMenuItem* pMenuItem = (CMenuItem*)pControlBase;
			if(pMenuItem->IsHover())
			{
				return pMenuItem;
			}
		}
	}

	return NULL;
}

// ��ȡ��ǰ���ڻ״̬���Ӳ˵���(�������λ�ò���)
CMenuItem* CDuiMenu::GetMenuItemWithPoint(CPoint point)
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl[i];
		if(pControlBase->IsClass(CMenuItem::GetClassName()))	// �����MenuItem���Ϳؼ�
		{
			CMenuItem* pMenuItem = (CMenuItem*)pControlBase;
			CRect rc = pMenuItem->GetRect();
			if(rc.PtInRect(point))
			{
				return pMenuItem;
			}
		}
	}

	return NULL;
}

// ��Ϣ��Ӧ
LRESULT CDuiMenu::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((Msg != MSG_BUTTON_UP) && (Msg != MSG_BUTTON_CHECK))
	{
		return 0;
	}

	CControlBase* pControl = GetControl(uID);
	if(pControl && !pControl->GetAction().IsEmpty())
	{
		// ����˵���������action������ӵ�������������У�ͨ��������ִ��
		CString strControlName = pControl->GetName();
		CString strAction = pControl->GetAction();
		//CDuiObject* pParent = pControl->GetParent();
		CDlgBase* pParentDlg = pControl->GetParentDialog();

		DuiSystem::Instance()->AddDuiActionTask(uID, Msg, wParam, lParam, strControlName, strAction, pParentDlg);
	}else
	{
		// ����͵���Popup�ĺ���
		__super::OnMessage(uID, Msg, wParam, lParam);

		/*tagMenuInfo* pMenuInfo = new tagMenuInfo;
		pMenuInfo->uMenuID = uID;
		pMenuInfo->bSelect = (bool)lParam;
		pMenuInfo->bDown = (bool)wParam;
	
		PostMessage(m_uMessageID, Msg, (LPARAM)pMenuInfo);*/
	}

	if(Msg == MSG_BUTTON_UP)
	{
		// ���������ǵ����˵�,���ùر�
		if(pControl && pControl->IsClass(CMenuItem::GetClassName()) && ((CMenuItem*)pControl)->IsPopup())
		{
			return 0;
		}

		// ����и��˵�,�����˵��ر�,������ֱ�ӹرյķ���,���������Զ��رձ�ʶ,��ͨ������¼������Զ��ر�
		CDuiMenu* pParentMenu = GetParentMenu();
		if(pParentMenu && !pParentMenu->IsAutoClose())
		{
			pParentMenu->SetAutoClose(TRUE);
			pParentMenu->SetForegroundWindow();
			pParentMenu->PostMessage(WM_LBUTTONDOWN, 0, 0);
		}
		// �ر�����
		CloseWindow();
	}

	return 0;
}

// ���ش���ȥ����ʱ��Ĺرմ��ڲ���
BOOL CDuiMenu::OnNcCloseWindow()
{
	// ����и��˵�,�����˵����ڹر�
	CDuiMenu* pParentMenu = GetParentMenu();
	if(pParentMenu && !pParentMenu->IsAutoClose())
	{
		// �������ڸ��˵�������,�򲻹رո�����
		CMenuItem* pHoverItem = pParentMenu->GetHoverMenuItem();
		if(pHoverItem == NULL)
		{
			pParentMenu->SetAutoClose(TRUE);
			pParentMenu->SetForegroundWindow();
			pParentMenu->PostMessage(WM_LBUTTONDOWN, 0, 0);
		}
	}

	// ����������ǲ˵���,�򽫲˵����еĵ����˵�ָ������Ϊ��
	CDuiObject* pParent = GetParent();
	if((pParent != NULL) && (pParent->IsClass(CMenuItem::GetClassName())))
	{
		((CMenuItem*)pParent)->SetPopupMenu(NULL);
	}

	// �ر�����
	m_pParentDuiObject = NULL;
	CloseWindow();	

	return TRUE;
}