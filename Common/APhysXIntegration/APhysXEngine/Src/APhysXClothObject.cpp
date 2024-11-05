
#include "APhysXStdAfx.h"

//----------------------------------------------------------------------
// make the following stuff internal...

class APhysXConstraintClothSubSimulationReport : public APhysXUserSubSimulationReport, public APhysXSimplePtrManager<APhysXClothObject>
{

public:

	APhysXConstraintClothSubSimulationReport()
	{
		// register myself to gPhysXSubSimulationReport 
		gPhysXSubSimulationReport->Register(this);
	}


	virtual void PreSubSimulation(float dt)
	{
		for(int i=0; i<m_arrElements.size(); i++)
		{
			m_arrElements[i]->ExecSubSimulationTask(dt);
		}
	}

};

static APhysXConstraintClothSubSimulationReport s_ConstraintClothSubSimulationReport;


//----------------------------------------------------------------------
// implementation of APhysXClothObject...

bool APhysXClothObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{

	if(!APhysXObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
		return false;

	APhysXClothObjectDesc& clothObjectDesc = *(APhysXClothObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;

	NxClothDesc clothDesc;
	if(clothObjectDesc.GetNxClothDesc(clothDesc))
	{
		
		// allocate the receive buffer...
		int iMaxVerNum = clothObjectDesc.mVertexNum * clothObjectDesc.mTearVertexNumFactor;
		int iMaxTriNum = clothObjectDesc.mTriangleNum;
		bool bTearable = (clothDesc.flags & NX_CLF_TEARABLE) != 0;

		AllocateReceiveBuffers(iMaxVerNum, iMaxTriNum, bTearable);

		clothDesc.meshData = m_ReceiveBuffers;
		clothDesc.globalPose = apxObjInstanceDesc.mGlobalPose;

		//--------------------------------------------------------------
		// revised by wenfeng, Mar. 25, 2010
		// force to use the global setting to control the cloth run on HW or SW

		// for Hardware PhysX support...
		/*
		if(gPhysXEngine->HasPhysXHardware())
		{
			if(gPhysXEngine->IsHardwarePhysXEnabled())
			{
				clothDesc.flags |= NX_CLF_HARDWARE;	

				//--------------------------------------------------------------
				// testing code and commented, Wenfeng, Mar. 31, 2009
				// to increase the solver iteration count of HW cloth to improve
				// the behaviour of HW cloths... this way is not good...
				// clothDesc.solverIterations = 100;
			}
			//--------------------
			// else case, it's up to the clothDesc's own flag NX_CLF_HARDWARE...
			else
			{
				clothDesc.flags &= ~NX_CLF_HARDWARE;	// disable the HW cloth...
			}
			
		}
		else
		{
			clothDesc.flags &= ~NX_CLF_HARDWARE;
		}
		*/

		if(gPhysXEngine->HasPhysXHardware() && gPhysXEngine->IsHardwarePhysXEnabled())
			clothDesc.flags |= NX_CLF_HARDWARE;	
		else
			clothDesc.flags &= ~NX_CLF_HARDWARE;

		
		//--------------------------------------------------------------
		// try some PhysX 2.8.4 features...
#if APHYSX_CURRENT_PHYSX_SDK_VERSION >= APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4)
	
		{
			clothDesc.flags |= NX_CLF_HARD_STRETCH_LIMITATION;
			clothDesc.hardStretchLimitationFactor = 1.f;
			// clothDesc.solverIterations = 10;
			// clothDesc.flags |= NX_CLF_UNTANGLING;
		}
		
#endif
		//--------------------------------------------------------------

#if APHYSX_SUPPORT_COMPARTMENT

		//--------------------------------------------------------------
		// compartment support, Wenfeng, April. 15, 2009
		// In two cases we will simulate the cloth in the compartment.
		// 1. mSimInCompartment == true
		// 2. we create the Hardware cloth
		
		// test create cloth in compartment...
		if(clothObjectDesc.mSimInCompartment || (clothDesc.flags & NX_CLF_HARDWARE))
			clothDesc.compartment = m_pAPhysXScene->GetClothCompartment();

#endif

		// check whether the cloth desc is valid or not...
		if(!clothDesc.isValid())
		{
			ReleaseReceiveBuffers();

			gPhysXLog->Log("Warning: Invalid NxClothDesc!");
			return false;
		}
		
		m_pAPhysXScene->LockWritingScene();
		m_pNxCloth = pAPhysXScene->GetNxScene()->createCloth(clothDesc);
		m_pAPhysXScene->UnlockWritingScene();

		if(m_pNxCloth)
		{
			m_pAPhysXScene->LockWritingScene();

			m_pNxCloth->userData = this;
			m_pNxClothMesh = clothDesc.clothMesh;
	
			// force to use APhysXScene's GetClothForceFieldMaterial()...
			m_pNxCloth->setForceFieldMaterial(pAPhysXScene->GetClothForceFieldMaterial());

#if APHYSX_CURRENT_PHYSX_SDK_VERSION >= APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4)

			if(clothDesc.flags & NX_CLF_HARD_STRETCH_LIMITATION && clothObjectDesc.mClothVertexConstraints != NULL)
			{

				// try to set constrain coefficients...
				int iVNum = clothObjectDesc.mVertexNum;
				NxClothConstrainCoefficients* cccBuf = new NxClothConstrainCoefficients[iVNum];
				for(int i=0; i<iVNum; ++i)
				{
#if 0
					cccBuf[i].maxDistance = clothObjectDesc.mConstrainMaxDistance;
					cccBuf[i].maxDistanceBias = clothObjectDesc.mConstrainMaxDistanceBias;
					cccBuf[i].collisionSphereRadius = clothObjectDesc.mConstrainCollisionSphereRadius;
					cccBuf[i].collisionSphereDistance = clothObjectDesc.mConstrainCollisionSphereDistance;

#else
					//---------------------------------------------------------------------
					// Note: in fact, only maxDistance take good effect on the cloth simulation,
					//       so, we currently just set this field of NxClothConstrainCoefficients.
					cccBuf[i].maxDistance = clothObjectDesc.mClothVertexConstraints[i].mMaxDistance;
					
					cccBuf[i].maxDistanceBias = 0.0f;
					cccBuf[i].collisionSphereRadius = 10.0f;
					cccBuf[i].collisionSphereDistance = 0.0f;

#endif
				}

				m_pNxCloth->setConstrainCoefficients(cccBuf);

				delete [] cccBuf;

				m_bEnableConstraints = true;

			}

#endif

			m_pAPhysXScene->UnlockWritingScene();

			if(IsConstraintsEnabled()) 
			{
				s_ConstraintClothSubSimulationReport.Register(this);
			}

			return true;
		}
		else
		{
			ReleaseReceiveBuffers();

			gPhysXLog->Log("Warning: Failed on creating NxCloth: create a NULL NxCloth.");
			return false;
		}
	
	}
	else
	{
		gPhysXLog->Log("Warning: Failed on getting NxClothDesc.");
		return false;
	}
}

void APhysXClothObject::OnReleaseBasicPhysXStuff()
{
	if(IsConstraintsEnabled())
	{
		s_ConstraintClothSubSimulationReport.UnRegister(this);
	}

	if(m_pAPhysXScene)
	{
		if(m_pNxCloth)
		{
			m_pAPhysXScene->LockWritingScene();

			// to avoid the async release of actors...
			m_pNxCloth->userData = NULL;

			m_pAPhysXScene->GetNxScene()->releaseCloth(*m_pNxCloth);
			m_pNxCloth = NULL;

			m_pAPhysXScene->UnlockWritingScene();
		}

#if 0
		// note: do not release the cloth mesh here due to the consideration of efficiency
		// if you want to release the specified cloth mesh forcedly, call APhysXMeshCooker's ReleaseClothMesh function
		if(m_pNxClothMesh)
		{
			//int iRefCounter = m_pNxClothMesh->getReferenceCount();
			gPhysXEngine->GetMeshCooker()->ForceReleaseClothMesh(m_pNxClothMesh);
			m_pNxClothMesh = NULL;
		}

#endif

		ReleaseReceiveBuffers();
	}

	if(m_BufConstraintPos) 
	{
		delete m_BufConstraintPos;
		m_BufConstraintPos = NULL;
	}

	if(m_BufConstraintPosVel) 
	{
		delete m_BufConstraintPosVel;
		m_BufConstraintPosVel = NULL;
	}

	APhysXObject::OnReleaseBasicPhysXStuff();
}

void APhysXClothObject::AllocateReceiveBuffers(int iMaxVertNum, int iMaxTriNum, bool bTearable)
{
	if(iMaxVertNum == 0 || iMaxTriNum == 0)
		return;


	NxVec3* pVertexBuffer = new NxVec3[iMaxVertNum];
	NxVec3* pNormalBuffer = new NxVec3[iMaxVertNum];

	//??? can be optimized??? if no tearing is allowed, no need to allocate the parent buffer!
	NxU32* pParentIndexBuffer = (bTearable)?(new NxU32 [iMaxVertNum]):NULL;

	// the number of triangles is constant, even if the cloth is torn
	int iMaxIndexNum = 3 * iMaxTriNum;
	NxU32* pIndexBuffer = new NxU32[iMaxIndexNum];
	
	m_ReceiveBuffers.verticesPosBegin         = (void *)pVertexBuffer;
	m_ReceiveBuffers.verticesNormalBegin      = (void *)pNormalBuffer;
	m_ReceiveBuffers.verticesPosByteStride    = sizeof(NxVec3);
	m_ReceiveBuffers.verticesNormalByteStride = sizeof(NxVec3);
	m_ReceiveBuffers.maxVertices              = iMaxVertNum;
	m_ReceiveBuffers.numVerticesPtr           = &m_uCurVertexNum;

	m_ReceiveBuffers.indicesBegin             = (void *)pIndexBuffer;
	m_ReceiveBuffers.indicesByteStride        = sizeof(NxU32);
	m_ReceiveBuffers.maxIndices               = iMaxIndexNum;
	m_ReceiveBuffers.numIndicesPtr            = &m_uCurIndexNum;

	// the parent index information would be needed if we used textured cloth
	if(bTearable)
	{
		m_ReceiveBuffers.parentIndicesBegin       = (void *) pParentIndexBuffer;
		m_ReceiveBuffers.parentIndicesByteStride  = sizeof(NxU32);
		m_ReceiveBuffers.maxParentIndices         = iMaxVertNum;
		m_ReceiveBuffers.numParentIndicesPtr      = &m_uCurParentIndexNum;
	}

	m_ReceiveBuffers.dirtyBufferFlagsPtr = &m_uMeshDirtyFlags;

	m_uCurVertexNum = 0;
	m_uCurIndexNum = 0;
	m_uCurParentIndexNum = 0;
	m_uMeshDirtyFlags = 0;
}

void APhysXClothObject::ReleaseReceiveBuffers()
{
	NxVec3* pVertexBuffer = (NxVec3 *)m_ReceiveBuffers.verticesPosBegin;
	NxVec3* pNormalBuffer = (NxVec3 *)m_ReceiveBuffers.verticesNormalBegin;
	NxU32* pParentIndexBuffer = (NxU32 *) m_ReceiveBuffers.parentIndicesBegin;
	NxU32* pIndexBuffer = (NxU32 *) m_ReceiveBuffers.indicesBegin;

	if(pVertexBuffer) delete [] pVertexBuffer;
	if(pNormalBuffer) delete [] pNormalBuffer;
	if(pParentIndexBuffer) delete [] pParentIndexBuffer;
	if(pIndexBuffer) delete [] pIndexBuffer;

}

void APhysXClothObject::SetPose(const NxMat34& globalPose)
{
	//  do nothing
}

NxMat34 APhysXClothObject::GetPose() const
{
	// return the aabb's center pos...
	NxMat34 matPose;

	if(m_pNxCloth)
	{
		NxBounds3 aabb;
		m_pNxCloth->getWorldBounds(aabb);
		aabb.getCenter(matPose.t);
	}

	return matPose;
}

void APhysXClothObject::GetAABB(NxBounds3& aabb) const
{
	aabb.setEmpty();

	if(m_pNxCloth)
	{
		m_pNxCloth->getWorldBounds(aabb);
	}
}

void APhysXClothObject::SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef)
{
	APhysXObject::SetCollisionChannel(apxCollisionChannel, bChangeChannelRef);

	if(m_pAPhysXScene && m_pNxCloth)
	{
		NxGroupsMask groupMask = m_pAPhysXScene->GetCollisionChannelManager()->GetChannelGroupMask(apxCollisionChannel);

		m_pAPhysXScene->LockWritingScene();
		m_pNxCloth->setGroupsMask(groupMask);
		m_pAPhysXScene->UnlockWritingScene();
	}
}

void APhysXClothObject::SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup)
{
	APhysXObject::SetCollisionGroup(apxCollisionGroup);

	if(m_pAPhysXScene && m_pNxCloth)
	{
		m_pAPhysXScene->LockWritingScene();
		m_pNxCloth->setGroup(apxCollisionGroup);
		m_pAPhysXScene->UnlockWritingScene();
	}

}

