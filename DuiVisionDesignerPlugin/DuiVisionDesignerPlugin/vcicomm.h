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

// �ӿ���Ϣ�ṹ
struct TInterfaceInfo
{
	LPCSTR csName;			// �ӿ�����
	LPCSTR csRevision;		// �汾��, �磺"1.02"
	DWORD dwRevisionType;	// REVISION_TYPE_xxx
	LPCSTR csGuid;			// GUID���磺"{FD1ABEE7-6E04-11D4-BBE0-00002148A399}"
	LPCSTR csDescription;	// �ӿ�����
	LPCSTR csAuthor;		// ����
	LPCSTR csDate;			// ����
};

// �ӿڶ��塢ʵ�ָ�����
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


// ʵ�����ͨ�ö���
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
// �������й������װ�Ķ���
//////////////////////////////////////////////////////////////////////////
//���õĺ궨��
#define DEBUG_OUT_EVENT 0x100	// ���Իص��¼�
#define OPEN_VCI_EVENT	0x101	// ��������ص��¼�
#define	CLOSE_VCI_EVENT	0x102	// �ر�������ص��¼�

//��־��Ϣ�ȼ�
#define LOG_LEVEL_DEBUG 0x0001      //������Ϣ
#define LOG_LEVEL_INFO	0x0002      //һ����Ϣ
#define LOG_LEVEL_ERROR 0x0004      //������Ϣ
#define LOG_LEVEL_CRITICAL 0x0008	//������Ϣ

// ������Ϣ������ (nType��ȡֵ)
#define TEXT_RECEIVE	0x10
#define TEXT_SEND		0x20
#define TEXT_DEBUG		0x30

// ������Ϣ����ʾ��ʽ (nMode��ȡֵ)
#define MODE_HEX		0x10
#define MODE_ASCII		0x20
#define MODE_USER		0x30

// ���ĵ�����Ϣ���С (����ʾ�������Ϣ����)
#define MAX_DEBUG_MSG_SIZE 4000

typedef int (DEBUG_OUT_FUNC)(LPCTSTR lpszComponentId, LPVOID pInstance, int nLevel, int nType, int nMode, LPCSTR lpData, int nLen);

struct TInitData_DebugOut
{
	DEBUG_OUT_FUNC *lpfnDebugOut;
	int		nDeviceID;		// ���ID(Ϊ�˼����ϵĽṹ)
	LPVOID	lpVciInstance;	// ���ʵ������ָ��
	CString	strComponentId;	// ���ID
};

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000

#define TCHAR_ARG   TCHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char
#define DOUBLE_ARG  double


// ���������
typedef LPVOID (OPEN_VCI_FUNC)(LPCSTR lpszVciName, LPCSTR lpszVersion, LPCSTR lpszInstanceName);
typedef BOOL (CLOSE_VCI_FUNC)(LPVOID lpIVci);

// ��ȡ֧�ֵ�����
typedef int (*TYPEOF_QueryCommandIsSupport)(LPCSTR lpcsCmd);
extern "C" __declspec(dllexport)
int gQueryCommandIsSupport(LPCSTR lpcsCmd);

// ��������ҳ�ṹ
typedef struct
{
	CString strParent;		// ���ڵ���
	CString strNode;		// �ڵ���
	CString strDesc;		// ����
	CWnd*	pPage;			// ����ҳָ��
	int		nGrade;			// Ȩ��
} VCI_PROPERTY_PAGE;

typedef CArray<VCI_PROPERTY_PAGE, VCI_PROPERTY_PAGE> CVciPropertyPageArray;

// ע������ҳ
typedef int (*TYPEOF_RegisterPropertyPage)(CVciPropertyPageArray& aPropertyPage);

// ע������ҳ,�����ֱ���:���ڵ���,�ڵ���,����,�Ի���ID,�Ի�������,Ȩ��
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



// ���Զ���
enum{
	VCI_LANGUAGE_ENGLISH,
	VCI_LANGUAGE_CHINESE,
};

class CVciMessage;

//
// ����VCI���ƽӿ�
//
#define IID_IVciControl "IVciControl"

interface IVciControl : public IUnknown
{
	// ��ȡ�����
	virtual int __stdcall GetVciName(CString& strVciName) = 0;
	// ��ȡ�������
	virtual int __stdcall GetVciDesc(CString& strVciDesc) = 0;
	// ��ȡ����汾
	virtual int __stdcall GetVciVersion(int& nVersion) = 0;
	// ��ȡ��������
	virtual int __stdcall GetIsDebugVersion(BOOL& bDebug) = 0;
	// ��ȡ�������
	virtual int __stdcall GetVciType(UINT& nType) = 0;
	// ���õ���ģʽ
	virtual int __stdcall SetDebugMode(BOOL bDebug) = 0;
	// ��������
	virtual int __stdcall SetLanguage(int nLanguage) = 0;
	// ��ȡ��������
	virtual int __stdcall GetLanguage() = 0;
	// ��ȡ������Ϣ
	virtual CString __stdcall GetErrorInfo(int nErrorCode) = 0;
	// ��ȡ����Ĵ������
	virtual int __stdcall GetLastError() = 0;
	// ��ʼ�����
	virtual int __stdcall Init(LPCSTR lpcsParams) = 0;
	// �������
	virtual int __stdcall Done(LPCSTR lpcsParams) = 0;
	// ��װ�ص�����
	virtual int __stdcall InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData) = 0;
	// ж�ػص�����
	virtual int __stdcall UninstallHandler(DWORD nEventID, LPVOID lpHandler) = 0;
	// ��Ϣ����
	virtual int __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut) = 0;
	// ��ѯ�����Ƿ�֧��
	virtual int __stdcall QueryCommandIsSupport(LPCSTR lpcsCmd) = 0;
	// ����ƽ̨�ӿ�
	virtual int __stdcall setIPlatUI(void* pIPlatUI) = 0;
};

