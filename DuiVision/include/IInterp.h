////////////////////////////////////////////////////////////////////////////
//	File:		IInterp.h
//	Version:	2.0.0.0
//	Created:	2000-07-20
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	interface of interperter
////////////////////////////////////////////////////////////////////////////
#if !defined(__VCI_INTERFACE_IInterp_H_INCLUDED__)
#define __VCI_INTERFACE_IInterp_H_INCLUDED__

// �ű�����ģʽ
enum{
	SCRIPT_RUN_NORMAL,		// ��ͨ����ģʽ
	SCRIPT_RUN_CANSTOP,		// ������ֹ������ģʽ
	SCRIPT_RUN_DEBUG_GO,	// �ϵ��������ģʽ
	SCRIPT_RUN_DEBUG_STEP,	// ������������ģʽ
};

// �ű�����״̬
enum{
	SCRIPT_STATE_IDLE,		// δ����״̬
	SCRIPT_STATE_RUN,		// ��������
	SCRIPT_STATE_BREAK,		// ����ϵ�
	SCRIPT_STATE_STEP,		// ���е�������
	SCRIPT_STATE_END,		// ��������
	SCRIPT_STATE_STOP,		// ����ֹ
};

// �ű����м���
enum{
	SCRIPT_LEVEL_SUPER,		// �����û�
	SCRIPT_LEVEL_ADMIN,		// ����Ա
	SCRIPT_LEVEL_DEVELOP,	// ������Ա
	SCRIPT_LEVEL_OPERATOR,	// ������Ա
	SCRIPT_LEVEL_NONE,		// ��ͼ���
};

// �ϵ�͵�����ģʽ
enum{
	SCRIPT_BREAK_NORMAL,	// ��ͨ�ϵ�
	SCRIPT_BREAK_LINE,		// �жϵ�
	SCRIPT_BREAK_NAME,		// �����ϵ�
	SCRIPT_STEP_INTO,		// ������̵ĵ���
	SCRIPT_STEP_NEXT,		// ��������̵ĵ���
	SCRIPT_STEP_RETURN,		// �ɹ��̷���
};

// �����¼�����
enum{
	// ����Ϊ�������ص��¼�
	IDB_USER_CALL,		// ���ú���
	IDB_USER_LINE,		// ִ��һ��
	IDB_USER_RETURN,	// ��������
	IDB_USER_EXCEPTION,	// �����쳣
	IDB_USER_SETCURRENTLINE,// ���õ�ǰ��

	// ����Ϊƽ̨���������¼�
	IDB_STOP	= 10,	// ��ֹ
	IDB_NEXT,			// ִ�е���һ���ϵ�
	IDB_GOTO,			// ִ�е�ĳ���жϵ�������ϵ�
	IDB_STEPINTO,		// �������뺯��
	IDB_SETPOUT,		// �Ӻ�������
	IDB_STEP,			// ���������뺯��
	IDB_END,			// ֪ͨ�������ļ����Խ���,����ִ���ƺ���

	IDB_SETFILE = 20,	// ���ñ����Ե��ļ�

	IDB_REFRESH_DEBUGWINDOW = 30,	// ˢ�µ��Դ���
	IDB_GET_VAR				= 31,	// ��ȡ������Ϣ
	IDB_SET_VAR				= 32,	// ���ñ���ֵ
};

#define IID_IInterp _T("IInterp")

interface IInterp : public IUnknown
{
	//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	// ��ȡ�ӿڵĶ�����Ϣ
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

