#include "StdAfx.h"
#include "DuiHandlerGroup.h"

//////////////////////////////////////////////////////////////
// CDuiHandlerGroup

CDuiHandlerGroup::CDuiHandlerGroup(void) : CDuiHandler()
{
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerGroup::~CDuiHandlerGroup(void)
{
}

// ��ʼ��
void CDuiHandlerGroup::OnInit()
{
}

// DUI��ʱ���¼�����
void CDuiHandlerGroup::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
