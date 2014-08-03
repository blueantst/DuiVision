#include "StdAfx.h"
#include "DuiListCtrl.h"

#define	SCROLL_V	1	// 滚动条控件ID
#define	LISTBK_AREA	2	// 背景Area控件ID

CDuiGridCtrl::CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiPanel(hWnd, pDuiObject)
{
	CRect rcBk = CRect(0,0,0,0);
	CControlBase* pControlBase = new CArea(hWnd, this, LISTBK_AREA, rcBk, 100, 100);
 	m_vecControl.push_back(pControlBase);
	m_pControBkArea = (CControlBase*)pControlBase;

	m_strFontTitle = DuiSystem::GetDefaultFont();
	m_nFontTitleWidth = 12;
	m_fontTitleStyle = FontStyleRegular;

	m_clrText = Color(225, 64, 64, 64);
	m_clrTextHover = Color(128, 0, 0);
	m_clrTextDown = Color(0, 112, 235);
	m_clrTitle = Color(255, 32, 32, 32);
	m_clrSeperator = Color(200, 160, 160, 160);
	m_nRowHeight = 50;
	m_nHeaderHeight = 0;
	m_nLeftPos = 0;

	m_pImageSeperator = NULL;
	m_sizeSeperator = CSize(0, 0);
	m_pImageCheckBox = NULL;
	m_sizeCheckBox = CSize(0, 0);

	m_nBkTransparent = 30;

	m_nHoverRow = 0;
	m_nDownRow = -1;
	m_bEnableDownRow = FALSE;
	m_bSingleLine = TRUE;
	m_bTextWrap = FALSE;

	m_bGridTooltip = TRUE;
	m_nTipRow = -1;
	m_nTipItem = -1;
	m_nTipVirtualTop = 0;

	m_nFirstViewRow = 0;
	m_nLastViewRow = 0;
	m_nVirtualTop = 0;
}

CDuiGridCtrl::~CDuiGridCtrl(void)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}
	if(m_pImageCheckBox != NULL)
	{
		delete m_pImageCheckBox;
		m_pImageCheckBox = NULL;
	}
}

// 图片属性的实现
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, CheckBox, 6)

