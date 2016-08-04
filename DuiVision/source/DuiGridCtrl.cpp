#include "StdAfx.h"
#include "DuiListCtrl.h"

#define	SCROLL_V	1	// ��ֱ�������ؼ�ID
#define	SCROLL_H	2	// ˮƽ�������ؼ�ID
//#define	LISTBK_AREA	3	// ����Area�ؼ�ID

CDuiGridCtrl::CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiPanel(hWnd, pDuiObject)
{
	m_strFontTitle = DuiSystem::GetDefaultFont();
	m_nFontTitleWidth = 12;
	m_fontTitleStyle = FontStyleRegular;
	m_uAlignmentHeader = Align_Center;
	m_uVAlignmentHeader = VAlign_Middle;

	m_clrHeader = Color(255, 32, 32, 32);
	m_clrText = Color(225, 64, 64, 64);
	m_clrTextHover = Color(128, 0, 0);
	m_clrTextDown = Color(0, 112, 235);
	m_clrTitle = Color(255, 32, 32, 32);
	m_clrSeperator = Color(0, 0, 0, 0);
	m_clrRowHover = Color(0, 128, 128, 128);	// ����ƶ�������ʾ�ı���ɫ,Ĭ����͸��ɫ
	m_nRowHeight = 50;
	m_nHeaderHeight = 0;
	m_nLeftPos = 0;

	m_pImageHeader = NULL;
	m_sizeHeader = CSize(0, 0);
	m_pImageColumnSeperator = NULL;
	m_sizeColumnSeperator = CSize(0, 0);
	m_pImageSeperator = NULL;
	m_sizeSeperator = CSize(0, 0);
	m_pImageCheckBox = NULL;
	m_sizeCheckBox = CSize(0, 0);

	m_nBkTransparent = 30;

	m_bDblClk = true;

	m_nHoverRow = 0;
	m_nDownRow = -1;
	m_bEnableDownRow = FALSE;
	m_bSingleLine = TRUE;
	m_bTextWrap = FALSE;
	m_bShowColumnSeperator = FALSE;

	m_bGridTooltip = TRUE;
	m_nTipRow = -1;
	m_nTipItem = -1;
	m_nTipVirtualTop = 0;

	m_bEnableModifyColumn = FALSE;
	m_bHoverSplitColumn = false;
	m_nHoverSplitColumn = -1;
	m_enButtonState = enBSNormal;

	m_nFirstViewRow = 0;
	m_nLastViewRow = 0;
	m_nVirtualTop = 0;
	m_nVirtualLeft = 0;
}

CDuiGridCtrl::~CDuiGridCtrl(void)
{
	if(m_pImageHeader != NULL)
	{
		delete m_pImageHeader;
		m_pImageHeader = NULL;
	}
	if(m_pImageColumnSeperator != NULL)
	{
		delete m_pImageColumnSeperator;
		m_pImageColumnSeperator = NULL;
	}
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

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, Header, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, ColumnSeperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiGridCtrl, CheckBox, 6)

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiGridCtrl::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
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
		CString strTitle = pColumnElem.attribute(_T("title")).value();
		CString strClrText = pColumnElem.attribute(_T("crtext")).value();
		CString strWidth = pColumnElem.attribute(_T("width")).value();
		CString strAlign = pColumnElem.attribute(_T("align")).value();
		CString strVAlign = pColumnElem.attribute(_T("valign")).value();
		DuiSystem::Instance()->ParseDuiString(strTitle);
		Color clrText = Color(0, 0, 0, 0);
		if(!strClrText.IsEmpty())
		{
			clrText = CDuiObject::StringToColor(strClrText);
		}
		int nWidth = -1;
		if(!strWidth.IsEmpty())
		{
			nWidth = _ttoi(strWidth);
		}
		UINT uAlignment = 0xFFFFUL;
		if(strAlign == _T("left"))
		{
			uAlignment = Align_Left;
		}else
		if(strAlign ==_T("center"))
		{
			uAlignment = Align_Center;
		}else
		if(strAlign == _T("right"))
		{
			uAlignment = Align_Right;
		}
		UINT uVAlignment = 0xFFFFUL;
		if(strVAlign == _T("top"))
		{
			uVAlignment = VAlign_Top;
		}else
		if(strVAlign == _T("middle"))
		{
			uVAlignment = VAlign_Middle;
		}else
		if(strVAlign == _T("bottom"))
		{
			uVAlignment = VAlign_Bottom;
		}
		InsertColumn(-1, strTitle, nWidth, clrText, uAlignment, uVAlignment);
	}

	// �����²��row�ڵ���Ϣ
	for (DuiXmlNode pRowElem = pXmlElem.child(_T("row")); pRowElem; pRowElem=pRowElem.next_sibling(_T("row")))
	{
		CString strId = pRowElem.attribute(_T("id")).value();
		CString strCheck = pRowElem.attribute(_T("check")).value();
		CString strImage = pRowElem.attribute(_T("image")).value();
		CString strRightImage = pRowElem.attribute(_T("right-img")).value();
		CString strClrText = pRowElem.attribute(_T("crtext")).value();
		CString strClrBack = pRowElem.attribute(_T("crback")).value();

		int nCheck = -1;
		if(!strCheck.IsEmpty())
		{
			nCheck = _ttoi(strCheck);
		}

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

		InsertRow(-1, strId, nImageIndex, clrText, strImage, nRightImageIndex, strRightImage, nCheck, clrBack);

		int nRowIndex = m_vecRowInfo.size()-1;
		int nItemIndex = 0;
		// �����²��item�ڵ���Ϣ
		for (DuiXmlNode pItemElem = pRowElem.child(_T("item")); pItemElem; pItemElem=pItemElem.next_sibling(_T("item")))
		{
			CString strTitle = pItemElem.attribute(_T("title")).value();
			CString strContent = pItemElem.attribute(_T("content")).value();
			CString strClrText = pItemElem.attribute(_T("crtext")).value();
			CString strImage = pItemElem.attribute(_T("image")).value();
			CString strLink = pItemElem.attribute(_T("link")).value();
			CString strLinkAction = pItemElem.attribute(_T("linkaction")).value();
			CString strFontTitle = pItemElem.attribute(_T("font-title")).value();
			DuiSystem::Instance()->ParseDuiString(strTitle);
			DuiSystem::Instance()->ParseDuiString(strContent);
			DuiSystem::Instance()->ParseDuiString(strLink);
			DuiSystem::Instance()->ParseDuiString(strLinkAction);
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

			BOOL bUseTitleFont = (strFontTitle == _T("1"));

			if(!strLink.IsEmpty())
			{
				SetSubItemLink(nRowIndex, nItemIndex, strLink, strLinkAction, nImageIndex, clrText, strImage);
			}else
			{
				SetSubItem(nRowIndex, nItemIndex, strTitle, strContent, bUseTitleFont, nImageIndex, clrText, strImage);
			}

			// �����²�Ŀؼ��ڵ���Ϣ
			GridItemInfo* pItemInfo = GetItemInfo(nRowIndex, nItemIndex);
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
	}

	// ������������
	CalcColumnsPos();
	// ����ÿһ�е�λ�ú͹�����
	CalcRowsPos();

    return TRUE;
}

