/*
* FILE: APhysXCharCtrlerLightWeight.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2010/02/10
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*/

#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerMgrImp.h"
#include "APhysXCharCtrlerImp.h"
#include "APhysXCharCtrlerLightWeight.h"

APhysXCCChannelMgr::APhysXCCChannelMgr(APhysXCharCtrlerMgrImp& ccMgrImp, APhysXScene& scene)
	: m_CCMgrImp(ccMgrImp), m_APhysXScene(scene), m_Scene(*scene.GetNxScene())
{
	m_ActiveGroups = 0xffffffff;
	ApplyHostModelChannel();
}

APhysXCCChannelMgr::~APhysXCCChannelMgr()
{
	// due to delayed release, so host model probably exist
	APhysXCollisionChannel curVal;
	int nCount = m_HostModelCIBackup.size();
	for (int i = 0; i < nCount; ++i)
	{
		curVal = m_HostModelCIBackup[i].pObj->GetCollisionChannel();
		m_HostModelCIBackup[i].RecoverSetting();
	}
	m_HostModelCIBackup.clear();
	m_HostModelCNLs.clear();
}

void APhysXCCChannelMgr::OpenGroup(const APhysXCollisionGroup& group)
{
	if (APX_COLLISION_GROUP_INVALID > group)
		m_ActiveGroups |= (1 << group);
}

void APhysXCCChannelMgr::CloseGroup(const APhysXCollisionGroup& group)
{
	if (APX_COLLISION_GROUP_INVALID > group)
		m_ActiveGroups &= ~(1 << group);
}

APhysXCCChannelMgr::CollisionInfo::CollisionInfo()
{
	pObj = 0;
	bHasOneWayBK = false;
	bHasChannelBK = false;
	bHasCollisionGroupBK = false;
	bkOneWayValue = 0;
	bkChannelValue = 0;
	bkCollisionGroupValue = 0;
}

bool APhysXCCChannelMgr::CollisionInfo::PrepareSetting(APhysXCollisionChannel& outNewCNL)
{
	if (0 == pObj)
		return false;

	// at present, doesn't make the one way setting 
#if 0
	bool bIsOneWay = pObj->GetObjectFlag(APX_OBJFLAG_ONEWAY_COLLISION);
	bkOneWayValue = bIsOneWay? 1 : 0;
	if (!bIsOneWay)
	{
		bHasOneWayBK = true;
		pObject->SetObjectFlag(APX_OBJFLAG_ONEWAY_COLLISION, true);
	}
#endif

	bkCollisionGroupValue = pObj->GetCollisionGroup();
	if (APX_COLLISION_GROUP_CC_HOST_RB != bkCollisionGroupValue)
	{
		bHasCollisionGroupBK = true;
		pObj->SetCollisionGroup(APX_COLLISION_GROUP_CC_HOST_RB);
	}

	outNewCNL = bkChannelValue = pObj->GetCollisionChannel();
	if ((outNewCNL > APX_COLLISION_CHANNEL_INVALID) && (outNewCNL < APX_COLLISION_CHANNEL_CC_COMMON_HOST))
	{
		bHasChannelBK = true;
		outNewCNL = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
		pObj->SetCollisionChannel(outNewCNL);
	}

	return true;
}

APhysXCollisionChannel APhysXCCChannelMgr::CollisionInfo::RecoverSetting()
{
	APhysXCollisionChannel cnlRtn = bkChannelValue;
	if (0 == pObj)
		return cnlRtn;

	if (bHasOneWayBK)
	{
		bool bVal = (0 == bkOneWayValue)? false : true;
		pObj->SetObjectFlag(APX_OBJFLAG_ONEWAY_COLLISION, bVal);
		bHasOneWayBK = false;
	}

	if (bHasChannelBK)
	{
		pObj->SetCollisionChannel(cnlRtn);
		bHasChannelBK = false;
	}

	if (bHasCollisionGroupBK)
	{
		pObj->SetCollisionGroup(APhysXCollisionGroup(bkCollisionGroupValue));
		bHasCollisionGroupBK = false;
	}

	return cnlRtn;
}

bool APhysXCCChannelMgr::RegisterHostModel(APhysXObject* pObject, APhysXCollisionChannel& outNewCNL)
{
	if (0 == pObject)
		return false;

	int nCount = m_HostModelCIBackup.size();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_HostModelCIBackup[i].pObj == pObject)
		{
			outNewCNL = pObject->GetCollisionChannel();
			return true;
		}
	}

	CollisionInfo colInfo;
	colInfo.pObj = pObject;
	colInfo.PrepareSetting(outNewCNL);
	m_HostModelCIBackup.push_back(colInfo);

	nCount = m_HostModelCNLs.size();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_HostModelCNLs[i].m_Channel == outNewCNL)
		{
			m_HostModelCNLs[i].m_nCount += 1;
			return true;
		}
	}

	CountableCNL cnlNode;
	cnlNode.m_Channel = outNewCNL;
	cnlNode.m_nCount = 1;
	m_HostModelCNLs.push_back(cnlNode);
	ApplyHostModelChannel();
	return true;
}

