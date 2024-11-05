
#include "APhysXStdAfx.h"

//----------------------------------------------------------------------
// make the following stuff internal...

APhysXRBSubSimulationReport g_RBSubSimulationReport;

//----------------------------------------------------------------------

bool APhysXRigidBodyObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{

	if(!APhysXObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
		return false;

	m_pAPhysXScene->GetPerfStats()->ResumePerfStat("Create RB Obj");

	APhysXRigidBodyObjectDesc& rbObjDesc = *(APhysXRigidBodyObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;

	// this is for multiple PhysX objects creation case
	bool bFailedOnCreatePhysXStuff = false;

	for(int i=0; i<rbObjDesc.GetNxActorDescNum(); i++)
	{

		NxActorDesc actorDesc;
		NxBodyDesc bodyDesc;

		rbObjDesc.GetNxActorDesc(i, actorDesc);
		if(rbObjDesc.HasBody(i))
		{
			rbObjDesc.GetNxBodyDesc(i, bodyDesc);
			actorDesc.body = &bodyDesc;
		}

		// we should try to scale the shapes of the actor using the actor scale3D and global scale3D
		NxVec3 vShapeScale3D;
		vShapeScale3D.arrayMultiply(rbObjDesc.GetActorScale3D(i), apxObjInstanceDesc.mScale3D);

		// before we get the NxShapeDesc, we should update the material id to global one!
		m_pAPhysXScene->GetPerfStats()->ResumePerfStat("Get Shape Desc");
		rbObjDesc.GetNxShapeDescs(i, m_pAPhysXScene, actorDesc.shapes, vShapeScale3D);
		m_pAPhysXScene->GetPerfStats()->PausePerfStat("Get Shape Desc");

		// since we only consider the uniform scale, so here we just use actor 0#'s scale as the scale.
		if(i == 0)	m_vUniformActorScale3D = vShapeScale3D;

		//---------- compute the global pose for component NxActor instance -----------
		// scale the NxActor's local pose use the global scale3D
		NxMat34 matNxActorLocalPose = rbObjDesc.GetNxActorLocalPose(i);
		matNxActorLocalPose.t.arrayMultiply(matNxActorLocalPose.t, apxObjInstanceDesc.mScale3D);

		actorDesc.globalPose = apxObjInstanceDesc.mGlobalPose * matNxActorLocalPose;

		//---------- set the actor's dominance group -----------
		if(actorDesc.body)	
		{
			// only handle the case of dynamic actors...
			actorDesc.dominanceGroup = rbObjDesc.GetDominanceGroup();
		}

#if APHYSX_SUPPORT_COMPARTMENT

		//--------------------------------------------------------------
		// compartment support, Wenfeng, April. 15, 2009

		if(rbObjDesc.mSimInCompartment) 
			actorDesc.compartment = m_pAPhysXScene->GetRBCompartment();

#endif

		//----------Give the ObjectDesc a chance to change the actorDesc -----------
		OnCreateNxActorDesc(&actorDesc, i);

		// verify the actorDesc is valid...
		if(!actorDesc.isValid())
		{
			gPhysXLog->Log("Warning: The Actor(%s) has the invalid NxActorDesc, so its creation failed.", actorDesc.name);
			bFailedOnCreatePhysXStuff = true;
			break;
		}

		//-----------------------------------------------------------------------------
		//bool bNxSceneWritable;
		//while(!(bNxSceneWritable = m_pAPhysXScene->GetNxScene()->isWritable()));
	
		//-----------------------------------------------------------------------------
		
		//-----------------------------------------------------------------------------
		// clamp the max solverIterationCount
		#define MAX_SOLVERITERATIONCOUNT 200
		//#define MAX_SOLVERITERATIONCOUNT 10
		bodyDesc.solverIterationCount = NX_MIN(bodyDesc.solverIterationCount, MAX_SOLVERITERATIONCOUNT);
		//-----------------------------------------------------------------------------
		m_pAPhysXScene->GetPerfStats()->ResumePerfStat("Create RB Lock");
		m_pAPhysXScene->LockWritingScene();
		m_pAPhysXScene->GetPerfStats()->PausePerfStat("Create RB Lock");

		// make sure that NxScene is currently writable...
		APHYSX_ASSERT(m_pAPhysXScene->GetNxScene()->isWritable());

		m_pAPhysXScene->GetPerfStats()->ResumePerfStat("Create Actor");
		NxActor* pNxActor = m_pAPhysXScene->GetNxScene()->createActor(actorDesc);
		m_pAPhysXScene->GetPerfStats()->PausePerfStat("Create Actor");

		m_pAPhysXScene->UnlockWritingScene();

		// test code...
		// float fMass = pNxActor->getMass();

		// we have to release the created shapes manually
		APhysXShapeDescManager::ReleaseNxShapeDescs(actorDesc.shapes);

		if(pNxActor)
		{
			m_pAPhysXScene->LockWritingScene();
			pNxActor->userData = this;
			
			// force to use APhysXScene's GetRBForceFieldMaterial()...
			pNxActor->setForceFieldMaterial(pAPhysXScene->GetRBForceFieldMaterial());

			m_pAPhysXScene->UnlockWritingScene();

			m_arrNxActors.push_back(pNxActor);

			//---------------------------------------------------------
			// revised by wenfeng, Jun 25, 2009
			// the initial kinematic actor will be pushed to the fixed actor array
			if(pNxActor->isDynamic() && pNxActor->readBodyFlag(NX_BF_KINEMATIC))
				m_arrFixedActors.push_back(pNxActor);

		}
		else
		{
			gPhysXLog->Log("Warning: Failed on creating NxActor: create a NUll NxActor.");
			bFailedOnCreatePhysXStuff = true;
			break;
		}

	}

	if(bFailedOnCreatePhysXStuff)
	{
		OnReleaseBasicPhysXStuff();

		m_pAPhysXScene->GetPerfStats()->PausePerfStat("Create RB Obj");
		
		return false;
	}
	else
	{
		// the default motion mode is dynamic mode...
		m_apxRBDrivenMode = APX_RBDRIVENMODE_PHYSICS;

		ComputeMass();
		m_arrPairFlags.clear();

		m_pAPhysXScene->GetPerfStats()->PausePerfStat("Create RB Obj");
		
		return true;
	}

}

void APhysXRigidBodyObject::OnReleaseBasicPhysXStuff()
{
	if(m_pAPhysXScene)
	{
		for(int i=0; i<m_arrNxActors.size(); i++)
		{
			NxActor* pNxActor = m_arrNxActors[i];
			
			m_pAPhysXScene->LockWritingScene();
			
			// to avoid the async release of actors...
			pNxActor->userData = NULL;
			m_pAPhysXScene->GetNxScene()->releaseActor(*pNxActor);
			
			m_pAPhysXScene->UnlockWritingScene();
		}

		m_arrNxActors.clear();

	}
	m_arrPairFlags.clear();
	
	APhysXObject::OnReleaseBasicPhysXStuff();
}

NxActor* APhysXRigidBodyObject::GetNxActor(const char* szActorName) const
{
	// for most cases, rigidbody object has only one NxActor...
	
	if(!szActorName)	// szActorName == NULL...
	{
		if(m_arrNxActors.size() > 0)
			return m_arrNxActors[0];
		else
			return NULL;
	}

	//---------------------------------------------------------------
	// make sure that parameters passed to strcmp(...) not NULL...
	//---------------------------------------------------------------
	for(int i=0; i<m_arrNxActors.size(); i++)
	{
		if(m_arrNxActors[i])
		{
			const char* szCurActorName = m_arrNxActors[i]->getName();
			
			//--------------------------------------------------------------------------
			// obsolete code???... it will affect efficiency dramatically...
			// const char* szGName = gGlobalStringManager->GetString(szCurActorName);

			if(szCurActorName &&  strcmp(szActorName, szCurActorName) == 0 )
			{
				return m_arrNxActors[i];
			}

		}
	}

	return NULL;
}

int APhysXRigidBodyObject::GetNxActorID(NxActor* pActor) const
{
	if(!pActor) return -1;

	for(int i=0; i<m_arrNxActors.size(); i++)
	{
		if(pActor == m_arrNxActors[i])
			return i;
	}
	return -1;
}

void APhysXRigidBodyObject::ComputeMass()
{

	float fTotalMass = 0.0f;
	for(int i=0; i<m_arrNxActors.size(); i++)
	{
		if(m_arrNxActors[i]->isDynamic())
		{
			fTotalMass += m_arrNxActors[i]->getMass();
		}
		else
		{
			fTotalMass = NX_MAX_F32;
			break;
		}
	}

	m_fMass = fTotalMass;

}

inline void APhysXRigidBodyObject::ResetFixedActors()
{
	// force to set those fixed actors to kinematic...
	for(int i=0; i<m_arrFixedActors.size(); i++)
	{
		if(m_arrNxActors[i]->isDynamic())
		{
			m_arrFixedActors[i]->raiseBodyFlag(NX_BF_KINEMATIC);
		}
	}

	//??? some other stuff to reset...
}

void APhysXRigidBodyObject::EnableKinematic(bool bEnable)
{

	if(m_pAPhysXScene)
	{
		/*
		if(	bEnable && m_apxRBMotionMode == APX_RBMMODE_KINEMATIC ||
			!bEnable && m_apxRBMotionMode == APX_RBMMODE_DYNAMIC	)
			return;
		*/

		m_pAPhysXScene->LockWritingScene();

		for(int i = 0; i<m_arrNxActors.size(); i++)
		{
			// only handle the non-static NxActors
			if(m_arrNxActors[i]->isDynamic())
			{
				if(bEnable)
					m_arrNxActors[i]->raiseBodyFlag(NX_BF_KINEMATIC);
				else
					m_arrNxActors[i]->clearBodyFlag(NX_BF_KINEMATIC);
			}
		}

		if(!bEnable)
		{
			ResetFixedActors();
		}

		m_pAPhysXScene->UnlockWritingScene();

		// m_apxRBMotionMode = bEnable? APX_RBMMODE_KINEMATIC : APX_RBMMODE_DYNAMIC;

	}
}

void APhysXRigidBodyObject::EnableKinematic(bool bEnable, APhysXArray<NxActor* >& arrExceptActors)
{
	EnableKinematic(bEnable);

	if(!arrExceptActors.size()) return;

	if(m_pAPhysXScene)
	{
		m_pAPhysXScene->LockWritingScene();

		for(int i=0; i<arrExceptActors.size(); i++)
		{
			// only handle the non-static NxActors
			if(arrExceptActors[i]->isDynamic())
			{
				if(bEnable)
					arrExceptActors[i]->clearBodyFlag(NX_BF_KINEMATIC);
				else
					arrExceptActors[i]->raiseBodyFlag(NX_BF_KINEMATIC);
			}

		}

		ResetFixedActors();

		m_pAPhysXScene->UnlockWritingScene();

		// m_apxRBMotionMode = APX_RBMMODE_PARTIAL_DYNAMIC;
	}

}


void APhysXRigidBodyObject::SetDrivenMode(APhysXRBDrivenMode apxRBDrivenMode, APhysXArray<NxActor* >* pActorsDrivenByPhysics /* = NULL */)
{
	if(apxRBDrivenMode == m_apxRBDrivenMode && apxRBDrivenMode != APX_RBDRIVENMODE_MIX) return;

	m_apxRBDrivenMode = apxRBDrivenMode;

	if(m_apxRBDrivenMode == APX_RBDRIVENMODE_ANIMATION)
	{
		EnableKinematic();
	}
	else if(m_apxRBDrivenMode == APX_RBDRIVENMODE_PHYSICS)
	{
		EnableKinematic(false);
	}
	else if(m_apxRBDrivenMode == APX_RBDRIVENMODE_MIX)
	{
		APHYSX_ASSERT(pActorsDrivenByPhysics);

		EnableKinematic(true, *pActorsDrivenByPhysics);
	}

}


void APhysXRigidBodyObject::EnableGravity(bool bEnable)
{
	if(m_pAPhysXScene)
	{
		m_pAPhysXScene->LockWritingScene();

		for(int i = 0; i<m_arrNxActors.size(); i++)
		{
			// only handle the non-static NxActors
			if(m_arrNxActors[i]->isDynamic())
			{
				if(bEnable)
					m_arrNxActors[i]->clearBodyFlag(NX_BF_DISABLE_GRAVITY);
				else
					m_arrNxActors[i]->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
			}
		}

		m_pAPhysXScene->UnlockWritingScene();

	}

}

void APhysXRigidBodyObject::ExeSetPose(const NxMat34& globalPose)
{
	m_pAPhysXScene->LockWritingScene();

	for(int i = 0; i<m_arrNxActors.size(); i++)
	{
		if(m_arrNxActors[i]->isDynamic())
		{
			if(m_arrNxActors[i]->readBodyFlag(NX_BF_KINEMATIC))
				m_arrNxActors[i]->moveGlobalPose(globalPose);
			
			else
				m_arrNxActors[i]->setGlobalPose(globalPose);
		}

		// for static NxActors, we just ignore this function
	}

	m_pAPhysXScene->UnlockWritingScene();
}

void APhysXRigidBodyObject::SetPose(const NxMat34& globalPose)
{

	if(m_pAPhysXScene)
	{
		if(m_pAPhysXScene->IsSubSimulationEnabled())
		{
			NxMat34 mtCurPose = GetPose();
			m_vSubSimCurPos = mtCurPose.t;
			mtCurPose.M.toQuat(m_qSubSimCurRot);
			
			m_vSubSimTargetPos = globalPose.t;
			globalPose.M.toQuat(m_qSubSimTargetRot);

			m_fAccumulatedSubSimDt = 0.0f;

			// register this RB Object
			g_RBSubSimulationReport.Register(this);
		}
		else
		{
			ExeSetPose(globalPose);

		}
	}

}

NxMat34 APhysXRigidBodyObject::GetPose() const
{
	if(m_arrNxActors.size())
	{
		return m_arrNxActors[0]->getGlobalPose();
	}
	else
	{
		NxMat34 matID;
		return matID;
	}

}

void APhysXRigidBodyObject::GetAABB(NxBounds3& aabb) const
{
	aabb.setEmpty();

	if(m_pAPhysXScene)
	{
		for(int i = 0; i<m_arrNxActors.size(); i++)
		{
			NxBounds3 aabbActor;
			APhysXUtilLib::GetActorAABB(m_arrNxActors[i], aabbActor);

			aabb.combine(aabbActor);
		}

	}
}

void APhysXRigidBodyObject::SetLinearVelocity(const NxVec3& vLinVel)
{
	if(m_pAPhysXScene)
	{
		m_pAPhysXScene->LockWritingScene();

		for(int i = 0; i<m_arrNxActors.size(); i++)
		{
			if(m_arrNxActors[i]->isDynamic())
			{
				m_arrNxActors[i]->setLinearVelocity(vLinVel);
			}

			// for static NxActors, we just ignore this function
		}

		m_pAPhysXScene->UnlockWritingScene();

	}

}

NxVec3 APhysXRigidBodyObject::GetLinearVelocity() const
{
	if(m_arrNxActors.size())
	{
		return m_arrNxActors[0]->getLinearVelocity();
	}
	else
	{
		return NxVec3(0.0f);
	}

}

void APhysXRigidBodyObject::SetAngularVelocity(const NxVec3& vAngVel)
{
	if(m_pAPhysXScene)
	{
		m_pAPhysXScene->LockWritingScene();

		for(int i = 0; i<m_arrNxActors.size(); i++)
		{
			if(m_arrNxActors[i]->isDynamic())
			{
				m_arrNxActors[i]->setAngularVelocity(vAngVel);
			}

			// for static NxActors, we just ignore this function
		}

		m_pAPhysXScene->UnlockWritingScene();

	}

}

NxVec3 APhysXRigidBodyObject::GetAngularVelocity() const
{
	if(m_arrNxActors.size())
	{
		return m_arrNxActors[0]->getAngularVelocity();
	}
	else
	{
		return NxVec3(0.0f);
	}

}


void APhysXRigidBodyObject::SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef)
{
	APhysXObject::SetCollisionChannel(apxCollisionChannel, bChangeChannelRef);
	
	if(m_pAPhysXScene && m_arrNxActors.size())
	{
		NxGroupsMask groupMask = m_pAPhysXScene->GetCollisionChannelManager()->GetChannelGroupMask(apxCollisionChannel);

		m_pAPhysXScene->LockWritingScene();
		
		for(int i = 0; i<m_arrNxActors.size(); i++)
		{
			NxActor* pNxActor = m_arrNxActors[i];
			for(int j=0; j<pNxActor->getNbShapes(); j++)
			{
				NxShape* pCurShape = pNxActor->getShapes()[j];
				pCurShape->setGroupsMask(groupMask);
			}
		}

		m_pAPhysXScene->UnlockWritingScene();
	}
}

void APhysXRigidBodyObject::SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup)
{
	APhysXObject::SetCollisionGroup(apxCollisionGroup);

	if(m_pAPhysXScene && m_arrNxActors.size())
	{
		m_pAPhysXScene->LockWritingScene();
		
		for(int i = 0; i<m_arrNxActors.size(); i++)
		{
			NxActor* pNxActor = m_arrNxActors[i];
			for(int j=0; j<pNxActor->getNbShapes(); j++)
			{
				NxShape* pCurShape = pNxActor->getShapes()[j];
				pCurShape->setGroup(apxCollisionGroup);
			}
		}

		m_pAPhysXScene->UnlockWritingScene();
	}

}