// �����
BOOL CDuiGridCtrl::InsertColumn(int nColumn, CString strTitle, int nWidth, Color clrText, UINT uAlignment, UINT uVAlignment)
{
	GridColumnInfo columnInfo;
	columnInfo.strTitle = strTitle;
	columnInfo.clrText = clrText;
	columnInfo.nWidth = nWidth;
	columnInfo.uAlignment = uAlignment;
	columnInfo.uVAlignment = uVAlignment;
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

	for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
	{
		GridColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
		int nWidth = columnInfoTemp.nWidth;
		if(nWidth == -1)	// -1��ʾ���һ��Ϊ����Ӧ���
		{
			nWidth = m_rc.Width() - nXPos;
			if(nWidth < 0)
			{
				nWidth = 100;	// �����Ȳ���,����һ����Сֵ
			}
		}
		columnInfoTemp.rcHeader.SetRect(nXPos, nYPos, nXPos + nWidth, nYPos + m_nRowHeight);
		nXPos += columnInfoTemp.nWidth;
	}

	// ������������
	CalcColumnsPos();

	UpdateControl(true);
	return true;
}

// �����п��
int CDuiGridCtrl::SetColumnWidth(int nColumn, int nWidth, int nWidthNextColumn)
{
	int nXPos = 0;
	int nYPos = 0;

	int nWidthResult = 0;
	for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
	{
		GridColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
		if(i == nColumn)
		{
			columnInfoTemp.nWidth = nWidth;
		}else
		if((i == (nColumn+1)) && (nWidthNextColumn != -1))
		{
			columnInfoTemp.nWidth = nWidthNextColumn;
		}
		int _nWidth = columnInfoTemp.nWidth;
		if(_nWidth == -1)	// -1��ʾ���һ��Ϊ����Ӧ���
		{
			_nWidth = m_rc.Width() - nXPos;
			if(_nWidth < 0)
			{
				_nWidth = 100;	// �����Ȳ���,����һ����Сֵ
			}
		}
		if(i == nColumn)
		{
			nWidthResult = _nWidth;
		}
		columnInfoTemp.rcHeader.SetRect(nXPos, nYPos, nXPos + _nWidth, nYPos + m_nRowHeight);
		nXPos += columnInfoTemp.nWidth;
	}

	// ������������
	CalcColumnsPos();
	// ���¼��������е�λ��
	CalcRowsPos();

	UpdateControl(true);

	return nWidthResult;
}

// �ƶ��зָ���λ��
void CDuiGridCtrl::MoveColumnSplit(int nColumn, int nPos)
{
	if((size_t)nColumn < m_vecColumnInfo.size())
	{
		GridColumnInfo &columnInfo1 = m_vecColumnInfo.at(nColumn);
		int nWidth1 = columnInfo1.nWidth;
		if(nWidth1 == -1)	// -1��ʾ���һ��Ϊ����Ӧ���
		{
			nWidth1 = m_rc.Width() - columnInfo1.rcHeader.right;
			if(nWidth1 < 0)
			{
				nWidth1 = 100;	// �����Ȳ���,����һ����Сֵ
			}
		}
		nWidth1 += (nPos - columnInfo1.rcHeader.right);

		int nWidth2 = -1;
		if((size_t)(nColumn+1) < m_vecColumnInfo.size())
		{
			GridColumnInfo &columnInfo2 = m_vecColumnInfo.at(nColumn+1);
			nWidth2 = columnInfo2.nWidth;
		}

		if(nWidth1 < 0)
		{
			return;
		}

		// �����п�,������ǰ����п��,����Ĳ���
		SetColumnWidth(nColumn, nWidth1, nWidth2);
	}
}

// ��ȡ�ܵ��п�
int CDuiGridCtrl::GetTotalColumnWidth()
{
	int nTotalWidth = 0;

	for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
	{
		GridColumnInfo &columnInfoTemp = m_vecColumnInfo.at(i);
		int nWidth = columnInfoTemp.nWidth;
		if(nWidth == -1)	// -1��ʾ���һ��Ϊ����Ӧ���
		{
			nWidth = m_rc.Width() - nTotalWidth;
			if(nWidth < 0)
			{
				nWidth = 100;	// �����Ȳ���,����һ����Сֵ
			}
		}
		nTotalWidth += nWidth;
	}

	return nTotalWidth;
}

