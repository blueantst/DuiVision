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
	SetBitmapCount(3);	// tabҳǩͼƬĬ����3��״̬��ͼƬ
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
	SetBitmapCount(3);	// tabҳǩͼƬĬ����3��״̬��ͼƬ
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

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTabCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTabCtrl, Hover, 2)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiTabCtrl, TabBtn, 4)

// ���ݿؼ��������ؼ�ʵ��
CControlBase* CDuiTabCtrl::_CreateControlByName(LPCTSTR lpszName)
{
	// ����Ѿ������˴��ھ��,ʹ�ô��ھ�������ؼ�
	HWND hWnd = m_hWnd;
	if(hWnd != NULL)
	{
		return DuiSystem::CreateControlByName(lpszName, hWnd, this);
	}

	// ���Ҹ��Ի���Ĵ��ھ��,ͨ�����Ի����������ؼ�
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

// ���ؼ���XML�ڵ㺯���������²��tabҳ������
BOOL CDuiTabCtrl::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	__super::Load(pXmlElem);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}

	// ���û������tabctrl�߶�/���,����hoverͼƬ�ĸ߶�/���
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
	// ���û������tab��Ŀ��,����hoverͼƬ�Ŀ��
	if((m_nTabType == TAB_TYPE_HORIZONTAL) && (m_pImageHover != NULL) && (m_nTabItemWidth == 0))
	{
		m_nTabItemWidth = m_sizeHover.cx;
	}else
	if((m_nTabType == TAB_TYPE_VERTICAL) && (m_pImageHover != NULL) && (m_nTabItemHeight == 0))
	{
		m_nTabItemHeight = m_sizeHover.cy;
	}

	BOOL bAllVisible = TRUE;
	
	// �����²��tabҳ�ڵ���Ϣ
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
			// ����Ѿ�������ͬ���ֵ�tabҳ,������
			continue;
		}

		CString strTabXml = pTabElem.attribute(_T("tabxml")).value();
		if(!strTabXml.IsEmpty())
		{
			// ��xml�ļ�����Ƕ�׵�tabҳ
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
		// visible���Կ�����visible��show
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

		// ����Panel�ؼ���ÿ��Tabҳ�����Զ�����һ��Panel�ؼ�����ʹû�м�����XML�ڵ�
		CDuiPanel* pControlPanel = (CDuiPanel*)_CreateControlByName(_T("div"));
		pControlPanel->SetName(strName);	// div�ؼ�����������Ϊtab������
		pControlPanel->SetEnableScroll(bEnableScroll);
		m_vecControl.push_back(pControlPanel);
		if(!strDivXml.IsEmpty())
		{
 			pControlPanel->LoadXmlFile(strDivXml);			
		}

		// ����XML��Tab�ڵ�ĸ����²�ؼ��ڵ�
		pControlPanel->Load(pTabElem);

		CString strTitle = pControlPanel->GetTitle();
		
		// ͨ��Skin��ȡ
		CString strSkin = _T("");
		if(strImage.Find(_T("skin:")) == 0)
		{
			strSkin = DuiSystem::Instance()->GetSkin(strImage);
		}else
		{
			strSkin = strImage;
		}

		if(strSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
		{
			CString strImgFile = strSkin;
			if(strSkin.Find(_T(":")) != -1)
			{
				strImgFile = strSkin;
			}
			InsertItem(-1, nId, strName, strTitle, strAction, strImgFile, pControlPanel, nImageCount, bOutLink);
		}else
		if(nImageIndex != -1)	// ����ͼƬ
		{
			InsertItem(-1, nId, strName, strTitle, strAction, nImageIndex, pControlPanel, bOutLink);
		}else
		if(!strSkin.IsEmpty())	// ͼƬ��Դ
		{
			UINT uResourceID = _ttoi(strSkin);
			InsertItem(-1, nId, strName, strTitle, strAction, uResourceID, pControlPanel, nImageCount, bOutLink);
		}else
		if(strSkin.IsEmpty())	// ͼƬΪ��
		{
			InsertItem(-1, nId, strName, strTitle, strAction, _T(""), pControlPanel, nImageCount, bOutLink);
		}

		TabItemInfo &itemInfo = m_vecItemInfo.at(nIdIndex);
		itemInfo.bVisible = bVisible;
		if(!bVisible)
		{
			bAllVisible = FALSE;
		}

		if(strActive == _T("true"))	// ����Ϊ��ǰ���ҳ��
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

	// ��������ڼ���Ƕ��tabҳ����,����Ҫ���������
	if(!bLoadSubControl)
	{
		return TRUE;
	}

	// ֻ��ʾ��ǰ���tabҳ��Ӧ��Panel��������ҳ���Panel��������
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

	// �����ҳ�������ص�,����Ҫˢ������ҳ��
	if(!bAllVisible)
	{
		RefreshItems();
	}

	m_bInit = TRUE;

    return TRUE;
}

// ��Ƕ�׵�xml�ļ��м���tabҳ
BOOL CDuiTabCtrl::LoadTabXml(CString strFileName)
{
	// ���Ƚ�����tabҳ������
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
			// �����²�tabҳ
			Load(pTabElem, FALSE);
		}
	}

	// ֻ��ʾ��ǰ���tabҳ��Ӧ��Panel��������ҳ���Panel��������
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

	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	RefreshItems();

	return TRUE;
}

// ����tabҳ(ʹ����ԴͼƬ)
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

// ����tabҳ(ʹ��ͼƬ�ļ�)
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

// ����tabҳ(����ͼƬ��ʽ)
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

