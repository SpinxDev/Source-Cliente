#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerAssistance.h"
#include "..\..\APhysXNxCharacter_Public\include\NxController.h"

float GetCosValue(const NxVec3& v0, const NxVec3& v1)
{
	NxVec3 V0(v0);
	NxVec3 V1(v1);
	
	V0.normalize();
	V1.normalize();
	return V0.dot(V1);
}

NxVec3 GetVerticalSubValue(const NxVec3& val, const NxVec3& accG)
{
	NxVec3 g(accG);
	g.normalize();
	
	const float cosRes = GetCosValue(val, g);
	return g * val.magnitude() * cosRes;	
}

void AttachedActor::SetToInvalid()
{
	pActor = 0;
	localPos.zero();
	localDir.zero();

	pose.zero();
	globalPos.zero();
	velPT.zero();
}

void AttachedActor::UpdateInfo(const NxVec3& attachedPos, const NxVec3& vDir, const NxActor& actor)
{
	pActor = &actor;

	globalPos = attachedPos;
	pose = actor.getGlobalPose();
	
	NxMat34 mat;
	pose.getInverse(mat);
	mat.multiply(globalPos, localPos);
	mat.M.multiply(vDir, localDir);

	velPT = pActor->getPointVelocity(globalPos);
	if (APHYSX_FLOAT_EPSILON > velPT.magnitudeSquared())
		velPT.zero();
}

void AttachedActor::UpdateInfo(const NxVec3& CurrentFootPos, const AttachedActor& LastAA)
{
	assert(LastAA.IsValid());
	if (!LastAA.IsValid())
		return;

	pActor = LastAA.pActor;

	globalPos = CurrentFootPos;
	pose = LastAA.pose;

	localPos = LastAA.localPos;
	localDir = LastAA.localDir;
	
	velPT = LastAA.velPT;
}

bool AttachedActor::IsDeleted(const APhysXObject& obj, NxVec3* pOutDelVel)
{
	if (0 != pOutDelVel)
		pOutDelVel->zero();
	
	if (!IsValid())
		return false;
	
	APhysXObject* pObj = static_cast<APhysXObject*>(pActor->userData);
	if (pObj != &obj)
		return false;
		
	// attached actor will be deleted
	if (0 != pOutDelVel)
		*pOutDelVel = velPT;
	SetToInvalid();
	return true;
}

const NxVec3 FrameInfo::gZero = NxVec3(0.0f);

FrameInfo::FrameInfo() : forceHorizontal(ForceRecorder::FRM_AVERAGE), forceVertical(ForceRecorder::FRM_GETMAX)
{
	SetToInvalid();
}

void FrameInfo::SetToInvalid()
{
	StatusFlags = 0;
	RaiseStatus(IS_IN_AIR);

	timeSec = 0;
	gravity.zero();
	velCC.zero();
	InputMoveDir.zero();

	PutToSleep();
	PushingActor.clear();
}

void FrameInfo::SetAAToInvalid()
{
	HitCosVal = -1;
	pAttaching = 0;
	AA.SetToInvalid();
}

void FrameInfo::PutToSleep()
{
	DispFMove.zero();
	DispFMoveUnfinished.zero();
	DispFMoveCandidate.zero();
	SetAAToInvalid();
	if (JumpingTakeOffBackUp.IsValid())
		JumpingTakeOffBackUp.SetToInvalid();
}

bool FrameInfo::GetStatus(const StatusInfo flag) const
{
	return (StatusFlags & flag)? true : false;
}

void FrameInfo::ClearStatus(const StatusInfo flag)
{
	StatusFlags &= ~flag;
}

void FrameInfo::RaiseStatus(const StatusInfo flag)
{
	StatusFlags |= flag;
}

void FrameInfo::CheckActorIsDeleted(const APhysXObject& obj)
{
	if (!AA.IsValid())
		return;
	
	NxVec3 vel;
	if (!AA.IsDeleted(obj, &vel))
		return;
	
	if (&AA == pAttaching)
	{
		if (GetStatus(IS_JUMPING_LANDING))
			velCC.zero();
		
		RaiseStatus(ATTACHMENT_IS_DELETED);
		RaiseStatus(IS_IN_AIR);
		velCC += vel;
		pAttaching = 0;
	}
}

