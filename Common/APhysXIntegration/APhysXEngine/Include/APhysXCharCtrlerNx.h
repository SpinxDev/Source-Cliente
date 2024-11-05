/*
* FILE: APhysXCharCtrlerNx.h
*
* DESCRIPTION: 
*	This just is a NxController wrapper. This is used as a performance standard to compare with others.
*
* CREATED BY: Yang Liu, 2010/12/31
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_NX_H_
#define _APHYSX_CHARACTER_CONTROLLER_NX_H_

class APHYSX_DLL_EXPORT APhysXNxCharCtrler : public APhysXHostCCBase
{
public:
	APhysXNxCharCtrler(NxController& controller, APhysXScene& scene, const APhysXCCDesc& desc);
	virtual ~APhysXNxCharCtrler() {};
	
	virtual void   SetDir(const NxVec3& vDir);
	virtual NxVec3 GetDir() const;
	virtual NxVec3 GetUpDir() const;
	virtual NxVec3 GetHorizonDir() const;

	virtual NxMat33 GetGlobalOrientation() const { return GetIDGlobalOrientation(); }

protected:
	enum InternalInfo
	{
		NCCII_ADDED_JUMP_VELOCITY		= (1<<0),
		NCCII_ADDED_JVEL_ABSOLUTE		= (1<<1),
		NCCII_DONE_CALL_SCENE_CHANGED	= (1<<2),
		NCCII_INCREASED_JUMP_VELOCITY   = (1<<3),
	};
	
	bool ReadFlag(const InternalInfo ccii) const { return (m_flagsInner & ccii)? true : false; }
	void ClearFlag(const InternalInfo ccii) { m_flagsInner &= ~ccii; }
	void RaiseFlag(const InternalInfo ccii) { m_flagsInner |= ccii; }

private:
	APhysXDynCharCtrler& operator= (const APhysXDynCharCtrler& rhs);
	
private:
	int		m_flagsInner;
	NxVec3  m_faceDir;
};

#endif