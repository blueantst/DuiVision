// DlgPopup.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgPopup.h"


// CDlgPopup

IMPLEMENT_DYNAMIC(CDlgPopup, CWnd)

CDlgPopup::CDlgPopup() : CDuiObject()
{
	m_pParentDuiObject = NULL;

	m_bInit = false;
	m_pImage = NULL;
	m_bTracking = false;
	m_bIsLButtonDown = FALSE;
	m_bIsLButtonDblClk = FALSE;
	m_bIsRButtonDown = FALSE;
	m_bIsRButtonDblClk = FALSE;
	m_bIsSetCapture = false;
	m_uMessageID = 0;
	m_enBackMode = enBMFrame;
	m_bBackAlpha = TRUE;
	m_nBackTranslucent = 255;
	m_nFrameSize = 4;
	m_nFrameWLT = 0;
	m_nFrameHLT = 0;
	m_nFrameWRB = 0;
	m_nFrameHRB = 0;
	m_bInitFinish = false;
	m_bAutoClose = true;
	m_bTopMost = true;
	m_bImageUseECM = false;

	m_pControl = NULL;
	m_pFocusControl = NULL;

	m_uTimerAnimation = 0;
	m_size = CSize(0,0);

	m_strXmlFile = _T("");
	m_strXmlContent = _T("");
}

CDlgPopup::~CDlgPopup()
{
	// 结束定时器
	CTimer::KillTimer();

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			delete pControlBase;
		}		
	}

	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			delete pControlBase;
		}		
	}

	if(m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}


BEGIN_MESSAGE_MAP(CDlgPopup, CWnd)
	ON_WM_CREATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_NCACTIVATE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CDlgPopup::Create(CWnd *pParent, CRect rc, UINT uMessageID, UINT nResourceID, enumBackMode enBackMode/* = enBMFrame*/, int nFrameSize/* = 4*/)
{
	CString sWindowClassName = AfxRegisterWndClass(CS_DBLCLKS,::LoadCursor(NULL, IDC_ARROW));

	m_uMessageID = uMessageID;
	m_point.SetPoint(rc.left, rc.top);
	m_size.SetSize(rc.Width(), rc.Height());

	m_enBackMode = enBackMode;
	m_nFrameSize = nFrameSize;

	SetBackBitmap(nResourceID);

	DWORD dwStyle = WS_EX_TOOLWINDOW;
	if(m_bTopMost)	// 窗口总在最前面
	{
		dwStyle |= WS_EX_TOPMOST;
	}
	if(!CWnd::CreateEx(dwStyle, sWindowClassName, NULL, WS_POPUP, rc, pParent, 0, NULL))
	{
		return	FALSE;
	}

	return TRUE;
}

BOOL CDlgPopup::Create(CWnd *pParent, CRect rc, UINT uMessageID, CString strImage, enumBackMode enBackMode/* = enBMFrame*/, int nFrameSize/* = 4*/)
{
	CString sWindowClassName = AfxRegisterWndClass(CS_DBLCLKS,::LoadCursor(NULL, IDC_ARROW));

	m_uMessageID = uMessageID;
	m_point.SetPoint(rc.left, rc.top);
	m_size.SetSize(rc.Width(), rc.Height());

	m_enBackMode = enBackMode;
	m_nFrameSize = nFrameSize;

	SetBackBitmap(strImage);

	DWORD dwStyle = WS_EX_TOOLWINDOW;
	if(m_bTopMost)	// 窗口总在最前面
	{
		dwStyle |= WS_EX_TOPMOST;
	}
	if(!CWnd::CreateEx(dwStyle, sWindowClassName, NULL, WS_POPUP, rc, pParent, 0, NULL))
	{
		return	FALSE;
	}

	return TRUE;
}

BOOL CDlgPopup::Create(CWnd *pParent, CRect rc, UINT uMessageID)
{
	CString sWindowClassName = AfxRegisterWndClass(CS_DBLCLKS,::LoadCursor(NULL, IDC_ARROW));

	m_uMessageID = uMessageID;
	m_point.SetPoint(rc.left, rc.top);
	// 如果窗口大小为0才使用传入的参数作为窗口大小
	if((m_size.cx == 0) || (m_size.cy == 0))
	{
		m_size.SetSize(rc.Width(), rc.Height());
	}else
	{
		// 按照m_size大小设置窗口的大小
		rc.right = rc.left + m_size.cx;
		rc.bottom = rc.top + m_size.cy;
	}

	DWORD dwStyle = WS_EX_TOOLWINDOW;
	if(m_bTopMost)	// 窗口总在最前面
	{
		dwStyle |= WS_EX_TOPMOST;
	}
	if(!CWnd::CreateEx(dwStyle, sWindowClassName, NULL, WS_POPUP, rc, pParent, 0, NULL))
	{
		return	FALSE;
	}

	return TRUE;
}

