
#include "APhysXStdAfx.h"

bool APhysXShapeDesc::Save(NxStream* pNxStream)
{
	if(!APhysXSerializerBase::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	// note the order!
	pNxStream->storeDword(mShapeFlags);
	pNxStream->storeBuffer((const void *)(&mLocalPose), sizeof(NxMat34));
	pNxStream->storeDword(mShapeGroup);
	pNxStream->storeDword(mMaterialIndex);

	return true;
}

bool APhysXShapeDesc::Load(NxStream* pNxStream)
{ 
	if(!APhysXSerializerBase::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	if(m_StreamObjVer == GetVersion())
	{
		mShapeFlags = pNxStream->readDword();
		pNxStream->readBuffer((void *)(&mLocalPose), sizeof(NxMat34));
		mShapeGroup = pNxStream->readDword();
		mMaterialIndex = pNxStream->readDword();

		return true;
	}
	else if(m_StreamObjVer == 0xcc000000)
	{
		// handle the former version loading

		return true;

	}
	else
		return false;
}

void APhysXShapeDesc::GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D)
{
	InNxShapeDesc.setToDefault();

	InNxShapeDesc.shapeFlags = mShapeFlags;
	InNxShapeDesc.localPose.M = mLocalPose.M;
	InNxShapeDesc.localPose.t.arrayMultiply(mLocalPose.t, vSacle3D);

	InNxShapeDesc.group = mShapeGroup;
	InNxShapeDesc.materialIndex = mMaterialIndex;
	
	InNxShapeDesc.name = m_szName;

}

void APhysXShapeDesc::GetNxForceFieldShapeDesc(NxForceFieldShapeDesc& InNxForceFieldShapeDesc, const NxVec3& vSacle3D )
{
	InNxForceFieldShapeDesc.setToDefault();

	InNxForceFieldShapeDesc.pose.M = mLocalPose.M;
	InNxForceFieldShapeDesc.pose.t.arrayMultiply(mLocalPose.t, vSacle3D);

	InNxForceFieldShapeDesc.name = m_szName;

}