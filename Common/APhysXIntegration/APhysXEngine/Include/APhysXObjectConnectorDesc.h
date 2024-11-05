/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

	Description class of APhysXObjectConnector
----------------------------------------------------------------------*/

#ifndef _APHYSXOBJECTCONNECTORDESC_H_
#define _APHYSXOBJECTCONNECTORDESC_H_


#define DeclareConnectorType(type)									\
	public:															\
		virtual APhysXConnectorType GetConnectorType() const {return type;}			\
	protected:												


enum APhysXConnectorType
{
	//---------------------------------------------------------
	// note: new type can be added in the relative category...

	//--------------------------------------------------------------------------
	// abstract base type...
	APX_CNTTYPE_ABSTRACT_BASE,
	APX_CNTTYPE_ABSTRACT_JOINT,

	//--------------------------------------------------------------------------
	// joint...
	APX_CNTTYPE_CYLINDRICAL_JOINT = 10,
	APX_CNTTYPE_D6_JOINT,
	APX_CNTTYPE_DISTANCE_JOINT,
	APX_CNTTYPE_FIXED_JOINT,
	APX_CNTTYPE_POINTINPLANE_JOINT,
	APX_CNTTYPE_POINTONLINE_JOINT,
	APX_CNTTYPE_PRISMATIC_JOINT,
	APX_CNTTYPE_PULLEY_JOINT,
	APX_CNTTYPE_REVOLUTE_JOINT,
	APX_CNTTYPE_SPHERICAL_JOINT,

	//--------------------------------------------------------------------------
	// attacher...
	//??? special for the case that cloths or fluids attach to rigid body object
	APX_CNTTYPE_CLOTH_ATTACH_RB = 30,
	APX_CNTTYPE_FLUIDEMITTER_ATTACH_RB,
};


class APhysXObject;

class APHYSX_DLL_EXPORT APhysXObjectConnectorDesc:public APhysXSerializerBase
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_ABSTRACT_BASE);

public:

	APhysXObjectConnectorDesc()
	{
		mPhysXObject1Name = NULL;
		mPhysXObject2Name = NULL;

		mPhysXObject1 = mPhysXObject2 = NULL;
	}

	virtual ~APhysXObjectConnectorDesc() {}
	virtual void Release() {}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const = 0;
	/*
	{
		*pDestConnectorDesc = *this;
	}
	*/

	// virtual interface since I have children...
	virtual APhysXObjectConnectorDesc* Clone() const = 0;

	// only for the case of inner connectors of APhysXObject, transform some
	// parameters in the local frame to global frame
	virtual void TransformLocal2Global(const NxMat34& globalPose, const NxVec3& vScale3D) {}

	bool IsAPhysXObjectsSet() { return mPhysXObject1 || mPhysXObject2; }
	bool IsAPhysXObjectNamesSet() { return mPhysXObject1Name || mPhysXObject2Name; }

public:

	//------------------------------------------------------------------------------
	// note:
	// we can choose two ways to set connector's object1 and object2
	// 1. set mPhysXObject1 and mPhysXObject2 directly, this is mostly applicable for the case that
	//    runtime creation of connectors.
	// 2. set mPhysXObject1Name and mPhysXObject2Name, then we can find the corresponding objects via
	//    the names. this is suitable for edit-time setting
	//------------------------------------------------------------------------------

	const char* mPhysXObject1Name;			// name of APhysXObject 1
	const char* mPhysXObject2Name;			// name of APhysXObject 2
	
	APhysXObject* mPhysXObject1;			// APhysXObject 1
	APhysXObject* mPhysXObject2;			// APhysXObject 2

	//???? seems not to be a good idea...
	// since we create the connector via APhysXObject, so we only keep 
	// the other object name
	// const char* mOtherPhysXObjectName;			// name of the other APhysXObject 

};

//--------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXObjectConnectorInstanceDesc 
{

public:
	APhysXObjectConnectorInstanceDesc()
	{
		SetToDefault();
	}

	void SetToDefault()
	{
		mGlobalPose.id();
		mScale3D.set(1.0f);

		mPhysXConnectorDesc = NULL;
	}

public:

	NxMat34 mGlobalPose;
	NxVec3 mScale3D;

	APhysXObjectConnectorDesc* mPhysXConnectorDesc;
};

//--------------------------------------------------------------------
// manager class of a set of APhysXObjectConnectorDescs, whose major 
// responsibility lies in to save and load from stream...
class APHYSX_DLL_EXPORT APhysXObjectConnectorDescManager
{

public:

	~APhysXObjectConnectorDescManager()
	{
		Release();
	}

	static APhysXObjectConnectorDesc* CreateConnectorDesc(APhysXConnectorType apxConnectorType);

	// serialization/deserialization
	bool Save(NxStream* pNxStream);
	bool Load(NxStream* pNxStream);

	void Release();

	// copy the data to a new created pointer and push it to the array
	void Add(const APhysXObjectConnectorDesc* pPhysXConnectorDesc)
	{
		APhysXObjectConnectorDesc* pMyConnectorDesc = CreateConnectorDesc(pPhysXConnectorDesc->GetConnectorType());
		APHYSX_ASSERT(pMyConnectorDesc);
		pPhysXConnectorDesc->CopyTo(pMyConnectorDesc);
		m_arrPhysXConnectorDescs.push_back(pMyConnectorDesc);
	}

	void Remove(APhysXObjectConnectorDesc* pPhysXConnectorDesc)
	{
		m_arrPhysXConnectorDescs.deleteEntry(pPhysXConnectorDesc);
	}

	int GetNum() const { return m_arrPhysXConnectorDescs.size(); }
	APhysXObjectConnectorDesc* GetPhysXConnectorDesc(int i) const
	{
		return m_arrPhysXConnectorDescs[i];
	}
	
	const APhysXObjectConnectorDescManager& operator= (const APhysXObjectConnectorDescManager& other)
	{
		Release();

		for(int i = 0; i< other.GetNum(); i++)
		{
			m_arrPhysXConnectorDescs.push_back(other.GetPhysXConnectorDesc(i)->Clone());	
		}

		return *this;
	}

protected:
	
	APhysXArray<APhysXObjectConnectorDesc* > m_arrPhysXConnectorDescs;

};

#endif
