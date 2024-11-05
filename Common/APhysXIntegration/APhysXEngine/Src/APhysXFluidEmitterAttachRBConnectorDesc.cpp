
#include "APhysXStdAfx.h"

bool APhysXFluidEmitterAttachRBConnectorDesc::Save(NxStream* pNxStream)
{
	if(!APhysXObjectConnectorDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);

	pNxStream->storeBuffer((const void*)&mFluidEmitterParameters, sizeof(APhysXFluidEmitterParameters));

	APhysXSerializerBase::StoreString(pNxStream, mAttachedNxActorName);


	return true;

}

bool APhysXFluidEmitterAttachRBConnectorDesc::Load(NxStream* pNxStream)
{
	if(!APhysXObjectConnectorDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

				
		pNxStream->readBuffer((void*)&mFluidEmitterParameters, sizeof(APhysXFluidEmitterParameters));

		mAttachedNxActorName = APhysXSerializerBase::ReadString(pNxStream);


		return true;
	}
	else
	{
		APHYSX_ASSERT(0);
		return false;
	}

}
