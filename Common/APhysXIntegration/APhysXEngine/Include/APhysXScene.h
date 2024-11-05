/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXSCENE_H_
#define _APHYSXSCENE_H_

//-----------------------------------------------------------------------------
enum APhysXForceFlag
{
	APX_FF_LOCALFORCE		= 1 << 0,
	APX_FF_ATPOS			= 1 << 1,
	APX_FF_LOCALPOS			= 1 << 2,

	APX_FF_INVALID_ACTOR	= 1<< 20,		// raise this flag if mActor is invalid...
};


struct APhysXForceInfo
{
	NxActor*			mActor;
	NxVec3				mForce;
	NxVec3				mPos;
	NxU32				mForceFlag;
	NxForceMode			mForceMode;

	APhysXForceInfo()
	{
		mActor = NULL;
		mForce.zero();
		mPos.zero();
		mForceFlag = 0;
		mForceMode = NX_FORCE;
	}
};


//-----------------------------------------------------------------------------
enum APhysXSceneUpdateGraphicsType
{
	APX_SCENE_UGT_SYNCH,
	APX_SCENE_UGT_ASYNCH
};

enum APhysXSimulateTimeStepMethod
{
	APX_SIMULATE_TSM_FIXED,
	APX_SIMULATE_TSM_VARIABLE,
};

class APHYSX_DLL_EXPORT APhysXSceneDesc
{

public:
	NxSceneDesc mNxSceneDesc;
	APhysXSceneUpdateGraphicsType mUpdateGraphicstype;
	bool mActive; 
	bool mEnableDebugRender;						// whether render physics data, actor, shape, axis, covex, cloth mesh and so on...
	bool mEnablePerfStatsRender;					// whether render physics performance statics...
	APhysXMaterialDesc mDefaultMaterial;
	float mGroundPlaneElevation;

	APhysXSimulateTimeStepMethod mSimulateTimeStepMethod;
	float mTimeStep;
	APhysXU32 mMaxIter;

	APhysXSceneDesc()
	{	
		mUpdateGraphicstype = APX_SCENE_UGT_SYNCH;
		mActive = true;
		mEnableDebugRender = false;
		mEnablePerfStatsRender = false;
		mGroundPlaneElevation = -6.0f;

		// set the default material values...
		mDefaultMaterial.mRestitution = gDefaultPhysXMaterialDesc.mRestitution;
		mDefaultMaterial.mStaticFriction = gDefaultPhysXMaterialDesc.mStaticFriction;
		mDefaultMaterial.mDynamicFriction = gDefaultPhysXMaterialDesc.mDynamicFriction;

		mSimulateTimeStepMethod = APX_SIMULATE_TSM_FIXED;
		mTimeStep = 1.0f/60.0f;
		mMaxIter = 8;
	}
};

struct APHYSX_DLL_EXPORT APhysXDebugRenderDesc
{
	APhysXDebugRenderDesc()
	{
		nPerfStatsTextStartX = 10;
		nPerfStatsTextStartY = 10;

		// -1 means following current position
		nSceneStatsTextStartX = -1;
		nSceneStatsTextStartY = -1;

		nSDKStatsTextStartX = -1;
		nSDKStatsTextStartY = -1;
	}

	int nPerfStatsTextStartX;
	int nPerfStatsTextStartY;

	int nSceneStatsTextStartX;
	int nSceneStatsTextStartY;

	int nSDKStatsTextStartX;
	int nSDKStatsTextStartY;
};

class APHYSX_DLL_EXPORT APhysXScene
{

	friend class APhysXEngine;
	friend class APhysXCreationDeletionReport;

public:

	NxScene* GetNxScene() { return m_pNxScene; }
	APhysXEngine* GetAPhysXEngine() { return m_pAPhysXEngine; }

	// new and return a specified APhysXObject pointer...
	// so, the user who keep the pointer should be responsible to delete it
	static APhysXObject* CreatePhysXObject(APhysXObjectType apxObjectType);