bool APhysXRigidBodyObject::IsSelfCollisionEnable() const
{
	if (0 < m_arrPairFlags.size())
		return false;
	return true;
}

void APhysXRigidBodyObject::EnableSelfCollision(bool bEnableCollision)
{
	NxScene* pScene = m_pAPhysXScene->GetNxScene();
	if (0 == pScene)
		return;

	int nSize = m_arrPairFlags.size();
	if (bEnableCollision)
	{
		if (0 < nSize)
		{
			m_pAPhysXScene->LockWritingScene();

			for (int i = 0; i < nSize; ++i)
				pScene->setActorPairFlags(*(m_arrPairFlags[i].mpActor0), *(m_arrPairFlags[i].mpActor1), m_arrPairFlags[i].mFlags);

			m_pAPhysXScene->UnlockWritingScene();
			m_arrPairFlags.clear();
		}
	}
	else
	{
		if (0 == nSize)
		{
			ActorPair ap;
			m_pAPhysXScene->LockWritingScene();

			int nbActors = m_arrNxActors.size();
			for (int i = 0; i < nbActors; ++i)
			{
				for (int j = i + 1; j < nbActors; ++j)
				{
					ap.mpActor0 = m_arrNxActors[i];
					ap.mpActor1 = m_arrNxActors[j];
					ap.mFlags = pScene->getActorPairFlags(*(ap.mpActor0), *(ap.mpActor1));
					m_arrPairFlags.push_back(ap);

					pScene->setActorPairFlags(*(ap.mpActor0), *(ap.mpActor1), NX_IGNORE_PAIR);
				}
			}
			m_pAPhysXScene->UnlockWritingScene();
		}
	}
}

