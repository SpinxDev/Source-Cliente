#include "APhysXStdAfx.h"
#include "APhysXCharCtrlerMgrImp.h"

// #include <vld.h>		// for debug memory leak...

#include "APhysXRegistryHack.h"


//------------------------------------------------------------------------------
// add NxuStream support...
// revision history:
//		Aug. 3, 2009:  APhysXEngine and NxuStream2 lib are both using DLL version CRT...

#include "..\..\NxuStream2\Include\NXU_helper.h"


//------------------------------------------------------------
// Note by Wenfeng, Aug 3, 2009
// 
// To add extra lib to your lib project in VS2005, we have two ways:
//
//		1. using the following 	#pragma	comment(lib, "NxuStream2_d.lib") option
//
//		2. add lib to your project properties\Libarian\Additional Dependencies\...
//
//	And, 2# is really adding the extra lib to your lib when building your lib project.
//  While, 1# is adding to your lib when linking your lib to some other App projects...
//
//  Beware that VC6 only support the way 1#....
//

#if _MSC_VER < 1400

#ifdef APHYSX_DEBUG

/*
	#ifdef _DLL

		// use the multi-threaded dll runtime lib version 
		#pragma	comment(lib, "NxuStream2_MTD_d.lib")
		
	#else
		
		#pragma	comment(lib, "NxuStream2_d.lib")
		
	#endif
*/

		#pragma	comment(lib, "NxuStream2_d.lib")
#else

/*
	#ifdef _DLL

		// use the multi-threaded dll runtime lib version 
		#pragma	comment(lib, "NxuStream2_MTD.lib")

	#else

		#pragma	comment(lib, "NxuStream2.lib")
	
	#endif
*/

	#pragma	comment(lib, "NxuStream2.lib")

#endif

#endif		// #if _MSC_VER < 1400

//------------------------------------------------------------
// Note by Wenfeng, May 9, 2011
//
// linked lib control...

#if APHYSX_CURRENT_PHYSX_SDK_VERSION >= APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4)

	#ifdef APHYSX_WIN64
		#pragma	comment(lib, "PhysXLoader64.lib")
		#pragma	comment(lib, "PhysXCooking64.lib")
	#else
		#pragma	comment(lib, "PhysXLoader.lib")
		#pragma	comment(lib, "PhysXCooking.lib")
	#endif

#else
	#pragma	comment(lib, "NxCooking.lib")
	#pragma	comment(lib, "PhysXLoader.lib")

#endif
#pragma comment(lib, "shlwapi.lib")
//------------------------------------------------------------

APHYSX_DEFINE_SINGLETON(APhysXEngine)

//------------------------------------------------------------------------------
// static and global variables and functions...


// when time passed every s_fReleaseObsoleteMeshInterval, we will check and release
// some mesh resources...
static float s_fReleaseObsoleteMeshInterval = 3.0f;

static const char* GetNxSDKCreateErrorString(const NxSDKCreateError& errorCode)
{
	switch(errorCode) 
	{
	case NXCE_NO_ERROR: return "NXCE_NO_ERROR";
	case NXCE_PHYSX_NOT_FOUND: return "NXCE_PHYSX_NOT_FOUND";
	case NXCE_WRONG_VERSION: return "NXCE_WRONG_VERSION";
	case NXCE_DESCRIPTOR_INVALID: return "NXCE_DESCRIPTOR_INVALID";
	case NXCE_CONNECTION_ERROR: return "NXCE_CONNECTION_ERROR";
	case NXCE_RESET_ERROR: return "NXCE_RESET_ERROR";
	case NXCE_IN_USE_ERROR: return "NXCE_IN_USE_ERROR";
	default: return "Unknown error";
	}
}



//------------------------------------------------------------------------------
// member functions...
/*
bool APhysXEngine::HasPhysXHardware()
{
	if(m_pNxPhysicsSDK)
	{
		NxHWVersion hwVersion = m_pNxPhysicsSDK->getHWVersion();
		return (m_pNxPhysicsSDK->getHWVersion() != NX_HW_VERSION_NONE);
	}

	return false;

}
*/


