
#include "APhysXStdAfx.h"

bool APhysXD6JointDesc::Save(NxStream* pNxStream)
{
	if(!APhysXJointDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	// constraints...
	pNxStream->storeDword(mXMotion);
	pNxStream->storeDword(mYMotion);
	pNxStream->storeDword(mZMotion);
	
	pNxStream->storeDword(mSwing1Motion);
	pNxStream->storeDword(mSwing2Motion);
	pNxStream->storeDword(mTwistMotion);

	pNxStream->storeBuffer((const void*)(&mLinearLimit), sizeof(NxJointLimitSoftDesc));
	pNxStream->storeBuffer((const void*)(&mSwing1Limit), sizeof(NxJointLimitSoftDesc));
	pNxStream->storeBuffer((const void*)(&mSwing2Limit), sizeof(NxJointLimitSoftDesc));
	pNxStream->storeBuffer((const void*)(&mTwistLimit), sizeof(NxJointLimitSoftPairDesc));

	// drive...
	pNxStream->storeBuffer((const void*)(&mXDrive), sizeof(NxJointDriveDesc));
	pNxStream->storeBuffer((const void*)(&mYDrive), sizeof(NxJointDriveDesc));
	pNxStream->storeBuffer((const void*)(&mZDrive), sizeof(NxJointDriveDesc));

	pNxStream->storeBuffer((const void*)(&mSwingDrive), sizeof(NxJointDriveDesc));
	pNxStream->storeBuffer((const void*)(&mTwistDrive), sizeof(NxJointDriveDesc));
	pNxStream->storeBuffer((const void*)(&mSlerpDrive), sizeof(NxJointDriveDesc));

	pNxStream->storeBuffer((const void*)(&mDrivePosition), sizeof(NxVec3));
	pNxStream->storeBuffer((const void*)(&mDriveOrientation), sizeof(NxQuat));
	pNxStream->storeBuffer((const void*)(&mDriveLinearVelocity), sizeof(NxVec3));
	pNxStream->storeBuffer((const void*)(&mDriveAngularVelocity), sizeof(NxVec3));

	pNxStream->storeDword(mProjectionMode);

	pNxStream->storeFloat(mProjectionDistance);
	pNxStream->storeFloat(mProjectionAngle);
	pNxStream->storeFloat(mGearRatio);

	pNxStream->storeDword(mD6JointFlags);

	return true;
}

bool APhysXD6JointDesc::Load(NxStream* pNxStream)
{
	if(!APhysXJointDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{
		// constraints...
		mXMotion = (NxD6JointMotion )pNxStream->readDword();	
		mYMotion = (NxD6JointMotion )pNxStream->readDword();
		mZMotion = (NxD6JointMotion )pNxStream->readDword();
		
		mSwing1Motion = (NxD6JointMotion )pNxStream->readDword();
		mSwing2Motion = (NxD6JointMotion )pNxStream->readDword();
		mTwistMotion = (NxD6JointMotion )pNxStream->readDword();

		pNxStream->readBuffer((void*)(&mLinearLimit), sizeof(NxJointLimitSoftDesc));
		pNxStream->readBuffer((void*)(&mSwing1Limit), sizeof(NxJointLimitSoftDesc));
		pNxStream->readBuffer((void*)(&mSwing2Limit), sizeof(NxJointLimitSoftDesc));
		pNxStream->readBuffer((void*)(&mTwistLimit), sizeof(NxJointLimitSoftPairDesc));

		// drive...
		pNxStream->readBuffer((void*)(&mXDrive), sizeof(NxJointDriveDesc));
		pNxStream->readBuffer((void*)(&mYDrive), sizeof(NxJointDriveDesc));
		pNxStream->readBuffer((void*)(&mZDrive), sizeof(NxJointDriveDesc));

		pNxStream->readBuffer((void*)(&mSwingDrive), sizeof(NxJointDriveDesc));
		pNxStream->readBuffer((void*)(&mTwistDrive), sizeof(NxJointDriveDesc));
		pNxStream->readBuffer((void*)(&mSlerpDrive), sizeof(NxJointDriveDesc));

		pNxStream->readBuffer((void*)(&mDrivePosition), sizeof(NxVec3));
		pNxStream->readBuffer((void*)(&mDriveOrientation), sizeof(NxQuat));
		pNxStream->readBuffer((void*)(&mDriveLinearVelocity), sizeof(NxVec3));
		pNxStream->readBuffer((void*)(&mDriveAngularVelocity), sizeof(NxVec3));

		mProjectionMode = (NxJointProjectionMode)pNxStream->readDword();

		mProjectionDistance = pNxStream->readFloat();
		mProjectionAngle = pNxStream->readFloat();
		mGearRatio = pNxStream->readFloat();
		
		mD6JointFlags = pNxStream->readDword();
		
		return true;
	}
	else
	{

		APHYSX_ASSERT(0);
		return false;
	}
	
}

void APhysXD6JointDesc::GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D)
{
	// get the parents' data members...
	APhysXJointDesc::GetNxJointDesc(jointDesc, globalPose, vScale3D);

	APHYSX_ASSERT(jointDesc.getType() == NX_JOINT_D6);

	NxD6JointDesc& d6JointDesc = (NxD6JointDesc&) jointDesc;

	d6JointDesc.xMotion = mXMotion;
	d6JointDesc.yMotion = mYMotion;
	d6JointDesc.zMotion = mZMotion;
	
	d6JointDesc.swing1Motion = mSwing1Motion;
	d6JointDesc.swing2Motion = mSwing2Motion;
	d6JointDesc.twistMotion = mTwistMotion;

	d6JointDesc.linearLimit = mLinearLimit;
	//---------------------------------------------------------------------------
	// some testing code of linear limit
#if 0

	d6JointDesc.linearLimit.restitution = 0.f;
	d6JointDesc.linearLimit.spring = 10.f;
	d6JointDesc.linearLimit.damping = .8f;

#endif


	d6JointDesc.swing1Limit = mSwing1Limit;
	d6JointDesc.swing2Limit = mSwing2Limit;
	d6JointDesc.twistLimit = mTwistLimit;
	//---------------------------------------------------------------------------
	// some testing code...
#if 0

	d6JointDesc.swing1Limit.restitution = 0.1f;
	d6JointDesc.swing1Limit.spring = 0.6f;
	d6JointDesc.swing1Limit.damping = 0.3f;

	d6JointDesc.swing2Limit.restitution = 0.1f;
	d6JointDesc.swing2Limit.spring = 0.6f;
	d6JointDesc.swing2Limit.damping = 0.3f;

#endif
	//---------------------------------------------------------------------------

	d6JointDesc.drivePosition = mDrivePosition;
	d6JointDesc.driveOrientation = mDriveOrientation;
	d6JointDesc.driveLinearVelocity = mDriveLinearVelocity;
	d6JointDesc.driveAngularVelocity = mDriveAngularVelocity;

	//---------------------------------------------------------------------------
	// some testing code...
#if 1
	//----------------------------------------------------------
	// Noted by Wenfeng, Nov. 7, 2012
	// force to turn on the projection mode so as to correct the position errors
	// during the simulation while they are defined by D6 fixed joints.

	d6JointDesc.projectionMode = NX_JPM_POINT_MINDIST;				// turn on the projection mode...
	d6JointDesc.solverExtrapolationFactor = 1.1f;


	bool bFixJoint =	( mXMotion == NX_D6JOINT_MOTION_LOCKED ) &&
						( mYMotion == NX_D6JOINT_MOTION_LOCKED ) &&
						( mZMotion == NX_D6JOINT_MOTION_LOCKED ) &&
						( mSwing1Motion == NX_D6JOINT_MOTION_LOCKED ) &&
						( mSwing2Motion == NX_D6JOINT_MOTION_LOCKED ) &&
						( mTwistMotion == NX_D6JOINT_MOTION_LOCKED );

	d6JointDesc.projectionDistance = bFixJoint? 0.0001f : 0.01f;
	d6JointDesc.projectionAngle = bFixJoint? 0.0001f : mProjectionAngle;


#else

	d6JointDesc.projectionMode = mProjectionMode;
	d6JointDesc.projectionDistance = mProjectionDistance;
	d6JointDesc.projectionAngle = mProjectionAngle;

#endif
	//---------------------------------------------------------------------------


	d6JointDesc.gearRatio = mGearRatio;
	d6JointDesc.flags = mD6JointFlags;
		 

	//-------------------------------------------------------
	// test slerp drive code...
#if 0

    d6JointDesc.flags|=NX_D6JOINT_SLERP_DRIVE;    
	d6JointDesc.slerpDrive.driveType=NX_D6JOINT_DRIVE_POSITION;    
	d6JointDesc.slerpDrive.spring=50.0f; 
	d6JointDesc.slerpDrive.damping=10.0f; 

	d6JointDesc.twistMotion = NX_D6JOINT_MOTION_FREE;
	d6JointDesc.swing1Motion = NX_D6JOINT_MOTION_FREE;
	d6JointDesc.swing2Motion = NX_D6JOINT_MOTION_FREE;

#endif

}