	//{{VCI_INTERFACE_BEGIN
	// ����ƽ̨UI�ӿ�ָ��
	virtual int __stdcall SetIPlatUI(LPVOID lpIPlatUI) = 0;
	// ���ÿ�·��
	virtual int __stdcall SetLibPath(LPCTSTR lpszPath) = 0;
	// �������Ƿ�������
	virtual int __stdcall IsInterpRunning() = 0;
	// ���ýű��ļ���
	virtual int __stdcall SetScriptFile(LPCTSTR lpszPathName) = 0;
	// ��ȡ�ű��ļ���
	virtual CString __stdcall GetScriptFile() = 0;
	// ���нű��ļ�
	virtual int __stdcall RunScriptFile(LPCTSTR lpszPathName) = 0;
	// ִ�нű�����
	virtual int __stdcall RunScriptCommand(LPCTSTR lpszTclCmd) = 0;
	// ֹͣ�ű�����
	virtual int __stdcall StopScript() = 0;
	// ��ȡ������ָ��
	virtual LPVOID __stdcall GetInterp() = 0;
	// ��ȡ������ID
	virtual int __stdcall GetInterpID() = 0;
	// ���ý�������
	virtual void __stdcall SetInterpName(LPCTSTR lpszInterptName) = 0;
	// ��ȡ��������
	virtual CString __stdcall GetInterpName() = 0;
	// ��ȡ�����к�
	virtual int __stdcall GetErrorLine() = 0;
	// ��ȡ�����Ϣ
	virtual CString __stdcall GetResult() = 0;
	// ���ñ���ֵ
	virtual int __stdcall SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue) = 0;
	// ��ȡ����ֵ
	virtual CString __stdcall GetVar(LPCTSTR lpszVarName) = 0;
	// ע����չ����
	virtual int __stdcall RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc) = 0;
	// ������չ����
	virtual int __stdcall SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue) = 0;
	// ��ȡ��չ����
	virtual CString __stdcall GetInterpParam(LPCTSTR lpszParamName) = 0;
	// ��������ģʽ
	virtual int __stdcall SetRunMode(int nRunMode) = 0;
	// ��ȡ����ģʽ
	virtual int __stdcall GetRunMode() = 0;
	// ��������״̬
	virtual int __stdcall SetRunState(int nRunState) = 0;
	// ��ȡ����״̬
	virtual int __stdcall GetRunState() = 0;
	// ��ȡ��ǰ��ջ����
	virtual int __stdcall GetCurStackLevel() = 0;
	// ��ȡ�ؼ����б�
	virtual int __stdcall GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord) = 0;
	// ���õ����¼�
	virtual int __stdcall SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam) = 0;
	// ���öϵ��б�
	virtual int __stdcall SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks) = 0;
	// ��ȡ�ϵ��б�
	virtual int __stdcall GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks) = 0;
	// ��ȡ�Ƿ��������ʷ���
	virtual int __stdcall GetEnableProfileAnalyse() = 0;
	// ��ȡ��Ч��������
	virtual int __stdcall GetAvailableLineCount() = 0;
	// ��ȡ�������и��Ǳ�
	virtual LPVOID __stdcall GetProfileMap() = 0;
	// �ж�ָ�������Ƿ�֧��
	virtual int __stdcall MethodIsSupport(LPCTSTR lpszMethod) = 0;
	// ��ʼ������ģʽ
	virtual int __stdcall InitialDebugMode(CString& strResult) = 0;
	// ���µ��Դ�����Ϣ
	virtual int __stdcall RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult) = 0;
	// ���ýű�����
	virtual int __stdcall CallMethod(LPCTSTR lpszMethod, CStringArray& asParam) = 0;
	// ����
	virtual int __stdcall Compile(LPCTSTR lpszSrcFile, CString& strDestFil, LPCTSTR lpszParams) = 0;
	// ����
	virtual int __stdcall Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams) = 0;
	//}}VCI_INTERFACE_END
};

// �����˳��ص�����
#define CALLBACK_EVENT_INTERP_EXIT	0x1000
typedef int (INTERP_EXIT_FUNC)(void* lpInterp);

// ������Իص�����
#define CALLBACK_EVENT_INTERP_DEBUG	0x1001
typedef int (INTERP_DEBUG_FUNC)(int nEvent, LPCTSTR lpszScriptFile, int nLine);


#endif // !defined(__VCI_INTERFACE_IInterp_H_INCLUDED__)
