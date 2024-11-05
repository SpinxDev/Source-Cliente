/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXFLUIDOBJECTDESC_H_
#define _APHYSXFLUIDOBJECTDESC_H_

struct APhysXFluidParameters
{
	enum
	{
		APX_FLUID_MAX_PARTICLE_NUM = 32767,			// refer to NxFluidDesc.maxParticles
	};
		
	APhysXFluidParameters()
	{
		SetToDefault();
	}

	void SetToDefault()
	{
		
		mMaxParticles				= APX_FLUID_MAX_PARTICLE_NUM;
		mNumReserveParticles			= 0;
		mRestParticlesPerMeter		= 50.0f;
		mRestDensity					= 1000.0f;
		mKernelRadiusMultiplier		= 1.2f;
		mMotionLimitMultiplier		= 3.0f * mKernelRadiusMultiplier;
		mCollisionDistanceMultiplier = 0.1f * mKernelRadiusMultiplier;
		mPacketSizeMultiplier		= 16;
		mStiffness					= 20.0f;
		mViscosity					= 6.0f;
		mSurfaceTension				= 0.0f;
		mDamping						= 0.0f;
		mFadeInTime					= 0.0f;
		mExternalAcceleration.zero();
		mProjectionPlane.set(NxVec3(0.0f, 0.0f, 1.0f), 0.0f);
		mRestitutionForStaticShapes	= 0.5f;
		mDynamicFrictionForStaticShapes = 0.05f;
		mStaticFrictionForStaticShapes = 0.05f;
		mAttractionForStaticShapes	= 0.0f;
		mRestitutionForDynamicShapes	= 0.5f;
		mDynamicFrictionForDynamicShapes = 0.5f;
		mStaticFrictionForDynamicShapes = 0.5f;
		mAttractionForDynamicShapes	= 0.0f;
		mCollisionResponseCoefficient = 0.2f;

		mSimulationMethod			= NX_F_SPH;
		mCollisionMethod			= NX_F_STATIC|NX_F_DYNAMIC;
		mForceFieldMaterial			= 0;
		mFlags						= NX_FF_VISUALIZATION|NX_FF_ENABLED|NX_FF_HARDWARE;

	}

	// Sets the maximal number of particles for the fluid used in the simulation.
	NxU32						mMaxParticles;

	// Defines the number of particles which are reserved for creation at runtime.
	NxU32						mNumReserveParticles;

	// brief The particle resolution given as particles per linear meter measured when the fluid is
	// in its rest state (relaxed).
	NxReal						mRestParticlesPerMeter;

	// Target density for the fluid (water is about 1000).
	NxReal						mRestDensity;

	// Radius of sphere of influence for particle interaction.
	NxReal						mKernelRadiusMultiplier;

	// Maximal distance a particle is allowed to travel within one timestep.
	NxReal						mMotionLimitMultiplier;

	// Defines the distance between particles and collision geometry, which is maintained during simulation.
	NxReal						mCollisionDistanceMultiplier;

	// This parameter controls the parallelization of the fluid.
	NxU32						mPacketSizeMultiplier;

	// The stiffness of the particle interaction related to the pressure.
	NxReal						mStiffness;

	// The viscosity of the fluid defines its viscous behavior.
	NxReal						mViscosity;

	// The surfaceTension of the fluid defines an attractive force between particles
	NxReal						mSurfaceTension;

	// Velocity damping constant, which is globally applied to each particle.
	NxReal						mDamping;

	// Defines a timespan for the particle "fade-in".
	NxReal						mFadeInTime;
	
	// Acceleration (m/s^2) applied to all particles at all time steps.
	NxVec3						mExternalAcceleration;

	// Defines the plane the fluid particles are projected to. This parameter is only used if
	NxPlane						mProjectionPlane;

	// Defines the restitution coefficient used for collisions of the fluid particles with static shapes.
	NxReal						mRestitutionForStaticShapes;

	// Defines the dynamic friction of the fluid regarding the surface of a static shape.
	NxReal						mDynamicFrictionForStaticShapes;

	// Defines the static friction of the fluid regarding the surface of a static shape.
	NxReal						mStaticFrictionForStaticShapes;

	// Defines the strength of attraction between the particles and static rigid bodies on collision. 
	NxReal						mAttractionForStaticShapes;

	// Defines the restitution coefficient used for collisions of the fluid particles with dynamic shapes.
	NxReal						mRestitutionForDynamicShapes;

	// Defines the dynamic friction of the fluid regarding the surface of a dynamic shape.
	NxReal						mDynamicFrictionForDynamicShapes;

