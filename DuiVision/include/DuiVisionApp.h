////////////////////////////////////////////////////////////////////////////
//	File:		DuiVisionApp.h
//	Version:	1.0.0.0
//	Created:	2017-02-12
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	header of Platform DuiVisionApp interface object.
////////////////////////////////////////////////////////////////////////////

#if !defined(__DUIVISIONAPP_H_)
#define __DUIVISIONAPP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DuiVisionApp.h : header file
//

#include "vcicomm.h"
#include "IDuiVisionApp.h"

/////////////////////////////////////////////////////////////////////////////
// CDuiVisionApp command target

class CDuiVisionApp : public CVisualComponent
{
public:
	CDuiVisionApp();           // protected constructor used by dynamic creation
	virtual ~CDuiVisionApp();

// Attributes
public:

// Interface define
public:
	BEGIN_INTERFACE_PART(VciControl, IVciControl)
		CONTROL_INTERFACE_DEFINE
	END_INTERFACE_PART(VciControl)
	EXPORT_CONTROL_INTERFACE_PART(VciControl)

    BEGIN_INTERFACE_PART(DuiVisionApp, IDuiVisionApp)

		// 平台操作
		STDMETHOD_( CString , GetAppName) ();				// 获取应用程序名
		STDMETHOD_( CString , GetPlatPath) ();				// 获取平台路径
		STDMETHOD_( CString , GetPlatVersion) ();			// 获取平台版本
		STDMETHOD_( int , GetCurrentLanguage) ();			// 获取当前语言
		STDMETHOD_( CString , GetPlatRegistry) ();			// 获取平台注册表根路径
		STDMETHOD_( CString , GetPlatCopyRight) ();			// 获取平台版权字符串
		STDMETHOD_( CString , GetPlatHomeURL) ();			// 获取主页URL
		STDMETHOD_( CString , GetPlatDownloadURL) ();		// 获取下载URL
		STDMETHOD_( int  , SendMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// 发送消息
		STDMETHOD_( int  , ProcessMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// 平台的消息处理
		STDMETHOD_( int  , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam);	// 发送平台命令
		STDMETHOD_( BOOL , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult);	// 发送平台命令

		// 日志操作
		STDMETHOD_( void , LogEvent) (int nLevel, CString strInfo);	// 记录日志
		STDMETHOD_( void , LogEventModule) (int nLevel, CString strModule, CString strInfo);	// 记录日志(带模块信息)

		// DuiVision操作
		STDMETHOD_( BOOL , IsEnableDuiVision) ();	// 是否支持DuiVision相关操作
		STDMETHOD_( CString , GetDuiResourcePath) () ;	// 获取DuiVision的resource目录
		STDMETHOD_( BOOL , DuiLoadResourceXml) (CString strResFile, CString strStyle);	// 加载XML资源文件(可以指定加载的风格)
		STDMETHOD_( CString , DuiGetConfig) (CString strName) ;	// 获取系统配置信息
		STDMETHOD_( CString , DuiGetXmlFile) (CString strName);	// 获取XML文件
		STDMETHOD_( CString , DuiGetSkin) (CString strName);	// 获取Skin
		STDMETHOD_( CString , DuiGetString) (CString strName);	// 获取字符串值
		STDMETHOD_( void , DuiSetString) (CString strName, CString strValue);	// 设置字符串值
		STDMETHOD_( void , ParseDuiString) (CString& strString);	// 解析字符串，替换其中的替换内容
		STDMETHOD_( void  , RegisterDuiControl) (CDuiObjectInfo* pDuiObjectInfo);	// 注册控件
		STDMETHOD_( BOOL , UnRegisterDuiControl) (LPCTSTR lpszName);	// 卸载控件
		STDMETHOD_( CControlBase*  , DuiCreateControlByName) (LPCTSTR lpszName, HWND hWnd, CDuiObject* pParentObject);	// 根据控件类名创建控件实例
		STDMETHOD_( CControlBase* , DuiGetControl) (CDuiObject* pDuiObject, CString strControlName);	// 获取子控件
		STDMETHOD_( CControlBase* , DuiGetDialogControl) (CString strDialogName, CString strControlName);
		STDMETHOD_( BOOL , RegisterDuiHandler) (CDuiObject* pDuiObject, CDuiHandler* pDuiHandler, CString strControlName, BOOL bInit);	// 给指定的DUI对象注册事件处理对象
		STDMETHOD_( void , AddDuiHandler) (CDuiHandler* pDuiHandler);	// 添加DUI事件处理对象
		STDMETHOD_( void , RemoveDuiHandler) (CDuiHandler* pDuiHandler);	// 删除DUI事件处理对象
		STDMETHOD_( CDlgBase* , CreateDuiDialog) (LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule, BOOL bAdd);	// 创建DUI对话框
		STDMETHOD_( int , ShowDuiDialog) (LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule);	// 显示DUI对话框
		STDMETHOD_( int , DuiMessageBox) (CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption, UINT uType, int nWidth, int nHeight);	// 显示DUI MessageBox
		STDMETHOD_( void , AddDuiDialog) (CDlgBase* pDuiDialog);	// 添加DUI对话框
		STDMETHOD_( void , RemoveDuiDialog) (CDlgBase* pDuiDialog);	// 删除DUI对话框
		STDMETHOD_( CDlgBase* , GetDuiDialog) (CString strName);	// 获取DUI对话框
		STDMETHOD_( void , AddDuiNotifyMsgTask) (CString strMsg, UINT uNotifyType, CString strCaption, int nDelayTime, int nWidth, int nHeight);	// 添加提示窗口任务
		STDMETHOD_( UINT , AddDuiTimer) (UINT uTimerResolution, CString strTimerName);	//  添加DUI定时器
		STDMETHOD_( void , RemoveDuiTimer) (CString strTimerName);	//  删除DUI定时器

	END_INTERFACE_PART(DuiVisionApp)
	EXPORT_INTERFACE_PART(DuiVisionApp)

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__DUIVISIONAPP_H_)
