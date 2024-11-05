/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXSHAPEDESC_H_
#define _APHYSXSHAPEDESC_H_


#define DeclareShapeType(type)												\
	public:																	\
		virtual APhysXShapeType GetShapeType() const {return type;}				\


enum APhysXShapeType
{
	// base
	APX_SHAPETYPE_BASE,

	// primitive
	APX_SHAPETYPE_PLANE,
	APX_SHAPETYPE_SPHERE,
	APX_SHAPETYPE_BOX,
	APX_SHAPETYPE_CAPSULE,
	
	// complex
	APX_SHAPETYPE_CONVEX,
	APX_SHAPETYPE_TRIANGLEMESH,
	APX_SHAPETYPE_HEIGHTFIELD,

	// others
	APX_SHAPETYPE_WHEEL,

	//---------------------------------------------------------
	// note: new type should be added from here...

};

class APHYSX_DLL_EXPORT APhysXShapeDesc:public APhysXSerializerBase
{
	DeclareVersion(0xcc000001);
	DeclareShapeType(APX_SHAPETYPE_BASE);


	friend class APhysXShapeDescManager;

	
	enum 
	{
		// this constant is for the member mMaterialIndex
		// when mMaterialIndex is set to this value, it means that the shape will use
		// the default material of the NxScene
		APX_SHAPEMATERIALINDEX_USEDEFAULT = 0xff,
	};

public:


	APhysXShapeDesc()
	{
		SetToDefault();
	}

	virtual ~APhysXShapeDesc() {}

	virtual void SetToDefault()
	{
		mShapeFlags = NX_SF_VISUALIZATION;
		mLocalPose.id();
		mShapeGroup = 0;
		mMaterialIndex = 0;
	}



	//-----------------------------------------------------------------------------------
	// to be aware that this class just new the pointer
	// to NxShapeDesc or NxForceFieldShapeDesc, but will not save it, so save and release
	// of the pointer is responsibility of APhysXShapeDescManager
	virtual NxShapeDesc* CreateNxShapeDesc(const NxVec3& vSacle3D = NxVec3(1.0f) ) { return NULL; }

	virtual void GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D = NxVec3(1.0f) );

	virtual NxForceFieldShapeDesc* CreateNxForceFieldShapeDesc(const NxVec3& vSacle3D = NxVec3(1.0f) ) { return NULL; }

	virtual void GetNxForceFieldShapeDesc(NxForceFieldShapeDesc& InNxForceFieldShapeDesc, const NxVec3& vSacle3D = NxVec3(1.0f) );
	//-----------------------------------------------------------------------------------


	virtual bool IsUseDefaultMaterial()
	{
		return mMaterialIndex == APX_SHAPEMATERIALINDEX_USEDEFAULT;
	}

	virtual void UseDefaultMaterial()
	{
		mMaterialIndex = APX_SHAPEMATERIALINDEX_USEDEFAULT;
	}

	// virtual interface since I have children...
	virtual APhysXShapeDesc* Clone() const
	{
		APhysXShapeDesc* pPhysXShapeDesc = new APhysXShapeDesc;
		*pPhysXShapeDesc = *this;
		return pPhysXShapeDesc;
	}

protected:

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	// properties are public for direct access
public:

	APhysXU32 mShapeFlags;
	NxMat34 mLocalPose;
	APhysXU32 mShapeGroup;

	//----------------------------------------------------------------------------------------
	// it will be set by APhysXRigidBodyObjectDesc::AddActorComponent(...) but not by user...
	// note here! this variable is used as a local index of material for a APhysXRigidBodyObject,
	// so when creating the real NxShapes, the materialIndex must be the global one of the NxScene
	// and we have to compute that global index of material via this local one!
	APhysXU32 mMaterialIndex;


};


#endif