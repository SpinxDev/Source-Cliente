
#include "APhysXStdAfx.h"

APhysXObjectDesc* APhysXObjectDescManager::CreateObjectDesc(APhysXObjectType apxObjectType, const char* szName)
{
	APhysXObjectDesc* pObjectDesc = NULL;

	switch (apxObjectType)
	{

		// rigid-body object...
	case APX_OBJTYPE_DUMMY:

		pObjectDesc = new APhysXDummyRBObjectDesc;

		break;

	case APX_OBJTYPE_STATIC:

		pObjectDesc = new APhysXStaticRBObjectDesc;

		break;

	case APX_OBJTYPE_DYNAMIC:

		pObjectDesc = new APhysXDynamicRBObjectDesc;

		break;

	case APX_OBJTYPE_TERRAIN_PATCH:

		pObjectDesc = new APhysXTerrainPatchRBObjectDesc;

		break;

	case APX_OBJTYPE_SKELETON:

		pObjectDesc = new APhysXSkeletonRBObjectDesc;

		break;


	case APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE:

		pObjectDesc = new APhysXSimpleBreakableSkeletonRBObjectDesc;

		break;

	case APX_OBJTYPE_SKELETON_BREAKABLE:

		pObjectDesc = new APhysXBreakableSkeletonRBObjectDesc;

		break;

		/*
		case APX_OBJTYPE_BONE:

		break;


		case APX_OBJTYPE_CHARACTER:

		break;
		case APX_OBJTYPE_VEHICLE:

		break;

		case APX_OBJTYPE_BREAKABLE:

		break;
		*/

	// cloth
	case APX_OBJTYPE_CLOTH:

		pObjectDesc = new APhysXClothObjectDesc;

		break;

	// fluid
	case APX_OBJTYPE_FLUID:

		pObjectDesc = new APhysXFluidObjectDesc;

		break;

	case APX_OBJTYPE_FLUID_ORIENTED_PARTICLE:

		pObjectDesc = new APhysXOrientedParticleObjectDesc;

		break;

	// force field...
	case APX_OBJTYPE_FORCEFIELD:

		pObjectDesc = new APhysXForceFieldObjectDesc;

		break;

	case APX_OBJTYPE_FORCEFIELD_VACUUM:

		pObjectDesc = new APhysXVacuumForceFieldObjectDesc;

		break;

	case APX_OBJTYPE_FORCEFIELD_GRAVITATION:

		pObjectDesc = new APhysXGravitationForceFieldObjectDesc;

		break;

	case APX_OBJTYPE_FORCEFIELD_BUOYANCY:

		pObjectDesc = new APhysXBuoyancyForceFieldObjectDesc;

		break;

	case APX_OBJTYPE_FORCEFIELD_EXPLOSION:

		pObjectDesc = new APhysXExplosionForceFieldObjectDesc;

		break;

	case APX_OBJTYPE_FORCEFIELD_WIND:

		pObjectDesc = new APhysXWindForceFieldObjectDesc;

		break;

	case APX_OBJTYPE_FORCEFIELD_VORTEX:

		pObjectDesc = new APhysXVortexForceFieldObjectDesc;

		break;

	case APX_OBJTYPE_FORCEFIELD_BOSS:

		pObjectDesc = new APhysXBossForceFieldObjectDesc;

		break;

	// compound...
	case APX_OBJTYPE_COMPOUND:

		pObjectDesc = new APhysXCompoundObjectDesc;

		break;

	case APX_OBJTYPE_ATTACHER:

		pObjectDesc = new APhysXAttacherDesc;

		break;


	case APX_OBJTYPE_RB_ATTACHER:

		pObjectDesc = new APhysXRBAttacherDesc;

		break;


	case APX_OBJTYPE_CLOTH_ATTACHER:

		pObjectDesc = new APhysXClothAttacherDesc;

		break;



	case APX_OBJTYPE_FLUIDEMITTER_ATTACHER:

		pObjectDesc = new APhysXFluidEmitterAttacherDesc;

		break;

	default:
		APHYSX_ASSERT(!"Oops! Unknown Object Descriptor!");
		break;
	};


	if(pObjectDesc)
	{
		pObjectDesc->SetName(szName);
	}

	return pObjectDesc;
}


