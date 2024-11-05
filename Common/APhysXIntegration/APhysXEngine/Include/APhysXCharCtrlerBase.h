/*
* FILE: APhysXCharCtrlerBase.h
*
* DESCRIPTION: 
*	A base class. Provide basic properties and channel management.
*
*
* CREATED BY: Yang Liu, 2010/02/22
*
* HISTORY: 
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*
*
*             Inheritance Hierarchies
*      -- interface class(export class)       ~~ implement class
*
*									--------------------
*										APhysXCCBase
*									--------------------
*									/				\
*								  /			  		    \
*								/			  			    \
*					    ------------------				        \ 
*				 	     APhysXHostCCBase					        \
*					    ------------------						        \
*						  /		\		   \							    \
*						/		  \			    \						        \
*					  /				\	             \       				        \
*					/				  \				      \					            \
*	 ---------------------------   --------------------   ---------------------    --------------------
*     APhysXCharacterController     APhysXNxCharCtrler     APhysXDynCharCtrler      APhysXLWCharCtrler
*	 ---------------------------   --------------------   ---------------------    --------------------
*                 |						    |	                    |                       | 
*                 |						    |	                    |                       |
*                 |						    |	                    |                       |
*		   ~~~~~~~~~~~~~		  	 ~~~~~~~~~~~~~~~         ~~~~~~~~~~~~~~~~		  ~~~~~~~~~~~~~~~
*			APhysXCCImp               APhysXNxCCImp           APhysXDynCCImp           APhysXLWCCImp
*		   ~~~~~~~~~~~~~		  	 ~~~~~~~~~~~~~~~         ~~~~~~~~~~~~~~~~		  ~~~~~~~~~~~~~~~
*
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_BASE_H_
#define _APHYSX_CHARACTER_CONTROLLER_BASE_H_

class APhysXCCChannelMgr;
class APhysXCharCtrlerMgrImp;

class APHYSX_DLL_EXPORT APhysXCCBase
{
public:
	enum APhysXVolumeType
	{
		APX_VT_UNKNOWN	= 0,
		APX_VT_CAPSULE	= 1,
		APX_VT_BOX		= 2,
	};

	enum APhysXCCStatus
	{
		APX_CCS_STANDON_SOMETHING	= (1<<0),
		APX_CCS_JUMPING_TAKEOFFE	= (1<<1),   // usually combine with APX_CCS_JUMPING_INAIR
		APX_CCS_JUMPING_INAIR		= (1<<2),
		APX_CCS_JUMPING_LANDING		= (1<<3),   // this flag excludes APX_CCS_JUMPING_INAIR
		APX_CCS_HANGING				= (1<<4),   // usually combine with APX_CCS_JUMPING_INAIR
	};

	/*
	checks whether the input space has occupied by any RB shapes or not. Note: assume Y+ direction is (0, 1, 0).
	
	Parameters:
		[in]	scene	The scene to be tested  
		[in]	posFoot	The testing foot position
		[in]	volume	The testing volume
		[in]	type	The type of character controller shape

			type				volume meaning
		APX_VT_CAPSULE:		NxVec3.x == radius, NxVec3.y == height(for definition refer to APhysXCCCapsuleDesc.height),  NxVec3.z == ignore
		APX_VT_BOX:			box dimensions
	  
	Return Value:
		return true if overlap with something, or return false.
		return true if input APX_VT_UNKNOWN type.

	Remarks:
		All default groups/channels that won't affect CC/LWCC/DynCC's creation/movement have been filtered out.
		The CC/LWCC/DynCC/NxCC's hidden actors have been filtered out. 
	*/
	static bool CheckOverlap(APhysXScene& scene, const NxVec3& posFoot, const NxVec3& volume, const APhysXVolumeType type);