// �����
int CDuiGridCtrl::InsertRow(int nRow, CString strId, int nImageIndex, Color clrText, CString strImage,
							 int nRightImageIndex, CString strRightImage, int nCheck, Color clrBack)
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
	rowInfo.bRowBackColor = FALSE;
	rowInfo.clrBack = clrBack;
	rowInfo.nHoverItem = -1;
	if(clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		rowInfo.bRowColor = TRUE;
	}
	if(clrBack.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		rowInfo.bRowBackColor = TRUE;
	}

	// ���ͼƬ
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, rowInfo.pImage))
		{
			rowInfo.sizeImage.SetSize(rowInfo.pImage->GetWidth() / 1, rowInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		rowInfo.pImage = NULL;
		if((rowInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	// �ұ�ͼƬ
	if(!strRightImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		if(DuiSystem::Instance()->LoadImageFile(strRightImage, m_bImageUseECM, rowInfo.pRightImage))
		{
			rowInfo.sizeRightImage.SetSize(rowInfo.pRightImage->GetWidth() / 1, rowInfo.pRightImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		rowInfo.pRightImage = NULL;
		if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeRightImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	return InsertRow(nRow, rowInfo);
}

// �����
int CDuiGridCtrl::InsertRow(int nRow, GridRowInfo &rowInfo)
{
	int nRetRow = -1;
	if(nRow <= -1 || nRow >= (int)m_vecRowInfo.size())
	{
		m_vecRowInfo.push_back(rowInfo);
		nRetRow = m_vecRowInfo.size()-1;
	}
	else
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nRow, rowInfo);
		nRetRow = nRow;
	}

	// �������б���е�λ��
	CalcRowsPos();	

	UpdateControl(true);
	return nRetRow;
}

// ���ñ��������(���ֱ����)
BOOL CDuiGridCtrl::SetSubItem(int nRow, int nItem, CString strTitle, CString strContent, BOOL bUseTitleFont, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
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
		GridColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
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
BOOL CDuiGridCtrl::SetSubItemLink(int nRow, int nItem, CString strLink, CString strLinkAction, int nImageIndex, Color clrText, CString strImage)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
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
		GridColumnInfo &columnInfo = m_vecColumnInfo.at(i);
		itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfo.rcRow.top,
				columnInfo.rcHeader.right, rowInfo.rcRow.bottom);
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

// �����������ӿؼ�
BOOL CDuiGridCtrl::AddSubItemControl(int nRow, int nItem, CControlBase* pControl)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}
	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return FALSE;
	}

	GridItemInfo* pItemInfo = GetItemInfo(nRow, nItem);
	if(pItemInfo == NULL)
	{
		SetSubItem(nRow, nItem, _T(""));
		pItemInfo = GetItemInfo(nRow, nItem);
	}
	if(pItemInfo == NULL)
	{
		return FALSE;
	}

	if(pControl)
	{
		pControl->SetParent(this);
		pControl->SetVisible(FALSE);
		// ���ؼ�ָ����ӵ�gridctrl�ؼ����ӿؼ��б���
		m_vecControl.push_back(pControl);
		// ���ؼ�ָ����ӵ���Ԫ��Ŀؼ��б���(�����ڰ��յ�Ԫ������ӿؼ�)
		pItemInfo->vecControl.push_back(pControl);
		UpdateControl(true);
	}

	return TRUE;
}

// ɾ��������ָ���ӿؼ�(ͨ���ؼ�����ָ��ɾ��)
BOOL CDuiGridCtrl::DeleteSubItemControl(CControlBase* pControl)
{
	// �������е�Ԫ��,ɾ����Ӧ�Ŀؼ���������
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
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

// ɾ��������ָ���ӿؼ�(ͨ���ؼ����Ϳؼ�IDɾ��)
BOOL CDuiGridCtrl::DeleteSubItemControl(CString strControlName, UINT uControlID)
{
	// �������е�Ԫ��,ɾ����Ӧ�Ŀؼ���������
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
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

// ɾ����
BOOL CDuiGridCtrl::DeleteRow(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}

	// �����е����е�Ԫ��,ɾ����Ӧ���ӿؼ�����
	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
	{
		GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
		vector<CControlBase*>::iterator it;
		for(it=itemInfo.vecControl.begin(); it!=itemInfo.vecControl.end(); ++it)
		{
			CControlBase* _pControl = *it;
			// ɾ����Ԫ��������ӿؼ�
			RemoveControl(_pControl);
		}
	}

	// ɾ������Ϣ
	int nIndex = 0;
	vector<GridRowInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nRow)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}

	// �������б���е�λ��
	CalcRowsPos();

	UpdateControl(true);
	return true;
}

