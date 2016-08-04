#include "StdAfx.h"
#include <mmsystem.h> 
#include "ControlBase.h"

#pragma comment(lib,"Winmm.lib")

#define	SCROLL_V	1	// ��ֱ�������ؼ�ID
#define	SCROLL_H	2	// ˮƽ�������ؼ�ID

// �ؼ�ID�Զ����ɱ������ؼ�ID��1000��ʼ����
static int g_nControlId = 1000;

CControlBase::CControlBase(HWND hWnd, CDuiObject* pDuiObject)
{
	m_pParentDuiObject = pDuiObject;
	m_hWnd = hWnd;
	m_hwndHost = NULL;	// ԭ���ؼ�����������
	m_uID = g_nControlId++;
	if(g_nControlId > 20000)	// ����ĳ��ֵ֮������
	{
		g_nControlId = 10000;
	}
	m_rc = CRect(0,0,0,0);
	m_strPos = "";
	m_bIsVisible = TRUE;
	m_bIsHide = FALSE;
	m_bIsDisable = FALSE;
	m_bRresponse = TRUE;
	m_bTabStop = FALSE;
	m_bUpdate = FALSE;
	m_nDCWidth = 0;
	m_nDCHeight = 0;
	m_nAniDCWidth = 0;
	m_nAniDCHeight = 0;
	m_pControl = NULL;

	m_bMouseDown = false;
	m_bDblClk = false;
	m_bIsSelect = false;
	m_bIsRun = false;
	m_bRunTime = false;
	m_bImageUseECM = false;
	m_bDragEnable = false;
	m_bDropFileEnable = false;

	m_nShortcutKey = 0;
	m_nShortcutFlag = 0;

	m_nWidth = 0;
	m_nHeight = 0;

	m_strTooltip = _T("");
	m_nTipWidth = 0;
	m_strAction = _T("");
	m_bTaskMsg = FALSE;

	m_pWndPopup = NULL;

	m_hCursor = NULL;
	m_bDuiMsgMouseMove = FALSE;
	m_bDuiMsgMouseLDown = FALSE;
	m_bDuiMsgMouseLUp = FALSE;
	m_bDuiMsgMouseLDblClk = FALSE;
	m_bDuiMsgMouseRDown = FALSE;
	m_bDuiMsgMouseRUp = FALSE;
	m_bDuiMsgMouseRDblClk = FALSE;
	m_bDuiMsgKeyDown = FALSE;
	m_bMouseLeave = TRUE;
}

CControlBase::CControlBase(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible, BOOL bIsDisable,
						   BOOL bRresponse)
{
	m_pParentDuiObject = pDuiObject;
	m_hWnd = hWnd;
	m_hwndHost = NULL;	// ԭ���ؼ�����������
	m_uID = uControlID;
	m_rc = rc;
	m_strPos = "";
	m_bIsVisible = bIsVisible;
	m_bIsHide = FALSE;
	m_bIsDisable = bIsDisable;
	m_bRresponse = bRresponse;
	m_bTabStop = FALSE;
	m_bUpdate = FALSE;
	m_nDCWidth = 0;
	m_nDCHeight = 0;
	m_nAniDCWidth = 0;
	m_nAniDCHeight = 0;
	m_pControl = NULL;

	m_bMouseDown = false;
	m_bDblClk = false;
	m_bIsSelect = false;
	m_bIsRun = false;
	m_bRunTime = false;
	m_bImageUseECM = false;
	m_bDragEnable = false;
	m_bDropFileEnable = false;

	m_nWidth = 0;
	m_nHeight = 0;

	m_posMenu.nCount = 0;

	m_strTooltip = _T("");
	m_nTipWidth = 0;
	m_strAction = _T("");
	m_bTaskMsg = FALSE;

	m_pWndPopup = NULL;

	m_hCursor = NULL;
	m_bDuiMsgMouseMove = FALSE;
	m_bDuiMsgMouseLDown = FALSE;
	m_bDuiMsgMouseLUp = FALSE;
	m_bDuiMsgMouseLDblClk = FALSE;
	m_bDuiMsgMouseRDown = FALSE;
	m_bDuiMsgMouseRUp = FALSE;
	m_bDuiMsgMouseRDblClk = FALSE;
	m_bDuiMsgKeyDown = TRUE;
	m_bMouseLeave = TRUE;
}

CControlBase::~CControlBase(void)
{
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
	
	// �ͷ��ڴ�DC
	if(m_bitmap.m_hObject)
	{
		if(m_memDC.m_hDC)
		{
			m_memDC.SelectObject(m_pOldBitmap);
			m_memDC.DeleteDC();
		}
		m_bitmap.DeleteObject();
	}

	// �ͷŶ���DC
	if(m_aniBitmap.m_hObject)
	{
		if(m_animateDC.m_hDC)
		{
			m_animateDC.SelectObject(m_pOldAniBitmap);
			m_animateDC.DeleteDC();
		}
		m_aniBitmap.DeleteObject();
	}
}

// �жϵ�ǰ�Ƿ������߳�
void CControlBase::TestMainThread()
{
	// �������ڶԻ���Ĳ��Ժ���
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->TestMainThread();
	}
}

void CControlBase::Draw(CDC &dc, CRect rcUpdate)
{
	if(m_bIsVisible && !(m_rc & rcUpdate).IsRectEmpty())
	{
		DrawControl(dc, rcUpdate);

		// ��������
		for (size_t i = 0; i < m_vecArea.size(); i++)
		{
			CControlBase * pControlBase = m_vecArea.at(i);
			if (pControlBase)
			{
				pControlBase->Draw(dc, rcUpdate);
			}
		}

		// ���ӿؼ�
		DrawSubControls(dc, rcUpdate);
	}
}

// ���ӿؼ�
BOOL CControlBase::DrawSubControls(CDC &dc, CRect rcUpdate)
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible())
		{
			pControlBase->Draw(dc, rcUpdate);		
		}
	}

	return TRUE;
}

void CControlBase::SetUpdate(BOOL bUpdate, COLORREF clr/* = 0*/)
{
	m_bUpdate = bUpdate;

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(bUpdate, clr);			
		}
	}
}

// �����ڴ�DC
void CControlBase::UpdateMemDC(CDC &dc, int nWidth, int nHeight)
{
	m_bUpdate = true;
	
	if(m_nDCWidth != nWidth || m_nDCHeight != nHeight)
	{		
		m_nDCWidth = nWidth;
		m_nDCHeight = nHeight;
		if(m_bitmap.m_hObject)
		{
			if(m_memDC.m_hDC)
			{
				m_memDC.SelectObject(m_pOldBitmap);
				m_memDC.DeleteDC();
			}
			m_bitmap.DeleteObject();			
		}

		m_memDC.CreateCompatibleDC(&dc);
		m_bitmap.CreateCompatibleBitmap(&dc, m_nDCWidth, m_nDCHeight);
		m_pOldBitmap = m_memDC.SelectObject(&m_bitmap);
	}
}

// ���¶���DC
void CControlBase::UpdateAnimateDC(CDC &dc, int nWidth, int nHeight)
{
	//m_bUpdate = true;
	
	//if(m_nAniDCWidth != nWidth || m_nAniDCHeight != nHeight)
	{		
		m_nAniDCWidth = nWidth;
		m_nAniDCHeight = nHeight;
		if(m_aniBitmap.m_hObject)
		{
			if(m_animateDC.m_hDC)
			{
				m_animateDC.SelectObject(m_pOldAniBitmap);
				m_animateDC.DeleteDC();
			}
			m_aniBitmap.DeleteObject();			
		}

		m_animateDC.CreateCompatibleDC(&dc);
		m_aniBitmap.CreateCompatibleBitmap(&dc, m_nAniDCWidth, m_nAniDCHeight);
		m_pOldAniBitmap = m_animateDC.SelectObject(&m_aniBitmap);
	}
}