public:
	APhysXCCBase(APhysXScene& scene);
	virtual ~APhysXCCBase();

	APhysXCharacterControllerManager& GetCCManager() const;
	APhysXScene& GetAPhysXScene() const;
	NxScene& GetNxScene() const;
	void GetAABB(NxBounds3& aabb) const;
	int GetStatus() const { return m_Status; };

	/*
	register/unregister host model channel and host children model channel to filter out all those host RBs in CC/LWCC/DynCC/NxCC move method. 
	*/
	bool RegisterHostModel(APhysXObject* pObject, APhysXCollisionChannel& outNewCNL);
	bool UnRegisterHostModel(APhysXObject* pObject, APhysXCollisionChannel& outOriginalCNL);

	/*
	enable/disable collision with the groups. default enable collision with all groups except 3 groups: APX_COLLISION_GROUP_ATTACHER, APX_COLLISION_GROUP_CLOTH_ATTACHER and APX_COLLISION_GROUP_CC_HOST_RB.
	*/
	NxU32 GetActiveGroups() const;
	void OpenGroup(const APhysXCollisionGroup& group);
	void CloseGroup(const APhysXCollisionGroup& group);

	/*
	get the group mask value which can be used to sweep/raycast directly.
	*/
	NxGroupsMask GetFilterGM(bool bEnforceCombineCCMRegGM) const;

	/*
	forces the CC/LWCC/DynCC/NxCC to sleep. CC/LWCC/NxCC's passive collision state(CC/LWCC/NxCC don't move, other object hit CC/LWCC/NxCC) was frozen until it was waked up. 
	
	For CC/NxCC:
		If CC is sleeping, CC can't work(e.g. MoveCC, lost attachment, zero stand/push force, no slide movement) any more until it was waked up. 
		CC can receive new volume, new position, but don't apply it until it was waked up. 

	For LWCC:
		If LWCC is sleeping, LWCC can't work(e.g. revise vertical pos) any more until it was waked up. LWCC can receive new volume, new position, and apply it immediately. 

	For DynCC:
		It can be put to sleep even DynCC is in hang mode.
		If DynCC is sleeping, DynCC can't work(e.g. MoveCC, hanging) any more until it was waked up. DynCC can receive new volume, new position, and apply it immediately.
	*/
	void PutToSleep() { OnPutToSleep(); m_IsSleeping = true; }
	void WakeUp() { OnWakeUp(); m_IsSleeping = false; }
	bool IsSleeping() const { return m_IsSleeping; }

	/*
	set the distance of detection in gravity direction.
	Return false if input a zero/negative value. 
	Default: the height of CCs
	*/
	bool SetDetectionDistance(float distance);
	float GetDetectionDistance() const { return m_detDistance; }

	/*
	get distance to ground or other collision-enable objects in gravity direction.	Note: the return value always >= 0
	For LWCC/NxCC, just return 0.0f or NX_MAX_F32
	For CC/DynCC, just return NX_MAX_F32 if the distance great than 3 * GetDetectionDistance()
	*/
	float GetDistToGround() const { return m_DistToGround; }

	/*
	get/set the volume of character controller.
	
	Parameters:
		[out]	outVT		The type of character controller  
		[in]	newVolume	The new volume  
	  
	Return Value:
		For GetVolume(): the return value has different meaning according to the output APhysXVolumeType. See following form 
							outVT				return value meaning
						APX_VT_CAPSULE:		NxVec3.x == radius, NxVec3.y == height,  NxVec3.z == -1
						APX_VT_BOX:			box dimensions

		For ChangeVolume(): return true, if the new volume was applied successful. Or return false. (Generally, there is no enough
							space to apply the new volume. For example, you can't set a big height if the character is in a house.)  
	*/
	virtual NxVec3 GetVolume(APhysXVolumeType& outVT) const = 0;
	virtual bool ChangeVolume(const NxVec3& newVolume) = 0;

	// NxController's getPosition() return the center pos
	// However, the foot pos is also useful info.
	virtual NxVec3 GetFootPosition() const = 0;
	virtual NxVec3 GetCenterPosition() const = 0;

	/*
	teleport to the position without blocker even the space was occupied by something.
	For DynCC:
		this function doesn't work if DynCC is in hang mode. 
	*/
	virtual void SetFootPosition(const NxVec3& pos) = 0;

	// retrieves the CC's world space orientation. 
	virtual NxMat33 GetGlobalOrientation() const = 0; 

	/*
	get current frame normal value of standing position on the ground/attachment if CC is not in air.

	Note:
		the normal value doesn't update until next MoveCC() or TickReviseVerticalPos() was called. 
		So, It would return wrong value if you call this function between SetFootPosition() and MoveCC()/TickReviseVerticalPos().

	Parameters:
	[out]	outpbIsDefVal	Indicate the return value is the default value or not.
	[in]	defVal			the default value that will be returned if CC is in air(or LWCC doesn't revise position).  

	Return Value:
		return defVal if CC is in air. Or return the normal of point on which CC is standing.
	*/
	virtual NxVec3 GetGroundNormal(bool* outpbIsDefVal = 0, const NxVec3& defVal = NxVec3(0, 1, 0)) const = 0;

	/*
	get instant ground info by sweep. Sweep a sphere to find the surface position around input position. The sweep start point is [pos] + GDir * -1 * volume.y * 0.5f. The sweep direction is gravity direction.
	(for ExtCC/NxCC box controller, using a approximation sphere instead of bos shape) 
	For LWCC, this method is similar with TickReviseVerticalPos(), but few points:
		1. don't considering LWCC's property requirements
		2. don't considering in air status
		3. don't update any data or status of LWCC
		4. use the whole radius of LWCC

	Parameters:
	[in]  sweepDist The distance of sweep  
	[in, out]  pos	The position that need to be revised. 
	[out, optional]  poutNormal The normal of the revised pos. If the pos doesn't revised, the normal wouldn't change

	Return Value:
	Return true, if the input position was updated. Return false, if sweep hit nothing.  
	*/
	virtual bool GetGroundPos(float sweepDist, NxVec3& pos, NxVec3* poutNormal = 0) const = 0;

	virtual bool IsComponent(const NxActor& actor) const = 0;
	virtual NxActor* GetHiddenActor() const = 0;

