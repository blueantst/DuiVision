// DlgBase.cpp
#include "stdafx.h"
#include <winuser.h>
#include <Windows.h>
#include <Windowsx.h>
#include <shlwapi.h>
#include <cmath>
#include "DlgBase.h"

IMPLEMENT_DYNAMIC(CDlgBase, CDialog)

CDlgBase::CDlgBase(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
: CDialog(nIDTemplate, pParent)
{
	m_nMainThreadId = ::GetCurrentThreadId();
	m_nIDTemplate = nIDTemplate;
	m_pParentDuiObject = NULL;
	m_strXmlFile = _T("");
	m_strXmlContent = _T("");
	m_strTitle = _T("");
	m_hIcon = NULL;
	m_bAppWin = FALSE;
	m_MinSize.cx = 0;
	m_MinSize.cy = 0;
	m_bChangeSize = FALSE;
	m_bInit = false;

	m_nFrameTopBottomSpace = 3;
	m_nFrameLeftRightSpace = 3;

	m_nOverRegioX = 100;
	m_nOverRegioY = 100;
	m_sizeBKImage.cx = 100;
	m_sizeBKImage.cy = 100;

	m_bTracking = false;
	m_bIsSetCapture = false;
	m_clrBK = RGB(186, 226, 239);
	m_crlBackTransParent = RGB(255, 255, 255);
	m_bDrawImage = FALSE;
	m_bImageUseECM = false;

	m_bIsLButtonDown = FALSE;
	m_bIsLButtonDblClk = FALSE;
	m_bIsRButtonDown = FALSE;
	m_bIsRButtonDblClk = FALSE;
	m_pOldMemBK = NULL;
	m_pControl = NULL;
	m_pFocusControl = NULL;
	m_pWndPopup = NULL;

	m_strFramePicture = _T("");
	m_nFrameSize = 3;
	m_nFrameWLT = 0;
	m_nFrameHLT = 0;
	m_nFrameWRB = 0;
	m_nFrameHRB = 0;

	m_pImageShadow = NULL;
	m_nShadowWLT = 0;
	m_nShadowHLT = 0;
	m_nShadowWRB = 0;
	m_nShadowHRB = 0;
	m_nShadowSize = 0;

	m_uTimerAnimation = 0;
	m_uTimerAutoClose = 0;

	m_bAutoClose = FALSE;
	m_bAutoHide = FALSE;
	m_uAutoCloseDelayTime = 0;

	m_strBkImg = _T("");
	m_crlBack = RGB(0,0,0);
	m_nBackTranslucent = 255;	// ����͸����,255��ʾ��͸��,1��ʾȫ͸��
	m_bTopMost = false;

	m_nTooltipCtrlID = 0;

	m_pTrayHandler = NULL;
	m_strTrayMenuXml = _T("");

	// ��ʼ��ͼ��
	CString strTrayIcon = DuiSystem::Instance()->GetSkin(_T("IDB_TRAY_ICON"));
	if(!strTrayIcon.IsEmpty())
	{
		if(strTrayIcon.Find(_T(".")) != -1)	// ����ͼ���ļ�
		{
			DuiSystem::Instance()->LoadIconFile(strTrayIcon, m_hIcon);
		}else	// ����ͼ����Դ
		{
			UINT nResourceID = _ttoi(strTrayIcon);
			LoadIconFromIDResource(nResourceID, m_hIcon);
		}
	}
}

CDlgBase::~CDlgBase()
{
	// ������ʱ��
	CTimer::KillTimer();

	if (m_hIcon)
	{
		DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}

	if(m_BKImage.m_hObject != NULL)
	{
		m_BKImage.DeleteObject();
		m_MemBKDC.SelectObject(m_pOldMemBK);
		m_MemBK.DeleteObject();
	}

	if(m_TitleFont.m_hObject != NULL)
	{
		m_TitleFont.DeleteObject();
	}
	m_Rgn.DeleteObject();

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			delete pControlBase;
		}		
	}

	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
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

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			delete pControlBase;
		}		
	}

	if(m_pImageShadow != NULL)
	{
		delete m_pImageShadow;
		m_pImageShadow = NULL;
	}
}

// ͼƬ���Ե�ʵ��
DUI_IMAGE_ATTRIBUTE_IMPLEMENT(CDlgBase, Shadow, 1)

BEGIN_MESSAGE_MAP(CDlgBase, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_NCDESTROY()
	ON_WM_CLOSE()
	ON_WM_NCACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_KEYDOWN()
	ON_WM_SYSCOMMAND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER_CLOSEWND, OnUserCloseWindow)
	ON_MESSAGE(WM_SKIN, OnMessageSkin)
	ON_MESSAGE(WM_UI_TASK, OnMessageUITask)
	ON_MESSAGE(WM_SYSTEM_TRAYICON, OnSystemTrayIcon)
	ON_REGISTERED_MESSAGE(WM_CHECK_ITS_ME, OnCheckItsMe)
END_MESSAGE_MAP()

// ������С���ڴ�С
void CDlgBase::SetMinSize(int iWidth, int iHeight)
{
	m_MinSize.cx = iWidth;
	m_MinSize.cy = iHeight;
}

CSize CDlgBase::GetMinSize()
{
	return	m_MinSize;
}

void CDlgBase::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialog::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = m_MinSize.cx;
	lpMMI->ptMinTrackSize.y = m_MinSize.cy;

	CRect	rc(0, 0, 1024, 768);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
	lpMMI->ptMaxPosition.x = rc.left;
	lpMMI->ptMaxPosition.y = rc.top;
	lpMMI->ptMaxSize.x = rc.Width();
	lpMMI->ptMaxSize.y = rc.Height();
}

// ���ݿؼ��������ؼ�ʵ��
CControlBase* CDlgBase::_CreateControlByName(LPCTSTR lpszName)
{
	return DuiSystem::CreateControlByName(lpszName, GetSafeHwnd(), this);
}

// �жϵ�ǰ�Ƿ������߳�
void CDlgBase::TestMainThread()
{
    // ���㿴�����������ʱ���Ҳ��ҵĸ����㣬��������߳���ˢ����
    // ����һ����Σ�յ�����
    DWORD dwCurThreadID = GetCurrentThreadId();
    BOOL bOK = (m_nMainThreadId == dwCurThreadID); // ��ǰ�̺߳͹������ʱ���߳�һ��
	if(!bOK)
	{
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("TestMainThread failed"));
	}
    ASSERT(bOK);
}

// CDlgBase message handlers

BOOL CDlgBase::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// ���ش��ڵ�XML�ļ�
	DuiXmlDocument xmlDoc;
	DuiXmlNode pDlgElem;

	BOOL bLoadXml = DuiSystem::Instance()->LoadXmlFile(xmlDoc, m_strXmlFile);
	if(!bLoadXml)
	{
		if(!m_strXmlContent.IsEmpty())	// ����XML����
		{
			bLoadXml = xmlDoc.load(m_strXmlContent);
		}
	}
	
	if(bLoadXml)
	{
		pDlgElem = xmlDoc.child(_T("dlg"));//RootElement();
		if(pDlgElem)
		{
			// ����dlg�ڵ�����
			Load(pDlgElem);
			// ���´��ڴ�С
			SetMinSize(m_MinSize.cx, m_MinSize.cy);
			SetRect(CRect(0, 0, m_MinSize.cx, m_MinSize.cy));
		}
	}

	// ��ʼ���Ի��������Ԥ��ֵ����
	InitDialogValue();

	SetWindowText(m_strTitle);

	CFont	tmpFont;
	tmpFont.Attach(GetStockObject(DEFAULT_GUI_FONT));

	LOGFONT font;
	memset(&font, 0, sizeof(font));
	tmpFont.GetLogFont(&font);

	CWindowDC dc(this);
	_tcscpy(font.lfFaceName,DuiSystem::GetDefaultFont());
	font.lfHeight = -10 * GetDeviceCaps(dc.m_hDC, LOGPIXELSY) / 72;
	font.lfWeight = 600;

	m_TitleFont.CreateFontIndirect(&font);

	if(m_bTopMost)
	{
		// ������ʾ���������ǰ��
		::SetWindowPos(m_hWnd, HWND_TOPMOST, 0,0,m_MinSize.cx, m_MinSize.cy, SWP_HIDEWINDOW | SWP_NOMOVE);
	}else
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, m_MinSize.cx, m_MinSize.cy, SWP_HIDEWINDOW | SWP_NOMOVE);
	}
	
	CRect rc;
	GetClientRect(rc);

	// ʹ��XML�ڵ��ʼ�����ڻ����ؼ�����ͨ�ؼ�
	if(pDlgElem)
	{
		DuiXmlNode pBaseElem = pDlgElem.child(_T("base"));
		if(pBaseElem)
		{
			InitBaseUI(rc, pBaseElem);
		}

		DuiXmlNode pBodyElem = pDlgElem.child(_T("body"));
		if(pBodyElem)
		{
			InitUI(rc, pBodyElem);
		}

		// ��ʼ��Ԥ��ֵ�Ŀؼ�ֵ
		InitControlValue();
	}

	if(!m_bAppWin)
	{
		// ����������(��������������ʾ���˴���),������TOOLWINDOWS����
		//::SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd,GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
		// ���NotifyMsg��������������ʾ������,��Ҫȥ��APPWINDOW����,������TOOLWINDOW����
		ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	}

	// ���ô��ڱ���͸������
	//HWND hWnd = GetSafeHwnd();
	//SetWindowLong(hWnd,GWL_EXSTYLE,GetWindowLong(hWnd,GWL_EXSTYLE) | WS_EX_TRANSPARENT);

	// ���ر���Ƥ��
	InitWindowBkSkin();

	CenterWindow();
	ShowWindow(SW_SHOW);

	// ��ʾ������Ӱ
	if(m_pImageShadow != NULL)	// �Ź���ʽ��ͼƬ��Ӱ
	{
		CWndShadow::Initialize(AfxGetInstanceHandle()); 
		m_Shadow.Create(GetSafeHwnd());
		m_Shadow.SetShadowImage(m_pImageShadow, m_nShadowWLT, m_nShadowHLT, m_nShadowWRB, m_nShadowHRB);
		m_Shadow.SetPosition(0, 0);
	}else
	if(m_nShadowSize > 0)	// �㷨��Ӱ
	{
		CWndShadow::Initialize(AfxGetInstanceHandle()); 
		m_Shadow.Create(GetSafeHwnd());
		m_Shadow.SetSize(m_nShadowSize);
		m_Shadow.SetPosition(0, 0);
	}

	//������ʱ��
	m_uTimerAnimation = CTimer::SetTimer(30);

	// ��ʼ��Tooltip
	m_wndToolTip.Create(this);

	m_bInit = true;

	// �����¼��������ĳ�ʼ������
	if(m_pDuiHandler)
	{
		m_pDuiHandler->OnInit();
	}

	return TRUE;
}

