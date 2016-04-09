#ifndef __LOG_MGR_H_
#define __LOG_MGR_H_

// 日志相关定义
#define MAXLOGFILESIZE (1024 << 10)		// 日志文件最大长度
#define _LOG_DATABUFF			1024	// 日志数据缓冲区大小

// 日志级别
#define	__DEBUG		_T("DEBUG")
#define	__ERROR		_T("ERROR")
#define	__INFO		_T("INFO")
#define	__CRITICAL	_T("CRITICAL")

// 日志转储文件名
#define	LOG_CONVEY_FILE_NAME	"LogBak"
#define	CONVEY_RULE				"LogBak.*"
#define	MAX_SAVE_NUM			5

//
// 日志管理
//
class CLogMgr
{
public:
	CLogMgr();
	~CLogMgr();
	
	static CLogMgr* Instance();
	static void Release();
	
	void SetLogFile(CString strLogFile) { m_strLogFile = strLogFile; m_bLogEnable = TRUE; }
	CString GetLogFile() { return m_strLogFile; }
	void SetLogLevel(int nLogLevel) { m_nLogLevel = nLogLevel; }
	int GetLogLevel() { return m_nLogLevel; }
	BOOL IsLogEnable() { return m_bLogEnable; }
	CRITICAL_SECTION* GetLogMutex() { return &m_WriteLogMutex; }
	

	static void LogEvent(int nLevel, LPCTSTR lpFormat, ...);
	int LogEventArgs(int nLevel, LPCTSTR lpFormat, va_list argp);

private:
	CString					m_strLogFile;			// 日志文件名
	int						m_nLogLevel;			// 日志级别
	BOOL					m_bLogEnable;			// 是否启用日志功能
	int						m_nMaxLogFileSize;		// 日志文件长度的最大值
	CRITICAL_SECTION		m_WriteLogMutex;		// 日志同步锁
};

#endif	// __LOG_MGR_H_