
#include "APhysXStdAfx.h"

bool APhysXClothAttachRBConnector::CreateConnector(const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc)
{
	APHYSX_ASSERT(apxObjConnectorInstanceDesc.mPhysXConnectorDesc->GetConnectorType() == APX_CNTTYPE_CLOTH_ATTACH_RB);

	APhysXClothAttachRBConnectorDesc& apxClothAttachRBDesc = (APhysXClothAttachRBConnectorDesc& )(*apxObjConnectorInstanceDesc.mPhysXConnectorDesc);

	APhysXClothObject* pClothObject = NULL;
	APhysXRigidBodyObject* pRBObject = NULL;
	
	if(m_pAPhysXObject1->IsCloth() && m_pAPhysXObject2->IsRigidBody())
	{
		pClothObject = (APhysXClothObject *)m_pAPhysXObject1;
		pRBObject = (APhysXRigidBodyObject *)m_pAPhysXObject2;
	}
	else if(m_pAPhysXObject2->IsCloth() && m_pAPhysXObject1->IsRigidBody())
	{
		pClothObject = (APhysXClothObject *)m_pAPhysXObject2;
		pRBObject = (APhysXRigidBodyObject *)m_pAPhysXObject1;
	}
	else
	{
		// invalid case...
		return false;
	}

	
	m_pNxCloth = pClothObject->GetNxCloth();
	APHYSX_ASSERT(m_pNxCloth);

	if(apxClothAttachRBDesc.mOnlyAttachSpecifiedObject)
	{
		if(apxClothAttachRBDesc.mAttachedNxActorName)
		{
			// if we specify the actor's name
			NxActor* pActor = pRBObject->GetNxActor(apxClothAttachRBDesc.mAttachedNxActorName);
			APHYSX_ASSERT(pActor);
			if(!pActor) return false;

			for(int i = 0; i < pActor->getNbShapes(); i++)
			{
				NxShape* pAttachedShape = pActor->getShapes()[i];		
				m_pNxCloth->attachToShape(pAttachedShape, apxClothAttachRBDesc.mAttachmentFlags);

			}
		}
		else
		{
			// else, we'll try attach the cloth to all NxActors of the RB object
			for(int i=0; i<pRBObject->GetNxActorNum(); i++)
			{
				NxActor* pActor = pRBObject->GetNxActor(i);
				for(int i = 0; i < pActor->getNbShapes(); i++)
				{
					NxShape* pAttachedShape = pActor->getShapes()[i];		
					m_pNxCloth->attachToShape(pAttachedShape, apxClothAttachRBDesc.mAttachmentFlags);

				}
			}

		}
	}
	else
	{
		m_pNxCloth->attachToCollidingShapes(apxClothAttachRBDesc.mAttachmentFlags);
	}
	
	return true;
}

void APhysXClothAttachRBConnector::ReleaseConnector()
{

	if(m_pNxCloth)
	{

	#define FORCE_RELEASE_CLOTH_ATTACHMENT
	#ifdef	FORCE_RELEASE_CLOTH_ATTACHMENT

/*
		if(m_pAttachedShape)
		{
			m_pNxCloth->detachFromShape(m_pAttachedShape);
		}
		else
*/
		{
			for(int i=0; i<m_pNxCloth->getNumberOfParticles(); i++)
			{
				m_pNxCloth->freeVertex(i);
			}
		}

	

	#else
	// no need to release the cloth attachment since no physics entities created...


	#endif

		m_pNxCloth = NULL;
		// m_pAttachedShape = NULL;
	}

}