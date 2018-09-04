#pragma once
#include <vector>
using namespace std;

//#define USE_WND_TIMER	// ʹ��windows��ʱ��
#define USE_WMM_TIMER	// ʹ�ö�ý�嶨ʱ��

// ��ʱ����Ŀ�ṹ
struct TIMER_ITEM
{
	UINT	uTimerID;
	CString strTimerName;
	BOOL	bAppTimer;		// �Ƿ�Ӧ�õĶ�ʱ��
};

class CTimer
{
public:
	CTimer(void);
	~CTimer(void);

	//����ʾ�����
	vector<TIMER_ITEM>		m_vecTimer;				// ��ʱ���б�
	static UINT				m_uAccuracy;			// ����ֱ��� 
	static BOOL				m_bIsTmierOK;			// �Ƿ���Զ�ʱ

	static CTimer* GetCTimer(UINT uTimerID);
	BOOL GetTimerIsRun(UINT uTimerID);
	BOOL GetTimerIsRun(CString strTimerName);
	CString GetTimerName(UINT uTimerID);
	BOOL IsAppTimer(UINT uTimerID);
	int SetTimer(UINT nTimerResolution, CString strTimerName = _T(""), BOOL bAppTimer = FALSE);
	int KillTimer(UINT uTimerID);
	int KillTimer(CString strTimerName);
	void KillTimer();
	#ifdef USE_WMM_TIMER
	static void CALLBACK TimerCallProc(UINT uTimerID, UINT msg, DWORD dwData, DWORD dwa, DWORD dwb);
	#endif
	#ifdef USE_WND_TIMER
	static void CALLBACK TimerCallProc(HWND hwnd, UINT message, UINT uTimerID, DWORD dwTime);
	#endif
	virtual void OnTimer(UINT uTimerID) = 0;
	virtual void OnTimer(UINT uTimerID, CString strTimerName) {};
};
