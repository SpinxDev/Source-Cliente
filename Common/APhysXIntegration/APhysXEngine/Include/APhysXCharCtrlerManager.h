/*
* FILE: APhysXCharCtrlerManager.h
*
* DESCRIPTION: 
*	This class is used for the management of character controller.
*
*
* CREATED BY: Yang Liu, 2009/08/31
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSX_CHARACTER_CONTROLLER_MANAGER_H_
#define _APHYSX_CHARACTER_CONTROLLER_MANAGER_H_

class NxPhysicsSDK;
class NxControllerManager;
class APhysXScene;
class APhysXCCDesc;
class APhysXCharacterController;
class APhysXLWCharCtrler;
class APhysXDynCharCtrler;
class APhysXNxCharCtrler;

/*
Specifies how a APhysX CC interacts with other characters. (The characters maybe has or hasn't APhysX CC.)
*/
enum APhysXCCCollisionFlag
{
	// Default value
	// Collide with all channel but APX_COLLISION_CHANNEL_CC_COMMON_HOST and APX_COLLISION_CHANNEL_CC_SKIP_COLLISION.
	APX_CCCF_COLLIDE_ALL_CHANNEL	= 0,

	// Filter out all the registered channels. (CC will have no collision each other)
 	APX_CCCF_FILTERING_REG_CHANNEL	= 1,

	// Not implemented
	//APX_CCCF_FILTERING_REG_OBJECT	= 2,
};

enum APhysXNewPoseFlushMode
{
	// Default value
	// For CC/NxCC:
	//     You can get new position information after you called MoceCC() immediately.
	// For LWCC:
	//	   You should call TickReviseVerticalPos() by yourself. And then get the position information. 
	// For DynCC:
	//	   Doesn't work.
	APX_CCFM_SYNCHRONOUS	= 0,

	// For DynCC:
	//	   Always work. And user can't get new pose until after APhysXScene.Simulate()/APhysXEngine.Simulate();
	// For Others:
	//	   See the description Tick().
	APX_CCFM_ASYNCHRONOUS	= 1,
};

class APHYSX_DLL_EXPORT APhysXCharacterControllerManager
{
public:
	/*
	Performs a linear sweep through space to find the closest environment object. All the CCs(ExtCC/LWCC/DYNCC/NxCC) and CC's registered host model and CCM's registered channel
	in the same scene will be filtered out.

	Parameters:
	[in]	flags		Indicate the mode of the sweep(see NxSweepFlags). Only 2 value (NX_SF_STATICS and NX_SF_DYNAMICS) can be used.
	[out]	outResult	the sweep result  

	Return Value:
	return -1 if error occurs(ex. CCM is not exist or NxScene is not exist) 
	return 0 if the sweep action is success, but hit nothing.
	return 1 if the sweep action is success, and hit something.
	*/
	static int ClosestSweepExcludesAllCCs(const APhysXScene& scene, const NxCapsule& cap, const NxVec3& motion, NxU32 flags, NxSweepQueryHit& outResult);
	/*
	Has the same effect as ClosestSweepExcludesAllCCs but using raycast instead of sweep detection.
	Parameters:
	[in, out] inoutResult	Input NxRaycastHit.flags to specify which field of NxRaycastHit users are interested in. See NxRaycastBit
							If the input flags is zero, all items will be retrieved.
							Output raycast result

	Return Value:
	the same as ClosestSweepExcludesAllCCs
	*/
	static int ClosestRaycastExcludesAllCCs(const APhysXScene& scene, const NxVec3& rayOrig, const NxVec3& motion, NxShapesType flags, NxRaycastHit& inoutResult);

	APhysXCharacterControllerManager() : userData(0) {}
	virtual ~APhysXCharacterControllerManager() {};

	APhysXCCCollisionFlag GetInteractFlag() const { return m_CollisionFlag; }
	void SetInteractFlag(const APhysXCCCollisionFlag flag) { m_CollisionFlag = flag; }

	APhysXNewPoseFlushMode GetFlushMode() const { return m_FlushMode; }
	void SetFlushMode(const APhysXNewPoseFlushMode flag) { m_FlushMode = flag; }

public:
	/*
	If APX_CCFM_SYNCHRONOUS flag was setting: 
		Do internal maintenance works.

	If APX_CCFM_ASYNCHRONOUS flag was setting:
		Update all character controller's movement result. For APhysXLWCharCtrler, do the "pos-revising" task by call TickReviseVerticalPos() function. 
		Note:
			You can visit the new position information after call this function.
			So, typically, you should do the following things:
			......
			...if you have CCs/NxCCs, move all CCs/NxCCs through APhysXCharacterController.MoveCC(...) one by one...
			...if you have LWCCs, update all LWCCs through APhysXLWCharCtrler.SetFootPosition(...) one by one...
			APhysXEngine.GetAPhysXCCManager()->Tick();
			...update model's position and dir by APhysXCharacterController.GetFootPosition() and APhysXCharacterController.GetHorizonDir()...
			...Or update model's position by APhysXLWCharCtrler.GetFootPosition()...
			...do the model's animation tick...
			APhysXEngine.Simulate(deltaTime);
			......
	*/
	virtual void Tick() const = 0;

	/*
	Release all various CCs created in the input scene. If input an empty pointer, release all of created various CCs.
	*/
	virtual void ReleaseAll(APhysXScene* pScene = 0) = 0;

	/*
	Note: There are some requirements to the host model before create CC/LWCC/DynCC/NxCC:
		1. Make sure the host model is in the collision group of APX_COLLISION_GROUP_CC_HOST_RB.(To avoid bug of PhysX2.8.1, see also APX_COLLISION_GROUP_CC_HOST_RB)
		2. Make sure the host model is in an independent or APX_COLLISION_CHANNEL_CC_COMMON_HOST collision channel.
		3. The host model is setting the flag APX_OBJFLAG_ONEWAY_COLLISION if it's necessary.
	*/
	virtual APhysXCharacterController* CreateCC(APhysXScene& scene, APhysXCCDesc& desc) = 0;
	virtual void ReleaseCC(APhysXCharacterController* pController) = 0;
	
	// see the "Note" above CreateCC() function
	virtual APhysXLWCharCtrler* CreateLWCC(APhysXScene& scene, APhysXLWCCDesc& desc) = 0;
	virtual void ReleaseLWCC(APhysXLWCharCtrler* pController) = 0;
	
	// see the "Note" above CreateCC() function
	virtual APhysXDynCharCtrler* CreateDynCC(APhysXScene& scene, APhysXCCCapsuleDesc& desc) = 0;
	virtual void ReleaseDynCC(APhysXDynCharCtrler* pController) = 0;

	// see the "Note" above CreateCC() function
	virtual APhysXNxCharCtrler* CreateNxCC(APhysXScene& scene, APhysXCCDesc& desc) = 0;
	virtual void ReleaseNxCC(APhysXNxCharCtrler* pController) = 0;

public:
	void* userData;

protected:
	static NxControllerManager* pNxCCMgr;

private:
	APhysXCCCollisionFlag m_CollisionFlag;
	APhysXNewPoseFlushMode m_FlushMode;
};

#endif