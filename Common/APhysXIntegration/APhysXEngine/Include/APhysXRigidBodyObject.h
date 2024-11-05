/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXRIGIDBODYOBJECT_H_
#define _APHYSXRIGIDBODYOBJECT_H_


//-------------------------------------------------------
// breakable routines, wenfeng, April 21, 2009
// RB Hit 

enum APhysXRBHitType
{
	APX_RBHITTYPE_RAY,				// cast a ray to hit and add force to the actor...
	APX_RBHITTYPE_CONTACT,			// hit by collision and contact...
};

struct APhysXRBHitInfo
{
	APhysXRBHitType mHitType;

	NxActor* mHitActor;					// the actor which is hit by the force
	NxVec3 mHitForce;

	NxVec3 mHitPos;


	// only for case of contact hit...
	NxActor* mSrcActor;					// the actor which enforces the force to the mHitActor
	
	NxForceMode mHitForceMode;
	float mMaxVelChange;				// only used for simply RB add force limitation, this would not work while breakable RBs involved(no matter RBs were broken or not)

	APhysXRBHitInfo()
	{
		mHitType = APX_RBHITTYPE_RAY;

		mHitActor = NULL;
		mSrcActor = NULL;

		mHitForceMode = NX_FORCE;
		mMaxVelChange = NX_MAX_F32;

		//??? need to initialize other members?...
		mHitPos.set(0.0f);
	}

	inline float GetBreakScale() { return mHitForce.magnitude(); }
};


enum APhysXRBDrivenMode
{
	APX_RBDRIVENMODE_ANIMATION,						// animation mode:	driven by animation...
	APX_RBDRIVENMODE_PHYSICS,						// dynamic mode:	driven by Physics simulation
	APX_RBDRIVENMODE_MIX,							// mix mode:		only parts of actors are driven by simulation
};


class APhysXJoint;


//-----------------------------------------------------------------
// APhysXRigidBodyObject is still a Abstract class
class APHYSX_DLL_EXPORT APhysXRigidBodyObject: public APhysXObject
{
	// just use GetType() instead
	//DeclareObjType(APX_OBJTYPE_ABSTRACT_BASE);

protected:

	// make the constructor "protected" so that the user can not instantialize it directly
	APhysXRigidBodyObject()
	{
		m_fMass = 0.0f;

		m_vUniformActorScale3D.set(1.0f);

		m_vSubSimCurPos.zero();
		m_vSubSimTargetPos.zero();
		m_qSubSimCurRot.id();
		m_qSubSimTargetRot.id();
		m_fAccumulatedSubSimDt = 0.0f;
	}

public:

	virtual ~APhysXRigidBodyObject() {}

	// Network sync
	// to do 
	virtual void SyncNetworkData(){}
	virtual void ReplicateNetworkData(){}


	virtual void PutToSleep();
	virtual void WakeUp();
	virtual bool IsSleeping();

	int GetNxActorNum () const { return m_arrNxActors.size(); }
	NxActor* GetNxActor(int i) const { return m_arrNxActors[i]; }
	NxActor* GetNxActor(const char* szActorName) const;
	int GetNxActorID(NxActor* pActor) const;
	bool HasNxActor(NxActor* pActor) const 
	{
		return (GetNxActorID(pActor) >= 0);
	}

	APhysXJoint* GetJoint(const char* szJointName) const;


	void ComputeMass();
	float GetMass() const
	{
		return m_fMass;
	}


	void EnableGravity(bool bEnable = true);
	void EnableKinematic(bool bEnable = true);
	void EnableKinematic(bool bEnable, APhysXArray<NxActor* >& arrExceptActors);

	virtual void EnableCollision(bool bEnable = true);

	// pActorsDrivenByPhysics: the actors driven by physics, it will take effect only if apxRBDrivenMode is set to APX_RBDRIVENMODE_MIX
	virtual void SetDrivenMode(APhysXRBDrivenMode apxRBDrivenMode, APhysXArray<NxActor* >* pActorsDrivenByPhysics = NULL);
	APhysXRBDrivenMode GetDrivenMode() const { return m_apxRBDrivenMode; }

	// remove the specified inner joint...
	void RemoveJoint(const char* szJointName);

	//------------------------------------------
	// to do!!!
	// initial implementation is a coarse one
	virtual void SetPose(const NxMat34& globalPose);
	virtual NxMat34 GetPose() const;
	virtual void GetAABB(NxBounds3& aabb) const;

	virtual void SetLinearVelocity(const NxVec3& vLinVel);
	virtual NxVec3 GetLinearVelocity() const;

	virtual void SetAngularVelocity(const NxVec3& vAngVel);
	virtual NxVec3 GetAngularVelocity() const;
	//------------------------------------------
	
