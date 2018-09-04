// DuiVisionDemo.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CDuiVisionScriptDemoApp:
// 有关此类的实现，请参阅 DuiVisionScriptDemo.cpp
//

class CDuiVisionScriptDemoApp : public CWinApp
{
public:
	CDuiVisionScriptDemoApp();

public:
	virtual BOOL InitInstance();

public:
	HINSTANCE			m_hInterpPluginHandle;		// 保存解释器插件动态库的句柄
	IInterp*			m_pInterpObject;			// 解释器插件对象

	DECLARE_MESSAGE_MAP()
};

extern CDuiVisionScriptDemoApp theApp;