// 加载XML节点，解析节点中的属性信息设置到当前控件的属性中
BOOL CDuiGridCtrl::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

    // 使用XML节点初始化div控件
	if(pXmlElem != NULL)
	{
		InitUI(m_rc, pXmlElem);
	}

	// 需要的总高度大于显示区高度才会显示滚动条
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_vecRowInfo.size() * m_nRowHeight);

	// 加载下层的cloumn节点信息
	TiXmlElement* pColumnElem = NULL;
	for (pColumnElem = pXmlElem->FirstChildElement("column"); pColumnElem != NULL; pColumnElem=pColumnElem->NextSiblingElement())
	{
		CStringA strTitleA = pColumnElem->Attribute("title");
		CStringA strClrTextA = pColumnElem->Attribute("crtext");
		CStringA strWidthA = pColumnElem->Attribute("width");
		DuiSystem::Instance()->ParseDuiString(strTitleA);
		CString strTitle = CA2T(strTitleA, CP_UTF8);
		Color clrText = Color(0,0,0,0);
		if(!strClrTextA.IsEmpty())
		{
			if(strClrTextA.Find(",") == -1)
			{
				clrText = CDuiObject::HexStringToColor(strClrTextA);
			}else
			{
				clrText = CDuiObject::StringToColor(strClrTextA);
			}
		}
		int nWidth = -1;
		if(!strWidthA.IsEmpty())
		{
			nWidth = atoi(strWidthA);
		}
		InsertColumn(-1, strTitle, nWidth, clrText);
	}

	// 加载下层的row节点信息
	TiXmlElement* pRowElem = NULL;
	for (pRowElem = pXmlElem->FirstChildElement("row"); pRowElem != NULL; pRowElem=pRowElem->NextSiblingElement())
	{
		CStringA strIdA = pRowElem->Attribute("id");
		CStringA strCheckA = pRowElem->Attribute("check");
		CStringA strImageA = pRowElem->Attribute("image");
		CStringA strRightImageA = pRowElem->Attribute("right-img");
		CStringA strClrTextA = pRowElem->Attribute("crtext");

		int nCheck = -1;
		if(!strCheckA.IsEmpty())
		{
			nCheck = atoi(strCheckA);
		}

		// 左边图片,通过Skin读取
		CStringA strSkin = "";
		if(strImageA.Find("skin:") == 0)
		{
			strSkin = DuiSystem::Instance()->GetSkin(strImageA);
		}else
		{
			strSkin = strImageA;
		}

		int nImageIndex = -1;
		CString strImage = _T("");
		if(strSkin.Find(".") != -1)
		{
			// 图片文件
			strImage = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		}else
		if(!strSkin.IsEmpty())
		{
			// 图片索引
			nImageIndex = atoi(strSkin);
		}

		// 右边图片,通过Skin读取
		CStringA strRightSkin = "";
		if(strRightImageA.Find("skin:") == 0)
		{
			strRightSkin = DuiSystem::Instance()->GetSkin(strRightImageA);
		}else
		{
			strRightSkin = strRightImageA;
		}

		int nRightImageIndex = -1;
		CString strRightImage = _T("");
		if(strRightSkin.Find(".") != -1)
		{
			// 图片文件
			strRightImage = DuiSystem::GetSkinPath() + CA2T(strRightSkin, CP_UTF8);
		}else
		if(!strRightSkin.IsEmpty())
		{
			// 图片索引
			nRightImageIndex = atoi(strRightSkin);
		}

		CString strId = CA2T(strIdA, CP_UTF8);
		Color clrText = Color(0,0,0,0);
		if(!strClrTextA.IsEmpty())
		{
			if(strClrTextA.Find(",") == -1)
			{
				clrText = CDuiObject::HexStringToColor(strClrTextA);
			}else
			{
				clrText = CDuiObject::StringToColor(strClrTextA);
			}
		}
		InsertRow(-1, strId, nImageIndex, clrText, strImage, nRightImageIndex, strRightImage, nCheck);

		int nRowIndex = m_vecRowInfo.size()-1;
		int nItemIndex = 0;
		// 加载下层的item节点信息
		TiXmlElement* pItemElem = NULL;
		for (pItemElem = pRowElem->FirstChildElement("item"); pItemElem != NULL; pItemElem=pItemElem->NextSiblingElement())
		{
			CStringA strTitleA = pItemElem->Attribute("title");
			CStringA strContentA = pItemElem->Attribute("content");
			CStringA strClrTextA = pItemElem->Attribute("crtext");
			CStringA strImageA = pItemElem->Attribute("image");
			CStringA strLinkA = pItemElem->Attribute("link");
			CStringA strLinkActionA = pItemElem->Attribute("linkaction");
			CStringA strFontTitleA = pItemElem->Attribute("font-title");
			DuiSystem::Instance()->ParseDuiString(strTitleA);
			DuiSystem::Instance()->ParseDuiString(strContentA);
			DuiSystem::Instance()->ParseDuiString(strLinkA);
			DuiSystem::Instance()->ParseDuiString(strLinkActionA);
			CString strTitle = CA2T(strTitleA, CP_UTF8);
			CString strContent = CA2T(strContentA, CP_UTF8);
			CString strLink = CA2T(strLinkA, CP_UTF8);
			CString strLinkAction = CA2T(strLinkActionA, CP_UTF8);
			Color clrText = Color(0,0,0,0);
			if(!strClrTextA.IsEmpty())
			{
				if(strClrTextA.Find(",") == -1)
				{
					clrText = CDuiObject::HexStringToColor(strClrTextA);
				}else
				{
					clrText = CDuiObject::StringToColor(strClrTextA);
				}
			}

			// 图片,通过Skin读取
			CStringA strSkin = "";
			if(strImageA.Find("skin:") == 0)
			{
				strSkin = DuiSystem::Instance()->GetSkin(strImageA);
			}else
			{
				strSkin = strImageA;
			}

			int nImageIndex = -1;
			CString strImage = _T("");
			if(strSkin.Find(".") != -1)
			{
				// 图片文件
				strImage = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
			}else
			if(!strSkin.IsEmpty())
			{
				// 图片索引
				nImageIndex = atoi(strSkin);
			}

			BOOL bUseTitleFont = (strFontTitleA == "1");

			if(!strLink.IsEmpty())
			{
				SetSubItemLink(nRowIndex, nItemIndex, strLink, strLinkAction, nImageIndex, clrText, strImage);
			}else
			{
				SetSubItem(nRowIndex, nItemIndex, strTitle, strContent, bUseTitleFont, nImageIndex, clrText, strImage);
			}
			nItemIndex++;
		}
	}

    return TRUE;
}

