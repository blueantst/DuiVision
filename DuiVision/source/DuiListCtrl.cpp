#include "StdAfx.h"
#include "DuiListCtrl.h"

#define	SCROLL_V	1	// ��ֱ�������ؼ�ID
#define	SCROLL_H	2	// ˮƽ�������ؼ�ID

CDuiListCtrl::CDuiListCtrl(HWND hWnd, CDuiObject* pDuiObject)
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

	m_bRowTooltip = TRUE;
	m_nTipRow = -1;
	m_nTipVirtualTop = 0;

	m_nFirstViewRow = 0;
	m_nLastViewRow = 0;
	m_nVirtualTop = 0;
}

CDuiListCtrl::~CDuiListCtrl(void)
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

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiListCtrl, Seperator, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiListCtrl, CheckBox, 6)

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiListCtrl::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
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

	// �����²��row�ڵ���Ϣ
	for (DuiXmlNode pRowElem = pXmlElem.child(_T("row")); pRowElem; pRowElem=pRowElem.next_sibling(_T("row")))
	{
		CString strId = pRowElem.attribute(_T("id")).value();
		CString strTitle = pRowElem.attribute(_T("title")).value();
		CString strContent = pRowElem.attribute(_T("content")).value();
		CString strTime = pRowElem.attribute(_T("time")).value();
		CString strCheck = pRowElem.attribute(_T("check")).value();
		CString strImage = pRowElem.attribute(_T("image")).value();
		CString strRightImage = pRowElem.attribute(_T("right-img")).value();
		CString strClrText = pRowElem.attribute(_T("crtext")).value();
		CString strClrBack = pRowElem.attribute(_T("crback")).value();
		CString strLink1 = pRowElem.attribute(_T("link1")).value();
		CString strLinkAction1 = pRowElem.attribute(_T("linkaction1")).value();
		CString strLink2 = pRowElem.attribute(_T("link2")).value();
		CString strLinkAction2 = pRowElem.attribute(_T("linkaction2")).value();

		DuiSystem::Instance()->ParseDuiString(strTitle);
		DuiSystem::Instance()->ParseDuiString(strContent);

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

		InsertItem(-1, strId, strTitle, strContent, strTime, nImageIndex, clrText, strImage, nRightImageIndex, strRightImage,
			strLink1, strLinkAction1, strLink2, strLinkAction2, nCheck, clrBack);
	}

	// ����ÿһ�е�λ�ú͹�����
	CalcItemsPos();

    return TRUE;
}

// ����б���
int CDuiListCtrl::InsertItem(int nItem, CString strId, CString strTitle, CString strContent, CString strTime,
							  int nImageIndex, Color clrText, CString strImage, int nRightImageIndex, CString strRightImage,
							  CString strLink1, CString strLinkAction1, CString strLink2, CString strLinkAction2,
							  int nCheck, Color clrBack)
{
	if(!strContent.IsEmpty())
	{
		m_bSingleLine = FALSE;
	}

	ListRowInfo rowInfo;
	rowInfo.strId = strId;
	rowInfo.strTitle = strTitle;
	rowInfo.strContent = strContent;
	rowInfo.strTime = strTime;
	rowInfo.nCheck = nCheck;
	rowInfo.nImageIndex = nImageIndex;
	rowInfo.sizeImage.SetSize(0, 0);
	rowInfo.nRightImageIndex = nRightImageIndex;
	rowInfo.sizeRightImage.SetSize(0, 0);
	rowInfo.bRowColor = FALSE;
	rowInfo.clrText = clrText;
	rowInfo.bRowBackColor = FALSE;
	rowInfo.clrBack = clrBack;
	rowInfo.strLink1 = strLink1;
	rowInfo.strLinkAction1 = strLinkAction1;
	rowInfo.strLink2 = strLink2;
	rowInfo.strLinkAction2 = strLinkAction2;
	rowInfo.nHoverLink = -1;
	rowInfo.bNeedTitleTip = FALSE;
	rowInfo.bNeedContentTip = FALSE;
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

	//rowInfo.rcRow.SetRect(m_rc.left, m_rc.top, m_rc.left, m_rc.bottom);

	return InsertItem(nItem, rowInfo);
}

