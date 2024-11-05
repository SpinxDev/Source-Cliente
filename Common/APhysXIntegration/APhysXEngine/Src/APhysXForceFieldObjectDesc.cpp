
#include "APhysXStdAfx.h"


void APhysXForceFieldLinearKernelParameters::GetNxForceFieldLinearKernelDesc(NxForceFieldLinearKernelDesc& fflKernelDesc)
{
	fflKernelDesc.constant					=		mConstant; 
	fflKernelDesc.positionMultiplier		=		mPositionMultiplier;
	fflKernelDesc.positionTarget			=		mPositionTarget;
	fflKernelDesc.velocityMultiplier		=		mVelocityMultiplier;
	fflKernelDesc.velocityTarget			=		mVelocityTarget;
	fflKernelDesc.falloffLinear				=		mFalloffLinear;
	fflKernelDesc.falloffQuadratic			=		mFalloffQuadratic;
	fflKernelDesc.noise						=		mNoise;
	fflKernelDesc.torusRadius				=		mTorusRadius;

}

//-------------------------------------------------------------------------------------

bool APhysXForceFieldObjectDesc::Save(NxStream* pNxStream)
{
	
	if(!APhysXObjectDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeBuffer(&mFFLKernelParameters, sizeof(APhysXForceFieldLinearKernelParameters));
	
	if(!mIncludeShapes.Save(pNxStream))
		return false;

	pNxStream->storeDword(mFFCoordinates);

	pNxStream->storeDword(mRBType);
	pNxStream->storeDword(mClothType);
	pNxStream->storeDword(mFluidType);
	pNxStream->storeDword(mSBType);
	
	pNxStream->storeFloat(mRBForceScale);
	pNxStream->storeFloat(mClothForceScale);
	pNxStream->storeFloat(mFluidForceScale);
	pNxStream->storeFloat(mSBForceScale);

	pNxStream->storeDword(mFlags);


	return true;
}

bool APhysXForceFieldObjectDesc::Load(NxStream* pNxStream)
{
	if(!APhysXObjectDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

		pNxStream->readBuffer(&mFFLKernelParameters, sizeof(APhysXForceFieldLinearKernelParameters));

		mIncludeShapes.Release();
		if(!mIncludeShapes.Load(pNxStream))
			return false;

		mFFCoordinates = (NxForceFieldCoordinates)pNxStream->readDword();

		mRBType = (NxForceFieldType)pNxStream->readDword();
		mClothType = (NxForceFieldType)pNxStream->readDword();
		mFluidType = (NxForceFieldType)pNxStream->readDword();
		mSBType = (NxForceFieldType)pNxStream->readDword();

		mRBForceScale = pNxStream->readFloat();
		mClothForceScale = pNxStream->readFloat();
		mFluidForceScale = pNxStream->readFloat();
		mSBForceScale = pNxStream->readFloat();

		mFlags = pNxStream->readDword();

		return true;
	}
	else
		return false;
}

/*
APhysXObjectDesc* APhysXForceFieldObjectDesc::Clone() const
{
	APhysXObjectDesc* pClonedObjectDesc = new APhysXForceFieldObjectDesc;


#define MAKE_DIRECT_COPY
#ifdef	MAKE_DIRECT_COPY
	
	*(APhysXForceFieldObjectDesc *)pClonedObjectDesc = *this;

#else

	//??? seems no need to clone like this...

	// clone parents' data
	*pClonedObjectDesc = *(APhysXObjectDesc* )this;

	APhysXForceFieldObjectDesc* pClonedFFObjectDesc = (APhysXForceFieldObjectDesc*) pClonedObjectDesc;


	pClonedFFObjectDesc->mFFLKernelParameters = mFFLKernelParameters;

	pClonedFFObjectDesc->mIncludeShapes = mIncludeShapes;

	pClonedFFObjectDesc->mFFCoordinates = mFFCoordinates;

	pClonedFFObjectDesc->mRBType = mRBType;
	pClonedFFObjectDesc->mClothType = mClothType;
	pClonedFFObjectDesc->mFluidType = mFluidType;
	pClonedFFObjectDesc->mSBType = mSBType;

	// to do...

	pClonedFFObjectDesc->mFlags = mFlags;


#endif


	return pClonedObjectDesc;

}
*/


void APhysXForceFieldObjectDesc::GetNxForceFieldDesc(NxForceFieldDesc& ffDesc)
{
	// include group shapes

	for(int i=0; i<mIncludeShapes.GetNum(); i++)
	{
		NxForceFieldShapeDesc* pFFShapeDesc = mIncludeShapes.GetNxForceFieldShapeDesc(i);
		if(pFFShapeDesc)
		{
			//-----------------------------------------------------------------
			//!!! note here:
			// we should keep the shape pointers until the NxForceField object is created,
			// and then, do not forget to delete these pointers...
			ffDesc.includeGroupShapes.push_back(pFFShapeDesc);
			//-----------------------------------------------------------------
		}
	}

	ffDesc.coordinates = mFFCoordinates;
	ffDesc.rigidBodyType = mRBType;
	ffDesc.clothType = mClothType;
	ffDesc.fluidType = mFluidType;
	ffDesc.softBodyType = mSBType;
	ffDesc.flags = mFlags;

	ffDesc.group = mCollisionGroup;

}

//-------------------------------------------------------------------------------------

ImplementVersion(APhysXVacuumForceFieldObjectDesc, APhysXForceFieldObjectDesc)

APhysXVacuumForceFieldObjectDesc::APhysXVacuumForceFieldObjectDesc()
{
	mFFLKernelParameters.mConstant = -APHYSX_STD_GRAVITY;

	mRBType = mClothType = mFluidType = mSBType = NX_FF_TYPE_GRAVITATIONAL;

	mFlags = 0;

}

/*
APhysXObjectDesc* APhysXVacuumForceFieldObjectDesc::Clone() const
{
	APhysXObjectDesc* pClonedObjectDesc = new APhysXVacuumForceFieldObjectDesc;

	*(APhysXVacuumForceFieldObjectDesc *)pClonedObjectDesc = *this;

	return pClonedObjectDesc;

}
*/



//-------------------------------------------------------------------------------------

APhysXGravitationForceFieldObjectDesc::APhysXGravitationForceFieldObjectDesc(const NxVec3& vGravity)
{

	mRBType = mClothType = mFluidType = mSBType = NX_FF_TYPE_GRAVITATIONAL;
	mFlags = 0;

	SetGravity(vGravity);
	
}

void APhysXGravitationForceFieldObjectDesc::SetGravity(const NxVec3& vGravity)
{
	mFFLKernelParameters.mConstant = vGravity - APHYSX_STD_GRAVITY ;
	m_vGravity = vGravity;
}


bool APhysXGravitationForceFieldObjectDesc::Save(NxStream* pNxStream)
{

	if(!APhysXForceFieldObjectDesc::Save(pNxStream))
		return false;
	
	// save my version...
	SaveVersion(pNxStream);

	pNxStream->storeBuffer(&m_vGravity, sizeof(NxVec3));

	return true;
}

bool APhysXGravitationForceFieldObjectDesc::Load(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

		pNxStream->readBuffer(&m_vGravity, sizeof(NxVec3));

	
		return true;
	}
	else
		return false;
}

