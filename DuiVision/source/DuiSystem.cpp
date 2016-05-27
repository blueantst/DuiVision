#include "StdAfx.h"
#include "DuiSystem.h"
#include <tchar.h>

template<> DuiSystem* Singleton<DuiSystem>::ms_Singleton = 0;

static DuiSystem* g_pIns = NULL;
static UINT g_nIDTemplate = 0;
static CString g_strRootPath = _T("");

static ULONG_PTR gdiplusToken;
static GdiplusStartupInput gdiplusStartupInput;

DuiSystem::DuiSystem(HINSTANCE hInst, DWORD dwLangID, CString strResourceFile, UINT uAppID, UINT nIDTemplate, CString strStyle, CString strLogFile)
    :m_hInst(hInst), m_uAppID(uAppID)
{
	g_pIns = this;
	m_dwLangID = dwLangID;
	if((g_nIDTemplate == 0) && (nIDTemplate != 0))
	{
		g_nIDTemplate = nIDTemplate;
	}
	m_pNotifyMsgBox = NULL;
	m_strCurStyle = strStyle;
	if(strResourceFile.IsEmpty())
	{
		m_strResourceFile = _T("xml\\resource.xml");
	}else
	{
		m_strResourceFile = strResourceFile;
	}
	m_hResourceZip = NULL;

	/*
	// TinyXml����Ϊ��ѹ���ո�ģʽ��Ĭ����ѹ���ո񣬻ᵼ�³���һ���Ŀո����ʱ��ת��Ϊһ���ո�
	TiXmlBase::SetCondenseWhiteSpace(false);
	*/

	ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);

	/*
	���ָ����LogFile�������Դ�ļ��е�logfile��
	�ȸ�ֵm_strLogFile
	*/
	m_strLogFile = strLogFile;

    createSingletons();

	// ��ʼ��COM��
	HRESULT hr = CoInitialize(NULL);
	if( FAILED(hr) )
	{
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("CoInitialize failed"));
	}

	// ���ؿؼ���
	LoadDuiControls();
}

DuiSystem::~DuiSystem(void)
{
	// �����¼��������
	for (size_t i = 0; i < m_vecDuiHandler.size(); i++)
	{
		CDuiHandler* pDuiHandler = m_vecDuiHandler.at(i);
		if (pDuiHandler)
		{
			delete pDuiHandler;
		}		
	}

	// ����DUI�Ի���
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		if (pDlgBase)
		{
			delete pDlgBase;
		}		
	}

	// �ͷŸ�����Դ��
	m_mapCfgPool.RemoveAll();
	m_mapStylePool.RemoveAll();
	m_mapXmlPool.RemoveAll();
	m_mapSkinPool.RemoveAll();
	m_mapStringPool.RemoveAll();
	m_mapFontPool.RemoveAll();

	// �ͷ�ͼƬ����
	ClearAllCachedMemFile();

	if(m_hResourceZip != NULL)
	{
		CloseZip(m_hResourceZip);
		m_hResourceZip = NULL;
	}

	// ����Trayͼ��
	DelTray();

	//if(m_rich20) FreeLibrary(m_rich20);
	//m_funCreateTextServices=NULL;
	// �ͷ�richedit��
	CDuiRichEdit::ReleaseTextService();

	// �ͷ�COM��
	CoUninitialize();

	// �ͷſؼ���
	ReleaseDuiControls();

	// ���������ô˺����ر�GDI+������ᵼ�¹ر�GDI+֮�󻹵���GDI+�ĺ���������쳣
	destroySingletons();
}

// �жϵ����Ƿ��Ѿ�����
BOOL DuiSystem::HaveInstance()
{
	return (g_pIns != NULL);
}

// ��������
DuiSystem* DuiSystem::Instance()
{
	if(g_pIns == NULL)
	{
		new DuiSystem(NULL);
	}

	return g_pIns;
}

// �ͷ�
void DuiSystem::Release()
{
	if(g_pIns != NULL)
	{
		delete g_pIns;
		g_pIns = NULL;
	}
}

void DuiSystem::createSingletons()
{
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// ������Դ�ļ�
	LoadResource();

	// ��ʼ����־
	InitLog();

	// ��������������߳�
	m_TaskMsg.Startup();
}

void DuiSystem::destroySingletons()
{
	// ֹͣ����������߳�
	m_TaskMsg.Shutdown();

	// ֹͣ��־
	DoneLog();

	//�ر�gdiplus�Ļ���
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

// ��ȡ����ϵͳ�汾�����汾��
int DuiSystem::GetOSMajorVersion()
{
	// ��ȡ����ϵͳ�汾��Ϣ
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!( GetVersionEx ((OSVERSIONINFO *) &osvi)))
	{
		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx ((OSVERSIONINFO *) &osvi))
		{
			return 0;
		}
	}

	return osvi.dwMajorVersion;
}

// ��ȡ��ǰ����(�ַ�������)
CString DuiSystem::GetLanguage()
{
	DWORD dwLangID = DuiSystem::Instance()->GetCurrentLanguage();
	if(dwLangID == 0)
	{
		// �������IDΪ0��ʾ��ȡ��ǰϵͳ������
		dwLangID = ::GetSystemDefaultLangID();
	}
	switch(dwLangID)
	{
	case LANGUAGE_PAGE_ENGLISH:
		return _T("en-us");break;	// Ӣ��
	case LANGUAGE_PAGE_CHINESE:
		return _T("zh-cn");break;	// ��������
	case LANGUAGE_PAGE_CHINESE_TW:
	case LANGUAGE_PAGE_CHINESE_HK:
	case LANGUAGE_PAGE_CHINESE_SGP:
		return _T("zh-tw");break;	// ��������
	}

	return _T("en-us");	// Ĭ��ΪӢ��
}

// ��ȡ��ǰ����
DWORD DuiSystem::GetCurrentLanguage()
{
	return m_dwLangID;
}

// ���õ�ǰ����
void DuiSystem::SetCurrentLanguage(DWORD dwLangID)
{
	m_dwLangID = dwLangID;
}

// ���ø�Ŀ¼
void DuiSystem::SetRootPath(CString strPath)
{
	g_strRootPath = strPath;
}

// ȡ����Ŀ¼
CString DuiSystem::GetExePath()
{
	if(!g_strRootPath.IsEmpty())
	{
		return g_strRootPath;
	}

	TCHAR szFullPath[MAX_PATH];
	TCHAR szdrive[_MAX_DRIVE];
	TCHAR szdir[_MAX_DIR];
	::GetModuleFileName(NULL, szFullPath, MAX_PATH);
	_tsplitpath(szFullPath, szdrive, szdir, NULL, NULL);

	CString szPath;
	szPath.Format(_T("%s%s"), szdrive, szdir);
	//szPath = szPath.Left(szPath.GetLength() - 1);

	return szPath;
}

// ��ȡSkinĿ¼
CString DuiSystem::GetSkinPath()
{
	return GetExePath();
}

// ��ȡXMLĿ¼
CString DuiSystem::GetXmlPath()
{
	return GetExePath() + _T("xml\\");
}

// ·����׼��
BOOL DuiSystem::PathCanonicalize(CString& strPath)
{
	TCHAR strOut[MAX_PATH];
	memset(strOut, 0, MAX_PATH*sizeof(TCHAR));
	if(::PathCanonicalize(strOut, strPath))
	{
		strPath = strOut;
		return TRUE;
	}
	return FALSE;
}

// ������Դ
BOOL DuiSystem::LoadResource()
{
	// ���ͷŸ�����Դ��
	m_mapCfgPool.RemoveAll();
	m_mapStylePool.RemoveAll();
	m_mapXmlPool.RemoveAll();
	m_mapSkinPool.RemoveAll();
	m_mapStringPool.RemoveAll();
	m_mapFontPool.RemoveAll();

	// �ͷ�ͼƬ����
	ClearAllCachedMemFile();

	// ����zip��Դ�ļ�
	CString strResFile = GetExePath() + m_strResourceFile;
	if(strResFile.Find(_T(".ui")) != -1)	// ���ļ�����zip��
	{
		// ����Ǻ�׺Ϊui���ļ�,��ʾ��ZIP�ļ�,�������ԴZIP�ļ�
		if(GetFileAttributes(strResFile) != 0xFFFFFFFF)
		{
			if(m_hResourceZip != NULL)
			{
				CloseZip(m_hResourceZip);
				m_hResourceZip = NULL;
			}
			// ��zip�ļ���������ص��ڴ���,��ͨ���ڴ�ķ�ʽ��ѹ,����������߼����ٶ�
			CFile file;
			DWORD dwSize = 0;
			char *pData = NULL;
			if(file.Open(strResFile, CFile::modeRead | CFile::shareDenyWrite))
			{
				dwSize = (DWORD)file.GetLength();
				HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_NODISCARD, dwSize );
				if(hGlobal)
				{
					pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
					if(pData)
					{
						file.Read( pData, dwSize );
					}
					GlobalUnlock( hGlobal );
				}
				file.Close();
			}
			if(pData)
			{
				m_hResourceZip = OpenZip((void*)pData, dwSize, ZIP_MEMORY);
			}
			//m_hResourceZip = OpenZip((void*)(LPCTSTR)strResFile, 0, ZIP_FILENAME);
			if(m_hResourceZip != NULL)
			{
				// ������ص�ZIP��Դ����ʹ�þ���·������XML��Դ�����ļ�
				strResFile = _T("xml\\resource.xml");			
			}
		}else
		{
			// zip��Դ�ļ�������,������ΪĬ�ϵ�xml�ļ�
			m_strResourceFile = _T("xml\\resource.xml");
			strResFile = m_strResourceFile;
		}
	}else
	if(m_strResourceFile.Find(_T("res:")) == 0)	// ����Դ�м���zip��
	{
		strResFile = m_strResourceFile;
		strResFile.Delete(0, 4);
		UINT nID = _ttoi(strResFile);	// res:����ı�ʾ��ԴID
		if(nID != 0)
		{
			// ��ȡ��Դ
			HINSTANCE hInst = AfxGetResourceHandle();  
			HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID), _T("UI"));
			if(hRsrc != NULL)
			{
				DWORD dwResFileSize = SizeofResource(hInst, hRsrc);  
				BYTE* lpRsrc = (BYTE*)::LoadResource(hInst, hRsrc);

				if(m_hResourceZip != NULL)
				{
					CloseZip(m_hResourceZip);
					m_hResourceZip = NULL;
				}
				m_hResourceZip = OpenZip((void*)lpRsrc, dwResFileSize, ZIP_MEMORY);
				if(m_hResourceZip != NULL)
				{
					// ������ص�ZIP��Դ����ʹ�þ���·������XML��Դ�����ļ�
					strResFile = _T("xml\\resource.xml");			
				}
			}else
			{
				// zip��Դ������,������ΪĬ�ϵ�xml�ļ�
				m_strResourceFile = _T("xml\\resource.xml");
				strResFile = m_strResourceFile;
			}
		}else
		{
			// zip��Դ������,������ΪĬ�ϵ�xml�ļ�
			m_strResourceFile = _T("xml\\resource.xml");
			strResFile = m_strResourceFile;
		}
	}else
	{
		strResFile = m_strResourceFile;
	}
	
	return LoadResourceXml(strResFile, m_strCurStyle);
}

