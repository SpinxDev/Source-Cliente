
#include "APhysXStdAfx.h"


APhysXMaterialDesc gDefaultPhysXMaterialDesc(0.3f, 0.6f, 0.6f);


bool APhysXMaterialDesc::Save(NxStream* pNxStream)
{
	if(!APhysXSerializerBase::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeFloat(mRestitution);
	pNxStream->storeFloat(mDynamicFriction);
	pNxStream->storeFloat(mStaticFriction);

	return true;

}

bool APhysXMaterialDesc::Load(NxStream* pNxStream)
{
	if(!APhysXSerializerBase::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{
		mRestitution = pNxStream->readFloat();
		mDynamicFriction = pNxStream->readFloat();
		mStaticFriction = pNxStream->readFloat();

		return true;
	}
	else
		return false;

}

