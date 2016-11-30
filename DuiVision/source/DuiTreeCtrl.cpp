#include "StdAfx.h"
#include "DuiTreeCtrl.h"

#define	SCROLL_V	1	// 垂直滚动条控件ID
#define	SCROLL_H	2	// 水平滚动条控件ID

CDuiTreeCtrl::CDuiTreeCtrl(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiPanel(hWnd, pDuiObject)
{
	m_strFontTitle = DuiSystem::GetDefaultFont();
	m_nFontTitleWidth = 12;
	m_fontTitleStyle = FontStyleRegular;

	m_clrText = Color(225, 64, 64, 64);
	m_clrTextHover = Color(128, 0, 0);
	m_clrTextDown = Color(0, 112, 235);
	m_clrTitle = Color(255, 32, 32, 32);
	m_clrSeperator = Color(0, 0, 0, 0);
	m_clrRowHover = Color(0, 128, 128, 128);	// 鼠标移动到行显示的背景色,默认是透明色
	m_clrRowCurrent = Color(0, 128, 128, 128);	// 当前行显示的背景色,默认是透明色
	m_nRowHeight = 50;
	m_nLeftPos = 0;

	m_pImageSeperator = NULL;
	m_sizeSeperator = CSize(0, 0);
	m_pImageCheckBox = NULL;
	m_sizeCheckBox = CSize(0, 0);
	m_pImageCollapse = NULL;
	m_sizeCollapse = CSize(0, 0);
	m_pImageToggle = NULL;
	m_sizeToggle = CSize(0, 0);

	m_nBkTransparent = 30;

	m_bDblClk = true;

	m_nHoverRow = 0;
	m_nDownRow = -1;
	m_bEnableDownRow = FALSE;
	m_bSingleLine = TRUE;
	m_bTextWrap = FALSE;

	m_bGridTooltip = TRUE;
	m_nTipNode = NULL;
	m_nTipItem = -1;
	m_nTipVirtualTop = 0;

	m_nFirstViewRow = 0;
	m_nVirtualTop = 0;
	m_nVisibleRowCount = 0;

	m_nNodeIndex = 1;
}

CDuiTreeCtrl::~CDuiTreeCtrl(void)
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
	if(m_pImageCollapse != NULL)
	{
		delete m_pImageCollapse;
		m_pImageCollapse = NULL;
	}
	if(m_pImageToggle != NULL)
	{
		delete m_pImageToggle;
		m_pImageToggle = NULL;
	}
}

// 图片属性的实现
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, CheckBox, 6)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, Collapse, 8)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, Toggle, 6)

// 加载XML节点，解析节点中的属性信息设置到当前控件的属性中
BOOL CDuiTreeCtrl::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
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

	// 加载下层的cloumn节点信息
	for (DuiXmlNode pColumnElem = pXmlElem.child(_T("column")); pColumnElem; pColumnElem=pColumnElem.next_sibling(_T("column")))
	{
		CString strClrText = pColumnElem.attribute(_T("crtext")).value();
		CStringA strClrTextA;
		strClrTextA = strClrText;
		CString strWidth = pColumnElem.attribute(_T("width")).value();
		CString strTitle = pColumnElem.attribute(_T("title")).value();
		DuiSystem::Instance()->ParseDuiString(strTitle);
		Color clrText = CDuiObject::StringToColor(strClrText);
		int nWidth = -1;
		if(!strWidth.IsEmpty())
		{
			nWidth = _ttoi(strWidth);
		}
		InsertColumn(-1, strTitle, nWidth, clrText);
	}

	// 加载下层的node节点信息
	LoadNode(NULL, pXmlElem);

	// 刷新所有行
	//RefreshNodeRows();

    return TRUE;
}

