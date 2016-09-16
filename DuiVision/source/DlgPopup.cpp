// DlgPopup.cpp : ʵ���ļ�
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
	// ������ʱ��
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
	if(m_bTopMost)	// ����������ǰ��
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
	if(m_bTopMost)	// ����������ǰ��
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
	// ������ڴ�СΪ0��ʹ�ô���Ĳ�����Ϊ���ڴ�С
	if((m_size.cx == 0) || (m_size.cy == 0))
	{
		m_size.SetSize(rc.Width(), rc.Height());
	}else
	{
		// ����m_size��С���ô��ڵĴ�С
		rc.right = rc.left + m_size.cx;
		rc.bottom = rc.top + m_size.cy;
	}

	DWORD dwStyle = WS_EX_TOOLWINDOW;
	if(m_bTopMost)	// ����������ǰ��
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
	// ���ô��ڷ��
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

	//������ʱ��
	m_uTimerAnimation = CTimer::SetTimer(30);

	m_bInit = true;

	return TRUE;
}

// ���ݿؼ��������ؼ�ʵ��
CControlBase* CDlgPopup::_CreateControlByName(LPCTSTR lpszName)
{
	return DuiSystem::CreateControlByName(lpszName, GetSafeHwnd(), this);
}

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDlgPopup::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

	if(!bLoadSubControl)
	{
		// �������ӿؼ�
		return TRUE;
	}

	// ���´��ڴ�С
	SetRect(CRect(0, 0, m_size.cx, m_size.cy));

    // ʹ��XML�ڵ��ʼ���ؼ�
	if(pXmlElem != NULL)
	{
		InitUI(m_rc, pXmlElem);
	}

	// �����¼��������ĳ�ʼ������
	if(m_pDuiHandler)
	{
		m_pDuiHandler->OnInit();
	}

    return TRUE;
}

// ����XML�ļ�
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
			// ����popup�ڵ�����
			Load(pDivElem);
		}
	}

	return TRUE;
}

// ����XML����
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
			// ����popup�ڵ�����
			Load(pDivElem);
		}
	}

	return TRUE;
}

// ��ʼ�����ڿؼ�
void CDlgPopup::InitUI(CRect rcClient, DuiXmlNode pNode)
{
	CRect rcTemp;
	int nStartX = 0;
	int nStartY = 0;
	CControlBase * pControlBase = NULL;

	// �������д��ڿؼ�
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
						// ���Load�ɹ�,����ӿؼ�
						if(pControl->IsClass(CArea::GetClassName()))
						{
							// Area������Ӧ���,����ӵ�Area�б���
							m_vecArea.push_back(pControl);
						}else
						{
							m_vecControl.push_back(pControl);
						}
					}else
					{
						// ����ֱ��ɾ���ؼ�����ָ��
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

// ��XML���ñ���ģʽ����
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

// ��XML����ͼƬ��Ϣ����
HRESULT CDlgPopup::OnAttributeBkImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		SetBackBitmap(strImgFile);
	}else	// ����ͼƬ��Դ
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
	// ��Ϊֱ��ʹ��CreateCompatibleBitmap��16λ��ɫ�»�������,��˸�Ϊ����DuiCreateCompatibleBitmap����
	HBITMAP	hMemBitmap = DuiCreateCompatibleBitmap(rcClient);
	memBit.Attach(hMemBitmap);

	CBitmap *pOldBit = MemDC.SelectObject(&memBit);

	MemDC.SetBkMode(TRANSPARENT);
	if(m_pImage)
	{
		Graphics graphics(MemDC);
		if(m_enBackMode == enBMFrame)	// �߿�ģʽ
		{
			DrawImageFrame(graphics, m_pImage, rcClient, 0, 0, m_sizeBKImage.cx, m_sizeBKImage.cy, m_nFrameSize);
		}else
		if(m_enBackMode == enBMMID)	// �Ź���߿�ģʽ
		{
			DrawImageFrameMID(graphics, m_pImage, rcClient, 0, 0, m_sizeBKImage.cx, m_sizeBKImage.cy,
				m_nFrameWLT, m_nFrameHLT, m_nFrameWRB, m_nFrameHRB);
		}else	// ͼƬģʽ
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
	// ���������Alphaͨ��,������Ƿ��б���ͼƬ�����Ƿ�ʹ��Aplhaͨ��
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

// ��������Ϣ����
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
			// ���ڿؼ��¼�����
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

// ����ƶ��¼�����
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
			// �û��ؼ�
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

		// ���������OnMouseMove
		if(OnMouseMove(point))
		{
			DrawWindow();
		}

		ResetControl();
	}
}