bool APhysXCCChannelMgr::UnRegisterHostModel(APhysXObject* pObject, APhysXCollisionChannel& outOriginalCNL)
{
	if (0 == pObject)
		return false;

	int nCount = m_HostModelCIBackup.size();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_HostModelCIBackup[i].pObj == pObject)
		{
			CollisionInfo colInfo = m_HostModelCIBackup[i];
			m_HostModelCIBackup[i] = m_HostModelCIBackup[nCount - 1];
			m_HostModelCIBackup.pop_back();
			outOriginalCNL = colInfo.RecoverSetting();

			nCount = m_HostModelCNLs.size();
			for (int j = 0; j < nCount; ++j)
			{
				if (m_HostModelCNLs[j].m_Channel == outOriginalCNL)
				{
					m_HostModelCNLs[j].m_nCount -= 1;
					if (0 == m_HostModelCNLs[j].m_nCount)
					{
						CountableCNL cntCNL = m_HostModelCNLs[j];
						m_HostModelCNLs[j] = m_HostModelCNLs[nCount - 1];
						m_HostModelCNLs.pop_back();
						ApplyHostModelChannel();
					}
					break;
				}
			}
			return true;
		}
	}
	return false;
}

void APhysXCCChannelMgr::ApplyHostModelChannel()
{
	APhysXCollisionChannelManager* pCCM = m_APhysXScene.GetCollisionChannelManager();
	assert(0 != pCCM);
	if (0 == pCCM)
		return;

	m_gmUnionSetHostSkip = pCCM->GetChannelGroupMask(APX_COLLISION_CHANNEL_CC_SKIP_COLLISION);
	NxGroupsMask gmTemp;
	int nSize = m_HostModelCNLs.size();
	for (int i = 0; i < nSize; ++i)
	{
		gmTemp = pCCM->GetChannelGroupMask(m_HostModelCNLs[i].m_Channel);
		m_gmUnionSetHostSkip.bits0 |= gmTemp.bits0;
		m_gmUnionSetHostSkip.bits1 |= gmTemp.bits1;
		m_gmUnionSetHostSkip.bits2 |= gmTemp.bits2;
		m_gmUnionSetHostSkip.bits3 |= gmTemp.bits3;
	}
}

NxGroupsMask APhysXCCChannelMgr::GetFilterGM(const bool bEnforceCombineCCMRegGM) const
{
	NxGroupsMask gmFilter;
	gmFilter.bits0 = gmFilter.bits1 = gmFilter.bits2 = gmFilter.bits3 = 0;
	
	if (bEnforceCombineCCMRegGM || (APX_CCCF_FILTERING_REG_CHANNEL == m_CCMgrImp.GetInteractFlag()))
		gmFilter = m_CCMgrImp.GetRegisteredGM(m_APhysXScene);
	
	gmFilter.bits0 |= m_gmUnionSetHostSkip.bits0;
	gmFilter.bits1 |= m_gmUnionSetHostSkip.bits1;
	gmFilter.bits2 |= m_gmUnionSetHostSkip.bits2;
	gmFilter.bits3 |= m_gmUnionSetHostSkip.bits3;
	
	gmFilter.bits0 = ~gmFilter.bits0;
	gmFilter.bits1 = ~gmFilter.bits1;
	gmFilter.bits2 = ~gmFilter.bits2;
	gmFilter.bits3 = ~gmFilter.bits3;
	return gmFilter;
}

NxVec3 APhysXLWCharCtrler::GetGroundNormal(bool* outpbIsDefVal, const NxVec3& defVal) const
{
	if (0 != outpbIsDefVal)
		*outpbIsDefVal = false;
	
	if (m_GroundNormal.isZero())
	{
		if (0 != outpbIsDefVal)
			*outpbIsDefVal = true;
		return defVal;
	}
	
	return m_GroundNormal;
}

bool APhysXLWCharCtrler::ReadFlag(const APhysXLWCCFlag flag) const
{
	return (m_flags & flag)? true : false;
}

void APhysXLWCharCtrler::ClearFlag(const APhysXLWCCFlag flag)
{
	m_flags &= ~flag;
}

void APhysXLWCharCtrler::RaiseFlag(const APhysXLWCCFlag flag)
{
	m_flags |= flag;
}

