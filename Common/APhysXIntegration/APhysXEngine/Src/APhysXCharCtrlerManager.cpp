#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerMgrImp.h"
#include "APhysXCharCtrlerImp.h"

#include "..\..\APhysXNxCharacter_Public\include\NxController.h"
#include "..\..\APhysXNxCharacter_Public\include\NxControllerManager.h"

//----------------------------------------------------------------
// load CC dll dynamically...
/*

// no need since we try to load CC dll dynamically...

#if _MSC_VER < 1400
#ifdef APHYSX_DEBUG
#pragma	comment(lib, "APhysXNxCharacter_d.lib")
#else
#pragma	comment(lib, "APhysXNxCharacter.lib")
#endif
#endif		// #if _MSC_VER < 1400

*/

typedef NxControllerManager*(NxCreateControllerManager_FUNC)(NxUserAllocator* allocator);
typedef void NxReleaseControllerManager_FUNC(NxControllerManager* manager);
static HMODULE	g_APhysXCCLibrary = 0;

#if defined(APHYSX_DEBUG)
	static const char*	g_APhysXCCLibraryName = "APhysXNxCharacter_d.dll";
#else
	static const char*	g_APhysXCCLibraryName = "APhysXNxCharacter.dll";
#endif

NxControllerManager* APhysXCharacterControllerManager::pNxCCMgr = 0;

int APhysXCharacterControllerManager::ClosestSweepExcludesAllCCs(const APhysXScene& scene, const NxCapsule& cap, const NxVec3& motion, NxU32 flags, NxSweepQueryHit& outResult)
{
	APhysXCharCtrlerMgrImp* pCCMImp = APhysXCharCtrlerMgrImp::GetCCM();
	if (0 == pCCMImp)
		return -1;

	APhysXScene* pScene = const_cast<APhysXScene*>(&scene);
	if (0 == pScene->GetNxScene())
		return -1;

	if (pCCMImp->ClosestSweepExcludesAllCCs(scene, cap, motion, flags, outResult))
		return 1;
	
	return 0;
}

int APhysXCharacterControllerManager::ClosestRaycastExcludesAllCCs(const APhysXScene& scene, const NxVec3& rayOrig, const NxVec3& motion, NxShapesType flags, NxRaycastHit& inoutResult)
{
	APhysXCharCtrlerMgrImp* pCCMImp = APhysXCharCtrlerMgrImp::GetCCM();
	if (0 == pCCMImp)
		return -1;

	APhysXScene* pScene = const_cast<APhysXScene*>(&scene);
	if (0 == pScene->GetNxScene())
		return -1;

	if (pCCMImp->ClosestRaycastExcludesAllCCs(scene, rayOrig, motion, flags, inoutResult))
		return 1;

	return 0;
}

const int APhysXCharCtrlerMgrImp::gTerrainHeightExtension = 1000;

APhysXCharCtrlerMgrImp* APhysXCharCtrlerMgrImp::InitCCM(APhysXEngine& engine)
{
	if (0 == pNxCCMgr)
	{
		g_APhysXCCLibrary = LoadLibraryA(g_APhysXCCLibraryName);
		if(0 == g_APhysXCCLibrary)
		{
			gPhysXLog->Log("Error: Failed on loading %s library!", g_APhysXCCLibraryName);
			return 0;
		}

		NxCreateControllerManager_FUNC* func = (NxCreateControllerManager_FUNC*)GetProcAddress(g_APhysXCCLibrary, "NxCreateControllerManager");
		pNxCCMgr = func(engine.GetAllocator());
	}

	return GetCCM();
}

APhysXCharCtrlerMgrImp* APhysXCharCtrlerMgrImp::GetCCM()
{
	if (0 == pNxCCMgr)
		return 0;

	static APhysXCharCtrlerMgrImp theOne;
	return &theOne;
}

void APhysXCharCtrlerMgrImp::ReleaseCCM()
{
	if (0 == pNxCCMgr)
		return;

	APhysXCharCtrlerMgrImp* pCCMImp = GetCCM();
	// Note: we just ignore the Nx... objects directly since the NxScene has already been released.
	//		 refer to APhysXEngine::Release(), this function was called after all APhysXScene released.
	pCCMImp->ReleaseAllExcution(true, 0);   
	pCCMImp->ResetToInit();

	assert(0 != g_APhysXCCLibrary);
	if(0 != g_APhysXCCLibrary)
	{
		NxReleaseControllerManager_FUNC* func = (NxReleaseControllerManager_FUNC*)GetProcAddress(g_APhysXCCLibrary, "NxReleaseControllerManager");
		func(pNxCCMgr);

		FreeLibrary(g_APhysXCCLibrary);
		g_APhysXCCLibrary = 0;
	}

	pNxCCMgr = 0;
}

APhysXCharCtrlerMgrImp::APhysXCharCtrlerMgrImp()
{
	ResetToInit();
}

