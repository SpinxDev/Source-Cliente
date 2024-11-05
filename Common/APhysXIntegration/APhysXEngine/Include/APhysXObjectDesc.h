/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/
#ifndef _APHYSXOBJECTDESC_H_
#define _APHYSXOBJECTDESC_H_


#define DeclareObjType(type)											\
	public:																\
		virtual APhysXObjectType GetObjType() const {return type;}		\
	protected:												


enum APhysXObjectType
{
	//---------------------------------------------------------
	// note: new type can be added in the relative category...

	//--------------------------------------------------------------------------
	// abstract base type...
	APX_OBJTYPE_ABSTRACT_BASE,
	APX_OBJTYPE_ABSTRACT_RB,

	//--------------------------------------------------------------------------
	// rigid body...
	APX_OBJTYPE_DUMMY = 10,

	APX_OBJTYPE_STATIC,

	APX_OBJTYPE_DYNAMIC,

	APX_OBJTYPE_TERRAIN_PATCH,

	APX_OBJTYPE_SKELETON,

/*
	// to do...
	// APX_OBJTYPE_BONE,

	APX_OBJTYPE_CHARACTER,

	APX_OBJTYPE_VEHICLE,

	APX_OBJTYPE_BREAKABLE,
*/

	//--------------------------------------------------------------------------
	// breakable...
	APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE = 20,

	APX_OBJTYPE_SKELETON_BREAKABLE,


	//--------------------------------------------------------------------------
	// cloth...
	APX_OBJTYPE_CLOTH = 30,

	//--------------------------------------------------------------------------
	// fluid...
	APX_OBJTYPE_FLUID = 40,
	APX_OBJTYPE_FLUID_ORIENTED_PARTICLE,

	//--------------------------------------------------------------------------
	// force field...
	APX_OBJTYPE_FORCEFIELD = 50,
	
	// define some useful force field template, so that we can set the parameters
	// more directly...
	APX_OBJTYPE_FORCEFIELD_VACUUM,
	
	APX_OBJTYPE_FORCEFIELD_GRAVITATION,

	APX_OBJTYPE_FORCEFIELD_BUOYANCY,

	APX_OBJTYPE_FORCEFIELD_EXPLOSION,

	APX_OBJTYPE_FORCEFIELD_WIND,

	APX_OBJTYPE_FORCEFIELD_VORTEX,
	
	APX_OBJTYPE_FORCEFIELD_BOSS,
	
	//--------------------------------------------------------------------------
	// compound object...

	// compound object which is made up of several type of objects
	// one case is a pole with a flag, which is made up of one cloth object and
	// one rigid body object, in this case all child objects must have the same
	// lifespan as the parent object.
	APX_OBJTYPE_COMPOUND = 100,

	// cloth attacher object is a type of compound object which comprises a cloth object and
	// a rigid body object that cloth object attaches to. we provide some specific interface 
	// for easy use of attaching this type of object to Angelica Engine's hanger...
	APX_OBJTYPE_CLOTH_ATTACHER,

	// similar with cloth attacher, while this is for fluid emitter which is attached to
	// a rigid body object...
	APX_OBJTYPE_FLUIDEMITTER_ATTACHER,

	APX_OBJTYPE_RB_ATTACHER,

	// general attacher, a base type of all other concrete attacher, such as
	// cloth attacher and fluid emitter attacher...
	APX_OBJTYPE_ATTACHER = 110,
};

inline static bool IsStaticRBType(APhysXObjectType apxObjType)
{
	return 
		(apxObjType == APX_OBJTYPE_STATIC)				||
		(apxObjType == APX_OBJTYPE_TERRAIN_PATCH)		// ||
		;
}