bool FrameInfo::GetAttachedPoint(NxVec3& outPT) const
{
	AttachedActor* pBinding = GetAttachmentBinding();
	if (0 == pBinding)
		return false;

	// Do Not get latest position, such as:  
	// pBinding->pActor->getGlobalPose().multiply(pBinding->localPos, outPT);
	// that lead to crash when pActor was released.
	// Because of this function was exposed by APhysXCCImp::GetAttachmentPos
	// which maybe invoked by other threads. (e.g. render)
	outPT = pBinding->globalPos;
	return true;
}

bool FrameInfo::GetAttachedDir(NxVec3& outDir) const
{
	AttachedActor* pBinding = GetAttachmentBinding();
	if (0 == pBinding)
		return false;
	
	// This function was designed to using by APhysXCCImp internal.
	// Using API getGlobalPose() instead of pose member variable to get latest dir
	pBinding->pActor->getGlobalPose().M.multiply(pBinding->localDir, outDir);
	return true;
}

AttachedActor* FrameInfo::GetAttachmentBinding() const
{
	if (0 != pAttaching)
	{
		assert(pAttaching->IsValid());
	}

	return pAttaching;
}

const AttachedActor* FrameInfo::GetAttachmentCandidate() const
{
	return AA.IsValid()? &AA : 0;
}

const AttachedActor* FrameInfo::GetJumpingTakeOffBackUp() const
{
	return JumpingTakeOffBackUp.IsValid()? &JumpingTakeOffBackUp : 0;
}

void FrameInfo::UpdateCandidateInfo(const AttachedActor& LastAA, const NxVec3& CurrentFootPos, const bool EnableAttach)
{
	AA.UpdateInfo(CurrentFootPos, LastAA);
	pAttaching = 0;
	if (EnableAttach && AA.IsValid())
		pAttaching = &AA;
}

void FrameInfo::UpdateCandidateInfo(const NxActor& actor, const NxVec3& attachedPos, const NxVec3& faceDir, const bool EnableAttach)
{
	AA.UpdateInfo(attachedPos, faceDir, actor);
	pAttaching = 0;
	if (EnableAttach && !GetStatus(IS_IN_AIR))
		pAttaching = &AA;
}

NxVec3 FrameInfo::GetInAirReviseMove()
{
	if (GetStatus(IS_IN_AIR))
		return DispFMoveCandidate;

	return gZero;
}

NxVec3 FrameInfo::GetForecastMove(float threshold)
{
	if (GetStatus(IS_IN_AIR))
	{
		if (GetStatus(IS_JUMPING_TAKEOFF))
		{
			if (JumpingTakeOffBackUp.IsValid())
				return JumpingTakeOffBackUp.velPT * timeSec;
		}
	}
	else
	{
		AttachedActor* pBinding = GetAttachmentBinding();
		if (0 != pBinding)
		{
			float m = pBinding->velPT.magnitude();
			if (threshold < m)
			{
				//AString str;
				//str.Format("threshold: %f, mV: %f\n", threshold, m);
				//OutputDebugStringA(str);
				return pBinding->velPT * timeSec;
			}
		}
	}
	return gZero;
}

void FrameInfo::UpdateStateByFlags(const NxU32 CollisionFlags)
{
	bool zeroVV = false;

	if (NXCC_COLLISION_UP & CollisionFlags)
	{
		float cosRes = GetCosValue(velCC, gravity);
		if (0 > cosRes)
			zeroVV = true;
	}
	
	if (NXCC_COLLISION_DOWN & CollisionFlags)
		ClearStatus(IS_IN_AIR);
	
	if (zeroVV || !GetStatus(IS_IN_AIR))
	{
		velCC -= GetVerticalSubValue(velCC, gravity);
		if (APHYSX_FLOAT_EPSILON > velCC.magnitudeSquared())
			velCC.zero();
	}
}

void FrameInfo::UpdateStateByFrame(const FrameInfo& FromLastFrame)
{
	if(GetStatus(IS_IN_AIR))
	{
		if (!FromLastFrame.GetStatus(IS_IN_AIR))
		{
			if (GetStatus(INCREASED_JUMPING_VELOCITY))
			{
				RaiseStatus(IS_JUMPING_TAKEOFF);
				AttachedActor* pBinding = FromLastFrame.GetAttachmentBinding();
				if (0 != pBinding)
					JumpingTakeOffBackUp = *pBinding;
			}
		}
	}
	else
	{
		if (FromLastFrame.GetStatus(IS_IN_AIR))
		{
			RaiseStatus(IS_JUMPING_LANDING);

			// At the beginning of jumping-takeoff, CC has taken count in the velocity of binding point. In the moment of jumping-landing,
			// CC has finished the movement, but the binding actor hasn't moved(not simulate yet).  So, the current frame binding info is 
			// different from the binding info of jumping-takeoff. In the average velocity binding movement, to keep the same binding info
			// between jump-start and jump-land, we using the last frame candidate local info instead of current binding info.
			const AttachedActor* pLastCandidate = FromLastFrame.GetAttachmentCandidate();
			if ((0 != pLastCandidate) && (0 != pAttaching))
			{
				if (pLastCandidate->pActor == pAttaching->pActor)
				{
					pAttaching->localPos = pLastCandidate->localPos;
					pAttaching->localDir = pLastCandidate->localDir;
					pAttaching->velPT = pLastCandidate->velPT;
				}
			}
		}
	}
}