void APhysXCharCtrlerMgrImp::ResetToInit()
{
	m_StaticObjectsChanged = false;
	SetInteractFlag(APX_CCCF_COLLIDE_ALL_CHANNEL);
	SetFlushMode(APX_CCFM_SYNCHRONOUS);
}

bool APhysXCharCtrlerMgrImp::TickForSynPos() const
{
	assert(0 != pNxCCMgr);
	if (0 != pNxCCMgr)
	{
		pNxCCMgr->updateControllers();
		return true;
	}
	return false;
}

void APhysXCharCtrlerMgrImp::TickForInternal() const
{
	if (!m_StaticObjectsChanged)
		return;

	m_StaticObjectsChanged = false;
	APhysXArray<APhysXCharacterController*>::const_iterator itCC = m_pVecCC.begin();
	APhysXArray<APhysXCharacterController*>::const_iterator itCCEnd = m_pVecCC.end();
	for (; itCC != itCCEnd; ++itCC)
	{
		APhysXCCImp* pCCImp = static_cast<APhysXCCImp*>(*itCC);
		pCCImp->ReportSceneChanged();
	}

	APhysXArray<APhysXNxCharCtrler*>::const_iterator itNxCC = m_pVecNxCC.begin();
	APhysXArray<APhysXNxCharCtrler*>::const_iterator itNxCCEnd = m_pVecNxCC.end();
	for (; itNxCC != itNxCCEnd; ++itNxCC)
	{
		APhysXNxCCImp* pNxCCImp = static_cast<APhysXNxCCImp*>(*itNxCC);
		pNxCCImp->ReportSceneChanged();
	}
}

void APhysXCharCtrlerMgrImp::Tick() const
{
	if (APX_CCFM_ASYNCHRONOUS == GetFlushMode())
	{
		TickForSynPos();

		APhysXArray<APhysXLWCharCtrler*>::const_iterator it = m_pVecCCLW.begin();
		APhysXArray<APhysXLWCharCtrler*>::const_iterator itEnd = m_pVecCCLW.end();
		for (; it != itEnd; ++it)
			(*it)->TickReviseVerticalPos();
	}

	TickForInternal();
}

void APhysXCharCtrlerMgrImp::ReleaseAll(APhysXScene* pScene)
{
	ReleaseAllExcution(false, pScene);
}

void APhysXCharCtrlerMgrImp::ReleaseAllExcution(bool ignoreNxObject, APhysXScene* pScene)
{
	// for thread safe, don't call pNxCCMgr->purgeControllers();
	int idx = 0;
	int nSize = m_pVecCC.size();
	while (idx < nSize)
	{
		if ((0 != pScene) && (pScene != &(m_pVecCC[idx]->GetAPhysXScene())))
		{
			++idx;
			continue;
		}
		ReleaseCCExcution(m_pVecCC[idx], ignoreNxObject);
		nSize = m_pVecCC.size();
	}
	
	idx = 0;
	nSize = m_pVecCCLW.size();
	while (0 < m_pVecCCLW.size())
	{
		if ((0 != pScene) && (pScene != &(m_pVecCCLW[idx]->GetAPhysXScene())))
		{
			++idx;
			continue;
		}
		ReleaseLWCCExcution(m_pVecCCLW[0], ignoreNxObject);
		nSize = m_pVecCCLW.size();
	}
	
	idx = 0;
	nSize = m_pVecCCDyn.size();
	while (0 < m_pVecCCDyn.size())
	{
		if ((0 != pScene) && (pScene != &(m_pVecCCDyn[idx]->GetAPhysXScene())))
		{
			++idx;
			continue;
		}
		ReleaseDynCCExcution(m_pVecCCDyn[0], ignoreNxObject);
		nSize = m_pVecCCLW.size();
	}

	idx = 0;
	nSize = m_pVecNxCC.size();
	while (idx < nSize)
	{
		if ((0 != pScene) && (pScene != &(m_pVecNxCC[idx]->GetAPhysXScene())))
		{
			++idx;
			continue;
		}
		ReleaseNxCCExcution(m_pVecNxCC[idx], ignoreNxObject);
		nSize = m_pVecNxCC.size();
	}

	if (0 == pScene)
	{
		assert(0 == m_RegChannels.size());
	}
}

APhysXCharacterController* APhysXCharCtrlerMgrImp::CreateCC(APhysXScene& scene, APhysXCCDesc& desc)
{
	assert(0 != pNxCCMgr);

	APhysXCollisionChannelManager* pCCM = scene.GetCollisionChannelManager();
	if (0 == pCCM)
		return 0;

	if (!desc.IsValid())
		return 0;

	const NxControllerDesc& NxDesc = desc.GetPhysXDesc();
	
	scene.LockWritingScene();
	NxController* pNxCtrler = pNxCCMgr->createController(scene.GetNxScene(), NxDesc);
	if (0 == pNxCtrler)
	{
		scene.UnlockWritingScene();
		return 0;
	}
	pNxCtrler->setCollision(true);
	pNxCtrler->getActor()->setDominanceGroup(APX_RB_DG_HIGH);
	scene.UnlockWritingScene();

	APhysXCharacterController* pCtrler = new APhysXCCImp(desc, NxDesc.skinWidth, *pNxCtrler, scene);
	if (0 != pCtrler)
		m_pVecCC.push_back(pCtrler);

	return pCtrler;
}

