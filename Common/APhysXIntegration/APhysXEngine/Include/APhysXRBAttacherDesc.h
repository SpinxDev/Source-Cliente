/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008


----------------------------------------------------------------------*/


#ifndef _APHYSXRBATTACHERDESC_H_
#define _APHYSXRBATTACHERDESC_H_

enum APhysXRBAttachType
{
	APX_RB_ATTYPE_FIX,						// the attached actor has the fix connection with the attacher
	APX_RB_ATTYPE_ROTATION_FREE,			// the attached actor has the D3-free connection with the attacher

};

/*

// it seems no need to add another struct to save the attach info...

class APHYSX_DLL_EXPORT APhysXRBAttachInfo:public APhysXSerializerBase
{
	DeclareVersion(0xcc000001)

public:

	APhysXRBAttachInfo()
	{
		mAttachedRBObjName = APX_ATTACHER_ATTACHED_RB_NAME;
		mAttachedRBActorName = NULL;
		mAttacherName = APX_ATTACHER_ATTACHER_NAME;

		mRBAttachType = APX_RB_ATTYPE_FIX;
	}

	const APhysXRBAttachInfo& operator= (const APhysXRBAttachInfo& other);


	const char* mAttachedRBObjName;
	const char* mAttachedRBActorName;
	const char* mAttacherName;

	APhysXRBAttachType mRBAttachType;

};

*/


class APHYSX_DLL_EXPORT APhysXRBAttacherDesc:public APhysXAttacherDesc
{

	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_RB_ATTACHER);

public:
	
	APhysXRBAttacherDesc() {}
	virtual ~APhysXRBAttacherDesc() { Release(); }


	void AddRBComponent(const APhysXObjectDesc* pRBObjectDesc)
	{
		AddMultiRBComponent(pRBObjectDesc, APX_ATTACHER_ATTACHED_RB_NAME);
	}


	void AddMultiRBComponent(const APhysXObjectDesc* pRBObjectDesc, const char* szRBName)
	{
		APHYSX_ASSERT(pRBObjectDesc->IsRigidBody() && szRBName);

		if(pRBObjectDesc->IsRigidBody() && szRBName)
			AddObjectComponent(pRBObjectDesc, szRBName);
	}

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	/*
	virtual APhysXObjectDesc* Clone() const
	{
		CloneThis(APhysXRBAttacherDesc, APhysXObjectDesc)
	}
	*/
	DeclareCloneAPhysXObjectDesc(APhysXRBAttacherDesc)

	// virtual void CompleteInnerConnectorInfo();
	
	void AddAttachInfo(
		const char* szAttachedActorName, 
		APhysXRBAttachType apxRBAttachType = APX_RB_ATTYPE_FIX, 
		const NxVec3& vAnchorPos = NxVec3(0.0f),								// this will take effect for the case of APX_RB_ATTYPE_ROTATION_FREE.
		const char* szAttacherName = APX_ATTACHER_ATTACHER_NAME,
		const char* szAttachedRBObjName = APX_ATTACHER_ATTACHED_RB_NAME
		);


};




#endif