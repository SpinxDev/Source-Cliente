
#include "APhysXStdAfx.h"


bool APhysXPicker::Raycast(APhysXScene* pAPhysXScene, const NxRay& worldRay, APhysXRayHitInfo& hitInfo, APhysXRaycastFilterType apxRFType, float fMaxDist, APhysXU32 uGroupFilter /* = 0xffffffff */)
{
	
	if(pAPhysXScene && pAPhysXScene->GetNxScene())
	{
		
		NxScene* pNxScene = pAPhysXScene->GetNxScene();
		if(apxRFType != APX_RFTYPE_CLOTH)
		{
			// we should raycast rb actors...
			NxShapesType shapesType;
			switch(apxRFType)
			{
			
			case APX_RFTYPE_DANYMIC_RB:
				shapesType = NX_DYNAMIC_SHAPES;
				break;
			
			case APX_RFTYPE_STATIC_RB:
				shapesType = NX_STATIC_SHAPES;
				break;
			
			case APX_RFTYPE_RB:
			
			case APX_RFTYPE_ALL:
				shapesType = NX_ALL_SHAPES;
			    break;

			default:
				APHYSX_ASSERT(0);
			    break;
			}
			
			NxRaycastHit nxHit;
			NxShape* pHitShape = NULL;
			pHitShape = pNxScene->raycastClosestShape(worldRay, shapesType, nxHit, uGroupFilter, fMaxDist);
			if(pHitShape)
			{
				hitInfo.mHitPos = nxHit.worldImpact;
				hitInfo.mDist = nxHit.distance;

				hitInfo.mHitNormal = nxHit.worldNormal;
				hitInfo.mShape = pHitShape;
				hitInfo.mActor = &(pHitShape->getActor());

#if 0
				// some debug code...
				NxShapeType sType = pHitShape->getType();
				NxCollisionGroup cGroup = pHitShape->getGroup();
				NxGroupsMask gMask = pHitShape->getGroupsMask();
#endif

			}
			else
			{
				hitInfo.mDist = fMaxDist;		// this is for reference of cloth raycasting...
			}


		} // end of if (apxRFType != APX_RFTYPE_CLOTH)

		if(apxRFType == APX_RFTYPE_CLOTH || apxRFType == APX_RFTYPE_ALL)
		{
			// we should raycast cloths...
			NxCloth* pHitCloth = NULL;
			NxU32 uHitVertexID;
			NxVec3 hit, vClosestClothHit;
			NxU32 vertexId;
			NxReal sqrDistance = hitInfo.mDist * hitInfo.mDist;
			NxCloth **cloths = pNxScene->getCloths();
			for (NxU32 i = 0; i < pNxScene->getNbCloths(); i++) 
			{
				if (cloths[i]->raycast(worldRay, hit, vertexId)) 
				{
					float  curSqrDistance = (hit - worldRay.orig).magnitudeSquared();
					if(curSqrDistance < sqrDistance)
					{
						pHitCloth = cloths[i];
						uHitVertexID = vertexId;
						sqrDistance = curSqrDistance;
						vClosestClothHit = hit;
					}
				}
			}	// end of for...

			if(pHitCloth)
			{
				// this means that cloth is closer than the closest hit shape...
				hitInfo.mHitPos = vClosestClothHit;
				hitInfo.mDist = NxMath::sqrt(sqrDistance);

				hitInfo.mCloth = pHitCloth;
				hitInfo.mClothHitVertexID = uHitVertexID;

				// clear the infos of hit actor ...
				hitInfo.mHitNormal.zero();
				hitInfo.mShape = NULL;
				hitInfo.mActor = NULL;			
			}	// end of if(pHitCloth)

		} // end of if (apxRFType == APX_RFTYPE_CLOTH || apxRFType == APX_RFTYPE_ALL)

		//-------------------------------------------------------
		// revised by wenfeng, Aug. 20, 2010
		
		// more check on whether the hit object is deleted?
		if(hitInfo.mActor)
		{
			APhysXObject* pObj = static_cast<APhysXObject *>(hitInfo.mActor->userData);
			if(pObj && pObj->IsDeleted())
				return false;

			return true;
		}

		if(hitInfo.mCloth)
		{
			APhysXObject* pObj = static_cast<APhysXObject *>(hitInfo.mCloth->userData);
			if(pObj && pObj->IsDeleted())
				return false;

			return true;
		}

		return false;
		//-------------------------------------------------------

	}
	else
		return false;
}

