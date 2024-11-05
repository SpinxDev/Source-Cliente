
#include "APhysXStdAfx.h"

#define MAX_VEL_AFTER_BREAK		15.0f
#define MIN_VEL_AFTER_BREAK		0.3f
#define LIMIT_VEL_AFTER_BREAK	1

//---------------------------------------------------------------------
// APhysXRBHitModifier

inline float APhysXRBHitModifier::ComputeBreakForceFactor(float mHitMass, float mSrcMass)
{
	APHYSX_ASSERT(!NxMath::equals(mHitMass, 0.0f, 0.00001f));
	
	return 1.0f / (1 + mSrcMass/mHitMass);
}

inline float APhysXRBHitModifier::ComputeBreakForceFactor(NxActor* pHitActor, NxActor* pSrcActor)
{
	return ComputeBreakForceFactor(pHitActor->getMass(), pSrcActor->getMass());
}



void APhysXRBHitModifier::OnHit(const APhysXRBHitInfo& apxRBHitInfo, bool bSetHitMass /* = false */ , float fHitMass /* = NX_MAX_F32 */ )
{

	if(apxRBHitInfo.mSrcActor)
	{
		for(int i=0; i<m_arrHitSrcActorInfo.size(); i++)
		{
			// only push once...
			if(m_arrHitSrcActorInfo[i].mSrcActor == apxRBHitInfo.mSrcActor)
			{
				if(!bSetHitMass)
					m_arrHitSrcActorInfo[i].mHitMass += apxRBHitInfo.mHitActor->getMass();
				
				return;
			}
		}

		HitSrcActorInfo srcActorInfo;
		srcActorInfo.mSrcActor = apxRBHitInfo.mSrcActor;
		srcActorInfo.mPreHitVel = srcActorInfo.mSrcActor->isDynamic()?srcActorInfo.mSrcActor->getLinearVelocity():NxVec3(0.0f);
		// srcActorInfo.mPreHitVel = srcActorInfo.mSrcActor->getLinearVelocity();
		srcActorInfo.mHitMass = (bSetHitMass)? fHitMass : apxRBHitInfo.mHitActor->getMass();
		m_arrHitSrcActorInfo.push_back(srcActorInfo);

	}

}


void APhysXRBHitModifier::ModifyHitForce(APhysXRBHitInfo& apxRBHitInfo)
{
	if(apxRBHitInfo.mSrcActor)
	{
		float mSrc = apxRBHitInfo.mSrcActor->getMass();

		float mHitMass = NX_MAX_F32;
		for(int i=0; i<m_arrHitSrcActorInfo.size(); i++)
		{
			if(apxRBHitInfo.mSrcActor == m_arrHitSrcActorInfo[i].mSrcActor)
			{
				mHitMass = m_arrHitSrcActorInfo[i].mHitMass;
			}

		}
		APHYSX_ASSERT(mHitMass);

		// scale the Hit Force...
		apxRBHitInfo.mHitForce *= ComputeBreakForceFactor(mHitMass, mSrc);
	}

}

void APhysXRBHitModifier::ModifySrcActorVel()
{
	for(int i=0; i<m_arrHitSrcActorInfo.size(); i++)
	{
		float mSrcMass = m_arrHitSrcActorInfo[i].mSrcActor->getMass();
		float mHitMass = m_arrHitSrcActorInfo[i].mHitMass;
		APHYSX_ASSERT(mHitMass);

		float fFactor = ComputeBreakForceFactor(mHitMass, mSrcMass);


		if(m_arrHitSrcActorInfo[i].mSrcActor->isDynamic())
		{
			NxVec3 vVel = m_arrHitSrcActorInfo[i].mSrcActor->getLinearVelocity();
			vVel = vVel * fFactor + m_arrHitSrcActorInfo[i].mPreHitVel * (1-fFactor);

			m_arrHitSrcActorInfo[i].mSrcActor->setLinearVelocity(vVel);
		}

		// testing code...
		// m_arrHitSrcActorInfo[i].mSrcActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);

	}

}


//---------------------------------------------------------------------
// APhysXSimpleBreakableSkeletonRBObject

#define MODIFY_BREAK_FORCE

void APhysXSimpleBreakableSkeletonRBObject::OnCreateNxActorDesc(NxActorDesc* pNxActorDesc, int iActorID) 
{
	APhysXSkeletonRBObject::OnCreateNxActorDesc(pNxActorDesc, iActorID);

	//------------------------------------------------------------------------------
	// adjust the following parameters for better effects...

	NxBodyDesc* pBodyDesc = (NxBodyDesc*)pNxActorDesc->body;

	pBodyDesc->linearDamping = 0.3f;
	pBodyDesc->angularDamping = 0.3f;

}