/*
APhysXObjectDesc* APhysXGravitationForceFieldObjectDesc::Clone() const
{
	APhysXObjectDesc* pClonedObjectDesc = new APhysXGravitationForceFieldObjectDesc(m_vGravity);

	*(APhysXGravitationForceFieldObjectDesc *)pClonedObjectDesc = *this;

	return pClonedObjectDesc;

}
*/


//-------------------------------------------------------------------------------------

APhysXBuoyancyForceFieldObjectDesc::APhysXBuoyancyForceFieldObjectDesc(float fDensity)
{
	// use the default value...

#ifdef RESET_DEFAULT_VALUE

	mRBType = mClothType = mFluidType = mSBType = NX_FF_TYPE_OTHER;

	// scaling the Force by the volume...
	mFlags =	NX_FFF_VOLUMETRIC_SCALING_FLUID | 
		NX_FFF_VOLUMETRIC_SCALING_CLOTH |
		NX_FFF_VOLUMETRIC_SCALING_SOFTBODY |
		NX_FFF_VOLUMETRIC_SCALING_RIGIDBODY;

#endif

	SetDensity(fDensity);
}

void APhysXBuoyancyForceFieldObjectDesc::SetDensity(float fDensity)
{
	m_fDensity = fDensity;

	mFFLKernelParameters.mConstant = -APHYSX_STD_GRAVITY * fDensity ;
}

