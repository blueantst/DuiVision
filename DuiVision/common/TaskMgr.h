#ifndef __DUIVISION_TASK_MGR_H_
#define __DUIVISION_TASK_MGR_H_

#include <list>
#include "BaseTask.h"

namespace DuiVision
{

//
// 任务管理
//
class CTaskMgr
{
public:
	typedef std::list<IBaseTask*>		TaskQueue;
	typedef TaskQueue::iterator			TaskIter;
	typedef TaskQueue::const_iterator	TaskCIter;
	
public:
	CTaskMgr();
	~CTaskMgr();

	//
	// 以指定的线程优先级启动任务处理器
	//
	BOOL Startup(int priority = THREAD_PRIORITY_NORMAL);
	BOOL IsStarted() const;
	BOOL IsExited() const;
	BOOL Shutdown(DWORD millisec = INFINITE);

	//
	// 开始执行任务
	//
	BOOL StartTask();

	//
	// 锁定/解锁任务队列
	//
	//@Remark
	// 所有的Naive开头的函数均需要先锁定任务队列
	//
	void TryLockQueue();
	void LockTaskQueue();
	void UnlockTaskQueue();

	void ClearTaskQueue();
	void ClearKindTask(ULONG type);
	void NaiveClearKindTask(ULONG type);

	BOOL IsEmpty();
	ULONG GetTaskCnt();

	// 返回任务队列，直接操作(慎用)
	TaskQueue& NaiveGetTaskQueue();

	//
	// 将当前任务按优先级顺序插入到任务队列中
	//
	//@Remark
	// 需要调用StartTask以启动任务
	//
	LONG AddTask(IBaseTask *pTask);
	LONG NaiveAddTask(IBaseTask *pTask);
	void NaiveInsertTask(IBaseTask *pTask);

private:
	void Init();
	IBaseTask* GetTask();
	void NaiveClearTaskQueue(TaskQueue &taskQueue);

	unsigned Run();
	static unsigned __stdcall TaskThread(LPVOID lpVoid);

private:
	BOOL				m_exited;
	LONG				m_taskId;		// 任务递增Id
	HANDLE				m_hThread;		// 任务线程句柄
	HANDLE				m_taskEvent;	// 任务启动通知事件
	TaskQueue			m_taskQueue;	// 任务队列
	CRITICAL_SECTION	m_taskLock;		// 任务队列锁
};
//////////////////////////////////////////////////////////////////////////
inline CTaskMgr::CTaskMgr() 
{ 
	Init(); 
	::InitializeCriticalSection(&m_taskLock);
	m_taskEvent = ::CreateEventW(NULL, FALSE, FALSE, NULL);
}
inline CTaskMgr::~CTaskMgr()
{ 
	Shutdown(); 
	::DeleteCriticalSection(&m_taskLock);
	::CloseHandle(m_taskEvent);
}

inline BOOL CTaskMgr::IsStarted() const 
{ return (m_hThread != (HANDLE)-1); }

inline BOOL CTaskMgr::StartTask()
{
	if(!IsStarted())
		return FALSE;

	::SetEvent(m_taskEvent);
	return TRUE;
}

inline void CTaskMgr::TryLockQueue() 
{ ::TryEnterCriticalSection(&m_taskLock); }
inline void CTaskMgr::LockTaskQueue() 
{ ::EnterCriticalSection(&m_taskLock); }
inline void CTaskMgr::UnlockTaskQueue() 
{ ::LeaveCriticalSection(&m_taskLock); }

inline CTaskMgr::TaskQueue& CTaskMgr::NaiveGetTaskQueue() 
{ return m_taskQueue; }

inline void CTaskMgr::Init()
{
	m_exited = FALSE;
	m_taskId = 1;
	m_hThread = (HANDLE)-1;
}

inline LONG CTaskMgr::AddTask(IBaseTask *pTask)
{	
	ULONG id;
	LockTaskQueue();
	id = NaiveAddTask(pTask);
	UnlockTaskQueue();
	return id;
}

inline ULONG CTaskMgr::GetTaskCnt()
{
	ULONG cnt;
	LockTaskQueue();
	cnt = static_cast<ULONG>(m_taskQueue.size());
	UnlockTaskQueue();
	return cnt;
}

inline BOOL CTaskMgr::IsEmpty()
{ return (GetTaskCnt() == 0);}

inline BOOL CTaskMgr::IsExited() const 
{ return m_exited; }

};

#endif	// __DUIVISION_TASK_MGR_H_