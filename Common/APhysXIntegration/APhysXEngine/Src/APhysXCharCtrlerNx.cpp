#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerMgrImp.h"
#include "APhysXCharCtrlerImp.h"
#include "APhysXCharCtrlerNx.h"

#include "..\..\APhysXNxCharacter_Public\include\NxBoxController.h"
#include "..\..\APhysXNxCharacter_Public\include\NxCapsuleController.h"

APhysXNxCharCtrler::APhysXNxCharCtrler(NxController& controller, APhysXScene& scene, const APhysXCCDesc& desc)
	: APhysXHostCCBase(scene, desc) 
{
	m_flagsInner = 0;
	m_faceDir.set(1, 0, 0);

	NxScene& nxScene = GetNxScene();
	assert(&nxScene == &controller.getActor()->getScene());
	NxGroupsMask gm = scene.GetCollisionChannelManager()->GetChannelGroupMask(APX_COLLISION_CHANNEL_CC_SKIP_COLLISION);
	scene.LockWritingScene();
	controller.getActor()->getShapes()[0]->setGroupsMask(gm);
	scene.UnlockWritingScene();

	switch(controller.getType())
	{
	case NX_CONTROLLER_BOX:
		{
			NxBoxController* pBox = static_cast<NxBoxController*>(&controller);
			m_NxVolume = pBox->getExtents();
			break;
		}
	case NX_CONTROLLER_CAPSULE:
		{
			NxCapsuleController* pCapsule = static_cast<NxCapsuleController*>(&controller);
			m_NxVolume.x = pCapsule->getRadius();
			m_NxVolume.y = pCapsule->getHeight();
			break;
		}
	default:
		assert(!"Oops! Unknown Type!");
		break;
	}
	SetDetectionDistance(m_NxVolume.y);
}

void APhysXNxCharCtrler::SetDir(const NxVec3& vDir) 
{
	m_faceDir = vDir;
	if (NxMath::equals(m_faceDir.x, 0.0f, APHYSX_FLOAT_EPSILON)) m_faceDir.x = 0.0f;
	if (NxMath::equals(m_faceDir.y, 0.0f, APHYSX_FLOAT_EPSILON)) m_faceDir.y = 0.0f;
	if (NxMath::equals(m_faceDir.z, 0.0f, APHYSX_FLOAT_EPSILON)) m_faceDir.z = 0.0f;
	m_faceDir.normalize();
}

NxVec3 APhysXNxCharCtrler::GetDir() const
{
	return m_faceDir;
}

NxVec3 APhysXNxCharCtrler::GetUpDir() const
{
	return APhysXHostCCBase::GetUpDir();
}

