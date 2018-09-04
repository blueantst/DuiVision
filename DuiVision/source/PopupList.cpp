#include "StdAfx.h"
#include "PopupList.h"

#define	SCROLL_V	1	// 垂直滚动条控件ID
#define	SCROLL_H	2	// 水平滚动条控件ID


CPopupList::CPopupList(void)
{
	m_nHoverItem = -1;
	m_pImageHead = NULL;
	m_pImageClose = NULL;
	m_rcClose.SetRectEmpty();
	m_buttonState = enBSNormal;
	m_nWidth = 191;
	m_nHeight = 0;
	m_strFont = DuiSystem::GetDefaultFont();
	m_nFontWidth = 12;
	m_fontStyle = FontStyleRegular;
	m_clrHover = Color(225, 0, 147, 209);
	m_bSingleLine = TRUE;
	m_nScrollWidth = 8;

	// 垂直滚动条
	CRect rcScroll = CRect(0,0,0,0);
	rcScroll.left = rcScroll.right - m_nScrollWidth;

 	m_pControScrollV = new CDuiScrollVertical(NULL, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(m_pControScrollV);

	m_nVirtualHeight = 0;
	m_nVirtualTop = 0;

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
						strImage = DuiSystem::Instance()->GetSkin(strImage);
					}

					if(strImage.Find(_T(".")) != -1)	// 加载图片文件
					{
						//strImage = DuiSystem::GetExePath() + strImage;
					}else	// 加载图片资源
					{
						nResourceID = _ttoi(strImage);
						strImage = _T("");
					}
				}

				AddItem(strName, strDesc, strValue, nResourceID, strImage, clrText, clrDesc);
			}
	}

    return TRUE;
}

// 设置列表项字体
void CPopupList::SetFont(CString strFont, int nFontWidth, FontStyle fontStyle)
{
	m_strFont = strFont;
	m_nFontWidth = nFontWidth;
	m_fontStyle = fontStyle;
}