// 加载XML树节点
BOOL CDuiTreeCtrl::LoadNode(HTREEITEM hParentNode, DuiXmlNode pXmlElem)
{
	// 加载下层的Node节点信息
	for (DuiXmlNode pNodeElem = pXmlElem.child(_T("node")); pNodeElem; pNodeElem=pNodeElem.next_sibling(_T("node")))
	{
		CString strId = pNodeElem.attribute(_T("id")).value();
		CString strCheck = pNodeElem.attribute(_T("check")).value();
		CString strImage = pNodeElem.attribute(_T("image")).value();
		CString strRightImage = pNodeElem.attribute(_T("right-img")).value();
		CString strClrText = pNodeElem.attribute(_T("crtext")).value();
		CString strClrBack = pNodeElem.attribute(_T("crback")).value();
		CString strCollapse = pNodeElem.attribute(_T("collapse")).value();

		int nCheck = -1;
		if(!strCheck.IsEmpty())
		{
			nCheck = _ttoi(strCheck);
		}

		BOOL bCollapse = (strCollapse == _T("1"));

		// 左边图片,通过Skin读取
		CString strSkin = _T("");
		if(strImage.Find(_T("skin:")) == 0)
		{
			strSkin = DuiSystem::Instance()->GetSkin(strImage);
		}else
		{
			strSkin = strImage;
		}

		int nImageIndex = -1;
		strImage = _T("");
		if(strSkin.Find(_T(".")) != -1)
		{
			// 图片文件
			strImage = strSkin;
		}else
		if(!strSkin.IsEmpty())
		{
			// 图片索引
			nImageIndex = _ttoi(strSkin);
		}

		// 右边图片,通过Skin读取
		CString strRightSkin = _T("");
		if(strRightImage.Find(_T("skin:")) == 0)
		{
			strRightSkin = DuiSystem::Instance()->GetSkin(strRightImage);
		}else
		{
			strRightSkin = strRightImage;
		}

		int nRightImageIndex = -1;
		strRightImage = _T("");
		if(strRightSkin.Find(_T(".")) != -1)
		{
			// 图片文件
			strRightImage = strRightSkin;
		}else
		if(!strRightSkin.IsEmpty())
		{
			// 图片索引
			nRightImageIndex = _ttoi(strRightSkin);
		}

		Color clrText = CDuiObject::StringToColor(strClrText);
		Color clrBack = CDuiObject::StringToColor(strClrBack);

		TreeNodeInfo nodeInfo;
		nodeInfo.hParentNode = hParentNode;
		nodeInfo.strId = strId;
		nodeInfo.nCheck = nCheck;
		nodeInfo.nImageIndex = nImageIndex;
		nodeInfo.pImage = NULL;
		nodeInfo.sizeImage.SetSize(0, 0);
		nodeInfo.nRightImageIndex = nRightImageIndex;
		nodeInfo.pRightImage = NULL;
		nodeInfo.sizeRightImage.SetSize(0, 0);
		nodeInfo.bRowColor = FALSE;
		nodeInfo.clrText = clrText;
		nodeInfo.bRowBackColor = FALSE;
		nodeInfo.clrBack = clrBack;
		nodeInfo.nHoverItem = -1;
		nodeInfo.bCollapse = bCollapse;
		nodeInfo.bHide = FALSE;
		if(clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
		{
			nodeInfo.bRowColor = TRUE;
		}
		if(clrBack.GetValue() != Color(0, 0, 0, 0).GetValue())
		{
			nodeInfo.bRowBackColor = TRUE;
		}
		HTREEITEM hNode = InsertNode(hParentNode, nodeInfo);
		if(hNode == NULL)
		{
			continue;
		}

		int nItemIndex = 0;
		// 加载下层的item节点信息
		for (DuiXmlNode pItemElem = pNodeElem.child(_T("item")); pItemElem; pItemElem=pItemElem.next_sibling(_T("item")))
		{
			CString strTitle = pItemElem.attribute(_T("title")).value();
			CString strContent = pItemElem.attribute(_T("content")).value();
			CString strClrText = pItemElem.attribute(_T("crtext")).value();
			CString strImage = pItemElem.attribute(_T("image")).value();
			CString strImageCount = pItemElem.attribute(_T("img-count")).value();
			CString strLink = pItemElem.attribute(_T("link")).value();
			CString strLinkAction = pItemElem.attribute(_T("linkaction")).value();
			CString strFontTitle = pItemElem.attribute(_T("font-title")).value();
			CString strShowCollapse = pItemElem.attribute(_T("collapse")).value();
			DuiSystem::Instance()->ParseDuiString(strTitle);
			DuiSystem::Instance()->ParseDuiString(strContent);
			DuiSystem::Instance()->ParseDuiString(strLink);
			DuiSystem::Instance()->ParseDuiString(strLinkAction);
			BOOL bShowCollapse = (strShowCollapse == _T("1"));
			Color clrText = CDuiObject::StringToColor(strClrText);

			// 图片,通过Skin读取
			CString strSkin = _T("");
			if(strImage.Find(_T("skin:")) == 0)
			{
				strSkin = DuiSystem::Instance()->GetSkin(strImage);
			}else
			{
				strSkin = strImage;
			}

			int nImageIndex = -1;
			strImage = _T("");
			if(strSkin.Find(_T(".")) != -1)
			{
				// 图片文件
				strImage = strSkin;
			}else
			if(!strSkin.IsEmpty())
			{
				// 图片索引
				nImageIndex = _ttoi(strSkin);
			}
			int nImageCount = _ttoi(strImageCount);

			BOOL bUseTitleFont = (strFontTitle == _T("1"));

			if(bShowCollapse)
			{
				// 设置收缩图片单元格
				SetSubItemCollapse(hNode, nItemIndex, strImage, nImageCount);
			}else
			if(!strLink.IsEmpty())
			{
				// 设置链接单元格
				SetSubItemLink(hNode, nItemIndex, strLink, strLinkAction, nImageIndex, clrText, strImage);
			}else
			{
				// 普通单元格
				SetSubItem(hNode, nItemIndex, strTitle, strContent, bUseTitleFont, nImageIndex, clrText, strImage);
			}

			// 加载下层的控件节点信息
			TreeItemInfo* pItemInfo = GetItemInfo(hNode, nItemIndex);
			for (DuiXmlNode pControlElem = pItemElem.first_child(); pControlElem; pControlElem=pControlElem.next_sibling())
			{
				if((pControlElem != NULL) && (pItemInfo != NULL))
				{
					CString strControlName = pControlElem.name();
					CControlBase* pControl = _CreateControlByName(strControlName);
					if(pControl)
					{
						pControl->Load(pControlElem);
						pControl->SetVisible(FALSE);
						// 将控件指针添加到gridctrl控件的子控件列表中
						m_vecControl.push_back(pControl);
						// 将控件指针添加到单元格的控件列表中(仅用于按照单元格查找子控件)
						pItemInfo->vecControl.push_back(pControl);
					}
				}
			}

			nItemIndex++;
		}

		// 加载子节点
		LoadNode(hNode, pNodeElem);
	}

	return TRUE;
}

// 添加列
BOOL CDuiTreeCtrl::InsertColumn(int nColumn, CString strTitle, int nWidth, Color clrText)
{
	TreeColumnInfo columnInfo;
	columnInfo.strTitle = strTitle;
	columnInfo.clrText = clrText;
	columnInfo.nWidth = nWidth;
	if(nColumn <= -1 || nColumn >= (int)m_vecColumnInfo.size())
	{
		m_vecColumnInfo.push_back(columnInfo);
	}
	else
	{
		m_vecColumnInfo.insert(m_vecColumnInfo.begin() + nColumn, columnInfo);
	}

	int nXPos = 0;
	int nYPos = 0;

	for(size_t i = 0; i < (int)m_vecColumnInfo.size(); i++)
	{
		TreeColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
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

// 添加树节点
HTREEITEM CDuiTreeCtrl::InsertNode(HTREEITEM hParentNode, CString strId, CString strTitle, BOOL bCollapse,
							int nImageIndex, Color clrText, CString strImage,
							int nRightImageIndex, CString strRightImage, int nCheck, Color clrBack)
{
	TreeNodeInfo nodeInfo;
	nodeInfo.hParentNode = hParentNode;
	nodeInfo.strId = strId;
	nodeInfo.nCheck = nCheck;
	nodeInfo.nImageIndex = nImageIndex;
	nodeInfo.sizeImage.SetSize(0, 0);
	nodeInfo.nRightImageIndex = nRightImageIndex;
	nodeInfo.sizeRightImage.SetSize(0, 0);
	nodeInfo.bRowColor = FALSE;
	nodeInfo.clrText = clrText;
	nodeInfo.bRowBackColor = FALSE;
	nodeInfo.clrBack = clrBack;
	nodeInfo.nHoverItem = -1;
	nodeInfo.bCollapse = bCollapse;
	nodeInfo.bHide = FALSE;
	if(clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		nodeInfo.bRowColor = TRUE;
	}
	if(clrBack.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		nodeInfo.bRowBackColor = TRUE;
	}

	// 左边图片
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, nodeInfo.pImage))
		{
			nodeInfo.sizeImage.SetSize(nodeInfo.pImage->GetWidth() / 1, nodeInfo.pImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		nodeInfo.pImage = NULL;
		if((nodeInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			nodeInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	// 右边图片
	if(!strRightImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(DuiSystem::Instance()->LoadImageFile(strRightImage, m_bImageUseECM, nodeInfo.pRightImage))
		{
			nodeInfo.sizeRightImage.SetSize(nodeInfo.pRightImage->GetWidth() / 1, nodeInfo.pRightImage->GetHeight());
		}
	}else
	{
		// 使用索引图片
		nodeInfo.pRightImage = NULL;
		if((nodeInfo.nRightImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			nodeInfo.sizeRightImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	HTREEITEM hNode = InsertNode(hParentNode, nodeInfo);

	if(!strTitle.IsEmpty() && (hNode != NULL))
	{
		SetSubItem(hNode, 0, strTitle);
	}

	return hNode;
}

// 添加树节点
HTREEITEM CDuiTreeCtrl::InsertNode(HTREEITEM hParentNode, TreeNodeInfo &nodeInfo)
{
	nodeInfo.hNode = m_nNodeIndex++;

	int nParentRow = GetNodeRow(hParentNode);
	int nLastChildRow = GetNodeLastChildRow(hParentNode);

	TreeNodeInfo* pNodeInfo = NULL;
	if(nLastChildRow != -1)
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nLastChildRow + 1, nodeInfo);
		pNodeInfo = &(m_vecRowInfo.at(nLastChildRow + 1));
	}else
	if(nParentRow != -1)
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nParentRow + 1, nodeInfo);
		pNodeInfo = &(m_vecRowInfo.at(nParentRow + 1));
	}else
	{
		m_vecRowInfo.push_back(nodeInfo);
		pNodeInfo = &(m_vecRowInfo.at(m_vecRowInfo.size() - 1));
	}

	if(pNodeInfo == NULL)
	{
		return NULL;
	}

	// 刷新所有行的位置
	RefreshNodeRows();

	return pNodeInfo->hNode;
}

// 设置表格项内容(文字表格项)
BOOL CDuiTreeCtrl::SetSubItem(HTREEITEM hNode, int nItem, CString strTitle, CString strContent, BOOL bUseTitleFont, int nImageIndex, Color clrText, CString strImage)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		TreeItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.nImageCount = 0;
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = bUseTitleFont;
		itemInfo.bShowCollapse = FALSE;
		TreeColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strTitle = strTitle;
	itemInfo.strContent = strContent;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	TreeColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// 图片
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
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
BOOL CDuiTreeCtrl::SetSubItemLink(HTREEITEM hNode, int nItem, CString strLink, CString strLinkAction, int nImageIndex, Color clrText, CString strImage)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		TreeItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.nImageCount = 0;
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = FALSE;
		itemInfo.bShowCollapse = FALSE;
		TreeColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strLink = strLink;
	itemInfo.strLinkAction = strLinkAction;
	itemInfo.nImageIndex = nImageIndex;
	itemInfo.sizeImage = CSize(0,0);
	itemInfo.clrText = clrText;

	TreeColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// 图片
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
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

// 设置表格项为收缩图片显示
BOOL CDuiTreeCtrl::SetSubItemCollapse(HTREEITEM hNode, int nItem, CString strImage, int nImageCount)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		TreeItemInfo itemInfo;
		itemInfo.strTitle = _T("");
		itemInfo.strContent = _T("");
		itemInfo.nImageIndex = -1;
		itemInfo.pImage = NULL;
		itemInfo.sizeImage = CSize(0,0);
		itemInfo.nImageCount = 0;
		itemInfo.clrText = Color(0, 0, 0, 0);
		itemInfo.strLink = _T("");
		itemInfo.strLinkAction = _T("");
		itemInfo.bNeedTitleTip = FALSE;
		itemInfo.bNeedContentTip = FALSE;
		itemInfo.bUseTitleFont = FALSE;
		itemInfo.bShowCollapse = FALSE;
		TreeColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.bShowCollapse = TRUE;
	itemInfo.nImageCount = nImageCount;

	TreeColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);

	// 收缩图片
	if(!strImage.IsEmpty())
	{
		// 使用行数据指定的图片
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
		{
			if(itemInfo.nImageCount == 0)
			{
				itemInfo.nImageCount = 6;
			}
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / itemInfo.nImageCount, itemInfo.pImage->GetHeight());
		}
	}

	return TRUE;
}

// 给树节点单元格添加子控件
BOOL CDuiTreeCtrl::AddSubItemControl(HTREEITEM hNode, int nItem, CControlBase* pControl)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	TreeItemInfo* pItemInfo = GetItemInfo(hNode, nItem);
	if(pItemInfo == NULL)
	{
		SetSubItem(hNode, nItem, _T(""));
		pItemInfo = GetItemInfo(hNode, nItem);
	}
	if(pItemInfo == NULL)
	{
		return FALSE;
	}

	if(pControl)
	{
		pControl->SetParent(this);
		pControl->SetVisible(FALSE);
		// 将控件指针添加到treectrl控件的子控件列表中
		m_vecControl.push_back(pControl);
		// 将控件指针添加到树节点单元格的控件列表中(仅用于按照单元格查找子控件)
		pItemInfo->vecControl.push_back(pControl);
		UpdateControl(true);
	}

	return TRUE;
}

