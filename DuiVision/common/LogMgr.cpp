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

// 设置日志文件名
void CLogMgr::SetLogFile(CString strLogFile)
{
	m_strLogFile = strLogFile;

	// 如果日志路径为空,则设置为日志文件所在目录
	if(m_strLogPath.IsEmpty())
	{
		CString szPath = strLogFile;
		int nPos = szPath.ReverseFind('\\');
		if(nPos >= 0)
		{
			m_strLogPath = szPath.Left(nPos+1);
			// 获取日志文件名的名字部分(不包含后缀)
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
		if (lSize > m_nMaxLogFileSize)
		{
			fclose(lpFile);

			// 进行日志文件转储，转储时保证只保存指定个数的备份文件
			FileReName(m_strLogFile, m_strLogFileName + _T(".") + LOG_CONVEY_FILE_NAME);

			// 删除多余文件(规矩通配符规则和时间顺序删除,保留最新的若干文件)
			FileConveySave(m_strLogPath, m_strLogFileName + _T(".") + LOG_CONVEY_RULE, m_nMaxLogFileNumber);

			// 删除备份文件
			//_tunlink(m_strLogFile + _T(".bak"));
			// 重命名文件名为备份文件名
			//_trename(m_strLogFile, m_strLogFile + _T(".bak"));

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

// 升序排列，按照文件创建时间进行排序
void CLogMgr::sort()
{
	time_t tem;
	char temfilename[_MAX_FILE_PATH];
	memset(temfilename, 0x00, _MAX_FILE_PATH);

	for(int i = 0; i < m_nSaveIndex - 1; i++)
		for(int j = i + 1; j < m_nSaveIndex; j++)
		{
			if(m_szFileCreateTime[i] > m_szFileCreateTime[j])
			{
				tem = m_szFileCreateTime[j];
				m_szFileCreateTime[j] = m_szFileCreateTime[i];
				m_szFileCreateTime[i] = tem;
				strncpy(temfilename, m_szFileName[j], _MAX_FILE_PATH);
				strncpy(m_szFileName[j],m_szFileName[i], _MAX_FILE_PATH);
				strncpy(m_szFileName[i], temfilename, _MAX_FILE_PATH);
			}
		}
}

// 设置遍历路径, 检查路径合法性
bool CLogMgr::SetInitDir(const char *dir)
{
	int nLen = 0;

	//判断目录是否存在
	if (_access(dir,0) != 0)
	{
		return false;
	}
	
	// 保存目录路径
	memset(m_szInitDir, 0x00, _MAX_FILE_PATH);
	memcpy(m_szInitDir, dir, strlen(dir));

	//如果目录的最后一个字母不是'\',则在最后加上一个'\'
	nLen=strlen(m_szInitDir);
	if (m_szInitDir[nLen - 1] != '\\')
	{
		strcat(m_szInitDir,"\\");
	}

	return true;
}

// 将遍历到的文件保存
bool CLogMgr::ProcessFile(const char *filename,time_t createtime)
{
	// 把文件名保存到数组中，判断如果文件个数多于须保存个数，
	// 安照文件生成的时间对文件进行排序，删除最早生成的文件
	// 遍历返回的是最先生产的文件，故这里不需要进行排序
	// 返回是以文件的创建时间为依据，最先创建的最先返回
	time_t filecreatetime;

	filecreatetime = createtime;
	// 小于保存数目时不删除
	if (m_nSaveIndex < MAX_MAINTENANCE_LOG_NUM)
	{
		_snprintf(m_szFileName[m_nSaveIndex],(MAX_MAINTENANCE_LOG_NUM - 1), "%s", filename);
		m_szFileCreateTime[m_nSaveIndex] = filecreatetime;
		m_nSaveIndex++;
	}
	else // 超过最大维护数，先排序删除最早创建的日志
	{
		// 排序
		sort();
		// 删除文件
		_unlink(m_szFileName[0]);
		_snprintf(m_szFileName[0], (MAX_MAINTENANCE_LOG_NUM - 1), "%s", filename);
		m_szFileCreateTime[0] = filecreatetime;
	}

	return true;
}

// 遍历文件以及子文件夹，这里不处理子文件夹
bool CLogMgr::BrowseDir(const char *dir,const char *filespec)
{
	// 遍历文件
	CFileFind logFind;
	ostringstream ostrDir;
	ostrDir<<dir<<"\\"<<filespec;

	try
	{
		// 查找第一个文件
		BOOL IsFinded=(BOOL)logFind.FindFile(ostrDir.str().c_str()); 
		while(IsFinded) 
		{  
			IsFinded=(BOOL)logFind.FindNextFile();
			if(!logFind.IsDots()) 
			{ 
				char foundFileName[_MAX_FILE_PATH]; 
				memset(foundFileName, 0, _MAX_FILE_PATH);

				CString strFileName = logFind.GetFileName();
				strncpy(foundFileName, strFileName.GetBuffer(_MAX_FILE_PATH), (_MAX_FILE_PATH - 1)); 
				strFileName.ReleaseBuffer();

				if(logFind.IsDirectory()) // 如果是目录不处理
				{ 
					continue;
				} 
				else // 查找到文件
				{ 
					char filename[_MAX_FILE_PATH];
					memset(filename, 0x00, _MAX_FILE_PATH);
					strcpy(filename,dir);
					strcat(filename,foundFileName);

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

// 实现文件转储功能
// const char *dir;      // 转储文件目录
// const char *filespec; // 转储文件规则
// const int nSaveNum;   // 保存文件数目
bool CLogMgr::FileConveySave(const char *dir,const char *filespec,const int nSaveNum)
{
	// 初始化
	memset(m_szFileName, 0x00, sizeof(m_szFileName));
	memset(m_szFileCreateTime, 0x00, sizeof(m_szFileCreateTime));
	m_nSaveIndex = 0;

	bool bRes = false;

	// 设置遍历目录
	bRes = SetInitDir(dir);
	if (!bRes)
	{
		// 转储失败
		return false;
	}
	
	// 遍历文件,并且将多余的文件删除掉(即大于保存数的文件)
	bRes = BrowseDir(m_szInitDir, filespec);
	if (!bRes)
	{
		// 遍历失败
		return false;
	}

	// 排序
	if (m_nSaveIndex > nSaveNum)
	{
		sort();

		// 循环删除多余旧文件
		for(int i = 0; i < m_nSaveIndex - nSaveNum; i++)
		{
			_unlink(m_szFileName[i]);
		}
	}

	return true;
}

// 文件重命名
// const char *pszSrcFileName; // 源文件绝对路径
// const char* pszDesFileName; // 目标文件名
bool CLogMgr::FileReName(const char *pszSrcFileName, const char* pszDesFileName)
{
	int  nLen = 0;
	char szPath[_MAX_FILE_PATH];
	char szDir[_MAX_FILE_PATH];
	char szDrive[_MAX_FILE_PATH];
	char szDesPath[_MAX_FILE_PATH];

	memset(szPath, 0x00, _MAX_FILE_PATH);
	memset(szDir, 0x00, _MAX_FILE_PATH);
	memset(szDrive, 0x00, _MAX_FILE_PATH);
	memset(szDesPath, 0x00, _MAX_FILE_PATH);

	// 分离文件路径
	_splitpath(pszSrcFileName, szDrive, szDir, NULL, NULL);

	// 获得目标文件路径
	_snprintf(szPath, (_MAX_FILE_PATH - 1), "%s%s",szDrive,szDir);

	//如果目录的最后一个字母不是'\',则在最后加上一个'\'
	nLen=strlen(szPath);
	if (szPath[nLen - 1] != '\\')
	{
		strcat(szPath,"\\");
	}

	// 获取当前系统时间
	SYSTEMTIME oCurrTime;
	GetLocalTime(&oCurrTime);
	char szCurrTime[32];
	memset(szCurrTime, 0x00, 32);
	_snprintf(szCurrTime, 31, "%02d-%02d-%02d-%02d-%02d-%02d", oCurrTime.wYear, oCurrTime.wMonth,
		oCurrTime.wDay, oCurrTime.wHour, oCurrTime.wMinute, oCurrTime.wSecond);

	_snprintf(szDesPath, 1023, "%s%s.%s.log", szPath, pszDesFileName, szCurrTime);

	// 重命名文件名
	rename(pszSrcFileName, szDesPath);

	return true; 
}