NxVec3 APhysXNxCharCtrler::GetHorizonDir() const
{
	// must get current gravity due to this is an open interface to outside, and the g maybe changed runtime
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

APhysXNxCCImp::APhysXNxCCImp(const APhysXCCDesc& desc, const float skinwidth, NxController& controller, APhysXScene& scene)
	: APhysXNxCharCtrler(controller, scene, desc)
{
	m_pController = &controller;
	controller.setUserData(this);
	m_SkinWidth	= skinwidth;
	m_JumpSpeed = 0.0f;
	m_velCC.set(0.0f);
}

float APhysXNxCCImp::GetHalfHeight() const
{
	float halfHeight = 0;
	
	switch(m_pController->getType())
	{
	case NX_CONTROLLER_BOX:
		halfHeight = m_NxVolume.y;
		break;
	case NX_CONTROLLER_CAPSULE:
		halfHeight = m_NxVolume.y * 0.5f + m_NxVolume.x;  
		break;
	default:
		assert(!"Oops! Unknown Type!");
		break;
	}
	return halfHeight;
}

NxVec3 APhysXNxCCImp::GetFootPositionInternal(const bool actualNxPhysXPos) const
{
	NxVec3 rtn = GetCenterPositionInternal(actualNxPhysXPos);
	rtn.y = rtn.y - GetHalfHeight() - m_SkinWidth;
	return rtn;
}

NxVec3 APhysXNxCCImp::GetCenterPositionInternal(const bool actualNxPhysXPos) const
{
	assert(0 != m_pController);
	NxExtendedVec3 p;
	if (actualNxPhysXPos)
		p = m_pController->getFilteredPosition();
	else
		p = m_pController->getPosition();
	return NxVec3(float(p.x), float(p.y), float(p.z));
}

void APhysXNxCCImp::SetFootPosition(const NxVec3& pos)
{
	assert(0 != m_pController);
	NxExtendedVec3 p(pos.x, pos.y, pos.z);

	APhysXScene& scene = GetAPhysXScene();
	scene.LockWritingScene();
	m_pController->setPosition(p);
	scene.UnlockWritingScene();
}

void APhysXNxCCImp::SetStepOffset(const float offset)
{
	assert(0 != m_pController);
	
	float newVal = (0 < offset)? offset : -offset;
	m_pController->setStepOffset(newVal);
}

float APhysXNxCCImp::GetStepOffset() const
{
	assert(0 != m_pController);
	return m_pController->getStepOffset();
}

NxVec3 APhysXNxCCImp::GetVolume(APhysXVolumeType& outVT) const
{
	assert(0 != m_pController);
	outVT = APX_VT_UNKNOWN;

	NxVec3 rtn(m_NxVolume);
	if (NX_CONTROLLER_CAPSULE == m_pController->getType())
	{
		outVT = APX_VT_CAPSULE;
		rtn.y += rtn.x * 2;
	}
	else if (NX_CONTROLLER_BOX == m_pController->getType())
	{
		outVT = APX_VT_BOX;
	}
	return rtn;
}

bool APhysXNxCCImp::ChangeVolume(const NxVec3& newVolume)
{
	assert(0 != m_pController);
	
	bool rtn = false;
	switch(m_pController->getType())
	{
	case NX_CONTROLLER_BOX:
//		rtn = ChangeBoxVolume(newVolume);
		assert(true == false);
		break;
	case NX_CONTROLLER_CAPSULE:
//		rtn = ChangeCapsuleVolume(newVolume.x, newVolume.y);
		assert(true == false);
		break;
	default:
		assert(!"Oops! Unknown Type!");
	}
	return rtn;
}

bool APhysXNxCCImp::IsComponent(const NxActor& actor) const
{
	assert(0 != m_pController);
	
	NxActor* pActor = m_pController->getActor();
	if (0 != pActor)
	{
		if (&actor == pActor)
			return true;
	}
	return false;
}

NxActor* APhysXNxCCImp::GetHiddenActor() const
{
	if (0 == m_pController)
		return 0;
	return m_pController->getActor();
}

void APhysXNxCCImp::ReportSceneChanged()
{
	if (ReadFlag(NCCII_DONE_CALL_SCENE_CHANGED))
	{
		ClearFlag(NCCII_DONE_CALL_SCENE_CHANGED);
		return;
	}
	
	assert(0 != m_pController);
	m_pController->reportSceneChanged();
}

/*
bool APhysXNxCCImp::CheckOverlapCapsule(const NxVec3& centerPos, const float radius, const float height)
{
	NxCapsule worldCapsule;
	worldCapsule.p0 = worldCapsule.p1 = centerPos;
	worldCapsule.p0.y -= height * 0.5f;
	worldCapsule.p1.y += height * 0.5f;
	worldCapsule.radius = radius;
	
	bool IsOccupied = true;
	NxU32 nShapes = GetNxScene().overlapCapsuleShapes(worldCapsule, NX_DYNAMIC_SHAPES, 0, 0, 0, GetActiveGroups(), &GetFilterGMExcludeBody(), true);
	if (0 == nShapes)
		IsOccupied = false;

	return IsOccupied;
}*/

bool APhysXNxCCImp::GetGroundPos(float sweepDist, NxVec3& pos, NxVec3* poutNormal) const
{
	NxVec3 gDir;
	GetNxScene().getGravity(gDir);
	gDir.normalize();

	APhysXVolumeType vt;
	NxVec3 vol = GetVolume(vt);
	assert(APX_VT_CAPSULE == vt);

	NxVec3 start = pos;
	start += gDir * (-1 * vol.y * 0.5f);
	NxVec3 motion = gDir * sweepDist;

	NxSweepQueryHit sqHit;
	if (!SweepClosestAction(start, motion, sqHit))
		return false;

	pos = sqHit.point;
	if (0 != poutNormal)
		*poutNormal = sqHit.normal;
	return true;
}

void APhysXNxCCImp::JumpCC(float fJumpingSpeed, bool bIsAbsoluteSpeed)
{
	m_JumpSpeed = fJumpingSpeed;
	if (bIsAbsoluteSpeed)
		RaiseFlag(NCCII_ADDED_JVEL_ABSOLUTE);
}

void APhysXNxCCImp::PreUpdateVelocity(const NxVec3& g, const NxVec3& MoveDir, const float deltaSecond)
{
	const int status = GetStatus();
	if ((status & APX_CCS_JUMPING_INAIR) && !g.isZero())
	{
		m_velCC += g * deltaSecond;
	}
	else
	{
		m_velCC = MoveDir;
		m_velCC.normalize();
		m_velCC *= GetSpeed();
	}
	
	if (!NxMath::equals(m_JumpSpeed, 0.0f, APHYSX_FLOAT_EPSILON))
	{
		RaiseFlag(NCCII_INCREASED_JUMP_VELOCITY);
		NxVec3 up = -1 * g;
		up.normalize();
		if (ReadFlag(NCCII_ADDED_JVEL_ABSOLUTE))
		{
			ClearFlag(NCCII_ADDED_JVEL_ABSOLUTE);
			m_velCC -= GetVerticalSubValue(m_velCC, g);
		}
		m_velCC += up * m_JumpSpeed;
		m_JumpSpeed = 0.0f;
	}
}

NxU32 APhysXNxCCImp::MoveCC(const float deltaSecond, const NxVec3& MoveDir)
{
	if (GetCCMImp().IsStaticObjectsChanged())
	{
		m_pController->reportSceneChanged();
		RaiseFlag(NCCII_DONE_CALL_SCENE_CHANGED);
	}
	NxVec3 g;
	GetNxScene().getGravity(g);
	
	PreUpdateVelocity(g, MoveDir, deltaSecond);
	if (IsSleeping())
		return 0;

	NxVec3 s = m_velCC * deltaSecond + 0.5f * g * deltaSecond * deltaSecond;
	if (s.equals(gZero, APHYSX_FLOAT_EPSILON))
		return 0;

	NxU32 CFs = 0;
	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM();
	m_pController->move(s, GetActiveGroups(), 0.000001f, CFs, 1.0f, &gmFilter);

	int newStatus = APX_CCS_JUMPING_INAIR;
	PostUpdateVelocity(g, CFs, newStatus);
	UpdateState(newStatus);

	if (APX_CCFM_SYNCHRONOUS == GetCCManager().GetFlushMode())
		APhysXCharCtrlerMgrImp::GetCCM()->TickForSynPos();
	return CFs;
}

void APhysXNxCCImp::PostUpdateVelocity(const NxVec3& g, const NxU32 CollisionFlags, int& inoutNewStatus)
{
	bool zeroHV = false;
	bool zeroVV = false;
	
	if (NXCC_COLLISION_SIDES & CollisionFlags)
		zeroHV = true;
	
	if (NXCC_COLLISION_UP & CollisionFlags)
	{
		float cosRes = GetCosValue(m_velCC, g);
		if (0 > cosRes)
			zeroVV = true;
	}
	
	if (NXCC_COLLISION_DOWN & CollisionFlags)
	{
		inoutNewStatus |= APX_CCS_STANDON_SOMETHING;
		inoutNewStatus &= ~APX_CCS_JUMPING_INAIR;
	}
	
	const bool bIsInAir = (inoutNewStatus & APX_CCS_JUMPING_INAIR)? true : false;
	if (zeroHV && !bIsInAir)
		m_velCC = GetVerticalSubValue(m_velCC, g);
	
	if (zeroVV || !bIsInAir)
	{
		m_velCC -= GetVerticalSubValue(m_velCC, g);
		if (APHYSX_FLOAT_EPSILON > m_velCC.magnitudeSquared())
			m_velCC.zero();
	}
}

void APhysXNxCCImp::UpdateState(int& newStatus)
{
	if(newStatus & APX_CCS_JUMPING_INAIR)
	{
		if (!(m_Status & APX_CCS_JUMPING_INAIR))
		{
			if (ReadFlag(NCCII_INCREASED_JUMP_VELOCITY))
				newStatus |= APX_CCS_JUMPING_TAKEOFFE;
		}
	}
	else
	{
		if (m_Status & APX_CCS_JUMPING_INAIR)
		{
			if (!(m_Status & APX_CCS_JUMPING_LANDING))
				newStatus |= APX_CCS_JUMPING_LANDING;
		}
	}

	ClearFlag(NCCII_INCREASED_JUMP_VELOCITY);
	m_Status = newStatus;

	m_DistToGround = NX_MAX_F32;
	if ((m_Status & APX_CCS_JUMPING_LANDING) || (m_Status & APX_CCS_STANDON_SOMETHING))
		m_DistToGround = 0.0f;
}

bool APhysXNxCCImp::SweepClosestAction(const NxVec3& startPos, const NxVec3& motion, NxSweepQueryHit& outSQHit) const
{
	// at present, only do the capsule sweep no matter what type of controller.
	// for box controller, maybe it's better to do box sweep.
	float radius = 0.0f;
	assert(0 != m_pController);
	switch(m_pController->getType())
	{
	case NX_CONTROLLER_BOX:
		{
			NxBoxShape* pBox = m_pController->getActor()->getShapes()[0]->isBox();
			NxVec3 dim = pBox->getDimensions();
			radius = (dim.x < dim.z)? dim.x : dim.z; 
		}
		break;
	case NX_CONTROLLER_CAPSULE:
		{
			NxCapsuleShape* pCapsule = m_pController->getActor()->getShapes()[0]->isCapsule();
			radius = pCapsule->getRadius();
		}
		break;
	default:
		assert(!"Oops! Unknown Type!");
	}

	if (radius < m_NxVolume.x)
		radius = m_NxVolume.x;

	NxCapsule worldCapsule;
	worldCapsule.radius = radius;
	worldCapsule.p0 = worldCapsule.p1 = startPos;
	worldCapsule.p0.y += worldCapsule.radius;
	worldCapsule.p1.y += worldCapsule.radius;

	NxSweepQueryHit sqHit;
	NxU32 flags = NX_SF_STATICS | NX_SF_DYNAMICS;
	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM(true);
	NxU32 nHit = GetNxScene().linearCapsuleSweep(worldCapsule, motion, flags, 0, 1, &outSQHit, 0, GetActiveGroups(), &gmFilter);
	return (0 == nHit)? false : true;
}

