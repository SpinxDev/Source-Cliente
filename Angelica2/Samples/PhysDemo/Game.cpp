//	Game.cpp

#include "Global.h"
#include "Game.h"
#include "Render.h"
#include "PhysTerrain.h"
#include "PhysObjectMan.h"
#include "PhysObject.h"

#include <A3DSkySphere.h>
#include <A3DViewport.h>
#include <A3DCamera.h>
#include <A3DSkinMan.h>
#include <A3DEngine.h>
#include <A3DFuncs.h>
#include <A3DSkinRender.h>
#include <A3DDevice.h>
#include <A3DCursor.h>
#include <A3DLitModelRender.h>

#include "APhysX.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define TIME_TICKANIMATION	30		//	ms

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

CGame	g_Game;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static WORD _RadianToChar(float rad)
{
	int a = (int)((rad / A3D_2PI) * 65536.0f + 0.5f);
	return (WORD)a;
}

WORD glb_CompressDirH2(float x, float z)
{
	static const float fInvInter = 65536.0f / 360.0f;

	if (fabs(x) < 0.00001)
	{
		if (z > 0.0f)
			return 16384;
		else
			return 49152;
	}
	else
	{
		float fDeg = RAD2DEG((float)atan2(z, x));
		return (WORD)(fDeg * fInvInter);
	}
}

A3DVECTOR3 glb_DecompressDirH2(WORD wDir)
{
	static const float fInter = 360.0f / 65536.0f;

	float fRad = DEG2RAD(wDir * fInter);
	A3DVECTOR3 v;
	v.x = (float)cos(fRad);
	v.z = (float)sin(fRad);
	v.y = 0.0f;

	return v;
}

void glb_CompressDir2(const A3DVECTOR3& v, WORD& dir0, WORD& dir1)
{
	static const float fInvInter = 65536.0f / 360.0f;

	A3DVECTOR3 vh(v.x, 0.0f, v.z);
	vh.Normalize();
	dir0 = glb_CompressDirH2(vh.x, vh.z);
	
	float fDeg = RAD2DEG((float)acos(v.y));
	dir1 = (WORD)(fDeg * fInvInter);
}

A3DVECTOR3 glb_DecompressDir2(WORD dir0, WORD dir1)
{
	static const float fInter0 = 360.0f / 65536.0f;
	static const float fInter1 = 360.0f / 65536.0f;

	float fRad = DEG2RAD(dir0 * fInter0);
	float fHei = DEG2RAD(dir1 * fInter1);

	A3DVECTOR3 v;
	float p = (float)sin(fHei);
	v.x = p * (float)cos(fRad);
	v.z = p * (float)sin(fRad);
	v.y = (float)cos(fHei);

	return v;
}

WORD _CompressDirH2(float x, float z)
{
	static const float fInvInter = 65536.0f / 360.0f;

	if (fabs(x) < 0.00001)
	{
		if (z > 0.0f)
			return 16384;
		else
			return 49152;
	}
	else
	{
		float fDeg = RAD2DEG((float)atan2(z, x));
		return (WORD)(fDeg * fInvInter);
	}
}