void FrameInfo::ApplyForce(APhysXScene& aScene)
{
	const NxActor* pBA = 0;
	const NxVec3* pBP = 0;
	const AttachedActor* pAA = GetAttachmentBinding();
	if (0 != pAA)
	{
		pBA = pAA->pActor;
		pBP = &(pAA->globalPos);
	}
	forceHorizontal.Apply(aScene, timeSec, pBA, pBP);
	forceVertical.Apply(aScene, timeSec, pBA, pBP);
}

void FrameInfo::AddForceHorizontal(NxActor& actor, const NxVec3& pos, const NxVec3& dir, const float pushFactor)
{
	if (!forceHorizontal.TotalForceIsValid())
	{
		if (0 < pushFactor)
		{
			NxVec3 hForce(dir);
			hForce.normalize();
			hForce *= pushFactor;
			forceHorizontal.SetTotalForce(hForce);
		}
	}
	forceHorizontal.Add(ForcePoint(actor, pos));
}

void FrameInfo::AddForceVertical(NxActor& actor, const NxVec3& pos, const NxVec3& force, const bool bIsJumpTakeOff)
{
	if (bIsJumpTakeOff)
		forceVertical.AddToConstant(ForcePoint(actor, pos, force));
	else
		forceVertical.Add(ForcePoint(actor, pos, force));
}

void FrameInfo::AddPushing(NxActor& actor)
{
	PushingActor.push_back(&actor);
}

bool FrameInfo::IsPushing(NxShape& shape) const
{
	NxActor& actor = shape.getActor();
	unsigned int nCount = PushingActor.size();
	for (unsigned int i = 0; i < nCount; ++i)
	{
		if (PushingActor[i] == &actor)
			return true;
	}
	return false;
}

bool EntityReport::onEvent(NxU32 nbEntities, NxShape** entities)
{
	bool bEnableAdd = true;
	for (int i = 0; i < nbEntities; ++i)
	{
		if (0 != m_pLastFrame)
		{
			if (m_pLastFrame->IsPushing(*entities[i]))
				bEnableAdd = false;
		}
		if (bEnableAdd)
			m_Shapes.push_back(*entities[i]);
	}
	return true;
}

void EntityReport::AddShape(NxShape& shape)
{
	if (0 != m_pLastFrame)
	{
		if (m_pLastFrame->IsPushing(shape))
			return;
	}

	if (0 != m_Shapes.size())
	{
		NxScene& sceneO = m_Shapes[0].pShape->getActor().getScene();
		NxScene& sceneN = shape.getActor().getScene();
		assert(&sceneN == & sceneO);
		if (&sceneN != & sceneO)
			return;
	}

	APhysXUtilLib::APhysXArray_PushOnce(m_Shapes, Node(shape));
}

void EntityReport::ChangeAttributes(const NxGroupsMask& newGM)
{
	const NxU32 nCount = m_Shapes.size();
	if (0 == nCount)
		return;
	
	NxScene& scene = m_Shapes[0].pShape->getActor().getScene();
	APhysXScene* pScene = static_cast<APhysXScene*>(scene.userData);
	assert(0 != pScene);
	if (0 != pScene)
		pScene->LockWritingScene();

	for (NxU32 i = 0; i < nCount; ++i)
	{
		m_Shapes[i].gmBackup = m_Shapes[i].pShape->getGroupsMask();
		m_Shapes[i].pShape->setGroupsMask(newGM);
	}

	if (0 != pScene)
		pScene->UnlockWritingScene();
}

void EntityReport::RecoverAttributes()
{
	const NxU32 nCount = m_Shapes.size();
	if (0 == nCount)
		return;

	NxScene& scene = m_Shapes[0].pShape->getActor().getScene();
	APhysXScene* pScene = static_cast<APhysXScene*>(scene.userData);
	assert(0 != pScene);
	if (0 != pScene)
		pScene->LockWritingScene();

	for (NxU32 i = 0; i < nCount; ++i)
		m_Shapes[i].pShape->setGroupsMask(m_Shapes[i].gmBackup);

	if (0 != pScene)
		pScene->UnlockWritingScene();
}

