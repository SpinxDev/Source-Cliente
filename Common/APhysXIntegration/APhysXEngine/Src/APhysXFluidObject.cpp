
#include "APhysXStdAfx.h"


bool APhysXFluidObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	if(!APhysXObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
		return false;

	APhysXFluidObjectDesc& fluidObjectDesc = *(APhysXFluidObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;
	
	NxFluidDesc fluidDesc;
	// fill the parameters of the fluid
	fluidObjectDesc.GetNxFluidDesc(fluidDesc);
	
	// fill the write data of the fluid
	AllocateParticleWriteData(fluidObjectDesc.mFluidParameters.mMaxParticles, fluidObjectDesc.mParticleWriteDataFlag);

	fluidDesc.particlesWriteData.numParticlesPtr = &m_uParticleNum;
	
	fluidDesc.particlesWriteData.bufferPos = (NxF32*)m_BufferPos;
	fluidDesc.particlesWriteData.bufferPosByteStride = sizeof(NxVec3);

	fluidDesc.particlesWriteData.bufferVel = (NxF32*)m_BufferVel;
	fluidDesc.particlesWriteData.bufferVelByteStride = sizeof(NxVec3);

	fluidDesc.particlesWriteData.bufferLife = (NxF32*)m_BufferLife;
	fluidDesc.particlesWriteData.bufferLifeByteStride = sizeof(float);
	
	fluidDesc.particlesWriteData.bufferDensity = (NxF32*)m_BufferDensity;
	fluidDesc.particlesWriteData.bufferDensityByteStride = sizeof(float);

	fluidDesc.particlesWriteData.bufferId = (NxU32*)m_BufferId;
	fluidDesc.particlesWriteData.bufferIdByteStride = sizeof(APhysXU32);

	fluidDesc.particlesWriteData.bufferFlag = (NxU32*)m_BufferFlag;
	fluidDesc.particlesWriteData.bufferFlagByteStride = sizeof(APhysXU32);

	fluidDesc.particlesWriteData.bufferCollisionNormal = (NxF32*)m_BufferCollisionNormal;
	fluidDesc.particlesWriteData.bufferCollisionNormalByteStride = sizeof(NxVec3);

	if(fluidObjectDesc.mParticleWriteDataFlag & APX_PWDFLAG_ID)
	{
		fluidDesc.particleCreationIdWriteData.numIdsPtr = &m_uCreationIdNum;
		fluidDesc.particleDeletionIdWriteData.numIdsPtr = &m_uDeletionIdNum;
	}
	
	fluidDesc.particleCreationIdWriteData.bufferId = (NxU32*) m_BufferCreationId;
	fluidDesc.particleCreationIdWriteData.bufferIdByteStride = sizeof(APhysXU32);

	fluidDesc.particleDeletionIdWriteData.bufferId = (NxU32*) m_BufferDeletionId;
	fluidDesc.particleDeletionIdWriteData.bufferIdByteStride = sizeof(APhysXU32);


	//-----------------------------------------------------------------------
	// revised by Wenfeng, June 24, 2013
	// refer to :
	//		NxErrorStream => ..\..\Physics\src\fluids\NpFluid.cpp (738) : warning : NxFluid::setGroupsMask: Collision filtering for fluids shouldn't be changed after creation
	// set collision channel here...
	fluidDesc.groupsMask = m_pAPhysXScene->GetCollisionChannelManager()->GetChannelGroupMask(m_CollisionChannel);

	//-----------------------------------------------------------------------
	//??? ignore the following parameters at present...
	/*
		fluidDesc.initialParticleData
		fluidDesc.fluidPacketData
		fluidDesc.userData
		fluidDesc.name
		fluidDesc.compartment
	*/

#if APHYSX_SUPPORT_COMPARTMENT

	//--------------------------------------------------------------
	// compartment support, Wenfeng, April. 15, 2009
	// fluid will always be simulated in my specified compartment...
	fluidDesc.compartment = m_pAPhysXScene->GetFluidCompartment();

	//--------------------------------------------------------------
	// revised by wenfeng, March.25, 2010
	// if we have HW PhysX support, but disable HW, we will not use the default created 
	// fluid compartment by APhysXScene since that compartment is created in HW mode.
	if(gPhysXEngine->HasPhysXHardware() && !gPhysXEngine->IsHardwarePhysXEnabled())
		fluidDesc.compartment = NULL;

#endif

	//--------------------------------------------------------------
	// revised by wenfeng, Mar. 25, 2010
	// force to use the global setting to control the cloth run on HW or SW

	/*
	if(gPhysXEngine->HasPhysXHardware())
	{
		if(gPhysXEngine->IsHardwarePhysXEnabled())
			fluidDesc.flags |= NX_FF_HARDWARE;	

		//--------------------		
		// else case, it's up to the fluidDesc's own flag NX_FF_HARDWARE...
		else
		{
			fluidDesc.flags &= ~NX_FF_HARDWARE;		// disable the HW Fluid...
		}
	}
	else
	{
		fluidDesc.flags &= ~NX_FF_HARDWARE;
	}
	*/

	if(gPhysXEngine->HasPhysXHardware() && gPhysXEngine->IsHardwarePhysXEnabled())
		fluidDesc.flags |= NX_FF_HARDWARE;
	else
		fluidDesc.flags &= ~NX_FF_HARDWARE;

	//--------------------------------------------------------------

	m_pAPhysXScene->LockWritingScene();
	m_pNxFluid = m_pAPhysXScene->GetNxScene()->createFluid(fluidDesc);
	m_pAPhysXScene->UnlockWritingScene();

	if(m_pNxFluid)
	{
		m_pAPhysXScene->LockWritingScene();

		m_pNxFluid->userData = this;	

		// force to use APhysXScene's GetFluidForceFieldMaterial()...
		m_pNxFluid->setForceFieldMaterial(pAPhysXScene->GetFluidForceFieldMaterial());

		m_pAPhysXScene->UnlockWritingScene();

		return true;
	}
	else
	{
		ReleaseParticleWriteData();

		gPhysXLog->Log("Warning: Failed on creating NxFluid: create a NULL NxFluid.");
		return false;
	}


}

void APhysXFluidObject::OnReleaseBasicPhysXStuff()
{
	if(m_pAPhysXScene && m_pNxFluid)
	{
		m_pAPhysXScene->LockWritingScene();

		// to avoid the async release of actors...
		m_pNxFluid->userData = NULL;

		m_pAPhysXScene->GetNxScene()->releaseFluid(*m_pNxFluid);
		m_pNxFluid = NULL;

		m_pAPhysXScene->UnlockWritingScene();

		ReleaseParticleWriteData();
	}

	APhysXObject::OnReleaseBasicPhysXStuff();
}

void APhysXFluidObject::AllocateParticleWriteData(int iMaxParticleNum, APhysXU32 apxPWDFlag)
{
	if(apxPWDFlag & APX_PWDFLAG_POS)
	{
		m_BufferPos = new NxVec3[iMaxParticleNum];
	}

	if(apxPWDFlag & APX_PWDFLAG_VEL)
	{
		m_BufferVel = new NxVec3[iMaxParticleNum];
	}

	if(apxPWDFlag & APX_PWDFLAG_LIFE)
	{
		m_BufferLife = new float[iMaxParticleNum];
	}

	if(apxPWDFlag & APX_PWDFLAG_DENSITY)
	{
		m_BufferDensity = new float[iMaxParticleNum];
	}

	if(apxPWDFlag & APX_PWDFLAG_ID)
	{
		m_BufferId = new APhysXU32[iMaxParticleNum];
		
		// allocate memory for creation ids and deletion ids
		m_BufferCreationId = new APhysXU32[iMaxParticleNum];
		m_BufferDeletionId = new APhysXU32[iMaxParticleNum];
	}

	if(apxPWDFlag & APX_PWDFLAG_FLAG)
	{
		m_BufferFlag = new APhysXU32[iMaxParticleNum];
	}

	if(apxPWDFlag & APX_PWDFLAG_COLLISIONNORMAL)
	{
		m_BufferCollisionNormal = new NxVec3[iMaxParticleNum];
	}

}

void APhysXFluidObject::ReleaseParticleWriteData()
{
	if(m_BufferPos)
	{
		delete [] m_BufferPos;
		m_BufferPos = NULL;
	}

	if(m_BufferVel)
	{
		delete [] m_BufferVel;
		m_BufferVel = NULL;
	}

	if(m_BufferLife)
	{
		delete [] m_BufferLife;
		m_BufferLife = NULL;
	}

	if(m_BufferDensity)
	{
		delete [] m_BufferDensity;
		m_BufferDensity = NULL;
	}

	if(m_BufferId)
	{
		delete [] m_BufferId;
		m_BufferId = NULL;
	}

	if(m_BufferFlag)
	{
		delete [] m_BufferFlag;
		m_BufferFlag = NULL;
	}

	if(m_BufferCollisionNormal)
	{
		delete [] m_BufferCollisionNormal;
		m_BufferCollisionNormal = NULL;
	}

	if(m_BufferCreationId)
	{
		delete [] m_BufferCreationId;
		m_BufferCreationId = NULL;
	}

	if(m_BufferDeletionId)
	{
		delete [] m_BufferDeletionId;
		m_BufferDeletionId = NULL;
	}
	
}

void APhysXFluidObject::SetPose(const NxMat34& globalPose)
{
	//  do nothing
}

NxMat34 APhysXFluidObject::GetPose() const
{
	// return the aabb's center pos...
	NxMat34 matPose;

	if(m_pNxFluid)
	{
		NxBounds3 aabb;
		m_pNxFluid->getWorldBounds(aabb);
		aabb.getCenter(matPose.t);
	}

	return matPose;

}

void APhysXFluidObject::GetAABB(NxBounds3& aabb) const
{
	aabb.setEmpty();

	if(m_pNxFluid)
	{
		m_pNxFluid->getWorldBounds(aabb);
	}
}

void APhysXFluidObject::SpawnParticles(int iParticleNum, const NxVec3* bufferPos, const NxVec3& vVelocity, float fLifetime)
{
	if(m_pNxFluid && iParticleNum > 0)
	{

// refer to NxFluid.h, line #148, the comment of virtual NxU32 addParticles(...)
#define MAX_ADDED_PARTICLE_NUM 4096			

		NxU32 uAddedParticleNum = (MAX_ADDED_PARTICLE_NUM > iParticleNum )?iParticleNum : MAX_ADDED_PARTICLE_NUM;


		NxVec3* bufferVel = new NxVec3[uAddedParticleNum];
		float* bufferLife = new float[uAddedParticleNum];
		for(int i=0; i<uAddedParticleNum; i++)
		{
			bufferVel[i] = vVelocity;
			bufferLife[i] = fLifetime;

		}

		//----------------------------------------------------------------------------------------
		// note here:
		//		in fact, we only need set the bufferPos, bufferVel, and bufferLife of NxParticleData
		//	to call the function addParticles(const NxParticleData& pData, ...)

		NxParticleData addedParticleData;
		addedParticleData.numParticlesPtr = &uAddedParticleNum;

		addedParticleData.bufferPos = (NxF32*)bufferPos;
		addedParticleData.bufferPosByteStride = sizeof(NxVec3);

		addedParticleData.bufferVel = (NxF32*)bufferVel;
		addedParticleData.bufferVelByteStride = sizeof(NxVec3);

		addedParticleData.bufferLife = (NxF32*)bufferLife;
		addedParticleData.bufferLifeByteStride = sizeof(float);

		m_pNxFluid->addParticles(addedParticleData);

		delete [] bufferVel;
		delete [] bufferLife;

	}

}

void APhysXFluidObject::SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef)
{
	//-----------------------------------------------------------------------
	// revised by Wenfeng, June 24, 2013
	// refer to :
	//		NxErrorStream => ..\..\Physics\src\fluids\NpFluid.cpp (738) : warning : NxFluid::setGroupsMask: Collision filtering for fluids shouldn't be changed after creation
	// so, do nothing here if users runtime change collision channel of fluid object...

#if 0

	APhysXObject::SetCollisionChannel(apxCollisionChannel, bChangeChannelRef);

	if(m_pAPhysXScene && m_pNxFluid)
	{
		NxGroupsMask groupMask = m_pAPhysXScene->GetCollisionChannelManager()->GetChannelGroupMask(apxCollisionChannel);

		m_pAPhysXScene->LockWritingScene();
		m_pNxFluid->setGroupsMask(groupMask);
		m_pAPhysXScene->UnlockWritingScene();
	}

#endif

}

