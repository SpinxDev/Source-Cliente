
#include "APhysXStdAfx.h"

//-------------------------------------------------------------------------------------
// APhysXRigidBodyObjectDesc::APhysXActorComponent member function definitions...

APhysXRigidBodyObjectDesc::APhysXActorComponent* APhysXRigidBodyObjectDesc::APhysXActorComponent::Clone() const
{
	APhysXActorComponent* pActorComponent = new APhysXActorComponent;
	pActorComponent->ActorID = ActorID;

	for(int i=0; i<ShapeIDs.size(); i++)
		pActorComponent->ShapeIDs.push_back(ShapeIDs[i]);

	return pActorComponent;
}

void APhysXRigidBodyObjectDesc::APhysXActorComponent::Save(NxStream* pNxStream)
{
	pNxStream->storeDword(ActorID);

	int iSize = ShapeIDs.size();
	APHYSX_ASSERT(iSize);				// at least have one shape...
	pNxStream->storeDword(iSize);

	if(iSize)
	{
		for(int i = 0; i<iSize; i++)
		{
			pNxStream->storeDword(ShapeIDs[i]);
		}
	}

}

void APhysXRigidBodyObjectDesc::APhysXActorComponent::Load(NxStream* pNxStream)
{
	ActorID = pNxStream->readDword();
	int iSize = pNxStream->readDword();
	APHYSX_ASSERT(iSize);				// at least have one shape...

	ShapeIDs.clear();
	for(int i=0; i<iSize; i++)
	{

		int iCur = pNxStream->readDword();
		ShapeIDs.push_back(iCur);
	}
}

//-------------------------------------------------------------------------------------
// APhysXRigidBodyObjectDesc member functions

