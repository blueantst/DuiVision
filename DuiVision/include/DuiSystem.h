#pragma once

#include "duisingleton.h"
#include  <afxtempl.h>

// ע��DUI�ؼ���
#define REGISTER_DUICONTROL(classname, pfShutdown)   \
	CDuiObjectInfo* pDuiObjectInfo##classname = new CDuiObjectInfo;	\
	pDuiObjectInfo##classname->m_pfGetClassName = classname::GetClassName;		\
	pDuiObjectInfo##classname->m_pfCheckAndNew = (DUIFunc_CheckAndNew)classname::CheckAndNew;		\
	pDuiObjectInfo##classname->m_pfShutdown = pfShutdown;		\
	DuiSystem::Instance()->RegisterDuiControl(pDuiObjectInfo##classname)	\


// ������ID����
#define LANGUAGE_PAGE_ENGLISH		0x0409	// Ӣ��
#define LANGUAGE_PAGE_CHINESE		0x0804	// ���ļ���
#define LANGUAGE_PAGE_CHINESE_TW	0x0404	// ̨�己��
#define LANGUAGE_PAGE_CHINESE_HK	0x0C04	// ��ۡ���������
#define LANGUAGE_PAGE_CHINESE_SGP	0x1004	// �¼��·���

// ����ϵͳ���Ͷ���
enum OSType
{
	OS_UNKNOWN		= -1,
    WINDOWS_WIN32	= 100,
    WINDOWS_95,
    WINDOWS_98,
    WINDOWS_ME,
    WINDOWS_NT,
    WINDOWS_2K,
    WINDOWS_XP,
    WINDOWS_2003,
    WINDOWS_VISTA,
	WINDOWS_7,
	WINDOWS_8,
    WINDOWS_HIGH
};

// ����ϵͳ�������Ͷ���
enum OSPack
{
	PACK_UNKNOWN	= -1,
    SP1				= 1,
    SP2, 
    SP3,
    SP4,
    SP5,
    SP6,
    SP7,
    SP8
};

// �ڴ��ļ���Ϣ����
struct MemFileInfo
{
	BYTE*	pByte;	// ������ָ��
	DWORD	dwSize;	// ��������С
};

class CDlgPopup;
class CTaskMgr;

class DuiSystem : public Singleton<DuiSystem>
{

public:
    DuiSystem(HINSTANCE hInst, DWORD dwLangID = 0, CString strResourceFile = _T(""), UINT uAppID = 0, UINT nIDTemplate = 0, CString strStyle = _T(""));
    ~DuiSystem(void);

	static BOOL HaveInstance();
	static DuiSystem* Instance();
	static void Release();

	void SetInstance(HINSTANCE hInst) { m_hInst = hInst; }
    HINSTANCE GetInstance() { return m_hInst; }

	// ��ȡ����ϵͳ�汾�����汾��
	static int GetOSMajorVersion();
	// ��ȡ��ǰ����(�ַ���)
	static CString GetLanguage();
	// ��ȡ��ǰ����(ID)
	DWORD GetCurrentLanguage();
	// ���õ�ǰ����(ID)
	void SetCurrentLanguage(DWORD dwLangID);
	// ��ȡAppID
	UINT GetAppID() { return m_uAppID; }
	// ���ø�Ŀ¼
	static void SetRootPath(CString strPath);
	// ��ȡ����·��
	static CString GetExePath();
	// ��ȡSkin·��
	static CString GetSkinPath();
	// ��ȡXML·��
	static CString GetXmlPath();
	// ·����׼��
	static BOOL PathCanonicalize(CString& strPath);
	// ������Դ
	BOOL LoadResource();
	// ����XML��Դ�ļ�(����ָ�����صķ��)
	BOOL LoadResourceXml(CString strResFile, CString strStyle = _T(""));
	// ��ȡZIP��Դ�ľ��
	HZIP GetResourceHZip() { return m_hResourceZip; }
	// ����ZIP��Դ�ļ�
	BYTE* LoadZipFile(CString strFile, DWORD& dwSize);
	// ��ȡ������ڴ��ļ���Ϣ
	BOOL GetCachedMemFile(CString strFile, MemFileInfo& memFile);
	// ���û�����ڴ��ļ���Ϣ
	void SetCachedMemFile(CString strFile, MemFileInfo& memFile);
	// ��������ڴ��ļ�����
	void ClearAllCachedMemFile();
	// ����XML�ļ�,֧�ִ�zip�ļ��м���
	BOOL LoadXmlFile(DuiXmlDocument& xmlDoc, CString strFileName);
	// ����ͼƬ�ļ�,֧�ִ�zip�ļ��м���
	BOOL LoadImageFile(CString strFileName, BOOL useEmbeddedColorManagement, Image*& pImage);
	// ����ͼƬ�ļ�,֧�ִ�zip�ļ��м���
	BOOL LoadBitmapFile(CString strFileName, CBitmap &bitmap, CSize &size);
	// ����ͼ���ļ�,֧�ִ�zip�ļ��м���
	BOOL LoadIconFile(CString strFileName, HICON& hIcon);
	// ����ͨ���ļ����ڴ�,֧�ִ�zip�ļ��м���
	BOOL LoadFileToBuffer(CString strFileName, BYTE*& pBuffer);
	// ���ؽ�������̬��
	BOOL LoadPluginFile(CString strFileName, CString strObjType, HINSTANCE& hPluginHandle, LPVOID& pPluginObj);
	// ��ȡϵͳ������Ϣ
	CString GetConfig(CString strName);
	// ��ȡXML�ļ�
	CString GetXmlFile(CString strName);
	// ��ȡSkin
	CString GetSkin(CString strName);
	// ��ȡ�ַ���ֵ
	CString GetString(CString strName);
	// �����ַ���ֵ
	void SetString(CString strName, CString strValue);
	// �����ַ������滻���е��滻����
	void ParseDuiString(CString& strString);
	// ��ȡ����ϵͳ����
	CString GetOSName();
	// ��鵱ǰ����ϵͳ�Ƿ���ָ����OS�ַ�����Χ��
	BOOL CheckOSName(CString strOS);
	// ��ȡ������Ϣ
	BOOL GetFont(CString strName, DuiFontInfo& fontInfo);
	// ��ȡȱʡ������Ϣ(��������������,���ж���������Ƿ񲻺���,�����ʾͻ��ɿ����õ�)
	static CString GetDefaultFont(CString strFont = _T(""));
	// ��ȡ���ڱ�����Ϣ
	BOOL GetWindowBkInfo(int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile);
	// ���ô��ڱ�����Ϣ
	BOOL SetWindowBkInfo(int nType, int nIDResource, COLORREF clr, CString strImgFile);

	// ע��ؼ�
	void RegisterDuiControl(CDuiObjectInfo* pDuiObjectInfo);
	// ж�ؿؼ�
	BOOL UnRegisterDuiControl(LPCTSTR lpszName);
	// ���ؿؼ���
	void LoadDuiControls();
	// �ͷſؼ���
	void ReleaseDuiControls();
	// ���ݿؼ����������ؼ�ʵ��
	static CControlBase* CreateControlByName(LPCTSTR lpszName, HWND hWnd, CDuiObject* pParentObject);
	// ��ȡ�ؼ���Ϣ�б��ָ��
	vector<CDuiObjectInfo*>* GetDuiObjectInfoVect() { return &m_vecDuiObjectInfo; }

	// ��ȡ�ӿؼ�����
	CControlBase* GetControlFromDuiDialog(UINT uControlID);
	static CControlBase* GetControl(CDuiObject* pDuiObject, UINT uControlID);
	static CControlBase* GetControl(CDuiObject* pDuiObject, CString strControlName);
	static CControlBase* GetDialogControl(CString strDialogName, CString strControlName);

	// ��ָ����DUI����ע���¼��������
	static BOOL RegisterHandler(CDuiObject* pDuiObject, CDuiHandler* pDuiHandler, CString strControlName = _T(""), BOOL bInit = FALSE);
	void AddDuiHandler(CDuiHandler* pDuiHandler);
	BOOL RemoveDuiHandler(CDuiHandler* pDuiHandler);
	CDuiHandler* GetDuiHandler(int nIndex);
	LRESULT CallDuiHandler(UINT uID, CString strName, UINT uMsg, WPARAM wParam, LPARAM lParam);
	vector<CDuiHandler*>* GetDuiHandlerVect() { return &m_vecDuiHandler; }

	// �Ի����������
	static CDlgBase* CreateDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName = _T(""), BOOL bModule = TRUE, UINT nIDTemplate = 0, BOOL bAdd = TRUE);
	static int ShowDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName = _T(""), BOOL bModule = TRUE);
	static int DuiMessageBox(CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption = _T(""), UINT uType = MB_OK|MB_ICONINFORMATION, int nWidth = 0, int nHeight = 0);
	void AddDuiDialog(CDlgBase* pDuiDialog);
	void RemoveDuiDialog(CDlgBase* pDuiDialog);
	CDlgBase* GetDuiDialog(int nIndex);
	CDlgBase* GetDuiDialog(CString strName);
	void ResetAllWindowsBkSkin();

	// ��̬��ʾ��Ϣ��
	CDlgBase* GetNotifyMsgBox() { return m_pNotifyMsgBox; }
	void CreateNotifyMsgBox(LPCTSTR lpszXmlTemplate, CString strName = _T(""));
	void RemoveNotifyMsgBox();
	static void SetNotifyMsgBoxControlTitle(CString strControlName, CString strControlTitle);
	static void SetNotifyMsgBoxControlImage(CString strControlName, CString strControlImage);
	static void SetNotifyMsgBoxControlVisible(CString strControlName, BOOL bVisible);
	static void SetNotifyMsgBoxSize(int nWidth, int nHeight);
	static void ShowNotifyMsgBox(UINT uDelayTime = 0);
	static void HideNotifyMsgBox();
	static void AddDuiNotifyMsgTask(CString strMsg, UINT uNotifyType = MB_ICONINFORMATION | 2, CString strCaption = _T(""), int nDelayTime = 0, int nWidth = 0, int nHeight = 0);

	// ����������
	static CDlgPopup* CreateDuiPopupWnd(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, UINT nIDTemplate = 0, BOOL bAdd = FALSE);

	// ����¼����������
	static BOOL ExecuteProcess(CString strProcessName, CString strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess);
	static int RunUITask(DuiVision::IBaseTask* pTask, const DuiVision::CTaskMgr* pTaskMgr);
	DuiVision::CTaskMgr* GetTaskMgr() { return &m_TaskMsg; }
	static void AddDuiActionTask(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam, CString strControlName, CString strAction, CDuiObject* pParent);

	// ��ʱ��������
	static UINT AddDuiTimer(UINT uTimerResolution, CString strTimerName = _T(""), BOOL bAppTimer = TRUE);
	static void RemoveDuiTimer(UINT uTimerID);
	static void RemoveDuiTimer(CString strTimerName);
	void CallDuiHandlerTimer(UINT uTimerID, CString strTimerName);

	// ���̲�������
	BOOL InitTray(CDuiHandler* pDuiHandler = NULL, CString strIcon = _T(""), CString strTip = _T(""), CString strMenuXml = _T(""));
	void DelTray();
	BOOL SetTrayIcon(CString strIcon);
	BOOL SetTrayTip(CString szToolTip);

	// ���̼���Ϣ����
	BOOL SendInterprocessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, CString strAppName, CString strInfo);

	// ��־����
	void InitLog();
	void DoneLog();
	static void	LogEvent(int nLevel, LPCTSTR lpFormat, ...);