// 添加列
BOOL CDuiGridCtrl::InsertColumn(int nColumn, CString strTitle, int nWidth, Color clrText)
{
	GridColumnInfo columnInfo;
	columnInfo.strTitle = strTitle;
	columnInfo.clrText = clrText;
	columnInfo.nWidth = nWidth;
	if(nColumn <= -1 || nColumn >= m_vecColumnInfo.size())
	{
		m_vecColumnInfo.push_back(columnInfo);
	}
	else
	{
		m_vecColumnInfo.insert(m_vecColumnInfo.begin() + nColumn, columnInfo);
	}

	int nXPos = 0;
	int nYPos = 0;

	for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
	{
		GridColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
		int nWidth = columnInfoTemp.nWidth;
		if(nWidth == -1)
		{
			nWidth = m_rc.Width() - nXPos;
		}
		columnInfoTemp.rcHeader.SetRect(nXPos, nYPos, nXPos + nWidth, nYPos + m_nRowHeight);
		nXPos += columnInfoTemp.nWidth;
	}

	UpdateControl(true);
	return true;
}

// 添加行
BOOL CDuiGridCtrl::InsertRow(int nItem, CString strId, int nImageIndex, Color clrText, CString strImage,
							 int nRightImageIndex, CString strRightImage, int nCheck)
{
	GridRowInfo rowInfo;
	rowInfo.strId = strId;
	rowInfo.nCheck = nCheck;
	rowInfo.nImageIndex = nImageIndex;
	rowInfo.sizeImage.SetSize(0, 0);
	rowInfo.nRightImageIndex = nRightImageIndex;
	rowInfo.sizeRightImage.SetSize(0, 0);
	rowInfo.bRowColor = FALSE;
	rowInfo.clrText = clrText;
	rowInfo.nHoverItem = -1;
	if(clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		rowInfo.bRowColor = TRUE;
	}

	// 左边图片
	if(!strImage.IsEmpty() && (strImage.Find(_T(":")) == -1))
	{
		strImage = DuiSystem::GetSkinPath() + strImage;
	}
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		rowInfo.pImage = Image::FromFile(strImage, m_bImageUseECM);
		if(rowInfo.pImage->GetLastStatus() == Ok)
		{
			rowInfo.sizeImage.SetSize(rowInfo.pImage->GetWidth() / 1, rowInfo.pImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		rowInfo.pImage = NULL;
		if((rowInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	// 右边图片
	if(!strRightImage.IsEmpty() && (strRightImage.Find(_T(":")) == -1))
	{
		strRightImage = DuiSystem::GetSkinPath() + strRightImage;
	}
	if(!strRightImage.IsEmpty())
	{
		// 使用行数据指定的图片
		rowInfo.pRightImage = Image::FromFile(strRightImage, m_bImageUseECM);
		if(rowInfo.pRightImage->GetLastStatus() == Ok)
		{
			rowInfo.sizeRightImage.SetSize(rowInfo.pRightImage->GetWidth() / 1, rowInfo.pRightImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		rowInfo.pRightImage = NULL;
		if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeRightImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return InsertRow(nItem, rowInfo);
}

// 添加行
BOOL CDuiGridCtrl::InsertRow(int nItem, GridRowInfo &rowInfo)
{
	if(nItem <= -1 || nItem >= m_vecRowInfo.size())
	{
		m_vecRowInfo.push_back(rowInfo);
	}
	else
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nItem, rowInfo);
	}

	// 计算所有表格行的位置
	CalcRowsPos();	

	UpdateControl(true);
	return true;
}

// 设置表格项内容(文字表格项)
BOOL CDuiGridCtrl::SetSubItem(int nRow, int nItem, CString strTitle, CString strContent, BOOL bUseTitleFont, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		GridItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = bUseTitleFont;
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strTitle = strTitle;
	itemInfo.strContent = strContent;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	GridColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// 图片
	if(!strImage.IsEmpty() && (strImage.Find(_T(":")) == -1))
	{
		strImage = DuiSystem::GetSkinPath() + strImage;
	}
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		itemInfo.pImage = Image::FromFile(strImage, m_bImageUseECM);
		if(itemInfo.pImage->GetLastStatus() == Ok)
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// 设置表格项内容(链接表格项)
BOOL CDuiGridCtrl::SetSubItemLink(int nRow, int nItem, CString strLink, CString strLinkAction, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		GridItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = FALSE;
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strLink = strLink;
	itemInfo.strLinkAction = strLinkAction;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	GridColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// 图片
	if(!strImage.IsEmpty() && (strImage.Find(_T(":")) == -1))
	{
		strImage = DuiSystem::GetSkinPath() + strImage;
	}
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		itemInfo.pImage = Image::FromFile(strImage, m_bImageUseECM);
		if(itemInfo.pImage->GetLastStatus() == Ok)
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// 删除行
BOOL CDuiGridCtrl::DeleteRow(int nItem)
{
	if((nItem < 0) || (nItem >= m_vecRowInfo.size()))
	{
		return FALSE;
	}

	int nIndex = 0;
	vector<GridRowInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nItem)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}

	// 计算所有表格行的位置
	CalcRowsPos();

	UpdateControl(true);
	return true;
}

// 计算表格行位置
void CDuiGridCtrl::CalcRowsPos()
{
	int nXPos = 0;//m_rc.left;
	int nYPos = 0;//m_rc.top;

	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		rowInfoTemp.rcCheck.SetRect(0,0,0,0);

		nYPos += m_nRowHeight;
	}

	// 需要的总高度大于显示区高度才会显示滚动条
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_vecRowInfo.size() * m_nRowHeight);
}

// 获取某一个行信息
GridRowInfo* CDuiGridCtrl::GetRowInfo(int nRow)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// 获取某一个单元格信息
GridItemInfo* CDuiGridCtrl::GetItemInfo(int nRow, int nItem)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return NULL;
	}

	if((nItem < 0) || (nItem >= m_vecColumnInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	if(nItem >= rowInfo.vecItemInfo.size())
	{
		return NULL;
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);

	return &itemInfo;
}

// 设置某一个行的颜色
void CDuiGridCtrl::SetRowColor(int nRow, Color clrText)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// 设置某一个行的检查框状态
void CDuiGridCtrl::SetRowCheck(int nRow, int nCheck)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.nCheck = nCheck;
}

// 获取某一行的检查框状态
int CDuiGridCtrl::GetRowCheck(int nRow)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return -1;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return rowInfo.nCheck;
}

// 清空列表
void CDuiGridCtrl::ClearItems()
{
	m_vecRowInfo.clear();
	m_pControScrollV->SetVisible(FALSE);
	UpdateControl(true);
}

// 从XML设置Font-title属性
HRESULT CDuiGridCtrl::OnAttributeFontTitle(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	DuiFontInfo fontInfo;
	BOOL bFindFont = DuiSystem::Instance()->GetFont(strValue, fontInfo);
	if (!bFindFont) return E_FAIL;

	m_strFontTitle = fontInfo.strFont;
	m_nFontTitleWidth = fontInfo.nFontWidth;	
	m_fontTitleStyle = fontInfo.fontStyle;

	return bLoading?S_FALSE:S_OK;
}

void CDuiGridCtrl::SetControlRect(CRect rc)
{
	m_rc = rc;
	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			UINT uControlID = pControlBase->GetControlID();
			if(SCROLL_V == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.top += m_nHeaderHeight;
				rcTemp.left = rcTemp.right - m_nScrollWidth;
			}else
			if(LISTBK_AREA == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.top += m_nHeaderHeight;
				rcTemp.right -= m_nScrollWidth;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// 重新计算所有行的位置
	int nXPos = 0;
	int nYPos = 0;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		nYPos += m_nRowHeight;
	}

	// 需要的总高度大于显示区高度才会显示滚动条
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CScrollV*)m_pControScrollV)->SetScrollMaxRange(m_vecRowInfo.size() * m_nRowHeight);
}