void APhysXRigidBodyObject::OnHit(const APhysXRBHitInfo& apxRBHitInfo)
{
	m_arrRBHitInfo.push_back(apxRBHitInfo);
}

void APhysXRigidBodyObject::RemoveJoint(const char* szJointName)
{
	APhysXObjectConnector* pJoint = GetInnerConnector(szJointName);

	if(pJoint)
	{
		m_pAPhysXScene->LockWritingScene();

		RemoveInnerConnector(pJoint);

		m_pAPhysXScene->UnlockWritingScene();

	}
}

NxActor* APhysXRigidBodyObject::AddForce(const NxRay& forceRay, float fForceMagnitude, float fMaxDist /* = NX_MAX_F32 */, NxForceMode forceMode /* = NX_FORCE */, float fMaxVelChange /* = NX_MAX_F32 */)
{
	APhysXRayHitInfo hitInfo;

	// Note: to be revised! Jan. 12, 2010
	// we should filter out the Attacher-Type RB Object...
	APhysXU32 uAttacherGroup = ~((1 << APX_COLLISION_GROUP_ATTACHER) | (1 << APX_COLLISION_GROUP_CLOTH_ATTACHER ));
	if(APhysXPicker::Raycast(m_pAPhysXScene, forceRay, hitInfo, APX_RFTYPE_DANYMIC_RB, fMaxDist, uAttacherGroup) )
	{
		APHYSX_ASSERT(hitInfo.mActor);

		if(hitInfo.mActor->userData == this)		// so, the force ray do hit my actor...
		{
			
			APhysXRBHitInfo rbHitInfo;
			rbHitInfo.mHitActor = hitInfo.mActor;
			rbHitInfo.mHitForce =forceRay.dir * fForceMagnitude;
			rbHitInfo.mHitPos = hitInfo.mHitPos;
			rbHitInfo.mHitForceMode = forceMode;
			rbHitInfo.mMaxVelChange = fMaxVelChange;

			OnHit(rbHitInfo);

			return rbHitInfo.mHitActor;

		}

	}

	return 0;

}

