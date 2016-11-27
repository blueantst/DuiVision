#include "StdAfx.h"
#include <mmsystem.h> 
#include "ControlBase.h"

#pragma comment(lib,"Winmm.lib")

#define	SCROLL_V	1	// 垂直滚动条控件ID
#define	SCROLL_H	2	// 水平滚动条控件ID

// 控件ID自动生成变量，控件ID从1000开始生成
static int g_nControlId = 1000;

CControlBase::CControlBase(HWND hWnd, CDuiObject* pDuiObject)
{
	m_pParentDuiObject = pDuiObject;
	m_hWnd = hWnd;
	m_hwndHost = NULL;	// 原生控件的宿主窗口
	m_uID = g_nControlId++;
	if(g_nControlId > 20000)	// 超过某个值之后重置
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
	m_bDuiMsgKeyUp = FALSE;
	m_bDuiMsgFocusChange = FALSE;
	m_bMouseLeave = TRUE;
}

CControlBase::CControlBase(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible, BOOL bIsDisable,
						   BOOL bRresponse)
{
	m_pParentDuiObject = pDuiObject;
	m_hWnd = hWnd;
	m_hwndHost = NULL;	// 原生控件的宿主窗口
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
	m_bDuiMsgKeyDown = FALSE;
	m_bDuiMsgKeyUp = FALSE;
	m_bDuiMsgFocusChange = FALSE;
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
	
	// 释放内存DC
	if(m_bitmap.m_hObject)
	{
		if(m_memDC.m_hDC)
		{
			m_memDC.SelectObject(m_pOldBitmap);
			m_memDC.DeleteDC();
		}
		m_bitmap.DeleteObject();
	}

	// 释放动画DC
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

// 判断当前是否在主线程
void CControlBase::TestMainThread()
{
	// 调用所在对话框的测试函数
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

		// 画子区域
		for (size_t i = 0; i < m_vecArea.size(); i++)
		{
			CControlBase * pControlBase = m_vecArea.at(i);
			if (pControlBase)
			{
				pControlBase->Draw(dc, rcUpdate);
			}
		}

		// 画子控件
		DrawSubControls(dc, rcUpdate);
	}
}

// 画子控件
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

// 更新内存DC
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

// 更新动画DC
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

// 设置控件的父窗口句柄(包含子控件的句柄)
void CControlBase::SetHWND(HWND hWnd)
{
	// 设置控件的m_hWnd
	m_hWnd = hWnd;

	// 设置子控件的m_hWnd
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetHWND(hWnd);		
		}
	}
}

// 获取控件的父窗口句柄
HWND CControlBase::GetPaintHWnd()
{
	// 如果找到父对话框,就用父对话框的HWnd
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		return pDlg->GetSafeHwnd();
	}

	// 如果找到插件HostWnd,则调用插件HostWnd接口的HWnd
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		return pIDuiHostWnd->GetPaintHWnd();
	}

    return NULL;
}

// 坐标转换为屏幕坐标
void CControlBase::ClientToScreen(LPPOINT lpPoint)
{
	// 如果找到父对话框,就用父对话框的转换函数
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClientToScreen(lpPoint);
		return;
	}

	// 如果找到插件HostWnd,则调用插件HostWnd接口的转换函数
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->ClientToScreen(lpPoint);
		return;
	}
}

// 设置焦点
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

// 设置控件焦点
BOOL CControlBase::OnFocus(BOOL bFocus)
{
	return SetControlFocus(bFocus);
}