// ����XML��Դ�ļ�
BOOL DuiSystem::LoadResourceXml(CString strResFile, CString strStyle)
{
	m_strCurStyle = strStyle;

	// ������Դ�ļ�
	DuiXmlDocument xmlDoc;
	DuiXmlParseResult xmlResult;
	if(m_hResourceZip != NULL)
	{
		// ��ʹ��zip�ļ��������,Ҳ����ʹ��Ŀ¼�е��ļ�
		if(GetFileAttributes(GetExePath() + strResFile) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			xmlResult = xmlDoc.load_file(GetExePath() + strResFile);
		}else
		if(GetFileAttributes(GetXmlPath() + strResFile) != 0xFFFFFFFF)	// ��xml·����ʼ����
		{
			xmlResult = xmlDoc.load_file(GetXmlPath() + strResFile);
		}else
		if(GetFileAttributes(strResFile) != 0xFFFFFFFF)	// ����·������
		{
			xmlResult = xmlDoc.load_file(strResFile);
		}else
		{
			DWORD dwSize = 0;
			BYTE* pByte = LoadZipFile(strResFile, dwSize);
			if(pByte != NULL)
			{
				xmlResult = xmlDoc.load(CA2T((char*)pByte, CP_UTF8), NULL);
				//delete[] pByte;
			}else
			{
				return FALSE;
			}
		}		
	}else
	{
		if(GetFileAttributes(GetExePath() + strResFile) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			xmlResult = xmlDoc.load_file(GetExePath() + strResFile);
		}else
		if(GetFileAttributes(GetXmlPath() + strResFile) != 0xFFFFFFFF)	// ��xml·����ʼ����
		{
			xmlResult = xmlDoc.load_file(GetXmlPath() + strResFile);
		}else
		if(GetFileAttributes(strResFile) != 0xFFFFFFFF)	// ����·������
		{
			xmlResult = xmlDoc.load_file(strResFile);
		}
	}
	if(xmlResult)
	{
			DuiXmlNode pResRoot = xmlDoc.child(_T("root"));
			for (DuiXmlNode pResElem = pResRoot.child(_T("res")); pResElem; pResElem = pResElem.next_sibling(_T("res")))
			{
				CString strType = pResElem.attribute(_T("type")).value();
				if(strType == _T("res"))	// ��Դ�ļ�
				{
					CString strLang = pResElem.attribute(_T("lang")).value();
					if(strLang.IsEmpty() || (strLang == DuiSystem::GetLanguage()))
					{
						// ������Դ�ļ�
						CString strFile = pResElem.attribute(_T("file")).value();
						CString strFileU = GetExePath() + strFile;
						if(m_hResourceZip != NULL)
						{
							strFileU = strFile;
						}
						LoadResourceXml(strFileU, strStyle);
					}
				}else
				if(strType == "cfg")	// ȫ������
				{
					CString strName = pResElem.attribute(_T("name")).value();
					CString strValue = pResElem.attribute(_T("value")).value();
					//m_strLogFile��Ϊ��ʱ����logfile��
					if (strName==_T("logfile") && !m_strLogFile.IsEmpty())
					{
						m_mapCfgPool.SetAt(strName, m_strLogFile);
					}else
					{
						m_mapCfgPool.SetAt(strName, strValue);
					}
					// ���DuiSystemδ���õ�ǰ������,�����ͨ��defaultStyle������������ǰ���
					if(m_strCurStyle.IsEmpty() && (strName == _T("defaultStyle")))
					{
						strStyle = strValue;
						m_strCurStyle = strValue;
					}
				}else
				if(strType == _T("style"))	// �����
				{
					CString strName = pResElem.attribute(_T("name")).value();
					CString strValue = pResElem.attribute(_T("value")).value();
					m_mapStylePool.SetAt(strName, strValue);
				}else
				if(strType == _T("xml"))	// XML�ļ�����
				{
					CString strStyle = pResElem.attribute(_T("style")).value();
					CString strName = pResElem.attribute(_T("name")).value();
					CString strFile = pResElem.attribute(_T("file")).value();
					if(strStyle.IsEmpty() || (strStyle == m_strCurStyle))
					{
						m_mapXmlPool.SetAt(strName, strFile);
					}
				}else
				if(strType == _T("img"))	// ͼ��
				{
					CString strStyle = pResElem.attribute(_T("style")).value();
					CString strName = pResElem.attribute(_T("name")).value();
					CString strFile = pResElem.attribute(_T("file")).value();
					if(strStyle.IsEmpty() || (strStyle == m_strCurStyle))
					{
						m_mapSkinPool.SetAt(strName, strFile);
					}
				}else
				if(strType == _T("str"))	// �ַ���
				{
					CString strLang = pResElem.attribute(_T("lang")).value();
					if(strLang.IsEmpty() || (strLang == DuiSystem::GetLanguage()))
					{
						// ���δָ�����Ի�ָ�������Բ��Һ͵�ǰ������ͬ��������ַ�����Դ
						CString strStyle = pResElem.attribute(_T("style")).value();
						CString strName = pResElem.attribute(_T("name")).value();
						CString strValue = pResElem.attribute(_T("value")).value();
						if(strStyle.IsEmpty() || (strStyle == m_strCurStyle))
						{
							m_mapStringPool.SetAt(strName, strValue);
						}
					}
				}else
				if(strType == _T("font"))	// ����
				{
					CString strLang = pResElem.attribute(_T("lang")).value();
					CString strName = pResElem.attribute(_T("name")).value();
					CString strFont = pResElem.attribute(_T("font")).value();
					int nFontWidth = _ttoi(pResElem.attribute(_T("size")).value());
					CString strOS = pResElem.attribute(_T("os")).value();
					CString strBold = pResElem.attribute(_T("bold")).value();
					BOOL bBold = FALSE;
					if(pResElem.attribute(_T("bold")).value())
					{
						bBold = (_tcscmp(pResElem.attribute(_T("bold")).value(), _T("true")) == 0);
					}
					BOOL bItalic = FALSE;
					if(pResElem.attribute(_T("italic")).value())
					{
						bItalic = (_tcscmp(pResElem.attribute(_T("italic")).value(), _T("true")) == 0);
					}
					BOOL bUnderline = FALSE;
					if(pResElem.attribute(_T("underline")).value())
					{
						bUnderline = (_tcscmp(pResElem.attribute(_T("underline")).value(), _T("true")) == 0);
					}
					BOOL bStrikeout = FALSE;
					if(pResElem.attribute(_T("strikeout")).value())
					{
						bStrikeout = (_tcscmp(pResElem.attribute(_T("strikeout")).value(), _T("true")) == 0);
					}
					FontStyle fontStyle = FontStyleRegular;
					if(bBold)
					{
						fontStyle = FontStyle((int)fontStyle + (int)FontStyleBold);
					}
					if(bItalic)
					{
						fontStyle = FontStyle((int)fontStyle + (int)FontStyleItalic);
					}
					if(bUnderline)
					{
						fontStyle = FontStyle((int)fontStyle + (int)FontStyleUnderline);
					}
					if(bStrikeout)
					{
						fontStyle = FontStyle((int)fontStyle + (int)FontStyleStrikeout);
					}
					DuiFontInfo fontInfo;
					fontInfo.strName = strName;
					// ��������һ�¹���,���ڲ�֧�ֵ�������ȱʡ�����滻
					fontInfo.strFont = DuiSystem::GetDefaultFont(strFont);
					fontInfo.nFontWidth = nFontWidth;
					fontInfo.fontStyle = fontStyle;
					fontInfo.strOS = strOS;
					if(!fontInfo.strOS.IsEmpty())
					{
						// ���OS���Էǿ�,���жϵ�ǰ����ϵͳ�Ƿ����OS����
						if(CheckOSName(fontInfo.strOS))
						{
							m_mapFontPool.SetAt(strName, fontInfo);
						}
					}else
					{
						m_mapFontPool.SetAt(strName, fontInfo);
					}
				}
			}
	}else
	{
		return FALSE;
	}

	return TRUE;
}

// ����ZIP��Դ�ļ�
BYTE* DuiSystem::LoadZipFile(CString strFile, DWORD& dwSize)
{
	dwSize = 0;
	if(m_hResourceZip == NULL)
	{
		return NULL;
	}

	// Zip�ļ��е�·����ʹ��/
	strFile.Replace(_T("\\"), _T("/"));

	// �ж��ļ��Ƿ��Ѿ�������,����Ѿ�����ͷ��ػ��������
	MemFileInfo memFile;
	memFile.dwSize = 0;
	memFile.pByte = NULL;
	if(GetCachedMemFile(strFile, memFile))
	{
		dwSize = memFile.dwSize;
		return memFile.pByte;
	}

	BYTE* pByte = NULL;
	ZENTRY ze;
	int i;
	if(FindZipItem(m_hResourceZip, (LPCTSTR)strFile, true, &i, &ze) == 0)
	{
		dwSize = ze.unc_size;
		if( dwSize == 0 )
		{
			DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("Load zip file %s failed, file is empty"), strFile);
			return NULL;
		}
		if ( dwSize > 4096*1024 )
		{
			DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("Load zip file %s failed, file too large"), strFile);
			return NULL;
		}
		pByte = new BYTE[ dwSize ];
		int res = UnzipItem(m_hResourceZip, i, pByte, dwSize, 3);
		if( res != 0x00000000 && res != 0x00000600)
		{
			delete[] pByte;
			DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("Load zip file %s failed, could not unzip file"), strFile);
			return NULL;
		}

		// ���»�������
		memFile.dwSize = dwSize;
		memFile.pByte = pByte;
		SetCachedMemFile(strFile, memFile);

		return pByte;
	}else
	{
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("Load zip file %s failed, not found file"), strFile);
	}

	return NULL;
}

// ��ȡ������ڴ��ļ���Ϣ
BOOL DuiSystem::GetCachedMemFile(CString strFile, MemFileInfo& memFile)
{
	strFile.MakeLower();
	return m_mapMemFileCatch.Lookup(strFile, memFile);
}

// ���û�����ڴ��ļ���Ϣ
void DuiSystem::SetCachedMemFile(CString strFile, MemFileInfo& memFile)
{
	strFile.MakeLower();
	m_mapMemFileCatch.SetAt(strFile, memFile);
}