//-----------------------------------------------
// this class is just used internally...
class APhysXSphereSweepRBObjectHitReport : public NxUserEntityReport<NxSweepQueryHit>
{

public:
	APhysXSphereSweepRBObjectHitReport():m_pRBObject(NULL)
	{
		// reset hit info...
		m_HitInfo.t = 1.1f;
		m_HitInfo.hitShape = NULL;
	}

	~APhysXSphereSweepRBObjectHitReport()
	{

	}

	void SetRBObject(const APhysXRigidBodyObject* pRBObject) 
	{ 
		m_pRBObject = pRBObject;
		
		// reset hit info...
		m_HitInfo.t = 1.1f;
		m_HitInfo.hitShape = NULL;
	};

	const NxSweepQueryHit& GetHitInfo() const { return m_HitInfo;}

	virtual bool  onEvent (NxU32 nbEntities, NxSweepQueryHit *entities)
	{
		for(int i=0; i<nbEntities; ++i)
		{
			if(entities[i].hitShape)
			{
				// check whether hit m_pRBObject...
				NxActor& hitActor = entities[i].hitShape->getActor();
				if((const void*)(hitActor.userData) == m_pRBObject && entities[i].t < m_HitInfo.t)
				{
					m_HitInfo = entities[i];
				}
			}
		}

		return true;
	}

protected:

	const APhysXRigidBodyObject* m_pRBObject;
	NxSweepQueryHit m_HitInfo;
};

static APhysXSphereSweepRBObjectHitReport s_SphereSweepRBObjectHitReport;

NxActor* APhysXRigidBodyObject::AddSweepForce(const NxRay& forceRay, float fForceMagnitude, float fSweepRadius /*= 0.01f*/, float fMaxDist /*= NX_MAX_F32*/, NxForceMode forceMode /*= NX_FORCE*/, float fMaxVelChange /*= NX_MAX_F32*/)
{

	s_SphereSweepRBObjectHitReport.SetRBObject(this);

	NxSegment seg(forceRay.orig, forceRay.orig);
	NxCapsule swpCapsule(seg, fSweepRadius);

	// we should filter out the Attacher-Type RB Object...
	APhysXU32 uAttacherGroup = ~((1 << APX_COLLISION_GROUP_ATTACHER) | (1 << APX_COLLISION_GROUP_CLOTH_ATTACHER ));

	NxU32 uHit = m_pAPhysXScene->GetNxScene()->linearCapsuleSweep(swpCapsule, fMaxDist * forceRay.dir, NX_SF_DYNAMICS | NX_SF_ALL_HITS , NULL, 0, NULL, &s_SphereSweepRBObjectHitReport, uAttacherGroup);

	if(uHit > 0 )
	{
		const NxSweepQueryHit& sqHit = s_SphereSweepRBObjectHitReport.GetHitInfo();

		if(sqHit.t <= 1.0f)		// so, the force ray do hit my actor...
		{

			APhysXRBHitInfo rbHitInfo;
			rbHitInfo.mHitActor = &(sqHit.hitShape->getActor());
			rbHitInfo.mHitForce = forceRay.dir * fForceMagnitude;
			rbHitInfo.mHitPos = sqHit.point;
			rbHitInfo.mHitForceMode = forceMode;
			rbHitInfo.mMaxVelChange = fMaxVelChange;

			OnHit(rbHitInfo);

			return rbHitInfo.mHitActor;

		}

	}

	return 0;
}