// ����б���
int CDuiListCtrl::InsertItem(int nItem, CString strTitle, int nCheck, Color clrText, int nImageIndex, CString strLink1, CString strLinkAction1, CString strLink2, CString strLinkAction2, Color clrBack)
{
	return InsertItem(nItem, _T(""), strTitle, _T(""), _T(""), nImageIndex, clrText, _T(""), -1, _T(""), strLink1, strLinkAction1, strLink2, strLinkAction2, nCheck, clrBack);
}

int CDuiListCtrl::InsertItem(int nItem, ListRowInfo &rowInfo)
{
	int nRetItem = -1;
	if(m_vecRowInfo.size() > 0)
	{
		CRect rc;
	}
	if(nItem <= -1 || nItem >= (int)m_vecRowInfo.size())
	{
		m_vecRowInfo.push_back(rowInfo);
		nRetItem = m_vecRowInfo.size()-1;
	}
	else
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nItem, rowInfo);
		nRetItem = nItem;
	}

	// ���������б��е�λ��
	CalcItemsPos();

	UpdateControl(true);
	return nRetItem;
}

// ɾ���б���
BOOL CDuiListCtrl::DeleteItem(int nItem)
{
	if((nItem < 0) || (nItem >= (int)m_vecRowInfo.size()))
	{
		return FALSE;
	}

	int nIndex = 0;
	vector<ListRowInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nItem)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}

	// ���������б��е�λ��
	CalcItemsPos();

	UpdateControl(true);
	return true;
}

// �����б���λ��
void CDuiListCtrl::CalcItemsPos()
{
	int nXPos = 0;//m_rc.left;
	int nYPos = 0;//m_rc.top;

	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		ListRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		rowInfoTemp.rcCheck.SetRect(0,0,0,0);
		rowInfoTemp.rcLink1.SetRect(0,0,0,0);
		rowInfoTemp.rcLink2.SetRect(0,0,0,0);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(((int)m_vecRowInfo.size() * m_nRowHeight) > m_rc.Height());
	((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange((int)m_vecRowInfo.size() * m_nRowHeight);
}

// ��ָ�����й������ɼ���Χ
BOOL CDuiListCtrl::EnsureVisible(int nRow, BOOL bPartialOK)
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
		int nFirstRow = nRow - (m_rc.Height() / m_nRowHeight) + 2;
		if(nFirstRow < 0)
		{
			nFirstRow = 0;
		}
		pScrollV->SetScrollCurrentPos(nFirstRow * m_nRowHeight);
	}

	UpdateControl(true);
	return TRUE;
}

// ��ȡĳһ���б���
ListRowInfo* CDuiListCtrl::GetItemInfo(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return NULL;
	}

	ListRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// ����ĳһ���е�������ɫ
void CDuiListCtrl::SetRowColor(int nRow, Color clrText)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	ListRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// ����ĳһ���еı�����ɫ
void CDuiListCtrl::SetRowBackColor(int nRow, Color clrBack)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	ListRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowBackColor = TRUE;
	rowInfo.clrBack = clrBack;
}

// ����ĳһ���еļ���״̬
void CDuiListCtrl::SetRowCheck(int nRow, int nCheck)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	ListRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.nCheck = nCheck;
}

// ��ȡĳһ�еļ���״̬
int CDuiListCtrl::GetRowCheck(int nRow)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return -1;
	}

	ListRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return rowInfo.nCheck;
}

// ����б�
void CDuiListCtrl::ClearItems()
{
	m_vecRowInfo.clear();
	m_pControScrollV->SetVisible(FALSE);
	UpdateControl(true);
}

// ��XML����Font-title����
HRESULT CDuiListCtrl::OnAttributeFontTitle(const CString& strValue, BOOL bLoading)
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