// 删除树节点单元格的指定子控件(通过控件对象指针删除)
BOOL CDuiTreeCtrl::DeleteSubItemControl(CControlBase* pControl)
{
	// 查找所有单元格,删除对应的控件对象引用
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
			vector<CControlBase*>::iterator it;
			for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
			{
				CControlBase* _pControl = *it;
				if(_pControl == pControl)
				{
					itemInfo.vecControl.erase(it);
					break;
				}
			}
		}
	}

	// 删除子控件中对应的控件对象
	RemoveControl(pControl);

	return TRUE;
}

// 删除树节点单元格的指定子控件(通过控件名和控件ID删除)
BOOL CDuiTreeCtrl::DeleteSubItemControl(CString strControlName, UINT uControlID)
{
	// 查找所有单元格,删除对应的控件对象引用
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
			vector<CControlBase*>::iterator it;
			for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
			{
				CControlBase* _pControl = *it;
				if(_pControl && _pControl->IsThisObject(uControlID, strControlName))
				{
					itemInfo.vecControl.erase(it);
					break;
				}
			}
		}
	}

	// 删除子控件中对应的控件对象
	RemoveControl(strControlName, uControlID);

	return TRUE;
}

// 删除节点
BOOL CDuiTreeCtrl::DeleteNode(HTREEITEM hNode)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}

	// 查找行的所有单元格,删除对应的子控件对象
	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
	{
		TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
		vector<CControlBase*>::iterator it;
		for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
		{
			CControlBase* _pControl = *it;
			// 删除单元格包含的子控件
			RemoveControl(_pControl);
		}
	}

	int nIndex = 0;
	vector<TreeNodeInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nRow)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}

	// 重新计算所有行的位置
	RefreshNodeRows();

	return true;
}

// 获取节点的行号
int CDuiTreeCtrl::GetNodeRow(HTREEITEM hNode)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hNode == hNode)
		{
			return i;
		}
	}

	return -1;
}

// 获取节点的最后一个子节点行号(遍历到最底层的子节点)
int CDuiTreeCtrl::GetNodeLastChildRow(HTREEITEM hNode)
{
	int nRow = -1;
	HTREEITEM hLastChildNode = NULL;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hNode)
		{
			hLastChildNode = rowInfoTemp.hNode;
			nRow = i;
		}
	}

	if(hLastChildNode != NULL)
	{
		int nLastChildRow = GetNodeLastChildRow(hLastChildNode);
		if(nLastChildRow != -1)
		{
			return nLastChildRow;
		}
	}

	return nRow;
}