NxActor* APhysXRigidBodyObject::CollideDetection(const NxRay& forceRay, float fSweepRadius /*= 0.01f*/, float fMaxDist /*= NX_MAX_F32*/, APhysXRayHitInfo* pOutResult /*= 0*/) const
{
	NxActor* pHitActor = 0;

	// we should filter out the Attacher-Type RB Object...
	APhysXU32 uAttacherGroup = ~((1 << APX_COLLISION_GROUP_ATTACHER) | (1 << APX_COLLISION_GROUP_CLOTH_ATTACHER ));

	if (NxMath::equals(fSweepRadius, 0.0f, APHYSX_FLOAT_EPSILON))
	{
		APhysXRayHitInfo hitInfo;
		if (APhysXPicker::Raycast(m_pAPhysXScene, forceRay, hitInfo, APX_RFTYPE_DANYMIC_RB, fMaxDist, uAttacherGroup) )
		{
			APHYSX_ASSERT(hitInfo.mActor);
			if((const void*)(hitInfo.mActor->userData) == this)
			{
				pHitActor = hitInfo.mActor;
				if (0 != pOutResult)
					*pOutResult = hitInfo;
			}
		}
	}
	else
	{
		s_SphereSweepRBObjectHitReport.SetRBObject(this);
		NxSegment seg(forceRay.orig, forceRay.orig);
		NxCapsule swpCapsule(seg, fSweepRadius);

		NxU32 uHit = m_pAPhysXScene->GetNxScene()->linearCapsuleSweep(swpCapsule, fMaxDist * forceRay.dir, NX_SF_DYNAMICS | NX_SF_ALL_HITS , NULL, 0, NULL, &s_SphereSweepRBObjectHitReport, uAttacherGroup);
		if (uHit > 0)
		{
			const NxSweepQueryHit& sqHit = s_SphereSweepRBObjectHitReport.GetHitInfo();
			if (!(sqHit.t > 1.0f))
			{
				pHitActor = &(sqHit.hitShape->getActor());
				if (0 != pOutResult)
				{
					pOutResult->mHitPos = sqHit.point;
					pOutResult->mDist = sqHit.t * fMaxDist;
					pOutResult->mHitNormal = sqHit.normal;
					pOutResult->mShape = sqHit.hitShape;
					pOutResult->mActor = pHitActor;
				}
			}
		}
	}
	return pHitActor;
}

