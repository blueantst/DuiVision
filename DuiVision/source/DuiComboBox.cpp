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

// 重载加载XML节点函数，加载下层的item内容
BOOL CDuiComboBox::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	__super::Load(pXmlElem);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}
	
	// 加载下层的item节点信息
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

			if(strImage.Find(_T(".")) == -1)	// 加载图片资源
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

		if(strImage.Find(_T(".")) != -1)	// 图片文件
		{
			/*if(strImage.Find(_T(":")) == -1)
			{
				strImage = DuiSystem::GetExePath() + strImage;
			}*/
		}else	// 加载图片资源
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

	// 如果是当前值,则更新编辑框的显示内容
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

// 清空Combo下拉项
void CDuiComboBox::ClearItems()
{
	m_vecItem.clear();

	m_buttonState = enBSNormal;
	m_EditState = enBSNormal;
	InvalidateRect(GetRect());

	// 关闭popuplist窗口
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

// 从XML设置图片信息属性
HRESULT CDuiComboBox::OnAttributeHeadImage(const CString& strValue, BOOL bLoading)
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
		m_strImageHeadBitmap = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			m_strImageHeadBitmap = strSkin;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = _ttoi(strSkin);
		m_nResourceIDHeadBitmap = nResourceID;
	}

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置图片信息属性
HRESULT CDuiComboBox::OnAttributeDeleteImage(const CString& strValue, BOOL bLoading)
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
		m_strImageDeleteBitmap = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			m_strImageDeleteBitmap = strSkin;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = _ttoi(strSkin);
		m_nResourceIDDeleteBitmap = nResourceID;
	}

	return bLoading?S_FALSE:S_OK;
}

// 消息处理
LRESULT CDuiComboBox::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if((MSG_CONTROL_BUTTON == uMsg) && (CONTROL_BUTTON == wParam) && (MSG_BUTTON_DOWN == lParam) && (m_pPopupList == NULL))	// 鼠标点击了编辑框的下拉按钮
	{
		CRect rcClient = GetRect();
		rcClient.top = rcClient.bottom;

		CPopupList *pPopupList = new CPopupList;
		m_pPopupList = pPopupList;
		pPopupList->SetParent(this);	// 将PopupList的父控件指向combobox
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
			// 只创建下拉列表窗口,先不显示,避免显示不全
			pDlg->OpenDlgPopup(pPopupList, rcClient, GetID(), FALSE);
		}else
		{
			// 如果父对话框不存在,则使用ControlBase封装的函数,只创建下拉列表窗口,先不显示,避免显示不全
			OpenDlgPopup(pPopupList, rcClient, GetID(), FALSE);
		}

		pPopupList->SetFont(m_strFont, m_nFontWidth, m_fontStyle);
		pPopupList->SetHoverColor(m_clrHover);

		// 必须窗口创建之后才能加载内容
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

		// 设置选择的项
		SetComboValue(m_strComboValue);

		// 显示下拉列表窗口
		pPopupList->ShowWindow(SW_SHOW);
	}else
	if((MSG_CONTROL_SELECT == uMsg) && m_pPopupList)	// 下拉框选择
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
	if((MSG_CONTROL_DELETE == uMsg) && m_pPopupList)	// 删除下拉框列表项
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
		SendMessage(MSG_CONTROL_BUTTON, CONTROL_BUTTON, MSG_BUTTON_DOWN);
		return true;
	}

	return false;
}