
#include "APhysXStdAfx.h"

bool APhysXRevoluteJointDesc::Save(NxStream* pNxStream)
{
	if(!APhysXJointDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);

	pNxStream->storeBuffer((const void*)(&mLimit), sizeof(NxJointLimitPairDesc));
	pNxStream->storeBuffer((const void*)(&mMotor), sizeof(NxMotorDesc));
	pNxStream->storeBuffer((const void*)(&mSpring), sizeof(NxSpringDesc));

	pNxStream->storeFloat(mProjectionDistance);
	pNxStream->storeFloat(mProjectionAngle);

	pNxStream->storeDword(mRevoluteJointFlags);
	pNxStream->storeDword(mProjectionMode);


	return true;
}

bool APhysXRevoluteJointDesc::Load(NxStream* pNxStream)
{
	if(!APhysXJointDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

		pNxStream->readBuffer((void *)&mLimit, sizeof(NxJointLimitPairDesc));
		pNxStream->readBuffer((void *)&mMotor, sizeof(NxMotorDesc));
		pNxStream->readBuffer((void *)&mSpring, sizeof(NxSpringDesc));
		
		mProjectionDistance = pNxStream->readFloat();
		mProjectionAngle = pNxStream->readFloat();

		

		mRevoluteJointFlags = pNxStream->readDword();
		mProjectionMode = (NxJointProjectionMode)pNxStream->readDword();

		return true;
	}
	else
	{
		APHYSX_ASSERT(0);
		return true;
	}

	
}

void APhysXRevoluteJointDesc::GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D)
{

	// get the parents' data members...
	APhysXJointDesc::GetNxJointDesc(jointDesc, globalPose, vScale3D);

	APHYSX_ASSERT(jointDesc.getType() == NX_JOINT_REVOLUTE);

	NxRevoluteJointDesc& revoluteJointDesc = (NxRevoluteJointDesc&) jointDesc;

	revoluteJointDesc.limit = mLimit;
	revoluteJointDesc.motor = mMotor;
	revoluteJointDesc.spring = mSpring;

	revoluteJointDesc.projectionDistance = mProjectionDistance;
	revoluteJointDesc.projectionAngle = mProjectionAngle;
	
	revoluteJointDesc.flags = mRevoluteJointFlags;
	revoluteJointDesc.projectionMode = mProjectionMode;

}