int CDlgPopup::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// 设置窗口风格
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE)
		| WS_SIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_EX_LAYERED;
	dwStyle &= ~(WS_CAPTION);

	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	::SetWindowLong(m_hWnd,GWL_EXSTYLE, WS_EX_TOOLWINDOW); 	

	SetBackMode(m_enBackMode);
	
	m_bInitFinish = true;
	
	CRect rcClient;
	GetClientRect(&rcClient);

	InitUI(rcClient);

	//DrawWindow();

	//启动定时器
	m_uTimerAnimation = CTimer::SetTimer(30);

	m_bInit = true;

	return TRUE;
}

// 根据控件名创建控件实例
CControlBase* CDlgPopup::_CreateControlByName(LPCTSTR lpszName)
{
	return DuiSystem::CreateControlByName(lpszName, GetSafeHwnd(), this);
}

// 加载XML节点，解析节点中的属性信息设置到当前控件的属性中
BOOL CDlgPopup::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

	if(!bLoadSubControl)
	{
		// 不加载子控件
		return TRUE;
	}

	// 更新窗口大小
	SetRect(CRect(0, 0, m_size.cx, m_size.cy));

    // 使用XML节点初始化控件
	if(pXmlElem != NULL)
	{
		InitUI(m_rc, pXmlElem);
	}

	// 调用事件处理对象的初始化函数
	if(m_pDuiHandler)
	{
		m_pDuiHandler->OnInit();
	}

    return TRUE;
}

// 加载XML文件
BOOL CDlgPopup::LoadXmlFile(CString strFileName)
{
	DuiXmlDocument xmlDoc;
	DuiXmlNode pDivElem;

	if(DuiSystem::Instance()->LoadXmlFile(xmlDoc, strFileName))
	{
		m_strXmlFile = strFileName;
		pDivElem = xmlDoc.child((const LPCTSTR)GetClassName());
		if(pDivElem)
		{
			// 加载popup节点属性
			Load(pDivElem);
		}
	}

	return TRUE;
}

// 加载XML内容
BOOL CDlgPopup::LoadXmlContent(CString strXmlContent)
{
	DuiXmlDocument xmlDoc;
	DuiXmlNode pDivElem;

	m_strXmlContent = strXmlContent;
	DuiXmlParseResult result = xmlDoc.load(m_strXmlContent);
	if(result)
	{
		pDivElem = xmlDoc.child((const LPCTSTR)GetClassName());
		if(pDivElem)
		{
			// 加载popup节点属性
			Load(pDivElem);
		}
	}

	return TRUE;
}

// UI初始化,此函数在窗口的OnCreate函数中调用
void CDlgPopup::InitUI(CRect rcClient)
{
	// 设置所有控件的父窗口句柄,因为DlgPopup在加载控件时候窗口还没有创建,设置的窗口句柄都为空,因此需要在窗口创建之后设置所有控件的窗口句柄
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetHWND(m_hWnd);
		}
	}

	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			pControlBase->SetHWND(m_hWnd);
		}
	}
}

// 初始化窗口控件
void CDlgPopup::InitUI(CRect rcClient, DuiXmlNode pNode)
{
	CRect rcTemp;
	int nStartX = 0;
	int nStartY = 0;
	CControlBase * pControlBase = NULL;

	// 加载所有窗口控件
	if(pNode)
	{
		for (DuiXmlNode pControlElem = pNode.first_child(); pControlElem; pControlElem=pControlElem.next_sibling())
		{
			if(pControlElem)
			{
				CString strControlName = pControlElem.name();
				CControlBase* pControl = _CreateControlByName(strControlName);
				if(pControl)
				{
					if(pControl->Load(pControlElem))
					{
						// 如果Load成功,则添加控件
						if(pControl->IsClass(CArea::GetClassName()))
						{
							// Area不能响应鼠标,必须加到Area列表中
							m_vecArea.push_back(pControl);
						}else
						{
							m_vecControl.push_back(pControl);
						}
					}else
					{
						// 否则直接删除控件对象指针
						delete pControl;
					}
				}
			}
		}
	}

	m_bInit = true;
}

