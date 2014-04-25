#include "StdAfx.h"
#include "ComboBox.h"

CDuiComboBox::CDuiComboBox(HWND hWnd, CDuiObject* pDuiObject)
	: CEditEx(hWnd, pDuiObject)
{
	m_bReadOnly = true;
	m_nResourceIDHeadBitmap = 0;
	m_nResourceIDDeleteBitmap = 0;
	m_strImageHeadBitmap = _T("");
	m_strImageDeleteBitmap = _T("");
	m_strXmlFile = _T("");
	m_pPopupList = NULL;
	m_strComboValue = _T("");
}

CDuiComboBox::~CDuiComboBox(void)
{
}

// 重载加载XML节点函数，加载下层的item内容
BOOL CDuiComboBox::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
{
	__super::Load(pXmlElem);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}
	
	// 加载下层的item节点信息
	TiXmlElement* pItemElem = NULL;
	for (pItemElem = pXmlElem->FirstChildElement("item"); pItemElem != NULL; pItemElem=pItemElem->NextSiblingElement())
	{
		CStringA strNameA = pItemElem->Attribute("name");
		DuiSystem::Instance()->ParseDuiString(strNameA);
		CString strName = CA2T(strNameA, CP_UTF8);
		CStringA strDescA = pItemElem->Attribute("desc");
		DuiSystem::Instance()->ParseDuiString(strDescA);
		CString strDesc = CA2T(strDescA, CP_UTF8);
		CStringA strValueA = pItemElem->Attribute("value");
		DuiSystem::Instance()->ParseDuiString(strValueA);
		CString strValue = CA2T(strValueA, CP_UTF8);
		UINT nResourceID = 0;
		CStringA strImageA = pItemElem->Attribute("image");
		CString strImage = CA2T(strImageA, CP_UTF8);
		if(!strImage.IsEmpty())
		{
			if(strImage.Find(_T("skin:")) == 0)
			{
				strImage = CEncodingUtil::AnsiToUnicode(DuiSystem::Instance()->GetSkin(CEncodingUtil::UnicodeToAnsi(strImage)));
			}

			if(strImage.Find(_T(".")) != -1)	// 加载图片文件
			{
				strImage = DuiSystem::GetExePath() + strImage;
			}else	// 加载图片资源
			{
				nResourceID = _wtoi(strImage);
				strImage = _T("");
			}
		}

		ComboListItem comboItem;
		comboItem.nResourceID = nResourceID;
		comboItem.strImageFile = strImage;
		comboItem.strName = strName;
		comboItem.strDesc = strDesc;
		comboItem.strValue = strValue;
		m_vecItem.push_back(comboItem);

		// 如果是当前值,则更新编辑框的显示内容
		if(!strValue.IsEmpty() && (strValue == m_strComboValue))
		{
			SetTitle(strName);
		}
	}

    return TRUE;
}

// 设置Combo选择项的值
void CDuiComboBox::SetComboValue(CString strComboValue)
{
	m_strComboValue = strComboValue;

	// 设置下拉列表的选择项
	if(m_pPopupList)
	{
		m_pPopupList->SetCurItem(strComboValue);
	}

	// 先设置一下编辑框内容为空,避免下拉列表为空时候未设置
	SetTitle(_T(""));

	// 设置编辑框内容
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

// 获取Combo选择项的值
CString CDuiComboBox::GetComboValue()
{
	return m_strComboValue;
}

// 获取Combo下拉项个数
int CDuiComboBox::GetItemCount()
{
	return m_vecItem.size();
}

// 添加Combo项
int CDuiComboBox::AddItem(CString strName, CString strDesc, CString strValue, int nResourceID, CString strImageFile)
{
	CString strImage = strImageFile;
	if(!strImage.IsEmpty())
	{
		if(strImage.Find(_T("skin:")) == 0)
		{
			strImage = CEncodingUtil::AnsiToUnicode(DuiSystem::Instance()->GetSkin(CEncodingUtil::UnicodeToAnsi(strImage)));
		}

		if(strImage.Find(_T(".")) != -1)	// 图片文件
		{
			if(strImage.Find(_T(":")) == -1)
			{
				strImage = DuiSystem::GetExePath() + strImage;
			}
		}else	// 加载图片资源
		{
			nResourceID = _wtoi(strImage);
			strImage = _T("");
		}
	}

	ComboListItem comboItem;
	comboItem.nResourceID = nResourceID;
	comboItem.strImageFile = strImage;
	comboItem.strName = strName;
	comboItem.strDesc = strDesc;
	comboItem.strValue = strValue;
	m_vecItem.push_back(comboItem);

	// 如果是当前值,则更新编辑框的显示内容
	if(!strValue.IsEmpty() && (strValue == m_strComboValue))
	{
		SetTitle(strName);
	}

	if(m_pPopupList)
	{
		return m_pPopupList->AddItem(strName, strDesc, strValue, nResourceID, strImageFile);
	}
	return 0;
}

// 清空Combo下拉项
void CDuiComboBox::ClearItems()
{
	m_vecItem.clear();

	// 关闭popuplist窗口
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		m_buttonState = enBSNormal;
		m_EditState = enBSNormal;
		InvalidateRect(GetRect());
		pDlg->CloseDlgPopup();
		m_pPopupList = NULL;
	}
}

