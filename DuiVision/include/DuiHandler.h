// DuiVision message handler base class
#pragma once

//////////////////////////////////////////////////////////////////////////
// DUI Message Handler declare
#define DUI_DECLARE_MESSAGE_BEGIN(classname)                            \
public:                                                             \
    virtual LRESULT OnDuiMessage(                                   \
        UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)    \
    {                                                               \
        LRESULT nRet = __super::OnDuiMessage(                       \
            uID, strName, Msg, wParam, lParam       \
            );                                                      \
        if (nRet)                                        \
            return nRet;                                            \
		if (Msg != MSG_MOUSE_MOVE)	\
		{	\
			DuiSystem::LogEvent(LOG_LEVEL_DEBUG,						\
			_T("%s::OnDuiMessage:uID=%d, name=%s, msg=%d, wParam=%d, lParam=%d"),	\
			_T(#classname), uID, strName, Msg, wParam, lParam);						\
		}	\

// 控件ID对应的事件处理
#define DUI_CONTROL_ID_MESSAGE(controlid, msghandler)     \
        if (controlid == uID)                            \
        {                                                           \
			nRet = msghandler(uID, strName, Msg, wParam, lParam);	\
            if(nRet)                      \
				return nRet;                      \
        }                                                           \
        else                                                        \

// 控件Msg对应的事件处理
#define DUI_CONTROL_MSG_MESSAGE(msg, msghandler)     \
        if (msg == Msg)                            \
        {                                                           \
			nRet = msghandler(uID, strName, Msg, wParam, lParam);	\
            if(nRet)                      \
				return nRet;                      \
        }                                                           \
        else                                                        \

// 控件ID和Msg对应的事件处理
#define DUI_CONTROL_IDMSG_MESSAGE(controlid, msg, msghandler)     \
        if (controlid == uID && msg == Msg)                            \
        {                                                           \
			nRet = msghandler(uID, strName, Msg, wParam, lParam);	\
            if(nRet)                      \
				return nRet;                      \
        }                                                           \
        else                                                        \

// 控件名对应的事件处理
#define DUI_CONTROL_NAME_MESSAGE(controlname, msghandler)     \
        if (controlname == strName)                            \
        {                                                           \
			nRet = msghandler(uID, strName, Msg, wParam, lParam);	\
            if(nRet)                      \
				return nRet;                      \
        }                                                           \
        else                                                        \

// 控件名和Msg对应的事件处理
#define DUI_CONTROL_NAMEMSG_MESSAGE(controlname, msg, msghandler)     \
        if (controlname == strName && msg == Msg)                            \
        {                                                           \
			nRet = msghandler(uID, strName, Msg, wParam, lParam);	\
            if(nRet)                      \
				return nRet;                      \
        }                                                           \
        else                                                        \

#define DUI_DECLARE_MESSAGE_END()                              \
            return 0;                                          \
                                                                    \
        return nRet;                                                \
    }                                                               \



class CDuiObject;

// DUI事件处理类
class CDuiHandler
{
public:
	CDuiHandler(void);
	virtual ~CDuiHandler(void);

	void SetDuiObject(CDuiObject* pDuiObject);
	CControlBase* GetControl(UINT uControlID);
	CControlBase* GetControl(CString strControlName);
	CDlgBase* GetControlDialog(UINT uControlID);

	void SetVisible(CString strControlName, BOOL bIsVisible);
	void SetDisable(CString strControlName, BOOL bIsDisable);
	void SetTitle(CString strControlName, CString strTitle);
	CString GetTitle(CString strControlName);

	virtual void OnInit();
	virtual LRESULT OnDuiMessage(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

protected:
	CDuiObject*	m_pDuiObject;			// DUI对象
};