protected:
    void createSingletons();
    void destroySingletons();

	CString					m_strResourceFile;						// ��Դ�����ļ���
	HZIP					m_hResourceZip;							// ��ԴZIP�ļ����
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapCfgPool;				// ϵͳ������Ϣ
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapStylePool;			// �����Ϣ
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapXmlPool;				// XML�ļ���
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapSkinPool;			// Skin��
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapStringPool;			// �ַ�����
	CMap<CString,LPCTSTR,DuiFontInfo,DuiFontInfo&> m_mapFontPool;	// �����
	CMap<CString,LPCTSTR,MemFileInfo,MemFileInfo&> m_mapMemFileCatch;	// �ڴ��ļ�����
	
	HINSTANCE				m_hInst;
	UINT					m_uAppID;								// Ӧ��ID

	DWORD					m_dwLangID;								// ��ǰ����ID
	CString					m_strCurStyle;							// ��ǰ�����

	DuiVision::CTaskMgr		m_TaskMsg;								// ������Ϣ�������

	vector<CDuiObjectInfo*>	m_vecDuiObjectInfo;				// �ؼ���Ϣ�����б�
	vector<CDlgBase*>		m_vecDuiDialog;							// DUI�Ի����б�
	vector<CDuiHandler*>	m_vecDuiHandler;						// �¼���������б�

	CDlgBase*				m_pNotifyMsgBox;						// ��̬��ʾ��Ϣ��

	NOTIFYICONDATA			m_NotifyIconData;						// ��������
};
