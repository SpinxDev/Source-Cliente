
#include "APhysXStdAfx.h"



bool APhysXFluidObjectDesc::Save(NxStream* pNxStream)
{
	if(!APhysXObjectDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeBuffer(&mFluidParameters, sizeof(APhysXFluidParameters));
	pNxStream->storeDword(mParticleWriteDataFlag);

	return true;
}

bool APhysXFluidObjectDesc::Load(NxStream* pNxStream)
{
	if(!APhysXObjectDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{
		Release();

		pNxStream->readBuffer(&mFluidParameters, sizeof(APhysXFluidParameters));

		mParticleWriteDataFlag = pNxStream->readDword();

		return true;
	}
	else
		return false;
	
}

/*
APhysXObjectDesc* APhysXFluidObjectDesc::Clone() const
{
	APhysXObjectDesc* pClonedObjectDesc = new APhysXFluidObjectDesc;

#define MAKE_DIRECT_COPY
#ifdef	MAKE_DIRECT_COPY

	*(APhysXFluidObjectDesc *)pClonedObjectDesc = *this;

#else

	//??? seems no need to clone like this...

	// clone parents' data
	*pClonedObjectDesc = *(APhysXObjectDesc* )this;

	APhysXFluidObjectDesc* pClonedFluidObjectDesc = (APhysXFluidObjectDesc*) pClonedObjectDesc;
	pClonedFluidObjectDesc->mFluidParameters = mFluidParameters;
	pClonedFluidObjectDesc->mParticleWriteDataFlag = mParticleWriteDataFlag;

#endif

	return pClonedObjectDesc;

}
*/


bool APhysXFluidObjectDesc::GetNxFluidDesc(NxFluidDesc& fluidDesc)
{
	
	fluidDesc.maxParticles						= mFluidParameters.mMaxParticles;
	fluidDesc.numReserveParticles				= mFluidParameters.mNumReserveParticles;
	fluidDesc.restParticlesPerMeter				= mFluidParameters.mRestParticlesPerMeter;
	fluidDesc.restDensity						= mFluidParameters.mRestDensity;
	fluidDesc.kernelRadiusMultiplier			= mFluidParameters.mKernelRadiusMultiplier;
	fluidDesc.motionLimitMultiplier				= mFluidParameters.mMotionLimitMultiplier;
	fluidDesc.collisionDistanceMultiplier		= mFluidParameters.mCollisionDistanceMultiplier;
	fluidDesc.packetSizeMultiplier				= mFluidParameters.mPacketSizeMultiplier;
	fluidDesc.stiffness							= mFluidParameters.mStiffness;
	fluidDesc.viscosity							= mFluidParameters.mViscosity;
	fluidDesc.surfaceTension					= mFluidParameters.mSurfaceTension;
	fluidDesc.damping							= mFluidParameters.mDamping;
	fluidDesc.fadeInTime						= mFluidParameters.mFadeInTime;
	fluidDesc.externalAcceleration				= mFluidParameters.mExternalAcceleration;
	fluidDesc.projectionPlane					= mFluidParameters.mProjectionPlane;
	fluidDesc.restitutionForStaticShapes		= mFluidParameters.mRestitutionForStaticShapes;
	fluidDesc.dynamicFrictionForStaticShapes	= mFluidParameters.mDynamicFrictionForStaticShapes;
	fluidDesc.staticFrictionForStaticShapes		= mFluidParameters.mStaticFrictionForStaticShapes;
	fluidDesc.attractionForStaticShapes			= mFluidParameters.mAttractionForStaticShapes;
	fluidDesc.restitutionForDynamicShapes		= mFluidParameters.mRestitutionForDynamicShapes;
	fluidDesc.dynamicFrictionForDynamicShapes	= mFluidParameters.mDynamicFrictionForDynamicShapes;
	fluidDesc.staticFrictionForDynamicShapes	= mFluidParameters.mStaticFrictionForDynamicShapes;
	fluidDesc.attractionForDynamicShapes		= mFluidParameters.mAttractionForDynamicShapes;
	fluidDesc.collisionResponseCoefficient		= mFluidParameters.mCollisionResponseCoefficient;

	fluidDesc.simulationMethod					= mFluidParameters.mSimulationMethod;
	

	//-------------------------------------------------------------------------
	// Note:
	// we should consider that the default simulation method of NxFluid is 
	// NX_F_NO_PARTICLE_INTERACTION this will gain great simulation efficiency...
	// fluidDesc.simulationMethod					= NX_F_NO_PARTICLE_INTERACTION;

	fluidDesc.collisionMethod					= mFluidParameters.mCollisionMethod;
	fluidDesc.forceFieldMaterial				= mFluidParameters.mForceFieldMaterial;
	fluidDesc.flags								= mFluidParameters.mFlags;

	fluidDesc.collisionGroup				= mCollisionGroup;

	return true;

}

//-----------------------------------------------------------------------------------------
// APhysXOrientedParticleObjectDesc

Begin_Save(APhysXOrientedParticleObjectDesc, APhysXFluidObjectDesc)

	pNxStream->storeDword(mSpinType);
	pNxStream->storeDword(mInitialOrt);
	pNxStream->storeFloat(mMaxAngularVel);
	pNxStream->storeFloat(mSpinRadius);

End_Save()


Begin_Load(APhysXOrientedParticleObjectDesc, APhysXFluidObjectDesc)

	if(m_StreamObjVer >= 0xcc000001)
	{
		mSpinType = (APhysXOrientedParticleSpinType)pNxStream->readDword();	
		mInitialOrt = (AphysXOrientedParticleInitialOrt)pNxStream->readDword();
		mMaxAngularVel = pNxStream->readFloat();
		mSpinRadius = pNxStream->readFloat();
	}


End_Load()