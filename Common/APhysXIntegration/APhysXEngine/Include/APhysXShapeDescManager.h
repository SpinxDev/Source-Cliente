/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXSHAPEDESCMANAGER_H_
#define _APHYSXSHAPEDESCMANAGER_H_


class APHYSX_DLL_EXPORT APhysXShapeDescManager
{

public:
	// only create an appropriate APhysXShapeDesc and return it
	// so, the user who keep the pointer should be responsible to delete it
	static APhysXShapeDesc* CreateShapeDesc(APhysXShapeType apxShapeType);

	// release the array of (NxShapeDesc*)pointers created by GetNxShapeDesc(...)
	static void ReleaseNxShapeDescs(NxArray<NxShapeDesc* >& shapeDescs)
	{
		for(unsigned int i=0; i<shapeDescs.size(); i++)
		{
			delete shapeDescs[i];
		}
		shapeDescs.clear();
	}

	// release the array of (NxForceFieldShapeDesc*)pointers created by GetNxForceFieldShapeDesc(...)
	static void ReleaseNxForceFieldShapeDescs(NxArray<NxForceFieldShapeDesc* >& ffShapeDescs)
	{
		for(unsigned int i=0; i<ffShapeDescs.size(); i++)
		{
			delete ffShapeDescs[i];
		}
		ffShapeDescs.clear();
	}


public:

	~APhysXShapeDescManager() { Release(); }

	//--------------------------------------------------------------
	// note here, April 23, 2008
	//   the initial design is that all APhysXShapeDescs can not be 
	// instantialized by themselves, and should be created, released,
	// and managed by this class.
	//   it seems that this is not a good design, and we'd better enable
	// defining a APhysXShapeDescs instance directly and push to 
	// this manager... refer to the implementation of 
	// APhysXObjectConnectorDescManager
	//--------------------------------------------------------------

	// create a new shape desc and then push back to the list...
	APhysXShapeDesc* CreateAndPushShapeDesc(APhysXShapeType apxShapeType);
	
	// create a new shape desc and then copy the data from pShapeDesc...
	APhysXShapeDesc* CreateAndPushShapeDesc(const APhysXShapeDesc* pShapeDesc);


	void ReleaseShapeDesc(APhysXShapeDesc* pShapeDesc);

	// serialization/deserialization
	bool Save(NxStream* pNxStream);
	bool Load(NxStream* pNxStream);

	void Release();

	int GetNum() const { return m_arrShapeDescs.size(); }
	APhysXShapeDesc* GetPhysXShapeDesc(int i) const { return m_arrShapeDescs[i]; }

	const APhysXShapeDescManager& operator= (const APhysXShapeDescManager& other);

	// NxShapeDesc* GetNxShapeDesc(int i) { return m_arrNxShapeDescs[i]; }
	NxShapeDesc* GetNxShapeDesc(int i, const NxVec3& vScale3D = NxVec3(1.0f))
	{
		return (m_arrShapeDescs[i]?m_arrShapeDescs[i]->CreateNxShapeDesc(vScale3D):NULL);
	}
	
	NxForceFieldShapeDesc* GetNxForceFieldShapeDesc(int i, const NxVec3& vScale3D = NxVec3(1.0f))
	{
		return (m_arrShapeDescs[i]?m_arrShapeDescs[i]->CreateNxForceFieldShapeDesc(vScale3D):NULL);
	}
	

	//void UpdateNxShapeDescs(); 


protected:

	
	
protected:

	APhysXArray<APhysXShapeDesc* > m_arrShapeDescs;
	
	// APhysXArray<NxShapeDesc* > m_arrNxShapeDescs;		// NxShapeDesc lists

};

#endif