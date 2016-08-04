// DuiVisionExplorer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "DuiVisionExplorer.h"
#include "DuiHandlerMain.h"
#include "DuiWkeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDuiVisionExplorerApp

BEGIN_MESSAGE_MAP(CDuiVisionExplorerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDuiVisionExplorerApp ����

CDuiVisionExplorerApp::CDuiVisionExplorerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDuiVisionExplorerApp ����

CDuiVisionExplorerApp theApp;


// CDuiVisionExplorerApp ��ʼ��

BOOL CDuiVisionExplorerApp::InitInstance()
{
	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("DuiVisionExplorer"));

	// ��ʼ��DuiVision�����,����ָ������,dwLangIDΪ0��ʾ�Զ��жϵ�ǰ����
	// 11160��Ӧ�ó���ID,ÿ��DUIӦ�ó���Ӧ��ʹ�ò�ͬ��ID,ID��Ҫ���ڽ��̼�ͨ�Ŵ���������ʱ������Ӧ��
	DWORD dwLangID = 0;
	new DuiSystem(m_hInstance, dwLangID, _T("DuiVisionExplorer.ui"), 11161, IDD_DUIVISIONAPP_DIALOG, _T(""));

	// ����Ƿ��Ѿ��н���������
	CString strAppMutex = DuiSystem::Instance()->GetConfig(_T("appMutex"));	// �������ļ��л�ȡ����������
	if(!strAppMutex.IsEmpty())
	{
		::CreateMutex(NULL,TRUE, _T("Global\\") + strAppMutex);
		if(ERROR_ALREADY_EXISTS == GetLastError() || ERROR_ACCESS_DENIED == GetLastError())
		{
			// ��ȡ�����в���,�������Ҫ���������п���ֱ���˳�
			CString strCmd = L"";
			if(__argc > 0)
			{
				strCmd = __targv[0];
				DuiSystem::LogEvent(LOG_LEVEL_DEBUG, L"Command line:%s", strCmd);
			}

			// ���ͽ��̼���Ϣ(lParamΪ1��ʾ����ʾ����,appMutex��ΪӦ����,��Ϣ�������������в���)
			CString strAppName = DuiSystem::Instance()->GetConfig(_T("appMutex"));
			DuiSystem::Instance()->SendInterprocessMessage(0, DuiSystem::Instance()->GetAppID(), 1, strAppName, strCmd);

			return FALSE; // Here we quit this application
		}
	}

	// ע��WKE�ؼ�
	REGISTER_DUICONTROL(CDuiWkeView, CDuiWkeView::WkeShutdown);

	// ����������
	CDlgBase* pMainDlg = DuiSystem::CreateDuiDialog(_T("dlg_main"), NULL, _T(""), TRUE);
	// ��������ע���¼��������
	CDuiHandlerMain* pHandler = new CDuiHandlerMain();
	pHandler->SetDialog(pMainDlg);
	DuiSystem::RegisterHandler(pMainDlg, pHandler);

	// ��ʼ����ʾ��Ϣ����
	DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));

	// ���շ�ģʽ�Ի��򴴽�������,����Ĭ������
	pMainDlg->Create(pMainDlg->GetIDTemplate(), NULL);
	//pMainDlg->ShowWindow(SW_HIDE);
	INT_PTR nResponse = pMainDlg->RunModalLoop();

	// ����ǰ���ģʽ�Ի�������������,ֻҪ��Ϊ���´���Ϳ���
	//INT_PTR nResponse = pMainDlg->DoModal();

	// �ͷ�DuiVision��������Դ
	DuiSystem::Release();

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
