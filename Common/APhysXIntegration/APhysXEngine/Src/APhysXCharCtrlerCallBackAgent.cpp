#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerImp.h"
#include "APhysXCharCtrlerCallBackAgent.h"

APHYSX_DEFINE_SINGLETON(APhysXCCHitReportAgent)

NxControllerAction APhysXCCHitReportAgent::onShapeHit(const NxControllerShapeHit& hit)
{
	if (0 > hit.length)
		return NX_ACTION_NONE;

	if (0 == hit.shape)
		return NX_ACTION_NONE;

	APhysXCCImp* pCCImp = static_cast<APhysXCCImp*>(hit.controller->getUserData());
	assert(0 != pCCImp);
	pCCImp->SetBlockShape(*(hit.shape));

	NxVec3 pos(float(hit.worldPos.x), float(hit.worldPos.y), float(hit.worldPos.z));
	NxActor& actor = hit.shape->getActor();
	pCCImp->UpdateAttachment(hit.dir, hit.worldNormal, pos, actor);

	bool bIsPushing = false;
	if (actor.isDynamic())
		pCCImp->ComputeHitForce(hit.dir, pos, actor, bIsPushing);

	if (0 <	gPhysXCCHitReport->GetRegisteredCount())
	{
		APhysXCCShapeHit APhysXHit;
		APhysXHit.object = static_cast<APhysXObject*>(actor.userData);
		if (0 != APhysXHit.object)
		{
			APhysXHit.worldPos.set(float(hit.worldPos.x), float(hit.worldPos.y), float(hit.worldPos.z));
			APhysXHit.controller  = pCCImp;
			APhysXHit.worldNormal = hit.worldNormal;
			APhysXHit.id	 = hit.id;
			APhysXHit.dir	 = hit.dir;
			APhysXHit.length = hit.length;
			APhysXHit.pushing = bIsPushing;	
			gPhysXCCHitReport->onShapeHit(APhysXHit);
		}
	}
	return NX_ACTION_NONE;
}

NxControllerAction APhysXCCHitReportAgent::onControllerHit(const NxControllersHit& hit)
{
	APhysXCCImp* pCCImp = static_cast<APhysXCCImp*>(hit.controller->getUserData());
	assert(0 != pCCImp);
	
	if (0 < gPhysXCCHitReport->GetRegisteredCount())
	{
		APhysXCCsHit APhysXHit;
		APhysXHit.controller = pCCImp;
		APhysXHit.other = static_cast<APhysXCCImp*>(hit.other->getUserData());
		if (0 != APhysXHit.other)
			gPhysXCCHitReport->onControllerHit(APhysXHit);
	}

	return NX_ACTION_NONE;
}

APHYSX_DEFINE_SINGLETON(APhysXCCHitReport)

int APhysXCCHitReport::GetRegisteredCount() const
{
	return m_arrElements.size();
}

void APhysXCCHitReport::onShapeHit(const APhysXCCShapeHit& hit)
{
	int nCount = m_arrElements.size();
	for(int i = 0; i < nCount; ++i)
	{
		m_arrElements[i]->onShapeHit(hit);
	}
}

void APhysXCCHitReport::onControllerHit(const APhysXCCsHit& hit)
{
	int nCount = m_arrElements.size();
	for(int i = 0; i < nCount; ++i)
	{
		m_arrElements[i]->onControllerHit(hit);
	}
}

void APhysXCCHitReport::onOverlapBegin(const APhysXCCOverlap& ol)
{
	int nCount = m_arrElements.size();
	for(int i = 0; i < nCount; ++i)
	{
		m_arrElements[i]->onOverlapBegin(ol);
	}
}

void APhysXCCHitReport::onOverlapContinue(const APhysXCCOverlap& ol)
{
	int nCount = m_arrElements.size();
	for(int i = 0; i < nCount; ++i)
	{
		m_arrElements[i]->onOverlapContinue(ol);
	}
}

void APhysXCCHitReport::onOverlapEnd(APhysXCharacterController& controller)
{
	int nCount = m_arrElements.size();
	for(int i = 0; i < nCount; ++i)
	{
		m_arrElements[i]->onOverlapEnd(controller);
	}
}
