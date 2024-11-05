
/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXFORCEFIELDOBJECTDESC_H_
#define _APHYSXFORCEFIELDOBJECTDESC_H_



/*

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

		About APhysXForceFieldLinearKernelParameters, Pls. refer to entry "Force Field - Kernels"  of PhysX 2.8.1 Docs...

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	The linear kernel implements a function which was also used by the force fields in the SDK version 2.7.2.
	The resulting force in each dimension i is calculated through the following formula:

		fi = [K + Mp(P-p) + Mv(V-v) + N anoise]i / [1 + li|Pi-pi| + qi(Pi-pi)2]

	Here, 

		f is the resulting force (or acceleration, see below) on the particle 
		K is the constant part, from NxForceFieldLinearKernelDesc.constant 
		p is the position of the object, in the force field coordinate system 
		Mp is the position dependence matrix, from NxForceFieldLinearKernelDesc.positionMultiplier 
		P is the position target, from NxForceFieldLinearKernelDesc.positionTarget 
		v is the velocity of the object, in the force field coordinate system 
		Mv is the velocity dependence matrix, from NxForceFieldLinearKernelDesc.velocityMultiplier 
		V is the velocity target, from NxForceFieldLinearKernelDesc.velocityTarget 
		anoise is the noise multiplier, from NxForceFieldLinearKernelDesc.noise 
		N is a 3x3 matrix whose diagonal elements are three random numbers in the range [0, 1] 
		l is the linear falloff term, from NxForceFieldLinearKernelDesc.falloffLinear 
		q is the quadratic falloff term, from NxForceFieldLinearKernelDesc.falloffQuadratic 


	As for the parameters in the force field linear kernel descriptor: 

		constant:			Location-invariant part of the force field function. 
		falloffLinear:		Linear falloff term, see the force field function. 
		falloffQuadratic:	Quadratic falloff term, see the force field function. 
		name:				Possible debug name. The string is not copied by the SDK, only the pointer is stored. 
		noise:				Random component of the force field function. 
		positionMultiplier: Location dependent part of the force field function. 
		positionTarget:		Equilibrium point of location dependent force component. 
		torusRadius:		Radius of toroidal force field function coordinate system, if used. 
		userData:			User can assign this to whatever, usually to create a 1:1 relationship with a user object. 
		velocityMultiplier: Velocity dependent part of the force field function. 
		velocityTarget:		Equilibrium velocity of velocity dependent force component. 

*/

struct APhysXForceFieldLinearKernelParameters
{

	APhysXForceFieldLinearKernelParameters()
	{
		SetToDefault();
	}

	void SetToDefault()
	{
		mConstant			.zero(); 
		mPositionMultiplier	.zero();
		mPositionTarget		.zero();
		mVelocityMultiplier	.zero();
		mVelocityTarget		.zero();
		mFalloffLinear		.zero();
		mFalloffQuadratic	.zero();
		mNoise				.zero();
		mTorusRadius		= 1.0f;
	}

	void GetNxForceFieldLinearKernelDesc(NxForceFieldLinearKernelDesc& fflKernelDesc);
	
	//----------------------------------------------------------------------------

	// Constant part of force field function
	NxVec3					mConstant;

	// Coefficient of force field function position term
	NxMat33					mPositionMultiplier;

	// Force field position target.
	NxVec3					mPositionTarget;

	// Coefficient of force field function velocity term
	NxMat33					mVelocityMultiplier;

	// Force field velocity target
	NxVec3					mVelocityTarget;

	// Radius for NX_FFC_TOROIDAL type coordinates.
	NxReal					mTorusRadius;
	
	// Linear term in magnitude falloff factor. Range (each component): [0, inf)
	NxVec3					mFalloffLinear;

	// Quadratic term in magnitude falloff factor. Range (each component): [0, inf)
	NxVec3					mFalloffQuadratic;
	
	// Noise scaling
	NxVec3					mNoise;	

};

class APHYSX_DLL_EXPORT APhysXForceFieldObjectDesc:public APhysXObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FORCEFIELD);
	DisableCopyConstructor(APhysXForceFieldObjectDesc);	
	
	friend class APhysXForceFieldObject;

public:

	APhysXForceFieldObjectDesc()
	{
		mFFCoordinates = NX_FFC_CARTESIAN;
		mRBType = mClothType = mFluidType = mSBType = NX_FF_TYPE_OTHER;

		mFlags =	NX_FFF_VOLUMETRIC_SCALING_FLUID | 
					NX_FFF_VOLUMETRIC_SCALING_CLOTH |
					NX_FFF_VOLUMETRIC_SCALING_SOFTBODY |
					NX_FFF_VOLUMETRIC_SCALING_RIGIDBODY;

		mRBForceScale = mClothForceScale = mFluidForceScale = mSBForceScale = 1.0f;

		mCustomKernel = NULL;

	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXForceFieldObjectDesc)

	void GetNxForceFieldDesc(NxForceFieldDesc& ffDesc);

