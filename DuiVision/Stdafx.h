// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#define WINVER		0x0501

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#include <afxcmn.h>         // MFC �� Windows �����ؼ���֧��

//#include <afxtempl.h>

#include <gdiplus.h> 
using namespace Gdiplus; 

#pragma comment(lib, "gdiplus.lib")

#include "DuiDef.h"
#include "DuiVision.h"
#include "DuiXml.h"

#include "common/XUnZip.h"
#include "common/encoding.h"
//#include "common/registry.h"
#include "common/LogMgr.h"

// �����֪ͨ��Ϣ����
static const UINT WM_CHECK_ITS_ME = RegisterWindowMessage(_T("##DUIVISION_APPLICATION##"));
