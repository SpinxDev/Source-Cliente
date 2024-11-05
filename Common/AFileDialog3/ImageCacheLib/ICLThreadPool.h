// ICLThreadPool.h: interface for the ICLThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICLTHREADPOOL_H__CA21919F_B7BF_4B7E_89FA_63E8EC71009F__INCLUDED_)
#define AFX_ICLTHREADPOOL_H__CA21919F_B7BF_4B7E_89FA_63E8EC71009F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <queue>
#include <afxmt.h>

class CSyncObject;
class ICLThreadTask;
class ICLThreadWrapper;

class ICLThreadPool  
{
public:
	ICLThreadPool(size_t nPoolSize = 3);
	virtual ~ICLThreadPool();

public:

	void AsyncStopAllThread();
	void PushTask(ICLThreadTask* pTask);
	ICLThreadTask* PopTask();
	CSyncObject& GetTaskEvent() { return m_eTaskEvent; }

private:
	static const size_t s_MAX_POOL_SIZE;
	static const TCHAR * s_PoolTaskEventName;
	CCriticalSection s_csTaskQueue;

	std::vector<ICLThreadWrapper*> m_aThreadPool;
	typedef std::queue<ICLThreadTask*> TaskQueue;
	TaskQueue m_aTaskPool;
	CEvent m_eTaskEvent;
};

#endif // !defined(AFX_ICLTHREADPOOL_H__CA21919F_B7BF_4B7E_89FA_63E8EC71009F__INCLUDED_)
