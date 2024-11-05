
#include "APhysXStdAfx.h"

bool APhysXCapsuleShapeDesc::Save(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeFloat(mRadius);
	pNxStream->storeFloat(mHeight);
	pNxStream->storeDword(mFlags);

	return true;

}

bool APhysXCapsuleShapeDesc::Load(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	mRadius = pNxStream->readFloat();
	mHeight = pNxStream->readFloat();
	mFlags = pNxStream->readDword();

	return true;
}

void APhysXCapsuleShapeDesc::GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D)
{
	// to scale capsule shape correctly, we should make sure
	// that vScale3D is uniform one
	APHYSX_ASSERT(NxMath::equals(vSacle3D.x, vSacle3D.z, APHYSX_SCALE_EPSILON));

	APhysXShapeDesc::GetNxShapeDesc(InNxShapeDesc, vSacle3D);
	NxCapsuleShapeDesc& CapsuleShapeDesc = (NxCapsuleShapeDesc&) InNxShapeDesc;
	CapsuleShapeDesc.radius = mRadius * vSacle3D.x;
	CapsuleShapeDesc.height = mHeight * vSacle3D.y;
	if(CapsuleShapeDesc.height < 1e-4)
	{
		CapsuleShapeDesc.height = 0.001f;
	}

	CapsuleShapeDesc.flags = mFlags;

}

void APhysXCapsuleShapeDesc::GetNxForceFieldShapeDesc(NxForceFieldShapeDesc& InNxForceFieldShapeDesc, const NxVec3& vSacle3D)
{
	// to scale capsule shape correctly, we should make sure
	// that vScale3D is uniform one
	APHYSX_ASSERT(NxMath::equals(vSacle3D.x, vSacle3D.z, APHYSX_SCALE_EPSILON));	
	
	APhysXShapeDesc::GetNxForceFieldShapeDesc(InNxForceFieldShapeDesc);

	NxCapsuleForceFieldShapeDesc& capsuleFFShapeDesc = (NxCapsuleForceFieldShapeDesc&) InNxForceFieldShapeDesc;
	capsuleFFShapeDesc.radius = mRadius * vSacle3D.x;
	capsuleFFShapeDesc.height = mHeight * vSacle3D.y;

}