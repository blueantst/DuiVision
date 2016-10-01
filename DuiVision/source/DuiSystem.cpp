#include "StdAfx.h"
#include "DuiSystem.h"
#include <tchar.h>

template<> DuiSystem* Singleton<DuiSystem>::ms_Singleton = 0;

static DuiSystem* g_pIns = NULL;
static UINT g_nIDTemplate = 0;
static CString g_strRootPath = _T("");

static ULONG_PTR gdiplusToken;
static GdiplusStartupInput gdiplusStartupInput;

DuiSystem::DuiSystem(HINSTANCE hInst, DWORD dwLangID, CString strResourceFile, UINT uAppID, UINT nIDTemplate, CString strStyle)
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
	// TinyXml设置为不压缩空格模式，默认是压缩空格，会导致超过一个的空格解析时候被转换为一个空格
	TiXmlBase::SetCondenseWhiteSpace(false);
	*/

	ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);

    createSingletons();

	// 初始化COM库
	HRESULT hr = CoInitialize(NULL);
	if( FAILED(hr) )
	{
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("CoInitialize failed"));
	}

	// 加载控件库
	LoadDuiControls();
}

DuiSystem::~DuiSystem(void)
{
	// 销毁事件处理对象
	for (size_t i = 0; i < m_vecDuiHandler.size(); i++)
	{
		CDuiHandler* pDuiHandler = m_vecDuiHandler.at(i);
		if (pDuiHandler)
		{
			delete pDuiHandler;
		}		
	}

	// 销毁DUI对话框
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		if (pDlgBase)
		{
			delete pDlgBase;
		}		
	}

	// 释放各种资源池
	m_mapCfgPool.RemoveAll();
	m_mapStylePool.RemoveAll();
	m_mapXmlPool.RemoveAll();
	m_mapSkinPool.RemoveAll();
	m_mapStringPool.RemoveAll();
	m_mapFontPool.RemoveAll();

	// 释放图片缓存
	ClearAllCachedMemFile();

	if(m_hResourceZip != NULL)
	{
		CloseZip(m_hResourceZip);
		m_hResourceZip = NULL;
	}

	// 销毁Tray图标
	DelTray();

	//if(m_rich20) FreeLibrary(m_rich20);
	//m_funCreateTextServices=NULL;
	// 释放richedit库
	CDuiRichEdit::ReleaseTextService();

	// 释放COM库
	CoUninitialize();

	// 释放控件库
	ReleaseDuiControls();

	// 必须最后调用此函数关闭GDI+，否则会导致关闭GDI+之后还调用GDI+的函数，造成异常
	destroySingletons();
}

// 判断单例是否已经创建
BOOL DuiSystem::HaveInstance()
{
	return (g_pIns != NULL);
}

// 创建单例
DuiSystem* DuiSystem::Instance()
{
	if(g_pIns == NULL)
	{
		new DuiSystem(NULL);
	}

	return g_pIns;
}

// 释放
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

	// 加载资源文件
	LoadResource();

	// 初始化日志
	InitLog();

	// 启动任务管理器线程
	m_TaskMsg.Startup();
}

void DuiSystem::destroySingletons()
{
	// 停止任务管理器线程
	m_TaskMsg.Shutdown();

	// 停止日志
	DoneLog();

	//关闭gdiplus的环境
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

// 获取操作系统版本的主版本号
int DuiSystem::GetOSMajorVersion()
{
	// 获取操作系统版本信息
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

// 获取当前语言(字符串类型)
CString DuiSystem::GetLanguage()
{
	DWORD dwLangID = DuiSystem::Instance()->GetCurrentLanguage();
	if(dwLangID == 0)
	{
		// 如果语言ID为0表示获取当前系统的语言
		dwLangID = ::GetSystemDefaultLangID();
	}
	switch(dwLangID)
	{
	case LANGUAGE_PAGE_ENGLISH:
		return _T("en-us");break;	// 英语
	case LANGUAGE_PAGE_CHINESE:
		return _T("zh-cn");break;	// 简体中文
	case LANGUAGE_PAGE_CHINESE_TW:
	case LANGUAGE_PAGE_CHINESE_HK:
	case LANGUAGE_PAGE_CHINESE_SGP:
		return _T("zh-tw");break;	// 繁体中文
	}

	return _T("en-us");	// 默认为英文
}

// 获取当前语言
DWORD DuiSystem::GetCurrentLanguage()
{
	return m_dwLangID;
}

// 设置当前语言
void DuiSystem::SetCurrentLanguage(DWORD dwLangID)
{
	m_dwLangID = dwLangID;
}

// 设置根目录
void DuiSystem::SetRootPath(CString strPath)
{
	g_strRootPath = strPath;
}

// 取进程目录
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

// 获取Skin目录
CString DuiSystem::GetSkinPath()
{
	return GetExePath();
}

// 获取XML目录
CString DuiSystem::GetXmlPath()
{
	return GetExePath() + _T("xml\\");
}

// 路径标准化
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

// 加载资源
BOOL DuiSystem::LoadResource()
{
	// 先释放各种资源池
	m_mapCfgPool.RemoveAll();
	m_mapStylePool.RemoveAll();
	m_mapXmlPool.RemoveAll();
	m_mapSkinPool.RemoveAll();
	m_mapStringPool.RemoveAll();
	m_mapFontPool.RemoveAll();

	// 释放图片缓存
	ClearAllCachedMemFile();

	// 加载zip资源文件
	CString strResFile = GetExePath() + m_strResourceFile;
	if(strResFile.Find(_T(".ui")) != -1)	// 从文件加载zip包
	{
		// 如果是后缀为ui的文件,表示是ZIP文件,则加载资源ZIP文件
		if(GetFileAttributes(strResFile) != 0xFFFFFFFF)
		{
			if(m_hResourceZip != NULL)
			{
				CloseZip(m_hResourceZip);
				m_hResourceZip = NULL;
			}
			// 将zip文件先整体加载到内存中,再通过内存的方式解压,这样可以提高加载速度
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
				// 如果加载的ZIP资源，则不使用绝对路径加载XML资源定义文件
				strResFile = _T("xml\\resource.xml");			
			}
		}else
		{
			// zip资源文件不存在,则设置为默认的xml文件
			m_strResourceFile = _T("xml\\resource.xml");
			strResFile = m_strResourceFile;
		}
	}else
	if(m_strResourceFile.Find(_T("res:")) == 0)	// 从资源中加载zip包
	{
		strResFile = m_strResourceFile;
		strResFile.Delete(0, 4);
		UINT nID = _ttoi(strResFile);	// res:后面的表示资源ID
		if(nID != 0)
		{
			// 读取资源
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
					// 如果加载的ZIP资源，则不使用绝对路径加载XML资源定义文件
					strResFile = _T("xml\\resource.xml");			
				}
			}else
			{
				// zip资源不存在,则设置为默认的xml文件
				m_strResourceFile = _T("xml\\resource.xml");
				strResFile = m_strResourceFile;
			}
		}else
		{
			// zip资源不存在,则设置为默认的xml文件
			m_strResourceFile = _T("xml\\resource.xml");
			strResFile = m_strResourceFile;
		}
	}else
	{
		strResFile = m_strResourceFile;
	}
	
	return LoadResourceXml(strResFile, m_strCurStyle);
}