float APhysXPicker::GetPickingRay(A3DViewport* pA3DViewPort, int iScreenX, int iScreenY, NxRay& worldRay)
{
	if(!pA3DViewPort)	return 0.0f;

	A3DVECTOR3 avNearPos((float)iScreenX, (float)iScreenY, 0.0f), avFarPos((float)iScreenX, (float)iScreenY, 1.0f);
	pA3DViewPort->InvTransform(avNearPos, avNearPos);
	pA3DViewPort->InvTransform(avFarPos, avFarPos);
	
	worldRay.orig= APhysXConverter::A2N_Vector3(avNearPos);
	worldRay.dir = APhysXConverter::A2N_Vector3(avFarPos) - worldRay.orig;
	return worldRay.dir.normalize();	

}

bool APhysXPicker::Raycast(APhysXScene* pAPhysXScene, A3DViewport* pA3DViewPort, int iScreenX, int iScreenY, APhysXRayHitInfo& hitInfo, APhysXRaycastFilterType apxRFType, APhysXU32 uGroupFilter /* = 0xffffffff */)
{
	if(pAPhysXScene && pA3DViewPort)
	{

/*
		// obsolete code...

		A3DVECTOR3 avNearPos((float)iScreenX, (float)iScreenY, 0.0f), avFarPos((float)iScreenX, (float)iScreenY, 1.0f);
		pA3DViewPort->InvTransform(avNearPos, avNearPos);
		pA3DViewPort->InvTransform(avFarPos, avFarPos);

		NxRay worldRay;
		float fMaxDist;

		worldRay.orig= APhysXConverter::A2N_Vector3(avNearPos);
		worldRay.dir = APhysXConverter::A2N_Vector3(avFarPos) - worldRay.orig;
		fMaxDist = worldRay.dir.normalize();	
*/

		NxRay worldRay;
		float fMaxDist;

		fMaxDist = GetPickingRay(pA3DViewPort, iScreenX, iScreenY, worldRay);

		return Raycast(pAPhysXScene, worldRay, hitInfo, apxRFType, fMaxDist, uGroupFilter);
	}
	else
		return false;

}


void APhysXPicker::Init(APhysXScene* pAPhysXScene, A3DViewport* pA3DViewPort)
{
	APHYSX_ASSERT(pAPhysXScene && pA3DViewPort);

	if(pAPhysXScene)
		m_pNxScene = pAPhysXScene->GetNxScene();
	if(pA3DViewPort)
		m_pA3DViewPort = pA3DViewPort;

	m_pPickedActor = NULL;
	m_pPickedCloth = NULL;
	m_uPickedClothVertexID = 0;

	m_pAnchorActor = NULL;
	m_pAnchorJoint = NULL;

	m_fPickedPosDepth = 0.0f;
}

void APhysXPicker::Release()
{

	// yeah, so that we can avoid the error introduced by the wrong order of release of 
	// APhysXEngine, Scene and Picker...
	//??? maybe we can do nothing here...

#if 0
	ReleasePicking();
#endif

	m_pNxScene = NULL;
	m_pA3DViewPort = NULL;
}

void APhysXPicker::ReleasePicking()
{
	//APHYSX_ASSERT(m_pNxScene);

	if(m_pNxScene)
	{
		// if an actor is picked
		if(m_pAnchorJoint)
		{
			m_pNxScene->releaseJoint(*m_pAnchorJoint);
			m_pAnchorJoint = NULL;
		}

		if(m_pAnchorActor)
		{
			m_pNxScene->releaseActor(*m_pAnchorActor);
			m_pAnchorActor = NULL;
		}

		m_pPickedActor = NULL;


		// if a cloth is picked...
		if(m_pPickedCloth)
		{
			m_pPickedCloth->freeVertex(m_uPickedClothVertexID);
			m_pPickedCloth = NULL;
		}
		
		m_uPickedClothVertexID = 0;
		m_fPickedPosDepth = 0.0f;
	}

}