	//-----------------------------------------------------------------------------
	// object/connector creation: in sync way!
	// 
	// the PhysXObject/Connector creation is always in sync way, while the new-created object
	// is laid in cached creation list but not in m_arrPhysXObjects/m_arrPhysXConnectors until
	// the calling of FlushCreation(). This works well in multi-threaded mode.
	// while, if the whole physX simulation/creation/release are all in the single-threaded mode, 
	// user can create object/connector instantly as well
	APhysXObject* CreatePhysXObject(const APhysXObjectInstanceDesc& apxObjInstanceDesc, bool bInstant = false);
	APhysXObjectConnector* CreatePhysXObjectConnector(const APhysXObjectConnectorInstanceDesc& apxConnectorInstanceDesc, bool bInstant = false);
	
	//-----------------------------------------------------------------------------
	// object/connector release
	// 
	// by default, we release PhysXObject in the ASync way and the real release will 
	// be executed till next simulation. this can make sure all things work well in multi-threaded mode.
	// while, if the whole physX simulation/creation/release are all in the single-threaded mode, 
	// user can release object/connector instantly as well
	void ReleasePhysXObject(APhysXObject* pPhysXObject, bool bInstant = false);
	void ReleasePhysXObjectConnnector(APhysXObjectConnector* pPhysXObjectConnector, bool bInstant = false);


	// retrieve functions...
	int GetPhysXObjectNum()
	{
		return m_arrPhysXObjects.size();
	}
	APhysXObject* GetPhysXObject(int i)
	{
		return m_arrPhysXObjects[i];
	}

	// search functions
	APhysXObject* GetPhysXObject(const char* szObjName);

	// some utility function, just for debug and easily creation
	void CreateDummySphereActor(const NxVec3& vPos, float fRadius = 1.0f, const NxVec3& vVel = NxVec3(0.0f));
	void ReleaseDummyActors();

	void CreateGroundPlane(float fElevation = -6.0f);
	void ReleaseGroundPlane();

	void StartSimulate();
	void EndSimulate();
	void Simulate(float dt) ;

	float GetCurFrameDt() const { return m_fCurFrameDt; }
	APhysXU32 GetSimFrameCount() const { return m_uSimFrameCount; }
	
	//---------------------------------------------------------------
	//
	// Beware: call this function once and only once in each frame.
	//
	// when we use APX_SIMULATE_TSM_FIXED simulation method, the real 
	// elapsed time(dt) maybe not use up for physics simulation, 
	// in this case, physics driven motion will be inconsistent with 
	// key frame animation. To avoid this case, we can tick all motion 
	// via the exact simulated time of physics system. To use this
	// method, you should call it each tick.
	float GetExactSimulateTime(float dt, bool bCutOutExtraTime = true);

	// try to call this function once the APhysXScene is created and do not re-set it at runtime...
	void SetSimulateTimeStep(APhysXSimulateTimeStepMethod apxSimulateTimeStepMethod = APX_SIMULATE_TSM_FIXED, float fTimeStep = 1.0f/60.0f, APhysXU32 uMaxIter = 8);
	
	APhysXSimulateTimeStepMethod GetSimulateTimeStepMethod() { return m_SimulateTimeStepMethod; }
	float GetSimulateTimeStep() { return m_fTimeStep; }
	APhysXU32 GetMaxIter() { return m_uMaxIter; }

	bool OnSimulate() { return m_bOnSimulate; }

	void PauseSimulate(bool bPause = true){ m_bPauseSimulate = bPause; }

	void DebugRender(APhysXDebugRenderDesc* pDesc = 0);		// debug render... should be called in Begin render and End render

	void EnableDebugRender(bool bEnableDebugRender = true)
	{
		m_bEnableDebugRender = bEnableDebugRender;
	}

	void EnablePerfStatsRender(bool bEnablePerfStatsRender = true)
	{
		m_bEnablePerfStatsRender = bEnablePerfStatsRender;
	}

	void EnableExtraDebugRender(bool bEnableExtraDebugRender = true)
	{
		m_bEnableExtraDebugRender = bEnableExtraDebugRender;
	}

	//void EnablePerfStatsLog(BYTE bEnablePerfStatsLogMask/*Mask: 0x1-EnableRecord, 0x2-EnablePrint*/)
	//{
	//	m_bEnablePerfStatsLog = bEnablePerfStatsLogMask;
	//}


