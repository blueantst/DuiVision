#include "StdAfx.h"
#include "DuiTreeCtrl.h"

#define	SCROLL_V	1	// ��ֱ�������ؼ�ID
#define	SCROLL_H	2	// ˮƽ�������ؼ�ID

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
	m_clrRowHover = Color(0, 128, 128, 128);	// ����ƶ�������ʾ�ı���ɫ,Ĭ����͸��ɫ
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

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, CheckBox, 6)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, Collapse, 8)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTreeCtrl, Toggle, 6)

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiTreeCtrl::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

    // ʹ��XML�ڵ��ʼ��div�ؼ�
	if(pXmlElem != NULL)
	{
		InitUI(m_rc, pXmlElem);
	}

	// �����²��cloumn�ڵ���Ϣ
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

	// �����²��node�ڵ���Ϣ
	LoadNode(NULL, pXmlElem);

	// ˢ��������
	//RefreshNodeRows();

    return TRUE;
}

// ����XML���ڵ�
BOOL CDuiTreeCtrl::LoadNode(HTREEITEM hParentNode, DuiXmlNode pXmlElem)
{
	// �����²��Node�ڵ���Ϣ
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

		// ���ͼƬ,ͨ��Skin��ȡ
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
			// ͼƬ�ļ�
			strImage = strSkin;
		}else
		if(!strSkin.IsEmpty())
		{
			// ͼƬ����
			nImageIndex = _ttoi(strSkin);
		}

		// �ұ�ͼƬ,ͨ��Skin��ȡ
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
			// ͼƬ�ļ�
			strRightImage = strRightSkin;
		}else
		if(!strRightSkin.IsEmpty())
		{
			// ͼƬ����
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
		// �����²��item�ڵ���Ϣ
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

			// ͼƬ,ͨ��Skin��ȡ
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
				// ͼƬ�ļ�
				strImage = strSkin;
			}else
			if(!strSkin.IsEmpty())
			{
				// ͼƬ����
				nImageIndex = _ttoi(strSkin);
			}
			int nImageCount = _ttoi(strImageCount);

			BOOL bUseTitleFont = (strFontTitle == _T("1"));

			if(bShowCollapse)
			{
				// ��������ͼƬ��Ԫ��
				SetSubItemCollapse(hNode, nItemIndex, strImage, nImageCount);
			}else
			if(!strLink.IsEmpty())
			{
				// �������ӵ�Ԫ��
				SetSubItemLink(hNode, nItemIndex, strLink, strLinkAction, nImageIndex, clrText, strImage);
			}else
			{
				// ��ͨ��Ԫ��
				SetSubItem(hNode, nItemIndex, strTitle, strContent, bUseTitleFont, nImageIndex, clrText, strImage);
			}

			// �����²�Ŀؼ��ڵ���Ϣ
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
						// ���ؼ�ָ����ӵ�gridctrl�ؼ����ӿؼ��б���
						m_vecControl.push_back(pControl);
						// ���ؼ�ָ����ӵ���Ԫ��Ŀؼ��б���(�����ڰ��յ�Ԫ������ӿؼ�)
						pItemInfo->vecControl.push_back(pControl);
					}
				}
			}

			nItemIndex++;
		}

		// �����ӽڵ�
		LoadNode(hNode, pNodeElem);
	}

	return TRUE;
}

// �����
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

