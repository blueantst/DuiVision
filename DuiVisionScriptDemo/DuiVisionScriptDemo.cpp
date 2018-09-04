// DuiVisionScriptDemo.cpp : ����Ӧ�ó��������Ϊ��
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


// CDuiVisionScriptDemoApp ����

CDuiVisionScriptDemoApp::CDuiVisionScriptDemoApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDuiVisionScriptDemoApp ����

CDuiVisionScriptDemoApp theApp;


// CDuiVisionScriptDemoApp ��ʼ��

BOOL CDuiVisionScriptDemoApp::InitInstance()
{
	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("DuiVisionScriptDemo"));

	// ��ʼ��DuiVision�����,����ָ������,dwLangIDΪ0��ʾ�Զ��жϵ�ǰ����
	// 11160��Ӧ�ó���ID,ÿ��DUIӦ�ó���Ӧ��ʹ�ò�ͬ��ID,ID��Ҫ���ڽ��̼�ͨ�Ŵ���������ʱ������Ӧ��
	DWORD dwLangID = 0;
	new DuiSystem(m_hInstance, dwLangID, _T("DuiVisionScriptDemo.ui"), 11164, IDD_DUIVISIONAPP_DIALOG, _T(""));

	// ���ؽ����������ű�
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
		// ִ�����ű��ļ�
		CString strMainScript = DuiSystem::Instance()->GetConfig(_T("mainScript"));
		if(strMainScript.IsEmpty())
		{
			DuiSystem::DuiMessageBox(NULL, _T("û������ִ�еĽű��ļ�!"), _T("����"), MB_OK|MB_ICONERROR);
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
				DuiSystem::DuiMessageBox(NULL, strResult, _T("���д���"), MB_OK|MB_ICONERROR);
			}
		}

		// �ͷŽű�������
		if(m_pInterpObject != NULL)
		{
			m_pInterpObject->Release();
			m_pInterpObject = NULL;
		}

		if(m_hInterpPluginHandle != NULL)
		{
			// �ͷ���Щ����,�ݲ��ͷŶ�̬��
			//FreeLibrary(m_hInterpPluginHandle);
			//m_hInterpPluginHandle = NULL;
		}
	}else
	{
		DuiSystem::DuiMessageBox(NULL, _T("���ؽű�������ʧ��!"), _T("����"), MB_OK|MB_ICONERROR);
	}

	// �ͷ�DuiVision��������Դ
	DuiSystem::Release();

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