void APhysXCharCtrlerMgrImp::ReleaseCC(APhysXCharacterController* pController)
{
	ReleaseCCExcution(pController, false);
}

void APhysXCharCtrlerMgrImp::ReleaseCCExcution(APhysXCharacterController* pController, bool ignoreNxObject)
{
	if (0 == pController)
		return;
	
	bool rtn = m_pVecCC.deleteEntry(pController);
	assert(true == rtn);
	
	if (!ignoreNxObject)
		ReleaseNxController(*pController);
	delete pController;
}

void APhysXCharCtrlerMgrImp::ReleaseNxController(APhysXCharacterController& controller) const
{
	assert(0 != pNxCCMgr);
	APhysXCCImp& CCImp = static_cast<APhysXCCImp&>(controller);
	CCImp.GetAPhysXScene().LockWritingScene();
	pNxCCMgr->releaseController(CCImp.GetNxController());
	CCImp.GetAPhysXScene().UnlockWritingScene();
}

APhysXLWCharCtrler* APhysXCharCtrlerMgrImp::CreateLWCC(APhysXScene& scene, APhysXLWCCDesc& desc)
{
	if (!desc.IsValid())
		return 0;

	NxBodyDesc descBody;
	descBody.flags |= NX_BF_KINEMATIC;
	
	NxCapsuleShapeDesc descCapShape;
	descCapShape.radius = APhysXLWCCImp::gExpFactor * desc.radius;
	descCapShape.height = APhysXLWCCImp::gExpFactor * (desc.height - 2 * desc.radius);
	descCapShape.groupsMask = scene.GetCollisionChannelManager()->GetChannelGroupMask(APX_COLLISION_CHANNEL_CC_SKIP_COLLISION);
	
	NxActorDesc descActor;
	descActor.body	  = &descBody;
	descActor.density = 1;
	descActor.globalPose.t.set(desc.posFoot.x, desc.posFoot.y + desc.height * 0.5f, desc.posFoot.z);
	descActor.shapes.pushBack(&descCapShape);
	
	scene.LockWritingScene();
	NxActor* pActor = scene.GetNxScene()->createActor(descActor);
	pActor->setDominanceGroup(APX_RB_DG_HIGH);
	scene.UnlockWritingScene();
	if (0 == pActor)
		return 0;

	APhysXLWCharCtrler* pCtrler = new APhysXLWCCImp(desc, *pActor, scene);
	if (0 != pCtrler)
		m_pVecCCLW.push_back(pCtrler);

	return pCtrler;
}

void APhysXCharCtrlerMgrImp::ReleaseLWCC(APhysXLWCharCtrler* pController)
{
	ReleaseLWCCExcution(pController, false);
}

void APhysXCharCtrlerMgrImp::ReleaseLWCCExcution(APhysXLWCharCtrler* pController, bool ignoreNxObject)
{
	if (0 == pController)
		return;

	bool rtn = m_pVecCCLW.deleteEntry(pController);
	assert(true == rtn);
	
	if (!ignoreNxObject)
	{
		APhysXLWCCImp& LWCCImp = static_cast<APhysXLWCCImp&>(*pController);
		LWCCImp.ReleaseNxActor();
	}
	delete pController;
}

APhysXDynCharCtrler* APhysXCharCtrlerMgrImp::CreateDynCC(APhysXScene& scene, APhysXCCCapsuleDesc& desc)
{
	if (!desc.IsValid())
		return 0;

	NxBodyDesc descBody;
	descBody.mass = desc.mass;
	descBody.flags |= APhysXDynCCImp::gFrozenFlags;
	// this is a practical value. To avoid some strange bad behavior(example: jitter, penetration...), we have to set a big value. 
	descBody.solverIterationCount = 40;
	
	NxCapsuleShapeDesc descCapShape;
	descCapShape.radius = desc.radius;
	descCapShape.height = desc.height - 2 * desc.radius;
	descCapShape.groupsMask = scene.GetCollisionChannelManager()->GetChannelGroupMask(APX_COLLISION_CHANNEL_DYNAMIC_CC);
	descCapShape.materialIndex = GetSmoothMaterial(scene);
	
	NxActorDesc descActor;
	descActor.body = &descBody;
	descActor.globalPose.t.set(desc.posFoot.x, desc.posFoot.y + desc.height * 0.5f, desc.posFoot.z);
	descActor.shapes.pushBack(&descCapShape);
	if (APhysXDynCharCtrler::APX_CCF_PASSIVE_EFFECT_ONLY & descCapShape.flags)
		descActor.dominanceGroup = APX_RB_DG_MIDDLE;

	scene.LockWritingScene();
	NxActor* pActor = scene.GetNxScene()->createActor(descActor);
	pActor->setDominanceGroup(APX_RB_DG_HIGH);
	scene.UnlockWritingScene();
	if (0 == pActor)
		return 0;
	
	APhysXDynCharCtrler* pCtrler = new APhysXDynCCImp(desc, *pActor, scene);
	if (0 != pCtrler)
		m_pVecCCDyn.push_back(pCtrler);
	
	return pCtrler;
}

