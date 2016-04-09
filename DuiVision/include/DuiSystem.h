#pragma once

#include "duisingleton.h"
#include  <afxtempl.h>

// 注册DUI控件宏
#define REGISTER_DUICONTROL(classname, pfShutdown)   \
	CDuiObjectInfo* pDuiObjectInfo##classname = new CDuiObjectInfo;	\
	pDuiObjectInfo##classname->m_pfGetClassName = classname::GetClassName;		\
	pDuiObjectInfo##classname->m_pfCheckAndNew = (DUIFunc_CheckAndNew)classname::CheckAndNew;		\
	pDuiObjectInfo##classname->m_pfShutdown = pfShutdown;		\
	DuiSystem::Instance()->RegisterDuiControl(pDuiObjectInfo##classname)	\


// 多语言ID定义
#define LANGUAGE_PAGE_ENGLISH		0x0409	// 英文
#define LANGUAGE_PAGE_CHINESE		0x0804	// 中文简体
#define LANGUAGE_PAGE_CHINESE_TW	0x0404	// 台湾繁体
#define LANGUAGE_PAGE_CHINESE_HK	0x0C04	// 香港、马来繁体
#define LANGUAGE_PAGE_CHINESE_SGP	0x1004	// 新加坡繁体

// 操作系统类型定义
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

// 操作系统补丁类型定义
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

// 内存文件信息定义
struct MemFileInfo
{
	BYTE*	pByte;	// 缓冲区指针
	DWORD	dwSize;	// 缓冲区大小
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

	// 获取操作系统版本的主版本号
	static int GetOSMajorVersion();
	// 获取当前语言(字符串)
	static CString GetLanguage();
	// 获取当前语言(ID)
	DWORD GetCurrentLanguage();
	// 设置当前语言(ID)
	void SetCurrentLanguage(DWORD dwLangID);
	// 获取AppID
	UINT GetAppID() { return m_uAppID; }
	// 设置根目录
	static void SetRootPath(CString strPath);
	// 获取进程路径
	static CString GetExePath();
	// 获取Skin路径
	static CString GetSkinPath();
	// 获取XML路径
	static CString GetXmlPath();
	// 路径标准化
	static BOOL PathCanonicalize(CString& strPath);
	// 加载资源
	BOOL LoadResource();
	// 加载XML资源文件(可以指定加载的风格)
	BOOL LoadResourceXml(CString strResFile, CString strStyle = _T(""));
	// 获取ZIP资源的句柄
	HZIP GetResourceHZip() { return m_hResourceZip; }
	// 加载ZIP资源文件
	BYTE* LoadZipFile(CString strFile, DWORD& dwSize);
	// 获取缓存的内存文件信息
	BOOL GetCachedMemFile(CString strFile, MemFileInfo& memFile);
	// 设置缓存的内存文件信息
	void SetCachedMemFile(CString strFile, MemFileInfo& memFile);
	// 清除所有内存文件缓存
	void ClearAllCachedMemFile();
	// 加载XML文件,支持从zip文件中加载
	BOOL LoadXmlFile(DuiXmlDocument& xmlDoc, CString strFileName);
	// 加载图片文件,支持从zip文件中加载
	BOOL LoadImageFile(CString strFileName, BOOL useEmbeddedColorManagement, Image*& pImage);
	// 加载图片文件,支持从zip文件中加载
	BOOL LoadBitmapFile(CString strFileName, CBitmap &bitmap, CSize &size);
	// 加载图标文件,支持从zip文件中加载
	BOOL LoadIconFile(CString strFileName, HICON& hIcon);
	// 加载通用文件到内存,支持从zip文件中加载
	BOOL LoadFileToBuffer(CString strFileName, BYTE*& pBuffer);
	// 加载界面插件动态库
	BOOL LoadPluginFile(CString strFileName, CString strObjType, HINSTANCE& hPluginHandle, LPVOID& pPluginObj);
	// 获取系统配置信息
	CString GetConfig(CString strName);
	// 获取XML文件
	CString GetXmlFile(CString strName);
	// 获取Skin
	CString GetSkin(CString strName);
	// 获取字符串值
	CString GetString(CString strName);
	// 设置字符串值
	void SetString(CString strName, CString strValue);
	// 解析字符串，替换其中的替换内容
	void ParseDuiString(CString& strString);
	// 获取操作系统名字
	CString GetOSName();
	// 检查当前操作系统是否在指定的OS字符串范围内
	BOOL CheckOSName(CString strOS);
	// 获取字体信息
	BOOL GetFont(CString strName, DuiFontInfo& fontInfo);
	// 获取缺省字体信息(如果给了输入参数,则判断输入参数是否不合适,不合适就换成可以用的)
	static CString GetDefaultFont(CString strFont = _T(""));
	// 获取窗口背景信息
	BOOL GetWindowBkInfo(int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile);
	// 设置窗口背景信息
	BOOL SetWindowBkInfo(int nType, int nIDResource, COLORREF clr, CString strImgFile);

	// 注册控件
	void RegisterDuiControl(CDuiObjectInfo* pDuiObjectInfo);
	// 卸载控件
	BOOL UnRegisterDuiControl(LPCTSTR lpszName);
	// 加载控件库
	void LoadDuiControls();
	// 释放控件库
	void ReleaseDuiControls();
	// 根据控件类名创建控件实例
	static CControlBase* CreateControlByName(LPCTSTR lpszName, HWND hWnd, CDuiObject* pParentObject);
	// 获取控件信息列表的指针
	vector<CDuiObjectInfo*>* GetDuiObjectInfoVect() { return &m_vecDuiObjectInfo; }

	// 获取子控件对象
	CControlBase* GetControlFromDuiDialog(UINT uControlID);
	static CControlBase* GetControl(CDuiObject* pDuiObject, UINT uControlID);
	static CControlBase* GetControl(CDuiObject* pDuiObject, CString strControlName);
	static CControlBase* GetDialogControl(CString strDialogName, CString strControlName);

	// 给指定的DUI对象注册事件处理对象
	static BOOL RegisterHandler(CDuiObject* pDuiObject, CDuiHandler* pDuiHandler, CString strControlName = _T(""), BOOL bInit = FALSE);
	void AddDuiHandler(CDuiHandler* pDuiHandler);
	BOOL RemoveDuiHandler(CDuiHandler* pDuiHandler);
	CDuiHandler* GetDuiHandler(int nIndex);
	LRESULT CallDuiHandler(UINT uID, CString strName, UINT uMsg, WPARAM wParam, LPARAM lParam);
	vector<CDuiHandler*>* GetDuiHandlerVect() { return &m_vecDuiHandler; }

	// 对话框操作函数
	static CDlgBase* CreateDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName = _T(""), BOOL bModule = TRUE, UINT nIDTemplate = 0, BOOL bAdd = TRUE);
	static int ShowDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName = _T(""), BOOL bModule = TRUE);
	static int DuiMessageBox(CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption = _T(""), UINT uType = MB_OK|MB_ICONINFORMATION, int nWidth = 0, int nHeight = 0);
	void AddDuiDialog(CDlgBase* pDuiDialog);
	void RemoveDuiDialog(CDlgBase* pDuiDialog);
	CDlgBase* GetDuiDialog(int nIndex);
	CDlgBase* GetDuiDialog(CString strName);
	void ResetAllWindowsBkSkin();

	// 动态提示信息框
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

	// 创建弹出框
	static CDlgPopup* CreateDuiPopupWnd(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, UINT nIDTemplate = 0, BOOL bAdd = FALSE);

	// 添加事件到任务队列
	static BOOL ExecuteProcess(CString strProcessName, CString strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess);
	static int RunUITask(DuiVision::IBaseTask* pTask, const DuiVision::CTaskMgr* pTaskMgr);
	DuiVision::CTaskMgr* GetTaskMgr() { return &m_TaskMsg; }
	static void AddDuiActionTask(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam, CString strControlName, CString strAction, CDuiObject* pParent);

	// 定时器处理函数
	static UINT AddDuiTimer(UINT uTimerResolution, CString strTimerName = _T(""), BOOL bAppTimer = TRUE);
	static void RemoveDuiTimer(UINT uTimerID);
	static void RemoveDuiTimer(CString strTimerName);
	void CallDuiHandlerTimer(UINT uTimerID, CString strTimerName);

	// 托盘操作函数
	BOOL InitTray(CDuiHandler* pDuiHandler = NULL, CString strIcon = _T(""), CString strTip = _T(""), CString strMenuXml = _T(""));
	void DelTray();
	BOOL SetTrayIcon(CString strIcon);
	BOOL SetTrayTip(CString szToolTip);

	// 进程间消息函数
	BOOL SendInterprocessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, CString strAppName, CString strInfo);

	// 日志函数
	void InitLog();
	void DoneLog();
	static void	LogEvent(int nLevel, LPCTSTR lpFormat, ...);

