// DuiVisionDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDuiVisionScriptDemoApp:
// �йش����ʵ�֣������ DuiVisionScriptDemo.cpp
//

class CDuiVisionScriptDemoApp : public CWinApp
{
public:
	CDuiVisionScriptDemoApp();

public:
	virtual BOOL InitInstance();

public:
	HINSTANCE			m_hInterpPluginHandle;		// ��������������̬��ľ��
	IInterp*			m_pInterpObject;			// �������������

	DECLARE_MESSAGE_MAP()
};

extern CDuiVisionScriptDemoApp theApp;