bool APhysXRigidBodyObject::AddForce(const NxVec3& vForce, NxForceMode forceMode /* = NX_FORCE */, float fMaxVelChange /* = NX_MAX_F32 */)
{
	
	// obviously, the following is a not good solution for case of breakable object...
	// we should call OnHit() function via common handling of AddFoce
	/*
	
	if(m_pAPhysXScene && m_arrNxActors.size())
	{
		m_pAPhysXScene->LockWritingScene();

		// NxVec3 vForceDir = vForce;
		// float fForceMag = vForceDir.normalize();

		for(int i = 0; i<m_arrNxActors.size(); i++)	
		{
			if(m_arrNxActors[i]->isDynamic())
			{

				if(	forceMode == NX_FORCE		||
					forceMode == NX_IMPULSE		||
					forceMode == NX_SMOOTH_IMPULSE
					)
				{
					// mass dependent...
					NxVec3 vActorForce = m_arrNxActors[i]->getMass() / GetMass() * vForce;
					m_arrNxActors[i]->addForce(vActorForce, forceMode);
				}
				else
				{
					// mass independent...
					m_arrNxActors[i]->addForce(vForce, forceMode);
				}

			}

		}

		m_pAPhysXScene->UnlockWritingScene();

		return true;
	}
	else
		return false;

	*/


	//??? Acquire Some Scene Writing Lock?...
	if(m_pAPhysXScene && m_arrNxActors.size())
	{

		for(int i = 0; i<m_arrNxActors.size(); i++)	
		{
			if(m_arrNxActors[i]->isDynamic())
			{

				APhysXRBHitInfo hitInfo;
				hitInfo.mHitActor = m_arrNxActors[i];
				hitInfo.mHitPos = m_arrNxActors[i]->getGlobalPosition();
				hitInfo.mHitForceMode = forceMode;
				hitInfo.mMaxVelChange = fMaxVelChange;

				if(	forceMode == NX_FORCE		||
					forceMode == NX_IMPULSE		||
					forceMode == NX_SMOOTH_IMPULSE
					)
				{
					// mass dependent...
					NxVec3 vActorForce = m_arrNxActors[i]->getMass() / GetMass() * vForce;
					hitInfo.mHitForce = vActorForce;
				}
				else
				{
					// mass independent...
					hitInfo.mHitForce = vForce;
				}

				OnHit(hitInfo);

			}

		}



		return true;
	}
	else
		return false;

}

bool APhysXRigidBodyObject::AddForce(const APhysXRBHitInfo& apxRBHitInfo)
{
	OnHit(apxRBHitInfo);
	return true;
}

void APhysXRigidBodyObject::PostSimulate(float dt)
{
	APhysXObject::PostSimulate(dt);

	HandleHit(false);
}


void APhysXRigidBodyObject::HandleHit(bool bIgnoreVelChangeLimit)
{
	if(!m_arrRBHitInfo.size()) return;

	for(int i=0; i<m_arrRBHitInfo.size(); i++)
	{
		// handle the case of ray hit...
		APhysXRBHitInfo& hitInfo = m_arrRBHitInfo[i];
		if(hitInfo.mHitType == APX_RBHITTYPE_RAY)
		{
			if(hitInfo.mHitActor && hitInfo.mHitActor->isDynamic() && !hitInfo.mHitActor->readBodyFlag(NX_BF_KINEMATIC))
			{
				if (!bIgnoreVelChangeLimit)
					GetPhysXScene()->ForceMagLimitation(*(hitInfo.mHitActor), hitInfo.mHitForce, hitInfo.mHitForceMode, hitInfo.mMaxVelChange);


				m_pAPhysXScene->LockWritingScene();
				hitInfo.mHitActor->addForceAtPos(hitInfo.mHitForce, hitInfo.mHitPos, hitInfo.mHitForceMode);
				m_pAPhysXScene->UnlockWritingScene();
			}


		}

	}
	m_arrRBHitInfo.clear();
}

