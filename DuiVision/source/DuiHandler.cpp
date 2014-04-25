#include "StdAfx.h"
#include "DuiObject.h"

CDuiHandler::CDuiHandler(void)
{
	m_pDuiObject = NULL;
}

CDuiHandler::~CDuiHandler(void)
{
}

void CDuiHandler::SetDuiObject(CDuiObject* pDuiObject)
{
    m_pDuiObject = pDuiObject;
}

// 获取控件对象
CControlBase* CDuiHandler::GetControl(UINT uControlID)
{
	return DuiSystem::GetControl(m_pDuiObject, uControlID);
}

// 获取控件对象
CControlBase* CDuiHandler::GetControl(CString strControlName)
{
	return DuiSystem::GetControl(m_pDuiObject, strControlName);
}

// 获取控件所在的对话框
CDlgBase* CDuiHandler::GetControlDialog(UINT uControlID)
{
	CControlBase* pControl = GetControl(uControlID);
	if(pControl)
	{
		CDlgBase* pDlg = pControl->GetParentDialog();
		return pDlg;
	}

	return NULL;
}

// 设置控件的可见性
void CDuiHandler::SetVisible(CString strControlName, BOOL bIsVisible)
{
	CControlBase* pControl = GetControl(strControlName);
	if(pControl)
	{
		pControl->SetVisible(bIsVisible);
	}
}

// 设置控件是否禁用
void CDuiHandler::SetDisable(CString strControlName, BOOL bIsDisable)
{
	CControlBase* pControl = GetControl(strControlName);
	if(pControl)
	{
		pControl->SetDisable(bIsDisable);
	}
}

// 设置控件的标题文字
void CDuiHandler::SetTitle(CString strControlName, CString strTitle)
{
	CControlBase* pControl = GetControl(strControlName);
	if(pControl)
	{
		((CControlBaseFont*)pControl)->SetTitle(strTitle);
	}
}

// 获取控件的标题文字
CString CDuiHandler::GetTitle(CString strControlName)
{
	CControlBase* pControl = GetControl(strControlName);
	if(pControl)
	{
		return ((CControlBaseFont*)pControl)->GetTitle();
	}

	return _T("");
}

// 初始化(关联的对话框等控件完成初始化之后会调用事件处理对象的初始化函数)
void CDuiHandler::OnInit()
{
}

// DUI事件处理
LRESULT CDuiHandler::OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

// DUI定时器事件处理
void CDuiHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
}