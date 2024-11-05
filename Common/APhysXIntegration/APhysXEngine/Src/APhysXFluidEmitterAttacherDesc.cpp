
#include "APhysXStdAfx.h"

ImplementVersion(APhysXFluidEmitterAttacherDesc, APhysXAttacherDesc)


bool APhysXFluidEmitterAttacherDesc::AddMultiAttacherComponent(const APhysXObjectDesc* pRBObjectDesc, const APhysXFluidEmitterParameters& fluidEmitterParameters, const char* szAttacherName)
{
	/*
	APHYSX_ASSERT(pRBObjectDesc->IsRigidBody() && szAttacherName);

	if(pRBObjectDesc->IsRigidBody() && szAttacherName)
	{
		AddObjectComponent(pRBObjectDesc, szAttacherName);

		// set the attacher object's collision group to APX_COLLISION_GROUP_CLOTH_ATTACHER
		APhysXObjectComponentDesc* pComponentDesc = m_arrObjectComponentDescs.back();
		pComponentDesc->mPhysXObjectDesc->mCollisionGroup = APX_COLLISION_GROUP_ATTACHER;

		// then, add the inner connector info...
		APhysXFluidEmitterAttachRBConnectorDesc apxFEARBConnectorDesc;
		apxFEARBConnectorDesc.mPhysXObject1Name = APX_ATTACHER_ATTACHED_FLUID_NAME;
		apxFEARBConnectorDesc.mPhysXObject2Name = szAttacherName;
		apxFEARBConnectorDesc.mFluidEmitterParameters = fluidEmitterParameters;
		//??? make it be NULL or transfered via parameters?
		// apxFEARBConnectorDesc.mAttachedNxActorName = NULL;		

		AddInnerConnector(apxFEARBConnectorDesc);
	}
	*/

	if(APhysXAttacherDesc::AddMultiAttacherComponent(pRBObjectDesc, szAttacherName))
	{
		// then, add the inner connector info...
		APhysXFluidEmitterAttachRBConnectorDesc apxFEARBConnectorDesc;
		apxFEARBConnectorDesc.mPhysXObject1Name = APX_ATTACHER_ATTACHED_FLUID_NAME;
		apxFEARBConnectorDesc.mPhysXObject2Name = szAttacherName;
		apxFEARBConnectorDesc.mFluidEmitterParameters = fluidEmitterParameters;
		//??? make it be NULL or transfered via parameters?
		// apxFEARBConnectorDesc.mAttachedNxActorName = NULL;		

		AddInnerConnector(apxFEARBConnectorDesc);

		return true;
	}
	else
		return false;

}

bool APhysXFluidEmitterAttacherDesc::IsValid()
{
	if(m_arrObjectComponentDescs.size() < 2) return false;

	bool bHasAttacher = false;
	bool bHasFluid = false;

	for(int i=0; i<m_arrObjectComponentDescs.size(); i++)
	{
		APhysXObjectComponentDesc* pCurComponentDesc = m_arrObjectComponentDescs[i];
		if(bHasFluid && bHasAttacher)
			break;

		if(pCurComponentDesc->mPhysXObjectDesc->IsFluid() && pCurComponentDesc->mLocalName)
		{
			bHasFluid = true;
		}

		if(pCurComponentDesc->mPhysXObjectDesc->IsRigidBody() && pCurComponentDesc->mLocalName)
		{
			bHasAttacher = true;
		}
	}

	return (bHasFluid && bHasAttacher);
}

/*
APhysXObjectDesc* APhysXFluidEmitterAttacherDesc::Clone() const
{
	CloneThis(APhysXFluidEmitterAttacherDesc, APhysXObjectDesc)
}
*/


APhysXFluidParameters* APhysXFluidEmitterAttacherDesc::GetFluidParameters()
{
	for(int i=0; i<m_arrObjectComponentDescs.size(); i++)	
	{
		APhysXObjectDesc* pObjDesc = m_arrObjectComponentDescs[i]->mPhysXObjectDesc;

		if(pObjDesc->IsFluid())
		{
			APhysXFluidObjectDesc* pFluidObjDesc = (APhysXFluidObjectDesc*) pObjDesc;

			return &pFluidObjDesc->mFluidParameters;
		}
	}

	return NULL;
}

APhysXFluidEmitterParameters* APhysXFluidEmitterAttacherDesc::GetFluidEmitterParameters(const char* szAttacherName)
{
	for(int i=0; i<mInnerConnectorDescManager.GetNum(); i++)	
	{
		APhysXObjectConnectorDesc* pConnectorDesc = mInnerConnectorDescManager.GetPhysXConnectorDesc(i);
		
		//---------------------------------------------------------------
		// make sure that parameters passed to strcmp(...) not NULL...
		//---------------------------------------------------------------
		if(	pConnectorDesc->GetConnectorType() == APX_CNTTYPE_FLUIDEMITTER_ATTACH_RB &&
			pConnectorDesc->mPhysXObject2Name && szAttacherName && strcmp(pConnectorDesc->mPhysXObject2Name, szAttacherName) == 0 )
		{
			APhysXFluidEmitterAttachRBConnectorDesc* pFEConnectorDesc = (APhysXFluidEmitterAttachRBConnectorDesc *)pConnectorDesc;

			return &pFEConnectorDesc->mFluidEmitterParameters;
		}

	}

	return NULL;


}
