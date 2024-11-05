
#include "APhysXStdAfx.h"

bool APhysXBoxShapeDesc::Save(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeBuffer((const void*)(&mDimensions), sizeof(NxVec3));

	return true;

}

bool APhysXBoxShapeDesc::Load(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	pNxStream->readBuffer((void *)(&mDimensions), sizeof(NxVec3));

	return true;
}

void APhysXBoxShapeDesc::GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D)
{
	APhysXShapeDesc::GetNxShapeDesc(InNxShapeDesc, vSacle3D);

	NxBoxShapeDesc& BoxShapeDesc = (NxBoxShapeDesc&) InNxShapeDesc;
	BoxShapeDesc.dimensions.arrayMultiply(mDimensions, vSacle3D);

}

void APhysXBoxShapeDesc::GetNxForceFieldShapeDesc(NxForceFieldShapeDesc& InNxForceFieldShapeDesc, const NxVec3& vSacle3D)
{
	APhysXShapeDesc::GetNxForceFieldShapeDesc(InNxForceFieldShapeDesc);

	NxBoxForceFieldShapeDesc& boxFFShapeDesc = (NxBoxForceFieldShapeDesc&) InNxForceFieldShapeDesc;

	boxFFShapeDesc.dimensions.arrayMultiply(mDimensions, vSacle3D);

}