void APhysXSimpleBreakableSkeletonRBObject::OnHit(const APhysXRBHitInfo& apxRBHitInfo)
{
	// just use the parent's method...
	APhysXSkeletonRBObject::OnHit(apxRBHitInfo);

#ifdef	MODIFY_BREAK_FORCE

	/*
	// obsolete code...
	if(apxRBHitInfo.mSrcActor)
	{
		for(int i=0; i<m_arrHitSrcActorInfo.size(); i++)
		{
			// only push once...
			if(m_arrHitSrcActorInfo[i].mActor == apxRBHitInfo.mSrcActor)
				return;
		}

		APhysXHitSrcActorInfo srcActorInfo;
		srcActorInfo.mActor = apxRBHitInfo.mSrcActor;
		srcActorInfo.mPreHitVel = srcActorInfo.mActor->getLinearVelocity();

		m_arrHitSrcActorInfo.push_back(srcActorInfo);

	}
	*/

	if(!m_bDynamic) 
		m_apxRBHitModifier.OnHit(apxRBHitInfo, true, GetMass());
		
#endif

/*

	if(apxRBHitInfo.mSrcActor)		// contact hit
	{
		for(int i=0; i< m_arrRBHitInfo.size(); i++)
		{
			if(apxRBHitInfo.mSrcActor == m_arrRBHitInfo[i].mSrcActor)
			{
				// we will ignore the hit actor here???
				m_arrRBHitInfo[i].mHitForce += apxRBHitInfo.mHitForce;
				m_arrRBHitInfo[i].mHitPos += apxRBHitInfo.mHitPos;
				m_arrRBHitInfo[i].mHitPos *= 0.5f;

			}

		}

	}


	// compute the break value, this is just a simple way...

	m_vBreakForce = apxRBHitInfo.mHitForce;
	m_vBreakCenter = (apxRBHitInfo.mHitType == APX_RBHITTYPE_RAY)?apxRBHitInfo.mHitPos:apxRBHitInfo.mHitActor->getGlobalPosition();

	float fBreakValue = apxRBHitInfo.mHitForce.magnitude();
	CheckBreak(fBreakValue);

*/

}

void APhysXSimpleBreakableSkeletonRBObject::HandleHit(bool bIgnoreVelChangeLimit)
{

	// compute the break value and then check whether I will be broken...
	if(!m_arrRBHitInfo.size()) return;

	// m_pAPhysXScene->GetPerfStats()->ResumePerfStat("Breakable HandleHit");

	if(IsBroken())
	{
		APhysXSkeletonRBObject::HandleHit(true);
	}
	else
	{
		m_vBreakCenter.set(0.0f);
		m_vBreakForce.set(0.0f);

		float t = 0.0f;
		float fMyMass = GetMass();




#ifdef	MODIFY_BREAK_FORCE
		
		NxVec3 vTotalHitForce(0.0f);

#endif

		//??? to do: how about the different Force_Mode?...
		for(int i=0; i<m_arrRBHitInfo.size(); i++)
		{
			// handle the case of ray hit...
			APhysXRBHitInfo& hitInfo = m_arrRBHitInfo[i];

			//------------------------------------------------
			// noted by wenfeng, Aug.27, 2012
			// disable the breaking effect caused by contact cases for "XiaoAoJiangHu"
			if(hitInfo.mHitType != APX_RBHITTYPE_RAY) 
				continue;

#ifdef	MODIFY_BREAK_FORCE

			// test the scale-force methods so as to enhance the breaking reality effect...
			vTotalHitForce += hitInfo.mHitForce;

			/*
			// obsolete code...
			if(hitInfo.mSrcActor)
			{
				float mSrc = hitInfo.mSrcActor->getMass();
				float fFactor = mSrc / fMyMass;

				// scale the Hit Force...
				hitInfo.mHitForce /= (1 + fFactor);
				
			}
			*/
			if(!m_bDynamic)
			{
				m_apxRBHitModifier.ModifyHitForce(hitInfo);
			}
			/*
			else
			{
				// dynamic-breakable case, we also modify the break force by some factor...
				// the real factor should represent the restitution of their material
				// while, here, we just use a experienced value...
				if(hitInfo.mSrcActor)
				{
					hitInfo.mHitForce *= 0.0f;
				}

			}
			*/


#endif

			float m1 = m_vBreakForce.magnitude();
			float m2 = hitInfo.mHitForce.magnitude();
			if(NxMath::equals(m2, 0.0f, 0.001f))				// note here! divide-zero Error...
				continue;
				
			t = m1/(m1 + m2);
			m_vBreakCenter = m_vBreakCenter * t + hitInfo.mHitPos * (1-t);
			m_vBreakForce += hitInfo.mHitForce;
		}


#ifdef	MODIFY_BREAK_FORCE
		float fBreakValue = vTotalHitForce.magnitude();
#else
		float fBreakValue = m_vBreakForce.magnitude();
#endif

		if(m_bDynamic)
		{
			m_vBreakForce *= 0.1f;
		}


		CheckBreak(fBreakValue);
		


#ifdef	MODIFY_BREAK_FORCE

		if(IsBroken())
		{
			// adjust the source actor's linear velocity...
			/*

			// the following code does not work well for un-perpendicular collision case...

			for(int i=0; i<arrSrcActors.size(); i++)
			{
				float mSrc = arrSrcActors[i]->getMass();
				float fFactor = mSrc / fMyMass;
				
				NxVec3 vVel = arrSrcActors[i]->getLinearVelocity();
				
				vVel *= (1 - fFactor)/(1+fFactor);
				
				arrSrcActors[i]->setLinearVelocity(vVel);
				
			}
			*/

			/*
			// obsolete code...
			for(int i=0; i<m_arrHitSrcActorInfo.size(); i++)
			{
				float mSrc = m_arrHitSrcActorInfo[i].mActor->getMass();
				float fFactor = mSrc / fMyMass;
				fFactor = 1.0f/(1 + fFactor);

				NxVec3 vVel = m_arrHitSrcActorInfo[i].mActor->getLinearVelocity();
				vVel = vVel * fFactor + m_arrHitSrcActorInfo[i].mPreHitVel * (1-fFactor);

				m_arrHitSrcActorInfo[i].mActor->setLinearVelocity(vVel);
			}
			*/
			if(!m_bDynamic)
			{
				m_pAPhysXScene->LockWritingScene();
				m_apxRBHitModifier.ModifySrcActorVel();
				m_pAPhysXScene->UnlockWritingScene();
			}


			// force to set m_apxRBDrivenMode to APX_RBDRIVENMODE_PHYSICS...
			m_apxRBDrivenMode = APX_RBDRIVENMODE_PHYSICS;
			
		}
		else if(m_bDynamic)
		{
			APhysXSkeletonRBObject::HandleHit(true);
		}

		// clear the hit info of current tick...
		m_arrRBHitInfo.clear();

#endif

	}

#ifdef	MODIFY_BREAK_FORCE
	
	// obsolete code...
	// m_arrHitSrcActorInfo.clear();			// clear at each tick...
	m_apxRBHitModifier.Reset();

#endif

	// m_pAPhysXScene->GetPerfStats()->PausePerfStat("Breakable HandleHit");
}

