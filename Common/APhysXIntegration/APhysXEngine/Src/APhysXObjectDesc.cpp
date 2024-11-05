
#include "APhysXStdAfx.h"

bool APhysXObjectDesc::Save(NxStream* pNxStream)
{
	APhysXSerializerBase::Save(pNxStream);

	// note the order!
	
	// we'd better save the ObjType via the factory class
	//pNxStream->storeDword(GetObjType());
	
	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeDword(mCollisionGroup);
	if(mNetworkSync)
		pNxStream->storeDword(1);
	else
		pNxStream->storeDword(0);

	mInnerConnectorDescManager.Save(pNxStream);

	pNxStream->storeFloat(mLifeSpan);

	if(mSimInCompartment)
		pNxStream->storeDword(1);
	else
		pNxStream->storeDword(0);
	
	pNxStream->storeDword(mFlags);

	return true;
}

bool APhysXObjectDesc::Load(NxStream* pNxStream)
{ 
	APhysXSerializerBase::Load(pNxStream);

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer >= 0xcc000001)
	{
		// we'd better save the ObjType via the factory class
		//if(GetObjType()==pNxStream->readDword())
		
		mCollisionGroup = pNxStream->readDword();
		mNetworkSync = (pNxStream->readDword() == 1);
	
		if(!mInnerConnectorDescManager.Load(pNxStream))
			return false;

	}
	
	if(m_StreamObjVer >= 0xcc000002)
	{
		// load other data member...
		mLifeSpan = pNxStream->readFloat();
	}

	if(m_StreamObjVer >= 0xcc000003)
	{
		// load other data member...
		mSimInCompartment = (pNxStream->readDword() == 1);
	}

	if(m_StreamObjVer >= 0xcc000004)
	{
		// load other data member...
		mFlags = pNxStream->readDword();
	}

	return true;
}

APhysXU32 APhysXObjectDesc::GetCollisionGroup() const
{
	if(mFlags & APX_OBJFLAG_SKIP_COLLISION_WITH_NON_DYNAMIC_RB)
	{
		return APX_COLLISION_GROUP_FILTER_OUT_NON_DYNAMIC_RB;
	}
	else
		return mCollisionGroup;

}

APhysXCollisionChannel APhysXObjectDesc::GetCollisionChannel() const
{
	if(mFlags & APX_OBJFLAG_CC_SKIP_COLLISION)
	{
		return APX_COLLISION_CHANNEL_CC_SKIP_COLLISION;
	}
	else
		return APX_COLLISION_CHANNEL_COMMON;
	

}


