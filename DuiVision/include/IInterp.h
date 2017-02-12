////////////////////////////////////////////////////////////////////////////
//	File:		IInterp.h
//	Version:	2.0.0.0
//	Created:	2000-07-20
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	interface of interperter
////////////////////////////////////////////////////////////////////////////
#if !defined(__VCI_INTERFACE_IInterp_H_INCLUDED__)
#define __VCI_INTERFACE_IInterp_H_INCLUDED__

// 脚本运行模式
enum{
	SCRIPT_RUN_NORMAL,		// 普通运行模式
	SCRIPT_RUN_CANSTOP,		// 可以中止的运行模式
	SCRIPT_RUN_DEBUG_GO,	// 断点调试运行模式
	SCRIPT_RUN_DEBUG_STEP,	// 单步调试运行模式
};

// 脚本运行状态
enum{
	SCRIPT_STATE_IDLE,		// 未运行状态
	SCRIPT_STATE_RUN,		// 正在运行
	SCRIPT_STATE_BREAK,		// 进入断点
	SCRIPT_STATE_STEP,		// 进行单步陷阱
	SCRIPT_STATE_END,		// 正常结束
	SCRIPT_STATE_STOP,		// 被中止
};

// 脚本运行级别
enum{
	SCRIPT_LEVEL_SUPER,		// 超级用户
	SCRIPT_LEVEL_ADMIN,		// 管理员
	SCRIPT_LEVEL_DEVELOP,	// 开发人员
	SCRIPT_LEVEL_OPERATOR,	// 操作人员
	SCRIPT_LEVEL_NONE,		// 最低级别
};

// 断点和单步的模式
enum{
	SCRIPT_BREAK_NORMAL,	// 普通断点
	SCRIPT_BREAK_LINE,		// 行断点
	SCRIPT_BREAK_NAME,		// 命名断点
	SCRIPT_STEP_INTO,		// 进入过程的单步
	SCRIPT_STEP_NEXT,		// 不进入过程的单步
	SCRIPT_STEP_RETURN,		// 由过程返回
};

// 调试事件类型
enum{
	// 以下为调试器回调事件
	IDB_USER_CALL,		// 调用函数
	IDB_USER_LINE,		// 执行一行
	IDB_USER_RETURN,	// 函数返回
	IDB_USER_EXCEPTION,	// 发生异常
	IDB_USER_SETCURRENTLINE,// 设置当前行

	// 以下为平台调试命令事件
	IDB_STOP	= 10,	// 中止
	IDB_NEXT,			// 执行到下一个断点
	IDB_GOTO,			// 执行到某个行断点或命名断点
	IDB_STEPINTO,		// 单步进入函数
	IDB_SETPOUT,		// 从函数返回
	IDB_STEP,			// 单步不进入函数
	IDB_END,			// 通知解释器文件调试结束,可以执行善后处理

	IDB_SETFILE = 20,	// 设置被调试的文件

	IDB_REFRESH_DEBUGWINDOW = 30,	// 刷新调试窗口
	IDB_GET_VAR				= 31,	// 获取变量信息
	IDB_SET_VAR				= 32,	// 设置变量值
};

#define IID_IInterp "IInterp"

