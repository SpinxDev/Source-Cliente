/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCLOTHOBJECTDESC_H_
#define _APHYSXCLOTHOBJECTDESC_H_


struct APHYSX_DLL_EXPORT APhysXClothParameters
{
	enum 
	{
		APX_CLOTH_MATERIAL_SILK,		// soft and smooth, just like the silk material
		APX_CLOTH_MATERIAL_FLAX,		// more stiff and damping than the silk material, just like the flax material
		APX_CLOTH_MATERIAL_FELT,		// more stiff and damping than the flax material, just like the felt material
	};

	APhysXClothParameters()
	{
		SetToDefault();
	}

	void SetToDefault()
	{
		// default values...
		// refer to the default values of NxClothDesc...
		mThickness = 0.01f;
		mDensity = 1.0f;
		mBendingStiffness = 1.0f;
		mStretchingStiffness = 1.0f;
		mDampingCoefficient = 0.5f;
		mFriction = 0.5f;
		mPressure = 1.0f;
		mTearFactor = 1.5f;
		mAttachmentTearFactor = 1.5f;
		mAttachmentResponseCoefficient = 0.2f;
		mCollisionResponseCoefficient = 0.2f;
		mToFluidResponseCoefficient = 1.0f;
		mFromFluidResponseCoefficient = 1.0f;
		mMinAdhereVelocity = 1.0f;
		mFlags = NX_CLF_GRAVITY;
		mSolverIterations = 5;
		mWakeUpCounter = NX_SLEEP_INTERVAL;
		mSleepLinearVelocity = -1.0f;
		mForceFieldMaterial = 0;
		mExternalAcceleration.set(0.0f, 0.0f, 0.0f);
		mWindAcceleration.set(0.0f, 0.0f, 0.0f);
		mValidBounds.setEmpty();
		mRelativeGridSpacing = 0.25f;

	}

	// iPresetMaterial: refer to the enum value APX_CLOTH_MATERIAL_XXX
	void LoadPresetMaterial(int iPresetMaterial);

	// Thickness of the cloth. 
	NxReal  mThickness;

	// Density of the cloth (mass per area). 
	NxReal  mDensity;

	// Bending stiffness of the cloth in the range 0 to 1. 
	NxReal  mBendingStiffness;

	// Stretching stiffness of the cloth in the range 0 to 1. 
	NxReal  mStretchingStiffness;

	// Spring damping of the cloth in the range 0 to 1. 
	NxReal  mDampingCoefficient;

	// Friction coefficient in the range 0 to 1. 
	NxReal  mFriction;

	// If the flag NX_CLF_PRESSURE is set, this variable defines the volume of air inside the mesh as volume = pressure * restVolume. 
	NxReal  mPressure;

	// If the flag NX_CLF_TEARABLE is set, this variable defines the elongation factor that causes the cloth to tear. 
	NxReal  mTearFactor;

	// Defines a factor for the impulse transfer from cloth to colliding rigid bodies. 
	NxReal  mCollisionResponseCoefficient;

	// Defines a factor for the impulse transfer from cloth to attached rigid bodies. 
	NxReal  mAttachmentResponseCoefficient;

	// If the flag NX_CLOTH_ATTACHMENT_TEARABLE is set in the attachment method of NxCloth, this variable defines the elongation factor that causes the attachment to tear. 
	NxReal  mAttachmentTearFactor;

	// Defines a factor for the impulse transfer from this cloth to colliding fluids. 
	NxReal  mToFluidResponseCoefficient;

	// Defines a factor for the impulse transfer from colliding fluids to this cloth. 
	NxReal  mFromFluidResponseCoefficient;

	// If the NX_CLF_ADHERE flag is set the cloth moves partially in the frame of the attached actor. 
	NxReal  mMinAdhereVelocity;

	// Number of solver iterations. 
	NxU32  mSolverIterations;

	// External acceleration which affects all non attached particles of the cloth. 
	NxVec3  mExternalAcceleration;

	// Acceleration which acts normal to the cloth surface at each vertex. 
	NxVec3  mWindAcceleration;

	// The cloth wake up counter. 
	NxReal  mWakeUpCounter;

	// Maximum linear velocity at which cloth can go to sleep. 
	NxReal  mSleepLinearVelocity;

	// Force Field Material Index, index != 0 has to be created. 
	NxU16  mForceFieldMaterial;

	// If the flag NX_CLF_VALIDBOUNDS is set, this variable defines the volume outside of which cloth particle are automatically removed from the simulation. 
	NxBounds3  mValidBounds;

