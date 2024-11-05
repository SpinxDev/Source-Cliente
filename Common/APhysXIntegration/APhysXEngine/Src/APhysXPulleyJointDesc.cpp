
#include "APhysXStdAfx.h"

bool APhysXPulleyJointDesc::Save(NxStream* pNxStream)
{
	if(!APhysXJointDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeBuffer((const void*)(&mPulley), 2 * sizeof(NxVec3));

	pNxStream->storeFloat(mDistance);
	pNxStream->storeFloat(mStiffness);
	pNxStream->storeFloat(mRatio);

	pNxStream->storeDword(mPulleyJointFlags);

	pNxStream->storeBuffer((const void*)(&mMotor), sizeof(NxMotorDesc));
	

	return true;
}

bool APhysXPulleyJointDesc::Load(NxStream* pNxStream)
{
	if(!APhysXJointDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{
		pNxStream->readBuffer((void *)&mPulley, 2 * sizeof(NxVec3));

		mDistance = pNxStream->readFloat();
		mStiffness = pNxStream->readFloat();
		mRatio = pNxStream->readFloat();

		mPulleyJointFlags = pNxStream->readDword();

		pNxStream->readBuffer((void *)&mMotor, sizeof(NxMotorDesc));

		return true;
	}
	else
	{
		APHYSX_ASSERT(0);
		return false;
	}

}

void APhysXPulleyJointDesc::GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D)
{
	// get the parents' data members...
	APhysXJointDesc::GetNxJointDesc(jointDesc, globalPose, vScale3D);

	APHYSX_ASSERT(jointDesc.getType() == NX_JOINT_PULLEY);

	NxPulleyJointDesc& pulleyJointDesc = (NxPulleyJointDesc&) jointDesc;

	pulleyJointDesc.pulley[0].arrayMultiply(mPulley[0], vScale3D);
	pulleyJointDesc.pulley[0] = globalPose * pulleyJointDesc.pulley[0];
	
	pulleyJointDesc.pulley[1].arrayMultiply(mPulley[1], vScale3D);
	pulleyJointDesc.pulley[1] = globalPose * pulleyJointDesc.pulley[1];

	pulleyJointDesc.distance = mDistance * vScale3D.x;
	pulleyJointDesc.stiffness = mStiffness;
	pulleyJointDesc.ratio = mRatio;

	pulleyJointDesc.flags = mPulleyJointFlags;

	pulleyJointDesc.motor = mMotor;

}
