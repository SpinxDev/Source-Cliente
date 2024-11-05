/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXALLOCATOR_H_
#define _APHYSXALLOCATOR_H_

#include <crtdbg.h>

#include <map>

class APhysXAllocator : public NxUserAllocator    
{    

#if APHYSX_TRACE_MEMORY_ALLOC

	typedef std::map<void* , APhysXU32>	MemAllocMap;
	// typedef APhysXMap<void* , APhysXU32>	MemAllocMap;
	typedef MemAllocMap::value_type			MemAllocPair;
	typedef MemAllocMap::iterator			MemAllocIter;

#endif


public:     

	void * malloc(size_t size)        
	{          
		void * pAddress = 

#if APHYSX_ANGELICA_ALLOCATOR
		
			a_malloc(size);

#else
			::malloc(size);

#endif

#if APHYSX_TRACE_MEMORY_ALLOC

	m_mutexMap.Lock();
	
	// double check...
	MemAllocIter it = m_mapMemAlloc.find(pAddress);
	if(it != m_mapMemAlloc.end())
	{
		APHYSX_ASSERT(0);
	}

	m_mapMemAlloc.insert(MemAllocPair(pAddress, size));
	m_mutexMap.Unlock();

#endif
		return pAddress;
	} 

	void * mallocDEBUG(size_t size,const char *fileName, int line) 
	{ 
		void * pAddress = 

#if APHYSX_ANGELICA_ALLOCATOR

			a_malloc(size);
#else

	::_malloc_dbg(size,_NORMAL_BLOCK, fileName, line);

#endif
			
#if APHYSX_TRACE_MEMORY_ALLOC

		m_mutexMap.Lock();

		// double check...
		MemAllocIter it = m_mapMemAlloc.find(pAddress);
		if(it != m_mapMemAlloc.end())
		{
			APHYSX_ASSERT(0);
		}

		m_mapMemAlloc.insert(MemAllocPair(pAddress, size));
		m_mutexMap.Unlock();

#endif
		return pAddress;
	}

	void* mallocDEBUG(size_t size, const char* fileName, int line, const char* className, NxMemoryType type)
	{
		return mallocDEBUG(size, fileName, line);	// Just so we don't break user code
	}

	void * realloc(void * memory, size_t size)
	{  

		// firstly, remove the old entry...
#if APHYSX_TRACE_MEMORY_ALLOC

		m_mutexMap.Lock();

		{
			MemAllocIter it = m_mapMemAlloc.find(memory);
			if(it != m_mapMemAlloc.end())
			{
				m_mapMemAlloc.erase(it);
			}
			else
				APHYSX_ASSERT(0);

		}

		m_mutexMap.Unlock();

#endif

		// then, realloc the memory...
		void * pAddress = 
			
#if APHYSX_ANGELICA_ALLOCATOR

			a_realloc(memory, size);
#else
		
			::realloc(memory,size);

#endif

		// at last, insert the new entry...
#if APHYSX_TRACE_MEMORY_ALLOC

		m_mutexMap.Lock();

		{
			// double check...
			MemAllocIter it = m_mapMemAlloc.find(pAddress);
			if(it != m_mapMemAlloc.end())
			{
				APHYSX_ASSERT(0);
			}
		}

		m_mapMemAlloc.insert(MemAllocPair(pAddress, size));
		m_mutexMap.Unlock();

#endif
		return pAddress; 
	}  

	void free(void * memory) 
	{

		// note here: we should remove the mem-alloc entry before free it. this is due to 
		// multi-thread consideration.

		// firstly, remove the entry...
#if APHYSX_TRACE_MEMORY_ALLOC

		m_mutexMap.Lock();

		MemAllocIter it = m_mapMemAlloc.find(memory);
		if(it != m_mapMemAlloc.end())
		{
			m_mapMemAlloc.erase(it);
		}
		else
			APHYSX_ASSERT(0);

		m_mutexMap.Unlock();

#endif


		// then, free the memory...
#if APHYSX_ANGELICA_ALLOCATOR

		a_free(memory);
#else
		
		::free(memory);       

#endif

	}    


	APhysXU32 GetAllocMemSize()
	{
#if APHYSX_TRACE_MEMORY_ALLOC

		APhysXU32 uSize = 0;

		m_mutexMap.Lock();

		for(MemAllocIter it = m_mapMemAlloc.begin(); it != m_mapMemAlloc.end(); ++it)
		{
			uSize += it->second;
		}

		m_mutexMap.Unlock();

		return uSize;
#else
		return 0;
#endif
	}


#if APHYSX_TRACE_MEMORY_ALLOC

protected:
	
	MemAllocMap m_mapMemAlloc;
	APhysXMutex m_mutexMap;

#endif

};


#endif