void APhysXSimpleBreakableSkeletonRBObject::SetDrivenMode(APhysXRBDrivenMode apxRBDrivenMode, APhysXArray<NxActor* >* pActorsDrivenByPhysics /* = NULL */)
{
	if(m_bDynamic)
		APhysXSkeletonRBObject::SetDrivenMode(apxRBDrivenMode, pActorsDrivenByPhysics);
	else
	{
		if(apxRBDrivenMode == APX_RBDRIVENMODE_PHYSICS)
		{
			// do nothing...

		}
		else
		{
			APhysXSkeletonRBObject::SetDrivenMode(apxRBDrivenMode, pActorsDrivenByPhysics);
		}

	}

}

bool APhysXSimpleBreakableSkeletonRBObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	bool bResult = APhysXSkeletonRBObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc);

	APhysXSimpleBreakableSkeletonRBObjectDesc* BSRBObjDesc = (APhysXSimpleBreakableSkeletonRBObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;

	//-------------------------------------------------------------------
	// here we scale the break limit property by its volume scaling...
	NxVec3 vScale3D;
	vScale3D.arrayMultiply(apxObjInstanceDesc.mScale3D, m_vUniformActorScale3D);
	SetBreakLimit(BSRBObjDesc->mBreakLimit * vScale3D.x * vScale3D.y * vScale3D.z);
	
	SetUserBreakReport(pAPhysXScene->GetUserBreakReport());

	// init the release joints time to the whole life span...
	m_fReleaseJointsTime = m_fLifeSpan;
	// m_fBreakFallOffRadius = 5.0f;

	// try to use the AABB's diagonal as the fall off radius...
	NxBounds3 aabb;
	GetAABB(aabb);
	NxVec3 vExtents;
	aabb.getExtents(vExtents);
	m_fBreakFallOffRadius = vExtents.magnitude();


	m_bDynamic = BSRBObjDesc->mDynamic;

	if(!m_bDynamic) EnableKinematic();

	return bResult;
}

bool APhysXSimpleBreakableSkeletonRBObject::PostCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{

	// firstly, we should create some extra hidden joint desc to connect dynamic actors...
	if(m_bDynamic)
	{
		APhysXFixedJointDesc apxFixJointDesc;
		apxFixJointDesc.mGlobalAxis.set(0.0f, 1.0f, 0.0f);
		apxFixJointDesc.mPhysXObject1 = apxFixJointDesc.mPhysXObject2 = this;
		
		// here, we should create about o(n^2) joints to avoid inner actors' overlapping...
#if 1

		apxFixJointDesc.mNxActor1 = m_arrNxActors[0];
		for(int i = 0; i<m_arrNxActors.size(); i++)
		{
			m_arrNxActors[i]->setSolverIterationCount(101);

			if(i>0)
			{
				apxFixJointDesc.mNxActor2 = m_arrNxActors[i];
				// notice here, this will change the Object desc...
				apxObjInstanceDesc.mPhysXObjectDesc->AddInnerConnector(apxFixJointDesc);

			}
			
		}

#else
		
		for(int i =0; i<m_arrNxActors.size(); i++)
		{
			apxFixJointDesc.mNxActor1 = m_arrNxActors[i];
			for(int j=i+1; j<m_arrNxActors.size(); j++)
			{
				apxFixJointDesc.mNxActor2 = m_arrNxActors[j];

				// notice here, this will change the Object desc...
				apxObjInstanceDesc.mPhysXObjectDesc->AddInnerConnector(apxFixJointDesc);
			}

			m_arrNxActors[i]->setSolverIterationCount(10);
		}

#endif
		
	}

	// then, call parent's method to create those joints
	bool bRes = APhysXSkeletonRBObject::PostCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc);

	if(m_bDynamic)
	{
		// after create the inner-connectors, we should recover the apxObjInstanceDesc.mPhysXObjectDesc to 
		// unchanged state...

		// currently, we only support breakable object without any initial inner-connectors...
		apxObjInstanceDesc.mPhysXObjectDesc->ReleaseInnerConnectors();
	}

	return bRes;

}


void APhysXSimpleBreakableSkeletonRBObject::PreSimulate(float dt)
{
	APhysXSkeletonRBObject::PreSimulate(dt);

	//--------------------------------------
	// test code...
	//if(IsBroken())
	//{
	//	for(int i=0; i<m_arrNxActors.size(); i++)
	//	{
	//		NxActor* pCurActor = m_arrNxActors[i];


	//		NxVec3 vVelocity = pCurActor->getLinearVelocity();
	//		float fVMag = vVelocity.normalize();
	//		char szInfo[256];
	//		sprintf(szInfo, "Vel: %f, ", fVMag);
	//		OutputDebugStringA(szInfo);
	//	}

	//	OutputDebugStringA("\n");
	//}

#if 0


	// check whether we are at the release joint time...
	if(m_fCurLife > m_fReleaseJointsTime)
	{
		ReleaseInnerConnectors();
		for(int i =0; i<m_arrNxActors.size(); i++)
		{
			m_arrNxActors[i]->setSolverIterationCount(4);
		}
	}

#endif

	m_dt = dt;

}

