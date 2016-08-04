#include "StdAfx.h"
#include "DuiLayout.h"

CDuiLayout::CDuiLayout(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBaseFont(hWnd, pDuiObject)
{
	m_nLayoutType = LAYOUT_TYPE_HORIZONTAL;
	m_bInit = FALSE;
	m_bHover = false;
	m_nHoverSplit = -1;
	m_enButtonState = enBSNormal;

	m_bRunTime = false;

	SetBitmapCount(2);

	m_pImageSplit = NULL;
	m_sizeSplit = CSize(0, 0);
	m_pImageThumb = NULL;
	m_sizeThumb = CSize(0, 0);
	m_nSplitWidth = 0;
	m_bSplitImm = TRUE;
	//m_nThumbWidth = 0;
	//m_nThumbHeight = 0;
	//m_nThumbPos = 0;
}

CDuiLayout::CDuiLayout(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress/* = 0*/, 
					 BOOL bIsVisible/* = TRUE*/, BOOL bIsDisable/* = FALSE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, TEXT(""), bIsVisible, bIsDisable)
{
	m_nLayoutType = LAYOUT_TYPE_HORIZONTAL;
	m_bInit = FALSE;
	m_bHover = false;
	m_nHoverSplit = -1;
	m_enButtonState = enBSNormal;

	m_bRunTime = false;

	SetBitmapCount(2);

	m_pImageSplit = NULL;
	m_sizeSplit = CSize(0, 0);
	m_pImageThumb = NULL;
	m_sizeThumb = CSize(0, 0);
	m_nSplitWidth = 0;
	m_bSplitImm = TRUE;
	//m_nThumbWidth = 0;
	//m_nThumbHeight = 0;
	//m_nThumbPos = 0;
}

CDuiLayout::~CDuiLayout(void)
{
	if(m_pImageSplit != NULL)
	{
		delete m_pImageSplit;
		m_pImageSplit = NULL;
	}
	if(m_pImageThumb != NULL)
	{
		delete m_pImageThumb;
		m_pImageThumb = NULL;
	}
}

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiLayout, Split, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiLayout, Thumb, 1)

// ���ؼ���XML�ڵ㺯���������²��div����
BOOL CDuiLayout::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	__super::Load(pXmlElem);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}
	
	// �����²��div�ڵ���Ϣ
	for (DuiXmlNode pDivElem = pXmlElem.child(_T("layout-div")); pDivElem; pDivElem=pDivElem.next_sibling(_T("layout-div")))
	{
		CString strDivPos = pDivElem.attribute(_T("div-pos")).value();
		int nMinPos = _ttoi(pDivElem.attribute(_T("min-pos")).value());
		int nMaxPos = _ttoi(pDivElem.attribute(_T("max-pos")).value());

		// ����div
		CDuiPanel*	pControlPanel = (CDuiPanel*)DuiSystem::CreateControlByName(_T("div"), m_hWnd, this);
 		m_vecControl.push_back(pControlPanel);

		// ����XML��Tab�ڵ�ĸ����²�ؼ��ڵ�
		pControlPanel->Load(pDivElem);

		LayoutItemInfo itemInfo;
		itemInfo.pControlPanel = pControlPanel;
		ParsePosition(strDivPos, itemInfo.pos);	// ����pos��Ϣ
		itemInfo.nMinPos = nMinPos;
		itemInfo.nMaxPos = nMaxPos;
		itemInfo.nPos = -1;	// ��ʼ��ʵ�ʵ�λ��ֵ
		itemInfo.rcSplit = CRect(0, 0, 0, 0);	// ��ʼ���ָ�������
		itemInfo.rcThumb = CRect(0, 0, 0, 0);	// ��ʼ����������
		m_vecItemInfo.push_back(itemInfo);
	}

	// ��ʼ��div�пؼ���λ��
	SetRect(m_rc);

	m_bInit = TRUE;

    return TRUE;
}

// ��ȡLayout��Ϣ
LayoutItemInfo* CDuiLayout::GetLayoutItemInfo(int nItem)
{
	if(nItem <= -1 || nItem >= (int)m_vecItemInfo.size())
	{
		return NULL;
	}

	return &m_vecItemInfo.at(nItem);
}

// ��ȡLayout��ʵ��λ��
int CDuiLayout::GetLayoutPos(LayoutItemInfo* pLayoutInfo)
{
	if(pLayoutInfo == NULL)
	{
		return 0;
	}

	// ����Ѿ�������ʵ�ʵ�ֵ,�������õ�ֵ
	if(pLayoutInfo->nPos != -1)
	{
		return pLayoutInfo->nPos;
	}

	// ����Ԥ�ò������õ�ֵ
	int nPos = 0;
	if(m_nLayoutType == LAYOUT_TYPE_HORIZONTAL)
	{
		nPos = PositionItem2Value(pLayoutInfo->pos, 0, m_rc.Height());
	}else
	if(m_nLayoutType == LAYOUT_TYPE_VERTICAL)
	{
		nPos = PositionItem2Value(pLayoutInfo->pos, 0, m_rc.Width());
	}

	return nPos;
}

// ����ָ���ָ�ڵ�λ��
int CDuiLayout::SetSplitPos(int nSplitItem, int nSplitPos)
{
	if(GetDisable())
	{
		return nSplitPos;
	}

	if(nSplitItem <= -1 || nSplitItem >= (int)m_vecItemInfo.size())
	{
		return nSplitPos;
	}

	LayoutItemInfo* pLayoutInfo1 = GetLayoutItemInfo(nSplitItem);
	//LayoutItemInfo* pLayoutInfo2 = GetLayoutItemInfo(nSplitItem+1);
	if(pLayoutInfo1 == NULL)
	{
		return nSplitPos;
	}

	int nOldSplitPos = GetLayoutPos(pLayoutInfo1);
	pLayoutInfo1->nPos = nSplitPos;
	// ����ֵ�������ֵ,�Զ�����Ϊ���ֵ
	if(m_nLayoutType == LAYOUT_TYPE_HORIZONTAL)
	{
		if(nSplitPos > m_rc.Height())
		{
			pLayoutInfo1->nPos = m_rc.Height();
		}
	}else
	if(m_nLayoutType == LAYOUT_TYPE_VERTICAL)
	{
		if(nSplitPos > m_rc.Width())
		{
			pLayoutInfo1->nPos = m_rc.Width();
		}
	}
	if(nSplitPos < 0)
	{
		pLayoutInfo1->nPos = 0;
	}
	if((pLayoutInfo1->nMinPos != 0) && (nSplitPos < pLayoutInfo1->nMinPos))
	{
		pLayoutInfo1->nPos = pLayoutInfo1->nMinPos;
	}
	if((pLayoutInfo1->nMaxPos != 0) && (nSplitPos > pLayoutInfo1->nMaxPos))
	{
		pLayoutInfo1->nPos = pLayoutInfo1->nMaxPos;
	}

	// ���¼���ָ�ڵ�λ��
	SetRect(m_rc);

	return pLayoutInfo1->nPos;
}

void CDuiLayout::SetControlRect(CRect rc)
{
	m_rc = rc;
	if(m_nSplitWidth == 0)
	{
		m_nSplitWidth = (m_nLayoutType == LAYOUT_TYPE_HORIZONTAL) ? m_sizeSplit.cy : m_sizeSplit.cx;
	}
	/*if(m_nThumbWidth == 0)
	{
		m_nThumbWidth = m_sizeThumb.cx;
	}
	if(m_nThumbHeight == 0)
	{
		m_nThumbHeight = m_sizeThumb.cy;
	}*/

	// ����panel���ָ��ߡ������λ��
	if(m_nLayoutType == LAYOUT_TYPE_HORIZONTAL)
	{
		int nTop = m_rc.top;
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			LayoutItemInfo &itemInfo = m_vecItemInfo.at(i);
			int nLayoutPos = GetLayoutPos(&itemInfo);
			int nPosBottom = nTop+nLayoutPos-1;
			if(i == (m_vecItemInfo.size() - 1))
			{
				nPosBottom = m_rc.bottom;
			}
			CRect rcPanel(m_rc.left, nTop, m_rc.right, nPosBottom);
			itemInfo.pControlPanel->SetRect(rcPanel);
			itemInfo.pControlPanel->OnPositionChange();
			itemInfo.rcSplit  = CRect(m_rc.left, nTop+nLayoutPos, m_rc.right, nTop+nLayoutPos+m_nSplitWidth);
			int nThumbX = m_rc.left + (m_rc.Width() - m_sizeThumb.cx) / 2;
			int nThumbY = nTop + nLayoutPos + (m_nSplitWidth - m_sizeThumb.cy) /2 ;
			itemInfo.rcThumb  = CRect(nThumbX, nThumbY, nThumbX+m_sizeThumb.cx, nThumbY+m_sizeThumb.cy);
			nTop += (nLayoutPos+m_nSplitWidth);
		}
	}else
	if(m_nLayoutType == LAYOUT_TYPE_VERTICAL)
	{
		int nLeft = m_rc.left;
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			LayoutItemInfo &itemInfo = m_vecItemInfo.at(i);
			int nLayoutPos = GetLayoutPos(&itemInfo);
			int nPosRight = nLeft+nLayoutPos-1;
			if(i == (m_vecItemInfo.size() - 1))
			{
				nPosRight = m_rc.right;
			}
			CRect rcPanel(nLeft, m_rc.top, nPosRight, m_rc.bottom);
			itemInfo.pControlPanel->SetRect(rcPanel);
			itemInfo.pControlPanel->OnPositionChange();
			itemInfo.rcSplit = CRect(nLeft+nLayoutPos, m_rc.top, nLeft+nLayoutPos+m_nSplitWidth, m_rc.bottom);
			int nThumbX = nLeft + nLayoutPos + (m_nSplitWidth - m_sizeThumb.cx) /2 ;
			int nThumbY = m_rc.top + (m_rc.Height() - m_sizeThumb.cy) / 2;
			itemInfo.rcThumb  = CRect(nThumbX, nThumbY, nThumbX+m_sizeThumb.cx, nThumbY+m_sizeThumb.cy);
			nLeft += (nLayoutPos+m_nSplitWidth);
		}
	}
}