// ��������λ��
void CDuiGridCtrl::CalcRowsPos()
{
	int nXPos = 0;//m_rc.left;
	int nYPos = 0;//m_rc.top;

	// ����ÿһ�е�λ��
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		rowInfoTemp.rcCheck.SetRect(0,0,0,0);

		// ���㵥Ԫ��λ��
		for(size_t j = 0; j < rowInfoTemp.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfoTemp.vecItemInfo.at(j);
			GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
			itemInfo.rcItem.SetRect(columnInfo.rcHeader.left, rowInfoTemp.rcRow.top,
					columnInfo.rcHeader.right, rowInfoTemp.rcRow.bottom);
		}

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(((int)m_vecRowInfo.size() * m_nRowHeight) > (m_rc.Height() - m_nHeaderHeight));
	((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange(m_vecRowInfo.size() * m_nRowHeight);
}

// ��������λ��
void CDuiGridCtrl::CalcColumnsPos()
{
	int nTotalWidth = GetTotalColumnWidth();

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollH->SetVisible(nTotalWidth > m_rc.Width());
	((CDuiScrollHorizontal*)m_pControScrollH)->SetScrollMaxRange(nTotalWidth);

	// ����ˮƽ������λ��
	if(nTotalWidth > m_rc.Width())
	{
		CRect rcTemp = m_rc;
		rcTemp.top = rcTemp.bottom - m_nScrollWidth;
		rcTemp.right = rcTemp.right - m_nScrollWidth;
		m_pControScrollH->SetRect(rcTemp);
		// ˮƽ��������ǰλ�ñ��ֲ���
		//((CDuiScrollHorizontal*)m_pControScrollH)->SetScrollCurrentPos();
	}
}

// ��ָ�����й������ɼ���Χ
BOOL CDuiGridCtrl::EnsureVisible(int nRow, BOOL bPartialOK)
{
	// ���ָ�������Ѿ����ڿɼ���Χ��ֱ�ӷ���
	if((nRow >= m_nFirstViewRow) && (nRow <= m_nLastViewRow))
	{
		return TRUE;
	}

	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	if(nRow < m_nFirstViewRow)
	{
		pScrollV->SetScrollCurrentPos(nRow * m_nRowHeight);
	}else
	{
		int nFirstRow = nRow - ((m_rc.Height() - m_nHeaderHeight) / m_nRowHeight) + 2;
		if(nFirstRow < 0)
		{
			nFirstRow = 0;
		}
		pScrollV->SetScrollCurrentPos(nFirstRow * m_nRowHeight);
	}

	UpdateControl(true);
	return TRUE;
}

// ��ȡĳһ������Ϣ
GridRowInfo* CDuiGridCtrl::GetRowInfo(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// ��ȡĳһ����Ԫ����Ϣ
GridItemInfo* CDuiGridCtrl::GetItemInfo(int nRow, int nItem)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return NULL;
	}

	if((nItem < 0) || (nItem >= (int)m_vecColumnInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	if(nItem >= (int)rowInfo.vecItemInfo.size())
	{
		return NULL;
	}

	GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(nItem);

	return &itemInfo;
}

// ����ĳһ���е�������ɫ
void CDuiGridCtrl::SetRowColor(int nRow, Color clrText)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// ����ĳһ���еı�����ɫ
void CDuiGridCtrl::SetRowBackColor(int nRow, Color clrBack)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowBackColor = TRUE;
	rowInfo.clrBack = clrBack;
}

// ����ĳһ���еļ���״̬
void CDuiGridCtrl::SetRowCheck(int nRow, int nCheck)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.nCheck = nCheck;
}

// ��ȡĳһ�еļ���״̬
int CDuiGridCtrl::GetRowCheck(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return -1;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return rowInfo.nCheck;
}

// ����б�
void CDuiGridCtrl::ClearItems()
{
	// ɾ�������ӿؼ�
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(i);
		for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
		{
			GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
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

// ��XML����Font-title����
HRESULT CDuiGridCtrl::OnAttributeFontTitle(const CString& strValue, BOOL bLoading)
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
	int nTotalColumnWidth = GetTotalColumnWidth();

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
			if((SCROLL_H == uControlID) && (nTotalColumnWidth > m_rc.Width()))
			{
				rcTemp = m_rc;
				rcTemp.top = rcTemp.bottom - m_nScrollWidth;
				rcTemp.right = rcTemp.right - m_nScrollWidth;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// ������������
	CalcColumnsPos();
	// ���¼��������е�λ��
	CalcRowsPos();
}

// �ж�ָ��������λ���Ƿ���ĳһ����
BOOL CDuiGridCtrl::PtInRow(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcRow;
	// rcRow�����ǲ����нڵ�ʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�еļ�����
BOOL CDuiGridCtrl::PtInRowCheck(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcCheck;
	// rcCheck�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left - m_nVirtualLeft, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�ĳ������,����������
int CDuiGridCtrl::PtInRowItem(CPoint point, GridRowInfo& rowInfo)
{
	for(size_t i = 0; i < rowInfo.vecItemInfo.size(); i++)
	{
		GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(i);
		CRect rc = itemInfo.rcItem;
		// rcItem�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
		rc.OffsetRect(m_rc.left + ((i == 0) ? m_nLeftPos : 0) - m_nVirtualLeft, m_rc.top+m_nHeaderHeight-m_nVirtualTop);
		if(i == 0)
		{
			rc.right -= m_nLeftPos;
		}
		if(rc.PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

// ���õ�Ԫ���Tooltip
void CDuiGridCtrl::SetGridTooltip(int nRow, int nItem, CString strTooltip)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	if((m_nTipRow != nRow) || (m_nTipItem != nItem) || (m_nTipVirtualTop != m_nVirtualTop))
	{
		GridItemInfo* pGridInfo = GetItemInfo(nRow, nItem);
		if(pGridInfo && (pGridInfo->bNeedTitleTip || pGridInfo->bNeedContentTip))
		{
			CRect rc = pGridInfo->rcItem;
			rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop+m_nHeaderHeight);
			SetTooltip(this, strTooltip, rc, TRUE);
		}else
		{
			ClearTooltip();
		}
		m_nTipRow = nRow;
		m_nTipItem = nItem;
		m_nTipVirtualTop = m_nVirtualTop;
	}
}

// ���Tooltip
void CDuiGridCtrl::ClearGridTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipRow = -1;
		m_nTipItem = -1;
		m_nTipVirtualTop = 0;
		return;
	}

	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->ClearTooltip();
		m_nTipRow = -1;
		m_nTipItem = -1;
		m_nTipVirtualTop = 0;
		return;
	}
}

// ����ƶ��¼�����
BOOL CDuiGridCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// �к͵�Ԫ����¼�����
	int nOldHoverRow = m_nHoverRow;
	BOOL bHoverItemChange = FALSE;
	int nOldHoverItem = -1;

	if(m_rc.PtInRect(point))
	{
		if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
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

				if(m_bGridTooltip)	// ���õ�Ԫ��Tooltip
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
		if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
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

				if(m_bGridTooltip)	// ���õ�Ԫ��Tooltip
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

	// �϶��зָ�����ر���
	enumButtonState buttonState = m_enButtonState;
	BOOL bHoverSplitColumn = m_bHoverSplitColumn;

	if(m_bEnableModifyColumn && m_rc.PtInRect(point))
	{
		if(m_enButtonState != enBSDown)
		{
			BOOL bMouseHover = FALSE;
			if(!m_bMouseDown)
			{
				for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
				{
					GridColumnInfo &columnInfo = m_vecColumnInfo.at(i);
					CRect rcSplit(columnInfo.rcHeader.right-2, columnInfo.rcHeader.top,
						columnInfo.rcHeader.right+m_sizeColumnSeperator.cx+2, columnInfo.rcHeader.bottom);
					rcSplit.OffsetRect(m_rc.left - m_nVirtualLeft, m_rc.top);
					if(rcSplit.PtInRect(point))
					{
						m_bHoverSplitColumn = true;
						bMouseHover = TRUE;
						m_enButtonState = enBSNormal;
						// ���������״
						m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE));	// ˫��ͷָ���ϱ�
						break;
					}
				}

				if(!bMouseHover)
				{
					m_bHoverSplitColumn = false;
					m_enButtonState = enBSNormal;
					// ��ԭ�����״
					m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));	// ��ͷ
				}
			}
		}
		else
		{
			// ��갴�ŷָ��߻������,�϶��������,���ƶ��ָ��߷�Χ(����ˢ��ģʽ)
			MoveColumnSplit(m_nHoverSplitColumn, point.x-m_rc.left+m_nVirtualLeft);
			return true;
		}
	}

	if((nOldHoverRow != m_nHoverRow) || bHoverItemChange || (buttonState != m_enButtonState) || (bHoverSplitColumn != m_bHoverSplitColumn))
	{
		UpdateControl(TRUE);
		return true;
	}
	
	return false;
}