bool APhysXBuoyancyForceFieldObjectDesc::Save(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);

	pNxStream->storeFloat(m_fDensity);

	return true;
}

bool APhysXBuoyancyForceFieldObjectDesc::Load(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

		m_fDensity = pNxStream->readFloat();


		return true;
	}
	else
		return false;

}

/*
APhysXObjectDesc* APhysXBuoyancyForceFieldObjectDesc::Clone() const
{
	APhysXObjectDesc* pClonedObjectDesc = new APhysXBuoyancyForceFieldObjectDesc;

	*(APhysXBuoyancyForceFieldObjectDesc *)pClonedObjectDesc = *this;

	return pClonedObjectDesc;

}
*/

//----------------------------------------------------------------------------

APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldObjectDesc()
{

	mFlags = 0;		// do not scale by the volume...
	
	mFFCoordinates = NX_FFC_CYLINDRICAL;	// use the cylindrical coordinate system...

	APhysXVortexForceFieldParameters apxVortexFFParameters;
	SetVortexFFParameters(apxVortexFFParameters);
}

void APhysXVortexForceFieldObjectDesc::SetVortexFFParameters(const APhysXVortexForceFieldParameters& apxVortexFFParameters)
{
	m_apxVortexFFParameters = apxVortexFFParameters;

	//??? should call this?...
	mFFLKernelParameters.SetToDefault();

	// fill the kernel data according to the vortex parameters...
	mFFLKernelParameters.mConstant.x = -apxVortexFFParameters.mConstantCentripetalForce;
	mFFLKernelParameters.mConstant.y = apxVortexFFParameters.mConstantElevationForce;

	mFFLKernelParameters.mPositionTarget.x = apxVortexFFParameters.mTargetRadius;
	mFFLKernelParameters.mPositionMultiplier(0, 0) = apxVortexFFParameters.mTargetRadiusScaleTerm;
	mFFLKernelParameters.mPositionMultiplier(0, 1) = -apxVortexFFParameters.mElevationExpandScaleTerm;

	mFFLKernelParameters.mVelocityTarget.z = apxVortexFFParameters.mTargetVelocity;
	mFFLKernelParameters.mVelocityMultiplier.diagonal(NxVec3(apxVortexFFParameters.mTargetVelocityScaleTerm));

	mFFLKernelParameters.mNoise.set(apxVortexFFParameters.mNoise);


}


bool APhysXVortexForceFieldObjectDesc::Save(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);

	pNxStream->storeBuffer(&m_apxVortexFFParameters, sizeof(APhysXVortexForceFieldParameters));

	return true;
}

bool APhysXVortexForceFieldObjectDesc::Load(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

		pNxStream->readBuffer(&m_apxVortexFFParameters, sizeof(APhysXVortexForceFieldParameters));
		SetVortexFFParameters(m_apxVortexFFParameters);

		return true;
	}
	else
		return false;

}

