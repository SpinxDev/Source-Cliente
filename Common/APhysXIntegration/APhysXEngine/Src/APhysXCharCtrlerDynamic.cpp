#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerMgrImp.h"
#include "APhysXCharCtrlerImp.h"
#include "APhysXCharCtrlerDynamic.h"

APhysXDynCharCtrler::APhysXDynCharCtrler(APhysXScene& scene, const APhysXCCDesc& desc)
	: APhysXHostCCBase(scene, desc) 
{
	m_maxHangDist = 15;
	m_minHangDist = 2;
	SetMaxHangAngle(70.0f);

	m_flags = desc.flags;
	m_flagsInner = 0;
}

bool APhysXDynCharCtrler::ReadFlag(const APhysXCCFlag flag) const
{
	return (m_flags & flag)? true : false;
}

void APhysXDynCharCtrler::ClearFlag(const APhysXCCFlag flag)
{
	m_flags &= ~flag;
}

void APhysXDynCharCtrler::RaiseFlag(const APhysXCCFlag flag)
{
	m_flags |= flag;
}

bool APhysXDynCharCtrler::CheckCCFlagForAttachment(const NxActor& actor, bool* poutIsEnforce)
{
	if (ReadFlag(APX_CCF_ATTACHMENT_ENFORCE_CLOSE))
	{
		if (0 != poutIsEnforce) *poutIsEnforce = true;
		return false; 
	}
	
	if (ReadFlag(APX_CCF_ATTACHMENT_ENFORCE_OPEN))
	{
		if (0 != poutIsEnforce) *poutIsEnforce = true;
		return true; 
	}
	
	if (0 != poutIsEnforce) *poutIsEnforce = false;
	return CheckFlagEnable(actor, APX_OBJFLAG_CC_CAN_ATTACH);
}

bool APhysXDynCharCtrler::CheckFlagEnable(const NxActor& actor, const APhysXObjectFlag flag) const
{
	APhysXObject* pObject = static_cast<APhysXObject*>(actor.userData);
	if (0 != pObject)
		return pObject->GetObjectFlag(flag);
	
#if EXCLUDE_RAW_PHYSX_ACTORS
	assert(0 != pObject);
	return false;
#endif
	
	return true;
}

void APhysXDynCharCtrler::SetMaxHangAngle(const float angle)
{
	m_maxHangAngle = angle;
	m_maxHangRadian = NxMath::mod(m_maxHangAngle, 180.0f) * NxPi / 180.0f;
}

void APhysXDynCharCtrler::SetAutoHangEndAngle(const float angle)
{
	m_endHangAngle = angle;
	if (m_endHangAngle < GetMaxHangAngle())
		m_endHangAngle = GetMaxHangAngle();

	m_endHangRadian = NxMath::mod(m_endHangAngle, 180.0f) * NxPi / 180.0f;
	OnSetAutoHangEndAngle(m_endHangRadian);
}

float APhysXDynCharCtrler::GetMaxHangLinearVelocitySquared(const float fHangRadius, const float cosVal, const float gVal) const
{
	// 0.5mv^2 + mgd(1- cosV) = mgd(1 - cosVmax)
	// v^2 = 2gd(cosV - cosVmax)
	float v2 = 2 * gVal * fHangRadius * (cosVal - (float)cos(m_maxHangRadian));
	return v2;
}

void APhysXDynCharCtrler::SetNewPos(const NxVec3& value)
{
	RaiseFlag(DCCII_HAS_NEW_POSITION);
	m_newPos = value;
}

void APhysXDynCharCtrler::SetNewVel(const NxVec3& value)
{
	RaiseFlag(DCCII_HAS_NEW_VELOCITY);
	m_newVel = value;
}

void APhysXDynCharCtrler::ApplyNewPosAndVel(NxActor& actor)
{
	bool bHasNewPos = ReadFlag(DCCII_HAS_NEW_POSITION);
	bool bHasNewVel = ReadFlag(DCCII_HAS_NEW_VELOCITY);
	if (bHasNewPos || bHasNewVel)
	{
		GetAPhysXScene().LockWritingScene();
		if (bHasNewPos)
		{
			assert(false == actor.readBodyFlag(NX_BF_KINEMATIC));
			actor.setGlobalPosition(m_newPos);
			ClearFlag(DCCII_HAS_NEW_POSITION);
		}
		if (bHasNewVel)
		{
			actor.setLinearVelocity(m_newVel);
			ClearFlag(DCCII_HAS_NEW_VELOCITY);
		}
		GetAPhysXScene().UnlockWritingScene();
	}
}

NxU32 APhysXDynCCImp::gFrozenFlags = NX_BF_FROZEN_ROT_X | NX_BF_FROZEN_ROT_Z;

APhysXDynCCImp::APhysXDynCCImp(const APhysXCCCapsuleDesc& desc, NxActor& actor, APhysXScene& scene)
	: APhysXDynCharCtrler(scene, desc), m_coreData(scene, actor)
{
	m_pDActor = &actor;

	NxCapsuleShape* pCap = actor.getShapes()[0]->isCapsule();
	assert(0 != pCap);
	m_NxVolume.x = pCap->getRadius();
	m_NxVolume.y = pCap->getHeight();
	SetDetectionDistance(m_NxVolume.y);

	m_Radius = desc.radius;
	m_Height = desc.height;

	m_pSupport = 0;
	m_pAttachment = 0;
	m_AAInfo.SetToInvalid();
	m_AttachPosLocalNormal.set(0, 1, 0);

	m_SlopeUpRadian = desc.GetSlopeUpRadian();
	m_SlideThreshold = (float)cos(m_SlopeUpRadian);
	m_SweepReport.SetSlideThreshold(m_SlideThreshold);
	m_SlopeDownRadian = desc.GetSlopeDownRadian();

	SetStepOffset(desc.stepOffset);
	m_maxHangDistEnable = 0.0f;
	SetAutoHangEndAngle(120);

	m_CooldownForAddForce = 0;
	m_CooldownForMoveUp   = 0;
	m_CooldownForMoveDown = 0;

	m_JumpSpeed = 0.0f;
	m_faceDir.set(1, 0, 0);
	m_LastPos = m_pDActor->getGlobalPosition();
	
	m_LinearVelSleepBackup.set(0.0f);
	m_AngularVelSleepBackup.set(0.0f);
	m_StateSleepBackup = ISituationConfigBase::DYNCC_S_CTRLMOVE_NOT_BIND;
	m_CurrentSituation = ISituationConfigBase::DYNCC_S_CTRLMOVE_NOT_BIND;
	CreateD6JointSetting(0);
	m_coreData.ApplyAllSettings();
	m_coreData.SetAppliedSituation(m_CurrentSituation);
}

APhysXDynCCImp::~APhysXDynCCImp()
{
	assert(0 == m_pDActor);
	m_pDActor = 0;
}

void APhysXDynCCImp::ReleaseNxObjects()
{
	m_coreData.ReleaseActorSetting();
	m_coreData.ReleaseD6JointSetting();
	m_coreData.ReleaseDistJointSetting();
	m_coreData.ApplyAllSettings();
	m_pDActor = 0;
}

NxVec3 APhysXDynCCImp::GetCenterPosition() const
{
	NxVec3 pos(gZero);
	if (0 == m_pDActor)
		return pos;
	
	pos = m_pDActor->getGlobalPosition();
	return pos;
}

NxVec3 APhysXDynCCImp::GetFootPosition() const
{
	NxVec3 pos(0, m_Height * -0.5f, 0);
	m_pDActor->getGlobalPose().multiply(pos, pos);
	return pos;
}

void APhysXDynCCImp::SetFootPosition(const NxVec3& pos)
{
	if (0 == m_pDActor)
		return;
	if (ReadFlag(DCCII_HANG_MODE))
		return;
	
	NxVec3 CenterPos = pos;
	NxVec3 halfH = GetUpDir() * m_Height * 0.5f;
	CenterPos += halfH;
	
	APhysXScene& scene = GetAPhysXScene();
	scene.LockWritingScene();
	if (m_pDActor->readBodyFlag(NX_BF_KINEMATIC))
		m_pDActor->moveGlobalPosition(CenterPos);
	else
		m_pDActor->setGlobalPosition(CenterPos);
	scene.UnlockWritingScene();

	m_Status = APX_CCS_JUMPING_INAIR;
	UpdateAttachment(0);
}

NxMat33 APhysXDynCCImp::GetGlobalOrientation() const
{
	return m_pDActor->getGlobalOrientation();
}

bool APhysXDynCCImp::IsDynDirUsing() const
{
	if (ReadFlag(DCCII_HANG_MODE))
		return true;

	if (m_coreData.AppliedSituationBelongToHangEnd())
		return true;

	if (ISituationConfigBase::DYNCC_S_HANG_POST == m_coreData.GetAppliedSituation())
		return true;

	return false;
}

NxVec3 APhysXDynCCImp::GetUpDir() const
{
	NxVec3 up = APhysXHostCCBase::GetUpDir();
	if (IsDynDirUsing())
	{
		NxVec3 dir;
		m_pDActor->getGlobalPose().M.multiply(up, dir);
		return dir;
	}
	return up;
}