void APhysXSimpleBreakableSkeletonRBObject::OnBreak()
{
#if 1

	if(m_bDynamic)
	{
		ReleaseInnerConnectors();

	}

	// EnableSelfCollision(false);

	EnableKinematic(false);


	// delay the connector release...
	// m_fReleaseJointsTime = m_fCurLife + 0.3f;

	// just do nothing and notify the breakage to user notify...

#else

	// test new radical breakable...

	bool bAllBreak = true;

	for(int i=0; i<m_arrNxActors.size(); i++)
	{
		NxActor* pCurActor = m_arrNxActors[i];

		if(pCurActor->readBodyFlag(NX_BF_KINEMATIC))
		{
			// compute the break force which will enforce to this actor...
			float fCurR = m_vBreakCenter.distance(pCurActor->getGlobalPosition());
			if(fCurR < m_fBreakFallOffRadius)
			{
				pCurActor->clearBodyFlag(NX_BF_KINEMATIC);
			}
			else
				bAllBreak = false;
		}

	}


	m_bBroken = bAllBreak;

#endif

}

void APhysXSimpleBreakableSkeletonRBObject::PostBreak()
{
	// m_pAPhysXScene->GetPerfStats()->ResumePerfStat("Breakable PostBreak");

	// call user call back...
	APhysXSimpleBreakable::PostBreak();

	// if(m_bDynamic) return;

	// float fSqrR = m_fBreakFallOffRadius * m_fBreakFallOffRadius;

	//-------------------------------------------------------------------------------
	// compute the unit force from the break force...

	float fTotalFactor = 0.0f;

	for(int i=0; i<m_arrNxActors.size(); i++)
	{
		NxActor* pCurActor = m_arrNxActors[i];

		if(!pCurActor->readBodyFlag(NX_BF_KINEMATIC))
		{
			// compute the break force which will enforce to this actor...
			float fCurR = m_vBreakCenter.distance(pCurActor->getGlobalPosition());
			if(fCurR < m_fBreakFallOffRadius)
			{
				float fFactor = (1.0f - fCurR/m_fBreakFallOffRadius);
				fFactor *= fFactor;

				fTotalFactor += fFactor;
			}
		}

		//--------------------------------------------------------
		// for test...
		//NxVec3 vVelocity = pCurActor->getLinearVelocity();
		//float fVMag = vVelocity.normalize();
		//char szInfo[256];
		//sprintf(szInfo, "Vel: %f, ", fVMag);
		//OutputDebugStringA(szInfo);

		// clear the velocity before we add force...
		m_pAPhysXScene->LockWritingScene();
		pCurActor->setLinearVelocity(NxVec3(0.0f));
		pCurActor->setAngularVelocity(NxVec3(0.0f));
		m_pAPhysXScene->UnlockWritingScene();
	}


	NxVec3 vBaseForce = (1.0f / fTotalFactor) * m_vBreakForce;

	// after the the breakage, we will add force to all actors inside the scope of breakage radius...
	for(int i=0; i<m_arrNxActors.size(); i++)
	{
		NxActor* pCurActor = m_arrNxActors[i];

		if(!pCurActor->readBodyFlag(NX_BF_KINEMATIC))
		{
			// compute the break force which will enforce to this actor...
			float fCurR = m_vBreakCenter.distance(pCurActor->getGlobalPosition());
			if(fCurR < m_fBreakFallOffRadius)
			{
				float fFactor = (1.0f - fCurR/m_fBreakFallOffRadius);
				fFactor *= fFactor;

				NxVec3 vDelta = pCurActor->getGlobalPosition() - m_vBreakCenter;
				vDelta.normalize();
				NxVec3 vForce = vBaseForce * fFactor;
				float fForceScale = vForce.normalize();
				// vForce += vDelta;

				if(m_bDynamic)
				{
					// for dynamic case, we only add the force in scatter direction...
					vForce = vDelta;
				}
				else
				{
					vForce = vForce * fFactor + vDelta * (1-fFactor);
					vForce.normalize();
				}

#if LIMIT_VEL_AFTER_BREAK		// Limit the max velocity after the breaking...


				float fV = fForceScale * m_dt / pCurActor->getMass();
				if(fV > MAX_VEL_AFTER_BREAK )
				{
					fForceScale *= MAX_VEL_AFTER_BREAK /fV;
					fV = MAX_VEL_AFTER_BREAK;

				}
				//--------------------------------------------------------
				// revised by wenfeng, May 15, 2013
				// to handle the case of very huge breakable object...
				else if(fV < MIN_VEL_AFTER_BREAK)
				{
					// fV = MAX_VEL_AFTER_BREAK * fFactor / fTotalFactor;
					fV = MAX_VEL_AFTER_BREAK;
				}




#endif

				// pCurActor->addForce(vForce * fForceScale);
				m_pAPhysXScene->LockWritingScene();
				pCurActor->addForce(vForce * fV, NX_VELOCITY_CHANGE);
				m_pAPhysXScene->UnlockWritingScene();

				// this may create some torque...
				// pCurActor->addForceAtPos(vForce * fForceScale, pCurActor->getGlobalPosition());
				
				// how about addForce at break center?
				// pCurActor->addForceAtPos(vForce * fForceScale, m_vBreakCenter);
				
				// pCurActor->setAngularVelocity(20 * APHYSX_PI * APhysXUtilLib::GenRndDir());
				// pCurActor->setAngularVelocity(APhysXUtilLib::GenRndAngVel(10 * APHYSX_PI));

			}

		}


	}

	// m_pAPhysXScene->GetPerfStats()->PausePerfStat("Breakable PostBreak");
}



