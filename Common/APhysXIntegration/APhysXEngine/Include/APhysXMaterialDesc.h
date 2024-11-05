/*----------------------------------------------------------------------

Common header of APhysXEngine header files...

Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXMATERIALDESC_H_
#define _APHYSXMATERIALDESC_H_


class APHYSX_DLL_EXPORT APhysXMaterialDesc:public APhysXSerializerBase
{
	DeclareVersion(0xcc000001);
	
public:

	APhysXMaterialDesc()
	{
		SetToDefault();
	}
	
	
	APhysXMaterialDesc(float fRestitution, float fDFriction, float fSFriction): mRestitution(fRestitution), mDynamicFriction(fDFriction), mStaticFriction(fSFriction)
	{
		
	}
	
	virtual void SetToDefault()
	{
		mRestitution = 0.0f;
		mDynamicFriction = 0.0f;
		mStaticFriction = 0.0f;
	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	bool operator == (const APhysXMaterialDesc& other) const
	{
		return	(mRestitution == other.mRestitution)			&&
				(mDynamicFriction == other.mDynamicFriction)	&&
				(mStaticFriction == other.mStaticFriction);
	}

	void GetNxMaterialDesc(NxMaterialDesc& materialDesc) const 
	{
		materialDesc.setToDefault();

		materialDesc.restitution = mRestitution;
		materialDesc.dynamicFriction = mDynamicFriction;
		materialDesc.staticFriction = mStaticFriction;
	}

	// none virtual interface...
	APhysXMaterialDesc* Clone() const
	{
		APhysXMaterialDesc* pPhysXMaterialDesc = new APhysXMaterialDesc;
		*pPhysXMaterialDesc = *this;
		return pPhysXMaterialDesc;
	}

	// properties are public for direct access
public:

	float mRestitution;
	float mDynamicFriction;
	float mStaticFriction;

};

extern APhysXMaterialDesc APHYSX_DLL_EXPORT gDefaultPhysXMaterialDesc;


/*

// seem to be no need to create a singleton manager...

class APHYSX_DLL_EXPORT APhysXMaterialDescManager
{

	//----------------------------------------------------------
	// singleton object???

protected:

	APhysXMaterialDescManager(){}

public:
	static APhysXMaterialDescManager* GetPtrInstance()
	{

		static APhysXMaterialDescManager s_APhysXMaterialDescManager;
		return &s_APhysXMaterialDescManager;
	}


	//----------------------------------------------------------

public:

	~APhysXMaterialDescManager() { Release(); }

	void Release()
	{
		m_arrMaterialDescs.clear();
	}
	
	int GetIndex(const APhysXMaterialDesc& materialDesc)
	{
		for(unsigned int i=0; i<m_arrMaterialDescs.size(); i++)
		{
			if(materialDesc == m_arrMaterialDescs[i])
				return i;
		}

		m_arrMaterialDescs.push_back(materialDesc);
		return m_arrMaterialDescs.size() - 1;

	}


protected:

	APhysXArray<APhysXMaterialDesc> m_arrMaterialDescs;

};

#define gGlobalPhysXMaterialManager (APhysXMaterialDescManager::GetPtrInstance())

*/

#endif