void APhysXCharCtrlerMgrImp::ReleaseDynCC(APhysXDynCharCtrler* pController)
{
	ReleaseDynCCExcution(pController, false);
}

void APhysXCharCtrlerMgrImp::ReleaseDynCCExcution(APhysXDynCharCtrler* pController, bool ignoreNxObject)
{
	if (0 == pController)
		return;
	
	APhysXScene& scene = pController->GetAPhysXScene();
	bool rtn = m_pVecCCDyn.deleteEntry(pController);
	assert(true == rtn);
	
	if (!ignoreNxObject)
	{
		APhysXDynCCImp& DynCCImp = static_cast<APhysXDynCCImp&>(*pController);
		DynCCImp.ReleaseNxObjects();
	}
	delete pController;

	if (!ignoreNxObject)
		ReleaseSmoothMaterial(scene);
}

APhysXNxCharCtrler* APhysXCharCtrlerMgrImp::CreateNxCC(APhysXScene& scene, APhysXCCDesc& desc)
{
	assert(0 != pNxCCMgr);
	
	APhysXCollisionChannelManager* pCCM = scene.GetCollisionChannelManager();
	if (0 == pCCM)
		return 0;
	
	if (!desc.IsValid())
		return 0;
	
	const NxControllerDesc& NxDesc = desc.GetPhysXDesc();
	NxControllerDesc& aa = const_cast<NxControllerDesc&>(NxDesc);
	aa.callback = 0;
	
	scene.LockWritingScene();
	NxController* pNxCtrler = pNxCCMgr->createController(scene.GetNxScene(), aa);
	if (0 == pNxCtrler)
	{
		scene.UnlockWritingScene();
		return 0;
	}
	pNxCtrler->setCollision(true);
	pNxCtrler->getActor()->setDominanceGroup(APX_RB_DG_HIGH);
	scene.UnlockWritingScene();
	
	APhysXNxCharCtrler* pCtrler = new APhysXNxCCImp(desc, NxDesc.skinWidth, *pNxCtrler, scene);
	if (0 != pCtrler)
		m_pVecNxCC.push_back(pCtrler);
	
	return pCtrler;
}

void APhysXCharCtrlerMgrImp::ReleaseNxCC(APhysXNxCharCtrler* pController)
{
	ReleaseNxCCExcution(pController, false);
}

void APhysXCharCtrlerMgrImp::ReleaseNxCCExcution(APhysXNxCharCtrler* pController, bool ignoreNxObject)
{
	if (0 == pController)
		return;
	
	bool rtn = m_pVecNxCC.deleteEntry(pController);
	assert(true == rtn);
	
	if (!ignoreNxObject)
		ReleaseNxController(*pController);
	delete pController;
}

void APhysXCharCtrlerMgrImp::ReleaseNxController(APhysXNxCharCtrler& controller) const
{
	assert(0 != pNxCCMgr);
	APhysXNxCCImp& NxCCImp = static_cast<APhysXNxCCImp&>(controller);
	NxCCImp.GetAPhysXScene().LockWritingScene();
	pNxCCMgr->releaseController(NxCCImp.GetNxController());
	NxCCImp.GetAPhysXScene().UnlockWritingScene();
}

bool APhysXCharCtrlerMgrImp::RegisterChannel(APhysXScene& scene, const APhysXCollisionChannel channel)
{
	APhysXCollisionChannelManager* pCCM = scene.GetCollisionChannelManager();
	assert(0 != pCCM);
	if (0 == pCCM)
		return false;
	
	SGM regVal(scene);
	regVal.gm = pCCM->GetChannelGroupMask(channel);

	int idx = APhysXUtilLib::APhysXArray_Find_RtnIdx(m_RegChannels, regVal);
	if (0 > idx)
	{
		regVal.pRegCount = new APhysXMap<APhysXCollisionChannel, int>;
		regVal.pRegCount->insert(APhysXMap<APhysXCollisionChannel, int>::value_type(channel, 1));
		m_RegChannels.push_back(regVal);
	}
	else
	{
		SGM* pTarget = &m_RegChannels[idx];
		pTarget->gm.bits0 |= regVal.gm.bits0;
		pTarget->gm.bits1 |= regVal.gm.bits1;
		pTarget->gm.bits2 |= regVal.gm.bits2;
		pTarget->gm.bits3 |= regVal.gm.bits3;
		int count = (*pTarget->pRegCount)[channel];
		(*pTarget->pRegCount)[channel] = ++count;
	}

	return true;
}

