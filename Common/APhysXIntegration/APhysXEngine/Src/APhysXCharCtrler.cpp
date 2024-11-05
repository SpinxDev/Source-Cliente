
#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerMgrImp.h"
#include "APhysXCharCtrlerBase.h"
#include "APhysXCharCtrlerImp.h"
#include "APhysXCharCtrlerAssistance.h"

#include "..\..\APhysXNxCharacter_Public\include\NxBoxController.h"
#include "..\..\APhysXNxCharacter_Public\include\NxCapsuleController.h"

#if defined(_DEBUG)
	#define EXCLUDE_RAW_PHYSX_ACTORS   0
#else
	#define EXCLUDE_RAW_PHYSX_ACTORS   1
#endif

const NxVec3 APhysXCCBase::gZero = NxVec3(0.0f);

bool APhysXCCBase::CheckOverlap(APhysXScene& scene, const NxVec3& posFoot, const NxVec3& volume, const APhysXVolumeType type)
{
	if (APX_VT_UNKNOWN == type)
		return true;

	NxU32 ActiveGroup = 0xffffffff;
	ActiveGroup &= ~(1 << APX_COLLISION_GROUP_ATTACHER);
	ActiveGroup &= ~(1 << APX_COLLISION_GROUP_CLOTH_ATTACHER);
	ActiveGroup &= ~(1 << APX_COLLISION_GROUP_CC_HOST_RB);

	NxGroupsMask gmFilter;
	NxGroupsMask* pGMFilter = 0;
	APhysXCollisionChannelManager* pCCM = scene.GetCollisionChannelManager();
	assert(0 != pCCM);
	if (0 != pCCM)
	{
		gmFilter = pCCM->GetChannelGroupMask(APX_COLLISION_CHANNEL_CC_COMMON_HOST);
		NxGroupsMask gmCCSkip = pCCM->GetChannelGroupMask(APX_COLLISION_CHANNEL_CC_SKIP_COLLISION);
		gmFilter.bits0 |= gmCCSkip.bits0;
		gmFilter.bits1 |= gmCCSkip.bits1;
		gmFilter.bits2 |= gmCCSkip.bits2;
		gmFilter.bits3 |= gmCCSkip.bits3;
		NxGroupsMask gmDynCCHiden = pCCM->GetChannelGroupMask(APX_COLLISION_CHANNEL_DYNAMIC_CC);
		gmFilter.bits0 |= gmDynCCHiden.bits0;
		gmFilter.bits1 |= gmDynCCHiden.bits1;
		gmFilter.bits2 |= gmDynCCHiden.bits2;
		gmFilter.bits3 |= gmDynCCHiden.bits3;

		gmFilter.bits0 = ~gmFilter.bits0;
		gmFilter.bits1 = ~gmFilter.bits1;
		gmFilter.bits2 = ~gmFilter.bits2;
		gmFilter.bits3 = ~gmFilter.bits3;
		pGMFilter = &gmFilter;
	}

	bool rtn = true;
	if (APX_VT_BOX == type)
	{
		NxVec3 centerPos = posFoot;
		centerPos.y += volume.y;
		NxBounds3 worldBounds;
		worldBounds.setCenterExtents(centerPos, volume);
		rtn = scene.GetNxScene()->checkOverlapAABB(worldBounds, NX_ALL_SHAPES, ActiveGroup, pGMFilter);
	}
	else
	{
		float capHalfHeight = (volume.y - volume.x * 2) * 0.5f;
		if (0 > capHalfHeight)
			return true;

		NxVec3 centerPos = posFoot;
		centerPos.y += volume.y * 0.5f;
		NxCapsule worldCapsule;
		worldCapsule.p0 = worldCapsule.p1 = centerPos;
		worldCapsule.p0.y -= capHalfHeight;
		worldCapsule.p1.y += capHalfHeight;
		worldCapsule.radius = volume.x;
		rtn = scene.GetNxScene()->checkOverlapCapsule(worldCapsule, NX_ALL_SHAPES, ActiveGroup, pGMFilter);
	}
	return rtn;
}

APhysXCCBase::APhysXCCBase(APhysXScene& scene)
{
	userData = 0;
	m_bDisableCollision = false;
	m_IsSleeping = false;
	m_Status = APX_CCS_JUMPING_INAIR;
	m_NxVolume.set(-1);
	m_GroundNormal.set(0.0f);
	m_detDistance = NX_MAX_F32;
	m_DistToGround = NX_MAX_F32;
	m_pCCCM = 0;
	m_SleepingTerrainAABB.setEmpty();

	APhysXCharCtrlerMgrImp* pCCMImp = APhysXCharCtrlerMgrImp::GetCCM();
	assert(0 != pCCMImp);
	if (0 != pCCMImp)
	{
		m_pCCCM = new APhysXCCChannelMgr(*pCCMImp, scene);
		if (0 != m_pCCCM)
		{
			m_pCCCM->CloseGroup(APX_COLLISION_GROUP_ATTACHER);
			m_pCCCM->CloseGroup(APX_COLLISION_GROUP_CLOTH_ATTACHER);
			m_pCCCM->CloseGroup(APX_COLLISION_GROUP_CC_HOST_RB);
		}
	}
}

APhysXCCBase::~APhysXCCBase()
{
	delete m_pCCCM;
	m_pCCCM = 0;
}

void APhysXCCBase::OnObjectDeleted(APhysXObject& obj)
{
	APhysXCollisionChannel out;
	UnRegisterHostModel(&obj, out);
}

void APhysXCCBase::TerrainChange(bool IsLoading, const NxBounds3& TerrainAABB, const APhysXScene& Scene)
{
	if (&GetAPhysXScene() != &Scene)
		return;

	if (IsLoading)
	{
		if (IsSleeping() && !m_SleepingTerrainAABB.isEmpty())
		{
			if (m_SleepingTerrainAABB.min.equals(TerrainAABB.min, APHYSX_FLOAT_EPSILON) && 
				m_SleepingTerrainAABB.min.equals(TerrainAABB.min, APHYSX_FLOAT_EPSILON))
			{
				WakeUp();
				m_SleepingTerrainAABB.setEmpty();
			}
		}
	}
	else
	{
		if (!IsSleeping())
		{
			NxBounds3 ccAABB;
			GetAABB(ccAABB);
			if (ccAABB.intersects(TerrainAABB))
			{
				m_SleepingTerrainAABB = TerrainAABB;
				PutToSleep();
			}
		}
	}
}

APhysXCharCtrlerMgrImp& APhysXCCBase::GetCCMImp() const
{
	assert(0 != m_pCCCM);
	return m_pCCCM->GetCCManager();
}

APhysXCharacterControllerManager& APhysXCCBase::GetCCManager() const
{
	return GetCCMImp();
}

APhysXScene& APhysXCCBase::GetAPhysXScene() const
{
	assert(0 != m_pCCCM);
	return m_pCCCM->GetAPhysXScene();
}

NxScene& APhysXCCBase::GetNxScene() const
{
	NxScene* pScene = GetAPhysXScene().GetNxScene();
	assert(0 != pScene);
	return *pScene;
}

NxU32 APhysXCCBase::GetActiveGroups() const
{
	assert(0 != m_pCCCM);
	return m_pCCCM->GetActiveGroups();
}

void APhysXCCBase::OpenGroup(const APhysXCollisionGroup& group)
{
	assert(0 != m_pCCCM);
	m_pCCCM->OpenGroup(group);
}

void APhysXCCBase::CloseGroup(const APhysXCollisionGroup& group)
{
	assert(0 != m_pCCCM);
	m_pCCCM->CloseGroup(group);
}

NxGroupsMask APhysXCCBase::GetFilterGM(bool bEnforceCombineCCMRegGM) const
{
	assert(0 != m_pCCCM);
	return m_pCCCM->GetFilterGM(bEnforceCombineCCMRegGM);
}

bool APhysXCCBase::RegisterHostModel(APhysXObject* pObject, APhysXCollisionChannel& outNewCNL)
{
	return m_pCCCM->RegisterHostModel(pObject, outNewCNL);
}

bool APhysXCCBase::UnRegisterHostModel(APhysXObject* pObject, APhysXCollisionChannel& outOriginalCNL)
{
	return m_pCCCM->UnRegisterHostModel(pObject, outOriginalCNL);
}

void APhysXCCBase::GetAABB(NxBounds3& aabb) const
{
	aabb.setEmpty();
	APhysXVolumeType vType;
	NxVec3 vol = GetVolume(vType);

	if (APX_VT_UNKNOWN == vType)
		return;

	if (APX_VT_CAPSULE == vType)
	{
		vol.z = vol.x;
		vol.y *= 0.5f;
	}

	NxVec3 center = GetCenterPosition();
	aabb.setCenterExtents(center, vol);
}

