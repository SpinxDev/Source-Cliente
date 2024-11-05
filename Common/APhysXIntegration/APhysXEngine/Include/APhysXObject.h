/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

	Histroy:

		revised on Apr.21, 2008, by wenfeng
			add a data member m_szName so that we can assign a name for physics objects

----------------------------------------------------------------------*/

#ifndef _APHYSXOBJECT_H_
#define _APHYSXOBJECT_H_

class APhysXObjectConnector;

// Abstract class
class APHYSX_DLL_EXPORT APhysXObject
{
	// just use GetType() instead
	//DeclareObjType(APX_OBJTYPE_ABSTRACT_RB);
	friend class APhysXScene;
	friend class APhysXCompoundObject;

public:

	virtual ~APhysXObject() {}

	//-------------------------------------------------------
	// make the following function non-virtual but inline!
	bool IsRigidBody() const
	{
		return IsRigidBodyType(m_apxObjectType);
	}

	bool IsStaticRB() const
	{
		return IsStaticRBType(m_apxObjectType);
	}

	bool IsDynamicRB() const
	{
		return IsDynamicRBType(m_apxObjectType);
	}


	bool IsCloth() const
	{
		return IsClothType(m_apxObjectType);
	}

	bool IsFluid() const
	{
		return IsFluidType(m_apxObjectType);
	}

	bool IsForceField() const
	{
		return IsForceFieldType(m_apxObjectType);
	}

	bool IsCompound() const
	{
		return	IsCompoundType(m_apxObjectType);
	}

	bool IsAttacher() const
	{
		return IsAttacherType(m_apxObjectType);
	}

	bool IsObjectType(APhysXObjectType apxObjType) const
	{
		return m_apxObjectType == apxObjType;
	}
	//-------------------------------------------------------

	// Serialization/Deserialization
	// to do: what's the parameter?....
	virtual void SerializePhysX() {}
	virtual void DeserializePhysX() {}

	// Sleep/Wakeup
	virtual void PutToSleep()= 0;
	virtual void WakeUp() = 0;
	virtual bool IsSleeping() = 0;

	// Graphics <---> Physics

	// update the graphics data from physics
	virtual void SyncGraphicsData()			// Graphics --> Physics
	{
		if(m_pUserGraphicsSync)
			m_pUserGraphicsSync->SyncGraphicsData(this);
	}
	
	// update the physics data from graphics
	virtual void SyncPhysicsData()			// Physics  --> Graphics
	{
		if(m_pUserGraphicsSync)
			m_pUserGraphicsSync->SyncPhysicsData(this);
	}

	// Change or set UserGraphicsSync at runtime...
	void SetUserGraphicsSync(APhysXUserGraphicsSync* pUserGraphicsSync)
	{
		m_pUserGraphicsSync = pUserGraphicsSync;
	}

	// virtual void Tick(float dt) {};

	// Network sync
	virtual void SyncNetworkData(){}					// = 0;
	virtual void ReplicateNetworkData(){}				// = 0;


	virtual void SetPose(const NxMat34& globalPose) = 0;
	virtual NxMat34 GetPose() const = 0;
	virtual void GetAABB(NxBounds3& aabb) const
	{
		aabb.setEmpty();
	}

	//----------------------------------------------------------------------------------------
	// change the collision channel runtime...

