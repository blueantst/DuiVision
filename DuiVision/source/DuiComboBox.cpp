#include "StdAfx.h"
#include "DuiComboBox.h"

CDuiComboBox::CDuiComboBox(HWND hWnd, CDuiObject* pDuiObject)
	: CDuiEdit(hWnd, pDuiObject)
{
	m_bReadOnly = true;
	m_nResourceIDHeadBitmap = 0;
	m_nResourceIDDeleteBitmap = 0;
	m_strImageHeadBitmap = _T("");
	m_strImageDeleteBitmap = _T("");
	m_strXmlFile = _T("");
	m_strImageScroll = _T("");
	m_pPopupList = NULL;
	m_nListHeight = 0;
	m_strComboValue = _T("");
	m_clrText = Color(255, 0, 20, 35);
	m_clrDesc = Color(255, 255, 255, 255);
	m_clrHover = Color(225, 0, 147, 209);
}

CDuiComboBox::~CDuiComboBox(void)
{
}

// ���ؼ���XML�ڵ㺯���������²��item����
BOOL CDuiComboBox::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	__super::Load(pXmlElem);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}
	
	// �����²��item�ڵ���Ϣ
	for (DuiXmlNode pItemElem = pXmlElem.child(_T("item")); pItemElem; pItemElem=pItemElem.next_sibling(_T("item")))
	{
		CString strName = pItemElem.attribute(_T("name")).value();
		DuiSystem::Instance()->ParseDuiString(strName);
		CString strDesc = pItemElem.attribute(_T("desc")).value();
		DuiSystem::Instance()->ParseDuiString(strDesc);
		CString strValue = pItemElem.attribute(_T("value")).value();
		DuiSystem::Instance()->ParseDuiString(strValue);
		CString strClrText = pItemElem.attribute(_T("crtext")).value();
		Color clrText = CDuiObject::StringToColor(strClrText, Color(255, 0, 20, 35));
		CString strClrDesc = pItemElem.attribute(_T("crdesc")).value();
		Color clrDesc = CDuiObject::StringToColor(strClrDesc, Color(255, 255, 255, 255));
		UINT nResourceID = 0;
		CString strImage = pItemElem.attribute(_T("image")).value();
		if(!strImage.IsEmpty())
		{
			if(strImage.Find(_T("skin:")) == 0)
			{
				strImage = DuiSystem::Instance()->GetSkin(strImage);
			}

			if(strImage.Find(_T(".")) == -1)	// ����ͼƬ��Դ
			{
				nResourceID = _ttoi(strImage);
				strImage = _T("");
			}
		}

		ComboListItem comboItem;
		comboItem.nResourceID = nResourceID;
		comboItem.strImageFile = strImage;
		comboItem.strName = strName;
		comboItem.strDesc = strDesc;
		comboItem.strValue = strValue;
		comboItem.clrText = clrText;
		comboItem.clrDesc = clrDesc;
		m_vecItem.push_back(comboItem);

		// ����ǵ�ǰֵ,����±༭�����ʾ����
		if(!strValue.IsEmpty() && (strValue == m_strComboValue))
		{
			SetTitle(strName);
		}
	}

    return TRUE;
}

// ����Comboѡ�����ֵ
void CDuiComboBox::SetComboValue(CString strComboValue)
{
	m_strComboValue = strComboValue;

	// ���������б��ѡ����
	if(m_pPopupList)
	{
		m_pPopupList->SetCurItem(strComboValue);
	}

	// ������һ�±༭������Ϊ��,���������б�Ϊ��ʱ��δ����
	SetTitle(_T(""));

	// ���ñ༭������
	for (size_t i = 0; i < m_vecItem.size(); i++)
	{
		ComboListItem* pItem = &(m_vecItem.at(i));
		if(pItem->strValue == strComboValue)
		{
			SetTitle(pItem->strName);
			break;
		}
	}

	UpdateControl();
}

// ��ȡComboѡ�����ֵ
CString CDuiComboBox::GetComboValue()
{
	return m_strComboValue;
}

// ��ȡCombo���������
int CDuiComboBox::GetItemCount()
{
	return m_vecItem.size();
}

// ���Combo��
int CDuiComboBox::AddItem(CString strName, CString strDesc, CString strValue, int nResourceID,
						  CString strImageFile, Color clrText, Color clrDesc)
{
	CString strImage = strImageFile;
	if(!strImage.IsEmpty())
	{
		if(strImage.Find(_T("skin:")) == 0)
		{
			strImage = DuiSystem::Instance()->GetSkin(strImage);
		}

		if(strImage.Find(_T(".")) != -1)	// ͼƬ�ļ�
		{
			/*if(strImage.Find(_T(":")) == -1)
			{
				strImage = DuiSystem::GetExePath() + strImage;
			}*/
		}else	// ����ͼƬ��Դ
		{
			nResourceID = _ttoi(strImage);
			strImage = _T("");
		}
	}

	ComboListItem comboItem;
	comboItem.nResourceID = nResourceID;
	comboItem.strImageFile = strImage;
	comboItem.strName = strName;
	comboItem.strDesc = strDesc;
	comboItem.strValue = strValue;
	comboItem.clrText = clrText;
	comboItem.clrDesc = clrDesc;
	m_vecItem.push_back(comboItem);

	// ����ǵ�ǰֵ,����±༭�����ʾ����
	if(!strValue.IsEmpty() && (strValue == m_strComboValue))
	{
		SetTitle(strName);
	}

	if(m_pPopupList)
	{
		return m_pPopupList->AddItem(strName, strDesc, strValue, nResourceID, strImageFile, clrText, clrDesc);
	}
	return 0;
}

