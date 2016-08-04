#ifndef __LOG_MGR_H_
#define __LOG_MGR_H_

// ��־��ض���
#define MAXLOGFILESIZE			(1024 << 10)		// ��־�ļ���󳤶�
#define _LOG_DATABUFF			1024	// ��־���ݻ�������С
#define _MAX_FILE_PATH			1024	// ֧�ֵ�����ļ�·����
#define MAX_MAINTENANCE_LOG_NUM	1024	// ��ά������ļ���1024

//��־������
#define LOG_LEVEL_DEBUG 0x0001      // ������Ϣ
#define LOG_LEVEL_INFO	0x0002      // һ����Ϣ
#define LOG_LEVEL_ERROR 0x0004      // ������Ϣ
#define LOG_LEVEL_CRITICAL 0x0008	// ������Ϣ

// ��־�����ַ���
#define	__DEBUG		_T("DEBUG")
#define	__INFO		_T("INFO")
#define	__ERROR		_T("ERROR")
#define	__CRITICAL	_T("CRITICAL")

// ��־ת���ļ�
#define	LOG_CONVEY_FILE_NAME	"logbak"
#define	LOG_CONVEY_RULE				"logbak.*"
#define	LOG_MAX_SAVE_NUM			5

//
// ��־����
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
	CRITICAL_SECTION		m_WriteLogMutex;		// ��־ͬ����
	CString					m_strLogPath;			// ��־�ļ�·��
	CString					m_strLogFile;			// ��־�ļ���
	CString					m_strLogFileName;			// ��־�ļ���ǰ׺
	int						m_nLogLevel;			// ��־����
	BOOL					m_bLogEnable;			// �Ƿ�������־����
	int						m_nMaxLogFileSize;		// ��־�ļ����ȵ����ֵ
	int						m_nMaxLogFileNumber;	// ��󱣴���־�ļ���

	TCHAR				m_szInitDir[_MAX_FILE_PATH];	// ��־·����
	TCHAR				m_szFileName[MAX_MAINTENANCE_LOG_NUM][_MAX_FILE_PATH];	// ����ļ�������
	time_t				m_szFileCreateTime[MAX_MAINTENANCE_LOG_NUM];	// �ļ�����ʱ��,���ļ���һһ��Ӧ
	int						m_nSaveIndex;		// ����������
};

#endif	// __LOG_MGR_H_