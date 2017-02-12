////////////////////////////////////////////////////////////////////////////
//	File:		IDuiVisionApp.h
//	Version:	1.0.0.0
//	Created:	2017-02-12
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Interface define of DuiVisionApp.
////////////////////////////////////////////////////////////////////////////
#ifndef __IDUIVISIONAPP_H_
#define __IDUIVISIONAPP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 接口使用的类定义
class CVciMessage;
// 定义DuiVision相关类
class CDuiObject;
class CDuiObjectInfo;
class CControlBase;
class CDuiHandler;
class CDlgBase;

interface IDuiVisionApp : public IUnknown
{
	// 平台操作
	virtual CString __stdcall GetAppName() = 0;				// 获取应用程序名
	virtual CString __stdcall GetPlatPath() = 0;			// 获取平台路径
	virtual CString __stdcall GetPlatVersion() = 0;			// 获取平台版本
	virtual int __stdcall GetCurrentLanguage() = 0;			// 获取当前语言
	virtual CString __stdcall GetPlatRegistry() = 0;		// 获取平台注册表根路径
	virtual CString __stdcall GetPlatCopyRight() = 0;		// 获取平台版权字符串
	virtual CString __stdcall GetPlatHomeURL() = 0;			// 获取主页URL
	virtual CString __stdcall GetPlatDownloadURL() = 0;		// 获取下载URL
	virtual int  __stdcall SendMessage(CVciMessage* pIn, CVciMessage* ppOut) = 0;	// 发送消息
	virtual int  __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut) = 0;	// 平台的消息处理
	virtual int  __stdcall SendCommand(int nCmd, WPARAM wParam, LPARAM lParam) = 0;	// 发送平台命令
	virtual BOOL __stdcall SendCommand(int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult) = 0;	// 发送平台命令

	// 日志操作
	virtual void __stdcall LogEvent(int nLevel, CString strInfo) = 0; // 记录日志
	virtual void __stdcall LogEventModule(int nLevel, CString strModule, CString strInfo) = 0; // 记录日志(带模块信息)

	// DuiVision操作
	virtual BOOL __stdcall IsEnableDuiVision() = 0;	// 是否支持DuiVision相关操作
	virtual CString __stdcall GetDuiResourcePath() = 0;	// 获取DuiVision的resource目录
	virtual BOOL __stdcall DuiLoadResourceXml(CString strResFile, CString strStyle) = 0;	// 加载XML资源文件(可以指定加载的风格)
	virtual CString __stdcall DuiGetConfig(CString strName) = 0;	// 获取系统配置信息
	virtual CString __stdcall DuiGetXmlFile(CString strName) = 0;	// 获取XML文件
	virtual CString __stdcall DuiGetSkin(CString strName) = 0;	// 获取Skin
	virtual CString __stdcall DuiGetString(CString strName) = 0;	// 获取字符串值
	virtual void __stdcall DuiSetString(CString strName, CString strValue) = 0;	// 设置字符串值
	virtual void __stdcall ParseDuiString(CString& strString) = 0;	// 解析字符串，替换其中的替换内容
	virtual void __stdcall RegisterDuiControl(CDuiObjectInfo* pDuiObjectInfo) = 0;	// 注册控件
	virtual BOOL __stdcall UnRegisterDuiControl(LPCTSTR lpszName) = 0;	// 卸载控件
	virtual CControlBase* __stdcall DuiCreateControlByName(LPCTSTR lpszName, HWND hWnd, CDuiObject* pParentObject) = 0;	// 根据控件类名创建控件实例
	virtual CControlBase* __stdcall DuiGetControl(CDuiObject* pDuiObject, CString strControlName) = 0;	// 获取子控件
	virtual CControlBase* __stdcall DuiGetDialogControl(CString strDialogName, CString strControlName) = 0;	// 获取对话框子控件
	virtual BOOL __stdcall RegisterDuiHandler(CDuiObject* pDuiObject, CDuiHandler* pDuiHandler, CString strControlName, BOOL bInit) = 0;	// 给指定的DUI对象注册事件处理对象
	virtual void __stdcall AddDuiHandler(CDuiHandler* pDuiHandler) = 0;	// 添加DUI事件处理对象
	virtual void __stdcall RemoveDuiHandler(CDuiHandler* pDuiHandler) = 0;	// 删除DUI事件处理对象
	virtual CDlgBase* __stdcall CreateDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule, BOOL bAdd) = 0;	// 创建DUI对话框
	virtual int __stdcall ShowDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule) = 0;	// 显示DUI对话框
	virtual int __stdcall DuiMessageBox(CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption, UINT uType, int nWidth, int nHeight) = 0;	// 显示DUI MessageBox
	virtual void __stdcall AddDuiDialog(CDlgBase* pDuiDialog) = 0;	// 添加DUI对话框
	virtual void __stdcall RemoveDuiDialog(CDlgBase* pDuiDialog) = 0;	// 删除DUI对话框
	virtual CDlgBase* __stdcall GetDuiDialog(CString strName) = 0;	// 获取DUI对话框
	virtual void __stdcall AddDuiNotifyMsgTask(CString strMsg, UINT uNotifyType, CString strCaption, int nDelayTime, int nWidth, int nHeight) = 0;	// 添加提示窗口任务
	virtual UINT __stdcall AddDuiTimer(UINT uTimerResolution, CString strTimerName) = 0;	// 添加DUI定时器
	virtual void __stdcall RemoveDuiTimer(CString strTimerName) = 0;	// 删除DUI定时器
};


#endif // !defined(__IDUIVISIONAPP_H_)