NxVec3 APhysXDynCCImp::GetDir() const
{
	if (IsDynDirUsing())
	{
		// Do Not use this method, to avoid rotation in y-axis
		// m_pDActor->getGlobalPose().M.multiply(m_faceDir, dir);

		NxVec3 Up(0, 1, 0);
		NxVec3 xAxis = Up.cross(m_faceDir);
		NxVec3 dir = xAxis.cross(GetUpDir());
		dir.normalize();
		return dir;
	}
	return m_faceDir;
}

void APhysXDynCCImp::SetDir(const NxVec3& vDir) 
{
	m_faceDir = vDir;
	if (NxMath::equals(m_faceDir.x, 0.0f, APHYSX_FLOAT_EPSILON)) m_faceDir.x = 0.0f;
	if (NxMath::equals(m_faceDir.y, 0.0f, APHYSX_FLOAT_EPSILON)) m_faceDir.y = 0.0f;
	if (NxMath::equals(m_faceDir.z, 0.0f, APHYSX_FLOAT_EPSILON)) m_faceDir.z = 0.0f;
	m_faceDir.normalize();

	if (AttachmentIsValid())
	{
		// Do Not use "m_pAttachment->pose" directly
		// Because of the attachment probably has changed the pose, so we have to get current pose.
		NxMat33 mat;
		m_pAttachment->pActor->getGlobalOrientation().getInverse(mat);
		mat.multiply(m_faceDir, m_pAttachment->localDir);
	}
}

NxVec3 APhysXDynCCImp::GetHorizonDir() const
{
	// must get current gravity due to this is an open interface to outside
	NxVec3 g;
	GetNxScene().getGravity(g);

	NxVec3 currentDir = GetDir();
	NxVec3 vDir = GetVerticalSubValue(currentDir, g);
	NxVec3 hDir = currentDir - vDir; 
	if (NxMath::equals(hDir.x, 0.0f, APHYSX_FLOAT_EPSILON)) hDir.x = 0.0f;
	if (NxMath::equals(hDir.y, 0.0f, APHYSX_FLOAT_EPSILON)) hDir.y = 0.0f;
	if (NxMath::equals(hDir.z, 0.0f, APHYSX_FLOAT_EPSILON)) hDir.z = 0.0f;
	hDir.normalize();
	return hDir;
}

void APhysXDynCCImp::SetStepOffset(const float offset)
{
	m_AutoStepUp = offset;
	ClearFlag(DCCII_AUTO_STEP_UP_ENABLE);
	if (!NxMath::equals(m_AutoStepUp, 0.0f, APHYSX_FLOAT_EPSILON))
	{
		if (0 < m_AutoStepUp)
			RaiseFlag(DCCII_AUTO_STEP_UP_ENABLE);
	}
}

bool APhysXDynCCImp::GetAttachmentPos(NxVec3& out) const
{
	if (AttachmentIsValid())
	{
		// Do Not just return "out = m_pAttachment->globalPos;"
		// Because of the attachment probably has changed the pose, so we have to re-calculate the correct position.
		// By the way, unlike common CC, we can make sure that attachment is valid or not in real time.
		m_pAttachment->pActor->getGlobalPose().multiply(m_pAttachment->localPos, out);
		return true;
	}

	return false;
}

NxVec3 APhysXDynCCImp::GetVolume(APhysXVolumeType& outVT) const
{
	outVT = APX_VT_CAPSULE;
	return NxVec3(m_Radius, m_Height, -1);	
}

bool APhysXDynCCImp::ChangeVolume(const NxVec3& newVolume)
{
	if (ReadFlag(DCCII_HANG_MODE))
		return false;

	assert(0 != m_pDActor);
	if (0 == m_pDActor)
		return false;
	
	NxCapsuleShape* pCapsule = m_pDActor->getShapes()[0]->isCapsule();
	assert(0 != pCapsule);
	if (0 == pCapsule)
		return false;

	const float radius = newVolume.x;
	const float height = newVolume.y;
	const float capHeight = height - radius * 2;
	if (0 > radius) return false;
	if (0 > capHeight) return false;
	
	NxVec3 newVol(radius, capHeight, -1);
	if (m_NxVolume.equals(newVol, APHYSX_FLOAT_EPSILON))
		return true;
	
	float HalfIns = (capHeight - m_NxVolume.y) * 0.5f + (radius - m_NxVolume.x);
	NxVec3 tmpPos = GetCenterPosition();
	tmpPos.y += HalfIns;
	if (CheckOverlapCapsule(tmpPos, radius - pCapsule->getSkinWidth() * 2, capHeight))
		return false;

	APhysXScene& scene = GetAPhysXScene();
	scene.LockWritingScene();
	if (m_pDActor->readBodyFlag(NX_BF_KINEMATIC))
		m_pDActor->moveGlobalPosition(tmpPos);
	else
		m_pDActor->setGlobalPosition(tmpPos);
	pCapsule->setRadius(radius);
	pCapsule->setHeight(capHeight);
	scene.UnlockWritingScene();

	m_Radius = radius;
	m_Height = height;
	m_NxVolume.x = radius;
	m_NxVolume.y = capHeight;
	
	if (ISituationConfigBase::DYNCC_S_STAND_AND_BIND == m_coreData.GetAppliedSituation())
	{
		NxActor* pActor = 0;
		if (0 != m_pAttachment)
			pActor = const_cast<NxActor*>(m_pAttachment->pActor);

		m_coreData.ReleaseD6JointSetting();
		CreateD6JointSetting(pActor);
		m_coreData.ApplyAllSettings();
		m_coreData.SetAppliedSituation(m_CurrentSituation);
	}
	return true;
}

bool APhysXDynCCImp::CheckOverlapCapsule(const NxVec3& centerPos, const float radius, const float height)
{
	NxCapsule worldCapsule;
	worldCapsule.p0 = worldCapsule.p1 = centerPos;
	worldCapsule.p0.y -= height * 0.5f;
	worldCapsule.p1.y += height * 0.5f;
	worldCapsule.radius = radius;
	
	bool IsOccupied = true;
	NxU32 nShapes = GetNxScene().overlapCapsuleShapes(worldCapsule, NX_DYNAMIC_SHAPES, 0, 0, 0, GetActiveGroups(), &GetFilterGMExcludeBody(false), true);
	if (0 == nShapes)
		IsOccupied = false;

	return IsOccupied;
}

bool APhysXDynCCImp::GetGroundPos(float sweepDist, NxVec3& pos, NxVec3* poutNormal) const
{
	NxScene& scene = GetNxScene();
	NxVec3 gDir;
	scene.getGravity(gDir);
	gDir.normalize();

	APhysXVolumeType vt;
	NxVec3 vol = GetVolume(vt);
	assert(APX_VT_CAPSULE == vt);

	NxVec3 start = pos;
	start += gDir * (-1 * vol.y * 0.5f);
	NxVec3 motion = gDir * sweepDist;

	NxCapsule worldCapsule;
	worldCapsule.p0 = worldCapsule.p1 = start;
	worldCapsule.radius = m_Radius;

	NxSweepQueryHit sqHit;
	NxU32 flags = NX_SF_STATICS | NX_SF_DYNAMICS;
	NxGroupsMask gmFilter = GetFilterGMExcludeBody();
	NxU32 nHit = scene.linearCapsuleSweep(worldCapsule, motion, flags, 0, 1, &sqHit, 0, GetActiveGroups(), &gmFilter);
	if (0 == nHit)
		return false;

	pos = sqHit.point;
	if (0 != poutNormal)
		*poutNormal = sqHit.normal;
	return true;
}

NxVec3 APhysXDynCCImp::GetInstantVelocity(bool bIsAbsVel) const
{
	assert(0 != m_pDActor);
	if (0 == m_pDActor)
		return NxVec3(0.0f);

	NxVec3 vel = m_pDActor->getLinearVelocity();
	if (!bIsAbsVel)
	{
		if (AttachmentIsValid())
		{
			// must get the instant binding velocity! 
			NxVec3 velPT = m_pAttachment->pActor->getLocalPointVelocity(m_pAttachment->localPos);
			vel -= velPT;
		}
	}
	return vel;
}

bool APhysXDynCCImp::IsComponent(const NxActor& actor) const
{
	assert(0 != m_pDActor);
	if (0 == m_pDActor)
		return false;
	
	return (&actor == m_pDActor);
}

NxActor* APhysXDynCCImp::GetHiddenActor() const
{
	return m_pDActor;
}

void APhysXDynCCImp::JumpCC(float fJumpingSpeed, bool bIsAbsoluteSpeed)
{
	assert(0 != m_pDActor);
	if (0 == m_pDActor)
		return;

	m_JumpSpeed = fJumpingSpeed;
	RaiseFlag(DCCII_ADDED_JUMP_VELOCITY);

	if (ReadFlag(DCCII_HANG_MODE))
	{
		HangEnd();
		ClearFlag(DCCII_ADDED_JVEL_ABSOLUTE);
	}
	else
	{
		if (bIsAbsoluteSpeed)
			RaiseFlag(DCCII_ADDED_JVEL_ABSOLUTE);
	}
}

