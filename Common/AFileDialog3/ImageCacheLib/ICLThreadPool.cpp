// ICLThreadPool.cpp: implementation of the ICLThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#include "ICLStdAfx.h"

const size_t ICLThreadPool::s_MAX_POOL_SIZE = 5;
const TCHAR * ICLThreadPool::s_PoolTaskEventName = _T("POOL_TASK_EVENT");
//CCriticalSection ICLThreadPool::s_csTaskQueue;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ICLThreadPool::ICLThreadPool(size_t nPoolSize)
: m_eTaskEvent(FALSE, FALSE, s_PoolTaskEventName)
{
	if (nPoolSize > s_MAX_POOL_SIZE)
		nPoolSize = s_MAX_POOL_SIZE;

	for (size_t nIdx = 0; nIdx < nPoolSize; ++nIdx)
	{
		ICLThreadWrapper* pICLThread = new ICLThreadWrapper(this);
		pICLThread->Start();
		m_aThreadPool.push_back(pICLThread);
	}
}

ICLThreadPool::~ICLThreadPool()
{
	size_t nIdx;
	for (nIdx = 0; nIdx < m_aThreadPool.size(); ++nIdx)
		delete m_aThreadPool[nIdx];

	while (m_aTaskPool.size())
	{
		delete m_aTaskPool.front();
		m_aTaskPool.pop();
	}
}

void ICLThreadPool::PushTask(ICLThreadTask* pTask)
{
	if (!pTask) return;
	lockerd_wrapper<CCriticalSection> _cs(s_csTaskQueue);
	m_aTaskPool.push(pTask);
	m_eTaskEvent.SetEvent();
}

ICLThreadTask* ICLThreadPool::PopTask()
{
	lockerd_wrapper<CCriticalSection> _cs(s_csTaskQueue);
	if (!m_aTaskPool.size())
		return NULL;

	ICLThreadTask* pTask = m_aTaskPool.front();
	m_aTaskPool.pop();
	return pTask;
}