bool APhysXCharCtrlerMgrImp::UnRegisterChannel(APhysXScene& scene, const APhysXCollisionChannel channel)
{
	APhysXCollisionChannelManager* pCCM = scene.GetCollisionChannelManager();
	assert(0 != pCCM);
	if (0 == pCCM)
		return false;

	SGM unregVal(scene);
	unregVal.gm = pCCM->GetChannelGroupMask(channel);

	int idx = APhysXUtilLib::APhysXArray_Find_RtnIdx(m_RegChannels, unregVal);
	if (0 <= idx)
	{
		SGM* pTarget = &m_RegChannels[idx];
		int count = (*pTarget->pRegCount)[channel];
		(*pTarget->pRegCount)[channel] = --count;

		if (0 == count)
		{
			SGM* pTarget = &m_RegChannels[idx];
			pTarget->gm.bits0 &= ~unregVal.gm.bits0;
			pTarget->gm.bits1 &= ~unregVal.gm.bits1;
			pTarget->gm.bits2 &= ~unregVal.gm.bits2;
			pTarget->gm.bits3 &= ~unregVal.gm.bits3;
			pTarget->pRegCount->erase(channel);
			
			if ((0 == pTarget->gm.bits0) && (0 == pTarget->gm.bits1) &&
				(0 == pTarget->gm.bits2) && (0 == pTarget->gm.bits3))
			{
				assert(0 == pTarget->pRegCount->size());
				delete pTarget->pRegCount;
				m_RegChannels.deleteEntry(unregVal);
			}
		}
	}
	return true;
}

NxGroupsMask APhysXCharCtrlerMgrImp::GetRegisteredGM(APhysXScene& scene) const
{
	SGM zeroVal(scene);

	int idx = APhysXUtilLib::APhysXArray_Find_RtnIdx(m_RegChannels, zeroVal);
	if (0 > idx)
		return zeroVal.gm;

	return m_RegChannels[idx].gm;
}

void APhysXCharCtrlerMgrImp::OnTerrainChange(bool IsLoading, const NxBounds3& aabb, const APhysXScene& Scene)
{
	APhysXArray<APhysXCharacterController*>::iterator itCC = m_pVecCC.begin();
	APhysXArray<APhysXCharacterController*>::iterator itCCEnd = m_pVecCC.end();
	for (; itCC != itCCEnd; ++itCC)
	{
		APhysXCCImp* pCCImp = static_cast<APhysXCCImp*>(*itCC);
		pCCImp->TerrainChange(IsLoading, aabb, Scene);
	}

	APhysXArray<APhysXLWCharCtrler*>::iterator itLWCC = m_pVecCCLW.begin();
	APhysXArray<APhysXLWCharCtrler*>::iterator itLWCCEnd = m_pVecCCLW.end();
	for (; itLWCC != itLWCCEnd; ++itLWCC)
	{
		APhysXLWCCImp* pLWCCImp = static_cast<APhysXLWCCImp*>(*itLWCC);
		pLWCCImp->TerrainChange(IsLoading, aabb, Scene);
	}

	APhysXArray<APhysXDynCharCtrler*>::iterator itCCDyn = m_pVecCCDyn.begin();
	APhysXArray<APhysXDynCharCtrler*>::iterator itCCEndDyn = m_pVecCCDyn.end();
	for (; itCCDyn != itCCEndDyn; ++itCCDyn)
	{
		APhysXDynCCImp* pDynCCImp = static_cast<APhysXDynCCImp*>(*itCCDyn);
		pDynCCImp->TerrainChange(IsLoading, aabb, Scene);
	}

	APhysXArray<APhysXNxCharCtrler*>::iterator itNxCC = m_pVecNxCC.begin();
	APhysXArray<APhysXNxCharCtrler*>::iterator itNxCCEnd = m_pVecNxCC.end();
	for (; itNxCC != itNxCCEnd; ++itNxCC)
	{
		APhysXNxCCImp* pNxCCImp = static_cast<APhysXNxCCImp*>(*itNxCC);
		pNxCCImp->TerrainChange(IsLoading, aabb, Scene);
	}
}