void CDlgPopup::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

	CRect	rcWindow;

	if (bCalcValidRects && lpncsp->lppos)
	{
		rcWindow.SetRect(lpncsp->lppos->x, lpncsp->lppos->y,
			lpncsp->lppos->x + lpncsp->lppos->cx,
			lpncsp->lppos->y + lpncsp->lppos->cy);
	}
	else
	{
		GetWindowRect(rcWindow);
	}

	lpncsp->rgrc[0] = rcWindow;

}

LRESULT CDlgPopup::OnNcHitTest(CPoint point)
{
	return	HTCLIENT;
}

BOOL CDlgPopup::OnEraseBkgnd (CDC* pDC)
{
	return TRUE;
}

void CDlgPopup::SetBackMode(enumBackMode enBackMode)
{	
	if(m_enBackMode == enBMImage && m_pImage != NULL)
	{
		m_enBackMode = enBackMode;
		SetWindowPos(NULL, m_point.x, m_point.y, m_sizeBKImage.cx, m_sizeBKImage.cy, SWP_SHOWWINDOW);
		m_size = m_sizeBKImage;
	}
	else
	{
		m_enBackMode = enBackMode;
	}
	//DrawWindow();
	InvalidateRect(NULL);
}

void CDlgPopup::SetFrameSize(int nFrameSize)
{
	m_nFrameSize = nFrameSize;
	InvalidateRect(NULL);
}

void CDlgPopup::SetBackBitmap(UINT nResourceID)
{
	if(LoadImageFromIDResource(nResourceID, TEXT("PNG"), m_bImageUseECM, m_pImage))
	{
		m_sizeBKImage.SetSize(m_pImage->GetWidth(), m_pImage->GetHeight());
		//DrawWindow();
		if(m_bInitFinish)
		{
			InvalidateRect(NULL);
		}
	}
}

void CDlgPopup::SetBackBitmap(CString strImage)
{
	if(DuiSystem::Instance()->LoadImageFile(strImage, FALSE, m_pImage))
	{
		m_sizeBKImage.SetSize(m_pImage->GetWidth(), m_pImage->GetHeight());
		//DrawWindow();
		if(m_bInitFinish)
		{
			InvalidateRect(NULL);
		}
	}
}

// 从XML设置背景模式属性
HRESULT CDlgPopup::OnAttributeBkMode(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	if(strValue == _T("image"))
	{
		m_enBackMode = enBMImage;
	}else
	if(strValue == _T("frame"))
	{
		m_enBackMode = enBMFrame;
	}else
	if(strValue == _T("mid"))
	{
		m_enBackMode = enBMMID;
	}else
	{
		return E_FAIL;
	}

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置图片信息属性
HRESULT CDlgPopup::OnAttributeBkImage(const CString& strValue, BOOL bLoading)
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
		SetBackBitmap(strImgFile);
	}else	// 加载图片资源
	{
		UINT nResourceID = _ttoi(strSkin);
		SetBackBitmap(nResourceID);
	}

	return bLoading?S_FALSE:S_OK;
}

void CDlgPopup::CloseWindow()
{
	CWnd::PostMessage(WM_CLOSE);
	m_bInit = false;
}

void CDlgPopup::DrawWindow(CDC &dc)
{
	if(!m_bInitFinish) return;
	DrawWindow(&dc);
}

void CDlgPopup::DrawWindow()
{
	if(!m_bInitFinish) return;

	CDC *pDC = GetDC();
	DrawWindow(pDC);
	ReleaseDC(pDC);
}