/*

//----------------------------------------------------------------
// obsolete code...

bool APhysXPicker::ScreenPosToWorldRay(int x, int y, NxRay& worldRay)
{
	if(m_pA3DViewPort)
	{
		A3DVECTOR3 avNearPos((float)x, (float)y, 0.0f), avFarPos((float)x, (float)y, 1.0f);
		m_pA3DViewPort->InvTransform(avNearPos, avNearPos);
		m_pA3DViewPort->InvTransform(avFarPos, avFarPos);

		worldRay.orig= APhysXConverter::A2N_Vector3(avNearPos);
		worldRay.dir = APhysXConverter::A2N_Vector3(avFarPos) - worldRay.orig;
		worldRay.dir.normalize();

		return true;
	}
	else
	{
		//worldRay.orig.set(0.0f);
		//worldRay.dir.set(0.0f);
		return false;
	}

}

NxVec3 APhysXPicker::PickActorAndCloth(const NxRay& worldRay)
{
	NxVec3 vHitPos(0.0f);

	if(m_pNxScene && m_pA3DViewPort)
	{

		// firstly, we raycast the closest actors...
		NxRaycastHit raycastHit;
		NxShape* pHitShape = NULL;
		pHitShape = m_pNxScene->raycastClosestShape(worldRay, NX_DYNAMIC_SHAPES, raycastHit);

		// then, we raycast all the cloths in the scene...
		NxCloth* pHitCloth = NULL;
		NxU32 uHitVertexID;
		NxVec3 hit, vClosestClothHit;
		NxU32 vertexId;
		NxReal sqrDistance = NX_MAX_REAL;
		NxCloth **cloths = m_pNxScene->getCloths();
		for (NxU32 i = 0; i < m_pNxScene->getNbCloths(); i++) 
		{
			if (cloths[i]->raycast(worldRay, hit, vertexId)) 
			{
				float  curSqrDistance = (hit - worldRay.orig).magnitudeSquared();
				if(curSqrDistance < sqrDistance)
				{
					pHitCloth = cloths[i];
					uHitVertexID = vertexId;
					sqrDistance = curSqrDistance;

					vClosestClothHit = hit;
				}
			}
		}

		// now, we should judge whether the cloth or the actor is closer...
		if(pHitShape)
		{
			if(pHitCloth && NxMath::sqrt(sqrDistance) < raycastHit.distance)
			{
					// cloth is closer...

					m_pPickedCloth = pHitCloth;
					m_uPickedClothVertexID = vertexId;

					vHitPos = vClosestClothHit;
			}
			else
			{
				// actor is closer...
				// and we can only pick up the dynamic actor!
				NxActor* pHitActor = &pHitShape->getActor();
				if(pHitActor->isDynamic() && !pHitActor->readBodyFlag(NX_BF_KINEMATIC))
				{
					m_pPickedActor = pHitActor;
					vHitPos = raycastHit.worldImpact;
				}
			}
		}
		else
		{

			if(pHitCloth)
			{
				// we only hit the cloth...
				m_pPickedCloth = pHitCloth;
				m_uPickedClothVertexID = vertexId;

				vHitPos = vClosestClothHit;
			}
		}

		// record the picked point Z (depth) value...
		if(pHitShape || pHitCloth)	
		{
			A3DVECTOR3 vScreenSpaceHitPos = APhysXConverter::N2A_Vector3(vHitPos);
			m_pA3DViewPort->Transform(vScreenSpaceHitPos, vScreenSpaceHitPos);
			m_fPickedPosDepth = vScreenSpaceHitPos.z;
		}
	
	}

	return vHitPos;

}

*/

