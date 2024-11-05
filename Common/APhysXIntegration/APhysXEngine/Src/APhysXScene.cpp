
#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerMgrImp.h"


//-------------------------------------------------------------------------------

#if APHYSX_ENABLE_PROFILING

#if 0
	
	#define APHYSX_BEGIN_PROFILING(name)	{ m_PerfStats.StartPerfStat(name); }
	#define APHYSX_END_PROFILING(name)			{ m_PerfStats.EndPerfStat(); }

#else
	// try to use these two macros instead of the above two macros since the following 
	// functions can accumulate the elapsed time more than one time each frame.

	#define APHYSX_BEGIN_PROFILING(name)	{ m_PerfStats.ResumePerfStat(name); }
	#define APHYSX_END_PROFILING(name)	{ m_PerfStats.PausePerfStat(name); }

#endif


#else

	#define APHYSX_BEGIN_PROFILING(name)
	#define APHYSX_END_PROFILING(name)

#endif

//-------------------------------------------------------------------------------
// handle AddForce() function in case of sub simulation

static void ExecuteAddForce(APhysXForceInfo apxForceInfo)
{
	if(apxForceInfo.mActor && apxForceInfo.mActor->isDynamic() && !apxForceInfo.mActor->readBodyFlag(NX_BF_KINEMATIC))
	{
		NxActor* pActor = apxForceInfo.mActor;

		if(apxForceInfo.mForceFlag & APX_FF_LOCALFORCE)
		{
			if(apxForceInfo.mForceFlag & APX_FF_ATPOS)
			{
				if(apxForceInfo.mForceFlag & APX_FF_LOCALPOS)
					pActor->addLocalForceAtLocalPos(apxForceInfo.mForce, apxForceInfo.mPos, apxForceInfo.mForceMode);
				else
					pActor->addLocalForceAtPos(apxForceInfo.mForce, apxForceInfo.mPos, apxForceInfo.mForceMode);
			}
			else
				pActor->addLocalForce(apxForceInfo.mForce, apxForceInfo.mForceMode);
		}
		else
		{
			if(apxForceInfo.mForceFlag & APX_FF_ATPOS)
			{
				if(apxForceInfo.mForceFlag & APX_FF_LOCALPOS)
					pActor->addForceAtLocalPos(apxForceInfo.mForce, apxForceInfo.mPos, apxForceInfo.mForceMode);
				else
					pActor->addForceAtPos(apxForceInfo.mForce, apxForceInfo.mPos, apxForceInfo.mForceMode);
			}
			else
				pActor->addForce(apxForceInfo.mForce, apxForceInfo.mForceMode);
		}


	}

}


class APhysXAddForceSubSimulationReport : public APhysXUserSubSimulationReport, public APhysXArray<APhysXForceInfo>
{

public:

	APhysXAddForceSubSimulationReport()
	{
		// register myself to gPhysXSubSimulationReport 
		gPhysXSubSimulationReport->Register(this);
	}

	virtual void PreSubSimulation(float dt)
	{
		// float fInvDt = (dt<1e-4)? 0.0f : 1.0f/dt;

		for(int i=0; i<size(); ++i)
		{
			APhysXForceInfo afi = (*this)[i];
			if(!afi.mActor || afi.mForceFlag & APX_FF_INVALID_ACTOR) continue;

			APhysXObject* pObj = (APhysXObject *)(afi.mActor->userData);
			APhysXScene* pScene = pObj->GetPhysXScene();

			if(	afi.mForceMode == NX_IMPULSE ||
				afi.mForceMode == NX_SMOOTH_IMPULSE ||	
				afi.mForceMode == NX_VELOCITY_CHANGE ||
				afi.mForceMode == NX_SMOOTH_VELOCITY_CHANGE 
				)
			{
				if(pScene->GetCurFrameDt() >0 )
					afi.mForce *= pScene->GetSimulateTimeStep() / pScene->GetCurFrameDt();

			}

			pScene->LockWritingScene();

			ExecuteAddForce(afi);

			pScene->UnlockWritingScene();

		}
	}

	virtual void OnObjectDeletion(APhysXObject* pObject)
	{
		if(pObject->IsRigidBody())
		{
			APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject* )pObject;
			for(int i=0; i<size(); ++i)
			{
				APhysXForceInfo& afi = (*this)[i];
				if(pRBObj->HasNxActor(afi.mActor))
					afi.mForceFlag |= APX_FF_INVALID_ACTOR;
			}

		}
	}

};

APhysXAddForceSubSimulationReport g_AddForceSubSimulationReport;


//-------------------------------------------------------------------------------
// handle AddForce() function in case of sub-simulation

struct APhysXSetActorPoseSubSimInfo
{
	APhysXSetActorPoseSubSimInfo()
	{
		mNxActor = NULL;
		mSubSimCurPos.zero();
		mSubSimTargetPos.zero();
		mSubSimCurRot.id();
		mSubSimTargetRot.id();
		mAccumulatedSubSimDt = 0.0f;
		mInvalid = false;
	}


	NxActor* mNxActor;
	NxVec3 mSubSimCurPos;			// Current position used in SetPose() during sub-simulation
	NxVec3 mSubSimTargetPos;		// target position used in SetPose() during sub-simulation
	NxQuat mSubSimCurRot;			// Cur rotation used SetPose() during sub-simulation
	NxQuat mSubSimTargetRot;		// target rotation used SetPose() during sub-simulation
	float mAccumulatedSubSimDt;
	bool mInvalid;					// whether this info is invalid or valid
};

class APhysXSetActorPoseSubSimulationReport : public APhysXUserSubSimulationReport, public APhysXArray<APhysXSetActorPoseSubSimInfo>
{

public:

	APhysXSetActorPoseSubSimulationReport()
	{
		// register myself to gPhysXSubSimulationReport 
		gPhysXSubSimulationReport->Register(this);
	}

	virtual void PreSubSimulation(float dt)
	{

		for(int i=0; i<size(); ++i)
		{
			APhysXSetActorPoseSubSimInfo& asi = (*this)[i];
			if(asi.mInvalid || !asi.mNxActor) continue;

			APhysXObject* pObj = (APhysXObject *)(asi.mNxActor->userData);
			APhysXScene* pScene = pObj->GetPhysXScene();

			// compute the real pose in current sub-simulation frame
			if(pScene->GetCurFrameDt() < 1e-5f)
				return;

			asi.mAccumulatedSubSimDt += dt;
			float t = asi.mAccumulatedSubSimDt / pScene->GetCurFrameDt();
			if(t > 1.0f) t = 1.0f;

			NxVec3 vCurPos = APhysXUtilLib::Lerp(asi.mSubSimCurPos, asi.mSubSimTargetPos, t);
			NxQuat qCurRot;
			qCurRot.slerp(t, asi.mSubSimCurRot, asi.mSubSimTargetRot);

			NxMat34 mtPose;
			mtPose.M.fromQuat(qCurRot);
			mtPose.t = vCurPos;

			pScene->ExeSetActorPose(asi.mNxActor, mtPose);

		}
	}

	virtual void OnObjectDeletion(APhysXObject* pObject)
	{
		if(pObject->IsRigidBody())
		{
			APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject* )pObject;

			for(int i=0; i<size(); ++i)
			{
				APhysXSetActorPoseSubSimInfo& asi = (*this)[i];
				
				if(pRBObj->HasNxActor(asi.mNxActor))
					asi.mInvalid = true;
			}
		}
	}
};

APhysXSetActorPoseSubSimulationReport g_SetActorPoseSubSimulationReport;


extern APhysXRBSubSimulationReport g_RBSubSimulationReport;

//-------------------------------------------------------------------------------