void _CompressDir2(const A3DVECTOR3& v, WORD& dir0, WORD& dir1)
{
	static const float fInvInter = 65536.0f / 360.0f;

	A3DVECTOR3 vh(v.x, 0.0f, v.z);
	vh.Normalize();
	dir0 = _CompressDirH2(vh.x, vh.z);
	
	float fDeg = RAD2DEG((float)acos(v.y));
	dir1 = (WORD)(fDeg * fInvInter + 0.5f);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement GAMESETTING
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CGame
//	
///////////////////////////////////////////////////////////////////////////

CGame::CGame()
{
	m_pTerrain		= NULL;
	m_pObjectMan	= NULL;
	m_pA3DSky		= NULL;
	m_pCursor		= NULL;
	m_pPhysXScene	= NULL;
	m_pPickedObject	= NULL;

	m_dwTickTime	= 0;
	m_bWireMode		= false;
	m_iPhysState	= CPhysObject::PHY_STATE_PURESIMULATE;
}

CGame::~CGame()
{
}

//	Initialize game
bool CGame::Init()
{
	//	Initialize physical engine
	if (!InitPhysX())
	{
		a_LogOutput(1, "CGame::Init, failed to initialzie physical engine !");
		return false;
	}

	//	Load vertex shaders which are needed by skin models
	A3DSkinMan* pA3DSkinMan = g_Render.GetA3DEngine()->GetA3DSkinMan();
	pA3DSkinMan->LoadSkinModelVertexShaders("Configs\\skinmodelvs.cfg");

	//	Load sky
	if (!LoadSky())
		return false;

	//	Initialize physical object manager
	m_pObjectMan = new CPhysObjectMan;

	if (!m_pObjectMan->Init())
		return false;

	//	Initialize cursor
	m_pCursor = new A3DSysCursor;
	if (!m_pCursor->Load(g_Render.GetA3DDevice(), "Cursors\\normal.ani"))
	{
		a_LogOutput(1, "CGame::Init, failed to cursor !");
		delete m_pCursor;
		m_pCursor = NULL;
	}
	else
	{
		g_Render.GetA3DDevice()->SetCursor(m_pCursor);
		g_Render.GetA3DDevice()->ShowCursor(true);
	}

/*	//	Test code..
	if (1)
	{
		A3DMATRIX4 mat = a3d_RotateY(DEG2RAD(10.0f));
		mat *= a3d_RotateX(DEG2RAD(10.0f));
		A3DVECTOR3 vAxis;
		float fAngle;
		WORD dir0, rad;
		WORD dir1;

		A3DQUATERNION quat(mat);
		quat.ConvertToAxisAngle(vAxis, fAngle);
		vAxis.Normalize();
	//	glb_CompressDir2(vAxis, dir0, dir1);
		_CompressDir2(vAxis, dir0, dir1);

		rad = _RadianToChar(fAngle);
		
		vAxis = glb_DecompressDir2(dir0, dir1);
		A3DQUATERNION quat2(vAxis, rad / 65536.0f * A3D_2PI);
		A3DMATRIX4 mat2;
		quat2.ConvertToMatrix(mat2);

		int bb = 0;
	}
*/
	return true;
}

//	Release game
void CGame::Release()
{
	A3DRELEASE(m_pPickedObject);

	if (m_pCursor)
	{
		g_Render.GetA3DDevice()->SetCursor(NULL);
		delete m_pCursor;
		m_pCursor = NULL;
	}

	A3DRELEASE(m_pObjectMan);

	A3DRELEASE(m_pA3DSky);
	A3DRELEASE(m_pTerrain);
	m_strTrnFile.Empty();

	//	Release physical engine
	ReleasePhysX();
}

//	Reset game
bool CGame::Reset()
{
	A3DRELEASE(m_pPickedObject);

	//	Reset object manager
	if (m_pObjectMan)
		m_pObjectMan->Reset();

	//	Release dummy actors
	if (m_pPhysXScene)
		m_pPhysXScene->ReleaseDummyActors();

	A3DRELEASE(m_pTerrain);
	m_strTrnFile.Empty();

	return true;
}

//	Load terrain
bool CGame::LoadTerrain(const char* szFile)
{
	//	Load terrain
	CPhysTerrain* pNewTerrain = new CPhysTerrain;
	if (!pNewTerrain)
	{
		a_LogOutput(1, "CGame::LoadTerrain", "Not enough memory");
		return false;
	}

	if (!pNewTerrain->Init(g_Render.GetA3DEngine(), 100.0f, 100.0f))
	{
		a_LogOutput(1, "CGame::LoadTerrain", "Failed to initialize terrain");
		pNewTerrain->Release();
		return false;
	}

	if (!pNewTerrain->Load(szFile, 0.0f, 0.0f, false))
	{
		a_LogOutput(1, "CGame::LoadTerrain, Failed to load A3DTerrain2.");
		return false;
	}

	pNewTerrain->SetLODType(A3DTerrain2::LOD_NOHEIGHT);
	pNewTerrain->SetActRadius(512.0f);
	pNewTerrain->SetViewRadius(512.0f);

	//	Release old terrain
	A3DRELEASE(m_pTerrain);
	m_strTrnFile.Empty();

	m_pTerrain = pNewTerrain;

	//	Reset camera's position
	A3DVECTOR3 vPos = g_vOrigin;
	vPos.y = m_pTerrain->GetPosHeight(vPos) + 80.0f;
	g_Render.GetCamera()->SetPos(vPos);

	//	Load terrain blocks around camera
	m_pTerrain->SetLoadCenter(vPos);

	//	Record terrain file
	m_strTrnFile = szFile;

	return true;
}

//	Load sky
bool CGame::LoadSky()
{
	//	Sphere type sky
	A3DSkySphere* pSky = new A3DSkySphere;
	if (!pSky)
		return false;

	//	Sky texture file name shouldn't be set here
	if (!pSky->Init(g_Render.GetA3DDevice(), NULL, "Cloudy\\Roof.bmp",
			"Cloudy\\01.bmp", "Cloudy\\02.bmp"))
	{
		a_LogOutput(1, "CGame::LoadSky, Failed to create sphere sky !");
		return false;
	}

	pSky->SetFlySpeedU(0.03f);
	pSky->SetFlySpeedV(0.03f);

	m_pA3DSky = pSky;
	
	//	This enable sky can do animation when A3DEngine::TickAnimation is called
	if (m_pA3DSky)
		g_Render.GetA3DEngine()->SetSky(m_pA3DSky);

	return true;
}

bool CGame::InitPhysX()
{
	//	Initialize physical engine
	APhysXEngineDesc apxEngineDesc;
	apxEngineDesc.mA3DEngine = g_Render.GetA3DEngine();
	if (!gPhysXEngine->Init(apxEngineDesc))
		return false;

	//	Create physical scene
	APhysXSceneDesc apxSceneDesc;
	apxSceneDesc.mEnableDebugRender = true;
	apxSceneDesc.mNxSceneDesc.groundPlane = true;
	apxSceneDesc.mNxSceneDesc.gravity = APHYSX_STD_GRAVITY;
	apxSceneDesc.mDefaultMaterial.mRestitution = 0.3f;
	apxSceneDesc.mDefaultMaterial.mStaticFriction = 0.5f;
	apxSceneDesc.mDefaultMaterial.mDynamicFriction = 0.5f;

	m_pPhysXScene = gPhysXEngine->CreatePhysXScene(apxSceneDesc);

	m_pPhysXScene->StartSimulate();

	return true;
}

void CGame::ReleasePhysX()
{
	if (m_pPhysXScene)
	{
		m_pPhysXScene->EndSimulate();
		gPhysXEngine->ReleasePhysXScene(m_pPhysXScene);
		m_pPhysXScene = NULL;
	}

	gPhysXEngine->Release();
}

//	Load physX terrain data
bool CGame::LoadPhysXTerrain(const char* szFile)
{
	if (!m_pPhysXScene)
		return false;

	//	TODO: Release current PhysX terrain data
	m_pPhysXScene->ReleaseTerrain();

	m_pPhysXScene->InitTerrain(szFile);

	return true;
}

//	Tick routine
bool CGame::Tick(DWORD dwDeltaTime)
{
	if (!IsTerrainInit())
		return true;

	m_dwTickTime = dwDeltaTime;

	//	Tick terrain
	A3DVECTOR3 vCamPos = g_Render.GetCamera()->GetPos();
	m_pTerrain->Update(dwDeltaTime, g_Render.GetCamera(), vCamPos);

	//	Tick physical objects
	m_pObjectMan->Tick(dwDeltaTime);

	if (m_pPickedObject)
		m_pPickedObject->Tick(dwDeltaTime);

	//	Tick physical system
	float fTime = dwDeltaTime * 0.001f;
	gPhysXEngine->Simulate(fTime);

	if (m_pPhysXScene)
	{
		APhysXTerrain* pPhysXTerrain = m_pPhysXScene->GetPhysXTerrain();
		if (pPhysXTerrain)
			pPhysXTerrain->UpdateSimCenter(APhysXConverter::A2N_Vector3(vCamPos));
	}

	//	Sync physical data to graphics data
	m_pObjectMan->Sync();

	if (m_pPickedObject)
		m_pPickedObject->Sync();

	//	A3DEngine::TickAnimation trigger animation of many objects.
	//	For example: A3DSky objects, GFX objects etc.
	static DWORD dwAnimTime = 0;
	if ((dwAnimTime += dwDeltaTime) >= TIME_TICKANIMATION)
	{
		dwAnimTime -= TIME_TICKANIMATION;
		g_Render.GetA3DEngine()->TickAnimation();

		//	Update ear position so that all 3D sounds' positions are correct
		g_Render.GetCamera()->UpdateEar();
	}

	return true;
}

//	Render routine
bool CGame::Render()
{
	//	Clear viewport
	g_Render.ClearViewport();

	if (!IsTerrainInit())
		return true;

	A3DEngine* pA3DEngine = g_Render.GetA3DEngine();
	A3DDevice* pA3DDevice = pA3DEngine->GetA3DDevice();
	A3DViewport* pA3DViewport = g_Render.GetViewport();
	A3DCamera* pA3DCamera = g_Render.GetCamera();

	//	Render sky first
	m_pA3DSky->SetCamera(pA3DCamera);
	m_pA3DSky->Render();

	//	Render terrain
	if (m_bWireMode)
		pA3DDevice->SetFillMode(A3DFILL_WIREFRAME);

	m_pTerrain->Render(pA3DViewport);
	pA3DDevice->SetFillMode(A3DFILL_SOLID);

	//	Render objects
	m_pObjectMan->Render(pA3DViewport);

	if (m_pPickedObject)
		m_pPickedObject->RenderProxy(pA3DViewport);

	//	Flush skin models
	pA3DEngine->GetA3DSkinMan()->GetCurSkinRender()->Render(pA3DViewport);
	//	Flush GFXs
	pA3DEngine->RenderGFX(pA3DViewport, 0xffffffff);

	//	Flush litmodels
	pA3DEngine->GetA3DLitModelRender()->Render(pA3DViewport, false);
	pA3DEngine->GetA3DLitModelRender()->ResetRenderInfo(false);
	pA3DEngine->GetA3DLitModelRender()->RenderAlpha(pA3DViewport);
	pA3DEngine->GetA3DLitModelRender()->ResetRenderInfo(true);

	if (m_pPhysXScene)
		m_pPhysXScene->DebugRender();
	
	return true;
}

//	Show or hide cursor
void CGame::ShowCursor(bool bShow)
{
	g_Render.GetA3DDevice()->ShowCursor(bShow);
}

//	Set picked object
void CGame::SetPickedObject(CPhysObject* pObject)
{
	//	Release old object
	if (m_pPickedObject)
	{
		m_pPickedObject->Release();
		delete m_pPickedObject;
	}

	m_pPickedObject = pObject;
}

//	Set up picked object
bool CGame::SetupPickedObject(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (!m_pPickedObject)
		return false;

	m_pPickedObject->Setup(vPos, vDir, vUp);
	m_pObjectMan->AddPhysObject(m_pPickedObject);
	m_pPickedObject = NULL;

	return true;
}

void CGame::SetPhysState(int iState)
{
	a_Clamp(iState, 0, CPhysObject::NUM_PHY_STATE-1);
	m_iPhysState = iState;
	m_pObjectMan->ChangePhysicalState(iState);
}