// �������ÿؼ��ɼ��Եĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiLayout::SetControlVisible(BOOL bIsVisible)
{
	__super::SetControlVisible(bIsVisible);

	// ����ÿ���ӿؼ���ԭ��Windows�ؼ��Ŀɼ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			if(pControlBase->IsClass(_T("div")) || pControlBase->IsClass(_T("tabctrl")) || pControlBase->IsClass(_T("layout")))
			{
				// ����ӿؼ����������Ϳؼ�,������ӿؼ������ÿɼ��Ժ���
				pControlBase->SetControlVisible(bIsVisible);
			}else
			{
				// �ж��ӿؼ���ǰ�Ƿ�ɼ�,���ݿɼ��������ӿؼ���ԭ���ؼ��Ŀɼ���
				pControlBase->SetControlWndVisible(pControlBase->GetVisible());
			}
		}
	}
}

// �ƶ��ָ���λ��
int CDuiLayout::MoveSplitPos(int nSplitItem, CPoint point)
{
	int nPos = 0;
	if(m_nLayoutType == LAYOUT_TYPE_HORIZONTAL)
	{
		nPos = point.y - m_rc.top;
		SetSplitPos(nSplitItem, nPos);
	}else
	if(m_nLayoutType == LAYOUT_TYPE_VERTICAL)
	{
		nPos = point.x - m_rc.left;
		SetSplitPos(nSplitItem, nPos);
	}

	return nPos;
}

