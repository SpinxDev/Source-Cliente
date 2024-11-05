/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXDISTANCEJOINTDESC_H_
#define _APHYSXDISTANCEJOINTDESC_H_


class APHYSX_DLL_EXPORT APhysXDistanceJointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_DISTANCE_JOINT);

public:

	APhysXDistanceJointDesc() 
	{
		mMaxDistance = mMinDistance = 0.0f;
		mDistanceJointFlags = 0;

		// mSpring.setToDefault();
	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	virtual void GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D);

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_DISTANCE_JOINT)
		{
			APhysXDistanceJointDesc* pDestJointDesc = (APhysXDistanceJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXDistanceJointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}

	virtual void TransformLocal2Global(const NxMat34& globalPose, const NxVec3& vScale3D) 
	{
		APhysXJointDesc::TransformLocal2Global(globalPose, vScale3D);
		
		// make sure it is the uniform scaling...
		APHYSX_ASSERT(vScale3D.x == vScale3D.y && vScale3D.x == vScale3D.z);

		mMaxDistance *= vScale3D.x;
		mMinDistance *= vScale3D.x;
	}

public:

	// to do... lots of new data members...
	float mMaxDistance;
	float mMinDistance;

	NxSpringDesc mSpring;

	// different from mJointFlags...
	APhysXU32 mDistanceJointFlags;


};

#endif