// �����������¼�����
BOOL CDuiGridCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	// ��ͷ�����ƶ��¼�����
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable && m_bEnableModifyColumn)
	{
		for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
		{
			GridColumnInfo &columnInfo = m_vecColumnInfo.at(i);
			CRect rcSplit(columnInfo.rcHeader.right-2, columnInfo.rcHeader.top,
				columnInfo.rcHeader.right+m_sizeColumnSeperator.cx+2, columnInfo.rcHeader.bottom);
			rcSplit.OffsetRect(m_rc.left - m_nVirtualLeft, m_rc.top);
			if(rcSplit.PtInRect(point))
			{
				// ����ڷָ�����,���¼���λ��
				m_enButtonState = enBSDown;
				m_nHoverSplitColumn = i;
				break;
			}
		}
	}

	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}

	// ���¼�����
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(MSG_BUTTON_DOWN, m_bEnableDownRow ? m_nDownRow : m_nHoverRow, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(MSG_BUTTON_DOWN, m_nDownRow, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

// �������ſ��¼�����
BOOL CDuiGridCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	// ��ͷ�����ƶ��¼�����
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable && m_bEnableModifyColumn)
	{
		m_enButtonState = enBSNormal;
		for(size_t i = 0; i < m_vecColumnInfo.size(); i++)
		{
			GridColumnInfo &columnInfo = m_vecColumnInfo.at(i);
			CRect rcSplit(columnInfo.rcHeader.right-2, columnInfo.rcHeader.top,
				columnInfo.rcHeader.right+m_sizeColumnSeperator.cx+2, columnInfo.rcHeader.bottom);
			rcSplit.OffsetRect(m_rc.left - m_nVirtualLeft, m_rc.top);
			if(rcSplit.PtInRect(point))
			{
				m_enButtonState = enBSHover;
				break;
			}
		}

		// ���ſ��ָ��߻������,���ƶ��ָ��߷�Χ
		if(m_nHoverSplitColumn != -1)
		{
			MoveColumnSplit(m_nHoverSplitColumn, point.x-m_rc.left+m_nVirtualLeft);
		}
	}

	m_nHoverSplitColumn = -1;

	// ���¼�����
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_CHECK, m_nHoverRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(MSG_BUTTON_CHECK, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}
	}

	return false;
}

// ������˫���¼�����
BOOL CDuiGridCtrl::OnControlLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if(m_rc.PtInRect(point))
	{
		for(size_t i = 0; i < m_vecRowInfo.size(); i++)
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(i);
			if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))
			{
				int nClickItem = PtInRowItem(point, rowInfo);
				SendMessage(MSG_BUTTON_DBLCLK, i, nClickItem);
				return true;
			}
		}
	}

	return false;
}

// ��ֱ�����¼�����
BOOL CDuiGridCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(((int)m_vecRowInfo.size() * m_nRowHeight) <= m_rc.Height())
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

// ����Ҽ������¼�����
BOOL CDuiGridCtrl::OnControlRButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(MSG_MOUSE_RDOWN, m_bEnableDownRow ? m_nDownRow : m_nHoverRow, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	} else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(MSG_MOUSE_RDOWN, m_nDownRow, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

// ����Ҽ��ſ��¼�����
BOOL CDuiGridCtrl::OnControlRButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))
		{
			//ȡ����������Ϣ���ܷ���
			if(!PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				SendMessage(MSG_MOUSE_RUP, m_nHoverRow, rowInfo.nHoverItem);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(MSG_MOUSE_RUP, m_nDownRow, rowInfo.nHoverItem);
			UpdateControl(TRUE);
			return true;
		}
	}

	return false;
}