BOOL CControlBase::PtInRect(CPoint point)
{
	if(!m_bIsVisible) return false;

	return m_rc.PtInRect(point);
}

// ��ȡ�ؼ��ĸ����ھ��
HWND CControlBase::GetPaintHWnd()
{
	// ����ҵ����Ի���,���ø��Ի����HWnd
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		return pDlg->GetSafeHwnd();
	}

	// ����ҵ����HostWnd,����ò��HostWnd�ӿڵ�HWnd
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		return pIDuiHostWnd->GetPaintHWnd();
	}

    return NULL;
}

// ����ת��Ϊ��Ļ����
void CControlBase::ClientToScreen(LPPOINT lpPoint)
{
	// ����ҵ����Ի���,���ø��Ի����ת������
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClientToScreen(lpPoint);
		return;
	}

	// ����ҵ����HostWnd,����ò��HostWnd�ӿڵ�ת������
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->ClientToScreen(lpPoint);
		return;
	}
}

// ���ý���
BOOL CControlBase::SetWindowFocus()
{
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->SetFocus();
		return TRUE;
	}

	return FALSE;
}

// ���ÿؼ�����
BOOL CControlBase::OnFocus(BOOL bFocus)
{
	return SetControlFocus(bFocus);
}

// ���ÿؼ�����
BOOL CControlBase::SetControlFocus(BOOL bFocus)
{
	// �������ȡ��,������ÿ���ӿؼ��Ľ���״̬,�������еĿؼ������޷�ȡ��������
	if(!bFocus)
	{
		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				pControlBase->SetControlFocus(bFocus);
			}
		}
	}

	return FALSE;
}

// �жϵ�ǰ�ؼ��Ƿ񽹵�ؼ�
BOOL CControlBase::IsFocusControl()
{
	CDlgBase* pDlg = GetParentDialog(FALSE);
	if(pDlg)
	{
		if(pDlg->GetFocusControl() == this)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// ��ȡ���Գ�Ϊ����ؼ��Ŀؼ�(�����ӿؼ���ѯ)
CControlBase* CControlBase::GetFocusControl(CControlBase* pFocusControl)
{
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && (pControlBase == pFocusControl) && pControlBase->IsTabStop())
		{
			return pControlBase;
		}else
		if (pControlBase && (pControlBase == m_pControl))
		{
			// �����ӿؼ�
			pControlBase = pControlBase->GetFocusControl(pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ��ȡ��һ�����Ի�ȡ������ӿؼ�
CControlBase* CControlBase::GetPrevFocusableControl(CControlBase* pFocusControl)
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
		if (pControlBase && (pControlBase == pFocusControl))
		{
			bStartSearch = TRUE;
		}
	}

	// �ٱ����ӿؼ�����
	if(m_pControl == NULL)
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
			pControlBase = pControlBase->GetPrevFocusableControl(pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ��ȡ��һ�����Ի�ȡ������ӿؼ�
CControlBase* CControlBase::GetNextFocusableControl(CControlBase* pFocusControl)
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
		if (pControlBase && (pControlBase == pFocusControl))
		{
			bStartSearch = TRUE;
		}
	}

	// �ٱ����ӿؼ�����
	if(m_pControl == NULL)
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
			pControlBase = pControlBase->GetNextFocusableControl(pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ����Tooltip
void CControlBase::SetTooltip(CControlBase* pControl, CString strTooltip, CRect rect, BOOL bControlWidth, int nTipWidth)
{
	// ����ҵ��˸��Ի���,����öԻ��������Tooltip����
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->SetTooltip(pControl, strTooltip, rect, bControlWidth, nTipWidth);
		return;
	}

	// ����ҵ���Popup����,�����Popup���ڵ�����Tooltip����

	// ����ҵ����HostWnd,����ò��HostWnd�ӿڵ�����Tooltip����
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		int _nTipWidth = nTipWidth;
		if(bControlWidth)
		{
			_nTipWidth = pControl->GetRect().Width();
		}
		pIDuiHostWnd->SetTooltip(pControl->GetID(), strTooltip, rect, _nTipWidth);
		return;
	}
}

// ���Tooltip
void CControlBase::ClearTooltip()
{
	// ����ҵ��˸��Ի���,����öԻ�������Tooltip����
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		return;
	}

	// ����ҵ���Popup����,�����Popup���ڵ����Tooltip����

	// ����ҵ����HostWnd,����ò��HostWnd�ӿڵ����Tooltip����
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->ClearTooltip();
		return;
	}
}

// ���õ�ǰTooltip�ؼ�ID
void CControlBase::SetTooltipCtrlID(int nTooltipCtrlID)
{
	// ����ҵ��˸��Ի���,����öԻ��������Tooltip�ؼ�ID����
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->SetTooltipCtrlID(nTooltipCtrlID);
		return;
	}

	// ����ҵ���Popup����,�����Popup���ڵ�����Tooltip�ؼ�ID����

	// ����ҵ����HostWnd,����ò��HostWnd�ӿڵ�����Tooltip�ؼ�ID����
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->SetTooltipCtrlID(nTooltipCtrlID);
		return;
	}
}

// ��ȡ��ǰTooltip�ؼ�ID
int CControlBase::GetTooltipCtrlID()
{
	// ����ҵ��˸��Ի���,����öԻ���Ļ�ȡTooltip�ؼ�ID����
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		return pDlg->GetTooltipCtrlID();
	}

	// ����ҵ���Popup����,�����Popup���ڵĻ�ȡTooltip�ؼ�ID����

	// ����ҵ����HostWnd,����ò��HostWnd�ӿڵĻ�ȡTooltip�ؼ�ID����
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		return pIDuiHostWnd->GetTooltipCtrlID();
	}

	return 0;
}

// ����ƶ��¼�����
BOOL CControlBase::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;
	
	// ����ԭʼ�����λ��,������λ�ñ任
	CPoint oldPoint = point;
	OnMousePointChange(point);

	BOOL bRresponse = false;
	if(m_pControl)
	{
		CPoint pt = point;
		// ����ǿؼ����ù������ӿؼ�,�򲻽���λ�ñ任,��Ϊ������λ���ǲ���Ҫ�任��
		UINT uControlID = m_pControl->GetControlID();
		if((SCROLL_V == uControlID) || (SCROLL_H == uControlID))
		{
			pt = oldPoint;
		}
		if((m_pControl->PtInRect(pt) && m_pControl->OnCheckMouseResponse(nFlags, pt)) || m_bMouseDown)
		{
			if(m_pControl->OnMouseMove(nFlags, pt))
			{
				return true;
			}
			return false;
		}
	}

	CControlBase * pOldControl = m_pControl;
	m_pControl =  NULL;

	if(!m_strTooltip.IsEmpty() && PtInRect(point) && OnCheckMouseResponse(nFlags, point))
	{
		// �����ǰ�ؼ���Tooltip,���ҵ�ǰTooltip���õ��������ؼ�,������Tooltip
		if(GetTooltipCtrlID() != GetID())
		{
			SetTooltip(this, m_strTooltip, m_rc, FALSE, m_nTipWidth);
			SetTooltipCtrlID(GetID());
		}
	}

	// ���ÿؼ�����������꺯��
	OnControlSetCursor(nFlags, point);

	// ���ÿؼ�������ƶ�����
	bRresponse = OnControlMouseMove(nFlags, point);

	// �ؼ��϶��Ĵ���
	if(m_bDragEnable && m_bMouseDown)
	{
		CControlBase* pParentCtrl = (CControlBase*)GetParent();
		if(pParentCtrl)
		{
			CRect rcParent = pParentCtrl->GetRect();
			if(rcParent.PtInRect(point))
			{
				// ������λ���ڸ��ؼ���Χ��������϶�
				// ��ȡ�ؼ��ĵ�ǰλ�ú���굱ǰλ������һ��λ�õĲ�ֵ,���ؼ���ǰλ�ü������λ�õĲ�ֵ
				CRect rc = GetRect();
				CSize offset = point - m_ptLastMousePosition;
				rc.OffsetRect(offset);
				SetRect(rc);
				// ˢ�������һ��λ�õı���
				m_ptLastMousePosition = point;
				// ˢ�¿ؼ�
				UpdateControl(true);
			}
		}
	}

	// ��������ƶ���DUI��Ϣ
	if(m_bDuiMsgMouseMove)
	{
		if(PtInRect(point))
		{
			// ��������ڿؼ���Χ���ƶ�����Ϣ
			SendMessage(MSG_MOUSE_MOVE, (WPARAM)nFlags, (LPARAM)(&point));
			m_bMouseLeave = FALSE;
		}else
		{
			if(!m_bMouseLeave)
			{
				// ��������뿪�ؼ�����Ϣ(�뿪֮�������һ���뿪��Ϣ)
				SendMessage(MSG_MOUSE_LEAVE, (WPARAM)nFlags, (LPARAM)(&point));
				m_bMouseLeave = TRUE;
			}
		}
	}

	if(!m_bMouseDown)
	{
		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				CPoint pt = point;
				// ����ǿؼ����ù������ӿؼ�,�򲻽���λ�ñ任,��Ϊ������λ���ǲ���Ҫ�任��
				UINT uControlID = pControlBase->GetControlID();
				if((SCROLL_V == uControlID) || (SCROLL_H == uControlID))
				{
					pt = oldPoint;
				}
				if(pControlBase->OnMouseMove(nFlags, pt))
				{
					if(pControlBase->PtInRect(pt))
					{
						m_pControl = pControlBase;
					}
					bRresponse = true;
				}
			}
		}


		if (pOldControl)
		{
			bRresponse = true;
		}

		if (m_pControl)
		{
			bRresponse = true;
		}
	}

	return bRresponse;
}

