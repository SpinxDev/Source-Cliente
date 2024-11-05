
#include "APhysXStdAfx.h"

bool APhysXActorDesc::Save(NxStream* pNxStream)
{
	APhysXSerializerBase::Save(pNxStream);

	// save my version...
	SaveVersion(pNxStream);
	
	// save body parameters
	if(mHasBody)
	{
		pNxStream->storeDword(1);

		pNxStream->storeDword(mBodyFlags);
		pNxStream->storeBuffer((const void*)(&mLinearVelocity), sizeof(NxVec3));
		pNxStream->storeBuffer((const void*)(&mAngularVelocity), sizeof(NxVec3));
		pNxStream->storeFloat(mLinearDamping);
		pNxStream->storeFloat(mAngularDamping);

		pNxStream->storeDword(mSolverIterationCount);
	}
	else
		pNxStream->storeDword(0);

	// save actor parameters
	pNxStream->storeDword(mActorFlags);
	pNxStream->storeDword(mActorGroup);
	pNxStream->storeFloat(mDensity);

	pNxStream->storeBuffer((const void*)(&mLocalPose), sizeof(NxMat34));
	pNxStream->storeBuffer((const void*)(&mScale3D), sizeof(NxVec3));
	
	return true;
}


bool APhysXActorDesc::Load(NxStream* pNxStream)
{ 
	// load the version and name
	APhysXSerializerBase::Load(pNxStream);

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer >= 0xcc000001)
	{

		mHasBody = (pNxStream->readDword() == 1);
		if(mHasBody)
		{
			mBodyFlags = pNxStream->readDword();
			pNxStream->readBuffer((void *)(&mLinearVelocity), sizeof(NxVec3));
			pNxStream->readBuffer((void *)(&mAngularVelocity), sizeof(NxVec3));
			mLinearDamping = pNxStream->readFloat();
			mAngularDamping = pNxStream->readFloat();

			if(m_StreamObjVer == GetVersion())
			{
				mSolverIterationCount = pNxStream->readDword();
			}

		}
		
		mActorFlags = pNxStream->readDword();
		mActorGroup = pNxStream->readDword();
		mDensity = pNxStream->readFloat();

		pNxStream->readBuffer((void*)(&mLocalPose), sizeof(NxMat34));

	}

	if(m_StreamObjVer >= 0xcc000002)
	{
		pNxStream->readBuffer((void*)(&mScale3D), sizeof(NxVec3));
	}


	return true;

}