// ����tabҳ
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

		if(m_nTabType == TAB_TYPE_HORIZONTAL)	// ˮƽģʽ
		{
			if(m_nTabItemWidth == 0)
			{
				// ���tabctrlû������ÿ��tabҳ�Ŀ��,���Ե�һ��tabҳ��ͼƬ���Ϊ׼
				m_nTabItemWidth = nItemWidth;
			}
			if(m_nTabCtrlHeight == 0)
			{
				// ���tabctrlû�����ø߶�,���Ե�һ��tabҳ��ͼƬ�߶�Ϊ׼
				m_nTabCtrlHeight = nItemHeight;
			}

			// ����tabҳǩ��λ��
			itemInfoTemp.rc.SetRect(nXPos, nYPos, nXPos + m_nTabItemWidth, nYPos + m_nTabCtrlHeight);

			// ����Tabҳǩ��ť��λ��
			if(m_pImageTabBtn != NULL)
			{
				int nLeft = nXTabBtnPos;
				int nTop = nYTabBtnPos + 3;

				// ����Tabҳǩ��ť����ʾλ��
				if(m_posTabBtn.nCount >= 2)
				{
					// ʹ�����õ�Tabҳǩ��ťλ��
					int nTabBtnX = PositionItem2Value(m_posTabBtn.Left, 0, m_nTabItemWidth);
					int nTabBtnY = PositionItem2Value(m_posTabBtn.Top, 0, m_nTabCtrlHeight);
					nLeft += nTabBtnX;
					nTop += nTabBtnY;
				}else
				{
					// ���û������λ����Ϣ,������Ĭ�ϵ�λ��Ϊ��tabҳǩ���Ͻ�
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
		if(m_nTabType == TAB_TYPE_VERTICAL)	// ��ֱģʽ
		{
			if(m_nTabItemHeight == 0)
			{
				// ���tabctrlû������ÿ��tabҳ�ĸ߶�,���Ե�һ��tabҳ��ͼƬ�߶�Ϊ׼
				m_nTabItemHeight = nItemHeight;
			}
			if(m_nTabCtrlWidth == 0)
			{
				// ���tabctrlû�����ÿ��,���Ե�һ��tabҳ��ͼƬ���Ϊ׼
				m_nTabCtrlWidth = nItemWidth;
			}

			// ����tabҳǩ��λ��
			itemInfoTemp.rc.SetRect(nXPos, nYPos, nXPos + m_nTabCtrlWidth, nYPos + m_nTabItemHeight);

			// ����Tabҳǩ��ť��λ��
			if(m_pImageTabBtn != NULL)
			{
				int nLeft = nXTabBtnPos;
				int nTop = nYTabBtnPos + (m_nTabItemHeight * i) + 3;

				// ����Tabҳǩ��ť����ʾλ��
				if(m_posTabBtn.nCount >= 2)
				{
					// ʹ�����õ�Tabҳǩ��ťλ��
					int nTabBtnX = PositionItem2Value(m_posTabBtn.Left, 0, m_nTabCtrlWidth);
					int nTabBtnY = PositionItem2Value(m_posTabBtn.Top, 0, m_nTabItemHeight);
					nLeft += nTabBtnX;
					nTop += nTabBtnY;
				}else
				{
					// ���û������λ����Ϣ,������Ĭ�ϵ�λ��Ϊ��tabҳǩ���Ͻ�
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
		// ����tabҳǩ�Ŀ��
		SetItemWidth(m_nTabItemMaxWidth, m_nTabItemMinWidth);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)
	{
		// ����tabҳǩ�ĸ߶�
		SetItemHeight(m_nTabItemMaxHeight, m_nTabItemMinHeight);
	}

	UpdateControl(true);
	return true;
}

// ��ȡtabҳǩ����
int CDuiTabCtrl::GetItemCount()
{
	return m_vecItemInfo.size();
}

// ����tab���ֻ�ȡ����
int CDuiTabCtrl::GetItemIndex(CString strTabName)
{
	// �������Ϊ��,���ò���,ֱ�ӷ���-1
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

// ��ȡָ��tabҳ��Ϣ
TabItemInfo* CDuiTabCtrl::GetItemInfo(int nItem)
{
	if((nItem < 0) || (nItem >= (int)m_vecItemInfo.size()))
	{
		return NULL;
	}

	TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
	return &itemInfo;
}

// ����ѡ���tabҳ
int CDuiTabCtrl::SetSelectItem(int nItem)
{
	int nOldDownItem = m_nDownItem;
	if(m_nDownItem != nItem && nItem >= 0 && nItem < (int)m_vecItemInfo.size())
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);

		if(!itemInfo.strAction.IsEmpty())
		{
			// ���action�ǿ�,��ִ�ж���
			DuiSystem::AddDuiActionTask(GetID(), MSG_BUTTON_UP, nItem, 0, GetName(), itemInfo.strAction, GetParent());
		}

		if(itemInfo.bOutLink)	// �ⲿ����
		{
			m_nHoverItem = -1;
			SendMessage(MSG_BUTTON_DOWN, nItem, 0);
		}else
		{
			m_nOldItem = m_nDownItem;	// �����л�ǰ��ҳ������,�����л�����
			m_nDownItem = nItem;					
			m_nHoverItem = -1;

			// ɾ���ɵ�Tooltip
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

			// ����¼���Ϣ
			SendMessage(MSG_BUTTON_DOWN, m_nDownItem, 0);

			// ֻ��ʾ��ǰ���tabҳ��Ӧ��Panel��������ҳ���Panel��������
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
				// ��������˶���,�������л�������ʱ��
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

// ��ȡѡ���tabҳ
int CDuiTabCtrl::GetSelectItem()
{
	return m_nDownItem;
}

// ����Tabҳǩ�Ŀ��(ˮƽģʽ)
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
		// ���������ҳǩ����С���,�����ҳǩʵ�ʿ��
		int nTabTotalWidth = m_rc.Width() - m_nTabLeftPading - m_nTabRightPading;
		int nTabItemCount = GetItemCount();
		int nTabItemAveWidth = nTabTotalWidth / nTabItemCount;
		if((nTabItemAveWidth < m_nTabItemMaxWidth) && (nTabItemAveWidth >= m_nTabItemMinWidth))
		{
			// ���ƽ����Ƚ�����С��Ⱥ������֮��,������Ϊƽ�����
			nTabItemWidth = nTabItemAveWidth;
		}else
		if(nTabItemAveWidth < m_nTabItemMinWidth)
		{
			// ���ƽ�����С����С���,������Ϊ��С���
			nTabItemWidth = m_nTabItemMinWidth;
		}
	}

	// ���������Ŀ�Ⱥ�֮ǰ�Ŀ�Ȳ�һ��,��ˢ��tabҳ
	if(nTabItemWidth != m_nTabItemWidth)
	{
		m_nTabItemWidth = nTabItemWidth;
		if(bRefresh)	// �Ƿ�ˢ��tabҳ
		{
			RefreshItems();
		}
	}
}

// ����Tabҳǩ�ĸ߶�(��ֱģʽ)
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
		// ���������ҳǩ����С�߶�,�����ҳǩʵ�ʸ߶�
		int nTabTotalHeight = m_rc.Height() - m_nTabTopPading - m_nTabBottomPading;
		int nTabItemCount = GetItemCount();
		int nTabItemAveHeight = nTabTotalHeight / nTabItemCount;
		if((nTabItemAveHeight < m_nTabItemMaxHeight) && (nTabItemAveHeight >= m_nTabItemMinHeight))
		{
			// ���ƽ���߶Ƚ�����С�߶Ⱥ����߶�֮��,������Ϊƽ���߶�
			nTabItemHeight = nTabItemAveHeight;
		}else
		if(nTabItemAveHeight < m_nTabItemMinHeight)
		{
			// ���ƽ���߶�С����С�߶�,������Ϊ��С�߶�
			nTabItemHeight = m_nTabItemMinHeight;
		}
	}

	// ���������ĸ߶Ⱥ�֮ǰ�ĸ߶Ȳ�һ��,��ˢ��tabҳ
	if(nTabItemHeight != m_nTabItemHeight)
	{
		m_nTabItemHeight = nTabItemHeight;
		if(bRefresh)	// �Ƿ�ˢ��tabҳ
		{
			RefreshItems();
		}
	}
}

// ˢ������Tabҳ
void CDuiTabCtrl::RefreshItems()
{
	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	int nXPos = m_rc.left + m_nTabLeftPading;
	int nYPos = m_rc.top + m_nTabTopPading;

	for(size_t i = 0; i < m_vecItemInfo.size(); i++)
	{
		TabItemInfo &itemInfoTemp = m_vecItemInfo.at(i);
		if(!itemInfoTemp.bVisible)
		{
			// ���ص�tabҳ��������Ϊ0
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

		if(m_nTabType == TAB_TYPE_HORIZONTAL)	// ˮƽģʽ
		{
			if(m_nTabItemWidth == 0)
			{
				// ���tabctrlû������ÿ��tabҳ�Ŀ��,���Ե�һ��tabҳ��ͼƬ���Ϊ׼
				m_nTabItemWidth = nItemWidth;
			}
			if(m_nTabCtrlHeight == 0)
			{
				// ���tabctrlû�����ø߶�,���Ե�һ��tabҳ��ͼƬ�߶�Ϊ׼
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

			// ����Tabҳǩ��ť��λ��
			if(m_pImageTabBtn != NULL)
			{
				int nLeft = m_nTabLeftPading + (m_nTabItemWidth * i);
				int nTop = 3;

				// ����Tabҳǩ��ť����ʾλ��
				if(m_posTabBtn.nCount >= 2)
				{
					// ʹ�����õ�Tabҳǩ��ťλ��
					int nTabBtnX = PositionItem2Value(m_posTabBtn.Left, 0, m_nTabItemWidth);
					int nTabBtnY = PositionItem2Value(m_posTabBtn.Top, 0, m_nTabCtrlHeight);
					nLeft += nTabBtnX;
					nTop += nTabBtnY;
				}else
				{
					// ���û������λ����Ϣ,������Ĭ�ϵ�λ��Ϊ��tabҳǩ���Ͻ�
					nLeft += (m_nTabItemWidth - m_sizeTabBtn.cx - 3);
					nTop += 3;
				}

				itemInfoTemp.rcButton.SetRect(nLeft, nTop, nLeft + m_sizeTabBtn.cx, nTop + m_sizeTabBtn.cy);
			}
		}else
		if(m_nTabType == TAB_TYPE_VERTICAL)	// ��ֱģʽ
		{
			if(m_nTabItemHeight == 0)
			{
				// ���tabctrlû������ÿ��tabҳ�ĸ߶�,���Ե�һ��tabҳ��ͼƬ�߶�Ϊ׼
				m_nTabItemHeight = nItemHeight;
			}
			if(m_nTabCtrlWidth == 0)
			{
				// ���tabctrlû�����ÿ��,���Ե�һ��tabҳ��ͼƬ���Ϊ׼
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

			// ����Tabҳǩ��ť��λ��
			if(m_pImageTabBtn != NULL)
			{
				int nLeft = m_nTabLeftPading;
				int nTop = m_nTabTopPading + (m_nTabItemHeight * i) + 3;

				// ����Tabҳǩ��ť����ʾλ��
				if(m_posTabBtn.nCount >= 2)
				{
					// ʹ�����õ�Tabҳǩ��ťλ��
					int nTabBtnX = PositionItem2Value(m_posTabBtn.Left, 0, m_nTabCtrlWidth);
					int nTabBtnY = PositionItem2Value(m_posTabBtn.Top, 0, m_nTabItemHeight);
					nLeft += nTabBtnX;
					nTop += nTabBtnY;
				}else
				{
					// ���û������λ����Ϣ,������Ĭ�ϵ�λ��Ϊ��tabҳǩ���Ͻ�
					nLeft += (m_nTabCtrlWidth - m_sizeTabBtn.cx - 3);
					nTop += 3;
				}

				itemInfoTemp.rcButton.SetRect(nLeft, nTop, nLeft + m_sizeTabBtn.cx, nTop + m_sizeTabBtn.cy);
			}
		}
	}

	// ���ɾ�������һ��ҳ��,��Ҫ���¼��㵱ǰҳ�������
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

	// ˢ����ʾ��ǰҳ
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

// ɾ��ָ��tabҳ
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
		// ����tabҳǩ�Ŀ��
		SetItemWidth(m_nTabItemMaxWidth, m_nTabItemMinWidth, FALSE);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)
	{
		// ����tabҳǩ�ĸ߶�
		SetItemHeight(m_nTabItemMaxHeight, m_nTabItemMinHeight, FALSE);
	}

	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	RefreshItems();
}

// ɾ��ָ��tabҳ(��������)
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
		// ����tabҳǩ�Ŀ��
		SetItemWidth(m_nTabItemMaxWidth, m_nTabItemMinWidth, FALSE);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)
	{
		// ����tabҳǩ�ĸ߶�
		SetItemHeight(m_nTabItemMaxHeight, m_nTabItemMinHeight, FALSE);
	}

	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	RefreshItems();
}

// ����tabҳ�Ŀɼ���
void CDuiTabCtrl::SetItemVisible(int nItem, BOOL bVisible)
{
	if((nItem < 0) || (nItem >= (int)m_vecItemInfo.size()))
	{
		return;
	}

	TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
	itemInfo.bVisible = bVisible;

	// ���¼���ÿ��tabҳ��λ��,��ˢ�½���
	RefreshItems();
}

// ����tabҳ�Ŀɼ���(����tab����)
void CDuiTabCtrl::SetItemVisible(CString strTabName, BOOL bVisible)
{
	int nItem = GetItemIndex(strTabName);
	if(nItem != -1)
	{
		SetItemVisible(nItem, bVisible);
	}
}

// ��ȡtabҳ�Ŀɼ���
BOOL CDuiTabCtrl::GetItemVisible(int nItem)
{
	if((nItem < 0) || (nItem >= (int)m_vecItemInfo.size()))
	{
		return FALSE;
	}

	TabItemInfo &itemInfo = m_vecItemInfo.at(nItem);
	return itemInfo.bVisible;
}

// ��ȡtabҳ�Ŀɼ���
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
		// ����tabҳǩ�Ŀ��
		SetItemWidth(m_nTabItemMaxWidth, m_nTabItemMinWidth, FALSE);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)
	{
		// ����tabҳǩ�ĸ߶�
		SetItemHeight(m_nTabItemMaxHeight, m_nTabItemMinHeight, FALSE);
	}

	// ���¼�������Tabҳ��λ��
	RefreshItems();
}