//-------------------------------------------------------------------------------
// APhysXBreakableSkeletonRBObject

bool APhysXBreakableSkeletonRBObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	bool bResult = APhysXSkeletonRBObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc);

	APhysXBreakableSkeletonRBObjectDesc* apxBSRBObjDesc = (APhysXBreakableSkeletonRBObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;

	SetUserBreakReport(pAPhysXScene->GetUserBreakReport());

	if(apxBSRBObjDesc->GetActorNeighborGraph())
	{
		m_pActorNeighborGraph = apxBSRBObjDesc->GetActorNeighborGraph();
		m_pActorNeighborGraph->AddRef();
	}

	if(apxBSRBObjDesc->GetActorSupportGraph())
	{
		m_pActorSupportGraph = apxBSRBObjDesc->GetActorSupportGraph();
		m_pActorSupportGraph->AddRef();
	}

	//-------------------------------------------------------------------
	// here we scale the break limit property by its volume scaling...
	NxVec3 vScale3D;
	vScale3D.arrayMultiply(apxObjInstanceDesc.mScale3D, m_vUniformActorScale3D);
	m_fActorBreakLimit = apxBSRBObjDesc->mActorBreakLimit * vScale3D.x * vScale3D.y * vScale3D.z ;
	m_fBreakFallOffFactor = apxBSRBObjDesc->mBreakFallOffFactor;

	for(int i=0; i<GetNxActorNum(); i++)
	{
		ActorBreakInfo actorBreakInfo;
		m_arrActorBreakInfo.push_back(actorBreakInfo);
	}

	EnableKinematic();

	return bResult;
	
}

void APhysXBreakableSkeletonRBObject::OnReleaseBasicPhysXStuff()
{
	
	if(m_pActorNeighborGraph) 
	{
		m_pActorNeighborGraph->RemoveRef();
		m_pActorNeighborGraph = NULL;
	}
	if(m_pActorSupportGraph)
	{
		m_pActorSupportGraph->RemoveRef();
		m_pActorSupportGraph = NULL;
	}


	APhysXSkeletonRBObject::OnReleaseBasicPhysXStuff();
}

void APhysXBreakableSkeletonRBObject::SetDrivenMode(APhysXRBDrivenMode apxRBDrivenMode, APhysXArray<NxActor* >* pActorsDrivenByPhysics /* = NULL */)
{
	if(apxRBDrivenMode == APX_RBDRIVENMODE_PHYSICS)
	{
		// do nothing...

	}
	else
	{
		APhysXSkeletonRBObject::SetDrivenMode(apxRBDrivenMode, pActorsDrivenByPhysics);
	}


}


APhysXGraph* APhysXBreakableSkeletonRBObject::BuildActorNeighborGraph(float fThickness)
{

	if(!m_pActorNeighborGraph)
	{
		m_pActorNeighborGraph = new APhysXGraph;
		m_pActorNeighborGraph->AddRef();
	}

	m_pActorNeighborGraph->Init(GetNxActorNum());

	for(int i=0; i<GetNxActorNum(); i++)
	{
		NxActor* pCurActor = GetNxActor(i);
		
		NxArray<NxActor* > HitActors;

		APhysXUtilLib::CheckOverlap_Inflate(*pCurActor, fThickness, HitActors);

		APhysXIDArray arrNeighbors;

		for(int j=0; j<HitActors.size(); j++)
		{
			int nid = GetNxActorID(HitActors[j]);

			if(nid != -1) arrNeighbors.push_back(nid);

		}

		m_pActorNeighborGraph->SetVertexNeighbors(i, arrNeighbors);
	}

	return m_pActorNeighborGraph;
}

