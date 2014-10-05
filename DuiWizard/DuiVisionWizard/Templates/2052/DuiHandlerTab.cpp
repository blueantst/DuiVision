#include "StdAfx.h"
#include "DuiHandler[!output TAB_CLASS].h"

//////////////////////////////////////////////////////////////
// CDuiHandler[!output TAB_CLASS]

CDuiHandler[!output TAB_CLASS]::CDuiHandler[!output TAB_CLASS](void) : CDuiHandler()
{
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandler[!output TAB_CLASS]::~CDuiHandler[!output TAB_CLASS](void)
{
}

// 初始化
void CDuiHandler[!output TAB_CLASS]::OnInit()
{
}

// DUI定时器事件处理
void CDuiHandler[!output TAB_CLASS]::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
