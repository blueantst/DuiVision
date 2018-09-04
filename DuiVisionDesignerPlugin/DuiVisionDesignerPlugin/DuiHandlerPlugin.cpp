#include "StdAfx.h"
#include "DuiHandlerPlugin.h"

//////////////////////////////////////////////////////////////
// CDuiHandlerPlugin

CDuiHandlerPlugin::CDuiHandlerPlugin(void) : CDuiHandler()
{
	m_pPanel = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerPlugin::~CDuiHandlerPlugin(void)
{
}

// 初始化
void CDuiHandlerPlugin::OnInit()
{
}

// DUI定时器事件处理
void CDuiHandlerPlugin::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}