	bool IsDebugRenderEnabled() const { return m_bEnableDebugRender; }
	bool IsPerfStatsRenderEnabled() const { return m_bEnablePerfStatsRender; }
	bool IsExtraDebugRenderEnabled() const { return m_bEnableExtraDebugRender; }
	
	//bool IsPerfStatsLogEnabled_Record() const { return gPhysXEngine->GetFlag(APX_ENGINEFLAG_ENABLEPROFILE); }
	//bool IsPerfStatsLogEnabled_Print() const { return gPhysXEngine->GetFlag(APX_ENGINEFLAG_LOGPROFILE); }

	bool IsPerfStatsLogEnabled_Record() const;
	bool IsPerfStatsLogEnabled_Print() const;

	int GetMaterialIndex(const APhysXMaterialDesc& materialDesc);

	APhysXPerfStats* GetPerfStats() { return &m_PerfStats; }

	// for APhysXEngine's internal usage...
	void BeginProfiling(const char* szProfName);
	void EndProfiling(const char* szProfName);
	//--------------------------------------------------------------------
	//??? to explain me!!! I think this function only release real physX objects but not
	// other auxiliary content such as material, force field material...
	// reset all status, but not release m_pNxScene!
	//	set bResetTerrain to false if you want to leave the terrain object alone...
	void Reset(bool bResetTerrain = true);

	//--------------------------------------------------------------------
	// Terrain function
	bool InitTerrain(const char* szTerrainConfigFileName);
	void ReleaseTerrain() { m_PhysXTerrain.Release(); }
	APhysXTerrain* GetPhysXTerrain() { return &m_PhysXTerrain; }
	
	void EnableTerrainDebugRender(bool bEnable = true);
	bool IsTerrainDebugRenderEnabled() { return m_bEnableTerrainDebugRender; }
	//--------------------------------------------------------------------
	// Picker function
	void InitPicker(A3DViewport* pA3DViewPort);
	APhysXPicker* GetPhysXPicker() { return &m_PhysXPicker; }


	NxForceFieldMaterial GetRBForceFieldMaterial() { return m_ffMaterialRB; }
	NxForceFieldMaterial GetClothForceFieldMaterial() { return m_ffMaterialCloth; }
	NxForceFieldMaterial GetFluidForceFieldMaterial() { return m_ffMaterialFluid; }
	NxForceFieldMaterial GetSBForceFieldMaterial() { return m_ffMaterialSB; }

	void LockWritingScene()
	{
		m_MutexNxSceneWriting.Lock();
	}

	void UnlockWritingScene()
	{
		m_MutexNxSceneWriting.Unlock();
	}

	APhysXCollisionChannelManager* GetCollisionChannelManager()
	{
		return &m_CollisionChannelManager;
	}

	void SetDefaultCollisionChannel(APhysXCollisionChannel apxCollisionChannel)
	{
		m_DefaultCollisionChannel = apxCollisionChannel;
	}

	APhysXCollisionChannel GetDefaultCollisionChannel()
	{
		return m_DefaultCollisionChannel;
	}

	void EnableCollisionChannel(bool bEnable = true);
	bool IsCollisionChannelEnabled() const { return m_bEnableCollisionChannel; }

	void EnableSubSimulation(bool bEnable = true) { m_bEnableSubSimulation = bEnable; }
	bool IsSubSimulationEnabled() const { return m_bEnableSubSimulation; }

	int GetConstraintClothNum() const { return m_iConstraintClothNum; }

	float GetExactSimDt() const { return m_fExactSimDt; }

#if APHYSX_SUPPORT_COMPARTMENT

	NxCompartment* GetRBCompartment() { return m_pRBCompartment; }
	NxCompartment* GetClothCompartment() { return m_pClothCompartment; }
	NxCompartment* GetFluidCompartment() { return m_pFluidCompartment; }

#endif

	//---------------------------------------------------------
	// try to use gPhysXBreakReport instead...
	// void SetUserBreakReport(APhysXUserBreakReport* pUserBreakReport){ m_pUserBreakReport = pUserBreakReport; }
	
	APhysXUserBreakReport* GetUserBreakReport() { return m_pUserBreakReport; }