NxMat33 APhysXCCBase::GetIDGlobalOrientation() const
{
	static NxMat33 idMat;
	idMat.id();
	return idMat;
}

bool APhysXCCBase::SetDetectionDistance(float distance)
{
	if (0 < distance)
	{
		m_detDistance = distance;
		return true;
	}
	return false;
}

APhysXHostCCBase::APhysXHostCCBase(APhysXScene& scene, const APhysXCCDesc& desc)
	: APhysXCCBase(scene) 
{
	m_JumpingFactor = -1;
	SetMass(desc.mass);
	SetSpeed(desc.speed);
}

NxVec3 APhysXHostCCBase::GetUpDir() const
{
	static NxVec3 up(0, 1, 0);
	return up;
}

NxVec3 APhysXHostCCBase::GetGroundNormal(bool* outpbIsDefVal, const NxVec3& defVal) const
{
	if (0 != outpbIsDefVal)
		*outpbIsDefVal = false;

	int nStatus = GetStatus();
	if ((APX_CCS_STANDON_SOMETHING & nStatus) || (APX_CCS_JUMPING_LANDING & nStatus))
	{
		if (!m_GroundNormal.isZero())
			return m_GroundNormal;
	}

	if (0 != outpbIsDefVal)
		*outpbIsDefVal = true;
	return defVal;
}

