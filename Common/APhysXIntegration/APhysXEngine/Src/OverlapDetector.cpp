#include "OverlapDetector.h"
#include <NxPhysics.h>

namespace APhysXUtility
{

// Do NOT assign gEpsilon < 0.000007f. (We get this value from the practical test.)
// Or it makes no sense for the PhysX simulation that you sweep a very very tiny distance
const float COverlapDetector::gEpsilon   = 0.000007f; 
const NxVec3 COverlapDetector::gvecZero(0.0f);
const NxReal COverlapDetector::gFrequency = 60.0f;

void COverlapDetector::Report::ClearResult()
{
	m_bResult = false;
	m_OwnHitShape.clear();
	m_OwnCollector.clear();
}

void COverlapDetector::Report::Initiate()
{
	ClearResult();
	m_Mode = MODE_DETECT_OVERLAP;
	m_pMain = 0;
	m_pTarget = 0;
	m_idxStart = 0;
	m_pHitShape = 0;
	m_pCollector = 0;
}

void COverlapDetector::Report::SetModeOverlap(const NxActor& main, const NxActor& Target, const bool bGetDetails)
{
	Initiate();
	m_Mode = MODE_DETECT_OVERLAP;
	m_pMain = &main;
	m_pTarget = &Target;
	if (bGetDetails)
	{
		m_pHitShape = &m_OwnHitShape;
		m_idxStart = m_pHitShape->size();
		m_pCollector = &m_OwnCollector;
	}
}

void COverlapDetector::Report::SetModeSweep(const NxActor& main, NxArray<NxShape*>& HitShapes, NxArray<COverlapDetailInfo>* pOutInfo)
{
	Initiate();
	m_Mode = MODE_DETECT_LINEARSWEEP;
	m_pMain = &main;
	m_pHitShape = &HitShapes;
	m_idxStart = m_pHitShape->size();
	m_pCollector = pOutInfo;
}

bool COverlapDetector::Report::IsExpected(const NxActor* pActor0, const NxActor* pActor1) const
{
	bool bHasMainActor = false;
	if (pActor0 == m_pMain) bHasMainActor = true;
	if (pActor1 == m_pMain) bHasMainActor = true;
	if (MODE_DETECT_LINEARSWEEP == m_Mode)
		return bHasMainActor;

	bool bHasTargetActor = false;
	if (pActor0 == m_pTarget) bHasTargetActor = true;
	if (pActor1 == m_pTarget) bHasTargetActor = true;
	if (MODE_DETECT_OVERLAP == m_Mode)
		return (bHasMainActor && bHasTargetActor);

	return false;
}

int COverlapDetector::Report::AddShape(NxShape& shape)
{
	m_bResult = true;

	const int userData = reinterpret_cast<const int>(shape.userData);
	if (0 < userData)
		return userData - 1;

	assert(0 != m_pHitShape);
	if (0 == m_pHitShape)
		return -1;

	m_pHitShape->pushBack(&shape);

	const int nSize = m_pHitShape->size();
	shape.userData = reinterpret_cast<void*>(nSize);
	return nSize - 1;
}

void COverlapDetector::Report::AddDetailInfo(const int index, const COverlapDetailInfo& diVal)
{
	if (0 == m_pCollector)
		return;

	assert(0 <= index);
	if (0 > index)
		return;

	int nSize = m_pCollector->size();
	assert(index <= nSize);
	if (index > nSize)
		return;

	if (index < nSize)
		(*m_pCollector)[index] += diVal;
	else
		m_pCollector->push_back(diVal);
}

void COverlapDetector::Report::onContactNotify(NxContactPair& pair, NxU32 events)
{
	if(pair.isDeletedActor[0] || pair.isDeletedActor[1])
		return;

	if (!IsExpected(pair.actors[0], pair.actors[1]))
		return;

	const bool bIsGetDetail((0 == m_pCollector)? false : true);
	if (MODE_DETECT_OVERLAP == m_Mode)
	{
		m_bResult = true;
		if (!bIsGetDetail)
			return;
	}

	NxContactStreamIterator i(pair.stream);
	while(i.goNextPair()) 
	{
		// Need to visit the interface of isDeletedShape() in next version of PhysX SDK!!
#if NX_SDK_VERSION_NUMBER > 281
		// those codes should be revised in the future. 
		if(i.isDeletedShape(0) || i.isDeletedShape(1))
		{
			// DO NOT skip those iteration!!
			// here is the limitation of PhysX SDK. See the doc of NxContactStreamIterator.
			while(i.goNextPatch())
			{
				while(i.goNextPoint()) {}
			}
			continue;
		}
#endif

		bool NegFlag = false;
		NxShape* pShape0 = i.getShape(0);
		NxShape* pShape1 = i.getShape(1);
		NxShape* pOtherActorShape = 0;

		if (&pShape0->getActor() == m_pMain)
			pOtherActorShape = pShape1;
		else 
		{
			assert(&pShape1->getActor() == m_pMain);
			pOtherActorShape = pShape0;
			NegFlag = true;
		}
		const int idx = AddShape(*pOtherActorShape);

		COverlapDetailInfo temp;
		while(i.goNextPatch())
		{
			if (bIsGetDetail)
			{
				NxVec3 cn = i.getPatchNormal();
				if (NegFlag) cn *= -1;
				temp.avgNormal += cn;
			}

			// DO NOT skip those iteration!!
			// here is the limitation of PhysX SDK. See the doc of NxContactStreamIterator.
			while(i.goNextPoint()) {}
		}
		
		AddDetailInfo(idx, temp);
	}
}

void COverlapDetector::Report::Translation() const
{
	if (0 == m_pHitShape)
		return;

	const NxU32 nSize = m_pHitShape->size();

	if (&m_OwnHitShape == m_pHitShape)
	{
		for (NxU32 i = m_idxStart; i < nSize; ++i)
			(*m_pHitShape)[i]->userData = 0;
		return;
	}

	NxU32 idx = 0;
	for (NxU32 i = m_idxStart; i < nSize; ++i)
	{
		NxShape* pShape = (*m_pHitShape)[i];
		pShape->userData = 0;
		NxActor& ghostActor = pShape->getActor();

		NxU32 nShapes = ghostActor.getNbShapes();
		NxShape*const * ppShapes = ghostActor.getShapes();
		for (idx = 0; idx < nShapes; ++idx)
		{
			if (ppShapes[idx] == pShape)
				break;
		}

		assert(idx < nShapes);
		if (idx < nShapes)
		{
			NxActor* pActor = static_cast<NxActor*>(ghostActor.userData);
			assert(nShapes == pActor->getNbShapes());
			if (idx < pActor->getNbShapes())
				(*m_pHitShape)[i] = pActor->getShapes()[idx];
			else
				(*m_pHitShape)[i] = 0;
		}
		else
		{
			assert(false);
			(*m_pHitShape)[i] = 0;
		}
	}
}

void COverlapDetector::Report::NormalizeDetails() const
{
	if (0 == m_pCollector)
		return;

	NxU32 nSize = m_pCollector->size();
	for (NxU32 i = 0; i < nSize; ++i)
		(*m_pCollector)[i].avgNormal.normalize();
}

bool COverlapDetector::Report::GetDetails(COverlapDetailInfo& OutInfo) const
{
	OutInfo.Zero();
	assert(0 != m_pCollector);
	if (0 == m_pCollector)
		return false;

	NxU32 nSize = m_pCollector->size();
	for (NxU32 i = 0; i < nSize; ++i)
		OutInfo += (*m_pCollector)[i];
	
	OutInfo.avgNormal.normalize();
	return true;
}

COverlapDetector::StreamNotify::~StreamNotify()
{
	ReleaseAllConvexMesh();
	ReleaseAllHeightField();
	ReleaseAllTriangleMesh();
}

void COverlapDetector::StreamNotify::NXU_errorMessage(bool isError, const char* str)
{
	if (isError)
		m_HasError = true;
}

bool COverlapDetector::StreamNotify::NXU_preNotifyActor(NxActorDesc& actor, const char* userProperties)
{
	actor.group = 0;
	actor.flags &= ~NX_AF_DISABLE_COLLISION;
	sscanf(userProperties, "%p", &actor.userData);

	if (0 != actor.body)
	{
		NxBodyDesc* pBody = const_cast<NxBodyDesc*>(actor.body);
		pBody->flags |= NX_BF_KINEMATIC;
		pBody->contactReportThreshold = 0;
		// make sure actor's velocity were cleared
		// in some special situations, the global position was changed by the angular velocity
		// This is a limitation(or bug) of PhysX SDK.
		pBody->linearVelocity.set(0, 0, 0);
		pBody->angularVelocity.set(0, 0, 0);
	}

	NxActor* pSrcActor = static_cast<NxActor*>(actor.userData);

	int nSize = actor.shapes.size();
	for (int i = 0; i< nSize; ++i)
	{
		NxShapeDesc* pDesc = actor.shapes[i];
		pDesc->userData = 0;
		pDesc->group = 0;
		pDesc->groupsMask.bits0 = 0;
		pDesc->groupsMask.bits1 = 0;
		pDesc->groupsMask.bits2 = 0;
		pDesc->groupsMask.bits3 = 0;
		pDesc->shapeFlags &= ~NX_SF_DISABLE_COLLISION;
		ChangeToSourceMeshData(*pDesc, *pSrcActor, i);
	}
	return true;
}

void COverlapDetector::StreamNotify::NXU_notifyConvexMesh(NxConvexMesh* c, const char* userProperties)
{
	m_ConvexMesh.push_back(c);
}

void COverlapDetector::StreamNotify::NXU_notifyHeightField(NxHeightField* t, const char* userProperties)
{
	m_HeightField.push_back(t); 
} 

void COverlapDetector::StreamNotify::NXU_notifyTriangleMesh(NxTriangleMesh* t, const char* userProperties)
{
	m_TriangleMesh.push_back(t); 
} 

void COverlapDetector::StreamNotify::ChangeToSourceMeshData(NxShapeDesc& descShape, const NxActor& srcActor, const unsigned int index)
{
	assert(index < srcActor.getNbShapes());
	NxShape* pShape = srcActor.getShapes()[index];

	NxConvexShape       *pConvexShape       = pShape->isConvexMesh();
	NxHeightFieldShape  *pHeightFieldShape  = pShape->isHeightField();
	NxTriangleMeshShape *pTriangleMeshShape = pShape->isTriangleMesh();

	if(0 != pConvexShape)
	{
		NxConvexShapeDesc& rGhostConDesc = static_cast<NxConvexShapeDesc&>(descShape);
		rGhostConDesc.meshData = &pConvexShape->getConvexMesh();
	}
	else if(0 != pHeightFieldShape)
	{
		NxHeightFieldShapeDesc& rGhostHFDesc = static_cast<NxHeightFieldShapeDesc&>(descShape);
		rGhostHFDesc.heightField = &pHeightFieldShape->getHeightField();
	}
	else if(0 != pTriangleMeshShape)
	{
		NxTriangleMeshShapeDesc& rGhostTriDesc = static_cast<NxTriangleMeshShapeDesc&>(descShape);
		rGhostTriDesc.meshData = &pTriangleMeshShape->getTriangleMesh();
	}
}

void COverlapDetector::StreamNotify::ReleaseAllConvexMesh()
{
	NxArray<NxConvexMesh*>::const_iterator it = m_ConvexMesh.begin();
	NxArray<NxConvexMesh*>::const_iterator itEnd = m_ConvexMesh.end();
	for (; it != itEnd; ++it)
	{
		assert(0 == (*it)->getReferenceCount());
		m_PhysXSDK.releaseConvexMesh(**it);
	}
}

void COverlapDetector::StreamNotify::ReleaseAllHeightField()
{
	NxArray<NxHeightField*>::const_iterator it = m_HeightField.begin();
	NxArray<NxHeightField*>::const_iterator itEnd = m_HeightField.end();
	for (; it != itEnd; ++it)
	{
		assert(0 == (*it)->getReferenceCount());
		m_PhysXSDK.releaseHeightField(**it);
	}
}

void COverlapDetector::StreamNotify::ReleaseAllTriangleMesh()
{
	NxArray<NxTriangleMesh*>::const_iterator it = m_TriangleMesh.begin();
	NxArray<NxTriangleMesh*>::const_iterator itEnd = m_TriangleMesh.end();
	for (; it != itEnd; ++it)
	{
		assert(0 == (*it)->getReferenceCount());
		m_PhysXSDK.releaseTriangleMesh(**it);
	}
}

COverlapDetector::COverlapDetector()
{
	m_pScene = 0;
}

COverlapDetector::~COverlapDetector()
{
	// do not release the ghost scene here, since this maybe a global objects...
	// ReleaseScene();
}

void COverlapDetector::ReleaseScene()
{
	if (0 != m_pScene)
	{
		m_pScene->getPhysicsSDK().releaseScene(*m_pScene);
		m_pScene = 0;
	}
}

bool COverlapDetector::CheckOverlap(const NxActor& actor0, const NxActor& actor1, ErrorCode& state, COverlapDetailInfo* pOutInfo)
{
	if (&actor0 == &actor1)
		return true;

	ActorInfo info;
	if (!PreDetection(state, info, actor0, &actor1))
		return false;

	const bool bIsGetDetail((0 == pOutInfo)? false : true);
	m_Receiver.SetModeOverlap(*info.pMain, *info.pTarget, bIsGetDetail);
	SceneSimulate(*m_pScene);

	bool rtn = PostDetection(info);
	if (bIsGetDetail)
		m_Receiver.GetDetails(*pOutInfo);
	return rtn;
}

bool COverlapDetector::Inflate6DCheckOverlap(const NxActor& actor0, const NxActor& actor1, float& growSize, ErrorCode& state, const NxMat33* pRotation, COverlapDetailInfo* pOutInfo)
{
	if (&actor0 == &actor1)
		return true;

	if ((0 > growSize) || (NxMath::equals(growSize, 0, gEpsilon)))
		growSize = 0;

	ActorInfo info;
	if (!PreDetection(state, info, actor0, &actor1))
		return false;

	const bool bIsGetDetail((0 == pOutInfo)? false : true);
	m_Receiver.SetModeOverlap(*info.pMain, *info.pTarget, bIsGetDetail);
	DoInflateDetectKernel(*info.pMain, growSize, pRotation);

	bool rtn = PostDetection(info);
	if (bIsGetDetail)
		m_Receiver.GetDetails(*pOutInfo);
	return rtn;
}

bool COverlapDetector::Inflate6DCheckOverlap(const NxActor& actor, float& growSize, NxArray<NxShape*>& HitShapes, ErrorCode& state, const NxMat33* pRotation, NxArray<COverlapDetailInfo>* pOutInfo)
{
	if ((0 > growSize) || (NxMath::equals(growSize, 0, gEpsilon)))
		growSize = 0;

	ActorInfo info;
	if (!PreDetection(state, info, actor))
		return false;

	m_Receiver.SetModeSweep(*info.pMain, HitShapes, pOutInfo);
	DoInflateDetectKernel(*info.pMain, growSize, pRotation);

	return PostDetection(info);
}

bool COverlapDetector::LinearSweep(const NxActor& actor, const NxVec3& dir, const float distance, NxArray<NxShape*>& HitShapes, ErrorCode& state, NxArray<COverlapDetailInfo>* pOutInfo)
{
	NxVec3 vDir(dir);
	vDir.normalize();
	return LinearSweep(actor, vDir * distance, HitShapes, state, pOutInfo);
}

bool COverlapDetector::LinearSweep(const NxActor& actor, const NxVec3& motion, NxArray<NxShape*>& HitShapes, ErrorCode& state, NxArray<COverlapDetailInfo>* pOutInfo)
{
	ActorInfo info;
	if (!PreDetection(state, info, actor))
		return false;

	m_Receiver.SetModeSweep(*info.pMain, HitShapes, pOutInfo);
	DoSweepKernel(*info.pMain, motion);

	return PostDetection(info);
}

bool COverlapDetector::SnapshotScene(NxScene& scene, ErrorCode& state)
{
	static const NxU32 flag = NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH;

	ReleaseScene();
	m_pScene = scene.getPhysicsSDK().createScene(NxSceneDesc());
	if (0 == m_pScene)
	{	
		state = E_CREATE_SCENE_FAIL;
		return false;
	}
	m_pScene->setActorGroupPairFlags(0, 0, flag);

	StreamNotify theNotify(scene.getPhysicsSDK());
	if (!CloneAllActors(scene, theNotify) || theNotify.HasError())
	{
		ReleaseScene();
		state = E_MAP_ACTORS_FAIL;
		return false;
	}

	m_pScene->setUserContactReport(const_cast<Report*>(&m_Receiver));
	return true;
}

bool COverlapDetector::CloneAllActors(NxScene& srcScene, StreamNotify& notify)
{
	assert(0 != m_pScene);
	if (0 == m_pScene)
		return false;
	assert(0 == m_pScene->getNbActors());

	NxPhysicsSDK& PhysXSDK = srcScene.getPhysicsSDK();
	NXU::NxuPhysicsCollection* pCollection = NXU::createCollection(); 
	if (0 == pCollection)
		return false;

	char strProperty[40];
	NxU32 nbActors = srcScene.getNbActors();
	NxActor** ppActors = srcScene.getActors();
	for (NxU32 i = 0; i < nbActors; ++i)
	{
		sprintf(strProperty, "%p", ppActors[i]);
		NXU::addActor(*pCollection, *ppActors[i], strProperty);
	}

	bool rtn = NXU::instantiateCollection(pCollection, PhysXSDK, m_pScene, 0, &notify);
	NXU::releaseCollection(pCollection);
	NXU::releasePersistentMemory();

	return rtn;
}

bool COverlapDetector::PreDetection(ErrorCode& state, ActorInfo& outInfo, const NxActor& actor0, const NxActor* pActor1)
{
	state = E_SUCCESS;

	if (0 != pActor1)
	{
		if (&actor0.getScene() != &pActor1->getScene())
		{
			state = E_DIFFERENT_SCENE_ACTORS;
			return false;
		}
	}

	if (HasInvalditeShape(actor0, state))
		return false;

	if (0 == m_pScene)
	{
		if (!SnapshotScene(actor0.getScene(), state))
			return false;
	}

	outInfo.pMain = FindActor(actor0);
	if (0 == outInfo.pMain)
	{
		state = E_MAP_ACTORS_FAIL;
		return false;
	}

	if (0 != pActor1)
	{
		outInfo.pTarget = FindActor(*pActor1);
		if (0 == outInfo.pTarget)
		{
			state = E_MAP_ACTORS_FAIL;
			return false;
		}
	}

	MainActorMakeup(*outInfo.pMain, outInfo);
	if (0 == outInfo.pMain)
	{
		state = E_MAP_ACTORS_FAIL;
		return false;
	}

	return true;
}

bool COverlapDetector::PostDetection(const ActorInfo& restoreInfo) const
{
	MainActorRemover(restoreInfo);
	return m_Receiver.GetResult();
}

bool COverlapDetector::HasInvalditeShape(const NxActor& actor, ErrorCode& state) const
{
	NxU32 nbActors = actor.getNbShapes();
	NxShape*const* ppShapes = actor.getShapes();
	for (NxU32 i = 0; i < nbActors; ++i)
	{
		if (0 != ppShapes[i]->isPlane())
		{
			state = E_HAS_PLANE_SHAPE;
			return true;
		}
		if (0 != ppShapes[i]->isTriangleMesh())
		{
			state = E_HAS_TRIANGLE_SHAPE;
			return true;
		}
		if (0 != ppShapes[i]->isHeightField())
		{
			state = E_HAS_HEIGHTFIELD_SHAPE;
			return true;
		}
	}
	return false;
}

NxActor* COverlapDetector::FindActor(const NxActor& srcActor) const
{
	assert(0 != m_pScene);
	if (0 == m_pScene)
		return 0;

	NxU32 nbActors = m_pScene->getNbActors();
	NxActor** ppActors = m_pScene->getActors();
	for (NxU32 i = 0; i < nbActors; ++i)
	{
		NxActor* pActor = static_cast<NxActor*>(ppActors[i]->userData);
		if (&srcActor == pActor)
			return ppActors[i];
	}
	return 0;
}

void COverlapDetector::MainActorRemover(const ActorInfo& restore) const
{
	assert(0 != restore.pMain);
	if (0 == restore.pMain)
		return;

	restore.pMain->setGlobalPosition(restore.posMain);
	restore.pMain->raiseBodyFlag(NX_BF_KINEMATIC);
	restore.pMain->putToSleep();
	SceneSimulate(restore.pMain->getScene());
}

void COverlapDetector::MainActorMakeup(NxActor& actor, ActorInfo& outInfo) const
{
	outInfo.pMain = &actor;
	outInfo.posMain = actor.getGlobalPosition();
	if (!actor.isDynamic())
	{
		NxBodyDesc bodyDesc;
		NxActorDesc actorDesc;
		actor.saveToDesc(actorDesc);
		actorDesc.body = &bodyDesc;
		actorDesc.density = 1;
		outInfo.pMain = CopyShapeDescAndCreateActor(actorDesc, actor);
		if (0 == outInfo.pMain)
			return;

		actor.getScene().releaseActor(actor);
	}

	outInfo.pMain->clearBodyFlag(NX_BF_KINEMATIC);
	outInfo.pMain->raiseActorFlag(NX_AF_DISABLE_RESPONSE);
	outInfo.pMain->setLinearDamping(0);
	// make sure the main actor is awake during the whole detection
	outInfo.pMain->wakeUp(100000000);
}

NxActor* COverlapDetector::CopyShapeDescAndCreateActor(NxActorDesc& actorDesc, const NxActor& srcActor) const
{
	NxU32 nShapes = srcActor.getNbShapes();
	for (NxU32 i = 0; i < nShapes; ++i)
	{
		NxShape* pShape = srcActor.getShapes()[i];

		// Note: omit the wheel shape and triangle mesh shape on purpose
		NxBoxShape          *pBoxShape          = pShape->isBox();
		NxCapsuleShape      *pCapsuleShape      = pShape->isCapsule();
		NxSphereShape       *pSphereShape       = pShape->isSphere();
		NxConvexShape       *pConvexShape       = pShape->isConvexMesh();
		NxHeightFieldShape  *pHeightFieldShape  = pShape->isHeightField();
		NxPlaneShape        *pPlaneShape        = pShape->isPlane();

		if(0 != pConvexShape)
		{
			NxConvexShapeDesc desc;
			pConvexShape->saveToDesc(desc);
			actorDesc.shapes.pushBack(&desc);
		}
		else if(0 != pBoxShape)
		{
			NxBoxShapeDesc desc;
			pBoxShape->saveToDesc(desc);
			actorDesc.shapes.pushBack(&desc);
		}
		else if(0 != pCapsuleShape)
		{
			NxCapsuleShapeDesc desc;
			pCapsuleShape->saveToDesc(desc);
			actorDesc.shapes.pushBack(&desc);
		}
		else if(0 != pSphereShape)
		{
			NxSphereShapeDesc desc;
			pSphereShape->saveToDesc(desc);
			actorDesc.shapes.pushBack(&desc);
		}
		else if(0 != pHeightFieldShape)
		{
			NxHeightFieldShapeDesc desc;
			pHeightFieldShape->saveToDesc(desc);
			actorDesc.shapes.pushBack(&desc);
		}
		else if(0 != pPlaneShape)
		{
			NxPlaneShapeDesc desc;
			pPlaneShape->saveToDesc(desc);
			actorDesc.shapes.pushBack(&desc);
		}
	}
	if (0 == actorDesc.shapes.size())
		return 0;

	return srcActor.getScene().createActor(actorDesc);
}

void COverlapDetector::DoSweepKernel(NxActor& mainActor, const NxVec3& motion) const
{
	const NxVec3 dirOriginal = ChangeVelocity(mainActor, motion);
	NxVec3 dirLast(0.0f);
	NxVec3 dirNow(dirOriginal);

	NxVec3 posNow = mainActor.getGlobalPosition();
	const NxVec3 vPosAim = posNow + motion;

	NxScene& scene = mainActor.getScene();
	do 
	{
		SceneSimulate(scene);
		posNow = mainActor.getGlobalPosition();
		dirNow = vPosAim - posNow;

		// Note: don't use the "threshold compare" method on purpose
		// just want check identity exactly
		if (dirLast == dirNow)  
			break;
		dirLast = dirNow;
	} while (!IsOppositeDirection(dirOriginal, dirNow));

	// Note: pin down main actor then do the simulation to generate the callback for the latest position
	mainActor.setLinearVelocity(gvecZero);
	SceneSimulate(scene);
}

void COverlapDetector::DoInflateDetectKernel(NxActor& mainActor, const float growSize, const NxMat33* pRotation) const
{
	const NxVec3 vPosMAOriginal = mainActor.getGlobalPosition();
	NxVec3 motion;

	NxScene& scene = mainActor.getScene();

	// -X
	motion.set(-growSize, 0, 0);
	if (0 != pRotation) pRotation->multiply(motion, motion);
	DoSweepKernel(mainActor, motion);
	mainActor.setGlobalPosition(vPosMAOriginal);
	SceneSimulate(scene);

	// +X
	motion.set(growSize, 0, 0);
	if (0 != pRotation) pRotation->multiply(motion, motion);
	DoSweepKernel(mainActor, motion);
	mainActor.setGlobalPosition(vPosMAOriginal);
	SceneSimulate(scene);

	// -Y
	motion.set(0, -growSize, 0);
	if (0 != pRotation) pRotation->multiply(motion, motion);
	DoSweepKernel(mainActor, motion);
	mainActor.setGlobalPosition(vPosMAOriginal);
	SceneSimulate(scene);

	// +Y
	motion.set(0, growSize, 0);
	if (0 != pRotation) pRotation->multiply(motion, motion);
	DoSweepKernel(mainActor, motion);
	mainActor.setGlobalPosition(vPosMAOriginal);
	SceneSimulate(scene);

	// -Z
	motion.set(0, 0, -growSize);
	if (0 != pRotation) pRotation->multiply(motion, motion);
	DoSweepKernel(mainActor, motion);
	mainActor.setGlobalPosition(vPosMAOriginal);
	SceneSimulate(scene);

	// +Z
	motion.set(0, 0, growSize);
	if (0 != pRotation) pRotation->multiply(motion, motion);
	DoSweepKernel(mainActor, motion); 
}

void COverlapDetector::SceneSimulate(NxScene& scene) const
{
	static const NxReal timeStep = 1.0f / gFrequency;

	scene.simulate(timeStep);	
	scene.flushStream();
	scene.fetchResults(NX_RIGID_BODY_FINISHED, true);
}

NxVec3 COverlapDetector::ChangeVelocity(NxActor& actor, const NxVec3& motion) const
{
	NxVec3 vDir = motion;
	if (vDir.equals(gvecZero, gEpsilon))
		vDir.set(0, 0, 0);

	NxReal m = vDir.magnitude() * gFrequency;
	while (30 < m)
		m *= 0.5;

	vDir.normalize();
	actor.setLinearVelocity(vDir * m);
	return vDir;
}

bool COverlapDetector::IsOppositeDirection(const NxVec3& dir1, const NxVec3& dir2) const
{
	NxVec3 nDir1(dir1);
	NxVec3 nDir2(dir2);

	nDir1.normalize();
	nDir2.normalize();

	float dot = nDir1.dot(nDir2);

	if (NxMath::equals(dot, 0, gEpsilon))
		return true;

	if (0 > dot)
	{
		if (NxMath::equals(dot, -1, gEpsilon))
			return true;
	}
	return false; 
}

};