/*
APhysXObjectDesc* APhysXVortexForceFieldObjectDesc::Clone() const
{
	APhysXObjectDesc* pClonedObjectDesc = new APhysXVortexForceFieldObjectDesc;

	*(APhysXVortexForceFieldObjectDesc *)pClonedObjectDesc = *this;

	return pClonedObjectDesc;

}
*/

//----------------------------------------------------------------------------

APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldObjectDesc()
{
	mFlags = 0;		// do not scale by the volume...
	
	mFFCoordinates = NX_FFC_SPHERICAL;		// use the spherical coordinate system...

	mClothForceScale = 0.01f;

	// use the default Explosion FF Parameters to initialize FF parameters...
	APhysXExplosionForceFieldParameters apxExplosionFFPara;
	SetExplosionFFParameters(apxExplosionFFPara);

}

void APhysXExplosionForceFieldObjectDesc::SetExplosionFFParameters(const APhysXExplosionForceFieldParameters& apxExplosionFFParameters)
{
	m_apxExplosionFFParameters = apxExplosionFFParameters;

	//??? should call this?...
	mFFLKernelParameters.SetToDefault();

	mFFLKernelParameters.mConstant.x = apxExplosionFFParameters.mCenterForce;
	mFFLKernelParameters.mFalloffQuadratic.x = apxExplosionFFParameters.mQuadricFallOff;

	if(mIncludeShapes.GetNum())
	{
		// we have created the sphere shape for the explosion FF...
		APhysXShapeDesc* pPhysXShapeDesc = mIncludeShapes.GetPhysXShapeDesc(0);
		APHYSX_ASSERT(pPhysXShapeDesc->GetShapeType() == APX_SHAPETYPE_SPHERE);

		((APhysXSphereShapeDesc *)pPhysXShapeDesc)->mRadius = apxExplosionFFParameters.mRadius;

	}
	else
	{
		APhysXSphereShapeDesc apxSphereShapeDesc;
		apxSphereShapeDesc.mRadius = apxExplosionFFParameters.mRadius;

		mIncludeShapes.CreateAndPushShapeDesc(&apxSphereShapeDesc);
	}

	mLifeSpan = apxExplosionFFParameters.mLifeTime;
}


bool APhysXExplosionForceFieldObjectDesc::Save(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);

	pNxStream->storeBuffer(&m_apxExplosionFFParameters, sizeof(APhysXExplosionForceFieldParameters));

	return true;
}

bool APhysXExplosionForceFieldObjectDesc::Load(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

		pNxStream->readBuffer(&m_apxExplosionFFParameters, sizeof(APhysXExplosionForceFieldParameters));
		SetExplosionFFParameters(m_apxExplosionFFParameters);

		return true;
	}
	else
		return false;

}

/*
APhysXObjectDesc* APhysXExplosionForceFieldObjectDesc::Clone() const
{
	APhysXObjectDesc* pClonedObjectDesc = new APhysXExplosionForceFieldObjectDesc;

	*(APhysXExplosionForceFieldObjectDesc *)pClonedObjectDesc = *this;

	return pClonedObjectDesc;

}
*/

//----------------------------------------------------------------------------

APhysXWindForceFieldObjectDesc::APhysXWindForceFieldObjectDesc()
{
	mFlags = 0;		// do not scale by the volume...

	mCycleDistance = 50.0f;

	// use the default Explosion FF Parameters to initialize FF parameters...
	APhysXWindForceFieldParameters apxWindFFPara;
	SetWindFFParameters(apxWindFFPara);

}

void APhysXWindForceFieldObjectDesc::SetWindFFParameters(const APhysXWindForceFieldParameters& apxWindFFParameters)
{
	m_apxWindFFParameters = apxWindFFParameters;

	//??? should call this?...
	mFFLKernelParameters.SetToDefault();

	mFFLKernelParameters.mConstant = apxWindFFParameters.mWindForce;
	
	// add some noise is important...
	mFFLKernelParameters.mNoise = apxWindFFParameters.mWindForce * 0.5f;		

}


