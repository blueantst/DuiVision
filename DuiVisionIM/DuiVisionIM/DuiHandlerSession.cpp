#include "StdAfx.h"
#include "DuiHandlerSession.h"

//////////////////////////////////////////////////////////////
// CDuiHandlerSession

CDuiHandlerSession::CDuiHandlerSession(void) : CDuiHandler()
{
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerSession::~CDuiHandlerSession(void)
{
}

// ��ʼ��
void CDuiHandlerSession::OnInit()
{
}

// DUI��ʱ���¼�����
void CDuiHandlerSession::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
