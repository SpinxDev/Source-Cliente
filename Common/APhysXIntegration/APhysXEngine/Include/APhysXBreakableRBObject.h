
/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	//////////////////////////////////////////////////////////	

	APhysXBreakableRBObjectDesc.h 

	Created by He Wenfeng, Aug.12, 2009

	All breakable RB Objects are defined in this file.


----------------------------------------------------------------------*/


#ifndef	_APHYSXBREAKABLERBOBJECT_H_
#define _APHYSXBREAKABLERBOBJECT_H_


//---------------------------------------------------------------------
// APhysXRBHitModifier: to modify/adjust the physics effect when some
// actors collide with breakable objects...
//
// Currently, we use the src actor's mass and hit actor's mass to 
// adjust the hit force and velocity of src actor after the collision.

class APhysXRBHitModifier
{

public:
	APhysXRBHitModifier(){}
	virtual ~APhysXRBHitModifier(){}

	void Reset() { m_arrHitSrcActorInfo.clear(); }
	virtual void OnHit(const APhysXRBHitInfo& apxRBHitInfo, bool bSetHitMass = false, float fHitMass = NX_MAX_F32);
	virtual void ModifyHitForce(APhysXRBHitInfo& apxRBHitInfo);
	virtual void ModifySrcActorVel();

	// make sure that mHitMass is not Zero!
	static float ComputeBreakForceFactor(float mHitMass, float mSrcMass);
	static float ComputeBreakForceFactor(NxActor* pHitActor, NxActor* pSrcActor);

protected:
	
	struct HitSrcActorInfo
	{
		NxActor*	mSrcActor;			// actor's pointer
		NxVec3		mPreHitVel;			// actor's linear velocity before the hit...
		float		mHitMass;			// actor's hitted object's mass

		HitSrcActorInfo() : mPreHitVel(0.0f)
		{
			mSrcActor = NULL;
			mHitMass = 0.0f;
		}

	};

	APhysXArray<HitSrcActorInfo> m_arrHitSrcActorInfo;
};




//---------------------------------------------------------------------
// APhysXSimpleBreakableSkeletonRBObject

class APHYSX_DLL_EXPORT  APhysXSimpleBreakableSkeletonRBObject: public APhysXSkeletonRBObject, public APhysXSimpleBreakable
{

	// friend class
	friend class APhysXScene;

public:

	virtual void OnHit(const APhysXRBHitInfo& apxRBHitInfo);

	virtual APhysXObject* IsAPhysXObject()
	{
		return (APhysXObject *)this;
	}

	virtual APhysXBreakable* IsBreakable()
	{
		return (APhysXBreakable* ) this;
	}

	virtual void SetDrivenMode(APhysXRBDrivenMode apxRBDrivenMode, APhysXArray<NxActor* >* pActorsDrivenByPhysics = NULL);

protected:

	APhysXSimpleBreakableSkeletonRBObject()
	{
		m_vBreakForce.set(0.0f);
		m_vBreakCenter.set(0.0f);

		m_fBreakFallOffRadius = 10.0f;			// tmp setting...
	}

	virtual void OnCreateNxActorDesc(NxActorDesc* pNxActorDesc, int iActorID);


	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

	virtual void OnBreak();

	virtual void PreSimulate(float dt);

	virtual void PostBreak();

	virtual void HandleHit(bool bIgnoreVelChangeLimit = false);

	virtual bool PostCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

protected:

	float m_fReleaseJointsTime;

	float m_fBreakFallOffRadius;

	float m_dt;		// current tick time

	//------------------------------------------------------
	// to adjust the visual effect of source hit objects
	// APhysXArray<APhysXHitSrcActorInfo> m_arrHitSrcActorInfo;

	APhysXRBHitModifier m_apxRBHitModifier;

	bool m_bDynamic;

};

//---------------------------------------------------------------------
// APhysXBreakableSkeletonRBObject

class APHYSX_DLL_EXPORT  APhysXBreakableSkeletonRBObject: public APhysXSkeletonRBObject, public APhysXBreakable
{
	// friend class
	friend class APhysXScene;

public:

	virtual APhysXObject* IsAPhysXObject()
	{
		return (APhysXObject *)this;
	}
	
	virtual APhysXBreakable* IsBreakable()
	{
		return (APhysXBreakable* ) this;
	}

	APhysXGraph* BuildActorNeighborGraph(float fThickness = 0.06f);			// build the actor neighbor graph at runtime
	APhysXGraph* BuildActorSupportGraph(float fThickness = 0.06f);			// build the actor support graph at runtime


	APhysXGraph* GetActorNeighborGraph() { return m_pActorNeighborGraph; }
	APhysXGraph* GetActorSupportGraph() { return m_pActorSupportGraph; }

	virtual void OnHit(const APhysXRBHitInfo& apxRBHitInfo);

	bool IsValid() { return m_pActorNeighborGraph && m_pActorSupportGraph; }

	virtual void SetDrivenMode(APhysXRBDrivenMode apxRBDrivenMode, APhysXArray<NxActor* >* pActorsDrivenByPhysics = NULL);

protected:
	
	APhysXBreakableSkeletonRBObject()
	{
		m_pActorNeighborGraph = NULL;
		m_pActorSupportGraph = NULL;

		m_fActorBreakLimit = 10000.0f;
		m_fBreakFallOffFactor = 0.3f;
		
	}

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);
	virtual void OnReleaseBasicPhysXStuff();

	virtual void PostSimulate(float dt);
	virtual void OnBreak();
	virtual void HandleHit(bool bIgnoreVelChangeLimit = false);


	bool IsActorBroken(int iActorID);
	bool CheckFullyBroken();
	void BreakActor(int iActorID);

	bool CanActorBreak(int iActorID);			// for case of user-specified fixed actor...

	bool IsActorSupported(int iActorID);


	// while, since our algorithm should use the Breadth-First graph traverse method, so, we do not use the recursive way...
	// Note: this is a recursive function and now it is deprecated...
	// void ComputeBreakForce_r(int iKernel, float fKernelForceMagnitude, APhysXArray<bool> arrIsActorVisited, int iCurLevel = 0);
	void ComputeBreakForce(int iKernel, const NxVec3& vKernelForce, APhysXArray<bool> arrIsActorVisited, NxActor* pHitSrcActor = NULL, int iMaxLevel = 5, float fSkipForceMagnitude = 10.0f);

protected:
	
	APhysXGraph* m_pActorNeighborGraph;
	APhysXGraph* m_pActorSupportGraph;

	// some other properties...
	float m_fActorBreakLimit;
	float m_fBreakFallOffFactor;


public:
	struct ActorBreakInfo
	{

		ActorBreakInfo(): mBreakForce(0.0f), mPostAddedForce(0.0f)
		{
		}

		NxVec3 mBreakForce;				// the force to check whether the actor will be broken...
		NxVec3 mPostAddedForce;			// the force to add the broken actor...
	};

protected:

	APhysXArray<ActorBreakInfo> m_arrActorBreakInfo;
	
	//------------------------------------------------------
	// to adjust the visual effect of source hit objects
	APhysXRBHitModifier m_apxRBHitModifier;

};



#endif
