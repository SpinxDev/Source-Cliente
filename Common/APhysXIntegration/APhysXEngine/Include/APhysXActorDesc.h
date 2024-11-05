/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXACTORDESC_H_
#define _APHYSXACTORDESC_H_

class APHYSX_DLL_EXPORT APhysXActorDesc:public APhysXSerializerBase
{
	DeclareVersion(0xcc000003);

public:

	APhysXActorDesc()
	{
		SetToDefault();
	}

	~APhysXActorDesc() 
	{

	}

	void SetToDefault()
	{
		mHasBody = true;
		mBodyFlags = NX_BF_VISUALIZATION | NX_BF_ENERGY_SLEEP_TEST;
		mLinearVelocity.zero();
		mAngularVelocity.zero();
		mLinearDamping = 0.0f;
		mAngularDamping = 0.05f;

		mActorFlags = 0;
		mActorGroup = 0;
		mDensity = 1.0f;

		// mLocalPose.id();
		mScale3D.set(1.0f);

		mSolverIterationCount = 4;
	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	void GetNxActorDesc(NxActorDesc& actorDesc)
	{
		actorDesc.setToDefault();
		actorDesc.flags = mActorFlags;
		actorDesc.group = mActorGroup;
		actorDesc.density = mDensity;
		actorDesc.name = m_szName;
	}

	void GetNxBodyDesc(NxBodyDesc& bodyDesc)
	{
		bodyDesc.setToDefault();

		bodyDesc.flags = mBodyFlags;
		bodyDesc.linearVelocity = mLinearVelocity;
		bodyDesc.angularVelocity = mAngularVelocity;
		bodyDesc.linearDamping = mLinearDamping;
		bodyDesc.angularDamping = mAngularDamping;

		bodyDesc.solverIterationCount = mSolverIterationCount;
	}

	// none virtual interface...
	APhysXActorDesc* Clone() const
	{
		APhysXActorDesc* pPhysXActorDesc = new APhysXActorDesc;
		*pPhysXActorDesc = *this;
		return pPhysXActorDesc;
	}

public:

	// body parameters...
	bool mHasBody;

	APhysXU32 mBodyFlags;
	NxVec3 mLinearVelocity;
	NxVec3 mAngularVelocity;
	float mLinearDamping;
	float mAngularDamping;
	
	// actor parameters...
	APhysXU32 mActorFlags;
	
	// only corresponding to Contact Report, currently all actors' group is set 0
	// and do not change its default value if you don't know what you want to do...
	NxActorGroup mActorGroup;		
	
	float mDensity;
	

	// others...

	//The pose of the actor component in the coordinate frame of the owning APhysXObject.
	NxMat34 mLocalPose;

	//--------------------------------------------------------------------
	// Note:
	//		version number ++ (0xcc000002)
	//
	// this scale will take effect for all shapes belong to the actor.
	// it is due to the requirements that Angelica Engine can control
	// the specified bone's scale of a skeleton model...
	//
	// for most cases, we only consider two component of scale, axis scale
	// (corresponding to X and Z of mScale3D)and radial scale (corresponding 
	// to Y of mScale3D), since the majority of the shapes in the skeleton model 
	// is capsule-type.
	NxVec3 mScale3D;				

	//--------------------------------------------------------------------
	// Note:
	//		version number ++ (0xcc000003)
	// 
	// to expose the parameter, solverIterationCount, to users so as to
	// tune the effect of jointed actors which often should be large to
	// achieve the stable behavior.
	APhysXU32 mSolverIterationCount;

};



#endif