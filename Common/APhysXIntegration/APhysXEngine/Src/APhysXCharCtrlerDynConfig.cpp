#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerImp.h"
#include "APhysXCharCtrlerDynConfig.h"

bool ISituationConfigBase::EqualJudgement(const Situation a, const Situation b)
{
	static bool bInited = false;
	static bool table[DYNCC_COUNTER_BOUND][DYNCC_COUNTER_BOUND];
	if (!bInited)
	{
		bInited = true;
		for (int i = 0; i < DYNCC_COUNTER_BOUND; ++i)
		{
			for (int j = 0; j < DYNCC_COUNTER_BOUND; ++j)
			{
				table[i][j] = false;
				if (i == j)
					table[i][j] = true;
			}
		}

		table[DYNCC_S_INIT_DEFAULT][DYNCC_S_INIT_DEFAULT] = false;

		table[DYNCC_S_STAND_NOT_BIND][DYNCC_S_CTRLMOVE_NOT_BIND] = true;
		table[DYNCC_S_CTRLMOVE_NOT_BIND][DYNCC_S_STAND_NOT_BIND] = true;
		table[DYNCC_S_STAND_NOT_BIND][DYNCC_S_SLIDE_CTRL]		 = true;
		table[DYNCC_S_SLIDE_CTRL][DYNCC_S_STAND_NOT_BIND]		 = true;
		table[DYNCC_S_CTRLMOVE_NOT_BIND][DYNCC_S_SLIDE_CTRL]	 = true;
		table[DYNCC_S_SLIDE_CTRL][DYNCC_S_CTRLMOVE_NOT_BIND]	 = true;

		table[DYNCC_S_SLIDE_FREE][DYNCC_S_JUMP_START]	= true;
		table[DYNCC_S_JUMP_START][DYNCC_S_SLIDE_FREE]	= true;
		table[DYNCC_S_SLIDE_FREE][DYNCC_S_JUMP_OR_INAIR] = true;
		table[DYNCC_S_JUMP_OR_INAIR][DYNCC_S_SLIDE_FREE] = true;
		table[DYNCC_S_JUMP_START][DYNCC_S_JUMP_OR_INAIR] = true;
		table[DYNCC_S_JUMP_OR_INAIR][DYNCC_S_JUMP_START] = true;

		table[DYNCC_S_HANG_END][DYNCC_S_HANG_END_JUMPING] = true;
		table[DYNCC_S_HANG_END_JUMPING][DYNCC_S_HANG_END] = true;
	}

	return table[a][b];
}

