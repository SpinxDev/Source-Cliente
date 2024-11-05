/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXENGINE_H_
#define _APHYSXENGINE_H_


class APHYSX_DLL_EXPORT APhysXEngineDesc
{
public:
	
	// Angelica reference...
	A3DEngine* mA3DEngine;

	// APhysXEngine properties
	
	// VRD
	bool mEnableVRD;
	const char* mVRDHostName;		

	//---------------------------------------------------------------------------
	// specify our own PhysXCorePath
	//
	// Note: this path must be the path where the path "v2.8.1" is laid in, i.e.
	// the application path structure is like this:
	// ..\Bin
	// ..\Bin\App.exe
	// ..\Bin\PhysXLoader.dll
	// ..\Bin\PhysX\v2.8.1\PhysXCore.dll
	// ...
	// And we should set mPhysXCorePath to:  "..\\Bin\\PhysX"
	// 
	const char* mPhysXCorePath;

	// disable the hardware PhysX support or not...
	// for case of double or triple running instances of an application.
	bool mDisableHardwarePhysXSupport;

	// Disable the whole PhysX system. Theoretically, APhysXEngine and APhysXScene should not be created.
	// But, outside codes(GfxCommon) maybe depend on APhysXScene. So, the work around solution is that 
	// APhysXEngine and APhysXScene can be created. And any objects(RBs, Clothes...) creation are prohibited.
	// And simulation is not executed also.
	bool mDisablePhysX;

	APhysXEngineDesc()
	{
		mA3DEngine = NULL;

		mEnableVRD = false;
		mVRDHostName = "localhost";	

		mPhysXCorePath = NULL;

		mDisableHardwarePhysXSupport = false;
		mDisablePhysX = false;
	}

};

enum APhysXEngineFlag
{
	APX_ENGINEFLAG_ENABLEPROFILE				=	1 << 0,
	APX_ENGINEFLAG_LOGPROFILE					=	1 << 1,
};

class APHYSX_DLL_EXPORT APhysXEngine
{

//----------------------------------------------------------
// singleton object
	
	APHYSX_DECLARE_SINGLETON(APhysXEngine)
	
	APhysXEngine()
	{
		m_pNxPhysicsSDK = NULL;

		m_pA3DEngine = NULL;
		m_pCCMgrImp  = NULL;
		m_pCurPhysXScene = NULL;

		m_pALog = NULL;

		m_fReleaseObsoleteMeshTimer = 0;
		
		m_bEnableHardwarePhysX = false;
		m_bIsPhysXHardwareAvailable = false;
		
		m_uFlags = 0;
	}

//----------------------------------------------------------

public:
	
	~APhysXEngine() { Release(); }

	bool Init(const APhysXEngineDesc& apxEngineDesc);
	void Release();

	//----------------------------------------------------------------------
	// by default, call this Simulate() function will simulate
	// all active scenes automatically. While, in some case,
	// maybe the user want to simulate each scene passing different dt(s),
	// in this case, they can call pPhysXScene->Simulate(dt) explicitly,
	// and then, call gPhysXEngine->Simulate(dt, false)
	void Simulate(float dt, bool bSimActiveScenes = true);

	APhysXScene* CreatePhysXScene(const APhysXSceneDesc& apsDesc);
	void ReleasePhysXScene(APhysXScene* pAPhysXScene);

	NxPhysicsSDK* GetNxPhysicsSDK() { return m_pNxPhysicsSDK; }
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }

	APhysXCharacterControllerManager* GetAPhysXCCManager();

	bool HasPhysXHardware() const { return m_bIsPhysXHardwareAvailable; }

	void SetNxPhysicsSDKParameter(NxParameter para, float value);
	float GetNxPhysicsSDKParameter(NxParameter para) const;

	// Is PhysX Engine valid? In most cases, the returned false means that
	// PhysX runtime dlls has been installed in user's machine...
	bool IsValid() { return (m_pNxPhysicsSDK != NULL); }

	// get function

	// need to NULL test...
	ALog* GetLog() { return m_pALog; }

	APhysXAllocator* GetAllocator() { return &m_PhysXAllocator; }
	APhysXOutputStream* GetOutputStream() { return &m_PhysXOutput; }
	APhysXMeshCooker* GetMeshCooker() { return &m_PhysXMeshCooker; }
	APhysXObjectDescManager* GetObjectDescManager() { return &m_PhysXObjectDescManager; } 

	APhysXScene* GetAPhysXScene(int i) { return m_arrPhysXScenes[i]; }

	enum APhysXCoreDumpFileType
	{
		APX_COREDUMP_FT_BIN,
		APX_COREDUMP_FT_XML,
		APX_COREDUMP_FT_COLLADA,

	};

	// core dump all physX data to a file...
	bool CoreDumpPhysX(const char* szFileName, APhysXCoreDumpFileType apxCoreDumpFileType = APX_COREDUMP_FT_BIN);

	// for Hardware PhysX support...
	void EnableHardwarePhysX(bool bEnable = true) { m_bEnableHardwarePhysX = bEnable; }
	bool IsHardwarePhysXEnabled() const { return m_bEnableHardwarePhysX; }

	// enable/disable the SDK switch of debug render...
	void EnableSDKDebugRender(bool bEnable = true);

	bool IsPhysXDisbaled() const { return m_bIsDisablePhysX; }

	void RaiseFlag(APhysXEngineFlag flag)
	{
		m_uFlags |= flag;
	}

	void ClearFlag(APhysXEngineFlag flag)
	{
		m_uFlags &= ~flag;
	}

	bool GetFlag(APhysXEngineFlag flag) const
	{
		return (m_uFlags & flag)? true : false;
	}

protected:

	// APhysXScene 
	APhysXArray<APhysXScene* > m_arrPhysXScenes;
	APhysXArray<APhysXScene* > m_arrActivePhysXScenes;		// the scenes which should be simulated...
	APhysXScene* m_pCurPhysXScene;

	// PhysX SDK properties
	NxPhysicsSDK* m_pNxPhysicsSDK;

	// Angelica reference...
	A3DEngine* m_pA3DEngine;

	// APhysX CC manager
	APhysXCharCtrlerMgrImp* m_pCCMgrImp;

	// log stream
	ALog* m_pALog;

	APhysXOutputStream m_PhysXOutput;

	// memory allocator
	APhysXAllocator m_PhysXAllocator;

	// mesh cooker
	APhysXMeshCooker m_PhysXMeshCooker;

	// PhysX Object Desc manager
	APhysXObjectDescManager m_PhysXObjectDescManager;

	//bool m_bValid;

	// m_fReleaseObsoleteMeshTimer is counted to check and release
	// the mesh resources...
	float m_fReleaseObsoleteMeshTimer;

	// for Hardware PhysX support...
	// now, only AGEIA PPU and NVIDIA Geforce 8 series and above GPU can make PhysX accelerated...
	bool m_bEnableHardwarePhysX;
	bool m_bIsPhysXHardwareAvailable;
	bool m_bIsDisablePhysX;
	
	APhysXU32 m_uFlags;
};


// some global reference

#define gPhysXEngine (APhysXEngine::GetPtrInstance())

// no test needed...
#define gPhysXLog if(gPhysXEngine->GetLog()) gPhysXEngine->GetLog()

#endif
