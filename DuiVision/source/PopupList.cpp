#include "StdAfx.h"
#include "PopupList.h"

CPopupList::CPopupList(void)
{
	m_nHoverItem = -1;
	m_pImageHead = NULL;
	m_pImageClose = NULL;
	m_rcClose.SetRectEmpty();
	m_buttonState = enBSNormal;
	m_nWidth = 191;

	VERIFY(m_font.CreateFont(
		18,							// 字体的高度  
		0,							// 字体的宽度  
		0,							// 字体显示的角度
		0,							// 字体的角度
		FW_DONTCARE,				// 字体的磅数
		FALSE,						// 斜体字体
		FALSE,						// 带下划线的字体
		0,							// 带删除线的字体
		GB2312_CHARSET,				// 所需的字符集
		OUT_DEFAULT_PRECIS,			// 输出的精度
		CLIP_DEFAULT_PRECIS,		// 裁减的精度
		DEFAULT_QUALITY,			// 逻辑字体与输出设备的实际
		DEFAULT_PITCH | FF_SWISS,	// 字体间距和字体集
		DuiSystem::GetDefaultFont()));	// 字体名称
}

CPopupList::~CPopupList(void)
{
	// 释放图片资源
	if(m_pImageHead != NULL)
	{
		delete m_pImageHead;
		m_pImageHead = NULL;
	}

	if(m_pImageClose != NULL)
	{
		delete m_pImageClose;
		m_pImageClose = NULL;
	}

	for(int i = 0; i < (int)m_vecItem.size(); i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		if(editListItem.pImage != NULL)
		{
			delete editListItem.pImage;
			editListItem.pImage = NULL;
		}
	}

	// 判断如果父控件是combo的话,将combo中的指针设置为空
	CDuiObject* pParent = GetParent();
	if(pParent->IsClass(CDuiComboBox::GetClassName()))
	{
		((CDuiComboBox*)pParent)->m_pPopupList = NULL;
	}
}

// 图片属性的实现
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CPopupList, Head, 2)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CPopupList, Close, 4)

// 加载XML节点
BOOL CPopupList::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

    // 使用XML节点初始化控件
	if(pXmlElem != NULL)
	{
		// 加载图片和颜色列表
		for (DuiXmlNode pControlElem = pXmlElem.child(_T("item")); pControlElem; pControlElem=pControlElem.next_sibling(_T("item")))
		{
				UINT nResourceID = 0;
				CString strName = pControlElem.attribute(_T("name")).value();
				DuiSystem::Instance()->ParseDuiString(strName);
				CString strDesc = pControlElem.attribute(_T("desc")).value();
				DuiSystem::Instance()->ParseDuiString(strDesc);
				CString strValue = pControlElem.attribute(_T("value")).value();
				DuiSystem::Instance()->ParseDuiString(strValue);
				CString strClrText = pControlElem.attribute(_T("crtext")).value();
				Color clrText = CDuiObject::StringToColor(strClrText, Color(255, 0, 20, 35));
				CString strClrDesc = pControlElem.attribute(_T("crdesc")).value();
				Color clrDesc = CDuiObject::StringToColor(strClrDesc, Color(255, 255, 255, 255));
				CString strImage = pControlElem.attribute(_T("image")).value();
				if(!strImage.IsEmpty())
				{
					if(strImage.Find(_T("skin:")) == 0)
					{
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

				AddItem(strName, strDesc, strValue, nResourceID, strImage, clrText, clrDesc);
			}
	}

    return TRUE;
}

// 设置选中的项
void CPopupList::SetCurItem(UINT nItem)
{
	if (nItem >= m_vecItem.size())  return;

	m_nHoverItem = nItem;
	SetItemPoint();
	InvalidateRect(NULL);
}

// 设置选中的项
void CPopupList::SetCurItem(CString strValue)
{
	int nItemCount =  m_vecItem.size();
	for(int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		if(editListItem.strValue == strValue)
		{
			m_nHoverItem = i;
			SetItemPoint();
			InvalidateRect(NULL);
			return;
		}
	}
}

bool CPopupList::GetItemDesc(UINT nItem, CString &strDesc)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	EditListItem &editListItem = m_vecItem.at(nItem);
	strDesc = editListItem.strDesc;

	return true;
}

