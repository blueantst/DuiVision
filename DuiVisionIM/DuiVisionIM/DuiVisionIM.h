// DuiVisionDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDuiVisionIMApp:
// �йش����ʵ�֣������ DuiVisionIM.cpp
//

class CDuiVisionIMApp : public CWinApp
{
public:
	CDuiVisionIMApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDuiVisionIMApp theApp;