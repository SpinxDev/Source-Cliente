
#include "APhysXStdAfx.h"

/*

//----------------------------------------------------------------------
// Note: by wenfeng, Feb. 23, 2009
// the following code is refactored and placed in its' base class...

bool APhysXFluidEmitterAttacher::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	if(!APhysXCompoundObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
		return false;	


	// make all attacher objects kinematic...

	EnableAttacherKinematic();

	return true;
}

void APhysXFluidEmitterAttacher::EnableAttacherKinematic(bool bEnable)
{
	for(int i=0; i<m_arrObjectComponents.size(); i++)
	{
		if(m_arrObjectComponents[i]->mPhysXObject->IsRigidBody())
		{
			APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject*)m_arrObjectComponents[i]->mPhysXObject;
			pRBObj->EnableKinematic(bEnable);
		}

	}
}

*/
