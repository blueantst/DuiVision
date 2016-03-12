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

// 初始化
void CDuiHandlerSession::OnInit()
{
}

// DUI定时器事件处理
void CDuiHandlerSession::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