// ���ش��ڻ����ؼ�
void CDlgBase::InitBaseUI(CRect rcClient, DuiXmlNode pNode)
{
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
					pControl->Load(pControlElem);
					if(pControl->IsClass(CArea::GetClassName()))
					{
						// Area������Ӧ���,����ӵ�Area�б���
						m_vecBaseArea.push_back(pControl);
					}else
					{
						m_vecBaseControl.push_back(pControl);
					}
				}
			}
		}
	}
}

// ��ʼ�����ڿؼ�
void CDlgBase::InitUI(CRect rcClient, DuiXmlNode pNode)
{
	CRect rcTemp;
	int nStartX = 0;
	int nStartY = 0;
	CControlBase * pControlBase = NULL;

	// �����ڵ�͸���Ƚ�����ɰ�(���ݴ��ڵ�frame����������)
	if(!m_strFramePicture.IsEmpty())
	{
		pControlBase = new CDuiPicture(GetSafeHwnd(), this, FRAME_MAINWND, rcClient);
		((CDuiPicture*)pControlBase)->OnAttributeImage(m_strFramePicture, FALSE);
		if(m_nFrameWLT != 0)
		{
			// �Ź���ģʽ
			((CDuiPicture*)pControlBase)->SetShowModeMID(m_nFrameWLT, m_nFrameHLT, m_nFrameWRB, m_nFrameHRB);
		}else
		{
			// �߿�ģʽ
			((CDuiPicture*)pControlBase)->SetShowMode(enSMFrame, m_nFrameSize);
		}
		pControlBase->OnAttributePosChange(_T("0,0,-0,-0"), FALSE);
		m_vecBaseArea.push_back(pControlBase);
	}

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
					pControl->Load(pControlElem);
					if(pControl->IsClass(CArea::GetClassName()))
					{
						// Area������Ӧ���,����ӵ�Area�б���
						m_vecArea.push_back(pControl);
					}else
					{
						m_vecControl.push_back(pControl);
						// �Ի����ʼ��ʱ����ʾһ�����пؼ�������ʹedit��Windowsԭ���ؼ������Զ���������
						//BOOL bIsVisible = pControl->GetVisible();
						//pControl->SetControlWndVisible(bIsVisible);
					}
				}
			}
		}
	}
}

// ��ӿؼ�Ԥ������Ϣ
void CDlgBase::SetControlValue(CString strName, CString strType, CString strValue)
{
	CONTROL_VALUE ctrlValue;
	ctrlValue.strName = strName;
	ctrlValue.strType = strType;
	ctrlValue.strValue = strValue;
	m_vecControlValue.push_back(ctrlValue);
}

// ��ʼ��Ԥ���öԻ�������
void CDlgBase::InitDialogValue()
{
	for (size_t i = 0; i < m_vecControlValue.size(); i++)
	{
		CONTROL_VALUE* pCtrlValue = &(m_vecControlValue.at(i));

		// �Ի����������Ը���
		if(pCtrlValue->strName.IsEmpty())
		{
			if(pCtrlValue->strType == _T("width"))
			{
				m_MinSize.cx = _ttoi(pCtrlValue->strValue);
				// ���´��ڴ�С
				SetMinSize(m_MinSize.cx, m_MinSize.cy);
				SetRect(CRect(0, 0, m_MinSize.cx, m_MinSize.cy));
			}else
			if(pCtrlValue->strType == _T("height"))
			{
				m_MinSize.cy = _ttoi(pCtrlValue->strValue);
				// ���´��ڴ�С
				SetMinSize(m_MinSize.cx, m_MinSize.cy);
				SetRect(CRect(0, 0, m_MinSize.cx, m_MinSize.cy));
			}
		}
	}
}

// ��ʼ��Ԥ���ÿؼ�ֵ
void CDlgBase::InitControlValue()
{
	for (size_t i = 0; i < m_vecControlValue.size(); i++)
	{
		CONTROL_VALUE* pCtrlValue = &(m_vecControlValue.at(i));

		// �Ի����������Ը���
		if(pCtrlValue->strName.IsEmpty())
		{
			continue;
		}

		CControlBase* pControl = GetControl(pCtrlValue->strName);
		if(pControl == NULL)
		{
			// �������ͨ�ؼ���δ�ҵ�,����һ�»����ؼ�
			pControl = GetBaseControl(pCtrlValue->strName);
		}
		if(pControl != NULL)
		{
			if(pCtrlValue->strType == _T("visible"))
			{
				pControl->SetVisible(_ttoi(pCtrlValue->strValue));
			}else
			if(pCtrlValue->strType == _T("disable"))
			{
				pControl->SetDisable(_ttoi(pCtrlValue->strValue));
			}else
			if(pCtrlValue->strType == _T("title"))
			{
				((CControlBaseFont*)pControl)->SetTitle(pCtrlValue->strValue);
			}else
			if(pCtrlValue->strType == _T("image"))
			{
				((CControlBaseFont*)pControl)->OnAttributeImage(pCtrlValue->strValue, TRUE);
			}else
			if(pCtrlValue->strType == _T("check"))
			{
				if(pControl->IsClass(CCheckButton::GetClassName()))
				{
					((CCheckButton*)pControl)->SetCheck(pCtrlValue->strValue == _T("true"));
				}else
				if(pControl->IsClass(CDuiRadioButton::GetClassName()))
				{
					((CDuiRadioButton*)pControl)->SetCheck(pCtrlValue->strValue == _T("true"));
				}
			}else
			if(pCtrlValue->strType == _T("value"))
			{
				if(pControl->IsClass(CDuiComboBox::GetClassName()))
				{
					((CDuiComboBox*)pControl)->SetComboValue(pCtrlValue->strValue);
				}
			}
		}
	}
}

// ���ô����Զ��رյĶ�ʱ��
void CDlgBase::SetAutoCloseTimer(UINT uDelayTime, BOOL bHideWnd)
{
	// ��ɾ����ʱ��,�ٴ����µĶ�ʱ��
	if(m_uTimerAutoClose != 0)
	{
		CTimer::KillTimer(m_uTimerAutoClose);
		m_uTimerAutoClose = 0;
	}
	m_uAutoCloseDelayTime = uDelayTime;
	m_bAutoClose = FALSE;
	m_bAutoHide = FALSE;
	if(uDelayTime != 0)
	{
		// ����ӳ�ʱ�����õ��Ƿ�0,�������Զ��ر�
		if(bHideWnd)
		{
			m_bAutoHide = TRUE;	// ���ش���ģʽ
		}else
		{
			m_bAutoClose = TRUE;	// �رմ���ģʽ
		}
		if(!m_bTracking)
		{
			// �����겻�ڴ��ڷ�Χ�ڲŴ�����ʱ��
			m_uTimerAutoClose = CTimer::SetTimer(uDelayTime);
		}
	}
}

void CDlgBase::OnSize(CRect rcClient)
{
}

// ��ȡ�ӿؼ�����
CControlBase *CDlgBase::GetControl(UINT uControlID)
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
CControlBase *CDlgBase::GetControl(CString strControlName)
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

// ��ȡ�ӿؼ�����
CControlBase *CDlgBase::GetBaseControl(UINT uControlID)
{
	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetControlID() == uControlID)
			{
				return pControlBase;
			}
		}
	}

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetControlID() == uControlID)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ��ȡ�ӿؼ�����
CControlBase *CDlgBase::GetBaseControl(CString strControlName)
{
	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetName() == strControlName)
			{
				return pControlBase;
			}
		}
	}

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetName() == strControlName)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ���õ�ǰ����ؼ�
void CDlgBase::SetFocusControl(CControlBase* pFocusControl)
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
CControlBase* CDlgBase::GetFocusControl()
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
CControlBase* CDlgBase::GetPrevFocusableControl()
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
CControlBase* CDlgBase::GetNextFocusableControl()
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

