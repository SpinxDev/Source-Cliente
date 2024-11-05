
#include "APhysXStdafx.h"
#include "APhysXCharCtrlerMgrImp.h"

//-------------------------------------------------------------------------------------------------------
// define the real global object...

APHYSX_DEFINE_SINGLETON(APhysXContactReport)


void  APhysXContactReport::onContactNotify(NxContactPair& contactPair, NxU32 events)
{

	// todo: consider that isDeletedActor[2] of contactPair...

	HandleRBHit(contactPair, events);

	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->onContactNotify(contactPair, events);
	}
}


void  APhysXContactReport::HandleRBHit(NxContactPair& contactPair, NxU32 events)
{

/*
 		about pair's sumNormalForce and sumFrictionForce

	since they both are NxVec3 type, this means that they have direction information. 
	while, we all know that "force" between two actors is interactive and force 
	from A to B is just the minus of B to A. However, in the PhysX Docs, no exact 
	words about the two data member is from 0# to #1 or otherwise...

	In my experiments, the force (in most cases, note here) is from #1 to #0!


*/
	if(events == NX_NOTIFY_ON_START_TOUCH)	
	{
		APhysXRigidBodyObject* pRBObject0 =	(APhysXRigidBodyObject*)contactPair.actors[0]->userData;
		APhysXRigidBodyObject* pRBObject1 =	(APhysXRigidBodyObject*)contactPair.actors[1]->userData;	

		if(pRBObject0 == pRBObject1 && pRBObject0 )
		{
			// handle the contact of two actors of same APhysXRigidBodyObject ...
			//??? just return ???
			return;
		}

		// compute the contact force
		NxVec3 vContactForce1to0 = contactPair.sumNormalForce + contactPair.sumFrictionForce;

		// the contact force is Zero, we just ignore this contact...
		if(vContactForce1to0.equals(NxVec3(0.0f), 0.001f))
			return;

		// compute the central contact pos
		NxContactStreamIterator csi(contactPair.stream);
		int iPointNum = 0;
		NxVec3 vCentralContactPos(0.0f);
		while(csi.goNextPair())
		{
			//user can also call getShape() and getNumPatches() here
			while(csi.goNextPatch())
			{
				//user can also call getPatchNormal() and getNumPoints() here
				const NxVec3& contactNormal = csi.getPatchNormal();

				while(csi.goNextPoint())
				{
					iPointNum++;				
					//user can also call getPoint() and getSeparation() here
					const NxVec3& contactPoint = csi.getPoint();
					vCentralContactPos += contactPoint;

				}

			}
		}
		
		vCentralContactPos /= (float)iPointNum;
		
		APhysXRBHitInfo rbHitInfo;
		rbHitInfo.mHitType = APX_RBHITTYPE_CONTACT;

		if(pRBObject0)
		{
			rbHitInfo.mHitActor = contactPair.actors[0];
			rbHitInfo.mSrcActor = contactPair.actors[1];
			rbHitInfo.mHitForce = vContactForce1to0;
			rbHitInfo.mHitPos = vCentralContactPos;

			pRBObject0->OnHit(rbHitInfo);
		}

		if(pRBObject1)
		{
			rbHitInfo.mHitActor = contactPair.actors[1];
			rbHitInfo.mSrcActor = contactPair.actors[0];
			rbHitInfo.mHitForce = -vContactForce1to0;

			rbHitInfo.mHitPos = vCentralContactPos;
			pRBObject1->OnHit(rbHitInfo);
		}


	}

}


//-------------------------------------------------------------------------------------------------------
// define the real global object...

APHYSX_DEFINE_SINGLETON(APhysXTriggerReport)

void  APhysXTriggerReport::onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{

	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->onTrigger(triggerShape, otherShape, status);
	}
}


//-------------------------------------------------------------------------------------------------------
// define the real global object...

APHYSX_DEFINE_SINGLETON(APhysXBreakReport)

void APhysXBreakReport::OnBreakNotify(APhysXBreakable* pBreakable)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnBreakNotify(pBreakable);
	}

}

void APhysXBreakReport::OnFirstBreakNotify(APhysXBreakable* pBreakable)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnFirstBreakNotify(pBreakable);
	}

}

void APhysXBreakReport::OnAllBreakNotify(APhysXBreakable* pBreakable)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnAllBreakNotify(pBreakable);
	}

}

//-------------------------------------------------------------------------------------------------------
// define the real global object...

APHYSX_DEFINE_SINGLETON(APhysXCreationDeletionReport)

void APhysXCreationDeletionReport::OnObjectCreationNotify(APhysXObject* pObject)
{
	
	PreUserObjectCreationNotify(pObject);

	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnObjectCreationNotify(pObject);
	}

	PostUserObjectCreationNotify(pObject);
}

void APhysXCreationDeletionReport::OnObjectDeletionNotify(APhysXObject* pObject)
{
	PreUserObjectDeletionNotify(pObject);

	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnObjectDeletionNotify(pObject);
	}

	PostUserObjectDeletionNotify(pObject);
}

void APhysXCreationDeletionReport::OnConnectorCreationNotify(APhysXObjectConnector* pConnector, bool bInnerConnector)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnConnectorCreationNotify(pConnector, bInnerConnector);
	}

}