// �����������¼�����
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

	// ��ǰ�ؼ��Ƿ���Բ������
	if (bIsSelect && !m_bIsSetCapture && ((m_pControl == NULL) || m_pControl->OnCheckMouseResponse(nFlags, point)))
	{
		SetCapture();
		m_bIsSetCapture = TRUE;
		ResetControl();

		return;
	}

	// ��������ĺ���
	if(OnLButtonDown(point))
	{
		DrawWindow();	
		return;
	}

	ResetControl();

	//PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));

	//CWnd::OnLButtonDown(nFlags, point);
}

// �������ſ��¼�����
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

	// ��������ĺ���
	if(OnLButtonUp(point))
	{
		DrawWindow();
	}

	ResetControl();

	CWnd::OnLButtonUp(nFlags, point);
}

// ������˫���¼�����
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

	// ��������ĺ���
	if(OnLButtonDblClk(point))
	{
		DrawWindow();
	}

	ResetControl();

	CWnd::OnLButtonDblClk(nFlags, point);
}

// ����Ҽ������¼�����
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

	// ��������ĺ���
	if(OnRButtonDown(point))
	{
		DrawWindow();	
		return;
	}

	ResetControl();
	//CWnd::OnRButtonDown(nFlags, point);
}

// ����Ҽ��ſ��¼�����
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

	// ��������ĺ���
	if(OnRButtonUp(point))
	{
		DrawWindow();
	}

	ResetControl();

	CWnd::OnRButtonUp(nFlags, point);
}

// ����Ҽ�˫���¼�����
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

	// ��������ĺ���
	if(OnRButtonDblClk(point))
	{
		DrawWindow();
	}

	ResetControl();

	CWnd::OnRButtonDblClk(nFlags, point);
}

// �����¼�����
void CDlgPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// ��ǰ�ؼ��Ƿ��ܴ���
	if (m_pControl && m_pControl->OnKeyDown(nChar, nRepCnt, nFlags))
	{
		return;
	}
	
	// �ӿؼ��Ƿ��ܴ���
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

void CDlgPopup::PostNcDestroy()
{
	CWnd::PostNcDestroy();
	delete this;
}

// ����ȥ����ʱ��Ĺرմ��ڲ���
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
		// ����ȥ����ʱ���Ϊ�����麯��,����������ǹرմ���,�˵���������������ؽ���һЩ�޸�
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

// ��ʱ����Ϣ
void CDlgPopup::OnTimer(UINT uTimerID)
{
	//if(!::IsWindow(GetSafeHwnd())) return ;

	// ������ʱ��
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

// ��Ϣ��Ӧ
LRESULT CDlgPopup::OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CControlBase* pControl = GetControl(uID);
	if(pControl == NULL)
	{
		return 0L;
	}

	// ����˹رհ�ť����ִ�йرն���
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
	// ����ÿ���ؼ������ĸ��ؼ��Ǵ�ID��������¼��Ĵ���
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

// ����֪ͨ��Ϣ
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

// ����֪ͨ��Ϣ
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

// ����֪ͨ��Ϣ�����ؼ�
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

// ��ȡ�ӿؼ�����
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
				// �����ӿؼ�
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
				// �����ӿؼ�
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

// ��ȡ�ӿؼ�����
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
				// �����ӿؼ�
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
				// �����ӿؼ�
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

// ���õ�ǰ����ؼ�
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

// ��ȡ��ǰ����ؼ�
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
			// �����ӿؼ�
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

// ��ȡ��һ�����Ի�ȡ������ӿؼ�
CControlBase* CDlgPopup::GetPrevFocusableControl()
{
	BOOL bStartSearch = FALSE;
	// �Ȱ��ս���ؼ�����һ��
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

	// �ٱ����ӿؼ�����
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
			// �����ӿؼ�
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

// ��ȡ��һ�����Ի�ȡ������ӿؼ�
CControlBase* CDlgPopup::GetNextFocusableControl()
{
	BOOL bStartSearch = FALSE;
	// �Ȱ��ս���ؼ�����һ��
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

	// �ٱ����ӿؼ�����
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
			// �����ӿؼ�
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

// �ƶ��ؼ�
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

// �ƶ��ؼ�
CControlBase * CDlgPopup::SetControlRect(CControlBase *pControlBase, CRect rc)
{
	if(pControlBase)
	{
		pControlBase->SetRect(rc);
		UpdateHover();
	}
	return pControlBase;
}

// ��ʾ�ؼ�
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

// ��ʾ�ؼ�
CControlBase * CDlgPopup::SetControlVisible(CControlBase *pControlBase, BOOL bVisible)
{
	if(pControlBase)
	{
		pControlBase->SetVisible(bVisible);
		UpdateHover();
	}
	return pControlBase;
}

// ���ÿؼ�
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

// ���ÿؼ�
CControlBase * CDlgPopup::SetControlDisable(CControlBase *pControlBase, BOOL bDisable)
{
	if(pControlBase)
	{
		pControlBase->SetDisable(bDisable);
		UpdateHover();
	}
	return pControlBase;
}

// ���ÿؼ�
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

// ����ѡ��
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

	// ������ʱ��
	CTimer::KillTimer();
}
