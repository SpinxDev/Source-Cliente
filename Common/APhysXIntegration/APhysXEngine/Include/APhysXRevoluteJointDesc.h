/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXREVOLUTEJOINTDESC_H_
#define _APHYSXREVOLUTEJOINTDESC_H_


class APHYSX_DLL_EXPORT APhysXRevoluteJointDesc:public APhysXJointDesc
{
	DeclareVersion(0xcc000001);
	DeclareConnectorType(APX_CNTTYPE_REVOLUTE_JOINT);

public:

	APhysXRevoluteJointDesc() 
	{
		mProjectionDistance = 1.0f;
		mProjectionAngle = 0.0872f;  //about 5 degrees in radians.
		mRevoluteJointFlags = 0;
		mProjectionMode = NX_JPM_NONE;

	}

	// virtual bool Save(NxStream* pNxStream);
	// virtual bool Load(NxStream* pNxStream);

	virtual void GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D);

	virtual void CopyTo(APhysXObjectConnectorDesc* pDestConnectorDesc) const
	{
		if(pDestConnectorDesc->GetConnectorType() == APX_CNTTYPE_REVOLUTE_JOINT)
		{
			APhysXRevoluteJointDesc* pDestJointDesc = (APhysXRevoluteJointDesc*) pDestConnectorDesc;
			*pDestJointDesc = *this;
		}
	}

	// virtual interface
	virtual APhysXObjectConnectorDesc* Clone() const
	{
		APhysXObjectConnectorDesc* pPhysXConnectorDesc = new APhysXRevoluteJointDesc;
		CopyTo(pPhysXConnectorDesc);
		return pPhysXConnectorDesc;
	}

public:

	// to do... lots of new data members...
	NxJointLimitPairDesc  mLimit;
	NxMotorDesc  mMotor;
	NxSpringDesc  mSpring;
	float  mProjectionDistance;
	float  mProjectionAngle;
	APhysXU32  mRevoluteJointFlags;
	NxJointProjectionMode  mProjectionMode;
	

};


#endif