	bool ForceMagLimitation(const NxActor& rActor, NxVec3& fInOutForce, NxForceMode forceMode, float fMaxVelChange) const;

	//---------------------------------------------------------
	// add the force to an actor and limit the force not too huge
	void AddLimitForce(NxActor* pActor, const NxVec3& vForce, NxForceMode forceMode = NX_FORCE, float fMaxVelChange = 10.0f);
	void AddLimitForceAtPos(NxActor* pActor, const NxVec3& vForce, const NxVec3& vPos, NxForceMode forceMode = NX_FORCE, float fMaxVelChange = 10.0f);

	//---------------------------------------------------------
	// add force robustly, considering sub-simulation case...
	void AddForce(APhysXForceInfo apxForceInfo);

	//---------------------------------------------------------
	// set actor's pose robustly, considering sub-simulation case...
	void SetActorPose(NxActor* pActor, const NxMat34& mtPose);
	
	void ExeSetActorPose(NxActor* pActor, const NxMat34& mtPose);

	// flush the cached creation and deletion...
	void FlushCreation();
	void FlushDeletion();

protected:
	
	// only can be initialized by APhysXEngine
	APhysXScene()
	{
		m_pNxScene = NULL;

#if APHYSX_SUPPORT_COMPARTMENT
		
		m_pRBCompartment = NULL;
		m_pClothCompartment = NULL;
		m_pFluidCompartment = NULL;

#endif

		m_bPauseSimulate = false;
		m_bOnSimulate = false;

		m_bEnableDebugRender = false;
		m_bEnablePerfStatsRender = false;
		m_bEnableExtraDebugRender = false;
		m_bEnableTerrainDebugRender = false;
		// m_bEnablePerfStatsLog = 0;

		m_pGroundPlaneActor = NULL;

		// init the 4 force field materials the default one of the scene
		m_ffMaterialRB = m_ffMaterialCloth = m_ffMaterialFluid = m_ffMaterialSB = 0;

		m_SimulateTimeStepMethod = APX_SIMULATE_TSM_FIXED;
		m_fTimeStep = 1.0f/60.0f;
		m_uMaxIter = 8;

		m_DefaultCollisionChannel = APX_COLLISION_CHANNEL_COMMON;
		m_bEnableCollisionChannel = true;

		m_fStartTimeStamp = 0.0f;
		m_fSimulateTime = 0.0f;
		m_fSimTimeAccumulator = 0.0f;

		m_pUserBreakReport = NULL;

		m_bEnableSubSimulation = false;
		m_iConstraintClothNum = 0;
		m_fExactSimDt = 0.0f;
	}

	// callback by APhysXEngine...
	bool OnCreate(APhysXEngine* pAPhysXEngine, const APhysXSceneDesc& apxSceneDesc);
	void OnRelease();

	
	void SyncGraphicsData();
	void SyncPhysicsData();

	void UpdateActivePhysXObjects();

	void PreObjectsSimulate(float dt);
	void PostObjectsSimulate(float dt);

	// the real creation and deletion operations occur in this function...
	void TickCreationAndDeletion();

	void IncreConstraintClothNum() { EnableSubSimulation(++m_iConstraintClothNum > 0); }
	void DecreConstraintClothNum() { EnableSubSimulation(--m_iConstraintClothNum > 0); }

	void LogPerfStats();

protected:

	NxScene* m_pNxScene;

#if APHYSX_SUPPORT_COMPARTMENT
	//-----------------------------------------------------
	// add compartments support, wenfeng, April 14, 2009
	
	NxCompartment* m_pRBCompartment;
	NxCompartment* m_pClothCompartment;
	NxCompartment* m_pFluidCompartment;
	
	//-----------------------------------------------------
#endif

	// APhysXObject
	APhysXArray<APhysXObject* > m_arrPhysXObjects;
	APhysXArray<APhysXObject* > m_arrActivePhysXObjects;	// 	PhysXObjects which need to update graphics data from physics simulation

	// APhysXObjectConnectors...
	APhysXArray<APhysXObjectConnector* > m_arrPhysXConnectors;