void APhysXFluidObject::SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup)
{
	APhysXObject::SetCollisionGroup(apxCollisionGroup);

	if(m_pAPhysXScene && m_pNxFluid)
	{
		m_pAPhysXScene->LockWritingScene();
		m_pNxFluid->setGroup(apxCollisionGroup);
		m_pAPhysXScene->UnlockWritingScene();
	}

}

void APhysXFluidObject::EnableDebugRender(bool bEnable)
{
	APhysXObject::EnableDebugRender(bEnable);

	if(m_pAPhysXScene && m_pNxFluid)
	{
		m_pAPhysXScene->LockWritingScene();

		if(bEnable)
			m_pNxFluid->setFlag(NX_FF_VISUALIZATION, true);
		else
			m_pNxFluid->setFlag(NX_FF_VISUALIZATION, false);

		m_pAPhysXScene->UnlockWritingScene();
	}
}

void APhysXFluidObject::PutToSleep()
{
	if(m_pNxFluid)
	{
		m_pAPhysXScene->LockWritingScene();
		m_pNxFluid->setFlag(NX_FF_ENABLED, false);
		m_pAPhysXScene->UnlockWritingScene();
	}
}

void APhysXFluidObject::WakeUp() 
{
	if(m_pNxFluid)
	{
		m_pAPhysXScene->LockWritingScene();
		m_pNxFluid->setFlag(NX_FF_ENABLED, true);
		m_pAPhysXScene->UnlockWritingScene();
	}
}	