bool CPopupList::GetItemName(UINT nItem, CString &strName)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	EditListItem &editListItem = m_vecItem.at(nItem);
	strName = editListItem.strName;

	return true;
}

bool CPopupList::GetItemValue(UINT nItem, CString &strValue)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	EditListItem &editListItem = m_vecItem.at(nItem);
	strValue = editListItem.strValue;

	return true;
}

bool CPopupList::GetItemImage(UINT nItem, UINT &nResourceID, CString &strImageFile)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	EditListItem &editListItem = m_vecItem.at(nItem);

	nResourceID = editListItem.nResourceID;
	strImageFile = editListItem.strImageFile;

	return true;
}

bool CPopupList::DeleteItem(UINT nItem)
{
	ASSERT(nItem < m_vecItem.size());
	if (nItem >= m_vecItem.size())  return false;

	m_vecItem.erase(m_vecItem.begin() + m_nHoverItem);
	if(m_nHoverItem >= (int)m_vecItem.size())
	{
		m_nHoverItem -= 1;
	}
	SetItemPoint();
	
	return true;
}

// 画文字信息
void CPopupList::DrawWindow(CDC &dc, CRect rcClient)
{
	int nItemCount =  m_vecItem.size();
	CFont *pOldFont = dc.SelectObject(&m_font);
	COLORREF clrOld = dc.SetTextColor(RGB(0, 20, 35));
	CRect rcItem, rcText;
	CSize sizeImage;

	for(int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		rcItem = editListItem.rcItem;
		sizeImage = editListItem.sizeImage;
		int nLeftStart = 47;
		if(editListItem.pImage == NULL)
		{
			nLeftStart = 5;
		}

		// 显示当前项
 		if((i == m_nHoverItem) && !editListItem.strDesc.IsEmpty())
 		{
 			dc.FillSolidRect(&rcItem, RGB(0, 147, 209));
 
			// 显示name和desc
			dc.SetTextColor(editListItem.clrText.ToCOLORREF());//RGB(0, 20, 35));
 			rcText.SetRect(rcItem.left + nLeftStart, rcItem.top + 6, rcItem.right - 2, rcItem.top + 24);
 			dc.DrawText(editListItem.strName, &rcText, DT_TOP | DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
 
 			dc.SetTextColor(editListItem.clrDesc.ToCOLORREF());//RGB(255, 255, 255));
 			rcText.OffsetRect(0, 21);
 			dc.DrawText(editListItem.strDesc, &rcText, DT_TOP | DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
 		}
 		else
		{
			if(i == m_nHoverItem)
			{
				dc.FillSolidRect(&rcItem, RGB(0, 147, 209));
			}

			// 只显示name
			dc.SetTextColor(editListItem.clrText.ToCOLORREF());//RGB(0, 20, 35));
			rcText.SetRect(rcItem.left + nLeftStart, rcItem.top, rcItem.right - 2, rcItem.bottom);
			dc.DrawText(editListItem.strName, &rcText, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		}
	}
	dc.SelectObject(pOldFont);
	dc.SetTextColor(clrOld);
}

// 画图片信息
void CPopupList::DrawWindowEx(CDC &dc, CRect rcClient)
{
	int nItemCount =  m_vecItem.size();
	CRect rcItem;
	CSize sizeImage;
	Graphics graphics(dc);

	for(int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		rcItem = editListItem.rcItem;
		sizeImage = editListItem.sizeImage;

		// 删除按钮
		if((i == m_nHoverItem) && (m_pImageClose != NULL))
		{
			graphics.DrawImage(m_pImageClose, RectF((Gdiplus::REAL)m_rcClose.left, (Gdiplus::REAL)m_rcClose.top, (Gdiplus::REAL)m_rcClose.Width(), (Gdiplus::REAL)m_rcClose.Height()),
				(Gdiplus::REAL)(m_buttonState * m_sizeClose.cx), 0, (Gdiplus::REAL)m_sizeClose.cx, (Gdiplus::REAL)m_sizeClose.cy, UnitPixel); 
		}

		// 列表图片
		if(editListItem.pImage)
		{	
			CRect rcHead(rcItem.left + 1, rcItem.top + 2, rcItem.left + 1 + rcItem.Height() - 4, rcItem.top + 2 + rcItem.Height() - 4);
			graphics.DrawImage(editListItem.pImage, RectF((Gdiplus::REAL)rcHead.left, (Gdiplus::REAL)rcHead.top, (Gdiplus::REAL)rcHead.Width(), (Gdiplus::REAL)rcHead.Height()),
				0, 0, (Gdiplus::REAL)editListItem.sizeImage.cx, (Gdiplus::REAL)editListItem.sizeImage.cy, UnitPixel);

			DrawImageFrame(graphics, m_pImageHead, rcHead, i == m_nHoverItem ? m_sizeHead.cx : 0, 0, m_sizeHead.cx, m_sizeHead.cy, 5);
		}
	}
}

void CPopupList::InitUI(CRect rcClient)
{
}

// 添加列表项
int CPopupList::AddItem(CString strName, CString strDesc, CString strValue, int nResourceID,
						CString strImageFile, Color clrText, Color clrDesc)
{
	EditListItem editListItem;
	editListItem.strName = strName;
	editListItem.strDesc = strDesc;
	editListItem.strValue= strValue;
	editListItem.clrText = clrText;
	editListItem.clrDesc = clrDesc;
	editListItem.strImageFile = strImageFile;
	editListItem.nResourceID = nResourceID;
	editListItem.pImage = NULL;
	editListItem.sizeImage.SetSize(0 ,0);
 	if(!strImageFile.IsEmpty())
 	{
		if(DuiSystem::Instance()->LoadImageFile(strImageFile, FALSE, editListItem.pImage))
 		{
 			editListItem.sizeImage.SetSize(editListItem.pImage->GetWidth(), editListItem.pImage->GetHeight());
 		}
 	}
	if(editListItem.pImage == NULL || editListItem.pImage->GetLastStatus() != Ok)
	{
		if(ImageFromIDResource(editListItem.nResourceID, _T("PNG"), editListItem.pImage))
		{
			editListItem.sizeImage.SetSize(editListItem.pImage->GetWidth(), editListItem.pImage->GetHeight());
		}
	}
	m_vecItem.push_back(editListItem);

	if(m_nHoverItem == -1)
	{
		m_nHoverItem = 0;
	}

	SetItemPoint();

	return m_vecItem.size();
}

// 计算列表项大小
void CPopupList::SetItemPoint()
{
	int nItemCount =  m_vecItem.size();

	// 判断是否有Desc字段,如果都没有的话,则显示的列表一律用相同高度的
	BOOL bHaveDesc = FALSE;
	for (int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		if(!editListItem.strDesc.IsEmpty())
		{
			bHaveDesc = TRUE;
			break;
		}
	}

	int nHeight = 4;
	nHeight += 24 * nItemCount;
	if(bHaveDesc)
	{
		if(0 == nItemCount) nHeight += 40;
		if(nItemCount >= 1) nHeight += 20;
		if(nItemCount >= 2) nHeight += 9;
		if(nItemCount >= 2) nHeight += 9;
	}

	int nStratTop = 2;
	for (int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		editListItem.rcItem.left = 2;
		editListItem.rcItem.right = m_nWidth - 2;
		if((i - 1 == m_nHoverItem) && (m_nHoverItem != -1) || (i + 1 == m_nHoverItem))
		{
			editListItem.rcItem.top = nStratTop;
			editListItem.rcItem.bottom = nStratTop + (bHaveDesc ? 33 : 24);
			nStratTop += (bHaveDesc ? 33 : 24);
		}
		else if(i == m_nHoverItem)
		{
			editListItem.rcItem.top = nStratTop;
			editListItem.rcItem.bottom = nStratTop + (bHaveDesc ? 44 : 24);
			nStratTop += (bHaveDesc ? 44 : 24);

			int nLeft = editListItem.rcItem.right - m_sizeClose.cx - 7;
			int nTop = editListItem.rcItem.top + ((bHaveDesc ? 44 : 24) - m_sizeClose.cy) / 2 + 1;

			m_rcClose.SetRect(nLeft, nTop, nLeft + m_sizeClose.cx, nTop + m_sizeClose.cy);
		}
		else
		{
			editListItem.rcItem.top = nStratTop;
			editListItem.rcItem.bottom = nStratTop + 24;
			nStratTop += 24;
		}
	}
	
	if(IsWindow(GetSafeHwnd()))
	{
		SetWindowPos(NULL, 0, 0, m_nWidth, nHeight, SWP_NOMOVE);
	}
}

BOOL CPopupList::OnMouseMove(CPoint point)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	BOOL bDraw = FALSE;
	if(rcClient.PtInRect(point))
	{	
		int nHoverItem = m_nHoverItem;
		
		if(m_nHoverItem != -1)
		{
			EditListItem &editListItem = m_vecItem.at(m_nHoverItem);
			if(editListItem.rcItem.PtInRect(point))
			{
				if((m_pImageClose != NULL) && m_rcClose.PtInRect(point))
				{
					bDraw = m_buttonState != enBSHover;
					m_buttonState = enBSHover;
				}
				else
				{
					bDraw = m_buttonState == enBSHover;
					m_buttonState = enBSNormal;
				}
			}
			else
			{
				nHoverItem = -1;	
				bDraw = TRUE;
			}
		}

		if(m_nHoverItem != nHoverItem)
		{
			int nItemCount =  m_vecItem.size();
			for (int i = 0; i < nItemCount; i++)
			{
				EditListItem &editListItem = m_vecItem.at(i);
				if(editListItem.rcItem.PtInRect(point))
				{
					m_nHoverItem = i;
					bDraw = TRUE;
					break;
				}
			}
			if(m_nHoverItem != -1)
			{
				SetItemPoint();
			}
			if((m_pImageClose != NULL) && m_rcClose.PtInRect(point))
			{
				m_buttonState = enBSHover;
			}
			else
			{
				m_buttonState = enBSNormal;
			}
			bDraw = TRUE;
		}
	}
	else
	{
		m_buttonState = enBSNormal;
		bDraw = TRUE;
	}
	
	return bDraw;
}

BOOL CPopupList::OnLButtonDown(CPoint point)
{
	if(m_buttonState == enBSHover)
	{
		m_buttonState = enBSDown;
		return TRUE;
	}
	
	return FALSE;
}

BOOL CPopupList::OnLButtonUp(CPoint point)
{ 
	BOOL bDraw = FALSE;
	if(m_buttonState == enBSDown)
	{
		if((m_pImageClose != NULL) && m_rcClose.PtInRect(point))
		{
			if(m_nHoverItem != -1)
			{
				//SendMessage(m_uMessageID, DELETE_ITEM, m_nHoverItem);
				SendMessageToParent(m_uMessageID, DELETE_ITEM, m_nHoverItem);
				bDraw = TRUE;
			}
		}	
		else
		{
			bDraw = m_buttonState != enBSNormal;
			m_buttonState = enBSNormal;
		}
	}
	else if(m_nHoverItem != -1)
	{
		EditListItem &editListItem = m_vecItem.at(m_nHoverItem);
		if(editListItem.rcItem.PtInRect(point))
		{
			//SendMessage(m_uMessageID, SELECT_ITEM, m_nHoverItem);
			SendMessageToParent(m_uMessageID, SELECT_ITEM, m_nHoverItem);
		}
	}
	
	return bDraw;
}

// 键盘事件处理
BOOL CPopupList::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nItemCount =  m_vecItem.size();

	if((nChar == VK_UP) && (m_nHoverItem == 0))	// 关闭下拉列表
	{
		OnNcCloseWindow();
		return true;
	}else
	if((nChar == VK_UP) && (m_nHoverItem > 0))	// 向上移动一项
	{
		m_nHoverItem--;
		SetItemPoint();
		InvalidateRect(NULL);
		return true;
	}else
	if((nChar == VK_DOWN) && (m_nHoverItem < (nItemCount - 1)))	// 向下移动一项
	{
		m_nHoverItem++;
		SetItemPoint();
		InvalidateRect(NULL);
		return true;
	}

	return false;
}