/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXPLANESHAPEDESC_H_
#define _APHYSXPLANESHAPEDESC_H_


class APHYSX_DLL_EXPORT APhysXPlaneShapeDesc:public APhysXShapeDesc
{
	DeclareVersion(0xcc000001);
	DeclareShapeType(APX_SHAPETYPE_PLANE);

/*
	friend class APhysXShapeDescManager;
protected:
*/
public:

	APhysXPlaneShapeDesc()
	{
		SetToDefault();
	}


	virtual void SetToDefault()
	{
		APhysXShapeDesc::SetToDefault();

		mNormal.set(0,1,0);
		mD = 0.0f;
	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	// to be aware that this class just new the pointer
	// to NxShapeDesc but not save it, so save and release
	// of the pointer is responsibility of APhysXShapeDescManager
	virtual NxShapeDesc* CreateNxShapeDesc(const NxVec3& vSacle3D = NxVec3(1.0f))
	{
		NxShapeDesc* pNxShapeDesc = new NxPlaneShapeDesc;
		GetNxShapeDesc(*pNxShapeDesc, vSacle3D);
		return pNxShapeDesc;
	}

	virtual void GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D = NxVec3(1.0f));

	// virtual interface
	virtual APhysXShapeDesc* Clone() const
	{
		APhysXShapeDesc* pPhysXShapeDesc = new APhysXPlaneShapeDesc;
		*(APhysXPlaneShapeDesc *)pPhysXShapeDesc = *this;
		return pPhysXShapeDesc;
	}

	// properties are public for direct access
public:

	// The plane equation takes the following form: 
	// mNormal.x * X + mNormal.y * Y + mNormal.z * Z = mD
	NxVec3 mNormal;
	float mD;

};




#endif