void CDlgPopup::DrawWindow(CDC *pDC)
{
	if(!m_bInitFinish) return;

	CRect rcClient;
	GetClientRect(&rcClient);

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);

	CBitmap memBit;
	//memBit.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());
	// 因为直接使用CreateCompatibleBitmap在16位颜色下会有问题,因此改为调用DuiCreateCompatibleBitmap函数
	HBITMAP	hMemBitmap = DuiCreateCompatibleBitmap(rcClient);
	memBit.Attach(hMemBitmap);

	CBitmap *pOldBit = MemDC.SelectObject(&memBit);

	MemDC.SetBkMode(TRANSPARENT);
	if(m_pImage)
	{
		Graphics graphics(MemDC);
		if(m_enBackMode == enBMFrame)	// 边框模式
		{
			DrawImageFrame(graphics, m_pImage, rcClient, 0, 0, m_sizeBKImage.cx, m_sizeBKImage.cy, m_nFrameSize);
		}else
		if(m_enBackMode == enBMMID)	// 九宫格边框模式
		{
			DrawImageFrameMID(graphics, m_pImage, rcClient, 0, 0, m_sizeBKImage.cx, m_sizeBKImage.cy,
				m_nFrameWLT, m_nFrameHLT, m_nFrameWRB, m_nFrameHRB);
		}else	// 图片模式
		{
			graphics.DrawImage(m_pImage, Rect(0, 0, m_sizeBKImage.cx, m_sizeBKImage.cy), 0, 0, m_sizeBKImage.cx, m_sizeBKImage.cy, UnitPixel);
		}
	}
	else
	{
		MemDC.FillSolidRect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), RGB(0, 147, 209));
		MemDC.FillSolidRect(rcClient.left + 1, rcClient.top + 1, rcClient.Width() - 2, rcClient.Height() - 2, RGB(255, 255, 255));
	}

	DrawWindow(MemDC, rcClient);
	DrawWindowEx(MemDC, rcClient);

	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(MemDC, rcClient);
		}
	}

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(MemDC, rcClient);			
		}
	}

	ClientToScreen(&rcClient);
	POINT pointDes;
	pointDes.x = rcClient.left;
	pointDes.y = rcClient.top;
	POINT pointSrc;
	pointSrc.x = 0;
	pointSrc.y = 0;
	SIZE sizeDes;
	sizeDes.cx = rcClient.Width();
	sizeDes.cy = rcClient.Height();

	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.AlphaFormat = AC_SRC_ALPHA ;
	blend.SourceConstantAlpha = m_nBackTranslucent;

	HWND hWnd = GetSafeHwnd();
	SetWindowLong(hWnd,GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	// 如果允许背景Alpha通道,则根据是否有背景图片决定是否使用Aplha通道
	DWORD dwFlags = 0;
	if(m_bBackAlpha)
	{
		dwFlags = (m_pImage ? ULW_ALPHA : ULW_COLORKEY);
	}
	UpdateLayeredWindow(pDC, &pointDes, &sizeDes, &MemDC, &pointSrc, 0, &blend, dwFlags);

	MemDC.SelectObject(pOldBit);
	memBit.DeleteObject();
	MemDC.DeleteDC();
}

LRESULT CDlgPopup::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = FALSE;
	if (!m_bIsLButtonDown)
	{
		if (m_pControl)
		{
			m_pControl->OnMouseMove(0, CPoint(-1, -1));
			ResetControl();
		}

		m_pControl = NULL;
	}

	return 0;
}

LRESULT CDlgPopup::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	//if (m_pControl)
	//{
	//	CPoint point;
	//	GetCursorPos(&point);
	//	//ScreenToClient(&point);
	//	m_pControl->OnMouseMove(0, point);
	//}
	return 0;
}

// 鼠标滚轮消息处理
BOOL CDlgPopup::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ScreenToClient(&pt);
 	zDelta /= WHEEL_DELTA;
	UINT nFlag = (zDelta>0) ? SB_LINEUP : SB_LINEDOWN;
	zDelta = abs(zDelta);
	BOOL bResponse = FALSE;
	for(short i = 0; i < zDelta; i++)
	{
		if (m_pControl && m_pControl->OnScroll(TRUE, nFlag, pt))
		{
			bResponse = TRUE;
			continue;
		}else
		{
			// 窗口控件事件处理
			for (size_t i = 0; i < m_vecControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecControl.at(i);
				if (pControlBase && pControlBase->OnScroll(TRUE, nFlag, pt))
				{
					bResponse = TRUE;
					break;
				}	
			}
		}
	}

    return bResponse;
}

// 鼠标移动事件处理
void CDlgPopup::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = ::_TrackMouseEvent(&tme);
	}

	BOOL bInvalidateRect = false;

	if (m_pControl)
	{
		if(((m_pControl->PtInRect(point) && m_pControl->OnCheckMouseResponse(nFlags, point)) || m_bIsLButtonDown) && m_bTracking)
		{			
			m_pControl->OnMouseMove(nFlags, point);
			ResetControl();
			return;
		}
	}

	if (!m_bIsLButtonDown)
	{
		CControlBase * pOldControl = m_pControl;
		BOOL bIsSelect = FALSE;
		BOOL bIsSystemSelect = FALSE;

		if(m_bTracking)
		{
			// 用户控件
			for (size_t i = 0; i < m_vecControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecControl.at(i);
				if (pControlBase)
				{
					pControlBase->OnMouseMove(nFlags, point);
					if (!bIsSystemSelect)
					{
						if(pControlBase->PtInRect(point) && pControlBase->GetRresponse())
						{
							m_pControl = pControlBase;
							bIsSelect = TRUE;
						}
					}						
				}		
			}
		}

		if (!bIsSelect && !bIsSystemSelect)
		{
			m_pControl = NULL;
		}

		// 调用自身的OnMouseMove
		if(OnMouseMove(point))
		{
			DrawWindow();
		}

		ResetControl();
	}
}

