#include "StdAfx.h"
#include "DuiMenu.h"

CDuiMenu::CDuiMenu(CString strFont/* = TEXT("宋体")*/, int nFontWidth/* = 12*/, FontStyle fontStyle/* = FontStyleRegular*/)
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

	m_clrRowHover = Color(254, 71, 156, 235);	// 鼠标移动到行显示的背景色
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

// 图片属性的实现
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

// 重载加载XML节点函数，加载下层的Menu item信息
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
		// 不加载子控件
		return TRUE;
	}

	// 菜单窗口宽度设置为popup窗口的宽度
	m_nWidth = m_size.cx;

	// 创建窗口
	Create(m_pParent, m_point, m_uMessageID);
	
	// 加载下层的item节点信息(正常情况下都使用DlgPopup的Load控件方式加载菜单项,下面的解析比较少用到)
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
			// 分隔线也可以用图片的方式
			AddSeparator();
			continue;
		}
		CString strTitleU = strTitle;
		if(strImage.Find(_T(".")) != -1)	// 加载图片文件
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

	// 刷新各菜单控件的位置
	SetMenuPoint();

	m_bInit = TRUE;

    return TRUE;
}

// 加载指定名字的菜单节点
BOOL CDuiMenu::LoadSubMenu(DuiXmlNode pXmlElem, CString strSubItemName)
{
	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	// 递归遍历下层节点,看是否有指定名字的节点
	for (DuiXmlNode pItemElem = pXmlElem.first_child(); pItemElem; pItemElem=pItemElem.next_sibling())
	{
		CString strName = pItemElem.attribute(_T("name")).value();
		if(strSubItemName == strName)
		{
			// 加载子菜单
			return Load(pItemElem);
		}
		if(LoadSubMenu(pItemElem, strSubItemName))
		{
			// 如果递归加载成功则返回,否则继续向下遍历查找
			return TRUE;
		}
	}
	return FALSE;
}

// 加载XML节点中定义的菜单和其他控件
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
					// 如果Load成功,则添加控件
					if(pControl->IsClass(CArea::GetClassName()))
					{
						// Area不能响应鼠标,必须加到Area列表中
						m_vecArea.push_back(pControl);
					}else
					{
						m_vecControl.push_back(pControl);
					}
					// 如果是菜单项控件,则设置菜单项的菜单XML属性
					if(pControl->IsClass(CMenuItem::GetClassName()))
					{
						((CMenuItem*)pControl)->SetMenuXml(strXmlFile);
					}
				}else
				{
					// 否则直接删除控件对象指针
					delete pControl;
				}
			}
	}

	return TRUE;
}

// 加载XML文件中定义的菜单
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
				// 加载一下XML的root节点的内容(不加载子控件)
				Load(pDivElem, FALSE);
				// 指定了子菜单名,则按照子菜单名进行查找并加载
				LoadSubMenu(pDivElem, strSubItemName);
			}else
			{
				// 加载menu节点(可以设置节点菜单项的XML文件属性)
				LoadXmlNode(pDivElem, strFileName);
			}
		}
		return TRUE;
	}

	return FALSE;
}

// 加载XML文件
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
				// 加载一下XML的root节点的内容(不加载子控件)
				Load(pDivElem, FALSE);
				// 指定了子菜单名,则按照子菜单名进行查找并加载
				bRet = LoadSubMenu(pDivElem, strSubItemName);
			}else
			{
				// 加载menu节点属性
				bRet = Load(pDivElem);
			}
		}
	}

	return bRet;
}

// UI初始化,此函数在窗口的OnCreate函数中调用
void CDuiMenu::InitUI(CRect rcClient)
{
	__super::InitUI(rcClient);

	// 如果有菜单项的预设置值,则设置相应的值到控件
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

		// 刷新菜单项位置信息
		SetMenuPoint();
	}
}

// 添加菜单项预设置信息(设置菜单项标题)
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

// 添加菜单项预设置信息(设置菜单项可见性)
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

// 添加菜单项预设置信息(设置菜单项是否禁用)
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

// 添加菜单项预设置信息(设置菜单项是否选择)
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

