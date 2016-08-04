// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

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


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlbase.h>


#include "DuiVision.h"

#include "exdispid.h"

// ע�����
#define REG_CONFIG_SUBKEY		_T("Software\\DuiVisionExplorer\\DuiVision\\Config")
#define REG_CONFIG_BKTYPE		_T("BkType")	// ��������(PIC/COLOR)
#define REG_CONFIG_BKPIC_RES	_T("BkPicRes")	// ����ͼƬ��ԴID
#define REG_CONFIG_BKPIC_FILE	_T("BkPicFile")	// ����ͼƬ�ļ���
#define REG_CONFIG_BKCOLOR		_T("BkColor")	// ������ɫ

#define REG_EXPLORER_SUBKEY		_T("Software\\DuiVisionExplorer\\Explorer")
#define REG_EXPLORER_WEBTYPE	_T("WebType")	// ������ں�����(ie|wke)
#define REG_EXPLORER_STARTOPEN	_T("StartOpen")	// ����ʱ�򿪵�ҳ��(last|home|none)
#define REG_EXPLORER_SEARCHTYPE	_T("SearchType")// Ĭ��������������(haosou|baidu|google)
#define REG_EXPLORER_SUBKEY_URL	_T("Software\\DuiVisionExplorer\\Explorer\\URL")