void CDuiListCtrl::SetControlRect(CRect rc)
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
	int nXPos = 0;
	int nYPos = 0;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		ListRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - m_nScrollWidth;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(((int)m_vecRowInfo.size() * m_nRowHeight) > m_rc.Height());
	((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange((int)m_vecRowInfo.size() * m_nRowHeight);
}

// �ж�ָ��������λ���Ƿ���ĳһ����
BOOL CDuiListCtrl::PtInRow(CPoint point, ListRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcRow;
	// rcRow�����ǲ����нڵ�ʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�еļ�����
BOOL CDuiListCtrl::PtInRowCheck(CPoint point, ListRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcCheck;
	// rcCheck�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�ĳ��������,������������
int CDuiListCtrl::PtInRowLink(CPoint point, ListRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcLink1;
	// rcLink�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	if(rc.PtInRect(point))
	{
		return 0;
	}

	rc = rowInfo.rcLink2;
	// rcLink�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	if(rc.PtInRect(point))
	{
		return 1;
	}

	return -1;
}

// �����е�Tooltip
void CDuiListCtrl::SetRowTooltip(int nRow, CString strTooltip)
{
	if((nRow < 0) || (nRow >= (int)m_vecRowInfo.size()))
	{
		return;
	}

	if((m_nTipRow != nRow) || (m_nTipVirtualTop != m_nVirtualTop))
	{
		ListRowInfo &rowInfo = m_vecRowInfo.at(nRow);
		if(rowInfo.bNeedTitleTip || rowInfo.bNeedContentTip)
		{
			CRect rc = rowInfo.rcRow;
			rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
			SetTooltip(this, strTooltip, rc, TRUE);
		}
		m_nTipRow = nRow;
		m_nTipVirtualTop = m_nVirtualTop;
	}
}

// ���Tooltip
void CDuiListCtrl::ClearRowTooltip()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		m_nTipRow = -1;
		m_nTipVirtualTop = 0;
		return;
	}

	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->ClearTooltip();
		m_nTipRow = -1;
		m_nTipVirtualTop = 0;
		return;
	}
}

// ����ƶ��¼�����
BOOL CDuiListCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	int nOldHoverRow = m_nHoverRow;
	BOOL bHoverLinkChange = FALSE;
	int nOldHoverLink = -1;

	if(m_rc.PtInRect(point))
	{
		// ���������ȵ���,�ж�����Ƿ���ĳ������λ��
		if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
		{
			ListRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
			nOldHoverLink = rowInfo.nHoverLink;
			if(PtInRow(point, rowInfo))
			{
				if(m_bRowTooltip)
				{
					if(rowInfo.bNeedTitleTip)
					{
						SetRowTooltip(m_nHoverRow, rowInfo.strTitle);
					}else
					if(rowInfo.bNeedContentTip)
					{
						SetRowTooltip(m_nHoverRow, rowInfo.strContent);
					}else
					{
						ClearRowTooltip();
					}
				}

				rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
				if(nOldHoverLink != rowInfo.nHoverLink)
				{
					bHoverLinkChange = TRUE;
					UpdateControl(TRUE);
				}
				return false;
			}
			m_nHoverRow = -1;		
		}

		// �������ڵ�ǰ��,�ж�����Ƿ���ĳ������λ��
		BOOL bMousenDown = false;
		if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
		{
			ListRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
			nOldHoverLink = rowInfo.nHoverLink;
			if(PtInRow(point, rowInfo))
			{
				if(m_bRowTooltip)
				{
					if(rowInfo.bNeedTitleTip)
					{
						SetRowTooltip(m_nDownRow, rowInfo.strTitle);
					}else
					if(rowInfo.bNeedContentTip)
					{
						SetRowTooltip(m_nDownRow, rowInfo.strContent);
					}else
					{
						ClearRowTooltip();
					}
				}

				rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
				bMousenDown = true;
				m_nHoverRow = -1;
				if(nOldHoverLink != rowInfo.nHoverLink)
				{
					bHoverLinkChange = TRUE;
				}
			}		
		}

		// �����ȵ��к��ȵ�����
		if(!bMousenDown)
		{
			for(size_t i = 0; i < m_vecRowInfo.size(); i++)
			{
				ListRowInfo &rowInfo = m_vecRowInfo.at(i);
				nOldHoverLink = rowInfo.nHoverLink;
				if(PtInRow(point, rowInfo))
				{
					rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
					m_nHoverRow = i;
					if(nOldHoverLink != rowInfo.nHoverLink)
					{
						bHoverLinkChange = TRUE;
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

	if((nOldHoverRow != m_nHoverRow) || bHoverLinkChange)
	{
		UpdateControl(TRUE);
		return true;
	}
	
	return false;
}

// �����������¼�����
BOOL CDuiListCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		ListRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				if(m_bEnableDownRow)
				{
					m_nDownRow = m_nHoverRow;
					m_nHoverRow = -1;
				}

				SendMessage(MSG_BUTTON_DOWN, m_bEnableDownRow ? m_nDownRow : m_nHoverRow, rowInfo.nHoverLink);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if((m_nDownRow >= 0) && (m_nDownRow < (int)m_vecRowInfo.size()))
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		ListRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverLink = PtInRowLink(point, rowInfo);
			SendMessage(MSG_BUTTON_DOWN, m_nDownRow, rowInfo.nHoverLink);
			return true;
		}
	}
	
	return false;
}

// �������ſ��¼�����
BOOL CDuiListCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if((m_nHoverRow >= 0) && (m_nHoverRow < (int)m_vecRowInfo.size()))
	{
		ListRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
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
		ListRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
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
BOOL CDuiListCtrl::OnControlLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if(m_rc.PtInRect(point))
	{
		for(size_t i = 0; i < m_vecRowInfo.size(); i++)
		{
			ListRowInfo &rowInfo = m_vecRowInfo.at(i);
			if(PtInRow(point, rowInfo))
			{
				SendMessage(MSG_BUTTON_DBLCLK, i, -1);
				return true;
			}
		}
	}

	return false;
}

// �����¼�����
BOOL CDuiListCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(((int)m_vecRowInfo.size() * m_nRowHeight) <= m_rc.Height())
	{
		return false;
	}

	// ���¹�����,��ˢ�½���
	CDuiScrollVertical* pScroll = (CDuiScrollVertical*)m_pControScrollV;
	if(pScroll->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
	}

	return true;
}

