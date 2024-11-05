
#include "APhysXStdAfx.h"

bool APhysXSphereShapeDesc::Save(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Save(pNxStream))
		return false;

	// save my version...
	SaveVersion(pNxStream);
	
	pNxStream->storeFloat(mRadius);

	return true;

}

bool APhysXSphereShapeDesc::Load(NxStream* pNxStream)
{
	if(!APhysXShapeDesc::Load(pNxStream))
		return false;

	// load my version...
	LoadVersion(pNxStream);

	mRadius = pNxStream->readFloat();

	return true;
}

void APhysXSphereShapeDesc::GetNxShapeDesc(NxShapeDesc& InNxShapeDesc, const NxVec3& vSacle3D)
{
	// to scale sphere shape correctly, we should make sure
	// that vScale3D is uniform one
	APHYSX_ASSERT(NxMath::equals(vSacle3D.x, vSacle3D.y, APHYSX_SCALE_EPSILON) && NxMath::equals(vSacle3D.x, vSacle3D.z, APHYSX_SCALE_EPSILON) );

	APhysXShapeDesc::GetNxShapeDesc(InNxShapeDesc, vSacle3D);

	NxSphereShapeDesc& SphereShapeDesc = (NxSphereShapeDesc&) InNxShapeDesc;
	SphereShapeDesc.radius = mRadius * vSacle3D.x;
}

void APhysXSphereShapeDesc::GetNxForceFieldShapeDesc(NxForceFieldShapeDesc& InNxForceFieldShapeDesc, const NxVec3& vSacle3D)
{
	// to scale sphere shape correctly, we should make sure
	// that vScale3D is uniform one
	APHYSX_ASSERT(NxMath::equals(vSacle3D.x, vSacle3D.y, APHYSX_SCALE_EPSILON) && NxMath::equals(vSacle3D.x, vSacle3D.z, APHYSX_SCALE_EPSILON));

	APhysXShapeDesc::GetNxForceFieldShapeDesc(InNxForceFieldShapeDesc);

	NxSphereForceFieldShapeDesc& sphereFFShapeDesc = (NxSphereForceFieldShapeDesc&) InNxForceFieldShapeDesc;

	sphereFFShapeDesc.radius = mRadius * vSacle3D.x;

}