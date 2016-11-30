#include "StdAfx.h"
#include "DuiTab.h"

CDuiTabCtrl::CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBaseFont(hWnd, pDuiObject)
{
	m_nTabType = TAB_TYPE_HORIZONTAL;
	m_pImageSeperator = NULL;
	m_pImageHover = NULL;
	m_pImageTabBtn = NULL;
	m_nHoverItem = 0;
	m_nDownItem = 0;
	m_nOldItem = -1;
	m_nTabItemWidth = 0;
	m_nTabItemMaxWidth = 0;
	m_nTabItemMinWidth = 0;
	m_nTabCtrlHeight = 0;
	m_nTabItemHeight = 0;
	m_nTabItemMaxHeight = 0;
	m_nTabItemMinHeight = 0;
	m_nTabCtrlWidth = 0;
	m_uAlignment = Align_Center;
	m_uVAlignment = VAlign_Middle;
	m_clrText = Color(225, 255, 255, 255);
	m_clrTextHover = Color(0, 0, 0, 0);
	m_clrTextDown = Color(0, 0, 0, 0);
	m_bAnimateChangeTab = FALSE;
	m_nAnimateCount = 10;
	m_nCurXPos = 0;
	m_nCurYPos = 0;
	m_nTabLeftPading = 0;
	m_nTabRightPading = 0;
	m_nTabTopPading = 0;
	m_nTabBottomPading = 0;
	m_posTabBtn.nCount = 0;
	m_enTabImageMode = enTIMNormal;
	SetBitmapCount(3);	// tab页签图片默认是3种状态的图片
	m_nWLT = 0;
	m_nHLT = 0;
	m_nWRB = 0;
	m_nHRB = 0;
	m_bTabTooltip = TRUE;
	m_nTipItem = -1;
	m_bInit = FALSE;
}

CDuiTabCtrl::CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle/*= TEXT("")*/, BOOL bIsVisible/* = TRUE*/, 
						   BOOL bIsDisable/* = FALSE*/ ,BOOL bIsPressDown/* = FALSE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, strTitle, bIsVisible, bIsDisable)
{
	m_nTabType = TAB_TYPE_HORIZONTAL;
	m_pImageSeperator = NULL;
	m_pImageHover = NULL;
	m_pImageTabBtn = NULL;
	m_nHoverItem = 0;
	m_nDownItem = 0;
	m_nOldItem = -1;
	m_nTabItemWidth = 0;
	m_nTabItemMaxWidth = 0;
	m_nTabItemMinWidth = 0;
	m_nTabCtrlHeight = 0;
	m_nTabItemHeight = 0;
	m_nTabItemMaxHeight = 0;
	m_nTabItemMinHeight = 0;
	m_nTabCtrlWidth = 0;
	m_uAlignment = Align_Center;
	m_uVAlignment = VAlign_Middle;
	m_clrText = Color(225, 255, 255, 255);
	m_clrTextHover = Color(0, 0, 0, 0);
	m_clrTextDown = Color(0, 0, 0, 0);
	m_bAnimateChangeTab = FALSE;
	m_nAnimateCount = 10;
	m_nCurXPos = 0;
	m_nCurYPos = 0;
	m_nTabLeftPading = 0;
	m_nTabRightPading = 0;
	m_nTabTopPading = 0;
	m_nTabBottomPading = 0;
	m_posTabBtn.nCount = 0;
	m_enTabImageMode = enTIMNormal;
	SetBitmapCount(3);	// tab页签图片默认是3种状态的图片
	m_nWLT = 0;
	m_nHLT = 0;
	m_nWRB = 0;
	m_nHRB = 0;
	m_bTabTooltip = TRUE;
	m_nTipItem = -1;
	m_bInit = FALSE;
}

CDuiTabCtrl::~CDuiTabCtrl(void)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}

	if(m_pImageHover != NULL)
	{
		delete m_pImageHover;
		m_pImageHover = NULL;
	}

	if(m_pImageTabBtn != NULL)
	{
		delete m_pImageTabBtn;
		m_pImageTabBtn = NULL;
	}

	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
		if(itemInfoTemp.pImage != NULL)
		{
			delete itemInfoTemp.pImage;
			itemInfoTemp.pImage = NULL;
		}
	}
}

// 图片属性的实现
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTabCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTabCtrl, Hover, 2)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTabCtrl, TabBtn, 4)

// 根据控件名创建控件实例
CControlBase* CDuiTabCtrl::_CreateControlByName(LPCTSTR lpszName)
{
	// 如果已经设置了窗口句柄,使用窗口句柄创建控件
	HWND hWnd = m_hWnd;
	if(hWnd != NULL)
	{
		return DuiSystem::CreateControlByName(lpszName, hWnd, this);
	}

	// 查找父对话框的窗口句柄,通过父对话框句柄创建控件
	CDuiObject* pParentObj = GetParent();
	while((pParentObj != NULL) && (!pParentObj->IsClass(_T("dlg"))) && (!pParentObj->IsClass(_T("popup"))))
	{
		pParentObj = ((CControlBase*)pParentObj)->GetParent();
	}

	if((pParentObj != NULL) && pParentObj->IsClass(_T("dlg")))
	{
		return DuiSystem::CreateControlByName(lpszName, ((CDlgBase*)pParentObj)->GetSafeHwnd(), this);
	}else
	if((pParentObj != NULL) && pParentObj->IsClass(_T("popup")))
	{
		return DuiSystem::CreateControlByName(lpszName, ((CDlgPopup*)pParentObj)->GetSafeHwnd(), this);
	}

	return NULL;
}

// 重载加载XML节点函数，加载下层的tab页面内容
BOOL CDuiTabCtrl::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	__super::Load(pXmlElem);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	// 如果没有设置tabctrl高度/宽度,则按照hover图片的高度/宽度
	if((m_nTabType == TAB_TYPE_HORIZONTAL) && (m_pImageHover != NULL) && (m_nTabCtrlHeight == 0))
	{
		m_nTabCtrlHeight = m_sizeHover.cy;
	}else
	if((m_nTabType == TAB_TYPE_VERTICAL) && (m_pImageHover != NULL) && (m_nTabCtrlWidth == 0))
	{
		m_nTabCtrlWidth = m_sizeHover.cx;
	}

	m_nTabItemWidth = m_nTabItemMaxWidth;
	m_nTabItemHeight = m_nTabItemMaxHeight;
	// 如果没有设置tab项的宽度,则按照hover图片的宽度
	if((m_nTabType == TAB_TYPE_HORIZONTAL) && (m_pImageHover != NULL) && (m_nTabItemWidth == 0))
	{
		m_nTabItemWidth = m_sizeHover.cx;
	}else
	if((m_nTabType == TAB_TYPE_VERTICAL) && (m_pImageHover != NULL) && (m_nTabItemHeight == 0))
	{
		m_nTabItemHeight = m_sizeHover.cy;
	}

	BOOL bAllVisible = TRUE;
	
	// 加载下层的tab页节点信息
	int nIdIndex = m_vecItemInfo.size();
	for (DuiXmlNode pTabElem = pXmlElem.child(_T("tab")); pTabElem; pTabElem=pTabElem.next_sibling(_T("tab")))
	{
		CString strId = pTabElem.attribute(_T("id")).value();
		int nId = nIdIndex;
		if(strId != _T(""))
		{
			nId = _ttoi(strId);
		}

		CString strName = pTabElem.attribute(_T("name")).value();
		if(GetItemIndex(strName) != -1)
		{
			// 如果已经存在相同名字的tab页,则跳过
			continue;
		}

		CString strTabXml = pTabElem.attribute(_T("tabxml")).value();
		if(!strTabXml.IsEmpty())
		{
			// 从xml文件加载嵌套的tab页
			LoadTabXml(strTabXml);
			nIdIndex = m_vecItemInfo.size();
			continue;
		}

		CString strAction = pTabElem.attribute(_T("action")).value();
		CString strOutLink = pTabElem.attribute(_T("outlink")).value();
		BOOL bOutLink = ((strOutLink == _T("1")) || (strOutLink == _T("true")));
		CString strImage = pTabElem.attribute(_T("image")).value();
		CString strImageIndex = pTabElem.attribute(_T("img-index")).value();
		int nImageIndex = -1;
		if(!strImageIndex.IsEmpty())
		{
			nImageIndex = _ttoi(strImageIndex);
		}
		CString strImageCount = pTabElem.attribute(_T("img-count")).value();
		int nImageCount = -1;
		if(!strImageCount.IsEmpty())
		{
			nImageCount = _ttoi(strImageCount);
		}
		// visible属性可以用visible或show
		CString strVisible = pTabElem.attribute(_T("visible")).value();
		if(strVisible.IsEmpty())
		{
			strVisible = pTabElem.attribute(_T("show")).value();
		}
		BOOL bVisible = ((strVisible == _T("1")) || (strVisible == _T("true")) || (strVisible == _T("")));
		CString strActive = pTabElem.attribute(_T("active")).value();
		CString strDivXml = pTabElem.attribute(_T("div")).value();

		CString strScroll = pTabElem.attribute(_T("scroll")).value();
		BOOL bEnableScroll = (strScroll == _T("1"));

		// 加载Panel控件，每个Tab页都会自动创建一个Panel控件，即使没有加载子XML节点
		CDuiPanel* pControlPanel = (CDuiPanel*)_CreateControlByName(_T("div"));
		pControlPanel->SetName(strName);	// div控件的名字设置为tab的名字
		pControlPanel->SetEnableScroll(bEnableScroll);
		m_vecControl.push_back(pControlPanel);
		if(!strDivXml.IsEmpty())
		{
 			pControlPanel->LoadXmlFile(strDivXml);			
		}

		// 加载XML中Tab节点的各个下层控件节点
		pControlPanel->Load(pTabElem);

		CString strTitle = pControlPanel->GetTitle();
		
		// 通过Skin读取
		CString strSkin = _T("");
		if(strImage.Find(_T("skin:")) == 0)
		{
			strSkin = DuiSystem::Instance()->GetSkin(strImage);
		}else
		{
			strSkin = strImage;
		}

		if(strSkin.Find(_T(".")) != -1)	// 加载图片文件
		{
			CString strImgFile = strSkin;
			if(strSkin.Find(_T(":")) != -1)
			{
				strImgFile = strSkin;
			}
			InsertItem(-1, nId, strName, strTitle, strAction, strImgFile, pControlPanel, nImageCount, bOutLink);
		}else
		if(nImageIndex != -1)	// 索引图片
		{
			InsertItem(-1, nId, strName, strTitle, strAction, nImageIndex, pControlPanel, bOutLink);
		}else
		if(!strSkin.IsEmpty())	// 图片资源
		{
			UINT uResourceID = _ttoi(strSkin);
			InsertItem(-1, nId, strName, strTitle, strAction, uResourceID, pControlPanel, nImageCount, bOutLink);
		}else
		if(strSkin.IsEmpty())	// 图片为空
		{
			InsertItem(-1, nId, strName, strTitle, strAction, _T(""), pControlPanel, nImageCount, bOutLink);
		}

		TabItemInfo &itemInfo = m_vecItemInfo.at(nIdIndex);
		itemInfo.bVisible = bVisible;
		if(!bVisible)
		{
			bAllVisible = FALSE;
		}

		if(strActive == _T("true"))	// 设置为当前活动的页面
		{
			m_nHoverItem = nIdIndex;
			m_nDownItem = nIdIndex;
		}
		if(!bVisible && (m_nDownItem == nIdIndex))
		{
			m_nDownItem++;
			m_nHoverItem = m_nDownItem;
		}

		nIdIndex++;
	}

	// 如果是用于加载嵌套tab页定义,则不需要下面的流程
	if(!bLoadSubControl)
	{
		return TRUE;
	}

	// 只显示当前活动的tab页对应的Panel对象，其他页面的Panel对象都隐藏
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(i);
		if(itemInfo.pControl != NULL)
		{
			if(i == m_nDownItem)
			{
				itemInfo.pControl->SetVisible(TRUE);
			}else
			{
				itemInfo.pControl->SetVisible(FALSE);
			}
		}
	}

	// 如果有页面是隐藏的,则需要刷新所有页面
	if(!bAllVisible)
	{
		RefreshItems();
	}

	m_bInit = TRUE;

    return TRUE;
}

// 从嵌套的xml文件中加载tab页
BOOL CDuiTabCtrl::LoadTabXml(CString strFileName)
{
	// 首先将所有tab页都隐藏
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(i);
		if(itemInfo.pControl != NULL)
		{
			itemInfo.pControl->SetVisible(FALSE);
		}
	}

	DuiXmlDocument xmlDoc;
	DuiXmlNode pTabElem;

	if(DuiSystem::Instance()->LoadXmlFile(xmlDoc, strFileName))
	{
		pTabElem = xmlDoc.child(GetClassName());
		if(pTabElem != NULL)
		{
			// 加载下层tab页
			Load(pTabElem, FALSE);
		}
	}

	// 只显示当前活动的tab页对应的Panel对象，其他页面的Panel对象都隐藏
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(i);
		if(itemInfo.pControl != NULL)
		{
			if(i == m_nDownItem)
			{
				itemInfo.pControl->SetVisible(TRUE);
			}else
			{
				itemInfo.pControl->SetVisible(FALSE);
			}
		}
	}

	// 重新计算每个tab页的位置,并刷新界面
	RefreshItems();

	return TRUE;
}

// 增加tab页(使用资源图片)
BOOL CDuiTabCtrl::InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, UINT nResourceID, CControlBase* pControl, int nImageCount, BOOL bOutLink, int nItemWidth/* = 0*/, CString strType/*= TEXT("PNG")*/)
{
	TabItemInfo itemInfo;
	itemInfo.bVisible = TRUE;
	itemInfo.nItemID = nItemID;
	itemInfo.strName = strName;
	itemInfo.strText = strItemText;
	itemInfo.bNeedTextTip = FALSE;
	itemInfo.strAction = strAction;
	itemInfo.bOutLink = bOutLink;
	itemInfo.sizeImage.SetSize(0, 0);
	itemInfo.nImageCount = 3;
	if(nImageCount != -1)
	{
		itemInfo.nImageCount = nImageCount;
	}
	itemInfo.nImageIndex = -1;

	if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, itemInfo.pImage))
	{
		itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / itemInfo.nImageCount, itemInfo.pImage->GetHeight());
	}

	itemInfo.rc.SetRect(m_rc.left, m_rc.top, m_rc.left + (nItemWidth == 0 ? itemInfo.sizeImage.cx : nItemWidth), m_rc.bottom);

	itemInfo.pControl = pControl;

	return InsertItem(nItem, itemInfo);
}