APhysXGraph* APhysXBreakableSkeletonRBObject::BuildActorSupportGraph(float fThickness)
{

	if(!m_pActorSupportGraph)
	{
		m_pActorSupportGraph = new APhysXGraph;
		m_pActorSupportGraph->AddRef();
	}

	m_pActorSupportGraph->Init(GetNxActorNum());

	for(int i=0; i<GetNxActorNum(); i++)
	{
		NxActor* pCurActor = GetNxActor(i);

		NxArray<NxActor* > HitActors;

		// linear sweep the gravity direction, by default it is (0, -1, 0) to get the support graph
		NxArray<APhysXUtilLib::APhysXDetailOverlapInfo> arrDOI;
		APhysXUtilLib::LinearSweep(*pCurActor, NxVec3(0, -1.0f, 0), fThickness, HitActors, &arrDOI);

		APhysXIDArray arrNeighbors;

		for(int j=0; j<HitActors.size(); j++)
		{
			int nid = GetNxActorID(HitActors[j]);

			NxVec3& vOverlapNormal = arrDOI[j].mOverlapNormal;

// cosine value of the angle between the normal and the vertical direction... 
// now, we use the threshold of 72.54 degree.
#define VERTICAL_ANGLE_THRESH_COS	0.3f

			if(nid != -1 && vOverlapNormal.y > VERTICAL_ANGLE_THRESH_COS) 
				arrNeighbors.push_back(nid);

		}

		m_pActorSupportGraph->SetVertexNeighbors(i, arrNeighbors);
	}

/*

	// Note by wenfeng, Sept.14, 2009
	// the following code snippet which is to remove the wrong support actors caused by overlapping case
	// is now obsolete, since we are now using the overlap normals to help to get the real supported actors.


	// review the overlap cases: when the actor A and actor B overlap, we do the LinearSweep check
	// and will get the results that B is a A's supported actor and A is a B's supported actor as well.
	// In this case, we should figure out that which one is the real supported one. here, we just use
	// their pos.y information.

	for(int i=0; i<GetNxActorNum(); i++)
	{
		// get the i# actor's neighbors...
		APhysXIDArray arrNeighbors;
		m_pActorSupportGraph->GetVertexNeighbors(i, arrNeighbors);
		
		APhysXIDArray arrFakeNeighbors;

		for(int j=0; j<arrNeighbors.size(); j++)
		{
			int nid = arrNeighbors[j];

			// search its' neighbors...
			APhysXIDArray arrNbNbs;

			m_pActorSupportGraph->GetVertexNeighbors(nid, arrNbNbs);

			if(APhysXUtilLib::APhysXArray_Find(arrNbNbs, i))
			{

				NxActor* pCurActor = GetNxActor(i);
				NxActor* pNeighborActor = GetNxActor(nid);
#if 0

				if(pCurActor->getGlobalPosition().y > pNeighborActor->getGlobalPosition().y)
				{
					arrNbNbs.deleteEntry(i);
					m_pActorSupportGraph->SetVertexNeighbors(nid, arrNbNbs);
				}
				else
				{
					arrFakeNeighbors.push_back(nid);
				}

#else

				// re-calculate the overlap normal between the actors...
				APhysXUtilLib::APhysXDetailOverlapInfo apxDOI;
				APhysXUtilLib::CheckOverlap_Inflate(*pCurActor, *pNeighborActor, fThickness, &apxDOI);


// cosine value of the angle between the normal and the vertical direction... 
// now, we use the threshold of 60 degree
#define VERTICAL_ANGLE_THRESH_COS	0.5f

				if(apxDOI.mOverlapNormal.y > VERTICAL_ANGLE_THRESH_COS)
				{
					// pNeighborActor supports pCurActor...
					arrNbNbs.deleteEntry(i);
					m_pActorSupportGraph->SetVertexNeighbors(nid, arrNbNbs);

				}
				else if( apxDOI.mOverlapNormal.y < -VERTICAL_ANGLE_THRESH_COS)
				{
					// pCurActor supports pNeighborActor...
					arrFakeNeighbors.push_back(nid);

				}
				else
				{
					// there is no real support
					arrNbNbs.deleteEntry(i);
					m_pActorSupportGraph->SetVertexNeighbors(nid, arrNbNbs);

					arrFakeNeighbors.push_back(nid);
				}

#endif

			}


		}

		for(int j=0; j<arrFakeNeighbors.size(); j++)
		{
			arrNeighbors.deleteEntry(arrFakeNeighbors[j]);
		}


	}
*/


	return m_pActorSupportGraph;

}


inline bool APhysXBreakableSkeletonRBObject::IsActorBroken(int iActorID)
{
	// simple version
	return !GetNxActor(iActorID)->readBodyFlag(NX_BF_KINEMATIC);
}

inline bool APhysXBreakableSkeletonRBObject::CheckFullyBroken()
{
	for(int i=0; i<GetNxActorNum(); i++)
	{
		if(!IsActorBroken(i) && CanActorBreak(i)) return false;
	}

	return true;
}

inline void APhysXBreakableSkeletonRBObject::BreakActor(int iActorID)
{
	GetNxActor(iActorID)->clearBodyFlag(NX_BF_KINEMATIC);
	GetNxActor(iActorID)->wakeUp();
}


inline bool APhysXBreakableSkeletonRBObject::CanActorBreak(int iActorID)
{
	return !APhysXUtilLib::APhysXArray_Find(m_arrFixedActors, GetNxActor(iActorID));
}


// Note: this is a recursive function and now it is deprecated...
#if 0

void APhysXBreakableSkeletonRBObject::ComputeBreakForce_r(int iKernel, float fKernelForceMagnitude, APhysXArray<bool> arrIsActorVisited, int iCurLevel /* =0 */ )
{

	static float s_fSkipForceMagnitude = 10.0f;
	static int s_iMaxLevel = 2;

	if(iCurLevel > s_iMaxLevel) return;

	fKernelForceMagnitude *= m_fBreakFallOffFactor;
	if(fKernelForceMagnitude < s_fSkipForceMagnitude) return;

	APhysXIDArray arrNeighbors;
	m_pActorNeighborGraph->GetVertexNeighbors(iKernel, arrNeighbors);
	for(int i=0; i<arrNeighbors.size(); i++)
	{
		int iNeighbor = arrNeighbors[i];
		if(!IsActorBroken(iNeighbor) && !arrIsActorVisited[iNeighbor])
		{	
			// compute the force...
			NxVec3 vForceDir = GetNxActor(iNeighbor)->getGlobalPosition() - GetNxActor(iKernel)->getGlobalPosition();
			vForceDir.normalize();

			// m_arrActorBreakInfo[iNeighbor].mBreakForce += vForceDir * fKernalForceMagnitude;
			m_arrActorBreakInfo[iNeighbor].mBreakForce = vForceDir * fKernelForceMagnitude;

			arrIsActorVisited[iNeighbor] = true;

		}
	}

	// recursive call...
	for(int i=0; i<arrNeighbors.size(); i++)
	{
		int iNeighbor = arrNeighbors[i];
		if(!IsActorBroken(iNeighbor))
		{
			ComputeBreakForce_r(iNeighbor, fKernelForceMagnitude, arrIsActorVisited, iCurLevel+1);
		}
	}

}