bool EntityReport::IsInclude(const AttachedActor& aa) const
{
	if (!aa.IsValid())
		return false;

	Node nodeAA(*(aa.pActor->getShapes()[0]));
	return APhysXUtilLib::APhysXArray_Find(m_Shapes, nodeAA);
}

bool EntityReport::IsInclude(const NxShape& shape) const
{
	Node nodeS(const_cast<NxShape&>(shape));
	return APhysXUtilLib::APhysXArray_Find(m_Shapes, nodeS);
}

APhysXU32 EntityReport::GetObjects(APhysXArray<APhysXObject*>& outObjects) const
{
	APhysXU32 nbObjs = outObjects.size();

	const NxU32 nCount = m_Shapes.size();
	for (NxU32 i = 0; i < nCount; ++i)
	{
		NxActor* pActor = &(m_Shapes[i].pShape->getActor());
		APhysXObject* pObj = static_cast<APhysXObject*>(pActor->userData);
		if (0 == pObj)
			continue;

		APhysXUtilLib::APhysXArray_PushOnce(outObjects, pObj);
	}

	APhysXU32 nb = outObjects.size() - nbObjs;
	return nb;
}

APhysXU32 EntityReport::GetNxActors(APhysXArray<NxActor*>& outNxActors) const
{
	APhysXU32 nbObjs = outNxActors.size();

	const NxU32 nCount = m_Shapes.size();
	for (NxU32 i = 0; i < nCount; ++i)
	{
		NxActor* pActor = &(m_Shapes[i].pShape->getActor());
		APhysXUtilLib::APhysXArray_PushOnce(outNxActors, pActor);
	}

	APhysXU32 nb = outNxActors.size() - nbObjs;
	return nb;
}

void ForceRecorder::ApplyForce(APhysXScene& aScene, NxActor& actor, const NxVec3& force, const NxVec3& pos)
{
#if 1

	APhysXForceInfo fi;
	fi.mActor = &actor;
	fi.mForce = force;
	fi.mPos = pos;
	fi.mForceFlag = APX_FF_ATPOS;
	if (FRT_FORCE == m_FType)
		fi.mForceMode = NX_FORCE;
	else if (FRT_IMPULSE == m_FType)
		fi.mForceMode = NX_IMPULSE;
	else
		fi.mForceMode = NX_SMOOTH_IMPULSE;

	aScene.AddForce(fi);

#else

//	limit the max force so that actors which are pushed won't be bounced away dramatically...
//	const float fMaxVelChange = 3.0f;

//	Note: Jul.6, 2012, YangLiu.
//	Remove the force limitation.  To do: add limitation, check impluse
	const float fMaxVelChange = 0.0f;

	bool bHasLimit = false;
	if (!NxMath::equals(fMaxVelChange, 0.0f, APHYSX_FLOAT_EPSILON))
		bHasLimit = true;

	if (FRT_FORCE == m_FType)
	{
		if (bHasLimit)
			aScene.AddLimitForceAtPos(&actor, force, pos, NX_FORCE, fMaxVelChange);
		else
			actor.addForceAtPos(force, pos, NX_FORCE);
	}
	else
	{
		NxVec3 I = force * aScene.GetExactSimDt();
		if (bHasLimit)
		{
			if (FRT_IMPULSE == m_FType)
				aScene.AddLimitForceAtPos(&actor, I, pos, NX_IMPULSE, fMaxVelChange);
			else
				aScene.AddLimitForceAtPos(&actor, I, pos, NX_SMOOTH_IMPULSE, fMaxVelChange);
		}
		else
		{
			if (FRT_IMPULSE == m_FType)
				actor.addForceAtPos(I, pos, NX_IMPULSE);
			else
				actor.addForceAtPos(I, pos, NX_SMOOTH_IMPULSE);
		}
	}
#endif
}