// �������ÿؼ��ɼ��Եĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiTabCtrl::SetControlVisible(BOOL bIsVisible)
{
	//__super::SetControlVisible(bIsVisible);

	if(bIsVisible)
	{
		// �������Ϊ�ɼ�״̬,����ɼ�ҳ�Ŀؼ�����Ϊ�ɼ�״̬
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
		// �������Ϊ����״̬,������ҳ��Ŀؼ�����Ҫ����
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

// �������ÿؼ�����״̬�ĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiTabCtrl::SetControlHide(BOOL bIsHide)
{
	//__super::SetControlHide(bIsHide);

	if(!bIsHide)
	{
		// �������Ϊ�ɼ�״̬,����ɼ�ҳ�Ŀؼ�����Ϊ�ɼ�״̬
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
		// �������Ϊ����״̬,������ҳ��Ŀؼ�����Ҫ����
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

// �ж�ָ��������λ���Ƿ���ĳһ��Tabҳǩ�İ�ť��
BOOL CDuiTabCtrl::PtInTabButton(CPoint point, TabItemInfo& itemInfo)
{
	CRect rc = itemInfo.rcButton;
	// �������ת��
	rc.OffsetRect(m_rc.left, m_rc.top);
	return rc.PtInRect(point);
}

// ��XML����Tabҳǩ��ťλ����Ϣ����
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

// ����Tabҳǩ��Tooltip
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

// ���Tooltip
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

// �ж�����Ƿ��ڿؼ�����Ӧ������
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

	// �ж������겻������tabҳ��Χ��,������Ӧ
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
				if(m_bTabTooltip)	// ����TabҳTooltip
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
				if(m_bTabTooltip)	// ����TabҳTooltip
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

	// ����Tabҳǩ�İ�ťͼƬ״̬
	if(m_rc.PtInRect(point) && (m_pImageTabBtn != NULL))
	{
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(i);
			// ����ƶ���tabҳǩ�İ�ť
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
	// �ж�Tabҳǩ�İ�ť���
	if(m_rc.PtInRect(point) && (m_pImageTabBtn != NULL))
	{
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			TabItemInfo &itemInfo = m_vecItemInfo.at(i);
			// �����tabҳǩ�İ�ť,������Ϣ
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
			// �л�����굱ǰ��tabҳ
			SetSelectItem(m_nHoverItem);
		}		
	}
	
	return false;
}

BOOL CDuiTabCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	return false;
}

// �����¼�����
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

// �����¼�����
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

// ���ؼ�
void CDuiTabCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	if(m_nTabType == TAB_TYPE_HORIZONTAL)	// ˮƽģʽ
	{
		DrawControlHorizontal(dc, rcUpdate);
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)	// ��ֱģʽ
	{
		DrawControlVertical(dc, rcUpdate);
	}
}

// ���ؼ�(ˮƽģʽ)
void CDuiTabCtrl::DrawControlHorizontal(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	//int nHeight = m_rc.Height();	// �����ڴ�DC�ĸ߶�������tabctrl�ĸ߶�,��ֻ��ҳǩ���ָ߶�
	int nTabHeight = m_nTabCtrlHeight;	// �����ڴ�DC�߶ȸ�Ϊtabctrl���ָ߶�

	if(!m_bUpdate)
	{
		// �����ڴ�DC,�����Ϊ6��:
		// 1.tabҳǩͼƬ-ԭͼ
		// 2.tabҳǩͼƬ-����ȵ�
		// 3.tabҳǩͼƬ-��갴��
		// 4,5,6��������3��ı���
		UpdateMemDC(dc, nWidth, nTabHeight * 3 * 2);

		Graphics graphics(m_memDC);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		//SolidBrush solidBrush(m_clrText);			// �������ֻ�ˢ

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// ����tabҳǩ���ֵ�ˮƽ�ʹ�ֱ���뷽ʽ
		DUI_STRING_ALIGN_DEFINE();
		strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
		
		// ���ڴ�DC��3���������
		for(int i = 0; i < 3; i++)
		{
			// ���������ݿ������ڴ�DC
			m_memDC.BitBlt(0, i * nTabHeight, nWidth, nTabHeight, &dc, m_rc.left, m_rc.top, SRCCOPY);

			// ��tabҳǩ
			int nXPos = m_nTabLeftPading;
			int nYPos = m_nTabTopPading + i * nTabHeight;
			for(size_t j = 0; j < m_vecItemInfo.size(); j++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(j);
				
				if(!itemInfo.bVisible)
				{
					continue;
				}

				// ͼƬλ��(���ݶ��뷽ʽ���м���)
				CPoint point = GetOriginPoint(m_nTabItemWidth, m_nTabCtrlHeight, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy,
						GetGDIAlignment(m_uAlignment), GetGDIVAlignment(m_uVAlignment));
				// �����ͼƬ������,��ͼƬ�Ĵ�ֱ���밴���϶��뷽ʽ
				if(!itemInfo.strText.IsEmpty())
				{
					point.y = 0;
				}

				// ��tabҳǩ��ͼ
				if(itemInfo.pImage != NULL)	// ���ҳǩ������ͼƬ,��ʹ��tabҳǩָ����ͼƬ
				{
					int nImageIndex = i;
					if(itemInfo.nImageCount == 1)
					{
						nImageIndex = 0;
					}
					graphics.DrawImage(itemInfo.pImage, Rect(nXPos + point.x, nYPos + point.y,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
						itemInfo.sizeImage.cx * nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
				}else
				if((m_pImage != NULL) && (itemInfo.nImageIndex != -1))	// ���������ҳǩͼƬ����,ʹ��tabctrlͼƬ������ͼƬ
				{
					if(m_enTabImageMode == enTIMNormal)	// ��ͨģʽ
					{
						graphics.DrawImage(m_pImage, Rect(nXPos + point.x, nYPos + point.y,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
							itemInfo.sizeImage.cx * itemInfo.nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
					}else
					if(m_enTabImageMode == enTIMMID)	// �Ź���ģʽ
					{
						CRect rcTemp(nXPos, nYPos, nXPos+m_nTabItemWidth, nYPos+m_nTabCtrlHeight);
						DrawImageFrameMID(graphics, m_pImage, rcTemp, m_sizeImage.cx * i, 0, m_sizeImage.cx, m_sizeImage.cy,
							m_nWLT, m_nHLT, m_nWRB, m_nHRB);
					}
				}

				// ��tabҳǩ�ȵ�ͼ(�������tabctrl���õ��ȵ�ͼ�Ļ�)
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

				// ��tabҳǩ����
				if(!itemInfo.strText.IsEmpty())
				{
					// ����ҳǩ������ɫ
					SolidBrush solidBrushItem(m_clrText);
					if((m_nHoverItem == j) && (m_clrTextHover.GetValue() != Color(0, 0, 0, 0).GetValue()))	// ����������ƶ���ɫ,��ʹ��
					{
						solidBrushItem.SetColor(m_clrTextHover);
					}else
					if((m_nDownItem == j) && (m_clrTextDown.GetValue() != Color(0, 0, 0, 0).GetValue()))	// ��������갴����ɫ,��ʹ��
					{
						solidBrushItem.SetColor(m_clrTextDown);
					}

					RectF rectText((Gdiplus::REAL)nXPos,
							(Gdiplus::REAL)(nYPos + itemInfo.sizeImage.cy + 1),
							(Gdiplus::REAL)((m_pImageTabBtn != NULL) ? (itemInfo.rc.Width()-m_sizeTabBtn.cx) : itemInfo.rc.Width()),
							(Gdiplus::REAL)(m_nTabCtrlHeight - itemInfo.sizeImage.cy - 1));
					if(m_nTabCtrlHeight <= itemInfo.sizeImage.cy)
					{
						// ���tabctrl�߶�С��ͼƬ�߶�,������ֱ�Ӿ�����ʾ
						rectText.Y = (Gdiplus::REAL)nYPos;
						rectText.Height = (Gdiplus::REAL)m_nTabCtrlHeight;
					}

					// �����Ƿ���Ҫ��ʾtip
					itemInfo.bNeedTextTip = rectText.Width < GetTextBounds(font, itemInfo.strText).Width;

					BSTR bsText = itemInfo.strText.AllocSysString();
					graphics.DrawString(bsText, (INT)wcslen(bsText), &font, rectText, &strFormat, &solidBrushItem);
					::SysFreeString(bsText);
				}

				nXPos += itemInfo.rc.Width();

				// ��tabҳǩ֮��ķָ�ͼƬ(�������췽ʽ)
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

		// �ڴ�dc����һ�ݽ��б���
		m_memDC.BitBlt(0, nTabHeight * 3, nWidth, nTabHeight * 3, &m_memDC, 0, 0, SRCCOPY);
	}

	// ��Tabҳǩ��ť���ڴ�dc
	if(m_pImageTabBtn != NULL)
	{
		// �ֽ����ݵ��ڴ�dc������лָ�,����ҳǩ��ť����֮���Ӱ��
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

	// �ڴ�dc�����dc
	// 1.��ԭͼ
	dc.BitBlt(m_rc.left,m_rc.top, nWidth, nTabHeight, &m_memDC, 0, 0, SRCCOPY);

	// 2.������ȵ��Tabҳǩ
	if((m_nHoverItem != -1) && (m_nHoverItem < (int)m_vecItemInfo.size()))
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nHoverItem);

		dc.BitBlt(itemInfo.rc.left, itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC,
			itemInfo.rc.left - m_rc.left, itemInfo.rc.top - m_rc.top + nTabHeight, SRCCOPY);
	}

	// 3.����갴�µ�Tabҳǩ
	if((m_nDownItem != -1) && (m_nDownItem < (int)m_vecItemInfo.size()))
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);

		dc.BitBlt(itemInfo.rc.left, itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC,
			itemInfo.rc.left - m_rc.left, itemInfo.rc.top - m_rc.top + nTabHeight * 2, SRCCOPY);
	}
}

// ���ؼ�(��ֱģʽ)
void CDuiTabCtrl::DrawControlVertical(CDC &dc, CRect rcUpdate)
{
	int nHeight = m_rc.Height();
	int nTabWidth = m_nTabCtrlWidth;	// �����ڴ�DC��ȸ�Ϊtabctrl���ֿ��

	if(!m_bUpdate)
	{
		// �����ڴ�DC,�����Ϊ6��:
		// 1.tabҳǩͼƬ-ԭͼ
		// 2.tabҳǩͼƬ-����ȵ�
		// 3.tabҳǩͼƬ-��갴��
		// 4,5,6��������3��ı���
		UpdateMemDC(dc, nTabWidth * 3 * 2, nHeight);

		Graphics graphics(m_memDC);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// ����tabҳǩ���ֵ�ˮƽ�ʹ�ֱ���뷽ʽ
		DUI_STRING_ALIGN_DEFINE();
		strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
		
		// ���ڴ�DC��3���������
		for(int i = 0; i < 3; i++)
		{
			// ���������ݿ������ڴ�DC
			m_memDC.BitBlt(i * nTabWidth, 0, nTabWidth, nHeight, &dc, m_rc.left, m_rc.top, SRCCOPY);

			// ��tabҳǩ
			int nXPos = m_nTabLeftPading + i * nTabWidth;
			int nYPos = m_nTabTopPading;
			for(size_t j = 0; j < m_vecItemInfo.size(); j++)
			{
				TabItemInfo &itemInfo = m_vecItemInfo.at(j);
				
				if(!itemInfo.bVisible)
				{
					continue;
				}

				// ͼƬλ��(���ݶ��뷽ʽ���м���)
				CPoint point = GetOriginPoint(m_nTabCtrlWidth, m_nTabItemHeight, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy,
						GetGDIAlignment(m_uAlignment), GetGDIVAlignment(m_uVAlignment));
				// �����ͼƬ������,��ͼƬ��ˮƽ���밴������뷽ʽ
				if(!itemInfo.strText.IsEmpty())
				{
					point.x = 0;
				}

				// ��tabҳǩ��ͼ
				if(itemInfo.pImage != NULL)	// ���ҳǩ������ͼƬ,��ʹ��tabҳǩָ����ͼƬ
				{
					int nImageIndex = i;
					if(itemInfo.nImageCount == 1)
					{
						nImageIndex = 0;
					}
					graphics.DrawImage(itemInfo.pImage, Rect(nXPos + point.x, nYPos + point.y,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
						itemInfo.sizeImage.cx * nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
				}else
				if((m_pImage != NULL) && (itemInfo.nImageIndex != -1))	// ���������ҳǩͼƬ����,ʹ��tabctrlͼƬ������ͼƬ
				{
					if(m_enTabImageMode == enTIMNormal)	// ��ͨģʽ
					{
						graphics.DrawImage(m_pImage, Rect(nXPos + point.x, nYPos + point.y,  itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
							itemInfo.sizeImage.cx * itemInfo.nImageIndex, 0, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy, UnitPixel);
					}else
					if(m_enTabImageMode == enTIMMID)	// �Ź���ģʽ
					{
						CRect rcTemp(nXPos, nYPos, nXPos+m_nTabCtrlWidth, nYPos+m_nTabItemHeight);
						DrawImageFrameMID(graphics, m_pImage, rcTemp, m_sizeImage.cx * i, 0, m_sizeImage.cx, m_sizeImage.cy,
							m_nWLT, m_nHLT, m_nWRB, m_nHRB);
					}
				}

				// ��tabҳǩ�ȵ�ͼ(�������tabctrl���õ��ȵ�ͼ�Ļ�)
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

				// ��tabҳǩ����
				if(!itemInfo.strText.IsEmpty())
				{
					// ����ҳǩ������ɫ
					SolidBrush solidBrushItem(m_clrText);
					if((m_nHoverItem == j) && (m_clrTextHover.GetValue() != Color(0, 0, 0, 0).GetValue()))	// ����������ƶ���ɫ,��ʹ��
					{
						solidBrushItem.SetColor(m_clrTextHover);
					}else
					if((m_nDownItem == j) && (m_clrTextDown.GetValue() != Color(0, 0, 0, 0).GetValue()))	// ��������갴����ɫ,��ʹ��
					{
						solidBrushItem.SetColor(m_clrTextDown);
					}

					RectF rectText((Gdiplus::REAL)(nXPos + itemInfo.sizeImage.cx + 1),
							(Gdiplus::REAL)nYPos,
							(Gdiplus::REAL)(m_nTabCtrlWidth - itemInfo.sizeImage.cx - 1),
							(Gdiplus::REAL)((m_pImageTabBtn != NULL) ? (itemInfo.rc.Height()-m_sizeTabBtn.cy) : itemInfo.rc.Height()));
					if(m_nTabCtrlWidth <= itemInfo.sizeImage.cx)
					{
						// ���tabctrl���С��ͼƬ���,������ֱ�Ӿ�����ʾ
						rectText.X = (Gdiplus::REAL)nXPos;
						rectText.Width = (Gdiplus::REAL)m_nTabCtrlWidth;
					}

					// �����Ƿ���Ҫ��ʾtip
					itemInfo.bNeedTextTip = rectText.Width < GetTextBounds(font, itemInfo.strText).Width;

					BSTR bsText = itemInfo.strText.AllocSysString();
					graphics.DrawString(bsText, (INT)wcslen(bsText), &font, rectText, &strFormat, &solidBrushItem);
					::SysFreeString(bsText);
				}

				nYPos += itemInfo.rc.Height();

				// ��tabҳǩ֮��ķָ�ͼƬ(�������췽ʽ)
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

		// �ڴ�dc����һ�ݽ��б���
		m_memDC.BitBlt(nTabWidth * 3, 0, nTabWidth * 3, nHeight, &m_memDC, 0, 0, SRCCOPY);
	}

	// ��Tabҳǩ��ť���ڴ�dc
	if(m_pImageTabBtn != NULL)
	{
		// �ֽ����ݵ��ڴ�dc������лָ�,����ҳǩ��ť����֮���Ӱ��
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

	// �ڴ�dc�����dc
	// 1.��ԭͼ
	dc.BitBlt(m_rc.left,m_rc.top, nTabWidth, nHeight, &m_memDC, 0, 0, SRCCOPY);

	// 2.������ȵ��Tabҳǩ
	if((m_nHoverItem != -1) && (m_nHoverItem < (int)m_vecItemInfo.size()))
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nHoverItem);

		dc.BitBlt(itemInfo.rc.left, itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC,
			itemInfo.rc.left - m_rc.left + nTabWidth, itemInfo.rc.top - m_rc.top, SRCCOPY);
	}

	// 3.����갴�µ�Tabҳǩ
	if((m_nDownItem != -1) && (m_nDownItem < (int)m_vecItemInfo.size()))
	{
		TabItemInfo &itemInfo = m_vecItemInfo.at(m_nDownItem);

		dc.BitBlt(itemInfo.rc.left, itemInfo.rc.top, itemInfo.rc.Width(), itemInfo.rc.Height(), &m_memDC,
			itemInfo.rc.left - m_rc.left + nTabWidth * 2, itemInfo.rc.top - m_rc.top, SRCCOPY);
	}
}

// �ؼ��Ķ�����ʱ��
BOOL CDuiTabCtrl::OnControlTimer()
{
	if(!m_bRunTime || (m_nOldItem == -1))
	{
		return FALSE;
	}

	BOOL bFinish = FALSE;
	if(m_nTabType == TAB_TYPE_HORIZONTAL)	// ˮƽģʽ
	{
		int nAnimatePos = m_rc.Width() / m_nAnimateCount;
		if(m_nDownItem > m_nOldItem)	// �����л�
		{
			m_nCurXPos += nAnimatePos;
			if(m_nCurXPos >= m_rc.Width())
			{
				bFinish = TRUE;
			}
		}else	// �����л�
		{
			m_nCurXPos -= nAnimatePos;
			if((m_rc.Width() + m_nCurXPos) <= 0)
			{
				bFinish = TRUE;
			}
		}
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)	// ��ֱģʽ
	{
		int nAnimatePos = m_rc.Height() / m_nAnimateCount;
		if(m_nDownItem > m_nOldItem)	// �����л�
		{
			m_nCurYPos += nAnimatePos;
			if(m_nCurYPos >= m_rc.Height())
			{
				bFinish = TRUE;
			}
		}else	// �����л�
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
		// �����л�������ʱ��
		m_bRunTime = false;
	}

	UpdateControl();

	return true;
}

// ���ӿؼ�(֧���л�tabҳ�Ķ�����ʾ)
BOOL CDuiTabCtrl::DrawSubControls(CDC &dc, CRect rcUpdate)
{
	if(!m_bRunTime)
	{
		// ���û�д��ڶ�������,��ֱ�ӻ������ӿؼ�
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

	// ��ʼ����tabҳ����dc,�����Ʊ���������dc
	CBitmap	aniBitmapOld;
	CDC animateDCOld;
	animateDCOld.CreateCompatibleDC(&dc);
	aniBitmapOld.CreateCompatibleBitmap(&dc, m_rc.right, m_rc.bottom);
	CBitmap* pOldAniBitmapOld = animateDCOld.SelectObject(&aniBitmapOld);
	animateDCOld.BitBlt(nLeft, nTop, nWidth, nHeight, &dc, nLeft, nTop, SRCCOPY);	// �������Ƶ�����dc

	// ��ʼ����tabҳ����dc,�����Ʊ���������dc
	CBitmap	aniBitmapNew;
	CDC animateDCNew;
	animateDCNew.CreateCompatibleDC(&dc);
	aniBitmapNew.CreateCompatibleBitmap(&dc, m_rc.right, m_rc.bottom);
	CBitmap* pOldAniBitmapNew = animateDCNew.SelectObject(&aniBitmapNew);
	animateDCNew.BitBlt(nLeft, nTop, nWidth, nHeight, &dc, nLeft, nTop, SRCCOPY);	// �������Ƶ�����dc

	// ����tabҳ
	// ���þ�tabҳΪ�ɼ�
	pOldTabInfo->pControl->SetVisible(TRUE);
	pNewTabInfo->pControl->SetVisible(FALSE);
	// ����tabҳ������dc
	pOldTabInfo->pControl->Draw(animateDCOld, rcUpdate);
	// ����dc���Ƶ�dc
	int nTabTop = pOldTabInfo->pControl->GetRect().top;
	int nTabLeft = pOldTabInfo->pControl->GetRect().left;
	if(m_nTabType == TAB_TYPE_HORIZONTAL)	// ˮƽģʽ
	{
		if(m_nCurXPos > 0)
		{
			dc.BitBlt(nLeft, nTabTop, nWidth-m_nCurXPos, m_rc.bottom-nTabTop, &animateDCOld, nLeft+m_nCurXPos, nTabTop, SRCCOPY);
		}else
		{
			dc.BitBlt(nLeft-m_nCurXPos, nTabTop, nWidth+m_nCurXPos, m_rc.bottom-nTabTop, &animateDCOld, nLeft, nTabTop, SRCCOPY);
		}
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)	// ��ֱģʽ
	{
		if(m_nCurYPos > 0)
		{
			dc.BitBlt(nTabLeft, nTop, m_rc.right-nTabLeft, nHeight-m_nCurYPos, &animateDCOld, nTabLeft, nTop+m_nCurYPos, SRCCOPY);
		}else
		{
			dc.BitBlt(nTabLeft, nTop-m_nCurYPos, m_rc.right-nTabLeft, nHeight+m_nCurYPos, &animateDCOld, nTabLeft, nTop, SRCCOPY);
		}
	}

	// �ͷž�tabҳ����dc
	animateDCOld.SelectObject(pOldAniBitmapOld);
	animateDCOld.DeleteDC();
	aniBitmapOld.DeleteObject();

	// ����tabҳ
	//UpdateAnimateDC(dc, m_rc.right, m_rc.bottom);	// ��ʼ������dc
	//m_animateDC.BitBlt(nLeft, nTop, nWidth, nHeight, &dc, nLeft, nTop, SRCCOPY);	// �������Ƶ�����dc
	// ������tabҳΪ�ɼ�
	pOldTabInfo->pControl->SetVisible(FALSE);
	pNewTabInfo->pControl->SetVisible(TRUE);
	// ����tabҳ������dc
	pNewTabInfo->pControl->Draw(animateDCNew, rcUpdate);
	// ����dc���Ƶ�dc
	nTabTop = pNewTabInfo->pControl->GetRect().top;
	if(m_nTabType == TAB_TYPE_HORIZONTAL)	// ˮƽģʽ
	{
		if(m_nCurXPos > 0)
		{
			dc.BitBlt(nLeft+nWidth-m_nCurXPos, nTabTop, m_nCurXPos, m_rc.bottom-nTabTop, &animateDCNew, nLeft, nTabTop, SRCCOPY);
		}else
		{
			dc.BitBlt(nLeft+0, nTabTop, -m_nCurXPos, m_rc.bottom-nTabTop, &animateDCNew, nLeft+nWidth+m_nCurXPos, nTabTop, SRCCOPY);
		}
	}else
	if(m_nTabType == TAB_TYPE_VERTICAL)	// ��ֱģʽ
	{
		if(m_nCurYPos > 0)
		{
			dc.BitBlt(nTabLeft, nTop+nHeight-m_nCurYPos, m_rc.right-nTabLeft, m_nCurYPos, &animateDCNew, nTabLeft, nTop, SRCCOPY);
		}else
		{
			dc.BitBlt(nTabLeft, nTop+0, m_rc.right-nTabLeft, -m_nCurYPos, &animateDCNew, nTabLeft, nTop+nHeight+m_nCurYPos, SRCCOPY);
		}
	}

	// �ͷ���tabҳ����dc
	animateDCNew.SelectObject(pOldAniBitmapNew);
	animateDCNew.DeleteDC();
	aniBitmapNew.DeleteObject();

	return TRUE;
}