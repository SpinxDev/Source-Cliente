/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXFORCEFIELDOBJECT_H_
#define _APHYSXFORCEFIELDOBJECT_H_

class APHYSX_DLL_EXPORT APhysXForceFieldObject: public APhysXObject
{
	// friend class
	friend class APhysXScene;

protected:

	APhysXForceFieldObject()
	{
		m_pNxForceField = NULL;
		m_pNxForceFieldKernel = NULL;

		m_ffTypeRB = m_ffTypeCloth = m_ffTypeFluid = m_ffTypeSB = NX_FF_TYPE_OTHER;

		m_ffVariety = 0;

	}

public:
	
	//------------------------------------------------------------------------------
	// to retrieve the PhysX objects...

	NxForceField* GetNxForceField() { return m_pNxForceField; }
	NxForceFieldKernel* GetNxForceFieldKernel() { return m_pNxForceFieldKernel; } 

	NxActor* GetAttachedNxActor()
	{
		if(m_pNxForceField)
			return m_pNxForceField->getActor();
		else
			return NULL;

	}


	//------------------------------------------------------------------------------
	// common interfaces of PhysX objects...
	//
	// note here:
	//		NxForceField objects have no Sleep/WakeUp functions...
	//
	virtual void PutToSleep();

	virtual void WakeUp();

	virtual bool IsSleeping();

	virtual void SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef = true);
	virtual void SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup);

	//------------------------------------------------------------------------------
	// current implementation:
	// only set/get the pose relative to the world frame or attached actor frame...
	virtual void SetPose(const NxMat34& globalPose);
	virtual NxMat34 GetPose() const;
	//------------------------------------------------------------------------------

	void SetForceScale(float fRBForceScale, float fClothForceScale, float fFluidForceScale, float fSBForceScale);
	void SetForceScale(float fForceScale)
	{
		SetForceScale(fForceScale, fForceScale, fForceScale, fForceScale);
	}

	

protected:

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

	virtual void OnReleaseBasicPhysXStuff();

protected:

	// PhysX stuff...
	NxForceField* m_pNxForceField;
	
	// PhysX FF kernel, we use kernel instead of linear kernel so as to support custom kernel...
	NxForceFieldKernel* m_pNxForceFieldKernel;
	// NxForceFieldLinearKernel* m_pNxForceFieldLinearKernel;

	
	
	// if we want to scale the force, we have to create a special NxForceFieldVariety
	// for this force field...
	NxForceFieldVariety m_ffVariety;		


	// these members should be initialized on the creation of the NxForceField
	NxForceFieldType m_ffTypeRB;
	NxForceFieldType m_ffTypeCloth;
	NxForceFieldType m_ffTypeFluid;
	NxForceFieldType m_ffTypeSB;
};


//---------------------------------------------------------------------------------
// a special class for wind force field because we should override the PreSimulate()
// function to simulate the wind effect
class APHYSX_DLL_EXPORT APhysXWindForceFieldObject: public APhysXForceFieldObject
{
	// friend class
	friend class APhysXScene;

	enum
	{
		APX_FF_WIND_STATE_WIND = 0,
		APX_FF_WIND_STATE_REST,

		APX_FF_WIND_STATE_NUM,
	};

public:

	virtual void PreSimulate(float dt);

protected:

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);
	virtual void OnReleaseBasicPhysXStuff();

protected:

	float m_fRecipCycleTime;			
	float m_fRecipCycleDist;
	float m_fElapsedTimeInCurState;

	int m_iCurState;
	float m_StateTime[APX_FF_WIND_STATE_NUM];

	NxVec3 m_vWindForceConstant;
	NxVec3 m_vWindForceNoise;

	
};

//---------------------------------------------------------------------------------
// boss momentum effect force field
// for the best effect: the FF's binding height(h) : kernel's radius(r) approximately equal(¡Ö) 3 : 5
//     FF center
//         |\ 
//       h |  \  r
//         |    \
//         |      \
//         ---------
//           terrain
//   
class NxForceFieldKernelBossAura;
class APHYSX_DLL_EXPORT APhysXBossForceFieldObject: public APhysXForceFieldObject
{
	// friend class
	friend class APhysXScene;

public:
	bool GetRestState() const;
	void SetRestState(bool rest);

	bool GetEnhanceMode() const;
	void SetEnhanceMode(bool enhance);

	float GetForceValue() const;
	void  SetForceValue(float force);

	float GetRadius() const;
	void  SetRadius(float r);

protected:

	NxForceFieldKernelBossAura* GetOwnKernel() const;

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);
	virtual void OnReleaseBasicPhysXStuff();

};
#endif