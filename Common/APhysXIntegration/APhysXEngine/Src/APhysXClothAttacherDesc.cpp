
#include "APhysXStdAfx.h"


ImplementVersion(APhysXClothAttacherDesc, APhysXAttacherDesc)

bool APhysXClothAttacherDesc::AddMultiAttacherComponent(const APhysXObjectDesc* pRBObjectDesc, const char* szAttacherName, const NxMat34& matLocalPose)
{
	if(APhysXAttacherDesc::AddMultiAttacherComponent(pRBObjectDesc, szAttacherName, matLocalPose))
	{
		// set the attacher object's collision group to APX_COLLISION_GROUP_ATTACHER
		APhysXObjectComponentDesc* pComponentDesc = m_arrObjectComponentDescs.back();

		// note here: the collision group is different from the base class...
		pComponentDesc->mPhysXObjectDesc->mCollisionGroup = APX_COLLISION_GROUP_CLOTH_ATTACHER;

		return true;
	}
	else
		return false;
}

/*
APhysXObjectDesc* APhysXClothAttacherDesc::Clone() const
{
	CloneThis(APhysXClothAttacherDesc, APhysXObjectDesc)
}
*/

bool APhysXClothAttacherDesc::IsValid()
{
	if(m_arrObjectComponentDescs.size() < 2) return false;

	bool bHasAttacher = false;
	bool bHasCloth = false;

	for(int i=0; i<m_arrObjectComponentDescs.size(); i++)
	{
		APhysXObjectComponentDesc* pCurComponentDesc = m_arrObjectComponentDescs[i];
		if(bHasCloth && bHasAttacher)
			break;

		if(pCurComponentDesc->mPhysXObjectDesc->IsCloth() && pCurComponentDesc->mLocalName)
		{
			bHasCloth = true;
		}
		
		if(pCurComponentDesc->mPhysXObjectDesc->IsRigidBody() && pCurComponentDesc->mLocalName)
		{
			bHasAttacher = true;
		}
	}

	return (bHasCloth && bHasAttacher);
}

//-------------------------------------------------------------------------------------
// note here:
//     It's in fact a little trick here to create the correct inner attaching information.
// Since in APhysXClothAttachRBConnectorDesc we use attachToCollidingShapes(...) by default,
// we do not specify the exact attacher object which the cloth attaches to, while, we just 
// set the another object of APhysXClothAttachRBConnectorDesc to any attacher object.
void APhysXClothAttacherDesc::CompleteInnerConnectorInfo()
{
	if(IsValid() && mInnerConnectorDescManager.GetNum() == 0 )
	{
		// find the first rigid body object which is a attacher
		APhysXObjectComponentDesc* pRBComponentDesc = NULL;
		for(int i=0; i<m_arrObjectComponentDescs.size(); i++)
		{
			pRBComponentDesc = m_arrObjectComponentDescs[i];
			if(pRBComponentDesc->mPhysXObjectDesc->IsRigidBody())
			{
				
				// attach all cloth object to this attacher...
				for(int i=0; i<m_arrObjectComponentDescs.size(); i++)
				{
					APhysXObjectComponentDesc* pCurComponentDesc= m_arrObjectComponentDescs[i];
					if(pCurComponentDesc->mPhysXObjectDesc->IsCloth())
					{
						APhysXClothAttachRBConnectorDesc clothAttachRBDesc;
						
						// cloth
						clothAttachRBDesc.mPhysXObject1Name = pCurComponentDesc->mLocalName;

						// attacher
						clothAttachRBDesc.mPhysXObject2Name = pRBComponentDesc->mLocalName;


						AddInnerConnector(clothAttachRBDesc);
					}
				}

			}
		}


	}
}

APhysXClothParameters* APhysXClothAttacherDesc::GetClothParameters(const char* szClothName)
{
	for(int i=0; i<m_arrObjectComponentDescs.size(); i++)	
	{
		APhysXObjectComponentDesc* pCurComponentDesc = m_arrObjectComponentDescs[i];
		APhysXObjectDesc* pObjDesc = pCurComponentDesc->mPhysXObjectDesc;

		//---------------------------------------------------------------
		// make sure that parameters passed to strcmp(...) not NULL...
		//---------------------------------------------------------------

		if(pObjDesc->IsCloth() && pCurComponentDesc->mLocalName && szClothName && strcmp(pCurComponentDesc->mLocalName, szClothName) == 0)
		{
			APhysXClothObjectDesc* pClothObjDesc = (APhysXClothObjectDesc*) pObjDesc;

			return &pClothObjDesc->mClothParameters;
		}
	}

	return NULL;

}