// 增加tab页(使用图片文件)
BOOL CDuiTabCtrl::InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, CString strImage, CControlBase* pControl, int nImageCount, BOOL bOutLink, int nItemWidth/* = 0*/)
{
	TabItemInfo itemInfo;
	itemInfo.bVisible = TRUE;
	itemInfo.nItemID = nItemID;
	itemInfo.strName = strName;
	itemInfo.strText = strItemText;
	itemInfo.bNeedTextTip = FALSE;
	itemInfo.strAction = strAction;
	itemInfo.bOutLink = bOutLink;
	itemInfo.sizeImage.SetSize(0, 0);
	itemInfo.nImageCount = 3;
	if(nImageCount != -1)
	{
		itemInfo.nImageCount = nImageCount;
	}
	itemInfo.nImageIndex = -1;

	BOOL bLoadImageOk = FALSE;
	if(strImage.IsEmpty())
	{
		itemInfo.pImage = NULL;
	}else
	{
		bLoadImageOk = DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, itemInfo.pImage);
	}

	if(itemInfo.pImage && bLoadImageOk)
	{
		itemInfo.sizeImage.SetSize(itemInfo.pImage->GetWidth() / itemInfo.nImageCount, itemInfo.pImage->GetHeight());
	}

	int nWidth = (nItemWidth == 0 ? itemInfo.sizeImage.cx : nItemWidth);
	if(nWidth == 0)
	{
		nWidth = m_sizeHover.cx;
	}
	itemInfo.rc.SetRect(m_rc.left, m_rc.top, m_rc.left + nWidth, m_rc.bottom);

	itemInfo.pControl = pControl;

	return InsertItem(nItem, itemInfo);
}

// 增加tab页(索引图片方式)
BOOL CDuiTabCtrl::InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, int nImageIndex, CControlBase* pControl, BOOL bOutLink, int nItemWidth/* = 0*/)
{
	TabItemInfo itemInfo;
	itemInfo.bVisible = TRUE;
	itemInfo.nItemID = nItemID;
	itemInfo.strName = strName;
	itemInfo.strText = strItemText;
	itemInfo.bNeedTextTip = FALSE;
	itemInfo.strAction = strAction;
	itemInfo.bOutLink = bOutLink;
	itemInfo.pImage = NULL;
	itemInfo.sizeImage.SetSize(0, 0);
	itemInfo.nImageCount = 1;
	itemInfo.nImageIndex = nImageIndex;

	if((m_pImage != NULL) && (m_nImagePicCount > 0))
	{
		itemInfo.sizeImage.SetSize(m_pImage->GetWidth() / m_nImagePicCount, m_pImage->GetHeight());
	}

	itemInfo.rc.SetRect(m_rc.left, m_rc.top, m_rc.left + (nItemWidth == 0 ? itemInfo.sizeImage.cx : nItemWidth), m_rc.bottom);

	itemInfo.pControl = pControl;

	return InsertItem(nItem, itemInfo);
}

