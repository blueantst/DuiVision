////////////////////////////////////////////////////////////////////////////
//	File:		IPlatUI.h
//	Version:	1.0.0.0
//	Created:	2005-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Interface define of Platform UI.
////////////////////////////////////////////////////////////////////////////
#ifndef __IPLATUI_H_
#define __IPLATUI_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVciMessage;

class CTTreeItemArray_;

interface IPlatUI : public IUnknown
{
	// ƽ̨����
	virtual CString __stdcall GetAppName() = 0;				// ��ȡӦ�ó�����
	virtual CString __stdcall GetPlatPath() = 0;			// ��ȡƽ̨·��
	virtual CString __stdcall GetPlatVersion() = 0;			// ��ȡƽ̨�汾
	virtual int __stdcall GetCurrentLanguage() = 0;			// ��ȡ��ǰ����
	virtual CString __stdcall GetPlatRegistry() = 0;		// ��ȡƽ̨ע����·��
	virtual CString __stdcall GetPlatCopyRight() = 0;		// ��ȡƽ̨��Ȩ�ַ���
	virtual CString __stdcall GetPlatHomeURL() = 0;			// ��ȡ��ҳURL
	virtual CString __stdcall GetPlatDownloadURL() = 0;		// ��ȡ����URL
	virtual int  __stdcall SendMessage(CVciMessage* pIn, CVciMessage* ppOut) = 0;	// ������Ϣ
	virtual int  __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut) = 0;	// ƽ̨����Ϣ����
	virtual int  __stdcall SendCommand(int nCmd, WPARAM wParam, LPARAM lParam) = 0;	// ����ƽ̨����
	virtual BOOL __stdcall SendCommand(int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult) = 0;	// ����ƽ̨����

	// ������ڲ���
	virtual int  __stdcall AddOutput(CString strName) = 0;	// ����������
	virtual BOOL __stdcall DelOutput(int nOT) = 0;			// ɾ���������
	virtual BOOL __stdcall ActiveOutput(int nOT) = 0;		// �����������
	virtual BOOL __stdcall Output(int nOT, LPCTSTR lpszOut) = 0;// �����Ϣ
	virtual BOOL __stdcall OutputFormat(int nOT, int nFmt, LPCTSTR lpszOut) = 0;// �����ʽ��Ϣ
	virtual BOOL __stdcall OutputVerify(int nOT, LPCTSTR lpszOut, LPCTSTR lpszVerify) = 0;// �����Ϣ����У���ַ����Ƚ�,��һ�µ��ַ���ʾΪ��ɫ
	virtual BOOL __stdcall OutputSet(int nOT, LPCTSTR lpszSet) = 0;// �����������
	virtual CString __stdcall OutputGet(int nOT, LPCTSTR lpszParam) = 0;// ��ȡ���������Ϣ
	virtual BOOL __stdcall OutputClear(int nOT) = 0;		// ����������
	virtual BOOL __stdcall OutputAddTreeItems(int nOT, CTTreeItemArray_* pItem) = 0;	// ����������������

	// �û��˵�����
	virtual void* __stdcall GetCommandBars() = 0;			// ��ȡCommandBars
	virtual BOOL __stdcall AddExtMenu(CMenu*& pMenu) = 0;		// �����չ�˵�
	virtual BOOL __stdcall DelExtMenu(CMenu*& pMenu) = 0;		// ɾ����չ�˵�
	virtual BOOL __stdcall AddSubMenu(CString strMainMenu, int nIndex, CMenu*& pMenu, LPVOID pProc, LPVOID pUpdateProc) = 0;	// �����չ�Ӳ˵�
	virtual BOOL __stdcall DelSubMenu(CString strMainMenu, int nIndex, CMenu*& pMenu) = 0;	// ɾ����չ�Ӳ˵�
	virtual BOOL __stdcall TrackPopupMenu(CMenu* pMenu, UINT nFlags, int x, int y, CWnd* pWnd) = 0;		// �����˵�

	// ״̬�в���
	virtual BOOL __stdcall AddStatusPane(int& nID, int nWidth) = 0;	// ���״̬��Pane
	virtual BOOL __stdcall DelStatusPane(int nID) = 0;				// ɾ��״̬��Pane
	virtual BOOL __stdcall SetPaneText(int nID, CString strPane) = 0;// ����Pane����
	virtual BOOL __stdcall SetPaneWidth(int nID, int nWidth) = 0;	// ����Pane���
	virtual BOOL __stdcall SetPaneImage(int nID, int nGroup, int nImage) = 0;// ����Paneͼ��

	// ���ڲ���
	virtual CDocument* __stdcall GetActiveDocument() = 0;	// ��ȡ��ǰ������ĵ�����
	virtual BOOL __stdcall CreateDockingPane(CString strOwmId, CString strTitle, int nID, int nDirection, int nAttachType, HICON hIcon, int nAttachPane = 0) = 0;	// ����DockingPane
	virtual BOOL __stdcall CreateDockingPane(CString strOwmId, int nID) = 0;	// ����DockingPane
	virtual CWnd* __stdcall GetDockingPaneWnd(int nPaneID) = 0;	// ��ȡDockingPane�����Ĵ��ڶ���
	virtual int __stdcall GetDockingPaneIdByName(CString strName) = 0;	// ��ȡDockingPane ID(��������)
	virtual BOOL __stdcall ActiveDockingPane(int nPaneID, BOOL bShow = TRUE) = 0;	// ����DockingPane
	virtual BOOL __stdcall CloseDockingPane(int nPaneID) = 0;	// �ر�DockingPane����

	// ��־����
	virtual BOOL __stdcall AddSysLogInfo(int nType, CString strFrom, CString strDesc) = 0; // ������־��Ϣ

	// OWM����
	virtual BOOL __stdcall IsOwmExist(CString strOwmId) = 0;	// ��ѯģ���Ƿ����
	virtual BOOL __stdcall SendOwmCmd(CString strOwmId, int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult) = 0;	// ����OWM����
	virtual BOOL __stdcall LoadOwm(CString strOwmName, int& nID, BOOL bLoad) = 0;	// ������չģ��

	// �ĵ�����
	virtual CDocument* __stdcall OpenDocumentFile(LPCTSTR lpszFileName, LPCTSTR lpszOwmId) = 0;	// ���ĵ�

	// ���Դ��ڲ���
	virtual CWnd* __stdcall GetPropertyGridWnd() = 0;	// ��ȡ������ָ��
	virtual BOOL __stdcall EnableUpdatePropertyGridContent() = 0;	// �ж��Ƿ���Ը���������
	virtual void __stdcall UpdatePropertyGridContent() = 0;	// ˢ�����Դ�������
	virtual void __stdcall BeginUpdatePropertyGridContent() = 0;	// ��ʼ���Դ���ˢ��
	virtual void __stdcall EndUpdatePropertyGridContent() = 0;	// �������Դ���ˢ��

	// VCI������ýӿ�
	virtual void* __stdcall CreateVciObject(CString strVciId, CString strInstName) = 0;	// ����һ���������
	virtual BOOL __stdcall ReleaseVciObject(void* pObject, BOOL bForceDelete) = 0;	// �ر�һ���������
	virtual BOOL __stdcall ReleaseObjectByInstanceName(CString strInstName) = 0;	// �ر�һ���������(ָ��ʵ����)
	virtual void* __stdcall GetObjectByInstanceName(CString strInstName) = 0;	// ��ȡһ���������(����ʵ����)
	virtual void* __stdcall GetObjectControlPtrByInstancePtr(LPVOID	pVciObj) = 0;	// ��ȡһ�����������ƽӿ�(�������ʵ���ӿ�ָ��)
	virtual int __stdcall GetInstalledVci(CStringArray& asVciId) = 0;	// ��ȡ���а�װ�����ID
	virtual CString __stdcall GetVciName(CString strId) = 0;	// ��ȡһ�������(�������ID)
	virtual CString __stdcall GetVciPath(CString strId) = 0;	// ��ȡһ�����·��(�������ID)
	virtual CString __stdcall GetVciVersion(CString strId) = 0;	// ��ȡһ������汾(�������ID)
	virtual CString __stdcall GetVciProvider(CString strId) = 0;	// ��ȡһ�������������Ϣ(�������ID)
	virtual CString __stdcall GetVciType(CString strId) = 0;	// ��ȡһ���������(�������ID)
	virtual CString __stdcall GetVciClass(CString strId) = 0;	// ��ȡһ�������ӿ�ID(�������ID)
	virtual CString __stdcall GetVciInterfaceFile(CString strId) = 0;	// ��ȡһ������ӿڶ����ļ�(�������ID)
	virtual CTime __stdcall GetVciBuildTime(CString strId) = 0;	// ��ȡһ���������ʱ��(�������ID)
	virtual CString __stdcall GetVciGUID(CString strId) = 0;	// ��ȡһ�����GUID(�������ID)
	virtual CString __stdcall GetVciAboutPage(CString strId) = 0;	// ��ȡһ����������ļ�λ��(�������ID)
	virtual int __stdcall GetVciDocExecuteModule(CString strId, CString strDocId, CString strExecuteType, CStringArray& arExecuteModule) = 0;	// ��ȡ����ִ��һ���ĵ�����չģ��ID�б�(�������Id���ĵ�Id)
	virtual CString __stdcall GetVciDocIcon(CString strId, CString strDocId, CString strIconType) = 0;	// ��ȡ�ĵ���ͼ��(�������Id,�ĵ�Id,ͼ������)
	virtual void __stdcall SetVciDocIcon(CString strId, CString strDocId, CString strIconType, HWND hWnd) = 0;	// �����ĵ���ͼ��(�������Id,�ĵ�Id,ͼ������,���ھ��)
	virtual void __stdcall SetDocumentTitle(CString strTitle, int nModifyType, HWND hWnd) = 0;	// �����ĵ��ı���(���ݴ��ھ��)
	virtual int __stdcall GetVciObjects(CString strId, CPtrArray& auVciObj) = 0;	// ��ȡһ�����������ʵ������ָ��
	virtual CString __stdcall GetInstanceNameByObject(CString strId, LPVOID pVciObj) = 0;	// ��ȡһ�����ʵ�������ʵ����
	virtual int __stdcall GetInstanceRefCount(CString strId, LPVOID pVciObj) = 0;	// ��ȡһ�����ʵ����������ü���
	virtual int __stdcall EnumVciParameters(CString strId, CStringArray& asParamName) = 0;	// ö��һ��VCI�������չ����
	virtual BOOL __stdcall GetVciParameter(CString strId, CString strName, CString& strValue, CString& strDefaultValue) = 0;	// ��ȡһ��VCI����Ĳ���
	virtual BOOL __stdcall SetVciParameter(CString strId, CString strName, CString strValue) = 0;	// ����һ��VCI����Ĳ���

	// ��ʱ������
	virtual void* __stdcall AddTimerTask(CString strTaskName, CString strInterpId, CString strInterpName, CString strCommand, int nSeconds, BOOL bStart) = 0;	// ��Ӷ�ʱ������
	virtual void* __stdcall AddTimerTask(CString strTaskName, LPVOID lpFunction, int nSeconds, BOOL bStart) = 0;	// ��Ӷ�ʱ������
	virtual void* __stdcall GetTimerTask(CString strTaskName) = 0;	// ������������ȡ��ʱ����ָ��
	virtual BOOL __stdcall DeleteTimerTask(LPVOID lpTimerTask) = 0;	// ɾ����ʱ����(��������ָ��)
	virtual BOOL __stdcall DeleteTimerTask(CString strTaskName) = 0;// ɾ����ʱ����(����������)
	virtual void __stdcall StartTimerTask(LPVOID lpTimerTask) = 0;	// ������ʱ����
	virtual void __stdcall SetTimerStartTime(LPVOID lpTimerTask, int tStart) = 0;	// ���ö�ʱ����ʼʱ��
	virtual void __stdcall SetTimerEndTime(LPVOID lpTimerTask, int tEnd) = 0;	// ���ö�ʱ����ʼʱ��
	virtual void __stdcall SetTimerTrigger(LPVOID lpTimerTask, CString strTrigger) = 0;	// ���ö�ʱ���񴥷������ű�
	virtual void __stdcall SetTimerExecCount(LPVOID lpTimerTask, int nCount) = 0;	// ���ö�ʱ����ִ�д���

	// ���߲���
	virtual int __stdcall GetInstalledTool(CStringArray& asToolId) = 0;	// ��ȡ���а�װ�Ĺ���ID
	virtual CString __stdcall GetToolName(CString strId) = 0;	// ��ȡһ��������(���ݹ���ID)
	virtual CString __stdcall GetToolPath(CString strId) = 0;	// ��ȡһ������·��(���ݹ���ID)
	virtual CString __stdcall GetToolVersion(CString strId) = 0;	// ��ȡһ�����߰汾(���ݹ���ID)
	virtual CString __stdcall GetToolProvider(CString strId) = 0;	// ��ȡһ�����߿�������Ϣ(���ݹ���ID)
	virtual BOOL __stdcall RunTool(CString strId) = 0;	// ���й���(���ݹ���ID)

	// ҳ�������ӿ�
};

// ����DockingPane�Ļص�����


#endif // !defined(__IPLATUI_H_)