bool APhysXScene::OnCreate(APhysXEngine* pAPhysXEngine, const APhysXSceneDesc& apxSceneDesc)
{
	
	bool bCreateMyGroundPlane = (apxSceneDesc.mNxSceneDesc.groundPlane == NX_TRUE); 

	{
		//-------------------------------------------------------------------------
		// make some forcible change to mNxSceneDes

		// 1. do not use the default plane created by NxScene...
		NxSceneDesc& sceneDesc = (NxSceneDesc&)apxSceneDesc.mNxSceneDesc;
		sceneDesc.groundPlane = false;
	
		// 2. set the flag NX_SF_ENABLE_ACTIVETRANSFORMS to enable NxScene::getActiveTransforms()
		sceneDesc.flags |= NX_SF_ENABLE_ACTIVETRANSFORMS;
		
		// 3. turn on this flag so that HW cloths will behave better?... the result is not good...
		// sceneDesc.flags |= NX_SF_SEQUENTIAL_PRIMARY;

		// 4. try to create the major scene in HW mode...
		//if(gPhysXEngine->HasPhysXHardware() && gPhysXEngine->IsHardwarePhysXEnabled())
		//{
		//	sceneDesc.simType = NX_SIMULATION_HW;
		//}
	}

	m_MutexNxSceneWriting.Lock();
	NxScene* pNxScene = pAPhysXEngine->GetNxPhysicsSDK()->createScene(apxSceneDesc.mNxSceneDesc);
	m_MutexNxSceneWriting.Unlock();

	if (!pNxScene)
	{
		gPhysXLog->Log("Warning: Failed on creating NxScene: create a Null NxScene.");
		return false;
	}

	m_pNxScene = pNxScene;
	m_pNxScene->userData = this;

	// Note: Dec. 15, Wenfeng
	// Since there are some issues as for APX_SCENE_UGT_ASYNCH case, 
	// we force to use APX_SCENE_UGT_SYNCH mode now...
	// m_SceneUpdateGraphicsType = apxSceneDesc.mUpdateGraphicstype;
	m_SceneUpdateGraphicsType = APX_SCENE_UGT_SYNCH;
	m_DebugRenderer.Init(pAPhysXEngine);
	
	EnableDebugRender(apxSceneDesc.mEnableDebugRender);
	EnablePerfStatsRender(apxSceneDesc.mEnablePerfStatsRender);

	m_pAPhysXEngine = pAPhysXEngine;

	if (!m_pAPhysXEngine->IsPhysXDisbaled())
	{
		// change the default physics material
		m_MutexMaterialDescs.Lock();
		m_arrMaterialDescs.push_back(apxSceneDesc.mDefaultMaterial);
		m_MutexMaterialDescs.Unlock();

		NxMaterial * defaultMaterial = m_pNxScene->getMaterialFromIndex(0); 
		defaultMaterial->setRestitution(apxSceneDesc.mDefaultMaterial.mRestitution);
		defaultMaterial->setStaticFriction(apxSceneDesc.mDefaultMaterial.mStaticFriction);
		defaultMaterial->setDynamicFriction(apxSceneDesc.mDefaultMaterial.mDynamicFriction);		

		if(bCreateMyGroundPlane)
		{
			CreateGroundPlane(apxSceneDesc.mGroundPlaneElevation);
		}

		LockWritingScene();

		//---------------------------------------------------------------------------------------------------------
		// collision group setting...
		// by default, we disable the collision between the attacher objects and other objects...
		
		// as to APX_COLLISION_GROUP_CLOTH_ATTACHER, disable the collision with all other collision group except cloth
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_CLOTH_ATTACHER, APX_COLLISION_GROUP_DEFAULT, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_CLOTH_ATTACHER, APX_COLLISION_GROUP_FLUID, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_CLOTH_ATTACHER, APX_COLLISION_GROUP_SOFTBODY, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_CLOTH_ATTACHER, APX_COLLISION_GROUP_ATTACHER, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_CLOTH_ATTACHER, APX_COLLISION_GROUP_FILTER_OUT_NON_RB, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_CLOTH_ATTACHER, APX_COLLISION_GROUP_FILTER_OUT_NON_DYNAMIC_RB, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_CLOTH_ATTACHER, APX_COLLISION_GROUP_STATIC_RB, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_CLOTH_ATTACHER, APX_COLLISION_GROUP_TERRAIN_RB, false);
		
		// as to APX_COLLISION_GROUP_ATTACHER, disable the collision with all other collision group
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_ATTACHER, APX_COLLISION_GROUP_DEFAULT, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_ATTACHER, APX_COLLISION_GROUP_FLUID, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_ATTACHER, APX_COLLISION_GROUP_CLOTH, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_ATTACHER, APX_COLLISION_GROUP_SOFTBODY, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_ATTACHER, APX_COLLISION_GROUP_FILTER_OUT_NON_RB, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_ATTACHER, APX_COLLISION_GROUP_FILTER_OUT_NON_DYNAMIC_RB, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_ATTACHER, APX_COLLISION_GROUP_STATIC_RB, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_ATTACHER, APX_COLLISION_GROUP_TERRAIN_RB, false);

		// disable the collision between APX_COLLISION_GROUP_FILTER_OUT_NON_RB and non RB group
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_FILTER_OUT_NON_RB, APX_COLLISION_GROUP_CLOTH, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_FILTER_OUT_NON_RB, APX_COLLISION_GROUP_FLUID, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_FILTER_OUT_NON_RB, APX_COLLISION_GROUP_SOFTBODY, false);

		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_FILTER_OUT_NON_DYNAMIC_RB, APX_COLLISION_GROUP_CLOTH, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_FILTER_OUT_NON_DYNAMIC_RB, APX_COLLISION_GROUP_FLUID, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_FILTER_OUT_NON_DYNAMIC_RB, APX_COLLISION_GROUP_SOFTBODY, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_FILTER_OUT_NON_DYNAMIC_RB, APX_COLLISION_GROUP_STATIC_RB, false);
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_FILTER_OUT_NON_DYNAMIC_RB, APX_COLLISION_GROUP_TERRAIN_RB, false);


		// disable the collision between cloth and static RB since there are some bad behavior
		// when cloth pass through the static RB...
		m_pNxScene->setGroupCollisionFlag(APX_COLLISION_GROUP_STATIC_RB, APX_COLLISION_GROUP_CLOTH, false);
		

		// collision channel setting...

		/************************************************************************
			 (G0 op0 K0) op2 (G1 op1 K1) == b 
			
		Here we set:
				
				op0		: or
				op1		: or
				op2		: and

				b		: true
				K0:		: 0
				K1:		: 0
		************************************************************************/
		
		
		m_pNxScene->setFilterOps(NX_FILTEROP_OR, NX_FILTEROP_OR, NX_FILTEROP_AND);
		m_pNxScene->setFilterBool(true);
		NxGroupsMask zeroGroupMask;
		zeroGroupMask.bits0 = zeroGroupMask.bits1 = zeroGroupMask.bits2 = zeroGroupMask.bits3 = 0;
		m_pNxScene->setFilterConstant0(zeroGroupMask);
		m_pNxScene->setFilterConstant1(zeroGroupMask);

		//---------------------------------------------------------------------------------------------------------
		
		// create the force field material...
		m_ffMaterialRB = m_pNxScene->createForceFieldMaterial();
		m_ffMaterialCloth = m_pNxScene->createForceFieldMaterial();
		m_ffMaterialFluid = m_pNxScene->createForceFieldMaterial();
		m_ffMaterialSB = m_pNxScene->createForceFieldMaterial();
		
		// set the default FF materail scales to 1.0f...
		m_pNxScene->setForceFieldScale(0, m_ffMaterialRB, 1.0f);
		m_pNxScene->setForceFieldScale(0, m_ffMaterialCloth, 1.0f);
		m_pNxScene->setForceFieldScale(0, m_ffMaterialFluid, 1.0f);
		m_pNxScene->setForceFieldScale(0, m_ffMaterialSB, 1.0f);


		// set the timestep method...
		SetSimulateTimeStep(apxSceneDesc.mSimulateTimeStepMethod, apxSceneDesc.mTimeStep, apxSceneDesc.mMaxIter);

		m_fCurFrameDt = m_fTimeStep;
		m_uSimFrameCount = 0;

		// set the default user notify and user contact report...
		m_pNxScene->setUserContactReport(gPhysXContactReport);

		m_pNxScene->setUserTriggerReport(gPhysXTriggerReport);

		m_pUserBreakReport = gPhysXBreakReport;

		//------------------------------------------------------------
		// to do: how to customize the flags?
		// currently all actors's actorGroup is 0, and so, we can make all actor contacts not be filtered...
		m_pNxScene->setActorGroupPairFlags(0,0,NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_FORCES);
		// m_pNxScene->setActorGroupPairFlags( 0, 0, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_FORCES);

#if APHYSX_SUPPORT_COMPARTMENT

		// create the compartments...
		NxCompartmentDesc CompartmentDesc;

		// RB compartment always simulate on CPU...
		CompartmentDesc.type = NX_SCT_RIGIDBODY;
		m_pRBCompartment = m_pNxScene->createCompartment(CompartmentDesc);

		// the compartments of cloth and fluid are set to Hardware(PPU/GPU) if it is available...
		if(gPhysXEngine->HasPhysXHardware())
		{
			CompartmentDesc.deviceCode = NX_DC_PPU_AUTO_ASSIGN;
		}
		else
		{
			CompartmentDesc.deviceCode = NX_DC_CPU;
		}
		CompartmentDesc.type = NX_SCT_CLOTH;
		m_pClothCompartment = m_pNxScene->createCompartment(CompartmentDesc);

		CompartmentDesc.type = NX_SCT_FLUID;
		m_pFluidCompartment = m_pNxScene->createCompartment(CompartmentDesc);