#endif



struct NeighborActorInfo
{
	int mID;
	int mLevel;
	float mForceMagnitude;
};


//-----------------------------------------------------------------
void APhysXBreakableSkeletonRBObject::ComputeBreakForce(int iKernel, const NxVec3& vKernelForce, APhysXArray<bool> arrIsActorVisited, NxActor* pHitSrcActor /* = NULL */, int iMaxLevel /* = 5 */, float fSkipForceMagnitude /* = 10.0f */)
{
	if(IsActorBroken(iKernel)) return;

	float fKernelForceMagnitude = vKernelForce.magnitude();

	APhysXSimpleQueue<NeighborActorInfo> qNeighborActors;

	NeighborActorInfo kernelNAInfo;
	kernelNAInfo.mID = iKernel;
	kernelNAInfo.mLevel = 0;
	kernelNAInfo.mForceMagnitude = fKernelForceMagnitude;

	qNeighborActors.PushBack(kernelNAInfo);

	while(!qNeighborActors.IsEmpty())
	{
		// pop the first element 
		NeighborActorInfo curKerlelNAInfo;
		qNeighborActors.PopFront(curKerlelNAInfo);

		if(curKerlelNAInfo.mForceMagnitude < fSkipForceMagnitude || curKerlelNAInfo.mLevel > iMaxLevel)
			continue;

		if(!IsActorBroken(curKerlelNAInfo.mID) && !arrIsActorVisited[curKerlelNAInfo.mID])
		{
			// if it is not broken and not handled yet...

			float fBreakForceFactor = pHitSrcActor? APhysXRBHitModifier::ComputeBreakForceFactor(GetNxActor(curKerlelNAInfo.mID), pHitSrcActor):1.0f;

			if(curKerlelNAInfo.mID == iKernel)
			{
				m_arrActorBreakInfo[curKerlelNAInfo.mID].mBreakForce += vKernelForce;			
				m_arrActorBreakInfo[curKerlelNAInfo.mID].mPostAddedForce += vKernelForce * fBreakForceFactor;
			}
			else
			{
				NxVec3 vForceDir = GetNxActor(curKerlelNAInfo.mID)->getGlobalPosition() - GetNxActor(iKernel)->getGlobalPosition();
				vForceDir.normalize();

				NxVec3 vBreakForce = vForceDir * curKerlelNAInfo.mForceMagnitude;
				m_arrActorBreakInfo[curKerlelNAInfo.mID].mBreakForce += vBreakForce;
				m_arrActorBreakInfo[curKerlelNAInfo.mID].mPostAddedForce += vBreakForce * fBreakForceFactor;

			}

			arrIsActorVisited[curKerlelNAInfo.mID] = true;

			// now, push back its neighbors...
			APhysXIDArray arrNeighbors;
			m_pActorNeighborGraph->GetVertexNeighbors(curKerlelNAInfo.mID, arrNeighbors);

			for(int i=0; i<arrNeighbors.size(); i++)
			{
				NeighborActorInfo naInfo;
				naInfo.mID = arrNeighbors[i];
				naInfo.mForceMagnitude = curKerlelNAInfo.mForceMagnitude * m_fBreakFallOffFactor;
				naInfo.mLevel = curKerlelNAInfo.mLevel +1;

				
				qNeighborActors.PushBack(naInfo);
			}


		}	// end of if...


	}	// end of while...





}



inline static void ResetActorBreakInfo(APhysXArray<APhysXBreakableSkeletonRBObject::ActorBreakInfo>& arrActorBreakInfo)
{
	for(int i=0; i<arrActorBreakInfo.size(); i++)
	{
		arrActorBreakInfo[i].mBreakForce.zero();
		arrActorBreakInfo[i].mPostAddedForce.zero();
	}

}

inline static void ResetActorVisitState(APhysXArray<bool>& arrActorVisitState)
{

	for(int i=0; i<arrActorVisitState.size(); i++)
	{
		arrActorVisitState[i] = false;
	}

}

void APhysXBreakableSkeletonRBObject::OnHit(const APhysXRBHitInfo& apxRBHitInfo)
{

	APhysXSkeletonRBObject::OnHit(apxRBHitInfo);

	if(IsValid() && !IsFullyBroken())
	{
		m_apxRBHitModifier.OnHit(apxRBHitInfo);
	}

}