// 加载XML资源文件
BOOL DuiSystem::LoadResourceXml(CString strResFile, CString strStyle)
{
	m_strCurStyle = strStyle;

	// 加载资源文件
	DuiXmlDocument xmlDoc;
	DuiXmlParseResult xmlResult;
	if(m_hResourceZip != NULL)
	{
		// 即使有zip文件的情况下,也优先使用目录中的文件
		if(GetFileAttributes(GetExePath() + strResFile) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			xmlResult = xmlDoc.load_file(GetExePath() + strResFile);
		}else
		if(GetFileAttributes(GetXmlPath() + strResFile) != 0xFFFFFFFF)	// 从xml路径开始查找
		{
			xmlResult = xmlDoc.load_file(GetXmlPath() + strResFile);
		}else
		if(GetFileAttributes(strResFile) != 0xFFFFFFFF)	// 绝对路径查找
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
		if(GetFileAttributes(GetExePath() + strResFile) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			xmlResult = xmlDoc.load_file(GetExePath() + strResFile);
		}else
		if(GetFileAttributes(GetXmlPath() + strResFile) != 0xFFFFFFFF)	// 从xml路径开始查找
		{
			xmlResult = xmlDoc.load_file(GetXmlPath() + strResFile);
		}else
		if(GetFileAttributes(strResFile) != 0xFFFFFFFF)	// 绝对路径查找
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
				if(strType == _T("res"))	// 资源文件
				{
					CString strLang = pResElem.attribute(_T("lang")).value();
					if(strLang.IsEmpty() || (strLang == DuiSystem::GetLanguage()))
					{
						// 加载资源文件
						CString strFile = pResElem.attribute(_T("file")).value();
						CString strFileU = GetExePath() + strFile;
						if(m_hResourceZip != NULL)
						{
							strFileU = strFile;
						}
						LoadResourceXml(strFileU, strStyle);
					}
				}else
				if(strType == "cfg")	// 全局配置
				{
					CString strName = pResElem.attribute(_T("name")).value();
					CString strValue = pResElem.attribute(_T("value")).value();
					m_mapCfgPool.SetAt(strName, strValue);
					// 如果DuiSystem未设置当前风格参数,则可以通过defaultStyle配置来决定当前风格
					if(m_strCurStyle.IsEmpty() && (strName == _T("defaultStyle")))
					{
						strStyle = strValue;
						m_strCurStyle = strValue;
					}
				}else
				if(strType == _T("style"))	// 风格定义
				{
					CString strName = pResElem.attribute(_T("name")).value();
					CString strValue = pResElem.attribute(_T("value")).value();
					m_mapStylePool.SetAt(strName, strValue);
				}else
				if(strType == _T("xml"))	// XML文件定义
				{
					CString strStyle = pResElem.attribute(_T("style")).value();
					CString strName = pResElem.attribute(_T("name")).value();
					CString strFile = pResElem.attribute(_T("file")).value();
					if(strStyle.IsEmpty() || (strStyle == m_strCurStyle))
					{
						m_mapXmlPool.SetAt(strName, strFile);
					}
				}else
				if(strType == _T("img"))	// 图像
				{
					CString strStyle = pResElem.attribute(_T("style")).value();
					CString strName = pResElem.attribute(_T("name")).value();
					CString strFile = pResElem.attribute(_T("file")).value();
					if(strStyle.IsEmpty() || (strStyle == m_strCurStyle))
					{
						m_mapSkinPool.SetAt(strName, strFile);
					}
				}else
				if(strType == _T("str"))	// 字符串
				{
					CString strLang = pResElem.attribute(_T("lang")).value();
					if(strLang.IsEmpty() || (strLang == DuiSystem::GetLanguage()))
					{
						// 如果未指定语言或指定了语言并且和当前语言相同，则加载字符串资源
						CString strStyle = pResElem.attribute(_T("style")).value();
						CString strName = pResElem.attribute(_T("name")).value();
						CString strValue = pResElem.attribute(_T("value")).value();
						if(strStyle.IsEmpty() || (strStyle == m_strCurStyle))
						{
							m_mapStringPool.SetAt(strName, strValue);
						}
					}
				}else
				if(strType == _T("font"))	// 字体
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
					// 将字体做一下过滤,对于不支持的字体用缺省字体替换
					fontInfo.strFont = DuiSystem::GetDefaultFont(strFont);
					fontInfo.nFontWidth = nFontWidth;
					fontInfo.fontStyle = fontStyle;
					fontInfo.strOS = strOS;
					if(!fontInfo.strOS.IsEmpty())
					{
						// 如果OS属性非空,则判断当前操作系统是否符合OS属性
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

// 加载ZIP资源文件
BYTE* DuiSystem::LoadZipFile(CString strFile, DWORD& dwSize)
{
	dwSize = 0;
	if(m_hResourceZip == NULL)
	{
		return NULL;
	}

	// Zip文件中的路径都使用/
	strFile.Replace(_T("\\"), _T("/"));

	// 判断文件是否已经被缓存,如果已经缓存就返回缓存的数据
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

		// 更新缓存数据
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

// 获取缓存的内存文件信息
BOOL DuiSystem::GetCachedMemFile(CString strFile, MemFileInfo& memFile)
{
	strFile.MakeLower();
	return m_mapMemFileCatch.Lookup(strFile, memFile);
}

// 设置缓存的内存文件信息
void DuiSystem::SetCachedMemFile(CString strFile, MemFileInfo& memFile)
{
	strFile.MakeLower();
	m_mapMemFileCatch.SetAt(strFile, memFile);
}

// 清除所有内存文件缓存
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

// 加载XML文件,支持从zip文件中加载
// 格式1: file-resource-name		-- 根据资源名加载,先找到文件名,到资源目录或资源zip中加载
// 格式2: filename.xml				-- 指定XML文件名,到资源目录或资源zip中加载
// 格式3: c:\filename.xml			-- 指定了XML的全路径
BOOL DuiSystem::LoadXmlFile(DuiXmlDocument& xmlDoc, CString strFileName)
{
	CString strXmlFile;
	DuiXmlParseResult xmlResult;
	if(strFileName.Find(_T("xml:")) == 0)	// 去掉前缀
	{
		strFileName.Delete(0, 4);
	}
	if(strFileName.Find(_T(".xml")) == -1)	// 需要到资源定义中查找
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

	if(m_hResourceZip != NULL)	// 存在资源zip文件
	{
		// 即使有zip文件的情况下,也优先使用目录中的文件
		if(GetFileAttributes(GetExePath() + strXmlFile) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			xmlResult = xmlDoc.load_file(GetExePath() + strXmlFile);
		}else
		if(GetFileAttributes(GetXmlPath() + strXmlFile) != 0xFFFFFFFF)	// 从xml路径开始查找
		{
			xmlResult = xmlDoc.load_file(GetXmlPath() + strXmlFile);
		}else
		if(GetFileAttributes(strXmlFile) != 0xFFFFFFFF)	// 绝对路径查找
		{
			xmlResult = xmlDoc.load_file(strXmlFile);
		}else
		{
			DWORD dwSize = 0;
			BYTE* pByte = LoadZipFile(strXmlFile, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("xml\\") + strXmlFile, dwSize);	// 尝试从xml子目录加载
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
		if(GetFileAttributes(GetExePath() + strXmlFile) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			xmlResult = xmlDoc.load_file(GetExePath() + strXmlFile);
		}else
		if(GetFileAttributes(GetXmlPath() + strXmlFile) != 0xFFFFFFFF)	// 从xml路径开始查找
		{
			xmlResult = xmlDoc.load_file(GetXmlPath() + strXmlFile);
		}else
		if(GetFileAttributes(strXmlFile) != 0xFFFFFFFF)	// 绝对路径查找
		{
			xmlResult = xmlDoc.load_file(strXmlFile);
		}else
		{
			// 文件不存在
			DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("DuiSystem::LoadXmlFile %s failed, not found xml file"), strXmlFile);
			return FALSE;
		}
	}

	return xmlResult;
}

// 加载图片文件,支持从zip文件中加载
BOOL DuiSystem::LoadImageFile(CString strFileName, BOOL useEmbeddedColorManagement, Image*& pImage)
{
	//BOOL bRet = LoadImageFromFile(strFileName, useEmbeddedColorManagement, pImage);
	BOOL bRet = FALSE;
	if(m_hResourceZip != NULL)	// 存在资源zip文件
	{
		// 即使有zip文件的情况下,也优先使用目录中的文件
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			bRet = LoadImageFromFile(DuiSystem::GetSkinPath() + strFileName, useEmbeddedColorManagement, pImage);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// 绝对路径查找
		{
			bRet = LoadImageFromFile(strFileName, useEmbeddedColorManagement, pImage);
		}else
		{
			DWORD dwSize = 0;
			BYTE* pByte = LoadZipFile(strFileName, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("skins\\") + strFileName, dwSize);	// 尝试从skins子目录加载
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
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			bRet = LoadImageFromFile(DuiSystem::GetSkinPath() + strFileName, useEmbeddedColorManagement, pImage);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// 绝对路径查找
		{
			bRet = LoadImageFromFile(strFileName, useEmbeddedColorManagement, pImage);
		}else
		{
			// 文件不存在
			return FALSE;
		}
	}
	return bRet;
}

// 加载图片文件,支持从zip文件中加载
BOOL DuiSystem::LoadBitmapFile(CString strFileName, CBitmap &bitmap, CSize &size)
{
	BOOL bRet = FALSE;
	if(m_hResourceZip != NULL)	// 存在资源zip文件
	{
		// 即使有zip文件的情况下,也优先使用目录中的文件
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			bRet = LoadBitmapFromFile(DuiSystem::GetSkinPath() + strFileName, bitmap, size);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// 绝对路径查找
		{
			bRet = LoadBitmapFromFile(strFileName, bitmap, size);
		}else
		{
			DWORD dwSize = 0;
			BYTE* pByte = LoadZipFile(strFileName, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("skins\\") + strFileName, dwSize);	// 尝试从skins子目录加载
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
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			bRet = LoadBitmapFromFile(DuiSystem::GetSkinPath() + strFileName, bitmap, size);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// 绝对路径查找
		{
			bRet = LoadBitmapFromFile(strFileName, bitmap, size);
		}else
		{
			// 文件不存在
			return FALSE;
		}
	}
	return bRet;
}

// 加载图标文件,支持从zip文件中加载
BOOL DuiSystem::LoadIconFile(CString strFileName, HICON& hIcon)
{
	BOOL bRet = FALSE;
	if(m_hResourceZip != NULL)	// 存在资源zip文件
	{
		// 即使有zip文件的情况下,也优先使用目录中的文件
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			bRet = LoadIconFromFile(DuiSystem::GetSkinPath() + strFileName, hIcon);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// 绝对路径查找
		{
			bRet = LoadIconFromFile(strFileName, hIcon);
		}else
		{
			DWORD dwSize = 0;
			BYTE* pByte = LoadZipFile(strFileName, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("skins\\") + strFileName, dwSize);	// 尝试从skins子目录加载
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
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			bRet = LoadIconFromFile(DuiSystem::GetSkinPath() + strFileName, hIcon);
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// 绝对路径查找
		{
			bRet = LoadIconFromFile(strFileName, hIcon);
		}else
		{
			// 文件不存在
			return FALSE;
		}
	}
	return bRet;
}

// 加载通用文件到内存,支持从zip文件中加载
BOOL DuiSystem::LoadFileToBuffer(CString strFileName, BYTE*& pBuffer)
{
	BOOL bRet = FALSE;
	BYTE* pByte = NULL;
	CString _strFileName = _T("");
	if(m_hResourceZip != NULL)	// 存在资源zip文件
	{
		// 即使有zip文件的情况下,也优先使用目录中的文件
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			_strFileName = DuiSystem::GetSkinPath() + strFileName;
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// 绝对路径查找
		{
			_strFileName = strFileName;
		}else
		{
			DWORD dwSize = 0;
			pByte = LoadZipFile(strFileName, dwSize);
			if(pByte == NULL)
			{
				pByte = LoadZipFile(_T("skins\\") + strFileName, dwSize);	// 尝试从skins子目录加载
			}
			if(pByte == NULL)
			{
				return FALSE;
			}
		}
	}else
	{
		if(GetFileAttributes(DuiSystem::GetSkinPath() + strFileName) != 0xFFFFFFFF)	// 从exe路径开始查找
		{
			_strFileName = DuiSystem::GetSkinPath() + strFileName;
		}else
		if(GetFileAttributes(strFileName) != 0xFFFFFFFF)	// 绝对路径查找
		{
			_strFileName = strFileName;
		}else
		{
			// 文件不存在
			return FALSE;
		}
	}

	// 直接从文件加载
	if((pByte == NULL) && (!_strFileName.IsEmpty()))
	{
		CFile file;
		// 打开文件
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

// 加载界面插件动态库
// 格式1: file-resource-name		-- 根据资源名加载,先找到文件名,到exe目录加载
// 格式2: filename.dll				-- 指定文件名,到exe目录加载
// 格式3: c:\filename.dll			-- 指定了dll的全路径
typedef LPVOID (*TYPEOF_CreateObject)(LPCTSTR lpcsInterfaceName, LPVOID* lppVciControl, LPVOID lpInitData);
BOOL DuiSystem::LoadPluginFile(CString strFileName, CString strObjType, HINSTANCE& hPluginHandle, LPVOID& pPluginObj)
{
	hPluginHandle = NULL;
	pPluginObj = NULL;
	CString strPluginFile;
	if(strFileName.Find(_T(".dll")) == -1)	// 需要到资源定义中查找
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

	// 加载插件动态库
	hPluginHandle = NULL;
	if(GetFileAttributes(DuiSystem::GetExePath() + strPluginFile) != 0xFFFFFFFF)	// 从exe路径开始查找
	{
		hPluginHandle = LoadLibrary(DuiSystem::GetExePath() + strPluginFile);
	}else
	if(GetFileAttributes(strPluginFile) != 0xFFFFFFFF)	// 绝对路径查找
	{
		hPluginHandle = LoadLibrary(strPluginFile);
	}

	if(hPluginHandle == NULL)
	{
		// 加载失败
		DWORD dwError = ::GetLastError();
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("Load UI plugin %s failed, errorcode is %u"), strPluginFile, dwError);
		return FALSE;
	}

	// 加载VCI组件对象
	// 获取函数指针
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

// 获取系统配置信息
CString DuiSystem::GetConfig(CString strName)
{
	CString strCfg;
	if(m_mapCfgPool.Lookup(strName, strCfg))
	{
		return strCfg;
	}
	return _T("");
}

// 获取XML文件
CString DuiSystem::GetXmlFile(CString strName)
{
	CString strXmlFile;
	if(m_mapXmlPool.Lookup(strName, strXmlFile))
	{
		return strXmlFile;
	}
	return _T("");
}

// 获取Skin
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

// 获取字符串值
CString DuiSystem::GetString(CString strName)
{
	CString strString;
	m_mapStringPool.Lookup(strName, strString);
	// 将字符串中的替换符进行替换
	ParseDuiString(strString);
	return strString;
}

// 设置字符串值
void DuiSystem::SetString(CString strName, CString strValue)
{
	m_mapStringPool.SetAt(strName, strValue);
}

// 解析字符串，替换其中的替换内容
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

// 获取操作系统名字
CString DuiSystem::GetOSName()
{
	int nOSType       =  OS_UNKNOWN;
	int nServicePack  =  PACK_UNKNOWN;
	
	// 获取操作系统版本信息
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
	
	//操作系统补丁版本
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

		//操作系统版本比Win8更高
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

// 检查当前操作系统是否在指定的OS字符串范围内
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

// 获取字体信息
BOOL DuiSystem::GetFont(CString strName, DuiFontInfo& fontInfo)
{
	return m_mapFontPool.Lookup(strName, fontInfo);
}

// 获取缺省字体信息(如果给了输入参数,则判断输入参数是否不合适,不合适就换成可以用的)
// 目前仅针对简体中文和英文系统的默认字体进行处理
CString DuiSystem::GetDefaultFont(CString strFont)
{
	int nOSVer = DuiSystem::GetOSMajorVersion();
	if(strFont.IsEmpty())
	{
		// 输入为空则按照操作系统版本和当前语言决定默认字体
		if(nOSVer >= 6)
		{
			return (GetLanguage() == "zh-cn") ? TEXT("微软雅黑") : TEXT("Segoe UI");
		}else
		{
			return TEXT("Tahoma");
		}
	}else
	if((nOSVer < 6) && ((strFont == _T("微软雅黑")) || (strFont == _T("Segoe UI"))))
	{
		// 如果是低版本操作系统,则不能用微软雅黑
		return TEXT("Tahoma");
	}else
	if(!strFont.IsEmpty())
	{
		// 符合条件则直接返回输入的字体
		return strFont;
	}else
	if(nOSVer >= 6)
	{
		// Vista以上默认字体
		return (GetLanguage() == "zh-cn") ? TEXT("微软雅黑") : TEXT("Segoe UI");
	}

	// 低版本操作系统默认字体
	return TEXT("Tahoma");	
}

// 获取窗口背景信息
BOOL DuiSystem::GetWindowBkInfo(int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile)
{
	// 调用事件处理对象从应用获取保存的背景信息
	CDuiHandler* pDuiHandler = GetDuiHandler(0);
	if(pDuiHandler == NULL)
	{
		return FALSE;
	}

	// 首先获取Skin类型，然后根据类型获取Skin值
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

// 设置窗口背景信息
BOOL DuiSystem::SetWindowBkInfo(int nType, int nIDResource, COLORREF clr, CString strImgFile)
{
	// 调用事件处理对象将背景信息保存到应用
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

// 注册控件
void DuiSystem::RegisterDuiControl(CDuiObjectInfo* pDuiObjectInfo)
{
	// 判断如果已经添加过，就不用重复添加
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

// 卸载控件,传入的参数为控件名
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

// 加载控件库
void DuiSystem::LoadDuiControls()
{
	// 注册DuiVision默认的控件
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

// 释放控件库
void DuiSystem::ReleaseDuiControls()
{
	for (size_t i = 0; i < m_vecDuiObjectInfo.size(); i++)
	{
		CDuiObjectInfo* pDuiObjectInfo = m_vecDuiObjectInfo.at(i);
		if (pDuiObjectInfo)
		{
			// 执行控件的依赖库释放函数
			if(pDuiObjectInfo->m_pfShutdown)
			{
				pDuiObjectInfo->m_pfShutdown();
			}

			delete pDuiObjectInfo;
		}		
	}
}

// 根据控件类名创建控件实例
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

// 根据控件ID获取子控件对象（从所有注册的DUI对话框中查找）
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

// 根据控件ID获取子控件对象
CControlBase* DuiSystem::GetControl(CDuiObject* pDuiObject, UINT uControlID)
{
	if(pDuiObject == NULL)
	{
		return NULL;
	}

	// 首先判断是否关联的DUI对象
	if(pDuiObject->GetID() == uControlID)
	{
		return (CControlBase*)pDuiObject;
	}

	// 判断是否下层DUI控件对象
	if(pDuiObject->IsClass(_T("dlg")))
	{
		CDlgBase* pDlgBase = (CDlgBase*)pDuiObject;
		CControlBase* pControlBase = pDlgBase->GetControl(uControlID);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}else
		{
			// 查找base control
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

	// 如果没有找到，再到所有注册的对话框中查找（仅针对ID查找方式）
	return DuiSystem::Instance()->GetControlFromDuiDialog(uControlID);
}

// 根据控件名获取子控件对象
CControlBase* DuiSystem::GetControl(CDuiObject* pDuiObject, CString strControlName)
{
	if(pDuiObject == NULL)
	{
		return NULL;
	}

	// 首先判断是否关联的DUI对象
	if(pDuiObject->GetName() == strControlName)
	{
		return (CControlBase*)pDuiObject;
	}

	// 判断是否下层DUI控件对象
	if(pDuiObject->IsClass(_T("dlg")))
	{
		CDlgBase* pDlgBase = (CDlgBase*)pDuiObject;
		CControlBase* pControlBase = pDlgBase->GetControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}else
		{
			// 查找base control
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

// 根据对话框名和子控件名获取对话框中的子控件对象
CControlBase* DuiSystem::GetDialogControl(CString strDialogName, CString strControlName)
{
	CDlgBase* pDuiObject = DuiSystem::Instance()->GetDuiDialog(strDialogName);
	if(pDuiObject == NULL)
	{
		return NULL;
	}

	// 判断是否下层DUI控件对象
	CDlgBase* pDlgBase = (CDlgBase*)pDuiObject;
	CControlBase* pControlBase = pDlgBase->GetControl(strControlName);
	if(pControlBase != NULL)
	{
		return pControlBase;
	}else
	{
		// 查找base control
		pControlBase = pDlgBase->GetBaseControl(strControlName);
		if(pControlBase != NULL)
		{
			return pControlBase;
		}
	}

	return NULL;
}

// 添加DUI事件处理对象
void DuiSystem::AddDuiHandler(CDuiHandler* pDuiHandler)
{
	// 判断如果已经添加过，就不用重复添加
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

// 删除DUI事件处理对象
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

// 获取DUI事件处理对象(根据索引)
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

// 给指定的DUI对象注册事件处理对象
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

	// 是否注册之后立即初始化
	if(bInit)
	{
		pDuiHandler->OnInit();
	}

	return TRUE;
}

// 调用所有注册的事件处理对象进行处理
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

// 添加DUI对话框
void DuiSystem::AddDuiDialog(CDlgBase* pDuiDialog)
{
	m_vecDuiDialog.push_back(pDuiDialog);
}

// 删除DUI对话框
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

// 获取DUI对话框指针(根据索引)
CDlgBase* DuiSystem::GetDuiDialog(int nIndex)
{
	if(nIndex < 0 || nIndex > ((int)m_vecDuiDialog.size() - 1))
	{
		return NULL;
	}
	return m_vecDuiDialog.at(nIndex);
}

// 获取DUI对话框指针(根据name)
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

// 更新所有对话框的背景皮肤
void DuiSystem::ResetAllWindowsBkSkin()
{
	for (size_t i = 0; i < m_vecDuiDialog.size(); i++)
	{
		CDlgBase* pDlgBase = m_vecDuiDialog.at(i);
		pDlgBase->InitWindowBkSkin();
	}
}

// 创建对话框
CDlgBase* DuiSystem::CreateDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule, UINT nIDTemplate, BOOL bAdd)
{
	// 解析XML模版文件或内容
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
		// 设置name
		pDlg->SetName(strName);
	}

	if(bAdd)
	{
		// 添加到对话框列表中
		DuiSystem::Instance()->AddDuiDialog(pDlg);
	}

	if(!bModule)
	{
		// 如果是非模式对话框,则直接创建窗口
		pDlg->Create(pDlg->GetIDTemplate(), NULL);
	}

	return pDlg;
}

// 显示对话框
int DuiSystem::ShowDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(lpszXmlTemplate, pParentObject, strName, bModule, 0, TRUE);
	if(pDlg == NULL)
	{
		return 0;
	}

	// 如果是非模态,则显示窗口之后返回
	if(!bModule)
	{
		pDlg->ShowWindow(SW_SHOW);
		return 0;
	}

	// 模态对话框,调用DoModal显示对话框
	int nResponse = pDlg->DoModal();
	DuiSystem::Instance()->RemoveDuiDialog(pDlg);
	return nResponse;
}

// 通用信息对话框
int DuiSystem::DuiMessageBox(CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption, UINT uType, int nWidth, int nHeight)
{
	CDlgBase* pDlg = DuiSystem::CreateDuiDialog(_T("dlg_msgbox"), pParent, _T(""), TRUE, 0, TRUE);
	if(pDlg == NULL)
	{
		return 0;
	}

	// 文字内容和标题
	pDlg->SetControlValue(_T("msgbox.text"), _T("title"), lpszText);
	if(lpszCaption != _T(""))
	{
		pDlg->SetControlValue(_T("msgbox.title"), _T("title"), lpszCaption);
	}

	// 按钮
	UINT uButtonType = uType & 0x0F;
	for(UINT i=0; i< 10; i++)
	{
		CString strButtonDivName;
		strButtonDivName.Format(_T("msgbox.type.%u"), i);
		pDlg->SetControlValue(strButtonDivName, _T("visible"), (uButtonType == i) ? _T("1") : _T("0"));
	}

	// 图标
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

// 动态提示信息框操作接口:创建动态信息提示框
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

// 删除动态信息提示框
void DuiSystem::RemoveNotifyMsgBox()
{
	if(m_pNotifyMsgBox != NULL)
	{
		RemoveDuiDialog(m_pNotifyMsgBox);
		m_pNotifyMsgBox = NULL;
	}
}

// 设置动态信息提示框控件文字
void DuiSystem::SetNotifyMsgBoxControlTitle(CString strControlName, CString strControlTitle)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		CControlBaseFont* pControl = (CControlBaseFont*)(pNotifyMsgBox->GetControl(strControlName));
		if(pControl == NULL)
		{
			// 如果在普通控件中未找到,则找一下基础控件
			pControl = (CControlBaseFont*)(pNotifyMsgBox->GetBaseControl(strControlName));
		}
		if(pControl)
		{
			pControl->SetTitle(strControlTitle);
		}
	}
}

// 设置动态信息提示框控件图片
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

// 设置动态信息提示框控件可见性
void DuiSystem::SetNotifyMsgBoxControlVisible(CString strControlName, BOOL bVisible)
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		CControlBase* pControl = pNotifyMsgBox->GetControl(strControlName);
		if(pControl == NULL)
		{
			// 如果在普通控件中未找到,则找一下基础控件
			pControl = (CControlBaseFont*)(pNotifyMsgBox->GetBaseControl(strControlName));
		}
		if(pControl)
		{
			pControl->SetVisible(bVisible);
		}
	}
}

// 设置动态信息提示框大小
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

// 显示动态信息提示框
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
			//居中显示
			rect.MoveToXY((nScreenWidth-rect.Width()) / 2, (nScreenHeight-rect.Height()) / 2);
			pNotifyMsgBox->MoveWindow(rect, FALSE);
		}*/
		// 显示到Windows右下角
		rect.MoveToXY(nScreenWidth-rect.Width(), nScreenHeight-rect.Height());
		pNotifyMsgBox->MoveWindow(rect, FALSE);
		// 把窗口放在最前面显示
		::SetForegroundWindow(pNotifyMsgBox->m_hWnd);
		pNotifyMsgBox->ShowWindow(SW_SHOW);
		::BringWindowToTop(pNotifyMsgBox->m_hWnd);
		// 窗口置顶
		::SetWindowPos(pNotifyMsgBox->m_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

		// 设置自动关闭定时器(隐藏窗口模式,将Notify窗口隐藏)
		pNotifyMsgBox->SetAutoCloseTimer(uDelayTime, TRUE);
	}
}

// 隐藏动态信息提示框
void DuiSystem::HideNotifyMsgBox()
{
	CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
	if (pNotifyMsgBox != NULL)
	{
		pNotifyMsgBox->ShowWindow(SW_HIDE);
	}
}

// 执行程序
BOOL DuiSystem::ExecuteProcess(CString strProcessName, CString strCmdLine, BOOL bForceAdmin, BOOL bWaitProcess)
{
    BOOL bRet = FALSE;
    PROCESS_INFORMATION processInfo;
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    HANDLE hProcess = NULL;

    if (bForceAdmin)	// 管理员方式运行
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

// 执行UI线程任务(将一个任务通过给主窗口发消息,在主窗口消息中再执行的方式实现任务线程到主界面线程的任务中转)
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
// DUI动作任务类
//
class CDuiActionTask : public DuiVision::IBaseTask
{
public:
	CDuiActionTask(LONG type, UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam, CString strControlName, CString strAction, CDuiObject* pParent)
		: DuiVision::IBaseTask(type), m_uID(uID), m_uMsg(uMsg), m_wParam(wParam), m_lParam(lParam),
		m_pParent(pParent), m_strControlName(strControlName), m_strAction(strAction)
	{
		SetUITask(TRUE);	// 设置为需要转UI线程处理的任务
	}

	// 任务处理
	virtual BOOL TaskProcess(DuiVision::CTaskMgr *pMgr)
	{
		DoAction();
		return TRUE;
	}

	void DoAction()
	{
		if(!m_strAction.IsEmpty())
		{
			// 如果设置了action,则解析执行
			if(m_strAction.Find(_T("dlg:")) == 0)	// 动作:打开一个对话框,有内存泄漏，改为通过DuiSystem创建和管理
			{
				if(m_uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
				{
					CString strXmlFile = m_strAction;
					strXmlFile.Delete(0, 4);
					if(!strXmlFile.IsEmpty())
					{
						DuiSystem::ShowDuiDialog(strXmlFile, m_pParent);
					}
				}
			}else
			if(m_strAction.Find(_T("link:")) == 0)	// 动作:打开一个页面链接
			{
				if(m_uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
				{
					CString strLink = m_strAction;
					strLink.Delete(0, 5);
					if(!strLink.IsEmpty())
					{
						ShellExecute(NULL, TEXT("open"), strLink, NULL,NULL,SW_NORMAL);
					}
				}
			}else
			if(m_strAction.Find(_T("run:")) == 0)	// 动作:执行一个进程
			{
				if(m_uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
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
						DuiSystem::PathCanonicalize(strProcess);	// 路径标准化
						DuiSystem::ExecuteProcess(strProcess, strCmdLine, bForceAdmin, bWaitProcess);
					}
				}
			}else
			if(m_strAction.Find(ACTION_CLOSE_WINDOW) == 0)	// 动作:关闭指定的窗口
			{
				if(m_uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
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
			if(m_strAction.Find(ACTION_SHOW_WINDOW) == 0)	// 动作:显示指定的窗口
			{
				if(m_uMsg == MSG_BUTTON_UP)	// 鼠标放开事件才处理
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
			// 找到控件,调用控件的消息处理
			CControlBase* pControl = DuiSystem::GetControl(m_pParent, m_uID);
			if(pControl)
			{
				pControl->CallDuiHandler(m_uMsg, m_wParam, m_lParam);
			}else
			{
				// 如果未找到控件,则通过DuiSystem调用所有注册的事件处理对象进行处理
				DuiSystem::Instance()->CallDuiHandler(m_uID, m_strControlName, m_uMsg, m_wParam, m_lParam);
			}
		}
	}

protected:
	CDuiObject*	m_pParent;			// 父对象
	UINT		m_uID;				// 控件ID
	CString		m_strControlName;	// 控件名
	UINT		m_uMsg;				// 消息
	WPARAM		m_wParam;			// 参数1
	LPARAM		m_lParam;			// 参数2
	CString		m_strAction;		// 动作
};

// 添加DUI动作任务
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
// 提示信息任务类
//
class CDuiNotifyMsgTask : public DuiVision::IBaseTask
{
public:
	CDuiNotifyMsgTask(LONG type, CString strMsg, UINT uNotifyType = MB_ICONINFORMATION | 2, CString strCaption = _T(""), int nDelayTime = 0, int nWidth = 0, int nHeight = 0)
		: DuiVision::IBaseTask(type), m_strMsg(strMsg), m_uNotifyType(uNotifyType), m_strCaption(strCaption),
		m_nDelayTime(nDelayTime), m_nWidth(nWidth), m_nHeight(nHeight)
	{
		SetUITask(TRUE);	// 设置为需要转UI线程处理的任务
	}

	// 任务处理
	virtual BOOL TaskProcess(DuiVision::CTaskMgr *pMgr)
	{
		DoAction();
		return TRUE;
	}

	void DoAction()
	{
		// 每次都先删除旧的窗口,重新创建一次,否则窗口曾经被关闭过的话,响应消息会有问题
		// 如果窗口已经被关闭了，则需要重新创建,否则响应消息会有问题
		CDlgBase* pNotifyMsgBox = DuiSystem::Instance()->GetNotifyMsgBox();
		if((pNotifyMsgBox == NULL) || !::IsWindow(pNotifyMsgBox->GetSafeHwnd()))
		{
			DuiSystem::Instance()->CreateNotifyMsgBox(_T("dlg_notifymsg"));
		}

		// 文字内容和标题
		DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.text"), m_strMsg);
		if(m_strCaption != _T(""))
		{
			DuiSystem::SetNotifyMsgBoxControlTitle(_T("notify.title"), m_strCaption);
		}

		// 按钮
		UINT uButtonType = m_uNotifyType & 0x0F;
		for(UINT i=0; i< 10; i++)
		{
			CString strButtonDivName;
			strButtonDivName.Format(_T("notify.type.%u"), i);
			DuiSystem::SetNotifyMsgBoxControlVisible(strButtonDivName, (uButtonType == i) ? TRUE : FALSE);
		}

		// 图标
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
	CString		m_strXmlTemplate;	// XML模版
	UINT		m_uNotifyType;		// 提示框图标和类型
	CString		m_strMsg;			// 提示信息
	CString		m_strCaption;		// 标题
	int			m_nCheck;			// 检查框的值
	int			m_nWidth;			// 窗口宽度
	int			m_nHeight;			// 窗口高度
	int			m_nDelayTime;		// 延迟时间(秒)
};

// 添加提示信息任务
void DuiSystem::AddDuiNotifyMsgTask(CString strMsg, UINT uNotifyType, CString strCaption, int nDelayTime, int nWidth, int nHeight)
{
	DuiVision::CTaskMgr* pTaskMgr = DuiSystem::Instance()->GetTaskMgr();
	if(pTaskMgr)
	{
		pTaskMgr->AddTask(new CDuiNotifyMsgTask(2, strMsg, uNotifyType, strCaption, nDelayTime, nWidth, nHeight));
		pTaskMgr->StartTask();
	}
}

// 创建弹出框
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

// 添加定时器
UINT DuiSystem::AddDuiTimer(UINT uTimerResolution, CString strTimerName, BOOL bAppTimer)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return 0;
	}

	return ((CTimer*)pDlg)->SetTimer(uTimerResolution, strTimerName, bAppTimer);
}

// 删除定时器(根据ID删除)
void DuiSystem::RemoveDuiTimer(UINT uTimerID)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return;
	}

	((CTimer*)pDlg)->KillTimer(uTimerID);
}

// 删除定时器(根据名字删除)
void DuiSystem::RemoveDuiTimer(CString strTimerName)
{
	CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(0);
	if(pDlg == NULL)
	{
		return;
	}

	((CTimer*)pDlg)->KillTimer(strTimerName);
}

// 调用事件处理对象的定时器处理函数
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

// 初始化托盘图标
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

	// 释放图标资源
	if(m_NotifyIconData.hIcon != NULL)
	{
		DestroyIcon(m_NotifyIconData.hIcon);
		m_NotifyIconData.hIcon = NULL;
	}

	if(strTrayIcon.Find(_T(".")) != -1)	// 加载图标文件
	{
		DuiSystem::Instance()->LoadIconFile(strTrayIcon, m_NotifyIconData.hIcon);
	}else	// 加载图标资源
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
		// 失败则重试最多3次
		nRetry--;
		Sleep(100);
	}

	return bRet;
}

// 删除托盘图标
void DuiSystem::DelTray()
{
	if(m_NotifyIconData.cbSize)
	{
		// 释放图标资源
		if(m_NotifyIconData.hIcon != NULL)
		{
			DestroyIcon(m_NotifyIconData.hIcon);
		}
		Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
		ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);
	}
}

// 设置托盘图标
BOOL DuiSystem::SetTrayIcon(CString strIcon)
{
	if(m_NotifyIconData.cbSize)
	{
		m_NotifyIconData.uFlags = NIF_ICON;

		// 释放图标资源
		if(m_NotifyIconData.hIcon != NULL)
		{
			DestroyIcon(m_NotifyIconData.hIcon);
			m_NotifyIconData.hIcon = NULL;
		}

		if(strIcon.Find(_T(".")) != -1)	// 加载图标文件
		{
			DuiSystem::Instance()->LoadIconFile(strIcon, m_NotifyIconData.hIcon);
		}else	// 加载图标资源
		{
			UINT nResourceID = _ttoi(strIcon);
			LoadIconFromIDResource(nResourceID, m_NotifyIconData.hIcon);
		}

		BOOL bRet = FALSE;
		int nRetry = 10;
		while(((bRet = Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData)) != TRUE) && (nRetry > 0))
		{
			// 失败则重试最多10次
			nRetry--;
		}

		return bRet;
	}

	return FALSE;
}

// 设置托盘的Tip信息
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

// 发送进程间通信消息
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

	// 初始化进程间通知消息
	DUI_INTERPROCESS_MSG interMsg;
	memset(&interMsg, 0, sizeof(DUI_INTERPROCESS_MSG));
	_tcscpy_s(interMsg.wAppName, strAppName.GetBuffer(0));
	strAppName.ReleaseBuffer();
	interMsg.uControlID = APP_IPC;	// 控件ID使用预定义的进程间消息控件ID
	interMsg.uMsg = uMsg;
	interMsg.wParam = wParam;
	interMsg.lParam = lParam;
	_tcscpy_s(interMsg.wInfo, strInfo.GetBuffer(0));
	strInfo.ReleaseBuffer();
	memcpy(pViewOfFile, &interMsg, sizeof(DUI_INTERPROCESS_MSG));

	DWORD result;
	LRESULT ok = SendMessageTimeout(HWND_BROADCAST,	// 发送广播消息
		WM_CHECK_ITS_ME,
		wParam,			// wParam参数,表示应用类型
		0,				// lParam参数,默认为0
		SMTO_BLOCK |
		SMTO_ABORTIFHUNG,
		200,			// 超时时间
		&result);

	UnmapViewOfFile(pViewOfFile);
	CloseHandle(hFileMapping);

	return TRUE;
}

// 日志初始化
void DuiSystem::InitLog()
{
	// 初始化日志文件路径和锁
	CString strLogFile = GetConfig(_T("logfile"));
	if(strLogFile.IsEmpty())
	{
		return;
	}
	strLogFile = GetExePath() + strLogFile;
	int nLogLevel = _ttoi(GetConfig(_T("loglevel")));
	int nLogFileSize = _ttoi(GetConfig(_T("logFileSize"))) * 1024;	// 单位是K
	int nLogFileNumber = _ttoi(GetConfig(_T("logFileNumber")));

	CLogMgr::Instance()->SetLogFile(strLogFile);
	CLogMgr::Instance()->SetLogLevel(nLogLevel);
	CLogMgr::Instance()->SetLogFileSize(nLogFileSize);
	CLogMgr::Instance()->SetLogFileNumber(nLogFileNumber);

	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("------------------DuiVision Start-------------------"));
}

// 结束日志
void DuiSystem::DoneLog()
{
	DuiSystem::LogEvent(LOG_LEVEL_DEBUG, _T("------------------DuiVision End-------------------"));
	CLogMgr::Release();
}

// 记录日志
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