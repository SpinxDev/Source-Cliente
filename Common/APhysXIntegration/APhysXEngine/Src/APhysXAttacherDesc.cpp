
#include "APhysXStdAfx.h"


ImplementVersion(APhysXAttacherDesc, APhysXCompoundObjectDesc)


bool APhysXAttacherDesc::AddMultiAttacherComponent(const APhysXObjectDesc* pRBObjectDesc, const char* szAttacherName, const NxMat34& matLocalPose)
{
	APHYSX_ASSERT(pRBObjectDesc->IsRigidBody() && szAttacherName);

	if(pRBObjectDesc->IsRigidBody() && szAttacherName)
	{
		AddObjectComponent(pRBObjectDesc, szAttacherName, matLocalPose);

		// set the attacher object's collision group to APX_COLLISION_GROUP_ATTACHER
		APhysXObjectComponentDesc* pComponentDesc = m_arrObjectComponentDescs.back();
		pComponentDesc->mPhysXObjectDesc->mCollisionGroup = APX_COLLISION_GROUP_ATTACHER;

		return true;
	}
	else
		return false;

}