// 判断指定的坐标位置是否在某一行中
BOOL CDuiGridCtrl::PtInRow(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcRow;
	// rcRow坐标是插入行节点时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
	rc.OffsetRect(m_rc.left, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// 判断指定的坐标位置是否在某一行的检查框中
BOOL CDuiGridCtrl::PtInRowCheck(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcCheck;
	// rcCheck坐标是画图时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
	rc.OffsetRect(m_rc.left, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// 判断指定的坐标位置是否在某一行的某个列中,返回列索引
int CDuiGridCtrl::PtInRowItem(CPoint point, GridRowInfo& rowInfo)
{
	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		CRect rc = itemInfo.rcItem;
		// rcItem坐标是画图时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
		rc.OffsetRect(m_rc.left + m_nLeftPos, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

// 设置单元格的Tooltip
void CDuiGridCtrl::SetGridTooltip(int nRow, int nItem, CString strTooltip)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return;
	}

	CDlgBase* pDlg = GetParentDialog();
	if(pDlg && ((m_nTipRow != nRow) || (m_nTipItem != nItem) || (m_nTipVirtualTop != m_nVirtualTop)))
	{
		GridItemInfo* pGridInfo = GetItemInfo(nRow, nItem);
		if(pGridInfo && (pGridInfo->bNeedTitleTip || pGridInfo->bNeedContentTip))
		{
			CRect rc = pGridInfo->rcItem;
			rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop+m_nHeaderHeight);
			pDlg->SetTooltip(this, strTooltip, rc, TRUE);
		}else
		{
			pDlg->ClearTooltip();
		}
		m_nTipRow = nRow;
		m_nTipItem = nItem;
		m_nTipVirtualTop = m_nVirtualTop;
	}
}

// 清除Tooltip
void CDuiGridCtrl::ClearGridTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipRow = -1;
		m_nTipItem = -1;
		m_nTipVirtualTop = 0;
	}
}

BOOL CDuiGridCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	int nOldHoverRow = m_nHoverRow;
	BOOL bHoverItemChange = FALSE;
	int nOldHoverItem = -1;

	if(m_rc.PtInRect(point))
	{
		if((m_nHoverRow >= 0) && (m_nHoverRow < m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
					UpdateControl(TRUE);
				}

				if(m_bGridTooltip)	// 设置单元格Tooltip
				{
					GridItemInfo* pGridInfo = GetItemInfo(m_nHoverRow, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(m_nHoverRow, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(m_nHoverRow, rowInfo.nHoverItem, pGridInfo->strContent);
					}else
					{
						ClearGridTooltip();
					}
				}

				return false;
			}
			m_nHoverRow = -1;		
		}

		BOOL bMousenDown = false;
		if((m_nDownRow >= 0) && (m_nDownRow < m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				bMousenDown = true;
				m_nHoverRow = -1;
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
				}

				if(m_bGridTooltip)	// 设置单元格Tooltip
				{
					GridItemInfo* pGridInfo = GetItemInfo(m_nDownRow, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(m_nDownRow, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(m_nDownRow, rowInfo.nHoverItem, pGridInfo->strContent);
					}else
					{
						ClearGridTooltip();
					}
				}
			}		
		}

		if(!bMousenDown)
		{
			for(size_t i = 0; i < m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				nOldHoverItem = rowInfo.nHoverItem;
				if(PtInRow(point, rowInfo))
				{
					rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
					m_nHoverRow = i;
					if(nOldHoverItem != rowInfo.nHoverItem)
					{
						bHoverItemChange = TRUE;
					}
					break;
				}
			}
		}
	}
	else
	{
		m_nHoverRow = -1;
	}

	if((nOldHoverRow != m_nHoverRow) || bHoverItemChange)
	{
		UpdateControl(TRUE);
		return true;
	}
	
	return false;
}