// 添加菜单分隔
int CDuiMenu::AddSeparator(int nIndex)
{
	// 可以使用矩形控件，也可以使用图片控件
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

// 设置菜单项位置
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
		if(pControlBase->IsClass(CMenuItem::GetClassName()))	// 如果是MenuItem类型控件
		{
			CMenuItem* pMenuItem = (CMenuItem*)pControlBase;
			pMenuItem->SetFrameWidth(m_nFrameWidth);
			if(!pMenuItem->GetVisible())
			{
				// 菜单项不可见
				rc.SetRect(0,0,0,0);
			}else
			if(pMenuItem->IsSeparator())
			{
				// 分隔线
				rc.SetRect(m_nLeft + 1, nYPos + 1, m_nWidth - 1, nYPos + 2);
				nYPos += 4;
			}else
			{
				// 普通菜单项
				rc.SetRect(nXPos, nYPos, m_nWidth - 2, nYPos + m_nHeight);
				nYPos += m_nHeight;
				pMenuItem->m_nLeft = m_nLeft;	// 设置菜单项对象的文字左边距
				// 设置菜单项的鼠标移动时候的背景
				if(m_pImageRowHover != NULL)
				{
					pMenuItem->m_pImageHover = m_pImageRowHover;
					pMenuItem->m_sizeHover = m_sizeRowHover;
				}else
				{
					pMenuItem->m_clrHover = m_clrRowHover;	// 设置菜单项的背景色
				}
				// 设置菜单项的弹出菜单箭头图片
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
	SetRect(CRect(0, 0, m_nWidth, nYPos));	// 设置菜单窗口的大小

	// 设置非菜单项控件的位置(必须在高度计算出来之后设置)
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

// 获取父菜单对象
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

// 获取当前处于活动状态的子菜单项
CMenuItem* CDuiMenu::GetHoverMenuItem()
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl[i];
		if(pControlBase->IsClass(CMenuItem::GetClassName()))	// 如果是MenuItem类型控件
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

// 获取当前处于活动状态的子菜单项(根据鼠标位置查找)
CMenuItem* CDuiMenu::GetMenuItemWithPoint(CPoint point)
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl[i];
		if(pControlBase->IsClass(CMenuItem::GetClassName()))	// 如果是MenuItem类型控件
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

// 消息响应
LRESULT CDuiMenu::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((Msg != MSG_BUTTON_UP) && (Msg != MSG_BUTTON_CHECK))
	{
		return 0;
	}

	CControlBase* pControl = GetControl(uID);
	if(pControl && !pControl->GetAction().IsEmpty())
	{
		// 如果菜单项设置了action，则添加到动作任务队列中，通过任务来执行
		CString strControlName = pControl->GetName();
		CString strAction = pControl->GetAction();
		//CDuiObject* pParent = pControl->GetParent();
		CDlgBase* pParentDlg = pControl->GetParentDialog();

		DuiSystem::Instance()->AddDuiActionTask(uID, Msg, wParam, lParam, strControlName, strAction, pParentDlg);
	}else
	{
		// 否则就调用Popup的函数
		__super::OnMessage(uID, Msg, wParam, lParam);

		/*tagMenuInfo* pMenuInfo = new tagMenuInfo;
		pMenuInfo->uMenuID = uID;
		pMenuInfo->bSelect = (bool)lParam;
		pMenuInfo->bDown = (bool)wParam;
	
		PostMessage(m_uMessageID, Msg, (LPARAM)pMenuInfo);*/
	}

	if(Msg == MSG_BUTTON_UP)
	{
		// 如果点击的是弹出菜单,则不用关闭
		if(pControl && pControl->IsClass(CMenuItem::GetClassName()) && ((CMenuItem*)pControl)->IsPopup())
		{
			return 0;
		}

		// 如果有父菜单,将父菜单关闭,不采用直接关闭的方法,而是设置自动关闭标识,并通过鼠标事件触发自动关闭
		CDuiMenu* pParentMenu = GetParentMenu();
		if(pParentMenu && !pParentMenu->IsAutoClose())
		{
			pParentMenu->SetAutoClose(TRUE);
			pParentMenu->SetForegroundWindow();
			pParentMenu->PostMessage(WM_LBUTTONDOWN, 0, 0);
		}
		// 关闭自身
		CloseWindow();
	}

	return 0;
}

// 重载窗口去激活时候的关闭窗口操作
BOOL CDuiMenu::OnNcCloseWindow()
{
	// 如果有父菜单,将父菜单窗口关闭
	CDuiMenu* pParentMenu = GetParentMenu();
	if(pParentMenu && !pParentMenu->IsAutoClose())
	{
		// 如果鼠标在父菜单窗口中,则不关闭父窗口
		CMenuItem* pHoverItem = pParentMenu->GetHoverMenuItem();
		if(pHoverItem == NULL)
		{
			pParentMenu->SetAutoClose(TRUE);
			pParentMenu->SetForegroundWindow();
			pParentMenu->PostMessage(WM_LBUTTONDOWN, 0, 0);
		}
	}

	// 如果父对象是菜单项,则将菜单项中的弹出菜单指针设置为空
	CDuiObject* pParent = GetParent();
	if((pParent != NULL) && (pParent->IsClass(CMenuItem::GetClassName())))
	{
		((CMenuItem*)pParent)->SetPopupMenu(NULL);
	}

	// 关闭自身
	m_pParentDuiObject = NULL;
	CloseWindow();	

	return TRUE;
}