// ������ڵ�
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

	// ���ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, nodeInfo.pImage))
		{
			nodeInfo.sizeImage.SetSize(nodeInfo.pImage->GetWidth() / 1, nodeInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		nodeInfo.pImage = NULL;
		if((nodeInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			nodeInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	// �ұ�ͼƬ
	if(!strRightImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strRightImage, m_bImageUseECM, nodeInfo.pRightImage))
		{
			nodeInfo.sizeRightImage.SetSize(nodeInfo.pRightImage->GetWidth() / 1, nodeInfo.pRightImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
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

// ������ڵ�
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

	// ˢ�������е�λ��
	RefreshNodeRows();

	return pNodeInfo->hNode;
}

// ���ñ��������(���ֱ����)
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

	// ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// ���ñ��������(���ӱ����)
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

	// ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage))
		{
			itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / 1, itemInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		itemInfo.pImage = NULL;
		if((itemInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			itemInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return TRUE;
}

// ���ñ����Ϊ����ͼƬ��ʾ
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

	// ����ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
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

// �����ڵ㵥Ԫ������ӿؼ�
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
		// ���ؼ�ָ����ӵ�treectrl�ؼ����ӿؼ��б���
		m_vecControl.push_back(pControl);
		// ���ؼ�ָ����ӵ����ڵ㵥Ԫ��Ŀؼ��б���(�����ڰ��յ�Ԫ������ӿؼ�)
		pItemInfo->vecControl.push_back(pControl);
		UpdateControl(true);
	}

	return TRUE;
}

// ɾ�����ڵ㵥Ԫ���ָ���ӿؼ�(ͨ���ؼ�����ָ��ɾ��)
BOOL CDuiTreeCtrl::DeleteSubItemControl(CControlBase* pControl)
{
	// �������е�Ԫ��,ɾ����Ӧ�Ŀؼ���������
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

	// ɾ���ӿؼ��ж�Ӧ�Ŀؼ�����
	RemoveControl(pControl);

	return TRUE;
}

// ɾ�����ڵ㵥Ԫ���ָ���ӿؼ�(ͨ���ؼ����Ϳؼ�IDɾ��)
BOOL CDuiTreeCtrl::DeleteSubItemControl(CString strControlName, UINT uControlID)
{
	// �������е�Ԫ��,ɾ����Ӧ�Ŀؼ���������
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

	// ɾ���ӿؼ��ж�Ӧ�Ŀؼ�����
	RemoveControl(strControlName, uControlID);

	return TRUE;
}

// ɾ���ڵ�
BOOL CDuiTreeCtrl::DeleteNode(HTREEITEM hNode)
{
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}

	// �����е����е�Ԫ��,ɾ����Ӧ���ӿؼ�����
	TreeNodeInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
	{
		TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
		vector<CControlBase*>::iterator it;
		for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
		{
			CControlBase* _pControl = *it;
			// ɾ����Ԫ��������ӿؼ�
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

	// ���¼��������е�λ��
	RefreshNodeRows();

	return true;
}

// ��ȡ�ڵ���к�
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

// ��ȡ�ڵ�����һ���ӽڵ��к�(��������ײ���ӽڵ�)
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

// �ж�һ���ڵ��Ƿ����ӽڵ�
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

// ��ȡ���ڵ���
HTREEITEM CDuiTreeCtrl::GetParentNode(HTREEITEM hNode)
{
	TreeNodeInfo* pNodeInfo = GetNodeInfo(hNode);
	if(pNodeInfo)
	{
		return pNodeInfo->hParentNode;
	}

	return NULL;
}

// ��ȡ��һ���ӽڵ���
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

// ��ȡ��һ���ӽڵ���
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

// ��ȡǰһ���ӽڵ���
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

// ��ȡĳ���ڵ���ӽڵ����
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

// ��ȡһ���ڵ�Ĳ㼶
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

// ���ݽڵ�ID��ȡ�ڵ�ľ��
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

// ��ȡĳһ������Ϣ
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

// ��ȡĳһ����Ԫ����Ϣ
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

// ����ĳһ����Ԫ����Ϣ
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

// ����ĳһ���е�������ɫ
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

// ����ĳһ���еı�����ɫ
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

// �л��ڵ������״̬
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

// �л��ڵ������״̬
void CDuiTreeCtrl::ExpandNode(HTREEITEM hNode, BOOL bExpand)
{
	// չ�����ڵ�
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

	// չ���������ӽڵ�
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

// ����ĳһ���ڵ�ļ���״̬
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

// ��ȡĳһ���ڵ�ļ���״̬
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

// ������ڵ�
void CDuiTreeCtrl::ClearNodes()
{
	// ɾ�������ӿؼ�
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

// �����ӽڵ�
void CDuiTreeCtrl::HideChildNodes(HTREEITEM hItem)
{
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		TreeNodeInfo &rowInfoTemp = m_vecRowInfo.at(i);
		if(rowInfoTemp.hParentNode == hItem)
		{
			rowInfoTemp.bHide = TRUE;
			// ���ش˽ڵ�����е�Ԫ���ӿؼ�
			for(size_t j = 0; j < rowInfoTemp.vecItemInfo.size(); j++)
			{
				TreeItemInfo &itemInfo = rowInfoTemp.vecItemInfo.at(j);
				for(size_t k = 0; k < itemInfo.vecControl.size(); k++)
				{
					CControlBase* pControl = itemInfo.vecControl.at(k);
					pControl->SetVisible(FALSE);
				}
			}
			// �����¼��ڵ�
			HideChildNodes(rowInfoTemp.hNode);
		}
	}
}

// ˢ�����нڵ���
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

	// ���¼��������е�λ��
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

		// ������λ��
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

		// �������λ��
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

		// ����ÿ����Ԫ��λ��
		for(int j = 0; j < (int)rowInfoTemp.vecItemInfo.size(); j++)
		{
			TreeItemInfo &itemInfo = rowInfoTemp.vecItemInfo.at(j);
			itemInfo.rcItem.top = rowInfoTemp.rcRow.top;
			itemInfo.rcItem.bottom = rowInfoTemp.rcRow.bottom;
			// ��һ����Ԫ�����λ��Ҫ�ų�����ͼƬ�Ŀ��
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

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((nVisibleRows * m_nRowHeight) > m_rc.Height());
	((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange(nVisibleRows * m_nRowHeight);

	UpdateControl(true);
}

// ��ָ���Ľڵ�������ɼ���Χ
BOOL CDuiTreeCtrl::EnsureVisible(HTREEITEM hNode, BOOL bPartialOK)
{
	// ����ڵ�δչ��,������չ���ڵ�
	ExpandNode(hNode, TRUE);

	// ����ڵ��Ӧ���к�
	int nRow = GetNodeRow(hNode);
	if(nRow == -1)
	{
		return FALSE;
	}

	int nViewRowCount = m_rc.Height() / m_nRowHeight;

	// ���ָ�������Ѿ����ڿɼ���Χ��ֱ�ӷ���
	if((nRow >= m_nFirstViewRow) && (nRow < (m_nFirstViewRow + nViewRowCount)))
	{
		return TRUE;
	}

	// �������ɼ���Χ
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

// ��XML����Font-title����
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

	// ���¼��������е�λ��
	RefreshNodeRows();
}

// �ж�ָ��������λ���Ƿ���ĳһ����
BOOL CDuiTreeCtrl::PtInRow(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return FALSE;
	}

	CRect rc = rowInfo.rcRow;
	// rcRow�����ǲ����нڵ�ʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�еļ�����
BOOL CDuiTreeCtrl::PtInRowCheck(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return FALSE;
	}

	CRect rc = rowInfo.rcCheck;
	// rcCheck�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�����ͼƬλ��
BOOL CDuiTreeCtrl::PtInRowCollapse(CPoint point, TreeNodeInfo& rowInfo)
{
	if(rowInfo.bHide)
	{
		return FALSE;
	}

	// �жϽڵ������ͼƬλ��
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

	// �ж�����Ƿ�����ʾ����ͼƬ�ĵ�Ԫ����
	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		TreeItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		if(!itemInfo.bShowCollapse)
		{
			continue;
		}

		CRect rc = itemInfo.rcItem;
		// rcItem�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
		rc.OffsetRect(m_rc.left + m_nLeftPos, m_rc.top-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return TRUE;
		}
	}

	return FALSE;
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�ĳ������,����������
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
		// rcItem�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
		rc.OffsetRect(m_rc.left + m_nLeftPos, m_rc.top-m_nVirtualTop);
		if(rc.PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

// ���õ�Ԫ���Tooltip
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

// ���Tooltip
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

// ����ƶ��¼�����
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

				if(m_bGridTooltip)	// ���õ�Ԫ��Tooltip
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

				if(m_bGridTooltip)	// ���õ�Ԫ��Tooltip
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

// �����������¼�����
BOOL CDuiTreeCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo) && !PtInRowCollapse(point, rowInfo))	// ����������¼�ֻ�����ſ�ʱ�򴥷�
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
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(MSG_BUTTON_DOWN, rowInfo.hNode, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

// �������ſ��¼�����
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
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_CHECK, rowInfo.hNode, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}else
			if(PtInRowCollapse(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.bCollapse = !rowInfo.bCollapse;
				RefreshNodeRows();

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		TreeNodeInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_CHECK, rowInfo.hNode, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}else
			if(PtInRowCollapse(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.bCollapse = !rowInfo.bCollapse;
				RefreshNodeRows();

				return true;
			}
		}
	}

	return false;
}

// ������˫���¼�����
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

// �����¼�����
BOOL CDuiTreeCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if((m_nVisibleRowCount * m_nRowHeight) <= m_rc.Height())
	{
		return false;
	}

	// ���¹�����,��ˢ�½���
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	if(pScrollV->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
	}

	return true;
}

// ��Ϣ��Ӧ
LRESULT CDuiTreeCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// ����ǹ�������λ�ñ���¼�,��ˢ����ʾ
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == MSG_BUTTON_DOWN) && (lParam != -1))
	{
		// ������е�ĳ������
		TreeNodeInfo* pRowInfo = GetNodeInfo(wParam);
		if(pRowInfo && (lParam >= 0) && (lParam < (int)pRowInfo->vecItemInfo.size()))
		{
			TreeItemInfo &itemInfo = pRowInfo->vecItemInfo.at(lParam);
			// ת��ΪMSG_BUTTON_UP��Ϣ,��ΪDuiSystem������ʱ��ֻ����MSG_BUTTON_UP��Ϣ
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
	// �б�ͼ����:
	// 1.�б������߶�Ϊÿһ�и߶�*����
	// 2.�б���ʾ��top������scroll�ؼ���¼
	// 3.�ػ�ʱ��,����top����λ�ü������ʾ�ĵ�һ�е����,������ʾ�߶ȼ������ʾ�����һ�е����
	// 4.���ݼ��������ʾ����,����Ӧ�����ݵ��ڴ�dc��
	// 5.�������ʾ��top��������ڴ�dc�Ŀ���
	int nWidth = m_rc.Width() - m_nScrollWidth;	// ��ȥ�������Ŀ��
	int nHeightAll = m_nVisibleRowCount*m_nRowHeight; // �ܵ�����߶� //m_rc.Height();
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();

	m_nVirtualTop = (nMaxRange > 0) ? (int)((double)nCurPos*(nHeightAll-m_rc.Height())/nMaxRange) : 0;	// ��ǰ������λ�ö�Ӧ�������topλ��
	if(m_nVirtualTop < 0)
	{
		m_nVirtualTop = 0;
		pScrollV->SetScrollCurrentPos(0);
	}
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;			// ��ʾ�ĵ�һ�����
	int nViewRowCount = m_rc.Height() / m_nRowHeight + 1;	// ��ʾ������
	int nHeightView = nViewRowCount * m_nRowHeight;	// ��ʾ�漰��������߶�
	int nYViewPos = m_nVirtualTop - (m_nFirstViewRow * m_nRowHeight);		// �ڴ�dc��ʾ����Ļʱ���topλ��
	if(nYViewPos < 0)
	{
		nYViewPos = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeightView);

		Graphics graphics(m_memDC);
		
		m_memDC.BitBlt(0, 0, nWidth, nHeightView, &dc, m_rc.left, m_rc.top, WHITENESS);	// ����ɫ����
		DrawVerticalTransition(m_memDC, dc, CRect(0, nYViewPos, nWidth, m_rc.Height()+nYViewPos),	// ����͸����
				m_rc, m_nBkTransparent, m_nBkTransparent);
		
		BSTR bsFontTitle = m_strFontTitle.AllocSysString();
		FontFamily fontFamilyTitle(bsFontTitle);
		Font fontTitle(&fontFamilyTitle, (REAL)m_nFontTitleWidth, m_fontTitleStyle, UnitPixel);
		::SysFreeString(bsFontTitle);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		SolidBrush solidBrush(m_clrText);			// �������ֻ�ˢ
		SolidBrush solidBrushH(m_clrTextHover);		// �ȵ����ֻ�ˢ
		SolidBrush solidBrushD(m_clrTextDown);		// ��ǰ�л�ˢ
		SolidBrush solidBrushT(m_clrTitle);			// �������ֻ�ˢ
		SolidBrush solidBrushS(m_clrSeperator);		// �ָ��߻�ˢ

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// ������ͨ���ֵ�ˮƽ�ʹ�ֱ���뷽ʽ
		DUI_STRING_ALIGN_DEFINE();

		strFormat.SetTrimming(StringTrimmingEllipsisWord);	// �Ե���Ϊ��λȥβ,��ȥ����ʹ��ʡ�Ժ�
		//strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		if(!m_bTextWrap)
		{
			strFormat.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}

		// �����ֶβ����м����
		StringFormat strFormatHeader;
		strFormatHeader.SetAlignment(StringAlignmentCenter);	// �м����
		strFormatHeader.SetLineAlignment(StringAlignmentCenter);	// �м����
		if(!m_bTextWrap)
		{
			strFormatHeader.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
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

				SolidBrush solidBrushRow(rowInfo.clrText);	// �ж������ɫ

				int nXPos = 0;
				int nVI = nRowIndex;//i - m_nFirstViewRow;
				nRowIndex++;

				// �����ڵ�����ͼƬ(toggle)
				if(m_pImageToggle != NULL)
				{
					int nNodeLevel = GetNodeLevel(rowInfo.hNode);
					nXPos += (nNodeLevel * m_sizeToggle.cx);
					if(HaveChildNode(rowInfo.hNode))
					{
						int nToggleImgY = (m_nRowHeight - m_sizeToggle.cy) / 2;
						int nToggleIndex = (m_nHoverRow == i) ? 1 : 0;
						graphics.DrawImage(m_pImageToggle, Rect(nXPos, nVI*m_nRowHeight + nToggleImgY, m_sizeToggle.cx, m_sizeToggle.cy),
							rowInfo.bCollapse ? nToggleIndex*m_sizeToggle.cx : (3+nToggleIndex)*m_sizeToggle.cx, 0, m_sizeToggle.cx, m_sizeToggle.cy, UnitPixel);
						nXPos += m_sizeToggle.cx;
					}
				}

				// ����ƶ�����ʱ����ʾ�ı�����ɫ(�������Ϊȫ0,����ʾ�б�����ɫ)
				if((m_nHoverRow == i) && (m_clrRowHover.GetValue() != Color(0, 0, 0, 0).GetValue()))
				{
					SolidBrush brush(m_clrRowHover);
					graphics.FillRectangle(&brush, nXPos, nVI*m_nRowHeight, nWidth-nXPos, m_nRowHeight);
				}else
				if(rowInfo.bRowBackColor)	// ����������еı�����ɫ,�������ɫ
				{
					SolidBrush brush(rowInfo.clrBack);
					graphics.FillRectangle(&brush, nXPos, nVI*m_nRowHeight, nWidth-nXPos, m_nRowHeight);
				}

				// ������
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

				// �������ͼƬ
				int nImgY = 3;
				if(rowInfo.pImage != NULL)
				{
					if((rowInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
					}
					// ʹ��������ָ����ͼƬ
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
					// ʹ������ͼƬ
					graphics.DrawImage(m_pImage, Rect(nXPos, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nXPos += (m_sizeImage.cx + 3);
				}

				// �����ұ�ͼƬ
				int nRightImageWidth = 0;
				nImgY = 3;
				if(rowInfo.pRightImage != NULL)
				{
					if((rowInfo.sizeRightImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeRightImage.cy) / 2 + 1;
					}
					// ʹ��������ָ����ͼƬ
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
					// ʹ������ͼƬ
					graphics.DrawImage(m_pImage, Rect(nWidth-m_sizeImage.cx-1, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nRightImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nRightImageWidth = m_sizeImage.cx + 1;
				}

				// ����Ԫ������
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
						// ���һ����Ҫ��ȥ���������
						rect.Width -= m_nScrollWidth;
					}

					// ����Ԫ��ͼƬ
					int nItemImageX = 0;
					int nImgY = 3;
					if((itemInfo.pImage != NULL) && (itemInfo.nImageCount <= 1))
					{
						if((itemInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - itemInfo.sizeImage.cy) / 2 + 1;
						}
						// ʹ�õ�Ԫ��ָ����ͼƬ
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
						// ʹ������ͼƬ
						graphics.DrawImage(m_pImage, Rect(nPosItemX+nItemImageX, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
							itemInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
						nItemImageX += (m_sizeImage.cx + 3);
					}
					rect.Offset((Gdiplus::REAL)nItemImageX, 0);
					rect.Width -= (Gdiplus::REAL)nItemImageX;

					// ������ͼƬ
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

					// ����Ԫ��������������
					SolidBrush solidBrushItem(m_clrText);
					if((m_nHoverRow == i) && (m_clrTextHover.GetValue() != Color(0, 0, 0, 0).GetValue()))	// ����������ƶ���ɫ,��ʹ��
					{
						solidBrushItem.SetColor(m_clrTextHover);
					}else
					if((m_nDownRow == i) && (m_clrTextDown.GetValue() != Color(0, 0, 0, 0).GetValue()))	// ��������갴����ɫ,��ʹ��
					{
						solidBrushItem.SetColor(m_clrTextDown);
					}else
					if(itemInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue())	// �����˵�Ԫ����ɫ,��ʹ��
					{
						solidBrushItem.SetColor(itemInfo.clrText);
					}else
					if(rowInfo.clrText.GetValue() != Color(0, 0, 0, 0).GetValue())	// ����������ɫ,��ʹ��
					{
						solidBrushItem.SetColor(rowInfo.clrText);
					}
					CString strItemTitle = itemInfo.strTitle;
					// �����Ƿ���Ҫ��ʾtip
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
					// ����bUseTitleFont�����ñ������廹����ͨ����
					BSTR bsItemTitle = strItemTitle.AllocSysString();
					graphics.DrawString(bsItemTitle, (INT)wcslen(bsItemTitle),
						itemInfo.bUseTitleFont ? &fontTitle : &font, rect, &strFormat, itemInfo.bUseTitleFont ? &solidBrushT : &solidBrushItem);
					::SysFreeString(bsItemTitle);

					// ����Ԫ������
					if(!bSingleLine)
					{
						rect.Offset(0, (Gdiplus::REAL)(m_nRowHeight / 2 + 2));
						rect.Height = (Gdiplus::REAL)(m_nRowHeight / 2 - 4);
						BSTR bsItemContent = itemInfo.strContent.AllocSysString();
						graphics.DrawString(bsItemContent, (INT)wcslen(bsItemContent), &font, rect, &strFormat, &solidBrushItem);
						::SysFreeString(bsItemContent);
					}

					// ���õ�Ԫ���ӿؼ���λ��
					for(size_t k = 0; k < itemInfo.vecControl.size(); k++)
					{
						CControlBase* pControl = itemInfo.vecControl.at(k);
						if(pControl)
						{
							CRect rcParent = CRect(nPosItemX, nVI*m_nRowHeight + 1,
								(int)(rect.X+rect.Width), (nVI+1)*m_nRowHeight - 1);
							if((int)(rect.GetRight()) > nWidth)
							{
								// ���һ����Ҫ��ȥ���������
								rcParent.right -= m_nScrollWidth;
							}
							rcParent.OffsetRect(m_rc.left, m_rc.top - nYViewPos);
							pControl->SetPositionWithParent(rcParent);
							CRect rcControl = pControl->GetRect();
							// ֻ�е�ǰ����ʾ��Χ�ڵĿؼ�����Ϊ�ɼ�
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
						// Ϊ��ʹ�ڶ��п�ʼ�Ƕ����,���Եڶ��п�ʼλ�ð��յ�һ�еĿ�ȼ���
						nPosItemX = itemInfo.rcItem.right;
					}else
					{
						nPosItemX += itemInfo.rcItem.Width();
					}
				}

				// ���ָ���(��������ģʽ)
				if(m_pImageSeperator != NULL)
				{
					// ʹ������ģʽ���Ի�ͼ
					graphics.DrawImage(m_pImageSeperator, RectF(0, (Gdiplus::REAL)((nVI+1)*m_nRowHeight), (Gdiplus::REAL)(nWidth-2), (Gdiplus::REAL)m_sizeSeperator.cy),
							0, 0, (Gdiplus::REAL)m_sizeSeperator.cx, (Gdiplus::REAL)m_sizeSeperator.cy, UnitPixel);
				}/*else
				{
					// δָ��ͼƬ,�򻭾���
					graphics.FillRectangle(&solidBrushS, 0, (nVI+1)*m_nRowHeight, nWidth-2, 1);
				}*/
			}

			// �Ѳ�����ʾ��Χ�ڵĵ�Ԫ��Ŀؼ�������Ϊ���ɼ�
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

				// ��ʾ����֮����еĿؼ�������
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