public:

	APhysXForceFieldLinearKernelParameters mFFLKernelParameters;

	APhysXShapeDescManager mIncludeShapes;			// only actors inside the Included Shape List will be affected by the force field

	NxForceFieldCoordinates mFFCoordinates;

	NxForceFieldType  mRBType;
	NxForceFieldType  mClothType;
	NxForceFieldType  mFluidType;
	NxForceFieldType  mSBType;

	// the value to scale force field kernel output...
	float mRBForceScale;		
	float mClothForceScale;
	float mFluidForceScale;
	float mSBForceScale;

	NxU32 mFlags;


	//------------------------------------------------------------
	// done... We need not to add this data member since we create
	// it internally according to the scale data member.
	// to do...
	// Currently, we do not support multi-variety force field since
	// non-zero variety must be created by NxScene's method...
	// So all force field 's variety id is 0.
	
	// NxForceFieldVariety  mForceFieldVariety;
	//------------------------------------------------------------

	// if you want to creat non-linear kernel, use this member
	NxForceFieldKernel* mCustomKernel;
};


//----------------------------------------------------------------------------
// define some useful force field template, so that we can set the parameters
// more directly...

class APHYSX_DLL_EXPORT APhysXVacuumForceFieldObjectDesc:public APhysXForceFieldObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FORCEFIELD_VACUUM);


public:

	APhysXVacuumForceFieldObjectDesc();

	//------------------------------------------------------------
	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXVacuumForceFieldObjectDesc)

};


//----------------------------------------------------------------------------

class APHYSX_DLL_EXPORT APhysXGravitationForceFieldObjectDesc:public APhysXForceFieldObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FORCEFIELD_GRAVITATION);


public:

	APhysXGravitationForceFieldObjectDesc(const NxVec3& vGravity = NxVec3(0.0f));

	void SetGravity(const NxVec3& vGravity);
	const NxVec3& GetGravity() const
	{
		return m_vGravity;
	}

	//------------------------------------------------------------
	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);
	
	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXGravitationForceFieldObjectDesc)


protected:

	// the expected gravity of the forcefield
	NxVec3 m_vGravity;
};


//----------------------------------------------------------------------------
//	the buoyancy force computation is by the following equation:
//		F = Density * Volume * g
//
//	Density:	the density of the liquid
//	Volume:		the volume of the object immerges in the the liquid
//	g:			the gravity constant

//	while, PhysX SDK 2.8.1 ForceField can not support submerge volume computation,
//	in this case, we can not compute the exact buoyancy for submerged objects.


class APHYSX_DLL_EXPORT APhysXBuoyancyForceFieldObjectDesc:public APhysXForceFieldObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FORCEFIELD_BUOYANCY);


public:

	APhysXBuoyancyForceFieldObjectDesc(float fDensity = 1.0f);

	void SetDensity(float fDensity);
	float GetDensity() const
	{
		return m_fDensity;
	}

	//------------------------------------------------------------
	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXBuoyancyForceFieldObjectDesc)


protected:

	// the density of the liquid which enforces the buoyancy.
	float m_fDensity;
};


//----------------------------------------------------------------------------

class APHYSX_DLL_EXPORT APhysXVortexForceFieldObjectDesc:public APhysXForceFieldObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FORCEFIELD_VORTEX);

public:
	
	// to beware that the coordinate system is cylindrical...
	struct APhysXVortexForceFieldParameters
	{
		float mTargetVelocity;				// the target velocity in the tangent direction, if it is not achieved, a compensating term will be added to force
		float mTargetVelocityScaleTerm;		// the scale term of the compensating force to achieve the target velocity...
		
		float mTargetRadius;				// the target radius, if it is not achieved, a compensating term will be added to force
		float mTargetRadiusScaleTerm;		// the scale term of the compensating force to achieve the target radius...

		float mConstantCentripetalForce;	// the constant Centripetal Force term, this will take the effect of pulling objects to the center
		float mConstantElevationForce;		// the constant Elevation Force term, this will take the effect of elevating objects

		float mElevationExpandScaleTerm;	// the scale term of expanding effect on increasing the elevation

		float mNoise;						// the noise term

		
		APhysXVortexForceFieldParameters()
		{
			SetToDefault();
		};


		void SetToDefault()
		{
			mTargetVelocity = 30.0f;
			mTargetVelocityScaleTerm = 1.0f;

			mTargetRadius = 3.0f;
			mTargetRadiusScaleTerm = 10.0f;

			mConstantCentripetalForce = 30.0f;
			mConstantElevationForce = 10.0f;
	
			mElevationExpandScaleTerm = 1.0f;

			mNoise = 5.0f;
	
		}
	};