// 判断一个节点是否有子节点
BOOL CDuiTreeCtrl::HaveChildNode(HTREEITEM hNode)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hNode)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// 获取父节点句柄
HTREEITEM CDuiTreeCtrl::GetParentNode(HTREEITEM hNode)
{
	TreeNodeInfo* pNodeInfo = GetNodeInfo(hNode);
	if(pNodeInfo)
	{
		return pNodeInfo->hParentNode;
	}

	return NULL;
}

// 获取第一个子节点句柄
HTREEITEM CDuiTreeCtrl::GetChildNode(HTREEITEM hNode)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hNode)
		{
			return rowInfoTemp.hNode;
		}
	}

	return NULL;
}

// 获取下一个子节点句柄
HTREEITEM CDuiTreeCtrl::GetNextSiblingNode(HTREEITEM hNode)
{
	BOOL bFind = FALSE;
	HTREEITEM hParentNode = NULL;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hNode == hNode)
		{
			hParentNode = rowInfoTemp.hParentNode;
			bFind = TRUE;
		}else
		if(bFind && (rowInfoTemp.hParentNode == hParentNode))
		{
			return rowInfoTemp.hNode;
		}
	}

	return NULL;
}

// 获取前一个子节点句柄
HTREEITEM CDuiTreeCtrl::GetPrevSiblingNode(HTREEITEM hNode)
{
	BOOL bFind = FALSE;
	HTREEITEM hParentNode = NULL;
	for(size_t i = m_vecRowInfo.size()-1; i >= 0; i--)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hNode == hNode)
		{
			hParentNode = rowInfoTemp.hParentNode;
			bFind = TRUE;
		}else
		if(bFind && (rowInfoTemp.hParentNode == hParentNode))
		{
			return rowInfoTemp.hNode;
		}
	}

	return NULL;
}

// 获取某个节点的子节点个数
int CDuiTreeCtrl::GetChildNodeCount(HTREEITEM hNode)
{
	int nCount = 0;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hNode)
		{
			nCount++;
		}
	}

	return nCount;
}

// 获取一个节点的层级
int CDuiTreeCtrl::GetNodeLevel(HTREEITEM hNode)
{
	int nLevel = 0;
	TreeNodeInfo* pNodeInfo = GetNodeInfo(hNode);
	while(pNodeInfo != NULL)
	{
		if(pNodeInfo->hParentNode == NULL)
		{
			break;
		}
		nLevel++;
		pNodeInfo = GetNodeInfo(pNodeInfo->hParentNode);
	}

	return nLevel;
}

// 根据节点ID获取节点的句柄
HTREEITEM CDuiTreeCtrl::GetNodeById(CString strId)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.strId == strId)
		{
			return rowInfoTemp.hNode;
		}
	}

	return NULL;
}

// 获取某一个行信息
TreeNodeInfo* CDuiTreeCtrl::GetNodeInfo(HTREEITEM hNode)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return NULL;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// 获取某一个单元格信息
TreeItemInfo* CDuiTreeCtrl::GetItemInfo(HTREEITEM hNode, int nItem)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return NULL;
	}

	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return NULL;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	if(nItem >= (int)rowInfo.vecItemInfo.size())
	{
		return NULL;
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);

	return &itemInfo;
}

// 设置某一个单元格信息
void CDuiTreeCtrl::SetItemInfo(HTREEITEM hNode, int nItem, TreeItemInfo* pItemInfo)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return;
	}

	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(int i = rowInfo.vecItemInfo.size(); i <= nItem; i++)
	{
		TreeItemInfo itemInfo;
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
		itemInfo.bShowCollapse = FALSE;
		TreeColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
		rowInfo.vecItemInfo.push_back(itemInfo);
	}

	TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);
	itemInfo.strTitle = pItemInfo->strTitle;
	itemInfo.strContent = pItemInfo->strContent;
	itemInfo.nImageIndex = pItemInfo->nImageIndex;
	itemInfo.pImage = pItemInfo->pImage;
	itemInfo.sizeImage = pItemInfo->sizeImage;
	itemInfo.clrText = pItemInfo->clrText;
	itemInfo.strLink = pItemInfo->strLink;
	itemInfo.strLinkAction = pItemInfo->strLinkAction;
	itemInfo.bNeedTitleTip = pItemInfo->bNeedTitleTip;
	itemInfo.bNeedContentTip = pItemInfo->bNeedContentTip;
	itemInfo.bUseTitleFont = pItemInfo->bUseTitleFont;
	itemInfo.bShowCollapse = pItemInfo->bShowCollapse;

	TreeColumnInfo &columnInfo = m_vecColumnInfo.at(nItem);
	itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
			columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
}

// 设置某一个行的文字颜色
void CDuiTreeCtrl::SetNodeColor(HTREEITEM hNode, Color clrText)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// 设置某一个行的背景颜色
void CDuiTreeCtrl::SetNodeBackColor(HTREEITEM hNode, Color clrBack)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowBackColor = TRUE;
	rowInfo.clrBack = clrBack;
}

// 切换节点的缩放状态
void CDuiTreeCtrl::ToggleNode(HTREEITEM hNode)
{
	if(HaveChildNode(hNode))
	{
		TreeNodeInfo* pNodeInfo = GetNodeInfo(hNode);
		if(pNodeInfo != NULL)
		{
			pNodeInfo->bCollapse = !pNodeInfo->bCollapse;
			RefreshNodeRows();
		}
	}
}

// 切换节点的缩放状态
void CDuiTreeCtrl::ExpandNode(HTREEITEM hNode, BOOL bExpand)
{
	// 展开父节点
	HTREEITEM hParentNode = NULL;
	HTREEITEM hTempNode = hNode;
	while((hParentNode = GetParentNode(hTempNode)) != NULL)
	{
		TreeNodeInfo* pParentNodeInfo = GetNodeInfo(hParentNode);
		if(pParentNodeInfo && pParentNodeInfo->bCollapse && bExpand)
		{
			pParentNodeInfo->bCollapse = FALSE;
		}
		hTempNode = hParentNode;
	}

	// 展开或收缩子节点
	if(HaveChildNode(hNode))
	{
		TreeNodeInfo* pNodeInfo = GetNodeInfo(hNode);
		if(pNodeInfo != NULL)
		{
			pNodeInfo->bCollapse = bExpand ? FALSE : TRUE;
		}
	}

	RefreshNodeRows();
}

// 设置某一个节点的检查框状态
void CDuiTreeCtrl::SetNodeCheck(HTREEITEM hNode, int nCheck)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.nCheck = nCheck;
}

// 获取某一个节点的检查框状态
int CDuiTreeCtrl::GetNodeCheck(HTREEITEM hNode)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return -1;
	}

	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	return rowInfo.nCheck;
}

