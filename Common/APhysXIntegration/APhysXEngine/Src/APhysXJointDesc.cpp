
#include "APhysXStdAfx.h"

bool APhysXJointDesc::Save(NxStream* pNxStream)
{
	if(!APhysXObjectConnectorDesc::Save(pNxStream))
		return false;
	
	// save my version...
	SaveVersion(pNxStream);
	
	APhysXSerializerBase::StoreString(pNxStream, mNxActor1Name);
	APhysXSerializerBase::StoreString(pNxStream, mNxActor2Name);

	pNxStream->storeBuffer((const void*)(&mGlobalAnchor), sizeof(NxVec3));
	pNxStream->storeBuffer((const void*)(&mGlobalAxis), sizeof(NxVec3));

	pNxStream->storeBuffer((const void*)(&mLocalAxis), 2 * sizeof(NxVec3));
	pNxStream->storeBuffer((const void*)(&mLocalNormal), 2 * sizeof(NxVec3));

	pNxStream->storeFloat(mMaxForce);
	pNxStream->storeFloat(mMaxTorque);

	pNxStream->storeDword(mJointFlags);

	pNxStream->storeFloat(mSolverExtrapolationFactor);

	return true;
}

bool APhysXJointDesc::Load(NxStream* pNxStream)
{ 
	if(!APhysXObjectConnectorDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer >= 0xcc000001)
	{
		mNxActor1Name = APhysXSerializerBase::ReadString(pNxStream);
		mNxActor2Name = APhysXSerializerBase::ReadString(pNxStream);

		pNxStream->readBuffer((void*)(&mGlobalAnchor), sizeof(NxVec3));
		pNxStream->readBuffer((void*)(&mGlobalAxis), sizeof(NxVec3));

		pNxStream->readBuffer((void*)(&mLocalAxis), 2 * sizeof(NxVec3));
		pNxStream->readBuffer((void*)(&mLocalNormal), 2 * sizeof(NxVec3));

		mMaxForce = pNxStream->readFloat();
		mMaxTorque = pNxStream->readFloat();

		mJointFlags = pNxStream->readDword();
		
	}


	if(m_StreamObjVer == GetVersion())
	{
		mSolverExtrapolationFactor = pNxStream->readFloat();
	}

	return true;

}

void APhysXJointDesc::GetNxJointDesc(NxJointDesc& jointDesc, const NxMat34& globalPose, const NxVec3& vScale3D)
{
	APHYSX_ASSERT(vScale3D.x == vScale3D.y && vScale3D.x == vScale3D.z);

	// we must make sure at least one of actor[2] member is not NULL
	if(jointDesc.actor[0] || jointDesc.actor[1])
	{
		// firstly, set joint's local axises/normals
		// since they are in local frame,no need to execute the transformation and scaling
		jointDesc.localAxis[0] = mLocalAxis[0];
		jointDesc.localAxis[1] = mLocalAxis[1];

		jointDesc.localNormal[0] = mLocalNormal[0];
		jointDesc.localNormal[1] = mLocalNormal[1];
		
		// transform the data to global frame, refer to TransformLocal2Global(...) function
		NxVec3 vGlobalAnchor;
		vGlobalAnchor.arrayMultiply(mGlobalAnchor, vScale3D);
		vGlobalAnchor = globalPose * vGlobalAnchor;
		jointDesc.setGlobalAnchor(vGlobalAnchor);

		// note here...
		// if mGlobalAxis is zero vector which means that users don't set it,
		// we will not call setGlobalAxis(...) function. In this case, mLocalAxis[2]
		// will take effect...
		if(!mGlobalAxis.isZero())
		{
			NxVec3 vGlobalAxis = globalPose.M * mGlobalAxis;
			jointDesc.setGlobalAxis(vGlobalAxis);
		}

		jointDesc.maxForce = mMaxForce;
		jointDesc.maxTorque = mMaxTorque;

		jointDesc.jointFlags = mJointFlags;

		jointDesc.solverExtrapolationFactor = mSolverExtrapolationFactor;
	}
	else
		APHYSX_ASSERT(0);
}

//-----------------------------------------------------------------------------------------

ImplementVersion(APhysXCylindricalJointDesc, APhysXJointDesc)

//-----------------------------------------------------------------------------------------

ImplementVersion(APhysXFixedJointDesc, APhysXJointDesc)

//-----------------------------------------------------------------------------------------

ImplementVersion(APhysXPointInPlaneJointDesc, APhysXJointDesc)

//-----------------------------------------------------------------------------------------

ImplementVersion(APhysXPointOnLineJointDesc, APhysXJointDesc)

//-----------------------------------------------------------------------------------------

ImplementVersion(APhysXPrismaticJointDesc, APhysXJointDesc)