ISituationConfigBase* ISituationConfigBase::GetSituationConfigObject(const Situation s)
{
	ISituationConfigBase* pOBj = 0;
	switch (s)
	{
	case DYNCC_S_STAND_NOT_BIND:
		{
			static SCStandNotBind theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_STAND_AND_BIND:
		{
			static SCStandAndBind theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_CTRLMOVE_NOT_BIND:
		{
			static SCCtrlMoveNotBind theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_CTRLMOVE_AND_BIND:
		{
			static SCCtrlMoveAndBind theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_SLIDE_FREE:
		{
			static SCSlideFree theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_SLIDE_CTRL:
		{
			static SCSlideCtrl theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_JUMP_START:
		{
			static SCJumpStart theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_JUMP_OR_INAIR:
		{
			static SCJumpOrInair theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_HANG_START:
		{
			static SCHangStart theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_HANGING:
		{
			static SCHanging theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_HANG_END:
		{
			static SCHangEnd theOne;
			pOBj = &theOne;
		}
		break;

	case DYNCC_S_HANG_END_JUMPING:
		{
			static SCHangEndJumping theOne;
			pOBj = &theOne;
		}
		break;
	case DYNCC_S_HANG_POST:
		{
			static SCHangPost theOne;
			pOBj = &theOne;
		}
		break;
	default:
		assert(!"Oops! Unknown Situation!");
		break;
	};
	return pOBj;
}

APhysXDynCCCoreData::APhysXDynCCCoreData(APhysXScene& scene, NxActor& actor)
	: m_pAPhysXScene(&scene), m_pDActor(&actor)
{
	m_pjntD6Ctrl = 0;
	m_pjntDistHang = 0;
	ClearAction();
	RaiseFlag(TUA_D6JOINT_DESC_CHANGED);
	m_appliedSituation = ISituationConfigBase::DYNCC_S_INIT_DEFAULT;

	m_ToApplyDominance = 0;
	m_ToApplyPose.id();
	m_ToApplyKinamatic = false;
	m_ToApplyFrozenRot = false;

	m_ToApplyLinearVel.zero();
	m_ToApplyAngularVel.zero();
	m_ToApplyPosition.zero();
	m_ToApplyForce.zero();

	m_distHangRope = 0.0f;
	m_dirHangRope.zero();
	m_dirHangFaceTo.zero();
}

ISituationConfigBase::Situation APhysXDynCCCoreData::GetAppliedSituation() const
{
	assert(ISituationConfigBase::DYNCC_COUNTER_BOUND != m_appliedSituation);
	return m_appliedSituation;
}

float APhysXDynCCCoreData::GetActorMass() const
{
	if (0 == m_pDActor)
		return 0.0f;

	return m_pDActor->getMass();
}

float APhysXDynCCCoreData::GetCurrentHangRopeDistance() const
{
	if (0 == m_pjntDistHang)
		return 0.0f;

	return m_distHangRope;
}

float APhysXDynCCCoreData::GetCurrentHangJointDistance() const
{
	if (0 == m_pjntDistHang)
		return 0.0f;

	return m_ToApplyDescDJ.maxDistance;
}

bool APhysXDynCCCoreData::GetHangGlobalAnchor0(NxVec3& outAnchor) const
{
	if (0 == m_pjntDistHang)
		return false;
	
	if (NX_JS_BROKEN == m_pjntDistHang->getState())
		return false;

	m_ToApplyDescDJ.actor[0]->getGlobalPose().multiply(m_ToApplyDescDJ.localAnchor[0], outAnchor);
	return true;
}

bool APhysXDynCCCoreData::IsBrokenD6joint() const
{
	if (0 != m_pjntD6Ctrl)
	{
		if (NX_JS_BROKEN == m_pjntD6Ctrl->getState())
			return true;
	}
	return false;
}

bool APhysXDynCCCoreData::IsDistanceJointAvailable(bool& outIsBroken) const
{
	outIsBroken = false;
	if (0 == m_pjntDistHang)
		return false;

	if (NX_JS_BROKEN == m_pjntDistHang->getState())
	{
		outIsBroken = true;
		return false;
	}
	
	return true;
}

bool APhysXDynCCCoreData::IsNotHangAndPossibleInAir() const
{
	bool rtn = ISituationConfigBase::EqualJudgement(ISituationConfigBase::DYNCC_S_JUMP_OR_INAIR, m_appliedSituation);
	if (!rtn)
		rtn = AppliedSituationBelongToHangEnd();
	if (!rtn)
	{
		if (ISituationConfigBase::DYNCC_S_HANG_POST == m_appliedSituation)
			rtn = true;
	}
	return rtn;
}

bool APhysXDynCCCoreData::AppliedSituationBelongToHangEnd() const
{
	const ISituationConfigBase::Situation s = GetAppliedSituation();
	if ((ISituationConfigBase::DYNCC_S_HANG_END == s) || (ISituationConfigBase::DYNCC_S_HANG_END_JUMPING == s))
		return true;

	return false;
}

void APhysXDynCCCoreData::SetHangDistanceJoint(NxDistanceJoint* pJoint, bool bIsSafeMode)
{
	if (0 != m_pjntDistHang)
	{
		if (bIsSafeMode)
			m_pAPhysXScene->LockWritingScene();
		m_pAPhysXScene->GetNxScene()->releaseJoint(*m_pjntDistHang);
		if (bIsSafeMode)
			m_pAPhysXScene->UnlockWritingScene();
	}

	m_pjntDistHang = pJoint;
	if (0 != m_pjntDistHang)
		m_pjntDistHang->saveToDesc(m_ToApplyDescDJ);
}

bool APhysXDynCCCoreData::ComputeHangRopeDirAndDist(NxVec3* poutDir)
{
	if (0 == m_pjntDistHang)
		return false;

	NxVec3 posHang0;
	m_ToApplyDescDJ.actor[0]->getGlobalPose().multiply(m_ToApplyDescDJ.localAnchor[0], posHang0);
	NxVec3 posHang1;
	m_ToApplyDescDJ.actor[1]->getGlobalPose().multiply(m_ToApplyDescDJ.localAnchor[1], posHang1);

	m_dirHangRope = posHang1 - posHang0;
	m_distHangRope = m_dirHangRope.magnitude();
	m_dirHangRope.normalize();

	if (0 != poutDir)
		*poutDir = m_dirHangRope;
	return true;
}

bool APhysXDynCCCoreData::ComputeActorStandPose()
{
	NxCapsuleShape* pShape = m_pDActor->getShapes()[0]->isCapsule();
	assert(0 != pShape);

	float h = pShape->getHeight() * 0.5f;
	NxMat34 pose = m_pDActor->getGlobalPose();

	NxVec3 btmPos;
	NxVec3 btmLocalPos(0, -h, 0);
	pose.multiply(btmLocalPos, btmPos);
	NxVec3 dirLeg = btmPos - pose.t;
	dirLeg.normalize();

	NxVec3 g;
	m_pAPhysXScene->GetNxScene()->getGravity(g);
	g.normalize();

	float cosRes = dirLeg.dot(g);
	if (NxMath::equals(1.0f, cosRes, 0.001f))
	{
		ResetActorIDOrientationImmediately();
		SetNewFrozenRot(true);
		UpdateD6JointDescForActorAndFrameOnly(0, NxVec3(1, 0, 0));
		return true;
	}

	return false;
}

bool APhysXDynCCCoreData::ReadFlag(const ToUpdateAction flag) const
{
	if (TUA_DEFAULT_EMPTY == flag)
		return TUA_DEFAULT_EMPTY == m_flags;

	return (m_flags & flag)? true : false;
}

void APhysXDynCCCoreData::ClearFlag(const ToUpdateAction flag)
{
	if (TUA_DEFAULT_EMPTY == flag)
		return;

	m_flags &= ~flag;
}

void APhysXDynCCCoreData::RaiseFlag(const ToUpdateAction flag)
{
	if (TUA_DEFAULT_EMPTY == flag)
	{
		m_flags = TUA_DEFAULT_EMPTY;
		return; 
	}

	m_flags |= flag;
}

void APhysXDynCCCoreData::SetNewDominance(const int value)
{
	m_ToApplyDominance = value;
	RaiseFlag(TUA_DOMINANCE);
}

void APhysXDynCCCoreData::ResetIDOrientation()
{
	m_ToApplyPose.id();
	RaiseFlag(TUA_D6JOINT_ORIENTATION);
}

void APhysXDynCCCoreData::ResetActorIDOrientationImmediately()
{
	assert(0 != m_pDActor);
	if (0 == m_pDActor)
		return;

	NxMat33 mat = m_pDActor->getGlobalOrientation();
	if (mat.isIdentity())
		return;

	m_ToApplyPose.id();
	NxScene* pNxScene = m_pAPhysXScene->GetNxScene();
	m_pAPhysXScene->LockWritingScene();
	m_pDActor->setGlobalOrientationQuat(m_ToApplyPose);
	m_pAPhysXScene->UnlockWritingScene();
}

void APhysXDynCCCoreData::SetNewKinamatic(const bool bIsToRaise)
{
	m_ToApplyKinamatic = bIsToRaise;
	RaiseFlag(TUA_FLAG_KINEMATIC);
}

void APhysXDynCCCoreData::SetNewFrozenRot(const bool bIsToRaise)
{
	m_ToApplyFrozenRot = bIsToRaise;
	RaiseFlag(TUA_FLAG_FROZEN_ROTATION);
}

void APhysXDynCCCoreData::SetNewLinearVel(const NxVec3& value)
{
	m_ToApplyLinearVel = value;
	RaiseFlag(TUA_DEBUG_LINEAR_VELOCITY);
}

void APhysXDynCCCoreData::SetNewAngularVel(const NxVec3& value)
{
	m_ToApplyAngularVel = value;
	RaiseFlag(TUA_ACTOR_ANGULAR_VELOCITY);
}

void APhysXDynCCCoreData::ClearUnknownOwnerNewLinearVel()
{
	if (ReadFlag(TUA_DEBUG_LINEAR_VELOCITY))
		ClearFlag(TUA_DEBUG_LINEAR_VELOCITY);
}

void APhysXDynCCCoreData::SetNewLinearVelOwner(const bool bIsToActor)
{
	if (ReadFlag(TUA_DEBUG_LINEAR_VELOCITY))
	{
		ClearFlag(TUA_DEBUG_LINEAR_VELOCITY);
		if (bIsToActor)
			RaiseFlag(TUA_ACTOR_LINEAR_VELOCITY);
		else
			RaiseFlag(TUA_JOINT_LINEAR_VELOCITY);
	}
}

void APhysXDynCCCoreData::SetNewPosition(const NxVec3& value)
{
	m_ToApplyPosition = value;
	RaiseFlag(TUA_ACTOR_GLOBAL_POSITION);
}

void APhysXDynCCCoreData::SetNewForce(const NxVec3& value)
{
	m_ToApplyForce = value;
	RaiseFlag(TUA_ACTOR_ADD_FORCE);
}

NxDistanceJointDesc& APhysXDynCCCoreData::GetDistJointDesc(bool& outIsValid)
{
	outIsValid = false;
	if (0 != m_pjntDistHang)
		outIsValid = true;

	return m_ToApplyDescDJ;
}

NxD6JointDesc& APhysXDynCCCoreData::CreateD6JointDesc()
{
	m_ToApplyDescD6.setToDefault();
	// there is a PhysX issue, we have to set the default value manually.
	m_ToApplyDescD6.linearLimit.setToDefault();
	m_ToApplyDescD6.swing1Limit.setToDefault();
	m_ToApplyDescD6.swing2Limit.setToDefault();
	m_ToApplyDescD6.twistLimit.setToDefault();
	NxJointDriveDesc def;
	m_ToApplyDescD6.xDrive = def;
	m_ToApplyDescD6.yDrive = def;
	m_ToApplyDescD6.zDrive = def;
	m_ToApplyDescD6.swingDrive = def;
	m_ToApplyDescD6.twistDrive = def;
	m_ToApplyDescD6.slerpDrive = def;
	m_ToApplyDescD6.jointFlags |= NX_JF_COLLISION_ENABLED;
	RaiseFlag(TUA_D6JOINT_DESC_CHANGED);
	RaiseFlag(TUA_CREATE_D6JOINT_DESC);
	ClearFlag(TUA_RELOAD_D6JOINT_DESC);
	return m_ToApplyDescD6;
}

NxD6JointDesc& APhysXDynCCCoreData::UpdateD6JointDesc()
{
	if (!ReadFlag(TUA_CREATE_D6JOINT_DESC))
	{
		m_ToApplyDescD6.xMotion = NX_D6JOINT_MOTION_FREE;
		m_ToApplyDescD6.yMotion = NX_D6JOINT_MOTION_FREE;
		m_ToApplyDescD6.zMotion = NX_D6JOINT_MOTION_FREE;
		m_ToApplyDescD6.swing1Motion = NX_D6JOINT_MOTION_FREE;
		m_ToApplyDescD6.swing2Motion = NX_D6JOINT_MOTION_FREE;
		m_ToApplyDescD6.twistMotion  = NX_D6JOINT_MOTION_FREE;
		m_ToApplyDescD6.xDrive.driveType = 0;
		m_ToApplyDescD6.yDrive.driveType = 0;
		m_ToApplyDescD6.zDrive.driveType = 0;
		m_ToApplyDescD6.slerpDrive.driveType = 0;
		m_ToApplyDescD6.flags = 0;
		RaiseFlag(TUA_D6JOINT_DESC_CHANGED);
		RaiseFlag(TUA_RELOAD_D6JOINT_DESC);
	}
	return m_ToApplyDescD6;
}

void APhysXDynCCCoreData::UpdateD6JointDescForHang(const NxVec3& dirHangFaceTo, const float mass)
{
	assert(0 != m_pjntDistHang);
	if (0 == m_pjntDistHang)
		return;

	m_dirHangFaceTo = dirHangFaceTo;
	m_dirHangFaceTo.normalize();

	NxD6JointDesc& descD6 = UpdateD6JointDesc();
	descD6.zMotion = NX_D6JOINT_MOTION_LOCKED;
	descD6.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
	descD6.slerpDrive.spring = 700 * mass;
	descD6.slerpDrive.damping = 0;
	descD6.flags |= NX_D6JOINT_SLERP_DRIVE;
	UpdateD6JointDescForActorAndFrameOnly(0, m_dirHangFaceTo, true);
}

void APhysXDynCCCoreData::UpdateD6JointDescForActorAndFrameOnly(JointInfo* pInfo, const NxVec3& globalAxis, const bool bIsActor1IDPose)
{
	assert(0 != m_pDActor);
	NxVec3 g;
	m_pAPhysXScene->GetNxScene()->getGravity(g);
	NxVec3 up = g * -1;
	up.normalize();

	NxActor* pActor0 = 0;
	NxVec3 ga = m_pDActor->getGlobalPosition();
	if (0 != pInfo)
	{
		pActor0 = pInfo->pActor0;
		ga = pInfo->globalAnchor;
	}
	m_ToApplyDescD6.actor[0] = pActor0;
	m_ToApplyDescD6.actor[1] = m_pDActor;
	m_ToApplyDescD6.setGlobalAnchor(ga);
	m_ToApplyDescD6.setGlobalAxis(globalAxis);

	NxVec3 localNormal0(up);
	if (0 != pActor0)
	{
		NxMat33 mat0;
		pActor0->getGlobalOrientation().getInverse(mat0);
		mat0.multiply(localNormal0, localNormal0);
	}
	m_ToApplyDescD6.localNormal[0] = localNormal0;

	NxVec3 localNormal1(up);
	if (!bIsActor1IDPose)
	{
		NxMat33 mat1;
		m_pDActor->getGlobalOrientation().getInverse(mat1);
		mat1.multiply(localNormal1, localNormal1);
	}
	m_ToApplyDescD6.localNormal[1] = localNormal1;

	RaiseFlag(TUA_D6JOINT_DESC_CHANGED);
	if (!ReadFlag(TUA_CREATE_D6JOINT_DESC))
		RaiseFlag(TUA_RELOAD_D6JOINT_DESC);
}

void APhysXDynCCCoreData::UpdateOrientationForHang()
{
	assert(0 != m_pjntDistHang);
	if (0 == m_pjntDistHang)
		return;
	
	NxVec3 g;
	m_pAPhysXScene->GetNxScene()->getGravity(g);
	g.normalize();

	// note here: m_ToApplyPose will apply to D6joint angular drives, and that is relation to the actor[0] frame 
	// so we have to compute the rotation from global frame to D6joint frame at first
	NxVec3 LocalX(1, 0, 0);
	NxVec3 LocalZ(0, 0, 1);
	float sign = 1;
	float cosSign = m_dirHangFaceTo.dot(LocalZ);
	if (0 < cosSign)
		sign = -1;

	float cosRotG2L = m_dirHangFaceTo.dot(LocalX);
	float thetaG2LRad = NxMath::acos(cosRotG2L);
	float thetaG2LDeg = NxMath::radToDeg(thetaG2LRad); 
	NxQuat rotG2L(thetaG2LDeg, g * sign);
	
	float cosHanging = m_dirHangRope.dot(g);
	float thetaRad = NxMath::acos(cosHanging);

	NxVec3 LocalHangRopeDir = rotG2L.rot(m_dirHangRope);
	NxVec3 LocalRealFaceTo = LocalZ.cross(LocalHangRopeDir);
	if (0 < LocalRealFaceTo.dot(g))
		thetaRad *= -1;

	m_ToApplyPose.fromAngleAxisFast(thetaRad, LocalZ);
	RaiseFlag(TUA_D6JOINT_ORIENTATION);

	float AbsthetaRad = NxMath::abs(thetaRad);
	if (m_autoHangEndT < AbsthetaRad)
	{
		ClearFlag(TUA_RELOAD_DJJOINT_DESC);
		RaiseFlag(TUA_FLAG_AUTO_HANG_END);
	}
}

void APhysXDynCCCoreData::ApplyAllSettings()
{
	assert(false == ReadFlag(TUA_DEBUG_LINEAR_VELOCITY));
	if (ReadFlag(TUA_DEFAULT_EMPTY))
		return;

	NxScene* pNxScene = m_pAPhysXScene->GetNxScene();
	m_pAPhysXScene->LockWritingScene();

	// do not change the sequence 
	OnApplySettingsRelease(*pNxScene);
	OnApplySettingsCreateAndReload(*pNxScene);
	OnApplySettingsAttribution();

	m_pAPhysXScene->UnlockWritingScene();

	ClearAction();
}

void APhysXDynCCCoreData::OnApplySettingsRelease(NxScene& scene)
{
	if(ReadFlag(TUA_RELEASE_D6JOINT))
	{
		if (0 != m_pjntD6Ctrl)
		{
			scene.releaseJoint(*m_pjntD6Ctrl);
			m_pjntD6Ctrl = 0;
		}
	}
	if (ReadFlag(TUA_RELEASE_DISTTANT_JOINT))
	{
		if (0 != m_pjntDistHang)
		{
			scene.releaseJoint(*m_pjntDistHang);
			m_pjntDistHang = 0;
		}
	}
	if (ReadFlag(TUA_RELEASE_ACTOR))
	{
		if (0 != m_pDActor)
		{
			scene.releaseActor(*m_pDActor);
			m_pDActor = 0;
		}
	}
}

void APhysXDynCCCoreData::OnApplySettingsCreateAndReload(NxScene& scene)
{
	if (ReadFlag(TUA_CREATE_D6JOINT_DESC))
		m_pjntD6Ctrl = static_cast<NxD6Joint*>(scene.createJoint(m_ToApplyDescD6));

	if (ReadFlag(TUA_RELOAD_D6JOINT_DESC))
	{
		assert(0 != m_pjntD6Ctrl);
		if (0 != m_pjntD6Ctrl)
			m_pjntD6Ctrl->loadFromDesc(m_ToApplyDescD6);
	}

	if (ReadFlag(TUA_RELOAD_DJJOINT_DESC))
	{
		assert(0 != m_pjntDistHang);
		if (0 != m_pjntDistHang)
			m_pjntDistHang->loadFromDesc(m_ToApplyDescDJ);
	}
}

void APhysXDynCCCoreData::OnApplySettingsAttribution()
{
	if (ReadFlag(TUA_DOMINANCE))
	{
		assert(0 != m_pDActor);
		if (0 != m_pDActor)
			m_pDActor->setDominanceGroup(m_ToApplyDominance);
	}
	if (ReadFlag(TUA_D6JOINT_ORIENTATION))
	{
		assert(0 != m_pjntD6Ctrl);
		if (0 != m_pjntD6Ctrl)
			m_pjntD6Ctrl->setDriveOrientation(m_ToApplyPose);
	}
	if (ReadFlag(TUA_FLAG_FROZEN_ROTATION))
	{
		assert(0 != m_pDActor);
		if (0 != m_pDActor)
		{
			if (m_ToApplyFrozenRot)
			{
				if (NX_BF_FROZEN_ROT_X & APhysXDynCCImp::gFrozenFlags) m_pDActor->raiseBodyFlag(NX_BF_FROZEN_ROT_X);
				if (NX_BF_FROZEN_ROT_Y & APhysXDynCCImp::gFrozenFlags) m_pDActor->raiseBodyFlag(NX_BF_FROZEN_ROT_Y);
				if (NX_BF_FROZEN_ROT_Z & APhysXDynCCImp::gFrozenFlags) m_pDActor->raiseBodyFlag(NX_BF_FROZEN_ROT_Z);
			}
			else
				m_pDActor->clearBodyFlag(NX_BF_FROZEN_ROT);
		}
	}
	if (ReadFlag(TUA_FLAG_KINEMATIC))
	{
		assert(0 != m_pDActor);
		if (0 != m_pDActor)
		{
			if (m_ToApplyKinamatic)
				m_pDActor->raiseBodyFlag(NX_BF_KINEMATIC);
			else
				m_pDActor->clearBodyFlag(NX_BF_KINEMATIC);
		}
	}
	if (ReadFlag(TUA_ACTOR_LINEAR_VELOCITY))
	{
		assert(0 != m_pDActor);
		if (0 != m_pDActor)
			m_pDActor->setLinearVelocity(m_ToApplyLinearVel);
	}
	if (ReadFlag(TUA_JOINT_LINEAR_VELOCITY))
	{
		assert(0 != m_pjntD6Ctrl);
		if (0 != m_pjntD6Ctrl)
			m_pjntD6Ctrl->setDriveLinearVelocity(m_ToApplyLinearVel);
	}
	if (ReadFlag(TUA_ACTOR_ANGULAR_VELOCITY))
	{
		assert(0 != m_pDActor);
		if (0 != m_pDActor)
			m_pDActor->setLinearVelocity(m_ToApplyAngularVel);
	}
	if (ReadFlag(TUA_ACTOR_GLOBAL_POSITION))
	{
		assert(0 != m_pDActor);
		if (0 != m_pDActor)
		{
			assert(false == m_pDActor->readBodyFlag(NX_BF_KINEMATIC));
			m_pDActor->setGlobalPosition(m_ToApplyPosition);
		}
	}
	if (ReadFlag(TUA_ACTOR_ADD_FORCE))
	{
		assert(0 != m_pDActor);
		if (0 != m_pDActor)
			m_pDActor->addForce(m_ToApplyForce);
	}
}

void SCStandNotBind::Configuration(APhysXDynCCCoreData& coreData)
{
	coreData.SetNewLinearVelOwner(false);
	if (!coreData.IsD6JointDescChanged())
	{
		if (EqualJudgement(GetSituation(), coreData.GetAppliedSituation()))
			return;
	}

	NxD6JointDesc& descD6 = coreData.UpdateD6JointDesc();
	descD6.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	descD6.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	descD6.twistMotion  = NX_D6JOINT_MOTION_LOCKED;
	descD6.xDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
	descD6.zDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
}

void SCStandAndBind::Configuration(APhysXDynCCCoreData& coreData)
{
	if (!coreData.IsD6JointDescChanged())
	{
		if (EqualJudgement(GetSituation(), coreData.GetAppliedSituation()))
			return;
	}

	NxD6JointDesc& descD6 = coreData.UpdateD6JointDesc();
	descD6.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	descD6.xMotion = NX_D6JOINT_MOTION_LOCKED;
	descD6.yMotion = NX_D6JOINT_MOTION_LOCKED;
	descD6.zMotion = NX_D6JOINT_MOTION_LOCKED;
}

void SCCtrlMoveNotBind::Configuration(APhysXDynCCCoreData& coreData)
{
	SCStandNotBind theSame;
	theSame.Configuration(coreData);
}

void SCCtrlMoveAndBind::Configuration(APhysXDynCCCoreData& coreData)
{
	coreData.SetNewLinearVelOwner(false);
	if (!coreData.IsD6JointDescChanged())
	{
		if (EqualJudgement(GetSituation(), coreData.GetAppliedSituation()))
			return;
	}

	NxD6JointDesc& descD6 = coreData.UpdateD6JointDesc();
	descD6.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	descD6.xDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
	descD6.yDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
	descD6.zDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
}

void SCSlideFree::Configuration(APhysXDynCCCoreData& coreData)
{
	Situation lastS = coreData.GetAppliedSituation();
	if (DYNCC_S_SLIDE_FREE != lastS)
		coreData.SetNewLinearVelOwner(true);

	SCJumpOrInair theSame;
	theSame.Configuration(coreData);
}

void SCSlideCtrl::Configuration(APhysXDynCCCoreData& coreData)
{
	coreData.SetNewLinearVelOwner(false);
	if (!coreData.IsD6JointDescChanged())
	{
		if (EqualJudgement(GetSituation(), coreData.GetAppliedSituation()))
			return;
	}
	
	NxD6JointDesc& descD6 = coreData.UpdateD6JointDesc();
	descD6.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	descD6.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	descD6.twistMotion  = NX_D6JOINT_MOTION_LOCKED;
	descD6.xDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
	descD6.yDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
	descD6.zDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
}

void SCJumpStart::Configuration(APhysXDynCCCoreData& coreData)
{
	coreData.SetNewLinearVelOwner(true);
	SCJumpOrInair theSame;
	theSame.Configuration(coreData);
}

void SCJumpOrInair::Configuration(APhysXDynCCCoreData& coreData)
{
	if (!coreData.IsD6JointDescChanged())
	{
		if (EqualJudgement(GetSituation(), coreData.GetAppliedSituation()))
			return;
	}

	NxD6JointDesc& descD6 = coreData.UpdateD6JointDesc();
	descD6.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	descD6.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	descD6.twistMotion  = NX_D6JOINT_MOTION_LOCKED;
}

void SCHangStart::Configuration(APhysXDynCCCoreData& coreData)
{
	coreData.UpdateOrientationForHang();
}

void SCHanging::Configuration(APhysXDynCCCoreData& coreData)
{
	coreData.UpdateOrientationForHang();
}

void SCHangEnd::Configuration(APhysXDynCCCoreData& coreData)
{
	if (!coreData.IsD6JointDescChanged())
	{
		if (EqualJudgement(GetSituation(), coreData.GetAppliedSituation()))
			return;
	}

	coreData.ResetIDOrientation();
	NxD6JointDesc& descD6 = coreData.UpdateD6JointDesc();
	descD6.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
	descD6.slerpDrive.damping = 100 * coreData.GetActorMass();
	descD6.flags |= NX_D6JOINT_SLERP_DRIVE;
}

void SCHangEndJumping::Configuration(APhysXDynCCCoreData& coreData)
{
	coreData.SetNewLinearVelOwner(true);
	SCHangEnd theSame;
	theSame.Configuration(coreData);
}

void SCHangPost::Configuration(APhysXDynCCCoreData& coreData)
{
}