const float APhysXLWCCImp::gExpFactor = 0.8f;

APhysXLWCCImp::APhysXLWCCImp(const APhysXLWCCDesc& desc, NxActor& actor, APhysXScene& scene)
	: APhysXLWCharCtrler(scene, desc.flags)
{
	m_pKActor = &actor;
	m_Radius = desc.radius;
	m_Height = desc.height;
	m_CurrentCenterPos = actor.getGlobalPosition();

	NxCapsuleShape* pCap = actor.getShapes()[0]->isCapsule();
	assert(0 != pCap);
	m_NxVolume.x = pCap->getRadius();
	m_NxVolume.y = pCap->getHeight();

	SetDetectionDistance(m_Height);
}

APhysXLWCCImp::~APhysXLWCCImp()
{
	assert(0 == m_pKActor);
	m_pKActor = 0;
}

void APhysXLWCCImp::ReleaseNxActor()
{
	if (0 == m_pKActor)
		return;

	APhysXScene& scene = GetAPhysXScene();
	scene.LockWritingScene();
	GetNxScene().releaseActor(*m_pKActor);
	scene.UnlockWritingScene();
	m_pKActor = 0;
}

NxVec3 APhysXLWCCImp::GetVolume(APhysXVolumeType& outVT) const
{
	outVT = APX_VT_CAPSULE;
	return NxVec3(m_Radius, m_Height, -1);	
}

bool APhysXLWCCImp::ChangeVolume(const NxVec3& newVolume)
{
	if (NxMath::equals(m_Radius, newVolume.x, APHYSX_FLOAT_EPSILON) && NxMath::equals(m_Height, newVolume.y, APHYSX_FLOAT_EPSILON))
		return true;

	assert(0 != m_pKActor);
	if (0 == m_pKActor)
		return false;

	NxCapsuleShape* pCapsule = m_pKActor->getShapes()[0]->isCapsule();
	assert(0 != pCapsule);
	if (0 == pCapsule)
		return false;

	float newRadius = gExpFactor * newVolume.x;
	float newHeight = gExpFactor * (newVolume.y - 2 * newVolume.x);
	if (0 > newRadius) return false;
	if (0 > newHeight) return false;
	
	float oldRadius = pCapsule->getRadius();
	float oldHeight = pCapsule->getHeight();
	m_CurrentCenterPos.y += (newHeight - oldHeight) * 0.5f + (newRadius - oldRadius);
	
	APhysXScene& scene = GetAPhysXScene();
	scene.LockWritingScene();
	m_pKActor->moveGlobalPosition(m_CurrentCenterPos);
	pCapsule->setRadius(newRadius);
	pCapsule->setHeight(newHeight);
	scene.UnlockWritingScene();

	m_Radius = newVolume.x;
	m_Height = newVolume.y;
	m_NxVolume.x = newRadius;
	m_NxVolume.y = newHeight;
	return true;
}

bool APhysXLWCCImp::IsComponent(const NxActor& actor) const
{
	if (0 == m_pKActor)
		return false;
	
	return (&actor == m_pKActor);
}

NxVec3 APhysXLWCCImp::GetFootPosition() const
{
	NxVec3 pos(m_CurrentCenterPos);

	if (0 == m_pKActor)
		return pos;

	pos.y -= m_Height * 0.5f;
	return pos;
}

void APhysXLWCCImp::SetFootPosition(const NxVec3& pos)
{
	if (0 == m_pKActor)
		return;

	m_CurrentCenterPos = pos;
	m_CurrentCenterPos.y += m_Height * 0.5f;

	APhysXScene& scene = GetAPhysXScene();
	scene.LockWritingScene();
	m_pKActor->moveGlobalPosition(m_CurrentCenterPos);
	scene.UnlockWritingScene();
}

bool APhysXLWCCImp::GetGroundPos(float sweepDist, NxVec3& pos, NxVec3* poutNormal) const
{
	static NxVec3 gDir(0, -1, 0);

	bool IsUpdated = false;

	APhysXVolumeType vt;
	NxVec3 vol = GetVolume(vt);
	assert(APX_VT_CAPSULE == vt);

	NxVec3 start = pos;
	start += gDir * (-1 * vol.y * 0.5f);
	NxCapsule worldCapsule;
	worldCapsule.p0 = worldCapsule.p1 = start;
	worldCapsule.radius = m_Radius;

	NxVec3 motion = gDir * sweepDist;
	NxU32  flags = NX_SF_STATICS | NX_SF_DYNAMICS | NX_SF_ALL_HITS;
	EntityReportSweep tempReport;
	tempReport = m_SweepReport;
	tempReport.ResetPos(worldCapsule.p0, pos, true);

	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM(true);
	GetNxScene().linearCapsuleSweep(worldCapsule, motion, flags, 0, 0, 0, &tempReport, GetActiveGroups(), &gmFilter);

	NxVec3 normal(0.0f);
	NxVec3 newpos = tempReport.GetProperPos(IsUpdated, normal, true);
	if (IsUpdated)
	{
		pos = newpos;
		if (0 != poutNormal)
			*poutNormal = normal;
	}
	return IsUpdated;
}

