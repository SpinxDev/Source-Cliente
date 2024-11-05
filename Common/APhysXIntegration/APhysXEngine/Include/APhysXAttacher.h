/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

	
		The base class of all type of attachers, such as cloth attacher, 
	fluid emitter attacher and rigid-body attacher.
----------------------------------------------------------------------*/


#ifndef _APHYSXATTACHER_H_
#define _APHYSXATTACHER_H_


class APHYSX_DLL_EXPORT APhysXAttacher: public APhysXCompoundObject
{

public:

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
		SetAttacherPose(APX_ATTACHER_ATTACHER_NAME, globalPose);
	};

	void SetAttacherPose(const char* szAttacherName, const NxMat34& globalPose)
	{
		APhysXObject* pObject = GetPhysXObjectComponent(szAttacherName);
		APHYSX_ASSERT(pObject && pObject->IsRigidBody());

		if(pObject)
			pObject->SetPose(globalPose);
	}

	// enable each attacher kinematic or not...
	void EnableAttacherKinematic(bool bEnable = true);
	// enable the specified attacher kinematic or not...
	void EnableAttacherKinematic(const char* szAttacherName, bool bEnable = true);

protected:

	virtual bool OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc);


};

#endif