// ��������ڴ��ļ�����
void DuiSystem::ClearAllCachedMemFile()
{
	POSITION pos = m_mapMemFileCatch.GetStartPosition();
	CString szFile;
	MemFileInfo memFile;
	while(pos != NULL)
	{
		m_mapMemFileCatch.GetNextAssoc(pos, szFile, memFile);
		if(memFile.pByte != NULL)
		{
			delete[] memFile.pByte;
			memFile.pByte = NULL;
		}
	}
	m_mapMemFileCatch.RemoveAll();
}

// ����XML�ļ�,֧�ִ�zip�ļ��м���
// ��ʽ1: file-resource-name		-- ������Դ������,���ҵ��ļ���,����ԴĿ¼����Դzip�м���
// ��ʽ2: filename.xml				-- ָ��XML�ļ���,����ԴĿ¼����Դzip�м���
// ��ʽ3: c:\filename.xml			-- ָ����XML��ȫ·��
BOOL DuiSystem::LoadXmlFile(DuiXmlDocument& xmlDoc, CString strFileName)
{
	CString strXmlFile;
	DuiXmlParseResult xmlResult;
	if(strFileName.Find(_T("xml:")) == 0)	// ȥ��ǰ׺
	{
		strFileName.Delete(0, 4);
	}
	if(strFileName.Find(_T(".xml")) == -1)	// ��Ҫ����Դ�����в���
	{
		if(m_mapXmlPool.Lookup(strFileName, strXmlFile))
		{
		}else
		{
			strXmlFile = strFileName;
		}
	}else
	{
		strXmlFile = strFileName;
	}

	if(m_hResourceZip != NULL)	// ������Դzip�ļ�
	{
		// ��ʹ��zip�ļ��������,Ҳ����ʹ��Ŀ¼�е��ļ�
		if(GetFileAttributes(GetExePath() + strXmlFile) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			xmlResult = xmlDoc.load_file(GetExePath() + strXmlFile);
		}else
		if(GetFileAttributes(GetXmlPath() + strXmlFile) != 0xFFFFFFFF)	// ��xml·����ʼ����
		{
			xmlResult = xmlDoc.load_file(GetXmlPath() + strXmlFile);
		}else
		if(GetFileAttributes(strXmlFile) != 0xFFFFFFFF)	// ����·������
		{
			xmlResult = xmlDoc.load_file(strXmlFile);
		}else
		{
			DWORD dwSize = 0;
			BYTE* pByte = LoadZipFile(strXmlFile, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("xml\\") + strXmlFile, dwSize);	// ���Դ�xml��Ŀ¼����
			}
			if(pByte != NULL)
			{
				xmlResult = xmlDoc.load(CA2T((char*)pByte, CP_UTF8), NULL);
				//delete[] pByte;
			}else
			{
				DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("DuiSystem::LoadXmlFile %s failed, not found xml in zip file"), strXmlFile);
				return FALSE;
			}
		}
	}else
	{
		if(GetFileAttributes(GetExePath() + strXmlFile) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			xmlResult = xmlDoc.load_file(GetExePath() + strXmlFile);
		}else
		if(GetFileAttributes(GetXmlPath() + strXmlFile) != 0xFFFFFFFF)	// ��xml·����ʼ����
		{
			xmlResult = xmlDoc.load_file(GetXmlPath() + strXmlFile);
		}else
		if(GetFileAttributes(strXmlFile) != 0xFFFFFFFF)	// ����·������
		{
			xmlResult = xmlDoc.load_file(strXmlFile);
		}else
		{
			// �ļ�������
			DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("DuiSystem::LoadXmlFile %s failed, not found xml file"), strXmlFile);
			return FALSE;
		}
	}

	return xmlResult;
}

// ����ͼƬ�ļ�,֧�ִ�zip�ļ��м���
BOOL DuiSystem::LoadImageFile(CString strFileName, BOOL useEmbeddedColorManagement, Image*& pImage)
{
	//BOOL bRet = LoadImageFromFile(strFileName, useEmbeddedColorManagement, pImage);
	BOOL bRet = FALSE;
	if(m_hResourceZip != NULL)	// ������Դzip�ļ�
	{
		// ��ʹ��zip�ļ��������,Ҳ����ʹ��Ŀ¼�е��ļ�
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			bRet = LoadImageFromFile(DuiSystem::GetSkinPath() + strFileName, useEmbeddedColorManagement, pImage);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// ����·������
		{
			bRet = LoadImageFromFile(strFileName, useEmbeddedColorManagement, pImage);
		}else
		{
			DWORD dwSize = 0;
			BYTE* pByte = LoadZipFile(strFileName, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("skins\\") + strFileName, dwSize);	// ���Դ�skins��Ŀ¼����
			}
			if(pByte != NULL)
			{
				bRet = LoadImageFromMem(pByte, dwSize, useEmbeddedColorManagement, pImage);
				//delete[] pByte;
			}else
			{
				return FALSE;
			}
		}
	}else
	{
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			bRet = LoadImageFromFile(DuiSystem::GetSkinPath() + strFileName, useEmbeddedColorManagement, pImage);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// ����·������
		{
			bRet = LoadImageFromFile(strFileName, useEmbeddedColorManagement, pImage);
		}else
		{
			// �ļ�������
			return FALSE;
		}
	}
	return bRet;
}

// ����ͼƬ�ļ�,֧�ִ�zip�ļ��м���
BOOL DuiSystem::LoadBitmapFile(CString strFileName, CBitmap &bitmap, CSize &size)
{
	BOOL bRet = FALSE;
	if(m_hResourceZip != NULL)	// ������Դzip�ļ�
	{
		// ��ʹ��zip�ļ��������,Ҳ����ʹ��Ŀ¼�е��ļ�
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			bRet = LoadBitmapFromFile(DuiSystem::GetSkinPath() + strFileName, bitmap, size);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// ����·������
		{
			bRet = LoadBitmapFromFile(strFileName, bitmap, size);
		}else
		{
			DWORD dwSize = 0;
			BYTE* pByte = LoadZipFile(strFileName, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("skins\\") + strFileName, dwSize);	// ���Դ�skins��Ŀ¼����
			}
			if(pByte != NULL)
			{
				bRet = LoadBitmapFromMem(pByte, dwSize, bitmap, size);
				//delete[] pByte;
			}else
			{
				return FALSE;
			}
		}
	}else
	{
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			bRet = LoadBitmapFromFile(DuiSystem::GetSkinPath() + strFileName, bitmap, size);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// ����·������
		{
			bRet = LoadBitmapFromFile(strFileName, bitmap, size);
		}else
		{
			// �ļ�������
			return FALSE;
		}
	}
	return bRet;
}

// ����ͼ���ļ�,֧�ִ�zip�ļ��м���
BOOL DuiSystem::LoadIconFile(CString strFileName, HICON& hIcon)
{
	BOOL bRet = FALSE;
	if(m_hResourceZip != NULL)	// ������Դzip�ļ�
	{
		// ��ʹ��zip�ļ��������,Ҳ����ʹ��Ŀ¼�е��ļ�
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			bRet = LoadIconFromFile(DuiSystem::GetSkinPath() + strFileName, hIcon);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// ����·������
		{
			bRet = LoadIconFromFile(strFileName, hIcon);
		}else
		{
			DWORD dwSize = 0;
			BYTE* pByte = LoadZipFile(strFileName, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("skins\\") + strFileName, dwSize);	// ���Դ�skins��Ŀ¼����
			}
			if(pByte != NULL)
			{
				bRet = LoadIconFromMem(pByte, dwSize, hIcon);
				//delete[] pByte;
			}else
			{
				return FALSE;
			}
		}
	}else
	{
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			bRet = LoadIconFromFile(DuiSystem::GetSkinPath() + strFileName, hIcon);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// ����·������
		{
			bRet = LoadIconFromFile(strFileName, hIcon);
		}else
		{
			// �ļ�������
			return FALSE;
		}
	}
	return bRet;
}

// ����ͨ���ļ����ڴ�,֧�ִ�zip�ļ��м���
BOOL DuiSystem::LoadFileToBuffer(CString strFileName, BYTE*& pBuffer)
{
	BOOL bRet = FALSE;
	BYTE* pByte = NULL;
	CString _strFileName = _T("");
	if(m_hResourceZip != NULL)	// ������Դzip�ļ�
	{
		// ��ʹ��zip�ļ��������,Ҳ����ʹ��Ŀ¼�е��ļ�
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			_strFileName = DuiSystem::GetSkinPath() + strFileName;
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// ����·������
		{
			_strFileName = strFileName;
		}else
		{
			DWORD dwSize = 0;
			pByte = LoadZipFile(strFileName, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("skins\\") + strFileName, dwSize);	// ���Դ�skins��Ŀ¼����
			}
			if(pByte == NULL)
			{
				return FALSE;
			}
		}
	}else
	{
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// ��exe·����ʼ����
		{
			_strFileName = DuiSystem::GetSkinPath() + strFileName;
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// ����·������
		{
			_strFileName = strFileName;
		}else
		{
			// �ļ�������
			return FALSE;
		}
	}

	// ֱ�Ӵ��ļ�����
	if((pByte == NULL) && (!_strFileName.IsEmpty()))
	{
		CFile file;
		// ���ļ�
		if ( !file.Open( _strFileName, CFile::modeRead ) )
		{
			return FALSE;
		}
		DWORD dwSize = (DWORD)file.GetLength();
		pByte = new BYTE[dwSize + 1];
		TRY
		{
			file.Read( pByte, dwSize );
		}
		CATCH( CFileException, e );                                          
		{
			TRACE(_T( "Load (file): An exception occured while reading the file %s\n"), _strFileName);
			e->Delete();
			file.Close();
			delete pByte;
			return FALSE;
		}
		END_CATCH

		file.Close();
	}

	if(pByte != NULL)
	{
		pBuffer = pByte;
		return TRUE;
	}

	return FALSE;
}

