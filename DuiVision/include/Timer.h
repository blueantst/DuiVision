#pragma once
#include <vector>
using namespace std;

// 定时器项目结构
struct TIMER_ITEM
{
	UINT	uTimerID;
	CString strTimerName;
	BOOL	bAppTimer;		// 是否应用的定时器
};

class CTimer
{
public:
	CTimer(void);
	~CTimer(void);

	//定显示器相关
	//vector<UINT>			m_vecTimer;				// 定时器ID
	vector<TIMER_ITEM>		m_vecTimer;				// 定时器列表
	static UINT				m_uAccuracy;			// 定义分辨率 
	static BOOL				m_bIsTmierOK;			// 是否可以定时 

	BOOL GetTimerIsRun(UINT uTimerID);
	BOOL GetTimerIsRun(CString strTimerName);
	CString GetTimerName(UINT uTimerID);
	BOOL IsAppTimer(UINT uTimerID);
	int SetTimer(UINT nTimerResolution, CString strTimerName = _T(""), BOOL bAppTimer = FALSE);
	int KillTimer(UINT uTimerID);
	int KillTimer(CString strTimerName);
	void KillTimer();
	static void CALLBACK TimerCallProc(UINT uTimerID, UINT msg, DWORD dwData, DWORD dwa, DWORD dwb);
	virtual void OnTimer(UINT uTimerID) = 0;
	virtual void OnTimer(UINT uTimerID, CString strTimerName) {};
};
