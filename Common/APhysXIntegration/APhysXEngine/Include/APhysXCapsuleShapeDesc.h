/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCAPSULESHAPEDESC_H_
#define _APHYSXCAPSULESHAPEDESC_H_


class APHYSX_DLL_EXPORT APhysXCapsuleShapeDesc:public APhysXShapeDesc
{
	DeclareVersion(0xcc000001);
	DeclareShapeType(APX_SHAPETYPE_CAPSULE);

/*
	friend class APhysXShapeDescManager;

protected:
*/
public:


	APhysXCapsuleShapeDesc()
	{
		SetToDefault();
	}


	virtual void SetToDefault()
	{
		APhysXShapeDesc::SetToDefault();
		
		mRadius = 1.0f;
		mHeight = 1.0f;
		mFlags = 0;
		
	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	// to be aware that this class just new the pointer
	// to NxShapeDesc but not save it, so save and release
	// of the pointer is responsibility of APhysXShapeDescManager
	virtual NxShapeDesc* CreateNxShapeDesc(const NxVec3& vSacle3D = NxVec3(1.0f))
	{
		NxShapeDesc* pNxShapeDesc = new NxCapsuleShapeDesc;
		GetNxShapeDesc(*pNxShapeDesc, vSacle3D);
		return pNxShapeDesc;
	}

	virtual void GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D = NxVec3(1.0f));

	virtual NxForceFieldShapeDesc* CreateNxForceFieldShapeDesc(const NxVec3& vSacle3D = NxVec3(1.0f) ) 
	{
		NxForceFieldShapeDesc* pNxForceFieldShapeDesc = new NxCapsuleForceFieldShapeDesc;
		GetNxForceFieldShapeDesc(*pNxForceFieldShapeDesc, vSacle3D);
		return pNxForceFieldShapeDesc;
	}

	virtual void GetNxForceFieldShapeDesc(NxForceFieldShapeDesc& InNxForceFieldShapeDesc, const NxVec3& vSacle3D = NxVec3(1.0f) );

	// virtual interface
	virtual APhysXShapeDesc* Clone() const
	{
		APhysXShapeDesc* pPhysXShapeDesc = new APhysXCapsuleShapeDesc;
		*(APhysXCapsuleShapeDesc *)pPhysXShapeDesc = *this;
		return pPhysXShapeDesc;
	}

// properties are public for direct access
public:

	float mRadius;
	float mHeight;
	APhysXU32 mFlags;

};




#endif