// 设置控件焦点
BOOL CControlBase::SetControlFocus(BOOL bFocus)
{
	// 发送控件焦点变化的DUI消息
	if(m_bDuiMsgFocusChange)
	{
		// wParam表示控件获取焦点还是取消焦点状态
		SendMessage(MSG_FOCUS_CHANGE, (WPARAM)bFocus, (LPARAM)0);
	}

	// 如果焦点取消,则设置每个子控件的焦点状态,解决插件中的控件焦点无法取消的问题
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

// 判断当前控件是否焦点控件
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

// 获取可以成为焦点控件的控件(遍历子控件查询)
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
			// 查找子控件
			pControlBase = pControlBase->GetFocusControl(pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// 获取上一个可以获取焦点的子控件
CControlBase* CControlBase::GetPrevFocusableControl(CControlBase* pFocusControl)
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
		if (pControlBase && (pControlBase == pFocusControl))
		{
			bStartSearch = TRUE;
		}
	}

	// 再遍历子控件查找
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
			// 查找子控件
			pControlBase = pControlBase->GetPrevFocusableControl(pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// 获取下一个可以获取焦点的子控件
CControlBase* CControlBase::GetNextFocusableControl(CControlBase* pFocusControl)
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
		if (pControlBase && (pControlBase == pFocusControl))
		{
			bStartSearch = TRUE;
		}
	}

	// 再遍历子控件查找
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
			// 查找子控件
			pControlBase = pControlBase->GetNextFocusableControl(pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// 设置Tooltip
void CControlBase::SetTooltip(CControlBase* pControl, CString strTooltip, CRect rect, BOOL bControlWidth, int nTipWidth)
{
	// 如果找到了父对话框,则调用对话框的设置Tooltip函数
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->SetTooltip(pControl, strTooltip, rect, bControlWidth, nTipWidth);
		return;
	}

	// 如果找到父Popup窗口,则调用Popup窗口的设置Tooltip函数

	// 如果找到插件HostWnd,则调用插件HostWnd接口的设置Tooltip函数
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

// 清除Tooltip
void CControlBase::ClearTooltip()
{
	// 如果找到了父对话框,则调用对话框的清除Tooltip函数
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->ClearTooltip();
		return;
	}

	// 如果找到父Popup窗口,则调用Popup窗口的清除Tooltip函数

	// 如果找到插件HostWnd,则调用插件HostWnd接口的清除Tooltip函数
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->ClearTooltip();
		return;
	}
}

// 设置当前Tooltip控件ID
void CControlBase::SetTooltipCtrlID(int nTooltipCtrlID)
{
	// 如果找到了父对话框,则调用对话框的设置Tooltip控件ID函数
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		pDlg->SetTooltipCtrlID(nTooltipCtrlID);
		return;
	}

	// 如果找到父Popup窗口,则调用Popup窗口的设置Tooltip控件ID函数

	// 如果找到插件HostWnd,则调用插件HostWnd接口的设置Tooltip控件ID函数
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		pIDuiHostWnd->SetTooltipCtrlID(nTooltipCtrlID);
		return;
	}
}

// 获取当前Tooltip控件ID
int CControlBase::GetTooltipCtrlID()
{
	// 如果找到了父对话框,则调用对话框的获取Tooltip控件ID函数
	CDlgBase* pDlg = GetParentDialog();
	if(pDlg)
	{
		return pDlg->GetTooltipCtrlID();
	}

	// 如果找到父Popup窗口,则调用Popup窗口的获取Tooltip控件ID函数

	// 如果找到插件HostWnd,则调用插件HostWnd接口的获取Tooltip控件ID函数
	IDuiHostWnd* pIDuiHostWnd = GetParentIDuiHostWnd();
	if(pIDuiHostWnd)
	{
		return pIDuiHostWnd->GetTooltipCtrlID();
	}

	return 0;
}