	// Defines the static friction of the fluid regarding the surface of a dynamic shape.
	NxReal						mStaticFrictionForDynamicShapes;

	// Defines the strength of attraction between the particles and the dynamic rigid bodies on collision. 
	NxReal						mAttractionForDynamicShapes;

	// Defines a factor for the impulse transfer from fluid to colliding rigid bodies.
	NxReal						mCollisionResponseCoefficient;

	// NxFluidSimulationMethod flags. Defines whether or not particle interactions are considered 
	NxU32						mSimulationMethod;

	// NxFluidCollisionMethod flags. Selects whether static collision and/or dynamic collision 
	NxU32						mCollisionMethod;

	// Force Field Material Index, index != 0 has to be created.
	NxForceFieldMaterial		mForceFieldMaterial;

	// Flags defining certain properties of the fluid.
	NxU32						mFlags;


};

enum APhysXParticleWriteDataFlag
{
	APX_PWDFLAG_POS					= (1<<0),
	APX_PWDFLAG_VEL					= (1<<1),
	APX_PWDFLAG_LIFE				= (1<<2),
	APX_PWDFLAG_DENSITY				= (1<<3),
	APX_PWDFLAG_ID					= (1<<4),
	APX_PWDFLAG_FLAG				= (1<<5),
	APX_PWDFLAG_COLLISIONNORMAL		= (1<<6),

};

class APHYSX_DLL_EXPORT APhysXFluidObjectDesc:public APhysXObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FLUID);
	DisableCopyConstructor(APhysXFluidObjectDesc);

	friend class APhysXFluidObject;

public:

	APhysXFluidObjectDesc()
	{
		mParticleWriteDataFlag = APX_PWDFLAG_POS;

		mCollisionGroup = APX_COLLISION_GROUP_FLUID;

	}

	//-----------------------------------------------------------------------------------
	// Note:
	// By default, we make all the particles have uniform rendering and other 
	// user defined properties, in this case, we can achieve great efficiency.
	// While, in some other cases, we have to track particles' whole lifetime
	// and their corresponding data for rendering, call this and use APhysXFluidObject's
	// 
	void TrackParticles(bool bTrack = true)
	{
		if(bTrack)
			mParticleWriteDataFlag |= APX_PWDFLAG_ID;
		else
			mParticleWriteDataFlag &= ~APX_PWDFLAG_ID;
	}

	bool GetNxFluidDesc(NxFluidDesc& fluidDesc);
	
	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXFluidObjectDesc)

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	//-------------------------------------------------------------------------
	//!!! to do...
	// how about the initial particle data and emitters...
	// where do we save them?
	//-------------------------------------------------------------------------

public:

	APhysXFluidParameters mFluidParameters;
	APhysXU32 mParticleWriteDataFlag;


};

//-----------------------------------------------------------------------------------
// Note:
//		Oriented particle object is just a extension to Fluid object. This type of 
// fluid object have Orientation for each particle other than the common information
// of particles...

enum APhysXOrientedParticleSpinType
{
	APX_OP_SPINTYPE_SPHERE,		// spin like a sphere
	APX_OP_SPINTYPE_FLAT,		// spin like a flat object, such as leaves...
	
	APX_OP_SPINTYPE_NUM,
	
};

// when new particles are spawned, the initial orientation they have.
enum AphysXOrientedParticleInitialOrt
{
	APX_OP_IORT_ID,				// identity rotation
	APX_OP_IORT_RANDOM,			// random rotation
};

class APHYSX_DLL_EXPORT APhysXOrientedParticleObjectDesc:public APhysXFluidObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FLUID_ORIENTED_PARTICLE);

public:
	APhysXOrientedParticleObjectDesc()
	{
		mSpinType = APX_OP_SPINTYPE_SPHERE;
		mInitialOrt = APX_OP_IORT_RANDOM;

		mMaxAngularVel = -1.0f;						// negative value means that we do not limit the max angular velocity
		mSpinRadius = -1.0f;						// negative value means that we will use particle's collision radius as spin radius.
	}

	DeclareCloneAPhysXObjectDesc(APhysXOrientedParticleObjectDesc)

public: 

	APhysXOrientedParticleSpinType mSpinType;
	AphysXOrientedParticleInitialOrt mInitialOrt;

	// max allowed angular velocity of particle, unit is radian/second
	float mMaxAngularVel;		

	// the spin radius of particle, only for case of APX_OP_SPINTYPE_SPHERE
	float mSpinRadius;
};

#endif
