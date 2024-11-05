
#include "APhysXStdAfx.h"


bool APhysXAttacher::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{

	//-------------------------------------------------------------------
	// we will call this function to revise the desc object just 
	// before creating the PhysX stuff...
	APhysXAttacherDesc* pAPhysXAttacherDesc = (APhysXAttacherDesc*)apxObjInstanceDesc.mPhysXObjectDesc;
	pAPhysXAttacherDesc->CompleteInnerConnectorInfo();

	if(!APhysXCompoundObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
		return false;	


	// by default, make all attacher objects kinematic...
	EnableAttacherKinematic();
	return true;
}

void APhysXAttacher::EnableAttacherKinematic(bool bEnable)
{
	for(int i=0; i<m_arrObjectComponents.size(); i++)
	{
		if(m_arrObjectComponents[i]->mPhysXObject->IsRigidBody())
		{
			APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject*)m_arrObjectComponents[i]->mPhysXObject;
			
			// we should test whether pRBObj is the attacher...
			if(pRBObj->GetCollisionGroup() == APX_COLLISION_GROUP_ATTACHER || pRBObj->GetCollisionGroup() == APX_COLLISION_GROUP_CLOTH_ATTACHER)
			{
				pRBObj->EnableKinematic(bEnable);

				//-------------------------------------------------------------------
				//??? fix me...
				// if we make the attacher dynamic, we disable its gravity... 
				if(!bEnable)
				{
					pRBObj->EnableGravity(false);
				}
				//-------------------------------------------------------------------

			}
		}

	}
}

void APhysXAttacher::EnableAttacherKinematic(const char* szAttacherName, bool bEnable)
{
	APhysXRigidBodyObject* pAttacher = GetAttacherObject(szAttacherName);
	if(pAttacher)
	{
		pAttacher->EnableKinematic(bEnable);
		//-------------------------------------------------------------------
		//??? fix me...
		// if we make the attacher dynamic, we disable its gravity... 
		if(!bEnable)
		{
			pAttacher->EnableGravity(false);
		}
		//-------------------------------------------------------------------

	}

}