APhysXCharacterController::APhysXCharacterController(NxController& controller, APhysXScene& scene, const APhysXCCDesc& desc)
	: APhysXHostCCBase(scene, desc)
{
	NxScene& nxScene = GetNxScene();
	assert(&nxScene == &controller.getActor()->getScene());

	m_PushFactor = 10;
	m_posFCThreshold = 0.2f;

	m_flags = desc.flags;
	ClearFlag(CCII_COLLISION_INFO_VALID);
	ClearFlag(CCII_ENFORCE_CLOSE_ATTACHMENT);
	ClearFlag(CCII_ENFORCE_CLOSE_FORCECOMPUTE);
	ClearFlag(CCII_OVERLAP_STATE);

	NxVec3 g;
	nxScene.getGravity(g);
	SetUserGravity(g);

	//-------------------------------------------------------------
	// note by Wenfeng, Dec. 16, 2009
	// we set the hidden kinematic actor's group mask according to 
	// the collision channel APX_COLLISION_CHANNEL_CC_SKIP_COLLISION,
	// so that Move()/Sweep() function will ignore the actor, moreover,
	// it can still collide with objects in the channel CC_SKIP_COLLISION.
	NxGroupsMask gm = scene.GetCollisionChannelManager()->GetChannelGroupMask(APX_COLLISION_CHANNEL_CC_SKIP_COLLISION);
	scene.LockWritingScene();
	controller.getActor()->getShapes()[0]->setGroupsMask(gm);
	scene.UnlockWritingScene();

	m_SlideDir.zero();
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

bool APhysXCharacterController::ReadFlag(const APhysXCCFlag flag) const
{
	return (m_flags & flag)? true : false;
}

void APhysXCharacterController::ClearFlag(const APhysXCCFlag flag)
{
	m_flags &= ~flag;
	if (APX_CCF_ATTACHMENT_ONLY_VERTICAL == flag)
		ClearFlag(CCII_NEW_ATTACHMENT_VERTICAL);
}

void APhysXCharacterController::RaiseFlag(const APhysXCCFlag flag)
{
	if (APX_CCF_ATTACHMENT_ONLY_VERTICAL == flag)
	{
		if (!ReadFlag(APX_CCF_ATTACHMENT_ONLY_VERTICAL)) 
			RaiseFlag(CCII_NEW_ATTACHMENT_VERTICAL);
	}
	m_flags |= flag;
}

NxVec3 APhysXCharacterController::GetGravity() const
{
	if (!ReadFlag(APX_CCF_GRAVITY_ENABLE))
		return NxVec3(0.0f);
	
	if (ReadFlag(APX_CCF_GRAVITY_READSCENE))
	{
		NxVec3 rtn(0.0f);
		GetNxScene().getGravity(rtn);
		return rtn;
	}
	return m_UserGravity;
}

void APhysXCharacterController::EnforceClearHorizonVelocity(bool bIsAbsoluteVel)
{
	if (bIsAbsoluteVel)
		RaiseFlag(CCII_CLEAR_ABSOLUTE_HORIZON_VEL);
	else
		RaiseFlag(CCII_CLEAR_RELATIVE_HORIZON_VEL);
}

bool APhysXCharacterController::CheckCCFlagForAttachment(const NxActor& actor, bool* poutIsEnforce)
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

bool APhysXCharacterController::CheckCombinedFlagEnable(const NxActor& actor, const APhysXCCFlag flag) const
{
	APhysXObjectFlag objFlag;
	bool success = Translation(flag, objFlag);
	assert(true == success);
	if (!success)
		return false;

	bool rtn = ReadFlag(flag);
	if (!rtn)
		return false;

	rtn = rtn && CheckFlagEnable(actor, objFlag);
	return rtn;
}

bool APhysXCharacterController::CheckFlagEnable(const NxActor& actor, const APhysXObjectFlag flag) const
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

bool APhysXCharacterController::Translation(const APhysXCCFlag ccFlag, APhysXObjectFlag& outObjFlag) const
{
	switch (ccFlag)
	{
	case APX_CCF_ADD_PUSHFORCE:
		outObjFlag = APX_OBJFLAG_CC_ADD_PUSHFORCE;
		break;
	case APX_CCF_ADD_STANDFORCE:
		outObjFlag = APX_OBJFLAG_CC_ADD_STANDFORCE;
		break;
	default:
		return false;
	}

	return true;
}

APhysXCCImp::APhysXCCImp(const APhysXCCDesc& desc, const float skinwidth, NxController& controller, APhysXScene& scene)
 : APhysXCharacterController(controller, scene, desc)
{
	m_pLastFrame = new FrameInfo;
	m_pCurrentFrame = new FrameInfo;
	m_pActiveForCallBack = m_pLastFrame;
	m_pLastForCallBack = 0;
	
	m_pController = &controller;
	controller.setUserData(this);
	
	m_SlopeUpRadian   = desc.GetSlopeUpRadian();
	m_SlopeDownRadian = desc.GetSlopeDownRadian();
	m_SkinWidth	= skinwidth;
	
	m_JumpSpeed = 0.0f;
	m_faceDir.set(1, 0, 0);
	m_faceDirNew = m_faceDir;

	m_newVolume.zero();
	m_newPos.zero();

	m_LastPos = desc.posFoot;
}

APhysXCCImp::~APhysXCCImp()
{
	delete m_pLastFrame;
	delete m_pCurrentFrame;
}

void APhysXCCImp::SetBlockShape(NxShape& shape)
{
	if (ReadFlag(CCII_MOVE_FOR_REVISE))
		m_pBlocks.push_back(&shape);
}

float APhysXCCImp::GetHalfHeight(const bool actualNxPhysXSize) const
{
	float halfHeight = 0;
	const NxVec3* pVol = &m_NxVolume;

	if (!actualNxPhysXSize)
	{
 		if (ReadFlag(CCII_HAS_NEW_VOLUME) || ReadFlag(CCII_HAS_NEW_POSITION))
			pVol = &m_newVolume;
	}

	switch(m_pController->getType())
	{
	case NX_CONTROLLER_BOX:
		halfHeight = pVol->y;
		break;
	case NX_CONTROLLER_CAPSULE:
		halfHeight = pVol->y * 0.5f + pVol->x;  
		break;
	default:
		assert(!"Oops! Unknown Type!");
		break;
	}
	return halfHeight;
}

NxVec3 APhysXCCImp::GetFootPositionInternal(const bool actualNxPhysXPos) const
{
	NxVec3 rtn = GetCenterPositionInternal(actualNxPhysXPos);
	rtn.y = rtn.y - GetHalfHeight(actualNxPhysXPos) - m_SkinWidth;
	return rtn;
}

NxVec3 APhysXCCImp::GetCenterPositionInternal(const bool actualNxPhysXPos) const
{
	NxExtendedVec3 p = m_newPos;
	if (ReadFlag(CCII_HAS_NEW_VOLUME) || ReadFlag(CCII_HAS_NEW_POSITION))
	{
		if (!actualNxPhysXPos)
			return NxVec3(float(p.x), float(p.y), float(p.z));
	}
		
	assert(0 != m_pController);
	if (actualNxPhysXPos)
		p = m_pController->getFilteredPosition();
	else
		p = m_pController->getPosition();
	return NxVec3(float(p.x), float(p.y), float(p.z));
}

void APhysXCCImp::SetFootPosition(const NxVec3& pos)
{
	m_newPos.set(pos.x, pos.y, pos.z);
	m_newPos.y += m_SkinWidth; 
	m_newPos.y += GetHalfHeight(); 
	RaiseFlag(CCII_HAS_NEW_POSITION);
}

NxVec3 APhysXCCImp::GetDir() const
{
	if (ReadFlag(CCII_HAS_NEW_DIR))
		return m_faceDirNew;

	return m_faceDir;
}

void APhysXCCImp::SetDir(const NxVec3& vDir) 
{
	m_faceDirNew = vDir;
	if (NxMath::equals(m_faceDirNew.x, 0.0f, APHYSX_FLOAT_EPSILON)) m_faceDirNew.x = 0.0f;
	if (NxMath::equals(m_faceDirNew.y, 0.0f, APHYSX_FLOAT_EPSILON)) m_faceDirNew.y = 0.0f;
	if (NxMath::equals(m_faceDirNew.z, 0.0f, APHYSX_FLOAT_EPSILON)) m_faceDirNew.z = 0.0f;
	m_faceDirNew.normalize();
	RaiseFlag(CCII_HAS_NEW_DIR);
}

NxVec3 APhysXCCImp::GetUpDir() const
{
	return APhysXHostCCBase::GetUpDir();
}

NxVec3 APhysXCCImp::GetHorizonDir() const
{
	NxVec3 currentDir = GetDir();
	NxVec3 vDir = GetVerticalSubValue(currentDir, m_pCurrentFrame->GetGravity());
	NxVec3 hDir = currentDir - vDir; 
	if (NxMath::equals(hDir.x, 0.0f, APHYSX_FLOAT_EPSILON)) hDir.x = 0.0f;
	if (NxMath::equals(hDir.y, 0.0f, APHYSX_FLOAT_EPSILON)) hDir.y = 0.0f;
	if (NxMath::equals(hDir.z, 0.0f, APHYSX_FLOAT_EPSILON)) hDir.z = 0.0f;
	hDir.normalize();
	return hDir;
}

void APhysXCCImp::UpdateDir()
{
	NxVec3 newDir;
	if (m_pLastFrame->GetAttachedDir(newDir))
	{
		if (!ReadFlag(CCII_HAS_NEW_DIR))
		{
			m_faceDir = newDir;
		}
		else
		{
			float cosVal = GetCosValue(m_faceDir, newDir);
			if (0.98f < cosVal)
			{
				// almost the same direction
				m_faceDir = m_faceDirNew;
			}
			else
			{
				float theta = NxMath::acos(cosVal);
				NxVec3 axis;
				axis.cross(m_faceDir, newDir);
				axis.normalize();
				NxQuat quat;
				quat.fromAngleAxisFast(theta, axis);
				m_faceDir = quat.rot(m_faceDirNew);  // for quadruped(4-footed animal), it's ok to finish the job.

				// for human walking(2-footed, always keep absolute-up in world), it is necessary to get horizontal dir
				NxVec3 vDir = GetVerticalSubValue(m_faceDir, NxVec3(0, -1, 0));
				m_faceDir -= vDir; 

			}
			ClearFlag(CCII_HAS_NEW_DIR);
		}
	}
	else
	{
		if (ReadFlag(CCII_HAS_NEW_DIR))
		{
			ClearFlag(CCII_HAS_NEW_DIR);
			m_faceDir = m_faceDirNew;
		}
	}
	m_faceDir.normalize();
}

void APhysXCCImp::JumpCC(float fJumpingSpeed, bool bIsAbsoluteSpeed)
{
	m_JumpSpeed = fJumpingSpeed;
	if (bIsAbsoluteSpeed)
		RaiseFlag(CCII_SET_ABSOLUTE_JUMPING_VEL);
}

void APhysXCCImp::SetStepOffset(const float offset)
{
	assert(0 != m_pController);

	float newVal = (0 < offset)? offset : -offset;
	m_pController->setStepOffset(newVal);
}

float APhysXCCImp::GetStepOffset() const
{
	assert(0 != m_pController);
	return m_pController->getStepOffset();
}

bool APhysXCCImp::GetAttachmentPos(NxVec3& out) const
{
	assert(0 != m_pCurrentFrame);
	return m_pCurrentFrame->GetAttachedPoint(out);
}

NxVec3 APhysXCCImp::GetVolume(APhysXVolumeType& outVT) const
{
	outVT = APX_VT_UNKNOWN;

	NxVec3 rtn(m_NxVolume);
	if (ReadFlag(CCII_HAS_NEW_VOLUME) || ReadFlag(CCII_HAS_NEW_POSITION))
		rtn = m_newVolume;
	
	assert(0 != m_pController);
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

bool APhysXCCImp::ChangeVolume(const NxVec3& newVolume)
{
	assert(0 != m_pController);

	bool rtn = false;
	switch(m_pController->getType())
	{
	case NX_CONTROLLER_BOX:
		rtn = ChangeBoxVolume(newVolume);
		break;
	case NX_CONTROLLER_CAPSULE:
		rtn = ChangeCapsuleVolume(newVolume.x, newVolume.y);
		break;
	default:
		assert(!"Oops! Unknown Type!");
	}
	return rtn;
}

bool APhysXCCImp::ChangeBoxVolume(const NxVec3& dim)
{
	if (0 > dim.x) return false;
	if (0 > dim.y) return false;
	if (0 > dim.z) return false;

	if (m_NxVolume.equals(dim, APHYSX_FLOAT_EPSILON))
		return true;

	float HalfIns = dim.y - m_NxVolume.y;
	NxVec3 tmpPos = GetCenterPositionInternal(true);
	tmpPos.y += HalfIns;
	if (CheckOverlapBox(tmpPos, dim))
		return false;

	m_newPos.set(tmpPos.x, tmpPos.y, tmpPos.z);
	m_newVolume = dim;
	RaiseFlag(CCII_HAS_NEW_VOLUME);
	return true;
}

bool APhysXCCImp::ChangeCapsuleVolume(const float radius, const float height)
{
	float capHeight = height - radius * 2;
	if (0 > radius) return false;
	if (0 > capHeight) return false;

	NxVec3 newVol(radius, capHeight, -1);
	if (m_NxVolume.equals(newVol, APHYSX_FLOAT_EPSILON))
		return true;

	float HalfIns = (capHeight - m_NxVolume.y) * 0.5f + (radius - m_NxVolume.x);
	NxVec3 tmpPos = GetCenterPositionInternal(true);
	tmpPos.y += HalfIns;
	if (CheckOverlapCapsule(tmpPos, radius, capHeight))
		return false;

	m_newPos.set(tmpPos.x, tmpPos.y, tmpPos.z);
	m_newVolume = newVol;
	RaiseFlag(CCII_HAS_NEW_VOLUME);
	return true;
}

void APhysXCCImp::ApplyNewSetting()
{
	assert(0 != m_pController);
	APhysXScene& scene = GetAPhysXScene();

	if (ReadFlag(CCII_HAS_NEW_VOLUME))
	{
		switch(m_pController->getType())
		{
		case NX_CONTROLLER_BOX:
			{
				NxBoxController* pBox = static_cast<NxBoxController*>(m_pController);
				scene.LockWritingScene();
				pBox->setPosition(m_newPos);
				pBox->setExtents(m_newVolume);
				scene.UnlockWritingScene();
			}
			break;
		case NX_CONTROLLER_CAPSULE:
			{
				NxCapsuleController* pCapsule = static_cast<NxCapsuleController*>(m_pController);
				scene.LockWritingScene();
				pCapsule->setPosition(m_newPos);
				pCapsule->setRadius(m_newVolume.x);
				pCapsule->setHeight(m_newVolume.y);
				scene.UnlockWritingScene();
			}
			break;
		default:
			assert(!"Oops! Unknown Type!");
		}
		m_NxVolume = m_newVolume;
		ClearFlag(CCII_HAS_NEW_VOLUME);
	}
}

bool APhysXCCImp::ProcessOverlapInAir(bool& outIsOverlap)
{
	assert(true == m_pLastFrame->GetStatus(FrameInfo::IS_IN_AIR));

	outIsOverlap = false;
	const AttachedActor* pCandidate = m_pLastFrame->GetAttachmentCandidate();
	if (0 == pCandidate)
		return false;
	if (!pCandidate->IsValid())
		return false;

	m_ToFilterOutShapes.Clear();
	NxVec3 reDisp = GetDispReviseForOLInAir();
	NxU32 CFs = DoReviseWork(reDisp);
	m_pLastFrame->UpdateStateByFlags(CFs);
	UpdateAttachCandidate(*m_pLastFrame);
	outIsOverlap = CheckOverlap(false);
	return true;
}

bool APhysXCCImp::CheckOverlap(bool EnableProcessOLInAir)
{
	assert(true == m_ToFilterOutShapes.IsEmpty());
	bool IsOverlap = false;
	NxVec3 cp = GetCenterPositionInternal(true);

	assert(0 != m_pController);
	switch(m_pController->getType())
	{
	case NX_CONTROLLER_BOX:
		IsOverlap = CheckOverlapBox(cp, m_NxVolume, true);
		break;
	case NX_CONTROLLER_CAPSULE:
		IsOverlap = CheckOverlapCapsule(cp, m_NxVolume.x, m_NxVolume.y, true);
		break;
	default:
		assert(!"Oops! Unknown Type!");
	}

	bool IsAttachEnable = false;
	bool IsOverlapCandidate = false;
	const AttachedActor* pCandidate = m_pLastFrame->GetAttachmentCandidate();
	if (0 != pCandidate && pCandidate->IsValid())
	{
		IsAttachEnable = CheckCCFlagForAttachment(*(pCandidate->pActor));
		IsOverlapCandidate = m_ToFilterOutShapes.IsInclude(*pCandidate);
	}

	if (IsOverlapCandidate)
	{
		if (IsAttachEnable)
		{
			if (EnableProcessOLInAir && m_pLastFrame->GetStatus(FrameInfo::IS_IN_AIR))
			{
				bool bRtn = false;
				if (ProcessOverlapInAir(bRtn))
					IsOverlap = bRtn;
			}
		}
		else
		{
			if (m_bDisableCollision)
			{
				// obsolete codes. The collision was controlled by dominance group
				/*
				if (!ReadFlag(CCII_COLLISION_INFO_VALID))
				{
					RaiseFlag(CCII_COLLISION_INFO_VALID);
					if (m_pController->getCollision())
						RaiseFlag(CCII_COLLISION_INFO_VALUE);
					else
						ClearFlag(CCII_COLLISION_INFO_VALUE);
					GetAPhysXScene().LockWritingScene();
					m_pController->getActor()->wakeUp();
					m_pController->setCollision(false);
					GetAPhysXScene().UnlockWritingScene();
				}
				*/
			}
		}
	}

	if (!IsOverlap)
	{
		if (m_bDisableCollision)
		{
			// obsolete codes. The collision was controlled by dominance group
			/*
			if (ReadFlag(CCII_COLLISION_INFO_VALID))
			{
				ClearFlag(CCII_COLLISION_INFO_VALID);
				GetAPhysXScene().LockWritingScene();
				m_pController->getActor()->wakeUp();
				m_pController->setCollision(ReadFlag(CCII_COLLISION_INFO_VALUE));
				GetAPhysXScene().UnlockWritingScene();
			}
			*/
		}
	}

	APhysXCCHitReport* pUserReport = APhysXCCHitReport::GetPtrInstance();
	if (0 < pUserReport->GetRegisteredCount())
	{
		const bool LastOverlapState = ReadFlag(CCII_OVERLAP_STATE);
		if (IsOverlap)
		{
			APhysXCCOverlap APhysXOL;
			APhysXOL.controller = this;
			APhysXArray<APhysXObject*> Objects;
			APhysXOL.nbObjects = m_ToFilterOutShapes.GetObjects(Objects);
			APhysXOL.objects = (0 == APhysXOL.nbObjects)? 0 : &(Objects[0]);
			APhysXArray<NxActor*> NxActors;
			APhysXOL.nbNxActors = m_ToFilterOutShapes.GetNxActors(NxActors);
			APhysXOL.nxactors = (0 == APhysXOL.nbNxActors)? 0 : &(NxActors[0]);
			if (LastOverlapState)
				pUserReport->onOverlapContinue(APhysXOL);
			else
			{
				pUserReport->onOverlapBegin(APhysXOL);
				RaiseFlag(CCII_OVERLAP_STATE);
			}
		}
		else
		{
			if (LastOverlapState)
			{
				pUserReport->onOverlapEnd(*this);
				ClearFlag(CCII_OVERLAP_STATE);
			}	
		}
	}
	return IsOverlap;
}

bool APhysXCCImp::CheckOverlapBox(const NxVec3& centerPos, const NxVec3& dimExtents, bool CollectShapes)
{
	NxBounds3 worldBounds;
	worldBounds.setCenterExtents(centerPos, dimExtents);
	
	bool IsOccupied = false;
	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM();
	if (CollectShapes)
	{
		NxU32 nShapes = GetNxScene().overlapAABBShapes(worldBounds, NX_DYNAMIC_SHAPES, 0, 0, &m_ToFilterOutShapes, GetActiveGroups(), &gmFilter, true); 
		IsOccupied = (0 == nShapes)? false : true;
	}
	else
	{
		IsOccupied = GetNxScene().checkOverlapAABB(worldBounds, NX_ALL_SHAPES, GetActiveGroups(), &gmFilter);
	}
	return IsOccupied;
}

bool APhysXCCImp::CheckOverlapCapsule(const NxVec3& centerPos, const float radius, const float height, bool CollectShapes)
{
	NxCapsule worldCapsule;
	worldCapsule.p0 = worldCapsule.p1 = centerPos;
	worldCapsule.p0.y -= height * 0.5f;
	worldCapsule.p1.y += height * 0.5f;
	worldCapsule.radius = radius;
	
	bool IsOccupied = false;
	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM();
	if (CollectShapes)
	{
		NxU32 nShapes = GetNxScene().overlapCapsuleShapes(worldCapsule, NX_DYNAMIC_SHAPES, 0, 0, &m_ToFilterOutShapes, GetActiveGroups(), &gmFilter, true);
		IsOccupied = (0 == nShapes)? false : true;
	}
	else
	{
		IsOccupied = GetNxScene().checkOverlapCapsule(worldCapsule, NX_ALL_SHAPES, GetActiveGroups(), &gmFilter);
	}
	return IsOccupied;
}

bool APhysXCCImp::GetGroundPos(float sweepDist, NxVec3& pos, NxVec3* poutNormal) const
{
	NxVec3 gDir = GetGravity();
	gDir.normalize();

	APhysXVolumeType vt;
	NxVec3 vol = GetVolume(vt);
	assert(APX_VT_CAPSULE == vt);
	float R = vol.x;

	NxVec3 start = pos;
	start += gDir * (-1 * vol.y * 0.5f);
	NxVec3 motion = gDir * sweepDist;

	NxSweepQueryHit sqHit;
	if (!SweepClosestAction(start, motion, sqHit, &R))
		return false;

	pos = sqHit.point;
	if (0 != poutNormal)
		*poutNormal = sqHit.normal;
	return true;
}

NxVec3 APhysXCCImp::GetRayStartPos(float& outOffsetBaseFootPos) const
{
	// don't use the m_pController->getActor()->getGlobalPosition(); It's possible different from CC center. 
	// and we need to get the latest position which doesn't matter "updateControllers".
	NxVec3 rtn = GetFootPositionInternal(true);

	outOffsetBaseFootPos = m_SkinWidth * 1.5f;
	rtn.y += outOffsetBaseFootPos;
	return rtn;
}

NxVec3 APhysXCCImp::GetDispForMove()
{
	// implement fixed binding mode (there is no relative displacement even the acceleration is toward down and over than G)
	NxVec3 s = GetDispFromFormula(m_pCurrentFrame->GetVel());

	const AttachedActor* pCandidate = m_pLastFrame->GetAttachmentCandidate();
	if (0 != pCandidate)
	{
		if (m_ToFilterOutShapes.IsInclude(*pCandidate))
			return s;
	}

	if (m_pLastFrame->GetStatus(FrameInfo::IS_IN_AIR))
	{
		// count in the displacement of candidate actor, to avoid penetration in next frame  
		if (0 != pCandidate)
		{
			NxVec3 G = m_pCurrentFrame->GetGravity();
			NxVec3 vvPT = GetVerticalSubValue(pCandidate->velPT, G);
			m_pCurrentFrame->DispFMoveCandidate = vvPT * m_pCurrentFrame->GetTime();
			s -= m_pCurrentFrame->DispFMoveCandidate;
		}
	}
	else
	{
		NxVec3 G = m_pCurrentFrame->GetGravity();
		float cosRes = GetCosValue(s, G);
		if (0 < cosRes)
		{
			// in down move situation: increase down moving displacement to make sure generate down flag
			if (ReadFlag(CCII_IS_SLIDING_MOVE))
			{
				NxVec3 gDir = G;
				gDir.normalize();
				s += gDir * 10;
			}
			else if (!ReviseVerticalDispForWalking(s))
			{
				if (0 != pCandidate)
				{
					NxVec3 vvPT = GetVerticalSubValue(pCandidate->velPT, G);
					float cosRes = GetCosValue(vvPT, G);
					if (0 < cosRes)
						vvPT *= -1;
					s -= vvPT * m_pCurrentFrame->GetTime();
				}
			}
		}
	}

	return s;
}

NxVec3 APhysXCCImp::GetDispFromFormula(const NxVec3& v0) const
{
	const float t = m_pCurrentFrame->GetTime();
	// s = v0t + 0.5att
	return v0 * t + 0.5f * m_pCurrentFrame->GetGravity() * t * t;
}

NxVec3 APhysXCCImp::GetDispReviseForForecase()
{
	NxVec3 RealDisp(0.0f);
	if (m_pLastFrame->GetStatus(FrameInfo::IS_IN_AIR))
	{
		// There are 4 reasons leading to "in air" status of last frame:
		// 1. CC's attachment just deleted in last simulation.
		// 2. CC is in free falling motion. 
		// 3. CC is in "jumping take off".
		// 4. CC is walking out of cliff.

		// handle case 1 
		if (m_pLastFrame->GetStatus(FrameInfo::ATTACHMENT_IS_DELETED))
		{
			RealDisp = m_pLastFrame->DispFMove - m_pLastFrame->DispFMoveUnfinished;
			RealDisp *= -1;
			return RealDisp;
		}
		// for case 2, 3, 4: just return unfinished DispFMoveUnfinished
	}
	else
	{
		const AttachedActor* pBinding = m_pLastFrame->GetAttachmentBinding();
		if (0 != pBinding)
		{
			NxVec3 newGlobalPos;
			pBinding->pActor->getGlobalPose().multiply(pBinding->localPos, newGlobalPos);
			RealDisp = newGlobalPos - pBinding->globalPos;
			RealDisp -= m_pLastFrame->DispFMove;
			if (ReadFlag(CCII_CLEAR_HORIZON_REVISE))
				RealDisp = GetVerticalSubValue(RealDisp, m_pLastFrame->GetGravity());
		}
	}

	return RealDisp + m_pLastFrame->DispFMoveUnfinished;
}

NxVec3 APhysXCCImp::GetDispReviseForOLInAir()
{
	// only handle the case of:
	// CC is in air, and CC overlap with candidate actor
	// So, we just look on as CC attach to candidate.
	const AttachedActor* pCandidate = m_pLastFrame->GetAttachmentCandidate();
	assert(0 != pCandidate);
	assert(true == pCandidate->IsValid());

	if (!ReadFlag(CCII_CLEAR_HORIZON_REVISE))
	{
		NxVec3 nowPos;
		pCandidate->pActor->getGlobalPose().multiply(pCandidate->localPos, nowPos);
		return nowPos - GetFootPositionInternal(true);
	}

	NxVec3 dRes(0.0f);
	float halfH = GetHalfHeight(); 
	NxVec3 motion(0, -1 * halfH * 2, 0);
	NxVec3 footPos = GetFootPositionInternal(true);

	NxCapsule cap;
	cap.radius = m_NxVolume.x;
	cap.p0 = cap.p1 = footPos;
	cap.p0.y += halfH * 2 + cap.radius;
	cap.p1.y += halfH * 2 + cap.radius;

	NxSweepQueryHit sqHit;
	NxU32 flags = NX_SF_STATICS | NX_SF_DYNAMICS;
	if (1 > APhysXCharacterControllerManager::ClosestSweepExcludesAllCCs(GetAPhysXScene(), cap, motion, flags, sqHit))
		return dRes;

	NxVec3 vNewCenter = cap.p0 + motion * sqHit.t;
	NxVec3 vLowCenter = footPos;
	footPos.y += m_NxVolume.x;
	dRes = vNewCenter - vLowCenter;
	return dRes;
}

bool APhysXCCImp::ReviseVerticalDispForWalking(NxVec3& S) const
{
	// re-edit vertical disp. by the slope down threshold in the case of walking(without jumping or free falling)
	NxVec3 G = m_pCurrentFrame->GetGravity();
	NxVec3 vs = GetVerticalSubValue(S, G);
	NxVec3 hs = S - vs;

	if (hs.isZero())
		return false;

	float cosRes = GetCosValue(vs, G);
	if (!NxMath::equals(cosRes, 1.0f, APHYSX_FLOAT_EPSILON))
		return false;

	float Offset = 0;
	NxSweepQueryHit sqHit;
	NxVec3 startpos = GetRayStartPos(Offset) + hs;

	static const float tanSDR = (float)tan(m_SlopeDownRadian);
	const float FreeFallThreshold = tanSDR * hs.magnitude() + Offset;
	NxVec3 motion = G;
	motion.normalize();
	motion *= FreeFallThreshold;

	if (SweepClosestAction(startpos, motion, sqHit))
	{
		float t = sqHit.t * FreeFallThreshold;
		if (t < 0.01f)
			sqHit.t = 0.01f / FreeFallThreshold;
		S += sqHit.t * motion;
	}
	return true;
}

NxU32 APhysXCCImp::MoveController(const NxVec3& disp, FrameInfo* pToFilterIt)
{
	assert(0 != m_pController);
	if (disp.equals(gZero, APHYSX_FLOAT_EPSILON))
		return 0;

	NxShape* pShape = 0;
	NxGroupsMask gmCtrl, gmActor;

	gmCtrl = m_pController->getActor()->getShapes()[0]->getGroupsMask();
	m_ToFilterOutShapes.ChangeAttributes(gmCtrl);
	if (0 != pToFilterIt)
	{
		const AttachedActor* pBinding = (0 == pToFilterIt)? 0 : pToFilterIt->GetAttachmentBinding();
		if (0 != pBinding)
		{
			pShape = pBinding->pActor->getShapes()[0];
			gmActor = pShape->getGroupsMask();
			GetAPhysXScene().LockWritingScene();
			pShape->setGroupsMask(gmCtrl);
			GetAPhysXScene().UnlockWritingScene();
		}
	}

	NxU32 CollisionFlags = 0;
	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM();
	m_pController->move(disp, GetActiveGroups(), 0.000001f, CollisionFlags, 1.0f, &gmFilter);

	if (0 != pShape)
	{
		GetAPhysXScene().LockWritingScene();
		pShape->setGroupsMask(gmActor);
		GetAPhysXScene().UnlockWritingScene();
	}
	m_ToFilterOutShapes.RecoverAttributes();
	return CollisionFlags;
}

void APhysXCCImp::SubmitFrameInfo()
{
	FrameInfo* pFI = m_pLastFrame;
	m_pLastFrame = m_pCurrentFrame;
	m_pCurrentFrame = pFI;
	m_pCurrentFrame->SetToInvalid();

	m_pActiveForCallBack = m_pLastFrame;
	m_pLastForCallBack = 0;
	m_ToFilterOutShapes.Clear();
	m_pBlocks.clear();
}

void APhysXCCImp::PrepareFrameInfo(const float deltaSecond)
{
	NxVec3 currentG = GetGravity();
	m_pCurrentFrame->SetGravity(currentG);
	m_pCurrentFrame->SetTime(deltaSecond);

	// NOTE: LocalDir is correct, do not revise it.
	// LocalDir will be updated in UpdateAttachment() that sequence revise action
	// So, make sure UpdateDir was called before revise position
	UpdateDir();

	// update position before CheckOverlap()
	if (ReadFlag(CCII_HAS_NEW_POSITION))
	{
		ClearFlag(CCII_HAS_NEW_POSITION);
		APhysXScene& scene = GetAPhysXScene();
		scene.LockWritingScene();
		m_pController->setPosition(m_newPos);
		RaiseFlag(CCII_DONE_TELEPORT);
		scene.UnlockWritingScene();
	}

	if (GetCCMImp().IsStaticObjectsChanged())
	{
		m_pController->reportSceneChanged();
		RaiseFlag(CCII_DONE_CALL_SCENE_CHANGED);
	}
}

void APhysXCCImp::UpdateInteract()
{
	assert(0 != m_pController);
	APhysXCCCollisionFlag flag = GetCCManager().GetInteractFlag();
	if (APX_CCCF_COLLIDE_ALL_CHANNEL == flag)
		m_pController->setInteraction(NXIF_INTERACTION_INCLUDE);
	else
		m_pController->setInteraction(NXIF_INTERACTION_EXCLUDE);
}

NxU32 APhysXCCImp::DoReviseWork(const NxVec3& disp)
{
	RaiseFlag(CCII_MOVE_FOR_REVISE);
	const NxVec3 targetPos = GetFootPositionInternal(true) + disp;
	float lastDisp = 0.0f;

	// disable Auto-Stepping to generate hit-shape callback
	const float stepBackup = m_pController->getStepOffset();
	m_pController->setStepOffset(0);

	NxU32 CollisionFlags = MoveController(disp, m_pLastFrame);

	// close the force-computing in the remain revise moving
	RaiseFlag(CCII_ENFORCE_CLOSE_FORCECOMPUTE);
	while (0 != CollisionFlags)
	{
		NxVec3 vDisp = targetPos - GetFootPositionInternal(true);
		if (vDisp.equals(gZero, APHYSX_FLOAT_EPSILON))
			break;

		// at present, we don't consider the case of this CC was blocked by other CC.
		// Only handle the case of this CC was blocked by actor.
		NxU32 nSize = m_pBlocks.size();
		if (0 == nSize)
			break;

		for (NxU32 i = 0; i < nSize; ++i)
		{
			NxActor& actor = m_pBlocks[i]->getActor();
			if (!actor.isDynamic())
				continue;

			if (actor.readBodyFlag(NX_BF_KINEMATIC))
				continue;

			m_ToFilterOutShapes.AddShape(*m_pBlocks[i]);
		}
		m_pBlocks.clear();

		if (m_ToFilterOutShapes.IsEmpty())
			break;

		CollisionFlags = MoveController(vDisp, m_pLastFrame);
		m_ToFilterOutShapes.Clear();

		float DispDist = vDisp.magnitudeSquared();
		if (DispDist > lastDisp)
			break;
		if (NxMath::equals(DispDist, lastDisp, APHYSX_FLOAT_EPSILON))
			break;
		lastDisp = DispDist;
	}
	ClearFlag(CCII_ENFORCE_CLOSE_FORCECOMPUTE);

	m_pController->setStepOffset(stepBackup);
	ClearFlag(CCII_MOVE_FOR_REVISE);
	return CollisionFlags;
}

NxU32 APhysXCCImp::DoReviseSurface(const NxVec3& disp)
{
	if (disp.equals(gZero, APHYSX_FLOAT_EPSILON))
		return 0;

	const AttachedActor* pBinding = m_pLastFrame->GetAttachmentBinding();
	if (0 == pBinding)
		return 0;

	if (!pBinding->pActor->isDynamic())
		return 0;

	if(pBinding->pActor->readBodyFlag(NX_BF_KINEMATIC))
		return 0;

	float d = disp.magnitude();
	const NxVec3 targetPos = GetFootPositionInternal(true) + disp;
	float MaxSweepDist = GetDetectionDistance() * 3 + d;
	if (5.0f > MaxSweepDist)
		MaxSweepDist = 5.0f;

	NxVec3 gDir = GetGravity();
	gDir.normalize();

	APhysXVolumeType vt;
	NxVec3 vol = GetVolume(vt);
	assert(APX_VT_CAPSULE == vt);
	float R = vol.x;

	NxVec3 start = targetPos;
	start += gDir * (-1 * vol.y * 0.5f);
	NxVec3 motion = gDir * MaxSweepDist;

	NxSweepQueryHit sqHit;
	if (!SweepClosestAction(start, motion, sqHit, &R))
	{
		m_pLastFrame->RaiseStatus(FrameInfo::IS_IN_AIR);
		m_DistToGround = MaxSweepDist * sqHit.t;
		return 0;
	}

	m_DistToGround = 0.0f;
	NxVec3 newPos = sqHit.point + sqHit.normal * R;
	newPos += gDir * R;
	newPos.y += GetHalfHeight(); 
	NxExtendedVec3 newPosExt = m_pController->getPosition();
	newPosExt.y = newPos.y;
	newPosExt.y += 0.0001; // avoid touching the surface
	APhysXScene& scene = GetAPhysXScene();
	scene.LockWritingScene();
	m_pController->setPosition(newPosExt);
	scene.UnlockWritingScene();

	NxActor& hitActor = sqHit.hitShape->getActor();
	UpdateAttachment(gDir, sqHit.normal, sqHit.point, hitActor);
	return NXCC_COLLISION_DOWN;
}

void APhysXCCImp::RevisePosAndAttachmentRoutine(NxU32& cfs, NxU32& cfsAll)
{
	if (ReadFlag(CCII_DONE_TELEPORT))
	{
		ClearFlag(CCII_DONE_TELEPORT);
	}
	else
	{
		bool bIsToSurface = false;
		if (ReadFlag(APX_CCF_ATTACHMENT_ONLY_VERTICAL))
		{
			if (!ReadFlag(CCII_NEW_ATTACHMENT_VERTICAL))
			{
				bIsToSurface = true;
				RaiseFlag(CCII_CLEAR_HORIZON_REVISE);
			}
		}

		NxVec3 dispError = GetDispReviseForForecase();
		if (!m_pLastFrame->GetStatus(FrameInfo::IS_IN_AIR))
		{
			if (ReadFlag(CCII_CLEAR_HORIZON_REVISE))
				dispError = GetVerticalSubValue(dispError, m_pLastFrame->GetGravity());
		}

		if (bIsToSurface)
		{
			cfs = DoReviseSurface(dispError);
			cfsAll |= cfs;
			return;
		}
		cfs = DoReviseWork(dispError);
		m_pLastFrame->UpdateStateByFlags(cfs);
		cfsAll |= cfs;
	}
	UpdateAttachCandidate(*m_pLastFrame);
}

void APhysXCCImp::UpdateVelocity(const NxVec3& MoveDir)
{
	NxVec3 realMoveDir = MoveDir;
	if (MoveDir.equals(gZero, APHYSX_FLOAT_EPSILON))
		realMoveDir = m_SlideDir;
	NxVec3 newVel = realMoveDir;
	newVel.normalize();
	newVel *= GetSpeed();

	bool bCurrentGIsZero = false;
	const NxVec3 curGravity = m_pCurrentFrame->GetGravity();
	if (curGravity.equals(gZero, APHYSX_FLOAT_EPSILON))
		bCurrentGIsZero = true;

	bool bIsInAir = false;
	if (m_pLastFrame->GetStatus(FrameInfo::IS_IN_AIR) && !bCurrentGIsZero)
	{
		bIsInAir = true;
		if (ReadFlag(APX_CCF_CONTROL_JUMPINGDIR_ENABLE))
		{
			NxVec3 lastVel = m_pLastFrame->GetVel();
			NxVec3 vVel = GetVerticalSubValue(lastVel, curGravity);
			newVel += vVel;
		}
		else
		{
			newVel = m_pLastFrame->GetVel();
		}
		newVel += curGravity * m_pCurrentFrame->GetTime();
		if (m_pLastFrame->GetStatus(FrameInfo::IS_JUMPING_TAKEOFF))
		{
			const AttachedActor* pJ = m_pLastFrame->GetJumpingTakeOffBackUp();
			if (0 != pJ)
			{
				float m = pJ->velPT.magnitude();
				if (GetAttachingPTLinearVelThreshold() < m)
				{
					NxVec3 ptVel = pJ->velPT;
					if (ReadFlag(APX_CCF_ATTACHMENT_ONLY_VERTICAL))
						ptVel = GetVerticalSubValue(ptVel, curGravity);
					newVel += ptVel;
				}
			}
		}
	}

	if (!NxMath::equals(m_JumpSpeed, 0.0f, APHYSX_FLOAT_EPSILON))
	{
		m_pCurrentFrame->RaiseStatus(FrameInfo::INCREASED_JUMPING_VELOCITY);
		NxVec3 up = -1 * curGravity;
		up.normalize();
		if (ReadFlag(CCII_SET_ABSOLUTE_JUMPING_VEL))
		{
			newVel -= GetVerticalSubValue(newVel, curGravity);
			if (!bIsInAir)
			{
				AttachedActor* pBinding = m_pLastFrame->GetAttachmentBinding();
				if (0 != pBinding)
				{
					NxVec3 v = GetVerticalSubValue(pBinding->velPT, curGravity);
					newVel -= v;
				}
			}
		}
		newVel += up * m_JumpSpeed;
		m_JumpSpeed = 0.0f;
	}

	if (ReadFlag(CCII_CLEAR_ABSOLUTE_HORIZON_VEL) || ReadFlag(CCII_CLEAR_RELATIVE_HORIZON_VEL))
		newVel = GetVerticalSubValue(newVel, curGravity);
	m_pCurrentFrame->SetVel(newVel);
}

NxU32 APhysXCCImp::MoveCC(const float deltaSecond, const NxVec3& MoveDir)
{
	SubmitFrameInfo();
	PrepareFrameInfo(deltaSecond);
	
	NxU32 CFs = 0;
	NxU32 CFsAll = 0;
	RevisePosAndAttachmentRoutine(CFs, CFsAll);
	CheckOverlap(true);
	ClearFlag(CCII_CLEAR_HORIZON_REVISE);
	ClearFlag(CCII_NEW_ATTACHMENT_VERTICAL);

	// from here, start the new frame action, CC initiative move
	m_LastPos = GetFootPositionInternal(true);
	m_pActiveForCallBack = m_pCurrentFrame;
	m_pLastForCallBack = m_pLastFrame;
	m_pCurrentFrame->SetInputMoveDir(MoveDir);
	if (MoveDir.isZero() && !m_SlideDir.isZero())
		RaiseFlag(CCII_IS_SLIDING_MOVE);
	else
		ClearFlag(CCII_IS_SLIDING_MOVE);
	UpdateVelocity(MoveDir);
	ClearFlag(CCII_SET_ABSOLUTE_JUMPING_VEL);
	ClearFlag(CCII_CLEAR_RELATIVE_HORIZON_VEL);
	UpdateInteract();
	if (IsSleeping())
	{
		m_pCurrentFrame->PutToSleep();
		return CFsAll;
	}

	NxVec3 dS = GetDispForMove();
	CFs = MoveController(dS);
	m_pCurrentFrame->UpdateStateByFlags(CFs);
	m_pCurrentFrame->UpdateStateByFrame(*m_pLastFrame);
	UpdateAttachCandidate(*m_pCurrentFrame);
	ComputeJumpingTakeOffForce();
	m_pCurrentFrame->ApplyForce(GetAPhysXScene());
	CFsAll |= CFs;
	if (m_pLastFrame->GetStatus(FrameInfo::IS_PUSHING))
	{
		if (!m_pCurrentFrame->GetStatus(FrameInfo::IS_PUSHING))
			ContinuesPushingDetection(MoveDir);
	}

	if (!m_pCurrentFrame->GetStatus(FrameInfo::IS_JUMPING_LANDING))
	{
		NxVec3 InAirReviseM = m_pCurrentFrame->GetInAirReviseMove();
		if (ReadFlag(APX_CCF_ATTACHMENT_ONLY_VERTICAL))
		{
			RaiseFlag(CCII_ENFORCE_CLOSE_ATTACHMENT);
			RaiseFlag(CCII_ENFORCE_CLOSE_FORCECOMPUTE);
			MoveController(InAirReviseM, m_pCurrentFrame);
			ClearFlag(CCII_ENFORCE_CLOSE_ATTACHMENT);
			ClearFlag(CCII_ENFORCE_CLOSE_FORCECOMPUTE);
		}
		else
		{
			// move CC to the forecast position according to the velocity of current attached point
			NxVec3 footPos = GetFootPositionInternal(true);
			footPos += InAirReviseM;
			m_pCurrentFrame->DispFMove = m_pCurrentFrame->GetForecastMove(GetAttachingPTLinearVelThreshold());
			if (ReadFlag(CCII_CLEAR_ABSOLUTE_HORIZON_VEL))
			{
				m_pCurrentFrame->DispFMove = GetVerticalSubValue(m_pCurrentFrame->DispFMove, m_pCurrentFrame->GetGravity());
				ClearFlag(CCII_CLEAR_ABSOLUTE_HORIZON_VEL);
				RaiseFlag(CCII_CLEAR_HORIZON_REVISE);
			}
			RaiseFlag(CCII_ENFORCE_CLOSE_ATTACHMENT);
			RaiseFlag(CCII_ENFORCE_CLOSE_FORCECOMPUTE);
			MoveController(m_pCurrentFrame->DispFMove + InAirReviseM, m_pCurrentFrame);
			ClearFlag(CCII_ENFORCE_CLOSE_ATTACHMENT);
			ClearFlag(CCII_ENFORCE_CLOSE_FORCECOMPUTE);
			m_pCurrentFrame->DispFMoveUnfinished = footPos + m_pCurrentFrame->DispFMove - GetFootPositionInternal(true);
		}
	}

	UpdateStatus();
	ApplyNewSetting();
	
	if (APX_CCFM_SYNCHRONOUS == GetCCManager().GetFlushMode())
		APhysXCharCtrlerMgrImp::GetCCM()->TickForSynPos();
	return CFsAll;
}

bool APhysXCCImp::SweepClosestAction(const NxVec3& startPos, const NxVec3& motion, NxSweepQueryHit& outSQHit, float* pR) const
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
	
	NxCapsule worldCapsule;
	worldCapsule.radius = radius;
	if (0 != pR)
		worldCapsule.radius = *pR;
	worldCapsule.p0 = worldCapsule.p1 = startPos;
	worldCapsule.p0.y += worldCapsule.radius;
	worldCapsule.p1.y += worldCapsule.radius;
	
	NxSweepQueryHit sqHit;
	NxU32 flags = NX_SF_STATICS | NX_SF_DYNAMICS;
	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM();
	NxU32 nHit = GetNxScene().linearCapsuleSweep(worldCapsule, motion, flags, 0, 1, &outSQHit, 0, GetActiveGroups(), &gmFilter);
	return (0 == nHit)? false : true;
}

void APhysXCCImp::UpdateAttachCandidate(FrameInfo& ToUpdate)
{
	m_DistToGround = NX_MAX_F32;
	if (0 < ToUpdate.GetHitCosVal())
	{
		// already exist candidate or attachment, just return
		m_DistToGround = 0.0f;
		return;
	}

	float MaxSweepDist = GetDetectionDistance() * 3;
	if (5.0f > MaxSweepDist)
		MaxSweepDist = 5.0f;
	NxVec3 sweepDir = ToUpdate.GetGravity();
	sweepDir.normalize();
	if (sweepDir.isZero()) sweepDir.set(0, -1, 0);
	NxVec3 motion = sweepDir * MaxSweepDist;

	float Offset = 0;
	NxSweepQueryHit sqHit;
	if (!SweepClosestAction(GetRayStartPos(Offset), motion, sqHit))
		return;

	NxActor& hitActor = sqHit.hitShape->getActor();
	APhysXObject* pObject = static_cast<APhysXObject*>(hitActor.userData);
	if (0 != pObject)
	{
		if (pObject->IsDeleted())
			return;
	}

	m_DistToGround = MaxSweepDist * sqHit.t;
	if (!hitActor.isDynamic())
		return;

	ToUpdate.UpdateCandidateInfo(hitActor, sqHit.point, m_faceDir, false);
}

bool APhysXCCImp::GetSlideDir(const NxVec3& normal, const NxVec3& g, NxVec3& outSlideDir)
{
	static const float MaxAngleValue = (float)cos(m_SlopeUpRadian);

	float cosRes = GetCosValue(normal, g * -1);

	bool bCanAttach = false;
	if (MaxAngleValue < cosRes)
		bCanAttach = true;

	if (bCanAttach)
	{
		outSlideDir.zero();
	}
	else if (ReadFlag(APX_CCF_SLIDE_ENABLE))
	{
		NxVec3 vVN = GetVerticalSubValue(normal, g);
		outSlideDir = normal - vVN;
	}
	return bCanAttach;
}

void APhysXCCImp::UpdateAttachment(const NxVec3& moveDir, const NxVec3& hitNormal, const NxVec3& hitPos, NxActor& hitActor)
{
	if (ReadFlag(CCII_ENFORCE_CLOSE_ATTACHMENT))
		return;

	APhysXObject* pObject = static_cast<APhysXObject*>(hitActor.userData);
	if (0 != pObject)
	{
		if (pObject->IsDeleted())
			return;
	}

	NxVec3 G = m_pActiveForCallBack->GetGravity();
	if (G.isZero()) G.set(0, -1, 0);
	float cosRes = GetCosValue(moveDir, G);
	if (0 >= cosRes)
	{
		m_pActiveForCallBack->SetHitCosVal(0.0f);
		return;
	}
	if (m_pActiveForCallBack->GetHitCosVal() >= cosRes)
		return;

	bool EnableAttach = CheckCCFlagForAttachment(hitActor);
	if (!hitActor.isDynamic())
		EnableAttach = false;
	if (!GetSlideDir(hitNormal, G, m_SlideDir))
		EnableAttach = false;
	if (EnableAttach)
		m_SlideDir.zero();

	m_GroundNormal = hitNormal;
	m_pActiveForCallBack->SetHitCosVal(cosRes);
	m_pActiveForCallBack->ClearStatus(FrameInfo::IS_IN_AIR);
	m_pActiveForCallBack->UpdateCandidateInfo(hitActor, hitPos, m_faceDir, EnableAttach);
}

void APhysXCCImp::ComputeHitForce(const NxVec3& moveDir, const NxVec3& hitPos, NxActor& hitActor, bool& outIsPushing)
{
	outIsPushing = false;
	if (ReadFlag(CCII_ENFORCE_CLOSE_FORCECOMPUTE))
		return;

	bool EnablePushForce  = CheckCombinedFlagEnable(hitActor, APX_CCF_ADD_PUSHFORCE);
	bool EnableStandForce = CheckCombinedFlagEnable(hitActor, APX_CCF_ADD_STANDFORCE);
	if (!EnablePushForce && !EnableStandForce)
		return;

	NxF32 timeSec = m_pActiveForCallBack->GetTime();
	if (NxMath::equals(timeSec, 0.0f, APHYSX_FLOAT_EPSILON))
		timeSec = 1;

	NxVec3 G = m_pActiveForCallBack->GetGravity();
	NxVec3 ccVel = m_pActiveForCallBack->GetVel();
	NxVec3 ccVVel = GetVerticalSubValue(ccVel, G);
	NxVec3 ccHVel = ccVel - ccVVel;

	NxVec3 vDir = GetVerticalSubValue(moveDir, G);
	NxVec3 hDir = moveDir - vDir;

	if (EnablePushForce && !hDir.isZero())
	{
		// push force was only generated while cc is working/running & without "in-air" status 
		if (!ccHVel.isZero())
		{
			bool IsInAir = false;
			if (0 != m_pLastForCallBack)
				IsInAir = m_pLastForCallBack->GetStatus(FrameInfo::IS_IN_AIR);

			if (!IsInAir)
			{
				m_pActiveForCallBack->AddPushing(hitActor);
				m_pActiveForCallBack->RaiseStatus(FrameInfo::IS_PUSHING);
				m_pActiveForCallBack->AddForceHorizontal(hitActor, hitPos, hDir, GetPushFactor());
				outIsPushing = true;
			}
		}
	}

	if (EnableStandForce && !vDir.isZero())
	{
		NxVec3 vForce(0.0f);
		if (ccVVel.isZero())
		{
			// add stand force when CC is stand/walk on something
			vForce = GetMass() * G;
		}
		else
		{
			// add landing force
			if (GetJumpingFactor() < 0)
			{
				NxVec3 hitActorVel = hitActor.getLinearVelocity();
				NxVec3 actorVVel = GetVerticalSubValue(hitActorVel, G);
				NxVec3 vRelative = ccVVel - actorVVel;
				
				NxVec3 t = G * 0.1f;
				if (vRelative.magnitude() < t.magnitude())
				{
					// here is an approximate calculation when the relative velocity is small enough
					vForce = GetMass() * G;
				}
				else
				{
					//  vF(CC to actor) = -1 * Fac(actor to CC)
					//  Fac + mg = ma  &&  v + at = 0  so,  Fac = -mv/t - mg      
					//  vF = m(g + v/t)
					vForce = GetMass() * (G + vRelative / timeSec);
				}
			}
			else
			{
				vForce = GetMass() * G * GetJumpingFactor();
			}
		}
		m_pActiveForCallBack->AddForceVertical(hitActor, hitPos, vForce, false);
	}
}

void APhysXCCImp::ComputeJumpingTakeOffForce()
{
	if (!m_pCurrentFrame->GetStatus(FrameInfo::IS_JUMPING_TAKEOFF))
		return;

	const AttachedActor* pJ = m_pCurrentFrame->GetJumpingTakeOffBackUp();
	if (0 == pJ)
		return;

	NxActor* pActor = const_cast<NxActor*>(pJ->pActor);
	if (!CheckCombinedFlagEnable(*pActor, APX_CCF_ADD_STANDFORCE))
		return;

	NxVec3 force(0.0f);
	if (GetJumpingFactor() < 0)
	{
		NxF32 timeSec = m_pCurrentFrame->GetTime();
		if (NxMath::equals(timeSec, 0.0f, APHYSX_FLOAT_EPSILON))
			timeSec = 1;

		//  F(CC to actor) = -1 * Fac(actor to CC)
		//  Fac + mg = ma  &&  0 + at = v  so,  Fac = mv/t - mg      
		//  F = m(g - v/t)
		force = GetMass() * (m_pCurrentFrame->GetGravity() - m_pCurrentFrame->GetVel() / timeSec);
	}
	else
	{
		force = GetMass() * m_pCurrentFrame->GetGravity() * GetJumpingFactor();
	}
	if (!force.isZero())
		m_pCurrentFrame->AddForceVertical(*pActor, pJ->globalPos, force, true);
}

NxVec3 APhysXCCImp::GetInstantVelocity(bool bIsAbsVel) const
{
	// using actual move velocity instead of input velocity
	NxVec3 vel = m_pCurrentFrame->GetVel();
	float speed = vel.magnitude();
	if (!NxMath::equals(speed, 0.0f, APHYSX_FLOAT_EPSILON))
	{
		float ftime = m_pCurrentFrame->GetTime();
		if (NxMath::equals(ftime, 0.0f, APHYSX_FLOAT_EPSILON))
			ftime = 0.1f;

		NxVec3 deltaMove = GetFootPositionInternal(true) - m_LastPos;
		vel = deltaMove / ftime;
	}
	if (bIsAbsVel)
	{
		const AttachedActor* pBinding = m_pCurrentFrame->GetAttachmentBinding();
		if (0 != pBinding)
			vel += pBinding->velPT;
	}
	return vel;
}

bool APhysXCCImp::IsComponent(const NxActor& actor) const
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

NxActor* APhysXCCImp::GetHiddenActor() const
{
	if (0 == m_pController)
		return 0;
	return m_pController->getActor();
}

void APhysXCCImp::ReportSceneChanged()
{
	if (ReadFlag(CCII_DONE_CALL_SCENE_CHANGED))
	{
		ClearFlag(CCII_DONE_CALL_SCENE_CHANGED);
		return;
	}

	assert(0 != m_pController);
	m_pController->reportSceneChanged();
}

void APhysXCCImp::OnObjectDeleted(APhysXObject& obj)
{
	APhysXCCBase::OnObjectDeleted(obj);
	m_pCurrentFrame->CheckActorIsDeleted(obj);
}

void APhysXCCImp::UpdateStatus()
{
	m_Status = APX_CCS_JUMPING_INAIR;
	if (!m_pCurrentFrame->GetStatus(FrameInfo::IS_IN_AIR))
		m_Status = APX_CCS_STANDON_SOMETHING;

	if (m_pCurrentFrame->GetStatus(FrameInfo::INCREASED_JUMPING_VELOCITY))
		m_Status |= APX_CCS_JUMPING_TAKEOFFE;

	if (m_pCurrentFrame->GetStatus(FrameInfo::IS_JUMPING_LANDING))
	{
		m_Status &= ~APX_CCS_JUMPING_INAIR;
		m_Status |= APX_CCS_JUMPING_LANDING;
	}
}

bool APhysXCCImp::ContinuesPushingDetection(const NxVec3& MoveDir)
{
	if (!(APX_CCS_STANDON_SOMETHING & GetStatus()))
		return false;

	float fdot = MoveDir.dot(m_pLastFrame->GetInputMoveDir());
	if (fdot < 0.05f)
		return false;

	float radius = 0.0f;
	APhysXVolumeType vt;
	NxVec3 volume = GetVolume(vt);
	radius = volume.x;
	if (APX_VT_BOX == vt)
	{
		if (volume.z < volume.x)
			radius = volume.z;
	}

	NxCapsule worldCapsule;
	worldCapsule.radius = radius * 0.75f;
	worldCapsule.p0 = worldCapsule.p1 = GetCenterPositionInternal(true);

	float sweepLength = radius;
	const NxActor* pActor = 0;
	AttachedActor* pAA = m_pLastFrame->GetAttachmentBinding();
	if (0 != pAA)
		pActor = pAA->pActor;
	EntityReportSweepForPushing sweepReport(*this, *m_pCurrentFrame, pActor, sweepLength);

	NxVec3 groundNormal = GetGroundNormal();
	NxVec3 right = groundNormal.cross(MoveDir);
	NxVec3 motion = right.cross(groundNormal);
	motion *= sweepLength;

	NxU32 flags = NX_SF_DYNAMICS | NX_SF_ALL_HITS;
	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM();
	GetNxScene().linearCapsuleSweep(worldCapsule, motion, flags, 0, 0, 0, &sweepReport, GetActiveGroups(), &gmFilter);
	return m_pCurrentFrame->GetStatus(FrameInfo::IS_PUSHING);
}