APhysXObjectDesc* APhysXObjectDescManager::CreateAndPushObjectDesc(APhysXObjectType apxObjectType, const char* szName)
{
	APhysXObjectDesc* pObjectDesc = CreateObjectDesc(apxObjectType, szName);

	if(pObjectDesc)
	{
		APhysXMutexLock apxMutexLock(m_MutexObjectDescs);

		m_arrObjectDescs.push_back(pObjectDesc);
	}

	return pObjectDesc;

}

void APhysXObjectDescManager::ReleaseObjectDesc(APhysXObjectDesc* pObjectDesc, bool bDeleteObjectDesc )
{
	//??? should I check the RefCount first before the release? Maybe I'd better add another function
	// which is just to remove the entry from the map...

	// if( !pObjectDesc || pObjectDesc->GetRefCount() > 0 ) return;
	if( !pObjectDesc ) return;

	APhysXMutexLock apxMutexLock(m_MutexObjectDescs);

	m_arrObjectDescs.deleteEntry(pObjectDesc);
	
	for(APhysXObjectDescMap::iterator it = m_mapObjectDesc.begin(); it != m_mapObjectDesc.end(); ++it)
	{
		if(pObjectDesc == it->second)
		{
			m_mapObjectDesc.erase(it);
			break;
		}
	}

	if(bDeleteObjectDesc)
	{
		delete pObjectDesc;
		//pObjectDesc = NULL;
	}

}

void APhysXObjectDescManager::ReleaseObjectDesc(const char* szFileName, const char* szObjName)
{
	APhysXString strObjName;
	if(szObjName)
	{
		strObjName.Format("%s<%s>", szFileName, szObjName);
	}
	else
	{
		char szDefaultObjName[128];
		sprintf(szDefaultObjName, "%s%d", APHYSX_OBJECTDESCNAME_PREFIX, 0);

		strObjName.Format("%s<%s>", szFileName, szDefaultObjName);
	}

	APhysXMutexLock apxMutexLock(m_MutexObjectDescs);

	APhysXObjectDescMap::iterator it = m_mapObjectDesc.find(strObjName);
	if(it != m_mapObjectDesc.end())
	{
		// we find it in the map
		APhysXObjectDesc* pObjectDesc = it->second;

		if(0 == pObjectDesc->GetRefCount())
		{
			m_mapObjectDesc.erase(it);
			m_arrObjectDescs.deleteEntry(pObjectDesc);
			
			delete pObjectDesc;

		}
	}

}

bool APhysXObjectDescManager::Save(NxStream* pNxStream)
{
	if(!pNxStream) return false;

	// firstly, we save the number of the array
	pNxStream->storeDword(m_arrObjectDescs.size());

	// then we save the each element of array
	char szName[128];

	APhysXMutexLock apxMutexLock(m_MutexObjectDescs);

	for(int i=0; i<m_arrObjectDescs.size(); i++)
	{
		APhysXObjectDesc* pObjectDesc = m_arrObjectDescs[i];

		// save the shape type
		pNxStream->storeDword(pObjectDesc->GetObjType());

		// just make sure that each APhysXObjectDesc has a name before saving them
		if(!pObjectDesc->GetName())
		{
			sprintf(szName, "%s%d", APHYSX_OBJECTDESCNAME_PREFIX, i);
			pObjectDesc->SetName(szName);
		}

		// save the shape data
		pObjectDesc->Save(pNxStream);

	}

	return true;
}


