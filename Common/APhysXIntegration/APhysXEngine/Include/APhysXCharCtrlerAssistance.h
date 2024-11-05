/*
* FILE: APhysXCharCtrlerAssistance.h
*
* DESCRIPTION: 
*	Definition a few classes that are used in APhysXCCImp class.
*
*   Caveats/Limitations:
*		1. We assume that the attached actor has only one shape.
*		2. We assume that the attached shape are NOT released during the whole binding period.
*          (Contrarily, the attached actor can be released)
*
*
* CREATED BY: Yang Liu, 2009/11/24
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_ASSISTANCE_H_
#define _APHYSX_CHARACTER_CONTROLLER_ASSISTANCE_H_

#include  <NxUserEntityReport.h>

float GetCosValue(const NxVec3& v0, const NxVec3& v1);
NxVec3 GetVerticalSubValue(const NxVec3& val, const NxVec3& accG);

// see the "Caveats/Limitations" at the top of this file
struct AttachedActor
{
	AttachedActor() { SetToInvalid(); }
	void SetToInvalid();
	
	bool IsValid() const { return (0 == pActor)? false : true; }
	bool IsDeleted(const APhysXObject& obj, NxVec3* pOutDelVel = 0);
	void UpdateInfo(const NxVec3& attachedPos, const NxVec3& vDir, const NxActor& actor);
	void UpdateInfo(const NxVec3& CurrentFootPos, const AttachedActor& LastAA);
	
	const NxActor* pActor;
	NxVec3 localPos;
	NxVec3 localDir;
	
	NxMat34 pose;       // pose of attached actor in global frame
	NxVec3 globalPos;   // position of attached point in global frame
	NxVec3 velPT;       // velocity of attached point
};

struct ForcePoint
{
	ForcePoint() { pActor = 0; pos.zero(); force.zero(); }
	ForcePoint(NxActor& actor, const NxVec3& Pos) { pActor = &actor; pos = Pos; force.zero(); }
	ForcePoint(NxActor& actor, const NxVec3& Pos, const NxVec3& Force) { pActor = &actor; pos = Pos; force = Force; }
	NxActor* pActor;
	NxVec3 pos;
	NxVec3 force;
};

class ForceRecorder
{
public:
	enum Mode
	{
		FRM_AVERAGE = 1,
		FRM_GETMAX  = 2,
	};
	enum ForceType
	{
		FRT_FORCE	= 1,
		FRT_IMPULSE = 2,
		FRT_SMOOTH_IMPULSE = 3,
	};

	ForceRecorder(Mode m, ForceType t = FRT_FORCE) { m_AddMode = m; m_FType = t; m_TFIsValid = false; }
	void Add(const ForcePoint& newFP);
	void AddToConstant(const ForcePoint& newFP);
	void Apply(APhysXScene& aScene, float fTickSec, const NxActor* pBindActor = 0, const NxVec3* pBindPos = 0);

	bool TotalForceIsValid() const { return m_TFIsValid; }
	void SetTotalForce(const NxVec3& val) { m_TotalForce = val; m_TFIsValid = true; }

private:
	void ApplyForce(APhysXScene& aScene, NxActor& actor, const NxVec3& force, const NxVec3& pos);

private:
	Mode m_AddMode;
	ForceType m_FType;

	bool m_TFIsValid;
	NxVec3 m_TotalForce;

	APhysXArray<ForcePoint> m_ForceInfo;
	ForcePoint m_ForceConstant;
};

class FrameInfo
{
public:
	enum StatusInfo
	{
		IS_IN_AIR					= (1<<0),
		IS_JUMPING_TAKEOFF			= (1<<1),
		IS_JUMPING_LANDING			= (1<<2),
		INCREASED_JUMPING_VELOCITY	= (1<<3),
		ATTACHMENT_IS_DELETED		= (1<<4),
		IS_PUSHING					= (1<<5),
	};

public:
	FrameInfo();
	void SetToInvalid();
	void SetAAToInvalid();
	void PutToSleep();
	
	bool GetStatus(const StatusInfo flag) const;
	void ClearStatus(const StatusInfo flag);
	void RaiseStatus(const StatusInfo flag);

	void CheckActorIsDeleted(const APhysXObject& obj);
	bool GetAttachedPoint(NxVec3& outPT) const;
	bool GetAttachedDir(NxVec3& outDir) const;

	void UpdateCandidateInfo(const AttachedActor& LastAA, const NxVec3& CurrentFootPos, const bool EnableAttach);
	void UpdateCandidateInfo(const NxActor& actor, const NxVec3& attachedPos, const NxVec3& faceDir, const bool EnableAttach);

	void UpdateStateByFlags(const NxU32 CollisionFlags);
	void UpdateStateByFrame(const FrameInfo& FromLastFrame);

	void  SetTime(const float sec) { timeSec = sec; }
	float GetTime() const { return timeSec; }

	void  SetHitCosVal(const float val) { HitCosVal = val; }
	float GetHitCosVal() const { return HitCosVal; }

	void   SetInputMoveDir(const NxVec3& dir) { InputMoveDir = dir; }
	NxVec3 GetInputMoveDir() const { return InputMoveDir; }

	void   SetGravity(const NxVec3& g) { gravity = g; }
	NxVec3 GetGravity() const { return gravity; }

	NxVec3 GetVel() const { return velCC; }
	void   SetVel(const NxVec3& vel) { velCC = vel; }

	NxVec3 GetForecastMove(float threshold);
	NxVec3 GetInAirReviseMove();

	AttachedActor* GetAttachmentBinding() const;
	const AttachedActor* GetAttachmentCandidate() const;
	const AttachedActor* GetJumpingTakeOffBackUp() const;

	void ApplyForce(APhysXScene& aScene);
	void AddForceHorizontal(NxActor& actor, const NxVec3& pos, const NxVec3& dir, const float pushFactor);
	void AddForceVertical(NxActor& actor, const NxVec3& pos, const NxVec3& force, const bool bIsJumpTakeOff);

	void AddPushing(NxActor& actor);
	bool IsPushing(NxShape& shape) const;

private:
	static const NxVec3 gZero;

	int		StatusFlags;
	float	timeSec;
	NxVec3	gravity;
	NxVec3  velCC;
	NxVec3  InputMoveDir;

	float HitCosVal;
	AttachedActor* pAttaching;
	AttachedActor AA;
	AttachedActor JumpingTakeOffBackUp;

	ForceRecorder forceHorizontal;
	ForceRecorder forceVertical;
	APhysXArray<NxActor*> PushingActor;

public:
	NxVec3  DispFMove;
	NxVec3	DispFMoveUnfinished;
	NxVec3  DispFMoveCandidate;
};

class EntityReport : public NxUserEntityReport<NxShape*>
{
public:
	EntityReport() { m_pLastFrame = 0; }
	virtual bool onEvent(NxU32 nbEntities, NxShape** entities);

	void AddShape(NxShape& shape);
	void Clear() { m_Shapes.clear(); }
	bool IsEmpty() const { return m_Shapes.isEmpty(); }
	void SetLastFrame(FrameInfo* pFrame) { m_pLastFrame = pFrame; }

	void ChangeAttributes(const NxGroupsMask& newGM);
	void RecoverAttributes();
	bool IsInclude(const AttachedActor& aa) const;
	bool IsInclude(const NxShape& shape) const;
	APhysXU32 GetObjects(APhysXArray<APhysXObject*>& outObjects) const;
	APhysXU32 GetNxActors(APhysXArray<NxActor*>& outNxActors) const;

private:
	struct Node
	{
		Node(NxShape& shape) { pShape = &shape; }
		bool operator == (const Node& rhs) const { return rhs.pShape == pShape; }

		NxShape* pShape;
		NxGroupsMask gmBackup;
	};

	FrameInfo* m_pLastFrame;
	APhysXArray<Node> m_Shapes;
};

class EntityReportSweep : public NxUserEntityReport<NxSweepQueryHit>
{
public:
	EntityReportSweep() { ResetPos(NxVec3(0.0f), NxVec3(0.0f)); m_pLastShape = 0; }
	
	virtual bool onEvent(NxU32 nbEntities, NxSweepQueryHit* entities);
	
	void ResetPos(const NxVec3& startPos, const NxVec3& targetPos, bool IsEnforceRevising = false);
	NxVec3 GetProperPos(bool& outIsNewPos, NxVec3& outNormal, bool bCountInLast = true) const;
	NxShape* GetClosestDynamicActorShape() const { return m_pClosestDynActShape; }

private:
	bool m_IsEnforceRevising;
	NxVec3 m_posStart;
	NxVec3 m_posTarget;
	
	float m_tClosest;
	NxVec3 m_norClosest;
	NxVec3 m_posClosest;
	NxShape* m_pShapeClosest;

	NxVec3 m_norLastShape;
	NxVec3 m_posLastShape;
	NxShape* m_pShapeLastShape;

	float m_tClosestDAS;
	NxShape* m_pClosestDynActShape;
	mutable NxShape* m_pLastShape;
};

class EntityReportSweepForClosestNonDynamic : public NxUserEntityReport<NxSweepQueryHit>
{
public:
	EntityReportSweepForClosestNonDynamic() { Reset(); }
	
	virtual bool onEvent(NxU32 nbEntities, NxSweepQueryHit* entities);
	
	void Reset() { m_pTheHit = 0; m_bIsHitDynamicStuff = false; }
	bool IsHitDynamicStuff() const { return m_bIsHitDynamicStuff; }
	NxSweepQueryHit* GetHitResult() const { return m_pTheHit; }

private:
	bool m_bIsHitDynamicStuff;
	NxSweepQueryHit m_sqh;
	NxSweepQueryHit* m_pTheHit;
};

class EntityReportSweepForDynCC : public NxUserEntityReport<NxSweepQueryHit>
{
public:
	EntityReportSweepForDynCC();
	
	virtual bool onEvent(NxU32 nbEntities, NxSweepQueryHit* entities);
	
	void Reset(const NxVec3& gDir, const NxVec3& ccDir, const NxVec3& ccVel, const NxVec3& btmCenter, const float ccRadius, const bool ccIsInAir);
	void SetSlideThreshold(const float t) { m_ccSlideThreshold = t; }
	bool IsInBodyHit() const;
	NxShape* GetHitShape() const;
	NxSweepQueryHit* GetHitResult() const { return m_pCurrentHit; }

	NxVec3 GetNormal(const bool isInBody, bool* poutIsDefNormal = 0) const;
	AttachedActor* GetSupportActor(const bool isInBody) const;

private:
	bool onEventCollectInfo(const NxSweepQueryHit& item, bool& outLastSqhIsInBody, const NxVec3& ccHVel);
	bool InBodyCaseChooseFirst(const NxSweepQueryHit& first, const NxSweepQueryHit& second, const NxVec3& ccHVel);

private:
	static NxVec3 gZero;

	NxSweepQueryHit m_sqhLast;
	NxSweepQueryHit m_sqhInBody;
	NxSweepQueryHit m_sqhOutBody;
	
	NxShape* m_pLastHitShape;
	NxSweepQueryHit* m_pCurrentHit;
	NxSweepQueryHit* m_pInBodyFarthest;
	NxSweepQueryHit* m_pOutBodyClosest;

	bool m_ccIsInAir;
	float m_ccRadius;
	float m_ccSlideThreshold;
	NxVec3 m_ccDir;
	NxVec3 m_ccVel;
	NxVec3 m_btmCenter;
	NxVec3 m_defNormal;

	AttachedActor* m_pSupport;
	AttachedActor m_AA;
};

class EntityReportSweepForPushing : public NxUserEntityReport<NxSweepQueryHit>
{
public:
	EntityReportSweepForPushing(APhysXCharacterController& currentCC, FrameInfo& currentFrame, const NxActor* pActor, float sweepDist);

	virtual bool onEvent(NxU32 nbEntities, NxSweepQueryHit* entities);

private:
	float m_sweepDist;
	APhysXCharacterController* m_ActiveCC;
	FrameInfo* m_pActiveFrame;
	const NxActor* m_pLastBinding;
};

#endif