// ���Combo������
void CDuiComboBox::ClearItems()
{
	m_vecItem.clear();

	m_buttonState = enBSNormal;
	m_EditState = enBSNormal;
	InvalidateRect(GetRect());

	// �ر�popuplist����
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->CloseDlgPopup();
	}else
	{
		CloseDlgPopup();
	}

	m_pPopupList = NULL;
}

// ��XML����ͼƬ��Ϣ����
HRESULT CDuiComboBox::OnAttributeHeadImage(const CString& strValue, BOOL bLoading)
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
		m_strImageHeadBitmap = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			m_strImageHeadBitmap = strSkin;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _ttoi(strSkin);
		m_nResourceIDHeadBitmap = nResourceID;
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML����ͼƬ��Ϣ����
HRESULT CDuiComboBox::OnAttributeDeleteImage(const CString& strValue, BOOL bLoading)
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
		m_strImageDeleteBitmap = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			m_strImageDeleteBitmap = strSkin;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _ttoi(strSkin);
		m_nResourceIDDeleteBitmap = nResourceID;
	}

	return bLoading?S_FALSE:S_OK;
}

// ��Ϣ����
LRESULT CDuiComboBox::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if((MSG_CONTROL_BUTTON == uMsg) && (CONTROL_BUTTON == wParam) && (MSG_BUTTON_DOWN == lParam) && (m_pPopupList == NULL))	// ������˱༭���������ť
	{
		CRect rcClient = GetRect();
		rcClient.top = rcClient.bottom;

		CPopupList *pPopupList = new CPopupList;
		m_pPopupList = pPopupList;
		pPopupList->SetParent(this);	// ��PopupList�ĸ��ؼ�ָ��combobox
		pPopupList->SetWidth(rcClient.Width());
		pPopupList->SetHeight(m_nListHeight);
		pPopupList->OnAttributeImageScrollV(m_strImageScroll, FALSE);
		if(m_nResourceIDHeadBitmap != 0)
		{
			pPopupList->SetHeadBitmap(m_nResourceIDHeadBitmap);
		}else
		if(!m_strImageHeadBitmap.IsEmpty())
		{
			pPopupList->SetHeadBitmap(m_strImageHeadBitmap);
		}

		if(m_nResourceIDDeleteBitmap != 0)
		{
			pPopupList->SetCloseBitmap(m_nResourceIDDeleteBitmap);
		}else
		if(!m_strImageDeleteBitmap.IsEmpty())
		{
			pPopupList->SetCloseBitmap(m_strImageDeleteBitmap);
		}
		
		CDlgBase* pDlg = GetParentDialog();
		if(pDlg)
		{
			// ֻ���������б���,�Ȳ���ʾ,������ʾ��ȫ
			pDlg->OpenDlgPopup(pPopupList, rcClient, GetID(), FALSE);
		}else
		{
			// ������Ի��򲻴���,��ʹ��ControlBase��װ�ĺ���,ֻ���������б���,�Ȳ���ʾ,������ʾ��ȫ
			OpenDlgPopup(pPopupList, rcClient, GetID(), FALSE);
		}

		pPopupList->SetFont(m_strFont, m_nFontWidth, m_fontStyle);
		pPopupList->SetHoverColor(m_clrHover);

		// ���봰�ڴ���֮����ܼ�������
		for (size_t i = 0; i < m_vecItem.size(); i++)
		{
			ComboListItem* pItem = &(m_vecItem.at(i));
			pPopupList->AddItem(pItem->strName, pItem->strDesc, pItem->strValue,
				pItem->nResourceID, pItem->strImageFile, pItem->clrText, pItem->clrDesc);
		}

		if(!m_strXmlFile.IsEmpty())
		{
			pPopupList->LoadXmlFile(m_strXmlFile);
		}

		// ����ѡ�����
		SetComboValue(m_strComboValue);

		// ��ʾ�����б���
		pPopupList->ShowWindow(SW_SHOW);
	}else
	if((MSG_CONTROL_SELECT == uMsg) && m_pPopupList)	// ������ѡ��
	{
		CString strName;
		m_pPopupList->GetItemName(lParam, strName);
		m_pPopupList->GetItemValue(lParam, m_strComboValue);
		SetTitle(strName);
		m_buttonState = enBSNormal;
		m_EditState = enBSNormal;
		InvalidateRect(GetRect());
		CDlgBase* pDlg = GetParentDialog();
		if(pDlg)
		{
			pDlg->CloseDlgPopup();
		}else
		{
			CloseDlgPopup();
		}
		m_pPopupList = NULL;
	}else
	if((MSG_CONTROL_DELETE == uMsg) && m_pPopupList)	// ɾ���������б���
	{
		// ���������ɾ����ťͼƬ���ſ��Խ���ɾ��
		if(!m_strImageDeleteBitmap.IsEmpty() || (m_nResourceIDDeleteBitmap != 0))
		{
			m_pPopupList->DeleteItem(lParam);
		}
	}

	return __super::OnMessage(uID, uMsg, wParam, lParam);
}

// �����¼�����
BOOL CDuiComboBox::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// �����ǰ���ڽ���״̬,���¼�ͷ���������б�
	if(m_bDown && (nChar == VK_DOWN) && (nFlags == 0) && IsFocusControl())
	{
		// ģ�������
		SendMessage(MSG_CONTROL_BUTTON, CONTROL_BUTTON, MSG_BUTTON_DOWN);
		return true;
	}

	return false;
}