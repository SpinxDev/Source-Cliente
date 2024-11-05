/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXOBJECTDESCMANAGER_H_
#define _APHYSXOBJECTDESCMANAGER_H_


#define APHYSX_OBJECTDESCNAME_PREFIX "PhysXObjectDesc_"


class APHYSX_DLL_EXPORT APhysXObjectDescManager
{

public:

	~APhysXObjectDescManager() { Release(); }
	
	static APhysXObjectDesc* CreateObjectDesc(APhysXObjectType apxObjectType, const char* szName = NULL);


	APhysXObjectDesc* CreateAndPushObjectDesc(APhysXObjectType apxObjectType, const char* szName = NULL);
	void ReleaseObjectDesc(APhysXObjectDesc* pObjectDesc, bool bDeleteObjectDesc = false);
	void ReleaseObjectDesc(const char* szFileName, const char* szObjName = NULL);

	bool Save(const char* szFileName);
	bool Load(const char* szFileName);
	bool LoadFromFileImage(const char* szFileName); 	// Load data by AFileImage

	void Release();

	int GetNum() const { return m_arrObjectDescs.size();}
	APhysXObjectDesc* GetPhysXObjectDesc(int i) const { return m_arrObjectDescs[i];}

	// To note: one file may include more than one APhysXObjectDesc! In this case, we must 
	// pass the obj name as the id as well!
	APhysXObjectDesc* GetPhysXObjectDesc(const char* szFileName, const char* szObjName = NULL);

	// copy the data to a new created pointer and push it to the array
	void Add(const APhysXObjectDesc* pObjectDesc)
	{
		if(pObjectDesc)
		{
			m_arrObjectDescs.push_back(pObjectDesc->Clone());
		}
	}

	bool AddObjectDescRef(APhysXObjectDesc* pObjectDesc);
	bool RemoveObjectDescRef(APhysXObjectDesc* pObjectDesc);

	void LockWriting()
	{
		m_MutexObjectDescs.Lock();
	}
	
	void UnlockWriting()
	{
		m_MutexObjectDescs.Unlock();
	}

public:

	bool Save(NxStream* pNxStream);
	// szFileName is used for create the Hash key of APhysXObjectDesc...
	bool Load(NxStream* pNxStream, const char* szFileName);		
	
protected:

	APhysXArray<APhysXObjectDesc* > m_arrObjectDescs;

	//-----------------------------------------------------------------
	// define maps...
	typedef APhysXMap<APhysXString, APhysXObjectDesc*> APhysXObjectDescMap;
	typedef APhysXObjectDescMap::value_type APhysXObjectDescPair;

	// note: only the APhysXObjectDescs which are loaded from stream will be
	// pushed into the map... ??? maybe some issue?
	APhysXObjectDescMap m_mapObjectDesc;

	// for multi-thread support
	APhysXMutex m_MutexObjectDescs;
};


#endif