bool APhysXEngine::Init(const APhysXEngineDesc& apxEngineDesc)
{

#if defined APHYSX_DLL

	// here, we have to initialize Angelica's working path since maybe we are in DLL mode.
	af_Initialize();

#endif

	// Init log and output stream
	m_pALog = new ALog;
	bool bLogOK = m_pALog->Init("APhysXEngine.log", "======APhysXEngine Log======", false);
	APHYSX_ASSERT(bLogOK);
	if(!bLogOK)
	{
		m_pALog->Release();
		delete m_pALog;
		m_pALog = NULL;
	}

	m_PhysXOutput.InitLog(m_pALog);

	// set Angelica reference...
	m_pA3DEngine = apxEngineDesc.mA3DEngine;
	
	// hack physX core path...
	APhysXRegistryHack apxRegHacker;
	if(apxEngineDesc.mPhysXCorePath)
		apxRegHacker.HackPhysXPath(apxEngineDesc.mPhysXCorePath);

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;

	//-----------------------------------------------------------
	// note: April, 7, 2008, by wenfeng...
	// it has no effect to set the flag NX_SDKF_NO_HARDWARE currently...
	// and this maybe a bug of PhysX SDK 2.8.1...
	if(apxEngineDesc.mDisableHardwarePhysXSupport)
	{
		desc.flags |= NX_SDKF_NO_HARDWARE;
	}
	else
	{
		desc.flags &= ~NX_SDKF_NO_HARDWARE;
	}

	// m_pNxPhysicsSDK = NxCreatePhysicsSDK(APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 1), &m_PhysXAllocator, &m_PhysXOutput, desc, &errorCode);
	m_pNxPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, &m_PhysXAllocator, &m_PhysXOutput, desc, &errorCode);
	// m_pNxPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, NULL, desc, &errorCode);
	if(m_pNxPhysicsSDK == NULL) 
	{
		gPhysXLog->Log("Error: Unable to initialize the PhysX SDK, SDK create error (%d - %s).", errorCode, GetNxSDKCreateErrorString(errorCode));
		return false;
	}

	// check whether HW PhysX is available...
	NxHWVersion hwVersion = m_pNxPhysicsSDK->getHWVersion();
	// m_bIsPhysXHardwareAvailable = ( hwVersion != NX_HW_VERSION_NONE );	
	
	//-----------------------------------------------------------
	// note: April, 7, 2008, by wenfeng...
	// so here, we have to m                                                                                                                                                                                   ake some trick to disable the HW PhysX...
	m_bIsPhysXHardwareAvailable = ( hwVersion != NX_HW_VERSION_NONE ) && (!apxEngineDesc.mDisableHardwarePhysXSupport);

	// connect VRD
	if(apxEngineDesc.mEnableVRD)
	{
		// try to connect VRD
		NxRemoteDebugger* pVRD = m_pNxPhysicsSDK->getFoundationSDK().getRemoteDebugger();
		if( pVRD && ! pVRD->isConnected() )
		{
			pVRD->connect(apxEngineDesc.mVRDHostName);
		}
		else
		{
			gPhysXLog->Log("Warning: Failed on getting Remote Debugger.");
		}

	}

	// NxPhysicsSDK parameters...
	
	//------------------------------------------------------------------------
	// skin width..
	m_pNxPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.005f);

	//------------------------------------------------------------------------
	// visualization parameters...
	//-----
	// Note by wenfeng, Sept. 5, 2012
	// The old setting is not flexible to debug in release mode, so we add an interface
	// to control it at runtime.
	//-----
	// Note by wenfeng, Aug. 13, 2010
	// In some cases, such as only kinematic and static actors in the scene, visualization
	// maybe the bottleneck of the whole simulation. So, we disable the visualization in 
	// release version lib.

#ifdef APHYSX_DEBUG_RENDER
	
	EnableSDKDebugRender(true);

#else

	EnableSDKDebugRender(false);