void APhysXDynCCImp::ComputeJumpTakeoffForce(const NxVec3& oldVel, const NxVec3& newVel, const NxVec3& g, const float timeSec)
{
	assert(true == AttachmentIsValid());
	if (m_pAttachment->pActor->readBodyFlag(NX_BF_KINEMATIC))
		return;

	if (ReadFlag(APX_CCF_PASSIVE_EFFECT_ONLY))
		return;

	NxVec3 velChange = newVel - oldVel;
	if (0 < GetCosValue(velChange, g))
		return;

	NxVec3 force(0.0f);
	if (GetJumpingFactor() < 0)
	{
		//  F(CC to actor) = -1 * Fac(actor to CC)
		//  Fac + mg = ma  &&  0 + at = v  so,  Fac = mv/t - mg
		//  F = m(g - v/t)
		force = GetMass() * (g - velChange / timeSec);

		//  I = F * t  &&  F = m(g - v/t)
		//  I = m(gt - v)
		//	NxVec3 impulse = GetMass() * (g * timeSec - velChange);
	}
	else
	{
		force = GetMass() * g * GetJumpingFactor();
	}
	if (!force.isZero())
	{
		APhysXForceInfo fi;
		fi.mActor = const_cast<NxActor*>(m_pAttachment->pActor);
		fi.mForce = force;
		m_pAttachment->pActor->getGlobalPose().multiply(m_pAttachment->localPos, fi.mPos);
		fi.mForceFlag = APX_FF_ATPOS;
		fi.mForceMode = NX_FORCE;
		GetAPhysXScene().AddForce(fi);
	}
}

void APhysXDynCCImp::ComputeTargetVelocityForJump(const NxVec3& velMove, const float deltaSecond)
{
	NxVec3 vel = GetAttachedPointVelocity(gZero);
	vel += velMove;
	if ((APX_CCS_JUMPING_INAIR & m_Status))
		vel = m_pDActor->getLinearVelocity();
	
	NxVec3 g;
	GetNxScene().getGravity(g);
	NxVec3 velV = GetVerticalSubValue(vel, g);
	NxVec3 velH = vel - velV;
	
	NxVec3 up = -1 * g;
	up.normalize();
	NxVec3 velVNew(0.0f);
	if (ReadFlag(DCCII_ADDED_JVEL_ABSOLUTE))
		velVNew = up * m_JumpSpeed;
	else
		velVNew += up * m_JumpSpeed;
	m_coreData.SetNewLinearVel(velH + velVNew);
	ClearFlag(DCCII_ADDED_JVEL_ABSOLUTE);
	
	if (AttachmentIsValid())
		ComputeJumpTakeoffForce(velV, velVNew, g, deltaSecond);
}

void APhysXDynCCImp::ComputeTargetVelocityForWalk(const NxVec3& velMove, const float deltaSecond)
{
	NxVec3 slideDir(0.0f);
	bool bHasSlideMove = CheckSlideAction(&slideDir);

	NxVec3 newLinearVel(0.0f);
	if (velMove.equals(gZero, APHYSX_FLOAT_EPSILON))
	{
		if (bHasSlideMove)
		{
			m_CurrentSituation = ISituationConfigBase::DYNCC_S_SLIDE_FREE;
			newLinearVel = GetAttachedPointVelocity(newLinearVel);
		}
		else
		{
			if (AttachmentIsValid())
				m_CurrentSituation = ISituationConfigBase::DYNCC_S_STAND_AND_BIND;
			else
				m_CurrentSituation = ISituationConfigBase::DYNCC_S_STAND_NOT_BIND;
		}
	}
	else
	{
		if (AttachmentIsValid())
			m_CurrentSituation = ISituationConfigBase::DYNCC_S_CTRLMOVE_AND_BIND;
		else
			m_CurrentSituation = ISituationConfigBase::DYNCC_S_CTRLMOVE_NOT_BIND;

		newLinearVel = velMove;
		NxVec3 groundNormal = GetGroundNormal();

		bool bAutoStepUpInvolved = false;
		if (ReadFlag(DCCII_ON_THE_EDGE))
		{
			if (ReadFlag(DCCII_AUTO_STEP_UP_ENABLE))
			{
				float cosV = GetCosValue(velMove, groundNormal);
				if (0 > cosV)
				{
					bHasSlideMove = false;
					bAutoStepUpInvolved = true;
				}
			}
		}
		if (!bAutoStepUpInvolved)
		{
			NxVec3 right = groundNormal.cross(velMove);
			NxVec3 realVelD = right.cross(groundNormal);
			realVelD.normalize();
			newLinearVel = realVelD * GetSpeed();
		}

		if (bHasSlideMove && !bAutoStepUpInvolved)
		{
			m_CurrentSituation = ISituationConfigBase::DYNCC_S_SLIDE_CTRL;
			float cosRes = GetCosValue(newLinearVel, slideDir);
			if (0.001f > cosRes)
			{
				NxVec3 subSlide = GetVerticalSubValue(newLinearVel, slideDir);
				newLinearVel -= subSlide;
			}
		}
		else
		{
			NxVec3 g;
			GetNxScene().getGravity(g);
			newLinearVel += g * deltaSecond; 
		}
	}

	if (ISituationConfigBase::DYNCC_S_STAND_AND_BIND != m_CurrentSituation)
	{
		bool IsContinueSF = false;
		if (ISituationConfigBase::DYNCC_S_SLIDE_FREE == m_CurrentSituation)
		{
			if (ISituationConfigBase::DYNCC_S_SLIDE_FREE == m_coreData.GetAppliedSituation())
				IsContinueSF = true;
		}
		if (!IsContinueSF)
			m_coreData.SetNewLinearVel(newLinearVel);
	}

	if (!bHasSlideMove && ReadFlag(DCCII_AUTO_STEP_UP_ENABLE))
		AutoStep(velMove, newLinearVel, deltaSecond);
}

void APhysXDynCCImp::SubMoveCCForNotHangMode(const NxVec3& MoveDir, const float deltaSecond)
{
	assert(false == ReadFlag(DCCII_HANG_MODE));
	NxVec3 moveVel(MoveDir);
	moveVel.normalize();
	moveVel *= GetSpeed();
	
	if (m_coreData.AppliedSituationBelongToHangEnd())
		m_CurrentSituation = ISituationConfigBase::DYNCC_S_HANG_POST;

	if (ISituationConfigBase::DYNCC_S_HANG_POST == m_CurrentSituation)
	{
		if (m_coreData.ComputeActorStandPose())
		{
			m_CurrentSituation = ISituationConfigBase::DYNCC_S_JUMP_OR_INAIR;
		}
		else
		{
			if (APX_CCS_STANDON_SOMETHING == GetStatus())
			{
				ComputeTargetVelocityForWalk(moveVel, deltaSecond);
				m_coreData.ReleaseD6JointSetting();
				CreateD6JointSetting(0);
			}
		}
	}
	else
	{
		if (ReadFlag(DCCII_ADDED_JUMP_VELOCITY))
		{
			ClearFlag(DCCII_ADDED_JUMP_VELOCITY);
			m_CurrentSituation = ISituationConfigBase::DYNCC_S_JUMP_START;
			ComputeTargetVelocityForJump(moveVel, deltaSecond);
		}
		else
		{
			if (APX_CCS_JUMPING_INAIR & m_Status)
				m_CurrentSituation = ISituationConfigBase::DYNCC_S_JUMP_OR_INAIR;
			else
				ComputeTargetVelocityForWalk(moveVel, deltaSecond);
		}
	}
}

void APhysXDynCCImp::SubMoveCCForHangMode(const NxVec3& MoveDir, const float deltaSecond)
{
	assert(true == ReadFlag(DCCII_HANG_MODE));
	bool bIsHangEnd = false;
	bool bIsBroken = false;
	if (!m_coreData.IsDistanceJointAvailable(bIsBroken))
	{
		if (!bIsBroken)
		{
			assert(!"Oops! Logic Error!");
		}
		bIsHangEnd = true;
	}

	if (bIsHangEnd || ReadFlag(DCCII_HANG_END))
	{
		HangEndImplement(deltaSecond);
		return;
	}
	
	if (ReadFlag(DCCII_HANG_START))
	{
		ClearFlag(DCCII_HANG_START);
		m_CurrentSituation = ISituationConfigBase::DYNCC_S_HANG_START;
		m_coreData.UpdateD6JointDescForHang(GetHorizonDir(), GetMass());
	}

	if (ISituationConfigBase::DYNCC_S_HANG_START == m_coreData.GetAppliedSituation())
		m_CurrentSituation = ISituationConfigBase::DYNCC_S_HANGING;

	NxVec3 dirHangRope;
	if (!m_coreData.ComputeHangRopeDirAndDist(&dirHangRope))
		return;

	HangMoveControl(MoveDir, deltaSecond, dirHangRope);
}

