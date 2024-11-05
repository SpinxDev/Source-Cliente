/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXPULLEYJOINTDESC_H_
#define _APHYSXPULLEYJOINTDESC_H_


class APHYSX_DLL_EXPORT APhysXPulleyJointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_PULLEY_JOINT);

public:

	APhysXPulleyJointDesc() 
	{
		mPulley[0].zero();
		mPulley[1].zero();
		mDistance = 0.0f;
		mStiffness = 1.0f;
		mRatio = 1.0f;
		mPulleyJointFlags = 0;

		mMotor.setToDefault();

	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	virtual void GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D);

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_PULLEY_JOINT)
		{
			APhysXPulleyJointDesc* pDestJointDesc = (APhysXPulleyJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXPulleyJointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}

	virtual void TransformLocal2Global(const NxMat34& globalPose, const NxVec3& vScale3D) 
	{
		APhysXJointDesc::TransformLocal2Global(globalPose, vScale3D);

		// make sure it is the uniform scaling...
		APHYSX_ASSERT(vScale3D.x == vScale3D.y && vScale3D.x == vScale3D.z);
		
		mPulley[0].arrayMultiply(mPulley[0], vScale3D);
		mPulley[0] = globalPose * mPulley[0];
		
		mPulley[1].arrayMultiply(mPulley[1], vScale3D);
		mPulley[1] = globalPose * mPulley[1];

		mDistance *= vScale3D.x;
	}

public:

	// to do... lots of new data members...
	NxVec3  mPulley[2];
	float  mDistance;
	float  mStiffness;
	float  mRatio;
	APhysXU32  mPulleyJointFlags;
	NxMotorDesc  mMotor; 

};
#endif