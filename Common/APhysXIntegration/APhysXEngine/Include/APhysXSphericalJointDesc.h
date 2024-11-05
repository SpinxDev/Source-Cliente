/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/
#ifndef _APHYSXSPHERICALJOINTDESC_H_
#define _APHYSXSPHERICALJOINTDESC_H_

class APHYSX_DLL_EXPORT APhysXSphericalJointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_SPHERICAL_JOINT);

public:

	APhysXSphericalJointDesc() 
	{
		mSwingAxis.set(0,0,1);
		mTwistLimit.setToDefault();
		mSwingLimit.setToDefault();
		mTwistSpring.setToDefault();
		mSwingSpring.setToDefault();
		mJointSpring.setToDefault();
		mProjectionDistance = 1.0f;
		mSphericalJointFlags = 0;
		mProjectionMode = NX_JPM_NONE;

	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	virtual void GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D);

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_SPHERICAL_JOINT)
		{
			APhysXSphericalJointDesc* pDestJointDesc = (APhysXSphericalJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXSphericalJointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}

public:

	// to do... lots of new data members...

	NxVec3  mSwingAxis;
	float  mProjectionDistance;

	NxJointLimitPairDesc  mTwistLimit;
	NxJointLimitDesc  mSwingLimit;
	
	NxSpringDesc  mTwistSpring;
	NxSpringDesc  mSwingSpring;
	NxSpringDesc  mJointSpring;
	
	APhysXU32  mSphericalJointFlags;
	
	NxJointProjectionMode  mProjectionMode;


};





#endif