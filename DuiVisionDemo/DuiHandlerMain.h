// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI事件处理类
class CDuiHandlerMain : public CDuiHandler
{
public:
	CDuiHandlerMain(void);
	virtual ~CDuiHandlerMain(void);

	void SetDialog(CDlgBase* pDlg) { m_pDlg = pDlg; };

	BOOL ProcessSkinMsg(UINT Msg, WPARAM wParam, LPARAM lParam);

	// 进程间消息
	void	ProcessInterprocessMsg(UINT Msg, WPARAM wParam, DUI_INTERPROCESS_MSG* pInterMsg);

	virtual void OnInit();
	virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	void	OnOption();
	void	OnOptionOK(CDlgBase* pDlg);

	CDlgBase* m_pDlg;
	UINT m_uTimerAni;	// 动画定时器
	int m_nAniIndex;	// 动画索引
};