void ForceRecorder::Apply(APhysXScene& aScene, float fTickSec, const NxActor* pBindActor, const NxVec3* pBindPos)
{
	const NxU32 nCount = m_ForceInfo.size();
	if (FRM_AVERAGE == m_AddMode)
	{
		if (m_TFIsValid)
		{
			assert(0 < nCount);
			if (0 < nCount)
			{
				NxVec3 F = m_TotalForce / float(nCount);
				for (NxU32 i = 0; i < nCount; ++i)
				{
					if (!m_ForceInfo[i].pActor->readBodyFlag(NX_BF_KINEMATIC))
						ApplyForce(aScene, *(m_ForceInfo[i].pActor), F, m_ForceInfo[i].pos);
				}
			}
		}
	}
	else if (FRM_GETMAX == m_AddMode)
	{
		NxActor* pActor = 0;
		NxVec3* pForce = 0;
		NxVec3 pos(0.0f);

		assert(2 > nCount);
		if (0 == nCount)
		{
			if (0 != m_ForceConstant.pActor)
			{
				if (!m_ForceConstant.pActor->readBodyFlag(NX_BF_KINEMATIC))
				{
					pActor = m_ForceConstant.pActor;
					pForce = &(m_ForceConstant.force);
					pos = m_ForceConstant.pos;
				}
			}
		}
		else if (1 == nCount)
		{
			if (!m_ForceInfo[0].pActor->readBodyFlag(NX_BF_KINEMATIC))
			{
				pActor = m_ForceInfo[0].pActor;
				pForce = &(m_ForceInfo[0].force);
				pos = m_ForceInfo[0].pos;
				if ((pBindActor == pActor) && (0 != pBindPos))
					pos = *pBindPos;

				if (0 != m_ForceConstant.pActor)
				{
					if (!m_ForceConstant.pActor->readBodyFlag(NX_BF_KINEMATIC))
					{
						NxF32 mc = m_ForceConstant.force.magnitude();
						NxF32 mt = pForce->magnitude();
						if (mc > mt)
						{
							pActor = m_ForceConstant.pActor;
							pForce = &(m_ForceConstant.force);
							pos = m_ForceConstant.pos;
						}
					}
				}
			}
		}
		if (0 != pActor)
			ApplyForce(aScene, *pActor, *pForce, pos);
	}

	m_TFIsValid = false;
	m_ForceInfo.clear();
	m_ForceConstant.pActor = 0;
}

void ForceRecorder::AddToConstant(const ForcePoint& newFP)
{
	if (0 == m_ForceConstant.pActor)
	{
		m_ForceConstant = newFP;
		return;
	}
	
	NxF32 mo = m_ForceConstant.force.magnitude();
	NxF32 mn = newFP.force.magnitude();
	if (mn > mo)
		m_ForceConstant = newFP;
}

void ForceRecorder::Add(const ForcePoint& newFP)
{
	// Note: we assume that all the force(existing force, new force) have the same Axis-Aligned direction.
	const NxU32 nCount = m_ForceInfo.size();

	if (FRM_GETMAX == m_AddMode)
	{
		if (0 == nCount)
		{
			m_ForceInfo.push_back(newFP);
		}
		else
		{
			NxF32 mo = m_ForceInfo[0].force.magnitude();
			NxF32 mn = newFP.force.magnitude();
			if (mn > mo)
				m_ForceInfo[0] = newFP;
		}
		return;
	}

	if (FRM_AVERAGE == m_AddMode)
	{
		// According to our assumption, just get the first existing ForcePoint by position without counting in the force direction.
		ForcePoint* pOldFP = 0;
		NxU32 i = 0;
		for (; i < nCount; ++i)
		{
			pOldFP = &m_ForceInfo[i];
			if (newFP.pActor != pOldFP->pActor)
				continue;

			if (0.0001f > newFP.pos.distanceSquared(pOldFP->pos))
				break;
		}
		if(nCount <= i)
			pOldFP = 0;

		if (0 == pOldFP)
		{
			m_ForceInfo.push_back(newFP);
		}
		else
		{
			pOldFP->pos += newFP.pos;
			pOldFP->pos *= 0.5f;
		}
	}
}

void EntityReportSweep::ResetPos(const NxVec3& startPos, const NxVec3& targetPos, bool IsEnforceRevising)
{
	m_IsEnforceRevising = IsEnforceRevising;
	m_posStart  = startPos;
	m_posTarget	= targetPos;

	m_tClosest = NX_MAX_F32;
	m_norClosest.zero();
	m_posClosest = m_posTarget;
	m_pShapeClosest = 0;

	m_norLastShape.zero();
	m_posLastShape.zero();
	m_pShapeLastShape = 0;

	m_tClosestDAS = NX_MAX_F32;
	m_pClosestDynActShape = 0;
}