	// only as to the child objects of APhysXCompoundObject, we set channel with the "bChangeChannelRef" to false...
	virtual void SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef = true);
	virtual APhysXCollisionChannel GetCollisionChannel() const
	{
		return m_CollisionChannel;
	}

	virtual void EnableCollision(bool bEnable = true) {}

	// Get/Set interfaces of collision group
	// Note: commonly, we can not change collision group at runtime!
	// but we provide the "SetCollisionGroup" interface for some specific requirements...
	APhysXCollisionGroup GetCollisionGroup() const { return m_CollisionGroup; }
	virtual void SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup)
	{
		m_CollisionGroup = apxCollisionGroup;
	}


	APhysXScene* GetPhysXScene() { return m_pAPhysXScene; }
	const APhysXScene* GetPhysXScene() const { return m_pAPhysXScene; }

	APhysXObjectType GetType() { return m_apxObjectType; }

	void SetName(const char* szName)
	{
		m_szName = gGlobalStringManager->GetString(szName);
	}

	const char* GetName()
	{
		return m_szName;
	}

	APhysXObjectConnector* GetInnerConnector (const char* szName) const;
	void RemoveInnerConnector(APhysXObjectConnector* pPhysXObjConnector);
	// release all inner connectors
	void ReleaseInnerConnectors();

	void RemoveOuterConnector(APhysXObjectConnector* pPhysXObjConnector);
	// release all outer connectors
	void ReleaseOuterConnectors();

	void AddOuterConnector(APhysXObjectConnector* pPhysXObjConnector)
	{
		m_arrOuterConnectors.push_back(pPhysXObjConnector);
	}

	void SetLifeSpan(float fLifeSpan) { m_fLifeSpan = fLifeSpan; }
	float GetLifespan() { return m_fLifeSpan; }
	
	float GetCurrentLife() { return m_fCurLife; }

	APhysXObject* GetParentObject() { return m_pParentObject; }

	virtual void SetObjectFlag(APhysXObjectFlag flag, bool val);
	virtual bool GetObjectFlag(APhysXObjectFlag flag) const;

	// for objects which are released in Async way:
	// true only at the time after one object is released in Async way, but before the real release is executed, 
	virtual bool IsDeleted() const { return m_bIsDeleted; }		

	// enable/disable the object to be visualized by DebugRender function
	virtual void EnableDebugRender(bool bEnable = true) {}

public:
	
	// public data member: to keep the pointer of user data...
	void* mUserData;		

protected:

	// called by APhysXScene...
	virtual bool OnCreate(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

	virtual void OnRelease();

	// called internally...
	// before we create the basic Physics stuff, we can config some parameters here???
	virtual bool PreCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
	{
		return true;
	}
	// create the basic physics stuff, such as rb NxActors, NxCloths, NxFluid...
	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);
	
	// after we create the basic physics stuff...
	// create some other stuff dependent on the above creation, such as NxJoints...
	// and set some status of the key physics stuff here as well
	virtual bool PostCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

	// before we release the basic Physics stuff, i.e., we can release NxJoints here...
	virtual void PreReleaseBasicPhysXStuff();

	// release the basic Physics stuff here
	virtual void OnReleaseBasicPhysXStuff() {}

	// after we release the basic physics stuff...
	virtual void PostReleaseBasicPhysXStuff();

	// called before we simulate the physics...
	// override this to set/change some physics states before the physics simulation
	virtual void PreSimulate(float dt);

	// called after we simulate the physics but before the update of the graphics data
	// override this to get the physics states and handle my own tick after the physics simulation
	virtual void PostSimulate(float dt);

	virtual void Expire();

	virtual void Delete()		// call this to raise the flag of IsDeleted
	{
		m_bIsDeleted = true;
	}

protected:
	
	APhysXScene* m_pAPhysXScene;
	
	APhysXUserGraphicsSync* m_pUserGraphicsSync;

	// this is for case of compound objects and each component object 
	// of compound object has this member set to that compound one. 
	APhysXObject* m_pParentObject;

	bool m_bNetworkSync;

	APhysXObjectType m_apxObjectType;

	const char* m_szName;

	// connector (joints)...
	APhysXArray<APhysXObjectConnector* > m_arrInnerConnectors;
	APhysXArray<APhysXObjectConnector* > m_arrOuterConnectors;

	float m_fLifeSpan;
	float m_fCurLife;

	// be ware of difference between m_CollisionGroup and m_CollisionChannel...
	APhysXCollisionChannel m_CollisionChannel;

	APhysXCollisionGroup m_CollisionGroup; 

	// keep the member of corresponding object desc
	APhysXObjectDesc* m_pMyObjectDesc;
	
	APhysXU32 m_apxObjectFlags;

	bool m_bIsDeleted;
	
};

#endif