void APhysXDynCCImp::HangEndImplement(const float deltaSecond, const bool bEnforceJump)
{
	ClearFlag(DCCII_HANG_START);
	ClearFlag(DCCII_HANG_END);
	ClearFlag(DCCII_HANG_MODE);
	m_CurrentSituation = ISituationConfigBase::DYNCC_S_HANG_END;
	if (bEnforceJump || ReadFlag(DCCII_ADDED_JUMP_VELOCITY))
	{
		ClearFlag(DCCII_ADDED_JUMP_VELOCITY);
		m_CurrentSituation = ISituationConfigBase::DYNCC_S_HANG_END_JUMPING;
		ComputeTargetVelocityForJump(gZero, deltaSecond);
	}
	m_coreData.ReleaseDistJointSetting();
}

void APhysXDynCCImp::HangMoveControl(const NxVec3& MoveDir, const float deltaSecond, const NxVec3& dirHangRope)
{
	assert(ReadFlag(DCCII_HANG_MODE));
	if (MoveDir.equals(gZero, APHYSX_FLOAT_EPSILON))
		return;

	bool bIsValid = false;
	NxDistanceJointDesc& descDJ = m_coreData.GetDistJointDesc(bIsValid);
	if (!bIsValid)
		return;

	NxVec3 g;
	GetNxScene().getGravity(g);
	float gVal = g.magnitude();
	g.normalize();

	NxVec3 vMoveDir = GetVerticalSubValue(MoveDir, g);
	NxVec3 hMoveDir = MoveDir - vMoveDir; 
	if (NxMath::equals(vMoveDir.x, 0.0f, APHYSX_FLOAT_EPSILON)) vMoveDir.x = 0.0f;
	if (NxMath::equals(vMoveDir.y, 0.0f, APHYSX_FLOAT_EPSILON)) vMoveDir.y = 0.0f;
	if (NxMath::equals(vMoveDir.z, 0.0f, APHYSX_FLOAT_EPSILON)) vMoveDir.z = 0.0f;
	if (NxMath::equals(hMoveDir.x, 0.0f, APHYSX_FLOAT_EPSILON)) hMoveDir.x = 0.0f;
	if (NxMath::equals(hMoveDir.y, 0.0f, APHYSX_FLOAT_EPSILON)) hMoveDir.y = 0.0f;
	if (NxMath::equals(hMoveDir.z, 0.0f, APHYSX_FLOAT_EPSILON)) hMoveDir.z = 0.0f;

	NxVec3 newPos;
	const float DeltaDist = 0.1f;
	float cosVMove = vMoveDir.dot(g);
	if ((0 >= m_CooldownForMoveUp) && (0 > cosVMove))  // move up
	{
		const float newDist = descDJ.maxDistance - DeltaDist;
		if (newDist > GetMinHangDistanceLimit())
		{
			descDJ.maxDistance = newDist;
			m_coreData.UpdateDistJointDesc();
			m_CooldownForMoveUp = 3;

	//		newPos = descDJ.actor[1]->getGlobalPosition() - dirHangRope * DeltaDist;
	//		m_coreData.SetNewPosition(newPos);
		}
	}

	if ((0 >= m_CooldownForMoveDown) && (0 < cosVMove))  // move down
	{
		const float newDist = descDJ.maxDistance + DeltaDist;
		if (newDist <= m_maxHangDistEnable && newDist <= GetMaxHangDistanceLimit())
		{
			descDJ.maxDistance = newDist;
			m_coreData.UpdateDistJointDesc();
			m_CooldownForMoveDown = 3;

	//		newPos = descDJ.actor[1]->getGlobalPosition() + dirHangRope * DeltaDist;
	//		m_coreData.SetNewPosition(newPos);
		}
	}

	if (0 < m_CooldownForAddForce)
		return;

	if (NxMath::equals(hMoveDir.magnitude(), 0.0f, APHYSX_FLOAT_EPSILON))
		return;

	// add force
	float cosHanging = dirHangRope.dot(g);
	NxVec3 nowVel = m_pDActor->getLinearVelocity();
	const float nowSpeed2 = nowVel.magnitudeSquared();
	const float maxVel2 = GetMaxHangLinearVelocitySquared(descDJ.maxDistance, cosHanging, gVal);
	if (0 < GetCosValue(hMoveDir, nowVel))
	{
		if (nowSpeed2 >= maxVel2)
			return;
	}

	const float DeltaVel = 0.1f;
	NxVec3 Up(0, 1, 0);
	NxVec3 xAxis = Up.cross(hMoveDir);
	NxVec3 nForce = xAxis.cross(dirHangRope * -1);
	nForce.normalize();
	nForce *= GetMass() * DeltaVel / deltaSecond;
	m_CooldownForAddForce = 3;
	m_coreData.SetNewForce(nForce);
}

NxU32 APhysXDynCCImp::MoveCC(const float deltaSecond, const NxVec3& MoveDir)
{
	assert(0 != m_pDActor);
	if (0 == m_pDActor)
		return 0;

	if (IsSleeping())
		return 0;

	if (0 < m_CooldownForAddForce) 	--m_CooldownForAddForce;
	if (0 < m_CooldownForMoveUp)	--m_CooldownForMoveUp;
	if (0 < m_CooldownForMoveDown)	--m_CooldownForMoveDown;

	if (ReadFlag(DCCII_HANG_MODE))
		SubMoveCCForHangMode(MoveDir, deltaSecond);
	else
		SubMoveCCForNotHangMode(MoveDir, deltaSecond);

	UpdateDominanceSetting();
	UpdateJointSetting(deltaSecond);
	m_coreData.ApplyAllSettings();
	if (ISituationConfigBase::DYNCC_S_STAND_AND_BIND == m_CurrentSituation)
	{
		// must get the instant binding velocity! 
		NxVec3 velPT = m_pAttachment->pActor->getLocalPointVelocity(m_pAttachment->localPos);
		m_pDActor->setLinearVelocity(velPT);
	}
	m_LastPos = m_pDActor->getGlobalPosition();
	return 1;
}

bool APhysXDynCCImp::CheckSlideAction(NxVec3* poutSlideDir)
{
	bool rtnSlideAction = false;
	if (0 != poutSlideDir)
		poutSlideDir->zero();

	if (!ReadFlag(APX_CCF_SLIDE_ENABLE))
		return rtnSlideAction;

	NxVec3 g;
	GetNxScene().getGravity(g);
	NxVec3 groundNormal(m_GroundNormal);
	if (groundNormal.isZero())
	{
		groundNormal = g * -1;
		groundNormal.normalize();
	}
	float cosRes = GetCosValue(groundNormal, g * -1);
	if (m_SlideThreshold > cosRes)
	{
		rtnSlideAction = true;
		if (0 != poutSlideDir)
		{
			NxVec3 right = groundNormal.cross(g);
			*poutSlideDir = right.cross(groundNormal);
			poutSlideDir->normalize();
		}
	}
	return rtnSlideAction;
}

void APhysXDynCCImp::UpdateJointSetting(const float deltaSecond)
{
	bool bIsBroken = false;
	AttachmentIsValid(&bIsBroken);
	if (bIsBroken)
	{
		m_coreData.ReleaseD6JointSetting();
		m_CurrentSituation = ISituationConfigBase::DYNCC_S_JUMP_OR_INAIR;
		CreateD6JointSetting(0);
		return;
	}

	ISituationConfigBase* pObjConfig = ISituationConfigBase::GetSituationConfigObject(m_CurrentSituation);
	pObjConfig->Configuration(m_coreData);
	if (m_coreData.IsAutoHangEndToDo() && ReadFlag(APX_CCF_AUTO_HANG_END_ENABLE))
	{
		HangEndImplement(deltaSecond, true);
		pObjConfig = ISituationConfigBase::GetSituationConfigObject(m_CurrentSituation);
		pObjConfig->Configuration(m_coreData);
	}
}

void APhysXDynCCImp::UpdateDominanceSetting()
{
	int valDomin = m_pDActor->getDominanceGroup();
	if (ReadFlag(APX_CCF_PASSIVE_EFFECT_ONLY) || ReadFlag(DCCII_AUTO_STEP_IS_WORKING))
	{
		if (APX_RB_DG_MIDDLE != valDomin)
			m_coreData.SetNewDominance(APX_RB_DG_MIDDLE);
	}
	else
	{
		if (APX_RB_DG_HIGH != valDomin)
			m_coreData.SetNewDominance(APX_RB_DG_HIGH);
	}
}

