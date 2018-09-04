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

	m_clrHover = Color(254, 71, 156, 235);	// 鼠标移动到行显示的背景色
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

	m_clrHover = Color(254, 71, 156, 235);	// 鼠标移动到行显示的背景色
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

// 重载加载XML节点函数,判断是否有子菜单
BOOL CMenuItem::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	BOOL bRet = __super::Load(pXmlElem);

	// 判断是否有定义子菜单
	if(pXmlElem && (pXmlElem.first_child() != NULL))
	{
		m_bIsPopup = TRUE;
	}

	// 如果是嵌套菜单(有menu属性),则通过调用父菜单的Load函数将嵌套菜单作为平级菜单加载
	CDuiMenu* pParentMenu = GetParentMenu();
	if(pParentMenu && !m_strMenuXml.IsEmpty())
	{
		pParentMenu->LoadXmlFile(m_strMenuXml);
		// 如果是嵌套菜单,则返回FALSE,这样就不会创建此菜单项,只会创建嵌套菜单中定义的菜单项
		return FALSE;
	}

	return bRet;
}

// 获取父菜单对象
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

// 显示弹出菜单
void CMenuItem::ShowPopupMenu()
{
	// 如果已经显示了子菜单,则直接退出
	if(m_pPopupMenu)
	{
		return;
	}

	if(m_bIsPopup)
	{
		m_pPopupMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);
		m_pPopupMenu->SetAutoClose(FALSE);
		m_pPopupMenu->SetParent(this);
		m_pPopupMenu->m_clrRowHover = m_clrHover;	// 设置菜单菜单的背景色
		CPoint point;
		CRect rc = GetRect();
		point.SetPoint(rc.left + rc.Width(), rc.top);
		int nMenuWidth = rc.Width();

		CDlgBase* pParentDlg = GetParentDialog();
		// 如果菜单项定义了XML文件,则使用此菜单项定义的XML文件加载子菜单
		// 否则查找父菜单对象,找到对应的XML文件名,使用此XML文件名加载子菜单
		CString strXmlFile = _T("");		
		CDuiMenu* pParentMenu = GetParentMenu();
		if(pParentMenu)
		{
			if(!m_strMenuXml.IsEmpty())
			{
				// 使用菜单项定义的XML文件
				strXmlFile = m_strMenuXml;
			}else
			{
				// XML文件设置为父菜单对象的XML文件
				strXmlFile = pParentMenu->GetXmlFile();
			}
			// 必须将父菜单对象的自动关闭关掉,否则子菜单显示时候,父菜单失去焦点,会自动销毁
			pParentMenu->SetAutoClose(FALSE);
			// 转换子菜单的坐标
			::ClientToScreen(pParentMenu->GetSafeHwnd(), &point);

			// 将父菜单的预设值菜单项列表添加到子菜单中
			for (size_t i = 0; i < pParentMenu->m_vecMenuItemValue.size(); i++)
			{
				MenuItemValue& itemValue = pParentMenu->m_vecMenuItemValue.at(i);
				m_pPopupMenu->m_vecMenuItemValue.push_back(itemValue);
			}
		}

		if(!strXmlFile.IsEmpty())
		{
			BOOL bSucc = m_pPopupMenu->LoadXmlFile(strXmlFile, pParentDlg, point, WM_DUI_MENU, GetName());
			if(bSucc)	// 加载弹出菜单成功才显示弹出菜单
			{
				// 计算菜单的位置并显示
				CRect rc;
				m_pPopupMenu->GetWindowRect(&rc);
				// 如果超出屏幕右侧范围,则菜单窗口往左移动一些,移动到当前菜单的左侧
				int nScreenWidth= GetSystemMetrics(SM_CXSCREEN);
				if(rc.right > nScreenWidth)
				{
					//rc.OffsetRect(nScreenWidth - rc.right -10, 0);	// 移动到屏幕最右侧
					rc.OffsetRect(-(nMenuWidth + rc.Width()), 0);	// 移动到当前菜单左侧
				}
				int nScreenHeight= GetSystemMetrics(SM_CYSCREEN);
				if(rc.bottom > nScreenHeight)
				{
					rc.OffsetRect(0, -(rc.Height() - m_rc.Height()));	// 高度超出屏幕则改为下对齐方式
				}
				m_pPopupMenu->MoveWindow(rc);
				m_pPopupMenu->ShowWindow(SW_SHOW);
				m_pPopupMenu->SetAutoClose(TRUE);
			}else
			{
				// 弹出菜单加载失败,删除菜单对象
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
		// 如果组名非空,说明是radiobutton组,则刷新同一个组下面其他radiobtn
		if(!m_strGroupName.IsEmpty())
		{
			ResetGroupCheck();
		}
		UpdateControl();
	}
	return m_bDown;
}

// 设置控件的选择状态,不会去设置其他控件的状态
void CMenuItem::SetControlCheck(BOOL bCheck)
{
	m_bDown = bCheck;
	m_enButtonState = bCheck ? enBSDown : enBSNormal;
	UpdateControl();
}

// 刷新父控件下面所有同一个组的RadioButton控件的状态
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
				// 重置控件状态
				pControl->SetControlCheck(FALSE);
			}
		}
	}

	return TRUE;
}

