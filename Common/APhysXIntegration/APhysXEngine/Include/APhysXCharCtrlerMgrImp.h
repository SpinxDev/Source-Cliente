/*
* FILE: APhysXCharCtrlerMgrImp.h
*
* DESCRIPTION:
*	Provide the actual definition of APhysXCharacterControllerManager.
*
*
* CREATED BY: Yang Liu, 2010/03/11
*
* HISTORY: 
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_MANAGER_IMP_H_
#define _APHYSX_CHARACTER_CONTROLLER_MANAGER_IMP_H_

#include "APhysXCharCtrlerManager.h"

class APhysXCharCtrlerMgrImp : public APhysXCharacterControllerManager
{
public:
	static APhysXCharCtrlerMgrImp* InitCCM(APhysXEngine& engine);
	static APhysXCharCtrlerMgrImp* GetCCM();
	static void ReleaseCCM();

public:
	virtual ~APhysXCharCtrlerMgrImp() {};

	virtual void Tick() const;
	virtual void ReleaseAll(APhysXScene* pScene = 0);
	
	virtual APhysXCharacterController* CreateCC(APhysXScene& scene, APhysXCCDesc& desc);
	virtual void ReleaseCC(APhysXCharacterController* pController);
	
	virtual APhysXLWCharCtrler* CreateLWCC(APhysXScene& scene, APhysXLWCCDesc& desc);
	virtual void ReleaseLWCC(APhysXLWCharCtrler* pController);

	virtual APhysXDynCharCtrler* CreateDynCC(APhysXScene& scene, APhysXCCCapsuleDesc& desc);
	virtual void ReleaseDynCC(APhysXDynCharCtrler* pController);
	
	virtual APhysXNxCharCtrler* CreateNxCC(APhysXScene& scene, APhysXCCDesc& desc);
	virtual void ReleaseNxCC(APhysXNxCharCtrler* pController);

public:
	bool IsStaticObjectsChanged() const { return m_StaticObjectsChanged; }
	NxGroupsMask GetRegisteredGM(APhysXScene& scene) const;
	bool TickForSynPos() const;
	void TickForInternal() const;
	bool ClosestSweepExcludesAllCCs(const APhysXScene& scene, const NxCapsule& cap, const NxVec3& motion, NxU32 flags, NxSweepQueryHit& outResult) const;
	bool ClosestRaycastExcludesAllCCs(const APhysXScene& scene, const NxVec3& rayOrig, const NxVec3& motion, NxShapesType flags, NxRaycastHit& inoutResult) const;

	void OnObjectChangeNotify(bool IsCreating, APhysXObject& rObject);
	void AddDebugData(APhysXDebugRenderer& drObj, const APhysXScene* pScene = 0);

private:
	void ResetToInit();
	void ReleaseAllExcution(bool ignoreNxObject, APhysXScene* pScene);
	void ReleaseCCExcution(APhysXCharacterController* pController, bool ignoreNxObject);
	void ReleaseLWCCExcution(APhysXLWCharCtrler* pController, bool ignoreNxObject);
	void ReleaseDynCCExcution(APhysXDynCharCtrler* pController, bool ignoreNxObject);
	void ReleaseNxCCExcution(APhysXNxCharCtrler* pController, bool ignoreNxObject);
	void ReleaseNxController(APhysXCharacterController& controller) const;
	void ReleaseNxController(APhysXNxCharCtrler& controller) const;

	void PushRenderData(const APhysXCCBase& cc, NxU32 color);
	void OnObjectChange(bool IsCreating, APhysXObject& rObjects);
	void OnTerrainChange(bool IsLoading, const NxBounds3& aabb, const APhysXScene& Scene);

	NxMaterialIndex GetSmoothMaterial(APhysXScene& scene);
	void ReleaseSmoothMaterial(APhysXScene& scene);
	int CountDynCC(APhysXScene& scene) const;

	void GetAllExcludesGroupAndMask(const APhysXScene& scene, NxU32& outActiveGroups, NxGroupsMask& outGM) const;

	/*
	Register/Unregister the collision channel that is scene related. The CC would not collide with the registered channel if 
	APX_CCCF_FILTERING_REG_CHANNEL flag was setting to CCM(cc manager).

	Note:
	1. This interface is additional filter out besides host model channel if user have some especial requirement.
	2. Each call RegisterChannel must be matched by a corresponding call to UnRegisterChannel, as CCM uses a reference counter internally.
	*/
	bool RegisterChannel(APhysXScene& scene, const APhysXCollisionChannel channel);
	bool UnRegisterChannel(APhysXScene& scene, const APhysXCollisionChannel channel);

private:
	static const int gTerrainHeightExtension;

	// Singleton object, prohibit construction
	APhysXCharCtrlerMgrImp();
	APhysXCharCtrlerMgrImp& operator= (const APhysXCharCtrlerMgrImp& rhs);

private:
	struct SGM
	{
		SGM(APhysXScene& as) : pScene(&as), pRegCount(0) { gm.bits0 = gm.bits1 = gm.bits2 = gm.bits3 = 0; }
		bool operator == (const SGM& rhs) const { return rhs.pScene == pScene; }

		APhysXScene* pScene;
		NxGroupsMask gm;
		APhysXMap<APhysXCollisionChannel, int>* pRegCount;
	};

	struct SMaterial
	{
	public:
		SMaterial(APhysXScene& scene) : m_pScene(&scene), m_pMaterial(0), m_UsingCount(0) {}

		void CreateMaterial();
		void ReleaseMaterial();
		NxMaterialIndex GetIndexMaterial();
		APhysXScene* GetAPhysXScene() const { return m_pScene; }
		int GetUsingCount() const { return m_UsingCount; }

	private:
		APhysXScene* m_pScene;
		NxMaterial* m_pMaterial;
		int m_UsingCount;
	};

private:
	mutable bool m_StaticObjectsChanged;

	APhysXArray<SGM> m_RegChannels;
	APhysXArray<APhysXNxCharCtrler*> m_pVecNxCC;
	APhysXArray<APhysXLWCharCtrler*> m_pVecCCLW;
	APhysXArray<APhysXDynCharCtrler*> m_pVecCCDyn;
	APhysXArray<APhysXCharacterController*> m_pVecCC;

	APhysXArray<SMaterial> m_matSmooth;
};

#endif