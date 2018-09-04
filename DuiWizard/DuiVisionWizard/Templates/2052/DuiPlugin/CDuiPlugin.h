#if !defined(__VCI_INTERFACE_CDuiPlugin_H_INCLUDED__)
#define __VCI_INTERFACE_CDuiPlugin_H_INCLUDED__


#include "vcicomm.h"
#include "IDuiPlugin.h"
#include "IDuiHostWnd.h"

#define INF_IDuiPlugin \
{\
	"IDuiPlugin",\
	"1.0.0.0",\
	THIS_REVISION_TYPE,\
	"{6F20ED82-A398-44CF-A45B-1E1906DA2C6A}",\
	"DuiPlugin",\
	"blueant",\
	__DATE__\
}
const TInterfaceInfo INTERFACE_INFO_IDuiPlugin = INF_IDuiPlugin;

class CDuiPlugin : public CVisualComponent
{
public:
	CDuiPlugin();
	~CDuiPlugin();

//////////////////////////////////////////////////////////////////////////
// 提供给接口的功能函数
//////////////////////////////////////////////////////////////////////////
public:
	//{{VCI_IMPLEMENT_DEFINE_BEGIN
	virtual int __stdcall OnInit(UINT nIDTemplate, HWND hWnd, LPCTSTR lpszName, CRect rc, IDuiHostWnd* pIDuiHostWnd);
	virtual int __stdcall SetRect(CRect rc);
	virtual int __stdcall SetVisible(BOOL bIsVisible);
	virtual int __stdcall SetDisable(BOOL bIsDisable);
	virtual int __stdcall SetFocus(BOOL bFocus);
	virtual int __stdcall DrawControl(CDC &dc, CRect rcUpdate);
	virtual int __stdcall OnTimer(UINT uTimerID, LPCTSTR lpszTimerName);
	virtual int __stdcall OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual int __stdcall OnMousePointChange(CPoint& point);
	virtual int __stdcall OnCheckMouseResponse(UINT nFlags, CPoint point);
	virtual int __stdcall OnMouseMove(UINT nFlags, CPoint point);
	virtual int __stdcall OnLButtonDown(UINT nFlags, CPoint point);
	virtual int __stdcall OnLButtonUp(UINT nFlags, CPoint point);
	virtual int __stdcall OnScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual int __stdcall OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual int __stdcall OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual int __stdcall SetUpdate(BOOL bUpdate, COLORREF clr);
	virtual int __stdcall OnRButtonDown(UINT nFlags, CPoint point);
	virtual int __stdcall OnRButtonUp(UINT nFlags, CPoint point);
	virtual int __stdcall OnRButtonDblClk(UINT nFlags, CPoint point);
	virtual int __stdcall OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}VCI_IMPLEMENT_DEFINE_END

	virtual int __stdcall InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData);
	virtual int __stdcall UninstallHandler(DWORD nEventID, LPVOID lpHandler);
	virtual int __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut);

public:
	CDuiPanel*			m_pDuiPanel;	// panel对象

//////////////////////////////////////////////////////////////////////////
// 要输出的功能接口
//////////////////////////////////////////////////////////////////////////
protected:
	// 接口定义
	BEGIN_INTERFACE_PART(VciControl, IVciControl)
		CONTROL_INTERFACE_DEFINE
	END_INTERFACE_PART(VciControl)
	EXPORT_CONTROL_INTERFACE_PART(VciControl)

	BEGIN_INTERFACE_PART(DuiPlugin, IDuiPluginPanel)
		//{{ 注意：!!!这里是保留的重要函数，不可删除!!!
		// 获取接口的额外信息
		virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData);
		//}} 注意：!!!这里是保留的重要函数，不可删除!!!
	//{{VCI_INTERFACE_PART_BEGIN
		virtual int __stdcall OnInit(UINT nIDTemplate, HWND hWnd, LPCTSTR lpszName, CRect rc, IDuiHostWnd* pIDuiHostWnd);
		virtual int __stdcall SetRect(CRect rc);
		virtual int __stdcall SetVisible(BOOL bIsVisible);
		virtual int __stdcall SetDisable(BOOL bIsDisable);
		virtual int __stdcall SetFocus(BOOL bFocus);
		virtual int __stdcall DrawControl(CDC &dc, CRect rcUpdate);
		virtual int __stdcall OnTimer(UINT uTimerID, LPCTSTR lpszTimerName);
		virtual int __stdcall OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual int __stdcall OnMousePointChange(CPoint& point);
		virtual int __stdcall OnCheckMouseResponse(UINT nFlags, CPoint point);
		virtual int __stdcall OnMouseMove(UINT nFlags, CPoint point);
		virtual int __stdcall OnLButtonDown(UINT nFlags, CPoint point);
		virtual int __stdcall OnLButtonUp(UINT nFlags, CPoint point);
		virtual int __stdcall OnScroll(BOOL bVertical, UINT nFlags, CPoint point);
		virtual int __stdcall OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		virtual int __stdcall OnLButtonDblClk(UINT nFlags, CPoint point);
		virtual int __stdcall SetUpdate(BOOL bUpdate, COLORREF clr);
		virtual int __stdcall OnRButtonDown(UINT nFlags, CPoint point);
		virtual int __stdcall OnRButtonUp(UINT nFlags, CPoint point);
		virtual int __stdcall OnRButtonDblClk(UINT nFlags, CPoint point);
		virtual int __stdcall OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}VCI_INTERFACE_PART_END
	END_INTERFACE_PART(DuiPlugin)
	EXPORT_INTERFACE_PART(DuiPlugin)
};


#endif // !defined(__VCI_INTERFACE_CDuiPlugin_H_INCLUDED__)
