/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXSTRINGMANAGER_H_
#define _APHYSXSTRINGMANAGER_H_


class APHYSX_DLL_EXPORT APhysXStringManager
{

//----------------------------------------------------------
// singleton object

	APHYSX_DECLARE_SINGLETON(APhysXStringManager)

	APhysXStringManager()
	{
		//m_Root = NULL;
		//m_StringCount = 0;
		//m_StringMem = 0;
	}

//----------------------------------------------------------

public:

	typedef APhysXMap<AString, char*> StringMap;

	~APhysXStringManager()
	{
		Release();
	}

	const char* GetString(const char* str);

	void Release();

protected:

	//struct StringList
	//{
	//	StringList *mNextString;
	//	const char *mString;
	//};

	//StringList* m_Root;
	//int m_StringCount;
	//int m_StringMem;

	StringMap m_StringMap;

	// for multi-thread support...
	APhysXMutex m_MutexStringList;
};

#define gGlobalStringManager (APhysXStringManager::GetPtrInstance())

#endif