int CDlgBase::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// ���ô��ڷ��
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE)
		| WS_MAXIMIZEBOX | WS_MINIMIZEBOX
		| WS_SYSMENU | WS_SIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CS_DBLCLKS;
	dwStyle &= ~(WS_CAPTION);

	// �ı䴰�ڴ�С
	if(!m_bChangeSize)
	{
		dwStyle &= ~(WS_THICKFRAME);
	}

	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	return CDialog::OnCreate(lpCreateStruct);
}

// ��XML����resize����
HRESULT CDlgBase::OnAttributeResize(const CString& strValue, BOOL bLoading)
{
    if (strValue.IsEmpty()) return E_FAIL;

	// ��ȡresize���ԣ�����������һ�´��ڷ��
	m_bChangeSize = _ttoi(strValue);

	// ���ô��ڷ��
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE)
		| WS_MAXIMIZEBOX | WS_MINIMIZEBOX
		| WS_SYSMENU | WS_SIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CS_DBLCLKS;
	dwStyle &= ~(WS_CAPTION);

	// �ı䴰�ڴ�С
	if(!m_bChangeSize)
	{
		dwStyle &= ~(WS_THICKFRAME);
	}

	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	
	return bLoading?S_FALSE:S_OK;
}

// ��ʼ�����ڱ���Ƥ��(���ص������ڴ�dc)
void CDlgBase::InitWindowBkSkin()
{
	int nType = 0;
	int nIDResource = 0;
	COLORREF clr = RGB(0,0,0);
	CString strImgFile = _T("");

	BOOL bRet = TRUE;
	if(!m_strBkImg.IsEmpty())	// ������������˱���ͼƬ���ԣ����ô˱���ͼƬ
	{
		// ͨ��Skin��ȡ
		if(m_strBkImg.Find(_T("skin:")) == 0)
		{
			strImgFile = DuiSystem::Instance()->GetSkin(m_strBkImg);
		}else
		{
			strImgFile = m_strBkImg;
		}

		if(strImgFile.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
		{
			nType = BKTYPE_IMAGE_FILE;
		}else	// ����ͼƬ��Դ
		{
			nIDResource = _ttoi(strImgFile);
			nType = BKTYPE_IMAGE_RESOURCE;
		}
	}else
	if(m_crlBack != RGB(0,0,0))	// ������������˱�����ɫ���ԣ����ô˱�����ɫ
	{
		nType = BKTYPE_COLOR;
		clr = m_crlBack;
	}else
	{
		// ����DuiSystem��Ӧ�ó����ȡ������Ϣ
		bRet = DuiSystem::Instance()->GetWindowBkInfo(nType, nIDResource, clr, strImgFile);
	}

	// ���ô��ڱ���Ƥ��
	if(bRet && !((nType == BKTYPE_IMAGE_RESOURCE) && (nIDResource == 0)))
	{
		if(nType == BKTYPE_IMAGE_RESOURCE)	// ͼƬ��Դ
		{
			LoadBackgroundImage(nIDResource);
		}else
		if(nType == BKTYPE_COLOR)	// ��ɫ
		{
			DrawBackground(clr);
		}else
		if(nType == BKTYPE_IMAGE_FILE)	// ͼƬ�ļ�
		{
			LoadBackgroundImage(strImgFile);
		}
	}else
	{
		// Ĭ�ϼ��ص�һ�ű���ͼƬ
		strImgFile = DuiSystem::Instance()->GetSkin(_T("SKIN_PIC_0"));
		LoadBackgroundImage(strImgFile);
	}
}

// ��קͼƬ���´��ڱ���ͼƬ
void CDlgBase::OnDropFiles(HDROP hDropInfo)
{
	CPoint ptDrop;
	BOOL bQueryPoint = (DragQueryPoint(hDropInfo, &ptDrop) && m_pControl);

	TCHAR szFileName[MAX_PATH + 1] = {0};
	UINT nFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	for(UINT i = 0; i < nFiles; i++)
	{
		DragQueryFile(hDropInfo, i, szFileName, MAX_PATH);
		if(PathIsDirectory(szFileName))
		{
			continue;
		}
		CString strFileName = szFileName;

		CString strEnableDragFile = DuiSystem::Instance()->GetConfig(_T("enableDragFile"));
		//strEnableDragFile������=0
		if(strEnableDragFile != _T("2"))	//ֻ������
		{
			// �����ǰ�ؼ����Դ�����ק�ļ����¼�,����Ҫ�����Ĵ���
			if(bQueryPoint && m_pControl->OnControlDropFile(ptDrop, strFileName))
			{
				continue;
			}
		}

		if(strEnableDragFile != _T("1"))
		{
			// ��ǰ�ؼ�δ������¼�,���ȡ�ļ���׺,����ļ���׺��ͼƬ����ı���
			strFileName = strFileName.Right(3);
			if (0 == strFileName.CompareNoCase(TEXT("bmp")) || 0 == strFileName.CompareNoCase(TEXT("jpg")) || 0 == strFileName.CompareNoCase(TEXT("png")))
			{
				LoadBackgroundImage(szFileName);
				// ���汳����Ϣ
				DuiSystem::Instance()->SetWindowBkInfo(BKTYPE_IMAGE_FILE, 0, RGB(0,0,0), szFileName);
				// ˢ�����д��ڵı���Ƥ��
				DuiSystem::Instance()->ResetAllWindowsBkSkin();
				break;
			}
		}
	}
	// CDialog::OnDropFiles(hDropInfo);
}

// ���ش��ڱ���ͼƬ(����Դ����)
void CDlgBase::LoadBackgroundImage(UINT nIDResource, CString strType)
{
	CBitmap bitBackground;
	LoadBitmapFromIDResource(nIDResource, bitBackground, m_sizeBKImage, strType);	
	DrawBackground(bitBackground);
}

// ���ش��ڱ���ͼƬ(���ļ���zip��Դ����)
void CDlgBase::LoadBackgroundImage(CString strFileName)
{
	CBitmap bitBackground;
	if(DuiSystem::Instance()->LoadBitmapFile(strFileName, bitBackground, m_sizeBKImage))
	{
		DrawBackground(bitBackground);
	}
}

// ������ͼƬ
void CDlgBase::DrawBackground(CBitmap &bitBackground)
{
	if(m_MinSize.cx - 2 > m_sizeBKImage.cx || m_MinSize.cy - 2 > m_sizeBKImage.cy || m_bChangeSize)
	{
		if(m_MinSize.cx - 2 > m_sizeBKImage.cx)
		{
			m_nOverRegioX = __min(100, m_sizeBKImage.cx - 2);
		}
		else
		{
			m_nOverRegioX = 0;
		}

		if(m_MinSize.cy - 2 > m_sizeBKImage.cy)
		{
			m_nOverRegioY = __min(100, m_sizeBKImage.cy - 2);
		}
		else
		{
			m_nOverRegioY = 0;
		}
	}
 	else
 	{
 		m_nOverRegioX = 0;
		m_nOverRegioY = 0;
 	}
	if(bitBackground.m_hObject)
	{
		m_bDrawImage = TRUE;
		int nWidth = m_sizeBKImage.cx;
		int nHeight = m_sizeBKImage.cy;

		if(m_MemBKDC.m_hDC)
		{
			m_MemBKDC.DeleteDC();
		}

		CDC *pDC = GetDC();
		
		::GetAverageColor(pDC, bitBackground, m_sizeBKImage, m_clrBK);

		m_MemBKDC.CreateCompatibleDC(pDC);

		if(m_MemBK.m_hObject)
		{
			m_MemBK.DeleteObject();
		}
		m_MemBK.CreateCompatibleBitmap(pDC, m_sizeBKImage.cx, m_sizeBKImage.cy);
		m_pOldMemBK =  m_MemBKDC.SelectObject(&m_MemBK);

		CDC TempDC;
		TempDC.CreateCompatibleDC(pDC);

		CBitmap* pOldBitmap = TempDC.SelectObject(&bitBackground);

		// ����ƽ��ͼƬ
		m_MemBKDC.FillSolidRect(0, 0, nWidth, nHeight, m_clrBK);
		// �������ֵı���͸��
		//m_MemBKDC.SetBkMode(TRANSPARENT);

		if(m_nOverRegioX > 0 && m_nOverRegioY > 0)
		{
			int nOverRegio = __min(m_nOverRegioX, m_nOverRegioY);

			// ����
			m_MemBKDC.BitBlt(0, 0, nWidth - nOverRegio, nHeight - nOverRegio, &TempDC, 0, 0, SRCCOPY ); 

			// �м�
			CRect rc(0, 0, nWidth, nHeight);
			DrawRightBottomTransition(m_MemBKDC, TempDC, rc, nOverRegio, m_clrBK);

			// ����
			rc.SetRect(nWidth - nOverRegio, 0, nWidth, nHeight - nOverRegio);
			DrawHorizontalTransition(m_MemBKDC, TempDC, rc, rc, 0, 100);

			// ����
			rc.SetRect(0, nHeight - nOverRegio, nWidth - nOverRegio, nHeight);
			DrawVerticalTransition(m_MemBKDC, TempDC, rc, rc, 0, 100);
		}
		else if(m_nOverRegioX > 0 && m_nOverRegioY == 0)
		{
			// ���
			m_MemBKDC.BitBlt(0, 0, nWidth - m_nOverRegioX, nHeight, &TempDC, 0, 0, SRCCOPY ); 

			// �ұ�
			CRect rc(nWidth - m_nOverRegioX, 0, nWidth, nHeight);
			DrawHorizontalTransition(m_MemBKDC, TempDC, rc, rc, 0, 100);
		}
		else if(m_nOverRegioX == 0 && m_nOverRegioY > 0)
		{
			// ����
			m_MemBKDC.BitBlt(0, 0, nWidth, nHeight - m_nOverRegioY, &TempDC, 0, 0, SRCCOPY ); 

			// �±�
			CRect rc(0, nHeight - m_nOverRegioY, nWidth, nHeight);
			DrawVerticalTransition(m_MemBKDC, TempDC, rc, rc, 0, 100);
		}
		else
		{
			m_MemBKDC.BitBlt(0, 0, nWidth, nHeight, &TempDC, 0, 0, SRCCOPY ); 
		}

		TempDC.SelectObject(pOldBitmap);
		TempDC.DeleteDC();
		ReleaseDC(pDC);

		ResetControl();		
	}
}

// ��������ɫ
void CDlgBase::DrawBackground(COLORREF clr)
{
	m_clrBK = clr;
	m_bDrawImage = FALSE;
	ResetControl();		
}

BOOL CDlgBase::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

HCURSOR CDlgBase::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDlgBase::OnPaint()
{
	if (IsIconic())
	{
		// ��С��״̬���û���ͼ��,�����봴��CPaintDC,�������ΪWM_PAINT��Ϣ�ò�����ȷ�����²��ϳ���,���CPUռ���ʸ�
		// CPaintDC�Ĺ�����������������CWnd::BeginPaint��CWnd::EndPaint,���WM_PAINT�Ĵ���
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

 		/*SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);*/
	}
	else
	{
 		CRect rcUpdate;
		GetUpdateRect(&rcUpdate);

		CRect rcClient;
 		GetClientRect(&rcClient);

		CPaintDC dc(this);
		// �����ڴ�dc
		CDC MemDC;
		MemDC.CreateCompatibleDC(&dc);
		CBitmap memBmp;
		memBmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		CBitmap *pOldmap =  MemDC.SelectObject(&memBmp);

		// �������Ϳؼ����ڴ�dc
		DrawBackgroundAndControls(MemDC, rcClient, rcUpdate);

		// �ڴ�dc���������dc
		dc.BitBlt(rcUpdate.left, rcUpdate.top, rcUpdate.Width(), rcUpdate.Height(), &MemDC, rcUpdate.left, rcUpdate.top, SRCCOPY);

		MemDC.SelectObject(pOldmap);
		MemDC.DeleteDC();
		memBmp.DeleteObject();
	}
}

// �����ڱ����Ϳؼ�
void CDlgBase::DrawBackgroundAndControls(CDC &dc, const CRect &rcClient, const CRect &rcUpdate)
{
	// û��ָ������,����䱳����ɫΪ����ͼƬ��ƽ��ɫ,������ͼƬ������ʱ�𵽽���ɫ��Ч��
	dc.FillSolidRect(rcUpdate.left, rcUpdate.top, rcUpdate.Width(), rcUpdate.Height(), m_clrBK);

	// ���ָ���˱���,�Ѿ������˱������ڴ�dc,�򻭱���,�ӱ����ڴ�dc�������ǰdc(����dc�Ĵ�С����С�ڵ�ǰdc)
	if(m_bDrawImage)
	{
		CRect rcBk(1, 1, 1 + m_sizeBKImage.cx, 1 + m_sizeBKImage.cy);
		rcBk = rcBk & rcUpdate;
		if(!rcBk.IsRectEmpty())
		{
			dc.BitBlt(rcBk.left, rcBk.top, rcBk.Width() , rcBk.Height(), &m_MemBKDC, rcBk.left, rcBk.top, SRCCOPY ); 
		}	
	}

	// ���ؼ�
	DrawControl(dc, rcUpdate);
}

// ���ÿؼ�
void CDlgBase::ResetControl()
{
	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE, m_clrBK);
		}
	}

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE, m_clrBK);
		}
	}

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE, m_clrBK);			
		}
	}

	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE, m_clrBK);			
		}
	}

	InvalidateRect(NULL);
}