void APhysXCharCtrlerMgrImp::OnObjectChange(bool IsCreating, APhysXObject& rObjects)
{
	if (IsCreating)
		return;

	const APhysXScene* pObjScene = rObjects.GetPhysXScene();

	APhysXArray<APhysXCharacterController*>::const_iterator itCC = m_pVecCC.begin();
	APhysXArray<APhysXCharacterController*>::const_iterator itCCEnd = m_pVecCC.end();
	for (; itCC != itCCEnd; ++itCC)
	{
		APhysXCCImp* pCCImp = static_cast<APhysXCCImp*>(*itCC);
		if (&pCCImp->GetAPhysXScene() != pObjScene)
			continue;
		pCCImp->OnObjectDeleted(rObjects);
	}

	APhysXArray<APhysXLWCharCtrler*>::const_iterator itLWCC = m_pVecCCLW.begin();
	APhysXArray<APhysXLWCharCtrler*>::const_iterator itLWCCEnd = m_pVecCCLW.end();
	for (; itLWCC != itLWCCEnd; ++itLWCC)
	{
		APhysXLWCCImp* pLWCCImp = static_cast<APhysXLWCCImp*>(*itLWCC);
		if (&pLWCCImp->GetAPhysXScene() != pObjScene)
			continue;
		pLWCCImp->OnObjectDeleted(rObjects);
	}

	APhysXArray<APhysXDynCharCtrler*>::const_iterator itCCDyn = m_pVecCCDyn.begin();
	APhysXArray<APhysXDynCharCtrler*>::const_iterator itCCEndDyn = m_pVecCCDyn.end();
	for (; itCCDyn != itCCEndDyn; ++itCCDyn)
	{
		APhysXDynCCImp* pDynCCImp = static_cast<APhysXDynCCImp*>(*itCCDyn);
		if (&pDynCCImp->GetAPhysXScene() != pObjScene)
			continue;
		pDynCCImp->OnObjectDeleted(rObjects);
	}

	APhysXArray<APhysXNxCharCtrler*>::const_iterator itNxCC = m_pVecNxCC.begin();
	APhysXArray<APhysXNxCharCtrler*>::const_iterator itNxCCEnd = m_pVecNxCC.end();
	for (; itNxCC != itNxCCEnd; ++itNxCC)
	{
		APhysXNxCCImp* pNxCCImp = static_cast<APhysXNxCCImp*>(*itNxCC);
		if (&pNxCCImp->GetAPhysXScene() != pObjScene)
			continue;
		pNxCCImp->OnObjectDeleted(rObjects);
	}
}

bool APhysXCharCtrlerMgrImp::ClosestSweepExcludesAllCCs(const APhysXScene& scene, const NxCapsule& cap, const NxVec3& motion, NxU32 flags, NxSweepQueryHit& outResult) const
{
	NxU32 nActiveGroups = 0xffffffff;
	NxGroupsMask gmFilter;
	GetAllExcludesGroupAndMask(scene, nActiveGroups, gmFilter);
	APhysXScene* pScene = const_cast<APhysXScene*>(&scene);
	NxScene* pNxScene = pScene->GetNxScene();

	NxU32 nFgs = 0;
	if (NX_SF_STATICS & flags)
		nFgs |= NX_SF_STATICS;
	if (NX_SF_DYNAMICS & flags)
		nFgs |= NX_SF_DYNAMICS;

	NxU32 nHit = pNxScene->linearCapsuleSweep(cap, motion, nFgs, 0, 1, &outResult, 0, nActiveGroups, &gmFilter);
	return (0 == nHit)? false : true;
}

bool APhysXCharCtrlerMgrImp::ClosestRaycastExcludesAllCCs(const APhysXScene& scene, const NxVec3& rayOrig, const NxVec3& motion, NxShapesType flags, NxRaycastHit& inoutResult) const
{
	NxU32 nActiveGroups = 0xffffffff;
	NxGroupsMask gmFilter;
	GetAllExcludesGroupAndMask(scene, nActiveGroups, gmFilter);
	APhysXScene* pScene = const_cast<APhysXScene*>(&scene);
	NxScene* pNxScene = pScene->GetNxScene();

	NxRay ray;
	ray.orig = rayOrig;
	ray.dir = motion;
	float dist = ray.dir.normalize();

	inoutResult.shape = 0;
	NxU32 hintFlags = -1;
	if (0 != inoutResult.flags)
	{
		hintFlags = inoutResult.flags;
		if (0x000000ff & hintFlags)
			hintFlags = -1;
		inoutResult.flags = 0;
	}

	NxShape* pShape = pNxScene->raycastClosestShape(ray, flags, inoutResult, nActiveGroups, dist, hintFlags, &gmFilter, 0);
	return (0 == pShape)? false : true;
}

void APhysXCharCtrlerMgrImp::OnObjectChangeNotify(bool IsCreating, APhysXObject& rObjects)
{
	OnObjectChange(IsCreating, rObjects);

	if(!rObjects.IsStaticRB())
		return;

	m_StaticObjectsChanged = true;
	if(rObjects.IsObjectType(APX_OBJTYPE_TERRAIN_PATCH))
	{
		// Note: we assume the rObjects is one of active terrain object
		// at present APhysXEngine create terrain block in instant mode, but release terrain block by asynchronism(not real release until tick) mode.
		// so, if user release the terrain and reload it immediately, then the object create first and release it later. This action lead to CC's incorrect sleep.
		NxBounds3 aabbTP;
		rObjects.GetAABB(aabbTP);
		aabbTP.max.y += gTerrainHeightExtension;
		OnTerrainChange(IsCreating, aabbTP, *rObjects.GetPhysXScene());
	}
}

