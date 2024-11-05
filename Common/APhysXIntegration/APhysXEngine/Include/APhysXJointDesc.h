/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXJOINTDESC_H_
#define _APHYSXJOINTDESC_H_


class APHYSX_DLL_EXPORT APhysXJointDesc:public APhysXObjectConnectorDesc
{
	DeclareVersion(0xcc000002);
	DeclareConnectorType(APX_CNTTYPE_ABSTRACT_JOINT);

public:

	APhysXJointDesc()
	{
		mNxActor1Name = NULL;
		mNxActor2Name = NULL;

		mNxActor1 = mNxActor2 = NULL;

		mJointFlags = NX_JF_VISUALIZATION;

		//----------------------------------------
		// this will be more exact...
		// mMaxForce = NX_MAX_REAL;
		// mMaxTorque = NX_MAX_REAL;
		mMaxForce = NX_MAX_F32;
		mMaxTorque = NX_MAX_F32;

		mGlobalAnchor.zero();
		mGlobalAxis.zero();

		mLocalAxis[0].set(0, 0, 1);
		mLocalAxis[1].set(0, 0, 1);

		mLocalNormal[0].set(1, 0, 0);
		mLocalNormal[1].set(1, 0, 0);

		mSolverExtrapolationFactor = 1.0f;
	}
	
	virtual ~APhysXJointDesc() {}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	// must be overriden
	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const = 0;
	/*
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_ABSTRACT_JOINT)
		{
			APhysXJointDesc* pDestJointDesc = (APhysXJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}
	*/

	// to beware that this function will change my own member...
	virtual void TransformLocal2Global(const NxMat34& globalPose, const NxVec3& vScale3D) 
	{
		APhysXObjectConnectorDesc::TransformLocal2Global(globalPose, vScale3D);
		
		mGlobalAnchor.arrayMultiply(mGlobalAnchor, vScale3D);
		mGlobalAnchor = globalPose * mGlobalAnchor;
		mGlobalAxis = globalPose.M * mGlobalAxis;

	}

	// refer to APhysXShapeDesc's CreateNxShapeDesc and GetNxShapeDesc...
	// virtual NxJointDesc* CreateNxJointDesc() { return NULL; }
	virtual void GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D);

	bool IsNxActorsSet() { return mNxActor1 || mNxActor2; }
	bool IsNxActorNamesSet() { return mNxActor1Name || mNxActor2Name; }

public:

	//------------------------------------------------------------------------------
	// note:
	// we can choose two ways to set joint's actor1 and actor2
	// 1. set mNxActor1 and mNxActor2 directly, this is mostly applicable for the case that
	//    runtime creation of joints.
	// 2. set mNxActor1Name and mNxActor2Name, then we can find the corresponding objects via
	//    the names. this is suitable for edit-time setting
	//------------------------------------------------------------------------------

	const char* mNxActor1Name;			// name of NxActor 1
	const char* mNxActor2Name;			// name of NxActor 2

	NxActor* mNxActor1;					// NxActor 1
	NxActor* mNxActor2;					// NxActor 1

	NxVec3 mGlobalAnchor;
	NxVec3 mGlobalAxis;

	NxVec3 mLocalAxis[2];
	NxVec3 mLocalNormal[2];

	// the max force and torque that the joint can withstand not to break apart...
	float mMaxForce;
	float mMaxTorque;

	APhysXU32 mJointFlags;

	//--------------------------------------------------------------------
	// Note:
	//		version number ++ (0xcc000002),	Mar 23, 2009
	// 
	// to expose the parameter, solverExtrapolationFactor, to users so as to
	// tune the effect of joints.
	//
	float mSolverExtrapolationFactor;

};

// no more data member than the base class APhysXJointDesc...
class APHYSX_DLL_EXPORT APhysXCylindricalJointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_CYLINDRICAL_JOINT);

public:
	
	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_CYLINDRICAL_JOINT)
		{
			APhysXCylindricalJointDesc* pDestJointDesc = (APhysXCylindricalJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXCylindricalJointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}
};


// no more data member than the base class APhysXJointDesc...
class APHYSX_DLL_EXPORT APhysXFixedJointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_FIXED_JOINT);

public:
	
	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_FIXED_JOINT)
		{
			APhysXFixedJointDesc* pDestJointDesc = (APhysXFixedJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXFixedJointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}

};

// no more data member than the base class APhysXJointDesc...
class APHYSX_DLL_EXPORT APhysXPointInPlaneJointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_POINTINPLANE_JOINT);

public:

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_POINTINPLANE_JOINT)
		{
			APhysXPointInPlaneJointDesc* pDestJointDesc = (APhysXPointInPlaneJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXPointInPlaneJointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}

};

// no more data member than the base class APhysXJointDesc...
class APHYSX_DLL_EXPORT APhysXPointOnLineJointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_POINTONLINE_JOINT);

public:

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_POINTONLINE_JOINT)
		{
			APhysXPointOnLineJointDesc* pDestJointDesc = (APhysXPointOnLineJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXPointOnLineJointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}
};

// no more data member than the base class APhysXJointDesc...
class APHYSX_DLL_EXPORT APhysXPrismaticJointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_PRISMATIC_JOINT);

public:

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_PRISMATIC_JOINT)
		{
			APhysXPrismaticJointDesc* pDestJointDesc = (APhysXPrismaticJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXPrismaticJointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}
};


#endif