BOOL CDuiGridCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// 设置窗口焦点,否则可能无法进行滚动事件的处理
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// 检查框事件只在鼠标放开时候触发
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(BUTTOM_DOWN, m_bEnableDownRow ? m_nDownRow : m_nHoverRow, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < m_vecRowInfo.size()))
	{
		// 如果点击的还是之前点击的行，也同样会发送鼠标点击事件
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// 检查框事件只在鼠标放开时候触发
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(BUTTOM_DOWN, m_nDownRow, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

BOOL CDuiGridCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// 检查框状态改变
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(BUTTOM_UP, m_nHoverRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < m_vecRowInfo.size()))
	{
		// 如果点击的还是之前点击的行，也同样会发送鼠标点击事件
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// 检查框状态改变
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(BUTTOM_UP, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}
	}

	return false;
}

// 滚动事件处理
BOOL CDuiGridCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if((m_vecRowInfo.size() * m_nRowHeight) <= m_rc.Height())
	{
		return false;
	}

	// 更新滚动条,并刷新界面
	CScrollV* pScroll = (CScrollV*)m_pControScrollV;
	if(pScroll->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
	}

	return true;
}

// 消息响应
LRESULT CDuiGridCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// 如果是滚动条的位置变更事件,则刷新显示
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == BUTTOM_DOWN) && (lParam != -1))
	{
		// 点击了行的某个链接
		GridRowInfo* pRowInfo = GetRowInfo(wParam);
		if(pRowInfo && (lParam >= 0) && (lParam < pRowInfo->vecItemInfo.size()))
		{
			GridItemInfo &itemInfo = pRowInfo->vecItemInfo.at(lParam);
			// 转换为BUTTOM_UP消息,因为DuiSystem任务处理时候只处理BUTTOM_UP消息
			if(!itemInfo.strLinkAction.IsEmpty())
			{
				DuiSystem::AddDuiActionTask(uID, BUTTOM_UP, wParam, lParam, GetName(), itemInfo.strLinkAction, GetParent());
			}
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiGridCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	// 列表画图方法:
	// 1.列表的虚拟高度为每一行高度*行数
	// 2.列表显示的top坐标由scroll控件记录
	// 3.重画时候,根据top坐标位置计算出显示的第一行的序号,根据显示高度计算出显示的最后一行的序号
	// 4.根据计算出的显示的行,画相应的内容到内存dc中
	// 5.计算出显示的top坐标进行内存dc的拷贝
	int nWidth = m_rc.Width() - m_nScrollWidth;	// 减去滚动条的宽度
	int nHeightAll = m_vecRowInfo.size()*m_nRowHeight; // 总的虚拟高度 //m_rc.Height();
	CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// 当前top位置
	int nMaxRange = pScrollV->GetScrollMaxRange();

	m_nVirtualTop = (nMaxRange > 0) ? nCurPos*(nHeightAll-m_rc.Height())/nMaxRange : 0;	// 当前滚动条位置对应的虚拟的top位置
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;					// 显示的第一行序号
	m_nLastViewRow = (m_nVirtualTop + m_rc.Height() - m_nHeaderHeight) / m_nRowHeight;	// 显示的最后一行序号
	if(m_nLastViewRow >= m_vecRowInfo.size())
	{
		m_nLastViewRow = m_vecRowInfo.size() - 1;
	}
	if(m_nLastViewRow < 0)
	{
		m_nLastViewRow = 0;
	}
	int nHeightView = (m_nLastViewRow - m_nFirstViewRow +1) * m_nRowHeight;	// 显示涉及到的虚拟高度
	int nYViewPos = m_nVirtualTop - (m_nFirstViewRow * m_nRowHeight);		// 内存dc显示到屏幕时候的top位置
	if(nYViewPos < 0)
	{
		nYViewPos = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeightView);

		Graphics graphics(m_memDC);
		
		m_memDC.BitBlt(0, 0, nWidth, nHeightView, &dc, m_rc.left, m_rc.top, WHITENESS);	// 画白色背景
		DrawVerticalTransition(m_memDC, dc, CRect(0, nYViewPos, nWidth, m_rc.Height()+nYViewPos-m_nHeaderHeight),	// 背景透明度
				m_rc, m_nBkTransparent, m_nBkTransparent);
		
		FontFamily fontFamilyTitle(m_strFontTitle.AllocSysString());
		Font fontTitle(&fontFamilyTitle, (REAL)m_nFontTitleWidth, m_fontTitleStyle, UnitPixel);

		FontFamily fontFamily(m_strFont.AllocSysString());
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);

		SolidBrush solidBrush(m_clrText);			// 正常文字画刷
		SolidBrush solidBrushH(m_clrTextHover);		// 热点文字画刷
		SolidBrush solidBrushD(m_clrTextDown);		// 当前行画刷
		SolidBrush solidBrushT(m_clrTitle);			// 标题文字画刷
		SolidBrush solidBrushS(m_clrSeperator);		// 分割线画刷

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// 普通文字的对齐方式
		StringFormat strFormat;
		strFormat.SetAlignment(StringAlignmentNear);	// 左对齐
		if(m_uVAlignment == VAlign_Top)
		{
			strFormat.SetLineAlignment(StringAlignmentNear);	// 上对其
		}else
		if(m_uVAlignment == VAlign_Middle)
		{
			strFormat.SetLineAlignment(StringAlignmentCenter);	// 中间对齐
		}else
		if(m_uVAlignment == VAlign_Bottom)
		{
			strFormat.SetLineAlignment(StringAlignmentFar);	// 下对齐
		}
		strFormat.SetTrimming(StringTrimmingEllipsisWord);	// 以单词为单位去尾,略去部分使用省略号
		//strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		if(!m_bTextWrap)
		{
			strFormat.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// 不换行
		}

		// 标题字段采用中间对齐
		StringFormat strFormatHeader;
		strFormatHeader.SetAlignment(StringAlignmentCenter);	// 中间对齐
		strFormatHeader.SetLineAlignment(StringAlignmentCenter);	// 中间对齐
		if(!m_bTextWrap)
		{
			strFormatHeader.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// 不换行
		}

		// 画标题行
		if((m_nHeaderHeight > 0) && (m_vecColumnInfo.size() > 0))
		{
			// 画单元格内容
			int nPosItemX = 0;
			for(size_t j = 0; j < m_vecColumnInfo.size(); j++)
			{
				GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
				int nWidth = columnInfo.nWidth;
				if(j == 0)
				{
					nWidth += m_nLeftPos;
				}
				RectF rect(nPosItemX, 0, nWidth, m_nHeaderHeight-1);

				// 画列标题
				CString strTitle = columnInfo.strTitle;
				graphics.DrawString(strTitle.AllocSysString(), (INT)wcslen(strTitle.AllocSysString()),
					&font, rect, &strFormatHeader, &solidBrushT);

				nPosItemX += nWidth;
			}
		}
		
		if(m_vecRowInfo.size() > 0)
		{
			for(size_t i = m_nFirstViewRow; i <= m_nLastViewRow && i < m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				SolidBrush solidBrushRow(rowInfo.clrText);	// 行定义的颜色

				int nXPos = 0;
				int nVI = i - m_nFirstViewRow;

				// 画检查框
				int nCheckImgY = 3;
				if((m_sizeCheckBox.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
				{
					nCheckImgY = (m_nRowHeight - m_sizeCheckBox.cy) / 2 + 1;
				}
				if((rowInfo.nCheck != -1) && (m_pImageCheckBox != NULL))
				{
					int nCheckImageIndex = ((m_nHoverRow == i) ? ((rowInfo.nCheck==1) ? 4 : 1) : ((rowInfo.nCheck==1) ? 2 : 0));
					graphics.DrawImage(m_pImageCheckBox, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nCheckImgY, m_sizeCheckBox.cx, m_sizeCheckBox.cy),
						nCheckImageIndex * m_sizeCheckBox.cx, 0, m_sizeCheckBox.cx, m_sizeCheckBox.cy, UnitPixel);
					rowInfo.rcCheck.SetRect(nXPos, i*m_nRowHeight + nCheckImgY, nXPos + m_sizeCheckBox.cx, i*m_nRowHeight + nCheckImgY + m_sizeCheckBox.cy);
					nXPos += (m_sizeCheckBox.cx + 3);
				}

				// 画行左边图片
				int nImgY = 3;
				if(rowInfo.pImage != NULL)
				{
					if((rowInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
					}
					// 使用行数据指定的图片
					graphics.DrawImage(rowInfo.pImage, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy),
						0, 0, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy, UnitPixel);
					nXPos += (rowInfo.sizeImage.cx + 3);
				}else
				if((rowInfo.nImageIndex != -1) && (m_pImage != NULL))
				{
					if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
					}
					// 使用索引图片
					graphics.DrawImage(m_pImage, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nXPos += (m_sizeImage.cx + 3);
				}

				// 画行右边图片
				int nRightImageWidth = 0;
				nImgY = 3;
				if(rowInfo.pRightImage != NULL)
				{
					if((rowInfo.sizeRightImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeRightImage.cy) / 2 + 1;
					}
					// 使用行数据指定的图片
					graphics.DrawImage(rowInfo.pRightImage, Rect(nWidth-rowInfo.sizeRightImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy),
						0, 0, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy, UnitPixel);
					nRightImageWidth = rowInfo.sizeRightImage.cx + 1;
				}else
				if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL))
				{
					if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
					}
					// 使用索引图片
					graphics.DrawImage(m_pImage, Rect(nWidth-m_sizeImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nRightImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nRightImageWidth = m_sizeImage.cx + 1;
				}

				// 画单元格内容
				int nPosItemX = (m_nLeftPos != 0) ? m_nLeftPos : nXPos;
				for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
				{
					GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
					BOOL bSingleLine = (itemInfo.strContent.IsEmpty() || !itemInfo.strLink.IsEmpty());
					RectF rect(nPosItemX, m_nHeaderHeight + nVI*m_nRowHeight + 1, itemInfo.rcItem.Width(), bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2));

					// 画单元格图片
					int nItemImageX = 0;
					int nImgY = 3;
					if(itemInfo.pImage != NULL)
					{
						if((itemInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
						}
						// 使用单元格指定的图片
						graphics.DrawImage(itemInfo.pImage, Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
							0, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
						nItemImageX += (itemInfo.sizeImage.cx + 3);
					}else
					if((itemInfo.nImageIndex != -1) && (m_pImage != NULL))
					{
						if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
						}
						// 使用索引图片
						graphics.DrawImage(m_pImage, Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
							itemInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
						nItemImageX += (m_sizeImage.cx + 3);
					}
					rect.Offset(nItemImageX, 0);
					rect.Width -= nItemImageX;

					// 画单元格标题或链接内容
					SolidBrush solidBrushItem(m_clrText);
					if(m_nHoverRow == i)
					{
						solidBrushItem.SetColor(m_clrTextHover);
					}else
					if(m_nDownRow == i)
					{
						solidBrushItem.SetColor(m_clrTextDown);
					}else
					if(itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
					{
						solidBrushItem.SetColor(itemInfo.clrText);
					}
					CString strItemTitle = itemInfo.strTitle;
					// 计算是否需要显示tip
					itemInfo.bNeedTitleTip = rect.Width < GetTextBounds(font, strItemTitle).Width;
					itemInfo.bNeedContentTip = rect.Width < GetTextBounds(font, itemInfo.strContent).Width;
					if(!itemInfo.strLink.IsEmpty())
					{
						strItemTitle = itemInfo.strLink;
						if((m_nHoverRow == i) && (rowInfo.nHoverItem == j))
						{
							solidBrushItem.SetColor(m_clrTextHover);
						}else
						{
							solidBrushItem.SetColor((itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue()) ? itemInfo.clrText : m_clrText);
						}
					}
					// 根据bUseTitleFont决定用标题字体还是普通字体
					graphics.DrawString(strItemTitle.AllocSysString(), (INT)wcslen(strItemTitle.AllocSysString()),
						itemInfo.bUseTitleFont ? &fontTitle : &font, rect, &strFormat, itemInfo.bUseTitleFont ? &solidBrushT : &solidBrushItem);

					// 画单元格内容
					if(!bSingleLine)
					{
						rect.Offset(0, m_nRowHeight / 2 + 2);
						rect.Height = m_nRowHeight / 2 - 4;
						graphics.DrawString(itemInfo.strContent.AllocSysString(), (INT)wcslen(itemInfo.strContent.AllocSysString()),
							&font, rect, &strFormat, &solidBrushItem);
					}

					nPosItemX += itemInfo.rcItem.Width();
				}

				// 画分隔线(采用拉伸模式)
				if(m_pImageSeperator != NULL)
				{
					// 使用拉伸模式属性画图
					graphics.DrawImage(m_pImageSeperator, RectF(0, m_nHeaderHeight + (nVI+1)*m_nRowHeight, nWidth-2, m_sizeSeperator.cy),
							0, 0, m_sizeSeperator.cx, m_sizeSeperator.cy, UnitPixel);
				}else
				{
					// 未指定图片,则画矩形
					graphics.FillRectangle(&solidBrushS, 0, m_nHeaderHeight + (nVI+1)*m_nRowHeight, nWidth-2, 1);
				}
			}
		}
	}

	// 输出到界面DC,使用与的方式合并背景
	if(m_nHeaderHeight > 0)
	{
		dc.BitBlt(m_rc.left,m_rc.top, nWidth, m_nHeaderHeight, &m_memDC, 0, 0, SRCCOPY);//SRCAND);
	}
	dc.BitBlt(m_rc.left,m_rc.top + m_nHeaderHeight, nWidth, m_rc.Height() - m_nHeaderHeight, &m_memDC, 0, nYViewPos + m_nHeaderHeight, SRCCOPY);//SRCAND);
}