void APhysXCharCtrlerMgrImp::AddDebugData(APhysXDebugRenderer& drObj, const APhysXScene* pScene)
{
	static NxDebugPoint atPos;
	atPos.color = 0xffff0000;

	APhysXArray<APhysXCharacterController*>::const_iterator itCC = m_pVecCC.begin();
	APhysXArray<APhysXCharacterController*>::const_iterator itCCEnd = m_pVecCC.end();
	for (; itCC != itCCEnd; ++itCC)
	{
		APhysXCCImp* pCCImp = static_cast<APhysXCCImp*>(*itCC);
		if ((0 != pScene) && (pScene != &pCCImp->GetAPhysXScene()))
			continue;
		
		if (pCCImp->GetAttachmentPos(atPos.p))
			drObj.RenderData(&NxDebugRenderable(1, &atPos, 0, 0, 0, 0));
		
		if (APhysXCCBase::APX_CCS_JUMPING_INAIR & pCCImp->GetStatus())
			drObj.RenderCC(*pCCImp, 0xffd0843d);
		else
			drObj.RenderCC(*pCCImp, 0xff33ffff);
	}
	
	APhysXArray<APhysXLWCharCtrler*>::const_iterator it = m_pVecCCLW.begin();
	APhysXArray<APhysXLWCharCtrler*>::const_iterator itEnd = m_pVecCCLW.end();
	for (; it != itEnd; ++it)
	{
		if ((0 != pScene) && (pScene != &(*it)->GetAPhysXScene()))
			continue;
		
		drObj.RenderCC(*(*it), 0xff804040);
	}

	APhysXArray<APhysXDynCharCtrler*>::const_iterator itDyn = m_pVecCCDyn.begin();
	APhysXArray<APhysXDynCharCtrler*>::const_iterator itEndDyn = m_pVecCCDyn.end();
	for (; itDyn != itEndDyn; ++itDyn)
	{
		APhysXDynCCImp* pDynCCImp = static_cast<APhysXDynCCImp*>(*itDyn);
		if ((0 != pScene) && (pScene != &pDynCCImp->GetAPhysXScene()))
			continue;
		
		if (pDynCCImp->GetAttachmentPos(atPos.p))
			drObj.RenderData(&NxDebugRenderable(1, &atPos, 0, 0, 0, 0));

		if (APhysXCCBase::APX_CCS_JUMPING_INAIR & pDynCCImp->GetStatus())
			drObj.RenderCC(*pDynCCImp, 0xffd0843d);
		else
			drObj.RenderCC(*pDynCCImp, 0xff33ffff);
	}

	APhysXArray<APhysXNxCharCtrler*>::const_iterator itNxCC = m_pVecNxCC.begin();
	APhysXArray<APhysXNxCharCtrler*>::const_iterator itNxCCEnd = m_pVecNxCC.end();
	for (; itNxCC != itNxCCEnd; ++itNxCC)
	{
		APhysXNxCCImp* pNxCCImp = static_cast<APhysXNxCCImp*>(*itNxCC);
		if ((0 != pScene) && (pScene != &pNxCCImp->GetAPhysXScene()))
			continue;
		
		if (APhysXCCBase::APX_CCS_JUMPING_INAIR & pNxCCImp->GetStatus())
			drObj.RenderCC(*pNxCCImp, 0xffd08450);
		else
			drObj.RenderCC(*pNxCCImp, 0xff33ffff);
	}
}

void APhysXCharCtrlerMgrImp::SMaterial::CreateMaterial()
{
	assert(0 != m_pScene);
	if (0 == m_pScene)
		return;

	if (0 == m_pMaterial)
	{
		NxMaterialDesc mtDesc;
		mtDesc.dynamicFriction	= 0.0f;
		mtDesc.staticFriction	= 0.0f;
		mtDesc.restitution		= 0.0f; 
		mtDesc.frictionCombineMode = NX_CM_MIN;
		mtDesc.restitutionCombineMode  = NX_CM_MIN;
		m_pMaterial = m_pScene->GetNxScene()->createMaterial(mtDesc);
		if (0 != m_pMaterial)
			++m_UsingCount;
	}
}

void APhysXCharCtrlerMgrImp::SMaterial::ReleaseMaterial()
{
	if (0 == m_pScene)
	{
		assert(0 == m_pMaterial);
		return;
	}

	if (0 != m_pMaterial)
	{	
		--m_UsingCount;
		if (0 == m_UsingCount)
		{
			m_pScene->GetNxScene()->releaseMaterial(*m_pMaterial);
			m_pMaterial = 0;
			m_pScene = 0;
		}
	}
}

NxMaterialIndex APhysXCharCtrlerMgrImp::SMaterial::GetIndexMaterial()
{
	assert(0 != m_pScene);
	if (0 != m_pMaterial)
	{
		++m_UsingCount;
		return m_pMaterial->getMaterialIndex();
	}
	return 0;
}