void APhysXCreationDeletionReport::OnConnectorDeletionNotify(APhysXObjectConnector* pConnector, bool bInnerConnector)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnConnectorDeletionNotify(pConnector, bInnerConnector);
	}

}

void APhysXCreationDeletionReport::OnSceneCreationNotify(APhysXScene* pScene)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnSceneCreationNotify(pScene);
	}

}

void APhysXCreationDeletionReport::OnSceneDeletionNotify(APhysXScene* pScene)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnSceneDeletionNotify(pScene);
	}

}

#define APX_TERRAIN_PATCH_AABB_HEIGHT_EXTENSION 1000.0f

void APhysXCreationDeletionReport::PreUserObjectCreationNotify(APhysXObject* pObject)
{
	// first, handle some CC callback...
	APhysXCharCtrlerMgrImp* pCCMImp = APhysXCharCtrlerMgrImp::GetCCM();
	if (pCCMImp && pObject)
		pCCMImp->OnObjectChangeNotify(true, *pObject);

	if(pObject->IsStaticRB())
	{
#if 1
		// this mean that we load a new terrain patch object...
		if(pObject->IsObjectType(APX_OBJTYPE_TERRAIN_PATCH))
		{
			NxBounds3 aabbTP;
			pObject->GetAABB(aabbTP);

			// just to extend the aabb of terrain patch to construct an airspace...
			aabbTP.max.y += APX_TERRAIN_PATCH_AABB_HEIGHT_EXTENSION;

			// then, we try to wake up those sleeping object when unloading this terrain patch...
			APhysXScene* pScene = pObject->GetPhysXScene();
			for(int i=0; i<pScene->GetPhysXObjectNum(); i++)
			{
				NxBounds3 aabb;
				APhysXObject* pCurObject = pScene->GetPhysXObject(i);
				if(pCurObject->IsStaticRB()) continue;

				pCurObject->GetAABB(aabb);
				if(aabbTP.intersects(aabb))
				{
					pCurObject->WakeUp();
				}
			}


		}
#endif

	}
}

void APhysXCreationDeletionReport::PostUserObjectCreationNotify(APhysXObject* pObject)
{
	if(pObject->IsCloth())
	{
		APhysXClothObject* pClothObj = (APhysXClothObject*)pObject;
		if(pClothObj->IsConstraintsEnabled())
			pClothObj->GetPhysXScene()->IncreConstraintClothNum();
	}
	
	// notify gPhysXSubSimulationReport
	gPhysXSubSimulationReport->OnObjectCreation(pObject);
}

void APhysXCreationDeletionReport::PreUserObjectDeletionNotify(APhysXObject* pObject)
{
	// first, handle some CC callback...
	APhysXCharCtrlerMgrImp* pCCMImp = APhysXCharCtrlerMgrImp::GetCCM();
	if (pCCMImp && pObject)
		pCCMImp->OnObjectChangeNotify(false, *pObject);

	if(pObject->IsStaticRB())
	{
#if 1
		// this mean that we unload a terrain patch object...
		if(pObject->IsObjectType(APX_OBJTYPE_TERRAIN_PATCH))
		{
			NxBounds3 aabbTP;
			pObject->GetAABB(aabbTP);

			// just to extend the aabb of terrain patch to construct an airspace...
			aabbTP.max.y += APX_TERRAIN_PATCH_AABB_HEIGHT_EXTENSION;

			// then, we try to sleep those object when unloading this terrain patch...
			APhysXScene* pScene = pObject->GetPhysXScene();
			for(int i=0; i<pScene->GetPhysXObjectNum(); i++)
			{
				NxBounds3 aabb;
				APhysXObject* pCurObject = pScene->GetPhysXObject(i);
				if(pCurObject->IsStaticRB()) continue;

				pCurObject->GetAABB(aabb);
				if(aabbTP.intersects(aabb))
				{
					pCurObject->PutToSleep();
				}
			}
		}
#endif
	}

	// notify gPhysXSubSimulationReport
	gPhysXSubSimulationReport->OnObjectDeletion(pObject);
}

void APhysXCreationDeletionReport::PostUserObjectDeletionNotify(APhysXObject* pObject)
{

	if(pObject->IsCloth())
	{
		APhysXClothObject* pClothObj = (APhysXClothObject*)pObject;
		if(pClothObj->IsConstraintsEnabled())
			pClothObj->GetPhysXScene()->DecreConstraintClothNum();
	}

}

//--------------------------------------------------------------------------

APHYSX_DEFINE_SINGLETON(APhysXSubSimulationReport)

void APhysXSubSimulationReport::PreSubSimulation(float dt)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->PreSubSimulation(dt);
	}
}

void APhysXSubSimulationReport::PostSubSimulation(float dt)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->PostSubSimulation(dt);
	}
}

void APhysXSubSimulationReport::OnObjectCreation(APhysXObject* pObject)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnObjectCreation(pObject);
	}
}

void APhysXSubSimulationReport::OnObjectDeletion(APhysXObject* pObject)
{
	for(int i=0; i<m_arrElements.size(); i++)
	{
		m_arrElements[i]->OnObjectDeletion(pObject);
	}
}
