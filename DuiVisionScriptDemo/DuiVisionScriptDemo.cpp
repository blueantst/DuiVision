// DuiVisionScriptDemo.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "DuiVisionScriptDemo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDuiVisionScriptDemoApp

BEGIN_MESSAGE_MAP(CDuiVisionScriptDemoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDuiVisionScriptDemoApp 构造

CDuiVisionScriptDemoApp::CDuiVisionScriptDemoApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CDuiVisionScriptDemoApp 对象

CDuiVisionScriptDemoApp theApp;


// CDuiVisionScriptDemoApp 初始化

BOOL CDuiVisionScriptDemoApp::InitInstance()
{
	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("DuiVisionScriptDemo"));

	// 初始化DuiVision界面库,可以指定语言,dwLangID为0表示自动判断当前语言
	// 11160是应用程序ID,每个DUI应用程序应该使用不同的ID,ID主要用于进程间通信传递命令行时候区分应用
	DWORD dwLangID = 0;
	new DuiSystem(m_hInstance, dwLangID, _T("DuiVisionScriptDemo.ui"), 11164, IDD_DUIVISIONAPP_DIALOG, _T(""));

	// 加载解释器和主脚本
	HINSTANCE hPluginHandle = NULL;
	LPVOID pPluginObj = NULL;
	#ifdef _DEBUG
	CString strDllFile = DuiSystem::Instance()->GetConfig(_T("interpDll_d"));
	#else
	CString strDllFile = DuiSystem::Instance()->GetConfig(_T("interpDll"));
	#endif
	CString strInstanceName = DuiSystem::Instance()->GetConfig(_T("interpInstance"));
	if(DuiSystem::Instance()->LoadInterpPlugin(strDllFile, strInstanceName, hPluginHandle, pPluginObj))
	{
		m_hInterpPluginHandle = hPluginHandle;
		m_pInterpObject = (IInterp*)pPluginObj;
		// 执行主脚本文件
		CString strMainScript = DuiSystem::Instance()->GetConfig(_T("mainScript"));
		if(strMainScript.IsEmpty())
		{
			DuiSystem::DuiMessageBox(NULL, _T("没有配置执行的脚本文件!"), _T("错误"), MB_OK|MB_ICONERROR);
		}else
		{
			CString strScriptFile;
			if(strMainScript.Find(_T(".tcl")) == -1)
			{
				strScriptFile = DuiSystem::Instance()->GetXmlFile(strMainScript);
			}else
			{
				strScriptFile = strMainScript;
			}
			if(strMainScript.Find(_T(":")) == -1)
			{
				strMainScript = DuiSystem::GetSkinPath() + strMainScript;
			}
			BOOL bRunRet = m_pInterpObject->RunScriptFile(strMainScript);
			CString strResult = m_pInterpObject->GetResult();
			if(!bRunRet)
			{
				DuiSystem::DuiMessageBox(NULL, strResult, _T("运行错误"), MB_OK|MB_ICONERROR);
			}
		}

		// 释放脚本解释器
		if(m_pInterpObject != NULL)
		{
			m_pInterpObject->Release();
			m_pInterpObject = NULL;
		}

		if(m_hInterpPluginHandle != NULL)
		{
			// 释放有些问题,暂不释放动态库
			//FreeLibrary(m_hInterpPluginHandle);
			//m_hInterpPluginHandle = NULL;
		}
	}else
	{
		DuiSystem::DuiMessageBox(NULL, _T("加载脚本解释器失败!"), _T("错误"), MB_OK|MB_ICONERROR);
	}

	// 释放DuiVision界面库的资源
	DuiSystem::Release();

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
