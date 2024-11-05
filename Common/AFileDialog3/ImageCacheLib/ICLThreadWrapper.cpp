// ICLThreadWrapper.cpp: implementation of the ICLThreadWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "ICLStdAfx.h"

UINT ThreadFunc(LPVOID n)
{
	ICLThreadWrapper* pThread = (ICLThreadWrapper*)n;
	ICLThreadPool* pPool = pThread->GetThreadPool();
	ASSERT(pThread && pPool);

	do {

		ICLThreadTask* pTask = pPool->PopTask();
		if (pTask) {
			pTask->Do();
			delete pTask;
		}
		else // once no task, wait for 100 ms
			WaitForSingleObject(pPool->GetTaskEvent(), 100);
	} while (!pThread->IsStopFlag());
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ICLThreadWrapper::ICLThreadWrapper(ICLThreadPool* pPool)
: m_pThread(NULL)
, m_pPool(pPool)
, m_bStopFlag(false)
{
	ASSERT(m_pPool && !m_pThread);
}

ICLThreadWrapper::~ICLThreadWrapper()
{
	if (m_pThread) {
		TerminateThread(m_pThread->m_hThread, 0);
		delete m_pThread;
	}
}

void ICLThreadWrapper::Start()
{
	if (m_pThread)
		return;

	InnerCreateThread();
	Resume();
}

void ICLThreadWrapper::SyncStop()
{
	if (!m_pThread)
		return;

	m_bStopFlag = true;
	WaitForSingleObject(m_pThread->m_hThread, INFINITE);
	delete m_pThread;
	m_bStopFlag = false;
}

void ICLThreadWrapper::Suspend()
{
	if (!m_pThread)
		return;

	m_pThread->SuspendThread();
}

void ICLThreadWrapper::Resume()
{
	if (!m_pThread)
		return;

	m_pThread->ResumeThread();
}

bool ICLThreadWrapper::IsThreadValid() const
{
	return m_pThread != NULL;
}

bool ICLThreadWrapper::InnerCreateThread()
{
	ASSERT(m_pPool && !m_pThread);
	m_pThread = AfxBeginThread(
		ThreadFunc, 
		this,
		THREAD_PRIORITY_NORMAL,
		0,
		CREATE_SUSPENDED);
	m_pThread->m_bAutoDelete = FALSE;
	return true;
}