// 增加tab页
BOOL CDuiTabCtrl::InsertItem(int nItem, TabItemInfo &itemInfo)
{
	if(m_vecItemInfo.size() > 0)
	{
		CRect rc;
		m_vecRcSeperator.push_back(rc);
	}
	if(nItem <= -1 || nItem >= (int)m_vecItemInfo.size())
	{
		m_vecItemInfo.push_back(itemInfo);
	}
	else
	{
		m_vecItemInfo.insert(m_vecItemInfo.begin() + nItem, itemInfo);
	}

	int nXPos = m_rc.left + m_nTabLeftPading;
	int nYPos = m_rc.top + m_nTabTopPading;
	int nXTabBtnPos = m_nTabLeftPading;
	int nYTabBtnPos = m_nTabTopPading;

	for(size_t i = 0; i < (int)m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
		int nItemWidth = itemInfoTemp.rc.Width();
		int nItemHeight = itemInfoTemp.sizeImage.cy;

		if(m_nTabType == TAB_TYPE_HORIZONTAL)	// 水平模式
		{
			if(m_nTabItemWidth == 0)
			{
				// 如果tabctrl没有设置每个tab页的宽度,则以第一个tab页的图片宽度为准
				m_nTabItemWidth = nItemWidth;
			}
			if(m_nTabCtrlHeight == 0)
			{
				// 如果tabctrl没有设置高度,则以第一个tab页的图片高度为准
				m_nTabCtrlHeight = nItemHeight;
			}

			// 设置tab页签的位置
			itemInfoTemp.rc.SetRect(nXPos, nYPos, nXPos + m_nTabItemWidth, nYPos + m_nTabCtrlHeight);

			// 设置Tab页签按钮的位置
			if(m_pImageTabBtn != NULL)
			{
				int nLeft = nXTabBtnPos;
				int nTop = nYTabBtnPos + 3;

				// 计算Tab页签按钮的显示位置
				if(m_posTabBtn.nCount >= 2)
				{
					// 使用设置的Tab页签按钮位置
					int nTabBtnX = PositionItem2Value(m_posTabBtn.Left, 0, m_nTabItemWidth);
					int nTabBtnY = PositionItem2Value(m_posTabBtn.Top, 0, m_nTabCtrlHeight);
					nLeft += nTabBtnX;
					nTop += nTabBtnY;
				}else
				{
					// 如果没有设置位置信息,则设置默认的位置为靠tab页签右上角
					nLeft += (m_nTabItemWidth - m_sizeTabBtn.cx - 3);
					nTop += 3;
				}

				itemInfoTemp.rcButton.SetRect(nLeft, nTop, nLeft + m_sizeTabBtn.cx, nTop + m_sizeTabBtn.cy);
			}

			nXPos += m_nTabItemWidth;
			nXTabBtnPos += m_nTabItemWidth;

			if(i < m_vecItemInfo.size() - 1 && m_pImageSeperator != NULL)
			{
				CRect &rc = m_vecRcSeperator.at(i);
				rc.SetRect(nXPos, nYPos, nXPos + m_sizeSeperator.cx, nYPos + m_sizeSeperator.cy);
				nXPos += m_sizeSeperator.cx;
				nXTabBtnPos += m_sizeSeperator.cx;
			}
		}else
		if(m_nTabType == TAB_TYPE_VERTICAL)	// 垂直模式
		{
			if(m_nTabItemHeight == 0)
			{
				// 如果tabctrl没有设置每个tab页的高度,则以第一个tab页的图片高度为准
				m_nTabItemHeight = nItemHeight;
			}
			if(m_nTabCtrlWidth == 0)
			{
				// 如果tabctrl没有设置宽度,则以第一个tab页的图片宽度为准
				m_nTabCtrlWidth = nItemWidth;
			}

			// 设置tab页签的位置
			itemInfoTemp.rc.SetRect(nXPos, nYPos, nXPos + m_nTabCtrlWidth, nYPos + m_nTabItemHeight);

			// 设置Tab页签按钮的位置
			if(m_pImageTabBtn != NULL)
			{
				int nLeft = nXTabBtnPos;
				int nTop = nYTabBtnPos + (m_nTabItemHeight * i) + 3;

				// 计算Tab页签按钮的显示位置
				if(m_posTabBtn.nCount >= 2)
				{
					// 使用设置的Tab页签按钮位置
					int nTabBtnX = PositionItem2Value(m_posTabBtn.Left, 0, m_nTabCtrlWidth);
					int nTabBtnY = PositionItem2Value(m_posTabBtn.Top, 0, m_nTabItemHeight);
					nLeft += nTabBtnX;
					nTop += nTabBtnY;
				}else
				{
					// 如果没有设置位置信息,则设置默认的位置为靠tab页签右上角
					nLeft += (m_nTabCtrlWidth - m_sizeTabBtn.cx - 3);
					nTop += 3;
				}

				itemInfoTemp.rcButton.SetRect(nLeft, nTop, nLeft + m_sizeTabBtn.cx, nTop + m_sizeTabBtn.cy);
			}

			nYPos += m_nTabItemHeight;
			nYTabBtnPos += m_nTabItemHeight;

			if(i < m_vecItemInfo.size() - 1 && m_pImageSeperator != NULL)
			{
				CRect &rc = m_vecRcSeperator.at(i);
				rc.SetRect(nXPos, nYPos, nXPos + m_sizeSeperator.cx, nYPos + m_sizeSeperator.cy);
				nYPos += m_sizeSeperator.cy;
				nYTabBtnPos += m_sizeSeperator.cy;
			}
		}

		itemInfoTemp.buttonState = enBSNormal;
		
		/*
		if(itemInfoTemp.pControl != NULL)
		{
			CRect rc = itemInfoTemp.pControl->GetRect();
			rc.top += itemInfoTemp.sizeImage.cy;
			itemInfoTemp.pControl->SetRect(rc);
			//itemInfoTemp.pControl->UpdateControl();
		}*/
	}

	if(m_nTabType == TAB_TYPE_HORIZONTAL)
	{
		// 调整tab页签的宽度
		SetItemWidth(m_nTabItemMaxWidth, m_nTabItemMinWidth);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)
	{
		// 调整tab页签的高度
		SetItemHeight(m_nTabItemMaxHeight, m_nTabItemMinHeight);
	}

	UpdateControl(true);
	return true;
}

// 获取tab页签数量
int CDuiTabCtrl::GetItemCount()
{
	return m_vecItemInfo.size();
}

// 根据tab名字获取索引
int CDuiTabCtrl::GetItemIndex(CString strTabName)
{
	// 如果名字为空,则不用查找,直接返回-1
	if(strTabName.IsEmpty())
	{
		return -1;
	}

	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
		if(itemInfoTemp.strName == strTabName)
		{
			return i;
		}
	}
	return -1;
}

// 获取指定tab页信息
TabItemInfo* CDuiTabCtrl::GetItemInfo(int nItem)
{
	if((nItem < 0) || (nItem >= (int)m_vecItemInfo.size()))
	{
		return NULL;
	}

	TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
	return &itemInfo;
}

// 设置选择的tab页
int CDuiTabCtrl::SetSelectItem(int nItem)
{
	int nOldDownItem = m_nDownItem;
	if(m_nDownItem != nItem && nItem >= 0 && nItem < (int)m_vecItemInfo.size())
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);

		if(!itemInfo.strAction.IsEmpty())
		{
			// 如果action非空,则执行动作
			DuiSystem::AddDuiActionTask(GetID(), MSG_BUTTON_UP, nItem, 0, GetName(), itemInfo.strAction, GetParent());
		}

		if(itemInfo.bOutLink)	// 外部链接
		{
			m_nHoverItem = -1;
			SendMessage(MSG_BUTTON_DOWN, nItem, 0);
		}else
		{
			m_nOldItem = m_nDownItem;	// 保存切换前的页面索引,用于切换动画
			m_nDownItem = nItem;					
			m_nHoverItem = -1;

			// 删除旧的Tooltip
			CDlgBase* pDlg = GetParentDialog();
			if(pDlg)
			{
				pDlg->ClearTooltip();
			}

			IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
			if(pIDuiHostWnd)
			{
				pIDuiHostWnd->ClearTooltip();
			}

			// 点击事件消息
			SendMessage(MSG_BUTTON_DOWN, m_nDownItem, 0);

			// 只显示当前活动的tab页对应的Panel对象，其他页面的Panel对象都隐藏
			for(size_t i = 0; i < m_vecItemInfo.size(); i++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(i);
				if(itemInfo.pControl != NULL)
				{
					if(i == m_nDownItem)
					{
						itemInfo.pControl->SetVisible(TRUE);
						SetWindowFocus();
					}else
					{
						itemInfo.pControl->SetVisible(FALSE);
					}
				}
				// 如果启用了动画,则启动切换动画定时器
				if(m_bAnimateChangeTab)
				{
					m_nCurXPos = 0;
					m_nCurYPos = 0;
					m_bRunTime = true;
				}
			}
		}

		UpdateControl();
	}

	return nOldDownItem;
}

// 获取选择的tab页
int CDuiTabCtrl::GetSelectItem()
{
	return m_nDownItem;
}

// 设置Tab页签的宽度(水平模式)
void CDuiTabCtrl::SetItemWidth(int nTabItemMaxWidth, int nTabItemMinWidth, BOOL bRefresh)
{
	if(nTabItemMaxWidth != m_nTabItemMaxWidth)
	{
		m_nTabItemMaxWidth = nTabItemMaxWidth;
	}
	if(nTabItemMinWidth != m_nTabItemMinWidth)
	{
		m_nTabItemMinWidth = nTabItemMinWidth;
	}

	if(GetItemCount() == 0)
	{
		return;
	}

	int nTabItemWidth = m_nTabItemMaxWidth;
	if(m_nTabItemMinWidth > 0)
	{
		// 如果设置了页签的最小宽度,则计算页签实际宽度
		int nTabTotalWidth = m_rc.Width() - m_nTabLeftPading - m_nTabRightPading;
		int nTabItemCount = GetItemCount();
		int nTabItemAveWidth = nTabTotalWidth / nTabItemCount;
		if((nTabItemAveWidth < m_nTabItemMaxWidth) && (nTabItemAveWidth >= m_nTabItemMinWidth))
		{
			// 如果平均宽度介于最小宽度和最大宽度之间,则设置为平均宽度
			nTabItemWidth = nTabItemAveWidth;
		}else
		if(nTabItemAveWidth < m_nTabItemMinWidth)
		{
			// 如果平均宽度小于最小宽度,则设置为最小宽度
			nTabItemWidth = m_nTabItemMinWidth;
		}
	}

	// 如果计算出的宽度和之前的宽度不一致,则刷新tab页
	if(nTabItemWidth != m_nTabItemWidth)
	{
		m_nTabItemWidth = nTabItemWidth;
		if(bRefresh)	// 是否刷新tab页
		{
			RefreshItems();
		}
	}
}