// ��Ϣ��Ӧ
LRESULT CDuiListCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// ����ǹ�������λ�ñ���¼�,��ˢ����ʾ
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == MSG_BUTTON_DOWN) && (lParam != -1))
	{
		// ������е�ĳ������
		ListRowInfo* pRowInfo = GetItemInfo(wParam);
		if(pRowInfo)
		{
			// ת��ΪMSG_BUTTON_UP��Ϣ,��ΪDuiSystem������ʱ��ֻ����MSG_BUTTON_UP��Ϣ
			if((lParam == 0) && (!pRowInfo->strLinkAction1.IsEmpty()))
			{
				DuiSystem::AddDuiActionTask(uID, MSG_BUTTON_UP, wParam, lParam, GetName(), pRowInfo->strLinkAction1, GetParent());
			}else
			if((lParam == 1) && (!pRowInfo->strLinkAction2.IsEmpty()))
			{
				DuiSystem::AddDuiActionTask(uID, MSG_BUTTON_UP, wParam, lParam, GetName(), pRowInfo->strLinkAction2, GetParent());
			}
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiListCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	// �б�ͼ����:
	// 1.�б������߶�Ϊÿһ�и߶�*����
	// 2.�б���ʾ��top������scroll�ؼ���¼
	// 3.�ػ�ʱ��,����top����λ�ü������ʾ�ĵ�һ�е����,������ʾ�߶ȼ������ʾ�����һ�е����
	// 4.���ݼ��������ʾ����,����Ӧ�����ݵ��ڴ�dc��
	// 5.�������ʾ��top��������ڴ�dc�Ŀ���
	int nWidth = m_rc.Width() - m_nScrollWidth;	// ��ȥ�������Ŀ��
	int nHeightAll = (int)m_vecRowInfo.size()*m_nRowHeight; // �ܵ�����߶� //m_rc.Height();
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();

	m_nVirtualTop = (nMaxRange > 0) ? (int)((double)nCurPos*(nHeightAll-m_rc.Height())/nMaxRange) : 0;	// ��ǰ������λ�ö�Ӧ�������topλ��
	if(m_nVirtualTop < 0)
	{
		m_nVirtualTop = 0;
		pScrollV->SetScrollCurrentPos(0);
	}
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;					// ��ʾ�ĵ�һ�����
	m_nLastViewRow = (m_nVirtualTop + m_rc.Height()) / m_nRowHeight;	// ��ʾ�����һ�����
	if(m_nLastViewRow >= (int)m_vecRowInfo.size())
	{
		m_nLastViewRow = (int)m_vecRowInfo.size() - 1;
	}
	if(m_nLastViewRow < 0)
	{
		m_nLastViewRow = 0;
	}
	int nHeightView = (m_nLastViewRow - m_nFirstViewRow +1) * m_nRowHeight;	// ��ʾ�漰��������߶�
	int nYViewPos = m_nVirtualTop - (m_nFirstViewRow * m_nRowHeight);		// �ڴ�dc��ʾ����Ļʱ���topλ��
	if(nYViewPos < 0)
	{
		nYViewPos = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeightView);

		Graphics graphics(m_memDC);
		
		m_memDC.BitBlt(0, 0, nWidth, nHeightView, &dc, m_rc.left ,m_rc.top, WHITENESS);	// ����ɫ����
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

		// ʱ���ֶβ����Ҷ���
		StringFormat strFormatRight;
		strFormatRight.SetAlignment(StringAlignmentFar);	// �Ҷ���
		if(m_uVAlignment == VAlign_Top)
		{
			strFormatRight.SetLineAlignment(StringAlignmentNear);	// �϶���
		}else
		if(m_uVAlignment == VAlign_Middle)
		{
			strFormatRight.SetLineAlignment(StringAlignmentCenter);	// �м����
		}else
		if(m_uVAlignment == VAlign_Bottom)
		{
			strFormatRight.SetLineAlignment(StringAlignmentFar);	// �¶���
		}
		//strFormatRight.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		if(!m_bTextWrap)
		{
			strFormatRight.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}
		
		if(m_vecRowInfo.size() > 0)
		{
			for(int i = m_nFirstViewRow; i <= m_nLastViewRow && i < (int)m_vecRowInfo.size(); i++)
			{
				ListRowInfo &rowInfo = m_vecRowInfo.at(i);
				SolidBrush solidBrushRow(rowInfo.clrText);	// �ж������ɫ

				int nXPos = 0;
				int nVI = i - m_nFirstViewRow;

				// ����ƶ�����ʱ����ʾ�ı�����ɫ(�������Ϊȫ0,����ʾ�б�����ɫ)
				if((m_nHoverRow == i) && (m_clrRowHover.GetValue() != Color(0, 0, 0, 0).GetValue()))
				{
					SolidBrush brush(m_clrRowHover);
					graphics.FillRectangle(&brush, 0, nVI*m_nRowHeight, nWidth, m_nRowHeight);
				}else
				if(rowInfo.bRowBackColor)	// ����������еı�����ɫ,�������ɫ
				{
					SolidBrush brush(rowInfo.clrBack);
					graphics.FillRectangle(&brush, 0, nVI*m_nRowHeight, nWidth, m_nRowHeight);
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

				// ������
				RectF rect((Gdiplus::REAL)nXPos, (Gdiplus::REAL)(nVI*m_nRowHeight + 1), (Gdiplus::REAL)(nWidth-20), (Gdiplus::REAL)(m_bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2)) );
				if(!rowInfo.strTime.IsEmpty())
				{
					Size size = GetTextBounds(font, rowInfo.strTime);
					rect.Width -= (size.Width + 5);
				}

				// ��������
				int nLinkWidth = 0;
				if(!rowInfo.strLink2.IsEmpty())
				{
					Size sizeLink = GetTextBounds(font, strFormatRight, rowInfo.strLink2);
					nLinkWidth += (sizeLink.Width + 10);
					RectF rectLink((Gdiplus::REAL)(nWidth-nLinkWidth-nRightImageWidth), (Gdiplus::REAL)(nVI*m_nRowHeight + (m_nRowHeight - sizeLink.Height)/2), (Gdiplus::REAL)(sizeLink.Width+8), (Gdiplus::REAL)sizeLink.Height);
					rowInfo.rcLink2.SetRect((int)rectLink.X,(int)rectLink.Y,(int)(rectLink.X+sizeLink.Width),(int)(rectLink.Y+rectLink.Height));
					rowInfo.rcLink2.OffsetRect(0, m_nFirstViewRow*m_nRowHeight);
					if(((m_nHoverRow == i) || (m_nDownRow == i)) && (rowInfo.nHoverLink == 1))
					{
						BSTR bsLink2 = rowInfo.strLink2.AllocSysString();
						graphics.DrawString(bsLink2, (INT)wcslen(bsLink2), &font, rectLink, &strFormatRight, &solidBrushH);
						::SysFreeString(bsLink2);
					}else
					{
						BSTR bsLink2 = rowInfo.strLink2.AllocSysString();
						graphics.DrawString(bsLink2, (INT)wcslen(bsLink2), &font, rectLink, &strFormatRight, &solidBrush);
						::SysFreeString(bsLink2);
					}
				}
				if(!rowInfo.strLink1.IsEmpty())
				{
					Size sizeLink = GetTextBounds(font, strFormatRight, rowInfo.strLink1);
					nLinkWidth += (sizeLink.Width + 10);
					RectF rectLink((Gdiplus::REAL)(nWidth-nLinkWidth-nRightImageWidth), (Gdiplus::REAL)(nVI*m_nRowHeight + (m_nRowHeight - sizeLink.Height)/2), (Gdiplus::REAL)(sizeLink.Width+8), (Gdiplus::REAL)sizeLink.Height);
					rowInfo.rcLink1.SetRect((int)rectLink.X,(int)rectLink.Y,(int)(rectLink.X+sizeLink.Width),(int)(rectLink.Y+rectLink.Height));
					rowInfo.rcLink1.OffsetRect(0, m_nFirstViewRow*m_nRowHeight);
					if(((m_nHoverRow == i) || (m_nDownRow == i)) && (rowInfo.nHoverLink == 0))
					{
						BSTR bsLink1 = rowInfo.strLink1.AllocSysString();
						graphics.DrawString(bsLink1, (INT)wcslen(bsLink1), &font, rectLink, &strFormatRight, &solidBrushH);
						::SysFreeString(bsLink1);
					}else
					{
						BSTR bsLink1 = rowInfo.strLink1.AllocSysString();
						graphics.DrawString(bsLink1, (INT)wcslen(bsLink1), &font, rectLink, &strFormatRight, &solidBrush);
						::SysFreeString(bsLink1);
					}
				}
				
				rect.Width -= nLinkWidth;
				// �����Ƿ���Ҫ��ʾtip
				rowInfo.bNeedTitleTip = rect.Width < GetTextBounds(font, rowInfo.strTitle).Width;
				rowInfo.bNeedContentTip = rect.Width < GetTextBounds(font, rowInfo.strContent).Width;
				Size sizeTime = GetTextBounds(font, strFormatRight, rowInfo.strTime);
				int nTimeWidth = sizeTime.Width + 10;
				RectF rectTime((Gdiplus::REAL)(nWidth-nRightImageWidth-2-nTimeWidth-nLinkWidth), (Gdiplus::REAL)(nVI*m_nRowHeight + 1), (Gdiplus::REAL)nTimeWidth, (Gdiplus::REAL)(m_bSingleLine ? m_nRowHeight : (m_nRowHeight / 2)) );
				if(m_nHoverRow == i)
				{
					// ��������
					BSTR bsTitle = rowInfo.strTitle.AllocSysString();
					graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &fontTitle, rect, &strFormat, &solidBrushH);
					::SysFreeString(bsTitle);
					// ��ʱ��
					if(!rowInfo.strTime.IsEmpty())
					{
						BSTR bsTime = rowInfo.strTime.AllocSysString();
						graphics.DrawString(bsTime, (INT)wcslen(bsTime), &font, rectTime, &strFormatRight, &solidBrushH);
						::SysFreeString(bsTime);
					}
					// ��������
					if(!m_bSingleLine)
					{
						rect.Offset(0, (Gdiplus::REAL)m_nRowHeight / 2 + 2);
						rect.Width = (Gdiplus::REAL)nWidth-20;
						rect.Height = (Gdiplus::REAL)m_nRowHeight / 2 - 4;
						BSTR bsContent = rowInfo.strContent.AllocSysString();
						graphics.DrawString(bsContent, (INT)wcslen(bsContent), &font, rect, &strFormat, &solidBrushH);
						::SysFreeString(bsContent);
					}
				}else
				if(m_nDownRow == i)
				{
					// ��������
					BSTR bsTitle = rowInfo.strTitle.AllocSysString();
					graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &fontTitle, rect, &strFormat, &solidBrushD);
					::SysFreeString(bsTitle);
					// ��ʱ��
					if(!rowInfo.strTime.IsEmpty())
					{
						BSTR bsTime = rowInfo.strTime.AllocSysString();
						graphics.DrawString(bsTime, (INT)wcslen(bsTime), &font, rectTime, &strFormatRight, &solidBrushD);
						::SysFreeString(bsTime);
					}
					// ��������
					if(!m_bSingleLine)
					{
						rect.Offset(0, (Gdiplus::REAL)m_nRowHeight / 2 + 2);
						rect.Width = (Gdiplus::REAL)nWidth-20;
						rect.Height = (Gdiplus::REAL)m_nRowHeight / 2 - 4;
						BSTR bsContent = rowInfo.strContent.AllocSysString();
						graphics.DrawString(bsContent, (INT)wcslen(bsContent), &font, rect, &strFormat, &solidBrushD);
						::SysFreeString(bsContent);
					}
				}else
				{
					// ��������
					BSTR bsTitle = rowInfo.strTitle.AllocSysString();
					graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &fontTitle, 
						rect, &strFormat, rowInfo.bRowColor ? &solidBrushRow : &solidBrushT);
					::SysFreeString(bsTitle);
					// ��ʱ��
					if(!rowInfo.strTime.IsEmpty())
					{
						BSTR bsTime = rowInfo.strTime.AllocSysString();
						graphics.DrawString(bsTime, (INT)wcslen(bsTime), &font, 
						rectTime, &strFormatRight, rowInfo.bRowColor ? &solidBrushRow : &solidBrush);
						::SysFreeString(bsTime);
					}
					// ��������
					if(!m_bSingleLine)
					{
						rect.Offset(0, (Gdiplus::REAL)m_nRowHeight / 2 + 2);
						rect.Width = (Gdiplus::REAL)nWidth-20;
						rect.Height = (Gdiplus::REAL)m_nRowHeight / 2 - 4;
						BSTR bsContent = rowInfo.strContent.AllocSysString();
						graphics.DrawString(bsContent, (INT)wcslen(bsContent), &font, 
								rect, &strFormat, rowInfo.bRowColor ? &solidBrushRow : &solidBrush);
						::SysFreeString(bsContent);
					}
				}

				// ���ָ���(��������ģʽ)
				if(m_pImageSeperator != NULL)
				{
					// ʹ������ģʽ���Ի�ͼ
					graphics.DrawImage(m_pImageSeperator, RectF(0, (Gdiplus::REAL)((nVI+1)*m_nRowHeight), (Gdiplus::REAL)(nWidth-2), (Gdiplus::REAL)m_sizeSeperator.cy),
							0, 0, (Gdiplus::REAL)m_sizeSeperator.cx, (Gdiplus::REAL)m_sizeSeperator.cy, UnitPixel);

					// ʹ��ƽ�̷�ʽ������(�ݲ�ʹ�����ַ�ʽ)
					//TextureBrush tileBrush(m_pImageSeperator, WrapModeTile);
					//graphics.FillRectangle(&tileBrush, RectF(0, (nVI+1)*m_nRowHeight, nWidth-2, m_sizeSeperator.cy));
				}else
				if(m_clrSeperator.GetValue() != Color(0, 0, 0, 0).GetValue())
				{
					// δָ��ͼƬ,���ҷָ�����ɫ����ȫ0,�򻭾���
					graphics.FillRectangle(&solidBrushS, 0, (nVI+1)*m_nRowHeight, nWidth-2, 1);
				}
			}
		}
	}

	// ���������DC,ʹ����ķ�ʽ�ϲ�����
	dc.BitBlt(m_rc.left,m_rc.top, nWidth, m_rc.Height(), &m_memDC, 0, nYViewPos, SRCCOPY);//SRCAND);
}