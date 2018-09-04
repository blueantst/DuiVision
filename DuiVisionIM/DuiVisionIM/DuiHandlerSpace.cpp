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

// 初始化
void CDuiHandlerSpace::OnInit()
{
}

// DUI定时器事件处理
void CDuiHandlerSpace::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
