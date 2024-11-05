/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXD6JOINTDESC_H_
#define _APHYSXD6JOINTDESC_H_


class APHYSX_DLL_EXPORT APhysXD6JointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_D6_JOINT);

public:

	APhysXD6JointDesc() 
	{
		// note here...
		// the translation is locked by default, which is not 
		// consistent with NxD6JointDesc's default values
		mXMotion = NX_D6JOINT_MOTION_LOCKED;
		mYMotion = NX_D6JOINT_MOTION_LOCKED;
		mZMotion = NX_D6JOINT_MOTION_LOCKED;
		
		// by default, all three axis rotations are free
		mTwistMotion = NX_D6JOINT_MOTION_FREE;
		mSwing1Motion = NX_D6JOINT_MOTION_FREE;
		mSwing2Motion = NX_D6JOINT_MOTION_FREE;

		mDrivePosition.set(0,0,0);
		mDriveOrientation.id();

		mDriveLinearVelocity.set(0,0,0);
		mDriveAngularVelocity.set(0,0,0);

		mProjectionMode = NX_JPM_NONE;	// choose NX_JPM_POINT_MINDIST to turn projection on
		mProjectionDistance = 0.1f;
		mProjectionAngle = 0.0872f;	//about 5 degrees in radians.

		mD6JointFlags = 0;
		mGearRatio = 1.0f;

		// change the default local axises to positive-Y direction
		// this will make capsule-shape ragdoll system easily constructed 
		// since the height of capsule is along the capsule's Y axis.
		mLocalAxis[0].set(0, 1, 0);
		mLocalAxis[1].set(0, 1, 0);
	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	virtual void GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D);

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_D6_JOINT)
		{
			APhysXD6JointDesc* pDestJointDesc = (APhysXD6JointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXD6JointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}

public:
	
	// to do... lots of new data members...

	/* Constraints */
	NxD6JointMotion         mXMotion, mYMotion, mZMotion; 
	NxD6JointMotion         mSwing1Motion, mSwing2Motion, mTwistMotion;
	
	NxJointLimitSoftDesc        mLinearLimit;
	NxJointLimitSoftDesc        mSwing1Limit; 
	NxJointLimitSoftDesc        mSwing2Limit;
	NxJointLimitSoftPairDesc    mTwistLimit;
	
	/* drive */
	NxJointDriveDesc        mXDrive, mYDrive, mZDrive;
	NxJointDriveDesc        mSwingDrive, mTwistDrive;
	NxJointDriveDesc        mSlerpDrive;
	
	NxVec3                  mDrivePosition;
	NxQuat                  mDriveOrientation;
	NxVec3                  mDriveLinearVelocity;
	NxVec3                  mDriveAngularVelocity;
	NxJointProjectionMode   mProjectionMode;
	
	float					mProjectionDistance; 
	float					mProjectionAngle;    
	float					mGearRatio;
	APhysXU32				mD6JointFlags;
};


#endif