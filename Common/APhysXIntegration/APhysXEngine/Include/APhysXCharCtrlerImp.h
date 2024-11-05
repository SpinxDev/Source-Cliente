/*
* FILE: APhysXCharCtrlerImp.h
*
* DESCRIPTION:
*   Define the common cc channel manager. 
*	Also provide the actual definition of APhysXLWCharCtrler, APhysXCharacterController, APhysXDynCharCtrler and APhysXNxCharCtrler.
*
*
* CREATED BY: Yang Liu, 2009/12/11
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_IMP_H_
#define _APHYSX_CHARACTER_CONTROLLER_IMP_H_

#include "APhysXCharCtrler.h"
#include "APhysXCharCtrlerNx.h"
#include "APhysXCharCtrlerDynamic.h"
#include "APhysXCharCtrlerDynConfig.h"
#include "APhysXCharCtrlerAssistance.h"
#include "..\..\APhysXNxCharacter_Public\include\NxExtended.h"

class APhysXCCChannelMgr
{
public:
	APhysXCCChannelMgr(APhysXCharCtrlerMgrImp& ccMgrImp, APhysXScene& scene);
	~APhysXCCChannelMgr();

	APhysXCharCtrlerMgrImp& GetCCManager() const { return m_CCMgrImp; }
	APhysXScene& GetAPhysXScene() const { return m_APhysXScene; }
	
	NxU32 GetActiveGroups() const { return m_ActiveGroups; }
	void OpenGroup(const APhysXCollisionGroup& group);
	void CloseGroup(const APhysXCollisionGroup& group);

	bool RegisterHostModel(APhysXObject* pObject, APhysXCollisionChannel& outNewCNL);
	bool UnRegisterHostModel(APhysXObject* pObject, APhysXCollisionChannel& outOriginalCNL);

	NxGroupsMask GetFilterGM(const bool bEnforceCombineCCMRegGM = false) const;

private:
	void ApplyHostModelChannel();

private:
	struct CollisionInfo 
	{
		CollisionInfo();
		bool PrepareSetting(APhysXCollisionChannel& outNewCNL);
		APhysXCollisionChannel RecoverSetting();

		APhysXObject* pObj;
		bool	bHasOneWayBK;
		bool	bHasChannelBK;
		bool	bHasCollisionGroupBK;
		int		bkOneWayValue;
		int		bkChannelValue;
		int		bkCollisionGroupValue;
	};

	struct CountableCNL
	{
		CountableCNL() { m_Channel = APX_COLLISION_CHANNEL_INVALID; m_nCount = 0; }
		APhysXCollisionChannel m_Channel;
		int m_nCount;
	};

private:
	APhysXCharCtrlerMgrImp& m_CCMgrImp;
	APhysXScene&	m_APhysXScene;
	NxScene&		m_Scene;
	
	NxU32	m_ActiveGroups;
	NxGroupsMask	m_gmUnionSetHostSkip;

	APhysXArray<CountableCNL>	m_HostModelCNLs;
	APhysXArray<CollisionInfo>	m_HostModelCIBackup;
};

class APhysXLWCCImp : public APhysXLWCharCtrler
{
public:
	APhysXLWCCImp(const APhysXLWCCDesc& desc, NxActor& actor, APhysXScene& scene);
	~APhysXLWCCImp();

	virtual NxVec3 GetVolume(APhysXVolumeType& outVT) const;
	virtual bool ChangeVolume(const NxVec3& newVolume);
	
	virtual NxVec3 GetCenterPosition() const { return m_CurrentCenterPos; }
	virtual NxVec3 GetFootPosition() const;
	virtual void   SetFootPosition(const NxVec3& pos);

	virtual bool IsComponent(const NxActor& actor) const;
	virtual NxActor* GetHiddenActor() const { return m_pKActor; }
	virtual bool GetGroundPos(float sweepDist, NxVec3& pos, NxVec3* poutNormal) const;
	virtual bool TickReviseVerticalPos();

	void ReleaseNxActor();

private:
	bool OverlapTest();

public:
	static const float gExpFactor;

private:
	NxActor* m_pKActor;

	float m_Radius;
	float m_Height;
	NxVec3 m_CurrentCenterPos;

	EntityReport m_OverlapReport;
	EntityReportSweep m_SweepReport;
};

class APhysXCCImp : public APhysXCharacterController
{
public:
	APhysXCCImp(const APhysXCCDesc& desc, const float skinwidth, NxController& controller, APhysXScene& scene);
	~APhysXCCImp();

	NxController& GetNxController() const { return *m_pController; }

	void SetBlockShape(NxShape& shape);
	void ComputeHitForce(const NxVec3& moveDir, const NxVec3& hitPos, NxActor& hitActor, bool& outIsPushing);
	void UpdateAttachment(const NxVec3& moveDir, const NxVec3& hitNormal, const NxVec3& hitPos, NxActor& hitActor);
	
	void ReportSceneChanged();
	void OnObjectDeleted(APhysXObject& obj);
	void TerrainChange(bool IsLoading, const NxBounds3& TerrainAABB, const APhysXScene& Scene) { APhysXCCBase::TerrainChange(IsLoading, TerrainAABB, Scene); }

public:
	virtual NxVec3 GetFootPosition() const { return GetFootPositionInternal(false); }
	virtual NxVec3 GetCenterPosition() const { return GetCenterPositionInternal(false); }
	virtual void   SetFootPosition(const NxVec3& pos);

	virtual void   SetDir(const NxVec3& vDir);
	virtual NxVec3 GetDir() const;
	virtual NxVec3 GetUpDir() const;
	virtual NxVec3 GetHorizonDir() const;

	virtual void  SetStepOffset(const float offset); 
	virtual float GetStepOffset() const;

	virtual NxVec3 GetVolume(APhysXVolumeType& outVT) const;
	virtual bool ChangeVolume(const NxVec3& newVolume);

	virtual NxU32 MoveCC(const float deltaSecond, const NxVec3& MoveDir);
	virtual void  JumpCC(float fJumpingSpeed, bool bIsAbsoluteSpeed = false);
	
	virtual bool GetAttachmentPos(NxVec3& out) const;
	virtual bool GetGroundPos(float sweepDist, NxVec3& pos, NxVec3* poutNormal) const;

	virtual NxVec3 GetInstantVelocity(bool bIsAbsVel = false) const;
	virtual bool IsComponent(const NxActor& actor) const;
	virtual NxActor* GetHiddenActor() const;

private:
	NxVec3 GetFootPositionInternal(const bool actualNxPhysXPos = false) const;
	NxVec3 GetCenterPositionInternal(const bool actualNxPhysXPos = false) const;
	float  GetHalfHeight(const bool actualNxPhysXSize = false) const;

	void UpdateStatus();
	void UpdateDir();
	bool ChangeBoxVolume(const NxVec3& dim);
	bool ChangeCapsuleVolume(const float radius, const float height);
	void ApplyNewSetting();

	bool CheckOverlap(bool EnableProcessOLInAir);
	bool CheckOverlapBox(const NxVec3& centerPos, const NxVec3& dimExtents, bool CollectShapes = false);
	bool CheckOverlapCapsule(const NxVec3& centerPos, const float radius, const float height, bool CollectShapes = false);
	bool ProcessOverlapInAir(bool& outIsOverlap);

	void SubmitFrameInfo();
	void PrepareFrameInfo(const float deltaSecond);
	void RevisePosAndAttachmentRoutine(NxU32& cfs, NxU32& cfsAll);
	void UpdateInteract();
	void UpdateVelocity(const NxVec3& MoveDir);

	void UpdateAttachCandidate(FrameInfo& ToUpdate);
	bool GetSlideDir(const NxVec3& normal, const NxVec3& g, NxVec3& outSlideDir);

	NxVec3 GetRayStartPos(float& outOffsetBaseFootPos) const;
	NxU32  MoveController(const NxVec3& disp, FrameInfo* pToFilterIt = 0);
	NxU32  DoReviseWork(const NxVec3& disp);
	NxU32  DoReviseSurface(const NxVec3& disp);

	NxVec3 GetDispForMove();
	NxVec3 GetDispFromFormula(const NxVec3& v0) const;
	NxVec3 GetDispReviseForForecase();
	NxVec3 GetDispReviseForOLInAir();
	bool ReviseVerticalDispForWalking(NxVec3& S) const;

	void ComputeJumpingTakeOffForce();
	bool SweepClosestAction(const NxVec3& startPos, const NxVec3& motion, NxSweepQueryHit& outSQHit, float* pR = 0) const;
	bool ContinuesPushingDetection(const NxVec3& MoveDir);

private:
	NxController* m_pController;
	
	float  m_SlopeUpRadian;
	float  m_SlopeDownRadian;
	// It seems there is no way to get dynamic skinwidth from NxController during the runtime.
	// So we have to save it by ourselves.
	float  m_SkinWidth;

	float  m_JumpSpeed;
	NxVec3 m_faceDir;
	NxVec3 m_faceDirNew;

	NxVec3	m_newVolume;
	NxExtendedVec3 m_newPos;
	NxVec3 m_LastPos;

	FrameInfo* m_pLastFrame;
	FrameInfo* m_pCurrentFrame;
	FrameInfo* m_pActiveForCallBack;
	FrameInfo* m_pLastForCallBack;

	APhysXArray<NxShape*> m_pBlocks;
	EntityReport m_ToFilterOutShapes;
};

class APhysXDynCCImp : public APhysXDynCharCtrler
{
public:
	APhysXDynCCImp(const APhysXCCCapsuleDesc& desc, NxActor& actor, APhysXScene& scene);
	~APhysXDynCCImp();
	
	void ReleaseNxObjects();
	void OnObjectDeleted(APhysXObject& obj);
	void TerrainChange(bool IsLoading, const NxBounds3& TerrainAABB, const APhysXScene& Scene) { APhysXCCBase::TerrainChange(IsLoading, TerrainAABB, Scene); }

public:
	virtual NxVec3 GetCenterPosition() const;
	virtual NxVec3 GetFootPosition() const;
	virtual void   SetFootPosition(const NxVec3& pos);
	virtual NxMat33 GetGlobalOrientation() const;

	virtual void   SetDir(const NxVec3& vDir);
	virtual NxVec3 GetDir() const;
	virtual NxVec3 GetUpDir() const;
	virtual NxVec3 GetHorizonDir() const;

	virtual void  SetStepOffset(const float offset); 
	virtual float GetStepOffset() const { return m_AutoStepUp; }

	virtual NxVec3 GetVolume(APhysXVolumeType& outVT) const;
	virtual bool ChangeVolume(const NxVec3& newVolume);
	
	virtual NxU32 MoveCC(const float deltaSecond, const NxVec3& MoveDir);
	virtual void  JumpCC(float fJumpingSpeed, bool bIsAbsoluteSpeed = false);
	virtual void RefeshCCBySimResult();
	
	virtual bool GetAttachmentPos(NxVec3& out) const;
	virtual bool GetGroundPos(float sweepDist, NxVec3& pos, NxVec3* poutNormal) const;

	virtual NxVec3 GetInstantVelocity(bool bIsAbsVel = false) const;
	virtual bool IsComponent(const NxActor& actor) const;
	virtual NxActor* GetHiddenActor() const;

	virtual bool HangStart(const NxVec3& hangDir);
	virtual void HangEnd();
	virtual	bool GetHangGlobalAnchor0(NxVec3& outAnchor) const;

private:
	bool IsDynDirUsing() const;
	bool CheckSlideAction(NxVec3* poutSlideDir = 0); 
	bool AttachmentIsValid(bool* poutJointIsBroken = 0) const;
	NxVec3 GetAttachedPointVelocity(const NxVec3& rtnDefaultValue);
	bool CheckOverlapCapsule(const NxVec3& centerPos, const float radius, const float height);
	NxGroupsMask GetFilterGMExcludeBody(bool bEnforceCombineCCMRegGM = true) const;
	
	void ClearVerticalVelocity(const NxVec3& gDir, const NxVec3& velAttachment);
	bool RevisePosition(const float skinWidth, const NxVec3& dirDetect, const NxSweepQueryHit& result, const float lenMotion);
	bool RevisePositionToAvoidOverlap(const float skinWidth, const NxVec3& dirDetect, const NxSweepQueryHit& result, const float lenMotion);

	void UpdateDir();
	void UpdateStatus(bool bIsHitSomething);
	void UpdateAttachment(AttachedActor* pNewAttachment, bool bEnableChangeSType = true);
	void UpdateJointSetting(const float deltaSecond);
	void UpdateDominanceSetting();

	void CreateD6JointSetting(NxActor* pActor0);

	void ComputeTargetVelocityForJump(const NxVec3& velMove, const float deltaSecond);
	void ComputeTargetVelocityForWalk(const NxVec3& velMove, const float deltaSecond);
	void ComputeJumpTakeoffForce(const NxVec3& oldVel, const NxVec3& newVel, const NxVec3& g, const float timeSec);
	
	bool AutoStepPrecondition(const	NxSweepQueryHit& sweepHit, const NxVec3& gDir, const NxVec3& hvelMoveDir, const float hvelSpeed,
							  const NxU32 activeGroups, const NxGroupsMask& gmFilter, const NxCapsuleShape& capShape);
	void AutoStep(const NxVec3& velOriMove, const NxVec3& velMove, const float deltaSecond);
	
	void SubMoveCCForNotHangMode(const NxVec3& MoveDir, const float deltaSecond);
	void SubMoveCCForHangMode(const NxVec3& MoveDir, const float deltaSecond);
	void HangMoveControl(const NxVec3& MoveDir, const float deltaSecond, const NxVec3& dirHangRope);
	void HangEndImplement(const float deltaSecond, const bool bEnforceJump = false);

	virtual void OnPutToSleep();
	virtual void OnWakeUp();
	virtual void OnSetAutoHangEndAngle(const float radian);

public:
	static NxU32 gFrozenFlags;

private:
	APhysXDynCCCoreData m_coreData;
	NxActor* m_pDActor;
	
	float m_Radius;
	float m_Height;

	const NxActor* m_pSupport;
	AttachedActor* m_pAttachment;
	AttachedActor m_AAInfo;
	NxVec3 m_AttachPosLocalNormal;

	float  m_SlopeUpRadian;
	float  m_SlideThreshold;
	float  m_SlopeDownRadian;

	float  m_AutoStepUp;
	float  m_maxHangDistEnable;

	int m_CooldownForAddForce;
	int m_CooldownForMoveUp;
	int m_CooldownForMoveDown;
	
	float m_JumpSpeed;
	NxVec3 m_faceDir;
	NxVec3 m_LastPos;

	NxVec3 m_AngularVelSleepBackup;
	NxVec3 m_LinearVelSleepBackup;
	NxD6JointDesc m_D6JSleepBackup;
	ISituationConfigBase::Situation m_StateSleepBackup;
	ISituationConfigBase::Situation m_CurrentSituation;

	EntityReportSweepForDynCC m_SweepReport;
};

class APhysXNxCCImp : public APhysXNxCharCtrler
{
public:
	APhysXNxCCImp(const APhysXCCDesc& desc, const float skinwidth, NxController& controller, APhysXScene& scene);

	NxController& GetNxController() const { return *m_pController; }
	void ReportSceneChanged();
	void OnObjectDeleted(APhysXObject& obj) { APhysXCCBase::OnObjectDeleted(obj); }
	void TerrainChange(bool IsLoading, const NxBounds3& TerrainAABB, const APhysXScene& Scene) { APhysXCCBase::TerrainChange(IsLoading, TerrainAABB, Scene); }

public:
	virtual NxVec3 GetFootPosition() const { return GetFootPositionInternal(false); }
	virtual NxVec3 GetCenterPosition() const { return GetCenterPositionInternal(false); }
	virtual void   SetFootPosition(const NxVec3& pos);
	
	virtual void  SetStepOffset(const float offset); 
	virtual float GetStepOffset() const;
	
	virtual NxVec3 GetVolume(APhysXVolumeType& outVT) const;
	virtual bool ChangeVolume(const NxVec3& newVolume);
	
	virtual NxU32 MoveCC(const float deltaSecond, const NxVec3& MoveDir);
	virtual void  JumpCC(float fJumpingSpeed, bool bIsAbsoluteSpeed = false);
	
	virtual bool GetAttachmentPos(NxVec3& out) const { return false; }
	virtual bool GetGroundPos(float sweepDist, NxVec3& pos, NxVec3* poutNormal) const;

	virtual NxVec3 GetInstantVelocity(bool bIsAbsVel = false) const { return m_velCC; }
	virtual bool IsComponent(const NxActor& actor) const;
	virtual NxActor* GetHiddenActor() const;

private:
	NxVec3 GetFootPositionInternal(const bool actualNxPhysXPos = false) const;
	NxVec3 GetCenterPositionInternal(const bool actualNxPhysXPos = false) const;
	float  GetHalfHeight() const;

	void PreUpdateVelocity(const NxVec3& g, const NxVec3& MoveDir, const float deltaSecond);
	void PostUpdateVelocity(const NxVec3& g, const NxU32 CollisionFlags, int& inoutNewStatus);
	void UpdateState(int& newStatus);

	bool SweepClosestAction(const NxVec3& startPos, const NxVec3& motion, NxSweepQueryHit& outSQHit) const;

private:
	NxController* m_pController;
	
	// It seems there is no way to get dynamic skinwidth from NxController during the runtime.
	// So we have to save it by ourselves.
	float  m_SkinWidth;
	
	float m_JumpSpeed;
	NxVec3  m_velCC;
};

#endif