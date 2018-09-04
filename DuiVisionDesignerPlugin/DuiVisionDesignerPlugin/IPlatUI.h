////////////////////////////////////////////////////////////////////////////
//	File:		IPlatUI.h
//	Version:	1.0.0.0
//	Created:	2005-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Interface define of Platform UI.
////////////////////////////////////////////////////////////////////////////
#ifndef __IPLATUI_H_
#define __IPLATUI_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVciMessage;

class CTTreeItemArray_;

interface IPlatUI : public IUnknown
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

	// 输出窗口操作
	virtual int  __stdcall AddOutput(CString strName) = 0;	// 添加输出窗口
	virtual BOOL __stdcall DelOutput(int nOT) = 0;			// 删除输出窗口
	virtual BOOL __stdcall ActiveOutput(int nOT) = 0;		// 激活输出窗口
	virtual BOOL __stdcall Output(int nOT, LPCTSTR lpszOut) = 0;// 输出信息
	virtual BOOL __stdcall OutputFormat(int nOT, int nFmt, LPCTSTR lpszOut) = 0;// 输出格式信息
	virtual BOOL __stdcall OutputVerify(int nOT, LPCTSTR lpszOut, LPCTSTR lpszVerify) = 0;// 输出信息并和校验字符串比较,不一致的字符显示为红色
	virtual BOOL __stdcall OutputSet(int nOT, LPCTSTR lpszSet) = 0;// 设置输出窗口
	virtual CString __stdcall OutputGet(int nOT, LPCTSTR lpszParam) = 0;// 获取输出窗口信息
	virtual BOOL __stdcall OutputClear(int nOT) = 0;		// 清除输出窗口
	virtual BOOL __stdcall OutputAddTreeItems(int nOT, CTTreeItemArray_* pItem) = 0;	// 添加树输出窗口内容

	// 用户菜单操作
	virtual void* __stdcall GetCommandBars() = 0;			// 获取CommandBars
	virtual BOOL __stdcall AddExtMenu(CMenu*& pMenu) = 0;		// 添加扩展菜单
	virtual BOOL __stdcall DelExtMenu(CMenu*& pMenu) = 0;		// 删除扩展菜单
	virtual BOOL __stdcall AddSubMenu(CString strMainMenu, int nIndex, CMenu*& pMenu, LPVOID pProc, LPVOID pUpdateProc) = 0;	// 添加扩展子菜单
	virtual BOOL __stdcall DelSubMenu(CString strMainMenu, int nIndex, CMenu*& pMenu) = 0;	// 删除扩展子菜单
	virtual BOOL __stdcall TrackPopupMenu(CMenu* pMenu, UINT nFlags, int x, int y, CWnd* pWnd) = 0;		// 弹出菜单

	// 状态行操作
	virtual BOOL __stdcall AddStatusPane(int& nID, int nWidth) = 0;	// 添加状态行Pane
	virtual BOOL __stdcall DelStatusPane(int nID) = 0;				// 删除状态行Pane
	virtual BOOL __stdcall SetPaneText(int nID, CString strPane) = 0;// 设置Pane文字
	virtual BOOL __stdcall SetPaneWidth(int nID, int nWidth) = 0;	// 设置Pane宽度
	virtual BOOL __stdcall SetPaneImage(int nID, int nGroup, int nImage) = 0;// 设置Pane图像

	// 窗口操作
	virtual CDocument* __stdcall GetActiveDocument() = 0;	// 获取当前激活的文档对象
	virtual BOOL __stdcall CreateDockingPane(CString strOwmId, CString strTitle, int nID, int nDirection, int nAttachType, HICON hIcon, int nAttachPane = 0) = 0;	// 创建DockingPane
	virtual BOOL __stdcall CreateDockingPane(CString strOwmId, int nID) = 0;	// 创建DockingPane
	virtual CWnd* __stdcall GetDockingPaneWnd(int nPaneID) = 0;	// 获取DockingPane包含的窗口对象
	virtual int __stdcall GetDockingPaneIdByName(CString strName) = 0;	// 获取DockingPane ID(根据名字)
	virtual BOOL __stdcall ActiveDockingPane(int nPaneID, BOOL bShow = TRUE) = 0;	// 激活DockingPane
	virtual BOOL __stdcall CloseDockingPane(int nPaneID) = 0;	// 关闭DockingPane窗口

	// 日志操作
	virtual BOOL __stdcall AddSysLogInfo(int nType, CString strFrom, CString strDesc) = 0; // 增加日志信息

	// OWM操作
	virtual BOOL __stdcall IsOwmExist(CString strOwmId) = 0;	// 查询模块是否存在
	virtual BOOL __stdcall SendOwmCmd(CString strOwmId, int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult) = 0;	// 发送OWM命令
	virtual BOOL __stdcall LoadOwm(CString strOwmName, int& nID, BOOL bLoad) = 0;	// 加载扩展模块

	// 文档操作
	virtual CDocument* __stdcall OpenDocumentFile(LPCTSTR lpszFileName, LPCTSTR lpszOwmId) = 0;	// 打开文档

	// 属性窗口操作
	virtual CWnd* __stdcall GetPropertyGridWnd() = 0;	// 获取属性树指针
	virtual BOOL __stdcall EnableUpdatePropertyGridContent() = 0;	// 判断是否可以更新属性树
	virtual void __stdcall UpdatePropertyGridContent() = 0;	// 刷新属性窗口内容
	virtual void __stdcall BeginUpdatePropertyGridContent() = 0;	// 开始属性窗口刷新
	virtual void __stdcall EndUpdatePropertyGridContent() = 0;	// 结束属性窗口刷新

	// VCI组件调用接口
	virtual void* __stdcall CreateVciObject(CString strVciId, CString strInstName) = 0;	// 创建一个组件对象
	virtual BOOL __stdcall ReleaseVciObject(void* pObject, BOOL bForceDelete) = 0;	// 关闭一个组件对象
	virtual BOOL __stdcall ReleaseObjectByInstanceName(CString strInstName) = 0;	// 关闭一个组件对象(指定实例名)
	virtual void* __stdcall GetObjectByInstanceName(CString strInstName) = 0;	// 获取一个组件对象(根据实例名)
	virtual void* __stdcall GetObjectControlPtrByInstancePtr(LPVOID	pVciObj) = 0;	// 获取一个组件对象控制接口(根据组件实例接口指针)
	virtual int __stdcall GetInstalledVci(CStringArray& asVciId) = 0;	// 获取所有安装的组件ID
	virtual CString __stdcall GetVciName(CString strId) = 0;	// 获取一个组件名(根据组件ID)
	virtual CString __stdcall GetVciPath(CString strId) = 0;	// 获取一个组件路径(根据组件ID)
	virtual CString __stdcall GetVciVersion(CString strId) = 0;	// 获取一个组件版本(根据组件ID)
	virtual CString __stdcall GetVciProvider(CString strId) = 0;	// 获取一个组件开发商信息(根据组件ID)
	virtual CString __stdcall GetVciType(CString strId) = 0;	// 获取一个组件类型(根据组件ID)
	virtual CString __stdcall GetVciClass(CString strId) = 0;	// 获取一个组件类接口ID(根据组件ID)
	virtual CString __stdcall GetVciInterfaceFile(CString strId) = 0;	// 获取一个组件接口定义文件(根据组件ID)
	virtual CTime __stdcall GetVciBuildTime(CString strId) = 0;	// 获取一个组件编译时间(根据组件ID)
	virtual CString __stdcall GetVciGUID(CString strId) = 0;	// 获取一个组件GUID(根据组件ID)
	virtual CString __stdcall GetVciAboutPage(CString strId) = 0;	// 获取一个组件关于文件位置(根据组件ID)
	virtual int __stdcall GetVciDocExecuteModule(CString strId, CString strDocId, CString strExecuteType, CStringArray& arExecuteModule) = 0;	// 获取可以执行一个文档的扩展模块ID列表(根据组件Id和文档Id)
	virtual CString __stdcall GetVciDocIcon(CString strId, CString strDocId, CString strIconType) = 0;	// 获取文档的图标(根据组件Id,文档Id,图标类型)
	virtual void __stdcall SetVciDocIcon(CString strId, CString strDocId, CString strIconType, HWND hWnd) = 0;	// 设置文档的图标(根据组件Id,文档Id,图标类型,窗口句柄)
	virtual void __stdcall SetDocumentTitle(CString strTitle, int nModifyType, HWND hWnd) = 0;	// 设置文档的标题(根据窗口句柄)
	virtual int __stdcall GetVciObjects(CString strId, CPtrArray& auVciObj) = 0;	// 获取一个组件的所有实例对象指针
	virtual CString __stdcall GetInstanceNameByObject(CString strId, LPVOID pVciObj) = 0;	// 获取一个组件实例对象的实例名
	virtual int __stdcall GetInstanceRefCount(CString strId, LPVOID pVciObj) = 0;	// 获取一个组件实例对象的引用计数
	virtual int __stdcall EnumVciParameters(CString strId, CStringArray& asParamName) = 0;	// 枚举一个VCI组件的扩展参数
	virtual BOOL __stdcall GetVciParameter(CString strId, CString strName, CString& strValue, CString& strDefaultValue) = 0;	// 获取一个VCI组件的参数
	virtual BOOL __stdcall SetVciParameter(CString strId, CString strName, CString strValue) = 0;	// 设置一个VCI组件的参数

	// 定时器操作
	virtual void* __stdcall AddTimerTask(CString strTaskName, CString strInterpId, CString strInterpName, CString strCommand, int nSeconds, BOOL bStart) = 0;	// 添加定时器任务
	virtual void* __stdcall AddTimerTask(CString strTaskName, LPVOID lpFunction, int nSeconds, BOOL bStart) = 0;	// 添加定时器任务
	virtual void* __stdcall GetTimerTask(CString strTaskName) = 0;	// 根据任务名获取定时任务指针
	virtual BOOL __stdcall DeleteTimerTask(LPVOID lpTimerTask) = 0;	// 删除定时任务(根据任务指针)
	virtual BOOL __stdcall DeleteTimerTask(CString strTaskName) = 0;// 删除定时任务(根据任务名)
	virtual void __stdcall StartTimerTask(LPVOID lpTimerTask) = 0;	// 启动定时任务
	virtual void __stdcall SetTimerStartTime(LPVOID lpTimerTask, int tStart) = 0;	// 设置定时任务开始时间
	virtual void __stdcall SetTimerEndTime(LPVOID lpTimerTask, int tEnd) = 0;	// 设置定时任务开始时间
	virtual void __stdcall SetTimerTrigger(LPVOID lpTimerTask, CString strTrigger) = 0;	// 设置定时任务触发条件脚本
	virtual void __stdcall SetTimerExecCount(LPVOID lpTimerTask, int nCount) = 0;	// 设置定时任务执行次数

	// 工具操作
	virtual int __stdcall GetInstalledTool(CStringArray& asToolId) = 0;	// 获取所有安装的工具ID
	virtual CString __stdcall GetToolName(CString strId) = 0;	// 获取一个工具名(根据工具ID)
	virtual CString __stdcall GetToolPath(CString strId) = 0;	// 获取一个工具路径(根据工具ID)
	virtual CString __stdcall GetToolVersion(CString strId) = 0;	// 获取一个工具版本(根据工具ID)
	virtual CString __stdcall GetToolProvider(CString strId) = 0;	// 获取一个工具开发商信息(根据工具ID)
	virtual BOOL __stdcall RunTool(CString strId) = 0;	// 运行工具(根据工具ID)

	// 页面风格管理接口
};

// 创建DockingPane的回调函数


#endif // !defined(__IPLATUI_H_)