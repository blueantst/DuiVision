#include "stdafx.h"
#include "WinDwmWrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable: 4571) // warning C4571: catch(...) blocks compiled with /EHs do not catch or re-throw Structured Exceptions


#define TRY_POINTER_CALL(proc)\
	try\
	{\
		CSharedData& sData = GetSharedData();\
		if (sData.m_hThemeDll != NULL && sData.m_ptrWrappers[duiWrapper##proc] == NULL)\
		{\
			sData.m_ptrWrappers[duiWrapper##proc] = ::GetProcAddress(\
				sData.m_hThemeDll, duiWrapperProc##proc);\
		}\
		PFN##proc ptr = (PFN##proc)sData.m_ptrWrappers[duiWrapper##proc];\
		if (ptr)\
		{
#define POINTER_CALL (*ptr)
#define CATCH_POINTER_CALL()\
		}\
	}\
	catch (...)\
	{\
	}


//////////////////////////////////////////////////////////////////////////
// 设置DPI的初始值
int CDuiWinDwmWrapper::m_nDpix =96;
int CDuiWinDwmWrapper::m_nDpiy =96;

CDuiWinDwmWrapper::CSharedData::CSharedData()
{
	ZeroMemory(&m_ptrWrappers, sizeof(m_ptrWrappers));
	m_hDwmDll = ::LoadLibrary(_T("dwmapi.dll"));
}


CDuiWinDwmWrapper::CSharedData& CDuiWinDwmWrapper::GetSharedData() const
{
	static CSharedData sData;
	return sData;
}


CDuiWinDwmWrapper::CDuiWinDwmWrapper()
{
}

CDuiWinDwmWrapper::~CDuiWinDwmWrapper()
{
}

BOOL CDuiWinDwmWrapper::IsCompositionEnabled()
{
	CSharedData& sData = GetSharedData();
	if (sData.m_hDwmDll != NULL && sData.m_ptrWrappers[duiWrapperDwmIsCompositionEnabled] == NULL)
	{
		sData.m_ptrWrappers[duiWrapperDwmIsCompositionEnabled] = ::GetProcAddress(
			sData.m_hDwmDll, duiWrapperProcDwmIsCompositionEnabled);
	}

	DUIPFNDwmIsCompositionEnabled ptr = (DUIPFNDwmIsCompositionEnabled)sData.m_ptrWrappers[duiWrapperDwmIsCompositionEnabled];
	if (ptr)
	{
		BOOL bEnabled = FALSE;
		LRESULT lResult = (*ptr)(&bEnabled);
		if (FAILED(lResult))
			return FALSE;

		return bEnabled;
	}
	return FALSE;
}

HRESULT CDuiWinDwmWrapper::DefWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult)
{
	CSharedData& sData = GetSharedData();
	if (sData.m_hDwmDll != NULL && sData.m_ptrWrappers[duiWrapperDwmDefWindowProc] == NULL)
	{
		sData.m_ptrWrappers[duiWrapperDwmDefWindowProc] = ::GetProcAddress(
			sData.m_hDwmDll, duiWrapperProcDwmDefWindowProc);
	}

	DUIPFNDwmDefWindowProc ptr = (DUIPFNDwmDefWindowProc)sData.m_ptrWrappers[duiWrapperDwmDefWindowProc];
	if (ptr)
	{
		return (*ptr)(hWnd, msg, wParam, lParam, plResult);
	}
	return E_FAIL;

}

HRESULT CDuiWinDwmWrapper::ExtendFrameIntoClientArea(HWND hWnd, int cxLeftWidth, int cyTopHeight, int cxRightWidth, int cyBottomHeight)
{
	CSharedData& sData = GetSharedData();
	if (sData.m_hDwmDll != NULL && sData.m_ptrWrappers[duiWrapperDwmExtendFrameIntoClientArea] == NULL)
	{
		sData.m_ptrWrappers[duiWrapperDwmExtendFrameIntoClientArea] = ::GetProcAddress(
			sData.m_hDwmDll, duiWrapperProcDwmExtendFrameIntoClientArea);
	}

	DUIPFNDwmExtendFrameIntoClientArea ptr = (DUIPFNDwmExtendFrameIntoClientArea)sData.m_ptrWrappers[duiWrapperDwmExtendFrameIntoClientArea];
	if (ptr)
	{
		DUI_DWM_MARGINS margins;
		margins.cxLeftWidth = cxLeftWidth;
		margins.cyTopHeight = cyTopHeight;
		margins.cxRightWidth = cxRightWidth;
		margins.cyBottomHeight = cyBottomHeight;

		return (*ptr)(hWnd, &margins);
	}
	return E_FAIL;
}

void CDuiWinDwmWrapper::LogicalToPhysicalPoint(HWND hWnd, LPPOINT lpPoint)
{
	HMODULE hModule = GetModuleHandle(_T("user32.dll"));
	ASSERT(hModule);
	if (!hModule)
		return;

	typedef void (WINAPI* PFNLOGICALTOPHYSICALPOINT)(HWND, LPPOINT);
	PFNLOGICALTOPHYSICALPOINT pfnLogicalToPhysicalPoint =
		(PFNLOGICALTOPHYSICALPOINT)GetProcAddress(hModule, "LogicalToPhysicalPoint");
	if (pfnLogicalToPhysicalPoint)
	{
		pfnLogicalToPhysicalPoint(hWnd, lpPoint);
	}
}

// 设置当前进程禁用DPI虚拟化功能
void CDuiWinDwmWrapper::SetProcessDPIAware()
{
	HMODULE hModule = GetModuleHandle(_T("user32.dll"));
	ASSERT(hModule);
	if (!hModule)
		return;

	typedef void (WINAPI* PFNSETPROCESSDPIAWARE)();
	PFNSETPROCESSDPIAWARE pfnSetProcessDPIAware =
		(PFNSETPROCESSDPIAWARE)GetProcAddress(hModule, "SetProcessDPIAware");
	if (pfnSetProcessDPIAware)
	{
		pfnSetProcessDPIAware();
	}
}

// 根据屏幕DPI设置当前DPI值
void CDuiWinDwmWrapper::SetDpiAdapter(int nDpix, int nDpiy)
{
	HDC hdcScreen;
	hdcScreen = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	if (NULL != hdcScreen)
	{
		m_nDpix = GetDeviceCaps(hdcScreen, LOGPIXELSX);
		m_nDpiy = GetDeviceCaps(hdcScreen, LOGPIXELSY);
		DeleteDC(hdcScreen);
	}

	// 如果传入了DPI值，则用传入的值作为当前DPI值
	if(nDpix != 0)
	{
		m_nDpix = nDpix;
	}
	if(nDpiy != 0)
	{
		m_nDpiy = nDpiy;
	}
}

// 获取当前DPI值
void CDuiWinDwmWrapper::GetDpiAdapter(int& nDpix, int& nDpiy)
{
	nDpix = m_nDpix;
	nDpiy = m_nDpiy;
}

// 根据当前DPI计算x值
void CDuiWinDwmWrapper::AdapterDpi(int& x)
{
	// x*m_nDpix/96
	x = MulDiv(x, m_nDpix, 96);
}

// 根据当前DPI计算x,y值
void CDuiWinDwmWrapper::AdapterDpi(LONG& x,LONG& y)
{
	x = MulDiv(x, m_nDpix, 96);
	y = MulDiv(y, m_nDpiy, 96);
}