// 从XML设置图片信息属性
HRESULT CDuiComboBox::OnAttributeHeadImage(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CStringA strSkin = "";
	if(strValue.Find("skin:") == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(".") != -1)	// 加载图片文件
	{
		m_strImageHeadBitmap = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			m_strImageHeadBitmap = CA2T(strSkin, CP_UTF8);
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = atoi(strSkin);
		m_nResourceIDHeadBitmap = nResourceID;
	}

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置图片信息属性
HRESULT CDuiComboBox::OnAttributeDeleteImage(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CStringA strSkin = "";
	if(strValue.Find("skin:") == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(".") != -1)	// 加载图片文件
	{
		m_strImageDeleteBitmap = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			m_strImageDeleteBitmap = CA2T(strSkin, CP_UTF8);
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = atoi(strSkin);
		m_nResourceIDDeleteBitmap = nResourceID;
	}

	return bLoading?S_FALSE:S_OK;
}

// 消息处理
LRESULT CDuiComboBox::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if((BUTTOM == wParam) && (BUTTOM_DOWN == lParam) && (m_pPopupList == NULL))	// 鼠标点击了编辑框的下拉按钮
	{
		CPopupList *pPopupList = new CPopupList;
		m_pPopupList = pPopupList;
		pPopupList->SetParent(this);	// 将PopupList的父控件指向combobox
		pPopupList->SetWidth(m_nWidth);
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
			pPopupList->SetDeleteBitmap(m_nResourceIDDeleteBitmap);
		}else
		if(!m_strImageDeleteBitmap.IsEmpty())
		{
			pPopupList->SetDeleteBitmap(m_strImageDeleteBitmap);
		}
		
		CRect rcClient = GetRect();
		rcClient.top = rcClient.bottom;
		CDlgBase* pDlg = GetParentDialog();
		if(pDlg)
		{
			pDlg->OpenDlgPopup(pPopupList, rcClient, GetID());
		}

		// 必须窗口创建之后才能加载内容
		for (size_t i = 0; i < m_vecItem.size(); i++)
		{
			ComboListItem* pItem = &(m_vecItem.at(i));
			pPopupList->AddItem(pItem->strName, pItem->strDesc, pItem->strValue,
				pItem->nResourceID, pItem->strImageFile);
		}

		if(!m_strXmlFile.IsEmpty())
		{
			pPopupList->LoadXmlFile(m_strXmlFile);
		}

		// 设置选择的项
		SetComboValue(m_strComboValue);
	}else
	if((SELECT_ITEM == wParam) && m_pPopupList)	// 下拉框选择
	{
		CString strName;
		m_pPopupList->GetItemName(lParam, strName);
		m_pPopupList->GetItemValue(lParam, m_strComboValue);
		SetTitle(strName);
		CDlgBase* pDlg = GetParentDialog();
		if(pDlg)
		{
			m_buttonState = enBSNormal;
			m_EditState = enBSNormal;
			InvalidateRect(GetRect());
			pDlg->CloseDlgPopup();
			m_pPopupList = NULL;
		}
	}else
	if((DELETE_ITEM == wParam) && m_pPopupList)	// 删除下拉框列表项
	{
		// 如果设置了删除按钮图片，才可以进行删除
		if(!m_strImageDeleteBitmap.IsEmpty() || (m_nResourceIDDeleteBitmap != 0))
		{
			m_pPopupList->DeleteItem(lParam);
		}
	}

	return __super::OnMessage(uID, uMsg, wParam, lParam);
}

// 键盘事件处理
BOOL CDuiComboBox::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 如果当前处于焦点状态,用下箭头可以下拉列表
	if(m_bDown && (nChar == VK_DOWN) && (nFlags == 0) && IsFocusControl())
	{
		// 模拟鼠标点击
		SendMessage(m_uID, BUTTOM, BUTTOM_DOWN);
		return true;
	}

	return false;
}