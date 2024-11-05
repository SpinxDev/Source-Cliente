
#include "APhysXStdAfx.h"

#define MAX_STORED_STR_LEN 1024

void APhysXSerializerBase::StoreString(NxStream* pNxStream, const char* str)
{
	// save the length first
	NxU32 l = str? ((NxU32)strlen(str)) : 0;
	if(l >= MAX_STORED_STR_LEN) return;

	pNxStream->storeDword(l);

	// then save the string
	if(l)
	{
		pNxStream->storeBuffer(str, l+1);
	}
}

const char* APhysXSerializerBase::ReadString(NxStream* pNxStream)
{
	APhysXU32 l = pNxStream->readDword();
	APHYSX_ASSERT(l < MAX_STORED_STR_LEN);
	if(l)
	{
		char buf[MAX_STORED_STR_LEN];
		pNxStream->readBuffer(buf, l+1);
		return gGlobalStringManager->GetString(buf);
	}
	else
		return NULL;

}

bool APhysXSerializerBase::Save(NxStream* pNxStream)
{
	SaveVersion(pNxStream);
	SaveName(pNxStream);

	return true;
}

bool APhysXSerializerBase::Load(NxStream* pNxStream)
{
	//LoadVersion(pNxStream);
	m_StreamObjVer = pNxStream->readDword();
	LoadName(pNxStream);

	return true;
}
