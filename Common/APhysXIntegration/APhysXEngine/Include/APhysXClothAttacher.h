/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCLOTHATTACHER_H_
#define _APHYSXCLOTHATTACHER_H_



class APHYSX_DLL_EXPORT APhysXClothAttacher: public APhysXAttacher
{

public:

	APhysXClothObject* GetClothObject()
	{
		return GetClothObject(APX_ATTACHER_ATTACHED_CLOTH_NAME);
	}
	
	APhysXClothObject* GetClothObject(const char* szClothName)
	{
		APhysXObject* pObject = GetPhysXObjectComponent(szClothName);
		APHYSX_ASSERT(pObject && pObject->IsCloth());

		return (APhysXClothObject*)pObject;
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
		SetAttacherPose(APX_ATTACHER_ATTACHER_NAME, globalPose);
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