// 鼠标移动事件处理
BOOL CControlBase::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;
	
	// 保存原始的鼠标位置,并进行位置变换
	CPoint oldPoint = point;
	OnMousePointChange(point);

	BOOL bRresponse = false;
	if(m_pControl)
	{
		CPoint pt = point;
		// 如果是控件内置滚动条子控件,则不进行位置变换,因为滚动条位置是不需要变换的
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
		// 如果当前控件有Tooltip,并且当前Tooltip设置的是其他控件,则设置Tooltip
		if(GetTooltipCtrlID() != GetID())
		{
			SetTooltip(this, m_strTooltip, m_rc, FALSE, m_nTipWidth);
			SetTooltipCtrlID(GetID());
		}
	}

	// 调用控件的设置鼠标光标函数
	OnControlSetCursor(nFlags, point);

	// 调用控件的鼠标移动函数
	bRresponse = OnControlMouseMove(nFlags, point);

	// 控件拖动的处理
	if(m_bDragEnable && m_bMouseDown)
	{
		CControlBase* pParentCtrl = (CControlBase*)GetParent();
		if(pParentCtrl)
		{
			CRect rcParent = pParentCtrl->GetRect();
			if(rcParent.PtInRect(point))
			{
				// 如果鼠标位置在父控件范围内则可以拖动
				// 获取控件的当前位置和鼠标当前位置与上一次位置的差值,将控件当前位置加上鼠标位置的差值
				CRect rc = GetRect();
				CSize offset = point - m_ptLastMousePosition;
				rc.OffsetRect(offset);
				SetRect(rc);
				// 刷新鼠标上一次位置的变量
				m_ptLastMousePosition = point;
				// 刷新控件
				UpdateControl(true);
			}
		}
	}

	// 发送鼠标移动的DUI消息
	if(m_bDuiMsgMouseMove)
	{
		if(PtInRect(point))
		{
			// 发送鼠标在控件范围内移动的消息
			SendMessage(MSG_MOUSE_MOVE, (WPARAM)nFlags, (LPARAM)(&point));
			m_bMouseLeave = FALSE;
		}else
		{
			if(!m_bMouseLeave)
			{
				// 发送鼠标离开控件的消息(离开之后仅发送一次离开消息)
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
				// 如果是控件内置滚动条子控件,则不进行位置变换,因为滚动条位置是不需要变换的
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

// 设置控件的鼠标光标
BOOL CControlBase::OnControlSetCursor(UINT nFlags, CPoint point)
{
	if((m_hCursor != NULL) && m_rc.PtInRect(point))
	{
		::SetCursor(m_hCursor);
		return true;
	}

	return false;
}

// 鼠标左键按下事件处理
BOOL CControlBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// 保存原始的鼠标位置,并进行位置变换
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = m_rc.PtInRect(point);
	if(m_bMouseDown)
	{
		m_ptLastMousePosition = point;
	}

	// 查找鼠标是否在某个内部控件位置,如果是的话就更新当前子控件(按照反向顺序查找,因为定义在后面的控件是优先级更高的)
	// 找到第一个符合条件的就结束查找
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
		// 如果是控件内置滚动条子控件,则不进行位置变换,因为滚动条位置是不需要变换的
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
		// 切换对话框中的当前焦点控件(暂不处理Popup窗口的切换)
		CDlgBase* pDlg = GetParentDialog(FALSE);
		if(pDlg)
		{
			pDlg->SetFocusControl(this);
		}

		// 发送鼠标左键按下DUI消息
		if(m_bDuiMsgMouseLDown && m_rc.PtInRect(point))
		{
			SendMessage(MSG_MOUSE_LDOWN, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlLButtonDown(nFlags, point);
	}

	return false;
}

// 鼠标左键放开事件处理
BOOL CControlBase::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// 保存原始的鼠标位置,并进行位置变换
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = false;
	if(m_pControl != NULL)
	{
		// 如果是控件内置滚动条子控件,则不进行位置变换,因为滚动条位置是不需要变换的
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
		// 发送鼠标左键放开DUI消息
		if(m_bDuiMsgMouseLUp)
		{
			SendMessage(MSG_MOUSE_LUP, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlLButtonUp(nFlags, point);
	}

	return false;
}

// 鼠标左键双击事件处理
BOOL CControlBase::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// 保存原始的鼠标位置,并进行位置变换
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = false;
	if(m_pControl != NULL)
	{
		// 如果是控件内置滚动条子控件,则不进行位置变换,因为滚动条位置是不需要变换的
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
			// 如果控件不允许双击,则调用单击处理函数
			return m_pControl->OnLButtonDown(nFlags, point);
		}
	}
	else
	{
		if(!GetDblClk())
		{
			// 如果控件不允许双击,则调用单击处理函数
			return OnControlLButtonDown(nFlags, point);
		}

		// 发送鼠标左键双击DUI消息
		if(m_bDuiMsgMouseLDblClk && m_rc.PtInRect(point))
		{
			SendMessage(MSG_BUTTON_DBLCLK, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlLButtonDblClk(nFlags, point);
	}

	return false;
}

// 鼠标右键按下事件处理
BOOL CControlBase::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// 保存原始的鼠标位置,并进行位置变换
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
		// 发送鼠标右键按下DUI消息
		if(m_bDuiMsgMouseRDown && m_rc.PtInRect(point))
		{
			SendMessage(MSG_MOUSE_RDOWN, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlRButtonDown(nFlags, point);
	}

	return false;
}

// 鼠标右键放开事件处理
BOOL CControlBase::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// 保存原始的鼠标位置,并进行位置变换
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = false;
	if(m_pControl != NULL)
	{
		// 如果是控件内置滚动条子控件,则不进行位置变换,因为滚动条位置是不需要变换的
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
		// 发送鼠标右键放开DUI消息
		if(m_bDuiMsgMouseRUp)
		{
			SendMessage(MSG_MOUSE_RUP, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlRButtonUp(nFlags, point);
	}

	return false;
}

// 鼠标右键双击事件处理
BOOL CControlBase::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;

	// 保存原始的鼠标位置,并进行位置变换
	CPoint oldPoint = point;
	OnMousePointChange(point);

	m_bMouseDown = false;
	if(m_pControl != NULL)
	{
		// 如果是控件内置滚动条子控件,则不进行位置变换,因为滚动条位置是不需要变换的
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
			// 如果控件不允许双击,则调用单击处理函数
			return m_pControl->OnRButtonDown(nFlags, point);
		}
	}
	else
	{
		if(!GetDblClk())
		{
			// 如果控件不允许双击,则调用单击处理函数
			return OnControlRButtonDown(nFlags, point);
		}

		// 发送鼠标右键双击DUI消息
		if(m_bDuiMsgMouseRDblClk && m_rc.PtInRect(point))
		{
			SendMessage(MSG_MOUSE_RDBLCLK, (WPARAM)nFlags, (LPARAM)(&point));
		}

		return OnControlRButtonDblClk(nFlags, point);
	}

	return false;
}

// 滚动事件处理
BOOL CControlBase::OnScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(!m_bIsVisible || !m_bRresponse) return false;
	
	BOOL bRresponse = false;
	if(m_pControl)
	{
		// 判断当前活动控件
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
		// 在此控件范围内,先判断此控件是否能处理
		if(OnControlScroll(bVertical, nFlags, point))
		{
			return true;
		}else
		{
			// 此控件没有处理,则遍历子控件看是否能处理
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

// 键盘按下事件处理
BOOL CControlBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!m_bIsVisible || !m_bRresponse) return false;
	
	BOOL bRresponse = false;
	// 判断当前活动控件
	if(m_pControl && m_pControl->OnKeyDown(nChar, nRepCnt, nFlags))
	{
		return true;
	}

	// 控件自身是否可以处理此事件
	if(OnControlKeyDown(nChar, nRepCnt, nFlags))
	{
		// 发送键盘按下DUI消息
		if(m_bDuiMsgKeyDown)
		{
			SendMessage(MSG_KEY_DOWN, (WPARAM)nChar, (LPARAM)nFlags);
		}
		return true;
	}

	// 此控件没有处理,则遍历子控件看是否能处理
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->OnKeyDown(nChar, nRepCnt, nFlags))
		{
			// 发送键盘按下DUI消息
			if(m_bDuiMsgKeyDown)
			{
				SendMessage(MSG_KEY_DOWN, (WPARAM)nChar, (LPARAM)nFlags);
			}
			return true;
		}
	}

	// 发送键盘按下DUI消息
	if(m_bDuiMsgKeyDown)
	{
		SendMessage(MSG_KEY_DOWN, (WPARAM)nChar, (LPARAM)nFlags);
	}

	return false;
}

// 键盘放开事件处理
BOOL CControlBase::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!m_bIsVisible || !m_bRresponse) return false;
	
	BOOL bRresponse = false;
	// 判断当前活动控件
	if(m_pControl && m_pControl->OnKeyUp(nChar, nRepCnt, nFlags))
	{
		return true;
	}

	// 控件自身是否可以处理此事件
	if(OnControlKeyUp(nChar, nRepCnt, nFlags))
	{
		// 发送键盘放开DUI消息
		if(m_bDuiMsgKeyUp)
		{
			SendMessage(MSG_KEY_UP, (WPARAM)nChar, (LPARAM)nFlags);
		}
		return true;
	}

	// 此控件没有处理,则遍历子控件看是否能处理
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->OnKeyUp(nChar, nRepCnt, nFlags))
		{
			// 发送键盘放开DUI消息
			if(m_bDuiMsgKeyUp)
			{
				SendMessage(MSG_KEY_UP, (WPARAM)nChar, (LPARAM)nFlags);
			}
			return true;
		}
	}

	// 发送键盘放开DUI消息
	if(m_bDuiMsgKeyUp)
	{
		SendMessage(MSG_KEY_UP, (WPARAM)nChar, (LPARAM)nFlags);
	}

	return false;
}

