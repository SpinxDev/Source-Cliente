
/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008


----------------------------------------------------------------------*/

#ifndef _APHYSXRBATTACHER_H_
#define _APHYSXRBATTACHER_H_


class APHYSX_DLL_EXPORT APhysXRBAttacher: public APhysXAttacher
{

public:

	APhysXRigidBodyObject* GetRBObject()
	{
		return GetRBObject(APX_ATTACHER_ATTACHED_RB_NAME);
	}

	APhysXRigidBodyObject* GetRBObject(const char* szRBName)
	{
		APhysXObject* pObject = GetPhysXObjectComponent(szRBName);
		APHYSX_ASSERT(pObject && pObject->IsRigidBody());

		return (APhysXRigidBodyObject*)pObject;
	}

};


#endif