// 设置选择的列表项背景颜色
void CPopupList::SetHoverColor(Color clrHover)
{
	m_clrHover = clrHover;
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

void CPopupList::InitUI(CRect rcClient)
{
	__super::InitUI(rcClient);
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
		if(LoadImageFromIDResource(editListItem.nResourceID, _T("PNG"), m_bImageUseECM, editListItem.pImage))
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
	m_bSingleLine = TRUE;
	for (int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		if(!editListItem.strDesc.IsEmpty())
		{
			bHaveDesc = TRUE;
			m_bSingleLine = FALSE;
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

	m_nVirtualHeight = nHeight;
	BOOL bShowScroll = (m_nHeight != 0) && (m_nVirtualHeight > m_nHeight);
	
	if(IsWindow(GetSafeHwnd()))
	{
		SetWindowPos(NULL, 0, 0, m_nWidth, (m_nHeight != 0) ? m_nHeight : nHeight, SWP_NOMOVE);
	}

	if(bShowScroll)
	{
		// 更新列表项显示的宽度
		for (int i = 0; i < nItemCount; i++)
		{
			EditListItem &editListItem = m_vecItem.at(i);
			editListItem.rcItem.right = m_nWidth - 2 - m_nScrollWidth;
		}

		// 更新滚动条控件的位置
		CRect rcScroll = CRect(0, 1, m_nWidth - 1, m_nHeight - 1);
		rcScroll.left = rcScroll.right - m_nScrollWidth;
		m_pControScrollV->SetRect(rcScroll);
	}

	// 需要的总高度大于显示区高度才会显示垂直滚动条
	m_pControScrollV->SetVisible(bShowScroll);
	((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange(m_nVirtualHeight);
}

// 从XML设置垂直滚动条图片信息属性
HRESULT CPopupList::OnAttributeImageScrollV(const CString& strValue, BOOL bLoading)
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

	return bLoading?S_FALSE:S_OK;
}

BOOL CPopupList::OnMouseMove(CPoint point)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	BOOL bDraw = FALSE;
	if(rcClient.PtInRect(point))
	{	
		int nVirtualTop = GetItemsOffset();
		point.Offset(0, nVirtualTop);
		int nHoverItem = -1;

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
	int nVirtualTop = GetItemsOffset();
	point.Offset(0, nVirtualTop);
	if(m_buttonState == enBSDown)
	{
		if((m_pImageClose != NULL) && m_rcClose.PtInRect(point))
		{
			if(m_nHoverItem != -1)
			{
				SendMessageToParent(MSG_CONTROL_DELETE, m_uMessageID, m_nHoverItem);
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
			SendMessageToParent(MSG_CONTROL_SELECT, m_uMessageID, m_nHoverItem);
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
	}else
	if(nChar == VK_RETURN)	// 回车键选择列表项
	{
		SendMessageToParent(MSG_CONTROL_SELECT, m_uMessageID, m_nHoverItem);
		return true;
	}

	return false;
}

// 画文字信息
void CPopupList::DrawWindow(CDC &dc, CRect rcClient)
{
	int nItemCount =  m_vecItem.size();
	CRect rcItem, rcText;

	Graphics graphics(dc);
	BSTR bsFont = m_strFont.AllocSysString();
	FontFamily fontFamily(bsFont);
	Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
	graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
	::SysFreeString(bsFont);
	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);	// 左对齐
	strFormat.SetLineAlignment(StringAlignmentCenter);	// 中间对齐

	//// 计算显示位置
	//CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	//int nCurPosV = pScrollV->GetScrollCurrentPos();	// 当前top位置
	//int nMaxRangeV = pScrollV->GetScrollMaxRange();
	////int nVirtualTop = (nMaxRangeV > 0) ? nCurPosV*(m_nVirtualHeight-m_rc.Height())/nMaxRangeV : 0;	// 当前显示的是虚拟图片中什么位置开始的图片
	//int nVirtualTop = (m_nVirtualHeight - m_rc.Height()) * nCurPosV / nMaxRangeV;	// 当前显示的是虚拟图片中什么位置开始的图片

	//int nFirstViewRow = m_nVirtualTop / 24;					// 显示的第一行序号
	int nVirtualTop = GetItemsOffset();

	for(int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		rcItem = editListItem.rcItem;
		int nLeftStart = 47;
		if(editListItem.pImage == NULL)	// 没有图片,左边距
		{
			nLeftStart = 5;
		}else
		if(m_bSingleLine)	// 单行,左边距减少一些
		{
			nLeftStart = 25;
		}

		// 显示当前项
 		if((i == m_nHoverItem) && !editListItem.strDesc.IsEmpty())
 		{
			SolidBrush brushHover(m_clrHover);
			graphics.FillRectangle(&brushHover, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height());
 
			// 显示name和desc
			SolidBrush solidBrushTitle(editListItem.clrText);
			SolidBrush solidBrushDesc(editListItem.clrDesc);
			RectF rect((Gdiplus::REAL)(rcItem.left + nLeftStart), (Gdiplus::REAL)(rcItem.top + 1), (Gdiplus::REAL)(rcItem.Width() - nLeftStart -2), (Gdiplus::REAL)(rcItem.Height()/2));
			rect.Offset(0, -(Gdiplus::REAL)nVirtualTop);
			BSTR bsTitle = editListItem.strName.AllocSysString();
			graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrushTitle);
			::SysFreeString(bsTitle);

			rect.Offset(0, (Gdiplus::REAL)(rcItem.Height() / 2));
			BSTR bsDesc = editListItem.strDesc.AllocSysString();
			graphics.DrawString(bsDesc, (INT)wcslen(bsDesc), &font, rect, &strFormat, &solidBrushDesc);
			::SysFreeString(bsTitle);
 		}
 		else
		{
			if(i == m_nHoverItem)
			{
				SolidBrush brushHover(m_clrHover);
				graphics.FillRectangle(&brushHover, rcItem.left, rcItem.top - nVirtualTop, rcItem.Width(), rcItem.Height());
			}

			// 只显示name
			SolidBrush solidBrushTitle(editListItem.clrText);
			RectF rect((Gdiplus::REAL)(rcItem.left + nLeftStart), (Gdiplus::REAL)rcItem.top, (Gdiplus::REAL)(rcItem.Width() - nLeftStart -2), (Gdiplus::REAL)rcItem.Height());
			rect.Offset(0, -(Gdiplus::REAL)nVirtualTop);
			BSTR bsTitle = editListItem.strName.AllocSysString();
			graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrushTitle);
			::SysFreeString(bsTitle);
		}
	}
}

// 画图片信息
void CPopupList::DrawWindowEx(CDC &dc, CRect rcClient)
{
	int nItemCount =  m_vecItem.size();
	CRect rcItem;
	CSize sizeImage;
	Graphics graphics(dc);

	//// 计算显示位置
	//CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	//int nCurPosV = pScrollV->GetScrollCurrentPos();	// 当前top位置
	//int nMaxRangeV = pScrollV->GetScrollMaxRange();
	//int nVirtualTop = (nMaxRangeV > 0) ? nCurPosV*(m_nVirtualHeight-m_rc.Height())/nMaxRangeV : 0;	// 当前显示的是虚拟图片中什么位置开始的图片
	//int nFirstViewRow = m_nVirtualTop / 24;					// 显示的第一行序号

	int nVirtualTop = GetItemsOffset();

	for(int i = 0; i < nItemCount; i++)
	{
		EditListItem &editListItem = m_vecItem.at(i);
		rcItem = editListItem.rcItem;
		sizeImage = editListItem.sizeImage;

		// 删除按钮
		if((i == m_nHoverItem) && (m_pImageClose != NULL))
		{
			RectF rect((Gdiplus::REAL)m_rcClose.left, (Gdiplus::REAL)m_rcClose.top, (Gdiplus::REAL)m_rcClose.Width(), (Gdiplus::REAL)m_rcClose.Height());
			rect.Offset(0, -(Gdiplus::REAL)nVirtualTop);
			graphics.DrawImage(m_pImageClose, rect,
				(Gdiplus::REAL)(m_buttonState * m_sizeClose.cx), 0, (Gdiplus::REAL)m_sizeClose.cx, (Gdiplus::REAL)m_sizeClose.cy, UnitPixel); 
		}

		// 列表图片
		if(editListItem.pImage)
		{	
			CRect rcHead(rcItem.left + 1, rcItem.top + 2, rcItem.left + 1 + rcItem.Height() - 4, rcItem.top + 2 + rcItem.Height() - 4);
			RectF rect((Gdiplus::REAL)rcHead.left, (Gdiplus::REAL)rcHead.top, (Gdiplus::REAL)rcHead.Width(), (Gdiplus::REAL)rcHead.Height());
			rect.Offset(0, -(Gdiplus::REAL)nVirtualTop);
			graphics.DrawImage(editListItem.pImage, rect,
				0, 0, (Gdiplus::REAL)editListItem.sizeImage.cx, (Gdiplus::REAL)editListItem.sizeImage.cy, UnitPixel);

			DrawImageFrame(graphics, m_pImageHead, rcHead, i == m_nHoverItem ? m_sizeHead.cx : 0, 0, m_sizeHead.cx, m_sizeHead.cy, 5);
		}
	}
}

int CPopupList::GetItemsOffset()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	int nCurPosV = pScrollV->GetScrollCurrentPos();	// 当前top位置
	int nMaxRangeV = pScrollV->GetScrollMaxRange();
	//int nVirtualTop = (nMaxRangeV > 0) ? nCurPosV*(m_nVirtualHeight-m_rc.Height())/nMaxRangeV : 0;	// 当前显示的是虚拟图片中什么位置开始的图片
	//m_rc有问题,重新获取
	int nVirtualTop = (m_nVirtualHeight - rcClient.Height()) * nCurPosV / nMaxRangeV;	// 当前显示的是虚拟图片中什么位置开始的图片
	return nVirtualTop;
}

IMPLEMENT_DYNAMIC(CPopupList, CDlgPopup)

BEGIN_MESSAGE_MAP(CPopupList, CDlgPopup)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

BOOL CPopupList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	if (pScrollV && pScrollV->GetVisible())
	{
		int nCurPosV = pScrollV->GetScrollCurrentPos();
		int nMaxRangeV = pScrollV->GetScrollMaxRange();
		int nNewPosV = nCurPosV - zDelta;
		if (nNewPosV > nMaxRangeV)
		{
			nNewPosV = nMaxRangeV;
		}
		if (nNewPosV < 0)
		{
			nNewPosV = 0;
		}
		pScrollV->SetScrollCurrentPos(nNewPosV);
		pScrollV->UpdateControl(TRUE, TRUE);
		return TRUE;
	}
	else
	{
		return CWnd::OnMouseWheel(nFlags, zDelta, pt);
	}
}