NxVec3 EntityReportSweep::GetProperPos(bool& outIsNewPos, NxVec3& outNormal, bool bCountInLast) const
{
	outIsNewPos = false;
	NxVec3 rtnPos = m_posTarget;
	outNormal.zero();

	if (NX_MAX_F32 != m_tClosest)
	{
		outIsNewPos = true;
		rtnPos.y = m_posClosest.y;
		m_pLastShape = m_pShapeClosest;
		outNormal = m_norClosest;
	}

	if (!outIsNewPos && bCountInLast)
	{
		if (0 == m_pShapeLastShape)
			m_pLastShape = 0;
		else
		{
			outIsNewPos = true;
			rtnPos.y = m_posLastShape.y;
			outNormal = m_norLastShape;
		}
	}

	return rtnPos;
}

bool EntityReportSweep::onEvent(NxU32 nbEntities, NxSweepQueryHit* entities)
{
	for (int i = 0; i < nbEntities; ++i)
	{
		if (NxMath::equals(0.0f, entities[i].t, APHYSX_FLOAT_EPSILON))
			continue;

		NxActor& actor = entities[i].hitShape->getActor();
		APhysXObject* pObject = static_cast<APhysXObject*>(actor.userData);
		if ((0 != pObject) && (pObject->IsDeleted()))
			continue;

		bool IsStatic = !actor.isDynamic();
		bool IsKinematic = false;
		if (!IsStatic)
			IsKinematic = actor.readBodyFlag(NX_BF_KINEMATIC);
		bool IsAttachEnable = false;
		if (0 != pObject)
			IsAttachEnable = pObject->GetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH);

		if (!IsStatic && !IsKinematic)
		{
			if (entities[i].t < m_tClosestDAS)
			{
				m_tClosestDAS = entities[i].t;
				m_pClosestDynActShape = entities[i].hitShape;
			}
		}

		if (entities[i].t < m_tClosest)
		{
			if (m_IsEnforceRevising || IsStatic || IsKinematic || IsAttachEnable)
			{
				m_tClosest		= entities[i].t;
				m_norClosest	= entities[i].normal;
				m_posClosest	= entities[i].point;
				m_pShapeClosest = entities[i].hitShape;
			}
		}

		if (m_pLastShape == entities[i].hitShape)
		{
			if (m_IsEnforceRevising || IsStatic || IsKinematic || IsAttachEnable)
			{
				m_norLastShape	  = entities[i].normal;
				m_posLastShape    = entities[i].point;
				m_pShapeLastShape = entities[i].hitShape;
			}
		}
	}
	return true;
}

bool EntityReportSweepForClosestNonDynamic::onEvent(NxU32 nbEntities, NxSweepQueryHit* entities)
{
	for (int i = 0; i < nbEntities; ++i)
	{
		bool isSorK = false;
		NxActor& actor = entities[i].hitShape->getActor();
		if (!actor.isDynamic())
		{
			isSorK = true;
		}
		else
		{
			if (actor.readBodyFlag(NX_BF_KINEMATIC))
				isSorK = true;
			else
				m_bIsHitDynamicStuff = true;
		}

		if (isSorK)
		{
			if (0 == m_pTheHit)
			{
				m_sqh = entities[i];
				m_pTheHit = &m_sqh;
			}
			else
			{
				if (m_pTheHit->t > entities[i].t)
					m_sqh = entities[i];
			}
		}
	}
	return true;
}

NxVec3 EntityReportSweepForDynCC::gZero(0.0f);
EntityReportSweepForDynCC::EntityReportSweepForDynCC()
{
	m_pCurrentHit = 0;
	m_ccSlideThreshold = 0.0f;
	m_AA.SetToInvalid();
	Reset(NxVec3(0, -1, 0), NxVec3(0, 0, 1), gZero, gZero, 0.0f, false);
}

