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

// 图片属性的实现
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiLayout, Split, 1)
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDuiLayout, Thumb, 1)

// 重载加载XML节点函数，加载下层的div内容
BOOL CDuiLayout::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	__super::Load(pXmlElem);

	if(pXmlElem == NULL)
	{
		return FALSE;
	}
	
	// 加载下层的div节点信息
	for (DuiXmlNode pDivElem = pXmlElem.child(_T("layout-div")); pDivElem; pDivElem=pDivElem.next_sibling(_T("layout-div")))
	{
		CString strDivPos = pDivElem.attribute(_T("div-pos")).value();
		int nMinPos = _ttoi(pDivElem.attribute(_T("min-pos")).value());
		int nMaxPos = _ttoi(pDivElem.attribute(_T("max-pos")).value());

		// 创建div
		CDuiPanel*	pControlPanel = (CDuiPanel*)DuiSystem::CreateControlByName(_T("div"), m_hWnd, this);
 		m_vecControl.push_back(pControlPanel);

		// 加载XML中Tab节点的各个下层控件节点
		pControlPanel->Load(pDivElem);

		LayoutItemInfo itemInfo;
		itemInfo.pControlPanel = pControlPanel;
		ParsePosition(strDivPos, itemInfo.pos);	// 解析pos信息
		itemInfo.nMinPos = nMinPos;
		itemInfo.nMaxPos = nMaxPos;
		itemInfo.nPos = -1;	// 初始化实际的位置值
		itemInfo.rcSplit = CRect(0, 0, 0, 0);	// 初始化分割线区域
		itemInfo.rcThumb = CRect(0, 0, 0, 0);	// 初始化滑块区域
		m_vecItemInfo.push_back(itemInfo);
	}

	// 初始化div中控件的位置
	SetRect(m_rc);

	m_bInit = TRUE;

    return TRUE;
}

// 获取Layout信息
LayoutItemInfo* CDuiLayout::GetLayoutItemInfo(int nItem)
{
	if(nItem <= -1 || nItem >= (int)m_vecItemInfo.size())
	{
		return NULL;
	}

	return &m_vecItemInfo.at(nItem);
}

// 获取Layout的实际位置
int CDuiLayout::GetLayoutPos(LayoutItemInfo* pLayoutInfo)
{
	if(pLayoutInfo == NULL)
	{
		return 0;
	}

	// 如果已经设置了实际的值,就用设置的值
	if(pLayoutInfo->nPos != -1)
	{
		return pLayoutInfo->nPos;
	}

	// 返回预置参数设置的值
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

// 设置指定分割窗口的位置
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
	// 设置值超过最大值,自动更改为最大值
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

	// 重新计算分割窗口的位置
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

	// 计算panel、分割线、滑块的位置
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

// 重载设置控件可见性的函数，需要调用子控件的函数
void CDuiLayout::SetControlVisible(BOOL bIsVisible)
{
	__super::SetControlVisible(bIsVisible);

	// 设置控件和子控件的原生Windows控件的可见性
	SetControlWndVisible(bIsVisible);
}

// 重载设置控件中windows原生控件可见性的函数，需要调用子控件的函数
void CDuiLayout::SetControlWndVisible(BOOL bIsVisible)
{
	__super::SetControlWndVisible(bIsVisible);

	// 设置每个子控件的原生Windows控件的可见性
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

// 移动分隔线位置
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

// 鼠标移动
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
						// 设置鼠标形状
						if(m_nLayoutType == LAYOUT_TYPE_HORIZONTAL)
						{
							m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS));	// 双箭头指向南北
						}else
						if(m_nLayoutType == LAYOUT_TYPE_VERTICAL)
						{
							m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE));	// 双箭头指向东西
						}
						break;
					}
				}

				if(!bMouseHover)
				{
					m_bHover = false;
					m_enButtonState = enBSNormal;
					// 还原鼠标形状
					m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));	// 箭头
				}
			}
		}
		else
		{
			// 鼠标按着分隔线或滚动块,拖动的情况下,如果配置的立即刷新,则移动分割线范围
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

// 鼠标左键按下
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
				// 如果在滚动块内,则记录鼠标位置
				m_enButtonState = enBSDown;
				m_nHoverSplit = i;
				MoveSplitPos(i, point);
				break;
			}else
			if(itemInfo.rcSplit.PtInRect(point))
			{
				// 如果在分隔线内,则记录鼠标位置
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

// 鼠标左键放开
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

		// 鼠标放开分隔线或滚动块,如果配置的不立即刷新,则移动分割线范围
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

		// 画4个状态的内存图片
		for(int i = 0; i < 4; i++)
		{
			// 复制背景
			m_memDC.BitBlt(0, i * nHeight, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

			int nPosY = i * nHeight;

			if(m_pImageSplit != NULL)	// 使用单张图片画分隔线
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

			// 画滑块
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