// 鼠标左键按下事件处理
void CDlgPopup::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL bIsSelect = false;
	m_bIsLButtonDblClk = FALSE;

	if(m_pFocusControl != m_pControl && m_pFocusControl != NULL)
	{
		m_pFocusControl->OnFocus(false);
		m_pFocusControl = NULL;
	}
	if (m_pControl)
	{
		if(m_pControl->GetVisible())
		{
			if (m_pControl->PtInRect(point) && m_pControl->OnCheckMouseResponse(nFlags, point))
			{
				bIsSelect = TRUE;
				m_bIsLButtonDown = TRUE;

				m_pFocusControl = m_pControl;
				m_pControl->OnLButtonDown(nFlags, point);	
			}
		}
		else
		{
			m_pControl = NULL;
		}
	}	

	// 当前控件是否可以捕获鼠标
	if (bIsSelect && !m_bIsSetCapture && ((m_pControl == NULL) || m_pControl->OnCheckMouseResponse(nFlags, point)))
	{
		SetCapture();
		m_bIsSetCapture = TRUE;
		ResetControl();

		return;
	}

	// 调用自身的函数
	if(OnLButtonDown(point))
	{
		DrawWindow();	
		return;
	}

	ResetControl();

	//PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));

	//CWnd::OnLButtonDown(nFlags, point);
}

// 鼠标左键放开事件处理
void CDlgPopup::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bIsSetCapture)
	{
		ReleaseCapture();
		m_bIsSetCapture = false;
	}

	m_bIsLButtonDown = FALSE;

	if (m_pControl)
	{
		if(m_pControl->GetVisible())
		{
			CRect rc = m_pControl->GetRect();
			
			m_pControl->OnLButtonUp(nFlags, point);

			if (!rc.PtInRect(point))
			{
				m_pControl = NULL;
			}	
		}
		else
		{
			m_pControl = NULL;
		}
	}

	m_bIsLButtonDblClk = FALSE;

	// 调用自身的函数
	if(OnLButtonUp(point))
	{
		DrawWindow();
	}

	ResetControl();

	CWnd::OnLButtonUp(nFlags, point);
}

// 鼠标左键双击事件处理
void CDlgPopup::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_bIsLButtonDblClk = TRUE;

	if(m_pControl)
	{
		if(m_pControl->GetVisible() && m_pControl->GetRresponse())
		{
			CRect rc = m_pControl->GetRect();
			m_pControl->OnLButtonDblClk(nFlags, point);				

			if (!rc.PtInRect(point))
			{
				m_pControl = NULL;
			}
		}
		else
		{
			m_pControl = NULL;
		}
	}

	// 调用自身的函数
	if(OnLButtonDblClk(point))
	{
		DrawWindow();
	}

	ResetControl();

	CWnd::OnLButtonDblClk(nFlags, point);
}

// 鼠标右键按下事件处理
void CDlgPopup::OnRButtonDown(UINT nFlags, CPoint point)
{
	BOOL bIsSelect = false;
	m_bIsRButtonDblClk = FALSE;

	if(m_pFocusControl != m_pControl && m_pFocusControl != NULL)
	{
		m_pFocusControl->OnFocus(false);
		m_pFocusControl = NULL;
	}
	if (m_pControl)
	{
		if(m_pControl->GetVisible())
		{
			if (m_pControl->PtInRect(point) && m_pControl->OnCheckMouseResponse(nFlags, point))
			{
				bIsSelect = TRUE;
				m_bIsRButtonDown = TRUE;

				m_pFocusControl = m_pControl;
				m_pControl->OnRButtonDown(nFlags, point);	
			}
		}
		else
		{
			m_pControl = NULL;
		}
	}

	// 调用自身的函数
	if(OnRButtonDown(point))
	{
		DrawWindow();	
		return;
	}

	ResetControl();
	//CWnd::OnRButtonDown(nFlags, point);
}

