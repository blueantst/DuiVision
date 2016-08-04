// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI�¼�������
class CDuiHandlerPlugin : public CDuiHandler
{
public:
	CDuiHandlerPlugin(void);
	virtual ~CDuiHandlerPlugin(void);
	
	void SetDuiPanel(CDuiPanel* pPanel) { m_pPanel = pPanel; }

	virtual void OnInit();
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	CDuiPanel*	m_pPanel;
	UINT m_uTimerAni;	// ������ʱ��
	int m_nAniIndex;	// ��������

	// ��Ϣ������
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerPlugin)
	DUI_DECLARE_MESSAGE_END()
};