	// This parameter defines the size of grid cells for collision detection. 
	NxReal  mRelativeGridSpacing;

	// Flag bits.
	NxU32  mFlags;
};

struct APHYSX_DLL_EXPORT APhysXClothVertexConstraintParameters
{
	NxReal mMaxDistance;

	APhysXClothVertexConstraintParameters()
	{
		SetToDefault();
	}

	void SetToDefault()
	{
		mMaxDistance = 0.0f;
	}
};

class APhysXMemoryWriteBuffer;
class APhysXMemoryReadBuffer;

class APHYSX_DLL_EXPORT APhysXClothObjectDesc:public APhysXObjectDesc
{

	//-------------------------------------------------------
	// version ++ : 
	//		to support Per-Vertex Constraint Setting
	DeclareVersion(0xcc000003);
	//-------------------------------------------------------
	// version ++ : 
	//		1. to support cloth constrain feature in 2.8.4
	//		2. to support different cook lib version
	// DeclareVersion(0xcc000002);

	// DeclareVersion(0xcc000001);

	DeclareObjType(APX_OBJTYPE_CLOTH);

	DisableCopyConstructor(APhysXClothObjectDesc);

	friend class APhysXClothObject;

public:

	APhysXClothObjectDesc() 
	{
		mCollisionGroup = APX_COLLISION_GROUP_CLOTH;

		// no teared vertices by default, if set the cloth flag NX_CLF_TEARABLE,
		// then remember to change this parameter to at least 2.
		mTearVertexNumFactor = 1;			

		mVertexNum = mTriangleNum = 0;

		mClothMeshBufSize = 0;
		mClothMeshBuf = NULL;

		m_pNxClothMesh = NULL;

		mConstrainMaxDistance = 0.0f;
		mConstrainMaxDistanceBias = 0.0f;
		mConstrainCollisionSphereRadius = 0.0f;
		mConstrainCollisionSphereDistance = 0.0f;

		mCookLibVersion = 0;

		mClothVertexConstraints = NULL;
	}

	virtual ~APhysXClothObjectDesc() { Release(); } 


	virtual void Release();

	bool InitClothMesh(const NxClothMeshDesc& clothMeshDesc, const NxVec3& vScale3D = NxVec3(1.0f));

	bool GetNxClothDesc(NxClothDesc& clothDesc);

	//-----------------------------------------------------------------
	// to do:
	//   complete the operator= so as to use the uniform macro...
	virtual APhysXObjectDesc* Clone() const;

	
	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	// Note: before calling this function, you have to InitClothMesh(), and users should
	// make sure that the apxCVCParas's size is just same as mVertexNum.
	bool SetClothVertexConstraints(const APhysXClothVertexConstraintParameters* apxCVCParas);

protected:

	void SetClothMeshBuf(APhysXMemoryWriteBuffer& apxMemWriteBuf);

	// since I do not copy buffer data out, we must create the NxCloth object before
	// APhysXClothDesc object vanishes.
	void GetMemoryReadBuf(APhysXMemoryReadBuffer& apxMemReadBuf);

public:		// make the member data public

	APhysXClothParameters mClothParameters;
	int mTearVertexNumFactor;				// the factor to scale the memory size so as to accomodate teared vertices...

protected:

	// to save the cooked cloth mesh
	APhysXU32 mClothMeshBufSize;
	APhysXU8* mClothMeshBuf;

	APhysXU32 mVertexNum;		// vertex number of cloth mesh
	APhysXU32 mTriangleNum;		// triangle number of cloth mesh

	//----------------------------------------------------------------------
	// for clothing feature: constrain cloth....
	float mConstrainMaxDistance;
	float mConstrainMaxDistanceBias;
	float mConstrainCollisionSphereRadius;
	float mConstrainCollisionSphereDistance;


	//----------------------------------------------------------------------
	// for Per-Vertex Constraint Setting...
	APhysXClothVertexConstraintParameters* mClothVertexConstraints;

	//----------------------------------------------------------------------
	// for compatible of different cooking formats...
	APhysXU32 mCookLibVersion;
	APhysXSimpleMeshDesc mClothMeshDesc;

	//----------------------------------------------------------------------
	// Note:	by Wenfeng He, Nov. 7, 2008
	// We are now using the outside APhysXClothObjectDesc cache way instead 
	// of inside APhysXMeshCooker cache method. So here we should keep the 
	// pointer of NxClothMesh...
	// ??? Is this new way all right?
	NxClothMesh* m_pNxClothMesh;
};




#endif