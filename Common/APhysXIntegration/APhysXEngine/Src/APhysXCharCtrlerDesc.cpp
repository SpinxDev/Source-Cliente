
#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerCallBackAgent.h"
#include "..\..\APhysXNxCharacter_Public\include\NxBoxController.h"
#include "..\..\APhysXNxCharacter_Public\include\NxCapsuleController.h"

void APhysXCCDescBase::SetToDefault()
{
	posFoot.zero();
}

void APhysXCCDesc::SetToDefault()
{
	APhysXCCDescBase::SetToDefault();
	mass = 1.2f;
	speed = 1.0f;
	stepOffset = 0.15f;
	slopeUpAngleLimit = 45;
	slopeDownAngleLimit = 75;
	SetDefaultFlags(false);
}

float APhysXCCDesc::AngleToRadian(const float angle)
{
	return NxMath::mod(angle, 360) * NxPi / 180.0f;
}

float APhysXCCDesc::GetSkinWidth(const float sw)
{
	static const float MaxSkinWidth = 0.05f;
	return (MaxSkinWidth < sw)? MaxSkinWidth : sw; 
}

void APhysXCCDesc::SetDefaultFlags(bool isDynCC)
{
	if (isDynCC)
	{
		flags = APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE;
		flags |= APhysXDynCharCtrler::APX_CCF_AUTO_HANG_END_ENABLE;
	}
	else
	{
		flags = APhysXCharacterController::APX_CCF_GRAVITY_ENABLE;
		flags |= APhysXCharacterController::APX_CCF_GRAVITY_READSCENE;
		flags |= APhysXCharacterController::APX_CCF_SLIDE_ENABLE;
		flags |= APhysXCharacterController::APX_CCF_ADD_PUSHFORCE;
		flags |= APhysXCharacterController::APX_CCF_ADD_STANDFORCE;
	}
}

void APhysXCCBoxDesc::SetToDefault()
{
	APhysXCCDesc::SetToDefault();
	dimensions.zero();
}

const NxControllerDesc& APhysXCCBoxDesc::GetPhysXDesc() const
{
	static NxBoxControllerDesc desc;
	
	const float size = (dimensions.x < dimensions.z)? dimensions.x : dimensions.z;
	
	desc.setToDefault();
	desc.extents.set(dimensions.x, dimensions.y, dimensions.z);
	desc.skinWidth	 = GetSkinWidth(size * 0.1f);
	desc.stepOffset	 = stepOffset;
	desc.upDirection = NX_Y;
	desc.callback	 = APhysXCCHitReportAgent::GetPtrInstance();
	desc.position.x	 = posFoot.x;
	desc.position.y	 = posFoot.y + desc.skinWidth + desc.extents.y;
	desc.position.z	 = posFoot.z;
	desc.slopeLimit  = (float)cos(GetSlopeUpRadian());
	
	return desc;
}

void APhysXCCCapsuleDesc::SetToDefault()
{
	APhysXCCDesc::SetToDefault();
	radius = height = 0;
}

const NxControllerDesc& APhysXCCCapsuleDesc::GetPhysXDesc() const
{
	static NxCapsuleControllerDesc desc;

	desc.setToDefault();
	desc.height		 = height - 2 * radius;
	desc.radius		 = radius;
	desc.skinWidth	 = GetSkinWidth(desc.radius * 0.1f);
	desc.stepOffset	 = stepOffset;
	desc.upDirection = NX_Y;
	desc.callback	 = APhysXCCHitReportAgent::GetPtrInstance();
	desc.position.x	 = posFoot.x;
	desc.position.y	 = posFoot.y + desc.skinWidth + desc.radius + desc.height * 0.5f;
	desc.position.z	 = posFoot.z;
	desc.slopeLimit  = (float)cos(GetSlopeUpRadian());

	return desc;
}

void APhysXLWCCDesc::SetToDefault()
{
	APhysXCCDescBase::SetToDefault();
	radius = height = 0;
	flags = 0;
}