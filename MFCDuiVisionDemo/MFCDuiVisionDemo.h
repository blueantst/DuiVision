// MFCDuiVisionDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCDuiVisionDemoApp:
// �йش����ʵ�֣������ MFCDuiVisionDemo.cpp
//

class CMFCDuiVisionDemoApp : public CWinApp
{
public:
	CMFCDuiVisionDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCDuiVisionDemoApp theApp;