bool APhysXWindForceFieldObjectDesc::Save(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);

	pNxStream->storeBuffer(&m_apxWindFFParameters, sizeof(APhysXWindForceFieldParameters));
	pNxStream->storeFloat(mCycleDistance);

	return true;
}

bool APhysXWindForceFieldObjectDesc::Load(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer >= 0xcc000001)
	{
		pNxStream->readBuffer(&m_apxWindFFParameters, sizeof(APhysXWindForceFieldParameters));
		SetWindFFParameters(m_apxWindFFParameters);
	}
	
	if(m_StreamObjVer >= 0xcc000002)
	{
		mCycleDistance = pNxStream->readFloat();
	}

	return true;

}

/*
APhysXObjectDesc* APhysXWindForceFieldObjectDesc::Clone() const
{
	APhysXObjectDesc* pClonedObjectDesc = new APhysXWindForceFieldObjectDesc;

	*(APhysXWindForceFieldObjectDesc *)pClonedObjectDesc = *this;

	return pClonedObjectDesc;

}
*/


//----------------------------------------------------------------------------

APhysXBossForceFieldObjectDesc::APhysXBossForceFieldObjectDesc()
{
	mFlags = 0;		// do not scale by the volume...
	mFFCoordinates = NX_FFC_CARTESIAN;	// use the cartesian coordinate system...
	
	APhysXBossForceFieldParameters apxBossFFPara;
	SetBossFFParameters(apxBossFFPara);
}

void APhysXBossForceFieldObjectDesc::SetBossFFParameters(const APhysXBossForceFieldParameters& apxBossFFParameters)
{
	m_apxBossFFParameters = apxBossFFParameters;
}

bool APhysXBossForceFieldObjectDesc::Save(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Save(pNxStream))
		return false;
	
	// save my version...
	SaveVersion(pNxStream);
	
	NxU32 flags = 0;
	if (m_apxBossFFParameters.mRest)		flags |= (1 << 0);
	if (m_apxBossFFParameters.mEnhanceMode)	flags |= (1 << 1);

	pNxStream->storeDword(flags);
	pNxStream->storeFloat(m_apxBossFFParameters.mRadius);
	pNxStream->storeFloat(m_apxBossFFParameters.mForceValue);
	pNxStream->storeBuffer(&m_apxBossFFParameters.mLocalXSide, sizeof(NxVec3));
	pNxStream->storeBuffer(&m_apxBossFFParameters.mLocalYUp, sizeof(NxVec3));
	pNxStream->storeBuffer(&m_apxBossFFParameters.mLocalZFaceTo, sizeof(NxVec3));

	return true;
}

bool APhysXBossForceFieldObjectDesc::Load(NxStream* pNxStream)
{
	if(!APhysXForceFieldObjectDesc::Load(pNxStream))
		return false;
	
	// load my version...
	LoadVersion(pNxStream);
	
	if(m_StreamObjVer >= 0xcc000001)
	{
		NxU32 flags = pNxStream->readDword();
		if (flags & (1 << 0))
			m_apxBossFFParameters.mRest = true;
		else
			m_apxBossFFParameters.mRest = false;
		if (flags & (1 << 1))
			m_apxBossFFParameters.mEnhanceMode = true;
		else
			m_apxBossFFParameters.mEnhanceMode = false;

		m_apxBossFFParameters.mRadius = pNxStream->readFloat();
		m_apxBossFFParameters.mForceValue = pNxStream->readFloat();
		pNxStream->readBuffer(&m_apxBossFFParameters.mLocalXSide, sizeof(NxVec3));
		pNxStream->readBuffer(&m_apxBossFFParameters.mLocalYUp, sizeof(NxVec3));
		pNxStream->readBuffer(&m_apxBossFFParameters.mLocalZFaceTo, sizeof(NxVec3));
	}
	
	return true;
}