// 键盘按下事件处理
BOOL CControlBase::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 如果快捷键能够匹配控件的快捷键,则发送一个模拟鼠标点击的消息
	if((m_nShortcutKey != 0) && (nChar == m_nShortcutKey) && (nFlags == m_nShortcutFlag))
	{
		SendMessage(MSG_BUTTON_DOWN, 0, 0);
		SendMessage(MSG_BUTTON_UP, 0, 0);
		return true;
	}

	return false;
}

// 键盘放开事件处理
BOOL CControlBase::OnControlKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return false;
}

// 设置控件的DUI消息发送标识
BOOL CControlBase::OnControlSetDuiMsg(LPCTSTR lpszDuiMsg)
{
	CString strDuiMsg = lpszDuiMsg;
	if(strDuiMsg == _T("mousemove"))	// 发送鼠标移动的DUI消息
	{
		m_bDuiMsgMouseMove = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouseldown"))	// 发送鼠标左键按下的DUI消息
	{
		m_bDuiMsgMouseLDown = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouselup"))	// 发送鼠标左键放开的DUI消息
	{
		m_bDuiMsgMouseLUp = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouseldblclk"))	// 发送鼠标左键双击的DUI消息
	{
		m_bDuiMsgMouseLDblClk = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouserdown"))	// 发送鼠标右键按下的DUI消息
	{
		m_bDuiMsgMouseRDown = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouserup"))	// 发送鼠标右键放开的DUI消息
	{
		m_bDuiMsgMouseRUp = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("mouserdblclk"))	// 发送鼠标右键双击的DUI消息
	{
		m_bDuiMsgMouseRDblClk = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("keydown"))		// 发送键盘按下DUI消息
	{
		m_bDuiMsgKeyDown = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("keyup"))		// 发送键盘放开DUI消息
	{
		m_bDuiMsgKeyUp = TRUE;
		return TRUE;
	}else
	if(strDuiMsg == _T("focuschange"))		// 发送控件焦点状态变化的DUI消息
	{
		m_bDuiMsgFocusChange = TRUE;
		return TRUE;
	}

	return FALSE;
}

// 鼠标拖拽文件事件处理
BOOL CControlBase::OnControlDropFile(CPoint point, CString strFileName)
{
	if(!m_bIsVisible || !m_bRresponse) return false;
	
	BOOL bRresponse = false;
	if(m_pControl)
	{
		// 判断当前活动控件
		if(m_pControl->PtInRect(point))
		{
			return m_pControl->OnControlDropFile(point, strFileName);
		}
	}

	if(PtInRect(point) && GetDropFileEnable())
	{
		// 在此控件范围内,并且控件设置了允许拖拽文件的标识,则发送消息
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

// 控件的位置信息需要刷新
void CControlBase::OnPositionChange()
{
	// 刷新控件自身的位置
	OnAttributePosChange(m_strPos, FALSE);

	// 刷新子控件的位置
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->OnPositionChange();
		}
	}
}

// 计算位置信息的具体坐标值
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

// 解析位置信息
LPCTSTR CControlBase::ParsePosition(LPCTSTR pszPos,DUI_POSITION_ITEM &pos)
{
	if(!pszPos) return NULL;

	// 过滤掉前面的空格
	while(pszPos[0]==_T(' '))
	{
		pszPos++;
	}

	if(pszPos[0]==_T('|'))	// 从中间开始计算
	{
		pos.pit=PIT_CENTER;
		pszPos++;
	}else
	if(pszPos[0]==_T('%'))	// 按照百分比计算
	{
		pos.pit=PIT_PERCENT;
		pszPos++;
	}else					// 普通的计算方式
	{
		pos.pit=PIT_NORMAL;
	}
	
	if(pszPos[0]==_T('-'))	// 负数
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

// 设置位置信息(指定父对象位置)
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

// 从XML设置位置信息属性
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

// 从XML设置宽度信息属性
HRESULT CControlBase::OnAttributeWidth(const CString& strValue, BOOL bLoading)
{
    if (strValue.IsEmpty()) return E_FAIL;

	m_nWidth = _ttoi(strValue);
	m_rc.right = m_rc.left + m_nWidth;
	SetRect(m_rc);

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置高度信息属性
HRESULT CControlBase::OnAttributeHeight(const CString& strValue, BOOL bLoading)
{
    if (strValue.IsEmpty()) return E_FAIL;

	m_nHeight = _ttoi(strValue);
	m_rc.bottom = m_rc.top + m_nHeight;
	SetRect(m_rc);

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置菜单位置信息属性
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

// 从XML设置快捷键信息属性
HRESULT CControlBase::OnAttributeShortcut(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	CDuiObject::ParseKeyCode(strValue, m_nShortcutKey, m_nShortcutFlag);

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置禁用属性
HRESULT CControlBase::OnAttributeDisable(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	SetControlDisable(::StrToInt(strValue) > 0 ? true : false);

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置鼠标光标属性
HRESULT CControlBase::OnAttributeCursor(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	if(strValue == _T("arrow"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));	// 箭头
	}else
	if(strValue == _T("wait"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT));	// 沙漏等待
	}else
	if(strValue == _T("cross"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_CROSS));	// 十字
	}else
	if(strValue == _T("sizewe"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE));	// 双箭头指向东西
	}else
	if(strValue == _T("sizens"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS));	// 双箭头指向南北
	}else
	if(strValue == _T("hand"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_HAND));	// 手型
	}else
	if(strValue == _T("help"))
	{
		m_hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_HELP));	// 箭头+问号
	}

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置是否发送指定的DUI消息的属性
HRESULT CControlBase::OnAttributeSendDuiMsg(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 解析需要发送的DUI消息列表
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

// 设置控件是否可见
void CControlBase::SetVisible(BOOL bIsVisible)
{
	SetControlVisible(bIsVisible);
	UpdateControl(true, true);
}

// 获取控件的可见性(遍历父控件,如果父控件不可见,则返回不可见)
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

// 设置控件是否隐藏
void CControlBase::SetHide(BOOL bIsHide)
{
	SetControlHide(bIsHide);
	UpdateControl(true, true);
}

// 设置控件是否禁用
void  CControlBase::SetDisable(BOOL bIsDisable)
{
	if(m_bIsDisable != bIsDisable)
	{
		SetControlDisable(bIsDisable);
		UpdateControl(true);
	}
}

// 刷新控件的显示内容
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

// 控件局部刷新
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

// 获取子控件对象
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

// 添加子控件
void CControlBase::AddControl(CControlBase* pControl)
{
	if(pControl != NULL)
	{
		m_vecControl.push_back(pControl);
	}
}

// 清除指定的子控件
BOOL CControlBase::RemoveControl(CControlBase* pControl)
{
	vector<CControlBase*>::iterator it;
	for(it=m_vecControl.begin();it!=m_vecControl.end();++it)
	{
		CControlBase* pControlBase = *it;
		if (pControlBase == pControl)
		{
			// 如果是焦点控件,则需要先将焦点设置为空
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

// 清除指定名字的子控件
BOOL CControlBase::RemoveControl(CString strControlName, UINT uControlID)
{
	vector<CControlBase*>::iterator it;
	for(it=m_vecControl.begin();it!=m_vecControl.end();++it)
	{
		CControlBase* pControlBase = *it;
		if (pControlBase && pControlBase->IsThisObject(uControlID, strControlName))
		{
			// 如果是焦点控件,则需要先将焦点设置为空
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

// 清除指定类型的子控件
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

// 清除所有子控件
void CControlBase::RemoveControls()
{
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		RemoveControl(pControlBase);
	}
}

// 获取父对话框
CDlgBase* CControlBase::GetParentDialog(BOOL bEnablePopup)
{
	CDuiObject* pParentObj = GetParent();
	while((pParentObj != NULL) && (!pParentObj->IsClass(_T("dlg"))))
	{
		if(pParentObj->IsClass(_T("popup")))
		{
			if(!bEnablePopup)	// 如果不允许Popup窗口的控件获取父对话框,则直接返回空
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

// 获取父控件的插件宿主窗口功能接口(如果是在插件中)
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

// 消息处理
LRESULT CControlBase::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_bTaskMsg)
	{
		// 如果设置了任务方式发消息的属性,则添加一个任务消息
		CString strControlName = GetName();
		CString strAction = GetAction();
		CDlgBase* pParentDlg = GetParentDialog();
		DuiSystem::Instance()->AddDuiActionTask(uID, uMsg, wParam, lParam, strControlName, strAction, pParentDlg);
		return 0;
	}

	if(m_strAction.Find(_T("dlg:")) == 0)	// 动作:打开一个对话框,有内存泄漏，改为通过DuiSystem创建和管理
	{
		if(uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
		{
			CString strXmlFile = m_strAction;
			strXmlFile.Delete(0, 4);
			DuiSystem::ShowDuiDialog(strXmlFile, GetParentDialog());
		}
	}else
	if(m_strAction.Find(_T("popup:")) == 0)	// 动作:打开一个Popup对话框
	{
		if(uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
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
	if(m_strAction.Find(_T("menu:")) == 0)	// 动作:打开一个菜单
	{
		CDuiMenu *pDuiMenu = new CDuiMenu(DuiSystem::GetDefaultFont(), 12);	// 可以考虑改为通过DuiSystem创建和管理
		pDuiMenu->SetAutoClose(FALSE);
		pDuiMenu->SetParent(this);
		CPoint point;
		CRect rc = GetRect();
		// 计算菜单的显示位置
		if(m_posMenu.nCount >= 2)
		{
			// 使用设置的菜单位置
			int nMenuX = PositionItem2Value(m_posMenu.Left, rc.left, rc.right);
			int nMenuY = PositionItem2Value(m_posMenu.Top, rc.top, rc.bottom);
			point.SetPoint(nMenuX, nMenuY);
		}else
		{
			// 如果没有设置位置信息,则默认按照控件底部开始显示,水平方向中间对齐
			point.SetPoint(rc.left + rc.Width() / 2, rc.bottom);
		}

		// 坐标转换为屏幕坐标
		ClientToScreen(&point);

		CDlgBase* pParentDlg = GetParentDialog();
		CString strXmlFile = m_strAction;
		strXmlFile.Delete(0, 5);
		if(pDuiMenu->LoadXmlFile(strXmlFile, pParentDlg, point, WM_DUI_MENU))
		{
			if(m_posMenu.nCount >= 4)
			{
				// 如果菜单pos有4个值,第三个值为负数表示菜单向左侧显示,第四个值为负数表示菜单向上显示
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
	if(m_strAction.Find(_T("link:")) == 0)	// 动作:打开一个页面链接
	{
		if(uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
		{
			CString strLink = m_strAction;
			strLink.Delete(0, 5);
			if(!strLink.IsEmpty())
			{
				ShellExecute(NULL, TEXT("open"), strLink, NULL,NULL,SW_NORMAL);
			}
		}
	}else
	if(m_strAction.Find(_T("run:")) == 0)	// 动作:执行一个进程
	{
		if(uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
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
				DuiSystem::PathCanonicalize(strProcess);	// 路径标准化
				DuiSystem::ExecuteProcess(strProcess, strCmdLine, bForceAdmin, bWaitProcess);
			}
		}
	}else
	if(m_strAction.Find(ACTION_CLOSE_WINDOW) == 0)	// 动作:关闭指定的窗口
	{
		if(uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
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
		// 首先判断如果是几个默认按钮,则直接做相应的处理
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
			// 调用控件的DUI事件处理对象
			return CallDuiHandler(uMsg, wParam, lParam);
		}
	}

	return 0;
}

// 调用DUI事件处理对象
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
		if(pParentObj->IsClass(_T("dlg")))	// 如果是对话框，暂时终结，不再找父窗口
		{
			pParentObj = ((CDlgBase*)pParentObj)->GetParent();
		}else
		{
			pParentObj = ((CControlBase*)pParentObj)->GetParent();
		}
	}

	// 如果未找到事件处理对象,则通过DuiSystem调用所有注册的事件处理对象进行处理
	return DuiSystem::Instance()->CallDuiHandler(GetID(), GetName(), uMsg, wParam, lParam);
}

// 发送通知消息
LRESULT CControlBase::SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//return ::SendMessage(m_hWnd, Msg, wParam, lParam);
	return m_pParentDuiObject->OnBaseMessage(m_uID, Msg, wParam, lParam);
}

// 打开弹出对话框
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

// 关闭弹出对话框
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

// 设置title
void CControlBaseFont::SetTitle(CString strTitle) 
{
	//if(strTitle != m_strTitle)
	{
		SetControlTitle(strTitle);
		UpdateControl(true);
	}
}

// 设置水平对齐方式
void CControlBaseFont::SetAlignment(UINT uAlignment)
{
	if(uAlignment != m_uAlignment)
	{
		m_uAlignment = uAlignment;
		UpdateControl(true);
	}
}

// 设置垂直对齐方式
void CControlBaseFont::SetVAlignment(UINT uVAlignment)
{
	if(uVAlignment != m_uVAlignment)
	{
		m_uVAlignment = uVAlignment;
		UpdateControl(true);
	}
}

// 设置对齐方式
void CControlBaseFont::SetAlignment(UINT uAlignment, UINT uVAlignment)
{
	if(uAlignment != m_uAlignment || uVAlignment != m_uVAlignment)
	{
		m_uAlignment = uAlignment;
		m_uVAlignment = uVAlignment;

		UpdateControl(true);
	}
}

// 转换水平对齐方式为GDI定义方式
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

// 转换垂直对齐方式为GDI定义方式
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

// 设置字体
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

// 设置图片
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

// 设置图片
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

// 设置Image中默认包含的图片个数
void CControlBaseFont::SetBitmapCount(int nCount)
{
	m_nImagePicCount = nCount;
}

// 设置图片
BOOL CControlBaseFont::SetImage(CString strImage)
{
	if(strImage.Find(_T(".")) != -1)	// 加载图片文件
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
	}else	// 加载图片资源
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

// 从XML设置图片信息属性
HRESULT CControlBaseFont::OnAttributeImage(const CString& strValue, BOOL bLoading)
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

	if(!SetImage(strSkin))
	{
		return E_FAIL;
	}

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置Skin属性
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

// 从XML设置Font属性
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