// 清空树节点
void CDuiTreeCtrl::ClearNodes()
{
	// 删除所有子控件
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
			vector<CControlBase*>::iterator it;
			for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
			{
				CControlBase* pControl = *it;
				RemoveControl(pControl);
			}
		}
	}

	m_vecRowInfo.clear();
	m_pControScrollV->SetVisible(FALSE);
	UpdateControl(true);
}

// 隐藏子节点
void CDuiTreeCtrl::HideChildNodes(HTREEITEM hItem)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hItem)
		{
			rowInfoTemp.bHide = TRUE;
			// 隐藏此节点的所有单元格子控件
			for(size_t j = 0; j < rowInfoTemp.vecItemInfo.size(); j++)
			{
				TreeItemInfo &itemInfo = rowInfoTemp.vecItemInfo.at(j);
				for(size_t k = 0; k < itemInfo.vecControl.size(); k++)
				{
					CControlBase* pControl = itemInfo.vecControl.at(k);
					pControl->SetVisible(FALSE);
				}
			}
			// 遍历下级节点
			HideChildNodes(rowInfoTemp.hNode);
		}
	}
}

// 刷新所有节点行
void CDuiTreeCtrl::RefreshNodeRows()
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		rowInfoTemp.bHide = FALSE;
	}

	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.bCollapse)
		{
			HideChildNodes(rowInfoTemp.hNode);
		}
	}

	// 重新计算所有行的位置
	int nXPos = 0;
	int nYPos = 0;
	int nVisibleRows = 0;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.bHide)
		{
			continue;
		}

		// 计算行位置
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		int nXPos = 0;
		if(m_pImageToggle != NULL)
		{
			int nNodeLevel = GetNodeLevel(rowInfoTemp.hNode);
			nXPos += (nNodeLevel * m_sizeToggle.cx);
			if(HaveChildNode(rowInfoTemp.hNode))
			{
				nXPos += m_sizeToggle.cx;
			}
		}

		// 计算检查框位置
		if((rowInfoTemp.nCheck != -1) && (m_pImageCheckBox != NULL))
		{
			int nCheckImgY = 3;
			if((m_sizeCheckBox.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
			{
				nCheckImgY = (m_nRowHeight - m_sizeCheckBox.cy) / 2 + 1;
			}
			rowInfoTemp.rcCheck.left = nXPos;
			rowInfoTemp.rcCheck.right = rowInfoTemp.rcCheck.left + m_sizeCheckBox.cx;
			rowInfoTemp.rcCheck.top = rowInfoTemp.rcRow.top + nCheckImgY;
			rowInfoTemp.rcCheck.bottom = rowInfoTemp.rcCheck.top + m_sizeCheckBox.cy;
		}

		// 计算每个单元格位置
		for(int j = 0; j < (int)rowInfoTemp.vecItemInfo.size(); j++)
		{
			TreeItemInfo &itemInfo = rowInfoTemp.vecItemInfo.at(j);
			itemInfo.rcItem.top = rowInfoTemp.rcRow.top;
			itemInfo.rcItem.bottom = rowInfoTemp.rcRow.bottom;
			// 第一个单元格左侧位置要排除收缩图片的宽度
			if((j == 0) && (m_pImageToggle != NULL) && HaveChildNode(rowInfoTemp.hNode))
			{
				int nNodeLevel = GetNodeLevel(rowInfoTemp.hNode);
				itemInfo.rcItem.left = (nNodeLevel+1) * m_sizeToggle.cx + rowInfoTemp.rcRow.left;
			}
		}

		nYPos += m_nRowHeight;
		nVisibleRows++;
	}
	m_nVisibleRowCount = nVisibleRows;

	// 需要的总高度大于显示区高度才会显示滚动条
	m_pControScrollV->SetVisible((nVisibleRows * m_nRowHeight) > m_rc.Height());
	((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange(nVisibleRows * m_nRowHeight);

	UpdateControl(true);
}

// 将指定的节点滚动到可见范围
BOOL CDuiTreeCtrl::EnsureVisible(HTREEITEM hNode, BOOL bPartialOK)
{
	// 如果节点未展开,则首先展开节点
	ExpandNode(hNode, TRUE);

	// 计算节点对应的行号
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}

	int nViewRowCount = m_rc.Height() / m_nRowHeight;

	// 如果指定的行已经处于可见范围则直接返回
	if((nRow >= m_nFirstViewRow) && (nRow < (m_nFirstViewRow + nViewRowCount)))
	{
		return TRUE;
	}

	// 滚动到可见范围
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	if(nRow < m_nFirstViewRow)
	{
		pScrollV->SetScrollCurrentPos(nRow * m_nRowHeight);
	}else
	{
		int nFirstRow = nRow - nViewRowCount + 2;
		if(nFirstRow < 0)
		{
			nFirstRow = 0;
		}
		pScrollV->SetScrollCurrentPos(nFirstRow * m_nRowHeight);
	}

	UpdateControl(true);
	return TRUE;
}

// 从XML设置Font-title属性
HRESULT CDuiTreeCtrl::OnAttributeFontTitle(const CString& strValue, BOOL bLoading)
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

void CDuiTreeCtrl::SetControlRect(CRect rc)
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
				rcTemp.left = rcTemp.right - m_nScrollWidth;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// 重新计算所有行的位置
	RefreshNodeRows();
}

// 判断指定的坐标位置是否在某一行中
BOOL CDuiTreeCtrl::PtInRow(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return FALSE;
	}

	CRect rc = rowInfo.rcRow;
	// rcRow坐标是插入行节点时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// 判断指定的坐标位置是否在某一行的检查框中
BOOL CDuiTreeCtrl::PtInRowCheck(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return FALSE;
	}

	CRect rc = rowInfo.rcCheck;
	// rcCheck坐标是画图时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// 判断指定的坐标位置是否在某一行的收缩图片位置
BOOL CDuiTreeCtrl::PtInRowCollapse(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return FALSE;
	}

	// 判断节点的缩放图片位置
	if((m_pImageToggle != NULL) && HaveChildNode(rowInfo.hNode))
	{
		int nNodeLevel = GetNodeLevel(rowInfo.hNode);
		CRect rc = rowInfo.rcRow;
		rc.left = nNodeLevel * m_sizeToggle.cx;
		rc.right = rc.left + m_sizeToggle.cx;
		rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return TRUE;
		}
	}

	// 判断鼠标是否在显示缩放图片的单元格内
	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		if(!itemInfo.bShowCollapse)
		{
			continue;
		}

		CRect rc = itemInfo.rcItem;
		// rcItem坐标是画图时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
		rc.OffsetRect(m_rc.left + m_nLeftPos, m_rc.top-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return TRUE;
		}
	}

	return FALSE;
}