// ���ؽ�������̬��
// ��ʽ1: file-resource-name		-- ������Դ������,���ҵ��ļ���,��exeĿ¼����
// ��ʽ2: filename.dll				-- ָ���ļ���,��exeĿ¼����
// ��ʽ3: c:\filename.dll			-- ָ����dll��ȫ·��
typedef LPVOID (*TYPEOF_CreateObject)(LPCTSTR lpcsInterfaceName, LPVOID* lppVciControl, LPVOID lpInitData);
BOOL DuiSystem::LoadPluginFile(CString strFileName, CString strObjType, HINSTANCE& hPluginHandle, LPVOID& pPluginObj)
{
	hPluginHandle = NULL;
	pPluginObj = NULL;
	CString strPluginFile;
	if(strFileName.Find(_T(".dll")) == -1)	// ��Ҫ����Դ�����в���
	{
		if(m_mapXmlPool.Lookup(strFileName, strPluginFile))
		{
		}else
		{
			strPluginFile = strFileName;
		}
	}else
	{
		strPluginFile = strFileName;
	}

	// ���ز����̬��
	hPluginHandle = NULL;
	if(GetFileAttributes(DuiSystem::GetExePath() + strPluginFile) != 0xFFFFFFFF)	// ��exe·����ʼ����
	{
		hPluginHandle = LoadLibrary(DuiSystem::GetExePath() + strPluginFile);
	}else
	if(GetFileAttributes(strPluginFile) != 0xFFFFFFFF)	// ����·������
	{
		hPluginHandle = LoadLibrary(strPluginFile);
	}

	if(hPluginHandle == NULL)
	{
		// ����ʧ��
		DWORD dwError = ::GetLastError();
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("Load UI plugin %s failed, errorcode is %u"), strPluginFile, dwError);
		return FALSE;
	}

	// ����VCI�������
	// ��ȡ����ָ��
	TYPEOF_CreateObject fnCreateObject = (TYPEOF_CreateObject)GetProcAddress(hPluginHandle, "CreateObject");
	if(fnCreateObject == NULL)
	{
		FreeLibrary(hPluginHandle);
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("Load UI plugin %s failed, not found CreateObject function"), strPluginFile);
		return FALSE;
	}

	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("Load UI plugin %s succ"), strPluginFile);

	LPVOID pIVciControl = NULL;
	//pPluginObj = fnCreateObject(CEncodingUtil::UnicodeToAnsi(strObjType), &pIVciControl, NULL);
	pPluginObj = fnCreateObject(strObjType, &pIVciControl, NULL);
	if(pPluginObj == NULL)
	{
		FreeLibrary(hPluginHandle);
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("Create UI plugin %s - %s object failed"), strPluginFile, strObjType);
		return FALSE;
	}

	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("Create UI plugin %s - %s object succ"), strPluginFile, strObjType);

	return TRUE;
}

// ��ȡϵͳ������Ϣ
CString DuiSystem::GetConfig(CString strName)
{
	CString strCfg;
	if(m_mapCfgPool.Lookup(strName, strCfg))
	{
		return strCfg;
	}
	return _T("");
}

// ��ȡXML�ļ�
CString DuiSystem::GetXmlFile(CString strName)
{
	CString strXmlFile;
	if(m_mapXmlPool.Lookup(strName, strXmlFile))
	{
		return strXmlFile;
	}
	return _T("");
}

// ��ȡSkin
CString DuiSystem::GetSkin(CString strName)
{
	if(strName.Find(_T("skin:")) == 0)
	{
		strName.Delete(0, 5);
	}

	CString strSkin;
	m_mapSkinPool.Lookup(strName, strSkin);
	return strSkin;
}

// ��ȡ�ַ���ֵ
CString DuiSystem::GetString(CString strName)
{
	CString strString;
	m_mapStringPool.Lookup(strName, strString);
	// ���ַ����е��滻�������滻
	ParseDuiString(strString);
	return strString;
}

// �����ַ���ֵ
void DuiSystem::SetString(CString strName, CString strValue)
{
	m_mapStringPool.SetAt(strName, strValue);
}

// �����ַ������滻���е��滻����
void DuiSystem::ParseDuiString(CString& strString)
{
	CString strTmp = strString;
	int nPos1 = strTmp.Find(_T("["));
	while(nPos1 != -1)
	{
		int nPos2 = strTmp.Find(_T("]"));
		if(nPos2 > (nPos1 + 1))
		{
			CString strName = strTmp.Mid(nPos1+1, nPos2-nPos1-1);
			CString strValue;
			if(m_mapStringPool.Lookup(strName, strValue))
			{
				strTmp.Replace(strTmp.Mid(nPos1, nPos2-nPos1+1), strValue);
			}
		}
		nPos1 = strTmp.Find(_T("["), nPos1+1);
	}
	strString = strTmp;
}

// ��ȡ����ϵͳ����
CString DuiSystem::GetOSName()
{
	int nOSType       =  OS_UNKNOWN;
	int nServicePack  =  PACK_UNKNOWN;
	
	// ��ȡ����ϵͳ�汾��Ϣ
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	
	if( !( GetVersionEx((OSVERSIONINFO *) &osvi ) ) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx( (OSVERSIONINFO *) &osvi) )
		{
			return _T("");
		}
	}
	
	//����ϵͳ�����汾
	nServicePack = osvi.wServicePackMajor;
	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		
		if ( osvi.dwMajorVersion <= 4 )
		{
			nOSType = WINDOWS_NT;
		}
		
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			nOSType = WINDOWS_2K;
		}
		
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			nOSType = WINDOWS_XP;
		}
		
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			nOSType = WINDOWS_2003;
		}
		
		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
		{
			nOSType = WINDOWS_VISTA;
		}

		if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
		{
			nOSType = WINDOWS_7;
		}

		if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
		{
			nOSType = WINDOWS_8;
		}

		//����ϵͳ�汾��Win8����
		if (osvi.dwMajorVersion > 6)
		{
			nOSType = WINDOWS_HIGH;
		}
		break;
		
	case VER_PLATFORM_WIN32_WINDOWS:
		
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			nOSType = WINDOWS_95;
		} 
		
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			nOSType = WINDOWS_98; 
		} 
		
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			nOSType = WINDOWS_ME;
		} 
		break;
		
	case VER_PLATFORM_WIN32s:  
		
		nOSType = WINDOWS_WIN32;
		break;
	default:
		nOSType = OS_UNKNOWN;
		break;
	}

	CString strOSType = _T("");
	switch( nOSType )
	{
	case WINDOWS_8:
		strOSType = _T("win8");
		break;
	case WINDOWS_7:
		strOSType = _T("win7");
		break;
	case WINDOWS_VISTA:
		strOSType = _T("vista");
		break;
	case WINDOWS_2003:
		strOSType = _T("win2003");
		break;
	case WINDOWS_XP:
		strOSType = _T("winxp");
		break;
	case WINDOWS_2K:
		strOSType = _T("win2000");
		break;
	case WINDOWS_NT:
		strOSType = _T("winnt");
		break;
	case WINDOWS_ME:
		strOSType = _T("winme");
		break;
	case WINDOWS_98:
		strOSType = _T("win98");
		break;
	case WINDOWS_95:
		strOSType = _T("win95");
		break;
	}
	return strOSType;
}

// ��鵱ǰ����ϵͳ�Ƿ���ָ����OS�ַ�����Χ��
BOOL DuiSystem::CheckOSName(CString strOS)
{
	CString strCurOSName = GetOSName();
	int nPos = -1;
	while((nPos = strOS.Find(_T(","))) != -1)
	{
		CString strTemp = strOS.Left(nPos);
		strOS.Delete(0, nPos+1);
		if(strCurOSName.CompareNoCase(strTemp) == 0)
		{
			return TRUE;
		}
	}
	if(!strOS.IsEmpty())
	{
		if(strCurOSName.CompareNoCase(strOS) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// ��ȡ������Ϣ
BOOL DuiSystem::GetFont(CString strName, DuiFontInfo& fontInfo)
{
	return m_mapFontPool.Lookup(strName, fontInfo);
}

// ��ȡȱʡ������Ϣ(��������������,���ж���������Ƿ񲻺���,�����ʾͻ��ɿ����õ�)
// Ŀǰ����Լ������ĺ�Ӣ��ϵͳ��Ĭ��������д���
CString DuiSystem::GetDefaultFont(CString strFont)
{
	int nOSVer = DuiSystem::GetOSMajorVersion();
	if(strFont.IsEmpty())
	{
		// ����Ϊ�����ղ���ϵͳ�汾�͵�ǰ���Ծ���Ĭ������
		if(nOSVer >= 6)
		{
			return (GetLanguage() == "zh-cn") ? TEXT("΢���ź�") : TEXT("Segoe UI");
		}else
		{
			return TEXT("Tahoma");
		}
	}else
	if((nOSVer < 6) && ((strFont == _T("΢���ź�")) || (strFont == _T("Segoe UI"))))
	{
		// ����ǵͰ汾����ϵͳ,������΢���ź�
		return TEXT("Tahoma");
	}else
	if(!strFont.IsEmpty())
	{
		// ����������ֱ�ӷ������������
		return strFont;
	}else
	if(nOSVer >= 6)
	{
		// Vista����Ĭ������
		return (GetLanguage() == "zh-cn") ? TEXT("΢���ź�") : TEXT("Segoe UI");
	}

	// �Ͱ汾����ϵͳĬ������
	return TEXT("Tahoma");	
}

// ��ȡ���ڱ�����Ϣ
BOOL DuiSystem::GetWindowBkInfo(int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile)
{
	// �����¼���������Ӧ�û�ȡ����ı�����Ϣ
	CDuiHandler* pDuiHandler = GetDuiHandler(0);
	if(pDuiHandler == NULL)
	{
		return FALSE;
	}

	// ���Ȼ�ȡSkin���ͣ�Ȼ��������ͻ�ȡSkinֵ
	if(pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_GET_SKIN_TYPE, (WPARAM)&nType, 0) != 0)
	{
		if(nType == BKTYPE_IMAGE_RESOURCE)
		{
			return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_GET_SKIN_VALUE, nType, (LPARAM)&nIDResource);
		}else
		if(nType == BKTYPE_COLOR)
		{
			return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_GET_SKIN_VALUE, nType, (LPARAM)&clr);
		}else
		if(nType == BKTYPE_IMAGE_FILE)
		{
			return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_GET_SKIN_VALUE, nType, (LPARAM)&strImgFile);
		}
	}

	return FALSE;
}

// ���ô��ڱ�����Ϣ
BOOL DuiSystem::SetWindowBkInfo(int nType, int nIDResource, COLORREF clr, CString strImgFile)
{
	// �����¼�������󽫱�����Ϣ���浽Ӧ��
	CDuiHandler* pDuiHandler = GetDuiHandler(0);
	if(pDuiHandler == NULL)
	{
		return FALSE;
	}

	if(nType == BKTYPE_IMAGE_RESOURCE)
	{
		return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_SET_SKIN_VALUE, nType, nIDResource);
	}else
	if(nType == BKTYPE_COLOR)
	{
		return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_SET_SKIN_VALUE, nType, (LPARAM)clr);
	}else
	if(nType == BKTYPE_IMAGE_FILE)
	{
		return pDuiHandler->OnDuiMessage(WND_SKIN, NAME_SKIN_WND, MSG_SET_SKIN_VALUE, nType, (LPARAM)(&strImgFile));
	}

	return TRUE;
}

// ע��ؼ�
void DuiSystem::RegisterDuiControl(CDuiObjectInfo* pDuiObjectInfo)
{
	// �ж�����Ѿ���ӹ����Ͳ����ظ����
	for (size_t i = 0; i < m_vecDuiObjectInfo.size(); i++)
	{
		CDuiObjectInfo* _pDuiObjectInfo = m_vecDuiObjectInfo.at(i);
		if (_pDuiObjectInfo == pDuiObjectInfo)
		{
			return;
		}		
	}

	m_vecDuiObjectInfo.push_back(pDuiObjectInfo);
}