//
// ������ͨ�ö���
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
// �������װ�ڲ�ʹ�õĹ��ܺ���
//////////////////////////////////////////////////////////////////////////
public:
	// �����ֶε��йض���
	CString m_strComponentId;
	int m_nDeviceID;
	DEBUG_OUT_FUNC *m_lpfnDebugOut;

	// ���������Ϣ
	void DEBUG_OUT(int nLevel, LPCSTR lpData, int nType = TEXT_DEBUG, int nMode = MODE_ASCII, int nLen = 0)
	{
		if(nMode == MODE_ASCII)
		{
			nLen = strlen(lpData);
		}
		if(m_lpfnDebugOut) m_lpfnDebugOut(m_strComponentId, NULL, nLevel, nType, nMode, lpData, nLen);
	}

	// ���������Ϣ(����ʽ������)
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

	// �����������
	OPEN_VCI_FUNC *m_lpfnOpenVci;
	CLOSE_VCI_FUNC *m_lpfnCloseVci;

	// �����
	virtual LPVOID __stdcall OpenComponent(LPCSTR lpszVciName, LPCSTR lpszVersion, LPCSTR lpszInstanceName)
	{
		if(m_lpfnOpenVci == NULL)
		{
			return NULL;
		}

		LPVOID pIVci = m_lpfnOpenVci(lpszVciName, lpszVersion, lpszInstanceName);

		return pIVci;
	}

	// �ر����
	virtual BOOL __stdcall CloseComponent(LPVOID lpIVci)
	{
		if(m_lpfnCloseVci == NULL)
		{
			return FALSE;
		}

		return m_lpfnCloseVci(lpIVci);
	}

	// ��ȡ��������
	virtual int __stdcall GetIsDebugVersion(BOOL& bDebug)
	{
		#ifdef _DEBUG
		bDebug = TRUE;
		#else
		bDebug = FALSE;
		#endif
		return 0;
	}

	// ���õ���ģʽ
	BOOL m_bDebug;
	virtual int __stdcall SetDebugMode(BOOL bDebug)
	{
		m_bDebug = bDebug;
		return 0;
	}

	// ���Բ���
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

	// ������Ϣ����
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
// ������еġ���׼�ġ���ѡ�����صĺ���
//////////////////////////////////////////////////////////////////////////
public:
	// ��ʼ��ģ��
	virtual int __stdcall Init(LPCSTR lpcsParams)
	{
		return 0;
	}
	// ģ�����
	virtual int __stdcall Done(LPCSTR lpcsParams)
	{
		return 0;
	}

	// ������Ϣ
	virtual int __stdcall SendMessage(CVciMessage* pIn, CVciMessage* ppOut)
	{
		// ����IPlatUI��SendMessage
		/*IPlatUI* pIPlatUI = (IPlatUI*)(getIPlatUI());
		if(pIPlatUI)
		{
			return pIPlatUI->SendMessage(pIn, ppOut);
		}*/
		return 0;
	}

	// ��Ϣ����
	virtual int __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
	{
		return 0;
	};

	// ��ѯ�����Ƿ�֧��
	virtual int __stdcall QueryCommandIsSupport(LPCSTR lpcsCmd)
	{
		return gQueryCommandIsSupport(lpcsCmd);
	}

	// ��װ�ص�����
	virtual int __stdcall InstallHandler(DWORD nEventID, LPVOID lpHandler, DWORD dwRefData)
	{
		if(nEventID == DEBUG_OUT_EVENT)
		{
			if(lpHandler == NULL)
			{
				return -1;
			}

			// ��װ������Ϣ����ص�����
			m_lpfnDebugOut = (DEBUG_OUT_FUNC*)lpHandler;
			// ���������Ϣ
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

			// ��װ������ص�����
			m_lpfnOpenVci = (OPEN_VCI_FUNC*)lpHandler;
			TRACE("InstallHandler: Event=OPEN_VCI_EVENT, FunctionEntry=0x%08X\n", lpHandler);
		}else
		if(nEventID == CLOSE_VCI_EVENT)
		{
			if(lpHandler == NULL)
			{
				return -1;
			}

			// ��װ�ر�����ص�����
			m_lpfnCloseVci = (CLOSE_VCI_FUNC*)lpHandler;
			TRACE("InstallHandler: Event=CLOSE_VCI_EVENT, FunctionEntry=0x%08X\n", lpHandler);
		}

		return 0;
	}

	// ж�ػص�����
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

	void*	m_pIPlatUI;		// ƽ̨����ӿ�
	// ����ƽ̨�ӿ�
	virtual int __stdcall setIPlatUI(void* pIPlatUI)
	{
		m_pIPlatUI = pIPlatUI;
		return 0;
	}
	// ��ȡƽ̨�ӿ�
	virtual void* __stdcall getIPlatUI()
	{
		return m_pIPlatUI;
	}
};


#endif // !defined(__VCICOMM_H_)