// ����ƶ�
BOOL  CDuiLayout::OnControlMouseMove(UINT nFlags, CPoint point)
{		
	enumButtonState buttonState = m_enButtonState;
	BOOL bHover = m_bHover;
	if (!m_bIsDisable)
	{
		if(m_enButtonState != enBSDown)
		{
			BOOL bMouseHover = FALSE;
			if(!m_bMouseDown)
			{
				for(size_t i = 0; i < m_vecItemInfo.size(); i++)
				{
					LayoutItemInfo &itemInfo = m_vecItemInfo.at(i);
					if(itemInfo.rcSplit.PtInRect(point))
					{
						m_bHover = true;
						bMouseHover = TRUE;
						if(itemInfo.rcThumb.PtInRect(point))
						{
							m_enButtonState = enBSHover;
						}
						else
						{
							m_enButtonState = enBSNormal;
						}
						// ���������״
						if(m_nLayoutType == LAYOUT_TYPE_HORIZONTAL)
						{
							m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS));	// ˫��ͷָ���ϱ�
						}else
						if(m_nLayoutType == LAYOUT_TYPE_VERTICAL)
						{
							m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE));	// ˫��ͷָ����
						}
						break;
					}
				}

				if(!bMouseHover)
				{
					m_bHover = false;
					m_enButtonState = enBSNormal;
					// ��ԭ�����״
					m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));	// ��ͷ
				}
			}
		}
		else
		{
			// ��갴�ŷָ��߻������,�϶��������,������õ�����ˢ��,���ƶ��ָ��߷�Χ
			if(m_bSplitImm)
			{
				MoveSplitPos(m_nHoverSplit, point);
			}
			return true;
		}
	}

	if(buttonState != m_enButtonState || !m_bUpdate || bHover != m_bHover)
	{
		SetUpdate(FALSE);
		UpdateControl();
		return true;
	}
	return false;
}

