/*
 * FILE: A3DModelPhysSync.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/5/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DMODELPHYSSYNC_H_
#define _A3DMODELPHYSSYNC_H_

#include <A3DMatrix.h>
#include <AArray.h>
#include <AString.h>
#include <vector.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinModel;
class APhysXSkeletonRBObject;
class A3DModelPhysSyncData;
class APhysXScene;
class APhysXRBAttacher;
class NxActor;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DModelPhysSync
//	
///////////////////////////////////////////////////////////////////////////

struct PhysBreakInfo
{
	A3DVECTOR3		mBreakPos;
	float			mBreakMagnitude;
};


class A3DModelPhysSync
{
public:		//	Types

	//	Physical state
	enum
	{
		PHY_STATE_SIMULATE = 0,			//	Pure physical simulation
		PHY_STATE_ANIMATION,			//	Pure model animation
		PHY_STATE_PARTSIMULATE,			//	Partially physical simulation
		NUM_PHY_STATE,
	};

	//	Actor implement
	struct ACTOR_IMP
	{
		NxActor*	pActor;				//	Actor object

		//	Operations
		ACTOR_IMP();
	};

	//	Absolute matrices of bones calculated by physical system
	struct BONE_MAT
	{
		A3DMATRIX4	matBoneAbs;			//	Bone's absolute matrix
		A3DMATRIX4	matBoneInvAbs;		//	Bone's inverse absolute matrix
		bool		bMainBone;			//	Main bone flag

		//	Operations
		BONE_MAT();
	};

	//	Attacher link info
	struct ATTACHER_LINK
	{
		AString		strParentHook;
		int			nParentHookIdx;
		AString		strAttacher;
		int			nAttacherIdx;
	};
	
	//	Bone scale cached data
	struct BONE_SCALE_CACHE_DATA
	{
		BONE_SCALE_CACHE_DATA(A3DBone* pBone);
		float fLocalLengthScale;
		float fLocalThickScale;
		float fAccuWholeScale;
	};

	friend class A3DModelPhysics;
	
public:		//	Constructor and Destructor

	//	A3DModelPhysSync must be created by A3DModelPhysics's instance.
	//	A3DModelPhysSync also need the A3DModelPhysics pointer during its lifetime.
	A3DModelPhysSync(A3DModelPhysics* pModelPhysics);
	virtual ~A3DModelPhysSync();

public:		//	Attributes

public:		//	Operations

	//	Update model's animation. Call this before physics simulation
	bool UpdateModelAnim(int iDeltaTime);
	//	Sync model state from physical data. Call this after physics simulation
	bool SyncModelPhys();

	//	Bind skin model with a physical object
	//	bRebuildDesc means release the old attacher desc and re-build a new one (used in editor for applying the changes)
	bool Bind(A3DSkinModel* pSkinModel, A3DModelPhysSyncData* pCoreData, APhysXScene* pPhysScene, bool bRebuildDesc = false);
	//	After model is teleport to a new position, this function should be could to sync physical data
	bool Teleport();

	//	Check if physical data bound successfully
	bool GetPhysBindFlag() const { return m_bPhysBindOk; }
	//	Change physical state
	bool ChangePhysState(int iState);
	//	Get current physical state
	int GetPhysState() const { return m_iPhysState; }
	//	Get RB object mass
	float GetModelMass() const;

	//	Open physical link
	bool OpenPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher);
	//	Close physical link
	void ClosePhysLinkAsChildModel();
	//	Add physical link
	bool AddPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher);
	//	Change physical link
	bool ChangePhysLinkAsChildModel(const char* szChildAttacher, const char* szNewHook);
	//	Remove physical link
	bool RemovePhysLinkAsChildModel(const char* szChildAttacher);
	//	Get attacher
	ATTACHER_LINK* GetPhysLinkAsChildModel(const char* szChildAttacher, int* pIndex = NULL) const;
	

	//	Get model physical object
	APhysXSkeletonRBObject* GetSkeletonRBObject() const { return m_pPhysModel; }

	//	Break Joint
	void BreakJoint(const char* szJoint);
	//	Break Joint by bone name
	void BreakJointByBone(const char* szBoneName);
	
	//	Apply force
	bool AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist, int iPhysForceType, float fSweepRadius = 0.0f);

	//----------------------------------------------------------------------------------
	// test code for breakable...
	void SetBoneAnimDriven(bool bAnimDriven = true);
	
	void OnBreak(const PhysBreakInfo& physBreakInfo);				// called by the APhysXEngine's Break Notify...
	void OnFirstBreak();		// called by the APhysXEngine's first Break Notify...
	void OnAllBreak();			// called by the APhysXEngine's all Break Notify...


	// get the reference of A3DModelPhysics...
	A3DModelPhysics* GetA3DModelPhysics() { return m_pModelPhysics; } 
	APhysXRBAttacher* GetAPhysxRBAttacher() { return m_pPhysAttacher; } 

protected:	//	Attributes

	APhysXScene*			m_pPhysScene;		//	Physical scene object
	A3DSkinModel*			m_pSkinModel;		//	Skin model
	APhysXSkeletonRBObject*	m_pPhysModel;		//	Physical object
	APhysXRBAttacher*		m_pPhysAttacher;	//	Physical attacher
	A3DModelPhysSyncData*	m_pCoreData;		//	Core data
	A3DModelPhysics*		m_pModelPhysics;	//	Model physics;

	bool					m_bPhysBindOk;		//	true, physical data bound successfully
	int						m_iPhysState;		//	Physical state
	bool					m_bPhysLinkOpen;	//	true, physical link opened
	bool					m_bPhysBroken;		//	true, physical skeleton object is broken now(first break called)

	APtrArray<ACTOR_IMP*>	m_aActorImps;		//	Actor implements
	APtrArray<ATTACHER_LINK*> m_aAttacherLinks;	//	Attacher link info
	AArray<BONE_MAT>		m_aBoneMats;		//	Absolute matrices of bones calculated by physical system
	APtrArray<BONE_SCALE_CACHE_DATA*>	m_aBoneScaleCache;	//	Bone Scale cache

protected:	//	Operations

	//	Check whether the physical link is already exist
	bool CheckPhysLinkExist(const char* szParentHook, const char* szChildAttacher);
	//	Check whether the skeleton is scaled
	bool CheckSkeletonScaled(A3DSkeleton* pSkeleton);
	//	Create physical model (attacher obj, and get the model from the attacher obj)
	bool CreatePhysAttacherAndModel(A3DSkinModel* pSkinModel, bool bRebuildDesc);
	//	Release physical attacher
	void ReleasePhysAttacher();
	//	Release all actor implements
	void ReleaseActorImps();
	//	Release all attacher link infos
	void ReleaseAttacherLinkInfo();
	//	Build actor implements
	bool BuildActorImps();
	//	Build attacher link infos
	bool BuildAttacherLinkInfos(const char* szParentHook, const char* szChildAttacher);
	//	Update actor state by animation
	void UpdateActorsByAnim(bool bTeleport);
	//	Update attachers state by parent's hooks position
	void UpdateAttachersByParentAndLinkInfo();
	//	Update physically driven bones' state
	void UpdateBonesByPhys();
	void UpdateBonesByPhys_r(const A3DMATRIX4& matInvModel, int iBoneIdx);
	//	Find actor through specified bone or hh-type hook
	NxActor* GetActorByHook(int iHookIndex, bool bHHIsBone, A3DMATRIX4& matHook);
	
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DMODELPHYSSYNC_H_
