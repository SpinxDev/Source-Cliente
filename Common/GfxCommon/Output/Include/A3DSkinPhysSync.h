/*
 * FILE: A3DSkinPhysSync.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/17
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSKINPHYSSYNC_H_
#define _A3DSKINPHYSSYNC_H_

#include <AString.h>
#include <AArray.h>
#include <A3DMatrix.h>

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
class A3DSkin;
class A3DSkinPhysSyncData;
class APhysXScene;
class APhysXClothAttacher;
class APhysXRigidBodyObject;
class APhysXClothAttacherDesc;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSkinPhysSync
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinPhysSync
{
public:		//	Types

	//	RB actor link info
	struct RBACTOR_LINK
	{
		int		iHookHHIdx;		//	hh-type hook's index
		bool	bHHIsBone;		//	hh-type hook is a bone
	};

	friend class A3DModelPhysics;

public:		//	Constructor and Destructor

	A3DSkinPhysSync();
	virtual ~A3DSkinPhysSync();

public:		//	Attributes

public:		//	Operations

	//	Bind physical data with specified skin
	bool Bind(APhysXScene* pPhysScene, A3DSkinModel* pSkinModel, A3DSkin* pSkin, const A3DVECTOR3& vRefPos);
	//	Un-bind physical data
	void Unbind(bool bDelCoreData=false);

	//	Update pose of RB actors using soft-link. Call this before physics simulation
	bool UpdateRBActorPose(A3DSkinModel* pSkinModel);

	// Update the skin by animation. This is to constrain Cloth simulation. Call this before physics simulation.
	bool UpdateSkinByAnimation(A3DSkinModel* pSkinModel);

	//	Sync skin mesh from physical data. Call this after physics simulation
	bool SyncSkinPhys(A3DSkin* pSkin);

	//	Check if some skin has been bound ?
	bool IsSkinBound() const { return m_pPhysSkin ? true : false; }

	//	Get sync data
	A3DSkinPhysSyncData* GetSyncData() const { return m_pCoreData; }
	//	Get cloth attacher object
	APhysXClothAttacher* GetClothAttacher() const { return m_pPhysSkin; }

	void SetClothWindAcceleration(const A3DVECTOR3& vWindAcceleration);

protected:	//	Attributes

	A3DSkinPhysSyncData*		m_pCoreData;		//	Sync data
	APhysXScene*				m_pPhysScene;		//	Physical scene object
	APhysXClothAttacher*		m_pPhysSkin;		//	Skin's physical representation
	APhysXClothAttacherDesc*	m_pScaleClothDesc;	//	Scaled cloth's physical description
	A3DSkin*					m_pA3DSkin;			//	Bound skin object

	AArray<RBACTOR_LINK>		m_aRBActorLinks;	//	RB actor link info

protected:	//	Operations

	//	Get specified RB actor
	APhysXRigidBodyObject* GetRBActor(int iActorIdx);
	//	Check if mesh needs to be scaled before it changes to cloth
	bool CheckMeshScaled(A3DSkinModel* pSkinModel);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DSKINPHYSSYNC_H_