interface IInterp : public IUnknown
{
	//{{ 注意：!!!这里是保留的重要函数，不可删除!!!
	// 获取接口的额外信息
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} 注意：!!!这里是保留的重要函数，不可删除!!!

	//{{VCI_INTERFACE_BEGIN
	// 设置平台UI接口指针
	virtual int __stdcall SetIPlatUI(LPVOID lpIPlatUI) = 0;
	// 设置库路径
	virtual int __stdcall SetLibPath(LPCTSTR lpszPath) = 0;
	// 解释器是否在运行
	virtual int __stdcall IsInterpRunning() = 0;
	// 设置脚本文件名
	virtual int __stdcall SetScriptFile(LPCTSTR lpszPathName) = 0;
	// 获取脚本文件名
	virtual CString __stdcall GetScriptFile() = 0;
	// 运行脚本文件
	virtual int __stdcall RunScriptFile(LPCTSTR lpszPathName) = 0;
	// 执行脚本命令
	virtual int __stdcall RunScriptCommand(LPCTSTR lpszTclCmd) = 0;
	// 停止脚本运行
	virtual int __stdcall StopScript() = 0;
	// 获取解释器指针
	virtual LPVOID __stdcall GetInterp() = 0;
	// 获取解释器ID
	virtual int __stdcall GetInterpID() = 0;
	// 设置解释器名
	virtual void __stdcall SetInterpName(LPCTSTR lpszInterptName) = 0;
	// 获取解释器名
	virtual CString __stdcall GetInterpName() = 0;
	// 获取错误行号
	virtual int __stdcall GetErrorLine() = 0;
	// 获取结果信息
	virtual CString __stdcall GetResult() = 0;
	// 设置变量值
	virtual int __stdcall SetVar(LPCTSTR lpszVarName, LPCTSTR lpszVarValue) = 0;
	// 获取变量值
	virtual CString __stdcall GetVar(LPCTSTR lpszVarName) = 0;
	// 注册扩展命令
	virtual int __stdcall RegisterCommand(LPVOID pInterp, LPCTSTR lpszCmd, LPVOID lpFunc) = 0;
	// 设置扩展参数
	virtual int __stdcall SetInterpParam(LPCTSTR lpszParamName, LPCTSTR lpszParamValue) = 0;
	// 获取扩展参数
	virtual CString __stdcall GetInterpParam(LPCTSTR lpszParamName) = 0;
	// 设置运行模式
	virtual int __stdcall SetRunMode(int nRunMode) = 0;
	// 获取运行模式
	virtual int __stdcall GetRunMode() = 0;
	// 设置运行状态
	virtual int __stdcall SetRunState(int nRunState) = 0;
	// 获取运行状态
	virtual int __stdcall GetRunState() = 0;
	// 获取当前堆栈层数
	virtual int __stdcall GetCurStackLevel() = 0;
	// 获取关键字列表
	virtual int __stdcall GetKeyWordList(LPCTSTR lpszType, CStringArray& asKeyWord) = 0;
	// 设置调试事件
	virtual int __stdcall SetDebugEvent(int nEvent, int nParam, LPCTSTR lpszParam) = 0;
	// 设置断点列表
	virtual int __stdcall SetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks) = 0;
	// 获取断点列表
	virtual int __stdcall GetBreakList(LPCTSTR lpszFile, CUIntArray& auBreaks) = 0;
	// 获取是否允许覆盖率分析
	virtual int __stdcall GetEnableProfileAnalyse() = 0;
	// 获取有效代码行数
	virtual int __stdcall GetAvailableLineCount() = 0;
	// 获取代码运行覆盖表
	virtual LPVOID __stdcall GetProfileMap() = 0;
	// 判断指定方法是否被支持
	virtual int __stdcall MethodIsSupport(LPCTSTR lpszMethod) = 0;
	// 初始化调试模式
	virtual int __stdcall InitialDebugMode(CString& strResult) = 0;
	// 更新调试窗口信息
	virtual int __stdcall RefreshDebugWindow(LPCTSTR lpszWindow, BOOL bActive, CString& strResult) = 0;
	// 调用脚本方法
	virtual int __stdcall CallMethod(LPCTSTR lpszMethod, CStringArray& asParam) = 0;
	// 编译
	virtual int __stdcall Compile(LPCTSTR lpszSrcFile, CString& strDestFil, LPCTSTR lpszParams) = 0;
	// 构建
	virtual int __stdcall Build(LPCTSTR lpszSrcFile, CString& strDestFile, LPCTSTR lpszParams) = 0;
	//}}VCI_INTERFACE_END
};

// 定义退出回调函数
#define CALLBACK_EVENT_INTERP_EXIT	0x1000
typedef int (INTERP_EXIT_FUNC)(void* lpInterp);

// 定义调试回调函数
#define CALLBACK_EVENT_INTERP_DEBUG	0x1001
typedef int (INTERP_DEBUG_FUNC)(int nEvent, LPCTSTR lpszScriptFile, int nLine);


#endif // !defined(__VCI_INTERFACE_IInterp_H_INCLUDED__)
