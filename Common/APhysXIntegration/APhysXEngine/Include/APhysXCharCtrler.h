/*
* FILE: APhysXCharCtrler.h
*
* DESCRIPTION: 
*	This class wrap the behavior of character.
*
*
* CREATED BY: Yang Liu, 2009/09/01
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_H_
#define _APHYSX_CHARACTER_CONTROLLER_H_

class NxController;

class APHYSX_DLL_EXPORT APhysXCharacterController : public APhysXHostCCBase
{
public:
	enum APhysXCCFlag
	{
		// Default: on
		// Enable gravity if raised, or disable gravity. 
		APX_CCF_GRAVITY_ENABLE		= (1<<0),

		// Default: on
		// Using the gravity that read from NxScene if raised, or using the user setting's gravity.
		APX_CCF_GRAVITY_READSCENE	= (1<<1),
		
		// Default: on
		// Enable slide movement if raised. When the CC is standing on a ramp and the ramp's slope-degree is 
		// greater than APhysXCCDesc.slopeUpAngleLimit, you can control the cc to slide down or not by this flag.  
		// See also APhysXCCDesc.slopeUpAngleLimit.
		APX_CCF_SLIDE_ENABLE		= (1<<2),

		// Default: on
		// Enable or disable add push force when touch something. 
		// The final applied force is depending on "CC'flag && APhysXObject's flag". See also APhysXObjectFlag.
		APX_CCF_ADD_PUSHFORCE		= (1<<3),

		// Default: on
		// Enable or disable add stand force(including jump start and jump landing) when stand on something.
		// The final applied force is depending on "CC'flag && APhysXObject's flag". See also APhysXObjectFlag.
		APX_CCF_ADD_STANDFORCE		= (1<<4),
		
		// Default: off
		// Enforce enable attach to dynamic(or kinematic) actor if using this flag.
		// The priority of this flag is higher than the related APhysXObject's flag. See also APhysXObjectFlag.
		APX_CCF_ATTACHMENT_ENFORCE_OPEN	= (1<<5),

		// Default: off
		// Enforce disable attach to dynamic(or kinematic) actor if using this flag.
		// The priority of this flag is higher than APX_CCF_ATTACHMENT_ENFORCE_OPEN and higher than the related APhysXObject's flag.
		// See also APhysXObjectFlag.
		APX_CCF_ATTACHMENT_ENFORCE_CLOSE	= (1<<6),

		// Default: off
		// If raise this flag, open the half-binding behavior. "Half-binding" means only revise vertical position to keep stand on surface.
		// There is no absolute horizontal revised movement if CC is standing.
		APX_CCF_ATTACHMENT_ONLY_VERTICAL	= (1<<7),

		// Default: off
		// If raise this flag, user can control CC'jumping direction even CC is in air state.
		APX_CCF_CONTROL_JUMPINGDIR_ENABLE	= (1<<8),
	};

public:
	APhysXCharacterController(NxController& controller, APhysXScene& scene, const APhysXCCDesc& desc);
	virtual ~APhysXCharacterController() {};

	bool ReadFlag(const APhysXCCFlag flag) const;
	void ClearFlag(const APhysXCCFlag flag);
	void RaiseFlag(const APhysXCCFlag flag);

	NxVec3 GetGravity() const;
	void   SetUserGravity(const NxVec3& g) { m_UserGravity = g; }

	/*
	get/set the total hit force's magnitude.  TotalHitForce = hitDir * m_PushFactor

	Default Value: 10
	*/
	float GetPushFactor() const { return m_PushFactor; }
	void  SetPushFactor(const float p) { m_PushFactor = p; }

	/*
	get/set attaching point linear velocity threshold.
	When the magnitude of attached point's linear velocity is bigger than the threshold, CC will forecast the moving distance of binding object.
	Also, the attached point's linear velocity will be count in if it's bigger than the threshold while CC start jump from the binding object.
	  
	Default Value: 0.2f
	*/
	float GetAttachingPTLinearVelThreshold() const { return m_posFCThreshold; }
	void  SetAttachingPTLinearVelThreshold(const float threshold) { m_posFCThreshold = threshold; }
	
	/*
	clear current horizon velocity no matter CC is in air or not.
	*/
	void EnforceClearHorizonVelocity(bool bIsAbsoluteVel = false);

	virtual NxMat33 GetGlobalOrientation() const { return GetIDGlobalOrientation(); }

protected:
	enum InternalInfo
	{
		CCII_NEW_ATTACHMENT_VERTICAL	= (1<<15),
		CCII_CLEAR_ABSOLUTE_HORIZON_VEL	= (1<<16),
		CCII_CLEAR_RELATIVE_HORIZON_VEL	= (1<<17),
		CCII_CLEAR_HORIZON_REVISE		= (1<<18),
		CCII_SET_ABSOLUTE_JUMPING_VEL	= (1<<19),
		CCII_IS_SLIDING_MOVE			= (1<<20),
		CCII_DONE_CALL_SCENE_CHANGED	= (1<<21),
		CCII_DONE_TELEPORT				= (1<<22),
		CCII_COLLISION_INFO_VALID		= (1<<23),
		CCII_COLLISION_INFO_VALUE		= (1<<24),
		CCII_ENFORCE_CLOSE_ATTACHMENT	= (1<<25),
		CCII_ENFORCE_CLOSE_FORCECOMPUTE = (1<<26),
		CCII_OVERLAP_STATE				= (1<<27),
		CCII_MOVE_FOR_REVISE			= (1<<28),
		CCII_HAS_NEW_POSITION			= (1<<29),
		CCII_HAS_NEW_VOLUME				= (1<<30),
		CCII_HAS_NEW_DIR				= (1<<31),
	};

	bool ReadFlag(const InternalInfo ccii) const { return (m_flags & ccii)? true : false; }
	void ClearFlag(const InternalInfo ccii) { m_flags &= ~ccii; }
	void RaiseFlag(const InternalInfo ccii) { m_flags |= ccii; }
	
	bool CheckCCFlagForAttachment(const NxActor& actor, bool* poutIsEnforce = 0);
	bool CheckCombinedFlagEnable(const NxActor& actor, const APhysXCCFlag flag) const;
	bool CheckFlagEnable(const NxActor& actor, const APhysXObjectFlag flag) const;
	
private:
	APhysXCharacterController& operator= (const APhysXCharacterController& rhs);
	bool Translation(const APhysXCCFlag ccFlag, APhysXObjectFlag& outObjFlag) const;

protected:
	NxVec3	m_SlideDir;

private:
	float   m_PushFactor;
	float	m_posFCThreshold;

	int		m_flags;
	NxVec3	m_UserGravity;
};

#endif