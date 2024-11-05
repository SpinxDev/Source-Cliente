
#ifndef _APHYSXMUTEX_H_
#define _APHYSXMUTEX_H_

class APhysXMutex
{
public:

	APhysXMutex()
	{
		
		m_Locked = 0;
		InitializeCriticalSection(&m_Mutex);
	}

	~APhysXMutex()
	{
		DeleteCriticalSection(&m_Mutex);
	}

	bool Lock()
	{
		EnterCriticalSection(&m_Mutex);

		::InterlockedCompareExchange(&m_Locked, 1, 0); //mRep->mLocked = 1;
		m_OwnerThreadId = ::GetCurrentThreadId();

		return true;
	}

	// try to enter the critical section without block...
	bool Trylock()
	{

#if(_WIN32_WINNT >= 0x0400)

		// beware that this function only works well in Win2000 or later and not
		// not supported in Win98...

		return (TryEnterCriticalSection(&m_Mutex) == TRUE);

#else

		if ((::InterlockedCompareExchange(&m_Locked, 1, 0) == 0) || (m_OwnerThreadId == ::GetCurrentThreadId()) ) 
		{
			return Lock();
		}
		else 
		{
			return false;
		}

#endif

	}

	bool Unlock()
	{

		::InterlockedCompareExchange(&m_Locked, 0, 1); //mRep->mLocked = 0;
		LeaveCriticalSection(&m_Mutex);
		return true;
	}

private:

	CRITICAL_SECTION	m_Mutex;

	volatile long		m_Locked;
	DWORD				m_OwnerThreadId;


};



//-----------------------------------------------------------------------------
// grab class (for people who like acquire-init-release-destruct)
// this will help to avoid multi-branch Unlock() call of APhysXMutex...

class APhysXMutexLock
{
	
public:

	APhysXMutexLock(APhysXMutex& mutex): m_Mutex(mutex) 
	{
		m_Mutex.Lock(); 
	}

	~APhysXMutexLock() 
	{
		m_Mutex.Unlock();
	}


private:

	APhysXMutex& m_Mutex;

};


#endif
