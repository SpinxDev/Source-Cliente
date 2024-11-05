/*
* FILE: APhysXCharCtrlerLightWeight.h
*
* DESCRIPTION: 
*	This is a light weight character controller. It was implemented the feature of revising vertical position. 
*   It also provide other limited features. This class was designed for non-main player("else player"), NPC...
*
*   Caveats/Limitations:
*		1. Only revise foot position at the direction of world-space Y axis(0, +/-1, 0).
*		2. By default, LWCC will be revised to the top surface of an object according to following rules:
*			1) search the top actor(in the detected range) that meet the property requirements. 
*			2) if there is no proper top actor, search the actor that was used to last revising and meet the
*			   property requirements. 
*			property requirements:
*			   The actor is static actor or kinematic actor or dynamic actor that also is a APhysXObject with
*			   enable attachment property.
*		3. If raised flag APX_LWCCF_JUMPING_INAIR, only detect in the volume of LWCC. Further more, the last 
*		   revising actor will not be count in if nothing detected.
*
* CREATED BY: Yang Liu, 2010/02/10
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_LIGHTWEIGHT_H_
#define _APHYSX_CHARACTER_CONTROLLER_LIGHTWEIGHT_H_

class APHYSX_DLL_EXPORT APhysXLWCharCtrler : public APhysXCCBase
{
public:
	enum APhysXLWCCFlag
	{
		// Default: off
		// Enforce enable revising position. Ignore the property requirements(reference details at the top of this file).
		APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN	= (1<<0),

		// Indicate that LWCC is in air. Only detect in the volume of LWCC if raise this flag.
		// User should take care this flag before TickReviseVerticalPos()/APhysXCharacterControllerManager.Tick();
		APX_LWCCF_JUMPING_INAIR					= (1<<1),
	};

public:
	APhysXLWCharCtrler(APhysXScene& scene, const int flags)
		: APhysXCCBase(scene) { m_flags = flags; }
	virtual ~APhysXLWCharCtrler() {};

	bool ReadFlag(const APhysXLWCCFlag flag) const;
	void ClearFlag(const APhysXLWCCFlag flag);
	void RaiseFlag(const APhysXLWCCFlag flag);
	
	virtual NxMat33 GetGlobalOrientation() const { return GetIDGlobalOrientation(); } 
	virtual NxVec3 GetGroundNormal(bool* outpbIsDefVal = 0, const NxVec3& defVal = NxVec3(0, 1, 0)) const;

	/*
	Return true if the position was revised or return false. 
	*/
	virtual bool TickReviseVerticalPos() = 0;

private:
	friend class APhysXCharCtrlerMgrImp;
	APhysXLWCharCtrler& operator= (const APhysXLWCharCtrler& rhs);
	void OnObjectDeleted(APhysXObject& obj) { APhysXCCBase::OnObjectDeleted(obj); }

private:
	int	m_flags;
};

#endif