// ж�ؿؼ�,����Ĳ���Ϊ�ؼ���
BOOL DuiSystem::UnRegisterDuiControl(LPCTSTR lpszName)
{
	vector<CDuiObjectInfo*>::iterator it;
	for(it=m_vecDuiObjectInfo.begin();it!=m_vecDuiObjectInfo.end();++it)
	{
		CDuiObjectInfo* pDuiObjectInfo = *it;
		if(pDuiObjectInfo && pDuiObjectInfo->m_pfGetClassName)
		{
			CString strClassName = pDuiObjectInfo->m_pfGetClassName();
			if(strClassName == lpszName)
			{
				m_vecDuiObjectInfo.erase(it);
				delete pDuiObjectInfo;
				return TRUE;
			}
		}
	}

	return FALSE;
}

// ���ؿؼ���
void DuiSystem::LoadDuiControls()
{
	// ע��DuiVisionĬ�ϵĿؼ�
	REGISTER_DUICONTROL(CDuiPanel, NULL);
	REGISTER_DUICONTROL(CDuiLayout, NULL);
	REGISTER_DUICONTROL(CDuiNativeWnd, NULL);

	REGISTER_DUICONTROL(CDuiButton, NULL);
	REGISTER_DUICONTROL(CImageButton, NULL);
	REGISTER_DUICONTROL(CCheckButton, NULL);
	REGISTER_DUICONTROL(CDuiRadioButton, NULL);
	REGISTER_DUICONTROL(CHideButton, NULL);
	REGISTER_DUICONTROL(CLinkButton, NULL);
	REGISTER_DUICONTROL(CTextButton, NULL);

	REGISTER_DUICONTROL(CDuiListCtrl, NULL);
	REGISTER_DUICONTROL(CDuiGridCtrl, NULL);
	REGISTER_DUICONTROL(CDuiTreeCtrl, NULL);

	REGISTER_DUICONTROL(CArea, NULL);
	REGISTER_DUICONTROL(CDuiFrame, NULL);
	REGISTER_DUICONTROL(CImageString, NULL);
	REGISTER_DUICONTROL(CRectangle, NULL);
	REGISTER_DUICONTROL(CDuiAnimateImage, NULL);
	REGISTER_DUICONTROL(CDuiScrollVertical, NULL);
	REGISTER_DUICONTROL(CDuiScrollHorizontal, NULL);
	REGISTER_DUICONTROL(CSelectBox, NULL);

	REGISTER_DUICONTROL(CDuiPicture, NULL);

	REGISTER_DUICONTROL(CDuiProgress, NULL);
	REGISTER_DUICONTROL(CDuiSlider, NULL);

	REGISTER_DUICONTROL(CDuiTabCtrl, NULL);

	REGISTER_DUICONTROL(CDuiText, NULL);

	REGISTER_DUICONTROL(CMenuItem, NULL);

	REGISTER_DUICONTROL(CDuiEdit, NULL);
	REGISTER_DUICONTROL(CDuiRichEdit, NULL);
	REGISTER_DUICONTROL(CDuiComboBox, NULL);

	REGISTER_DUICONTROL(CDuiActiveX, NULL);
	REGISTER_DUICONTROL(CDuiWebBrowserCtrl, NULL);
	REGISTER_DUICONTROL(CDuiFlashCtrl, NULL);
	REGISTER_DUICONTROL(CDuiMediaPlayer, NULL);
}

// �ͷſؼ���
void DuiSystem::ReleaseDuiControls()
{
	for (size_t i = 0; i < m_vecDuiObjectInfo.size(); i++)
	{
		CDuiObjectInfo* pDuiObjectInfo = m_vecDuiObjectInfo.at(i);
		if (pDuiObjectInfo)
		{
			// ִ�пؼ����������ͷź���
			if(pDuiObjectInfo->m_pfShutdown)
			{
				pDuiObjectInfo->m_pfShutdown();
			}

			delete pDuiObjectInfo;
		}		
	}
}

// ���ݿؼ����������ؼ�ʵ��
CControlBase* DuiSystem::CreateControlByName(LPCTSTR lpszName, HWND hWnd, CDuiObject* pParentObject)
{
	CControlBase *pControl = NULL;

	vector<CDuiObjectInfo*>* pvecDuiObjectInfo = DuiSystem::Instance()->GetDuiObjectInfoVect();
	for (size_t i = 0; i < pvecDuiObjectInfo->size(); i++)
	{
		CDuiObjectInfo* pDuiObjectInfo = pvecDuiObjectInfo->at(i);
		if (pDuiObjectInfo && pDuiObjectInfo->m_pfCheckAndNew)
		{
			pControl = (CControlBase*)pDuiObjectInfo->m_pfCheckAndNew(lpszName, hWnd, pParentObject);
			if(pControl)
			{
				return pControl;
			}
		}
	}

	return NULL;
}

// ���ݿؼ�ID��ȡ�ӿؼ����󣨴�����ע���DUI�Ի����в��ң�
CControlBase* DuiSystem::GetControlFromDuiDialog(UINT uControlID)
{
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		if (pDlgBase)
		{
			CControlBase* pControlBase = pDlgBase->GetControl(uControlID);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ���ݿؼ�ID��ȡ�ӿؼ�����
CControlBase* DuiSystem::GetControl(CDuiObject* pDuiObject, UINT uControlID)
{
	if(pDuiObject == NULL)
	{
		return NULL;
	}

	// �����ж��Ƿ������DUI����
	if(pDuiObject->GetID() == uControlID)
	{
		return (CControlBase*)pDuiObject;
	}

	// �ж��Ƿ��²�DUI�ؼ�����
	if(pDuiObject->IsClass(_T("dlg")))
	{
		CDlgBase* pDlgBase = (CDlgBase*)pDuiObject;
		CControlBase* pControlBase = pDlgBase->GetControl(uControlID);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}else
		{
			// ����base control
			pControlBase = pDlgBase->GetBaseControl(uControlID);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}else
	if(pDuiObject->IsClass(_T("popup")))
	{
		CDlgPopup* pDlgPopup = (CDlgPopup*)pDuiObject;
		CControlBase* pControlBase = pDlgPopup->GetControl(uControlID);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}else
	{
		CControlBase* pControl = (CControlBase*)pDuiObject;
		CControlBase* pControlBase = pControl->GetControl(uControlID);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}

	// ���û���ҵ����ٵ�����ע��ĶԻ����в��ң������ID���ҷ�ʽ��
	return DuiSystem::Instance()->GetControlFromDuiDialog(uControlID);
}

// ���ݿؼ�����ȡ�ӿؼ�����
CControlBase* DuiSystem::GetControl(CDuiObject* pDuiObject, CString strControlName)
{
	if(pDuiObject == NULL)
	{
		return NULL;
	}

	// �����ж��Ƿ������DUI����
	if(pDuiObject->GetName() == strControlName)
	{
		return (CControlBase*)pDuiObject;
	}

	// �ж��Ƿ��²�DUI�ؼ�����
	if(pDuiObject->IsClass(_T("dlg")))
	{
		CDlgBase* pDlgBase = (CDlgBase*)pDuiObject;
		CControlBase* pControlBase = pDlgBase->GetControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}else
		{
			// ����base control
			pControlBase = pDlgBase->GetBaseControl(strControlName);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}else
	if(pDuiObject->IsClass(_T("popup")))
	{
		CDlgPopup* pDlgPopup = (CDlgPopup*)pDuiObject;
		CControlBase* pControlBase = pDlgPopup->GetControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}else
	{
		CControlBase* pControl = (CControlBase*)pDuiObject;
		CControlBase* pControlBase = pControl->GetControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}

	return NULL;
}

// ���ݶԻ��������ӿؼ�����ȡ�Ի����е��ӿؼ�����
CControlBase* DuiSystem::GetDialogControl(CString strDialogName, CString strControlName)
{
	CDlgBase* pDuiObject = DuiSystem::Instance()->GetDuiDialog(strDialogName);
	if(pDuiObject == NULL)
	{
		return NULL;
	}

	// �ж��Ƿ��²�DUI�ؼ�����
	CDlgBase* pDlgBase = (CDlgBase*)pDuiObject;
	CControlBase* pControlBase = pDlgBase->GetControl(strControlName);
	if(pControlBase != NULL)
	{
		return pControlBase;
	}else
	{
		// ����base control
		pControlBase = pDlgBase->GetBaseControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}

	return NULL;
}

// ���DUI�¼��������
void DuiSystem::AddDuiHandler(CDuiHandler* pDuiHandler)
{
	// �ж�����Ѿ���ӹ����Ͳ����ظ����
	for (size_t i = 0; i < m_vecDuiHandler.size(); i++)
	{
		CDuiHandler* _pDuiHandler = m_vecDuiHandler.at(i);
		if (_pDuiHandler == pDuiHandler)
		{
			return;
		}		
	}

	m_vecDuiHandler.push_back(pDuiHandler);
}

// ɾ��DUI�¼��������
BOOL DuiSystem::RemoveDuiHandler(CDuiHandler* pDuiHandler)
{
	vector<CDuiHandler*>::iterator it;
	for(it=m_vecDuiHandler.begin();it!=m_vecDuiHandler.end();++it)
	{
		if(*it == pDuiHandler)
		{
			m_vecDuiHandler.erase(it);
			delete pDuiHandler;
			return TRUE;
		}
	}

	return FALSE;
}

// ��ȡDUI�¼��������(��������)
CDuiHandler* DuiSystem::GetDuiHandler(int nIndex)
{
	if(m_vecDuiHandler.size() == 0)
	{
		return NULL;
	}

	if(nIndex < 0 || nIndex > ((int)m_vecDuiHandler.size() - 1))
	{
		return NULL;
	}
	return m_vecDuiHandler.at(nIndex);
}

// ��ָ����DUI����ע���¼��������
BOOL DuiSystem::RegisterHandler(CDuiObject* pDuiObject, CDuiHandler* pDuiHandler, CString strControlName, BOOL bInit)
{
	if(pDuiObject == NULL || pDuiHandler == NULL)
	{
		return FALSE;
	}

	DuiSystem::Instance()->AddDuiHandler(pDuiHandler);

	if(strControlName.IsEmpty())
	{
		pDuiObject->RegisterHandler(pDuiHandler);
	}else
	{
		CControlBase* pSubControl = DuiSystem::GetControl(pDuiObject, strControlName);
		if(pSubControl == NULL)
		{
			return FALSE;
		}
		pSubControl->RegisterHandler(pDuiHandler);
	}

	// �Ƿ�ע��֮��������ʼ��
	if(bInit)
	{
		pDuiHandler->OnInit();
	}

	return TRUE;
}

// ��������ע����¼����������д���
LRESULT DuiSystem::CallDuiHandler(UINT uID, CString strName, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (size_t i = 0; i < m_vecDuiHandler.size(); i++)
	{
		CDuiHandler* pDuiHandler = m_vecDuiHandler.at(i);
		if (pDuiHandler)
		{
			pDuiHandler->OnDuiMessage(uID, strName, uMsg, wParam, lParam);
		}		
	}

	return 0;
}

// ���DUI�Ի���
void DuiSystem::AddDuiDialog(CDlgBase* pDuiDialog)
{
	m_vecDuiDialog.push_back(pDuiDialog);
}

// ɾ��DUI�Ի���
void DuiSystem::RemoveDuiDialog(CDlgBase* pDuiDialog)
{
	vector<CDlgBase*>::iterator it;
	for(it=m_vecDuiDialog.begin();it!=m_vecDuiDialog.end();++it)
	{
		if(*it == pDuiDialog)
		{
			m_vecDuiDialog.erase(it);
			break;
		}
	}
	if(pDuiDialog != NULL)
	{
		delete pDuiDialog;
	}
}

// ��ȡDUI�Ի���ָ��(��������)
CDlgBase* DuiSystem::GetDuiDialog(int nIndex)
{
	if(nIndex < 0 || nIndex > ((int)m_vecDuiDialog.size() - 1))
	{
		return NULL;
	}
	return m_vecDuiDialog.at(nIndex);
}

// ��ȡDUI�Ի���ָ��(����name)
CDlgBase* DuiSystem::GetDuiDialog(CString strName)
{
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		if(pDlgBase->GetName() == strName)
		{
			return pDlgBase;
		}
	}
	return NULL;
}

// �������жԻ���ı���Ƥ��
void DuiSystem::ResetAllWindowsBkSkin()
{
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		pDlgBase->InitWindowBkSkin();
	}
}

