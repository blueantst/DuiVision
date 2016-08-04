#include "StdAfx.h"
#include "LogMgr.h"
#include <sys/stat.h>
#include <afx.h>
#include <io.h>
#include <direct.h>
#include <iostream>
#include <time.h>
#include <string>
#include <sstream>

#ifdef _UNICODE
	#define T_STRNCPY	wcsncpy
	#define T_STRCPY	wcscpy
	#define T_STRLEN	wcslen
	#define T_STRBYTE		2*wcslen
	#define T_STRCAT	wcscat
	#define T_SNPRINTF	_snwprintf
	#define T_STAT	_wstat
	#define T_ACCESS	_waccess
#else
	#define T_STRNCPY	strncpy
	#define T_STRCPY	strcpy
	#define T_STRLEN	strlen
	#define T_STRBYTE		strlen
	#define T_STRCAT	strcat
	#define T_SNPRINTF	_snprintf
	#define T_STAT	_tstat
	#define T_ACCESS	_access
#endif


//////////////////////////////////////////////////////////////////////////
static CLogMgr* g_pInsLogMgr = NULL;

CLogMgr::CLogMgr() 
{
	g_pInsLogMgr = this;
	m_bLogEnable = FALSE;
	m_strLogPath = _T("");
	m_strLogFile = _T("");
	m_strLogFileName = _T("");
	m_nLogLevel = LOG_LEVEL_ERROR;
	m_nMaxLogFileNumber = LOG_MAX_SAVE_NUM;
	m_nMaxLogFileSize = MAXLOGFILESIZE;
	InitializeCriticalSection(&m_WriteLogMutex);
}

CLogMgr::~CLogMgr()
{ 
	DeleteCriticalSection(&m_WriteLogMutex);
}

// ��������
CLogMgr* CLogMgr::Instance()
{
	if(g_pInsLogMgr == NULL)
	{
		new CLogMgr();
	}

	return g_pInsLogMgr;
}

// �ͷ�
void CLogMgr::Release()
{
	if(g_pInsLogMgr != NULL)
	{
		delete g_pInsLogMgr;
		g_pInsLogMgr = NULL;
	}
}

// ������־�ļ���
void CLogMgr::SetLogFile(CString strLogFile)
{
	m_strLogFile = strLogFile;

	// �����־·��Ϊ��,������Ϊ��־�ļ�����Ŀ¼
	if(m_strLogPath.IsEmpty())
	{
		CString szPath = strLogFile;
		int nPos = szPath.ReverseFind('\\');
		if(nPos >= 0)
		{
			m_strLogPath = szPath.Left(nPos+1);
			// ��ȡ��־�ļ��������ֲ���(��������׺)
			m_strLogFileName = m_strLogFile;
			m_strLogFileName.Delete(0, nPos+1);
			int nPosDot = m_strLogFileName.ReverseFind('.');
			if(nPosDot > 0)
			{
				m_strLogFileName = m_strLogFileName.Left(nPosDot);
			}
		}
	}

	if(!m_strLogFile.IsEmpty() && !m_strLogPath.IsEmpty())
	{
		m_bLogEnable = TRUE;
	}
}

// ��¼��־
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