// �ƶ��ؼ�
CControlBase* CDlgBase::SetControlRect(UINT uControlID, CRect rc)
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
CControlBase* CDlgBase::SetControlRect(CControlBase *pControlBase, CRect rc)
{
	if(pControlBase)
	{
		pControlBase->SetRect(rc);
		UpdateHover();
	}
	return pControlBase;
}

// ��ʾ�ؼ�
CControlBase* CDlgBase::SetControlVisible(UINT uControlID, BOOL bVisible)
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
CControlBase* CDlgBase::SetControlVisible(CControlBase *pControlBase, BOOL bVisible)
{
	if(pControlBase)
	{
		pControlBase->SetVisible(bVisible);
		UpdateHover();
	}
	return pControlBase;
}

// ���ÿؼ�
CControlBase* CDlgBase::SetControlDisable(UINT uControlID, BOOL bDisable)
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
CControlBase* CDlgBase::SetControlDisable(CControlBase *pControlBase, BOOL bDisable)
{
	if(pControlBase)
	{
		pControlBase->SetDisable(bDisable);
		UpdateHover();
	}
	return pControlBase;
}

// ���������������
void CDlgBase::UpdateHover()
{
	CPoint point;
	GetCursorPos(&point);
	OnMouseMove(0, point);
}

// ���ؼ�
void CDlgBase::DrawControl(CDC &dc, const CRect &rcUpdate)
{
	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase* pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(dc, rcUpdate);
		}
	}

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase* pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(dc, rcUpdate);
		}
	}

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(dc, rcUpdate);			
		}
	}

	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase* pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(dc, rcUpdate);			
		}
	}	
}

void CDlgBase::OnNcPaint()
{
}

LRESULT CDlgBase::OnNcHitTest(CPoint point)
{
	// ���ܸı䴰�ڴ�С
	if(!m_bChangeSize)
	{
		return	HTCLIENT;
	}
	CRect	rc;
	GetWindowRect(rc);
	rc.OffsetRect(-rc.left, -rc.top);
	ScreenToClient(&point);
	int x = point.x;
	int y = point.y;

	if ( x < m_nFrameLeftRightSpace && y < m_nFrameTopBottomSpace)
	{
		return	HTTOPLEFT;
	}
	if ( x <= m_nFrameLeftRightSpace && y >= rc.bottom - m_nFrameTopBottomSpace)
	{
		return	HTBOTTOMLEFT;
	}
	if ( x > rc.right - m_nFrameLeftRightSpace && y < m_nFrameTopBottomSpace)
	{
		return	HTTOPRIGHT;
	}
	if ( x >= rc.right - m_nFrameLeftRightSpace && y >= rc.bottom - m_nFrameTopBottomSpace)
	{
		return	HTBOTTOMRIGHT;
	}

	if ( x < m_nFrameLeftRightSpace)
	{
		return	HTLEFT;
	}
	if ( x >= rc.right - m_nFrameLeftRightSpace)
	{
		return	HTRIGHT;
	}
	if ( y < m_nFrameTopBottomSpace)
	{
		return	HTTOP;
	}
	if ( y > rc.bottom - m_nFrameTopBottomSpace)
	{
		return	HTBOTTOM;
	}
	// 	if ( y <= m_nFrameTopSpace)
	// 	{
	// 		return	HTCAPTION;
	// 	}
	return	HTCLIENT;
	//	return CDialog::OnNcHitTest(point);
}

void CDlgBase::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (!IsIconic())	// ����С��״̬
	{
		BOOL bIsMaximize = IsZoomed();
		// ����Բ�Ǿ��κ�����϶�������
		int border_offset[] = {3, 2, 1};
		if (bIsMaximize)
		{				
			SetupRegion(border_offset, 0);
			m_nFrameLeftRightSpace = m_nFrameTopBottomSpace = 0;
		}
		else
		{
			SetupRegion(border_offset, 0);	// �ݲ�����Բ�Ǿ���
			m_nFrameLeftRightSpace = m_nFrameTopBottomSpace = 3;	// ���ÿ�������϶��ı䴰�ڴ�С��������
		}

		// ���ô��ڱ���͸���Ȼ�͸����ɫ
		SetupBackTranslucent();

		// �����ڵ�͸��������ɰ��ھŹ���ģʽ����������²Ż�ɼ�
		CControlBase *pControlBase = GetBaseControl(FRAME_MAINWND);
		if (pControlBase && (m_nFrameWLT == 0))
		{
			pControlBase->SetVisible(!bIsMaximize);
		}
	}

	// ���ô��ڴ�С
	CRect rc;
	GetClientRect(&rc);
	SetRect(rc);
	OnSize(rc);

	// ���������ؼ���λ��
	if (!IsIconic())	// ����С��״̬
	{
		// ����AREA�Ĵ�С����
		for (size_t i = 0; i < m_vecBaseArea.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseArea.at(i);
			if (pControlBase)
			{
				pControlBase->OnPositionChange();
				UpdateHover();
			}
		}
		// �����ؼ��Ĵ�С����
		for (size_t i = 0; i < m_vecBaseControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseControl.at(i);
			if (pControlBase)
			{
				pControlBase->OnPositionChange();
				UpdateHover();
			}
		}

		// �û�AREA�Ĵ�С����
		for (size_t i = 0; i < m_vecArea.size(); i++)
		{
			CControlBase * pControlBase = m_vecArea.at(i);
			if (pControlBase)
			{
				pControlBase->OnPositionChange();
				UpdateHover();
			}
		}
		// �û��ؼ��Ĵ�С����
		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				pControlBase->OnPositionChange();
				UpdateHover();
			}
		}
	}

	InvalidateRect(NULL);
}