// 设置Tab页签的高度(垂直模式)
void CDuiTabCtrl::SetItemHeight(int nTabItemMaxHeight, int nTabItemMinHeight, BOOL bRefresh)
{
	if(nTabItemMaxHeight != m_nTabItemMaxHeight)
	{
		m_nTabItemMaxHeight = nTabItemMaxHeight;
	}
	if(nTabItemMinHeight != m_nTabItemMinHeight)
	{
		m_nTabItemMinHeight = nTabItemMinHeight;
	}

	if(GetItemCount() == 0)
	{
		return;
	}

	int nTabItemHeight = m_nTabItemMaxHeight;
	if(m_nTabItemMinHeight > 0)
	{
		// 如果设置了页签的最小高度,则计算页签实际高度
		int nTabTotalHeight = m_rc.Height() - m_nTabTopPading - m_nTabBottomPading;
		int nTabItemCount = GetItemCount();
		int nTabItemAveHeight = nTabTotalHeight / nTabItemCount;
		if((nTabItemAveHeight < m_nTabItemMaxHeight) && (nTabItemAveHeight >= m_nTabItemMinHeight))
		{
			// 如果平均高度介于最小高度和最大高度之间,则设置为平均高度
			nTabItemHeight = nTabItemAveHeight;
		}else
		if(nTabItemAveHeight < m_nTabItemMinHeight)
		{
			// 如果平均高度小于最小高度,则设置为最小高度
			nTabItemHeight = m_nTabItemMinHeight;
		}
	}

	// 如果计算出的高度和之前的高度不一致,则刷新tab页
	if(nTabItemHeight != m_nTabItemHeight)
	{
		m_nTabItemHeight = nTabItemHeight;
		if(bRefresh)	// 是否刷新tab页
		{
			RefreshItems();
		}
	}
}

// 刷新所有Tab页
void CDuiTabCtrl::RefreshItems()
{
	// 重新计算每个tab页的位置,并刷新界面
	int nXPos = m_rc.left + m_nTabLeftPading;
	int nYPos = m_rc.top + m_nTabTopPading;

	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
		if(!itemInfoTemp.bVisible)
		{
			// 隐藏的tab页坐标设置为0
			itemInfoTemp.rc.SetRect(0,0,0,0);
			if(i < m_vecRcSeperator.size())
			{
				CRect &rc = m_vecRcSeperator.at(i);
				rc.SetRect(0,0,0,0);
			}			
			continue;
		}

		int nItemWidth = itemInfoTemp.rc.Width();
		int nItemHeight = itemInfoTemp.sizeImage.cy;

		if(m_nTabType == TAB_TYPE_HORIZONTAL)	// 水平模式
		{
			if(m_nTabItemWidth == 0)
			{
				// 如果tabctrl没有设置每个tab页的宽度,则以第一个tab页的图片宽度为准
				m_nTabItemWidth = nItemWidth;
			}
			if(m_nTabCtrlHeight == 0)
			{
				// 如果tabctrl没有设置高度,则以第一个tab页的图片高度为准
				m_nTabCtrlHeight = nItemHeight;
			}

			itemInfoTemp.rc.SetRect(nXPos, nYPos, nXPos + m_nTabItemWidth, nYPos + m_nTabCtrlHeight);

			nXPos += m_nTabItemWidth;

			if(i < m_vecItemInfo.size() - 1 && m_pImageSeperator != NULL)
			{
				CRect &rc = m_vecRcSeperator.at(i);
				rc.SetRect(nXPos, nYPos, nXPos + m_sizeSeperator.cx, nYPos + m_sizeSeperator.cy);
				nXPos += m_sizeSeperator.cx;
			}

			// 设置Tab页签按钮的位置
			if(m_pImageTabBtn != NULL)
			{
				int nLeft = m_nTabLeftPading + (m_nTabItemWidth * i);
				int nTop = 3;

				// 计算Tab页签按钮的显示位置
				if(m_posTabBtn.nCount >= 2)
				{
					// 使用设置的Tab页签按钮位置
					int nTabBtnX = PositionItem2Value(m_posTabBtn.Left, 0, m_nTabItemWidth);
					int nTabBtnY = PositionItem2Value(m_posTabBtn.Top, 0, m_nTabCtrlHeight);
					nLeft += nTabBtnX;
					nTop += nTabBtnY;
				}else
				{
					// 如果没有设置位置信息,则设置默认的位置为靠tab页签右上角
					nLeft += (m_nTabItemWidth - m_sizeTabBtn.cx - 3);
					nTop += 3;
				}

				itemInfoTemp.rcButton.SetRect(nLeft, nTop, nLeft + m_sizeTabBtn.cx, nTop + m_sizeTabBtn.cy);
			}
		}else
		if(m_nTabType == TAB_TYPE_VERTICAL)	// 垂直模式
		{
			if(m_nTabItemHeight == 0)
			{
				// 如果tabctrl没有设置每个tab页的高度,则以第一个tab页的图片高度为准
				m_nTabItemHeight = nItemHeight;
			}
			if(m_nTabCtrlWidth == 0)
			{
				// 如果tabctrl没有设置宽度,则以第一个tab页的图片宽度为准
				m_nTabCtrlWidth = nItemWidth;
			}

			itemInfoTemp.rc.SetRect(nXPos, nYPos, nXPos + m_nTabCtrlWidth, nYPos + m_nTabItemHeight);

			nYPos += m_nTabItemHeight;

			if(i < m_vecItemInfo.size() - 1 && m_pImageSeperator != NULL)
			{
				CRect &rc = m_vecRcSeperator.at(i);
				rc.SetRect(nXPos, nYPos, nXPos + m_sizeSeperator.cx, nYPos + m_sizeSeperator.cy);
				nYPos += m_sizeSeperator.cy;
			}

			// 设置Tab页签按钮的位置
			if(m_pImageTabBtn != NULL)
			{
				int nLeft = m_nTabLeftPading;
				int nTop = m_nTabTopPading + (m_nTabItemHeight * i) + 3;

				// 计算Tab页签按钮的显示位置
				if(m_posTabBtn.nCount >= 2)
				{
					// 使用设置的Tab页签按钮位置
					int nTabBtnX = PositionItem2Value(m_posTabBtn.Left, 0, m_nTabCtrlWidth);
					int nTabBtnY = PositionItem2Value(m_posTabBtn.Top, 0, m_nTabItemHeight);
					nLeft += nTabBtnX;
					nTop += nTabBtnY;
				}else
				{
					// 如果没有设置位置信息,则设置默认的位置为靠tab页签右上角
					nLeft += (m_nTabCtrlWidth - m_sizeTabBtn.cx - 3);
					nTop += 3;
				}

				itemInfoTemp.rcButton.SetRect(nLeft, nTop, nLeft + m_sizeTabBtn.cx, nTop + m_sizeTabBtn.cy);
			}
		}
	}

	// 如果删除了最后一个页面,需要重新计算当前页面的索引
	if(m_nDownItem >= (int)m_vecItemInfo.size())
	{
		m_nDownItem = -1;
		if(m_vecItemInfo.size() > 0)
		{
			for(size_t i = (int)m_vecItemInfo.size()-1; i >= 0; i--)
			{
				TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
				if(itemInfoTemp.bVisible && !itemInfoTemp.bOutLink)
				{
					m_nDownItem = i;
					break;
				}
			}
		}
	}
	if(m_nHoverItem >= (int)m_vecItemInfo.size())
	{
		m_nHoverItem = -1;
	}

	// 刷新显示当前页
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(i);
		if(itemInfo.pControl != NULL)
		{
			if(i == m_nDownItem)
			{
				itemInfo.pControl->SetVisible(TRUE);
				SetWindowFocus();
			}else
			{
				itemInfo.pControl->SetVisible(FALSE);
			}
		}
	}

	UpdateControl(true);
}

