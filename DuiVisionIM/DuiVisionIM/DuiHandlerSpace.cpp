#include "StdAfx.h"
#include "DuiHandlerSpace.h"

//////////////////////////////////////////////////////////////
// CDuiHandlerSpace

CDuiHandlerSpace::CDuiHandlerSpace(void) : CDuiHandler()
{
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerSpace::~CDuiHandlerSpace(void)
{
}

// ��ʼ��
void CDuiHandlerSpace::OnInit()
{
}

// DUI��ʱ���¼�����
void CDuiHandlerSpace::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