protected:
	APhysXCharCtrlerMgrImp& GetCCMImp() const;
	void OnObjectDeleted(APhysXObject& obj);
	void TerrainChange(bool IsLoading, const NxBounds3& TerrainAABB, const APhysXScene& Scene);
	NxMat33 GetIDGlobalOrientation() const; 

private:
	APhysXCCBase& operator= (const APhysXCCBase& rhs);
	
	virtual void OnPutToSleep() {}
	virtual void OnWakeUp() {}

public:
	void* userData;

protected:
	static const NxVec3 gZero;

	bool m_bDisableCollision;
	bool m_IsSleeping;
	int	 m_Status;

	NxVec3 m_NxVolume;
	NxVec3 m_GroundNormal;

	float m_detDistance;
	float m_DistToGround;

	APhysXCCChannelMgr* m_pCCCM;
	NxBounds3 m_SleepingTerrainAABB;
};

class APHYSX_DLL_EXPORT APhysXHostCCBase : public APhysXCCBase
{
public:
	APhysXHostCCBase(APhysXScene& scene, const APhysXCCDesc& desc); 
	virtual ~APhysXHostCCBase() {};

	float GetMass() const { return m_mass; }
	void  SetMass(const float m) { m_mass = m; }

	float GetSpeed() const { return m_speed; }
	void  SetSpeed(const float v) { m_speed = v; }

	/*
	get/set the factor which is used to compute jump-start force and jump-landing force. TheForce = M * G * m_JumpingFactor.
	if the factor < 0, the jumping force will be computed following physics rule.

	Default Value: -1
	*/
	float GetJumpingFactor() const { return m_JumpingFactor; }
	void  SetJumpingFactor(const float f) { m_JumpingFactor = f; }

	virtual NxVec3 GetGroundNormal(bool* outpbIsDefVal = 0, const NxVec3& defVal = NxVec3(0, 1, 0)) const;

	/*
	get/set the auto-stepping height. See also NxController::setStepOffset() 
	*/
	virtual float GetStepOffset() const = 0;
	virtual void  SetStepOffset(const float offset) = 0;

	// get/set direction of CC face toward. The direction is in world frame.
	virtual NxVec3 GetDir() const = 0;
	virtual void   SetDir(const NxVec3& vDir) = 0;
	// get direction of CC head toward. The direction is in world frame.
	virtual NxVec3 GetUpDir() const = 0;
	// get horizon direction of CC face toward. The direction is perpendicular to CC's gravity.
	virtual NxVec3 GetHorizonDir() const = 0;

	/* get current instant velocity.
	
	Parameters:
	[in]  bIsAbsVel Indicate to retrieve absolute velocity or the velocity relative to binding point.  
	*/
	virtual NxVec3 GetInstantVelocity(bool bIsAbsVel = false) const = 0;

	/*
	take jump action
	
	Parameters:
		[in]	fJumpingSpeed		In the y+ direction which is always opposite CC's gravity.
		[in]	bIsAbsoluteSpeed    Indicate input an absolute jump speed or not.
	*/
	virtual void  JumpCC(float fJumpingSpeed, bool bIsAbsoluteSpeed = false) = 0;

	/*
	get attached global point.
	
	Parameters:
		[out]  out	The attached point  
	  
	Return Value:
		Return true, if the controller is binding to an actor. Or return false, and the output parameter has no meaning. 
	*/
	virtual bool GetAttachmentPos(NxVec3& out) const = 0;

	/*
	In each frame, characters can be moved using this function.
	
	Parameters:
		[in]	deltaSecond	The delta time to move.	Unit: Second
		[in]	MoveDir		The direction to move.  

	Return Value:
		Return the collision flags. This is a combination of NxControllerFlag flags. See also NxControllerFlag.
		For dynamic-cc, always return value zero!

	Note:
		1)
		If APX_CCFM_ASYNCHRONOUS was setting to CCM, when you called MoceCC(), you can't get new position information until 
		APhysXCharacterControllerManager.Tick() was called. So, the correct way is:  (see also APhysXCharacterControllerManager.Tick())
		......
		...do the move task for all APhysX CCs..
		then call APhysXCharacterControllerManager.Tick();
		...OK, you can get the latest position by GetFootPosition() or similar functions
		......
	    2)
		For dynamic-cc, besides above common function, an additional feature that is "control hanging" are provided. 
		You can control the hanging distance and amplitude by the input dir.
		Decrease hang distance, if the input dir is opposite to the gravity of scene.
		Increase hang distance, if the input dir is the same with the gravity of scene.
		Add force to the tangent dir of hanging, according to the relative of the input dir and dynCC's horizon dir().  
	*/
	virtual NxU32 MoveCC(const float deltaSecond, const NxVec3& MoveDir) = 0;

private:
	APhysXHostCCBase& operator= (const APhysXHostCCBase& rhs);
	
private:
	float	m_JumpingFactor;
	float   m_mass;
	float	m_speed;
};
#endif