public:

	APhysXVortexForceFieldObjectDesc();

	void SetVortexFFParameters(const APhysXVortexForceFieldParameters& apxVortexFFParameters);
	const APhysXVortexForceFieldParameters& GetVortexFFParameters() const 
	{
		return m_apxVortexFFParameters;
	}

	//------------------------------------------------------------
	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXVortexForceFieldObjectDesc)


protected:

	APhysXVortexForceFieldParameters m_apxVortexFFParameters;

};

//----------------------------------------------------------------------------
// no need to create the include shape outside for this type of force field
// we will create the shape internally.
class APHYSX_DLL_EXPORT APhysXExplosionForceFieldObjectDesc:public APhysXForceFieldObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FORCEFIELD_EXPLOSION);

public:

	struct APhysXExplosionForceFieldParameters
	{
		float mCenterForce;
		float mRadius;
		float mQuadricFallOff;
		float mLifeTime;


		APhysXExplosionForceFieldParameters()
		{
			SetToDefault();
		}

		void SetToDefault()
		{
			mCenterForce = 10000.0f;
			mRadius = 10.0f;
			mQuadricFallOff = 0.1f;
			mLifeTime = 0.5f;
		}
	};

public:

	APhysXExplosionForceFieldObjectDesc();

	void SetExplosionFFParameters(const APhysXExplosionForceFieldParameters& apxExplosionFFParameters);
	const APhysXExplosionForceFieldParameters& GetExplosionFFParameters() const 
	{
		return m_apxExplosionFFParameters;
	}


	//------------------------------------------------------------
	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXExplosionForceFieldObjectDesc)


protected:

	APhysXExplosionForceFieldParameters m_apxExplosionFFParameters;

};


//----------------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXWindForceFieldObjectDesc:public APhysXForceFieldObjectDesc
{

	//-------------------------------------------------------------
	// current version...
	DeclareVersion(0xcc000002);							// add mCycleDistance

	// version history...
	// DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FORCEFIELD_WIND);

public:

	struct APhysXWindForceFieldParameters
	{
		NxVec3 mWindForce;		// define the direction and the magnitude of the wind force
		
		float mWindTime;		// the time when winding
		float mRestTime;		// the time when resting
	
		float mCycleTime;		// the cycle time of a sin() function to scale the force

		APhysXWindForceFieldParameters()
		{
			SetToDefault();
		}

		void SetToDefault()
		{
			mWindForce.set(10.0f, 0, 0);

			mWindTime = 10.0f;
			mRestTime = 2.0f;

			mCycleTime = 3.0f;

		}
	};

	// to add position-dependent wind effect, this is the distance cycle of sin() function...
	// the default value: 50.0f
	float mCycleDistance;			

public:

	APhysXWindForceFieldObjectDesc();

	void SetWindFFParameters(const APhysXWindForceFieldParameters& apxWindFFParameters);
	const APhysXWindForceFieldParameters& GetWindFFParameters() const 
	{
		return m_apxWindFFParameters;
	}


	//------------------------------------------------------------
	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXWindForceFieldObjectDesc)


protected:

	APhysXWindForceFieldParameters m_apxWindFFParameters;

};


//----------------------------------------------------------------------------
class APHYSX_DLL_EXPORT APhysXBossForceFieldObjectDesc:public APhysXForceFieldObjectDesc
{
	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FORCEFIELD_BOSS);

public:
	
	struct APhysXBossForceFieldParameters
	{
		bool mRest;
		bool mEnhanceMode;

		float mRadius;
		float mForceValue;

		NxVec3 mLocalXSide;
		NxVec3 mLocalYUp;
		NxVec3 mLocalZFaceTo;
		
		APhysXBossForceFieldParameters()
		{
			SetToDefault();
		}
		
		void SetToDefault()
		{
			mRest = false;
			mEnhanceMode = false;

			mRadius		= 1.5f;
			mForceValue = 20.0f;

			mLocalXSide.set(1, 0, 0);
			mLocalYUp.set(0, 1, 0);
			mLocalZFaceTo.set(0, 0, 1);
		}
	};

public:
		
	APhysXBossForceFieldObjectDesc();
	
	void SetBossFFParameters(const APhysXBossForceFieldParameters& apxBossFFParameters);
	const APhysXBossForceFieldParameters& GetBossFFParameters() const 
	{
		return m_apxBossFFParameters;
	}
	
	DeclareCloneAPhysXObjectDesc(APhysXBossForceFieldObjectDesc)
			
protected:
		
	APhysXBossForceFieldParameters m_apxBossFFParameters;
};

#endif