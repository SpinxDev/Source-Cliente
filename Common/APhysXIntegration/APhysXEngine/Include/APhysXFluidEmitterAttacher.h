
/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXFLUIDEMITTERATTACHER_H_
#define _APHYSXFLUIDEMITTERATTACHER_H_



class APHYSX_DLL_EXPORT APhysXFluidEmitterAttacher: public APhysXAttacher
{

public:

	APhysXFluidObject* GetFluidObject()
	{
		APhysXObject* pObject = GetPhysXObjectComponent(APX_ATTACHER_ATTACHED_FLUID_NAME);
		APHYSX_ASSERT(pObject && pObject->IsFluid());

		return (APhysXFluidObject*)pObject;
	}

/*

	//----------------------------------------------------------------------
	// Note: by wenfeng, Feb. 23, 2009
	// the following code is refactored and placed in its' base class...

	APhysXRigidBodyObject* GetAttacherObject()
	{
		return GetAttacherObject(APX_ATTACHER_ATTACHER_NAME);
	}

	APhysXRigidBodyObject* GetAttacherObject(const char* szAttacherName)
	{
		APhysXObject* pObject = GetPhysXObjectComponent(szAttacherName);
		APHYSX_ASSERT(pObject && pObject->IsRigidBody());

		return (APhysXRigidBodyObject*)pObject;
	}

	void SetAttacherPose(const NxMat34& globalPose)
	{
		SetAttacherPose(APX_FLUIDEMITTERATTACHER_ATTACHER_NAME, globalPose);
	};

	void SetAttacherPose(const char* szAttacherName, const NxMat34& globalPose)
	{
		APhysXObject* pObject = GetPhysXObjectComponent(szAttacherName);
		APHYSX_ASSERT(pObject && pObject->IsRigidBody());

		if(pObject)
			pObject->SetPose(globalPose);
	}

	void EnableAttacherKinematic(bool bEnable = true);

protected:

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);

*/

};

#endif