// 删除指定tab页
void CDuiTabCtrl::DeleteItem(int nItem)
{
	if((nItem < 0) || (nItem >= (int)m_vecItemInfo.size()))
	{
		return;
	}

	int nIndex = 0;
	vector<TabItemInfo>::iterator it;
	for(it=m_vecItemInfo.begin();it!=m_vecItemInfo.end();++it)
	{
		if(nIndex == nItem)
		{
			TabItemInfo &itemInfo = *it;
			if(itemInfo.pControl != NULL)
			{
				RemoveControl(itemInfo.pControl);
			}
			m_vecItemInfo.erase(it);
			break;
		}
		nIndex++;
	}

	if(m_nTabType == TAB_TYPE_HORIZONTAL)
	{
		// 调整tab页签的宽度
		SetItemWidth(m_nTabItemMaxWidth, m_nTabItemMinWidth, FALSE);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)
	{
		// 调整tab页签的高度
		SetItemHeight(m_nTabItemMaxHeight, m_nTabItemMinHeight, FALSE);
	}

	// 重新计算每个tab页的位置,并刷新界面
	RefreshItems();
}

// 删除指定tab页(按照名字)
void CDuiTabCtrl::DeleteItem(CString strTabName)
{
	vector<TabItemInfo>::iterator it;
	for(it=m_vecItemInfo.begin();it!=m_vecItemInfo.end();++it)
	{
		TabItemInfo &itemInfo = *it;
		if(itemInfo.strName == strTabName)
		{
			if(itemInfo.pControl != NULL)
			{
				RemoveControl(itemInfo.pControl);
			}
			m_vecItemInfo.erase(it);
			break;
		}
	}

	if(m_nTabType == TAB_TYPE_HORIZONTAL)
	{
		// 调整tab页签的宽度
		SetItemWidth(m_nTabItemMaxWidth, m_nTabItemMinWidth, FALSE);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)
	{
		// 调整tab页签的高度
		SetItemHeight(m_nTabItemMaxHeight, m_nTabItemMinHeight, FALSE);
	}

	// 重新计算每个tab页的位置,并刷新界面
	RefreshItems();
}

// 设置tab页的可见性
void CDuiTabCtrl::SetItemVisible(int nItem, BOOL bVisible)
{
	if((nItem < 0) || (nItem >= (int)m_vecItemInfo.size()))
	{
		return;
	}

	TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
	itemInfo.bVisible = bVisible;

	// 重新计算每个tab页的位置,并刷新界面
	RefreshItems();
}

// 设置tab页的可见性(根据tab名字)
void CDuiTabCtrl::SetItemVisible(CString strTabName, BOOL bVisible)
{
	int nItem = GetItemIndex(strTabName);
	if(nItem != -1)
	{
		SetItemVisible(nItem, bVisible);
	}
}

// 获取tab页的可见性
BOOL CDuiTabCtrl::GetItemVisible(int nItem)
{
	if((nItem < 0) || (nItem >= (int)m_vecItemInfo.size()))
	{
		return FALSE;
	}

	TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
	return itemInfo.bVisible;
}

// 获取tab页的可见性
BOOL CDuiTabCtrl::GetItemVisible(CString strTabName)
{
	int nItem = GetItemIndex(strTabName);
	if(nItem != -1)
	{
		return GetItemVisible(nItem);
	}

	return FALSE;
}

void CDuiTabCtrl::SetControlRect(CRect rc)
{
	__super::SetControlRect(rc);

	if(m_nTabType == TAB_TYPE_HORIZONTAL)
	{
		// 调整tab页签的宽度
		SetItemWidth(m_nTabItemMaxWidth, m_nTabItemMinWidth, FALSE);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)
	{
		// 调整tab页签的高度
		SetItemHeight(m_nTabItemMaxHeight, m_nTabItemMinHeight, FALSE);
	}

	// 重新计算所有Tab页的位置
	RefreshItems();
}

// 重载设置控件可见性的函数，需要调用子控件的函数
void CDuiTabCtrl::SetControlVisible(BOOL bIsVisible)
{
	//__super::SetControlVisible(bIsVisible);

	if(bIsVisible)
	{
		// 如果设置为可见状态,则仅可见页的控件设置为可见状态
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(i);
			if((itemInfo.pControl != NULL) && (i == m_nDownItem))
			{
				itemInfo.pControl->SetControlVisible(bIsVisible);
			}else
			if((itemInfo.pControl != NULL) && (i != m_nDownItem))
			{
				itemInfo.pControl->SetControlVisible(FALSE);
			}
		}
	}else
	{
		// 如果设置为隐藏状态,则所有页面的控件都需要隐藏
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(i);
			if(itemInfo.pControl != NULL)
			{
				itemInfo.pControl->SetControlVisible(FALSE);
			}
		}
	}
}

// 重载设置控件中windows原生控件可见性的函数，需要调用子控件的函数
void CDuiTabCtrl::SetControlWndVisible(BOOL bIsVisible)
{
	__super::SetControlWndVisible(bIsVisible);

	// 设置控件和子控件的原生Windows控件的可见性
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			// 判断子控件当前是否可见,根据可见性设置子控件的原生控件的可见性
			BOOL bVisible = pControlBase->GetVisible();
			pControlBase->SetControlWndVisible(bVisible);
		}
	}
}

// 重载设置控件隐藏状态的函数，需要调用子控件的函数
void CDuiTabCtrl::SetControlHide(BOOL bIsHide)
{
	//__super::SetControlHide(bIsHide);

	if(!bIsHide)
	{
		// 如果设置为可见状态,则仅可见页的控件设置为可见状态
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(i);
			if((itemInfo.pControl != NULL) && (i == m_nDownItem))
			{
				itemInfo.pControl->SetControlHide(bIsHide);
			}else
			if((itemInfo.pControl != NULL) && (i != m_nDownItem))
			{
				itemInfo.pControl->SetControlHide(TRUE);
			}
		}
	}else
	{
		// 如果设置为隐藏状态,则所有页面的控件都需要隐藏
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(i);
			if(itemInfo.pControl != NULL)
			{
				itemInfo.pControl->SetControlHide(TRUE);
			}
		}
	}
}

// 判断指定的坐标位置是否在某一个Tab页签的按钮上
BOOL CDuiTabCtrl::PtInTabButton(CPoint point, TabItemInfo& itemInfo)
{
	CRect rc = itemInfo.rcButton;
	// 鼠标坐标转换
	rc.OffsetRect(m_rc.left, m_rc.top);
	return rc.PtInRect(point);
}

// 从XML设置Tab页签按钮位置信息属性
HRESULT CDuiTabCtrl::OnAttributeTabBtnPosChange(const CString& strValue, BOOL bLoading)
{
    if (strValue.IsEmpty()) return E_FAIL;

	m_posTabBtn.nCount=0;
	LPCTSTR pszValue=strValue;
	while(m_posTabBtn.nCount<4 && pszValue)
	{
		pszValue=ParsePosition(pszValue,m_posTabBtn.Item[m_posTabBtn.nCount++]);
	}

    return bLoading?S_FALSE:S_OK;
}

// 设置Tab页签的Tooltip
void CDuiTabCtrl::SetTabTooltip(int nItem, CString strTooltip)
{
	if(m_nTipItem != nItem)
	{
		TabItemInfo* pTabInfo = GetItemInfo(nItem);
		BOOL bHaveDivTip = ((pTabInfo->pControl != NULL) && !pTabInfo->pControl->GetTooltip().IsEmpty());
		if(pTabInfo && (pTabInfo->bNeedTextTip || bHaveDivTip))
		{
			CRect rc = pTabInfo->rc;
			SetTooltip(this, strTooltip, rc, TRUE);
		}else
		{
			ClearTooltip();
		}

		m_nTipItem = nItem;
	}
}

// 清除Tooltip
void CDuiTabCtrl::ClearTabTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipItem = -1;
		return;
	}

	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->ClearTooltip();
		m_nTipItem = -1;
		return;
	}
}

// 判断鼠标是否在控件可响应的区域
BOOL CDuiTabCtrl::OnCheckMouseResponse(UINT nFlags, CPoint point)
{
	CRect rc = m_rc;
	if(m_nTabType == TAB_TYPE_HORIZONTAL)
	{
		rc.bottom = rc.top + m_nTabCtrlHeight;
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)
	{
		rc.right = rc.left + m_nTabCtrlWidth;
	}
	if(!rc.PtInRect(point))
	{
		return true;
	}

	// 判断如果鼠标不在所有tab页范围内,则不做响应
	BOOL bPtInTabs = false;
	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(i);
		if(itemInfo.bVisible && itemInfo.rc.PtInRect(point))
		{
			bPtInTabs = true;
			break;
		}
	}
	
	return bPtInTabs;
}

