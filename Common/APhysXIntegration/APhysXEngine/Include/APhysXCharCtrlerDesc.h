/*
* FILE: APhysXCharCtrlerDesc.h
*
* DESCRIPTION: 
*	This class is used for the creation of character controller.
*
*
* CREATED BY: Yang Liu, 2009/09/01
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSX_CHARACTER_CONTROLLER_DESCRIPTION_H_
#define _APHYSX_CHARACTER_CONTROLLER_DESCRIPTION_H_

class NxControllerDesc;
class APhysXCCHitReport;

class APhysXCCDescBase
{
protected:
	virtual	~APhysXCCDescBase() {}

public:
	static bool HostModelChannelIsValid(const APhysXCollisionChannel& HostModelCC)
	{
		if ((APX_COLLISION_CHANNEL_INVALID < HostModelCC) && (APX_COLLISION_CHANNEL_CC_COMMON_HOST != HostModelCC))
			return false;
		return true;
	}

public:
	virtual	void SetToDefault() = 0;
	virtual bool IsValid() const { return true;	}

public:
	/*
	[Deprecated]
	Default: false
	*/
	//bool hostModelHasPhysX;

	/*
	[Deprecated] Using APhysXCCBase.RegisterHostModel() instead of this property;
	Default: APX_COLLISION_CHANNEL_CC_COMMON_HOST
	Note: If the value of hostModelHasPhysX is false, the applied value is APX_COLLISION_CHANNEL_CC_COMMON_HOST.
	*/
	//APhysXCollisionChannel hostModelChannel;

	/*
	The position of CC's foot
	*/
	NxVec3 posFoot;
};

class APHYSX_DLL_EXPORT APhysXCCDesc : public APhysXCCDescBase
{
protected:
	static float AngleToRadian(const float angle);
	static float GetSkinWidth(const float sw);

public:
	virtual const NxControllerDesc& GetPhysXDesc() const = 0;
	virtual	void SetToDefault();
	virtual bool IsValid() const
	{
		if (!APhysXCCDescBase::IsValid())
			return false;
		if (0.0f > mass)
			return false;
		if (0.0f > speed)
			return false;
		if (0.0f > stepOffset)
			return false;
		if (0.0f > slopeUpAngleLimit)
			return false;
		if (0.0f > slopeDownAngleLimit)
			return false;
		return true;
	}

	float GetSlopeUpRadian() const { return AngleToRadian(slopeUpAngleLimit); }
	float GetSlopeDownRadian() const { return AngleToRadian(slopeDownAngleLimit); }
	void  SetDefaultFlags(bool isDynCC);

public:
	/*
	The mass of CC. 
	Default: 1.2
	Note: For NxCC, doesn't work. 
	*/
	float mass;

	/*
	The horizontal speed of CC.  
	Default: 1
	*/
	float speed;
	
	/*
	The maximum height of an obstacle which the character can climb. This is the identical item with NxControllerDesc::stepOffset.  
	Default: 0.15
	*/
	float stepOffset;

	/*
	The maximum slope degree which the character can walk up. This degree also impact the action of attachment and slide.
	CC can't attach to the position that surface slope is greater that this degree whatever attachment flags were raised. 
	CC will slide down when CC is standing on a ramp that surface slope is greater that this degree if APX_CCF_SLIDE_ENABLE flag was raised. 

	Default: 45
	Unit: Degrees
	Note: If the angle > 360 the applied value is angle mod 360.

		    \
	         \
    the angle \
       --------
	*/
	float slopeUpAngleLimit;

	/*
	The maximum slope degree which the character can walk down. 
	The character will be moved in the way of Free Falling if the 
	actual slope degree > this Limit.

	Default: 75
	Unit: Degrees
	Note: If the angle > 360 the applied value is angle mod 360.
		  For NxCC, doesn't work. 

    --------
	\ the angle
	 \
      \
	*/
	float slopeDownAngleLimit;
	
	/*
	[Deprecated] Using gPhysXCCHitReport->Register();
	Callback routines when CC hit something. (only supported by APhysXCharacterController) 
	Default: null
	*/
	// APhysXCCHitReport* callback;
	
	/*
	CC's flags or DynCC's flags. See also APhysXCCFlag. 
	Default: (reference CC's APhysXCCFlag)
	*/
	int flags;
};

class APHYSX_DLL_EXPORT APhysXCCBoxDesc : public APhysXCCDesc
{
public:
	APhysXCCBoxDesc() { SetToDefault(); }
	virtual const NxControllerDesc& GetPhysXDesc() const;
	virtual	void SetToDefault();
	virtual bool IsValid() const
	{
		if (!APhysXCCDesc::IsValid())
			return false;
		if (0.0f >= dimensions.x)
			return false;
		if (0.0f >= dimensions.y)
			return false;
		if (0.0f >= dimensions.z)
			return false;
		return true;
	}

public:
	NxVec3 dimensions;			
};

class APHYSX_DLL_EXPORT APhysXCCCapsuleDesc : public APhysXCCDesc
{
public:
	APhysXCCCapsuleDesc() { SetToDefault(); }
	virtual const NxControllerDesc& GetPhysXDesc() const;
	virtual	void SetToDefault();
	virtual bool IsValid() const
	{
		if (!APhysXCCDesc::IsValid())
			return false;
		if (0.0f >= radius)
			return false;
		if (0.0f >= height)
			return false;
		if (height < radius * 2)
			return false;
		return true;
	}

public:
	float radius;

	/*
	CC's total height from top to bottom. Requirement: height >= 2 * radius
	Default: 2 * radius
	*/
	float height;
};

class APHYSX_DLL_EXPORT APhysXLWCCDesc : public APhysXCCDescBase
{
public:
	APhysXLWCCDesc() { SetToDefault(); }
	virtual	void SetToDefault();
	virtual bool IsValid() const
	{
		if (!APhysXCCDescBase::IsValid())
			return false;
		if (0.0f >= radius)
			return false;
		if (0.0f >= height)
			return false;
		if (height < radius * 2)
			return false;
		return true;
	}

public:
	float radius;
	
	/*
	CC's total height from top to bottom. Requirement: height >= 2 * radius
	Default: 2 * radius
	*/
	float height;
	
	/*
	LWCC's flags. See also APhysXLWCCFlag.
	Default: (reference APhysXLWCCFlag)
	*/
	int flags;
};

#endif