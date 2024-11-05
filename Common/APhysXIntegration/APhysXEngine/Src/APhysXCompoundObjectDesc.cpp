
#include "APhysXStdAfx.h"

bool APhysXCompoundObjectDesc::Save(NxStream* pNxStream)
{
	if(!APhysXObjectDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	int iSize = m_arrObjectComponentDescs.size();
	pNxStream->storeDword(iSize);
	for(int i=0; i<iSize; i++)
	{
		APhysXObjectComponentDesc* pObjComponentDesc = m_arrObjectComponentDescs[i];

		// save the shape type
		pNxStream->storeDword(pObjComponentDesc->mPhysXObjectDesc->GetObjType());

		pObjComponentDesc->mPhysXObjectDesc->Save(pNxStream);
		
		pNxStream->storeBuffer((const void*)(&pObjComponentDesc->mLocalPose), sizeof(NxMat34));

		APhysXSerializerBase::StoreString(pNxStream, pObjComponentDesc->mLocalName);
	}



	return true;

}

bool APhysXCompoundObjectDesc::Load(NxStream* pNxStream)
{ 
	if(!APhysXObjectDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{

		Release();

		int iSize = 0;
		iSize = pNxStream->readDword();
		for(int i=0; i<iSize; i++)
		{
			APhysXU32 ObjectType = pNxStream->readDword();
			APhysXObjectDesc* pObjectDesc = APhysXObjectDescManager::CreateObjectDesc((APhysXObjectType)ObjectType);
			if(pObjectDesc->Load(pNxStream))
			{
				APhysXObjectComponentDesc* pObjComponentDesc = new APhysXObjectComponentDesc;
				pObjComponentDesc->mPhysXObjectDesc = pObjectDesc;
				pNxStream->readBuffer((void*)(&pObjComponentDesc->mLocalPose), sizeof(NxMat34));
				
				pObjComponentDesc->mLocalName = APhysXSerializerBase::ReadString(pNxStream);

				m_arrObjectComponentDescs.push_back(pObjComponentDesc);
			}
			else
			{
				delete pObjectDesc;
				gPhysXLog->Log("Warning: Failed on loading APhysXObjectDesc.");

				return false;
			}
		}


		return true;

	}
	else
		return false;


}

const APhysXCompoundObjectDesc& APhysXCompoundObjectDesc::operator= (const APhysXCompoundObjectDesc& other)
{
	if(this == &other) return *this;

	// clone parents' data
	(APhysXObjectDesc& ) *this = (APhysXObjectDesc& )other;

	// copy object components...
	other.CopyObjectComponentDescs(this, true);

	return *this;
}

/*
APhysXObjectDesc* APhysXCompoundObjectDesc::Clone() const
{
	CloneThis(APhysXCompoundObjectDesc, APhysXObjectDesc)
}
*/

void  APhysXCompoundObjectDesc::CompleteGlobalObjectComponentName()
{
	char szGlobalName[1024];

	for(int i=0; i<m_arrObjectComponentDescs.size(); i++)
	{
		APhysXObjectDesc* pComponentObject = m_arrObjectComponentDescs[i]->mPhysXObjectDesc;
		if(GetName())
		{
			sprintf(szGlobalName, "%s_%s", GetName(), m_arrObjectComponentDescs[i]->mLocalName);
		}
		else
		{
			sprintf(szGlobalName, "CommonCompoundObjectDescName_%s", m_arrObjectComponentDescs[i]->mLocalName);
		}

		
		pComponentObject->SetName(szGlobalName);
	}


}

void APhysXCompoundObjectDesc::Release()
{
	for(int i=0; i<m_arrObjectComponentDescs.size(); i++)
	{
		delete m_arrObjectComponentDescs[i];
	}

	m_arrObjectComponentDescs.clear();
}

bool APhysXCompoundObjectDesc::AddObjectComponent(const APhysXObjectDesc* pObjectDesc, const char* szLoacalName, const NxMat34& matLocalPose)
{
	// note: the name of pObjectDesc is just a local name under 
	// the scope of APhysXCompoundObjectDesc/APhysXCompoundObject
	// make sure that pObjectDesc has a name!
	APHYSX_ASSERT(szLoacalName);

	if(szLoacalName)
	{
		APhysXObjectComponentDesc* pObjComponentDesc = new APhysXObjectComponentDesc;
		pObjComponentDesc->mPhysXObjectDesc = pObjectDesc->Clone();
		pObjComponentDesc->mLocalPose = matLocalPose;
		pObjComponentDesc->mLocalName = gGlobalStringManager->GetString(szLoacalName);

		m_arrObjectComponentDescs.push_back(pObjComponentDesc);

		return true;
	}
	else
		return false;
}

void APhysXCompoundObjectDesc::CopyObjectComponentDescs(APhysXCompoundObjectDesc* pOtherCompoundObjectDesc, bool bReleaseOtherComponents) const 
{
	if(bReleaseOtherComponents)
	{
		pOtherCompoundObjectDesc->Release();
	}

	for(int i=0; i<GetObjectComponentDescNum(); i++)
	{
		APhysXObjectComponentDesc* pCurObjComponentDesc = GetObjectComponentDesc(i);
		pOtherCompoundObjectDesc->AddObjectComponent(pCurObjComponentDesc->mPhysXObjectDesc, pCurObjComponentDesc->mLocalName, pCurObjComponentDesc->mLocalPose);
	}

}