// ���ô��ڱ���͸���Ȼ򱳾�͸����ɫ
void CDlgBase::SetupBackTranslucent()
{
	if(m_nBackTranslucent != 255)
	{
		// ���ñ���͸����
 		HWND hWnd = GetSafeHwnd();
 		SetWindowLong(hWnd,GWL_EXSTYLE,GetWindowLong(hWnd,GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(0, m_nBackTranslucent, LWA_ALPHA);
	}else
	if(m_crlBackTransParent != RGB(255,255,255))
	{
		// ���ñ���͸������ɫ
		HWND hWnd = GetSafeHwnd();
 		SetWindowLong(hWnd,GWL_EXSTYLE,GetWindowLong(hWnd,GWL_EXSTYLE) | WS_EX_LAYERED);
 		SetLayeredWindowAttributes(m_crlBackTransParent, 255, LWA_COLORKEY );
	}
}

// ���ô�������
void CDlgBase::SetupRegion(int border_offset[], int nSize)
{
	CDC* pDC = GetDC();

	CRect	rc;
	GetWindowRect(rc);
	rc.OffsetRect(-rc.left, -rc.top);

	// ͨ������ü��ķ�ʽ�����ڵ�Բ�Ǿ���,nSize��ʾԲ�Ǿ��εİ뾶
	CRgn	rgn;
	rgn.CreateRectRgn(0, 0, rc.Width(), rc.Height());
	CRgn	rgn_xor;
	CRect	rcXor;
	
	for (int y = 0; y < nSize; ++y)
	{
		rcXor.SetRect(0, y, border_offset[y], y + 1);
		rgn_xor.CreateRectRgn(0, y, border_offset[y], y + 1);
		rgn.CombineRgn(&rgn, &rgn_xor, RGN_XOR);
		rgn_xor.DeleteObject();
	}

	for (int y = 0; y < nSize; ++y)
	{
		rcXor.SetRect(rc.right - border_offset[y], y, rc.right, y + 1);
		rgn_xor.CreateRectRgn(rc.right - border_offset[y], y, rc.right, y + 1);
		rgn.CombineRgn(&rgn, &rgn_xor, RGN_XOR);
		rgn_xor.DeleteObject();
	}

	for (int y = 0; y < nSize; ++y)
	{
		rcXor.SetRect(0, rc.bottom - y - 1, border_offset[y],  rc.bottom - y);
		rgn_xor.CreateRectRgn(0, rc.bottom - y - 1, border_offset[y],  rc.bottom - y);
		rgn.CombineRgn(&rgn, &rgn_xor, RGN_XOR);
		rgn_xor.DeleteObject();
	}

	for (int y = 0; y < nSize; ++y)
	{
		rcXor.SetRect(rc.right - border_offset[y], rc.bottom - y - 1, rc.right, rc.bottom -  y);
		rgn_xor.CreateRectRgn(rc.right - border_offset[y], rc.bottom - y - 1, rc.right,rc.bottom -  y);
		rgn.CombineRgn(&rgn, &rgn_xor, RGN_XOR);
		rgn_xor.DeleteObject();
	}

	SetWindowRgn((HRGN)rgn, TRUE);
	m_Rgn.DeleteObject();
	m_Rgn.Attach(rgn.Detach());
	ReleaseDC(pDC);
}

void CDlgBase::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);

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

void CDlgBase::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (lpwndpos->cx < m_MinSize.cx)
	{
		lpwndpos->cx = m_MinSize.cx;
	}
	if (lpwndpos->cy < m_MinSize.cy)
	{
		lpwndpos->cy = m_MinSize.cy;
	}
}

void CDlgBase::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	//	CDialog::OnWindowPosChanged(lpwndpos);

	::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED
		| SWP_NOSENDCHANGING | SWP_NOACTIVATE);

	SendMessage(WM_MOVE, 0, MAKELPARAM(lpwndpos->x, lpwndpos->y));
	SendMessage(WM_SIZE, 0, MAKELPARAM(lpwndpos->cx, lpwndpos->cy));

	OnSize(0, lpwndpos->cx, lpwndpos->cy);
}

void CDlgBase::PostNcDestroy()
{

}

// ���ڹر�
void CDlgBase::OnClose()
{
	// ���ص�WM_CLOSE��Ϣ������,�����κ�����,����ͨ��Windows���������Թرճ���
	CControlBase* pControlBase = GetBaseControl(NAME_BT_CLOSE);
	if(pControlBase)
	{
		if(pControlBase->GetAction() == ACTION_HIDE_WINDOW)
		{
			// ��������˶���Ϊ���ش���,��ֻ���ش���
			ShowWindow(SW_HIDE);
		}else
		{
			// �رմ���
			DoClose();
		}
	}
}

// �Զ���Ĵ��ڹر���Ϣ����
LRESULT CDlgBase::OnUserCloseWindow(WPARAM wParam, LPARAM lParam)
{
	// ɾ�����ж�ʱ��
	CTimer::KillTimer();
	// wParam������ʾ�Ի���ķ���ֵ
	if(wParam == IDOK)
	{
		OnOK();
	}else
	if(wParam == IDCANCEL)
	{
		OnCancel();
	}else
	{
		EndDialog(wParam);
	}

	return 0;
}

// ������С��
void CDlgBase::OnMinimize()
{
	ShowWindow(SW_MINIMIZE);
	// ��С���ᵼ������ӵ�еĴ��ڲ���ʾ,�����Ҫ��ʾ����ӵ�еĴ���
	ShowOwnedPopups();
}

// ��������л�
BOOL CDlgBase::OnMaximize()
{

 	if (IsZoomed())
 	{
 		ShowWindow(SW_RESTORE);
		return FALSE;
 	}
 	else
 	{
 		ShowWindow(SW_SHOWMAXIMIZED);
		return TRUE;
 	}
}

// ���ڵ�Ƥ��ѡ��
void CDlgBase::OnSkin()
{
	CControlBase* pControlBase = GetBaseControl(NAME_BT_SKIN);
	if(pControlBase == NULL)
	{
		return;
	}

	CDlgSkin *pDlgSkin = new CDlgSkin;
	CRect rc = pControlBase->GetRect();
	rc.OffsetRect(-95, rc.Height());
	ClientToScreen(&rc);
	pDlgSkin->LoadXmlFile(DuiSystem::Instance()->GetXmlFile(_T("dlg_skin")));
	pDlgSkin->Create(this, rc, WM_SKIN);
	pDlgSkin->ShowWindow(SW_SHOW);
}

// ������С����ť����Ϣ����
LRESULT CDlgBase::OnMessageButtonMin(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(MSG_BUTTON_UP == uMsg)
	{
		OnMinimize();
	}

	return 0;
}

// ����
LRESULT CDlgBase::OnMessageSkin(WPARAM wParam, LPARAM lParam)
{
	if(MSG_CLOSE == wParam)
	{
	}else
	if(MSG_SKIN == wParam)
	{
		// ѡ���˱���Ƥ��
		SelectInfo *pSelectInfo = (SelectInfo *)lParam;
		if(pSelectInfo != NULL)
		{
			if(pSelectInfo->nType == BKTYPE_COLOR)
			{
				Color &clr = pSelectInfo->clr;
				COLORREF crlBack = RGB(clr.GetRed(), clr.GetGreen(), clr.GetBlue());
				DrawBackground(crlBack);
				// ���汳����Ϣ
				DuiSystem::Instance()->SetWindowBkInfo(BKTYPE_COLOR, 0, crlBack, _T(""));
				// ˢ�����д��ڵı���Ƥ��
				DuiSystem::Instance()->ResetAllWindowsBkSkin();
			}else
			if(pSelectInfo->nType == BKTYPE_IMAGE_RESOURCE)
			{
				// ��ȡ����ͼƬ�ļ���,������
				CString strBkSkin;
				strBkSkin.Format(_T("SKIN_PIC_%d"), pSelectInfo->uIndex);
				CString strImgFile = DuiSystem::Instance()->GetSkin(strBkSkin);
				LoadBackgroundImage(strImgFile);
				// ���汳����Ϣ
				DuiSystem::Instance()->SetWindowBkInfo(BKTYPE_IMAGE_FILE, 0, RGB(0,0,0), strImgFile);
				// ˢ�����д��ڵı���Ƥ��
				DuiSystem::Instance()->ResetAllWindowsBkSkin();
			}
		}
	}else
	if(MSG_SELECT_SKIN == wParam)
	{
		// ѡ��Ƥ���ļ�
 		CFileDialog DlgFile(TRUE,NULL,NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY ,
 			TEXT("Skin picture(*.bmp;*.png;*.jpg)|*.bmp;*.png;*.jpg||"),this, 0);
 		DlgFile.m_ofn.nFilterIndex=1;
 		DlgFile.m_ofn.hwndOwner=m_hWnd;
 		DlgFile.m_ofn.lStructSize=sizeof(OPENFILENAME);
 		DlgFile.m_ofn.lpstrTitle=TEXT("Open");
 		DlgFile.m_ofn.nMaxFile=MAX_PATH;
 		if(DlgFile.DoModal() == IDOK)
 		{
 			CString strFileName = DlgFile.GetPathName();
 			CString strFileType = strFileName.Right(3);
 			if (0 == strFileType.CompareNoCase(TEXT("bmp")) || 0 == strFileType.CompareNoCase(TEXT("jpg")) || 0 == strFileType.CompareNoCase(TEXT("png")))
 			{
 				LoadBackgroundImage(strFileName);
				// ���汳����Ϣ
				DuiSystem::Instance()->SetWindowBkInfo(BKTYPE_IMAGE_FILE, 0, RGB(0,0,0), strFileName);
				// ˢ�����д��ڵı���Ƥ��
				DuiSystem::Instance()->ResetAllWindowsBkSkin();
 			}
 		}
	}

	return 0;
}

