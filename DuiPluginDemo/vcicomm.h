//////////////////////////////////////////////////////////////////////////
// VCIComm.h

#if !defined(__VCICOMM_H_)
#define __VCICOMM_H_

//#include "IPlatUI.h"

#define __VCI_VERSION__ 0x100

#define REVISION_TYPE_DEBUG		1
#define REVISION_TYPE_RELEASE	0
#ifdef _DEBUG
	#define THIS_REVISION_TYPE REVISION_TYPE_DEBUG
#else
	#define THIS_REVISION_TYPE REVISION_TYPE_RELEASE
#endif

// 接口信息结构
struct TInterfaceInfo
{
	LPCSTR csName;			// 接口名称
	LPCSTR csRevision;		// 版本号, 如："1.02"
	DWORD dwRevisionType;	// REVISION_TYPE_xxx
	LPCSTR csGuid;			// GUID，如："{FD1ABEE7-6E04-11D4-BBE0-00002148A399}"
	LPCSTR csDescription;	// 接口描述
	LPCSTR csAuthor;		// 作者
	LPCSTR csDate;			// 日期
};

// 接口定义、实现辅助宏
#define GET_INTERFACE_OBJECT(theClass) \
	((C##theClass*)((BYTE*)this - offsetof(C##theClass, m_x##theClass)))

#define GET_CONTROL_INTERFACE_OBJECT(theClass) \
	((C##theClass*)((BYTE*)this - offsetof(C##theClass, m_xVciControl)))

#define EXPORT_INTERFACE_PART(localClass) \
	public: \
	virtual LPVOID ExternalQueryInterface() \
	{ ((IUnknown *)&m_x##localClass)->AddRef(); return (LPVOID)&m_x##localClass; }

#define EXPORT_CONTROL_INTERFACE_PART(localClass) \
	public: \
	virtual LPVOID VciControlQueryInterface() \
	{ ((IUnknown *)&m_x##localClass)->AddRef(); return (LPVOID)&m_x##localClass; }

#define INTERFACE_IMPLEMENT(theClass) \
	STDMETHODIMP_(ULONG) C##theClass::X##theClass::AddRef() \
	{ \
		C##theClass *pThis = GET_INTERFACE_OBJECT(theClass); \
		return ++(pThis->m_ulRefCount); \
	} \
	STDMETHODIMP_(ULONG) C##theClass::X##theClass::Release() \
	{ \
		C##theClass *pThis = GET_INTERFACE_OBJECT(theClass); \
		(pThis->m_ulRefCount)--; \
		if((pThis->m_ulRefCount) == 0) { delete pThis; OutputDebugString(_T("A C\"#theClass\" object was deleted!\n")); return 0; } \
		return (pThis->m_ulRefCount); \
	} \
	STDMETHODIMP C##theClass::X##theClass::QueryInterface(REFIID riid, VOID** ppvObj) \
	{ *ppvObj = this; return S_OK; }

#define CONTROL_INTERFACE_DEFINE \
	virtual int __stdcall GetVciName(CString& strVciName); \
	virtual int __stdcall GetVciDesc(CString& strVciDesc); \
	virtual int __stdcall GetVciVersion(int& nVersion); \
	virtual int __stdcall GetIsDebugVersion(BOOL& bDebug); \
	virtual int __stdcall GetVciType(UINT& nType); \
	virtual int __stdcall SetDebugMode(BOOL bDebug); \
	virtual int __stdcall SetLanguage(int nLanguage); \
	virtual int __stdcall GetLanguage(); \
	virtual CString __stdcall GetErrorInfo(int nErrorCode); \
	virtual int __stdcall GetLastError(); \
	virtual int __stdcall Init(LPCSTR lpcsParams); \
	virtual int __stdcall Done(LPCSTR lpcsParams); \
	virtual int __stdcall InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData); \
	virtual int __stdcall UninstallHandler(DWORD nEventID, LPVOID lpHandler); \
	virtual int __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut); \
	virtual int __stdcall QueryCommandIsSupport(LPCSTR lpcsCmd);	\
	virtual int __stdcall setIPlatUI(void* pIPlatUI);


#define CONTROL_INTERFACE_IMPLEMENT(theClass) \
	STDMETHODIMP_(ULONG) C##theClass::XVciControl::AddRef() \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->m_ulRefCount; \
	} \
	STDMETHODIMP_(ULONG) C##theClass::XVciControl::Release() \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return (pThis->m_ulRefCount); \
	} \
	STDMETHODIMP C##theClass::XVciControl::QueryInterface(REFIID riid, VOID** ppvObj) \
	{ *ppvObj = this; return S_OK; } \
	STDMETHODIMP_(int) C##theClass::XVciControl::GetVciName(CString& strVciName) \
	{ \
		/*strVciName = INTERFACE_INFO_I##theClass.csName;*/ \
		return 0; \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::GetVciDesc(CString& strVciDesc) \
	{ \
		/*strVciDesc = INTERFACE_INFO_I##theClass.csDescription;*/ \
		return 0; \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::GetVciVersion(int& nVersion) \
	{ \
		nVersion = 1; \
		return 0; \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::GetIsDebugVersion(BOOL& bDebug) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->GetIsDebugVersion(bDebug); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::GetVciType(UINT& nType) \
	{ \
		nType = 0; \
		return 0; \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::SetDebugMode(BOOL bDebug) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->SetDebugMode(bDebug); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::SetLanguage(int nLanguage) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->SetLanguage(nLanguage); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::GetLanguage() \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->GetLanguage(); \
	} \
	STDMETHODIMP_(CString) C##theClass::XVciControl::GetErrorInfo(int nErrorCode) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->GetErrorInfo(nErrorCode); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::GetLastError() \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->GetLastError(); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::Init(LPCSTR lpcsParams) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->Init(lpcsParams); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::Done(LPCSTR lpcsParams) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->Done(lpcsParams); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->InstallHandler(nEventID, lpHandler, dwRefData); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::UninstallHandler(DWORD nEventID, LPVOID lpHandler) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->UninstallHandler(nEventID, lpHandler); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->ProcessMessage(pIn, ppOut); \
	} \
	STDMETHODIMP_(int) C##theClass::XVciControl::QueryCommandIsSupport(LPCSTR lpcsCmd) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->QueryCommandIsSupport(lpcsCmd); \
	}	\
	STDMETHODIMP_(int) C##theClass::XVciControl::setIPlatUI(void* pIPlatUI) \
	{ \
		C##theClass *pThis = GET_CONTROL_INTERFACE_OBJECT(theClass); \
		return pThis->setIPlatUI(pIPlatUI); \
	}	\


// 实现类的通用定义
class CCommon
{
protected:
	ULONG m_ulRefCount;
public:
	CCommon(){ m_ulRefCount = 0; }
	virtual LPVOID ExternalQueryInterface() = 0;
	virtual LPVOID VciControlQueryInterface() = 0;
};


//////////////////////////////////////////////////////////////////////////
// 下面是有关组件封装的定义
//////////////////////////////////////////////////////////////////////////
//公用的宏定义
#define DEBUG_OUT_EVENT 0x100	// 调试回调事件
#define OPEN_VCI_EVENT	0x101	// 打开子组件回调事件
#define	CLOSE_VCI_EVENT	0x102	// 关闭子组件回调事件

//日志信息等级
#define LOG_LEVEL_DEBUG 0x0001      //调试信息
#define LOG_LEVEL_INFO	0x0002      //一般信息
#define LOG_LEVEL_ERROR 0x0004      //错误信息
#define LOG_LEVEL_CRITICAL 0x0008	//致命信息

// 调试信息的类型 (nType可取值)
#define TEXT_RECEIVE	0x10
#define TEXT_SEND		0x20
#define TEXT_DEBUG		0x30

// 调试信息的显示方式 (nMode可取值)
#define MODE_HEX		0x10
#define MODE_ASCII		0x20
#define MODE_USER		0x30

// 最大的调试信息块大小 (可显示的最大信息长度)
#define MAX_DEBUG_MSG_SIZE 4000

typedef int (DEBUG_OUT_FUNC)(LPCTSTR lpszComponentId, LPVOID pInstance, int nLevel, int nType, int nMode, LPCSTR lpData, int nLen);

struct TInitData_DebugOut
{
	DEBUG_OUT_FUNC *lpfnDebugOut;
	int		nDeviceID;		// 组件ID(为了兼容老的结构)
	LPVOID	lpVciInstance;	// 组件实例对象指针
	CString	strComponentId;	// 组件ID
};

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000

#define TCHAR_ARG   TCHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char
#define DOUBLE_ARG  double


// 子组件操作
typedef LPVOID (OPEN_VCI_FUNC)(LPCSTR lpszVciName, LPCSTR lpszVersion, LPCSTR lpszInstanceName);
typedef BOOL (CLOSE_VCI_FUNC)(LPVOID lpIVci);

// 获取支持的命令
typedef int (*TYPEOF_QueryCommandIsSupport)(LPCSTR lpcsCmd);
extern "C" __declspec(dllexport)
int gQueryCommandIsSupport(LPCSTR lpcsCmd);

// 定义属性页结构
typedef struct
{
	CString strParent;		// 父节点名
	CString strNode;		// 节点名
	CString strDesc;		// 描述
	CWnd*	pPage;			// 属性页指针
	int		nGrade;			// 权限
} VCI_PROPERTY_PAGE;

typedef CArray<VCI_PROPERTY_PAGE, VCI_PROPERTY_PAGE> CVciPropertyPageArray;

// 注册属性页
typedef int (*TYPEOF_RegisterPropertyPage)(CVciPropertyPageArray& aPropertyPage);

// 注册属性页,参数分别是:父节点名,节点名,描述,对话框ID,对话框类名,权限
#define VCI_REGISTER_PROPERTYPAGE(szParent, szNode, szDesc, idtemplate, dialog, grade) \
	{	\
		get_dll_resource();	\
		CString lpszParent = szParent;	\
		CString lpszNode = szNode;	\
		CString lpszDesc = szDesc;	\
		CWnd* pOptionPage = new dialog;	\
		if(pOptionPage->IsKindOf(RUNTIME_CLASS(CDialog)))	\
		{	\
			((CDialog*)pOptionPage)->Create(idtemplate);	\
		}	\
		VCI_PROPERTY_PAGE oo;	\
		oo.strParent = lpszParent;	\
		oo.strNode = lpszNode;	\
		oo.strDesc = lpszDesc;	\
		oo.pPage = pOptionPage;	\
		oo.nGrade = grade;	\
		aPropertyPage.Add(oo);	\
		reset_dll_resource();	\
	}	\



// 语言定义
enum{
	VCI_LANGUAGE_ENGLISH,
	VCI_LANGUAGE_CHINESE,
};

class CVciMessage;

//
// 定义VCI控制接口
//
#define IID_IVciControl "IVciControl"

interface IVciControl : public IUnknown
{
	// 获取组件名
	virtual int __stdcall GetVciName(CString& strVciName) = 0;
	// 获取组件描述
	virtual int __stdcall GetVciDesc(CString& strVciDesc) = 0;
	// 获取组件版本
	virtual int __stdcall GetVciVersion(int& nVersion) = 0;
	// 获取调试类型
	virtual int __stdcall GetIsDebugVersion(BOOL& bDebug) = 0;
	// 获取组件类型
	virtual int __stdcall GetVciType(UINT& nType) = 0;
	// 设置调试模式
	virtual int __stdcall SetDebugMode(BOOL bDebug) = 0;
	// 设置语言
	virtual int __stdcall SetLanguage(int nLanguage) = 0;
	// 获取语言类型
	virtual int __stdcall GetLanguage() = 0;
	// 获取错误信息
	virtual CString __stdcall GetErrorInfo(int nErrorCode) = 0;
	// 获取最近的错误代码
	virtual int __stdcall GetLastError() = 0;
	// 初始化组件
	virtual int __stdcall Init(LPCSTR lpcsParams) = 0;
	// 结束组件
	virtual int __stdcall Done(LPCSTR lpcsParams) = 0;
	// 安装回调函数
	virtual int __stdcall InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData) = 0;
	// 卸载回调函数
	virtual int __stdcall UninstallHandler(DWORD nEventID, LPVOID lpHandler) = 0;
	// 消息处理
	virtual int __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut) = 0;
	// 查询命令是否支持
	virtual int __stdcall QueryCommandIsSupport(LPCSTR lpcsCmd) = 0;
	// 设置平台接口
	virtual int __stdcall setIPlatUI(void* pIPlatUI) = 0;
};

//
// 组件类的通用定义
//
class CVisualComponent : public CCommon
{
public:
	CVisualComponent()
	{
		m_strComponentId = _T("");
		m_nDeviceID = 0;
		m_lpfnDebugOut = NULL;
		m_lpfnOpenVci = NULL;
		m_lpfnCloseVci = NULL;
		m_nLanguage = VCI_LANGUAGE_ENGLISH;
		m_bDebug = FALSE;
		m_nLastError = 0;
		m_pIPlatUI = NULL;
	}
	virtual ~CVisualComponent() {}

//////////////////////////////////////////////////////////////////////////
// 供组件封装内部使用的功能函数
//////////////////////////////////////////////////////////////////////////
public:
	// 调试手段的有关定义
	CString m_strComponentId;
	int m_nDeviceID;
	DEBUG_OUT_FUNC *m_lpfnDebugOut;

	// 输出调试信息
	void DEBUG_OUT(int nLevel, LPCSTR lpData, int nType = TEXT_DEBUG, int nMode = MODE_ASCII, int nLen = 0)
	{
		if(nMode == MODE_ASCII)
		{
			nLen = strlen(lpData);
		}
		if(m_lpfnDebugOut) m_lpfnDebugOut(m_strComponentId, NULL, nLevel, nType, nMode, lpData, nLen);
	}

	// 输出调试信息(带格式化参数)
	void DEBUG_OUTF(int nLevel, LPCTSTR lpszFormat, ...)
	{
		ASSERT(AfxIsValidString(lpszFormat, FALSE));

		va_list argList;
		va_start( argList, lpszFormat );     /* Initialize variable arguments. */

		va_list argListSave = argList;
		// make a guess at the maximum length of the resulting string
		int nMaxLen = 0;
		for (LPCTSTR lpsz = lpszFormat; *lpsz != '\0'; lpsz = _tcsinc(lpsz)) {

			// handle '%' character, but watch out for '%%'
			if (*lpsz != '%' || *(lpsz = _tcsinc(lpsz)) == '%') {

				nMaxLen += _tclen(lpsz);
				continue;
			}

			int nItemLen = 0;
			// handle '%' character with format
			int nWidth = 0;
			for (; *lpsz != '\0'; lpsz = _tcsinc(lpsz)) {

				// check for valid flags
				if (*lpsz == '#')
					nMaxLen += 2;   // for '0x'
				else if (*lpsz == '*')
					nWidth = va_arg(argList, int);
				else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
					*lpsz == ' ')
					;
				else // hit non-flag character
					break;
			}
			// get width and skip it
			if (nWidth == 0) {

				// width indicated by
				nWidth = _ttoi(lpsz);
				for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
					;
			}
			ASSERT(nWidth >= 0);

			int nPrecision = 0;
			if (*lpsz == '.') {

				// skip past '.' separator (width.precision)
				lpsz = _tcsinc(lpsz);

				// get precision and skip it
				if (*lpsz == '*') {

					nPrecision = va_arg(argList, int);
					lpsz = _tcsinc(lpsz);
				}
				else {

					nPrecision = _ttoi(lpsz);
					for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
						;
				}
				ASSERT(nPrecision >= 0);
			}

			// should be on type modifier or specifier
			int nModifier = 0;
			switch (*lpsz) {

			// modifiers that affect size
				case 'h':
					nModifier = FORCE_ANSI;
					lpsz = _tcsinc(lpsz);
					break;
				case 'l':
					nModifier = FORCE_UNICODE;
					lpsz = _tcsinc(lpsz);
					break;

				// modifiers that do not affect size
				case 'F':
				case 'N':
				case 'L':
					lpsz = _tcsinc(lpsz);
					break;
			}

			// now should be on specifier
			switch (*lpsz | nModifier) {

				// single characters
				case 'c':
				case 'C':
					nItemLen = 2;
					va_arg(argList, TCHAR_ARG);
					break;
				case 'c'|FORCE_ANSI:
				case 'C'|FORCE_ANSI:
					nItemLen = 2;
					va_arg(argList, CHAR_ARG);
					break;
				case 'c'|FORCE_UNICODE:
				case 'C'|FORCE_UNICODE:
					nItemLen = 2;
					va_arg(argList, WCHAR_ARG);
					break;

				// strings
				case 's': {

					LPCTSTR pstrNextArg = va_arg(argList, LPCTSTR);
					if (pstrNextArg == NULL)
					   nItemLen = 6;  // "(null)"
					else {

					   nItemLen = lstrlen(pstrNextArg);
					   nItemLen = max(1, nItemLen);
					}
					break;
				}

				case 'S': {

		#ifndef _UNICODE
					LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
					if (pstrNextArg == NULL)
					   nItemLen = 6;  // "(null)"
					else {

					   nItemLen = wcslen(pstrNextArg);
					   nItemLen = max(1, nItemLen);
					}
		#else
					LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
					if (pstrNextArg == NULL)
					   nItemLen = 6; // "(null)"
					else {

					   nItemLen = lstrlenA(pstrNextArg);
					   nItemLen = max(1, nItemLen);
					}
		#endif
					break;
				}

				case 's'|FORCE_ANSI:
				case 'S'|FORCE_ANSI:
				{

					LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
					if (pstrNextArg == NULL)
					   nItemLen = 6; // "(null)"
					else
					{

					   nItemLen = lstrlenA(pstrNextArg);
					   nItemLen = max(1, nItemLen);
					}
					break;
				}

		#ifndef _MAC
				case 's'|FORCE_UNICODE:
				case 'S'|FORCE_UNICODE: {

					LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
					if (pstrNextArg == NULL)
					   nItemLen = 6; // "(null)"
					else
					{

					   nItemLen = wcslen(pstrNextArg);
					   nItemLen = max(1, nItemLen);
					}
					break;
				}
		#endif
			}

				// adjust nItemLen for strings
			if (nItemLen != 0) {

				nItemLen = max(nItemLen, nWidth);
				if (nPrecision != 0)
					nItemLen = min(nItemLen, nPrecision);
			}
			else{

				switch (*lpsz) {

					// integers
					case 'd':
					case 'i':
					case 'u':
					case 'x':
					case 'X':
					case 'o':
						va_arg(argList, int);
						nItemLen = 32;
						nItemLen = max(nItemLen, nWidth+nPrecision);
						break;

					case 'e':
					case 'f':
					case 'g':
					case 'G':
						va_arg(argList, DOUBLE_ARG);
						nItemLen = 128;
						nItemLen = max(nItemLen, nWidth+nPrecision);
						break;

					case 'p':
						va_arg(argList, void*);
						nItemLen = 32;
						nItemLen = max(nItemLen, nWidth+nPrecision);
						break;

					// no output
					case 'n':
						va_arg(argList, int*);
						break;

					default:
						//ASSERT(FALSE);  // unknown formatting option
						nMaxLen += 2;
				}
			}

			// adjust nMaxLen for output nItemLen
			nMaxLen += nItemLen;	// add for blueant
		}

		char  *p_ssData;
		p_ssData = new char[nMaxLen+1];
		//VERIFY(_vstprintf(p_ssData, lpszFormat, argListSave) <= nMaxLen+1);

		if(m_lpfnDebugOut) m_lpfnDebugOut(m_strComponentId, NULL, nLevel, TEXT_DEBUG, MODE_ASCII, p_ssData, nMaxLen+1);

		delete[] p_ssData;

		va_end(argListSave);
		va_end(argList);
	}

	// 组件操作定义
	OPEN_VCI_FUNC *m_lpfnOpenVci;
	CLOSE_VCI_FUNC *m_lpfnCloseVci;

	// 打开组件
	virtual LPVOID __stdcall OpenComponent(LPCSTR lpszVciName, LPCSTR lpszVersion, LPCSTR lpszInstanceName)
	{
		if(m_lpfnOpenVci == NULL)
		{
			return NULL;
		}

		LPVOID pIVci = m_lpfnOpenVci(lpszVciName, lpszVersion, lpszInstanceName);

		return pIVci;
	}

	// 关闭组件
	virtual BOOL __stdcall CloseComponent(LPVOID lpIVci)
	{
		if(m_lpfnCloseVci == NULL)
		{
			return FALSE;
		}

		return m_lpfnCloseVci(lpIVci);
	}

	// 获取调试类型
	virtual int __stdcall GetIsDebugVersion(BOOL& bDebug)
	{
		#ifdef _DEBUG
		bDebug = TRUE;
		#else
		bDebug = FALSE;
		#endif
		return 0;
	}

	// 设置调试模式
	BOOL m_bDebug;
	virtual int __stdcall SetDebugMode(BOOL bDebug)
	{
		m_bDebug = bDebug;
		return 0;
	}

	// 语言操作
	int	m_nLanguage;
	virtual int __stdcall SetLanguage(int nLanguage)
	{
		m_nLanguage = nLanguage;
		return 0;
	}
	virtual int __stdcall GetLanguage()
	{
		return m_nLanguage;
	}

	// 错误信息操作
	int	m_nLastError;
	virtual int __stdcall SetLastError(int nErrorCode)
	{
		m_nLastError = nErrorCode;
		return 0;
	}
	virtual int __stdcall GetLastError()
	{
		return m_nLastError;
	}
	virtual CString __stdcall GetErrorInfo(int nErrorCode)
	{
		return _T("");
	}

//////////////////////////////////////////////////////////////////////////
// 组件特有的、标准的、可选择重载的函数
//////////////////////////////////////////////////////////////////////////
public:
	// 初始化模块
	virtual int __stdcall Init(LPCSTR lpcsParams)
	{
		return 0;
	}
	// 模块结束
	virtual int __stdcall Done(LPCSTR lpcsParams)
	{
		return 0;
	}

	// 发送消息
	virtual int __stdcall SendMessage(CVciMessage* pIn, CVciMessage* ppOut)
	{
		// 调用IPlatUI的SendMessage
		/*IPlatUI* pIPlatUI = (IPlatUI*)(getIPlatUI());
		if(pIPlatUI)
		{
			return pIPlatUI->SendMessage(pIn, ppOut);
		}*/
		return 0;
	}

	// 消息处理
	virtual int __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
	{
		return 0;
	};

	// 查询命令是否支持
	virtual int __stdcall QueryCommandIsSupport(LPCSTR lpcsCmd)
	{
		return gQueryCommandIsSupport(lpcsCmd);
	}

	// 安装回调函数
	virtual int __stdcall InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
	{
		if(nEventID == DEBUG_OUT_EVENT)
		{
			if(lpHandler == NULL)
			{
				return -1;
			}

			// 安装调试信息输出回调函数
			m_lpfnDebugOut = (DEBUG_OUT_FUNC*)lpHandler;
			// 组件附加信息
			TInitData_DebugOut* pInitData = (TInitData_DebugOut*)dwRefData;
			if(pInitData != NULL)
			{
				m_strComponentId = pInitData->strComponentId;
				m_nDeviceID = pInitData->nDeviceID;
			}
			TRACE("InstallHandler: Event=DEBUG_OUT_EVENT, FunctionEntry=0x%08X, ComponentId=%s, Instance=0x%08X, DeviceID=0x%04X\n",
				lpHandler, m_strComponentId, NULL, m_nDeviceID);
		}else
		if(nEventID == OPEN_VCI_EVENT)
		{
			if(lpHandler == NULL)
			{
				return -1;
			}

			// 安装打开组件回调函数
			m_lpfnOpenVci = (OPEN_VCI_FUNC*)lpHandler;
			TRACE("InstallHandler: Event=OPEN_VCI_EVENT, FunctionEntry=0x%08X\n", lpHandler);
		}else
		if(nEventID == CLOSE_VCI_EVENT)
		{
			if(lpHandler == NULL)
			{
				return -1;
			}

			// 安装关闭组件回调函数
			m_lpfnCloseVci = (CLOSE_VCI_FUNC*)lpHandler;
			TRACE("InstallHandler: Event=CLOSE_VCI_EVENT, FunctionEntry=0x%08X\n", lpHandler);
		}

		return 0;
	}

	// 卸载回调函数
	virtual int __stdcall UninstallHandler(DWORD nEventID, LPVOID lpHandler)
	{
		if(nEventID == DEBUG_OUT_EVENT)
		{
			m_lpfnDebugOut = NULL;
		}else
		if(nEventID == OPEN_VCI_EVENT)
		{
			m_lpfnOpenVci = NULL;
		}else
		if(nEventID == CLOSE_VCI_EVENT)
		{
			m_lpfnCloseVci = NULL;
		}

		return 0;
	}

	void*	m_pIPlatUI;		// 平台界面接口
	// 设置平台接口
	virtual int __stdcall setIPlatUI(void* pIPlatUI)
	{
		m_pIPlatUI = pIPlatUI;
		return 0;
	}
	// 获取平台接口
	virtual void* __stdcall getIPlatUI()
	{
		return m_pIPlatUI;
	}
};


#endif // !defined(__VCICOMM_H_)