#endif

		UnlockWritingScene();
	}

	// user's creation callback
	gPhysXCreationDeletionReport->OnSceneCreationNotify(this);
	return true;
}


//--------------------------------------------------------------------------------
// factory function
APhysXObject* APhysXScene::CreatePhysXObject(APhysXObjectType apxObjectType)
{
	APhysXObject* pPhysXObj = NULL;
	switch (apxObjectType)
	{

	//------------------------------------------------------
	// rigid body objects...
	case APX_OBJTYPE_DUMMY:

		pPhysXObj = new APhysXDummyRBObject;

		break;

	case APX_OBJTYPE_STATIC:

		pPhysXObj = new APhysXStaticRBObject;

		break;

	case APX_OBJTYPE_DYNAMIC:

		pPhysXObj = new APhysXDynamicRBObject;

		break;

	case APX_OBJTYPE_TERRAIN_PATCH:

		pPhysXObj = new APhysXTerrainPatchRBObject;

		break;

	case APX_OBJTYPE_SKELETON:

		pPhysXObj = new APhysXSkeletonRBObject;

		break;

	//------------------------------------------------------
	// breakable objects...
	case APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE:

		pPhysXObj = new APhysXSimpleBreakableSkeletonRBObject;

		break;

	case APX_OBJTYPE_SKELETON_BREAKABLE:

		pPhysXObj = new APhysXBreakableSkeletonRBObject;

		break;

	//------------------------------------------------------
	// cloth objects...
	case APX_OBJTYPE_CLOTH:

		pPhysXObj = new APhysXClothObject;

		break;

	//------------------------------------------------------
	// fluid objects...

	case APX_OBJTYPE_FLUID:

		pPhysXObj = new APhysXFluidObject;

		break;

	case APX_OBJTYPE_FLUID_ORIENTED_PARTICLE:

		pPhysXObj = new APhysXOrientedParticleObject;

		break;

	//------------------------------------------------------
	// forcefield objects...
	case APX_OBJTYPE_FORCEFIELD:
	case APX_OBJTYPE_FORCEFIELD_VACUUM:
	case APX_OBJTYPE_FORCEFIELD_GRAVITATION:
	case APX_OBJTYPE_FORCEFIELD_BUOYANCY:
	case APX_OBJTYPE_FORCEFIELD_EXPLOSION:
	case APX_OBJTYPE_FORCEFIELD_VORTEX:

		pPhysXObj = new APhysXForceFieldObject;

		break;

		// a special class of forcefield...
	case APX_OBJTYPE_FORCEFIELD_WIND:
	
		pPhysXObj = new APhysXWindForceFieldObject;

		break;
		
	case APX_OBJTYPE_FORCEFIELD_BOSS:

		pPhysXObj = new APhysXBossForceFieldObject;

		break;

	//------------------------------------------------------
	// compound objects...
	case APX_OBJTYPE_COMPOUND:

		pPhysXObj = new APhysXCompoundObject;

		break;

	case APX_OBJTYPE_ATTACHER:

		pPhysXObj = new APhysXAttacher;

		break;

	case APX_OBJTYPE_RB_ATTACHER:

		pPhysXObj = new APhysXRBAttacher;

		break;


	case APX_OBJTYPE_CLOTH_ATTACHER:

		pPhysXObj = new APhysXClothAttacher;

		break;
	
	case APX_OBJTYPE_FLUIDEMITTER_ATTACHER:

		pPhysXObj = new APhysXFluidEmitterAttacher;

		break;

	default:

		APHYSX_ASSERT(0);
		break;
	}

	return pPhysXObj;
}

APhysXObject* APhysXScene::CreatePhysXObject(const APhysXObjectInstanceDesc& apxObjInstanceDesc, bool bInstant /* = false */ )
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
		return NULL;

	bool bLoadObjDescByManager = false;
	if(!apxObjInstanceDesc.mPhysXObjectDesc)
	{
		if(apxObjInstanceDesc.mPxdFileName)
		{
			gPhysXEngine->GetObjectDescManager()->LockWriting();
			apxObjInstanceDesc.mPhysXObjectDesc = gPhysXEngine->GetObjectDescManager()->GetPhysXObjectDesc(apxObjInstanceDesc.mPxdFileName, apxObjInstanceDesc.mObjName);
			if(!apxObjInstanceDesc.mPhysXObjectDesc)
			{
				gPhysXEngine->GetObjectDescManager()->UnlockWriting();
				return NULL;
			}
			
			bLoadObjDescByManager = true;
		}
		else
			return NULL;
	}

	APhysXObject* pPhysXObj = CreatePhysXObject(apxObjInstanceDesc.mPhysXObjectDesc->GetObjType());
	APHYSX_ASSERT(pPhysXObj);

	//-----------------------------------------------
	// revised by Wenfeng, Dec. 11, 2012
	// remove the m_MutexNxSceneWriting's Lock()/Unlock() here and 
	// move them to pPhysXObject->OnCreate()
	bool bResult = pPhysXObj->OnCreate(this, apxObjInstanceDesc);
	//-----------------------------------------------

	if(bLoadObjDescByManager)
		gPhysXEngine->GetObjectDescManager()->UnlockWriting();

	if(bResult)
	{
		/*
		m_arrPhysXObjects.push_back(pPhysXObj);

		// push the named object to the map
		if(pPhysXObj->GetName())
		{
			APhysXString strObjName(pPhysXObj->GetName());
			m_mapNamedPhysXObjects.insert(APhysXObjectPair(strObjName, pPhysXObj));
		}
		*/
		
		// set the collision channel to the default one...
		// pPhysXObj->SetCollisionChannel(m_DefaultCollisionChannel);

		m_MutexCachedCreationObjects.Lock();
		m_arrCachedCreationObjects.push_back(pPhysXObj);
		m_MutexCachedCreationObjects.Unlock();

		if(bInstant) FlushCreation();
	}
	else
	{
		delete pPhysXObj;
		pPhysXObj = NULL;
	}

	return pPhysXObj;
}

void APhysXScene::ReleasePhysXObject(APhysXObject* pPhysXObject, bool bInstant /* = false */ )
{
	
/*	
	pPhysXObject->OnRelease();
	
	// delete the record in array and map
	m_arrPhysXObjects.deleteEntry(pPhysXObject);
	m_arrActivePhysXObjects.deleteEntry(pPhysXObject);
	if(pPhysXObject->GetName())
	{
		APhysXString strObjName(pPhysXObject->GetName());

#if 1
		m_mapNamedPhysXObjects.erase(strObjName);
#else
		APhysXObjectMap::iterator it = m_mapNamedPhysXObjects.find(strObjName);
		if(it != m_mapNamedPhysXObjects.end())
		{
			m_mapNamedPhysXObjects.erase(it);
		}
#endif

	}

	//????? Should I delete pPhysXObject here????
	// so how about some references of pPhysXObject outside???
	delete pPhysXObject;
	pPhysXObject = NULL;

*/
	
	// to raise pPhysXObject's IsDeleted flag...
	pPhysXObject->Delete();

	m_MutexCachedDeletionObjects.Lock();
	m_arrCachedDeletionObjects.push_back(pPhysXObject);
	m_MutexCachedDeletionObjects.Unlock();

	if(bInstant) FlushDeletion();
}

APhysXObjectConnector* APhysXScene::CreatePhysXObjectConnector(const APhysXObjectConnectorInstanceDesc& apxConnectorInstanceDesc, bool bInstant /* = false */ )
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
		return NULL;

	APhysXObjectConnectorInstanceDesc myConnectorInstanceDesc = apxConnectorInstanceDesc;
	
	// ignore the global pose and scale data members
	myConnectorInstanceDesc.mGlobalPose.id();
	myConnectorInstanceDesc.mScale3D.set(1.0f);

	
	APhysXObjectConnector* pConnector = APhysXObjectConnector::CreateConnectorPtr(myConnectorInstanceDesc.mPhysXConnectorDesc->GetConnectorType());
	if(pConnector)
	{
		m_MutexNxSceneWriting.Lock();
		bool bResult = pConnector->OnCreateByPhysXScene(this, myConnectorInstanceDesc);
		m_MutexNxSceneWriting.Unlock();

		if(bResult)
		{
			/*
			m_arrPhysXConnectors.push_back(pConnector);
			*/

			m_MutexCachedCreationConnectors.Lock();
			m_arrCachedCreationConnectors.push_back(pConnector);
			m_MutexCachedCreationConnectors.Unlock();

			if(bInstant) FlushCreation();
		}
		else
		{
			delete pConnector;
			pConnector = NULL;

			gPhysXLog->Log("Warning: Failed on creating APhysXObjectConnector.");
		}
	}
	
	return pConnector;

}

