
#include "APhysXStdAfx.h"

/*

//----------------------------------------------------------------------
// Note: by wenfeng, Feb. 23, 2009
// the following code is refactored and placed in its' base class...

bool APhysXClothAttacher::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	if(!APhysXCompoundObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
		return false;	


	// by default, make all attacher objects kinematic...
	EnableAttacherKinematic();
	return true;
}

void APhysXClothAttacher::EnableAttacherKinematic(bool bEnable)
{
	for(int i=0; i<m_arrObjectComponents.size(); i++)
	{
		if(m_arrObjectComponents[i]->mPhysXObject->IsRigidBody())
		{
			APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject*)m_arrObjectComponents[i]->mPhysXObject;
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

*/
