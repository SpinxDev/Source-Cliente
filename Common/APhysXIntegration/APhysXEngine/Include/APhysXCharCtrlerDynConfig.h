/*
* FILE: APhysXCharCtrlerDynConfig.h
*
* DESCRIPTION: 
*	Provide joints management and actor setting for APhysXDynCharCtrler. 
*
* CREATED BY: Yang Liu, 2010/09/03
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_DYNAMIC_CONFIG_H_
#define _APHYSX_CHARACTER_CONTROLLER_DYNAMIC_CONFIG_H_

class APhysXDynCCCoreData;

class ISituationConfigBase
{
public:
	enum Situation
	{
		DYNCC_S_INIT_DEFAULT	  = 0,
		DYNCC_S_STAND_NOT_BIND	  = 1,
		DYNCC_S_STAND_AND_BIND	  = 2,
		DYNCC_S_CTRLMOVE_NOT_BIND = 3,
		DYNCC_S_CTRLMOVE_AND_BIND = 4,
		DYNCC_S_SLIDE_FREE		  = 5,
		DYNCC_S_SLIDE_CTRL		  = 6,
		DYNCC_S_JUMP_START		  = 7,
		DYNCC_S_JUMP_OR_INAIR	  = 8,
		DYNCC_S_HANG_START		  = 9,
		DYNCC_S_HANGING			  = 10,
		DYNCC_S_HANG_END		  = 11,
		DYNCC_S_HANG_END_JUMPING  = 12,
		DYNCC_S_HANG_POST		  = 13,
		DYNCC_COUNTER_BOUND		  = 14,
	};

public:
	static bool EqualJudgement(const Situation a, const Situation b);

public:
	static ISituationConfigBase* GetSituationConfigObject(const ISituationConfigBase::Situation s);

	virtual Situation GetSituation() const = 0;
	virtual void Configuration(APhysXDynCCCoreData& coreData) = 0;
};

class  APhysXDynCCCoreData
{
public:
	struct JointInfo
	{
		NxActor* pActor0;
		NxVec3 globalAnchor;
	};

public:
	APhysXDynCCCoreData(APhysXScene& scene, NxActor& actor);

	ISituationConfigBase::Situation GetAppliedSituation() const;
	void SetAppliedSituation(const ISituationConfigBase::Situation s) { m_appliedSituation = s; }

	float GetActorMass() const;
	float GetCurrentHangRopeDistance() const;
	float GetCurrentHangJointDistance() const;  // Joint Distance maybe different from Rope Distance due to the spring and damping.
	bool  GetHangGlobalAnchor0(NxVec3& outAnchor) const;

	bool IsBrokenD6joint() const;
	bool IsDistanceJointAvailable(bool& outIsBroken) const;
	bool IsNotHangAndPossibleInAir() const;
	bool AppliedSituationBelongToHangEnd() const;
	void SetHangDistanceJoint(NxDistanceJoint* pJoint, bool bIsSafeMode);

	bool ComputeHangRopeDirAndDist(NxVec3* poutDir = 0);
	bool ComputeActorStandPose();

	bool IsAutoHangEndToDo() const { return ReadFlag(TUA_FLAG_AUTO_HANG_END); }
	float GetAutoHangEndThreshold() const { return m_autoHangEndT; }
	void SetAutoHangEndThreshold(const float value) { m_autoHangEndT = value; }

	void ClearAction() { RaiseFlag(TUA_DEFAULT_EMPTY); }
	void SetNewDominance(const int value);
	void ResetIDOrientation();
	void ResetActorIDOrientationImmediately();
	void SetNewKinamatic(const bool bIsToRaise);
	void SetNewFrozenRot(const bool bIsToRaise);

	void SetNewLinearVel(const NxVec3& value);
	void SetNewLinearVelOwner(const bool bIsToActor);
	void ClearUnknownOwnerNewLinearVel();
	void SetNewAngularVel(const NxVec3& value);
	void SetNewPosition(const NxVec3& value);
	void SetNewForce(const NxVec3& value);
	void ApplyAllSettings();

	void UpdateOrientationForHang();
	void UpdateD6JointDescForHang(const NxVec3& dirHangFaceTo, const float mass);
	void UpdateD6JointDescForActorAndFrameOnly(JointInfo* pInfo, const NxVec3& globalAxis, const bool bIsActor1IDPose = false);

	void ReleaseD6JointSetting() { RaiseFlag(TUA_RELEASE_D6JOINT); }
	void ReleaseDistJointSetting() { RaiseFlag(TUA_RELEASE_DISTTANT_JOINT); }
	void ReleaseActorSetting() { RaiseFlag(TUA_RELEASE_ACTOR); }

	bool IsD6JointDescChanged() const { return ReadFlag(TUA_D6JOINT_DESC_CHANGED); }
	NxD6JointDesc& CreateD6JointDesc();
	NxD6JointDesc& UpdateD6JointDesc();
	NxDistanceJointDesc& GetDistJointDesc(bool& outIsValid);
	void UpdateDistJointDesc() { RaiseFlag(TUA_RELOAD_DJJOINT_DESC); }

private:
	APhysXDynCCCoreData& operator= (const APhysXDynCCCoreData& rhs);

private:
	enum ToUpdateAction
	{
		TUA_DEFAULT_EMPTY			= 0,
		TUA_DOMINANCE				= (1<<0),
		TUA_D6JOINT_ORIENTATION		= (1<<1),
		TUA_FLAG_KINEMATIC			= (1<<2),
		TUA_FLAG_FROZEN_ROTATION	= (1<<3),
		TUA_DEBUG_LINEAR_VELOCITY	= (1<<4),
		TUA_ACTOR_LINEAR_VELOCITY	= (1<<5),
		TUA_JOINT_LINEAR_VELOCITY	= (1<<6),
		TUA_ACTOR_ANGULAR_VELOCITY	= (1<<7),
		TUA_RELOAD_D6JOINT_DESC		= (1<<8),
		TUA_CREATE_D6JOINT_DESC		= (1<<9),
		TUA_RELEASE_D6JOINT			= (1<<10),
		TUA_RELEASE_DISTTANT_JOINT	= (1<<11),
		TUA_RELEASE_ACTOR			= (1<<12),
		TUA_RELOAD_DJJOINT_DESC		= (1<<13),
		TUA_ACTOR_GLOBAL_POSITION	= (1<<14),
		TUA_ACTOR_ADD_FORCE			= (1<<15),

		TUA_FLAG_AUTO_HANG_END		= (1<<30),
		TUA_D6JOINT_DESC_CHANGED	= (1<<31),
	};

	bool ReadFlag(const ToUpdateAction flag) const;
	void ClearFlag(const ToUpdateAction flag);
	void RaiseFlag(const ToUpdateAction flag);

	void OnApplySettingsRelease(NxScene& scene);
	void OnApplySettingsCreateAndReload(NxScene& scene);
	void OnApplySettingsAttribution();
	
private:
	APhysXScene*		m_pAPhysXScene;
	NxActor*			m_pDActor;
	NxD6Joint*			m_pjntD6Ctrl;
	NxDistanceJoint*	m_pjntDistHang;
	int					m_flags;

	ISituationConfigBase::Situation m_appliedSituation;
	
	int		m_ToApplyDominance;
	NxQuat	m_ToApplyPose;
	bool	m_ToApplyKinamatic;
	bool	m_ToApplyFrozenRot;

	NxVec3	m_ToApplyLinearVel;
	NxVec3	m_ToApplyAngularVel;
	NxVec3	m_ToApplyPosition;
	NxVec3	m_ToApplyForce;

	NxD6JointDesc m_ToApplyDescD6;
	NxDistanceJointDesc m_ToApplyDescDJ;

	float	m_autoHangEndT;
	float	m_distHangRope;
	NxVec3	m_dirHangRope;
	NxVec3	m_dirHangFaceTo;
};

class SCStandNotBind : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_STAND_NOT_BIND; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCStandAndBind : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_STAND_AND_BIND; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCCtrlMoveNotBind : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_CTRLMOVE_NOT_BIND; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCCtrlMoveAndBind : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_CTRLMOVE_AND_BIND; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCSlideFree : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_SLIDE_FREE; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCSlideCtrl : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_SLIDE_CTRL; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCJumpStart : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_JUMP_START; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCJumpOrInair : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_JUMP_OR_INAIR; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCHangStart : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_HANG_START; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCHanging : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_HANGING; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCHangEnd : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_HANG_END; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCHangEndJumping : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_HANG_END_JUMPING; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

class SCHangPost : public ISituationConfigBase
{
public:
	virtual Situation GetSituation() const { return DYNCC_S_HANG_POST; };
	virtual void Configuration(APhysXDynCCCoreData& coreData);
};

#endif