// 判断指定的坐标位置是否在某一行的某个列中,返回列索引
int CDuiTreeCtrl::PtInRowItem(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return -1;
	}

	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		CRect rc = itemInfo.rcItem;
		// rcItem坐标是画图时候计算出的按照控件虚拟显示区域为参照的坐标,需要转换为鼠标坐标
		rc.OffsetRect(m_rc.left + m_nLeftPos, m_rc.top-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

// 设置单元格的Tooltip
void CDuiTreeCtrl::SetGridTooltip(HTREEITEM hNode, int nItem, CString strTooltip)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return;
	}

	if((m_nTipNode != hNode) || (m_nTipItem != nItem) || (m_nTipVirtualTop != m_nVirtualTop))
	{
		TreeItemInfo* pGridInfo = GetItemInfo(hNode, nItem);
		if(pGridInfo && (pGridInfo->bNeedTitleTip || pGridInfo->bNeedContentTip))
		{
			CRect rc = pGridInfo->rcItem;
			rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
			SetTooltip(this, strTooltip, rc, TRUE);
		}else
		{
			ClearTooltip();
		}
		m_nTipNode = hNode;
		m_nTipItem = nItem;
		m_nTipVirtualTop = m_nVirtualTop;
	}
}

// 清除Tooltip
void CDuiTreeCtrl::ClearGridTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipNode = NULL;
		m_nTipItem = -1;
		m_nTipVirtualTop = 0;
		return;
	}

	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->ClearTooltip();
		m_nTipNode = NULL;
		m_nTipItem = -1;
		m_nTipVirtualTop = 0;
		return;
	}
}

// 鼠标移动事件处理
BOOL CDuiTreeCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
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
		if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
		{
			TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
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
					TreeItemInfo* pGridInfo = GetItemInfo(rowInfo.hNode, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(rowInfo.hNode, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(rowInfo.hNode, rowInfo.nHoverItem, pGridInfo->strContent);
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
		if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
		{
			TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
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
					TreeItemInfo* pGridInfo = GetItemInfo(rowInfo.hNode, rowInfo.nHoverItem);
					if(pGridInfo && pGridInfo->bNeedTitleTip)
					{
						SetGridTooltip(rowInfo.hNode, rowInfo.nHoverItem, pGridInfo->strTitle);
					}else
					if(pGridInfo && pGridInfo->bNeedContentTip)
					{
						SetGridTooltip(rowInfo.hNode, rowInfo.nHoverItem, pGridInfo->strContent);
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
				TreeNodeInfo &rowInfo = m_vecRowInfo.at(i);
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

// 鼠标左键按下事件处理
BOOL CDuiTreeCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// 设置窗口焦点,否则可能无法进行滚动事件的处理
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo) && !PtInRowCollapse(point, rowInfo))	// 检查框和收缩事件只在鼠标放开时候触发
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(MSG_BUTTON_DOWN, rowInfo.hNode, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// 如果点击的还是之前点击的行，也同样会发送鼠标点击事件
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// 检查框事件只在鼠标放开时候触发
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(MSG_BUTTON_DOWN, rowInfo.hNode, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

// 鼠标左键放开事件处理
BOOL CDuiTreeCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// 检查框状态改变
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_CHECK, rowInfo.hNode, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}else
			if(PtInRowCollapse(point, rowInfo))	// 收缩状态改变
			{
				rowInfo.bCollapse = !rowInfo.bCollapse;
				RefreshNodeRows();

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// 如果点击的还是之前点击的行，也同样会发送鼠标点击事件
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// 检查框状态改变
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_CHECK, rowInfo.hNode, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}else
			if(PtInRowCollapse(point, rowInfo))	// 收缩状态改变
			{
				rowInfo.bCollapse = !rowInfo.bCollapse;
				RefreshNodeRows();

				return true;
			}
		}
	}

	return false;
}

// 鼠标左键双击事件处理
BOOL CDuiTreeCtrl::OnControlLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if(m_rc.PtInRect(point))
	{
		for(size_t i = 0; i < m_vecRowInfo.size(); i++)
		{
			TreeNodeInfo &rowInfo = m_vecRowInfo.at(i);
			if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo) && !PtInRowCollapse(point, rowInfo))
			{
				int nClickItem = PtInRowItem(point, rowInfo);
				SendMessage(MSG_BUTTON_DBLCLK, rowInfo.hNode, nClickItem);
				return true;
			}
		}
	}

	return false;
}

// 滚动事件处理
BOOL CDuiTreeCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if((m_nVisibleRowCount * m_nRowHeight) <= m_rc.Height())
	{
		return false;
	}

	// 更新滚动条,并刷新界面
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	if(pScrollV->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
	}

	return true;
}

// 键盘事件处理
BOOL CDuiTreeCtrl::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 如果当前处于焦点状态,用上下键可以移动当前选择行
	if(IsFocusControl() && m_bEnableDownRow && (nChar == VK_UP) && (nFlags == 0))
	{
		if(m_nDownRow > 0)
		{
			m_nDownRow--;
			TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
			EnsureVisible(rowInfo.hNode, TRUE);
			UpdateControl(TRUE);
		}
		return true;
	}else
	if(IsFocusControl() && m_bEnableDownRow && (nChar == VK_DOWN) && (nFlags == 0))
	{
		if(m_nDownRow < (GetNodeCount() - 1))
		{
			m_nDownRow++;
			TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
			EnsureVisible(rowInfo.hNode, TRUE);
			UpdateControl(TRUE);
		}
		return true;
	}

	return __super::OnControlKeyDown(nChar, nRepCnt, nFlags);
}

