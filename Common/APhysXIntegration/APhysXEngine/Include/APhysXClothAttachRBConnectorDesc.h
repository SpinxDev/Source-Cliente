/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCLOTHATTACHRBCONNECTORDESC_H_
#define _APHYSXCLOTHATTACHRBCONNECTORDESC_H_


class APHYSX_DLL_EXPORT APhysXClothAttachRBConnectorDesc:public APhysXObjectConnectorDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_CLOTH_ATTACH_RB);

public:
	
	APhysXClothAttachRBConnectorDesc() 
	{
		mOnlyAttachSpecifiedObject = true;
		mAttachedNxActorName = NULL;

		mAttachmentFlags = 0;
	}

	~APhysXClothAttachRBConnectorDesc() {}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_CLOTH_ATTACH_RB)
		{
			APhysXClothAttachRBConnectorDesc* pDestClothAttachRBConnectorDesc = (APhysXClothAttachRBConnectorDesc*) pDestConnectorDesc;
			*pDestClothAttachRBConnectorDesc = *this;
		}
	}

	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXClothAttachRBConnectorDesc;
		*(APhysXClothAttachRBConnectorDesc *)pPhysXConnectorDesc = *this;

		return pPhysXConnectorDesc;
	}


public:
	
	//---------------------------------------------------------------------
	// note here:
	// if mOnlyAttachSpecifiedObject == true and mAttachedNxActorName == NULL
	// we will try attaching cloth to all NxActors of the RB object
	bool mOnlyAttachSpecifiedObject;
	const char* mAttachedNxActorName;
	//---------------------------------------------------------------------

	APhysXU32 mAttachmentFlags;


};



#endif