// תUI�̴߳��������
LRESULT CDlgBase::OnMessageUITask(WPARAM wParam, LPARAM lParam)
{
	DuiVision::IBaseTask* pTask = (DuiVision::IBaseTask*)wParam;
	DuiVision::CTaskMgr* pTaskMgr = (DuiVision::CTaskMgr*)lParam;

	BOOL bRet = FALSE;
	if((pTask != NULL) && (pTaskMgr != NULL))
	{
		bRet = pTask->TaskProcess(pTaskMgr);
		if(bRet)
		{
			pTask->TaskNotify(pTaskMgr, DuiVision::IBaseTask::TE_Completed);
		}else
		{
			pTask->TaskNotify(pTaskMgr, DuiVision::IBaseTask::TE_Canceled);
		}
	}
	return bRet;
}

// Tray��Ϣ
LRESULT CDlgBase::OnSystemTrayIcon(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case WM_LBUTTONDBLCLK:
		{
			CString strTrayDbClickMsg = DuiSystem::Instance()->GetConfig(_T("trayDbClickMsg"));
			if(strTrayDbClickMsg == _T("1"))
			{
				// ������˫����Ϣ
				DuiSystem::AddDuiActionTask(TRAY_ICON, MSG_TRAY_DBCLICK, 0, 0, NAME_TRAY_ICON, _T(""), this);
			}else
			{
				// ��ʾ����
				SetForegroundWindow();
				ShowWindow(SW_NORMAL);
				ShowWindow(SW_SHOW);
				UpdateWindow();
				BringWindowToTop();
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			// ���������������Ϣ
			DuiSystem::AddDuiActionTask(TRAY_ICON, MSG_TRAY_LBUTTONDOWN, 0, 0, NAME_TRAY_ICON, _T(""), this);
		}
		break;
	case WM_RBUTTONUP:
		{
			// ��ʾ���̲˵�
			CString strXmlFile = _T("menu_tray");
			if(!m_strTrayMenuXml.IsEmpty())	// ������������̲˵��ļ�,��ʹ�����õ��ļ�
			{
				strXmlFile = m_strTrayMenuXml;
			}
			if(!strXmlFile.IsEmpty())
			{
				SetForegroundWindow();
				// ���ز˵��ؼ�
				CDuiMenu* pDuiMenu = new CDuiMenu();
				pDuiMenu->SetAutoClose(FALSE);
				pDuiMenu->SetParent(this);
				if(m_pTrayHandler != NULL)
				{
					// �����¼��������
					pDuiMenu->RegisterHandler(m_pTrayHandler);
				}
				CPoint point;
				GetCursorPos(&point);
				if(pDuiMenu->LoadXmlFile(strXmlFile, this, point, WM_DUI_MENU))
				{
					// ����˵���λ�ò���ʾ
					CRect rc;
					pDuiMenu->GetWindowRect(&rc);
					int nMenuWidth = rc.Width();
					rc.OffsetRect(0, -rc.Height());
					// ���������Ļ�Ҳ෶Χ,��˵����������ƶ�һЩ
					// ע:SM_CXSCREEN��ʾ������Ļ�Ŀ��,SM_CXFULLSCREEN��ʾ��ʾ��Ļ�Ŀ��,����������������
					int nScreenWidth= GetSystemMetrics(SM_CXSCREEN);
					if(rc.right > nScreenWidth)
					{
						rc.OffsetRect(-nMenuWidth, 0);	// �ƶ�����ǰ�˵����
					}
					// ����˵�����С����Ļ����,��˵�������Ϊ��Ļ����λ��
					//int nScreenHeight= GetSystemMetrics(SM_CYSCREEN);
					if(rc.top < 0)
					{
						rc.OffsetRect(0, 10 - rc.top);
					}
					pDuiMenu->MoveWindow(rc);
					pDuiMenu->ShowWindow(SW_SHOW);
					pDuiMenu->SetAutoClose(TRUE);
				}else
				{
					delete pDuiMenu;
				}
			}
		}
		break;
	}
	return 0;
}

// ���ڿ���̴�����Ϣ��ע����Ϣ,�ڶ���DuiVisionӦ�ý���ͨ������Ϣ��һЩ��Ϣ���ݸ��������е�DuiVisionӦ�ý���
LRESULT CDlgBase::OnCheckItsMe(WPARAM wParam, LPARAM lParam)
{
	// wParam��������Ӧ��,lParam���ھ�������,���͵�������ͨ���ڴ�ӳ���ļ�����
	// �˿������Ϣ��ͨ���㲥��ʽ���͵�,������д��ڶ����յ�,ֻ����������Ҫ�������Ϣ,��������Ӧ�ú���
	// �������յ���Ϣ֮������¼����������д���

	// �ж��Ƿ�������
	if(DuiSystem::Instance()->GetDuiDialog(0) != this)
	{
		return FALSE;
	}

	// �ж��Ƿ�ǰ��DuiVisionӦ��
	if(wParam != DuiSystem::Instance()->GetAppID())
	{
		return FALSE;
	}

	// ��ȡ�ڴ�ӳ���ļ�������
	// number of characters in memory-mapped file
	const DWORD dwMemoryFileSize = sizeof(DUI_INTERPROCESS_MSG);

	// memory-mapped file name
	const LPCTSTR sMemoryFileName = _T("DF034858-1608-4147-0604-4A0CD86F6C9F");

	HANDLE hFileMapping = NULL;
	LPVOID pViewOfFile = NULL;

	// Create file mapping which can contain dwMemoryFileSize characters
	hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,           // system paging file
		NULL,                           // security attributes
		PAGE_READWRITE,                 // protection
		0,                              // high-order DWORD of size
		dwMemoryFileSize*sizeof(TCHAR), // low-order DWORD of size
		sMemoryFileName);               // name

	DWORD dwError = GetLastError();     // if ERROR_ALREADY_EXISTS 
	// this instance is not first (other instance created file mapping)

	if(! hFileMapping)
	{
		//TRACE(_T("Creating of file mapping failed.\n"));
		return FALSE;
	}

	pViewOfFile = MapViewOfFile(
		hFileMapping,               // handle to file-mapping object
		FILE_MAP_ALL_ACCESS,        // desired access
		0,
		0,
		0);                         // map all file

	if(!pViewOfFile)
	{
		//TRACE(_T("MapViewOfFile failed.\n"));
		CloseHandle(hFileMapping);
		return FALSE;
	}

	DUI_INTERPROCESS_MSG interMsg;
	memcpy(&interMsg, pViewOfFile, sizeof(DUI_INTERPROCESS_MSG));

	UnmapViewOfFile(pViewOfFile);
	CloseHandle(hFileMapping);

	// �ж�Ӧ�����Ƿ�һ��(ʹ��appMutex��ΪӦ����)
	CString strAppName = interMsg.wAppName;
	if(DuiSystem::Instance()->GetConfig(_T("appMutex")) != strAppName)
	{
		return FALSE;
	}

	// �����¼����������д���
	DuiSystem::Instance()->CallDuiHandler(interMsg.uControlID, interMsg.wControlName, interMsg.uMsg, interMsg.wParam, (LPARAM)(&interMsg));

	return TRUE;
}

// ������󻯰�ť����Ϣ����
LRESULT CDlgBase::OnMessageButtonMax(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(MSG_BUTTON_UP == uMsg)
	{
		OnMaximize();
	}

	return 0;
}

// ���ڹرհ�ť����Ϣ����
LRESULT CDlgBase::OnMessageButtonClose(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(MSG_BUTTON_UP == uMsg)
	{
		DoClose();
	}

	return 0;
}

// ����Skin��ť����Ϣ����
LRESULT CDlgBase::OnMessageButtonSkin(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(MSG_BUTTON_UP == uMsg)
	{
		OnSkin();
	}

	return 0;
}