bool APhysXPicker::OnMouseButtonDown(int x, int y)
{
	APHYSX_ASSERT(m_pNxScene);

	bool bPicked = false;

	if(m_pNxScene)
	{
/*
//----------------------------------------------------------------
// obsolete code...

		NxRay ray;
		if(ScreenPosToWorldRay(x, y, ray))
		{

			NxVec3 vHitPos = PickActorAndCloth(ray);
			
*/
		APhysXRayHitInfo hitInfo;
		APhysXScene* pAPhysXScene = (APhysXScene*)(m_pNxScene->userData);
		if(Raycast(pAPhysXScene, m_pA3DViewPort, x, y, hitInfo))
		{
			// fill some members from hitInfo...
			m_pPickedActor = hitInfo.mActor;
			m_pPickedCloth = hitInfo.mCloth;
			m_uPickedClothVertexID = hitInfo.mClothHitVertexID;
			A3DVECTOR3 vScreenSpaceHitPos = APhysXConverter::N2A_Vector3(hitInfo.mHitPos);
			m_pA3DViewPort->Transform(vScreenSpaceHitPos, vScreenSpaceHitPos);
			m_fPickedPosDepth = vScreenSpaceHitPos.z;
			
			// post-raycast logic...
			if(m_pPickedActor && m_pPickedActor->isDynamic() && !m_pPickedActor->readBodyFlag(NX_BF_KINEMATIC))
			{
				// create an anchor actor and an anchor joint...
				// note:
				//    the anchor actor should be kinematic and its' collision with other actors
				// should be turned off...

				bPicked = true;

				NxSphereShapeDesc sphereShapeDesc;
				sphereShapeDesc.radius = 0.1f;
				NxBodyDesc bodyDesc;
				bodyDesc.flags |= NX_BF_KINEMATIC;

				NxActorDesc actorDesc;
				actorDesc.shapes.push_back(&sphereShapeDesc);
				actorDesc.body = &bodyDesc;
				actorDesc.density = 1.0f;
				actorDesc.globalPose.t = hitInfo.mHitPos;
				actorDesc.flags |= NX_AF_DISABLE_COLLISION;

				m_pAnchorActor = m_pNxScene->createActor(actorDesc);
			
				if(m_pAnchorActor)
				{
					// then, we create the joint...
					NxDistanceJointDesc distJointDesc;
					distJointDesc.flags = NX_DJF_MAX_DISTANCE_ENABLED | NX_DJF_MIN_DISTANCE_ENABLED | NX_DJF_SPRING_ENABLED;
					distJointDesc.spring.spring = 2000.0f;
					distJointDesc.spring.damper = 1.0f;
					distJointDesc.minDistance = 0.0f;
					distJointDesc.maxDistance = 0.0f;
					distJointDesc.actor[0] = m_pAnchorActor;
					distJointDesc.actor[1] = m_pPickedActor;

#if 0
					distJointDesc.setGlobalAnchor(vHitPos);
#else

					distJointDesc.localAnchor[0].set(0.0f);

					// compute the anchor pos in actor[1]'s local frame...
					NxVec3 vLocalAnchor;
					m_pPickedActor->getGlobalPose().multiplyByInverseRT(hitInfo.mHitPos, vLocalAnchor);
					distJointDesc.localAnchor[1].set(vLocalAnchor);
#endif

					m_pAnchorJoint = m_pNxScene->createJoint(distJointDesc);
				}


			}
			else if(m_pPickedCloth)
			{
				// attach the picked vertex to vHitPos...

				m_pPickedCloth->attachVertexToGlobalPosition(m_uPickedClothVertexID, hitInfo.mHitPos);
				bPicked = true;

			}


		}
	}

	return bPicked;
}

void APhysXPicker::OnMouseButtonUp(int x, int y)
{
	ReleasePicking();
}

void APhysXPicker::OnMouseMove(int x, int y)
{
	APHYSX_ASSERT(m_pNxScene && m_pA3DViewPort);

	if(m_pNxScene && m_pA3DViewPort)
	{
		if(m_pPickedActor || m_pPickedCloth)
		{
			// compute the new anchor position...
			A3DVECTOR3 avAnchorPos((float)x, (float)y, m_fPickedPosDepth);
			m_pA3DViewPort->InvTransform(avAnchorPos, avAnchorPos);

			NxVec3 vAnchorPos = APhysXConverter::A2N_Vector3(avAnchorPos);

			if(m_pAnchorActor)
			{
				m_pAnchorActor->moveGlobalPosition(vAnchorPos);
			}
			else if(m_pPickedCloth)
			{
				m_pPickedCloth->attachVertexToGlobalPosition(m_uPickedClothVertexID, vAnchorPos);
			}

		}

	}

}

bool APhysXPicker::Invalidate(const APhysXObject* pObject)
{
	if(!(m_pPickedActor || m_pPickedCloth)) return false;

	if(!pObject) return false;

	if(pObject->IsRigidBody())
	{
		APhysXRigidBodyObject* pRBObject = (APhysXRigidBodyObject*)pObject;
		
		for(int i=0; i<pRBObject->GetNxActorNum(); i++)
		{
			if(Invalidate(pRBObject->GetNxActor(i)))
				return true;
		}

	}
	else if(pObject->IsCloth())
	{
		APhysXClothObject* pClothObject = (APhysXClothObject*)pObject;
		return Invalidate(pClothObject->GetNxCloth());
	}
	else if(pObject->IsCompound())
	{
		// note: here we use the recursive call for each component object...
		APhysXCompoundObject* pCompoundObject = (APhysXCompoundObject*)pObject;
		for(int i=0; i<pCompoundObject->GetPhysXObjectComponentNum(); i++)
		{
			APhysXObject* pComponentObject = pCompoundObject->GetPhysXObjectComponent(i);

			if(Invalidate(pComponentObject))
				return true;
		}
	}

	return false;
}

bool APhysXPicker::Invalidate(const NxActor* pActor)
{
	if(!m_pPickedActor) return false;

	if(pActor == m_pPickedActor)
	{
		ReleasePicking();
		return true;
	}
	else
		return false;

}

bool APhysXPicker::Invalidate(const NxCloth* pCloth)
{
	if(!m_pPickedCloth) return false;

	if(pCloth == m_pPickedCloth)
	{
		ReleasePicking();
		return true;
	}
	else
		return false;

}

