
#include "APhysXStdAfx.h"

ImplementVersion(APhysXRBAttacherDesc, APhysXAttacherDesc)


void APhysXRBAttacherDesc::AddAttachInfo(
				   const char* szAttachedActorName, 
				   APhysXRBAttachType apxRBAttachType, 
				   const NxVec3& vAnchorPos,
				   const char* szAttacherName ,
				   const char* szAttachedRBObjName
				   )
{

#if 0

	//----------------------------------------------------------------------------
	// note by wenfeng, April. 16, 2009
	// use Fix Joint to implement the RB attach type: APX_RB_ATTYPE_FIX
	// it seems to be a little complex...

	if(szAttachedActorName)
	{
		// create D6 joint...
		APhysXJointDesc* pAttachJointDesc = NULL;
		APhysXD6JointDesc attachD6JointDesc;
		APhysXFixedJointDesc attachFixedJointDesc;

		
		if(apxRBAttachType == APX_RB_ATTYPE_FIX)
		{
			
			attachFixedJointDesc.mGlobalAxis.set(0.f, 1.f, 0.f);
			// attachFixedJointDesc.mGlobalAnchor = vAnchorPos;
			pAttachJointDesc = &attachFixedJointDesc;
			
			/*
			attachJointDesc.mTwistMotion = NX_D6JOINT_MOTION_LOCKED;
			attachJointDesc.mSwing1Motion = NX_D6JOINT_MOTION_LOCKED;
			attachJointDesc.mSwing2Motion = NX_D6JOINT_MOTION_LOCKED;
			attachJointDesc.mGlobalAxis.set(0.f, 1.f, 0.f);
			*/

		}
 		else
		{
			attachD6JointDesc.mGlobalAnchor = vAnchorPos;
			pAttachJointDesc = & attachD6JointDesc;
			
		}

		pAttachJointDesc->mPhysXObject1Name = szAttachedRBObjName;
		pAttachJointDesc->mNxActor1Name = szAttachedActorName;

		pAttachJointDesc->mPhysXObject2Name = szAttacherName;
		pAttachJointDesc->mNxActor2Name = NULL;

		/*
		attachJointDesc.mPhysXObject1Name = szAttachedRBObjName;
		attachJointDesc.mNxActor1Name = szAttachedActorName;

		attachJointDesc.mPhysXObject2Name = szAttacherName;
		attachJointDesc.mNxActor2Name = NULL;

		*/

		// add the joint as the inner connector...
		AddInnerConnector(*pAttachJointDesc);

	}

#else

	//----------------------------------------------------------------------------
	// note by wenfeng, April. 16, 2009
	// both attach types use D6 joint to implement, it is simple but reliable?
	// D6 joint with twist and swing locked sometimes still jitters...

	if(szAttachedActorName)
	{
		// create D6 joint...
		APhysXD6JointDesc attachJointDesc;
		if(apxRBAttachType == APX_RB_ATTYPE_FIX)
		{

			attachJointDesc.mTwistMotion = NX_D6JOINT_MOTION_LOCKED;
			attachJointDesc.mSwing1Motion = NX_D6JOINT_MOTION_LOCKED;
			attachJointDesc.mSwing2Motion = NX_D6JOINT_MOTION_LOCKED;
			attachJointDesc.mGlobalAxis.set(0.f, 1.f, 0.f);
			attachJointDesc.mGlobalAnchor = vAnchorPos;
			attachJointDesc.mProjectionMode = NX_JPM_POINT_MINDIST;
			attachJointDesc.mProjectionDistance = 0.01f;

		}
		else
			attachJointDesc.mGlobalAnchor = vAnchorPos;

		attachJointDesc.mPhysXObject1Name = szAttachedRBObjName;
		attachJointDesc.mNxActor1Name = szAttachedActorName;

		attachJointDesc.mPhysXObject2Name = szAttacherName;
		attachJointDesc.mNxActor2Name = NULL;

		// add the joint as the inner connector...
		AddInnerConnector(attachJointDesc);

	}

#endif

}