// ����������
BOOL CDuiLayout::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			LayoutItemInfo &itemInfo = m_vecItemInfo.at(i);
			if(itemInfo.rcThumb.PtInRect(point))
			{
				// ����ڹ�������,���¼���λ��
				m_enButtonState = enBSDown;
				m_nHoverSplit = i;
				MoveSplitPos(i, point);
				break;
			}else
			if(itemInfo.rcSplit.PtInRect(point))
			{
				// ����ڷָ�����,���¼���λ��
				m_enButtonState = enBSDown;
				m_nHoverSplit = i;
				MoveSplitPos(i, point);
				break;
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

// �������ſ�
BOOL  CDuiLayout::OnControlLButtonUp(UINT nFlags, CPoint point)
{	
	enumButtonState buttonState = m_enButtonState;
	if (!m_bIsDisable)
	{
		m_enButtonState = enBSNormal;
		for(size_t i = 0; i < m_vecItemInfo.size(); i++)
		{
			LayoutItemInfo &itemInfo = m_vecItemInfo.at(i);
			if(itemInfo.rcThumb.PtInRect(point))
			{
				m_enButtonState = enBSHover;
				break;
			}else
			if(itemInfo.rcSplit.PtInRect(point))
			{
				m_enButtonState = enBSHover;
				break;
			}
		}

		// ���ſ��ָ��߻������,������õĲ�����ˢ��,���ƶ��ָ��߷�Χ
		if(!m_bSplitImm && (m_nHoverSplit != -1))
		{
			MoveSplitPos(m_nHoverSplit, point);
		}
	}

	m_nHoverSplit = -1;
	
	if(buttonState != m_enButtonState)
	{
		UpdateControl();
		return true;
	}
	return false;
}

void CDuiLayout::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeight * 4);

		Graphics graphics(m_memDC);

		// ��4��״̬���ڴ�ͼƬ
		for(int i = 0; i < 4; i++)
		{
			// ���Ʊ���
			m_memDC.BitBlt(0, i * nHeight, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

			int nPosY = i * nHeight;

			if(m_pImageSplit != NULL)	// ʹ�õ���ͼƬ���ָ���
			{
				for(size_t i = 0; i < m_vecItemInfo.size(); i++)
				{
					LayoutItemInfo &itemInfo = m_vecItemInfo.at(i);
					if(i != (m_vecItemInfo.size() - 1))
					{
						Rect rect(itemInfo.rcSplit.left-m_rc.left, nPosY+itemInfo.rcSplit.top-m_rc.top, itemInfo.rcSplit.Width(), itemInfo.rcSplit.Height());
						graphics.DrawImage(m_pImageSplit, rect, 0, 0, m_sizeSplit.cx, m_sizeSplit.cy, UnitPixel);
					}
				}
			}

			// ������
			if(m_pImageThumb != NULL)
			{
				for(size_t i = 0; i < m_vecItemInfo.size(); i++)
				{
					LayoutItemInfo &itemInfo = m_vecItemInfo.at(i);
					if(i != (m_vecItemInfo.size() - 1))
					{
						Rect rect(itemInfo.rcThumb.left-m_rc.left, nPosY+itemInfo.rcThumb.top-m_rc.top, itemInfo.rcThumb.Width(), itemInfo.rcThumb.Height());
						graphics.DrawImage(m_pImageThumb, rect, i * m_sizeThumb.cx, 0, m_sizeThumb.cx, m_sizeThumb.cy, UnitPixel);
					}
				}
			}
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, 0, m_enButtonState * nHeight, SRCCOPY);
}