void APhysXDynCCImp::RefeshCCBySimResult()
{
	if (IsSleeping())
		return;

	m_coreData.SetAppliedSituation(m_CurrentSituation);
	UpdateDir();

	ClearFlag(DCCII_AUTO_STEP_WORKED_LAST);
	if (ReadFlag(DCCII_AUTO_STEP_IS_WORKING))
	{
		RaiseFlag(DCCII_AUTO_STEP_WORKED_LAST);
		ClearFlag(DCCII_AUTO_STEP_IS_WORKING);
	}

	NxScene& scene = GetNxScene();
	NxVec3 g;
	scene.getGravity(g);
	NxVec3 gDir(g);
	gDir.normalize();

	if (ReadFlag(DCCII_HANG_MODE))
	{
		m_GroundNormal = gDir * -1;
		UpdateStatus(false);
		UpdateAttachment(0);
		return;
	}

	NxVec3 btmCenterPos = GetCenterPosition() + gDir * (m_Height * 0.5f - m_Radius);
	const ISituationConfigBase::Situation s = m_coreData.GetAppliedSituation();
	if (ISituationConfigBase::DYNCC_S_STAND_AND_BIND == s)
	{
		bool bIsBroken = false;
		AttachmentIsValid(&bIsBroken);
		if (!bIsBroken)
		{	
			assert(0 != m_pAttachment);
			NxMat34 pose = m_pAttachment->pActor->getGlobalPose();

			NxVec3 normal;
			pose.M.multiply(m_AttachPosLocalNormal, normal);
			float cosRes = GetCosValue(normal, g * -1);
			if (m_SlideThreshold <= cosRes)
				return;
		}
	}

	const float skinWidth = m_pDActor->getShapes()[0]->getSkinWidth();
	NxCapsule worldCapsule;
	worldCapsule.p0 = worldCapsule.p1 = GetCenterPosition();
	worldCapsule.radius = m_Radius;

	NxU32 activeGroups = GetActiveGroups();
	NxGroupsMask gmFilter = GetFilterGMExcludeBody();

	NxVec3 motion = gDir * m_Height * 2;
	bool isInAir = m_coreData.IsNotHangAndPossibleInAir();
	m_SweepReport.Reset(gDir, m_faceDir, m_pDActor->getLinearVelocity(), btmCenterPos, worldCapsule.radius, isInAir);
	NxU32  flagsSweep = NX_SF_STATICS | NX_SF_DYNAMICS | NX_SF_ALL_HITS;
	NxU32 nSweepHitShapes = scene.linearCapsuleSweep(worldCapsule, motion, flagsSweep, 0, 0, 0, &m_SweepReport, activeGroups, &gmFilter);

	bool IsHitSomething = false;
	NxShape* pSweepHitShape = m_SweepReport.GetHitShape();
	if (0 != pSweepHitShape)
		IsHitSomething = true;
	m_GroundNormal = m_SweepReport.GetNormal(false);
	AttachedActor* pNewAttachment = m_SweepReport.GetSupportActor(false);

	bool bToClearHit  = false;
	bool bIsClearVVel = false;
	bool bIsToBeSlide = false;
	NxSweepQueryHit* pResult = m_SweepReport.GetHitResult();
	if (m_SweepReport.IsInBodyHit())
	{
		if (pNewAttachment->pActor->isDynamic())
		{
			if (!pNewAttachment->pActor->readBodyFlag(NX_BF_KINEMATIC))
				RevisePositionToAvoidOverlap(skinWidth, gDir, *pResult, motion.magnitude());
		}
		bIsToBeSlide = CheckSlideAction();
		if (isInAir && bIsToBeSlide)
			bToClearHit = true;
	}
	else
	{
		if (0 != pSweepHitShape)
		{
			switch (s)
			{
			case ISituationConfigBase::DYNCC_S_SLIDE_FREE:
			case ISituationConfigBase::DYNCC_S_SLIDE_CTRL:
				bToClearHit = true;
				break;
			case ISituationConfigBase::DYNCC_S_STAND_AND_BIND:
			case ISituationConfigBase::DYNCC_S_CTRLMOVE_AND_BIND:
			case ISituationConfigBase::DYNCC_S_STAND_NOT_BIND:
			case ISituationConfigBase::DYNCC_S_CTRLMOVE_NOT_BIND:
				{
					if (ISituationConfigBase::DYNCC_S_STAND_NOT_BIND == s)
						bIsClearVVel = true;
					NxSweepQueryHit* pResult = m_SweepReport.GetHitResult();
					if (!RevisePosition(skinWidth, gDir, *pResult, motion.magnitude()))
						bToClearHit = true;
				}
				break;
			}
		}
	}
	if (bToClearHit)
	{
		pSweepHitShape = 0;
		IsHitSomething = false;
		m_GroundNormal = gDir * -1;
		pNewAttachment = 0;
	}

	const NxActor* pNewSupport = 0;
	if (0 != pNewAttachment)
	{
		pNewSupport = pNewAttachment->pActor;
		if (!CheckCCFlagForAttachment(*(pNewAttachment->pActor)))
			pNewAttachment = 0;
		else if (!pNewAttachment->pActor->isDynamic())
			pNewAttachment = 0;
		if (bIsToBeSlide)
			pNewAttachment = 0;

		assert(0 != pSweepHitShape);
		NxRay ray;
		ray.orig = GetCenterPosition();
		ray.dir = gDir;
		NxRaycastHit RaycastReport;
		RaycastReport.shape = 0;
		NxU32 flagRay = NX_RAYCAST_SHAPE | NX_RAYCAST_NORMAL | NX_RAYCAST_DISTANCE;
		scene.raycastClosestShape(ray, NX_ALL_SHAPES, RaycastReport, activeGroups, m_Height * 10, flagRay, &gmFilter, 0);
		if (RaycastReport.distance > m_Height * 0.5f)
		{
			if (pSweepHitShape != RaycastReport.shape)
			{
				float theGap = RaycastReport.distance - m_Height * 0.5f;
				if (theGap > skinWidth * 2)
					RaiseFlag(DCCII_ON_THE_EDGE);
			}
		}
	}

	if (bIsClearVVel || ReadFlag(DCCII_AUTO_STEP_WORKED_LAST))
	{
		NxVec3 velAttach(0.0f);
		if (0 != pNewAttachment)
			velAttach = pNewAttachment->pActor->getLinearVelocity();
		ClearVerticalVelocity(gDir, velAttach);
	}
	ApplyNewPosAndVel(*m_pDActor);

	UpdateStatus(IsHitSomething);
	UpdateAttachment(pNewAttachment);
	m_pSupport = pNewSupport;
}

void APhysXDynCCImp::ClearVerticalVelocity(const NxVec3& gDir, const NxVec3& velAttachment)
{
	NxVec3 velActor = m_pDActor->getLinearVelocity();
	NxVec3 velRelative = velActor - velAttachment;
	NxVec3 vVelRel = GetVerticalSubValue(velRelative, gDir);
	velRelative -= vVelRel;
	SetNewVel(velRelative + velAttachment);
}

bool APhysXDynCCImp::RevisePosition(const float skinWidth, const NxVec3& dirDetect, const NxSweepQueryHit& result, const float lenMotion)
{
	bool bToRevise = false;
	NxVec3 deltaMove = m_pDActor->getGlobalPosition() - m_LastPos;
	NxVec3 vDM = GetVerticalSubValue(deltaMove, dirDetect);
	NxVec3 hDM = deltaMove - vDM;
	
	static const float tanSDR = (float)tan(m_SlopeDownRadian);
	float fhDM = hDM.magnitude();
	const float distThreshold = tanSDR * fhDM + skinWidth;

	float theGap = result.t * lenMotion - (m_Height * 0.5f - m_Radius);
	if (theGap < distThreshold)
	{
		bToRevise = true;
		if (0 > theGap)
		{
			if (RevisePositionToAvoidOverlap(skinWidth, dirDetect, result, lenMotion))
				return bToRevise;
		}
	}
	else
	{
		if (m_pSupport ==  &result.hitShape->getActor())
			bToRevise = true;
	}
	if (ReadFlag(DCCII_AUTO_STEP_WORKED_LAST))
		bToRevise = true;

	if (bToRevise)
	{
		float f = theGap + skinWidth;
		if (0.05f < f)
		{
			NxVec3 oldPos = m_pDActor->getGlobalPosition();
			SetNewPos(oldPos + dirDetect * f);
		}
	}
	return bToRevise;
}

bool APhysXDynCCImp::RevisePositionToAvoidOverlap(const float skinWidth, const NxVec3& dirDetect, const NxSweepQueryHit& result, const float lenMotion)
{
	NxVec3 maxMotion = (m_Height * 0.5f - m_Radius - skinWidth) * dirDetect;
	NxVec3 deltaMove = result.t * lenMotion * dirDetect - maxMotion;
	float cosval = GetCosValue(deltaMove, dirDetect);
	if (0 < cosval)
		return false;

	NxVec3 oldPos = m_pDActor->getGlobalPosition();
	SetNewPos(oldPos + deltaMove);
	return true;
}

void APhysXDynCCImp::UpdateDir()
{
	if (AttachmentIsValid())
		m_pAttachment->pActor->getGlobalPose().M.multiply(m_pAttachment->localDir, m_faceDir);
}

