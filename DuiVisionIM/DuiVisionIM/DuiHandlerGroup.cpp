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

// 初始化
void CDuiHandlerGroup::OnInit()
{
}

// DUI定时器事件处理
void CDuiHandlerGroup::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
