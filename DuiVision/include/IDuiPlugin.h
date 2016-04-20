#if !defined(__INTERFACE_IDUIPLUGIN_H_INCLUDED__)
#define __INTERFACE_IDUIPLUGIN_H_INCLUDED__


// 接口对象ID定义
#define IID_IDuiPluginPanel _T("IDuiPluginPanel")	// panel对象

interface IDuiPluginPanel : public IUnknown
{
	//{{ 注意：!!!这里是保留的重要函数，不可删除!!!
	// 获取接口的额外信息
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} 注意：!!!这里是保留的重要函数，不可删除!!!

	//{{VCI_INTERFACE_BEGIN
	virtual int __stdcall OnInit(UINT nIDTemplate, HWND hWnd, LPCTSTR lpszName, CRect rc, IDuiHostWnd* pIDuiHostWnd) = 0;
	virtual int __stdcall SetRect(CRect rc) = 0;
	virtual int __stdcall SetVisible(BOOL bIsVisible) = 0;
	virtual int __stdcall SetDisable(BOOL bIsDisable) = 0;
	virtual int __stdcall SetFocus(BOOL bFocus) = 0;
	virtual int __stdcall DrawControl(CDC &dc, CRect rcUpdate) = 0;
	virtual int __stdcall OnTimer(UINT uTimerID, LPCTSTR lpszTimerName) = 0;
	virtual int __stdcall OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual int __stdcall OnMousePointChange(CPoint& point) = 0;
	virtual int __stdcall OnCheckMouseResponse(UINT nFlags, CPoint point) = 0;
	virtual int __stdcall OnMouseMove(UINT nFlags, CPoint point) = 0;
	virtual int __stdcall OnLButtonDown(UINT nFlags, CPoint point) = 0;
	virtual int __stdcall OnLButtonUp(UINT nFlags, CPoint point) = 0;
	virtual int __stdcall OnScroll(BOOL bVertical, UINT nFlags, CPoint point) = 0;
	virtual int __stdcall OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) = 0;
	virtual int __stdcall OnLButtonDblClk(UINT nFlags, CPoint point) = 0;
	virtual int __stdcall SetUpdate(BOOL bUpdate, COLORREF clr) = 0;
	virtual int __stdcall OnRButtonDown(UINT nFlags, CPoint point) = 0;
	virtual int __stdcall OnRButtonUp(UINT nFlags, CPoint point) = 0;
	virtual int __stdcall OnRButtonDblClk(UINT nFlags, CPoint point) = 0;
	//}}VCI_INTERFACE_END
};


#endif // !defined(__INTERFACE_IDUIPLUGIN_H_INCLUDED__)
