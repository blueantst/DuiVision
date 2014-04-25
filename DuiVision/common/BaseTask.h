#ifndef __DUIVISION_TASK_H_
#define __DUIVISION_TASK_H_

namespace DuiVision
{

//
// 任务基类
//
class CTaskMgr;
class IBaseTask
{
public:
	//
	// 优先级
	//
	enum TPriority
	{
		TP_Highest		= 0,
		TP_Above_Normal	= (1<<8),
		TP_Normal		= (1<<16),
		TP_Below_Normal	= (1<<24),
		TP_Lowest		= (1<<30),
		TP_Invalid		= (TP_Lowest + 1),
	};

	//
	// 事件
	//
	enum TEvent
	{
		TE_Unknown = 0,
		TE_Canceled,
		TE_Removed,
		TE_Completed,
	};

public:
	IBaseTask(LONG type);
	virtual ~IBaseTask() {}

public:
	//
	// 任务信息
	//
	void SetId(LONG id);
	LONG GetId() const;

	// 类型必须大于0
	void SetType(LONG type);
	LONG GetType() const;

	void SetPriority(ULONG priority);
	ULONG GetPriority() const;

	void SetUITask(BOOL bUITask);
	BOOL IsUITask() const;

	//
	// 引用计数
	//
	LONG AddRef();
	LONG Release();

	//
	// 任务操作
	//
	//@Return
	// 返回TRUE，则继续处理，否则放弃该任务
	//
	virtual BOOL TaskProcess(CTaskMgr *pMgr);
	virtual void TaskNotify(CTaskMgr *pMgr, TEvent event);

private:
	IBaseTask(const IBaseTask&);
	IBaseTask& operator=(const IBaseTask&);

protected:
	LONG			m_id;		// 任务对象ID
	LONG			m_type;		// 任务类型
	ULONG			m_priority;	// 优先级
	BOOL			m_bUITask;	// 是否需要UI线程处理的任务
	volatile LONG	m_refCnt;	// 引用计数
};


//
// 任务适配器
//
template<class Type>
class CTaskAdapter : public DuiVision::IBaseTask
{
public:
	typedef BOOL (Type::*PFProcess)(DuiVision::CTaskMgr*);
	typedef void (Type::*PFNotify)(DuiVision::CTaskMgr*, TEvent);

public:
	CTaskAdapter(LONG type, Type *pInst, PFProcess pfProcess, PFNotify pfNotify = NULL)
		: IBaseTask(type), _pInst(pInst), _pfProcess(pfProcess), _pfNotify(pfNotify) {}

	virtual BOOL TaskProcess(DuiVision::CTaskMgr *pMgr)
	{
		if(_pInst != NULL && _pfProcess != NULL)
			return (_pInst->*_pfProcess)(pMgr);
		return TRUE;
	}

	virtual void TaskNotify(DuiVision::CTaskMgr *pMgr, TEvent event)
	{
		if(_pInst != NULL && _pfNotify != NULL)
			(_pInst->*_pfNotify)(pMgr, event);
	}

private:
	Type *_pInst;
	PFProcess _pfProcess;
	PFNotify _pfNotify;
};

//
// 任务适配器助手函数
//
//@Usage
// IBase *pTask = MakeTask(type, this, &ClassName::MemberFunc);
//
template<class Type> inline 
CTaskAdapter<Type>* MakeTask(LONG type, Type *pInst, 
							 BOOL (Type::*pfProcess)(DuiVision::CTaskMgr*), 
							 void (Type::*pfNotify)(DuiVision::CTaskMgr*, DuiVision::IBaseTask::TEvent) = NULL)
{
	return new CTaskAdapter<Type>(type, pInst, pfProcess, pfNotify);
}

//////////////////////////////////////////////////////////////////////////
inline IBaseTask::IBaseTask(LONG type)
{
	m_id		= 0L;
	m_type		= type;
	m_priority	= TP_Normal;
	m_bUITask	= false;
	m_refCnt	= 0L;	// 任务对象初始时候引用计数设置为0
}

inline void IBaseTask::SetId(LONG id) 
{ m_id = id; }
inline LONG IBaseTask::GetId() const 
{ return m_id; }

inline void IBaseTask::SetType(LONG type) 
{ m_type = type; }
inline LONG IBaseTask::GetType() const 
{ return m_type; }

inline void IBaseTask::SetPriority(ULONG priority) 
{ m_priority = priority; }
inline ULONG IBaseTask::GetPriority() const 
{ return m_priority; }

inline void IBaseTask::SetUITask(BOOL bUITask) 
{ m_bUITask = bUITask; }
inline BOOL IBaseTask::IsUITask() const 
{ return m_bUITask; }

inline LONG IBaseTask::AddRef() 
{ return InterlockedIncrement(&m_refCnt); }
inline LONG IBaseTask::Release() 
{ 
	if(InterlockedDecrement(&m_refCnt) <= 0)
	{ 
		delete this; return 0L; 
	}
	else 
	{ 
		return m_refCnt;
	}
}

inline BOOL IBaseTask::TaskProcess(CTaskMgr *pMgr) { return FALSE; }
inline void IBaseTask::TaskNotify(CTaskMgr *pMgr, IBaseTask::TEvent event) {}

}

#endif	// __DUIVISION_TASK_H_