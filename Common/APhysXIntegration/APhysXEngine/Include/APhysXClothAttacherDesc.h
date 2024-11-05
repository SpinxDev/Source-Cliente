/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCLOTHATTACHERDESC_H_
#define _APHYSXCLOTHATTACHERDESC_H_


// define the local name...
// #define APX_CLOTHATTACHER_CLOTH_NAME "ClothAttacher_Cloth"
// #define APX_CLOTHATTACHER_ATTACHER_NAME "ClothAttacher_RBAttacher"

class APHYSX_DLL_EXPORT APhysXClothAttacherDesc:public APhysXAttacherDesc
{

	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_CLOTH_ATTACHER);

public:

	APhysXClothAttacherDesc() 
	{
		/*
		m_pRBObjectDesc = NULL;
		m_pClothObjectDesc = NULL;
		*/

	}

	virtual ~APhysXClothAttacherDesc() { Release(); }

	void AddClothComponent(const APhysXObjectDesc* pClothObjectDesc)
	{
		AddMultiClothComponent(pClothObjectDesc, APX_ATTACHER_ATTACHED_CLOTH_NAME);
	}


	// if you want to add more than one cloth, use this function.
	void AddMultiClothComponent(const APhysXObjectDesc* pClothObjectDesc, const char* szClothName)
	{
		APHYSX_ASSERT(pClothObjectDesc->IsCloth() && szClothName);

		if(pClothObjectDesc->IsCloth() && szClothName)
			AddObjectComponent(pClothObjectDesc, szClothName);
	}

	/*

	//----------------------------------------------------------------------
	// Note: by wenfeng, Feb. 23, 2009
	// the following code is refactored and placed in its' base class...

	// by default, we only support one attacher object. 
	void AddAttacherComponent(const APhysXObjectDesc* pRBObjectDesc)
	{
		AddMultiAttacherComponent(pRBObjectDesc, APX_CLOTHATTACHER_ATTACHER_NAME);
	}


	// if you want to add more than one attacher, use this function.
	void AddMultiAttacherComponent(const APhysXObjectDesc* pRBObjectDesc, const char* szAttacherName)
	{
		APHYSX_ASSERT(pRBObjectDesc->IsRigidBody() && szAttacherName);

		if(pRBObjectDesc->IsRigidBody() && szAttacherName)
		{
			AddObjectComponent(pRBObjectDesc, szAttacherName);
		
			// set the attacher object's collision group to APX_COLLISION_GROUP_CLOTH_ATTACHER
			APhysXObjectComponentDesc* pComponentDesc = m_arrObjectComponentDescs.back();
			pComponentDesc->mPhysXObjectDesc->mCollisionGroup = APX_COLLISION_GROUP_CLOTH_ATTACHER;

			//??? to do...
			// maybe we'd better process other RB attacher's properties, such as the density, the mass,
			// so that the attacher won't have some side-effect on physics behavior...
		}
	}
	*/
	
	virtual bool AddMultiAttacherComponent(const APhysXObjectDesc* pRBObjectDesc, const char* szAttacherName, const NxMat34& matLocalPose = NxMat34(true));

	virtual bool IsValid();

	// call after add cloth and attacher components to add 
	// inner connector according to component info...
	virtual void CompleteInnerConnectorInfo();

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXClothAttacherDesc)

	// access fluid parameters and emitter parameters directly...
	APhysXClothParameters* GetClothParameters(const char* szClothName = APX_ATTACHER_ATTACHED_CLOTH_NAME);

// properties...
protected:

/*
	//??? keep the pointer of 
	APhysXObjectDesc* m_pRBObjectDesc;	
	APhysXObjectDesc* m_pClothObjectDesc;
*/

};


#endif