// 获取所在Radio组选择的控件的值
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

// 从XML设置检查框属性
HRESULT CMenuItem::OnAttributeCheck(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	SetCheck(strValue == _T("true"));

	return bLoading?S_FALSE:S_OK;
}

// 鼠标移动事件处理
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
				// 如果有子菜单,则加载弹出菜单
				if(m_bIsPopup)
				{
					ShowPopupMenu();
				}
			}
		}
		else
		{
			// 如果存在弹出菜单,并且鼠标不在弹出菜单范围内,则关闭弹出菜单
			if(m_bIsPopup && (m_pPopupMenu != NULL))
			{
				// 检查父菜单的各个子菜单,看鼠标当前是否在其他菜单项上面
				CDuiMenu* pParentMenu = GetParentMenu();
				//CMenuItem* pHoverItem = pParentMenu->GetHoverMenuItem();
				CMenuItem* pHoverItem = pParentMenu->GetMenuItemWithPoint(point);
				if((pHoverItem != NULL) && (pHoverItem != this))
				{
					// 如果鼠标在其他平级的菜单上,则关闭子菜单
					if(IsWindow(m_pPopupMenu->GetSafeHwnd()))
					{
						m_pPopupMenu->CloseWindow();
					}
					m_pPopupMenu = NULL;
				
					// 父菜单对象设置回自动关闭
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

// 鼠标左键按下事件处理
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

// 鼠标左键放开事件处理
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
					// 如果组名非空,说明是radiobutton组,则刷新同一个组下面其他radiobtn
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
			// 如果存在弹出菜单,并且鼠标不在弹出菜单范围内,则关闭父菜单
			if(m_bIsPopup)
			{
				// 父菜单对象设置回自动关闭
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

// 设置控件禁用属性
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

// 画控件
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

		// 刷新图片的大小(因为m_bSelect选项有可能在SetBitmap之后有变化)
		if(m_pImage != NULL)
		{
			if(m_bIsPopup && (m_pImagePopupArrow == NULL))
			{
				// 弹出菜单的箭头,固定是两个小图片
				m_sizeImage.SetSize(m_pImage->GetWidth() / 2, m_pImage->GetHeight());
			}else
			if(m_bIsSeparator)
			{
				// 分隔线,只有一个图片
				m_sizeImage.SetSize(m_pImage->GetWidth(), m_pImage->GetHeight());
			}else
			if(m_bSelect)
			{
				// checkbox或radiobutton,固定为6个小图片
				m_sizeImage.SetSize(m_pImage->GetWidth() / 6, m_pImage->GetHeight());
			}else
			{
				// 按照设置的小图片个数计算
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
				// 画菜单项背景
				if(m_pImageHover != NULL)
				{
					// 使用拉伸模式属性画图
					graphics.DrawImage(m_pImageHover, RectF((Gdiplus::REAL)(i * nWidth+m_nFrameWidth), 0, (Gdiplus::REAL)(nWidth-m_nFrameWidth*2), (Gdiplus::REAL)nHeight),
							0, 0, (Gdiplus::REAL)m_sizeHover.cx, (Gdiplus::REAL)m_sizeHover.cy, UnitPixel);
				}else
				{
					// 使用颜色填充
					SolidBrush brush(m_clrHover);//Color(254, 71, 156, 235));
					graphics.FillRectangle(&brush, i * nWidth+m_nFrameWidth, 0, nWidth-m_nFrameWidth*2, nHeight);
				}
			}

			// 画菜单项图片
			if(m_pImage != NULL)
			{
				if(m_bIsSeparator)
				{
					// 如果是分隔线,则采用平铺方式画图
					TextureBrush tileBrush(m_pImage, WrapModeTile);
					graphics.FillRectangle(&tileBrush, RectF((Gdiplus::REAL)rcTemp.left, (Gdiplus::REAL)(rcTemp.top + (nHeight - m_sizeImage.cy) / 2), (Gdiplus::REAL)(nWidth-m_nFrameWidth*2), (Gdiplus::REAL)m_sizeImage.cy));
				}else
				if(m_bIsPopup && (m_pImagePopupArrow == NULL))
				{
					// 如果是弹出菜单,并且没有设置菜单的箭头图片,则用菜单图片作为右侧的箭头图片
					graphics.DrawImage(m_pImage, Rect(rcTemp.right - m_sizeImage.cx - 6, rcTemp.top + (nHeight - m_sizeImage.cy) / 2, m_sizeImage.cx, m_sizeImage.cy),
						(i % 2) * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
				}else
				if(m_bSelect)
				{
					// checkbox或radiobutton
					graphics.DrawImage(m_pImage, Rect(rcTemp.left + (m_nLeft - m_sizeImage.cx) / 2, rcTemp.top + (nHeight - m_sizeImage.cy) / 2, m_sizeImage.cx, m_sizeImage.cy),
						i * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
				}else
				{
					// 普通菜单项的图片,如果小图片个数不足,则使用第一个小图片
					graphics.DrawImage(m_pImage, Rect(rcTemp.left + (m_nLeft - m_sizeImage.cx) / 2, rcTemp.top + (nHeight - m_sizeImage.cy) / 2, m_sizeImage.cx, m_sizeImage.cy),
						((m_nImagePicCount-1 < i) ? 0 : i) * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
				}
			}

			// 如果是弹出菜单,并且设置了菜单的箭头图片,则画右侧的箭头图片
			if(m_bIsPopup && (m_pImagePopupArrow != NULL))
			{
				graphics.DrawImage(m_pImagePopupArrow, Rect(rcTemp.right - m_sizePopupArrow.cx - 6, rcTemp.top + (nHeight - m_sizePopupArrow.cy) / 2, m_sizePopupArrow.cx, m_sizePopupArrow.cy),
					(i % 2) * m_sizePopupArrow.cx, 0, m_sizePopupArrow.cx, m_sizePopupArrow.cy, UnitPixel);
			}

			rcTemp.OffsetRect(nWidth, 0);
		}
		
		// 画菜单项文字
		if(!m_strTitle.IsEmpty())
		{
			m_memDC.SetBkMode(TRANSPARENT);

			rcTemp.SetRect(0, 0, nWidth, nHeight);

			BSTR bsFont = m_strFont.AllocSysString();
			FontFamily fontFamily(bsFont);
			Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
			graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
			::SysFreeString(bsFont);

			// 设置菜单文字的水平和垂直对齐方式,默认是水平方向左对齐,垂直方向中间对齐
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