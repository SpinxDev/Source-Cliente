
#include "APhysXStdAfx.h"


bool APhysXFluidEmitterAttachRBConnector::CreateConnector(const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc)
{
	APHYSX_ASSERT(apxObjConnectorInstanceDesc.mPhysXConnectorDesc->GetConnectorType() == APX_CNTTYPE_FLUIDEMITTER_ATTACH_RB);

	APhysXFluidEmitterAttachRBConnectorDesc& apxFluidEmitterAttachRBDesc = (APhysXFluidEmitterAttachRBConnectorDesc& )(*apxObjConnectorInstanceDesc.mPhysXConnectorDesc);

	APhysXFluidObject* pFluidObject = NULL;
	APhysXRigidBodyObject* pRBObject = NULL;

	if(m_pAPhysXObject1->IsFluid() && m_pAPhysXObject2->IsRigidBody())
	{
		pFluidObject = (APhysXFluidObject *)m_pAPhysXObject1;
		pRBObject = (APhysXRigidBodyObject *)m_pAPhysXObject2;
	}
	else if(m_pAPhysXObject2->IsFluid() && m_pAPhysXObject1->IsRigidBody())
	{
		pFluidObject = (APhysXFluidObject *)m_pAPhysXObject2;
		pRBObject = (APhysXRigidBodyObject *)m_pAPhysXObject1;
	}
	else
	{
		// invalid case...
		return false;
	}

	m_pNxFluid = pFluidObject->GetNxFluid();
	APHYSX_ASSERT(m_pNxFluid);

	NxFluidEmitterDesc fluidEmitterDesc;
	apxFluidEmitterAttachRBDesc.mFluidEmitterParameters.GetNxFluidEmitterDesc(fluidEmitterDesc);

	// search the actor and the shape which the emitter will attach to...
	NxActor* pActor = pRBObject->GetNxActor(apxFluidEmitterAttachRBDesc.mAttachedNxActorName);
	if(pActor)
	{
		fluidEmitterDesc.frameShape = pActor->getShapes()[0];
	}

	m_pAPhysXScene->LockWritingScene();
	m_pNxFluidEmitter = m_pNxFluid->createEmitter(fluidEmitterDesc);
	m_pAPhysXScene->UnlockWritingScene();

	if(!m_pNxFluidEmitter)
	{
		gPhysXLog->Log("Warning: Failed on creating NxFluidEmitter: create a NULL NxFluidEmitter.");
	}

	return true;

}

void APhysXFluidEmitterAttachRBConnector::ReleaseConnector()
{
	if(m_pNxFluid && m_pNxFluidEmitter)
	{
		m_pAPhysXScene->LockWritingScene();
		m_pNxFluid->releaseEmitter(*m_pNxFluidEmitter);
		m_pAPhysXScene->UnlockWritingScene();

		m_pNxFluidEmitter = NULL;
		m_pNxFluid = NULL;
	}

}
