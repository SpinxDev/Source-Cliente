
#include "APhysXStdAfx.h"

bool APhysXCompoundObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	if(!APhysXObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
		return false;

	APHYSX_ASSERT(apxObjInstanceDesc.mPhysXObjectDesc->IsCompound());
	APhysXCompoundObjectDesc& compoundObjDesc = *(APhysXCompoundObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;

	// call to update the names of its component object...
	compoundObjDesc.CompleteGlobalObjectComponentName();

	bool bFailedOnCreatePhysXStuff = false;

	for(int i=0; i<compoundObjDesc.GetObjectComponentDescNum(); i++)
	{
		APhysXObjectInstanceDesc componentObjInstanceDesc;
		componentObjInstanceDesc.mPhysXObjectDesc = compoundObjDesc.GetObjectComponentDesc(i)->mPhysXObjectDesc;
		componentObjInstanceDesc.mScale3D = apxObjInstanceDesc.mScale3D;

		//---------- compute the global pose for component object instance -----------
		// first, scale the offset
		NxMat34 matComponentObjPose = compoundObjDesc.GetObjectComponentDesc(i)->mLocalPose;
		matComponentObjPose.t.arrayMultiply(matComponentObjPose.t, apxObjInstanceDesc.mScale3D);
		// then, transform to the global pose
		componentObjInstanceDesc.mGlobalPose = apxObjInstanceDesc.mGlobalPose * matComponentObjPose;
		//---------- compute the global pose for component object instance -----------

		// note here: use the static function APhysXScene::CreatePhysXObject(...) so you should remember to 
		// delete the pointer ...
		APhysXObject* pObject = APhysXScene::CreatePhysXObject(componentObjInstanceDesc.mPhysXObjectDesc->GetObjType());
		if(pObject)
		{
			// set m_pParentObject to this before pObject's OnCreate() function 
			// in order to identify that pObject is a child object
			pObject->m_pParentObject = this;

			if(pObject->OnCreate(m_pAPhysXScene, componentObjInstanceDesc))
			{

				APhysXObjectComponent* pObjectComponent = new APhysXObjectComponent;
				
				pObjectComponent->mPhysXObject = pObject;
				pObjectComponent->mLocalName = compoundObjDesc.GetObjectComponentDesc(i)->mLocalName;

				m_arrObjectComponents.push_back(pObjectComponent);
			}
			else
			{
				gPhysXLog->Log("Warning: Failed on creating Component Object: OnCreate(...) return false.");
				delete pObject;
				
				bFailedOnCreatePhysXStuff = true;
				break;
			}
		}
		else
		{
			
			gPhysXLog->Log("Warning: Failed on create Component Object: new APhysXObject failed or undefined APhysXObject Type.");
			
			bFailedOnCreatePhysXStuff = true;
			break;
		}
		
	}

	if(bFailedOnCreatePhysXStuff)
	{
		OnReleaseBasicPhysXStuff();
		return false;
	}
	else
		return true;

}


void APhysXCompoundObject::OnReleaseBasicPhysXStuff()
{
	for(int i = 0; i<m_arrObjectComponents.size(); i++)
	{
		m_arrObjectComponents[i]->mPhysXObject->OnRelease();
		delete m_arrObjectComponents[i]->mPhysXObject;
		delete m_arrObjectComponents[i];
	}

	m_arrObjectComponents.clear();

	// not to forget...
	APhysXObject::OnReleaseBasicPhysXStuff();
}

APhysXObject* APhysXCompoundObject::GetPhysXObjectComponent(const char* szName)
{
	if(szName == NULL) return NULL;

	for(int i=0; i<m_arrObjectComponents.size(); i++)
	{
		//---------------------------------------------------------------
		// make sure that parameters passed to strcmp(...) not NULL...
		//---------------------------------------------------------------
		if(m_arrObjectComponents[i]->mLocalName && strcmp(szName, m_arrObjectComponents[i]->mLocalName) == 0)
		{
			return m_arrObjectComponents[i]->mPhysXObject;
		}
	}

	return NULL;
}

void APhysXCompoundObject::SetPose(const NxMat34& globalPose)
{
	//  do nothing
}

NxMat34 APhysXCompoundObject::GetPose() const
{
	// return the id NxMat34
	NxMat34 matPose;
	return matPose;
}

void APhysXCompoundObject::GetAABB(NxBounds3& aabb) const
{
	aabb.setEmpty();

	if(m_pAPhysXScene && m_arrObjectComponents.size())
	{
		for(int i = 0; i<m_arrObjectComponents.size(); i++)
		{
			NxBounds3 aabbComponent;
			m_arrObjectComponents[i]->mPhysXObject->GetAABB(aabbComponent);

			aabb.combine(aabbComponent);
		}

	}
}

void APhysXCompoundObject::SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef)
{
	APhysXObject::SetCollisionChannel(apxCollisionChannel, bChangeChannelRef);

	if(m_pAPhysXScene && m_arrObjectComponents.size())
	{
		// NxGroupsMask groupMask = m_pAPhysXScene->GetCollisionChannelManager()->GetChannelGroupMask(apxCollisionChannel);

		for(int i = 0; i<m_arrObjectComponents.size(); i++)
		{
			m_arrObjectComponents[i]->mPhysXObject->SetCollisionChannel(apxCollisionChannel, false);
		}

	}
}

void APhysXCompoundObject::SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup)
{
	APhysXObject::SetCollisionGroup(apxCollisionGroup);

	if(m_pAPhysXScene && m_arrObjectComponents.size())
	{

		for(int i = 0; i<m_arrObjectComponents.size(); i++)
		{
			m_arrObjectComponents[i]->mPhysXObject->SetCollisionGroup(apxCollisionGroup);
		}

	}

}
void APhysXCompoundObject::EnableCollision(bool bEnable /* = true */)
{
	APhysXObject::EnableCollision(bEnable);

	if(m_pAPhysXScene && m_arrObjectComponents.size())
	{
		for(int i = 0; i<m_arrObjectComponents.size(); i++)
		{
			m_arrObjectComponents[i]->mPhysXObject->EnableCollision(bEnable);
		}
		
	}

}

void APhysXCompoundObject::Delete()
{

	APhysXObject::Delete();

	if(m_pAPhysXScene && m_arrObjectComponents.size())
	{
		for(int i = 0; i<m_arrObjectComponents.size(); i++)
		{
			m_arrObjectComponents[i]->mPhysXObject->Delete();
		}

	}

}

void APhysXCompoundObject::EnableDebugRender(bool bEnable)
{
	APhysXObject::EnableDebugRender(bEnable);

	if(m_pAPhysXScene && m_arrObjectComponents.size())
	{
		for(int i = 0; i<m_arrObjectComponents.size(); i++)
		{
			m_arrObjectComponents[i]->mPhysXObject->EnableDebugRender(bEnable);
		}

	}
}