// �����Ի���
CDlgBase* DuiSystem::CreateDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule, UINT nIDTemplate, BOOL bAdd)
{
	// ����XMLģ���ļ�������
	CString strXmlContent = lpszXmlTemplate;
	CString strXmlFile = _T("");
	if(strXmlContent.Find(_T("<?xml")) != 0)
	{
		strXmlContent = _T("");
		strXmlFile = lpszXmlTemplate;
		if(strXmlFile.Find(_T(".xml")) == -1)
		{
			strXmlFile = DuiSystem::Instance()->GetXmlFile(strXmlFile);
		}else
		if(strXmlFile.Find(_T(":")) == -1)
		{
			strXmlFile = _T("xml:") + strXmlFile;
		}
		if(strXmlFile.IsEmpty())
		{
			return NULL;
		}
	}

	CDlgBase* pParentDlg = NULL;
	if(pParentObject && pParentObject->IsClass(_T("dlg")))
	{
		pParentDlg = ((CDlgBase*)pParentObject);
		if(nIDTemplate == 0)
		{
			nIDTemplate = pParentDlg->GetIDTemplate();
		}
	}
	if(nIDTemplate == 0 && g_nIDTemplate != 0)
	{
		nIDTemplate = g_nIDTemplate;
	}
	if(g_nIDTemplate == 0)
	{
		g_nIDTemplate = nIDTemplate;
	}
	CDlgBase* pDlg = new CDlgBase(nIDTemplate, pParentDlg);
	pDlg->SetParent(pParentObject);
	pDlg->SetXmlFile(strXmlFile);
	pDlg->SetXmlContent(strXmlContent);

	if(!strName.IsEmpty())
	{
		// ����name
		pDlg->SetName(strName);
	}

	if(bAdd)
	{
		// ��ӵ��Ի����б���
		DuiSystem::Instance()->AddDuiDialog(pDlg);
	}

	if(!bModule)
	{
		// ����Ƿ�ģʽ�Ի���,��ֱ�Ӵ�������
		pDlg->Create(pDlg->GetIDTemplate(), NULL);
	}

	return pDlg;
}

// ��ʾ�Ի���
int DuiSystem::ShowDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(lpszXmlTemplate, pParentObject, strName, bModule, 0, TRUE);
	if(pDlg == NULL)
	{
		return 0;
	}

	// ����Ƿ�ģ̬,����ʾ����֮�󷵻�
	if(!bModule)
	{
		pDlg->ShowWindow(SW_SHOW);
		return 0;
	}

	// ģ̬�Ի���,����DoModal��ʾ�Ի���
	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	return nResponse;
}

// ͨ����Ϣ�Ի���
int DuiSystem::DuiMessageBox(CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption, UINT uType, int nWidth, int nHeight)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_msgbox"), pParent, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return 0;
	}

	// �������ݺͱ���
	pDlg->SetControlValue(_T("msgbox.text"), _T("title"), lpszText);
	if(lpszCaption != _T(""))
	{
		pDlg->SetControlValue(_T("msgbox.title"), _T("title"), lpszCaption);
	}

	// ��ť
	UINT uButtonType = uType & 0x0F;
	for(UINT i=0; i< 10; i++)
	{
		CString strButtonDivName;
		strButtonDivName.Format(_T("msgbox.type.%u"), i);
		pDlg->SetControlValue(strButtonDivName, _T("visible"), (uButtonType == i) ? _T("1") : _T("0"));
	}

	// ͼ��
	if((uType & 0xF0) == MB_ICONINFORMATION)
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_INFO"));
	}else
	if((uType & 0xF0) == MB_ICONQUESTION)
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_QUESTION"));
	}else
	if((uType & 0xF0) == MB_ICONWARNING)
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_WARN"));
	}else
	if((uType & 0xF0) == MB_ICONERROR)
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_ERROR"));
	}else
	{
		pDlg->SetControlValue(_T("msgbox.icon"), _T("image"), _T("skin:IDB_ICON_CHECK"));
	}

	if(nWidth != 0)
	{
		CString strTemp;
		strTemp.Format(_T("%d"), nWidth);
		pDlg->SetControlValue(_T(""), _T("width"), strTemp);
	}
	if(nHeight != 0)
	{
		CString strTemp;
		strTemp.Format(_T("%d"), nHeight);
		pDlg->SetControlValue(_T(""), _T("height"), strTemp);
	}

	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	return nResponse;
}

// ��̬��ʾ��Ϣ������ӿ�:������̬��Ϣ��ʾ��
void DuiSystem::CreateNotifyMsgBox(LPCTSTR lpszXmlTemplate, CString strName)
{
	if(m_pNotifyMsgBox != NULL)
	{
		RemoveNotifyMsgBox();
	}

	m_pNotifyMsgBox = CreateDuiDialog(lpszXmlTemplate, NULL, strName, FALSE);
	if(m_pNotifyMsgBox != NULL)
	{
		m_pNotifyMsgBox->ShowWindow(SW_HIDE);
	}
}

// ɾ����̬��Ϣ��ʾ��
void DuiSystem::RemoveNotifyMsgBox()
{
	if(m_pNotifyMsgBox != NULL)
	{
		RemoveDuiDialog(m_pNotifyMsgBox);
		m_pNotifyMsgBox = NULL;
	}
}

// ���ö�̬��Ϣ��ʾ��ؼ�����
void DuiSystem::SetNotifyMsgBoxControlTitle(CString strControlName, CString strControlTitle)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		CControlBaseFont* pControl = (CControlBaseFont*)(pNotifyMsgBox->GetControl(strControlName));
		if(pControl == NULL)
		{
			// �������ͨ�ؼ���δ�ҵ�,����һ�»����ؼ�
			pControl = (CControlBaseFont*)(pNotifyMsgBox->GetBaseControl(strControlName));
		}
		if(pControl)
		{
			pControl->SetTitle(strControlTitle);
		}
	}
}

// ���ö�̬��Ϣ��ʾ��ؼ�ͼƬ
void DuiSystem::SetNotifyMsgBoxControlImage(CString strControlName, CString strControlImage)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		CControlBaseFont* pControl = (CControlBaseFont*)(pNotifyMsgBox->GetControl(strControlName));
		if(pControl)
		{
			pControl->OnAttributeImage(strControlImage, TRUE);
		}
	}
}

// ���ö�̬��Ϣ��ʾ��ؼ��ɼ���
void DuiSystem::SetNotifyMsgBoxControlVisible(CString strControlName, BOOL bVisible)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		CControlBase* pControl = pNotifyMsgBox->GetControl(strControlName);
		if(pControl == NULL)
		{
			// �������ͨ�ؼ���δ�ҵ�,����һ�»����ؼ�
			pControl = (CControlBaseFont*)(pNotifyMsgBox->GetBaseControl(strControlName));
		}
		if(pControl)
		{
			pControl->SetVisible(bVisible);
		}
	}
}

// ���ö�̬��Ϣ��ʾ���С
void DuiSystem::SetNotifyMsgBoxSize(int nWidth, int nHeight)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		pNotifyMsgBox->SetMinSize(nWidth, nHeight);
		pNotifyMsgBox->SetRect(CRect(0, 0, nWidth, nHeight));
		::SetWindowPos(pNotifyMsgBox->m_hWnd, NULL, 0, 0, nWidth, nHeight, SWP_SHOWWINDOW);
	}
}

// ��ʾ��̬��Ϣ��ʾ��
void DuiSystem::ShowNotifyMsgBox(UINT uDelayTime)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		int nScreenWidth= GetSystemMetrics(SM_CXFULLSCREEN);
		int nScreenHeight= GetSystemMetrics(SM_CYFULLSCREEN);
		CRect rect;
		pNotifyMsgBox->GetWindowRect(rect);
		/*if(rect.left == 0 && rect.top == 0)
		{
			//������ʾ
			rect.MoveToXY((nScreenWidth-rect.Width()) / 2, (nScreenHeight-rect.Height()) / 2);
			pNotifyMsgBox->MoveWindow(rect, FALSE);
		}*/
		// ��ʾ��Windows���½�
		rect.MoveToXY(nScreenWidth-rect.Width(), nScreenHeight-rect.Height());
		pNotifyMsgBox->MoveWindow(rect, FALSE);
		// �Ѵ��ڷ�����ǰ����ʾ
		::SetForegroundWindow(pNotifyMsgBox->m_hWnd);
		pNotifyMsgBox->ShowWindow(SW_SHOW);
		::BringWindowToTop(pNotifyMsgBox->m_hWnd);
		// �����ö�
		::SetWindowPos(pNotifyMsgBox->m_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

		// �����Զ��رն�ʱ��(���ش���ģʽ,��Notify��������)
		pNotifyMsgBox->SetAutoCloseTimer(uDelayTime, TRUE);
	}
}

// ���ض�̬��Ϣ��ʾ��
void DuiSystem::HideNotifyMsgBox()
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		pNotifyMsgBox->ShowWindow(SW_HIDE);
	}
}

