#include "StdAfx.h"
#include "LogMgr.h"
#include <sys/stat.h>

//////////////////////////////////////////////////////////////////////////
static CLogMgr* g_pInsLogMgr = NULL;

CLogMgr::CLogMgr() 
{
	g_pInsLogMgr = this;
	m_bLogEnable = FALSE;
	InitializeCriticalSection(&m_WriteLogMutex);
}

CLogMgr::~CLogMgr()
{ 
	DeleteCriticalSection(&m_WriteLogMutex);
}

// 创建单例
CLogMgr* CLogMgr::Instance()
{
	if(g_pInsLogMgr == NULL)
	{
		new CLogMgr();
	}

	return g_pInsLogMgr;
}

// 释放
void CLogMgr::Release()
{
	if(g_pInsLogMgr != NULL)
	{
		delete g_pInsLogMgr;
		g_pInsLogMgr = NULL;
	}
}

// 记录日志
void CLogMgr::LogEvent(int nLevel, LPCTSTR lpFormat, ...)
{
	if(!CLogMgr::Instance()->IsLogEnable())
	{
		return;
	}

	if(nLevel < CLogMgr::Instance()->GetLogLevel())
	{
		return;
	}
	
	EnterCriticalSection(CLogMgr::Instance()->GetLogMutex());
	
	va_list ap;
	va_start(ap, lpFormat);
	CLogMgr::Instance()->LogEventArgs(nLevel, lpFormat, ap);
	va_end(ap);
	
	LeaveCriticalSection(CLogMgr::Instance()->GetLogMutex());
}

// 记录日志
int CLogMgr::LogEventArgs(int nLevel, LPCTSTR lpFormat, va_list argp)
{
	const int nBufLen = MAX_PATH * 2;
	TCHAR szBuf[nBufLen];

	int nStrLen=_vsntprintf_s(szBuf, nBufLen-1, lpFormat, argp);
	if(nStrLen>0) szBuf[nStrLen - 1] = 0;

	FILE* lpFile = _tfopen(m_strLogFile, _T("a+"));
	if ( lpFile != NULL )
	{
		// 获取文件大小
		int nResult = -1;
#ifdef _UNICODE
		struct _stat FileBuff;
		nResult = _wstat(m_strLogFile, &FileBuff);
#else
		struct _tstat FileBuff;
		nResult = _tstat(m_strLogFile, &FileBuff);
#endif
		if (0 != nResult)
		{
			return -1;	// 获取文件大小失败
		}

		long lSize = FileBuff.st_size;

		// 文件大于设定大小需要进行转储，默认最大1MB
		if (lSize > MAXLOGFILESIZE)
		{
			fclose(lpFile);

			// 删除备份文件
			_tunlink(m_strLogFile + _T(".bak"));

			// 重命名文件名为备份文件名
			_trename(m_strLogFile, m_strLogFile + _T(".bak"));

			// 打开新文件
			lpFile = _tfopen(m_strLogFile, _T("w+"));//,ccs=UTF-8"));
			if (lpFile == NULL)
			{
				return -2;	// 打开可写的新文件失败
			}
		}

		SYSTEMTIME st;
		GetLocalTime(&st);

		CString strLevel;
		if (nLevel == LOG_LEVEL_DEBUG)
		{
			strLevel = __DEBUG;
		}
		else if (nLevel == LOG_LEVEL_INFO)
		{
			strLevel = __INFO;
		}
		else if (nLevel == LOG_LEVEL_ERROR)
		{
			strLevel = __ERROR;
		}
		else if (nLevel == LOG_LEVEL_CRITICAL)
		{
			strLevel = __CRITICAL;
		}
		else
		{
			strLevel = __DEBUG;
		}

		LPCTSTR lpStr = _tcschr(lpFormat, _T('\n'));
		if ( lpStr != NULL )
		{
			lpStr = _T("%s %02d-%02d-%02d %02d:%02d:%02d[%u] : %s");
		}
		else
		{
			lpStr = _T("%s %02d-%02d-%02d %02d:%02d:%02d[%u] : %s\n");
		}

		DWORD dwCurThreadID = GetCurrentThreadId();

		_ftprintf(lpFile, lpStr, strLevel, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, dwCurThreadID, szBuf);
		fclose(lpFile);
	}

	return 0;
}
