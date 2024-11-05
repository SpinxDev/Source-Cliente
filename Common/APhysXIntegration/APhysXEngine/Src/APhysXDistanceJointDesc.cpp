

#include "APhysXStdAfx.h"

bool APhysXDistanceJointDesc::Save(NxStream* pNxStream)
{
	if(!APhysXJointDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeFloat(mMaxDistance);
	pNxStream->storeFloat(mMinDistance);

	pNxStream->storeBuffer((const void*)(&mSpring), sizeof(NxSpringDesc));
	pNxStream->storeDword(mDistanceJointFlags);

	return true;
}

bool APhysXDistanceJointDesc::Load(NxStream* pNxStream)
{
	if(!APhysXJointDesc::Load(pNxStream))
		return false;
	
	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{
		mMaxDistance = pNxStream->readFloat();
		mMinDistance = pNxStream->readFloat();

		pNxStream->readBuffer((void *)&mSpring, sizeof(NxSpringDesc));

		mDistanceJointFlags = pNxStream->readDword();
		
		return true;
	}
	else
	{
		APHYSX_ASSERT(0);
		return false;
	}

}

void APhysXDistanceJointDesc::GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D)
{
	// get the parents' data members...
	APhysXJointDesc::GetNxJointDesc(jointDesc, globalPose, vScale3D);

	// transform and scale the distance...
	mMaxDistance *= vScale3D.x;
	mMinDistance *= vScale3D.x;

	APHYSX_ASSERT(jointDesc.getType() == NX_JOINT_DISTANCE);

	NxDistanceJointDesc& distJointDesc = (NxDistanceJointDesc&) jointDesc;

	// note here:
	// we should ignore the globalAnchor we set since the distance joint always 
	// has the local anchor in zero position
	distJointDesc.localAnchor[0].zero();
	distJointDesc.localAnchor[1].zero();

	distJointDesc.maxDistance = mMaxDistance;
	distJointDesc.minDistance = mMinDistance;
	distJointDesc.spring = mSpring;
	distJointDesc.flags = mDistanceJointFlags;


}
