/*
 * FILE: AObjectPool.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2013/2/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AOBJECTPOOL_H_
#define _AOBJECTPOOL_H_

#include "ABaseDef.h"
#include "AList2.h"
#include "AMemory.h"
#include "ACSWrapper.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AObjectPoolType
//	
///////////////////////////////////////////////////////////////////////////

class AObjectPoolType
{
public:

	AObjectPoolType() : 
	m_bAllocFromPool(false)
	{}

public:

	void SetPoolFlag(bool bFromPool) { m_bAllocFromPool = bFromPool; }
	bool GetPoolFlag() const { return m_bAllocFromPool; }

protected:

	bool	m_bAllocFromPool;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class AObjectPool
//	
///////////////////////////////////////////////////////////////////////////

template <class T>
class AObjectPool
{
public:		//	Types

public:		//	Constructor and Destructor

	AObjectPool(int iGrowBy);
	~AObjectPool();

public:		//	Attributes

public:		//	Operations

	//	Allocate an object
	T* Alloc();
	//	Free an object
	void Free(T* p);

	//	Get pool size
	int GetPoolSize() const { return m_iPoolSize; }
	//	Get allocation times
	DWORD GetAllocCnt() const { return m_dwAllocCnt; }

protected:	//	Attributes

	int		m_iGrowBy;
	int		m_iPoolSize;		//	Total allocated object number
	DWORD	m_dwAllocCnt;		//	Allocation times

	APtrList<T*>		m_FreeList;
	CRITICAL_SECTION	m_csFree;		//	Lock for free list

protected:	//	Operations

	//	Extend object pool
	bool ExtendPool();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

template <class T>
AObjectPool<T>::AObjectPool(int iGrowBy) :
m_iGrowBy(iGrowBy),
m_iPoolSize(0),
m_dwAllocCnt(0),
m_FreeList(iGrowBy)
{
	ASSERT(iGrowBy > 0);
	InitializeCriticalSection(&m_csFree);
}

template <class T>
AObjectPool<T>::~AObjectPool()
{
	ASSERT(m_FreeList.GetCount() == m_iPoolSize);

	ALISTPOSITION pos = m_FreeList.GetHeadPosition();
	while (pos)
	{
		T* p = m_FreeList.GetNext(pos);
		delete p;
	}

	m_FreeList.RemoveAll();

	DeleteCriticalSection(&m_csFree);
}

template <class T>
T* AObjectPool<T>::Alloc()
{
	ACSWrapper csw(&m_csFree);

	if (!m_FreeList.GetCount())
	{
		if (!ExtendPool())
			return NULL;
	}

	m_dwAllocCnt++;

	return m_FreeList.RemoveHead();
}

template <class T>
void AObjectPool<T>::Free(T* p)
{
	AObjectPoolType* pBase = dynamic_cast<AObjectPoolType*>(p);
	ASSERT(pBase);
	if (pBase->GetPoolFlag())
	{
		ACSWrapper csw(&m_csFree);
		m_FreeList.AddTail(p);
	}
	else
	{
		delete p;
	}
}

template <class T>
bool AObjectPool<T>::ExtendPool()
{
	for (int i=0; i < m_iGrowBy; i++)
	{
		T* p = new T;
		if (!p)
		{
			return i != 0;
		}

		AObjectPoolType* pBase = dynamic_cast<AObjectPoolType*>(p);
		ASSERT(pBase);
		pBase->SetPoolFlag(true);

		m_FreeList.AddTail(p);
		m_iPoolSize++;
	}

	return true;
}



#endif	//	_AOBJECTPOOL_H_