// ���ÿؼ��������
BOOL CControlBase::OnControlSetCursor(UINT nFlags, CPoint point)
{
	if((m_hCursor != NULL) && m_rc.PtInRect(point))
	{
		::SetCursor(m_hCursor);
		return true;
	}

	return false;
}

// �����������¼�����
BOOL CControlBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// ����ԭʼ�����λ��,������λ�ñ任
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = m_rc.PtInRect(point);
	if(m_bMouseDown)
	{
		m_ptLastMousePosition = point;
	}

	// ��������Ƿ���ĳ���ڲ��ؼ�λ��,����ǵĻ��͸��µ�ǰ�ӿؼ�(���շ���˳�����,��Ϊ�����ں���Ŀؼ������ȼ����ߵ�)
	// �ҵ���һ�����������ľͽ�������
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->PtInRect(point))
		{
			if(pControlBase->GetVisible() && !pControlBase->GetDisable() && pControlBase->GetRresponse())
			{
				m_pControl = pControlBase;
				break;
			}
		}
	}

	if(m_pControl != NULL)
	{
		// ����ǿؼ����ù������ӿؼ�,�򲻽���λ�ñ任,��Ϊ������λ���ǲ���Ҫ�任��
		UINT uControlID = m_pControl->GetControlID();
		if((SCROLL_V == uControlID) || (SCROLL_H == uControlID))
		{
			point = oldPoint;
		}
		if(m_pControl->OnLButtonDown(nFlags, point))
		{
			return true;
		}		
	}
	else
	{
		// �л��Ի����еĵ�ǰ����ؼ�(�ݲ�����Popup���ڵ��л�)
		CDlgBase* pDlg = GetParentDialog(FALSE);
		if(pDlg)
		{
			pDlg->SetFocusControl(this);
		}

		// ��������������DUI��Ϣ
		if(m_bDuiMsgMouseLDown && m_rc.PtInRect(point))
		{
			SendMessage(MSG_MOUSE_LDOWN, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlLButtonDown(nFlags, point);
	}

	return false;
}

// �������ſ��¼�����
BOOL CControlBase::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// ����ԭʼ�����λ��,������λ�ñ任
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = false;
	if(m_pControl != NULL)
	{
		// ����ǿؼ����ù������ӿؼ�,�򲻽���λ�ñ任,��Ϊ������λ���ǲ���Ҫ�任��
		UINT uControlID = m_pControl->GetControlID();
		if((SCROLL_V == uControlID) || (SCROLL_H == uControlID))
		{
			point = oldPoint;
		}
		if(m_pControl->OnLButtonUp(nFlags, point))
		{
			return true;
		}		
	}
	else
	{
		// �����������ſ�DUI��Ϣ
		if(m_bDuiMsgMouseLUp)
		{
			SendMessage(MSG_MOUSE_LUP, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlLButtonUp(nFlags, point);
	}

	return false;
}

// ������˫���¼�����
BOOL CControlBase::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// ����ԭʼ�����λ��,������λ�ñ任
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = false;
	if(m_pControl != NULL)
	{
		// ����ǿؼ����ù������ӿؼ�,�򲻽���λ�ñ任,��Ϊ������λ���ǲ���Ҫ�任��
		UINT uControlID = m_pControl->GetControlID();
		if((SCROLL_V == uControlID) || (SCROLL_H == uControlID))
		{
			point = oldPoint;
		}
		if(m_pControl->OnLButtonDblClk(nFlags, point))
		{
			return true;
		}else
		if(!m_pControl->GetDblClk())
		{
			// ����ؼ�������˫��,����õ���������
			return m_pControl->OnLButtonDown(nFlags, point);
		}
	}
	else
	{
		if(!GetDblClk())
		{
			// ����ؼ�������˫��,����õ���������
			return OnControlLButtonDown(nFlags, point);
		}

		// ����������˫��DUI��Ϣ
		if(m_bDuiMsgMouseLDblClk && m_rc.PtInRect(point))
		{
			SendMessage(MSG_BUTTON_DBLCLK, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlLButtonDblClk(nFlags, point);
	}

	return false;
}

// ����Ҽ������¼�����
BOOL CControlBase::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// ����ԭʼ�����λ��,������λ�ñ任
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = false;
	if(m_pControl != NULL)
	{
		if(m_pControl->OnRButtonDown(nFlags, point))
		{
			return true;
		}		
	}
	else
	{
		// ��������Ҽ�����DUI��Ϣ
		if(m_bDuiMsgMouseRDown && m_rc.PtInRect(point))
		{
			SendMessage(MSG_MOUSE_RDOWN, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlRButtonDown(nFlags, point);
	}

	return false;
}

// ����Ҽ��ſ��¼�����
BOOL CControlBase::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// ����ԭʼ�����λ��,������λ�ñ任
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = false;
	if(m_pControl != NULL)
	{
		// ����ǿؼ����ù������ӿؼ�,�򲻽���λ�ñ任,��Ϊ������λ���ǲ���Ҫ�任��
		UINT uControlID = m_pControl->GetControlID();
		if(SCROLL_V == uControlID)
		{
			point = oldPoint;
		}
		if(m_pControl->OnRButtonUp(nFlags, point))
		{
			return true;
		}		
	}
	else
	{
		// ��������Ҽ��ſ�DUI��Ϣ
		if(m_bDuiMsgMouseRUp)
		{
			SendMessage(MSG_MOUSE_RUP, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlRButtonUp(nFlags, point);
	}

	return false;
}

// ����Ҽ�˫���¼�����
BOOL CControlBase::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// ����ԭʼ�����λ��,������λ�ñ任
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = false;
	if(m_pControl != NULL)
	{
		// ����ǿؼ����ù������ӿؼ�,�򲻽���λ�ñ任,��Ϊ������λ���ǲ���Ҫ�任��
		UINT uControlID = m_pControl->GetControlID();
		if((SCROLL_V == uControlID) || (SCROLL_H == uControlID))
		{
			point = oldPoint;
		}
		if(m_pControl->OnRButtonDblClk(nFlags, point))
		{
			return true;
		}else
		if(!m_pControl->GetDblClk())
		{
			// ����ؼ�������˫��,����õ���������
			return m_pControl->OnRButtonDown(nFlags, point);
		}
	}
	else
	{
		if(!GetDblClk())
		{
			// ����ؼ�������˫��,����õ���������
			return OnControlRButtonDown(nFlags, point);
		}

		// ��������Ҽ�˫��DUI��Ϣ
		if(m_bDuiMsgMouseRDblClk && m_rc.PtInRect(point))
		{
			SendMessage(MSG_MOUSE_RDBLCLK, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlRButtonDblClk(nFlags, point);
	}

	return false;
}

// �����¼�����
BOOL CControlBase::OnScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;
	
	BOOL bRresponse = false;
	if(m_pControl)
	{
		// �жϵ�ǰ��ؼ�
		if(m_pControl->PtInRect(point) && m_pControl->OnCheckMouseResponse(nFlags, point))
		{
			if(m_pControl->OnScroll(bVertical, nFlags, point))
			{
				return true;
			}
			return false;
		}
	}

	if(PtInRect(point))
	{
		// �ڴ˿ؼ���Χ��,���жϴ˿ؼ��Ƿ��ܴ���
		if(OnControlScroll(bVertical, nFlags, point))
		{
			return true;
		}else
		{
			// �˿ؼ�û�д���,������ӿؼ����Ƿ��ܴ���
			for (size_t i = 0; i < m_vecControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecControl.at(i);
				if (pControlBase && pControlBase->PtInRect(point))
				{
					if(pControlBase->OnScroll(bVertical, nFlags, point))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

// �����¼�����
BOOL CControlBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!m_bIsVisible || !m_bRresponse) return false;
	
	BOOL bRresponse = false;
	// �жϵ�ǰ��ؼ�
	if(m_pControl && m_pControl->OnKeyDown(nChar, nRepCnt, nFlags))
	{
		return true;
	}

	// �ؼ������Ƿ���Դ�����¼�
	if(OnControlKeyDown(nChar, nRepCnt, nFlags))
	{
		// ���ͼ��̰���DUI��Ϣ
		if(m_bDuiMsgKeyDown)
		{
			SendMessage(MSG_KEY_DOWN, (WPARAM)nChar, (LPARAM)nFlags);
		}
		return true;
	}

	// �˿ؼ�û�д���,������ӿؼ����Ƿ��ܴ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->OnKeyDown(nChar, nRepCnt, nFlags))
		{
			// ���ͼ��̰���DUI��Ϣ
			if(m_bDuiMsgKeyDown)
			{
				SendMessage(MSG_KEY_DOWN, (WPARAM)nChar, (LPARAM)nFlags);
			}
			return true;
		}
	}

	// ���ͼ��̰���DUI��Ϣ
	if(m_bDuiMsgKeyDown)
	{
		SendMessage(MSG_KEY_DOWN, (WPARAM)nChar, (LPARAM)nFlags);
	}

	return false;
}

// �����¼�����
BOOL CControlBase::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// �����ݼ��ܹ�ƥ��ؼ��Ŀ�ݼ�,����һ��ģ�����������Ϣ
	if((m_nShortcutKey != 0) && (nChar == m_nShortcutKey) && (nFlags == m_nShortcutFlag))
	{
		SendMessage(MSG_BUTTON_DOWN, 0, 0);
		SendMessage(MSG_BUTTON_UP, 0, 0);
		return true;
	}

	return false;
}

// ���ÿؼ���DUI��Ϣ���ͱ�ʶ
BOOL CControlBase::OnControlSetDuiMsg(LPCTSTR lpszDuiMsg)
{
	CString strDuiMsg = lpszDuiMsg;
	if(strDuiMsg == _T("mousemove"))	// ��������ƶ���DUI��Ϣ
	{
		m_bDuiMsgMouseMove = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouseldown"))	// �������������µ�DUI��Ϣ
	{
		m_bDuiMsgMouseLDown = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouselup"))	// �����������ſ���DUI��Ϣ
	{
		m_bDuiMsgMouseLUp = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouseldblclk"))	// ����������˫����DUI��Ϣ
	{
		m_bDuiMsgMouseLDblClk = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouserdown"))	// ��������Ҽ����µ�DUI��Ϣ
	{
		m_bDuiMsgMouseRDown = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouserup"))	// ��������Ҽ��ſ���DUI��Ϣ
	{
		m_bDuiMsgMouseRUp = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouserdblclk"))	// ��������Ҽ�˫����DUI��Ϣ
	{
		m_bDuiMsgMouseRDblClk = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("keydown"))		// ���ͼ��̰���DUI��Ϣ
	{
		m_bDuiMsgKeyDown = TRUE;
		return TRUE;
	}

	return FALSE;
}

// �����ק�ļ��¼�����
BOOL CControlBase::OnControlDropFile(CPoint point, CString strFileName)
{
	if(!m_bIsVisible || !m_bRresponse) return false;
	
	BOOL bRresponse = false;
	if(m_pControl)
	{
		// �жϵ�ǰ��ؼ�
		if(m_pControl->PtInRect(point))
		{
			return m_pControl->OnControlDropFile(point, strFileName);
		}
	}

	if(PtInRect(point) && GetDropFileEnable())
	{
		// �ڴ˿ؼ���Χ��,���ҿؼ�������������ק�ļ��ı�ʶ,������Ϣ
		SendMessage(MSG_DROP_FILE, (WPARAM)(&point), (LPARAM)(&strFileName));
		return true;
	}

	return false;
}

BOOL CControlBase::OnTimer()
{
	BOOL bRresponse = false;
	CRect rcnControlUpdate;

	bRresponse = OnControlTimer();

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			rcnControlUpdate.SetRectEmpty();
			if(pControlBase->OnTimer())
			{
				bRresponse = true;
			}
		}
	}

	return bRresponse;
}

void CControlBase::SetRect(CRect rc)
{ 
	CRect rcAll = m_rc | rc;
	SetControlRect(rc);
	UpdateControl(rcAll, m_bIsVisible, true);
}

// �ؼ���λ����Ϣ��Ҫˢ��
void CControlBase::OnPositionChange()
{
	// ˢ�¿ؼ������λ��
	OnAttributePosChange(m_strPos, FALSE);

	// ˢ���ӿؼ���λ��
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->OnPositionChange();
		}
	}
}

// ����λ����Ϣ�ľ�������ֵ
int CControlBase::PositionItem2Value( const DUI_POSITION_ITEM &pos ,int nMin, int nMax)
{
	int nRet=0;
	int nWid=nMax-nMin;

	switch(pos.pit)
	{
	case PIT_CENTER: 
		nRet=(int)pos.nPos * (pos.bMinus?-1:1) + nWid/2 + nMin;
		break;
	case PIT_NORMAL: 
		if(pos.bMinus)
			nRet=nMax-(int)pos.nPos;
		else
			nRet=nMin+(int)pos.nPos;
		break;
	case PIT_PERCENT: 
		nRet=nMin+(int)(pos.nPos*nWid/100);
		if(nRet>nMax) nRet=nMax;
		break;
	}
	
	return nRet;

}

// ����λ����Ϣ
LPCTSTR CControlBase::ParsePosition(LPCTSTR pszPos,DUI_POSITION_ITEM &pos)
{
	if(!pszPos) return NULL;

	// ���˵�ǰ��Ŀո�
	while(pszPos[0]==_T(' '))
	{
		pszPos++;
	}

	if(pszPos[0]==_T('|'))	// ���м俪ʼ����
	{
		pos.pit=PIT_CENTER;
		pszPos++;
	}else
	if(pszPos[0]==_T('%'))	// ���հٷֱȼ���
	{
		pos.pit=PIT_PERCENT;
		pszPos++;
	}else					// ��ͨ�ļ��㷽ʽ
	{
		pos.pit=PIT_NORMAL;
	}
	
	if(pszPos[0]==_T('-'))	// ����
	{
		pos.bMinus=TRUE;
		pszPos++;
	}else
	{
		pos.bMinus=FALSE;
	}

	pos.nPos=(float)_tstof(pszPos);

	LPCTSTR pNext=StrChr(pszPos,_T(','));
	if(pNext)
	{
		pNext++;
	}

	return pNext;
}

// ����λ����Ϣ(ָ��������λ��)
BOOL CControlBase::SetPositionWithParent(CRect rectParent)
{
	if(m_strPos.IsEmpty())
	{
		return FALSE;
	}

	DUI_POSITION pos;

	pos.nCount = 0;
	LPCTSTR pszValue = m_strPos;
	while(pos.nCount < 4 && pszValue)
	{
		pszValue=ParsePosition(pszValue, pos.Item[pos.nCount++]);
	}

    if (2 == pos.nCount || 4 == pos.nCount)
    {
		if(4 == pos.nCount)
		{
			CRect rect;
			rect.left = PositionItem2Value(pos.Left, rectParent.left, rectParent.right);
			rect.top = PositionItem2Value(pos.Top, rectParent.top, rectParent.bottom);
			rect.right = PositionItem2Value(pos.Right, rectParent.left, rectParent.right);
			rect.bottom = PositionItem2Value(pos.Bottom, rectParent.top, rectParent.bottom);
			SetRect(rect);
		}else
		if(2 == pos.nCount)
		{
			CRect rect;
			rect.left = PositionItem2Value(pos.Left, rectParent.left, rectParent.right);
			rect.top = PositionItem2Value(pos.Top, rectParent.top, rectParent.bottom);
			if(m_nWidth != 0)
			{
				rect.right = rect.left + m_nWidth;
			}else
			{
				rect.right = PositionItem2Value(pos.Left, rectParent.left, rectParent.right);
			}
			if(m_nHeight != 0)
			{
				rect.bottom = rect.top + m_nHeight;
			}else
			{
				rect.bottom = PositionItem2Value(pos.Top, rectParent.top, rectParent.bottom);
			}
			SetRect(rect);
		}
    }else
	{
        pos.nCount = 0;
		return FALSE;
	}

	return TRUE;
}

// ��XML����λ����Ϣ����
HRESULT CControlBase::OnAttributePosChange(const CString& strValue, BOOL bLoading)
{
    if (strValue.IsEmpty()) return E_FAIL;

	m_strPos = strValue;

	DUI_POSITION pos;

	pos.nCount = 0;
	LPCTSTR pszValue = strValue;
	while(pos.nCount < 4 && pszValue)
	{
		pszValue=ParsePosition(pszValue, pos.Item[pos.nCount++]);
	}

    if (2 == pos.nCount || 4 == pos.nCount)
    {
		if(4 == pos.nCount)
		{
			CRect rectParent = CRect(0,0,0,0);
			CDuiObject* pParent = GetParent();
			if(pParent)
			{
				rectParent = pParent->GetRect();
			}
			CRect rect;
			rect.left = PositionItem2Value(pos.Left, rectParent.left, rectParent.right);
			rect.top = PositionItem2Value(pos.Top, rectParent.top, rectParent.bottom);
			rect.right = PositionItem2Value(pos.Right, rectParent.left, rectParent.right);
			rect.bottom = PositionItem2Value(pos.Bottom, rectParent.top, rectParent.bottom);
			SetRect(rect);
		}else
		if(2 == pos.nCount)
		{
			//m_uPositionType = (m_uPositionType & ~SizeX_Mask) | SizeX_FitContent;
			//m_uPositionType = (m_uPositionType & ~SizeY_Mask) | SizeY_FitContent;
			CRect rectParent = CRect(0,0,0,0);
			CDuiObject* pParent = GetParent();
			if(pParent)
			{
				rectParent = pParent->GetRect();
			}
			CRect rect;
			rect.left = PositionItem2Value(pos.Left, rectParent.left, rectParent.right);
			rect.top = PositionItem2Value(pos.Top, rectParent.top, rectParent.bottom);
			if(m_nWidth != 0)
			{
				rect.right = rect.left + m_nWidth;
			}else
			{
				rect.right = PositionItem2Value(pos.Left, rectParent.left, rectParent.right);
			}
			if(m_nHeight != 0)
			{
				rect.bottom = rect.top + m_nHeight;
			}else
			{
				rect.bottom = PositionItem2Value(pos.Top, rectParent.top, rectParent.bottom);
			}
			SetRect(rect);
		}
    }else
	{
        pos.nCount = 0;
	}

    return bLoading?S_FALSE:S_OK;
}

// ��XML���ÿ����Ϣ����
HRESULT CControlBase::OnAttributeWidth(const CString& strValue, BOOL bLoading)
{
    if (strValue.IsEmpty()) return E_FAIL;

	m_nWidth = _ttoi(strValue);
	m_rc.right = m_rc.left + m_nWidth;
	SetRect(m_rc);

	return bLoading?S_FALSE:S_OK;
}

// ��XML���ø߶���Ϣ����
HRESULT CControlBase::OnAttributeHeight(const CString& strValue, BOOL bLoading)
{
    if (strValue.IsEmpty()) return E_FAIL;

	m_nHeight = _ttoi(strValue);
	m_rc.bottom = m_rc.top + m_nHeight;
	SetRect(m_rc);

	return bLoading?S_FALSE:S_OK;
}

// ��XML���ò˵�λ����Ϣ����
HRESULT CControlBase::OnAttributeMenuPosChange(const CString& strValue, BOOL bLoading)
{
    if (strValue.IsEmpty()) return E_FAIL;

	m_posMenu.nCount=0;
	LPCTSTR pszValue=strValue;
	while(m_posMenu.nCount<4 && pszValue)
	{
		pszValue=ParsePosition(pszValue,m_posMenu.Item[m_posMenu.nCount++]);
	}

    return bLoading?S_FALSE:S_OK;
}

// ��XML���ÿ�ݼ���Ϣ����
HRESULT CControlBase::OnAttributeShortcut(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	CDuiObject::ParseKeyCode(strValue, m_nShortcutKey, m_nShortcutFlag);

	return bLoading?S_FALSE:S_OK;
}

// ��XML���ý�������
HRESULT CControlBase::OnAttributeDisable(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	SetControlDisable(::StrToInt(strValue) > 0 ? true : false);

	return bLoading?S_FALSE:S_OK;
}

// ��XML�������������
HRESULT CControlBase::OnAttributeCursor(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	if(strValue == _T("arrow"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));	// ��ͷ
	}else
	if(strValue == _T("wait"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT));	// ɳ©�ȴ�
	}else
	if(strValue == _T("cross"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_CROSS));	// ʮ��
	}else
	if(strValue == _T("sizewe"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE));	// ˫��ͷָ����
	}else
	if(strValue == _T("sizens"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS));	// ˫��ͷָ���ϱ�
	}else
	if(strValue == _T("hand"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_HAND));	// ����
	}else
	if(strValue == _T("help"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_HELP));	// ��ͷ+�ʺ�
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML�����Ƿ���ָ����DUI��Ϣ������
HRESULT CControlBase::OnAttributeSendDuiMsg(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ������Ҫ���͵�DUI��Ϣ�б�
	CStringArray asDuiMsg;
	CString strDuiMsg = strValue;
	int nPos = -1;
	while((nPos = strDuiMsg.Find(_T("|"))) != -1)
	{
		CString strTemp = strDuiMsg.Left(nPos);
		strDuiMsg.Delete(0, nPos+1);
		asDuiMsg.Add(strTemp);
	}
	if(!strDuiMsg.IsEmpty())
	{
		asDuiMsg.Add(strDuiMsg);
	}

	for(int i=0; i<asDuiMsg.GetSize(); i++)
	{
		OnControlSetDuiMsg(asDuiMsg[i]);
	}

	return bLoading?S_FALSE:S_OK;
}

// ���ÿؼ��Ƿ�ɼ�
void CControlBase::SetVisible(BOOL bIsVisible)
{
	SetControlVisible(bIsVisible);
	UpdateControl(true, true);
}

// ��ȡ�ؼ��Ŀɼ���(�������ؼ�,������ؼ����ɼ�,�򷵻ز��ɼ�)
BOOL CControlBase::IsControlVisible()
{
	if(!m_bIsVisible || m_bIsHide)
	{
		return FALSE;
	}

	CDuiObject* pParentObj = GetParent();
	if(pParentObj == NULL)
	{
		return TRUE;
	}

	if(pParentObj->IsClass(_T("dlg")) || pParentObj->IsClass(_T("popup")))
	{
		return TRUE;
	}

	return ((CControlBase*)pParentObj)->IsControlVisible();
}

// ���ÿؼ��Ƿ�����
void CControlBase::SetHide(BOOL bIsHide)
{
	SetControlHide(bIsHide);
	UpdateControl(true, true);
}

// ���ÿؼ��Ƿ����
void  CControlBase::SetDisable(BOOL bIsDisable)
{
	if(m_bIsDisable != bIsDisable)
	{
		SetControlDisable(bIsDisable);
		UpdateControl(true);
	}
}

// ˢ�¿ؼ�����ʾ����
void CControlBase::UpdateControl(BOOL bUpdate, BOOL bVisible)
{
	if((m_bIsVisible || bVisible) && (m_pParentDuiObject != NULL))
	{
		m_pParentDuiObject->OnControlUpdate(m_rc, bUpdate, this);
	}
}

void CControlBase::UpdateControl(CRect rc, BOOL bVisible, BOOL bUpdate)
{
	if((m_bIsVisible || bVisible) && (m_pParentDuiObject != NULL))
	{
		m_pParentDuiObject->OnControlUpdate(rc, bUpdate, this);
	}
}

// �ؼ��ֲ�ˢ��
void CControlBase::InvalidateRect(LPCRECT lpRect, BOOL bErase)
{
	if(m_hWnd != NULL)
	{
		::InvalidateRect(m_hWnd, lpRect, bErase);
		return;
	}

	CDuiObject* pParentObj = GetParent();
	while((pParentObj != NULL) && (!pParentObj->IsClass(_T("dlg"))) && (!pParentObj->IsClass(_T("popup"))))
	{
		pParentObj = ((CControlBase*)pParentObj)->GetParent();
	}

	if((pParentObj != NULL) && pParentObj->IsClass(_T("dlg")))
	{
		((CDlgBase*)pParentObj)->InvalidateRect(lpRect, bErase);
	}else
	if((pParentObj != NULL) && pParentObj->IsClass(_T("popup")))
	{
		//((CDlgPopup*)pParentObj)->InvalidateRect(lpRect, bErase);
	}
}

// ��ȡ�ӿؼ�����
CControlBase *CControlBase::GetControl(UINT uControlID)
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
CControlBase *CControlBase::GetControl(CString strControlName)
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

// ����ӿؼ�
void CControlBase::AddControl(CControlBase* pControl)
{
	if(pControl != NULL)
	{
		m_vecControl.push_back(pControl);
	}
}

// ���ָ�����ӿؼ�
BOOL CControlBase::RemoveControl(CControlBase* pControl)
{
	vector<CControlBase*>::iterator it;
	for(it=m_vecControl.begin();it!=m_vecControl.end();++it)
	{
		CControlBase* pControlBase = *it;
		if (pControlBase == pControl)
		{
			// ����ǽ���ؼ�,����Ҫ�Ƚ���������Ϊ��
			if(pControlBase->IsFocusControl())
			{
				CDlgBase* pDlg = GetParentDialog(FALSE);
				if(pDlg)
				{
					pDlg->SetFocusControlPtr(NULL);
				}
			}
			m_vecControl.erase(it);
			delete pControlBase;
			return TRUE;
		}
	}

	return FALSE;
}

// ���ָ�����ֵ��ӿؼ�
BOOL CControlBase::RemoveControl(CString strControlName, UINT uControlID)
{
	vector<CControlBase*>::iterator it;
	for(it=m_vecControl.begin();it!=m_vecControl.end();++it)
	{
		CControlBase* pControlBase = *it;
		if (pControlBase && pControlBase->IsThisObject(uControlID, strControlName))
		{
			// ����ǽ���ؼ�,����Ҫ�Ƚ���������Ϊ��
			if(pControlBase->IsFocusControl())
			{
				CDlgBase* pDlg = GetParentDialog(FALSE);
				if(pDlg)
				{
					pDlg->SetFocusControlPtr(NULL);
				}
			}
			m_vecControl.erase(it);
			delete pControlBase;
			return TRUE;
		}
	}

	return FALSE;
}

// ���ָ�����͵��ӿؼ�
void CControlBase::RemoveControls(CString strClassName)
{
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if(pControlBase->IsClass(strClassName))
		{
			RemoveControl(pControlBase);
		}
	}
}

// ��������ӿؼ�
void CControlBase::RemoveControls()
{
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		RemoveControl(pControlBase);
	}
}

// ��ȡ���Ի���
CDlgBase* CControlBase::GetParentDialog(BOOL bEnablePopup)
{
	CDuiObject* pParentObj = GetParent();
	while((pParentObj != NULL) && (!pParentObj->IsClass(_T("dlg"))))
	{
		if(pParentObj->IsClass(_T("popup")))
		{
			if(!bEnablePopup)	// ���������Popup���ڵĿؼ���ȡ���Ի���,��ֱ�ӷ��ؿ�
			{
				return NULL;
			}
			pParentObj = ((CDlgPopup*)pParentObj)->GetParent();
		}else
		{
			pParentObj = ((CControlBase*)pParentObj)->GetParent();
		}
	}

	if((pParentObj != NULL) && pParentObj->IsClass(_T("dlg")))
	{
		return (CDlgBase*)pParentObj;
	}

	return NULL;
}

// ��ȡ���ؼ��Ĳ���������ڹ��ܽӿ�(������ڲ����)
IDuiHostWnd* CControlBase::GetParentIDuiHostWnd()
{
	CDuiObject* pParentObj = GetParent();
	while(pParentObj != NULL)
	{
		if(pParentObj->IsClass(_T("div")) && (((CDuiPanel*)pParentObj)->GetIDuiHostWnd() != NULL))
		{
			return ((CDuiPanel*)pParentObj)->GetIDuiHostWnd();
		}

		if(pParentObj->IsClass(_T("dlg")))
		{
			pParentObj = ((CDlgBase*)pParentObj)->GetParent();
		}else
		if(pParentObj->IsClass(_T("popup")))
		{
			pParentObj = ((CDlgPopup*)pParentObj)->GetParent();
		}else
		{
			pParentObj = ((CControlBase*)pParentObj)->GetParent();
		}
	}

	return NULL;
}

// ��Ϣ����
LRESULT CControlBase::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_bTaskMsg)
	{
		// �������������ʽ����Ϣ������,�����һ��������Ϣ
		CString strControlName = GetName();
		CString strAction = GetAction();
		CDlgBase* pParentDlg = GetParentDialog();
		DuiSystem::Instance()->AddDuiActionTask(uID, uMsg, wParam, lParam, strControlName, strAction, pParentDlg);
		return 0;
	}

	if(m_strAction.Find(_T("dlg:")) == 0)	// ����:��һ���Ի���,���ڴ�й©����Ϊͨ��DuiSystem�����͹���
	{
		if(uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
		{
			CString strXmlFile = m_strAction;
			strXmlFile.Delete(0, 4);
			DuiSystem::ShowDuiDialog(strXmlFile, GetParentDialog());
		}
	}else
	if(m_strAction.Find(_T("popup:")) == 0)	// ����:��һ��Popup�Ի���
	{
		if(uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
		{
			/*UINT nIDTemplate = 0;
			CDlgBase* pParentDlg = GetParentDialog();
			if(pParentDlg != NULL)
			{
				nIDTemplate = pParentDlg->GetIDTemplate();
			}
			CDlgPopup* pPopup = new CDlgPopup;
			pPopup->SetParent(this);
			CString strXmlFile = m_strAction;
			strXmlFile.Delete(0, 6);
			pPopup->SetXmlFile(_T("xml:") +strXmlFile );
			CRect rc = pControlBase->GetRect();
			rc.OffsetRect(-95, rc.Height());
			ClientToScreen(&rc);
			pPopup->Create(this, rc, WM_SKIN);
			pPopup->ShowWindow(SW_SHOW);*/
		}
	}else
	if(m_strAction.Find(_T("menu:")) == 0)	// ����:��һ���˵�
	{
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);	// ���Կ��Ǹ�Ϊͨ��DuiSystem�����͹���
		pDuiMenu->SetAutoClose(FALSE);
		pDuiMenu->SetParent(this);
		CPoint point;
		CRect rc = GetRect();
		// ����˵�����ʾλ��
		if(m_posMenu.nCount >= 2)
		{
			// ʹ�����õĲ˵�λ��
			int nMenuX = PositionItem2Value(m_posMenu.Left, rc.left, rc.right);
			int nMenuY = PositionItem2Value(m_posMenu.Top, rc.top, rc.bottom);
			point.SetPoint(nMenuX, nMenuY);
		}else
		{
			// ���û������λ����Ϣ,��Ĭ�ϰ��տؼ��ײ���ʼ��ʾ,ˮƽ�����м����
			point.SetPoint(rc.left + rc.Width() / 2, rc.bottom);
		}

		// ����ת��Ϊ��Ļ����
		ClientToScreen(&point);

		CDlgBase* pParentDlg = GetParentDialog();
		CString strXmlFile = m_strAction;
		strXmlFile.Delete(0, 5);
		if(pDuiMenu->LoadXmlFile(strXmlFile, pParentDlg, point, WM_DUI_MENU))
		{
			if(m_posMenu.nCount >= 4)
			{
				// ����˵�pos��4��ֵ,������ֵΪ������ʾ�˵��������ʾ,���ĸ�ֵΪ������ʾ�˵�������ʾ
				CRect rcMenu;
				pDuiMenu->GetWindowRect(&rcMenu);
				int nOffsetX = 0;
				int nOffsetY = 0;
				if(m_posMenu.Right.bMinus)
				{
					nOffsetX = -rcMenu.Width();
				}
				if(m_posMenu.Bottom.bMinus)
				{
					nOffsetY = -rcMenu.Height();
				}
				rcMenu.OffsetRect(nOffsetX, nOffsetY);
				pDuiMenu->MoveWindow(rcMenu);
			}
			pDuiMenu->ShowWindow(SW_SHOW);
			pDuiMenu->SetAutoClose(TRUE);
		}
	}else
	if(m_strAction.Find(_T("link:")) == 0)	// ����:��һ��ҳ������
	{
		if(uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
		{
			CString strLink = m_strAction;
			strLink.Delete(0, 5);
			if(!strLink.IsEmpty())
			{
				ShellExecute(NULL, TEXT("open"), strLink, NULL,NULL,SW_NORMAL);
			}
		}
	}else
	if(m_strAction.Find(_T("run:")) == 0)	// ����:ִ��һ������
	{
		if(uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
		{
			CString strProcess = m_strAction;
			strProcess.Delete(0, 4);
			strProcess.MakeLower();
			if(!strProcess.IsEmpty())
			{
				strProcess.MakeLower();
				BOOL bForceAdmin = FALSE;
				if(strProcess.Find(_T("admin@")) == 0)
				{
					bForceAdmin = TRUE;
					strProcess.Delete(0, 6);
				}
				BOOL bWaitProcess = FALSE;
				if(strProcess.Find(_T("&")) == (strProcess.GetLength()-1))
				{
					bWaitProcess = TRUE;
					strProcess.Delete(strProcess.GetLength()-1, 1);
				}
				if(strProcess.Find(_T(".exe")) == -1)
				{
					strProcess = DuiSystem::Instance()->GetString(strProcess);
				}
				if(strProcess.Find(_T("{platpath}")) == 0)
				{
					strProcess.Delete(0, 10);
					strProcess = DuiSystem::GetExePath() + strProcess;
				}
				CString strCmdLine = _T("");
				int nPos = strProcess.Find(_T("|"));
				if(nPos != -1)
				{
					strCmdLine = strProcess;
					strCmdLine.Delete(0, nPos+1);
					strProcess = strProcess.Left(nPos);
				}
				DuiSystem::PathCanonicalize(strProcess);	// ·����׼��
				DuiSystem::ExecuteProcess(strProcess, strCmdLine, bForceAdmin, bWaitProcess);
			}
		}
	}else
	if(m_strAction.Find(ACTION_CLOSE_WINDOW) == 0)	// ����:�ر�ָ���Ĵ���
	{
		if(uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
		{
			CString strWndName = m_strAction;
			strWndName.Delete(0, 13);
			if(!strWndName.IsEmpty())
			{
				CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(strWndName);
				if(pDlg != NULL)
				{
					//pDlg->DoClose();
					pDlg->PostMessage(WM_QUIT, 0, 0);
				}
			}
		}
	}else
	{
		// �����ж�����Ǽ���Ĭ�ϰ�ť,��ֱ������Ӧ�Ĵ���
		CDlgBase* pParentDlg = GetParentDialog();
		if(IsThisObject(BT_OK, NAME_BT_OK))
		{
			if((MSG_BUTTON_UP == uMsg) && (pParentDlg != NULL)) { pParentDlg->DoOK(); }
		}else
		if(IsThisObject(BT_CANCEL, NAME_BT_CANCEL))
		{
			if((MSG_BUTTON_UP == uMsg) && (pParentDlg != NULL)) { pParentDlg->DoCancel(); }
		}else
		if(IsThisObject(BT_YES, NAME_BT_YES))
		{
			if((MSG_BUTTON_UP == uMsg) && (pParentDlg != NULL)) { pParentDlg->DoYes(); }
		}else
		if(IsThisObject(BT_NO, NAME_BT_NO))
		{
			if((MSG_BUTTON_UP == uMsg) && (pParentDlg != NULL)) { pParentDlg->DoNo(); }
		}else
		{
			// ���ÿؼ���DUI�¼��������
			return CallDuiHandler(uMsg, wParam, lParam);
		}
	}

	return 0;
}

// ����DUI�¼��������
LRESULT CControlBase::CallDuiHandler(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDuiHandler* pDuiHandler = GetDuiHandler();
	if(pDuiHandler != NULL)
	{
		return pDuiHandler->OnDuiMessage(GetID(), GetName(), uMsg, wParam, lParam);
	}

	CDuiObject* pParentObj = GetParent();
	while(pParentObj != NULL)
	{
		CDuiHandler* pDuiHandler = pParentObj->GetDuiHandler();
		if(pDuiHandler != NULL)
		{
			return pDuiHandler->OnDuiMessage(GetID(), GetName(), uMsg, wParam, lParam);
		}

		if(pParentObj->IsClass(_T("popup")))
		{
			pParentObj = ((CDlgPopup*)pParentObj)->GetParent();
		}else
		if(pParentObj->IsClass(_T("dlg")))	// ����ǶԻ�����ʱ�սᣬ�����Ҹ�����
		{
			pParentObj = ((CDlgBase*)pParentObj)->GetParent();
		}else
		{
			pParentObj = ((CControlBase*)pParentObj)->GetParent();
		}
	}

	// ���δ�ҵ��¼��������,��ͨ��DuiSystem��������ע����¼����������д���
	return DuiSystem::Instance()->CallDuiHandler(GetID(), GetName(), uMsg, wParam, lParam);
}

// ����֪ͨ��Ϣ
LRESULT CControlBase::SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//return ::SendMessage(m_hWnd, Msg, wParam, lParam);
	return m_pParentDuiObject->OnBaseMessage(m_uID, Msg, wParam, lParam);
}

// �򿪵����Ի���
void CControlBase::OpenDlgPopup(CDlgPopup *pWndPopup, CRect rc, UINT uMessageID, BOOL bShow)
{
	ASSERT(pWndPopup);
	CloseDlgPopup();
	CPoint point;
	point.SetPoint(rc.left, rc.top);
	::ClientToScreen(GetHWND(), &point);
	rc.OffsetRect(point.x-rc.left, point.y-rc.top);
	m_pWndPopup = pWndPopup;
	m_pWndPopup->Create(CWnd::FromHandle(GetHWND()), rc, uMessageID);
	if(bShow)
	{
		m_pWndPopup->ShowWindow(SW_SHOW);
	}
}

// �رյ����Ի���
void CControlBase::CloseDlgPopup()
{
	if(m_pWndPopup)
	{
		if(IsWindow(m_pWndPopup->GetSafeHwnd()))
		{
			m_pWndPopup->CloseWindow();			
		}
	}
	m_pWndPopup = NULL;
}


////////////////////////////////////////////////////////////////////////
// CControlBaseFont

CControlBaseFont::CControlBaseFont(HWND hWnd, CDuiObject* pDuiObject)
		: CControlBase(hWnd, pDuiObject)								  
{
	m_strTitle = _T("");
	m_strFont = DuiSystem::GetDefaultFont();
	m_nFontWidth = 12;
	m_fontStyle = FontStyleRegular;
	m_uAlignment = Align_Left;
	m_uVAlignment = VAlign_Top;

	m_pImage = NULL;
	m_nImagePicCount = 4;
}

CControlBaseFont::CControlBaseFont(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, BOOL bIsVisible/* = TRUE*/, BOOL bIsDisable/* = FALSE*/ , BOOL bRresponse/* = TRUE*/,
								   CString strFont, int nFontWidth/* = 12*/, FontStyle fontStyle/* = FontStyleRegular*/)
								   : CControlBase(hWnd, pDuiObject, uControlID, rc, bIsVisible, bIsDisable, bRresponse)								  
{
	m_strTitle = strTitle;
	m_strFont = DuiSystem::GetDefaultFont(strFont);
	m_nFontWidth = nFontWidth;
	m_fontStyle = fontStyle;
	m_uAlignment = Align_Left;
	m_uVAlignment = VAlign_Top;

	m_pImage = NULL;
	m_nImagePicCount = 4;
}

CControlBaseFont::~CControlBaseFont(void)
{
	if(m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

// ����title
void CControlBaseFont::SetTitle(CString strTitle) 
{
	//if(strTitle != m_strTitle)
	{
		SetControlTitle(strTitle);
		UpdateControl(true);
	}
}

// ����ˮƽ���뷽ʽ
void CControlBaseFont::SetAlignment(UINT uAlignment)
{
	if(uAlignment != m_uAlignment)
	{
		m_uAlignment = uAlignment;
		UpdateControl(true);
	}
}

// ���ô�ֱ���뷽ʽ
void CControlBaseFont::SetVAlignment(UINT uVAlignment)
{
	if(uVAlignment != m_uVAlignment)
	{
		m_uVAlignment = uVAlignment;
		UpdateControl(true);
	}
}

// ���ö��뷽ʽ
void CControlBaseFont::SetAlignment(UINT uAlignment, UINT uVAlignment)
{
	if(uAlignment != m_uAlignment || uVAlignment != m_uVAlignment)
	{
		m_uAlignment = uAlignment;
		m_uVAlignment = uVAlignment;

		UpdateControl(true);
	}
}

// ת��ˮƽ���뷽ʽΪGDI���巽ʽ
UINT CControlBaseFont::GetGDIAlignment(UINT uAlignment)
{
	if(uAlignment == Align_Left)
	{
		return DT_LEFT;
	}else
	if(uAlignment == Align_Center)
	{
		return DT_CENTER;
	}else
	if(uAlignment == Align_Right)
	{
		return DT_RIGHT;
	}
	return DT_LEFT;
}

// ת����ֱ���뷽ʽΪGDI���巽ʽ
UINT CControlBaseFont::GetGDIVAlignment(UINT uVAlignment)
{
	if(uVAlignment == VAlign_Top)
	{
		return DT_TOP;
	}else
	if(uVAlignment == VAlign_Middle)
	{
		return DT_VCENTER;
	}else
	if(uVAlignment == VAlign_Bottom)
	{
		return DT_BOTTOM;
	}
	return DT_TOP;
}

// ��������
void CControlBaseFont::SetFont(CString strFont, int nFontWidth, FontStyle fontStyle)
{
	if(m_strFont != strFont || m_nFontWidth != nFontWidth || m_fontStyle != fontStyle)
	{
		m_strFont = DuiSystem::GetDefaultFont(strFont);
		m_nFontWidth = nFontWidth;
		m_fontStyle = fontStyle;
		UpdateControl(true);
	}
}

// ����ͼƬ
BOOL CControlBaseFont::SetBitmap(UINT nResourceID, CString strType)
{
	if(m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, m_pImage))
	{
		m_sizeImage.SetSize(m_pImage->GetWidth() / m_nImagePicCount, m_pImage->GetHeight());
		UpdateControl(true);
		return true;
	}
	return false;
}

// ����ͼƬ
BOOL CControlBaseFont::SetBitmap(CString strImage)
{
	if(m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, m_pImage))
	{
		m_sizeImage.SetSize(m_pImage->GetWidth() / m_nImagePicCount, m_pImage->GetHeight());
		UpdateControl(true);
		return true;
	}
	return false;
}

// ����Image��Ĭ�ϰ�����ͼƬ����
void CControlBaseFont::SetBitmapCount(int nCount)
{
	m_nImagePicCount = nCount;
}

// ����ͼƬ
BOOL CControlBaseFont::SetImage(CString strImage)
{
	if(strImage.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = strImage;
		if(strImage.Find(_T(":")) != -1)
		{
			strImgFile = strImage;
		}
		if(!SetBitmap(strImgFile))
		{
			return FALSE;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _ttoi(strImage);
		if(!SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!SetBitmap(nResourceID, TEXT("BMP")))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

// ��XML����ͼƬ��Ϣ����
HRESULT CControlBaseFont::OnAttributeImage(const CString& strValue, BOOL bLoading)
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

	if(!SetImage(strSkin))
	{
		return E_FAIL;
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML����Skin����
HRESULT CControlBaseFont::OnAttributeSkin(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	CString strSkin = DuiSystem::Instance()->GetSkin(strValue);
	if (strSkin.IsEmpty()) return E_FAIL;

	if(!SetImage(strSkin))
	{
		return E_FAIL;
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML����Font����
HRESULT CControlBaseFont::OnAttributeFont(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	DuiFontInfo fontInfo;
	BOOL bFindFont = DuiSystem::Instance()->GetFont(strValue, fontInfo);
	if (!bFindFont) return E_FAIL;

	m_strFont = fontInfo.strFont;
	m_nFontWidth = fontInfo.nFontWidth;	
	m_fontStyle = fontInfo.fontStyle;

	return bLoading?S_FALSE:S_OK;
}