BOOL CDuiTabCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	int nOldHoverItem = m_nHoverItem;
	BOOL bTabBtnChange = FALSE;

	if(m_rc.PtInRect(point))
	{
		if(m_nHoverItem != -1)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(m_nHoverItem);
			if(itemInfo.rc.PtInRect(point))
			{
				if(m_bTabTooltip)	// 设置Tab页Tooltip
				{
					if(itemInfo.bNeedTextTip)	
					{
						SetTabTooltip(m_nHoverItem, itemInfo.strText);
					}else
					if((itemInfo.pControl != NULL) && (!itemInfo.pControl->GetTooltip().IsEmpty()))
					{
						SetTabTooltip(m_nHoverItem, itemInfo.pControl->GetTooltip());
					}else
					{
						ClearTabTooltip();
					}
				}
				return false;
			}
			m_nHoverItem = -1;		
		}		

		BOOL bMousenDown = false;
		if(m_nDownItem != -1)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);
			if(itemInfo.rc.PtInRect(point))
			{
				bMousenDown = true;
				m_nHoverItem = -1;
				if(m_bTabTooltip)	// 设置Tab页Tooltip
				{
					if(itemInfo.bNeedTextTip)
					{
						SetTabTooltip(m_nDownItem, itemInfo.strText);
					}else
					if((itemInfo.pControl != NULL) && (!itemInfo.pControl->GetTooltip().IsEmpty()))
					{
						SetTabTooltip(m_nDownItem, itemInfo.pControl->GetTooltip());
					}else
					{
						ClearTabTooltip();
					}
				}
			}		
		}

		if(!bMousenDown)
		{
			for(size_t i = 0; i < m_vecItemInfo.size(); i++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(i);
				if(itemInfo.rc.PtInRect(point))
				{
					m_nHoverItem = i;
					break;
				}
			}
		}
	}
	else
	{
		m_nHoverItem = -1;
		m_nTipItem = -1;
	}

	// 计算Tab页签的按钮图片状态
	if(m_rc.PtInRect(point) && (m_pImageTabBtn != NULL))
	{
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(i);
			// 鼠标移动到tab页签的按钮
			if(PtInTabButton(point, itemInfo))
			{
				if(itemInfo.buttonState != enBSHover)
				{
					bTabBtnChange = TRUE;
				}
				itemInfo.buttonState = enBSHover;
			}else
			{
				if(itemInfo.buttonState != enBSNormal)
				{
					bTabBtnChange = TRUE;
				}
				itemInfo.buttonState = enBSNormal;
			}
		}
	}

	if((nOldHoverItem != m_nHoverItem) || bTabBtnChange)
	{
		UpdateControl();
		return true;
	}
	
	return false;
}

BOOL CDuiTabCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	// 判断Tab页签的按钮点击
	if(m_rc.PtInRect(point) && (m_pImageTabBtn != NULL))
	{
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(i);
			// 点击了tab页签的按钮,发送消息
			if(PtInTabButton(point, itemInfo))
			{
				itemInfo.buttonState = enBSDown;
				UpdateControl();
				SendMessage(MSG_CONTROL_BUTTON, i, 0);
				return true;
			}
		}
	}

	if(m_nHoverItem != -1)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nHoverItem);
		if(itemInfo.rc.PtInRect(point))
		{
			// 切换到鼠标当前的tab页
			SetSelectItem(m_nHoverItem);
		}		
	}
	
	return false;
}

BOOL CDuiTabCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	return false;
}

// 滚动事件处理
BOOL CDuiTabCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(m_nDownItem != -1)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);
		if(itemInfo.rc.PtInRect(point))
		{
			if(itemInfo.pControl != NULL)
			{
				return itemInfo.pControl->OnScroll(bVertical, nFlags, point);
			}
		}
	}

	return false;
}

// 键盘按下事件处理
BOOL CDuiTabCtrl::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_nDownItem != -1)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);
		if(itemInfo.pControl != NULL)
		{
			return itemInfo.pControl->OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}

	return false;
}

// 键盘放开事件处理
BOOL CDuiTabCtrl::OnControlKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_nDownItem != -1)
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);
		if(itemInfo.pControl != NULL)
		{
			return itemInfo.pControl->OnKeyUp(nChar, nRepCnt, nFlags);
		}
	}

	return false;
}

// 画控件
void CDuiTabCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	if(m_nTabType == TAB_TYPE_HORIZONTAL)	// 水平模式
	{
		DrawControlHorizontal(dc, rcUpdate);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)	// 垂直模式
	{
		DrawControlVertical(dc, rcUpdate);
	}
}

// 画控件(水平模式)
void CDuiTabCtrl::DrawControlHorizontal(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	//int nHeight = m_rc.Height();	// 纵向内存DC的高度是整个tabctrl的高度,不只是页签部分高度
	int nTabHeight = m_nTabCtrlHeight;	// 纵向内存DC高度改为tabctrl部分高度

	if(!m_bUpdate)
	{
		// 创建内存DC,纵向分为6层:
		// 1.tab页签图片-原图
		// 2.tab页签图片-鼠标热点
		// 3.tab页签图片-鼠标按下
		// 4,5,6层是上面3层的备份
		UpdateMemDC(dc, nWidth, nTabHeight * 3 * 2);

		Graphics graphics(m_memDC);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		//SolidBrush solidBrush(m_clrText);			// 正常文字画刷

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// 设置tab页签文字的水平和垂直对齐方式
		DUI_STRING_ALIGN_DEFINE();
		strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
		
		// 画内存DC的3个层的内容
		for(int i = 0; i < 3; i++)
		{
			// 将背景内容拷贝到内存DC
			m_memDC.BitBlt(0, i * nTabHeight, nWidth, nTabHeight, &dc, m_rc.left, m_rc.top, SRCCOPY);

			// 画tab页签
			int nXPos = m_nTabLeftPading;
			int nYPos = m_nTabTopPading + i * nTabHeight;
			for(size_t j = 0; j < m_vecItemInfo.size(); j++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(j);
				
				if(!itemInfo.bVisible)
				{
					continue;
				}

				// 图片位置(根据对齐方式进行计算)
				CPoint point = GetOriginPoint(m_nTabItemWidth, m_nTabCtrlHeight, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy,
						GetGDIAlignment(m_uAlignment), GetGDIVAlignment(m_uVAlignment));
				// 如果有图片和文字,则图片的垂直对齐按照上对齐方式
				if(!itemInfo.strText.IsEmpty())
				{
					point.y = 0;
				}

				// 画tab页签底图
				if(itemInfo.pImage != NULL)	// 如果页签设置了图片,则使用tab页签指定的图片
				{
					int nImageIndex = i;
					if(itemInfo.nImageCount == 1)
					{
						nImageIndex = 0;
					}
					graphics.DrawImage(itemInfo.pImage, Rect(nXPos + point.x, nYPos + point.y,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
						itemInfo.sizeImage.cx * nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
				}else
				if((m_pImage != NULL) && (itemInfo.nImageIndex != -1))	// 如果设置了页签图片索引,使用tabctrl图片的索引图片
				{
					if(m_enTabImageMode == enTIMNormal)	// 普通模式
					{
						graphics.DrawImage(m_pImage, Rect(nXPos + point.x, nYPos + point.y,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
							itemInfo.sizeImage.cx * itemInfo.nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
					}else
					if(m_enTabImageMode == enTIMMID)	// 九宫格模式
					{
						CRect rcTemp(nXPos, nYPos, nXPos+m_nTabItemWidth, nYPos+m_nTabCtrlHeight);
						DrawImageFrameMID(graphics, m_pImage, rcTemp, m_sizeImage.cx * i, 0, m_sizeImage.cx, m_sizeImage.cy,
							m_nWLT, m_nHLT, m_nWRB, m_nHRB);
					}
				}

				// 画tab页签热点图(如果存在tabctrl设置的热点图的话)
				if((m_pImageHover != NULL) && (i > 0))
				{
					int nX = (itemInfo.rc.Width() - m_sizeHover.cx) / 2;
					if(nX < 0)
					{
						nX = 0;
					}
					graphics.DrawImage(m_pImageHover, Rect(nXPos + nX, nYPos,  m_sizeHover.cx, m_sizeHover.cy),
						m_sizeHover.cx * (i-1), 0, m_sizeHover.cx, m_sizeHover.cy, UnitPixel);
				}

				// 画tab页签文字
				if(!itemInfo.strText.IsEmpty())
				{
					// 设置页签文字颜色
					SolidBrush solidBrushItem(m_clrText);
					if((m_nHoverItem == j) && (m_clrTextHover.GetValue() != Color(0, 0, 0, 0).GetValue()))	// 设置了鼠标移动颜色,则使用
					{
						solidBrushItem.SetColor(m_clrTextHover);
					}else
					if((m_nDownItem == j) && (m_clrTextDown.GetValue() != Color(0, 0, 0, 0).GetValue()))	// 设置了鼠标按下颜色,则使用
					{
						solidBrushItem.SetColor(m_clrTextDown);
					}

					RectF rectText((Gdiplus::REAL)nXPos,
							(Gdiplus::REAL)(nYPos + itemInfo.sizeImage.cy + 1),
							(Gdiplus::REAL)((m_pImageTabBtn != NULL) ? (itemInfo.rc.Width()-m_sizeTabBtn.cx) : itemInfo.rc.Width()),
							(Gdiplus::REAL)(m_nTabCtrlHeight - itemInfo.sizeImage.cy - 1));
					if(m_nTabCtrlHeight <= itemInfo.sizeImage.cy)
					{
						// 如果tabctrl高度小于图片高度,则文字直接居中显示
						rectText.Y = (Gdiplus::REAL)nYPos;
						rectText.Height = (Gdiplus::REAL)m_nTabCtrlHeight;
					}

					// 计算是否需要显示tip
					itemInfo.bNeedTextTip = rectText.Width < GetTextBounds(font, itemInfo.strText).Width;

					BSTR bsText = itemInfo.strText.AllocSysString();
					graphics.DrawString(bsText, (INT)wcslen(bsText), &font, rectText, &strFormat, &solidBrushItem);
					::SysFreeString(bsText);
				}

				nXPos += itemInfo.rc.Width();

				// 画tab页签之间的分隔图片(采用拉伸方式)
				if(j < m_vecItemInfo.size() - 1 && m_pImageSeperator != NULL)
				{
					CRect &rc = m_vecRcSeperator.at(j);
					int nSepHeight = itemInfo.rc.Height();	// m_sizeSeperator.cy
					graphics.DrawImage(m_pImageSeperator, Rect(nXPos, nYPos, m_sizeSeperator.cx, nSepHeight),
						0, 0, m_sizeSeperator.cx, m_sizeSeperator.cy, UnitPixel);

					nXPos += m_sizeSeperator.cx;
				}
			}
		}

		// 内存dc复制一份进行备份
		m_memDC.BitBlt(0, nTabHeight * 3, nWidth, nTabHeight * 3, &m_memDC, 0, 0, SRCCOPY);
	}

	// 画Tab页签按钮到内存dc
	if(m_pImageTabBtn != NULL)
	{
		// 现将备份的内存dc整体进行恢复,避免页签按钮叠加之后的影响
		m_memDC.BitBlt(0, 0, nWidth, nTabHeight * 3, &m_memDC, 0, nTabHeight * 3, SRCCOPY);

		Graphics graphics(m_memDC);
		for(int i = 0; i < 3; i++)
		{
			for(size_t j = 0; j < m_vecItemInfo.size(); j++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(j);
				graphics.DrawImage(m_pImageTabBtn,
					RectF((Gdiplus::REAL)itemInfo.rcButton.left, (Gdiplus::REAL)(nTabHeight * i + itemInfo.rcButton.top),
					(Gdiplus::REAL)itemInfo.rcButton.Width(), (Gdiplus::REAL)itemInfo.rcButton.Height()),
					(Gdiplus::REAL)(itemInfo.buttonState * m_sizeTabBtn.cx), 0,
					(Gdiplus::REAL)m_sizeTabBtn.cx, (Gdiplus::REAL)m_sizeTabBtn.cy,
					UnitPixel); 
			}
		}
	}

	// 内存dc输出到dc
	// 1.画原图
	dc.BitBlt(m_rc.left,m_rc.top, nWidth, nTabHeight, &m_memDC, 0, 0, SRCCOPY);

	// 2.画鼠标热点的Tab页签
	if((m_nHoverItem != -1) && (m_nHoverItem < (int)m_vecItemInfo.size()))
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nHoverItem);

		dc.BitBlt(itemInfo.rc.left, itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC,
			itemInfo.rc.left - m_rc.left, itemInfo.rc.top - m_rc.top + nTabHeight, SRCCOPY);
	}

	// 3.画鼠标按下的Tab页签
	if((m_nDownItem != -1) && (m_nDownItem < (int)m_vecItemInfo.size()))
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);

		dc.BitBlt(itemInfo.rc.left, itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC,
			itemInfo.rc.left - m_rc.left, itemInfo.rc.top - m_rc.top + nTabHeight * 2, SRCCOPY);
	}
}

