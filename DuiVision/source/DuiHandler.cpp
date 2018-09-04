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

// ��ȡ�ؼ�����
CControlBase* CDuiHandler::GetControl(UINT uControlID)
{
	return DuiSystem::GetControl(m_pDuiObject, uControlID);
}

// ��ȡ�ؼ�����
CControlBase* CDuiHandler::GetControl(CString strControlName)
{
	return DuiSystem::GetControl(m_pDuiObject, strControlName);
}

// ��ȡ�ؼ����ڵĶԻ���
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

// ���ÿؼ��Ŀɼ���
void CDuiHandler::SetVisible(CString strControlName, BOOL bIsVisible)
{
	CControlBase* pControl = GetControl(strControlName);
	if(pControl)
	{
		pControl->SetVisible(bIsVisible);
	}
}

// ���ÿؼ��Ƿ����
void CDuiHandler::SetDisable(CString strControlName, BOOL bIsDisable)
{
	CControlBase* pControl = GetControl(strControlName);
	if(pControl)
	{
		pControl->SetDisable(bIsDisable);
	}
}

// ���ÿؼ��ı�������
void CDuiHandler::SetTitle(CString strControlName, CString strTitle)
{
	CControlBase* pControl = GetControl(strControlName);
	if(pControl)
	{
		((CControlBaseFont*)pControl)->SetTitle(strTitle);
	}
}

// ��ȡ�ؼ��ı�������
CString CDuiHandler::GetTitle(CString strControlName)
{
	CControlBase* pControl = GetControl(strControlName);
	if(pControl)
	{
		return ((CControlBaseFont*)pControl)->GetTitle();
	}

	return _T("");
}

// ��ʼ��(�����ĶԻ���ȿؼ���ɳ�ʼ��֮�������¼��������ĳ�ʼ������)
void CDuiHandler::OnInit()
{
}

BOOL CDuiHandler::OnValidate()
{
	return TRUE;
}

// DUI�¼�����
LRESULT CDuiHandler::OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

// DUI��ʱ���¼�����
void CDuiHandler::OnTimer(UINT uTimerID, CString strTimerName)
{
}