	virtual void SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef = true);
	virtual void SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup);

	// Always return TRUE value for the rigid-body object that has only one NxActor.
	virtual bool IsSelfCollisionEnable() const;
	virtual void EnableSelfCollision(bool bEnableCollision);

	//------------------------------------------
	// raycast force...
	// if the force ray hit any actor of mine, the force will take effect and return the actor which was added force; 
	// else return null pointer...
	// make sure that the dir of forceRay is normalized...
	// "fMaxVelChange" will be ignored if any breakable RBs involved(no matter RBs were broken or not)...
	virtual NxActor* AddForce(const NxRay& forceRay, float fForceMagnitude, float fMaxDist = NX_MAX_F32, NxForceMode forceMode = NX_FORCE, float fMaxVelChange = NX_MAX_F32);
	
	// general force which will be enforced at the pos of actor's mass center...
	// "fMaxVelChange" will be ignored if any breakable RBs involved(no matter RBs were broken or not)...
	virtual bool AddForce(const NxVec3& vForce, NxForceMode forceMode = NX_FORCE, float fMaxVelChange = NX_MAX_F32);
	
	//--------------------------------------------------------------
	// sweep force
	// this is a extension of raycast force since sometimes the ray 
	// does not hit any actor, we can use sweep instead...
	virtual NxActor* AddSweepForce(const NxRay& forceRay, float fForceMagnitude, float fSweepRadius = 0.01f, float fMaxDist = NX_MAX_F32, NxForceMode forceMode = NX_FORCE, float fMaxVelChange = NX_MAX_F32);
	
	virtual bool AddForce(const APhysXRBHitInfo& apxRBHitInfo);

	//--------------------------------------------------------------
	// collide detection
	// if fSweepRadius is 0 then execute raycast test, or execute sweep test.
	// if any one of this RBs object was detected then return the actor pointer, or return null pointer...
	virtual NxActor* CollideDetection(const NxRay& forceRay, float fSweepRadius = 0.01f, float fMaxDist = NX_MAX_F32, APhysXRayHitInfo* pOutResult = 0) const;

	// when contact or ray hit me, call this function...
	virtual void OnHit(const APhysXRBHitInfo& apxRBHitInfo);


	// to retrieve the APhysXBreakable interface...
	virtual APhysXBreakable* IsBreakable()
	{ 
		return NULL;
	}

	virtual void SetObjectFlag(APhysXObjectFlag flag, bool val);

	virtual void EnableDebugRender(bool bEnable = true);

	void ExecSubSimulationTask(float dt);

protected:

	//virtual bool OnCreate(APhysXScene* pAPhysXScene, APhysXObjectDesc& apxObjDesc, const NxMat34& globalPose, const NxVec3& vScale3D = NxVec3(1.0f));
	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

	virtual void OnReleaseBasicPhysXStuff();

	// called after the NxActorDesc is created but before NxActor creation
	// override this function can get or modify the NxActorDesc before creating NxActor
	virtual void OnCreateNxActorDesc(NxActorDesc* pNxActorDesc, int iActorID) {};

	void ResetFixedActors();

	virtual void PostSimulate(float dt);

	// the real hit-handle code in this function
	virtual void HandleHit(bool bIgnoreVelChangeLimit = false);

	void ExeSetPose(const NxMat34& globalPose);

protected:

	virtual void SyncNetworkRBState(){};
	virtual void ReplicateNetworkRBState(){};

protected:
	struct ActorPair
	{
		ActorPair()
		{
			mpActor0 = 0;
			mpActor1 =0;
			mFlags = 0;
		}

		NxActor* mpActor0;
		NxActor* mpActor1;
		int mFlags;
	};

protected:
	
	// note: one rigid-body object may correspond to more than one NxActors...
	// so that we can make the jointed actors and static multi-actors map to 
	// only one APhysXObject...
	APhysXArray<NxActor* > m_arrNxActors;

	//-----------------------------------------------------------------------------
	// added by wenfeng, Jun 25, 2009
	// describe the simulation status of the RB object.
	//
	// refer to APhysXRBDrivenMode
	APhysXRBDrivenMode m_apxRBDrivenMode;

	// some actors which will be fixed even in dynamic simulation mode
	APhysXArray<NxActor* > m_arrFixedActors;


	APhysXArray<APhysXRBHitInfo> m_arrRBHitInfo;

	// actor pair flags backup
	APhysXArray<ActorPair> m_arrPairFlags;


	float m_fMass;			// the total mass of mine...

	NxVec3 m_vUniformActorScale3D;		// when all the actors of RB object are scaled uniformly, this member will keep the scale info...

	//------------------------------------------------------------------------------
	// added by wenfeng, Nov. 28, 2012
	// add some data to handle sub-simulation case
	NxVec3 m_vSubSimCurPos;			// Current position used in SetPose() during sub-simulation
	NxVec3 m_vSubSimTargetPos;		// target position used in SetPose() during sub-simulation
	NxQuat m_qSubSimCurRot;			// Cur rotation used SetPose() during sub-simulation
	NxQuat m_qSubSimTargetRot;		// target rotation used SetPose() during sub-simulation
	float m_fAccumulatedSubSimDt;
};

class APhysXRBSubSimulationReport : public APhysXUserSubSimulationReport, public APhysXSimplePtrManager<APhysXRigidBodyObject>
{

public:

	APhysXRBSubSimulationReport()
	{
		// register myself to gPhysXSubSimulationReport 
		gPhysXSubSimulationReport->Register(this);
	}


	virtual void PreSubSimulation(float dt)
	{
		for(int i=0; i<m_arrElements.size(); i++)
		{
			m_arrElements[i]->ExecSubSimulationTask(dt);
		}
	}

	virtual void OnObjectDeletion(APhysXObject* pObject)
	{
		if(pObject->IsRigidBody())
		{
			APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject* )pObject;
			UnRegister(pRBObj);
		}
	}

};


#endif