NxMaterialIndex APhysXCharCtrlerMgrImp::GetSmoothMaterial(APhysXScene& scene)
{
	APhysXArray<SMaterial>::iterator it = m_matSmooth.begin();
	APhysXArray<SMaterial>::iterator itEnd = m_matSmooth.end();
	for (; it != itEnd; ++it)
	{
		if (&scene == it->GetAPhysXScene())
			return it->GetIndexMaterial();
	}

	SMaterial sm(scene);
	sm.CreateMaterial();
	m_matSmooth.push_back(sm);
	return sm.GetIndexMaterial();
}

void APhysXCharCtrlerMgrImp::ReleaseSmoothMaterial(APhysXScene& scene)
{
	APhysXArray<SMaterial>::iterator it = m_matSmooth.begin();
	APhysXArray<SMaterial>::iterator itEnd = m_matSmooth.end();
	for (; it != itEnd; ++it)
	{
		if (&scene == it->GetAPhysXScene())
		{
			it->ReleaseMaterial();
			assert(it->GetUsingCount() == CountDynCC(scene));
			return;
		}
	}
}

int APhysXCharCtrlerMgrImp::CountDynCC(APhysXScene& scene) const
{
	int ccCount = 0;
	APhysXArray<APhysXDynCharCtrler*>::const_iterator itCCDyn = m_pVecCCDyn.begin();
	APhysXArray<APhysXDynCharCtrler*>::const_iterator itCCEndDyn = m_pVecCCDyn.end();
	for (; itCCDyn != itCCEndDyn; ++itCCDyn)
	{
		if (&scene == &(*itCCDyn)->GetAPhysXScene())
			++ccCount;
	}
	return ccCount;
}

void APhysXCharCtrlerMgrImp::GetAllExcludesGroupAndMask(const APhysXScene& scene, NxU32& outActiveGroups, NxGroupsMask& outGM) const
{
	outActiveGroups = 0xffffffff;
	outGM.bits0 = outGM.bits1 = outGM.bits2 = outGM.bits3 = 0xffffffff;
	NxGroupsMask gm;
	const APhysXScene* pScene = &scene;
	APhysXCCBase* pCCBase = 0;

	APhysXArray<APhysXCharacterController*>::const_iterator itCC = m_pVecCC.begin();
	APhysXArray<APhysXCharacterController*>::const_iterator itCCEnd = m_pVecCC.end();
	for (; itCC != itCCEnd; ++itCC)
	{
		pCCBase = *itCC;
		if (&pCCBase->GetAPhysXScene() != pScene)
			continue;
		outActiveGroups &= pCCBase->GetActiveGroups();
		gm = pCCBase->GetFilterGM(true);
		outGM.bits0 &= gm.bits0;
		outGM.bits1 &= gm.bits1;
		outGM.bits2 &= gm.bits2;
		outGM.bits3 &= gm.bits3;
	}

	APhysXArray<APhysXLWCharCtrler*>::const_iterator itLWCC = m_pVecCCLW.begin();
	APhysXArray<APhysXLWCharCtrler*>::const_iterator itLWCCEnd = m_pVecCCLW.end();
	for (; itLWCC != itLWCCEnd; ++itLWCC)
	{
		pCCBase = *itLWCC;
		if (&pCCBase->GetAPhysXScene() != pScene)
			continue;
		outActiveGroups &= pCCBase->GetActiveGroups();
		gm = pCCBase->GetFilterGM(true);
		outGM.bits0 &= gm.bits0;
		outGM.bits1 &= gm.bits1;
		outGM.bits2 &= gm.bits2;
		outGM.bits3 &= gm.bits3;
	}

	APhysXArray<APhysXDynCharCtrler*>::const_iterator itCCDyn = m_pVecCCDyn.begin();
	APhysXArray<APhysXDynCharCtrler*>::const_iterator itCCEndDyn = m_pVecCCDyn.end();
	for (; itCCDyn != itCCEndDyn; ++itCCDyn)
	{
		pCCBase = *itCCDyn;
		if (&pCCBase->GetAPhysXScene() != pScene)
			continue;
		outActiveGroups &= pCCBase->GetActiveGroups();
		gm = pCCBase->GetFilterGM(true);
		outGM.bits0 &= gm.bits0;
		outGM.bits1 &= gm.bits1;
		outGM.bits2 &= gm.bits2;
		outGM.bits3 &= gm.bits3;
	}

	APhysXArray<APhysXNxCharCtrler*>::const_iterator itNxCC = m_pVecNxCC.begin();
	APhysXArray<APhysXNxCharCtrler*>::const_iterator itNxCCEnd = m_pVecNxCC.end();
	for (; itNxCC != itNxCCEnd; ++itNxCC)
	{
		pCCBase = *itNxCC;
		if (&pCCBase->GetAPhysXScene() != pScene)
			continue;
		outActiveGroups &= pCCBase->GetActiveGroups();
		gm = pCCBase->GetFilterGM(true);
		outGM.bits0 &= gm.bits0;
		outGM.bits1 &= gm.bits1;
		outGM.bits2 &= gm.bits2;
		outGM.bits3 &= gm.bits3;
	}
}