bool APhysXLWCCImp::TickReviseVerticalPos()
{
	static NxVec3 gDir(0, -1, 0);

	if (IsSleeping())
		return false;
	
	if (0 == m_pKActor)
		return false;

	bool IsUpdated = false;
	bool IsEnforceRevising = ReadFlag(APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN);

	NxCapsule worldCapsule;
	worldCapsule.p0 = worldCapsule.p1 = GetCenterPosition();
	worldCapsule.radius = m_Radius * gExpFactor;
	NxCapsuleShape* pCap = m_pKActor->getShapes()[0]->isCapsule();
	if (0 != pCap)
		worldCapsule.radius = pCap->getRadius();
	
	NxVec3 motion = gDir * GetDetectionDistance();
	bool bIsInair = false;
	if (ReadFlag(APX_LWCCF_JUMPING_INAIR))
	{
		bIsInair = true;
		static const float skinWidth = 0.01f;
		float dist = m_Height * 0.5f - worldCapsule.radius - skinWidth;
		if (0 > dist)
			dist = 0.0f;
		motion = gDir * dist;
	}

	NxU32  flags = NX_SF_STATICS | NX_SF_DYNAMICS | NX_SF_ALL_HITS;
	m_SweepReport.ResetPos(worldCapsule.p0, GetFootPosition(), IsEnforceRevising);
	
	NxGroupsMask gmFilter = m_pCCCM->GetFilterGM(true);
	GetNxScene().linearCapsuleSweep(worldCapsule, motion, flags, 0, 0, 0, &m_SweepReport, GetActiveGroups(), &gmFilter);
	
	NxVec3 pos = m_SweepReport.GetProperPos(IsUpdated, m_GroundNormal, !bIsInair);
	m_DistToGround = NX_MAX_F32;
	m_Status = APX_CCS_JUMPING_INAIR;
	if (IsUpdated)
	{
		SetFootPosition(pos);
		m_DistToGround = 0.0f;
		m_Status = APX_CCS_STANDON_SOMETHING;
	}
	
	// obsolete codes. The collision was controlled by dominance group
	//if (m_bDisableCollision)
	//	OverlapTest();
	return IsUpdated;
}

bool APhysXLWCCImp::OverlapTest()
{
	bool IsOverlap = false;
	bool IsOverlapClosest = false;
	m_OverlapReport.Clear();

	NxCapsuleShape* pCap = m_pKActor->getShapes()[0]->isCapsule();
	assert(0 != pCap);
	if (0 != pCap)
	{
		NxCapsule worldCapsule;
		pCap->getWorldCapsule(worldCapsule);

		NxGroupsMask gmFilter = m_pCCCM->GetFilterGM(true);
		NxU32 nShapes = GetNxScene().overlapCapsuleShapes(worldCapsule, NX_DYNAMIC_SHAPES, 0, 0, &m_OverlapReport, GetActiveGroups(), &gmFilter, true);
		if (0 < nShapes)
			IsOverlap = true;

		NxShape* pCDASShape = m_SweepReport.GetClosestDynamicActorShape();
		if (0 != pCDASShape)
		{
			if (m_OverlapReport.IsInclude(*pCDASShape))
				IsOverlapClosest = true;
		}
	}
		
	if (IsOverlapClosest)
	{
		if (!m_pKActor->readActorFlag(NX_AF_DISABLE_COLLISION))
		{
			APhysXScene& scene = GetAPhysXScene();
			scene.LockWritingScene();
			if (m_pKActor->isSleeping()) m_pKActor->wakeUp();
			m_pKActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
			scene.UnlockWritingScene();
		}
	}

	if (!IsOverlap)
	{
		if (m_pKActor->readActorFlag(NX_AF_DISABLE_COLLISION))
		{
			APhysXScene& scene = GetAPhysXScene();
			scene.LockWritingScene();
			if (m_pKActor->isSleeping()) m_pKActor->wakeUp();
			m_pKActor->clearActorFlag(NX_AF_DISABLE_COLLISION);
			scene.UnlockWritingScene();
		}
	}
	
	return IsOverlap;
}