protected:
    void createSingletons();
    void destroySingletons();

	CString					m_strResourceFile;						// 资源定义文件名
	HZIP					m_hResourceZip;							// 资源ZIP文件句柄
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapCfgPool;				// 系统配置信息
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapStylePool;			// 风格信息
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapXmlPool;				// XML文件池
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapSkinPool;			// Skin池
	CMap<CString,LPCTSTR,CString,LPCTSTR> m_mapStringPool;			// 字符串池
	CMap<CString,LPCTSTR,DuiFontInfo,DuiFontInfo&> m_mapFontPool;	// 字体池
	CMap<CString,LPCTSTR,MemFileInfo,MemFileInfo&> m_mapMemFileCatch;	// 内存文件缓存
	
	HINSTANCE				m_hInst;
	UINT					m_uAppID;								// 应用ID

	DWORD					m_dwLangID;								// 当前语言ID
	CString					m_strCurStyle;							// 当前风格名

	DuiVision::CTaskMgr		m_TaskMsg;								// 界面消息任务队列

	vector<CDuiObjectInfo*>	m_vecDuiObjectInfo;				// 控件信息对象列表
	vector<CDlgBase*>		m_vecDuiDialog;							// DUI对话框列表
	vector<CDuiHandler*>	m_vecDuiHandler;						// 事件处理对象列表

	CDlgBase*				m_pNotifyMsgBox;						// 动态提示信息框

	NOTIFYICONDATA			m_NotifyIconData;						// 托盘数据
};
