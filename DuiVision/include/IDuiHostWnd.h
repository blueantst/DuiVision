////////////////////////////////////////////////////////////////////////////
//	File:		IDuiHostWnd.h
//	Version:	1.0.0.0
/*
  Copyright (c) 2014-2016 by blueant <script.net@gmail.net>
  This file is part of the DuiVision library. https://github.com/blueantst/DuiVision
*/
#if !defined(__INTERFACE_IDUIHOSTWND_H_INCLUDED__)
#define __INTERFACE_IDUIHOSTWND_H_INCLUDED__

// 接口对象ID定义
#define IID_IDuiHostWnd _T("IDuiHostWnd")	// 插件宿主窗口功能接口

class CVciMessage;

// 插件宿主窗口功能接口
interface IDuiHostWnd : public IUnknown
{
	//{{VCI_INTERFACE_BEGIN
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

	// DuiVision系统功能
	virtual int __stdcall GetAppID() = 0;	// 获取DuiVision应用ID

	// 窗口操作
	virtual BOOL __stdcall GetWindowBkInfo(int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile) = 0;	// 获取窗口背景信息
	virtual BOOL __stdcall SetWindowBkInfo(int nType, int nIDResource, COLORREF clr, LPCTSTR lpszImgFile) = 0;	// 设置窗口背景信息

	// Tooltip操作
	virtual void __stdcall SetTooltip(int nCtrlID, LPCTSTR lpszTooltip, CRect rect, int nTipWidth) = 0;	// 设置Tooltip
	virtual void __stdcall ClearTooltip() = 0;	// 清除Tooltip
	virtual void __stdcall SetTooltipCtrlID(int nTooltipCtrlID) = 0;	// 设置当前Tooltip控件ID
	virtual int  __stdcall GetTooltipCtrlID() = 0;	// 获取当前Tooltip控件ID

	// 托盘操作

	// 日志

	//}}VCI_INTERFACE_END
};


#endif // !defined(__INTERFACE_IDUIHOSTWND_H_INCLUDED__)