void APhysXBreakableSkeletonRBObject::HandleHit(bool bIgnoreVelChangeLimit)
{

	if(!m_arrRBHitInfo.size()) return;
	
	if(!IsValid() || IsFullyBroken())
	{
		APhysXSkeletonRBObject::HandleHit(true);
		return;
	}

	// reset break info...
	m_vBreakCenter.set(0.0f);
	m_vBreakForce.set(0.0f);

	// update the actor break info...
	ResetActorBreakInfo(m_arrActorBreakInfo);

	APhysXArray<bool> arrActorVisitState;
	for(int i=0; i<GetNxActorNum(); i++)
	{
		arrActorVisitState.push_back(false);
	}

	for(int i=0; i<m_arrRBHitInfo.size(); i++)
	{
		

		// handle the case of ray hit...
		APhysXRBHitInfo& hitInfo = m_arrRBHitInfo[i];
		int iHitActorID = GetNxActorID(hitInfo.mHitActor);

		if(!IsActorBroken(iHitActorID) && CanActorBreak(iHitActorID))
		{
			ResetActorVisitState(arrActorVisitState);
			ComputeBreakForce(iHitActorID, hitInfo.mHitForce, arrActorVisitState, hitInfo.mSrcActor);

		}

		// handle the ray hit force to broken actors...
		if(IsActorBroken(iHitActorID) && hitInfo.mHitType == APX_RBHITTYPE_RAY )
		{
			hitInfo.mHitActor->addForceAtPos(hitInfo.mHitForce, hitInfo.mHitPos, hitInfo.mHitForceMode);
		}

		m_vBreakCenter += hitInfo.mHitPos;
		m_vBreakForce += hitInfo.mHitForce;
	}

	m_vBreakCenter /= (float)m_arrRBHitInfo.size();
	m_vBreakForce /= (float)m_arrRBHitInfo.size();

	// test if the actor breaking?
	bool bActorBroken = false;
	for(int i =0; i<GetNxActorNum(); i++)
	{
		if(!IsActorBroken(i) && CanActorBreak(i))
		{

			if(m_arrActorBreakInfo[i].mBreakForce.magnitude() > m_fActorBreakLimit)
			{
				BreakActor(i);
				
				// add breaking force...
				
				/*
				// test code...
				float fMass = GetNxActor(i)->getMass();
				NxVec3 vForce = m_arrActorBreakInfo[i].mPostAddedForce;
				*/

				NxVec3& vForce = m_arrActorBreakInfo[i].mPostAddedForce;
				NxActor* pCurActor = GetNxActor(i);

#if LIMIT_VEL_AFTER_BREAK		// Limit the max velocity after the breaking...

				float fForceScale = vForce.normalize();
				float fV = fForceScale * m_pAPhysXScene->GetCurFrameDt() / pCurActor->getMass();



				
				if(fV > MAX_VEL_AFTER_BREAK)
				{
					fForceScale *= MAX_VEL_AFTER_BREAK/fV;

				}



				vForce *= fForceScale;
#endif



				pCurActor->addForce(vForce);
				
				bActorBroken = true;
			}



		}

	} // end of for...

	if(bActorBroken)
	{
		// modify the hit src actor's velocity...
		m_apxRBHitModifier.ModifySrcActorVel();

		// check whether this break is the first or fully...
		if(!IsBroken()) 
		{
			NotifyFirstBreak();
			m_apxRBDrivenMode = APX_RBDRIVENMODE_PHYSICS;
		}
		
		if(!IsFullyBroken() && CheckFullyBroken()) NotifyAllBreak();

		// notify user break call back...
		Break();
	}

	// reset the m_apxRBHitModifier
	m_apxRBHitModifier.Reset();

	// clear the RBHitInfo...
	m_arrRBHitInfo.clear();

}

bool APhysXBreakableSkeletonRBObject::IsActorSupported(int iActorID)
{
	APhysXIDArray supportActors;
	m_pActorSupportGraph->GetVertexNeighbors(iActorID, supportActors);

	if(supportActors.size())
	{
		for(int i=0; i<supportActors.size(); i++)
		{
			if(!IsActorBroken(supportActors[i]))
				return true;
		}

		return false;

	}
	else
	{
		// note here: if there is no neighbors in a actor's support graph, it means that
		// this actor is the bottom actor which can be supported by outside objects...
		return true;					
	}

}


void APhysXBreakableSkeletonRBObject::PostSimulate(float dt)
{
	APhysXSkeletonRBObject::PostSimulate(dt);

	// invalid case and full broken case, we return directly...
	if(!IsValid() || IsFullyBroken()) return;

//-----------------------------------------------------------------------
// if we turn on the DELAY_UNSUPPORT_BREAK, it can augment the 
//	un-real but cool effect...
#define DELAY_UNSUPPORT_BREAK 0					
#if DELAY_UNSUPPORT_BREAK

	static float s_fCurTestSupportInterval = 0.0f;
	s_fCurTestSupportInterval += dt;
	
	if(s_fCurTestSupportInterval > 0.01f)

#endif

	{

		// just try to break those actors without supported actors...
		// test whether the actor supported?
		bool bActorBroken = false;
		for(int i =0; i<GetNxActorNum(); i++)
		{
			if(!IsActorBroken(i) && !IsActorSupported(i) && CanActorBreak(i))
			{
				BreakActor(i);
				bActorBroken = true;

#if DELAY_UNSUPPORT_BREAK

				break;			// break only one unsupported actor at once in a tick

#endif

			}
		}

		if(bActorBroken)
		{
			// check whether this break is the first or fully...
			if(!IsBroken()) 
			{
				NotifyFirstBreak();
				m_apxRBDrivenMode = APX_RBDRIVENMODE_PHYSICS;
			}

			if(!IsFullyBroken() && CheckFullyBroken()) NotifyAllBreak();

			// notify user break call back...
			//??? passive breaking, do not notify the user?...
			// Break();
		}


#if DELAY_UNSUPPORT_BREAK
		
		s_fCurTestSupportInterval = 0.0f;

#endif

	}
}

void APhysXBreakableSkeletonRBObject::OnBreak()
{
	// do nothing...

}


#undef MAX_VEL_AFTER_BREAK