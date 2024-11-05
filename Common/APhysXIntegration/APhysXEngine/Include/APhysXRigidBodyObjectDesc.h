/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXRIGIDBODYOBJECTDESC_H_
#define _APHYSXRIGIDBODYOBJECTDESC_H_

enum APhysXRBDominanceGroup
{
	APX_RB_DG_TOP					=	0,
	APX_RB_DG_HIGH					=	8,				// the default dominant group...
	APX_RB_DG_MIDDLE				=	16,
	APX_RB_DG_LOW					=	24,
	APX_RB_DG_BOTTOM				=	31,

};

class APhysXScene;

class APHYSX_DLL_EXPORT APhysXRigidBodyObjectDesc:public APhysXObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_ABSTRACT_RB);
	DisableCopyConstructor(APhysXRigidBodyObjectDesc);

	// each actor component has only one APhysXActorDesc and more than one APhysXShapeDescs...
	struct APhysXActorComponent
	{
		APhysXU32 ActorID;
		APhysXArray<APhysXU32> ShapeIDs;
		
		APhysXActorComponent()
		{
			ActorID = 0;
		}

		APhysXActorComponent* Clone() const;

		void Save(NxStream* pNxStream);

		void Load(NxStream* pNxStream);

	};

public:

	APhysXRigidBodyObjectDesc() {}
	virtual ~APhysXRigidBodyObjectDesc() { Release(); }
	
	virtual void Release();

	// -----------------------------------------------------------------------------------------
	// note: all shapes of one actor component should share the same material desc...
	// for editor...
	virtual void AddActorComponent(const APhysXActorDesc& physXActorDesc, const APhysXMaterialDesc& materialDesc, const APhysXArray<APhysXShapeDesc*>& physXShapeDescs);

	// for manager to create the NxActors...
	int GetNxActorDescNum() { return m_arrActorComponents.size(); }
	// each rigidbody may comprise more than one NxActors and so
	// id is the index corresponding to one special NxActor
	void GetNxActorDesc(int id, NxActorDesc& actorDesc)
	{
		int aid = m_arrActorComponents[id]->ActorID;
		m_arrActorDescs[aid]->GetNxActorDesc(actorDesc);
	}
	APhysXActorDesc* GetActorDesc(int id) 
	{
		int aid = m_arrActorComponents[id]->ActorID;
		return m_arrActorDescs[aid];
	}

	bool HasBody(int id)
	{
		int aid = m_arrActorComponents[id]->ActorID;
		return m_arrActorDescs[aid]->mHasBody;
	}

	void GetNxBodyDesc(int id, NxBodyDesc& bodyDesc)
	{
		int aid = m_arrActorComponents[id]->ActorID;
		m_arrActorDescs[aid]->GetNxBodyDesc(bodyDesc);
	}

	const NxMat34& GetNxActorLocalPose(int id)
	{
		int aid = m_arrActorComponents[id]->ActorID;
		return m_arrActorDescs[aid]->mLocalPose;
	}

	const NxVec3& GetActorScale3D(int id)
	{
		int aid = m_arrActorComponents[id]->ActorID;
		return m_arrActorDescs[aid]->mScale3D;
	}
	
	// input pPhysXScene as the parameter so as to update the material index of NxShapeDesc!
	virtual void GetNxShapeDescs(int id, APhysXScene* pPhysXScene, NxArray<NxShapeDesc* >& shapeDescs, const NxVec3& vScale3D = NxVec3(1.0f));

	const APhysXRigidBodyObjectDesc& operator= (const APhysXRigidBodyObjectDesc& other);

	// virtual interface to clone myself
	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXRigidBodyObjectDesc)

	//void UpdateMaterialID2Global(APhysXScene* pPhysXScene);

	APhysXU32 GetDominanceGroup() const;

	// provide a chance to get a APhysXRigidBodyObjectDesc of different type but have same RB properties as this,
	// and user must keep the returned pointer and release it manually.
	APhysXRigidBodyObjectDesc* CreateRBObjectDesc(APhysXObjectType objType);

protected:
	
	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	
	//void UpdateNxActorDescs();

protected:

	APhysXArray<APhysXActorDesc* > m_arrActorDescs;
	APhysXArray<APhysXMaterialDesc* > m_arrMaterialDescs;
	
	APhysXShapeDescManager m_ShapeDescManager;

	APhysXArray<APhysXActorComponent* > m_arrActorComponents;

	// seems to be wrong to lay the properties here!
	//APhysXArray<NxActorDesc* > m_arrNxActorDescs;
};


//-----------------------------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXDummyRBObjectDesc:public APhysXRigidBodyObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_DUMMY);

public:
	
	// virtual interface to clone myself
	DeclareCloneAPhysXObjectDesc(APhysXDummyRBObjectDesc)

};

//-----------------------------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXStaticRBObjectDesc:public APhysXRigidBodyObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_STATIC);

public:

	APhysXStaticRBObjectDesc()
	{
		mCollisionGroup = APX_COLLISION_GROUP_STATIC_RB;
	}


	// virtual interface to clone myself
	DeclareCloneAPhysXObjectDesc(APhysXStaticRBObjectDesc)

	// for editor...
	virtual void AddActorComponent(const APhysXActorDesc& physXActorDesc, const APhysXMaterialDesc& materialDesc, const APhysXArray<APhysXShapeDesc*>& physXShapeDescs);

};

//-----------------------------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXDynamicRBObjectDesc:public APhysXRigidBodyObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_DYNAMIC);

public:

	// virtual interface to clone myself
	DeclareCloneAPhysXObjectDesc(APhysXDynamicRBObjectDesc)

	// for editor...
	virtual void AddActorComponent(const APhysXActorDesc& physXActorDesc, const APhysXMaterialDesc& materialDesc, const APhysXArray<APhysXShapeDesc*>& physXShapeDescs);

};

//-----------------------------------------------------------------------------------------
// note here, Mar. 20, 2008
// as for this class, we just ignore the material passed in and force to use the default material.
// the reason to do this lie in comments of line#292 of APhysXRigidBodyObjectDesc.cpp
class APHYSX_DLL_EXPORT APhysXTerrainPatchRBObjectDesc:public APhysXStaticRBObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_TERRAIN_PATCH);

	// terrain patch object's loading and saving are managed by APhysXTerrainBlock
	friend class APhysXTerrainBlock;

public:

	// virtual interface to clone myself
	DeclareCloneAPhysXObjectDesc(APhysXTerrainPatchRBObjectDesc)

	// override to support special case of NxHeightfieldShapeDesc's material index
	virtual void GetNxShapeDescs(int id, APhysXScene* pPhysXScene, NxArray<NxShapeDesc* >& shapeDescs, const NxVec3& vScale3D = NxVec3(1.0f));

	// just ignore the material passed in and force to use the default material
	virtual void AddActorComponent(const APhysXActorDesc& physXActorDesc, const APhysXMaterialDesc& materialDesc, const APhysXArray<APhysXShapeDesc*>& physXShapeDescs);

};

//-----------------------------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXSkeletonRBObjectDesc:public APhysXDynamicRBObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_SKELETON);

public:

	// virtual interface to clone myself
	DeclareCloneAPhysXObjectDesc(APhysXSkeletonRBObjectDesc)

/*
	const APhysXSkeletonRBObjectDesc& operator= (const APhysXSkeletonRBObjectDesc& other)
	{

		int i= 0;
		return *this;
	};
*/

};



#endif