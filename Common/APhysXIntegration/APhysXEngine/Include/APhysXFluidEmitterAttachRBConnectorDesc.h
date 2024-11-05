/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXFLUIDEMITTERATTACHRBCONNECTORDESC_H_
#define _APHYSXFLUIDEMITTERATTACHRBCONNECTORDESC_H_


struct APhysXFluidEmitterParameters
{
	APhysXFluidEmitterParameters()
	{
		SetToDefault();
	}

	void SetToDefault()
	{
		mRelPose						.id();
		mType							= NX_FE_CONSTANT_PRESSURE;
		mMaxParticles					= 0;
		mShape							= NX_FE_RECTANGULAR;
		mDimensionX						= 0.25f;
		mDimensionY						= 0.25f;
		mRandomPos						.zero();
		mRandomAngle					= 0.0f;
		mFluidVelocityMagnitude			= 1.0f;
		mRate							= 100.0f;
		mParticleLifetime				= 0.0f;
		mRepulsionCoefficient			= 1.0f;
		mFlags							= NX_FEF_ENABLED|NX_FEF_VISUALIZATION|NX_FEF_ADD_BODY_VELOCITY;
	}

	void GetNxFluidEmitterDesc(NxFluidEmitterDesc& fluidEmitterDesc)
	{
		fluidEmitterDesc.relPose					= mRelPose;
		fluidEmitterDesc.type						= mType;
		fluidEmitterDesc.maxParticles				= mMaxParticles;
		fluidEmitterDesc.shape						= mShape;
		fluidEmitterDesc.dimensionX					= mDimensionX;
		fluidEmitterDesc.dimensionY					= mDimensionY;
		fluidEmitterDesc.randomPos					= mRandomPos;
		fluidEmitterDesc.randomAngle				= mRandomAngle;
		fluidEmitterDesc.fluidVelocityMagnitude		= mFluidVelocityMagnitude;
		fluidEmitterDesc.rate						= mRate;
		fluidEmitterDesc.particleLifetime			= mParticleLifetime;
		fluidEmitterDesc.repulsionCoefficient		= mRepulsionCoefficient;
		fluidEmitterDesc.flags						= mFlags;
	}

	// The emitter's pose relative to the frameShape.
	NxMat34					mRelPose;

	// The emitter's mode of operation.
	NxU32					mType;

	// The maximum number of particles which are emitted from this emitter.
	NxU32					mMaxParticles;

	// The emitter's shape can either be rectangular or elliptical.
	NxU32					mShape;

	// The sizes of the emitter in the directions of the first and the second axis of its orientation 
	NxReal					mDimensionX;
	NxReal					mDimensionY;

	// Random vector with values for each axis direction of the emitter orientation.
	NxVec3					mRandomPos;

	// Random angle deviation from emission direction.
	NxReal					mRandomAngle;

	// The velocity magnitude of the emitted fluid particles.
	NxReal					mFluidVelocityMagnitude;

	// The rate specifies how many particles are emitted per second.
	NxReal					mRate;

	// This specifies the time in seconds an emitted particle lives.
	NxReal					mParticleLifetime;

	// Defines a factor for the impulse transfer from attached emitter to body.
	NxReal					mRepulsionCoefficient;

	// A combination of NxFluidEmitterFlags.
	NxU32					mFlags;

};

class APHYSX_DLL_EXPORT APhysXFluidEmitterAttachRBConnectorDesc:public APhysXObjectConnectorDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_FLUIDEMITTER_ATTACH_RB);

public:

	APhysXFluidEmitterAttachRBConnectorDesc() 
	{
		mAttachedNxActorName = NULL;
	}

	~APhysXFluidEmitterAttachRBConnectorDesc() {}


	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_FLUIDEMITTER_ATTACH_RB)
		{
			APhysXFluidEmitterAttachRBConnectorDesc* pDestFluidEmitterAttachRBConnectorDesc = (APhysXFluidEmitterAttachRBConnectorDesc*) pDestConnectorDesc;
			*pDestFluidEmitterAttachRBConnectorDesc = *this;
		}
	}

	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXFluidEmitterAttachRBConnectorDesc;
		*(APhysXFluidEmitterAttachRBConnectorDesc *)pPhysXConnectorDesc = *this;

		return pPhysXConnectorDesc;
	}

public:

	// the emitter's parameters...
	APhysXFluidEmitterParameters mFluidEmitterParameters;

	// the actor's name which the emitter is attached to...
	const char* mAttachedNxActorName;
};


#endif
