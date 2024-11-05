
#include "APhysXStdAfx.h"

bool APhysXJoint::CreateConnector(const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc)
{
	NxJointDesc* pNxJointDesc = NULL;
	switch(apxObjConnectorInstanceDesc.mPhysXConnectorDesc->GetConnectorType())
	{
	case APX_CNTTYPE_CYLINDRICAL_JOINT:

		pNxJointDesc = new NxCylindricalJointDesc;
		break;
	
	case APX_CNTTYPE_D6_JOINT:
	
		pNxJointDesc = new NxD6JointDesc;

		break;

	case APX_CNTTYPE_DISTANCE_JOINT:

		pNxJointDesc = new NxDistanceJointDesc;

	    break;

	case APX_CNTTYPE_FIXED_JOINT:

		pNxJointDesc = new NxFixedJointDesc;

		break;
	
	case APX_CNTTYPE_POINTINPLANE_JOINT:

		pNxJointDesc = new NxPointInPlaneJointDesc;

		break;

	case APX_CNTTYPE_POINTONLINE_JOINT:

		pNxJointDesc = new NxPointInPlaneJointDesc;

		break;

	case APX_CNTTYPE_PRISMATIC_JOINT:

		pNxJointDesc = new NxPrismaticJointDesc;

		break;

	case APX_CNTTYPE_PULLEY_JOINT:

		// not supporting at present...
		// pNxJointDesc = NxPulleyJointDesc;

		break;

	case APX_CNTTYPE_REVOLUTE_JOINT:

		pNxJointDesc = new NxRevoluteJointDesc;

		break;

	case APX_CNTTYPE_SPHERICAL_JOINT:

		pNxJointDesc = new NxSphericalJointDesc;

		break;

	default:
		
		APHYSX_ASSERT(0);
	
		break;
	}

	if(!pNxJointDesc) return false;

	APhysXJointDesc& apxJointDesc = (APhysXJointDesc&)(*apxObjConnectorInstanceDesc.mPhysXConnectorDesc);

	//------------------------------------------------------------
	// some testing code to test the breakage of joints and commented, wenfeng, April 16, 2009
#if 0
	
	apxJointDesc.mMaxForce = 60;
	apxJointDesc.mMaxTorque = 60;

#endif

	//------------------------------------------------------------

	// get the NxActor_1 and NxActor_2
	NxActor * pNxActor1 = NULL, * pNxActor2 = NULL;
	APhysXRigidBodyObject* pRBObject = NULL;
	if(m_pAPhysXObject1)
	{
		APHYSX_ASSERT(m_pAPhysXObject1->IsRigidBody());

		if(apxJointDesc.IsNxActorsSet())
		{
			pNxActor1 = apxJointDesc.mNxActor1;
		}
		else
		{
			pRBObject = (APhysXRigidBodyObject*)m_pAPhysXObject1;
			pNxActor1 = pRBObject->GetNxActor(apxJointDesc.mNxActor1Name);

/*
			// if the name is wrong or not set, try to use the first NxActor
			if(!pNxActor1 && pRBObject->GetNxActorNum())
			{
				pNxActor1 = pRBObject->GetNxActor(0);
			}
*/

		}
	}

	if(m_pAPhysXObject2)
	{
		APHYSX_ASSERT(m_pAPhysXObject2->IsRigidBody());
		
		if(apxJointDesc.IsNxActorsSet())
		{
			pNxActor2 = apxJointDesc.mNxActor2;
		}
		else
		{
			pRBObject = (APhysXRigidBodyObject*)m_pAPhysXObject2;
			pNxActor2 = pRBObject->GetNxActor(apxJointDesc.mNxActor2Name);

/*
			// if the name is wrong or not set, try to use the first NxActor
			if(!pNxActor2 && pRBObject->GetNxActorNum())
			{
				pNxActor2 = pRBObject->GetNxActor(0);
			}
*/

		}
	}

	// make sure that we have at least one non-null actor
	if(pNxActor1 || pNxActor2)
	{
		pNxJointDesc->actor[0] = pNxActor1;
		pNxJointDesc->actor[1] = pNxActor2;

		apxJointDesc.GetNxJointDesc(*pNxJointDesc, apxObjConnectorInstanceDesc.mGlobalPose, apxObjConnectorInstanceDesc.mScale3D);

		m_pAPhysXScene->LockWritingScene();
		m_pNxJoint = m_pAPhysXScene->GetNxScene()->createJoint(*pNxJointDesc);
		m_pAPhysXScene->UnlockWritingScene();

		delete pNxJointDesc;

		if(m_pNxJoint)
			return true;
		else
		{
			gPhysXLog->Log("Warning: Failed on creating NxJoint: create a NULL NxJoint.");
			return false;
		}

	}
	else
		return false;

}

void APhysXJoint::ReleaseConnector()
{
	if(m_pNxJoint)
	{
		m_pAPhysXScene->LockWritingScene();
		m_pAPhysXScene->GetNxScene()->releaseJoint(*m_pNxJoint);
		m_pAPhysXScene->UnlockWritingScene();

		m_pNxJoint = NULL;
	}

}