void APhysXClothObject::EnableCollision(bool bEnable /* = true */)
{

	APhysXObject::EnableCollision(bEnable);

	if(m_pNxCloth)
	{
		NxU32 uFlags = m_pNxCloth->getFlags();
		
		if(bEnable)
			uFlags &= ~NX_CLF_DISABLE_COLLISION;
		else
			uFlags |= NX_CLF_DISABLE_COLLISION;

		m_pAPhysXScene->LockWritingScene();
		m_pNxCloth->setFlags(uFlags);
		m_pAPhysXScene->UnlockWritingScene();
	}
	

}

void APhysXClothObject::EnableDebugRender(bool bEnable)
{
	APhysXObject::EnableDebugRender(bEnable);

	if(m_pNxCloth)
	{
		NxU32 flags = m_pNxCloth->getFlags();
		
		if(bEnable)
			flags |= NX_CLF_VISUALIZATION;
		else
			flags &= ~NX_CLF_VISUALIZATION;

		m_pAPhysXScene->LockWritingScene();
		m_pNxCloth->setFlags(flags);
		m_pAPhysXScene->UnlockWritingScene();
	}
}

void APhysXClothObject::PutToSleep()
{
	if(m_pNxCloth)
	{
		m_pAPhysXScene->LockWritingScene();	
		m_pNxCloth->putToSleep();
		m_pAPhysXScene->UnlockWritingScene();
	}
}