// 鼠标右键放开事件处理
void CDlgPopup::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bIsRButtonDown = FALSE;

	if (m_pControl)
	{
		if(m_pControl->GetVisible())
		{
			CRect rc = m_pControl->GetRect();
			
			m_pControl->OnRButtonUp(nFlags, point);

			if (!rc.PtInRect(point))
			{
				m_pControl = NULL;
			}	
		}
		else
		{
			m_pControl = NULL;
		}
	}

	m_bIsRButtonDblClk = FALSE;

	// 调用自身的函数
	if(OnRButtonUp(point))
	{
		DrawWindow();
	}

	ResetControl();

	CWnd::OnRButtonUp(nFlags, point);
}

// 鼠标右键双击事件处理
void CDlgPopup::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	m_bIsRButtonDblClk = TRUE;

	if(m_pControl)
	{
		if(m_pControl->GetVisible() && m_pControl->GetRresponse())
		{
			CRect rc = m_pControl->GetRect();
			m_pControl->OnRButtonDblClk(nFlags, point);				

			if (!rc.PtInRect(point))
			{
				m_pControl = NULL;
			}
		}
		else
		{
			m_pControl = NULL;
		}
	}

	// 调用自身的函数
	if(OnRButtonDblClk(point))
	{
		DrawWindow();
	}

	ResetControl();

	CWnd::OnRButtonDblClk(nFlags, point);
}

// 键盘按下事件处理
void CDlgPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 当前控件是否能处理
	if (m_pControl && m_pControl->OnKeyDown(nChar, nRepCnt, nFlags))
	{
		return;
	}
	
	// 子控件是否能处理
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->OnKeyDown(nChar, nRepCnt, nFlags))
		{
			return;
		}	
	}

	if(OnControlKeyDown(nChar, nRepCnt, nFlags))
	{
		return;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// 键盘放开事件处理
void CDlgPopup::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 当前控件是否能处理
	if (m_pControl && m_pControl->OnKeyUp(nChar, nRepCnt, nFlags))
	{
		return;
	}
	
	// 子控件是否能处理
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->OnKeyUp(nChar, nRepCnt, nFlags))
		{
			return;
		}	
	}

	if(OnControlKeyUp(nChar, nRepCnt, nFlags))
	{
		return;
	}

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CDlgPopup::PostNcDestroy()
{
	CWnd::PostNcDestroy();
	delete this;
}

// 窗口去激活时候的关闭窗口操作
BOOL CDlgPopup::OnNcCloseWindow()
{
	CloseWindow();
	return TRUE;
}

BOOL CDlgPopup::OnNcActivate(BOOL bActive)
{
	if(!bActive && m_bAutoClose)
	{
		//CloseWindow();
		// 窗口去激活时候改为调用虚函数,正常情况下是关闭窗口,菜单等派生类可以重载进行一些修改
		OnNcCloseWindow();
	}
	else
	{
		m_bNCActive = bActive;

		if(m_bNCActive)
		{

		}
		else
		{
			m_bTracking = false;
			m_bIsSetCapture = false;
			m_bIsLButtonDblClk = FALSE;

			if(m_bIsLButtonDown)
			{
				m_bIsLButtonDown = FALSE;
				if(m_pControl)
				{
					CRect rcControlUpdate;
					m_pControl->OnLButtonUp(0, CPoint(-1, -1));
					m_pControl = NULL;				
				}
			}	
			else
			{
				if(m_pControl)
				{
					CRect rcControlUpdate;
					m_pControl->OnMouseMove(0, CPoint(-1, -1));
					m_pControl = NULL;				
				}
			}		
		}
		//DrawWindow();
		InvalidateRect(NULL);

		return TRUE;	
	}
	return TRUE;	
}

// 定时器消息
void CDlgPopup::OnTimer(UINT uTimerID)
{
	//if(!::IsWindow(GetSafeHwnd())) return ;

	// 动画定时器
	if(m_uTimerAnimation == uTimerID)
	{
		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				pControlBase->OnTimer();
			}		
		}
	}
}

// 消息响应
LRESULT CDlgPopup::OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CControlBase* pControl = GetControl(uID);
	if(pControl == NULL)
	{
		return 0L;
	}

	// 点击了关闭按钮，则执行关闭动作
	if(pControl->IsThisObject(BT_CLOSE, NAME_BT_CLOSE))
	{
		CloseWindow();
	}else
	{
		OnMessage(uID, uMsg, wParam, lParam);
	}
	return 0L; 
}