// ִ�г���
BOOL DuiSystem::ExecuteProcess(CString strProcessName, CString strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess)
{
    BOOL bRet = FALSE;
    PROCESS_INFORMATION processInfo;
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    HANDLE hProcess = NULL;

    if (bForceAdmin)	// ����Ա��ʽ����
    {
        SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };

        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.hwnd = NULL;
        sei.lpVerb = _T("runas");
        sei.lpFile = strProcessName;
        sei.lpParameters = (LPCTSTR)strCmdLine;
        sei.nShow = SW_SHOWNORMAL;

        bRet = ::ShellExecuteEx(&sei);

        hProcess = sei.hProcess;
    }else
    {
        bRet = ::CreateProcess(
            strProcessName, 
            (LPTSTR)(LPCTSTR)strCmdLine, 
            NULL, NULL, FALSE, 0, NULL, NULL, &si, &processInfo
            );

        if (bRet)
        {
            ::CloseHandle(processInfo.hThread);
            hProcess = processInfo.hProcess;
        }
    }

    if (bRet)
    {
        if (bWaitProcess)
        {
            ::WaitForSingleObject(hProcess, INFINITE);
        }
        ::CloseHandle(hProcess);
    }

    return bRet;
}

// ִ��UI�߳�����(��һ������ͨ���������ڷ���Ϣ,����������Ϣ����ִ�еķ�ʽʵ�������̵߳��������̵߳�������ת)
int DuiSystem::RunUITask(DuiVision::IBaseTask* pTask, const DuiVision::CTaskMgr* pTaskMgr)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	return pDlg->SendMessage(WM_UI_TASK, (WPARAM)pTask, (LPARAM)pTaskMgr);
}

//
// DUI����������
//
class CDuiActionTask : public DuiVision::IBaseTask
{
public:
	CDuiActionTask(LONG type, UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam, CString strControlName, CString strAction, CDuiObject* pParent)
		: DuiVision::IBaseTask(type), m_uID(uID), m_uMsg(uMsg), m_wParam(wParam), m_lParam(lParam),
		m_pParent(pParent), m_strControlName(strControlName), m_strAction(strAction)
	{
		SetUITask(TRUE);	// ����Ϊ��ҪתUI�̴߳��������
	}

	// ������
	virtual BOOL TaskProcess(DuiVision::CTaskMgr *pMgr)
	{
		DoAction();
		return TRUE;
	}

	void DoAction()
	{
		if(!m_strAction.IsEmpty())
		{
			// ���������action,�����ִ��
			if(m_strAction.Find(_T("dlg:")) == 0)	// ����:��һ���Ի���,���ڴ�й©����Ϊͨ��DuiSystem�����͹���
			{
				if(m_uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
				{
					CString strXmlFile = m_strAction;
					strXmlFile.Delete(0, 4);
					if(!strXmlFile.IsEmpty())
					{
						DuiSystem::ShowDuiDialog(strXmlFile, m_pParent);
					}
				}
			}else
			if(m_strAction.Find(_T("link:")) == 0)	// ����:��һ��ҳ������
			{
				if(m_uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
				{
					CString strLink = m_strAction;
					strLink.Delete(0, 5);
					if(!strLink.IsEmpty())
					{
						ShellExecute(NULL, TEXT("open"), strLink, NULL,NULL,SW_NORMAL);
					}
				}
			}else
			if(m_strAction.Find(_T("run:")) == 0)	// ����:ִ��һ������
			{
				if(m_uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
				{
					CString strProcess = m_strAction;
					strProcess.Delete(0, 4);
					strProcess.MakeLower();
					if(!strProcess.IsEmpty())
					{
						strProcess.MakeLower();
						BOOL bForceAdmin = FALSE;
						if(strProcess.Find(_T("admin@")) == 0)
						{
							bForceAdmin = TRUE;
							strProcess.Delete(0, 6);
						}
						BOOL bWaitProcess = FALSE;
						if(strProcess.Find(_T("&")) == (strProcess.GetLength()-1))
						{
							bWaitProcess = TRUE;
							strProcess.Delete(strProcess.GetLength()-1, 1);
						}
						if(strProcess.Find(_T(".exe")) == -1)
						{
							strProcess = DuiSystem::Instance()->GetString(strProcess);
						}
						if(strProcess.Find(_T("{platpath}")) == 0)
						{
							strProcess.Delete(0, 10);
							strProcess = DuiSystem::GetExePath() + strProcess;
						}
						CString strCmdLine = _T("");
						int nPos = strProcess.Find(_T("|"));
						if(nPos != -1)
						{
							strCmdLine = strProcess;
							strCmdLine.Delete(0, nPos+1);
							strProcess = strProcess.Left(nPos);
						}
						DuiSystem::PathCanonicalize(strProcess);	// ·����׼��
						DuiSystem::ExecuteProcess(strProcess, strCmdLine, bForceAdmin, bWaitProcess);
					}
				}
			}else
			if(m_strAction.Find(ACTION_CLOSE_WINDOW) == 0)	// ����:�ر�ָ���Ĵ���
			{
				if(m_uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
				{
					CString strWndName = m_strAction;
					strWndName.Delete(0, 13);
					if(!strWndName.IsEmpty())
					{
						CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(strWndName);
						if(pDlg != NULL)
						{
							//pDlg->PostMessage(WM_QUIT, 0, 0);
							pDlg->DoClose();
						}
					}
				}
			}else
			if(m_strAction.Find(ACTION_SHOW_WINDOW) == 0)	// ����:��ʾָ���Ĵ���
			{
				if(m_uMsg == MSG_BUTTON_UP)	// ���ſ��¼��Ŵ���
				{
					CString strWndName = m_strAction;
					strWndName.Delete(0, 12);
					if(!strWndName.IsEmpty())
					{
						CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(strWndName);
						if(pDlg != NULL)
						{
							pDlg->SetForegroundWindow();
							pDlg->ShowWindow(SW_NORMAL);
							pDlg->ShowWindow(SW_SHOW);
							pDlg->UpdateWindow();
							pDlg->BringWindowToTop();
						}
					}
				}
			}
		}else
		{
			// �ҵ��ؼ�,���ÿؼ�����Ϣ����
			CControlBase* pControl = DuiSystem::GetControl(m_pParent, m_uID);
			if(pControl)
			{
				pControl->CallDuiHandler(m_uMsg, m_wParam, m_lParam);
			}else
			{
				// ���δ�ҵ��ؼ�,��ͨ��DuiSystem��������ע����¼����������д���
				DuiSystem::Instance()->CallDuiHandler(m_uID, m_strControlName, m_uMsg, m_wParam, m_lParam);
			}
		}
	}

protected:
	CDuiObject*	m_pParent;			// ������
	UINT		m_uID;				// �ؼ�ID
	CString		m_strControlName;	// �ؼ���
	UINT		m_uMsg;				// ��Ϣ
	WPARAM		m_wParam;			// ����1
	LPARAM		m_lParam;			// ����2
	CString		m_strAction;		// ����
};

// ���DUI��������
void DuiSystem::AddDuiActionTask(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam, CString strControlName, CString strAction, CDuiObject* pParent)
{
	DuiVision::CTaskMgr* pTaskMgr = DuiSystem::Instance()->GetTaskMgr();
	if(pTaskMgr)
	{
		pTaskMgr->AddTask(new CDuiActionTask(1, uID, uMsg, wParam, lParam, strControlName, strAction, pParent));
		pTaskMgr->StartTask();
	}
}

//
// ��ʾ��Ϣ������
//
class CDuiNotifyMsgTask : public DuiVision::IBaseTask
{
public:
	CDuiNotifyMsgTask(LONG type, CString strMsg, UINT uNotifyType = MB_ICONINFORMATION | 2, CString strCaption = _T(""), int nDelayTime = 0, int nWidth = 0, int nHeight = 0)
		: DuiVision::IBaseTask(type), m_strMsg(strMsg), m_uNotifyType(uNotifyType), m_strCaption(strCaption),
		m_nDelayTime(nDelayTime), m_nWidth(nWidth), m_nHeight(nHeight)
	{
		SetUITask(TRUE);	// ����Ϊ��ҪתUI�̴߳��������
	}

	// ������
	virtual BOOL TaskProcess(DuiVision::CTaskMgr *pMgr)
	{
		DoAction();
		return TRUE;
	}

	void DoAction()
	{
		// ÿ�ζ���ɾ���ɵĴ���,���´���һ��,���򴰿��������رչ��Ļ�,��Ӧ��Ϣ��������
		// ��������Ѿ����ر��ˣ�����Ҫ���´���,������Ӧ��Ϣ��������
		CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
		if((pNotifyMsgBox == NULL) || !::IsWindow(pNotifyMsgBox->GetSafeHwnd()))
		{
			DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));
		}

		// �������ݺͱ���
		DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.text"), m_strMsg);
		if(m_strCaption != _T(""))
		{
			DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.title"), m_strCaption);
		}

		// ��ť
		UINT uButtonType = m_uNotifyType & 0x0F;
		for(UINT i=0; i< 10; i++)
		{
			CString strButtonDivName;
			strButtonDivName.Format(_T("notify.type.%u"), i);
			DuiSystem::SetNotifyMsgBoxControlVisible(strButtonDivName, (uButtonType == i) ? TRUE : FALSE);
		}

		// ͼ��
		if((m_uNotifyType & 0xF0) == MB_ICONINFORMATION)
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_INFO"));
		}else
		if((m_uNotifyType & 0xF0) == MB_ICONQUESTION)
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_QUESTION"));
		}else
		if((m_uNotifyType & 0xF0) == MB_ICONWARNING)
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_WARN"));
		}else
		if((m_uNotifyType & 0xF0) == MB_ICONERROR)
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_ERROR"));
		}else
		{
			DuiSystem::SetNotifyMsgBoxControlImage(_T("notify.icon"), _T("skin:IDB_ICON_CHECK"));
		}

		if((m_nWidth != 0) && (m_nHeight != 0))
		{
			DuiSystem::SetNotifyMsgBoxSize(m_nWidth, m_nHeight);
		}

		DuiSystem::ShowNotifyMsgBox(m_nDelayTime);
	}

protected:
	CString		m_strXmlTemplate;	// XMLģ��
	UINT		m_uNotifyType;		// ��ʾ��ͼ�������
	CString		m_strMsg;			// ��ʾ��Ϣ
	CString		m_strCaption;		// ����
	int			m_nCheck;			// �����ֵ
	int			m_nWidth;			// ���ڿ��
	int			m_nHeight;			// ���ڸ߶�
	int			m_nDelayTime;		// �ӳ�ʱ��(��)
};

// �����ʾ��Ϣ����
void DuiSystem::AddDuiNotifyMsgTask(CString strMsg, UINT uNotifyType, CString strCaption, int nDelayTime, int nWidth, int nHeight)
{
	DuiVision::CTaskMgr* pTaskMgr = DuiSystem::Instance()->GetTaskMgr();
	if(pTaskMgr)
	{
		pTaskMgr->AddTask(new CDuiNotifyMsgTask(2, strMsg, uNotifyType, strCaption, nDelayTime, nWidth, nHeight));
		pTaskMgr->StartTask();
	}
}

