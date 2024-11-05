
#include "APhysXStdAfx.h"

bool APhysXClothAttachRBConnectorDesc::Save(NxStream* pNxStream)
{
	if(!APhysXObjectConnectorDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeDword(mOnlyAttachSpecifiedObject ? 1 : 0);
	
	APhysXSerializerBase::StoreString(pNxStream, mAttachedNxActorName);

	pNxStream->storeDword(mAttachmentFlags);

	return true;
}

bool APhysXClothAttachRBConnectorDesc::Load(NxStream* pNxStream)
{ 
	if(!APhysXObjectConnectorDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{
		
		mOnlyAttachSpecifiedObject = pNxStream->readDword() ? true : false;

		mAttachedNxActorName = APhysXSerializerBase::ReadString(pNxStream);

		mAttachmentFlags = pNxStream->readDword();
		
		return true;
	}
	else
	{
		APHYSX_ASSERT(0);
		return false;
	}

}

