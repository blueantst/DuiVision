// DuiVisionDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDuiVisionExplorerApp:
// �йش����ʵ�֣������ DuiVisionExplorer.cpp
//

class CDuiVisionExplorerApp : public CWinApp
{
public:
	CDuiVisionExplorerApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDuiVisionExplorerApp theApp;