void APhysXDynCCImp::UpdateStatus(bool bIsHitSomething)
{
	int newStatus = APX_CCS_JUMPING_INAIR;
	if (ReadFlag(DCCII_HANG_MODE))
		newStatus |= APX_CCS_HANGING;

	if (bIsHitSomething)
		newStatus = APX_CCS_STANDON_SOMETHING;
	
	if (APX_CCS_STANDON_SOMETHING == m_Status)
	{
		if (APX_CCS_JUMPING_INAIR == newStatus)
			newStatus |= APX_CCS_JUMPING_TAKEOFFE;
	}
	
	if (APX_CCS_JUMPING_INAIR == m_Status)
	{
		if (APX_CCS_STANDON_SOMETHING == newStatus)
		{
			newStatus |= APX_CCS_JUMPING_LANDING;
			newStatus &= ~APX_CCS_JUMPING_INAIR;
		}
	}
	m_Status = newStatus;

	m_DistToGround = NX_MAX_F32;
	if ((m_Status & APX_CCS_JUMPING_LANDING) || (m_Status & APX_CCS_STANDON_SOMETHING))
	{
		m_DistToGround = 0.0f;
		return;
	}

	NxCapsule worldCapsule;
	worldCapsule.p0 = worldCapsule.p1 = GetCenterPosition();
	worldCapsule.radius = m_Radius;

	NxU32 activeGroups = GetActiveGroups();
	NxGroupsMask gmFilter = GetFilterGMExcludeBody();

	float MaxSweepDist = GetDetectionDistance() * 3;
	if (m_Height * 2 > MaxSweepDist)
		MaxSweepDist = m_Height * 2;

	NxScene& scene = GetNxScene();
	NxVec3 g;
	scene.getGravity(g);
	NxVec3 gDir(g);
	gDir.normalize();
	NxVec3 motion = gDir * MaxSweepDist;
	NxSweepQueryHit sqHit;
	NxU32 nSweepHitShapes = scene.linearCapsuleSweep(worldCapsule, motion, NX_SF_STATICS | NX_SF_DYNAMICS, 0, 1, &sqHit, 0, activeGroups, &gmFilter);
	if (0 < nSweepHitShapes)
		m_DistToGround = sqHit.t * MaxSweepDist;

	m_DistToGround -= m_Height * 0.5f;
	if (0.0f > m_DistToGround)
		m_DistToGround = 0.0f;
}

void APhysXDynCCImp::UpdateAttachment(AttachedActor* pNewAttachment, bool bEnableChangeSType)
{
	NxVec3 g;
	GetNxScene().getGravity(g);

	const NxActor* pOldAttachActor = 0;
	bool reCreateJoint = false;
	if (0 == m_pAttachment)
	{
		if (0 != pNewAttachment)
		{
			reCreateJoint = true;
			m_AAInfo = *pNewAttachment;
			m_pAttachment = &m_AAInfo;
			if (bEnableChangeSType)
			{
				NxVec3 vel = m_pDActor->getLinearVelocity();
				NxVec3 velV = GetVerticalSubValue(vel, g);
				NxVec3 velH = vel - velV;
				if (velH.equals(gZero, APHYSX_FLOAT_EPSILON))
					m_CurrentSituation = ISituationConfigBase::DYNCC_S_STAND_AND_BIND;
				else
					m_CurrentSituation = ISituationConfigBase::DYNCC_S_CTRLMOVE_AND_BIND;
			}
			else
			{
				if (ISituationConfigBase::DYNCC_S_STAND_NOT_BIND == m_CurrentSituation)
					m_CurrentSituation = ISituationConfigBase::DYNCC_S_STAND_AND_BIND;
				else if (ISituationConfigBase::DYNCC_S_CTRLMOVE_NOT_BIND == m_CurrentSituation)
					m_CurrentSituation = ISituationConfigBase::DYNCC_S_CTRLMOVE_AND_BIND;
			}
		}
	}
	else
	{
		pOldAttachActor = m_pAttachment->pActor;
		if (0 == pNewAttachment)
		{
			reCreateJoint = true;
			m_pAttachment = 0;
			m_CurrentSituation = ISituationConfigBase::DYNCC_S_JUMP_OR_INAIR;
		}
		else
		{
			if (m_pAttachment->pActor == pNewAttachment->pActor)
			{
				if (ISituationConfigBase::DYNCC_S_STAND_AND_BIND != m_coreData.GetAppliedSituation())
					reCreateJoint = true;
				else if (!m_pAttachment->globalPos.equals(pNewAttachment->globalPos, APHYSX_FLOAT_EPSILON))
					reCreateJoint = true;
			}
			else
			{
				reCreateJoint = true;
			}
			m_AAInfo = *pNewAttachment;
		}
	}
	
	NxActor* pNewAttachActor = 0;
	if (0 == pNewAttachment)
	{
		if (pOldAttachActor == m_pSupport)
			m_pSupport = 0;
	}
	else
	{
		pNewAttachActor = const_cast<NxActor*>(pNewAttachment->pActor);
		m_pSupport = pNewAttachActor;
	}
	
	if (reCreateJoint)
	{
		m_coreData.ReleaseD6JointSetting();
		CreateD6JointSetting(pNewAttachActor);
		m_coreData.ApplyAllSettings();
		m_coreData.SetAppliedSituation(m_CurrentSituation);
	}

	if (0 != m_pAttachment)
	{
		NxMat34 pose = m_pAttachment->pActor->getGlobalPose();
		NxMat34 mat;
		pose.getInverse(mat);
		mat.M.multiply(m_GroundNormal, m_AttachPosLocalNormal);
	}
}

void APhysXDynCCImp::OnObjectDeleted(APhysXObject& obj)
{
	APhysXCCBase::OnObjectDeleted(obj);

	if (0 != m_pSupport)
	{
		APhysXObject* pObj = static_cast<APhysXObject*>(m_pSupport->userData);
		if (&obj == pObj)
			m_pSupport = 0;
	}

	if (0 != m_pAttachment)
	{
		APhysXObject* pObj = static_cast<APhysXObject*>(m_pAttachment->pActor->userData);
		if (&obj == pObj)
			UpdateAttachment(0);
	}
}

void APhysXDynCCImp::OnPutToSleep()
{
	m_LinearVelSleepBackup  = m_pDActor->getLinearVelocity();
	m_AngularVelSleepBackup = m_pDActor->getAngularVelocity();
	m_StateSleepBackup = m_coreData.GetAppliedSituation();

	if (ISituationConfigBase::DYNCC_S_JUMP_OR_INAIR != m_StateSleepBackup)
	{
		m_coreData.SetAppliedSituation(ISituationConfigBase::DYNCC_S_JUMP_OR_INAIR);
		ISituationConfigBase* pObjConfig = ISituationConfigBase::GetSituationConfigObject(m_StateSleepBackup);
		pObjConfig->Configuration(m_coreData);
	}
	m_coreData.SetNewKinamatic(true);
	m_coreData.ApplyAllSettings();
}

void APhysXDynCCImp::OnWakeUp()
{
	if (ISituationConfigBase::DYNCC_S_JUMP_OR_INAIR != m_StateSleepBackup)
	{
		m_coreData.SetAppliedSituation(m_StateSleepBackup);
		ISituationConfigBase* pObjConfig = ISituationConfigBase::GetSituationConfigObject(m_StateSleepBackup);
		pObjConfig->Configuration(m_coreData);
	}

	m_coreData.SetNewLinearVel(m_LinearVelSleepBackup);
	m_coreData.SetNewAngularVel(m_AngularVelSleepBackup);
	m_coreData.SetNewLinearVelOwner(true);
	m_coreData.SetNewKinamatic(false);
	m_coreData.ApplyAllSettings();
}

void APhysXDynCCImp::OnSetAutoHangEndAngle(const float radian)
{
	m_coreData.SetAutoHangEndThreshold(radian);
}

bool APhysXDynCCImp::AttachmentIsValid(bool* poutJointIsBroken) const
{
	if (0 != poutJointIsBroken)
		*poutJointIsBroken = false;
	if (0 == m_pAttachment)
		return false;

	if (m_coreData.IsBrokenD6joint())
	{
		if (0 != poutJointIsBroken)
			*poutJointIsBroken = true;
		return false;
	}

	if (0 != m_pAttachment->pActor)
	{
		APhysXObject* pObject = static_cast<APhysXObject*>(m_pAttachment->pActor->userData);
		if ((0 != pObject) && pObject->IsDeleted())
		{
			// attachment will be invalid, do not call UpdateAttachment() directly. We maybe visit the attachment info later.
			// UpdateAttachment() will be called in the moment of actor releasing call back.
			if (0 != poutJointIsBroken)
				*poutJointIsBroken = true;
			return false;
		}
	}

	return true;
}

NxVec3 APhysXDynCCImp::GetAttachedPointVelocity(const NxVec3& rtnDefaultValue)
{
	if (0 == m_pAttachment)
		return rtnDefaultValue;

	if (m_coreData.IsBrokenD6joint())
	{
		UpdateAttachment(0);
		return rtnDefaultValue;
	}

	if (0 != m_pAttachment->pActor)
		return m_pAttachment->pActor->getLocalPointVelocity(m_pAttachment->localPos);

	return rtnDefaultValue;
}