// ��Ϣ��Ӧ
LRESULT CDuiGridCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(((uID == SCROLL_V) || (uID == SCROLL_H)) && (Msg == MSG_SCROLL_CHANGE))
	{
		// ����ǹ�������λ�ñ���¼�,��ˢ����ʾ
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == MSG_BUTTON_DOWN) && (lParam != -1))
	{
		// ������е�ĳ������
		GridRowInfo* pRowInfo = GetRowInfo(wParam);
		if(pRowInfo && (lParam >= 0) && (lParam < (int)pRowInfo->vecItemInfo.size()))
		{
			GridItemInfo &itemInfo = pRowInfo->vecItemInfo.at(lParam);
			// ת��ΪMSG_BUTTON_UP��Ϣ,��ΪDuiSystem������ʱ��ֻ����MSG_BUTTON_UP��Ϣ
			if(!itemInfo.strLinkAction.IsEmpty())
			{
				DuiSystem::AddDuiActionTask(uID, MSG_BUTTON_UP, wParam, lParam, GetName(), itemInfo.strLinkAction, GetParent());
			}
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiGridCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	// �б�ͼ����:
	// 1.�б������߶�Ϊÿһ�и߶�*����
	// 2.�б���ʾ��top������scroll�ؼ���¼
	// 3.�ػ�ʱ��,����top����λ�ü������ʾ�ĵ�һ�е����,������ʾ�߶ȼ������ʾ�����һ�е����
	// 4.���ݼ��������ʾ����,����Ӧ�����ݵ��ڴ�dc��
	// 5.�������ʾ��top��������ڴ�dc�Ŀ���
	int nTotalColumnWidth = GetTotalColumnWidth();	// �ܵ��п��
	int nViewWidth = m_rc.Width() - m_nScrollWidth;	// ��ȥ����������ʾ������
	CDuiScrollHorizontal* pScrollH = (CDuiScrollHorizontal*)m_pControScrollH;
	int nCurPosH = pScrollH->GetScrollCurrentPos();	// ��ǰleftλ��
	int nMaxRangeH = pScrollH->GetScrollMaxRange();
	int nContentWidth = (nTotalColumnWidth > nViewWidth) ? nTotalColumnWidth : nViewWidth;	// ���ݲ��ֵĿ��(����ܵ��п�С����ʾ������,��ʹ����ʾ������)
	m_nVirtualLeft = (nMaxRangeH > 0) ? (int)((double)nCurPosH*(nContentWidth-nViewWidth)/nMaxRangeH) : 0;	// ��ǰ������λ�ö�Ӧ�������leftλ��

	int nHeightAll = m_vecRowInfo.size()*m_nRowHeight; // �ܵ�����߶� //m_rc.Height();
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	int nCurPosV = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRangeV = pScrollV->GetScrollMaxRange();
	m_nVirtualTop = (nMaxRangeV > 0) ? (int)((double)nCurPosV*(nHeightAll-(m_rc.Height() - m_nHeaderHeight))/nMaxRangeV) : 0;	// ��ǰ������λ�ö�Ӧ�������topλ��
	if(m_nVirtualTop < 0)
	{
		m_nVirtualTop = 0;
		pScrollV->SetScrollCurrentPos(0);
	}
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;					// ��ʾ�ĵ�һ�����
	m_nLastViewRow = (m_nVirtualTop + m_rc.Height() - m_nHeaderHeight) / m_nRowHeight;	// ��ʾ�����һ�����
	if(m_nLastViewRow >= (int)m_vecRowInfo.size())
	{
		m_nLastViewRow = m_vecRowInfo.size() - 1;
	}
	if(m_nLastViewRow < 0)
	{
		m_nLastViewRow = 0;
	}
	int nHeightView = (m_nLastViewRow - m_nFirstViewRow +1) * m_nRowHeight + m_nHeaderHeight;	// ��ʾ�漰��������߶�
	int nYViewPos = m_nVirtualTop - (m_nFirstViewRow * m_nRowHeight);		// �ڴ�dc��ʾ����Ļʱ���topλ��
	if(nYViewPos < 0)
	{
		nYViewPos = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nTotalColumnWidth, nHeightView);

		Graphics graphics(m_memDC);
		
		// ����ɫ����
		m_memDC.BitBlt(m_nVirtualLeft, 0, nViewWidth, nHeightView, &dc, m_rc.left, m_rc.top, WHITENESS);	
		// ����͸������
		// �����в���
		CRect rcTitle = m_rc;
		rcTitle.bottom = rcTitle.top + m_nHeaderHeight;
		DrawVerticalTransition(m_memDC, dc, CRect(m_nVirtualLeft, 0, nViewWidth+m_nVirtualLeft, m_nHeaderHeight),	
				rcTitle, m_nBkTransparent, m_nBkTransparent);
		// ���ݲ���
		CRect rcContent = m_rc;
		rcContent.top = m_rc.top + m_nHeaderHeight;
		DrawVerticalTransition(m_memDC, dc, CRect(m_nVirtualLeft, m_nHeaderHeight+nYViewPos, nViewWidth+m_nVirtualLeft, m_rc.Height()+nYViewPos),
				rcContent, m_nBkTransparent, m_nBkTransparent);
		
		BSTR bsFontTitle = m_strFontTitle.AllocSysString();
		FontFamily fontFamilyTitle(bsFontTitle);
		Font fontTitle(&fontFamilyTitle, (REAL)m_nFontTitleWidth, m_fontTitleStyle, UnitPixel);
		::SysFreeString(bsFontTitle);

		BSTR bsFont = m_strFont.AllocSysString();
		FontFamily fontFamily(bsFont);
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
		::SysFreeString(bsFont);

		SolidBrush solidBrush(m_clrText);			// �������ֻ�ˢ
		SolidBrush solidBrushHT(m_clrHeader);			// ���������ֻ�ˢ
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

		// ���ñ��������ֵ�ˮƽ�ʹ�ֱ���뷽ʽ
		DUI_STRING_ALIGN_DEFINENAME(Header, m_uAlignmentHeader, m_uVAlignmentHeader);
		if(!m_bTextWrap)
		{
			strFormatHeader.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}

		// ��������
		if((m_nHeaderHeight > 0) && (m_vecColumnInfo.size() > 0))
		{
			// �������б���
			if(m_pImageHeader != NULL)
			{
				CRect  rcHeader(0, 0, nViewWidth, m_nHeaderHeight);
				DrawImageFrame(graphics, m_pImageHeader, rcHeader, 0, 0, m_sizeHeader.cx, m_sizeHeader.cy, 0);
			}

			// ����Ԫ������
			int nPosItemX = 0;
			for(size_t j = 0; j < m_vecColumnInfo.size(); j++)
			{
				GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
				int nWidth = columnInfo.nWidth;
				if(nWidth== -1)
				{
					nWidth = m_rc.Width() - nPosItemX;
				}

				// ���������зָ���
				if((m_pImageColumnSeperator != NULL) && (j < (m_vecColumnInfo.size()-1)))
				{
					RectF rectSep((Gdiplus::REAL)(nPosItemX+nWidth), 0, (Gdiplus::REAL)m_sizeColumnSeperator.cx, (Gdiplus::REAL)(m_nHeaderHeight-1));
					graphics.DrawImage(m_pImageColumnSeperator, rectSep, 0, 0, (Gdiplus::REAL)m_sizeColumnSeperator.cx, (Gdiplus::REAL)m_sizeColumnSeperator.cy, UnitPixel);
				}

				// ���б���
				RectF rect((Gdiplus::REAL)nPosItemX, 0, (Gdiplus::REAL)nWidth, (Gdiplus::REAL)(m_nHeaderHeight-1));
				CString strTitle = columnInfo.strTitle;
				BSTR bsTitle = strTitle.AllocSysString();
				graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &fontTitle, rect, &strFormatHeader, &solidBrushHT);
				::SysFreeString(bsTitle);

				nPosItemX += nWidth;
			}
		}
		
		if(m_vecRowInfo.size() > 0)
		{
			for(int i = m_nFirstViewRow; i <= m_nLastViewRow && i < (int)m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				SolidBrush solidBrushRow(rowInfo.clrText);	// �ж������ɫ

				int nXPos = 0;
				int nVI = i - m_nFirstViewRow;

				// ����ƶ�����ʱ����ʾ�ı�����ɫ(�������Ϊȫ0,����ʾ�б�����ɫ)
				if((m_nHoverRow == i) && (m_clrRowHover.GetValue() != Color(0, 0, 0, 0).GetValue()))
				{
					SolidBrush brush(m_clrRowHover);
					graphics.FillRectangle(&brush, 0, m_nHeaderHeight + nVI*m_nRowHeight, nContentWidth, m_nRowHeight);
				}else
				if(rowInfo.bRowBackColor)	// ����������еı�����ɫ,�������ɫ
				{
					SolidBrush brush(rowInfo.clrBack);
					graphics.FillRectangle(&brush, 0, m_nHeaderHeight + nVI*m_nRowHeight, nContentWidth, m_nRowHeight);
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
					graphics.DrawImage(m_pImageCheckBox, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nCheckImgY, m_sizeCheckBox.cx, m_sizeCheckBox.cy),
						nCheckImageIndex * m_sizeCheckBox.cx, 0, m_sizeCheckBox.cx, m_sizeCheckBox.cy, UnitPixel);
					rowInfo.rcCheck.SetRect(nXPos, i*m_nRowHeight + nCheckImgY, nXPos + m_sizeCheckBox.cx, i*m_nRowHeight + nCheckImgY + m_sizeCheckBox.cy);
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
					// ʹ������ͼƬ
					graphics.DrawImage(m_pImage, Rect(nXPos, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
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
					graphics.DrawImage(rowInfo.pRightImage, Rect(nContentWidth-rowInfo.sizeRightImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy),
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
					graphics.DrawImage(m_pImage, Rect(nContentWidth-m_sizeImage.cx-1, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nRightImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nRightImageWidth = m_sizeImage.cx + 1;
				}

				// ����Ԫ������
				int nPosItemX = (m_nLeftPos != 0) ? m_nLeftPos : nXPos;
				for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
				{
					GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
					GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
					BOOL bSingleLine = (itemInfo.strContent.IsEmpty() || !itemInfo.strLink.IsEmpty());
					RectF rect((Gdiplus::REAL)nPosItemX,
						(Gdiplus::REAL)(m_nHeaderHeight + nVI*m_nRowHeight + 1),
						(Gdiplus::REAL)((j == 0) ? (itemInfo.rcItem.Width() - nPosItemX): itemInfo.rcItem.Width()),
						(Gdiplus::REAL)(bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2)));
					if((int)(rect.GetRight()) > nContentWidth)
					{
						// ���һ����Ҫ��ȥ���������
						rect.Width -= m_nScrollWidth;
					}

					// ����Ԫ��ͼƬ
					int nItemImageX = 0;
					int nImgY = 3;
					if(itemInfo.pImage != NULL)
					{
						if((itemInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
						{
							nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
						}
						// ʹ�õ�Ԫ��ָ����ͼƬ
						graphics.DrawImage(itemInfo.pImage,
							Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, itemInfo.sizeImage.cx, itemInfo.sizeImage.cy),
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
						graphics.DrawImage(m_pImage, Rect(nPosItemX+nItemImageX, m_nHeaderHeight + nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
							itemInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
						nItemImageX += (m_sizeImage.cx + 3);
					}
					rect.Offset((Gdiplus::REAL)nItemImageX, 0);
					rect.Width -= (Gdiplus::REAL)nItemImageX;

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
					if(rowInfo.bRowColor)	// ����������ɫ,��ʹ��
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

					// ���õ�Ԫ�����ֶ��뷽ʽ,ʹ���еĶ��뷽ʽ
					StringFormat strFormatColumn;
					UINT uAlignment = m_uAlignment;
					if(columnInfo.uAlignment != 0xFFFFUL)
					{
						uAlignment = columnInfo.uAlignment;
					}
					if(uAlignment == Align_Left)
					{
						strFormatColumn.SetAlignment(StringAlignmentNear);
					}else
					if(uAlignment == Align_Center)
					{
						strFormatColumn.SetAlignment(StringAlignmentCenter);
					}else
					if(uAlignment == Align_Right)
					{
						strFormatColumn.SetAlignment(StringAlignmentFar);
					}
					UINT uVAlignment = m_uVAlignment;
					if(columnInfo.uVAlignment != 0xFFFFUL)
					{
						uVAlignment = columnInfo.uVAlignment;
					}
					if(uVAlignment == VAlign_Top)
					{
						strFormatColumn.SetLineAlignment(StringAlignmentNear);
					}else
					if(uVAlignment == VAlign_Middle)
					{
						strFormatColumn.SetLineAlignment(StringAlignmentCenter);
					}else
					if(uVAlignment == VAlign_Bottom)
					{
						strFormatColumn.SetLineAlignment(StringAlignmentFar);
					}
					if(!m_bTextWrap)
					{
						strFormatColumn.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
					}

					// ����bUseTitleFont�����ñ������廹����ͨ����
					BSTR bsItemTitle = strItemTitle.AllocSysString();
					graphics.DrawString(bsItemTitle, (INT)wcslen(bsItemTitle),
						itemInfo.bUseTitleFont ? &fontTitle : &font, rect, &strFormatColumn, itemInfo.bUseTitleFont ? &solidBrushT : &solidBrushItem);
					::SysFreeString(bsItemTitle);

					// ����Ԫ������
					if(!bSingleLine)
					{
						rect.Offset(0, (Gdiplus::REAL)m_nRowHeight / 2 + 2);
						rect.Height = (Gdiplus::REAL)m_nRowHeight / 2 - 4;
						BSTR bsItemContent = itemInfo.strContent.AllocSysString();
						graphics.DrawString(bsItemContent, (INT)wcslen(bsItemContent), &font, rect, &strFormatColumn, &solidBrushItem);
						::SysFreeString(bsItemContent);
					}

					// ���õ�Ԫ���ӿؼ���λ��
					for(size_t k = 0; k < itemInfo.vecControl.size(); k++)
					{
						CControlBase* pControl = itemInfo.vecControl.at(k);
						if(pControl)
						{
							CRect rcParent = CRect(nPosItemX, m_nHeaderHeight + nVI*m_nRowHeight + 1,
								(int)(rect.X+rect.Width), (nVI+1)*m_nRowHeight - 1);
							if((int)(rect.GetRight()) > nContentWidth)
							{
								// ���һ����Ҫ��ȥ���������
								rcParent.right -= m_nScrollWidth;
							}
							rcParent.OffsetRect(m_rc.left - m_nVirtualLeft, m_rc.top - nYViewPos);
							pControl->SetPositionWithParent(rcParent);
							CRect rcControl = pControl->GetRect();
							// ֻ�е�ǰ����ʾ��Χ�ڵĿؼ�����Ϊ�ɼ�
							if( (rcControl.top < (m_rc.top+m_nHeaderHeight)) || (rcControl.bottom > m_rc.bottom) ||
								(rcControl.left < m_rc.left) || (rcControl.right > m_rc.right) )
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
					// ʹ������ģʽ��ͼ
					graphics.DrawImage(m_pImageSeperator,
							RectF(0, (Gdiplus::REAL)(m_nHeaderHeight + (nVI+1)*m_nRowHeight), (Gdiplus::REAL)(nContentWidth-2), (Gdiplus::REAL)m_sizeSeperator.cy),
							0, 0, (Gdiplus::REAL)m_sizeSeperator.cx, (Gdiplus::REAL)m_sizeSeperator.cy, UnitPixel);
				}else
				if(m_clrSeperator.GetValue() != Color(0, 0, 0, 0).GetValue())
				{
					// δָ��ͼƬ,���ҷָ�����ɫ����ȫ0,�򻭾���
					graphics.FillRectangle(&solidBrushS, 0, m_nHeaderHeight + (nVI+1)*m_nRowHeight, nContentWidth-2, 1);
				}
			}

			// �����ݲ��ֵ��зָ���
			if(m_bShowColumnSeperator && (m_pImageColumnSeperator != NULL))
			{
				int nPosItemX = 0;
				for(size_t j = 0; j < m_vecColumnInfo.size(); j++)
				{
					GridColumnInfo &columnInfo = m_vecColumnInfo.at(j);
					int nWidth = columnInfo.nWidth;
					if(nWidth== -1)
					{
						nWidth = m_rc.Width() - nPosItemX;
					}

					if(j < (m_vecColumnInfo.size()-1))
					{
						RectF rectSep((Gdiplus::REAL)(nPosItemX+nWidth), (Gdiplus::REAL)m_nHeaderHeight,
							(Gdiplus::REAL)m_sizeColumnSeperator.cx, (Gdiplus::REAL)(nHeightView - m_nHeaderHeight));
						graphics.DrawImage(m_pImageColumnSeperator, rectSep, 0, 0, (Gdiplus::REAL)m_sizeColumnSeperator.cx, (Gdiplus::REAL)m_sizeColumnSeperator.cy, UnitPixel);
					}

					nPosItemX += nWidth;
				}
			}

			// �Ѳ�����ʾ��Χ�ڵĵ�Ԫ��Ŀؼ�������Ϊ���ɼ�
			for(int i = 0; i < (int)m_vecRowInfo.size(); i++)
			{
				if((i < m_nFirstViewRow) || (i > m_nLastViewRow))
				{
					GridRowInfo &rowInfo = m_vecRowInfo.at(i);
					for(size_t j = 0; j < rowInfo.vecItemInfo.size(); j++)
					{
						GridItemInfo &itemInfo = rowInfo.vecItemInfo.at(j);
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

	// ���������DC,ʹ����ķ�ʽ�ϲ�����
	// ���������
	if(m_nHeaderHeight > 0)
	{
		dc.BitBlt(m_rc.left,m_rc.top, nViewWidth, m_nHeaderHeight, &m_memDC, m_nVirtualLeft, 0, SRCCOPY);//SRCAND);
	}
	// ���ݲ������
	int nContentHeight = m_rc.Height() - m_nHeaderHeight;
	if(nTotalColumnWidth > m_rc.Width())
	{
		nContentHeight -= m_nScrollWidth;
	}
	dc.BitBlt(m_rc.left,m_rc.top + m_nHeaderHeight, nViewWidth, nContentHeight, &m_memDC, m_nVirtualLeft, nYViewPos + m_nHeaderHeight, SRCCOPY);//SRCAND);
}