bool EntityReportSweepForDynCC::onEventCollectInfo(const NxSweepQueryHit& item, bool& outLastSqhIsInBody, const NxVec3& ccHVel)
{
	bool rtn = false;
	outLastSqhIsInBody = false;
	if (NxMath::equals(0.0f, item.t, APHYSX_FLOAT_EPSILON))
		return rtn;
	
	bool isInBody = false;
	NxVec3 vd = item.point - m_btmCenter;
	float cosVal = GetCosValue(vd, m_defNormal);
	if (0 < cosVal)
		return false;
	if (vd.magnitude() < m_ccRadius)
		isInBody = true;

	if (0 != m_pLastHitShape)
	{
		if (m_pLastHitShape == item.hitShape)
		{
			m_sqhLast = item;
			outLastSqhIsInBody = isInBody;
			rtn = true;
		}
	}
		
	if (isInBody)
	{
		if (0 == m_pInBodyFarthest)
		{
			m_sqhInBody = item;
			m_pInBodyFarthest = &m_sqhInBody;
		}
		else
		{
			if (ccHVel.equals(gZero, APHYSX_FLOAT_EPSILON))
			{
				if (m_pInBodyFarthest->t < item.t)
					m_sqhInBody = item;
			}
			else
			{
				float dt = NxMath::abs(m_pInBodyFarthest->t - item.t);
				if (0.05f > dt)
				{
					if (InBodyCaseChooseFirst(item, *m_pInBodyFarthest, ccHVel))
						m_sqhInBody = item;
				}
				else
				{
					if (m_pInBodyFarthest->t < item.t)
						m_sqhInBody = item;
				}
			}
		}
	}
	else
	{
		if (0 == m_pOutBodyClosest)
		{
			m_sqhOutBody = item;
			m_pOutBodyClosest = &m_sqhOutBody;
		}
		else
		{
			if (m_pOutBodyClosest->t > item.t)
				m_sqhOutBody = item;
		}
	}
	return rtn;
}

bool EntityReportSweepForDynCC::onEvent(NxU32 nbEntities, NxSweepQueryHit* entities)
{
	if (0 == nbEntities)
		return true;

	NxVec3 gDir = m_defNormal * -1;
	NxVec3 ccVVel = GetVerticalSubValue(m_ccVel, gDir);
	NxVec3 ccHVel = m_ccVel - ccVVel;

	bool bLastHitIsInBody = false;
	NxSweepQueryHit* pLastHit = 0;
	for (int i = 0; i < nbEntities; ++i)
	{
		bool temp = false;
		if (onEventCollectInfo(entities[i], temp, ccHVel))
		{
			pLastHit = &m_sqhLast;
			bLastHitIsInBody = temp;
		}
	}

	bool bNewHitIsInBody = true;
	NxSweepQueryHit* pNewHit = m_pInBodyFarthest;
	if (0 == pNewHit)
	{
		bNewHitIsInBody = false;
		pNewHit = m_pOutBodyClosest;
	}

	if ((0 == pLastHit) && (0 == pNewHit))
		return true;
	else if ((0 != pLastHit) && (0 == pNewHit))
		m_pCurrentHit = pLastHit;
	else if ((0 == pLastHit) && (0 != pNewHit))
		m_pCurrentHit = pNewHit;
	else
	{
		m_pCurrentHit = pNewHit;
		if (pLastHit->hitShape != pNewHit->hitShape)
		{
			if (bLastHitIsInBody && bNewHitIsInBody)
			{
				if (InBodyCaseChooseFirst(*pLastHit, *pNewHit, ccHVel))
					m_pCurrentHit = pLastHit;
			}
			else if (bLastHitIsInBody && !bNewHitIsInBody)
				m_pCurrentHit = pLastHit;
			else if (!bLastHitIsInBody && !bNewHitIsInBody)
			{
				if (pLastHit->t < pNewHit->t)
					m_pCurrentHit = pLastHit;
			}
		}
	}

	assert(0 != m_pCurrentHit);
	NxVec3 dHit = m_pCurrentHit->point - m_btmCenter;
	float cosHitVal = GetCosValue(dHit, m_defNormal);
	if (0 < cosHitVal)
	{
		//just omit this hit point
		m_pCurrentHit = 0;
		return true;
	}

	if (m_ccIsInAir)
	{
		if (m_pCurrentHit != m_pInBodyFarthest)
		{
			m_pCurrentHit = 0;
			return true;
		}
	}

	m_AA.UpdateInfo(m_pCurrentHit->point, m_ccDir, m_pCurrentHit->hitShape->getActor());
	m_pSupport = &m_AA;
	return true;
}