// ����������
CDlgPopup* DuiSystem::CreateDuiPopupWnd(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, UINT nIDTemplate, BOOL bAdd)
{
	CString strXmlFile = lpszXmlTemplate;
	if(strXmlFile.Find(_T(".xml")) == -1)
	{
		strXmlFile = DuiSystem::Instance()->GetXmlFile(strXmlFile);
	}else
	if(strXmlFile.Find(_T(":")) == -1)
	{
		strXmlFile = _T("xml:") + strXmlFile;
	}
	if(strXmlFile.IsEmpty())
	{
		return NULL;
	}

	CDlgBase* pParentDlg = NULL;
	if(pParentObject && pParentObject->IsClass(_T("dlg")))
	{
		pParentDlg = ((CDlgBase*)pParentObject);
		if(nIDTemplate == 0)
		{
			nIDTemplate = pParentDlg->GetIDTemplate();
		}
	}
	if(nIDTemplate == 0 && g_nIDTemplate != 0)
	{
		nIDTemplate = g_nIDTemplate;
	}
	if(g_nIDTemplate == 0)
	{
		g_nIDTemplate = nIDTemplate;
	}
	/*
	CDlgPopup* pPopup = new CPopupList(nIDTemplate, pParentDlg);
	pPopup->SetParent(pParentObject);
	pPopup->SetXmlFile(strXmlFile);
	if(bAdd)
	{
		//DuiSystem::Instance()->AddDuiDialog(pDlg);
	}*/

	return NULL;
}

// ��Ӷ�ʱ��
UINT DuiSystem::AddDuiTimer(UINT uTimerResolution, CString strTimerName, BOOL bAppTimer)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return 0;
	}

	return ((CTimer*)pDlg)->SetTimer(uTimerResolution, strTimerName, bAppTimer);
}

// ɾ����ʱ��(����IDɾ��)
void DuiSystem::RemoveDuiTimer(UINT uTimerID)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return;
	}

	((CTimer*)pDlg)->KillTimer(uTimerID);
}

// ɾ����ʱ��(��������ɾ��)
void DuiSystem::RemoveDuiTimer(CString strTimerName)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return;
	}

	((CTimer*)pDlg)->KillTimer(strTimerName);
}

// �����¼��������Ķ�ʱ��������
void DuiSystem::CallDuiHandlerTimer(UINT uTimerID, CString strTimerName)
{
	for (size_t i = 0; i < m_vecDuiHandler.size(); i++)
	{
		CDuiHandler* pDuiHandler = m_vecDuiHandler.at(i);
		if (pDuiHandler)
		{
			pDuiHandler->OnTimer(uTimerID, strTimerName);
		}		
	}
}

// ��ʼ������ͼ��
BOOL DuiSystem::InitTray(CDuiHandler* pDuiHandler, CString strIcon, CString strTip, CString strMenuXml)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return FALSE;
	}

	pDlg->SetTrayHandler(pDuiHandler);
	pDlg->SetTrayMenuXml(strMenuXml);

	m_NotifyIconData.cbSize = NOTIFYICONDATAA_V1_SIZE;
	m_NotifyIconData.hWnd = pDlg->m_hWnd;
	m_NotifyIconData.uID = 1;
	m_NotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_NotifyIconData.uCallbackMessage = WM_SYSTEM_TRAYICON;

	CString strTrayIcon = DuiSystem::Instance()->GetSkin(_T("IDB_TRAY_ICON"));
	if(!strIcon.IsEmpty())
	{
		strTrayIcon = strIcon;
	}
	if (strTrayIcon.IsEmpty()) return FALSE;

	// �ͷ�ͼ����Դ
	if(m_NotifyIconData.hIcon != NULL)
	{
		DestroyIcon(m_NotifyIconData.hIcon);
		m_NotifyIconData.hIcon = NULL;
	}

	if(strTrayIcon.Find(_T(".")) != -1)	// ����ͼ���ļ�
	{
		DuiSystem::Instance()->LoadIconFile(strTrayIcon, m_NotifyIconData.hIcon);
	}else	// ����ͼ����Դ
	{
		UINT nResourceID = _ttoi(strTrayIcon);
		LoadIconFromIDResource(nResourceID, m_NotifyIconData.hIcon);
	}

	CString sWindowText = DuiSystem::Instance()->GetString(_T("APP_NAME"));
	if(!strTip.IsEmpty())
	{
		sWindowText = strTip;
	}
	_tcscpy_s(m_NotifyIconData.szTip, sWindowText);

	BOOL bRet = FALSE;
	int nRetry = 3;
	while(((bRet = Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData)) != TRUE) && (nRetry > 0))
	{
		// ʧ�����������3��
		nRetry--;
		Sleep(100);
	}

	return bRet;
}

// ɾ������ͼ��
void DuiSystem::DelTray()
{
	if(m_NotifyIconData.cbSize)
	{
		// �ͷ�ͼ����Դ
		if(m_NotifyIconData.hIcon != NULL)
		{
			DestroyIcon(m_NotifyIconData.hIcon);
		}
		Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
		ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);
	}
}

// ��������ͼ��
BOOL DuiSystem::SetTrayIcon(CString strIcon)
{
	if(m_NotifyIconData.cbSize)
	{
		m_NotifyIconData.uFlags = NIF_ICON;

		// �ͷ�ͼ����Դ
		if(m_NotifyIconData.hIcon != NULL)
		{
			DestroyIcon(m_NotifyIconData.hIcon);
			m_NotifyIconData.hIcon = NULL;
		}

		if(strIcon.Find(_T(".")) != -1)	// ����ͼ���ļ�
		{
			DuiSystem::Instance()->LoadIconFile(strIcon, m_NotifyIconData.hIcon);
		}else	// ����ͼ����Դ
		{
			UINT nResourceID = _ttoi(strIcon);
			LoadIconFromIDResource(nResourceID, m_NotifyIconData.hIcon);
		}

		BOOL bRet = FALSE;
		int nRetry = 10;
		while(((bRet = Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData)) != TRUE) && (nRetry > 0))
		{
			// ʧ�����������10��
			nRetry--;
		}

		return bRet;
	}

	return FALSE;
}

// �������̵�Tip��Ϣ
BOOL DuiSystem::SetTrayTip(CString szToolTip)
{
	if(m_NotifyIconData.cbSize)
	{
		m_NotifyIconData.uFlags = NIF_MESSAGE | NIF_TIP;
		CString sWindowText = DuiSystem::Instance()->GetString(_T("APP_NAME"));
		if(szToolTip != _T(""))
		{
			sWindowText += _T("\n");
			sWindowText += szToolTip;
		}
		_tcscpy_s(m_NotifyIconData.szTip, sWindowText);
		return Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
	}
	return FALSE;
}

// ���ͽ��̼�ͨ����Ϣ
BOOL DuiSystem::SendInterprocessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, CString strAppName, CString strInfo)
{
	// number of characters in memory-mapped file
	const DWORD dwMemoryFileSize = sizeof(DUI_INTERPROCESS_MSG);

	// memory-mapped file name
	const LPCTSTR sMemoryFileName = _T("DF034858-1608-4147-0604-4A0CD86F6C9F");

	HANDLE hFileMapping = NULL;
	LPVOID pViewOfFile = NULL;

	// Create file mapping which can contain dwMemoryFileSize characters
	hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,           // system paging file
		NULL,                           // security attributes
		PAGE_READWRITE,                 // protection
		0,                              // high-order DWORD of size
		dwMemoryFileSize*sizeof(TCHAR), // low-order DWORD of size
		sMemoryFileName);               // name

	DWORD dwError = GetLastError();     // if ERROR_ALREADY_EXISTS 
	// this instance is not first (other instance created file mapping)

	if(! hFileMapping)
	{
		//TRACE(_T("Creating of file mapping failed.\n"));
		return FALSE;
	}

	pViewOfFile = MapViewOfFile(
		hFileMapping,               // handle to file-mapping object
		FILE_MAP_ALL_ACCESS,        // desired access
		0,
		0,
		0);                         // map all file

	if(!pViewOfFile)
	{
		//TRACE(_T("MapViewOfFile failed.\n"));
		CloseHandle(hFileMapping);
		return FALSE;
	}

	// ��ʼ�����̼�֪ͨ��Ϣ
	DUI_INTERPROCESS_MSG interMsg;
	memset(&interMsg, 0, sizeof(DUI_INTERPROCESS_MSG));
	_tcscpy_s(interMsg.wAppName, strAppName.GetBuffer(0));
	strAppName.ReleaseBuffer();
	interMsg.uControlID = APP_IPC;	// �ؼ�IDʹ��Ԥ�������̼���Ϣ�ؼ�ID
	interMsg.uMsg = uMsg;
	interMsg.wParam = wParam;
	interMsg.lParam = lParam;
	_tcscpy_s(interMsg.wInfo, strInfo.GetBuffer(0));
	strInfo.ReleaseBuffer();
	memcpy(pViewOfFile, &interMsg, sizeof(DUI_INTERPROCESS_MSG));

	DWORD result;
	LRESULT ok = SendMessageTimeout(HWND_BROADCAST,	// ���͹㲥��Ϣ
		WM_CHECK_ITS_ME,
		wParam,			// wParam����,��ʾӦ������
		0,				// lParam����,Ĭ��Ϊ0
		SMTO_BLOCK |
		SMTO_ABORTIFHUNG,
		200,			// ��ʱʱ��
		&result);

	UnmapViewOfFile(pViewOfFile);
	CloseHandle(hFileMapping);

	return TRUE;
}

// ��־��ʼ��
void DuiSystem::InitLog()
{
	// ��ʼ����־�ļ�·������
	CString strLogFile = GetConfig(_T("logfile"));
	if(strLogFile.IsEmpty())
	{
		return;
	}
	strLogFile = GetExePath() + strLogFile;
	int nLogLevel = _ttoi(GetConfig(_T("loglevel")));
	int nLogFileSize = _ttoi(GetConfig(_T("logFileSize"))) * 1024;	// ��λ��K
	int nLogFileNumber = _ttoi(GetConfig(_T("logFileNumber")));

	CLogMgr::Instance()->SetLogFile(strLogFile);
	CLogMgr::Instance()->SetLogLevel(nLogLevel);
	CLogMgr::Instance()->SetLogFileSize(nLogFileSize);
	CLogMgr::Instance()->SetLogFileNumber(nLogFileNumber);

	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("------------------DuiVision Start-------------------"));
}

// ������־
void DuiSystem::DoneLog()
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("------------------DuiVision End-------------------"));
	CLogMgr::Release();
}

// ��¼��־
void DuiSystem::LogEvent(int nLevel, LPCTSTR lpFormat, ...)
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