// 消息响应
LRESULT CDuiTreeCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// 如果是滚动条的位置变更事件,则刷新显示
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == MSG_BUTTON_DOWN) && (lParam != -1))
	{
		// 点击了行的某个链接
		TreeNodeInfo* pRowInfo = GetNodeInfo(wParam);
		if(pRowInfo && (lParam >= 0) && (lParam < (int)pRowInfo->vecItemInfo.size()))
		{
			TreeItemInfo &itemInfo = pRowInfo->vecItemInfo.at(lParam);
			// 转换为MSG_BUTTON_UP消息,因为DuiSystem任务处理时候只处理MSG_BUTTON_UP消息
			if(!itemInfo.strLinkAction.IsEmpty())
			{
				DuiSystem::AddDuiActionTask(uID, MSG_BUTTON_UP, wParam, lParam, GetName(), itemInfo.strLinkAction, GetParent());
			}
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiTreeCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	// 列表画图方法:
	// 1.列表的虚拟高度为每一行高度*行数
	// 2.列表显示的top坐标由scroll控件记录
	// 3.重画时候,根据top坐标位置计算出显示的第一行的序号,根据显示高度计算出显示的最后一行的序号
	// 4.根据计算出的显示的行,画相应的内容到内存dc中
	// 5.计算出显示的top坐标进行内存dc的拷贝
	int nWidth = m_rc.Width() - m_nScrollWidth;	// 减去滚动条的宽度
	int nHeightAll = m_nVisibleRowCount*m_nRowHeight; // 总的虚拟高度 //m_rc.Height();
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// 当前top位置
	int nMaxRange = pScrollV->GetScrollMaxRange();

	m_nVirtualTop = (nMaxRange > 0) ? (int)((double)nCurPos*(nHeightAll-m_rc.Height())/nMaxRange) : 0;	// 当前滚动条位置对应的虚拟的top位置
	if(m_nVirtualTop < 0)
	{
		m_nVirtualTop = 0;
		pScrollV->SetScrollCurrentPos(0);
	}
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;			// 显示的第一行序号
	int nViewRowCount = m_rc.Height() / m_nRowHeight + 1;	// 显示的行数
	int nHeightView = nViewRowCount * m_nRowHeight;	// 显示涉及到的虚拟高度
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
		DrawVerticalTransition(m_memDC, dc, CRect(0, nYViewPos, nWidth, m_rc.Height()+nYViewPos),	// 背景透明度
				m_rc, m_nBkTransparent, m_nBkTransparent);
		
		BSTR bsFontTitle = m_strFontTitle.AllocSysString();
		FontFamily fontFamilyTitle(bsFontTitle);
		Font fontTitle(&fontFamilyTitle, (REAL)m_nFontTitleWidth, m_fontTitleStyle, UnitPixel);
		::SysFreeString(bsFontTitle);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		SolidBrush solidBrush(m_clrText);			// 正常文字画刷
		SolidBrush solidBrushH(m_clrTextHover);		// 热点文字画刷
		SolidBrush solidBrushD(m_clrTextDown);		// 当前行画刷
		SolidBrush solidBrushT(m_clrTitle);			// 标题文字画刷
		SolidBrush solidBrushS(m_clrSeperator);		// 分割线画刷

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// 设置普通文字的水平和垂直对齐方式
		DUI_STRING_ALIGN_DEFINE();

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
		
		if(m_vecRowInfo.size() > 0)
		{
			int nRowIndex = 0;
			int nFirstRowCount = 0;
			for(size_t i = 0; (nRowIndex < nViewRowCount) && (i < m_vecRowInfo.size()); i++)
			{
				TreeNodeInfo &rowInfo = m_vecRowInfo.at(i);
				if(rowInfo.bHide)
				{
					continue;
				}
				if(nFirstRowCount < m_nFirstViewRow)
				{
					nFirstRowCount++;
					continue;
				}

				SolidBrush solidBrushRow(rowInfo.clrText);	// 行定义的颜色

				int nXPos = 0;
				int nVI = nRowIndex;//i - m_nFirstViewRow;
				nRowIndex++;

				// 画树节点缩放图片(toggle)
				if(m_pImageToggle != NULL)
				{
					int nNodeLevel = GetNodeLevel(rowInfo.hNode);
					nXPos += (nNodeLevel * m_sizeToggle.cx);
					// 节点存在子节点，或者是顶层节点的情况下，画缩放图片
					if(HaveChildNode(rowInfo.hNode) || (GetParentNode(rowInfo.hNode) == NULL))
					{
						int nToggleImgY = (m_nRowHeight - m_sizeToggle.cy) / 2;
						int nToggleIndex = (m_nHoverRow == i) ? 1 : 0;
						graphics.DrawImage(m_pImageToggle, Rect(nXPos, nVI*m_nRowHeight + nToggleImgY, m_sizeToggle.cx, m_sizeToggle.cy),
							rowInfo.bCollapse ? nToggleIndex*m_sizeToggle.cx : (3+nToggleIndex)*m_sizeToggle.cx, 0, m_sizeToggle.cx, m_sizeToggle.cy, UnitPixel);
						nXPos += m_sizeToggle.cx;
					}
				}

				// 鼠标移动到行时候显示的背景颜色(如果设置为全0,则不显示行背景颜色)
				if((m_nHoverRow == i) && (m_clrRowHover.GetValue() != Color(0, 0, 0, 0).GetValue()))
				{
					SolidBrush brush(m_clrRowHover);
					graphics.FillRectangle(&brush, nXPos, nVI*m_nRowHeight, nWidth-nXPos, m_nRowHeight);
				}else
				if((m_nDownRow == i) && (m_clrRowCurrent.GetValue() != Color(0, 0, 0, 0).GetValue()))	// 如果是当前行,显示当前行的背景色
				{
					SolidBrush brush(m_clrRowCurrent);
					graphics.FillRectangle(&brush, nXPos, nVI*m_nRowHeight, nWidth-nXPos, m_nRowHeight);
				}else
				if(rowInfo.bRowBackColor)	// 如果设置了行的背景颜色,则填充颜色
				{
					SolidBrush brush(rowInfo.clrBack);
					graphics.FillRectangle(&brush, nXPos, nVI*m_nRowHeight, nWidth-nXPos, m_nRowHeight);
				}

				// 画检查框
				int nCheckImgY = 3;
				if((m_sizeCheckBox.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
				{
					nCheckImgY = (m_nRowHeight - m_sizeCheckBox.cy) / 2 + 1;
				}
				if((rowInfo.nCheck != -1) && (m_pImageCheckBox != NULL))
				{
					int nCheckImageIndex = ((m_nHoverRow == i) ? ((rowInfo.nCheck==1) ? 4 : 1) : ((rowInfo.nCheck==1) ? 2 : 0));
					graphics.DrawImage(m_pImageCheckBox, Rect(nXPos, nVI*m_nRowHeight + nCheckImgY, m_sizeCheckBox.cx, m_sizeCheckBox.cy),
						nCheckImageIndex * m_sizeCheckBox.cx, 0, m_sizeCheckBox.cx, m_sizeCheckBox.cy, UnitPixel);
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
					graphics.DrawImage(rowInfo.pImage, Rect(nXPos, nVI*m_nRowHeight + nImgY, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy),
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
					graphics.DrawImage(m_pImage, Rect(nXPos, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
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
					graphics.DrawImage(rowInfo.pRightImage, Rect(nWidth-rowInfo.sizeRightImage.cx-1, nVI*m_nRowHeight + nImgY, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy),
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
					graphics.DrawImage(m_pImage, Rect(nWidth-m_sizeImage.cx-1, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nRightImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nRightImageWidth = m_sizeImage.cx + 1;
				}

				// 画单元格内容
				int nPosItemX = (m_nLeftPos != 0) ? m_nLeftPos : nXPos;
				for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
				{
					TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
					BOOL bSingleLine = (itemInfo.strContent.IsEmpty() || !itemInfo.strLink.IsEmpty());
					RectF rect((Gdiplus::REAL)nPosItemX,
						(Gdiplus::REAL)(nVI*m_nRowHeight + 1),
						(Gdiplus::REAL)((j == 0) ? (itemInfo.rcItem.Width() - nPosItemX): itemInfo.rcItem.Width()),
						(Gdiplus::REAL)(bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2)));
					if((int)(rect.GetRight()) > nWidth)
					{
						// 最后一列需要减去滚动条宽度
						rect.Width -= m_nScrollWidth;
					}

					// 画单元格图片
					int nItemImageX = 0;
					int nImgY = 3;
					if((itemInfo.pImage != NULL) && (itemInfo.nImageCount <= 1))
					{
						if((itemInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - itemInfo.sizeImage.cy) / 2 + 1;
						}
						// 使用单元格指定的图片
						graphics.DrawImage(itemInfo.pImage, Rect(nPosItemX+nItemImageX, nVI*m_nRowHeight + nImgY, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
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
						graphics.DrawImage(m_pImage, Rect(nPosItemX+nItemImageX, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
							itemInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
						nItemImageX += (m_sizeImage.cx + 3);
					}
					rect.Offset((Gdiplus::REAL)nItemImageX, 0);
					rect.Width -= (Gdiplus::REAL)nItemImageX;

					// 画收缩图片
					if(itemInfo.bShowCollapse)
					{
						int nCollapseIndex = (m_nHoverRow == i) ? 1 : 0;
						if(itemInfo.pImage != NULL)
						{
							graphics.DrawImage(itemInfo.pImage, Rect(nPosItemX+nItemImageX, nVI*m_nRowHeight + (m_nRowHeight-itemInfo.sizeImage.cy)/2, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
								rowInfo.bCollapse ? nCollapseIndex*itemInfo.sizeImage.cx : ((itemInfo.nImageCount / 2)+nCollapseIndex)*itemInfo.sizeImage.cx, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
						}else
						if(m_pImageCollapse != NULL)
						{
							graphics.DrawImage(m_pImageCollapse, Rect(nPosItemX+nItemImageX, nVI*m_nRowHeight + (m_nRowHeight-m_sizeCollapse.cy)/2, m_sizeCollapse.cx, m_sizeCollapse.cy),
								rowInfo.bCollapse ? nCollapseIndex*m_sizeCollapse.cx : (4+nCollapseIndex)*m_sizeCollapse.cx, 0, m_sizeCollapse.cx, m_sizeCollapse.cy, UnitPixel);
						}
					}

					// 画单元格标题或链接内容
					SolidBrush solidBrushItem(m_clrText);
					if((m_nHoverRow == i) && (m_clrTextHover.GetValue() != Color(0, 0, 0, 0).GetValue()))	// 设置了鼠标移动颜色,则使用
					{
						solidBrushItem.SetColor(m_clrTextHover);
					}else
					if((m_nDownRow == i) && (m_clrTextDown.GetValue() != Color(0, 0, 0, 0).GetValue()))	// 设置了鼠标按下颜色,则使用
					{
						solidBrushItem.SetColor(m_clrTextDown);
					}else
					if(itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue())	// 设置了单元格颜色,则使用
					{
						solidBrushItem.SetColor(itemInfo.clrText);
					}else
					if(rowInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue())	// 设置了行颜色,则使用
					{
						solidBrushItem.SetColor(rowInfo.clrText);
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
					BSTR bsItemTitle = strItemTitle.AllocSysString();
					graphics.DrawString(bsItemTitle, (INT)wcslen(bsItemTitle),
						itemInfo.bUseTitleFont ? &fontTitle : &font, rect, &strFormat, itemInfo.bUseTitleFont ? &solidBrushT : &solidBrushItem);
					::SysFreeString(bsItemTitle);

					// 画单元格内容
					if(!bSingleLine)
					{
						rect.Offset(0, (Gdiplus::REAL)(m_nRowHeight / 2 + 2));
						rect.Height = (Gdiplus::REAL)(m_nRowHeight / 2 - 4);
						BSTR bsItemContent = itemInfo.strContent.AllocSysString();
						graphics.DrawString(bsItemContent, (INT)wcslen(bsItemContent), &font, rect, &strFormat, &solidBrushItem);
						::SysFreeString(bsItemContent);
					}

					// 设置单元格子控件的位置
					for(size_t k = 0; k < itemInfo.vecControl.size(); k++)
					{
						CControlBase* pControl = itemInfo.vecControl.at(k);
						if(pControl)
						{
							CRect rcParent = CRect(nPosItemX, nVI*m_nRowHeight + 1,
								(int)(rect.X+rect.Width), (nVI+1)*m_nRowHeight - 1);
							if((int)(rect.GetRight()) > nWidth)
							{
								// 最后一列需要减去滚动条宽度
								rcParent.right -= m_nScrollWidth;
							}
							rcParent.OffsetRect(m_rc.left, m_rc.top - nYViewPos);
							pControl->SetPositionWithParent(rcParent);
							CRect rcControl = pControl->GetRect();
							// 只有当前在显示范围内的控件设置为可见
							if((rcControl.top < m_rc.top) || (rcControl.bottom > m_rc.bottom))
							{
								pControl->SetVisible(FALSE);
							}else
							{
								pControl->SetVisible(TRUE);
							}
						}
					}

					if(j == 0)
					{
						// 为了使第二列开始是对齐的,所以第二列开始位置按照第一列的宽度计算
						nPosItemX = itemInfo.rcItem.right;
					}else
					{
						nPosItemX += itemInfo.rcItem.Width();
					}
				}

				// 画分隔线(采用拉伸模式)
				if(m_pImageSeperator != NULL)
				{
					// 使用拉伸模式属性画图
					graphics.DrawImage(m_pImageSeperator, RectF(0, (Gdiplus::REAL)((nVI+1)*m_nRowHeight), (Gdiplus::REAL)(nWidth-2), (Gdiplus::REAL)m_sizeSeperator.cy),
							0, 0, (Gdiplus::REAL)m_sizeSeperator.cx, (Gdiplus::REAL)m_sizeSeperator.cy, UnitPixel);
				}/*else
				{
					// 未指定图片,则画矩形
					graphics.FillRectangle(&solidBrushS, 0, (nVI+1)*m_nRowHeight, nWidth-2, 1);
				}*/
			}

			// 把不在显示范围内的单元格的控件都设置为不可见
			nRowIndex = 0;
			nFirstRowCount = 0;
			for(int i = 0; i < (int)m_vecRowInfo.size(); i++)
			{
				TreeNodeInfo &rowInfo = m_vecRowInfo.at(i);
				if(rowInfo.bHide)
				{
					continue;
				}
				BOOL bHideControl = TRUE;
				if(nFirstRowCount < m_nFirstViewRow)
				{
					nFirstRowCount++;
				}else
				{
					nRowIndex++;
					if(nRowIndex < nViewRowCount)
					{
						bHideControl = FALSE;
					}
				}

				// 显示区域之外的行的控件都隐藏
				if(bHideControl)
				{
					for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
					{
						TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
						for(size_t k = 0; k < itemInfo.vecControl.size(); k++)
						{
							CControlBase* pControl = itemInfo.vecControl.at(k);
							if(pControl)
							{
								pControl->SetVisible(FALSE);
							}
						}
					}
				}
			}
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, nWidth, m_rc.Height(), &m_memDC, 0, nYViewPos, SRCCOPY);//SRCAND);
}