// ��¼��־
int CLogMgr::LogEventArgs(int nLevel, LPCTSTR lpFormat, va_list argp)
{
	const int nBufLen = MAX_PATH * 2;
	TCHAR szBuf[nBufLen];

	int nStrLen=_vsntprintf_s(szBuf, nBufLen-1, lpFormat, argp);
	if(nStrLen>0) szBuf[nStrLen] = 0;

	FILE* lpFile = _tfopen(m_strLogFile, _T("a+"));
	if ( lpFile != NULL )
	{
		// ��ȡ�ļ���С
		int nResult = -1;
		struct _stat FileBuff;
		nResult = T_STAT(m_strLogFile, &FileBuff);
		if (0 != nResult)
		{
			return -1;	// ��ȡ�ļ���Сʧ��
		}

		long lSize = FileBuff.st_size;

		// �ļ������趨��С��Ҫ����ת����Ĭ�����1MB
		if (lSize > m_nMaxLogFileSize)
		{
			fclose(lpFile);

			// ������־�ļ�ת����ת��ʱ��ֻ֤����ָ�������ı����ļ�
			FileReName(m_strLogFile, m_strLogFileName + _T(".") + LOG_CONVEY_FILE_NAME);

			// ɾ�������ļ�(���ͨ��������ʱ��˳��ɾ��,�������µ������ļ�)
			FileConveySave(m_strLogPath, m_strLogFileName + _T(".") + LOG_CONVEY_RULE, m_nMaxLogFileNumber);

			// ɾ�������ļ�
			//_tunlink(m_strLogFile + _T(".bak"));
			// �������ļ���Ϊ�����ļ���
			//_trename(m_strLogFile, m_strLogFile + _T(".bak"));

			// �����ļ�
			lpFile = _tfopen(m_strLogFile, _T("w+"));//,ccs=UTF-8"));
			if (lpFile == NULL)
			{
				return -2;	// �򿪿�д�����ļ�ʧ��
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

// �������У������ļ�����ʱ���������
void CLogMgr::sort()
{
	time_t tem;
	TCHAR temfilename[_MAX_FILE_PATH];
	memset(temfilename, 0x00, _MAX_FILE_PATH);

	for(int i = 0; i < m_nSaveIndex - 1; i++)
		for(int j = i + 1; j < m_nSaveIndex; j++)
		{
			if(m_szFileCreateTime[i] > m_szFileCreateTime[j])
			{
				tem = m_szFileCreateTime[j];
				m_szFileCreateTime[j] = m_szFileCreateTime[i];
				m_szFileCreateTime[i] = tem;
				T_STRNCPY(temfilename, m_szFileName[j], _MAX_FILE_PATH);
				T_STRNCPY(m_szFileName[j],m_szFileName[i], _MAX_FILE_PATH);
				T_STRNCPY(m_szFileName[i], temfilename, _MAX_FILE_PATH);
			}
		}
}

// ���ñ���·��, ���·���Ϸ���
bool CLogMgr::SetInitDir(const TCHAR *dir)
{
	int nLen = 0;

	//�ж�Ŀ¼�Ƿ����
	if (T_ACCESS(dir,0) != 0)
	{
		return false;
	}
	// ����Ŀ¼·��,���Ŀ¼�����һ����ĸ����'\',����������һ��'\'
	memset(m_szInitDir, 0x00, _MAX_FILE_PATH*sizeof(TCHAR));
	memcpy(m_szInitDir, dir, T_STRBYTE(dir));
	nLen=T_STRLEN(m_szInitDir);

	if (m_szInitDir[nLen - 1] != _T('\\'))
	{
		T_STRCAT(m_szInitDir,_T("\\"));
	}

	return true;
}

// �����������ļ�����
bool CLogMgr::ProcessFile(const TCHAR *filename,time_t createtime)
{
	// ���ļ������浽�����У��ж�����ļ����������뱣�������
	// �����ļ����ɵ�ʱ����ļ���������ɾ���������ɵ��ļ�
	// �������ص��������������ļ��������ﲻ��Ҫ��������
	// ���������ļ��Ĵ���ʱ��Ϊ���ݣ����ȴ��������ȷ���
	time_t filecreatetime;

	filecreatetime = createtime;
	// С�ڱ�����Ŀʱ��ɾ��
	if (m_nSaveIndex < MAX_MAINTENANCE_LOG_NUM)
	{
		T_SNPRINTF(m_szFileName[m_nSaveIndex],(MAX_MAINTENANCE_LOG_NUM - 1), _T("%s"), filename);
		m_szFileCreateTime[m_nSaveIndex] = filecreatetime;
		m_nSaveIndex++;
	}
	else // �������ά������������ɾ�����紴������־
	{
		// ����
		sort();
		// ɾ���ļ�
		_tunlink(m_szFileName[0]);
		T_SNPRINTF(m_szFileName[0], (MAX_MAINTENANCE_LOG_NUM - 1), _T("%s"), filename);
		m_szFileCreateTime[0] = filecreatetime;
	}

	return true;
}

// �����ļ��Լ����ļ��У����ﲻ�������ļ���
bool CLogMgr::BrowseDir(const TCHAR *dir,const TCHAR *filespec)
{
	// �����ļ�
	CFileFind logFind;
	CString strDir = dir;
	strDir.Format(_T("%s\\%s"), dir, filespec);

	try
	{
		// ���ҵ�һ���ļ�
		BOOL IsFinded=(BOOL)logFind.FindFile(strDir); 
		while(IsFinded) 
		{  
			IsFinded=(BOOL)logFind.FindNextFile();
			if(!logFind.IsDots()) 
			{ 
				TCHAR foundFileName[_MAX_FILE_PATH]; 
				memset(foundFileName, 0, _MAX_FILE_PATH);

				CString strFileName = logFind.GetFileName();
				T_STRNCPY(foundFileName, strFileName.GetBuffer(_MAX_FILE_PATH), (_MAX_FILE_PATH - 1));
				strFileName.ReleaseBuffer();

				if(logFind.IsDirectory()) // �����Ŀ¼������
				{ 
					continue;
				} 
				else // ���ҵ��ļ�
				{ 
					TCHAR filename[_MAX_FILE_PATH];
					memset(filename, 0x00, _MAX_FILE_PATH*sizeof(TCHAR));
					T_STRCPY(filename,dir);
					T_STRCAT(filename,foundFileName);

					CTime lastWriteTime;
					logFind.GetLastWriteTime(lastWriteTime);
					if (!ProcessFile(filename, (time_t)lastWriteTime.GetTime()))
						return false;
				} 
			} 
		} 
		logFind.Close(); 
	}
	catch (...)
	{
		return false;
	}

	return true;
}

// ʵ���ļ�ת������
// const char *dir;      // ת���ļ�Ŀ¼
// const char *filespec; // ת���ļ�����
// const int nSaveNum;   // �����ļ���Ŀ
bool CLogMgr::FileConveySave(const TCHAR *dir,const TCHAR *filespec,const int nSaveNum)
{
	// ��ʼ��
	memset(m_szFileName, 0x00, sizeof(m_szFileName));
	memset(m_szFileCreateTime, 0x00, sizeof(m_szFileCreateTime));
	m_nSaveIndex = 0;

	bool bRes = false;

	// ���ñ���Ŀ¼
	bRes = SetInitDir(dir);
	if (!bRes)
	{
		// ת��ʧ��
		return false;
	}
	
	// �����ļ�,���ҽ�������ļ�ɾ����(�����ڱ��������ļ�)
	bRes = BrowseDir(m_szInitDir, filespec);
	if (!bRes)
	{
		// ����ʧ��
		return false;
	}

	// ����
	if (m_nSaveIndex > nSaveNum)
	{
		sort();

		// ѭ��ɾ��������ļ�
		for(int i = 0; i < m_nSaveIndex - nSaveNum; i++)
		{
			_tunlink(m_szFileName[i]);
		}
	}

	return true;
}

// �ļ�������
// const char *pszSrcFileName; // Դ�ļ�����·��
// const char* pszDesFileName; // Ŀ���ļ���
bool CLogMgr::FileReName(const TCHAR *pszSrcFileName, const TCHAR* pszDesFileName)
{
	int  nLen = 0;
	TCHAR szPath[_MAX_FILE_PATH];
	TCHAR szDir[_MAX_FILE_PATH];
	TCHAR szDrive[_MAX_FILE_PATH];
	TCHAR szDesPath[_MAX_FILE_PATH];

	memset(szPath, 0x00, _MAX_FILE_PATH);
	memset(szDir, 0x00, _MAX_FILE_PATH);
	memset(szDrive, 0x00, _MAX_FILE_PATH);
	memset(szDesPath, 0x00, _MAX_FILE_PATH);

	// �����ļ�·��
	_tsplitpath(pszSrcFileName, szDrive, szDir, NULL, NULL);

	// ���Ŀ���ļ�·��
	T_SNPRINTF(szPath, (_MAX_FILE_PATH - 1), _T("%s%s"),szDrive,szDir);

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\'
	nLen=T_STRLEN(szPath);
	if (szPath[nLen - 1] != _T('\\'))
	{
		T_STRCAT(szPath,_T("\\"));
	}

	// ��ȡ��ǰϵͳʱ��
	SYSTEMTIME oCurrTime;
	GetLocalTime(&oCurrTime);
	TCHAR szCurrTime[32];
	memset(szCurrTime, 0x00, 32);
	T_SNPRINTF(szCurrTime, 31, _T("%02d-%02d-%02d-%02d-%02d-%02d"), oCurrTime.wYear, oCurrTime.wMonth,
		oCurrTime.wDay, oCurrTime.wHour, oCurrTime.wMinute, oCurrTime.wSecond);

	T_SNPRINTF(szDesPath, 1023, _T("%s%s.%s.log"), szPath, pszDesFileName, szCurrTime);

	// �������ļ���
	_trename(pszSrcFileName, szDesPath);

	return true; 
}
