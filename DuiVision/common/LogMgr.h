#ifndef __LOG_MGR_H_
#define __LOG_MGR_H_

// 日志相关定义
#define MAXLOGFILESIZE			(1024 << 10)		// 日志文件最大长度
#define _LOG_DATABUFF			1024	// 日志数据缓冲区大小
#define _MAX_FILE_PATH			1024	// 支持的最大文件路径名
#define MAX_MAINTENANCE_LOG_NUM	1024	// 可维护最大文件数1024

//日志级别定义
#define LOG_LEVEL_DEBUG 0x0001      // 调试信息
#define LOG_LEVEL_INFO	0x0002      // 一般信息
#define LOG_LEVEL_ERROR 0x0004      // 错误信息
#define LOG_LEVEL_CRITICAL 0x0008	// 致命信息

// 日志级别字符串
#define	__DEBUG		_T("DEBUG")
#define	__INFO		_T("INFO")
#define	__ERROR		_T("ERROR")
#define	__CRITICAL	_T("CRITICAL")

// 日志转储文件
#define	LOG_CONVEY_FILE_NAME	"logbak"
#define	LOG_CONVEY_RULE				"logbak.*"
#define	LOG_MAX_SAVE_NUM			5

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
	
	void SetLogPath(CString strLogPath) { m_strLogPath = strLogPath; }
	CString GetLogPath() { return m_strLogPath; }
	void SetLogFile(CString strLogFile);
	CString GetLogFile() { return m_strLogFile; }
	void SetLogLevel(int nLogLevel) { m_nLogLevel = nLogLevel; }
	int GetLogLevel() { return m_nLogLevel; }
	void SetLogFileSize(int nLogFileSize) { m_nMaxLogFileSize = nLogFileSize; }
	int GetLogFileSize() { return m_nMaxLogFileSize; }
	void SetLogFileNumber(int nLogFileNumber) { m_nMaxLogFileNumber = nLogFileNumber; }
	int GetLogFileNumber() { return m_nMaxLogFileNumber; }
	BOOL IsLogEnable() { return m_bLogEnable; }
	CRITICAL_SECTION* GetLogMutex() { return &m_WriteLogMutex; }
	

	static void LogEvent(int nLevel, LPCTSTR lpFormat, ...);
	int LogEventArgs(int nLevel, LPCTSTR lpFormat, va_list argp);

protected:
	void sort();
	bool SetInitDir(const TCHAR *dir);
	bool ProcessFile(const TCHAR *filename,time_t createtime);
	bool BrowseDir(const TCHAR *dir,const TCHAR *filespec);
	bool FileConveySave(const TCHAR *dir,const TCHAR *filespec,const int nSaveNum);
	bool FileReName(const TCHAR *pszSrcFileName, const TCHAR* pszDesFileName);

private:
	CRITICAL_SECTION		m_WriteLogMutex;		// 日志同步锁
	CString					m_strLogPath;			// 日志文件路径
	CString					m_strLogFile;			// 日志文件名
	CString					m_strLogFileName;			// 日志文件名前缀
	int						m_nLogLevel;			// 日志级别
	BOOL					m_bLogEnable;			// 是否启用日志功能
	int						m_nMaxLogFileSize;		// 日志文件长度的最大值
	int						m_nMaxLogFileNumber;	// 最大保存日志文件数

	TCHAR				m_szInitDir[_MAX_FILE_PATH];	// 日志路径名
	TCHAR				m_szFileName[MAX_MAINTENANCE_LOG_NUM][_MAX_FILE_PATH];	// 存放文件名数组
	time_t				m_szFileCreateTime[MAX_MAINTENANCE_LOG_NUM];	// 文件创建时间,与文件名一一对应
	int						m_nSaveIndex;		// 保存索引号
};

#endif	// __LOG_MGR_H_