// Windows DWM wrapper
#ifndef __DUIWINDWMWRAPPER_H__
#define __DUIWINDWMWRAPPER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct DUI_DWM_MARGINS
{
	int cxLeftWidth;      // width of left border that retains its size
	int cxRightWidth;     // width of right border that retains its size
	int cyTopHeight;      // height of top border that retains its size
	int cyBottomHeight;   // height of bottom border that retains its size
};

#define TYPEDEF_DUIWRAPPER_POINTER(val, type, proc)\
	const int duiWrapper##proc = val;\
	const LPCSTR duiWrapperProc##proc = #proc;\
	typedef type (__stdcall* DUIPFN##proc)

TYPEDEF_DUIWRAPPER_POINTER(0, HRESULT, DwmDefWindowProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
TYPEDEF_DUIWRAPPER_POINTER(1, HRESULT, DwmIsCompositionEnabled)( BOOL*);
TYPEDEF_DUIWRAPPER_POINTER(2, HRESULT, DwmExtendFrameIntoClientArea)(HWND, const DUI_DWM_MARGINS*);

// The CDuiWinDwmWrapper class wraps the dwp api.
class CDuiWinDwmWrapper
{
private:
	// Shared data of wrappers
	class CSharedData
	{
	public:

		CSharedData();

	public:
		LPVOID m_ptrWrappers[3];   // Wrapper pointer
		HMODULE m_hDwmDll;        // Handle to the theme dll.
	};

public:

	CDuiWinDwmWrapper();
	~CDuiWinDwmWrapper();

public:

	HRESULT DefWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
	BOOL IsCompositionEnabled();
	HRESULT ExtendFrameIntoClientArea(HWND hWnd, int cxLeftWidth, int cyTopHeight, int cxRightWidth, int cyBottomHeight);

public:
	void LogicalToPhysicalPoint(HWND hWnd, LPPOINT lpPoint);
	void SetProcessDPIAware();
	static void SetDpiAdapter(int nDpix = 0, int nDpiy = 0);
	static void GetDpiAdapter(int& nDpix, int& nDpiy);
	static void AdapterDpi(int& x);
	static void AdapterDpi(LONG& x,LONG& y);

private:
	CSharedData& GetSharedData() const;
	static int m_nDpix;
	static int m_nDpiy;
};

#endif // __DUIWINDWMWRAPPER_H__