#endif
	
	//------------------------------------------------------------------------
	// Disables scene locks when creating/releasing meshes.
	// Prevents the SDK from locking all scenes when creating 
	// and releasing triangle meshes, convex meshes, height field meshes,
	// softbody meshes and cloth meshes, which is the default behavior.
	// Can be used to improve parallelism but beware of possible side effects.
	//	
	//	Warning:  Experimental feature.

#define	APHYSX_USE_SDK_PARA_ASYNC_MESH_CREATION
#ifdef	APHYSX_USE_SDK_PARA_ASYNC_MESH_CREATION

	m_pNxPhysicsSDK->setParameter(NX_ASYNCHRONOUS_MESH_CREATION, 1);

#endif

	//------------------------------------------------------------------------
	// max angular velocity..
	// m_pNxPhysicsSDK->setParameter(NX_MAX_ANGULAR_VELOCITY, 100 * APHYSX_PI);

	m_bIsDisablePhysX = apxEngineDesc.mDisablePhysX;
	if (m_bIsDisablePhysX)
	{
		gPhysXLog->Log("Note: Using Disable PhysX mode!");
	}
	else
	{
		m_PhysXMeshCooker.Init(this);
	}

	return true;

}

void APhysXEngine::Release()
{
#if 1	

	for(int i = 0; i < m_arrPhysXScenes.size(); i++)	
	{
		m_arrPhysXScenes[i]->OnRelease();
		delete m_arrPhysXScenes[i];
	}

	m_arrPhysXScenes.clear();
	m_arrActivePhysXScenes.clear();

	m_pCurPhysXScene = NULL;

	if (m_pCCMgrImp)
	{
		APhysXCharCtrlerMgrImp::ReleaseCCM();
		m_pCCMgrImp = NULL;
	}

	//----------------------------------------------------
	// revised by wenfeng, Jan. 18, 2013
	// we have to release ObjectDescManager before release Mesh Cooker
	// since some ObjectDesc keep the pointer of convex meshes...
	m_PhysXObjectDescManager.Release();

	m_PhysXMeshCooker.Release();
	
	if(m_pALog)
	{
		m_pALog->Release();
		delete m_pALog;
		m_pALog = NULL;
	}
	

	// m_pNxPhysicsSDK should be release at last!!!
	NxReleasePhysicsSDK(m_pNxPhysicsSDK);

	//-------------------------------------------------------------
	// Note:  Jan. 5, 2009, by Wenfeng...
	
	// do not call another global object's function in one global
	// object's destruction...
	
	// gGlobalStringManager->Release();
	//-------------------------------------------------------------

	NXU::releasePersistentMemory();

#endif

#if defined APHYSX_DLL
	
	af_Finalize();

#endif

}

APhysXScene* APhysXEngine::CreatePhysXScene(const APhysXSceneDesc& apsDesc)
{
	if(!IsValid()) return NULL;

	APhysXScene* pAPhysXScene = new APhysXScene;
	
	
	if(pAPhysXScene->OnCreate(this, apsDesc))
	{
		// array operation
		m_arrPhysXScenes.push_back(pAPhysXScene);
		if(apsDesc.mActive)
			m_arrActivePhysXScenes.push_back(pAPhysXScene);

		m_pCurPhysXScene = pAPhysXScene;


	}
	else
	{
		delete pAPhysXScene;
		pAPhysXScene = NULL;
	}

	return pAPhysXScene;

}

void APhysXEngine::ReleasePhysXScene(APhysXScene* pAPhysXScene)
{
	pAPhysXScene->OnRelease();

	m_arrPhysXScenes.deleteEntry(pAPhysXScene);
	m_arrActivePhysXScenes.deleteEntry(pAPhysXScene);

	delete pAPhysXScene;
	pAPhysXScene = NULL;
}