LRESULT CDlgPopup::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 遍历每个控件，看哪个控件是此ID，则进行事件的处理
	CControlBase * pThisControlBase = NULL;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && (pControlBase->GetControlID() == uID))
		{
			pThisControlBase = pControlBase;
			pControlBase->OnMessage(uID, Msg, wParam, lParam);
		}
	}

	return 0;
}

LRESULT CDlgPopup::OnControlUpdate(CRect rcUpdate, BOOL bUpdate, CControlBase *pUpdateControlBase) 
{
	if(pUpdateControlBase == NULL) return 0;

	if(bUpdate)
	{
		pUpdateControlBase->SetUpdate(FALSE);
	}

	CRect rcAllUpDate = rcUpdate;

	if(m_bInit)
	{
		BOOL bFind = false;

		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE);
					}
				}
				else if(pControlBase == pUpdateControlBase)
				{
					bFind = true;
				}		
			}
		}
	}

	if(GetSafeHwnd())
	{
		InvalidateRect(&rcAllUpDate);
	}

	return 0L; 
};

// 发送通知消息
BOOL CDlgPopup::SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND hParent = ::GetParent(m_hWnd);
	if (hParent)
	{
		::SendMessage(hParent, Msg, wParam, lParam);
		return true;
	}
	return false;
}

// 发送通知消息
BOOL CDlgPopup::PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND hParent = ::GetParent(m_hWnd);
	if (hParent)
	{
		::PostMessage(hParent, Msg, wParam, lParam);
		return true;
	}
	return false;
}

// 发送通知消息给父控件
BOOL CDlgPopup::SendMessageToParent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDuiObject* pParentObj = GetParent();
	if(pParentObj == NULL)
	{
		return FALSE;
	}

	if(pParentObj->IsClass(_T("dlg")))
	{
		return ((CDlgBase*)pParentObj)->OnMessage(GetID(), uMsg, wParam, lParam);
	}else
	if(pParentObj->IsClass(_T("popup")))
	{
		return ((CDlgPopup*)pParentObj)->OnMessage(GetID(), uMsg, wParam, lParam);
	}else
	{
		return ((CControlBase*)pParentObj)->OnMessage(GetID(), uMsg, wParam, lParam);
	}

	return FALSE;
}

// 获取子控件对象
CControlBase *CDlgPopup::GetControl(UINT uControlID)
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetControlID() == uControlID)
			{
				return pControlBase;
			}else
			{
				// 查找子控件
				CControlBase * pSubControl = pControlBase->GetControl(uControlID);
				if(pSubControl != NULL)
				{
					return pSubControl;
				}
			}
		}
	}

	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetControlID() == uControlID)
			{
				return pControlBase;
			}else
			{
				// 查找子控件
				CControlBase * pSubControl = pControlBase->GetControl(uControlID);
				if(pSubControl != NULL)
				{
					return pSubControl;
				}
			}
		}
	}

	return NULL;
}

// 获取子控件对象
CControlBase *CDlgPopup::GetControl(CString strControlName)
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetName() == strControlName)
			{
				return pControlBase;
			}else
			{
				// 查找子控件
				CControlBase * pSubControl = pControlBase->GetControl(strControlName);
				if(pSubControl != NULL)
				{
					return pSubControl;
				}
			}
		}
	}

	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetName() == strControlName)
			{
				return pControlBase;
			}else
			{
				// 查找子控件
				CControlBase * pSubControl = pControlBase->GetControl(strControlName);
				if(pSubControl != NULL)
				{
					return pSubControl;
				}
			}
		}
	}

	return NULL;
}

// 设置当前焦点控件
void CDlgPopup::SetFocusControl(CControlBase* pFocusControl)
{
	if(pFocusControl != m_pFocusControl)
	{
		if(m_pFocusControl != NULL)
		{
			m_pFocusControl->OnFocus(FALSE);
		}
		if(pFocusControl != NULL)
		{
			pFocusControl->OnFocus(TRUE);
		}
		m_pFocusControl = pFocusControl;
	}
}

// 获取当前焦点控件
CControlBase* CDlgPopup::GetFocusControl()
{
	/*for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && (pControlBase == m_pFocusControl) && pControlBase->IsTabStop())
		{
			return pControlBase;
		}else
		if (pControlBase && (pControlBase == m_pControl))
		{
			// 查找子控件
			pControlBase = pControlBase->GetFocusControl(m_pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;*/
	return m_pFocusControl;
}