bool APhysXRigidBodyObjectDesc::Save(NxStream* pNxStream)
{
	if(!APhysXObjectDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	// actor descs array
	int iSize = m_arrActorDescs.size();
	pNxStream->storeDword(iSize);
	for(int i=0; i<iSize; i++)
	{
		APhysXActorDesc* pAPXActorDesc = m_arrActorDescs[i];
		pAPXActorDesc->Save(pNxStream);
		//m_arrActorDescs[i].Save(pNxStream);
	}

	// material descs array
	iSize = m_arrMaterialDescs.size();
	pNxStream->storeDword(iSize);
	for(int i=0; i<iSize; i++)
	{
		APhysXMaterialDesc* pAPXMaterialDesc = m_arrMaterialDescs[i];
		pAPXMaterialDesc->Save(pNxStream);
	}

	// APhysXShapeDescManager save
	m_ShapeDescManager.Save(pNxStream);

	// ActorComponents
	iSize = m_arrActorComponents.size();
	pNxStream->storeDword(iSize);
	for(int i=0; i<iSize; i++)
	{
		m_arrActorComponents[i]->Save(pNxStream);
	}

	return true;

}

bool APhysXRigidBodyObjectDesc::Load(NxStream* pNxStream)
{ 
	if(!APhysXObjectDesc::Load(pNxStream))
		return false;
	
	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{
		int iSize = 0;
		
		Release();

		// actor descs array
		iSize = pNxStream->readDword();
		for(int i=0; i<iSize; i++)
		{
			APhysXActorDesc* pActorDesc = new APhysXActorDesc;
			pActorDesc->Load(pNxStream);
			m_arrActorDescs.push_back(pActorDesc);
		}

		// material descs array
		iSize = pNxStream->readDword();
		for(int i=0; i<iSize; i++)
		{
			APhysXMaterialDesc* pMaterialDesc = new APhysXMaterialDesc;
			pMaterialDesc->Load(pNxStream);
			m_arrMaterialDescs.push_back(pMaterialDesc);
		}

		// APhysXShapeDescManager load
		m_ShapeDescManager.Load(pNxStream);
		
		// ActorComponents
		iSize = pNxStream->readDword();
		m_arrActorComponents.size();
		for(int i=0; i<iSize; i++)
		{
			APhysXActorComponent* pActorComponent = new APhysXActorComponent;
			pActorComponent->Load(pNxStream);
			m_arrActorComponents.push_back(pActorComponent);
		}

		return true;

	}
	else
		return false;


}

/*
void APhysXRigidBodyObjectDesc::UpdateMaterialID2Global(APhysXScene* pPhysXScene)
{
	for(int i = 0; i<m_ShapeDescManager.GetNum(); i++)
	{
		APhysXShapeDesc* pShapeDesc = m_ShapeDescManager.GetPhysXShapeDesc(i);
		pShapeDesc->mMaterialIndex = pPhysXScene->GetMaterialIndex(m_arrMaterialDescs[pShapeDesc->mMaterialIndex]);
	}
}
*/

void APhysXRigidBodyObjectDesc::AddActorComponent(const APhysXActorDesc& physXActorDesc, const APhysXMaterialDesc& materialDesc, const APhysXArray<APhysXShapeDesc* >& physXShapeDescs)
{
	if(physXShapeDescs.size() == 0)
	{
#ifdef APHYSX_DEBUG

		gPhysXLog->Log("Warning: add the actor component which has no shapes!");
#endif
		return;
	}

	APhysXActorDesc* pAPXActorDesc = new APhysXActorDesc;
	*pAPXActorDesc = physXActorDesc;
	m_arrActorDescs.push_back(pAPXActorDesc);
	int aid = m_arrActorDescs.size() - 1;

	APhysXMaterialDesc* pAPXMaterialDesc = new APhysXMaterialDesc;
	*pAPXMaterialDesc = materialDesc;
	m_arrMaterialDescs.push_back(pAPXMaterialDesc);
	int mid = m_arrMaterialDescs.size() - 1;

	APhysXActorComponent* pActorComponent = new APhysXActorComponent;
	pActorComponent->ActorID = aid;
	
	// here, we only consider the material id of the first shape, if it use the default material, then, all shapes use it as well,
	// otherwise, all shapes will use the materialDesc passed in...

	bool bUseDefaultMaterial = physXShapeDescs[0]->IsUseDefaultMaterial();

	for(int i=0;i<physXShapeDescs.size(); i++)
	{
		APhysXShapeDesc* pPhysXShapeDesc = m_ShapeDescManager.CreateAndPushShapeDesc(physXShapeDescs[i]);
		APHYSX_ASSERT(pPhysXShapeDesc);

		// note the material index...
		if(bUseDefaultMaterial)
		{
			pPhysXShapeDesc->UseDefaultMaterial();
		}
		else
		{
			pPhysXShapeDesc->mMaterialIndex = mid;
		}
		pActorComponent->ShapeIDs.push_back(m_ShapeDescManager.GetNum()-1);
	}

	if(bUseDefaultMaterial)
	{
		// pop up the last APhysXMaterialDesc in the array m_arrMaterialDescs
		delete m_arrMaterialDescs.back();
		m_arrMaterialDescs.pop_back();
	}

	// we should update NxShapeDesc here so that to get the correct NxShapeDesc list
	//m_ShapeDescManager.UpdateNxShapeDescs();

	// add a APhysXActorComponent...
	m_arrActorComponents.push_back(pActorComponent);
}
/*
// make the following function inline...

void APhysXRigidBodyObjectDesc::GetNxActorDesc(int id, NxActorDesc& actorDesc)
{
	int aid = m_arrActorComponents[id]->ActorID;
	m_arrActorDescs[aid]->GetNxActorDesc(actorDesc);
}

bool APhysXRigidBodyObjectDesc::HasBody(int id)
{
	int aid = m_arrActorComponents[id]->ActorID;
	return m_arrActorDescs[aid]->mHasBody;
}

void APhysXRigidBodyObjectDesc::GetNxBodyDesc(int id, NxBodyDesc& bodyDesc)
{
	int aid = m_arrActorComponents[id]->ActorID;
	m_arrActorDescs[aid]->GetNxBodyDesc(bodyDesc);
}

*/

void APhysXRigidBodyObjectDesc::GetNxShapeDescs(int id, APhysXScene* pPhysXScene, NxArray<NxShapeDesc* >& shapeDescs, const NxVec3& vScale3D)
{
	APhysXArray<APhysXU32>& sids = m_arrActorComponents[id]->ShapeIDs;
	for(int j=0; j<sids.size(); j++)
	{
		// convert the local material index to global one
		int iCurID = sids[j];
		NxShapeDesc* pNxShapeDesc = m_ShapeDescManager.GetNxShapeDesc(iCurID, vScale3D);
		APhysXShapeDesc* pAPXShapeDesc = m_ShapeDescManager.GetPhysXShapeDesc(iCurID);
		if(pNxShapeDesc && pAPXShapeDesc)
		{
			pNxShapeDesc->materialIndex = (pAPXShapeDesc->IsUseDefaultMaterial())?0:pPhysXScene->GetMaterialIndex(*m_arrMaterialDescs[pAPXShapeDesc->mMaterialIndex]);
			// pNxShapeDesc->group = mCollisionGroup;
			pNxShapeDesc->group = GetCollisionGroup();
			shapeDescs.push_back(pNxShapeDesc);
		}
	}
}

void APhysXRigidBodyObjectDesc::Release()
{
	APhysXObjectDesc::Release();

	for(int i=0; i< m_arrActorDescs.size(); i++)
	{
		delete m_arrActorDescs[i];
	}
	m_arrActorDescs.clear();

	for(int i=0; i< m_arrMaterialDescs.size(); i++)
	{
		delete m_arrMaterialDescs[i];
	}
	m_arrMaterialDescs.clear();

	m_ShapeDescManager.Release();

	for(int i=0; i<m_arrActorComponents.size(); i++)
	{
		delete m_arrActorComponents[i];
	}
	m_arrActorComponents.clear();

	/*
	for(int i=0; i<m_arrNxActorDescs.size(); i++)
	{
	delete m_arrNxActorDescs[i];
	}
	m_arrNxActorDescs.clear();
	*/
}

const APhysXRigidBodyObjectDesc& APhysXRigidBodyObjectDesc::operator= (const APhysXRigidBodyObjectDesc& other)
{
	if(this == &other) return *this;

	// release my old data...
	Release();

	// clone parents' data
	(APhysXObjectDesc& ) *this = (APhysXObjectDesc& )other;

	// clone data of mine
	for(int i=0; i<other.m_arrActorDescs.size(); i++)
		m_arrActorDescs.push_back(other.m_arrActorDescs[i]->Clone());

	for(int i=0; i<other.m_arrMaterialDescs.size(); i++)
		m_arrMaterialDescs.push_back(other.m_arrMaterialDescs[i]->Clone());

	m_ShapeDescManager = other.m_ShapeDescManager;

	for(int i=0; i<other.m_arrActorComponents.size(); i++)
		m_arrActorComponents.push_back(other.m_arrActorComponents[i]->Clone());

	return *this;
}

APhysXU32 APhysXRigidBodyObjectDesc::GetDominanceGroup() const
{
	if(mFlags & APX_OBJFLAG_ONEWAY_COLLISION)
	{
		return APX_RB_DG_LOW;
	}
	//----------------------------------------------------------------
	// Revised by Wenfeng, 2012.3.21...
	// We set attached RB object's dominant group to the top rank, 
	// so that it will not be affected by any other RB objects.
	// And only controled by our AddForce function...
	else if(mFlags & APX_OBJFLAG_CC_CAN_ATTACH)
	{
		return APX_RB_DG_TOP;
	}
	else
		return APX_RB_DG_HIGH;

}

APhysXRigidBodyObjectDesc* APhysXRigidBodyObjectDesc::CreateRBObjectDesc(APhysXObjectType objType)
{
	APhysXObjectDesc* pObjDesc = APhysXObjectDescManager::CreateObjectDesc(objType);

	if(pObjDesc && pObjDesc->IsRigidBody())
	{
		APhysXRigidBodyObjectDesc* pRBObjDesc = (APhysXRigidBodyObjectDesc *)pObjDesc;
		*pRBObjDesc = *this;

		if(GetObjType() != pObjDesc->GetObjType())
		{
			if(pObjDesc->GetObjType() == APX_OBJTYPE_STATIC)
			{
				for(int i=0; i<pRBObjDesc->m_arrActorDescs.size(); i++)
				{
					pRBObjDesc->m_arrActorDescs[i]->mHasBody = false;
				}
			}
			else if(pObjDesc->GetObjType() == APX_OBJTYPE_DYNAMIC)
			{
				for(int i=0; i<pRBObjDesc->m_arrActorDescs.size(); i++)
				{
					pRBObjDesc->m_arrActorDescs[i]->mHasBody = true;
				}

			}
			// add more here...


		}

		return pRBObjDesc;

	}
	else
		return NULL;

}


/*
APhysXObjectDesc* APhysXRigidBodyObjectDesc::Clone() const
{

	// obsolete code: not using operator = function...

	APhysXObjectDesc* pClonedObjectDesc = APhysXObjectDescManager::CreateObjectDesc(GetObjType(), GetName());

	APHYSX_ASSERT(pClonedObjectDesc);
	
	// clone parents' data
	*pClonedObjectDesc = *(APhysXObjectDesc* )this;

	APhysXRigidBodyObjectDesc* pRBClonedObjectDesc = (APhysXRigidBodyObjectDesc*) pClonedObjectDesc;

	// clone data of mine
	for(int i=0; i<m_arrActorDescs.size(); i++)
		pRBClonedObjectDesc->m_arrActorDescs.push_back(m_arrActorDescs[i]->Clone());

	for(int i=0; i<m_arrMaterialDescs.size(); i++)
		pRBClonedObjectDesc->m_arrMaterialDescs.push_back(m_arrMaterialDescs[i]->Clone());
	
	pRBClonedObjectDesc->m_ShapeDescManager = m_ShapeDescManager;

	for(int i=0; i<m_arrActorComponents.size(); i++)
		pRBClonedObjectDesc->m_arrActorComponents.push_back(m_arrActorComponents[i]->Clone());


	return pClonedObjectDesc;

}
*/



/*
void APhysXRigidBodyObjectDesc::UpdateNxActorDescs()
{
	m_arrNxActorDescs.clear();
	for(int i=0; i<m_arrActorComponents.size(); i++)
	{
		NxActorDesc actorDesc;
		
		// actor desc
		int aid = m_arrActorComponents[i]->ActorID;
		m_arrActorDescs[aid].GetNxActorDesc(actorDesc);
		
		// body desc
		if(m_arrActorDescs[aid].mHasBody)
		{
			NxBodyDesc bodyDesc;
			m_arrActorDescs[aid].GetNxBodyDesc(bodyDesc);
			actorDesc.body = &bodyDesc;
		}

		// shapes...
		APhysXArray<APhysXU32>& sids = m_arrActorComponents[i]->ShapeIDs;
		for(int j=0; j<sids.size(); j++)
		{
			actorDesc.shapes.push_back(m_ShapeDescManager.GetNxShapeDesc(j));
		}

		// push the actorDesc to the array
		//m_a
		
	}

}
*/

//-----------------------------------------------------------------------------------------
ImplementVersion(APhysXDummyRBObjectDesc, APhysXRigidBodyObjectDesc)

//-----------------------------------------------------------------------------------------

ImplementVersion(APhysXStaticRBObjectDesc, APhysXRigidBodyObjectDesc)

void APhysXStaticRBObjectDesc::AddActorComponent(const APhysXActorDesc& physXActorDesc, const APhysXMaterialDesc& materialDesc, const APhysXArray<APhysXShapeDesc* >& physXShapeDescs)
{
	APhysXActorDesc apxStaticActorDesc = physXActorDesc;
	apxStaticActorDesc.mHasBody = false;						// disable the body

	APhysXRigidBodyObjectDesc::AddActorComponent(apxStaticActorDesc, materialDesc, physXShapeDescs);
}

//-----------------------------------------------------------------------------------------
ImplementVersion(APhysXDynamicRBObjectDesc, APhysXRigidBodyObjectDesc)

void APhysXDynamicRBObjectDesc::AddActorComponent(const APhysXActorDesc& physXActorDesc, const APhysXMaterialDesc& materialDesc, const APhysXArray<APhysXShapeDesc* >& physXShapeDescs)
{
	APhysXActorDesc apxDynamicActorDesc = physXActorDesc;
	apxDynamicActorDesc.mHasBody = true;						// enable the body

	APhysXRigidBodyObjectDesc::AddActorComponent(apxDynamicActorDesc, materialDesc, physXShapeDescs);

}

//-----------------------------------------------------------------------------------------

ImplementVersion(APhysXTerrainPatchRBObjectDesc, APhysXStaticRBObjectDesc)

void APhysXTerrainPatchRBObjectDesc::GetNxShapeDescs(int id, APhysXScene* pPhysXScene, NxArray<NxShapeDesc* >& shapeDescs, const NxVec3& vScale3D)
{
	APhysXStaticRBObjectDesc::GetNxShapeDescs(id, pPhysXScene, shapeDescs, vScale3D);

	// as for TerrainPatch object, the shapeDescs's size should always be 1!
	for(int i=0; i<shapeDescs.size(); i++)
	{
		NxHeightFieldShapeDesc* pHFShapeDesc = (NxHeightFieldShapeDesc*) shapeDescs[i];
		if(pHFShapeDesc->materialIndex == 0)
		{
			// this means that the height field just use the default material of the scene
			// and in this case, we do nothing...
			return;
		}

		int iSampleSize = pHFShapeDesc->heightField->getNbColumns() * pHFShapeDesc->heightField->getNbRows();
		 
		NxU8* currentByte = (NxU8*)pHFShapeDesc->heightField->getCells();
		for(int j = 0; j < iSampleSize; j++)
		{
			//??? oh, it's in fact a hacking way to changing the heightfield's sample data
			// since heightField->getCells() return a const pointer but I force to convert it to a non-const one
			// and this may be incur some crash??? So, at last, I give up and force that 
			// terrain must use the default material of the scene. Mar. 20, 2008
		
			NxHeightFieldSample* currentSample = (NxHeightFieldSample* )currentByte;
			currentSample->materialIndex0 = pHFShapeDesc->materialIndex;
			currentSample->materialIndex1 = pHFShapeDesc->materialIndex;

			currentByte += sizeof(NxU32);
		}
	}
}

void APhysXTerrainPatchRBObjectDesc::AddActorComponent(const APhysXActorDesc& physXActorDesc, const APhysXMaterialDesc& materialDesc, const APhysXArray<APhysXShapeDesc* >& physXShapeDescs)
{

	// force to use the default material and ignore the parameter materialDesc;
	APHYSX_ASSERT(physXShapeDescs.size() == 1);

	for(int i=0;i<physXShapeDescs.size(); i++)
	{
		physXShapeDescs[i]->UseDefaultMaterial();
	}

	// then, just call father's function
	APhysXRigidBodyObjectDesc::AddActorComponent(physXActorDesc, materialDesc, physXShapeDescs);
}

//-----------------------------------------------------------------------------------------

ImplementVersion(APhysXSkeletonRBObjectDesc, APhysXDynamicRBObjectDesc)