	// keep a map of PhysX Objects with names
	typedef APhysXMap<APhysXString, APhysXObject*> APhysXObjectMap;
	typedef APhysXObjectMap::value_type APhysXObjectPair;
	APhysXObjectMap m_mapNamedPhysXObjects;

	// Time counting
	float m_fStartTimeStamp;		// global time stamp when starting simulation, not used yet, March 5, 2009
	float m_fSimulateTime;			// the accumulated time after we start the simulation...

	float m_fCurFrameDt;			// the "dt" of current frame
	APhysXU32 m_uSimFrameCount;		// the simulated count of frames
	

	float m_fSimTimeAccumulator;	// for use of compute the exact simulated time of PhysX

	bool m_bPauseSimulate;
	bool m_bOnSimulate;
	
	APhysXSceneUpdateGraphicsType m_SceneUpdateGraphicsType;

	// BYTE m_bEnablePerfStatsLog;
	bool m_bEnableDebugRender;
	bool m_bEnablePerfStatsRender;
	bool m_bEnableExtraDebugRender;
	bool m_bEnableTerrainDebugRender;
	APhysXDebugRenderer m_DebugRenderer;

	APhysXPerfStats m_PerfStats;

	APhysXEngine* m_pAPhysXEngine;

	// keep a list of APhysXMaterials...
	APhysXArray<APhysXMaterialDesc> m_arrMaterialDescs;

	// keep a list of NxActors which are not corresponding to APhysXObject
	APhysXArray<NxActor* > m_arrDummyNxActors;

	NxActor* m_pGroundPlaneActor;

	APhysXTerrain m_PhysXTerrain;
	
	APhysXPicker m_PhysXPicker;

	// force field material for 4 common physics object types, RB, Cloth, Fluid, SB...
	NxForceFieldMaterial m_ffMaterialRB;
	NxForceFieldMaterial m_ffMaterialCloth;
	NxForceFieldMaterial m_ffMaterialFluid;
	NxForceFieldMaterial m_ffMaterialSB;

	// simulate method and parameters...
	APhysXSimulateTimeStepMethod m_SimulateTimeStepMethod;
	float m_fTimeStep;
	APhysXU32 m_uMaxIter;

	// default collision channel...
	APhysXCollisionChannel m_DefaultCollisionChannel;
	APhysXCollisionChannelManager m_CollisionChannelManager;
	bool m_bEnableCollisionChannel;

	//--------------------------------------------------------------
	// APhysX user call back... 
	APhysXUserBreakReport* m_pUserBreakReport;

	//--------------------------------------------------------------
	// revised by wenfeng, Apr. 18, 2012
	// if sub-simulation is enabled, we will split  m_pNxScene->simulate(dt) to
	// n steps and each step will be just simulate(subtimestep)...
	bool m_bEnableSubSimulation;
	int  m_iConstraintClothNum;
	float m_fExactSimDt;

	//----------------------------------------------------------------------------
	// Note: 
	//
	// Dec. 8, Wenfeng
	// Since we may use multi-threading physx objects creation and release, we 
	// should consider the thread-safe way, so we keep a cache creation list 
	// and a deletion list to handle the issue...
	// 
	// Dec 15, Wenfeng
	// To avoid the issue that we may create the null PhysX object(NxActor, NxCloth, NxFluid...)
	// during the write lock phase of NxScene (between simulate() and fetchResults()), we should
	// add mutex lock for NxScene's writing...
	//----------------------------------------------------------------------------	
	
	APhysXArray<APhysXObject* > m_arrCachedCreationObjects;
	APhysXMutex m_MutexCachedCreationObjects;

	APhysXArray<APhysXObject* > m_arrCachedDeletionObjects;
	APhysXMutex m_MutexCachedDeletionObjects;

	APhysXArray<APhysXObjectConnector* > m_arrCachedCreationConnectors;
	APhysXMutex m_MutexCachedCreationConnectors;

	APhysXArray<APhysXObjectConnector* > m_arrCachedDeletionConnectors;
	APhysXMutex m_MutexCachedDeletionConnectors;

	APhysXMutex m_MutexNxSceneWriting;

	APhysXMutex m_MutexMaterialDescs;
};

#endif