inline static bool IsDynamicRBType(APhysXObjectType apxObjType)
{
	return 
		(apxObjType == APX_OBJTYPE_ABSTRACT_RB)			||
		(apxObjType == APX_OBJTYPE_DUMMY)				||
		(apxObjType == APX_OBJTYPE_DYNAMIC)				||
		(apxObjType == APX_OBJTYPE_SKELETON)			||

		(apxObjType == APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE)	||
		(apxObjType == APX_OBJTYPE_SKELETON_BREAKABLE)			// ||

		/*
		(m_apxObjectType == APX_OBJTYPE_BONE)			||
		(m_apxObjectType == APX_OBJTYPE_CHARACTER)		||
		(m_apxObjectType == APX_OBJTYPE_VEHICLE)		||
		(m_apxObjectType == APX_OBJTYPE_BREAKABLE)
		*/

		;	
}

inline static bool IsRigidBodyType(APhysXObjectType apxObjType)
{
	return IsStaticRBType(apxObjType) || IsDynamicRBType(apxObjType);
}

inline static bool IsClothType(APhysXObjectType apxObjType) 
{
	return ( apxObjType == APX_OBJTYPE_CLOTH);
}

inline static bool IsFluidType(APhysXObjectType apxObjType) 
{
	return	
		( apxObjType == APX_OBJTYPE_FLUID)				||
		( apxObjType == APX_OBJTYPE_FLUID_ORIENTED_PARTICLE)
		;
		
}

inline static bool IsForceFieldType(APhysXObjectType apxObjType) 
{
	return	( apxObjType == APX_OBJTYPE_FORCEFIELD)					||
			( apxObjType == APX_OBJTYPE_FORCEFIELD_VACUUM)			||
			( apxObjType == APX_OBJTYPE_FORCEFIELD_GRAVITATION)		||
			( apxObjType == APX_OBJTYPE_FORCEFIELD_BUOYANCY)		||
			( apxObjType == APX_OBJTYPE_FORCEFIELD_EXPLOSION)		||
			( apxObjType == APX_OBJTYPE_FORCEFIELD_WIND)			||
			( apxObjType == APX_OBJTYPE_FORCEFIELD_VORTEX)			||
			( apxObjType == APX_OBJTYPE_FORCEFIELD_BOSS)			//||
		;
}

inline static bool IsAttacherType(APhysXObjectType apxObjType)
{

	return 

		(apxObjType == APX_OBJTYPE_ATTACHER)				||
		(apxObjType == APX_OBJTYPE_RB_ATTACHER)				||
		(apxObjType == APX_OBJTYPE_CLOTH_ATTACHER)			||
		(apxObjType == APX_OBJTYPE_FLUIDEMITTER_ATTACHER)			
		;

}


inline static bool IsCompoundType(APhysXObjectType apxObjType)
{
	return	
		
		(apxObjType == APX_OBJTYPE_COMPOUND)				||
		IsAttacherType(apxObjType)
		;
}

//---------------------------------------------------------------------------------
// Note: 
//		do not change or set collision group directly, use the default value or 
//		change it via APhysXObjectFlag instead...

enum APhysXCollisionGroup
{
	APX_COLLISION_GROUP_DEFAULT = 0,
/*	
	APX_COLLISION_GROUP_SKIP_CC_MOVE = 1,					// Objects of this group will be skipped when character controller is moving.

	APX_COLLISION_GROUP_SKIP_STATIC_RB = 2,					// Objects in this collision group will skip the collision with static RB group
*/

	APX_COLLISION_GROUP_FILTER_OUT_NON_RB = 1,				// filter out all non-RB objects' collision, including cloth, fluid, SB...
	APX_COLLISION_GROUP_FILTER_OUT_NON_DYNAMIC_RB = 2,		// filter out all non-StaticRB objects' collision, including static RB, cloth, fluid, SB...
	
	APX_COLLISION_GROUP_CC_HOST_RB,							// for the RB actors which are parts of skeleton RB object bound to APhysXCC. 
															// since in 2.8.1 PhysX SDK, NxScene's Overlap/Raycast functions will not be accelerated by their groupsMask parameter,
															// when utilizing these functions for APhysXCC, we always want to filter out APhysXCC's host actors. So, we can filter out this group by the parameter activeGroups.
	