void APhysXScene::ReleasePhysXObjectConnnector(APhysXObjectConnector* pPhysXObjectConnector, bool bInstant /* = false */)
{
/*
	pPhysXObjectConnector->OnReleaseByPhysXScene();
	m_arrPhysXConnectors.deleteEntry(pPhysXObjectConnector);
	
	delete pPhysXObjectConnector;
*/

	m_MutexCachedDeletionConnectors.Lock();
	m_arrCachedDeletionConnectors.push_back(pPhysXObjectConnector);
	m_MutexCachedDeletionConnectors.Unlock();

	if(bInstant) FlushDeletion();
}


void APhysXScene::CreateDummySphereActor(const NxVec3& vPos, float fRadius, const NxVec3& vVel)
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
		return;

	NxSphereShapeDesc ssDesc;
	ssDesc.radius = fRadius;
	ssDesc.groupsMask = m_CollisionChannelManager.GetChannelGroupMask(APX_COLLISION_CHANNEL_COMMON);

	NxBodyDesc bodyDesc;
	bodyDesc.linearVelocity = vVel;

	NxActorDesc actorDesc;
	actorDesc.shapes.push_back(&ssDesc);
	actorDesc.globalPose.t = vPos;
	actorDesc.body = &bodyDesc;
	actorDesc.density = 1.0f;

	m_MutexNxSceneWriting.Lock();
	NxActor* pNxActor = m_pNxScene->createActor(actorDesc);
	m_MutexNxSceneWriting.Unlock();

	APHYSX_ASSERT(pNxActor);
	m_arrDummyNxActors.push_back(pNxActor);
}

//--------------------------------------------------------------------------------

void APhysXScene::StartSimulate()
{
	if(m_pNxScene == NULL || m_bOnSimulate) return;

	// first sync physics data from graphics data
	SyncPhysicsData();

	if(m_SceneUpdateGraphicsType == APX_SCENE_UGT_ASYNCH)
	{
		// we should execute one tick firstly
		m_MutexNxSceneWriting.Lock();
		m_pNxScene->simulate(APHYSX_STD_TICKTIME);
		m_pNxScene->flushStream();

	}
	
	m_bOnSimulate = true;

	// reset simulate time to zero...

	m_fSimulateTime = 0.0f;
	m_fSimTimeAccumulator = 0.0f;

	/*
		//test code, to make sure we are now using the PhysX SDK 2.8.1
		NxClothMeshDesc clothMeshDesc;
		clothMeshDesc.flags = NX_CLOTH_MESH_WELD_VERTICES;
	*/
}

void APhysXScene::EndSimulate()
{

	if(m_pNxScene == NULL || !m_bOnSimulate) return;

	if(m_SceneUpdateGraphicsType == APX_SCENE_UGT_ASYNCH)
	{
		// we should execute one tick firstly
		m_pNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
		m_MutexNxSceneWriting.Unlock();

	}

	m_bOnSimulate = false;
}

void APhysXScene::PreObjectsSimulate(float dt)
{
	//--------------------------------------------------------------------------
	// to do... Sept.22, 2008
	// currently, this function calls the PreSimulate() for each PhysX Objects
	// maybe, in the future, we can improve it by keeping a PreSimulate object list
	// to avoid to visit all PhysX objects...
	//--------------------------------------------------------------------------
	
	// first, we tick the cached creation and deletion...
	TickCreationAndDeletion();

	APHYSX_BEGIN_PROFILING("PreObjectsSimulate");


	for(int i=0; i<m_arrPhysXObjects.size(); i++)
	{
		m_arrPhysXObjects[i]->PreSimulate(dt);
	}

	APHYSX_END_PROFILING("PreObjectsSimulate");

}

void APhysXScene::PostObjectsSimulate(float dt)
{
	//--------------------------------------------------------------------------
	// to do... Sept.22, 2008
	// currently, this function calls the PreSimulate() for each PhysX Objects
	// maybe, in the future, we can improve it by keeping a PreSimulate object list
	// to avoid to visit all PhysX objects...
	//--------------------------------------------------------------------------

	APHYSX_BEGIN_PROFILING("PostObjectsSimulate");

	for(int i=0; i<m_arrPhysXObjects.size(); i++)
	{
		m_arrPhysXObjects[i]->PostSimulate(dt);
	}


	// clear all sub-simulation related stuff...
	g_AddForceSubSimulationReport.clear();
	g_SetActorPoseSubSimulationReport.clear();
	g_RBSubSimulationReport.RemoveAll();

	APHYSX_END_PROFILING("PostObjectsSimulate");

}

void APhysXScene::SetSimulateTimeStep(APhysXSimulateTimeStepMethod apxSimulateTimeStepMethod , float fTimeStep, APhysXU32 uMaxIter)
{
	if(!m_pNxScene) return;

	m_SimulateTimeStepMethod = apxSimulateTimeStepMethod;
	m_fTimeStep = fTimeStep;
	m_uMaxIter = uMaxIter;

	LockWritingScene();

	if(m_SimulateTimeStepMethod == APX_SIMULATE_TSM_FIXED)
	{
		m_pNxScene->setTiming(fTimeStep, uMaxIter);
	}
	else if(m_SimulateTimeStepMethod == APX_SIMULATE_TSM_VARIABLE)
	{
		m_pNxScene->setTiming(fTimeStep, uMaxIter, NX_TIMESTEP_VARIABLE);
	}

	UnlockWritingScene();
}

// static float s_fAccumulatedTime = 0.0f;

float APhysXScene::GetExactSimulateTime(float dt, bool bCutOutExtraTime)
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
	{
		m_fExactSimDt = dt;
		return dt;
	}

	if(m_SimulateTimeStepMethod == APX_SIMULATE_TSM_FIXED)
	{
		float fSimTime = 0.0f;
		APhysXU32 uIter = 0;
		m_fSimTimeAccumulator += dt;

		while( m_fSimTimeAccumulator >= m_fTimeStep && uIter < m_uMaxIter)
		{
			m_fSimTimeAccumulator -= m_fTimeStep;
			fSimTime += m_fTimeStep;

			uIter++;
		}

		if(bCutOutExtraTime && uIter >= m_uMaxIter)
		{
			// clear the accumulated time to Zero!
			m_fSimTimeAccumulator = 0.0f;
		}

		m_fExactSimDt = fSimTime;

		return fSimTime;
	}
	else // (m_SimulateTimeStepMethod == APX_SIMULATE_TSM_VARIABLE)
	{
		m_fExactSimDt = dt;
		return dt;
	}

}


void APhysXScene::Simulate(float dt)
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
		return;

	//??? to do: when pausing simulation, should we still count time?
	if( !m_bOnSimulate || m_bPauseSimulate || !m_pNxScene) return;

	// if dt is (approximating to) 0 , we just return...
	if(dt < 0.000001f) return;

	// Performance Stats...
	APHYSX_BEGIN_PROFILING("APXScene_Simulate");


	m_fSimulateTime += dt;
	m_fCurFrameDt = dt;
	m_uSimFrameCount++;
	m_fExactSimDt = dt;

	switch (m_SceneUpdateGraphicsType)
	{
	case APX_SCENE_UGT_SYNCH:

		PreObjectsSimulate(dt);

		APHYSX_BEGIN_PROFILING("NxScene_Simulate");

		// simulate first
		m_MutexNxSceneWriting.Lock();
		
		if(m_SimulateTimeStepMethod == APX_SIMULATE_TSM_FIXED)
		{
			if(!m_bEnableSubSimulation)
			{
				// general simulation routine...
				APHYSX_BEGIN_PROFILING("NxScene_Sim_Once");
				
				m_pNxScene->simulate(dt);
				m_pNxScene->flushStream();
				m_pNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);	
				
				APHYSX_END_PROFILING("NxScene_Sim_Once");
			}
			else
			{
				// sub-simulation routine...

				float fLeftTime = dt;
				APhysXU32 uIter = 0;

				while(fLeftTime > 0.0f && uIter < m_uMaxIter)
				{
					gPhysXSubSimulationReport->PreSubSimulation(m_fTimeStep);

					//-------------------------------------------------------------
					// Note here: we only profile the first sub-simulation time 
					// for reference of simulation once...
					if(uIter == 0) APHYSX_BEGIN_PROFILING("NxScene_Sim_Once");
					//-------------------------------------------------------------

					m_pNxScene->simulate(m_fTimeStep);
					m_pNxScene->flushStream();
					m_pNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);	

					if(uIter == 0) APHYSX_END_PROFILING("NxScene_Sim_Once");

					gPhysXSubSimulationReport->PostSubSimulation(m_fTimeStep);

					fLeftTime -= m_fTimeStep;
					uIter++;
				}

			}



		}
		else if(m_SimulateTimeStepMethod == APX_SIMULATE_TSM_VARIABLE)
		{
			// we will split the dt to several m_fTimeStep here...
			float fLeftTime = dt;
			APhysXU32 uIter = 1;
			while(fLeftTime > m_fTimeStep && uIter < m_uMaxIter)
			{
				m_pNxScene->simulate(m_fTimeStep);
				m_pNxScene->flushStream();
				m_pNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);	

				fLeftTime -= m_fTimeStep;
				uIter++;
			}

			if(fLeftTime > 0.001f)
			{
				m_pNxScene->simulate(fLeftTime);
				m_pNxScene->flushStream();
				m_pNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);	
			}

		}
		

		m_MutexNxSceneWriting.Unlock();

		APHYSX_END_PROFILING("NxScene_Simulate");

		PostObjectsSimulate(dt);

		// render next
		APHYSX_BEGIN_PROFILING("Update_Graphics");

		UpdateActivePhysXObjects();
		SyncGraphicsData();

		APHYSX_END_PROFILING("Update_Graphics");


		// debug render
		//if(m_bEnableDebugRender) DebugRender();

		// then call A3D render routine...

		break;

	case APX_SCENE_UGT_ASYNCH:
		
		// we should be sure that NxScene's simulate() be called before this
		m_pNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
		m_MutexNxSceneWriting.Unlock();

		PostObjectsSimulate(dt);

		// sync physics data to graphics
		UpdateActivePhysXObjects();
		SyncGraphicsData();

		// debug render
		//??? laid here
		//if(m_bEnableDebugRender) DebugRender();

		PreObjectsSimulate(dt);

		// simulate next tick
		m_MutexNxSceneWriting.Lock();

		m_pNxScene->simulate(dt);
		m_pNxScene->flushStream();

		// then call A3D render routine...
		

		break;

	default:
		break;
	};

	APHYSX_END_PROFILING("APXScene_Simulate");

	//---------------------------------------------------------------
	// to clear some perf stat items...