void APhysXClothObject::WakeUp() 
{
	if(m_pNxCloth)
	{
		m_pAPhysXScene->LockWritingScene();	
		m_pNxCloth->wakeUp();
		m_pAPhysXScene->UnlockWritingScene();
	}
}	

void APhysXClothObject::SetWindAcceleration(const NxVec3& vWindAcceleration)
{
	if(m_pNxCloth)
	{
		m_pAPhysXScene->LockWritingScene();	
		m_pNxCloth->setWindAcceleration(vWindAcceleration);
		m_pAPhysXScene->UnlockWritingScene();
	}
	
}


void APhysXClothObject::SetConstrainPositions(NxVec3* vPosBuf)
{

#if APHYSX_CURRENT_PHYSX_SDK_VERSION >= APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4)

	// do the real "setConstrainPositions()" stuff until in the function "ExecSubSimulationTask()"

#if 0
	if(m_pNxCloth)
	{
		m_pAPhysXScene->LockWritingScene();

		m_pNxCloth->setConstrainPositions((void *)vPosBuf);

		m_pAPhysXScene->UnlockWritingScene();
	}

#else

	APHYSX_ASSERT(m_pNxCloth && IsConstraintsEnabled());

	int iVertNum = m_pNxCloth->getNumberOfParticles();

	bool bResetBufConstraintPos = false;
	if(!m_BufConstraintPos)
	{	
		m_BufConstraintPos = new NxVec3 [iVertNum];
		bResetBufConstraintPos = true;
	}
	
	if(!m_BufConstraintPosVel) m_BufConstraintPosVel = new NxVec3 [iVertNum];

	if(m_pAPhysXScene->GetSimFrameCount() - m_uLastSetConstraintPosFrame != 1)
	{
		// if there is not per-frame calling of this function, we just reset the buffer positions...
		bResetBufConstraintPos = true;
	}

	if(bResetBufConstraintPos)
	{
		memcpy(m_BufConstraintPos, vPosBuf, iVertNum * sizeof(NxVec3));
	}
	

	float fInvDt = (GetPhysXScene()->GetExactSimDt() > 0.0f)? (1.0f/GetPhysXScene()->GetExactSimDt()): (1.0f/GetPhysXScene()->GetSimulateTimeStep());

	for(int i = 0; i<iVertNum; ++i)
	{
		m_BufConstraintPosVel[i] = ( vPosBuf[i] - m_BufConstraintPos[i] ) * fInvDt;
	}

#endif

	m_uLastSetConstraintPosFrame = GetPhysXScene()->GetSimFrameCount();
	m_fSubSimulationDtCounter = 0.0f;

#endif

}