// ���ڵļ���״̬�ı�ʱ����ô˺���
BOOL CDlgBase::OnNcActivate(BOOL bActive)
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
				m_pControl->OnLButtonUp(0, CPoint(-1, -1));
				m_pControl = NULL;				
			}
		}	
		else
		{
			if(m_pControl)
			{
				m_pControl->OnMouseMove(0, CPoint(-1, -1));
				m_pControl = NULL;				
			}
		}		
	}
	InvalidateRect(NULL);

	return TRUE;	
}

void CDlgBase::PreSubclassWindow()
{
	// �ж��Ƿ�������קͼƬ�ļ���Ϊ���ڱ���
	CString strEnableDragFile = DuiSystem::Instance()->GetConfig(_T("enableDragFile"));
	if(strEnableDragFile != _T("0"))
	{
		DragAcceptFiles(TRUE);
	}
	
	CDialog::PreSubclassWindow();
}

// ���õ�ǰ��Tooltip
void CDlgBase::SetTooltip(CControlBase* pControl, CString strTooltip, CRect rect, BOOL bControlWidth, int nTipWidth)
{
	m_wndToolTip.DelTool(this, 1);
	m_wndToolTip.Activate(FALSE);
	if(!strTooltip.IsEmpty())
	{
		//int nCount = m_wndToolTip.GetToolCount();
		if(bControlWidth)
		{
			m_wndToolTip.SetMaxTipWidth(pControl->GetRect().Width());
		}else
		if(nTipWidth > 0)
		{
			// ָ��tooltip�Ŀ��
			m_wndToolTip.SetMaxTipWidth(nTipWidth);
		}
		m_wndToolTip.AddTool(this, strTooltip, rect, 1);//TooltipĬ�϶���1��ΪID, pControl->GetID());
		m_wndToolTip.Activate(TRUE);
		m_nTooltipCtrlID = pControl->GetID();
	}
}

// �����ǰ��Tooltip
void CDlgBase::ClearTooltip()
{
	m_wndToolTip.DelTool(this, 1);
}

void CDlgBase::OnMouseMove(UINT nFlags, CPoint point) 
{	
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = ::_TrackMouseEvent(&tme);

		// ����д��ڹرն�ʱ��,����ֹͣ��ʱ��
		if(m_bTracking && (m_uTimerAutoClose != 0))
		{
			CTimer::KillTimer(m_uTimerAutoClose);
			m_uTimerAutoClose = 0;
		}
	}
	
	if (m_pControl)
	{
		/* ����tooltip��Ϊͨ���ؼ���OnMouseMove����������
		if((m_nTooltipCtrlID != m_pControl->GetID()) && !m_pControl->GetTooltip().IsEmpty())
		{
			// �����ǰ�ؼ���Tooltip,�����һ��Tooltip,Tooltip��IDĬ�϶���1
			m_wndToolTip.DelTool(this, 1);
			m_wndToolTip.Activate(FALSE);
			m_wndToolTip.AddTool(this, m_pControl->GetTooltip(), m_pControl->GetRect(), 1);
			m_wndToolTip.Activate(TRUE);
			m_nTooltipCtrlID = m_pControl->GetID();
		}*/

		if(((m_pControl->PtInRect(point) && m_pControl->OnCheckMouseResponse(nFlags, point)) || m_bIsLButtonDown) && m_bTracking)
		{			
			m_pControl->OnMouseMove(nFlags, point);
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
			// ��������Ļ����ؼ�
			for (size_t i = 0; i < m_vecBaseControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecBaseControl.at(i);
				if (pControlBase)
				{
					pControlBase->OnMouseMove(nFlags, point);
					if(pControlBase->PtInRect(point) && pControlBase->GetRresponse())
					{
						m_pControl = pControlBase;
						bIsSystemSelect = TRUE;
					}
				}		
			}
			
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
	}
	
	//CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgBase::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = FALSE;
	if (!m_bIsLButtonDown)
	{
		if (m_pControl)
		{
			m_pControl->OnMouseMove(0, CPoint(-1, -1));
		}
			
		m_pControl = NULL;
	}

	// ����д��ڹرջ����ر�־,�򴴽��رն�ʱ��
	if((m_bAutoClose || m_bAutoHide) && (m_uAutoCloseDelayTime != 0) && (m_uTimerAutoClose == 0))
	{
		m_uTimerAutoClose = CTimer::SetTimer(m_uAutoCloseDelayTime);
	}
	
	return 0;
}

LRESULT CDlgBase::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
 	//if (m_pControl)
 	//{
 	//	CPoint point;
 	//	GetCursorPos(&point);
 	//	//ScreenToClient(&point);
		//m_pControl->OnMouseMove(0, point);
 	//}
	return 0;
}

// ��������Ϣ����
BOOL CDlgBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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
			// ��������Ļ����ؼ�
			for (size_t i = 0; i < m_vecBaseControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecBaseControl.at(i);
				if (pControlBase && pControlBase->OnScroll(TRUE, nFlag, pt))
				{
					bResponse = TRUE;
					break;
				}		
			}
			
			// �û��ؼ�
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

// ����������
void CDlgBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL bIsSelect = false;
	m_bIsLButtonDblClk = FALSE;

	// ���������Ĳ���ԭ���Ľ���ؼ�,�����ԭ���Ľ���ؼ�
	if((m_pFocusControl != m_pControl) && (m_pFocusControl != NULL))
	{
		SetFocusControl(NULL);
	}

	BOOL bHandled = FALSE;
	if (m_pControl)
	{
		if(m_pControl->GetVisible() && m_pControl->GetRresponse())
		{
			if (m_pControl->PtInRect(point) && m_pControl->OnCheckMouseResponse(nFlags, point))
			{
				bIsSelect = TRUE;
				m_bIsLButtonDown = TRUE;

				m_pFocusControl = m_pControl;
				bHandled = m_pControl->OnLButtonDown(nFlags, point);						
			}
		}
	}
	
	// ��ǰ�ؼ��Ƿ���Բ������
	if (bIsSelect && !m_bIsSetCapture && ((m_pControl == NULL) || m_pControl->OnCheckMouseResponse(nFlags, point)))
	{
		SetCapture();
		m_bIsSetCapture = TRUE;
		
		return;
	}

	if(!bHandled)
	{
		// �����϶���Ϣ
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));
	}

	CDialog::OnLButtonDown(nFlags, point);
}

// �������ſ�
void CDlgBase::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bIsSetCapture)
	{
		ReleaseCapture();
		m_bIsSetCapture = false;
	}

	m_bIsLButtonDown = FALSE;

	if (m_pControl)
	{
		if(m_pControl->GetVisible() && m_pControl->GetRresponse())
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

	CDialog::OnLButtonUp(nFlags, point);
}

// ������˫��
void CDlgBase::OnLButtonDblClk(UINT nFlags, CPoint point)
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

	// �ж��Ƿ���ڴ��ڱ�������ؼ�
	CControlBase* pCaptionControl = GetBaseControl(NAME_AREA_CAPTION);
	if(pCaptionControl == NULL)
	{
		pCaptionControl = GetBaseControl(AREA_CAPTION);
	}
	if(pCaptionControl)
	{
		// ����ҵ����ڱ�������ؼ�,��������ڴ˿ؼ���Χ��,���ͷǿͻ���˫���¼�,�������ڵ���󻯺ͻָ�
		CRect rcCaption = pCaptionControl->GetRect();
		if(rcCaption.PtInRect(point))
		{
			PostMessage(WM_NCLBUTTONDBLCLK,HTCAPTION,MAKELPARAM(point.x, point.y));
		}
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

// ����Ҽ�����
void CDlgBase::OnRButtonDown(UINT nFlags, CPoint point)
{
	BOOL bIsSelect = false;
	m_bIsRButtonDblClk = FALSE;

	// ���������Ĳ���ԭ���Ľ���ؼ�,�����ԭ���Ľ���ؼ�
	if((m_pFocusControl != m_pControl) && (m_pFocusControl != NULL))
	{
		SetFocusControl(NULL);
	}

	BOOL bHandled = FALSE;
	if (m_pControl)
	{
		if(m_pControl->GetVisible() && m_pControl->GetRresponse())
		{
			if (m_pControl->PtInRect(point) && m_pControl->OnCheckMouseResponse(nFlags, point))
			{
				bIsSelect = TRUE;
				m_bIsRButtonDown = TRUE;

				m_pFocusControl = m_pControl;
				bHandled = m_pControl->OnRButtonDown(nFlags, point);						
			}
		}
	}

	CDialog::OnRButtonDown(nFlags, point);
}

// ����Ҽ��ſ�
void CDlgBase::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bIsRButtonDown = FALSE;

	if (m_pControl)
	{
		if(m_pControl->GetVisible() && m_pControl->GetRresponse())
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

	CDialog::OnRButtonUp(nFlags, point);
}

// ����Ҽ�˫��
void CDlgBase::OnRButtonDblClk(UINT nFlags, CPoint point)
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

	CDialog::OnRButtonDblClk(nFlags, point);
}

