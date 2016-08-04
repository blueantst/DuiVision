// DuiVision message handler base class
#pragma once
class CDuiObject;

// DUI�¼�������
class CDuiHandlerSpace : public CDuiHandler
{
public:
	CDuiHandlerSpace(void);
	virtual ~CDuiHandlerSpace(void);

	virtual void OnInit();
	
	//virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	UINT m_uTimerAni;	// ������ʱ��
	int m_nAniIndex;	// ��������

	// ��Ϣ������
	DUI_DECLARE_MESSAGE_BEGIN(CDuiHandlerSpace)
	DUI_DECLARE_MESSAGE_END()
};