// 画控件(垂直模式)
void CDuiTabCtrl::DrawControlVertical(CDC &dc, CRect rcUpdate)
{
	int nHeight = m_rc.Height();
	int nTabWidth = m_nTabCtrlWidth;	// 横向内存DC宽度改为tabctrl部分宽度

	if(!m_bUpdate)
	{
		// 创建内存DC,横向分为6层:
		// 1.tab页签图片-原图
		// 2.tab页签图片-鼠标热点
		// 3.tab页签图片-鼠标按下
		// 4,5,6层是上面3层的备份
		UpdateMemDC(dc, nTabWidth * 3 * 2, nHeight);

		Graphics graphics(m_memDC);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// 设置tab页签文字的水平和垂直对齐方式
		DUI_STRING_ALIGN_DEFINE();
		strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
		
		// 画内存DC的3个层的内容
		for(int i = 0; i < 3; i++)
		{
			// 将背景内容拷贝到内存DC
			m_memDC.BitBlt(i * nTabWidth, 0, nTabWidth, nHeight, &dc, m_rc.left, m_rc.top, SRCCOPY);

			// 画tab页签
			int nXPos = m_nTabLeftPading + i * nTabWidth;
			int nYPos = m_nTabTopPading;
			for(size_t j = 0; j < m_vecItemInfo.size(); j++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(j);
				
				if(!itemInfo.bVisible)
				{
					continue;
				}

				// 图片位置(根据对齐方式进行计算)
				CPoint point = GetOriginPoint(m_nTabCtrlWidth, m_nTabItemHeight, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy,
						GetGDIAlignment(m_uAlignment), GetGDIVAlignment(m_uVAlignment));
				// 如果有图片和文字,则图片的水平对齐按照左对齐方式
				if(!itemInfo.strText.IsEmpty())
				{
					point.x = 0;
				}

				// 画tab页签底图
				if(itemInfo.pImage != NULL)	// 如果页签设置了图片,则使用tab页签指定的图片
				{
					int nImageIndex = i;
					if(itemInfo.nImageCount == 1)
					{
						nImageIndex = 0;
					}
					graphics.DrawImage(itemInfo.pImage, Rect(nXPos + point.x, nYPos + point.y,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
						itemInfo.sizeImage.cx * nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
				}else
				if((m_pImage != NULL) && (itemInfo.nImageIndex != -1))	// 如果设置了页签图片索引,使用tabctrl图片的索引图片
				{
					if(m_enTabImageMode == enTIMNormal)	// 普通模式
					{
						graphics.DrawImage(m_pImage, Rect(nXPos + point.x, nYPos + point.y,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
							itemInfo.sizeImage.cx * itemInfo.nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
					}else
					if(m_enTabImageMode == enTIMMID)	// 九宫格模式
					{
						CRect rcTemp(nXPos, nYPos, nXPos+m_nTabCtrlWidth, nYPos+m_nTabItemHeight);
						DrawImageFrameMID(graphics, m_pImage, rcTemp, m_sizeImage.cx * i, 0, m_sizeImage.cx, m_sizeImage.cy,
							m_nWLT, m_nHLT, m_nWRB, m_nHRB);
					}
				}

				// 画tab页签热点图(如果存在tabctrl设置的热点图的话)
				if((m_pImageHover != NULL) && (i > 0))
				{
					int nY = (itemInfo.rc.Height() - m_sizeHover.cy) / 2;
					if(nY < 0)
					{
						nY = 0;
					}
					graphics.DrawImage(m_pImageHover, Rect(nXPos, nYPos + nY,  m_sizeHover.cx, m_sizeHover.cy),
						m_sizeHover.cx * (i-1), 0, m_sizeHover.cx, m_sizeHover.cy, UnitPixel);
				}

				// 画tab页签文字
				if(!itemInfo.strText.IsEmpty())
				{
					// 设置页签文字颜色
					SolidBrush solidBrushItem(m_clrText);
					if((m_nHoverItem == j) && (m_clrTextHover.GetValue() != Color(0, 0, 0, 0).GetValue()))	// 设置了鼠标移动颜色,则使用
					{
						solidBrushItem.SetColor(m_clrTextHover);
					}else
					if((m_nDownItem == j) && (m_clrTextDown.GetValue() != Color(0, 0, 0, 0).GetValue()))	// 设置了鼠标按下颜色,则使用
					{
						solidBrushItem.SetColor(m_clrTextDown);
					}

					RectF rectText((Gdiplus::REAL)(nXPos + itemInfo.sizeImage.cx + 1),
							(Gdiplus::REAL)nYPos,
							(Gdiplus::REAL)(m_nTabCtrlWidth - itemInfo.sizeImage.cx - 1),
							(Gdiplus::REAL)((m_pImageTabBtn != NULL) ? (itemInfo.rc.Height()-m_sizeTabBtn.cy) : itemInfo.rc.Height()));
					if(m_nTabCtrlWidth <= itemInfo.sizeImage.cx)
					{
						// 如果tabctrl宽度小于图片宽度,则文字直接居中显示
						rectText.X = (Gdiplus::REAL)nXPos;
						rectText.Width = (Gdiplus::REAL)m_nTabCtrlWidth;
					}

					// 计算是否需要显示tip
					itemInfo.bNeedTextTip = rectText.Width < GetTextBounds(font, itemInfo.strText).Width;

					BSTR bsText = itemInfo.strText.AllocSysString();
					graphics.DrawString(bsText, (INT)wcslen(bsText), &font, rectText, &strFormat, &solidBrushItem);
					::SysFreeString(bsText);
				}

				nYPos += itemInfo.rc.Height();

				// 画tab页签之间的分隔图片(采用拉伸方式)
				if(j < m_vecItemInfo.size() - 1 && m_pImageSeperator != NULL)
				{
					CRect &rc = m_vecRcSeperator.at(j);
					int nSepWidth = itemInfo.rc.Width();
					graphics.DrawImage(m_pImageSeperator, Rect(nXPos, nYPos, nSepWidth, m_sizeSeperator.cy),
						0, 0, m_sizeSeperator.cx, m_sizeSeperator.cy, UnitPixel);

					nYPos += m_sizeSeperator.cy;
				}
			}
		}

		// 内存dc复制一份进行备份
		m_memDC.BitBlt(nTabWidth * 3, 0, nTabWidth * 3, nHeight, &m_memDC, 0, 0, SRCCOPY);
	}

	// 画Tab页签按钮到内存dc
	if(m_pImageTabBtn != NULL)
	{
		// 现将备份的内存dc整体进行恢复,避免页签按钮叠加之后的影响
		m_memDC.BitBlt(0, 0, nTabWidth * 3, nHeight, &m_memDC, nTabWidth * 3, 0, SRCCOPY);

		Graphics graphics(m_memDC);
		for(int i = 0; i < 3; i++)
		{
			for(size_t j = 0; j < m_vecItemInfo.size(); j++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(j);
				graphics.DrawImage(m_pImageTabBtn,
					RectF((Gdiplus::REAL)(nTabWidth * i + itemInfo.rcButton.left), (Gdiplus::REAL)itemInfo.rcButton.top,
					(Gdiplus::REAL)itemInfo.rcButton.Width(), (Gdiplus::REAL)itemInfo.rcButton.Height()),
					(Gdiplus::REAL)(itemInfo.buttonState * m_sizeTabBtn.cx), 0,
					(Gdiplus::REAL)m_sizeTabBtn.cx, (Gdiplus::REAL)m_sizeTabBtn.cy,
					UnitPixel); 
			}
		}
	}

	// 内存dc输出到dc
	// 1.画原图
	dc.BitBlt(m_rc.left,m_rc.top, nTabWidth, nHeight, &m_memDC, 0, 0, SRCCOPY);

	// 2.画鼠标热点的Tab页签
	if((m_nHoverItem != -1) && (m_nHoverItem < (int)m_vecItemInfo.size()))
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nHoverItem);

		dc.BitBlt(itemInfo.rc.left, itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC,
			itemInfo.rc.left - m_rc.left + nTabWidth, itemInfo.rc.top - m_rc.top, SRCCOPY);
	}

	// 3.画鼠标按下的Tab页签
	if((m_nDownItem != -1) && (m_nDownItem < (int)m_vecItemInfo.size()))
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);

		dc.BitBlt(itemInfo.rc.left, itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC,
			itemInfo.rc.left - m_rc.left + nTabWidth * 2, itemInfo.rc.top - m_rc.top, SRCCOPY);
	}
}