	APX_COLLISION_GROUP_TERRAIN_RB = 8,						// all terrain RB objects are in this collision group
	APX_COLLISION_GROUP_STATIC_RB = 9,						// all static RB objects are in this collision group except terrain...
	

	// 10--14: reserved for cloth related...
	APX_COLLISION_GROUP_CLOTH = 10,
	APX_COLLISION_GROUP_CLOTH_ATTACHER,						// cloth attacher: disable collision with all other collision group except cloth

	// 15--19: reserved for soft-body related...
	APX_COLLISION_GROUP_SOFTBODY = 15,

	// 20--24: reserved for fluid related...
	APX_COLLISION_GROUP_FLUID = 20,
	
	// 25--31: reserved for ...
	APX_COLLISION_GROUP_ATTACHER = 25,						// attacher: disable collision with all other collision group...

	//-----------------------------------
	// can not beyond this value...
	APX_COLLISION_GROUP_INVALID = 32,
	//-----------------------------------
};

//---------------------------------------------------------------------------------
enum APhysXObjectFlag
{

	//---------------------------------------------------------
	// the common flags


	//---------------------------------------------------------
	// the following flags are just for RB object

	APX_OBJFLAG_SKIP_COLLISION_WITH_NON_DYNAMIC_RB		=   ( 1 << 10 ),			
	APX_OBJFLAG_ONEWAY_COLLISION						=   ( 1 << 11 ),			

	// CC related...
	APX_OBJFLAG_CC_ADD_PUSHFORCE						=	( 1 << 20 ),			// CC will add pushing force to me or not
	APX_OBJFLAG_CC_ADD_STANDFORCE						=	( 1 << 21 ),			// CC will add standing force to me or not
	APX_OBJFLAG_CC_CAN_ATTACH							=   ( 1 << 22 ),			// CC can stand and attach to me or not. This flag also affect LWCC's pos-revising.  
	APX_OBJFLAG_CC_SKIP_COLLISION						=   ( 1 << 23 ),			// CC will skip the collision when CC is moving
	
	// Cloth related...
	APX_OBJFLAG_CLOTH_DISABLE_CONSTRAINTNORMAL			=   ( 1 << 28 ),			// the constrained cloth's constrain normals will be disabled...
		
};

class APhysXObjectConnectorDesc;

//---------------------------------------------------------------------------------
// the base class of PhysX object description
// comprise the information of physics assets and properties which are shared
// by a set of PhysX objects.
// so, this description is referenced by the APhysXObjectInstanceDesc which is used to create
// the real PhysX object in a scene
class APHYSX_DLL_EXPORT APhysXObjectDesc: public APhysXRefCountable, public APhysXSerializerBase
{
	//-------------------------------------------------------------
	// current version...
	DeclareVersion(0xcc000004);					// add mFlags 

	// version history...
	// DeclareVersion(0xcc000003);				// add mSimInCompartment 
	// DeclareVersion(0xcc000002);				// add life span 
	// DeclareVersion(0xcc000001);
	
	//-------------------------------------------------------------

	DeclareObjType(APX_OBJTYPE_ABSTRACT_BASE);
	
	// APhysXObjectDescManager is the real class responsible for Create/Release and
	// Load and Save of this class and all its child classes
	friend class APhysXObjectDescManager;
	friend class APhysXCompoundObjectDesc;

//----------------------------------------------------------------------------
// disable the copy constructor...this is for consideration of efficiency...
// if you really want to create a APhysXObjectDesc based on some other 
// one, try to use the Clone() function or operator=

	DisableCopyConstructor(APhysXObjectDesc);

//----------------------------------------------------------------------------
public:
	
	APhysXObjectDesc()
	{
		SetToDefault();
	}

	virtual void SetToDefault()
	{
		mCollisionGroup = APX_COLLISION_GROUP_DEFAULT;
		mNetworkSync = false;

		mLifeSpan = NX_MAX_F32;

		mSimInCompartment = false;

		mFlags = APX_OBJFLAG_CC_ADD_PUSHFORCE;
	}