#if 0	
	m_PerfStats.ResumePerfStat("Create RB Obj");
	m_PerfStats.PausePerfStat("Create RB Obj");

	m_PerfStats.ResumePerfStat("Create RB Lock");
	m_PerfStats.PausePerfStat("Create RB Lock");

	m_PerfStats.ResumePerfStat("Create Actor");
	m_PerfStats.PausePerfStat("Create Actor");

	m_PerfStats.ResumePerfStat("Get Shape Desc");
	m_PerfStats.PausePerfStat("Get Shape Desc");

#endif
	//---------------------------------------------------------------

	m_PerfStats.EndFrame();

	if(IsPerfStatsLogEnabled_Print())
	{
		LogPerfStats();
	}

}

// so active PhysX objects are updated at the end of each tick?
void APhysXScene::UpdateActivePhysXObjects()
{
	m_arrActivePhysXObjects.clear();

	// as to rigid body actors, we use active transform notification of NxScene.
    // get the Active Transforms from the scene    
	NxU32 nbTransforms = 0;    
	NxActiveTransform *activeTransforms = m_pNxScene->getActiveTransforms(nbTransforms);

	APhysXObject* pPhysXObj = NULL;
	for(int i=0; i<nbTransforms; i++)
	{
		NxActor* pNxActor = activeTransforms[i].actor;
		pPhysXObj = static_cast<APhysXObject *>(pNxActor->userData);
		
		//-------------------------------------------------------------------------
		// do not ASSERT, since we may create some utility NxActors...
		// APHYSX_ASSERT(pPhysXObj);
		if(pPhysXObj)
		{
			//??? fix me, try to handle the case that one APhysXObject is made up of
			// more than one NxActors...

			APhysXUtilLib::APhysXArray_PushOnce(m_arrActivePhysXObjects, pPhysXObj);
			// m_arrActivePhysXObjects.push_back(pPhysXObj);
			// pPhysXObj->SyncGraphicsData();
		}
	}
	
	// as to cloth and fluid objects, we have to cover all of them to test whether they are asleep or not
	NxCloth* pNxCloth = NULL;	
	NxU32 nbCloths = m_pNxScene->getNbCloths();
	for(int i = 0; i< nbCloths; i++)
	{
		pNxCloth = m_pNxScene->getCloths()[i];
		if(!pNxCloth->isSleeping())
		{
			pPhysXObj = static_cast<APhysXObject *> (pNxCloth->userData);
			
			//APHYSX_ASSERT(pPhysXObj);
			if(pPhysXObj)
			{
				APhysXUtilLib::APhysXArray_PushOnce(m_arrActivePhysXObjects, pPhysXObj);
				// m_arrActivePhysXObjects.push_back(pPhysXObj);
				// pPhysXObj->SyncGraphicsData();
			}
		}

	}

	//??? fluids are always active???
	NxFluid* pNxFluid = NULL;
	NxU32 nbFluids = m_pNxScene->getNbFluids();
	for(int i = 0; i< nbFluids; i++)
	{
		pNxFluid = m_pNxScene->getFluids()[i];
		pPhysXObj = static_cast<APhysXObject *> (pNxFluid->userData);
	
		//APHYSX_ASSERT(pPhysXObj);
		if(pPhysXObj)
		{
			APhysXUtilLib::APhysXArray_PushOnce(m_arrActivePhysXObjects, pPhysXObj);
			// m_arrActivePhysXObjects.push_back(pPhysXObj);
			// pPhysXObj->SyncGraphicsData();
		}
	}
}

void APhysXScene::SyncGraphicsData()
{
	// only need to update active physics objects' graphics data
	for(int i=0; i<m_arrActivePhysXObjects.size(); i++)
	{
		m_arrActivePhysXObjects[i]->SyncGraphicsData();
	}

}

void APhysXScene::SyncPhysicsData()
{
	//??? how about the static assets?
	// must update all objects's initial physics data from graphics...
	for(int i=0; i<m_arrPhysXObjects.size(); i++)
	{
		m_arrPhysXObjects[i]->SyncPhysicsData();
	}

}

void APhysXScene::DebugRender(APhysXDebugRenderDesc* pDesc)
{
	if(m_pNxScene)
	{

		//??? why should I lock the NxSceneWriting mutex for render data?... 
		
		// after some try, the answer is Yes, we must add lock/unlock mechanism to 
		// access the the m_pNxScene->getDebugRenderable() and NxSceneStats in thread
		// safe mode...  Jan. 9, 2009

		// Maybe it is just a bug of PhysX SDK?... Dec. 31, 2008

		m_MutexNxSceneWriting.Lock();

		APHYSX_BEGIN_PROFILING("APhysX_Debug_Render");

		if( m_bEnableDebugRender)
		{
			m_DebugRenderer.RenderData(m_pNxScene->getDebugRenderable());

			int nCount = m_pNxScene->getNbForceFields();
			NxForceField** ppFF = m_pNxScene->getForceFields();
			for (int i = 0; i < nCount; ++i)
				m_DebugRenderer.RenderForceField(*ppFF[i]);

			APhysXCharCtrlerMgrImp* pCCMImp = APhysXCharCtrlerMgrImp::GetCCM();
			if (0 != pCCMImp)
				pCCMImp->AddDebugData(m_DebugRenderer, this);
		}

		if(m_bEnableExtraDebugRender)
			m_DebugRenderer.RenderExtraDebugData(this);


		APHYSX_END_PROFILING("APhysX_Debug_Render");

		if(m_bEnablePerfStatsRender)
		{
			int x = 10;
			int y = 10;
			if (0 != pDesc)
			{
				if (-1 != pDesc->nPerfStatsTextStartX)
					x = pDesc->nPerfStatsTextStartX; 
				if (-1 != pDesc->nPerfStatsTextStartY)
					y = pDesc->nPerfStatsTextStartY; 
			}
			m_DebugRenderer.SetTextStartXY(x, y);
			m_DebugRenderer.PreRender();
			m_DebugRenderer.RenderPerfStats(&m_PerfStats);

			if (0 != pDesc)
			{
				x = m_DebugRenderer.GetCurTextStartX();
				y = m_DebugRenderer.GetCurTextStartY();
				if (-1 != pDesc->nSceneStatsTextStartX)
					x = pDesc->nSceneStatsTextStartX; 
				if (-1 != pDesc->nSceneStatsTextStartY)
					y = pDesc->nSceneStatsTextStartY; 
				m_DebugRenderer.SetTextStartXY(x, y);
				m_DebugRenderer.PreRender();
			}
			m_DebugRenderer.RenderSceneStats(m_pNxScene);

			if (0 != pDesc)
			{
				x = m_DebugRenderer.GetCurTextStartX();
				y = m_DebugRenderer.GetCurTextStartY();
				if (-1 != pDesc->nSDKStatsTextStartX)
					x = pDesc->nSDKStatsTextStartX; 
				if (-1 != pDesc->nSDKStatsTextStartY)
					y = pDesc->nSDKStatsTextStartY; 
				m_DebugRenderer.SetTextStartXY(x, y);
				m_DebugRenderer.PreRender();
			}
			m_DebugRenderer.RenderPhysXSDKStats(m_pAPhysXEngine->GetNxPhysicsSDK());
		}


		m_MutexNxSceneWriting.Unlock();

	}

}

