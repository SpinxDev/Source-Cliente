/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXFLUIDOBJECT_H_
#define _APHYSXFLUIDOBJECT_H_

class APhysXOrientedParticleObject;

class APHYSX_DLL_EXPORT APhysXFluidObject: public APhysXObject
{
	// friend class
	friend class APhysXScene;
	
protected:
	
	// make the constructor "protected" so that the user can not instantialize it directly
	
	APhysXFluidObject()
	{
		m_pNxFluid = NULL;
		
		m_uParticleNum = 0;
		m_BufferPos = NULL;

		// the following data is optional
		m_BufferVel = NULL;

		m_BufferLife = NULL;
		m_BufferDensity = NULL;

		m_BufferId = NULL;
		m_BufferFlag = NULL;

		m_BufferCollisionNormal = NULL;

		m_uCreationIdNum = m_uDeletionIdNum = 0;
		m_BufferCreationId = NULL;
		m_BufferDeletionId = NULL;

	}

public:

	//------------------------------------------------------------------------------
	// to retrieve the fluid particle write data for rendering or other usage
	// note: 
	//		call these function at each tick and these buffer data are accessed
	// directly by i but not ID...
	int GetParticleNum() const { return m_uParticleNum; }
	
	const NxVec3* GetParticlePositions() const { return m_BufferPos; }
	const NxVec3* GetParticleVelocities() const { return m_BufferVel; }

	const float* GetParticleLives() const { return m_BufferLife; }
	const float* GetParticleDensities() const { return m_BufferDensity; }

	// note: the data and ID order in the buffer of IDsr may be different in next frame,
	// so, you must update your IDs each tick...
	const APhysXU32* GetParticleIDs() const { return m_BufferId; }
	const APhysXU32* GetParticleFlags() const { return m_BufferFlag; }

	const NxVec3* GetParticleCollisionNormals() const { return m_BufferCollisionNormal; }

	int GetCreatedIdNum() const { return m_uCreationIdNum; }
	const APhysXU32* GetCreatedParticleIds() const { return m_BufferCreationId; }

	int GetDeletedIdNum() const { return m_uDeletionIdNum; }
	const APhysXU32* GetDeletedParticleIds() const { return m_BufferDeletionId; }

	//------------------------------------------------------------------------------
	// to retrieve the PhysX objects...
	NxFluid* GetNxFluid() { return m_pNxFluid; }

	int GetEmitterNum() { return m_pNxFluid->getNbEmitters(); }
	NxFluidEmitter* GetEmitter(int i) { return m_pNxFluid->getEmitters()[i]; }

	//------------------------------------------------------------------------------
	// common interfaces of PhysX objects...
	//
	// note here:
	//		NxFluid objects have no Sleep/WakeUp functions...
	//
	virtual void PutToSleep();
	virtual void WakeUp();
	virtual bool IsSleeping()
	{ 
		if(m_pNxFluid && (NX_TRUE == m_pNxFluid->getFlag(NX_FF_ENABLED)))
			return false;
		else
			return true;
	}

	virtual void SetPose(const NxMat34& globalPose);

	virtual NxMat34 GetPose() const;
	virtual void GetAABB(NxBounds3& aabb) const;

	virtual void SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef = true);
	virtual void SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup);

	//------------------------------------------------------------------------------
	// some utility functions...

	// user should fill in the bufferPos, bufferVel and bufferLife of addedParticleData
	// and keep the pointer of them untill these function returns...
	void SpawnParticles(const NxParticleData& addedParticleData)
	{
		if(m_pNxFluid)
		{
			m_pNxFluid->addParticles(addedParticleData);
		}
	}

	void SpawnParticles(int iParticleNum, const NxVec3* bufferPos, const NxVec3& vVelocity = NxVec3(0.0f), float fLifetime = 3.0f);

	virtual void EnableDebugRender(bool bEnable = true);

	float GetCollisionRadius() const;

	//------------------------------------------------------------------------------
	// interface query functions...
	virtual APhysXOrientedParticleObject* GetOrientedParticleObject() { return NULL; }


protected:

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

	virtual void OnReleaseBasicPhysXStuff();

	virtual void AllocateParticleWriteData(int iMaxParticleNum, APhysXU32 apxPWDFlag);
	virtual void ReleaseParticleWriteData();

protected:

	NxFluid* m_pNxFluid;										// NxFluid object

	//-----------------------------------------------------------------
	// buffer for rendering or other user usage
	
	// these buffer data is necessary
	APhysXU32 m_uParticleNum;
	NxVec3* m_BufferPos;

	// the following data is optional
	NxVec3* m_BufferVel;
	
	float* m_BufferLife;
	float* m_BufferDensity;

	APhysXU32* m_BufferId;
	APhysXU32* m_BufferFlag;

	NxVec3* m_BufferCollisionNormal;
	
	//-----------------------------------------------------------------
	// note:
	//----------------
	// at present, we do not consider the following user-defined data buffers:
	//
	//		particleDeletionIdWriteData
	//		particleCreationIdWriteData
	//		fluidPacketData
	//
	// add them if having relative requirements...
	//----------------
	// Nov. 19, 2008
	// for the case of tracking particles, we add particleDeletionIdWriteData
	// and particleCreationIdWriteData buffers...

	APhysXU32 m_uCreationIdNum;
	APhysXU32 m_uDeletionIdNum;
	APhysXU32* m_BufferCreationId;
	APhysXU32* m_BufferDeletionId;

	//-----------------------------------------------------------------

	
};

//-----------------------------------------------------------------------------------

class APHYSX_DLL_EXPORT APhysXOrientedParticleObject: public APhysXFluidObject
{
	// friend class
	friend class APhysXScene;

protected:

	APhysXOrientedParticleObject()
	{
		m_BufferOrientation = NULL;

		m_BufferAngularVel = NULL;
	}

public:

	// note: different from GetParticlePositions(), GetParticleVelocities()...
	// these data are accessed via ID.
	const NxQuat* GetParticleOrientationsByID() const { return m_BufferOrientation; }

	virtual APhysXOrientedParticleObject* GetOrientedParticleObject() { return this; }

protected:

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

	virtual void AllocateParticleWriteData(int iMaxParticleNum, APhysXU32 apxPWDFlag);
	virtual void ReleaseParticleWriteData();

	virtual void PostSimulate(float dt);

protected:

	// note: these data are accessed by ID not by i...
	NxQuat* m_BufferOrientation;

	APhysXOrientedParticleSpinType m_SpinType;
	AphysXOrientedParticleInitialOrt m_InitialOrt;
	float m_fMaxAngularVel;		
	float m_fSpinRadius;

	NxVec3* m_BufferAngularVel;			// the angular velocity buffer for each particle...
};

#endif