void APhysXDynCCImp::CreateD6JointSetting(NxActor* pActor0)
{
	NxD6JointDesc& descD6joint = m_coreData.CreateD6JointDesc();
	ISituationConfigBase* pObjConfig = ISituationConfigBase::GetSituationConfigObject(m_CurrentSituation);
	pObjConfig->Configuration(m_coreData);

	NxVec3 g;
	GetNxScene().getGravity(g);
	float magG = g.magnitude();
	descD6joint.xDrive.forceLimit = 2 * GetMass() * magG;
	descD6joint.yDrive.forceLimit = FLT_MAX;
	descD6joint.zDrive.forceLimit = 2 * GetMass() * magG;
	m_coreData.ResetActorIDOrientationImmediately();

	APhysXDynCCCoreData::JointInfo info;
	info.pActor0 = pActor0;
	info.globalAnchor = m_pDActor->getGlobalPosition();
	if (0 != pActor0)
		info.globalAnchor = m_pAttachment->globalPos;
	m_coreData.UpdateD6JointDescForActorAndFrameOnly(&info, NxVec3(1, 0, 0));
	assert(true == descD6joint.isValid());
}

NxGroupsMask APhysXDynCCImp::GetFilterGMExcludeBody(bool bEnforceCombineCCMRegGM) const
{
	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM(bEnforceCombineCCMRegGM);
	gmFilter.bits0 = ~gmFilter.bits0;
	gmFilter.bits1 = ~gmFilter.bits1;
	gmFilter.bits2 = ~gmFilter.bits2;
	gmFilter.bits3 = ~gmFilter.bits3;
	NxGroupsMask myself = m_pDActor->getShapes()[0]->getGroupsMask();
	gmFilter.bits0 |= myself.bits0;
	gmFilter.bits1 |= myself.bits1;
	gmFilter.bits2 |= myself.bits2;
	gmFilter.bits3 |= myself.bits3;
	gmFilter.bits0 = ~gmFilter.bits0;
	gmFilter.bits1 = ~gmFilter.bits1;
	gmFilter.bits2 = ~gmFilter.bits2;
	gmFilter.bits3 = ~gmFilter.bits3;
	return gmFilter;
}

bool APhysXDynCCImp::HangStart(const NxVec3& hangDir)
{
	NxScene& nxScene = GetNxScene();
	NxVec3 g;
	nxScene.getGravity(g);
	NxVec3 upDir(g);
	upDir *= -1;
	upDir.normalize();
	float cosVal= GetCosValue(hangDir, upDir);
	if (!ReadFlag(DCCII_HANG_MODE))
	{
		if (0.707 > cosVal)
			return false;
	}

	NxRay ray;
	ray.dir = hangDir;
	NxVec3 ccHangLocalPos(0.0f);
	m_pDActor->getGlobalPose().multiply(ccHangLocalPos, ray.orig);

	NxRaycastHit hit;
	NxU32  flagsRaycast = NX_RAYCAST_SHAPE | NX_RAYCAST_IMPACT | NX_RAYCAST_DISTANCE;
	NxShape* pHitShape = nxScene.raycastClosestShape(ray, NX_ALL_SHAPES, hit, GetActiveGroups(), GetMaxHangDistanceLimit(), flagsRaycast, &GetFilterGMExcludeBody(), 0);
	if (0 == pHitShape)
		return false;

	float hitDist = -1;
	if (NX_RAYCAST_DISTANCE & hit.flags)
		hitDist = hit.distance;
	if (GetMinHangDistanceLimit() > hitDist)
		return false;

	assert(0 != (NX_RAYCAST_IMPACT & hit.flags));
	assert(0 != (NX_RAYCAST_DISTANCE & hit.flags));
	const float delDist = m_Height * 0.5f;
	m_maxHangDistEnable = (hit.distance * cosVal) - delDist;
	float currentMaxHang = m_maxHangDistEnable;

	if (ReadFlag(DCCII_HANG_MODE))
	{
		NxRay rayDown;
		rayDown.dir = upDir * -1;
		rayDown.orig = hit.worldImpact;
		NxRaycastHit hitDown;
		NxU32  flagsRaycastD = NX_RAYCAST_SHAPE | NX_RAYCAST_IMPACT | NX_RAYCAST_DISTANCE;
		NxShape* pHitShapeD = nxScene.raycastClosestShape(rayDown, NX_ALL_SHAPES, hitDown, GetActiveGroups(), GetMaxHangDistanceLimit(), flagsRaycastD, &GetFilterGMExcludeBody(), 0);
		if (0 != pHitShape)
			m_maxHangDistEnable = hitDown.distance - delDist;

		if (hit.distance < m_maxHangDistEnable)
			currentMaxHang = hit.distance;
	}

	const float minDist = GetMinHangDistanceLimit();
	if (currentMaxHang < minDist)
		currentMaxHang = minDist;
	if (m_maxHangDistEnable < minDist)
		m_maxHangDistEnable = minDist;

	NxDistanceJointDesc distDesc;
    distDesc.actor[0] = &(pHitShape->getActor());
    distDesc.actor[1] = m_pDActor;
	NxMat34 pose = pHitShape->getActor().getGlobalPose();
	NxMat34 mat;
	pose.getInverse(mat);
	NxVec3 localPos;
	mat.multiply(hit.worldImpact, localPos);
	distDesc.localAnchor[0] = localPos;
	distDesc.localAnchor[1] = ccHangLocalPos;
	distDesc.maxDistance = currentMaxHang;
	distDesc.minDistance = minDist;
    distDesc.flags = NX_DJF_MAX_DISTANCE_ENABLED | NX_DJF_MIN_DISTANCE_ENABLED | NX_DJF_SPRING_ENABLED;
	distDesc.spring.spring = 30 * GetMass();
	distDesc.spring.damper = 1.5f * GetMass();
	distDesc.jointFlags |= NX_JF_COLLISION_ENABLED;

	APhysXScene& scene = GetAPhysXScene();
	scene.LockWritingScene();
	NxDistanceJoint* pNewDJ = static_cast<NxDistanceJoint*>(nxScene.createJoint(distDesc));
	if (0 != pNewDJ)
	{
		m_coreData.SetHangDistanceJoint(pNewDJ, false);
		m_pDActor->clearBodyFlag(NX_BF_FROZEN_ROT);
	}
	scene.UnlockWritingScene();
	if (0 == pNewDJ)
		return false;

	ClearFlag(DCCII_HANG_END);
	RaiseFlag(DCCII_HANG_MODE);
	RaiseFlag(DCCII_HANG_START);
	return true;
}

void APhysXDynCCImp::HangEnd()
{
	if (!ReadFlag(DCCII_HANG_MODE))
		return;

	ClearFlag(DCCII_HANG_START);
	RaiseFlag(DCCII_HANG_END);
}

bool APhysXDynCCImp::GetHangGlobalAnchor0(NxVec3& outAnchor) const
{
	if (ReadFlag(DCCII_HANG_MODE))
		return m_coreData.GetHangGlobalAnchor0(outAnchor);
	return false;
}

bool APhysXDynCCImp::AutoStepPrecondition(const	NxSweepQueryHit& sweepHit, const NxVec3& gDir, const NxVec3& hvelMoveDir, const float hvelSpeed,
										  const NxU32 activeGroups, const NxGroupsMask& gmFilter, const NxCapsuleShape& capShape)
{
	// the case is slope-up enough
	float cosVal = GetCosValue(sweepHit.normal, gDir);
	if (-0.707f > cosVal)
		return false;
	
	NxActor& hitActor = sweepHit.hitShape->getActor();
	if (hitActor.isDynamic())
	{
		if (!hitActor.readBodyFlag(NX_BF_KINEMATIC))
		{
			// the case is approximately the same direction between cc move-dir and actor vel-dir && actor vel is not very slowly 
			NxVec3 velActor = hitActor.getLinearVelocity();
			NxVec3 vvelActor = GetVerticalSubValue(velActor, gDir);
			NxVec3 hvelActor = velActor - vvelActor;
			float cosValVel = GetCosValue(hvelActor, hvelMoveDir);
			if (0.95f < cosValVel)
			{
				float hActorSpeed = hvelActor.magnitude();
				if (hActorSpeed > hvelSpeed * 0.5f)
					return false;
			}
		}
	}
	
	// the case is cc hit something where is not in front dir(cc move-dir) && sweep a small part of actor
	NxVec3 vNormal = GetVerticalSubValue(sweepHit.normal, gDir);
	NxVec3 hNormal = sweepHit.normal - vNormal;
	hNormal.normalize();
	float cosValHN = GetCosValue(hNormal, hvelMoveDir * -1);
	if (0.866f > cosValHN)    // 0.866 = cos(30)
	{
		float h = capShape.getHeight() * 0.5f;
		NxMat34 pose = m_pDActor->getGlobalPose();
		NxVec3 actorBTMCenterPos;
		NxVec3 btmLocalCenterPos(0, -h, 0);
		pose.multiply(btmLocalCenterPos, actorBTMCenterPos);

		NxVec3 dv = sweepHit.point - actorBTMCenterPos;
		NxVec3 vdv = GetVerticalSubValue(dv, gDir);
		NxVec3 hdv = dv - vdv;
		
		NxRay ray;
		ray.orig = sweepHit.point - hdv;
		ray.dir = hvelMoveDir;
		NxRaycastHit RaycastReport;
		RaycastReport.shape = 0;
		GetNxScene().raycastClosestShape(ray, NX_ALL_SHAPES, RaycastReport, activeGroups, NX_MAX_F32, NX_RAYCAST_SHAPE, &gmFilter, 0);
		if (RaycastReport.shape != sweepHit.hitShape)
			return false;
	}

	return true;
}