bool EntityReportSweepForDynCC::InBodyCaseChooseFirst(const NxSweepQueryHit& first, const NxSweepQueryHit& second, const NxVec3& ccHVel)
{
	NxVec3 gDir = m_defNormal * -1;
				
	NxVec3 dvFirst = first.point - m_btmCenter;
	NxVec3 vdvFirst = GetVerticalSubValue(dvFirst, gDir);
	NxVec3 hdvFirst = dvFirst - vdvFirst;
	if (hdvFirst.equals(gZero, APHYSX_FLOAT_EPSILON))
		return true;

	NxVec3 dvSecond = second.point - m_btmCenter;
	NxVec3 vdvSecond = GetVerticalSubValue(dvSecond, gDir);
	NxVec3 hdvSecond = dvSecond - vdvSecond;
	if (hdvSecond.equals(gZero, APHYSX_FLOAT_EPSILON))
		return false;
	
	if (!ccHVel.equals(gZero, APHYSX_FLOAT_EPSILON))
	{			
		float cosFirst = GetCosValue(hdvFirst, ccHVel);
		float cosSecond  = GetCosValue(hdvSecond, ccHVel);
		if ((0 > cosFirst) && (0 < cosSecond))
			return false;
		if ((0 < cosFirst) && (0 > cosSecond))
			return true;
	}
	
	NxVec3 velFirst = first.hitShape->getActor().getLinearVelocity();
	NxVec3 velSecond = second.hitShape->getActor().getLinearVelocity();
	NxVec3 dv = velSecond - velFirst;
	if (0 > GetCosValue(dv, m_defNormal))
		return true;

	return false;
}

void EntityReportSweepForDynCC::Reset(const NxVec3& gDir, const NxVec3& ccDir, const NxVec3& ccVel, const NxVec3& btmCenter, const float ccRadius, const bool ccIsInAir)
{
	if (0 == m_pCurrentHit)
	{
		m_pLastHitShape = 0;
	}
	else
	{
		m_sqhLast = *m_pCurrentHit;
		m_pLastHitShape = m_sqhLast.hitShape;
		m_pCurrentHit = 0;
	}
	m_pInBodyFarthest = 0;
	m_pOutBodyClosest = 0;

	m_ccIsInAir = ccIsInAir;
	m_ccRadius = ccRadius;
	m_ccDir = ccDir;
	m_ccVel = ccVel;
	m_btmCenter = btmCenter;
	m_defNormal = -1 * gDir;

	m_pSupport = 0;
}

bool EntityReportSweepForDynCC::IsInBodyHit() const
{
	if ((0 != m_pCurrentHit) && (m_pCurrentHit == m_pInBodyFarthest))
		return true;

	return false;
}

NxVec3 EntityReportSweepForDynCC::GetNormal(const bool isInBody, bool* poutIsDefNormal) const
{
	if (0 != poutIsDefNormal)
		poutIsDefNormal = false;

	if (0 == m_pCurrentHit)
	{
		if (0 != poutIsDefNormal)
			*poutIsDefNormal = true;
		return m_defNormal;
	}
	
	if (isInBody)
	{
		if (!IsInBodyHit())
			return m_defNormal;
	}
	return m_pCurrentHit->normal;
}

AttachedActor* EntityReportSweepForDynCC::GetSupportActor(const bool isInBody) const
{
	if (!isInBody)
		return m_pSupport;

	if (IsInBodyHit())
		return m_pSupport;

	return 0;
}

NxShape* EntityReportSweepForDynCC::GetHitShape() const
{
	if (0 == m_pCurrentHit)
		return 0;
	
	return m_pCurrentHit->hitShape; 
}

EntityReportSweepForPushing::EntityReportSweepForPushing(APhysXCharacterController& currentCC, FrameInfo& currentFrame, const NxActor* pActor, float sweepDist)
{
	m_ActiveCC = &currentCC,
	m_pActiveFrame = &currentFrame; 
	m_pLastBinding = pActor;
	m_sweepDist = sweepDist;
}

bool EntityReportSweepForPushing::onEvent(NxU32 nbEntities, NxSweepQueryHit* entities)
{
	for (int i = 0; i < nbEntities; ++i)
	{
		NxActor& actor = entities[i].hitShape->getActor();
		APhysXObject* pObject = static_cast<APhysXObject*>(actor.userData);
		if ((0 != pObject) && (pObject->IsDeleted()))
			continue;

		if (m_pLastBinding == &actor)
			continue;

		m_pActiveFrame->AddPushing(actor);
		m_pActiveFrame->RaiseStatus(FrameInfo::IS_PUSHING);
		if (0 <	gPhysXCCHitReport->GetRegisteredCount())
		{
			APhysXCCShapeHit APhysXHit;
			APhysXHit.object = pObject;
			if (0 != APhysXHit.object)
			{
				APhysXHit.controller  = m_ActiveCC;
				APhysXHit.worldPos    = entities[i].point;
				APhysXHit.worldNormal = entities[i].normal;
				APhysXHit.id	  = 0;
				APhysXHit.dir	  = m_pActiveFrame->GetInputMoveDir();
				APhysXHit.length  = entities[i].t * m_sweepDist;
				APhysXHit.pushing = true;	
				gPhysXCCHitReport->onShapeHit(APhysXHit);
			}
		}
	}
	return true;
}