bool APhysXObjectDescManager::Load(NxStream* pNxStream, const char* szFileName)
{
	if(!pNxStream) return false;

	APhysXU32 l = pNxStream->readDword();
	if(l)
	{
		// not clear any more, because we will support load data again and again from
		// more than one files...
		//m_arrObjectDescs.clear();

		for(int i=0; i<l; i++)
		{
			APhysXU32 ObjectType = pNxStream->readDword();
			APhysXObjectDesc* pObjectDesc = CreateObjectDesc((APhysXObjectType)ObjectType);

			APHYSX_ASSERT(pObjectDesc);

			if(!pObjectDesc->Load(pNxStream))
			{
				// fail to load...
				delete pObjectDesc;

				gPhysXLog->Log("Warning: Failed on loading APhysXObjectDesc.");

				return false;

				// APHYSX_ASSERT(0);
			}
			else
			{
				m_MutexObjectDescs.Lock();

				m_arrObjectDescs.push_back(pObjectDesc);

				APhysXString strObjName;
				strObjName.Format("%s<%s>", szFileName, pObjectDesc->GetName());

				m_mapObjectDesc.insert(APhysXObjectDescPair(strObjName, pObjectDesc));

				m_MutexObjectDescs.Unlock();

				/*
				
				// revised by wenfeng, Jun. 17, 2009
				// do not push one item more than once...

				// and, we also insert the first item just with the name of szFileName so that
				// we can get the item when szObjName is not provided...
				if(i == 0)
				{
					strObjName.Format("%s", szFileName);
					m_mapObjectDesc.insert(APhysXObjectDescPair(strObjName, pObjectDesc));
				}
				*/


			}

		}
	}

	return true;
}


bool APhysXObjectDescManager::Save(const char* szFileName)
{
	APhysXUserStream apxFile(szFileName, false);
	if (apxFile.IsValid())
		return Save(&apxFile);
	return false;
}

bool APhysXObjectDescManager::Load(const char* szFileName)
{
	APhysXUserStream apxFile(szFileName, true);
	if (apxFile.IsValid())
		return Load(&apxFile, szFileName);
	return false;
}

bool APhysXObjectDescManager::LoadFromFileImage(const char* szFileName)
{
	APhysXFileImageStream apxFile(szFileName, true);
	return Load(&apxFile, szFileName);
}


void APhysXObjectDescManager::Release()
{
	APhysXMutexLock apxMutexLock(m_MutexObjectDescs);

	for(int i=0; i< m_arrObjectDescs.size(); i++)
	{
		delete m_arrObjectDescs[i];
	}

	m_arrObjectDescs.clear();
	m_mapObjectDesc.clear();

}


APhysXObjectDesc* APhysXObjectDescManager::GetPhysXObjectDesc(const char* szFileName, const char* szObjName)
{
	// construct the key
	APhysXString strObjName;
	if(szObjName)
	{
		strObjName.Format("%s<%s>", szFileName, szObjName);
	}
	else
	{
		char szDefaultObjName[128];
		sprintf(szDefaultObjName, "%s%d", APHYSX_OBJECTDESCNAME_PREFIX, 0);

		strObjName.Format("%s<%s>", szFileName, szDefaultObjName);
	}

	APhysXMutexLock apxMutexLock(m_MutexObjectDescs);

	APhysXObjectDescMap::iterator it = m_mapObjectDesc.find(strObjName);
	if(it != m_mapObjectDesc.end())
	{
		return it->second;
	}

	// otherwise, we have to load the file...
	//Load(szFileName);
	LoadFromFileImage(szFileName);

	// search the map again
	it = m_mapObjectDesc.find(strObjName);
	if(it != m_mapObjectDesc.end())
	{
		return it->second;
	}
	else
		return NULL;
}

bool APhysXObjectDescManager::AddObjectDescRef(APhysXObjectDesc* pObjectDesc)
{
	APhysXMutexLock apxMutexLock(m_MutexObjectDescs);

	// firstly, we search whether pObjectDesc is in my array...
	for(int i=0; i<m_arrObjectDescs.size(); i++)
	{
		if(pObjectDesc == m_arrObjectDescs[i])
		{
	
	// secondly, we add reference to that pointer...
			pObjectDesc->AddRef();
			return true;
		}
	}

	return false;


}

bool APhysXObjectDescManager::RemoveObjectDescRef(APhysXObjectDesc* pObjectDesc)
{
	APhysXMutexLock apxMutexLock(m_MutexObjectDescs);

	// firstly, we search whether pObjectDesc is in my array...
	bool bRemoveEntry = false;

	for(int i=0; i<m_arrObjectDescs.size(); i++)
	{
		if(pObjectDesc == m_arrObjectDescs[i])
		{
			bRemoveEntry = (0==pObjectDesc->RemoveRef());
			break;
		}
	}

	// then, if we find it, remove it from array
	if(bRemoveEntry)
	{
		ReleaseObjectDesc(pObjectDesc);

		return true;
	}

	return false;

}