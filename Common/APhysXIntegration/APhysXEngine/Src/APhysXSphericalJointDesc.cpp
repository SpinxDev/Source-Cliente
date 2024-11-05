

#include "APhysXStdAfx.h"


bool APhysXSphericalJointDesc::Save(NxStream* pNxStream)
{
	if(!APhysXJointDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeBuffer((const void*)(&mSwingAxis), sizeof(NxVec3));
	pNxStream->storeFloat(mProjectionDistance);

	pNxStream->storeBuffer((const void*)(&mTwistLimit), sizeof(NxJointLimitPairDesc));
	pNxStream->storeBuffer((const void*)(&mSwingLimit), sizeof(NxJointLimitDesc));

	pNxStream->storeBuffer((const void*)(&mTwistSpring), sizeof(NxSpringDesc));
	pNxStream->storeBuffer((const void*)(&mSwingSpring), sizeof(NxSpringDesc));
	pNxStream->storeBuffer((const void*)(&mJointSpring), sizeof(NxSpringDesc));


	pNxStream->storeDword(mSphericalJointFlags);
	pNxStream->storeDword(mProjectionMode);

	return true;
}

bool APhysXSphericalJointDesc::Load(NxStream* pNxStream)
{

	if(!APhysXJointDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

		pNxStream->readBuffer((void *)&mSwingAxis, sizeof(NxVec3));
		mProjectionDistance = pNxStream->readFloat();

		pNxStream->readBuffer((void *)&mTwistLimit, sizeof(NxJointLimitPairDesc));
		pNxStream->readBuffer((void *)&mSwingLimit, sizeof(NxJointLimitDesc));
		
		pNxStream->readBuffer((void *)&mTwistSpring, sizeof(NxSpringDesc));
		pNxStream->readBuffer((void *)&mSwingSpring, sizeof(NxSpringDesc));
		pNxStream->readBuffer((void *)&mJointSpring, sizeof(NxSpringDesc));

		mSphericalJointFlags = pNxStream->readDword();
		mProjectionMode = (NxJointProjectionMode)pNxStream->readDword();


		return true;
	}
	else
	{
		APHYSX_ASSERT(0);
		return false;
	}
}

void APhysXSphericalJointDesc::GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D)
{
	// get the parents' data members...
	APhysXJointDesc::GetNxJointDesc(jointDesc, globalPose, vScale3D);

	APHYSX_ASSERT(jointDesc.getType() == NX_JOINT_SPHERICAL);

	NxSphericalJointDesc& sphericalJointDesc = (NxSphericalJointDesc&) jointDesc;

	
	sphericalJointDesc.swingAxis = mSwingAxis;
	sphericalJointDesc.projectionDistance = mProjectionDistance;

	sphericalJointDesc.twistLimit = mTwistLimit;
	sphericalJointDesc.swingLimit = mSwingLimit;
	
	sphericalJointDesc.twistSpring = mTwistSpring;
	sphericalJointDesc.swingSpring = mSwingSpring;
	sphericalJointDesc.jointSpring = mJointSpring;

	sphericalJointDesc.flags = mSphericalJointFlags;
	sphericalJointDesc.projectionMode = mProjectionMode;


}