void APhysXRigidBodyObject::SetObjectFlag(APhysXObjectFlag flag, bool val)
{
	APhysXObject::SetObjectFlag(flag, val);

	if(flag == APX_OBJFLAG_ONEWAY_COLLISION)
	{
		m_pAPhysXScene->LockWritingScene();

		for(int i = 0; i<m_arrNxActors.size(); i++)	
		{
			NxActor* pCurActor = m_arrNxActors[i];
			
			if(pCurActor->isDynamic())
			{
				if(val)
					pCurActor->setDominanceGroup(APX_RB_DG_LOW);
				else
					pCurActor->setDominanceGroup(APX_RB_DG_HIGH);
			}
		}

		m_pAPhysXScene->UnlockWritingScene();
	}
	//----------------------------------------------------------------
	// Revised by Wenfeng, 2012.3.21...
	// We set attached RB object's dominant group to the top rank, 
	// so that it will not be affected by any other RB objects.
	// And only controled by our AddForce function...

	else if (flag == APX_OBJFLAG_CC_CAN_ATTACH)
	{

		m_pAPhysXScene->LockWritingScene();

		for(int i = 0; i<m_arrNxActors.size(); i++)	
		{
			NxActor* pCurActor = m_arrNxActors[i];

			if(pCurActor->isDynamic())
			{
				if(val)
					pCurActor->setDominanceGroup(APX_RB_DG_TOP);
				else
					pCurActor->setDominanceGroup(APX_RB_DG_HIGH);
			}
		}

		m_pAPhysXScene->UnlockWritingScene();

	}

}

void APhysXRigidBodyObject::EnableCollision(bool bEnable /* = true */)
{
	APhysXObject::EnableCollision(bEnable);

	m_pAPhysXScene->LockWritingScene();

	for(int i = 0; i<m_arrNxActors.size(); i++)	
	{
		NxActor* pCurActor = m_arrNxActors[i];

		if(bEnable)
			pCurActor->clearActorFlag(NX_AF_DISABLE_COLLISION);
		else
			pCurActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);

	}

	m_pAPhysXScene->UnlockWritingScene();

}

void APhysXRigidBodyObject::EnableDebugRender(bool bEnable)
{
	APhysXObject::EnableDebugRender(bEnable);

	m_pAPhysXScene->LockWritingScene();

	for(int i = 0; i<m_arrNxActors.size(); i++)	
	{
		NxActor* pNxActor = m_arrNxActors[i];
		
		APhysXUtilLib::EnableDebugRender(pNxActor, bEnable);
	}

	m_pAPhysXScene->UnlockWritingScene();

}

void APhysXRigidBodyObject::PutToSleep()
{ 
	m_pAPhysXScene->LockWritingScene();
	for(unsigned int i = 0; i<m_arrNxActors.size(); i++)
	{
		m_arrNxActors[i]->putToSleep();
	}
	m_pAPhysXScene->UnlockWritingScene();
}

void APhysXRigidBodyObject::WakeUp()
{
	m_pAPhysXScene->LockWritingScene();
	for(unsigned int i = 0; i<m_arrNxActors.size(); i++)
	{
		m_arrNxActors[i]->wakeUp();
	}
	m_pAPhysXScene->UnlockWritingScene();
}

bool APhysXRigidBodyObject::IsSleeping()
{
	bool bSleeping = true;
	for(unsigned int i = 0; i<m_arrNxActors.size(); i++)
	{
		if(!m_arrNxActors[i]->isSleeping())
		{
			bSleeping = false;
			break;
		}
	}

	return bSleeping;
}

APhysXJoint* APhysXRigidBodyObject::GetJoint(const char* szJointName) const
{
	APhysXObjectConnector* pOC = GetInnerConnector(szJointName);
	if(pOC)
		return pOC->GetJoint();
	else
		return NULL;
}

void APhysXRigidBodyObject::ExecSubSimulationTask(float dt)
{
	if(m_pAPhysXScene->GetCurFrameDt() < 1e-5f)
		return;

	m_fAccumulatedSubSimDt += dt;
	float t = m_fAccumulatedSubSimDt / m_pAPhysXScene->GetCurFrameDt();
	if(t > 1.0f) t = 1.0f;

	NxVec3 vCurPos = APhysXUtilLib::Lerp(m_vSubSimCurPos, m_vSubSimTargetPos, t);
	NxQuat qCurRot;
	qCurRot.slerp(t, m_qSubSimCurRot, m_qSubSimTargetRot);
	
	NxMat34 mtPose;
	mtPose.M.fromQuat(qCurRot);
	mtPose.t = vCurPos;

	ExeSetPose(mtPose);

}