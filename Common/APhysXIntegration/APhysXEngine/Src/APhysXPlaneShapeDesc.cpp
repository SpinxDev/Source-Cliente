
#include "APhysXStdAfx.h"

bool APhysXPlaneShapeDesc::Save(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeBuffer((const void*)(&mNormal), sizeof(NxVec3));
	pNxStream->storeFloat(mD);

	return true;

}

bool APhysXPlaneShapeDesc::Load(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	pNxStream->readBuffer((void *)(&mNormal), sizeof(NxVec3));
	mD = pNxStream->readFloat();

	return true;
}

void APhysXPlaneShapeDesc::GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D)
{
	// To note: the plane shape do not support scale!

	APhysXShapeDesc::GetNxShapeDesc(InNxShapeDesc, vSacle3D);

	NxPlaneShapeDesc& PlaneShapeDesc = (NxPlaneShapeDesc&) InNxShapeDesc;
	PlaneShapeDesc.normal = mNormal;
	PlaneShapeDesc.d = mD;

}
