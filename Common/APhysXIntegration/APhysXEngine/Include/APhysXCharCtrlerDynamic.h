/*
* FILE: APhysXCharCtrlerDynamic.h
*
* DESCRIPTION: 
*	This a dynamic actor based character controller. 
*
* CREATED BY: Yang Liu, 2010/07/12
*
* Note: not support hit report yet
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_DYNAMIC_H_
#define _APHYSX_CHARACTER_CONTROLLER_DYNAMIC_H_

class APHYSX_DLL_EXPORT APhysXDynCharCtrler : public APhysXHostCCBase
{
public:
	enum APhysXCCFlag
	{
		// Default: on
		// Enable slide movement if raised. When the CC is standing on a ramp and the ramp's slope-degree is 
		// greater than APhysXCCDesc.slopeUpAngleLimit, you can control the cc to slide down or not by this flag.  
		// See also APhysXCCDesc.slopeUpAngleLimit.
		APX_CCF_SLIDE_ENABLE			= (1<<0),

		// Default: on
		// Enable auto hang end if the hand angle is greater than the threshold value. 
		APX_CCF_AUTO_HANG_END_ENABLE	= (1<<1),

		// Default: off
		// Turn on "passive effect" if raised. Passive Effect means CC can't affect other environment objects.
		// No standing force, no push force are applied to the touching objects. CC can be only affected by other environment objects.    
		APX_CCF_PASSIVE_EFFECT_ONLY		= (1<<2),

		// Default: off
		// Enforce enable attach to dynamic(or kinematic) actor if using this flag.
		// The priority of this flag is higher than the related APhysXObject's flag. See also APhysXObjectFlag.
		APX_CCF_ATTACHMENT_ENFORCE_OPEN	= (1<<3),

		// Default: off
		// Enforce disable attach to dynamic(or kinematic) actor if using this flag.
		// The priority of this flag is higher than APX_CCF_ATTACHMENT_ENFORCE_OPEN and higher than the related APhysXObject's flag.
		// See also APhysXObjectFlag.
		APX_CCF_ATTACHMENT_ENFORCE_CLOSE	= (1<<4),
	};
	

public:
	APhysXDynCharCtrler(APhysXScene& scene, const APhysXCCDesc& desc);
	virtual ~APhysXDynCharCtrler() {};

	bool ReadFlag(const APhysXCCFlag flag) const;
	void ClearFlag(const APhysXCCFlag flag);
	void RaiseFlag(const APhysXCCFlag flag);

	/*
	Default: 70
	Unit: Degrees
	Note: If the angle > 180 the applied value is angle mod 180.

	|\
	|  \
	|    \
	|      \
	|        \
	|          \
	| the angle  \
	*/
	float GetMaxHangAngle() const { return m_maxHangAngle; };
	void  SetMaxHangAngle(const float angle);
	
	/*
	Default: 120
	Unit: Degrees
	Note: If the angle > 180 the applied value is angle mod 180. If the angle < GetMaxHangAngle() the applied value is the max hang angle.
		  

	|\
	|  \
	|    \
	|      \
	|        \
	|          \
	| the angle  \
	*/
	float GetAutoHangEndAngle() const { return m_endHangAngle; };
	void  SetAutoHangEndAngle(const float angle);
	
	virtual void RefeshCCBySimResult() = 0;

	virtual bool HangStart(const NxVec3& hangDir) = 0;
	virtual void HangEnd() = 0;
	virtual	bool GetHangGlobalAnchor0(NxVec3& outAnchor) const = 0;

protected:
	enum InternalInfo
	{
		DCCII_ADDED_JUMP_VELOCITY	= (1<<0),
		DCCII_ADDED_JVEL_ABSOLUTE	= (1<<1),
		DCCII_ON_THE_EDGE			= (1<<2),
		DCCII_AUTO_STEP_UP_ENABLE	= (1<<3),
		DCCII_HANG_MODE				= (1<<4),	
		DCCII_HANG_START			= (1<<5),
		DCCII_HANG_END				= (1<<6),
		DCCII_AUTO_STEP_IS_WORKING	= (1<<7),
		DCCII_AUTO_STEP_WORKED_LAST	= (1<<8),
		DCCII_HAS_NEW_POSITION		= (1<<9),
		DCCII_HAS_NEW_VELOCITY		= (1<<10),
	};
	
	bool ReadFlag(const InternalInfo ccii) const { return (m_flagsInner & ccii)? true : false; }
	void ClearFlag(const InternalInfo ccii) { m_flagsInner &= ~ccii; }
	void RaiseFlag(const InternalInfo ccii) { m_flagsInner |= ccii; }

	bool CheckCCFlagForAttachment(const NxActor& actor, bool* poutIsEnforce = 0);
	bool CheckFlagEnable(const NxActor& actor, const APhysXObjectFlag flag) const;

	float GetMaxHangDistanceLimit() const { return m_maxHangDist; }
	float GetMinHangDistanceLimit() const { return m_minHangDist; }
	float GetMaxHangLinearVelocitySquared(const float fHangRadius, const float cosVal, const float gVal) const;

	void SetNewPos(const NxVec3& value);
	void SetNewVel(const NxVec3& value);
	void ApplyNewPosAndVel(NxActor& actor);

private:
	virtual void OnSetAutoHangEndAngle(const float radian) = 0;

private:
	APhysXDynCharCtrler& operator= (const APhysXDynCharCtrler& rhs);
	
private:
	float   m_maxHangDist;
	float   m_minHangDist;

	float	m_maxHangAngle;
	float   m_maxHangRadian;

	float	m_endHangAngle;
	float   m_endHangRadian;
	
	int		m_flags;
	int		m_flagsInner;

	NxVec3	m_newPos;
	NxVec3	m_newVel;
};

#endif