// �����¼�����(�Ƶ�PreTranslateMessage��ʵ���ӿؼ��ļ����¼�����,��Ϊ�Ի����OnKeyDown�����о�����,���ܲ���ALT����ϼ�)
void CDlgBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

// ��ϢԤ����
BOOL CDlgBase::PreTranslateMessage(MSG* pMsg)
{
	if (( pMsg->message == WM_KEYDOWN ) || ( pMsg->message == WM_SYSKEYDOWN ))
	{
		// �����¼�����
		UINT nFlags = 0;
		BOOL bCtrl=::GetKeyState(VK_CONTROL)&0x8000;
		BOOL bShift=::GetKeyState(VK_SHIFT)&0x8000;
		BOOL bAlt=::GetKeyState(VK_MENU)&0x8000;	// ALT��ֻ��WM_SYSKEYDOWN��Ϣ���ܲ���
		nFlags |= (bCtrl ? VK_CONTROL : 0);
		nFlags |= (bShift ? VK_SHIFT : 0);
		nFlags |= (bAlt ? VK_MENU : 0);

		// ����tab����shift+tab��,�л�����
		if((pMsg->wParam == VK_TAB) && (nFlags == 0))
		{
			CControlBase* pFocusControl = GetFocusControl();
			CControlBase* pNextControl = GetNextFocusableControl();
			if(pNextControl != pFocusControl)
			{
				SetFocusControl(pNextControl);
				return TRUE;
			}
		}else
		if((pMsg->wParam == VK_TAB) && (nFlags == VK_SHIFT))
		{
			CControlBase* pFocusControl = GetFocusControl();
			CControlBase* pPrevControl = GetPrevFocusableControl();
			if(pPrevControl != pFocusControl)
			{
				SetFocusControl(pPrevControl);
				return TRUE;
			}
		}

		// ��ǰ�ؼ��Ƿ��ܴ���
		if (m_pControl && m_pControl->OnKeyDown(pMsg->wParam, 1, nFlags))
		{
			return TRUE;
		}

		// ��������Ļ����ؼ�
		for (size_t i = 0; i < m_vecBaseControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseControl.at(i);
			if (pControlBase && pControlBase->OnKeyDown(pMsg->wParam, 1, nFlags))
			{
				return TRUE;
			}		
		}
		
		// �û��ؼ�
		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase && pControlBase->OnKeyDown(pMsg->wParam, 1, nFlags))
			{
				return TRUE;
			}	
		}

		// ����ؼ�û�д���,��س���ESC������Ҫ���ε�
		if(pMsg->wParam == VK_ESCAPE) 
		{
			return TRUE;
		}
		if(pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}else
	/*if ( pMsg->message == WM_CHAR)
	{
		//DuiSystem::LogEvent(LOG_LEVEL_DEBUG, L"WM_CHAR:%d,%d", pMsg->wParam, pMsg->lParam);
		//CControlBase* pFocusControl = GetFocusControl();
		//if(pFocusControl && (pFocusControl->GetNativeHWnd() != NULL))
		//{
		//	::SendMessage(pFocusControl->GetNativeHWnd(), WM_CHAR, pMsg->wParam, pMsg->lParam);
		//}
		// �û��ؼ�
		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase && (pControlBase->GetNativeHWnd() != NULL))
			{
				::SendMessage(pControlBase->GetNativeHWnd(), WM_CHAR, pMsg->wParam, pMsg->lParam);
			}	
		}
	}else*/
	if ( pMsg->message == WM_MOUSEMOVE ||
		 pMsg->message == WM_LBUTTONDOWN ||
		 pMsg->message == WM_LBUTTONUP )
	{
		m_wndToolTip.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// ��ʱ����Ϣ(����CTimer��ĺ���)
void CDlgBase::OnTimer(UINT uTimerID)
{
	if(m_uTimerAnimation == uTimerID)	// ������ʱ��
	{
		for (size_t i = 0; i < m_vecBaseControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseControl.at(i);
			if (pControlBase)
			{
				pControlBase->OnTimer();
			}		
		}

		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				pControlBase->OnTimer();
			}		
		}
	}else
	if(m_uTimerAutoClose == uTimerID)	// �����Զ��رյĶ�ʱ��
	{
		CTimer::KillTimer(m_uTimerAutoClose);
		m_uTimerAutoClose = 0;
		if(m_bAutoHide)
		{
			ShowWindow(SW_HIDE);	// ���ش���ģʽ
		}else
		{
			DoClose();	// �رմ���ģʽ
		}
	}
}

// ��ʱ����Ϣ(����ʱ�����ֵĶ�ʱ����,����CTimer��ĺ���)
void CDlgBase::OnTimer(UINT uTimerID, CString strTimerName)
{
	// Ӧ�ô����Ķ�ʱ���������¼��������Ķ�ʱ������
	DuiSystem::Instance()->CallDuiHandlerTimer(uTimerID, strTimerName);
}

// �����ؼ�����Ϣ��Ӧ
LRESULT CDlgBase::OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CControlBase* pControlBase = GetBaseControl(uID);
	CControlBase* pControl = GetControl(uID);
	if((pControlBase == NULL) && (pControl == NULL))
	{
		return 0L;
	}

	if(pControlBase != NULL)
	{
		// 2.�ж��Ƿ񼸸�ϵͳ��ť,ִ����Ӧ��ϵͳ����
		if(pControlBase->IsThisObject(BT_MIN, NAME_BT_MIN))
		{
			OnMessageButtonMin(uMsg, wParam, lParam);
		}
		else if(pControlBase->IsThisObject(BT_MAX, NAME_BT_MAX))
		{
			OnMessageButtonMax(uMsg, wParam, lParam);
		}
		else if(pControlBase->IsThisObject(BT_CLOSE, NAME_BT_CLOSE))
		{
			if(pControlBase->GetAction() == ACTION_HIDE_WINDOW)
			{
				// ��������˶���Ϊ���ش���,��ֻ���ش���
				ShowWindow(SW_HIDE);
			}else
			{
				// �رմ���
				OnMessageButtonClose(uMsg, wParam, lParam);
			}
		}
		else if(pControlBase->IsThisObject(BT_SKIN, NAME_BT_SKIN))
		{
			OnMessageButtonSkin(uMsg, wParam, lParam);
		}
		else
		{
			// 3.����ϵͳ��ť,����ø����Ի���Ĭ�Ͽؼ����¼�������
			for (size_t i = 0; i < m_vecBaseControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecBaseControl.at(i);
				if (pControlBase && (pControlBase->GetControlID() == uID))
				{
					pControlBase->OnMessage(uID, uMsg, wParam, lParam);
				}
			}
		}
	}else
	if(pControl != NULL)
	{
		// �Ի���ؼ����¼�����
		OnMessage(uID, uMsg, wParam, lParam);
	}

	return 0L; 
}

LRESULT CDlgBase::OnControlUpdate(CRect rcUpdate, BOOL bUpdate, CControlBase *pUpdateControlBase) 
{
	if(pUpdateControlBase == NULL) return 0;

	CRect rcAllUpDate = rcUpdate;
	if(bUpdate)
	{
		pUpdateControlBase->SetUpdate(FALSE, m_clrBK);
	}
	
	if(m_bInit)
	{
		BOOL bFind = false;
		for (size_t i = 0; i < m_vecArea.size(); i++)
		{
			CControlBase * pControlBase = m_vecArea.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE, m_clrBK);
					}
				}
				else if(pControlBase == pUpdateControlBase )
				{
					bFind = true;
				}
			}
		}

		for (size_t i = 0; i < m_vecBaseArea.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseArea.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE, m_clrBK);
					}
				}
				else if(pControlBase == pUpdateControlBase )
				{
					bFind = true;
				}
			}
		}

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
						pControlBase->SetUpdate(FALSE, m_clrBK);
					}
				}
				else if(pControlBase == pUpdateControlBase )
				{
					bFind = true;
				}		
			}
		}

		for (size_t i = 0; i < m_vecBaseControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseControl.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE, m_clrBK);
					}
				}
				else if(pControlBase == pUpdateControlBase )
				{
					bFind = true;
				}			
			}
		}	
	}

	InvalidateRect(&rcAllUpDate);

	return 0L; 
};

// DUI��Ϣ������
LRESULT CDlgBase::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
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

// �򿪵����Ի���
void CDlgBase::OpenDlgPopup(CDlgPopup *pWndPopup, CRect rc, UINT uMessageID, BOOL bShow)
{
	ASSERT(pWndPopup);
	CloseDlgPopup();
	ClientToScreen(&rc);
	m_pWndPopup = pWndPopup;
	m_pWndPopup->Create(this, rc, uMessageID);
	if(bShow)
	{
		m_pWndPopup->ShowWindow(SW_SHOW);
	}
}

// �رյ����Ի���
void CDlgBase::CloseDlgPopup()
{
	if(m_pWndPopup)
	{
 		if(m_pFocusControl && (m_pControl != m_pFocusControl))
 		{
 			SetFocusControl(NULL);
 		}
		if(IsWindow(m_pWndPopup->GetSafeHwnd()))
		{
			m_pWndPopup->CloseWindow();			
		}
	}
	m_pWndPopup = NULL;
}

void CDlgBase::OnDestroy()
{
	__super::OnDestroy();

	// ������ʱ��
	CTimer::KillTimer();
}
