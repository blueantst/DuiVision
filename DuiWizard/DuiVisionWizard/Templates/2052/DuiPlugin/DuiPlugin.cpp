// [!output PROJECT_NAME].cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "CDuiPlugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE [!output PROJECT_NAME]DLL = { NULL, NULL };

HINSTANCE GetDllInstance()
{
	return [!output PROJECT_NAME]DLL.hModule;
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("[!output PROJECT_NAME].DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule([!output PROJECT_NAME]DLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary([!output PROJECT_NAME]DLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("[!output PROJECT_NAME].DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule([!output PROJECT_NAME]DLL);
	}
	return 1;   // ok
}

/////////////////////////////////////////////////////////////////////////////
// ��Դ�л�����
// get_dll_resource   �л���DLL����Դ
// reset_dll_resource �ָ���ǰ����Դ
/////////////////////////////////////////////////////////////////////////////
static int resource_counter = 0;		// ��Դ����
static HINSTANCE save_hInstance = NULL;	// ��Դ����

void get_dll_resource(void)
{
   if (resource_counter == 0)
   {
	   save_hInstance = AfxGetResourceHandle();
	   AfxSetResourceHandle(GetDllInstance());
   }

   resource_counter++;
}

void reset_dll_resource(void)
{
   if (resource_counter > 0)
      resource_counter--;

   if (resource_counter == 0)
      AfxSetResourceHandle(save_hInstance);
}

//////////////////////////////////////////////////////////////////////////
// ȫ�ֱ���
//////////////////////////////////////////////////////////////////////////
const TInterfaceInfo INTERFACE_LIST[] =
{
	INF_IDuiPlugin,
};


//////////////////////////////////////////////////////////////////////////
// �� DLL ����ĺ���
//////////////////////////////////////////////////////////////////////////
//
// ��ѯ�� DLL ����Ľӿ��б����ؽӿ���Ŀ
//
extern "C" __declspec(dllexport)
int GetInterfaceList(const TInterfaceInfo **pInfo)
{
	*pInfo = INTERFACE_LIST;
	return (sizeof(INTERFACE_LIST)/sizeof(INTERFACE_LIST[0]));
}

//
// ע������ҳ
//
extern "C" __declspec(dllexport)
int gRegisterPropertyPage(CVciPropertyPageArray& aPropertyPage)
{
	// �ڴ��������ҳ

	return aPropertyPage.GetSize();
}

//
// �������󲢷��ض���Ľӿ�
//
extern "C" __declspec(dllexport)
LPVOID CreateObject(LPCTSTR lpcsInterfaceName, LPVOID* lppVciControl, LPVOID lpInitData)
{
	TRACE(_T("CreateObject(\"%s\")\n"), lpcsInterfaceName);

	CVisualComponent *pObject;
	if(_tcscmp(lpcsInterfaceName, IID_IDuiPluginPanel) == 0)	// ����Panel���
	{
		pObject = new CDuiPlugin;
	}
	else
	{
		TRACE(_T("Can't find Object: %s, CreateObject failed!\n"), lpcsInterfaceName);
		return NULL;
	}

	// Ϊ�����װ������Ϣ����ص�����
	if(lpInitData)
	{
		TInitData_DebugOut *pInitData_DebugOut = (TInitData_DebugOut *)lpInitData;
		pObject->InstallHandler(DEBUG_OUT_EVENT, pInitData_DebugOut->lpfnDebugOut, pInitData_DebugOut->nDeviceID);
	}

	*lppVciControl = pObject->VciControlQueryInterface();

	return pObject->ExternalQueryInterface();
}