// 获取上一个可以获取焦点的子控件
CControlBase* CDlgPopup::GetPrevFocusableControl()
{
	BOOL bStartSearch = FALSE;
	// 先按照焦点控件查找一次
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && bStartSearch && pControlBase->IsTabStop())
		{
			return pControlBase;			
		}else
		if (pControlBase && (pControlBase == m_pFocusControl))
		{
			bStartSearch = TRUE;
		}
	}

	// 再遍历子控件查找
	if(m_pFocusControl == NULL)
	{
		bStartSearch = TRUE;
	}
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if(m_pControl == NULL)
		{
			m_pControl = pControlBase;
		}
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && bStartSearch && pControlBase->IsTabStop())
		{
			return pControlBase;			
		}else
		if (pControlBase && (pControlBase == m_pControl))
		{
			// 查找子控件
			pControlBase = pControlBase->GetPrevFocusableControl(m_pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
			//bStartSearch = TRUE;
		}
	}

	return NULL;
}

// 获取下一个可以获取焦点的子控件
CControlBase* CDlgPopup::GetNextFocusableControl()
{
	BOOL bStartSearch = FALSE;
	// 先按照焦点控件查找一次
	for (int i = 0; i < (int)m_vecControl.size(); i++)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && bStartSearch && pControlBase->IsTabStop())
		{
			return pControlBase;			
		}else
		if (pControlBase && (pControlBase == m_pFocusControl))
		{
			bStartSearch = TRUE;
		}
	}

	// 再遍历子控件查找
	if(m_pFocusControl == NULL)
	{
		bStartSearch = TRUE;
	}
	for (int i = 0; i < (int)m_vecControl.size(); i++)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if(m_pControl == NULL)
		{
			m_pControl = pControlBase;
		}
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && bStartSearch && pControlBase->IsTabStop())
		{
			return pControlBase;			
		}else
		if (pControlBase && (pControlBase == m_pControl))
		{
			// 查找子控件
			pControlBase = pControlBase->GetNextFocusableControl(m_pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
			//bStartSearch = TRUE;
		}
	}

	return NULL;
}

// 移动控件
CControlBase * CDlgPopup::SetControlRect(UINT uControlID, CRect rc)
{
	CControlBase *pControlBase = GetControl(uControlID);
	if(pControlBase)
	{
		pControlBase->SetRect(rc);
		UpdateHover();
	}
	return pControlBase;
}

// 移动控件
CControlBase * CDlgPopup::SetControlRect(CControlBase *pControlBase, CRect rc)
{
	if(pControlBase)
	{
		pControlBase->SetRect(rc);
		UpdateHover();
	}
	return pControlBase;
}

// 显示控件
CControlBase * CDlgPopup::SetControlVisible(UINT uControlID, BOOL bVisible)
{
	CControlBase *pControlBase = GetControl(uControlID);
	if(pControlBase)
	{
		pControlBase->SetVisible(bVisible);
		UpdateHover();
	}
	return pControlBase;
}

// 显示控件
CControlBase * CDlgPopup::SetControlVisible(CControlBase *pControlBase, BOOL bVisible)
{
	if(pControlBase)
	{
		pControlBase->SetVisible(bVisible);
		UpdateHover();
	}
	return pControlBase;
}

// 禁用控件
CControlBase * CDlgPopup::SetControlDisable(UINT uControlID, BOOL bDisable)
{
	CControlBase *pControlBase = GetControl(uControlID);
	if(pControlBase)
	{
		pControlBase->SetDisable(bDisable);
		UpdateHover();
	}
	return pControlBase;
}

// 禁用控件
CControlBase * CDlgPopup::SetControlDisable(CControlBase *pControlBase, BOOL bDisable)
{
	if(pControlBase)
	{
		pControlBase->SetDisable(bDisable);
		UpdateHover();
	}
	return pControlBase;
}

// 重置控件
void CDlgPopup::ResetControl()
{
	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE);//, m_clrBK);
		}
	}

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE);//, m_clrBK);			
		}
	}

	InvalidateRect(NULL);
}

// 更新选中
void CDlgPopup::UpdateHover()
{
	CPoint point;
	GetCursorPos(&point);
	OnMouseMove(0, point);
}

void CDlgPopup::OnClose()
{
	SendMessage(m_uMessageID, MSG_CLOSE, 0);

	CWnd::OnClose();
}

void CDlgPopup::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DrawWindow(&dc);
}

void CDlgPopup::OnDestroy()
{
	__super::OnDestroy();

	// 结束定时器
	CTimer::KillTimer();
}