float APhysXFluidObject::GetCollisionRadius() const
{
	if(m_pNxFluid)
	{
		return m_pNxFluid->getCollisionDistanceMultiplier()/m_pNxFluid->getRestParticlesPerMeter();
	}

	return 0.0f;

}

//-----------------------------------------------------------------------------------------
// APhysXOrientedParticleObject

#define MAX_ANGULAR_VELOCITY_MAG (APHYSX_PI * 2.0f)

bool APhysXOrientedParticleObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	// init the members before the base class's function so that the AllocateParticleWriteData() function can handle the different case...
	APhysXOrientedParticleObjectDesc& OrtParticleObjectDesc = *(APhysXOrientedParticleObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;
	m_SpinType = OrtParticleObjectDesc.mSpinType;
	m_InitialOrt = OrtParticleObjectDesc.mInitialOrt;
	m_fMaxAngularVel = OrtParticleObjectDesc.mMaxAngularVel;
	m_fSpinRadius = OrtParticleObjectDesc.mSpinRadius;


	if(!APhysXFluidObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
		return false;

	return true;

}


void APhysXOrientedParticleObject::AllocateParticleWriteData(int iMaxParticleNum, APhysXU32 apxPWDFlag)
{
	// force to retrieve the vel info...
	apxPWDFlag |= APX_PWDFLAG_VEL;

	// force to track the lifetime of particles...
	apxPWDFlag |= APX_PWDFLAG_ID;

	// force to set the flag to retrieve the collision normal data...
	apxPWDFlag |= APX_PWDFLAG_COLLISIONNORMAL;

	// call parents' function
	APhysXFluidObject::AllocateParticleWriteData(iMaxParticleNum, apxPWDFlag);

	// alloc orientation informations and initialize them...
	m_BufferOrientation = new NxQuat[iMaxParticleNum];

	for(int i=0; i<iMaxParticleNum; i++)
	{
		if(m_InitialOrt == APX_OP_IORT_ID)
			m_BufferOrientation[i].id();
		else if(m_InitialOrt == APX_OP_IORT_RANDOM)
			m_BufferOrientation[i].random();
	}

	if(m_SpinType == APX_OP_SPINTYPE_FLAT)
	{
		// alloc angular velocities buffer...
		m_BufferAngularVel = new NxVec3[iMaxParticleNum];

		// for the case of flat spin type, we must limit the angular velocity mag...
		if(m_fMaxAngularVel < 0)
		{
			m_fMaxAngularVel = MAX_ANGULAR_VELOCITY_MAG;
		}

		// then init them...
		for(int i=0; i<iMaxParticleNum; i++)
		{
			m_BufferAngularVel[i] = APhysXUtilLib::GenRndAngVel(m_fMaxAngularVel);
		}

	}

}

void APhysXOrientedParticleObject::ReleaseParticleWriteData()
{
	// call parents' function
	APhysXFluidObject::ReleaseParticleWriteData();

	if(m_BufferOrientation)
	{
		delete [] m_BufferOrientation;
		m_BufferOrientation = NULL;
	}

	if(m_BufferAngularVel)
	{
		delete [] m_BufferAngularVel;
		m_BufferAngularVel = NULL;
	}
}

void APhysXOrientedParticleObject::PostSimulate(float dt)
{
	APhysXFluidObject::PostSimulate(dt);

	if(!m_pNxFluid) return;

	//-------------------------------------------------------------
	// Note:
	//	We should track the IDs to find the correct particle 
	
	// firstly, init the new created particles' orientation
	const APhysXU32* creatIDs = GetCreatedParticleIds();
	for(int i=0; i<GetCreatedIdNum(); i++)
	{
		APhysXU32 uID = creatIDs[i];

		if(m_InitialOrt == APX_OP_IORT_ID)
			m_BufferOrientation[uID].id();
		else if(m_InitialOrt == APX_OP_IORT_RANDOM)
			m_BufferOrientation[uID].random();

		if(m_BufferAngularVel && m_SpinType == APX_OP_SPINTYPE_FLAT)
		{
			m_BufferAngularVel[uID] = APhysXUtilLib::GenRndAngVel(m_fMaxAngularVel);
		}
	}

	// then, compute all particles' orientation
	const APhysXU32* IDs = GetParticleIDs();
	NxVec3 vZero(0.0f);
	const float fEpsilon = 1e-3f;

	float fR = (m_fSpinRadius < 0.0f)? GetCollisionRadius() : m_fSpinRadius;
	// const float fFactor = 180 * dt / (fR * APHYSX_PI);				// compute this factor only once
	const float fFactor =  dt / fR ;				// compute this factor only once

	for(int i=0; i<GetParticleNum(); i++)
	{
		APhysXU32 uID = IDs[i];
		NxVec3 vLV = m_BufferVel[i];
		APHYSX_ASSERT(m_BufferCollisionNormal);
		NxVec3 vN = m_BufferCollisionNormal[i];
		// NxVec3 vN(0, 1.0f, 0);

		if(m_SpinType == APX_OP_SPINTYPE_SPHERE)
		{
			if(vLV.equals(vZero,fEpsilon)) continue;

			if(vN.equals(vZero,fEpsilon)) continue;

			float fV = vLV.normalize();
			NxVec3 vAxis;
			vAxis.cross(vN, vLV);
			fV *= fFactor;

			// clamp the max angular velocity
			if(m_fMaxAngularVel>0.0f && fV > m_fMaxAngularVel)
				fV = m_fMaxAngularVel;

			NxQuat q;
			q.fromAngleAxisFast(fV, vAxis);

			m_BufferOrientation[uID] = q * m_BufferOrientation[uID];
			m_BufferOrientation[uID].normalize();

		}
		else if(m_SpinType == APX_OP_SPINTYPE_FLAT)
		{
			if(vN.equals(vZero,fEpsilon))
			{
				// fly on air...
				NxVec3 vRot = m_BufferAngularVel[uID];
				float fMag = vRot.normalize();
				NxQuat q;
				q.fromAngleAxisFast(fMag * dt, vRot);

				m_BufferOrientation[uID] = q * m_BufferOrientation[uID];
				m_BufferOrientation[uID].normalize();

			}
			else
			{
				// land on ground... we should correct the up direction(Y-axis)
				NxMat33 mtRot(m_BufferOrientation[uID]);
				NxVec3 vUp(mtRot.getColumn(1));					// we regard Y-axis (the column #1 vector) as the up direction vector
				if(vUp.y < 0.0f) vUp = -vUp;

				NxVec3 vOrtCorrection;
				vOrtCorrection.cross(vUp, vN);

				float fMag = vOrtCorrection.normalize();
				if(NxMath::equals(fMag, 0.0f, fEpsilon))
					continue;

				if(fMag > m_fMaxAngularVel * dt)
				{
					fMag = m_fMaxAngularVel * dt;
				}
				
				NxQuat q;
				q.fromAngleAxisFast(fMag, vOrtCorrection);
				
				m_BufferOrientation[uID] = q * m_BufferOrientation[uID];
				m_BufferOrientation[uID].normalize();

			}

		}

	}

}