	virtual ~APhysXObjectDesc() {}
	virtual void Release() {}

	virtual void AddInnerConnector(const APhysXObjectConnectorDesc& apxConnectorDesc)
	{
		mInnerConnectorDescManager.Add(&apxConnectorDesc);
	}

	APhysXObjectConnectorDesc* GetInnerConnector(int iIndex)
	{
		if ((0 <= iIndex) && (iIndex < mInnerConnectorDescManager.GetNum()))
			return mInnerConnectorDescManager.GetPhysXConnectorDesc(iIndex);
		return 0;
	}
	
	// remove the specified inner-connector, note: this function won't delete the pInnerConnector
	void RemoveInnerConnector(APhysXObjectConnectorDesc* pInnerConnector)
	{
		mInnerConnectorDescManager.Remove(pInnerConnector);
	}

	// release all inner connectors...
	void ReleaseInnerConnectors()
	{
		mInnerConnectorDescManager.Release();
	}

	// virtual interface to clone myself, users who call this function is responsible to delete the pointer
	virtual APhysXObjectDesc* Clone() const = 0;

	bool IsRigidBody() const
	{
		return IsRigidBodyType(GetObjType());
	}
	
	bool IsStaticRB() const
	{
		return IsStaticRBType(GetObjType());
	}

	bool IsDynamicRB() const
	{
		return IsDynamicRBType(GetObjType());
	}

	bool IsCloth() const
	{
		return IsClothType(GetObjType());
	}

	bool IsFluid() const
	{
		return IsFluidType(GetObjType());
	}
	
	bool IsForceField() const 
	{
		return IsForceFieldType(GetObjType());
	}

	bool IsCompound() const
	{
		return	IsCompoundType(GetObjType());
	}

	bool IsAttacher() const
	{
		return IsAttacherType(GetObjType());
	}



	// use this interface instead of mCollisionGroup...
	APhysXU32 GetCollisionGroup() const;
	APhysXCollisionChannel GetCollisionChannel() const ;

//------------------------------------------------------------------------------------
//!!! it seems that we'd better make Save/Load routines "protected" functions so
// that we can only save and load the content via APhysXObjectDescManager. May 9, 2008
//??? why not just make the class public since it is a desc class. Mar.24, 2008

protected:
	
	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);


public:

		
	bool mNetworkSync;
	APhysXU32 mCollisionGroup;

	APhysXObjectConnectorDescManager mInnerConnectorDescManager;

	float mLifeSpan;

	bool mSimInCompartment;			// does the object simulate in compartment or primary scene...

	APhysXU32 mFlags;
};


//---------------------------------------------------------------------------------
// revised on Apr.21, 2008, by wenfeng
// make the APhysXObjectInstanceDesc inherited from APhysXSerializerBase, so that 
// we can assign a name for physics objects
//
// define all the information to create the real PhysX object
class APhysXUserGraphicsSync;
class APHYSX_DLL_EXPORT APhysXObjectInstanceDesc : public APhysXSerializerBase
{

public:
	APhysXObjectInstanceDesc()
	{
		SetToDefault();
	}

	void SetToDefault()
	{
		mGlobalPose.id();
		mScale3D.set(1.0f);
		
		mPhysXObjectDesc = NULL;
		mPhysXUserGraphicsSync = NULL;

		mPxdFileName = NULL;
		mObjName = NULL;
	}

public:

	NxMat34 mGlobalPose;
	NxVec3 mScale3D;

	mutable APhysXObjectDesc* mPhysXObjectDesc;

	APhysXUserGraphicsSync* mPhysXUserGraphicsSync;
	
	//---------------------------------------------------
	// another optional parameters instead of passing mPhysXObjectDesc directly
	// this would load mPhysXObjectDesc from the specified file by gPhysXEngine->GetObjectDescManager()
	// and would make the whole object's creation thread-safe...
	const char* mPxdFileName;
	const char* mObjName;

};

#endif