// 控件的动画定时器
BOOL CDuiTabCtrl::OnControlTimer()
{
	if(!m_bRunTime || (m_nOldItem == -1))
	{
		return FALSE;
	}

	BOOL bFinish = FALSE;
	if(m_nTabType == TAB_TYPE_HORIZONTAL)	// 水平模式
	{
		int nAnimatePos = m_rc.Width() / m_nAnimateCount;
		if(m_nDownItem > m_nOldItem)	// 向右切换
		{
			m_nCurXPos += nAnimatePos;
			if(m_nCurXPos >= m_rc.Width())
			{
				bFinish = TRUE;
			}
		}else	// 向左切换
		{
			m_nCurXPos -= nAnimatePos;
			if((m_rc.Width() + m_nCurXPos) <= 0)
			{
				bFinish = TRUE;
			}
		}
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)	// 垂直模式
	{
		int nAnimatePos = m_rc.Height() / m_nAnimateCount;
		if(m_nDownItem > m_nOldItem)	// 向下切换
		{
			m_nCurYPos += nAnimatePos;
			if(m_nCurYPos >= m_rc.Height())
			{
				bFinish = TRUE;
			}
		}else	// 向上切换
		{
			m_nCurYPos -= nAnimatePos;
			if((m_rc.Height() + m_nCurYPos) <= 0)
			{
				bFinish = TRUE;
			}
		}
	}

	if(bFinish)
	{
		// 结束切换动画定时器
		m_bRunTime = false;
	}

	UpdateControl();

	return true;
}

// 画子控件(支持切换tab页的动画显示)
BOOL CDuiTabCtrl::DrawSubControls(CDC &dc, CRect rcUpdate)
{
	if(!m_bRunTime)
	{
		// 如果没有处于动画过程,则直接画所有子控件
		return __super::DrawSubControls(dc, rcUpdate);
	}

	TabItemInfo* pOldTabInfo = GetItemInfo(m_nOldItem);
	TabItemInfo* pNewTabInfo = GetItemInfo(m_nDownItem);
	if((pOldTabInfo == NULL) || (pNewTabInfo == NULL))
	{
		return FALSE;
	}

	int nLeft = m_rc.left;
	int nTop = m_rc.top;
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	// 初始化旧tab页动画dc,并复制背景到动画dc
	CBitmap	aniBitmapOld;
	CDC animateDCOld;
	animateDCOld.CreateCompatibleDC(&dc);
	aniBitmapOld.CreateCompatibleBitmap(&dc, m_rc.right, m_rc.bottom);
	CBitmap* pOldAniBitmapOld = animateDCOld.SelectObject(&aniBitmapOld);
	animateDCOld.BitBlt(nLeft, nTop, nWidth, nHeight, &dc, nLeft, nTop, SRCCOPY);	// 背景复制到动画dc

	// 初始化新tab页动画dc,并复制背景到动画dc
	CBitmap	aniBitmapNew;
	CDC animateDCNew;
	animateDCNew.CreateCompatibleDC(&dc);
	aniBitmapNew.CreateCompatibleBitmap(&dc, m_rc.right, m_rc.bottom);
	CBitmap* pOldAniBitmapNew = animateDCNew.SelectObject(&aniBitmapNew);
	animateDCNew.BitBlt(nLeft, nTop, nWidth, nHeight, &dc, nLeft, nTop, SRCCOPY);	// 背景复制到动画dc

	// 画旧tab页
	// 设置旧tab页为可见
	pOldTabInfo->pControl->SetVisible(TRUE);
	pNewTabInfo->pControl->SetVisible(FALSE);
	// 画旧tab页到动画dc
	pOldTabInfo->pControl->Draw(animateDCOld, rcUpdate);
	// 动画dc复制到dc
	int nTabTop = pOldTabInfo->pControl->GetRect().top;
	int nTabLeft = pOldTabInfo->pControl->GetRect().left;
	if(m_nTabType == TAB_TYPE_HORIZONTAL)	// 水平模式
	{
		if(m_nCurXPos > 0)
		{
			dc.BitBlt(nLeft, nTabTop, nWidth-m_nCurXPos, m_rc.bottom-nTabTop, &animateDCOld, nLeft+m_nCurXPos, nTabTop, SRCCOPY);
		}else
		{
			dc.BitBlt(nLeft-m_nCurXPos, nTabTop, nWidth+m_nCurXPos, m_rc.bottom-nTabTop, &animateDCOld, nLeft, nTabTop, SRCCOPY);
		}
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)	// 垂直模式
	{
		if(m_nCurYPos > 0)
		{
			dc.BitBlt(nTabLeft, nTop, m_rc.right-nTabLeft, nHeight-m_nCurYPos, &animateDCOld, nTabLeft, nTop+m_nCurYPos, SRCCOPY);
		}else
		{
			dc.BitBlt(nTabLeft, nTop-m_nCurYPos, m_rc.right-nTabLeft, nHeight+m_nCurYPos, &animateDCOld, nTabLeft, nTop, SRCCOPY);
		}
	}

	// 释放旧tab页动画dc
	animateDCOld.SelectObject(pOldAniBitmapOld);
	animateDCOld.DeleteDC();
	aniBitmapOld.DeleteObject();

	// 画新tab页
	//UpdateAnimateDC(dc, m_rc.right, m_rc.bottom);	// 初始化动画dc
	//m_animateDC.BitBlt(nLeft, nTop, nWidth, nHeight, &dc, nLeft, nTop, SRCCOPY);	// 背景复制到动画dc
	// 设置新tab页为可见
	pOldTabInfo->pControl->SetVisible(FALSE);
	pNewTabInfo->pControl->SetVisible(TRUE);
	// 画新tab页到动画dc
	pNewTabInfo->pControl->Draw(animateDCNew, rcUpdate);
	// 动画dc复制到dc
	nTabTop = pNewTabInfo->pControl->GetRect().top;
	if(m_nTabType == TAB_TYPE_HORIZONTAL)	// 水平模式
	{
		if(m_nCurXPos > 0)
		{
			dc.BitBlt(nLeft+nWidth-m_nCurXPos, nTabTop, m_nCurXPos, m_rc.bottom-nTabTop, &animateDCNew, nLeft, nTabTop, SRCCOPY);
		}else
		{
			dc.BitBlt(nLeft+0, nTabTop, -m_nCurXPos, m_rc.bottom-nTabTop, &animateDCNew, nLeft+nWidth+m_nCurXPos, nTabTop, SRCCOPY);
		}
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)	// 垂直模式
	{
		if(m_nCurYPos > 0)
		{
			dc.BitBlt(nTabLeft, nTop+nHeight-m_nCurYPos, m_rc.right-nTabLeft, m_nCurYPos, &animateDCNew, nTabLeft, nTop, SRCCOPY);
		}else
		{
			dc.BitBlt(nTabLeft, nTop+0, m_rc.right-nTabLeft, -m_nCurYPos, &animateDCNew, nTabLeft, nTop+nHeight+m_nCurYPos, SRCCOPY);
		}
	}

	// 释放新tab页动画dc
	animateDCNew.SelectObject(pOldAniBitmapNew);
	animateDCNew.DeleteDC();
	aniBitmapNew.DeleteObject();

	return TRUE;
}