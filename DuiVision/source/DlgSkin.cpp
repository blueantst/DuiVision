#include "StdAfx.h"
#include "DlgSkin.h"

CDlgSkin::CDlgSkin(void)
{
	m_bImage = true;
}

CDlgSkin::~CDlgSkin(void)
{
}

void CDlgSkin::DrawWindowEx(CDC &dc, CRect rcClient)
{
}

void CDlgSkin::InitUI(CRect rcClient)
{
	__super::InitUI(rcClient);
}

// ÏûÏ¢ÏìÓ¦
LRESULT CDlgSkin::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CControlBase* pControl = GetControl(uID);
	if(pControl == NULL)
	{
		return 0;
	}

	CString strControlName = pControl->GetName();

	if(strControlName == NAME_BT_SKIN_IMAGE)
	{
		if(Msg == MSG_BUTTON_UP && !m_bImage)
		{
			m_bImage = true;
			CControlBase * pControlBase = GetControl(NAME_SKIN_PIC_SELECT);
			if(pControlBase)
			{
				CRect rc = pControlBase->GetRect();
				rc.OffsetRect(-40, 0);
				SetControlRect(pControlBase, rc);
			}
			pControlBase = GetControl(NAME_SKIN_BOX_COLOR);
			if(pControlBase)
			{
				pControlBase->SetVisible(FALSE);
			}
			pControlBase = GetControl(NAME_SKIN_BOX_IMAGE);
			if(pControlBase)
			{
				pControlBase->SetVisible(TRUE);
			}
		}
	}
	else if(strControlName == NAME_BT_SKIN_COLOR)
	{
		if(Msg == MSG_BUTTON_UP && m_bImage)
		{
			m_bImage = false;
			CControlBase * pControlBase = GetControl(NAME_SKIN_PIC_SELECT);
			if(pControlBase)
			{
				CRect rc = pControlBase->GetRect();
				rc.OffsetRect(40, 0);
				SetControlRect(pControlBase, rc);
			}
			pControlBase = GetControl(NAME_SKIN_BOX_IMAGE);
			if(pControlBase)
			{
				pControlBase->SetVisible(FALSE);
			}
			pControlBase = GetControl(NAME_SKIN_BOX_COLOR);
			if(pControlBase)
			{
				pControlBase->SetVisible(TRUE);
			}
			
		}
	}
	else if(strControlName == NAME_SKIN_BOX_COLOR)
	{
		if(Msg == MSG_BUTTON_DOWN)
		{
			SendMessage(m_uMessageID, MSG_SKIN, lParam);
		}
	}
	else if(strControlName == NAME_SKIN_BOX_IMAGE)
	{
		if(Msg == MSG_BUTTON_DOWN)
		{
			SendMessage(m_uMessageID, MSG_SKIN, lParam);
		}
	}
	else if(strControlName == NAME_BT_SKIN_CUSTOMIZE)
	{
		if(Msg == MSG_BUTTON_DOWN)
		{
			PostMessage(m_uMessageID, MSG_SELECT_SKIN, lParam);
		}
	}

	return 0;
}