void APhysXEngine::Simulate(float dt, bool bSimActiveScenes)
{
	if(!IsValid()) return;

	if (m_pCCMgrImp)
	{
		// internal update for CCM
		if (APX_CCFM_SYNCHRONOUS == m_pCCMgrImp->GetFlushMode())
			m_pCCMgrImp->TickForInternal();
	}
	
	// simulate active scenes' physics
	if(bSimActiveScenes)
	{
		for(int i = 0; i < m_arrActivePhysXScenes.size(); i++)	
		{
			m_arrActivePhysXScenes[i]->Simulate(dt);
		}

	}

	// check and release mesh resources...
	// Note: 
	//		In PhysX Doc says that Avoid release calls while the scene 
	// is simulating (in between simulate() and fetchResults() calls).
	// So, we should lock all scene's writing here...
	//
	//		another way is to set NX_ASYNCHRONOUS_MESH_CREATION to 1, 
	// while, PhysX Doc says that it is just in a Experimental status
	// and maybe incur some possible side effects...
	m_fReleaseObsoleteMeshTimer += dt;
	if(m_fReleaseObsoleteMeshTimer >= s_fReleaseObsoleteMeshInterval)
	{

#ifndef APHYSX_USE_SDK_PARA_ASYNC_MESH_CREATION

		// lock all active scenes' writing
		for(int i = 0; i < m_arrActivePhysXScenes.size(); i++)	
		{
			m_arrActivePhysXScenes[i]->LockWritingScene();
		}

#endif

		m_PhysXMeshCooker.ReleaseObsoleteMesh();

#ifndef APHYSX_USE_SDK_PARA_ASYNC_MESH_CREATION

		// unlock them
		for(int i = 0; i < m_arrActivePhysXScenes.size(); i++)	
		{
			m_arrActivePhysXScenes[i]->UnlockWritingScene();
		}

#endif

		m_fReleaseObsoleteMeshTimer -= s_fReleaseObsoleteMeshInterval;
	}
}

APhysXCharacterControllerManager* APhysXEngine::GetAPhysXCCManager()
{
	if (m_pCCMgrImp)
		return m_pCCMgrImp;
	
	m_pCCMgrImp = APhysXCharCtrlerMgrImp::InitCCM(*this);
	return m_pCCMgrImp;
}

void APhysXEngine::SetNxPhysicsSDKParameter(NxParameter para, float value)
{
	if(m_pNxPhysicsSDK)
	{
		m_pNxPhysicsSDK->setParameter(para, value);
	}
}

float APhysXEngine::GetNxPhysicsSDKParameter(NxParameter para) const
{
	if(m_pNxPhysicsSDK)
	{
		return m_pNxPhysicsSDK->getParameter(para);
	}
	else
		return 0.0f;
}

bool APhysXEngine::CoreDumpPhysX(const char* szFileName, APhysXCoreDumpFileType apxCoreDumpFileType)
{

	if(m_pNxPhysicsSDK)
	{
		NXU::NXU_FileType ft;		
		switch(apxCoreDumpFileType)
		{
		
		case APX_COREDUMP_FT_BIN:
			ft = NXU::FT_BINARY;
			break;
		
		case APX_COREDUMP_FT_XML:
			ft = NXU::FT_XML;		    
			break;

		case APX_COREDUMP_FT_COLLADA:
			ft = NXU::FT_COLLADA;		    
			break;

		default:
			ft = NXU::FT_BINARY;
		    break;
		}

		return NXU::coreDump(m_pNxPhysicsSDK, szFileName, ft);
		

	}
	else
		return false;

}

void APhysXEngine::EnableSDKDebugRender(bool bEnable /*= true*/)
{
	float fValue = bEnable?1.0f:0.0f;

	m_pNxPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, fValue);

	//m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_WORLD_AXES, fValue);
	//m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, fValue);
	//m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AXES, fValue);

	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, fValue);
	//m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AABBS, fValue);

	// for rendering joints...
	// m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_BODY_JOINT_GROUPS, fValue);		// what's the meaning...
	// m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, fValue);
	// m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, fValue);
	//m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_WORLD_AXES, fValue);

	// for rendering fluids...
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_FLUID_POSITION, fValue);
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_FLUID_EMITTERS, fValue);

	// for rendering cloths...
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_CLOTH_MESH, fValue);

	// for rendering force fields...
	// m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_FORCE_FIELDS, fValue);

}