static AString TrimString(const AString& strIn, int iLen)
{
	if(strIn.GetLength() >= iLen)
		return strIn.Left(iLen);

	AString strOut(strIn);
	for(int i= strIn.GetLength(); i<iLen; i++)
	{
		strOut += AString(" ");
	}

	return strOut;
}

void APhysXScene::LogPerfStats()
{
	gPhysXLog->Log("----------------Frame #: %d-----------------------------", m_uSimFrameCount);

	for(int i=0; i<m_PerfStats.GetPerfStatsNum(); i++)
	{
		APhysXPerfItem* pPerfItem = m_PerfStats.GetPerfItem(i);

		// filter out some trivial performance items...
#define TRIVIAL_MS 0.001f
		if(	pPerfItem->GetCurFrameMs() < TRIVIAL_MS 
			// &&	pPerfItem->GetAvgFrameMs() < TRIVIAL_MS 
			// &&	pPerfItem->GetMaxFrameSecondsInLastWindow() * 1000 < TRIVIAL_MS
			)
			continue;

		gPhysXLog->Log("PerfStats: %s, \tFrameTime: %7.3f Ms, AvgFrameTime: %7.3f Ms, MaxFrameTime: %7.3f Ms", pPerfItem->GetName(), pPerfItem->GetCurFrameMs(), pPerfItem->GetAvgFrameMs(), pPerfItem->GetMaxFrameSecondsInLastWindow() * 1000);
	}
	
	// log scene stats as well...
	if(!m_pNxScene) return;

	AString strOutput;
	const int iTrimLen = 25;
	NxSceneStats sceneStats;
	m_pNxScene->getStats(sceneStats);

	// num of actors
	strOutput.Format("    %d", sceneStats.numActors);
	strOutput = TrimString("numActors:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of dynamic actors
	strOutput.Format("    %d", sceneStats.numDynamicActors);
	strOutput = TrimString("numDynamicActors:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of awake dynamic actors
	strOutput.Format("    %d", sceneStats.numDynamicActorsInAwakeGroups);
	strOutput = TrimString("numAwakeDynamicActors:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of pairs
	strOutput.Format("    %d", sceneStats.numPairs);
	strOutput = TrimString("numPairs:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of contacts
	strOutput.Format("    %d", sceneStats.numContacts);
	strOutput = TrimString("numContacts:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of joints
	strOutput.Format("    %d", sceneStats.numJoints);
	strOutput = TrimString("numJoints:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of static shapes
	strOutput.Format("    %d", sceneStats.numStaticShapes);
	strOutput = TrimString("numStaticShapes:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of dynamic shapes
	strOutput.Format("    %d", sceneStats.numDynamicShapes);
	strOutput = TrimString("numDynamicShapes:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of materials
	strOutput.Format("    %d", m_pNxScene->getNbMaterials());
	strOutput = TrimString("numMaterials:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of forcefields
	strOutput.Format("    %d", m_pNxScene->getNbForceFields());
	strOutput = TrimString("numForceFields:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of cloths and cloth vertices...
	int iClothNum = m_pNxScene->getNbCloths();
	int iClothVertexNum = 0;
	for(int i=0; i<iClothNum; i++)
	{
		iClothVertexNum += m_pNxScene->getCloths()[i]->getNumberOfParticles();
	}
	strOutput.Format("    %d", iClothNum);
	strOutput = TrimString("numCloths:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);
	strOutput.Format("    %d", iClothVertexNum);
	strOutput = TrimString("numTotalClothVertices:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

	// num of fluids and fluid particles...
	int iFluidNum = m_pNxScene->getNbFluids();
	int iFluidParticleNum = 0;
	for(int i=0; i<iFluidNum; i++)
	{
		NxParticleData particleData = m_pNxScene->getFluids()[i]->getParticlesWriteData();
		iFluidParticleNum += *particleData.numParticlesPtr;
	}
	strOutput.Format("    %d", iFluidNum);
	strOutput = TrimString("numFluids:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);
	strOutput.Format("    %d", iFluidParticleNum);
	strOutput = TrimString("numTotalFluidParticles:", iTrimLen) + strOutput;
	gPhysXLog->LogString(strOutput);

}

void APhysXScene::Reset(bool bResetTerrain)
{
	//------------------------------------------------------------
	// note the order of the following release!!!
	// We should firstly release m_PhysXTerrain before release all APhysXObjects
	// since the terrain objects will still be referenced in m_PhysXTerrain.
	//------------------------------------------------------------

	//------------------------------------------------------------
	// first we release the terrain according to the flag bResetTerrain
	// since the APhysXObject release operation is cached, we should then
	// call TickCreationAndDeletion() after releasing terrain...
	if(bResetTerrain)
		m_PhysXTerrain.Release();

	//------------------------------------------------------------
	// before the release, we should make sure that the cached
	// creation and deletion are handled...
	TickCreationAndDeletion();

	// release all APhysXConnectors, this should be done before release all APhysXObjects...
	for(int i=0; i<m_arrPhysXConnectors.size(); i++)
	{
		m_arrPhysXConnectors[i]->OnReleaseByPhysXScene();
		delete m_arrPhysXConnectors[i];
	}

	m_arrPhysXConnectors.clear();

	// release all CCs created in this scene ...
	APhysXCharCtrlerMgrImp* pCCMImp = APhysXCharCtrlerMgrImp::GetCCM();
	if (0 != pCCMImp)
		pCCMImp->ReleaseAll(this);

	// release all APhysXObjects ...

	APhysXArray<APhysXObject *> arrTerrainPatchObjects;

	for(int i=0; i<m_arrPhysXObjects.size(); i++)
	{
		if(!bResetTerrain && m_arrPhysXObjects[i]->GetType() == APX_OBJTYPE_TERRAIN_PATCH)
		{
			arrTerrainPatchObjects.push_back(m_arrPhysXObjects[i]);
		}
		else
		{
			// invalidate the deleted object for picker
			m_PhysXPicker.Invalidate(m_arrPhysXObjects[i]);

			m_CollisionChannelManager.RemoveChannelRef(m_arrPhysXObjects[i]->GetCollisionChannel());
			m_arrPhysXObjects[i]->OnRelease();
			delete m_arrPhysXObjects[i];
		}
	}

	m_arrPhysXObjects.clear();
	m_arrActivePhysXObjects.clear();
	m_mapNamedPhysXObjects.clear();

	for(int i=0; i<arrTerrainPatchObjects.size(); i++)
	{
		m_arrPhysXObjects.push_back(arrTerrainPatchObjects[i]);
	}

	// note here, the No.1 material is the default material of the NxScene
	// so, we have to keep it!
	m_MutexMaterialDescs.Lock();
	if(m_arrMaterialDescs.size())
	{
		APhysXMaterialDesc apxDefaultMaterialDesc = m_arrMaterialDescs[0];
		m_arrMaterialDescs.clear();
		m_arrMaterialDescs.push_back(apxDefaultMaterialDesc);

		// not to release materials in m_pNxScene as well!
		int iNum = m_pNxScene->getNbMaterials();
		for(int i=1; i<iNum; i++)
		{
			m_MutexNxSceneWriting.Lock();
			NxMaterial* pNxMaterial = m_pNxScene->getMaterialFromIndex(i);
			m_pNxScene->releaseMaterial(* pNxMaterial);
			m_MutexNxSceneWriting.Unlock();		
		}

		//-----------------------------------------------------------
		// testing code...
		// iNum = m_pNxScene->getNbMaterials();
		// int iHID = m_pNxScene->getHighestMaterialIndex();
	}
	m_MutexMaterialDescs.Unlock();

	//??? you should StartSimulation() again ???
	m_bPauseSimulate = false;
	m_bOnSimulate = false;

	

	//??? should I release the ground actor here???

	ReleaseDummyActors();
}

void APhysXScene::OnRelease()
{
	/*
	// release all APhysXObjects ...
	for(int i=0; i<m_arrPhysXObjects.size(); i++)
	{
		m_arrPhysXObjects[i]->OnRelease();
		delete m_arrPhysXObjects[i];
	}

	m_arrPhysXObjects.clear();
	m_arrActivePhysXObjects.clear();
	*/

	// user's deletion callback
	gPhysXCreationDeletionReport->OnSceneDeletionNotify(this);

	Reset();

	m_MutexNxSceneWriting.Lock();

	// release the force field materials...
	m_pNxScene->releaseForceFieldMaterial(m_ffMaterialRB);
	m_pNxScene->releaseForceFieldMaterial(m_ffMaterialCloth);
	m_pNxScene->releaseForceFieldMaterial(m_ffMaterialFluid);
	m_pNxScene->releaseForceFieldMaterial(m_ffMaterialSB);

	m_pAPhysXEngine->GetNxPhysicsSDK()->releaseScene(*m_pNxScene);

	m_MutexNxSceneWriting.Unlock();

	m_pNxScene = NULL;

}

int APhysXScene::GetMaterialIndex(const APhysXMaterialDesc& materialDesc)
{
	// we should make sure that the material desc is  1 to 1 mapping the the 
	// real material id of NxScene...
	
	// use construct-lock-destruct-unlock instead multi-branch unlock
	APhysXMutexLock mlMaterialDescs(m_MutexMaterialDescs);
	// m_MutexMaterialDescs.Lock();
	
	for(int i=0; i<m_arrMaterialDescs.size(); i++)
	{
		if(materialDesc == m_arrMaterialDescs[i])
		{
			// m_MutexMaterialDescs.Unlock();
			return i;
		}
	}

	m_arrMaterialDescs.push_back(materialDesc);
	NxMaterialDesc mtDesc;
	materialDesc.GetNxMaterialDesc(mtDesc);
	LockWritingScene();
	NxMaterial * pNxMaterial = m_pNxScene->createMaterial(mtDesc);
	UnlockWritingScene();

	int mid = 0;
	if(pNxMaterial)
	{
		mid = pNxMaterial->getMaterialIndex();
	}
	else
	{
		gPhysXLog->Log("Warning: Failed on creating physics material.");
	}
	
	
	//----------------------------------------------------
	// testing code...
	// int iMatNum = m_pNxScene->getNbMaterials();
	// int iHID = m_pNxScene->getHighestMaterialIndex();

	// Note here: by wenfeng, Aug. 3, 2009
	// mid may be not the last index of m_pNxScene->getNbMaterials() - 1 
	// remove the assert of mid == m_arrMaterialDescs.size() - 1 because 
	// NxMaterial management use the cached mechanism and maybe not 
	// release the material instantly. Pls. refer to PhysX API Docs for 
	// more details...
	//
	// APHYSX_ASSERT(mid == m_arrMaterialDescs.size() - 1);
	//----------------------------------------------------

	// m_MutexMaterialDescs.Unlock();
	
	return mid;

}

void APhysXScene::ReleaseDummyActors()
{
	for(int i=0; i<m_arrDummyNxActors.size(); i++)
	{
		// invalidate the deleted actor for picker
		m_PhysXPicker.Invalidate(m_arrDummyNxActors[i]);

		m_MutexNxSceneWriting.Lock();
		m_pNxScene->releaseActor(*m_arrDummyNxActors[i]);
		m_MutexNxSceneWriting.Unlock();
	}

	m_arrDummyNxActors.clear();
}

bool APhysXScene::InitTerrain(const char* szTerrainConfigFileName)
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
		return false;

	return m_PhysXTerrain.Init(this, szTerrainConfigFileName);
}


void APhysXScene::InitPicker(A3DViewport* pA3DViewPort)
{
	m_PhysXPicker.Init(this, pA3DViewPort);
}

void APhysXScene::CreateGroundPlane(float fElevation)
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
		return;

	ReleaseGroundPlane();

	if(m_pNxScene)
	{
		NxPlaneShapeDesc planeShapeDesc;
		planeShapeDesc.d = fElevation;
		planeShapeDesc.groupsMask = m_CollisionChannelManager.GetChannelGroupMask(APX_COLLISION_CHANNEL_COMMON);
		
		NxActorDesc groundActorDesc;
		groundActorDesc.shapes.push_back(&planeShapeDesc);

		m_MutexNxSceneWriting.Lock();
		m_pGroundPlaneActor = m_pNxScene->createActor(groundActorDesc);	
		m_MutexNxSceneWriting.Unlock();

		APHYSX_ASSERT(m_pGroundPlaneActor);
	}
}

void APhysXScene::ReleaseGroundPlane()
{
	if(m_pGroundPlaneActor)
	{
		APHYSX_ASSERT(m_pNxScene);
		
		m_MutexNxSceneWriting.Lock();
		m_pNxScene->releaseActor(*m_pGroundPlaneActor);
		m_MutexNxSceneWriting.Unlock();
		
		m_pGroundPlaneActor = NULL;
	}
}

APhysXObject* APhysXScene::GetPhysXObject(const char* szObjName)
{
	if(szObjName == NULL) return NULL;

	APhysXString strObjName(szObjName);
	APhysXObjectMap::iterator it = m_mapNamedPhysXObjects.find(strObjName);
	if(it != m_mapNamedPhysXObjects.end())
		return it->second;
	else
		return NULL;

}


void APhysXScene::FlushCreation()
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
	{
		APHYSX_ASSERT(true == m_arrCachedCreationObjects.isEmpty());
		APHYSX_ASSERT(true == m_arrCachedCreationConnectors.isEmpty());
		return;
	}

	//--------------------------------------------------------------------------------
	// the real creation operations occur in this function
	// Note:	Dec.10, 2008, Wenfng.
	//		In fact, the real creation operation is immediately done when calling  
	// CreatePhysXObject(...), here, we just push the cached creation objects to
	// m_arrPhysXObjects. I can do like this because I can't see any problems due
	// to this way.
	//--------------------------------------------------------------------------------
	//
	// Beware the following order of creation and deletion...
	//
	//--------------------------------------------------------------------------------

	// first, we handle the object creation...
	m_MutexCachedCreationObjects.Lock();
	for(int i=0; i<m_arrCachedCreationObjects.size(); i++)
	{
		APhysXObject* pPhysXObj = m_arrCachedCreationObjects[i];

		m_arrPhysXObjects.push_back(pPhysXObj);

		// push the named object to the map
		if(pPhysXObj->GetName())
		{
			APhysXString strObjName(pPhysXObj->GetName());
			m_mapNamedPhysXObjects.insert(APhysXObjectPair(strObjName, pPhysXObj));
		}
	}

	m_arrCachedCreationObjects.clear();

	m_MutexCachedCreationObjects.Unlock();

	// second, we handle the connector creation...

	m_MutexCachedCreationConnectors.Lock();
	for(int i=0; i<m_arrCachedCreationConnectors.size(); i++)
	{
		APhysXObjectConnector* pConnector = m_arrCachedCreationConnectors[i];

		m_arrPhysXConnectors.push_back(pConnector);
	}

	m_arrCachedCreationConnectors.clear();
	m_MutexCachedCreationConnectors.Unlock();

}

void APhysXScene::FlushDeletion()
{
	if (m_pAPhysXEngine->IsPhysXDisbaled())
	{
		APHYSX_ASSERT(true == m_arrCachedDeletionObjects.isEmpty());
		APHYSX_ASSERT(true == m_arrCachedDeletionConnectors.isEmpty());
		return;
	}
	//--------------------------------------------------------------------------------
	// the real deletion operations occur in this function
	// Note:	Dec.10, 2008, Wenfng.
	//
	// we must do the real deletion work in this function.
	//--------------------------------------------------------------------------------
	//
	// Beware the following order of creation and deletion...
	//
	//--------------------------------------------------------------------------------

	// firstly, we handle the connector deletion...
	m_MutexCachedDeletionConnectors.Lock();
	for(int i=0; i<m_arrCachedDeletionConnectors.size(); i++)
	{
		APhysXObjectConnector* pConnector = m_arrCachedDeletionConnectors[i];

		m_arrPhysXConnectors.deleteEntry(pConnector);

		// Furthermore, check if the entry in cached creation list...
		// ??? dead-lock ???
		m_MutexCachedCreationConnectors.Lock();
		m_arrCachedCreationConnectors.deleteEntry(pConnector);		
		m_MutexCachedCreationConnectors.Unlock();

		m_MutexNxSceneWriting.Lock();
		pConnector->OnReleaseByPhysXScene();
		m_MutexNxSceneWriting.Unlock();

		delete pConnector;
	}

	m_arrCachedDeletionConnectors.clear();
	m_MutexCachedDeletionConnectors.Unlock();


	// at last, we handle the object deletion...

	m_MutexCachedDeletionObjects.Lock();
	for(int i=0; i<m_arrCachedDeletionObjects.size(); i++)
	{
		APhysXObject* pPhysXObject = m_arrCachedDeletionObjects[i];

		// invalidate the deleted object for picker
		m_PhysXPicker.Invalidate(pPhysXObject);
			
		// delete the record in array and map
		m_arrPhysXObjects.deleteEntry(pPhysXObject);
		m_arrActivePhysXObjects.deleteEntry(pPhysXObject);
		if(pPhysXObject->GetName())
		{
			APhysXString strObjName(pPhysXObject->GetName());

#if 1
			m_mapNamedPhysXObjects.erase(strObjName);
#else
			APhysXObjectMap::iterator it = m_mapNamedPhysXObjects.find(strObjName);
			if(it != m_mapNamedPhysXObjects.end())
			{
				m_mapNamedPhysXObjects.erase(it);
			}
#endif

		}

		// Furthermore, check if the entry in cached creation list...
		// ??? dead-lock ???
		m_MutexCachedCreationObjects.Lock();
		m_arrCachedCreationObjects.deleteEntry(pPhysXObject);		
		m_MutexCachedCreationObjects.Unlock();

		// remove the reference to the collision channel...
		m_CollisionChannelManager.RemoveChannelRef(pPhysXObject->GetCollisionChannel());

		//-----------------------------------------------
		// revised by Wenfeng, Dec. 11, 2012
		// remove the m_MutexNxSceneWriting's Lock()/Unlock() here and 
		// move them to pPhysXObject->OnRelease()
		pPhysXObject->OnRelease();
		//-----------------------------------------------
		
		//????? Should I delete pPhysXObject here????
		// so how about some references of pPhysXObject outside???
		delete pPhysXObject;
	}

	m_arrCachedDeletionObjects.clear();
	m_MutexCachedDeletionObjects.Unlock();

}


void APhysXScene::TickCreationAndDeletion()
{

	FlushCreation();
	FlushDeletion();
}


void APhysXScene::EnableCollisionChannel(bool bEnable)
{

	if(m_pNxScene && m_bEnableCollisionChannel != bEnable)
	{
		NxGroupsMask groupMask;

		if(bEnable)
		{
			groupMask.bits0 = groupMask.bits1 = groupMask.bits2 = groupMask.bits3 = 0;
		}
		else
		{
			groupMask.bits0 = 1;
		}


		m_MutexNxSceneWriting.Lock();

		m_pNxScene->setFilterConstant0(groupMask);
		m_pNxScene->setFilterConstant1(groupMask);
		
		m_MutexNxSceneWriting.Unlock();

		m_bEnableCollisionChannel = bEnable;

	}
}

bool APhysXScene::ForceMagLimitation(const NxActor& rActor, NxVec3& fInOutForce, NxForceMode forceMode, float fMaxVelChange) const
{
	float dt = GetExactSimDt();//  do not use GetCurFrameDt(), it isn't updated until call APhysXScene::Simulate()
	float fInputMag = fInOutForce.magnitude();
	fMaxVelChange = NxMath::abs(fMaxVelChange);

	if (forceMode == NX_FORCE)
	{
		if (!(dt > APHYSX_FLOAT_EPSILON))
			return false;

		if(IsSubSimulationEnabled()) 
		{	
			// scale the force since we apply the force for one time step instead of dt...
			float fScale = dt / m_fTimeStep;
			fInputMag *= fScale;
			fInOutForce *= fScale;
			dt = m_fTimeStep;
		}

		float fVelChange = fInputMag * dt / rActor.getMass();
		if (fVelChange > fMaxVelChange)
			fInOutForce = fInOutForce * (fMaxVelChange / fVelChange);
	}
	else if(forceMode == NX_IMPULSE || forceMode == NX_SMOOTH_IMPULSE)
	{
		float fVelChange = fInputMag / rActor.getMass();
		if (fVelChange > fMaxVelChange)
			fInOutForce = fInOutForce * (fMaxVelChange / fVelChange);
	}
	else if(forceMode == NX_VELOCITY_CHANGE || forceMode == NX_SMOOTH_VELOCITY_CHANGE)
	{
		float fVelChange = fInputMag;
		if(fVelChange > fMaxVelChange)
			fInOutForce = fInOutForce * (fMaxVelChange / fVelChange);
	}
	else if(forceMode == NX_ACCELERATION)
	{
		if (!(dt > APHYSX_FLOAT_EPSILON))
			return false;

		if(IsSubSimulationEnabled()) 
		{	
			// scale the force since we apply the force for one time step instead of dt...
			float fScale  = dt / m_fTimeStep;
			fInputMag *= fScale;
			fInOutForce *= fScale;
			dt = m_fTimeStep;
		}

		float fVelChange = fInputMag * dt;
		if(fVelChange > fMaxVelChange)
			fInOutForce = fInOutForce * (fMaxVelChange / fVelChange);
	}
	return true;
}

void APhysXScene::AddLimitForce(NxActor* pActor, const NxVec3& vForce, NxForceMode forceMode /* = NX_FORCE */, float fMaxVelChange /* = 10.0f */)
{
	if (0 == pActor)
		return;

	NxVec3 vApplyForce = vForce;
	if (pActor->isDynamic() && !pActor->readBodyFlag(NX_BF_KINEMATIC) &&
		ForceMagLimitation(*pActor, vApplyForce, forceMode, fMaxVelChange))
	{
		m_MutexNxSceneWriting.Lock();
		pActor->addForce(vApplyForce, forceMode);
		m_MutexNxSceneWriting.Unlock();
	}
}

void APhysXScene::AddLimitForceAtPos(NxActor* pActor, const NxVec3& vForce, const NxVec3& vPos, NxForceMode forceMode /* = NX_FORCE */, float fMaxVelChange /* = 10.0f */)
{
	if (0 == pActor)
		return;

	NxVec3 vApplyForce = vForce;
	if (pActor->isDynamic() && !pActor->readBodyFlag(NX_BF_KINEMATIC) &&
		ForceMagLimitation(*pActor, vApplyForce, forceMode, fMaxVelChange))
	{
		m_MutexNxSceneWriting.Lock();
		pActor->addForceAtPos(vApplyForce, vPos, forceMode);
		m_MutexNxSceneWriting.Unlock();
	}
}

void APhysXScene::EnableTerrainDebugRender(bool bEnable /* = true */)
{
	if(m_bEnableTerrainDebugRender == bEnable) return;

	m_bEnableTerrainDebugRender = bEnable;
	for(int i=0; i<m_arrPhysXObjects.size(); i++)
	{
		if(m_arrPhysXObjects[i]->GetType() == APX_OBJTYPE_TERRAIN_PATCH)
		{
			m_arrPhysXObjects[i]->EnableDebugRender(bEnable);
		}
	}

}

void APhysXScene::BeginProfiling(const char* szProfName)
{
	APHYSX_BEGIN_PROFILING(szProfName);
}

void APhysXScene::EndProfiling(const char* szProfName)
{
	APHYSX_END_PROFILING(szProfName);
}



void APhysXScene::AddForce(APhysXForceInfo apxForceInfo)
{
	if(IsSubSimulationEnabled())
	{
		g_AddForceSubSimulationReport.pushBack(apxForceInfo);
	}
	else
	{
		LockWritingScene();
		ExecuteAddForce(apxForceInfo);
		UnlockWritingScene();
	}
}

void APhysXScene::ExeSetActorPose(NxActor* pActor, const NxMat34& mtPose)
{
	if(!pActor) return;

	LockWritingScene();

	if(pActor->isDynamic())
	{
		if(pActor->readBodyFlag(NX_BF_KINEMATIC))
			pActor->moveGlobalPose(mtPose);

		else
			pActor->setGlobalPose(mtPose);
	}


	UnlockWritingScene();
}

void APhysXScene::SetActorPose(NxActor* pActor, const NxMat34& mtPose)
{
	if(!pActor) return;

	if(IsSubSimulationEnabled())
	{
		APhysXSetActorPoseSubSimInfo apxSAPInfo;
		apxSAPInfo.mNxActor = pActor;
		
		
		apxSAPInfo.mSubSimCurPos = pActor->getGlobalPosition();
		apxSAPInfo.mSubSimCurRot = pActor->getGlobalOrientationQuat();

		apxSAPInfo.mSubSimTargetPos = mtPose.t;
		mtPose.M.toQuat(apxSAPInfo.mSubSimTargetRot);

		apxSAPInfo.mAccumulatedSubSimDt = 0.0f;

		g_SetActorPoseSubSimulationReport.push_back(apxSAPInfo);

	}
	else
		ExeSetActorPose(pActor, mtPose);
}

bool APhysXScene::IsPerfStatsLogEnabled_Record() const 
{ 
	return gPhysXEngine->GetFlag(APX_ENGINEFLAG_ENABLEPROFILE); 
}

bool APhysXScene::IsPerfStatsLogEnabled_Print() const 
{ 
	return gPhysXEngine->GetFlag(APX_ENGINEFLAG_LOGPROFILE); 
}
