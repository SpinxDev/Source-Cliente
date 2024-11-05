/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

		The description class of APhysXAttacher...
----------------------------------------------------------------------*/

#ifndef _APHYSXATTACHERDESC_H_
#define _APHYSXATTACHERDESC_H_


#define APX_ATTACHER_ATTACHER_NAME					"APhysXAttacher_Attacher"
#define APX_ATTACHER_ATTACHED_RB_NAME				"APhysXAttacher_Attached_RB"			// RB: rigid-body
#define APX_ATTACHER_ATTACHED_CLOTH_NAME			"APhysXAttacher_Attached_Cloth"
#define APX_ATTACHER_ATTACHED_FLUID_NAME			"APhysXAttacher_Attached_FLUID"			

#define APX_DEFAULT_ATTACHER_SIZE 0.01f
#define DeclareSphereAttacherDesc(attacher_desc_name)										\
	APhysXDynamicRBObjectDesc attacher_desc_name;											\
	{																						\
		APhysXActorDesc apxActorDesc;														\
		apxActorDesc.mDensity = 1.0f;														\
		APhysXMaterialDesc apxMaterialDesc;													\
		apxMaterialDesc.mDynamicFriction = 0.1f;											\
		apxMaterialDesc.mRestitution = 1.0f;												\
		APhysXSphereShapeDesc sphereDesc;													\
		sphereDesc.mRadius = APX_DEFAULT_ATTACHER_SIZE;										\
		sphereDesc.UseDefaultMaterial();													\
		APhysXArray<APhysXShapeDesc* > Shapes;												\
		Shapes.push_back(&sphereDesc);														\
		attacher_desc_name.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);		\
	}

#define DeclareBoxAttacherDesc(attacher_desc_name)											\
	APhysXDynamicRBObjectDesc attacher_desc_name;											\
	{																						\
		APhysXActorDesc apxActorDesc;														\
		apxActorDesc.mDensity = 1.0f;														\
		APhysXMaterialDesc apxMaterialDesc;													\
		apxMaterialDesc.mDynamicFriction = 0.1f;											\
		apxMaterialDesc.mRestitution = 1.0f;												\
		APhysXBoxShapeDesc boxDesc;															\
		boxDesc.mDimensions = NxVec3(APX_DEFAULT_ATTACHER_SIZE);							\
		boxDesc.UseDefaultMaterial();														\
		APhysXArray<APhysXShapeDesc* > Shapes;												\
		Shapes.push_back(&boxDesc);															\
		attacher_desc_name.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);		\
	}


class APHYSX_DLL_EXPORT APhysXAttacherDesc:public APhysXCompoundObjectDesc
{

	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_ATTACHER);

public:

	APhysXAttacherDesc() {}
	virtual ~APhysXAttacherDesc() {}


	virtual bool IsValid() { return true; }

	//--------------------------------------------------------------------
	// for some cases, we can generate the inner connector info 
	// automatically according to component info, such as the 
	// case of cloth attacher.
	//
	// we will call this function to revise the desc object just 
	// before creating the PhysX stuff...
	// 
	virtual void CompleteInnerConnectorInfo() 
	{
		// do nothing by default...
	}

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	/*
	virtual APhysXObjectDesc* Clone() const
	{
		CloneThis(APhysXAttacherDesc, APhysXObjectDesc)
	}
	*/
	DeclareCloneAPhysXObjectDesc(APhysXAttacherDesc)


	// by default, we only support one attacher object. 
	virtual bool AddAttacherComponent(const APhysXObjectDesc* pRBObjectDesc, const NxMat34& matLocalPose = NxMat34(true))
	{
		return AddMultiAttacherComponent(pRBObjectDesc, APX_ATTACHER_ATTACHER_NAME, matLocalPose);
	}

	// if you want to add more than one attacher, use this function.
	virtual bool AddMultiAttacherComponent(const APhysXObjectDesc* pRBObjectDesc, const char* szAttacherName, const NxMat34& matLocalPose = NxMat34(true));


};


#endif