void APhysXDynCCImp::AutoStep(const NxVec3& velOriMove, const NxVec3& velMove, const float deltaSecond)
{
	assert(true == ReadFlag(DCCII_AUTO_STEP_UP_ENABLE));
	if (velMove.equals(gZero, APHYSX_FLOAT_EPSILON))
		return;
	if ((ISituationConfigBase::DYNCC_S_CTRLMOVE_NOT_BIND != m_CurrentSituation) && (ISituationConfigBase::DYNCC_S_CTRLMOVE_AND_BIND != m_CurrentSituation))
		return;

	NxScene& nxScene = GetNxScene();
	NxVec3 g;
	nxScene.getGravity(g);
	NxVec3 gDir(g);
	gDir.normalize();
	NxVec3 upDir = gDir * -1;

	NxCapsuleShape* pCapShape = m_pDActor->getShapes()[0]->isCapsule();
	assert(0 != pCapShape);
	NxCapsule worldCapsule;
	pCapShape->getWorldCapsule(worldCapsule);
	worldCapsule.radius -= pCapShape->getSkinWidth();
	worldCapsule.radius *= 0.9f;
	float dr = pCapShape->getRadius() - worldCapsule.radius;

	NxVec3 motion(0.0f);
	NxSweepQueryHit sweepHit; 
	NxU32 flags = NX_SF_STATICS | NX_SF_DYNAMICS;
	NxU32 activeGroups = GetActiveGroups();
	NxGroupsMask gmFilter = GetFilterGMExcludeBody();

	NxVec3 vVelMove = GetVerticalSubValue(velMove, gDir);
	NxVec3 hvelMove = velMove - vVelMove;
	float hvelSpeed = hvelMove.magnitude(); 
	NxVec3 hvelMoveDir = hvelMove;
	hvelMoveDir.normalize();
	if (ReadFlag(DCCII_ON_THE_EDGE))
	{
		ClearFlag(DCCII_ON_THE_EDGE);
		float cosV = GetCosValue(velOriMove, GetGroundNormal());
		if (0 < cosV) // from edge to outside
			return;
	}
	else
	{
		motion = hvelMoveDir * (hvelSpeed * deltaSecond + dr);
		NxU32 nHitShapes = nxScene.linearCapsuleSweep(worldCapsule, motion, flags, 0, 1, &sweepHit, 0, activeGroups, &gmFilter);
		if (0 == nHitShapes)
			return;

		if (!ReadFlag(DCCII_AUTO_STEP_WORKED_LAST))
		{
			if (!AutoStepPrecondition(sweepHit, gDir, hvelMoveDir, hvelSpeed, activeGroups, gmFilter, *pCapShape))
				return;
		}
	}
	
	// up
	sweepHit.t = 1.0f;
	motion = upDir * (GetStepOffset() + dr);
	nxScene.linearCapsuleSweep(worldCapsule, motion, flags, 0, 1, &sweepHit, 0, activeGroups, &gmFilter);
	NxVec3 deltaUpMove = motion * sweepHit.t - upDir * dr;
	if (0.0f > GetCosValue(deltaUpMove, upDir))
		deltaUpMove.zero();
	deltaUpMove -= upDir * dr;
	worldCapsule.p0 += deltaUpMove;
	worldCapsule.p1 += deltaUpMove;

	// horizon predict
	motion = hvelMoveDir * (m_Radius + dr);
	flags = NX_SF_STATICS | NX_SF_DYNAMICS | NX_SF_ALL_HITS;
	EntityReportSweepForClosestNonDynamic sweepResult;
	nxScene.linearCapsuleSweep(worldCapsule, motion, flags, 0, 0, 0, &sweepResult, activeGroups, &gmFilter);
	if (sweepResult.IsHitDynamicStuff())
		return;
	NxSweepQueryHit* pResult = sweepResult.GetHitResult();
	if (0 != pResult)
	{
		NxVec3 vhitNormal = GetVerticalSubValue(pResult->normal, gDir);
		NxVec3 hhitNormal = pResult->normal - vhitNormal; 
		NxVec3 hHitVelMove = GetVerticalSubValue(hvelMove, hhitNormal);
		NxVec3 hSubVelMove = hvelMove - hHitVelMove;
		m_coreData.SetNewLinearVel(hSubVelMove + vVelMove);
		return;
	}
	NxVec3 deltaPreHznMove = motion - hvelMoveDir * dr;
	NxCapsule wCaptemp(worldCapsule);
	wCaptemp.p0 += deltaPreHznMove;
	wCaptemp.p1 += deltaPreHznMove;

	// down move base on horizon predict(to get the normal)
	motion = gDir * (GetStepOffset() + dr) + g * deltaSecond * 2;
	flags = NX_SF_STATICS | NX_SF_DYNAMICS;
	sweepResult.Reset();
	nxScene.linearCapsuleSweep(wCaptemp, motion, flags, 0, 0, 0, &sweepResult, activeGroups, &gmFilter);
	pResult = sweepResult.GetHitResult();
	if (0 != pResult)
	{
		if (!ReadFlag(DCCII_AUTO_STEP_WORKED_LAST))
		{
			NxVec3 normal = pResult->normal;
			float cosRes = GetCosValue(pResult->normal, upDir);
			if (m_SlideThreshold > cosRes)
				return;
		}
	}

	// horizon real
	sweepHit.t = 1.0f;
	motion = hvelMoveDir * (hvelSpeed * deltaSecond + dr);
	flags = NX_SF_STATICS | NX_SF_DYNAMICS;
	NxU32 nHitShapes = nxScene.linearCapsuleSweep(worldCapsule, motion, flags, 0, 1, &sweepHit, 0, activeGroups, &gmFilter);
	assert(0 == nHitShapes);
	NxVec3 deltaHznMove = motion - hvelMoveDir * dr;
	worldCapsule.p0 += deltaHznMove;
	worldCapsule.p1 += deltaHznMove;
	m_coreData.ClearUnknownOwnerNewLinearVel();

	// down real
	worldCapsule.radius += dr;
	worldCapsule.p0 += upDir * dr;
	worldCapsule.p1 += upDir * dr;
	motion = gDir * GetStepOffset() + g * deltaSecond * 2;
	NxVec3 btmCenterPos = worldCapsule.p1;
	NxVec3 capDir = worldCapsule.p0 - worldCapsule.p1;
	float cosVal= GetCosValue(capDir, upDir);
	if (0 > cosVal)
		btmCenterPos = worldCapsule.p0;
	m_SweepReport.Reset(gDir, m_faceDir, velOriMove, btmCenterPos, worldCapsule.radius, false);
	nxScene.linearCapsuleSweep(worldCapsule, motion, flags, 0, 0, 0, &m_SweepReport, activeGroups, &gmFilter);

	NxVec3 deltaDownMove(0.0f);
	NxShape* pSweepHitShape = m_SweepReport.GetHitShape();
	if (0 != pSweepHitShape)
	{
		NxSweepQueryHit* pResult = m_SweepReport.GetHitResult();
		deltaDownMove += motion * pResult->t;
	}

	NxVec3 oldPos = m_pDActor->getGlobalPosition();
	NxVec3 newPos = (worldCapsule.p0 + worldCapsule.p1) * 0.5f + deltaDownMove;
	NxVec3 move = newPos - oldPos;
	NxVec3 vMove = GetVerticalSubValue(move, g);
	NxVec3 hMove = move - vMove;
	float hM = hMove.magnitude();
	if (pCapShape->getSkinWidth() > hM)
		return;

	if (0 == pSweepHitShape)
	{
		m_CurrentSituation = ISituationConfigBase::DYNCC_S_JUMP_OR_INAIR;
	}
	else
	{
		m_CurrentSituation = ISituationConfigBase::DYNCC_S_CTRLMOVE_NOT_BIND;
		newPos += gDir * pCapShape->getSkinWidth();
		SetNewPos(newPos);

		AttachedActor* pNewAttachment = m_SweepReport.GetSupportActor(false);
		m_pSupport = pNewAttachment->pActor;
		m_GroundNormal = m_SweepReport.GetNormal(false);
		if (CheckSlideAction())
		{
			pNewAttachment = 0;
			m_CurrentSituation = ISituationConfigBase::DYNCC_S_SLIDE_CTRL;
		}
		else
		{
			if (!CheckCCFlagForAttachment(*(pNewAttachment->pActor)))
				pNewAttachment = 0;
			else if (!pNewAttachment->pActor->isDynamic())
				pNewAttachment = 0;
			else
				m_CurrentSituation = ISituationConfigBase::DYNCC_S_CTRLMOVE_AND_BIND;
		}
		UpdateAttachment(pNewAttachment, false);
	}

	RaiseFlag(DCCII_AUTO_STEP_IS_WORKING);
	ApplyNewPosAndVel(*m_pDActor);
}