void APhysXClothObject::SetConstrainNormals(NxVec3* vNBuf)
{

#if APHYSX_CURRENT_PHYSX_SDK_VERSION >= APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4)

	if(m_pNxCloth && !(m_apxObjectFlags & APX_OBJFLAG_CLOTH_DISABLE_CONSTRAINTNORMAL))
	{
		m_pAPhysXScene->LockWritingScene();

		m_pNxCloth->setConstrainNormals((void *)vNBuf);

		m_pAPhysXScene->UnlockWritingScene();
	}

#endif

}

void APhysXClothObject::ExecSubSimulationTask(float dt)
{

#if APHYSX_CURRENT_PHYSX_SDK_VERSION >= APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4)

	if(m_pNxCloth && m_BufConstraintPos && m_BufConstraintPosVel)
	{
		if(m_fSubSimulationDtCounter < GetPhysXScene()->GetExactSimDt())
		{
			for(int i=0 ; i<GetVertexNum(); ++i)
			{
				m_BufConstraintPos[i] += m_BufConstraintPosVel[i] * dt;
			}

			m_pAPhysXScene->LockWritingScene();

			m_pNxCloth->setConstrainPositions((void *)m_BufConstraintPos);

			m_pAPhysXScene->UnlockWritingScene();

		}

		m_fSubSimulationDtCounter += dt;
	}

#endif

}

