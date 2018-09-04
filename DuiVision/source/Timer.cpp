#include "StdAfx.h"
#include "Timer.h"

#ifdef USE_WMM_TIMER
#include <mmsystem.h> 
#pragma comment(lib,"Winmm.lib") 
#endif

UINT				CTimer::m_uAccuracy = 1;				// ����ֱ��� 
BOOL				CTimer::m_bIsTmierOK = false;			// �Ƿ���Զ�ʱ(��ʱ���Ƿ��ѳ�ʼ��)
vector<void*>		m_vecCTimer;				// CTimerʵ���б�

CTimer::CTimer(void)
{
	m_vecCTimer.push_back(this);
}

CTimer::~CTimer(void)
{
	KillTimer();

	#ifdef USE_WMM_TIMER
	if (m_bIsTmierOK)
	{
		timeEndPeriod(m_uAccuracy);
	}
	#endif

	for(size_t i = 0; i < m_vecCTimer.size(); i++)
	{
		if(m_vecCTimer[i] == this)
		{
			m_vecCTimer.erase(m_vecCTimer.begin() + i);
			break;
		}
	}
}

#ifdef USE_WMM_TIMER
void CALLBACK CTimer::TimerCallProc(UINT uTimerID, UINT msg, DWORD dwData, DWORD dwa, DWORD dwb)
{
	CTimer *pTimer = (CTimer *)dwData;
	if (pTimer)
	{
		pTimer->OnTimer(uTimerID);
		if(pTimer->IsAppTimer(uTimerID))
		{
			// �����Ӧ�õĶ�ʱ��,����ô����������Ķ�ʱ����
			CString strTimerName = pTimer->GetTimerName(uTimerID);
			pTimer->OnTimer(uTimerID, strTimerName);
		}
	}
}
#endif

#ifdef USE_WND_TIMER
void CALLBACK CTimer::TimerCallProc(HWND hwnd, UINT message, UINT uTimerID, DWORD dwTime)
{
	CTimer *pTimer = CTimer::GetCTimer(uTimerID);
	if (pTimer)
	{
		pTimer->OnTimer(uTimerID);
		if(pTimer->IsAppTimer(uTimerID))
		{
			// �����Ӧ�õĶ�ʱ��,����ô����������Ķ�ʱ����
			CString strTimerName = pTimer->GetTimerName(uTimerID);
			pTimer->OnTimer(uTimerID, strTimerName);
		}
	}
}
#endif

int CTimer::SetTimer(UINT nTimerResolution, CString strTimerName, BOOL bAppTimer)
{
	#ifdef USE_WMM_TIMER
	if(!m_bIsTmierOK)
	{
		TIMECAPS tc;
		//���ú���timeGetDevCapsȡ��ϵͳ�ֱ��ʵ�ȡֵ��Χ������޴������ 
		if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR) 	
		{ 
			// �ֱ��ʵ�ֵ���ܳ���ϵͳ��ȡֵ��Χ  
			m_uAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);
			//����timeBeginPeriod�������ö�ʱ���ķֱ��� 
			timeBeginPeriod(m_uAccuracy); 
			m_bIsTmierOK = true;
		} 
	}

	UINT uTimerID = timeSetEvent(nTimerResolution, m_uAccuracy,(LPTIMECALLBACK) TimerCallProc, (DWORD)this, TIME_PERIODIC);
	#endif

	#ifdef USE_WND_TIMER
	UINT uTimerID = ::SetTimer(NULL, 0, nTimerResolution, (TIMERPROC)CTimer::TimerCallProc);
	#endif

	TIMER_ITEM item;
	item.uTimerID = uTimerID;
	item.strTimerName = strTimerName;
	item.bAppTimer = bAppTimer;
	m_vecTimer.push_back(item);
	return uTimerID;
}

CTimer* CTimer::GetCTimer(UINT uTimerID)
{
	for(size_t t = 0; t < m_vecCTimer.size(); t++)
	{
		CTimer* pTimer = (CTimer*)m_vecCTimer[t];
		for(size_t i = 0; i < pTimer->m_vecTimer.size(); i++)
		{
			if(uTimerID == pTimer->m_vecTimer[i].uTimerID)
			{
				return pTimer;
			}
		}
	}
	return NULL;
}

CString CTimer::GetTimerName(UINT uTimerID)
{
	for(size_t i = 0; i < m_vecTimer.size(); i++)
	{
		if(uTimerID == m_vecTimer[i].uTimerID)
		{
			return m_vecTimer[i].strTimerName;
		}
	}
	return _T("");
}

BOOL CTimer::IsAppTimer(UINT uTimerID)
{
	for(size_t i = 0; i < m_vecTimer.size(); i++)
	{
		if(uTimerID == m_vecTimer[i].uTimerID)
		{
			return m_vecTimer[i].bAppTimer;
		}
	}
	return FALSE;
}

int CTimer::KillTimer(UINT uTimerID)
{
	for(size_t i = 0; i < m_vecTimer.size(); i++)
	{
		if(uTimerID == m_vecTimer[i].uTimerID)
		{
			#ifdef USE_WMM_TIMER
			timeKillEvent(uTimerID);
			#endif
			#ifdef USE_WND_TIMER
			::KillTimer(NULL, uTimerID);
			#endif
			m_vecTimer.erase(m_vecTimer.begin() + i);
			break;
		}
	}
	return m_vecTimer.size();
}

int CTimer::KillTimer(CString strTimerName)
{
	for(size_t i = 0; i < m_vecTimer.size(); i++)
	{
		if(strTimerName == m_vecTimer[i].strTimerName)
		{
			#ifdef USE_WMM_TIMER
			timeKillEvent(m_vecTimer[i].uTimerID);
			#endif
			#ifdef USE_WND_TIMER
			::KillTimer(NULL, m_vecTimer[i].uTimerID);
			#endif
			m_vecTimer.erase(m_vecTimer.begin() + i);
			break;
		}
	}
	return m_vecTimer.size();
}

void CTimer::KillTimer()
{
	for(size_t i = 0; i < m_vecTimer.size(); i++)
	{
		#ifdef USE_WMM_TIMER
		timeKillEvent(m_vecTimer[i].uTimerID);
		#endif
		#ifdef USE_WND_TIMER
		::KillTimer(NULL, m_vecTimer[i].uTimerID);
		#endif
	}
	m_vecTimer.clear();
}

BOOL CTimer::GetTimerIsRun(UINT uTimerID) 
{
	for(size_t i = 0; i < m_vecTimer.size(); i++)
	{
		if(uTimerID == m_vecTimer[i].uTimerID)
		{
			return TRUE;
		}
	}
	return FALSE; 
}

BOOL CTimer::GetTimerIsRun(CString strTimerName) 
{
	for(size_t i = 0; i < m_vecTimer.size(); i++)
	{
		if(strTimerName == m_vecTimer[i].strTimerName)
		{
			return TRUE;
		}
	}
	return FALSE; 
}