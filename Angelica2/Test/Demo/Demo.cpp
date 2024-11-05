                                         /*
 * FILE: TestFont.cpp
 *
 * DESCRIPTION: test font
 *
 * CREATED BY: duyuxin, 2003/12/9
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Render.h"
#include "terrain.h"
#include "trail.h"
#include "modelshadow.h"
#include "fullglow.h"
#include "Camera.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define TIME_TICKANIMATION	30

//	Game mode
enum
{
	GAMEMODE_GHOST = 0,
	GAMEMODE_CAMERATRACK,
	GAMEMODE_CAMERAACT,
};

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

A3DIBLScene*	g_pA3DIBLScene = NULL;		//	IBL scene object
A3DSky*			g_pA3DSky = NULL;			//	Sky object
A3DSkinModel *	g_pSkinModel = NULL;		//	Skin model
A3DSkinModel *	g_pTreeModel = NULL;
A3DPlants *		g_pPlants = NULL;			//	plants

A3DSkinModel *	g_pSoftObject0 = NULL;
A3DSkinModel *	g_pSoftObject1 = NULL;
A3DSkinModel *	g_pSoftObject2 = NULL;
A3DSkinModel *	g_pSoftObject3 = NULL;

A3DCOLORVALUE	g_BaseColor;

A3DModel *		g_pModelWeapon = NULL;

A3DModel *		g_pModelMox	= NULL;
A3DModel *		g_pModelDog = NULL;

A3DSkinModel *	g_pHorseModel = NULL;

HA3DFONT		g_h3DFont;
A3DString		g_testString;

APerlinNoise1D  g_noise;
bool			g_bPaused = false;
AMSoundStream * g_pBackMusic = NULL;		// back music
int				g_iGameMode = GAMEMODE_GHOST;
bool			g_bDrawPrompt = true;

bool			g_bFighting = false;
bool			g_bPlayDone = false;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

static bool _Run();
static bool _LogicRun(DWORD dwTickTime);
static bool _Render();
static void _MoveCamera(DWORD dwTickTime);
static void _RotateCamera(DWORD dwTickTime);
static void _DrawPrompt();
static bool _RenderMainWeapon(A3DViewport * pCurrentViewport);

static bool _InitGame();
static void _ReleaseGame();

//	Load IBL scene
static bool _LoadIBLScene(const char* szSceneFile, const char* szKDTFile);
//	Load sky
static bool _LoadSky(int iSkyType);
//	Load skin model
static bool _LoadSkinModel(const char* szFile, const char* szTreeFile);
//	load plants
static bool _LoadPlants();
//	load back music
static bool _LoadMusic(const char * szFile);

// load and release soft objects.
static bool _LoadSoftObjects();
static bool _ReleaseSoftObjects();

// load horse
static bool _LoadHorse();
static bool _ReleaseHorse();

static POINT _ptCenter;
static POINT _ptOld;

void ActionStopCallBack(A3DSkinModel* pSkinModel, A3DSkinModelAction* pAction, DWORD dwUserData)
{
	if( pSkinModel == g_pSkinModel )
	{
		g_bPlayDone = true;
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, ACHAR* lpCmdLine, int nCmdShow)
{
	//	Set work directory
	GetCurrentDirectory(MAX_PATH, g_szWorkDir);
	af_Initialize();
	af_SetBaseDir(g_szWorkDir);

	//	Open log file
	g_Log.Init("Demo.log", "Angelica Demo");

 	//	Initialize global strings
	MyRegisterClass(hInstance);

	//	Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) 
		return -1;

	if (!g_Render.Init(hInstance, g_hWnd))
	{
		g_Log.Log("Failed to initialize render");
		return -1;
	}

	//	Initialize game data
	if (!_InitGame())
	{
		g_Log.Log("Failed to initialize game");
		return -1;
	}

	//	Now run the loop
	_Run();

	_ReleaseGame();

	//	Release A3DEngine
	g_Render.Release();

	//	Close app's log file
	g_Log.Release();

	return 0L;
}

bool _InitGame()
{
	//	Load vertex shaders which are needed by skin models
	g_Render.GetA3DEngine()->GetA3DSkinMan()->LoadSkinModelVertexShaders("Configs\\skinmodelvs.cfg");
	//	Set directional light which are needed when render skin models
	g_Render.GetA3DEngine()->GetA3DSkinMan()->SetDirLight(g_Render.GetDirLight());
	//	Set dynamic light
	//g_Render.GetA3DEngine()->GetA3DSkinMan()->SetDPointLight(g_Render.GetPointLight());

	//	Load terrain
	if( !LoadTerrain() )
		return false;

	//	Load IBL scene
	if (!_LoadIBLScene("Models\\ÑÝÎä³¡\\ÑÝÎä³¡¾°.ils", "Models\\ÑÝÎä³¡\\ÑÝÎä³¡¾°.kdt"))
		return false;

	//if (!_LoadIBLScene("Models\\ÃÔ¹¬\\ÃÔ¹¬.ils", "Models\\ÃÔ¹¬\\ÃÔ¹¬.kdt"))
	//	return false;

	if( !_LoadSkinModel("Models\\woman\\woman11.SMD", "Models\\tree\\tree.SMD") )
		return false;

	if( !_LoadHorse() )
		return false;

	//	Load sky
	if (!_LoadSky(0))
		return false;

	if( !_LoadPlants() )
		return false;

	//if( !_LoadSoftObjects() )
	//	return false;

	if( !_LoadMusic("music\\bgm01.mp3") )
		return false;

	if( !Trail_Init("textures\\trail\\trail.tga", A3DVECTOR3(-0.8f, 0.0f, 0.0f), A3DVECTOR3(-0.1f, 0.0f, 0.0f)) )
		return false;

	if( !Shadow_Init(A3DVECTOR3(0.0f, -1.0f, 0.0f)) )
		return false;

	//if( !FullGlow_Init() )
	//	return false;

	if (!Camera_Init("Configs\\camera.txt"))
		return false;

	// Load a weapon
	if( !g_Render.GetA3DEngine()->GetA3DModelMan()->LoadModelFile("woman\\mox.mod", &g_pModelWeapon) )
		return false;

	if( !g_Render.GetA3DEngine()->GetA3DModelMan()->LoadModelFile("ÑÝÎä³¡\\ÑÝÎä³¡¾°.mod", &g_pModelMox) )
		return false;

	if( !g_Render.GetA3DEngine()->GetA3DModelMan()->LoadModelFile("Â¹\\giraffem.mod", &g_pModelDog) )
		return false;

	g_pModelDog->SetPos(A3DVECTOR3(50.0f, 0.0f, -10.0f));
	g_pModelDog->Turn(DEG2RAD(-60.0f));
	
	g_noise.Init(256, 2.0f, 32, 0.9f, 5, 0x12345);

	// Now center the mouse
	RECT rectWindow;
	GetWindowRect(g_hWnd, &rectWindow);

	_ptCenter.x = (rectWindow.right + rectWindow.left) >> 1;
	_ptCenter.y = (rectWindow.bottom + rectWindow.top)	>> 1;

	_ptOld = _ptCenter;
	SetCursorPos(_ptCenter.x, _ptCenter.y);
	ShowCursor(false);

	g_BaseColor.r = 1.0f;
	g_BaseColor.g = 1.0f;
	g_BaseColor.b = 1.0f;
	g_BaseColor.a = 1.0f;
	//g_pA3DConfig->RT_SetShowBoundBoxFlag(true);

	g_h3DFont = g_Render.GetA3DEngine()->GetA3DFontMan()->RegisterFontType("Arial Narrow", 36, 0, 'A');
	g_testString.Init(g_Render.GetA3DDevice(), "Type \'/help for a lis. ÏÂÃæÊÇÖÐÎÄ£¬ºÇºÇ¡£", g_h3DFont, true);
	g_Render.GetA3DEngine()->GetA3DDevice()->SetSpecularEnable(true);
	return true;
}

void _ReleaseGame()
{
	ReleaseTerrain();

	A3DRELEASE(g_pPlants);
	A3DRELEASE(g_pSkinModel);
	A3DRELEASE(g_pTreeModel);
	A3DRELEASE(g_pA3DIBLScene);
	A3DRELEASE(g_pA3DSky);

	_ReleaseSoftObjects();
	_ReleaseHorse();

	g_Render.GetA3DEngine()->SetSky(NULL);

	if (g_pBackMusic)
	{
		g_pBackMusic->Stop();
		g_pBackMusic->Release();
		delete g_pBackMusic;
		g_pBackMusic = NULL;
	}

	if( g_pModelWeapon )
	{
		g_Render.GetA3DEngine()->GetA3DModelMan()->ReleaseModel(g_pModelWeapon);
		g_pModelWeapon = NULL;
	}

	if( g_pModelMox )
	{
		g_Render.GetA3DEngine()->GetA3DModelMan()->ReleaseModel(g_pModelMox);
		g_pModelMox = NULL;
	}

	if( g_pModelDog )
	{
		g_Render.GetA3DEngine()->GetA3DModelMan()->ReleaseModel(g_pModelDog);
		g_pModelDog = NULL;
	}

	//FullGlow_Release();
	Shadow_Release();
	Trail_Release();
	//	Finalize AF system
	af_Finalize();
}

bool _Run()
{
	MSG	msg;
	DWORD dwLastTick = a_GetTime();

	while (1)
	{
		DWORD dwTime = a_GetTime();
		DWORD dwTickTime = dwTime - dwLastTick;
		dwLastTick = dwTime;
	//	if (dwTickTime > 50)
	//		dwTickTime = 50;

		_LogicRun(dwTickTime);
		_Render();

		if (dwTickTime < 30)
			Sleep(30 - dwTickTime);

		//	Message tick which will prevent the application hanging up;
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0); 
			if (msg.message == WM_QUIT || msg.message == WM_CLOSE || msg.message == WM_DESTROY)
				goto EXIT;
	
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

EXIT:

	return true;
}

bool _LogicRun(DWORD dwTickTime)
{
	if (g_iGameMode == GAMEMODE_GHOST)
	{
		//	Move camera
		_MoveCamera(dwTickTime);
		//	Rotate camera
		_RotateCamera(dwTickTime);
	}
	else if (g_iGameMode == GAMEMODE_CAMERATRACK)
	{
		Camera_UpdateTrack((int)dwTickTime, g_Render.GetA3DCamera());
	}
	else if (g_iGameMode == GAMEMODE_CAMERAACT)
	{
		Camera_UpdateActions((int)dwTickTime, g_Render.GetA3DCamera());
	}

	//	Update scene's PVS
	if (g_pA3DIBLScene)
		g_pA3DIBLScene->UpdateVisibleSets(g_Render.GetA3DViewport());

	//	A3DEngine::TickAnimation trigger animation of many objects.
	//	For example: A3DSky objects, GFX objects etc.
	static DWORD dwAnimTime = 0;
	if ((dwAnimTime += dwTickTime) >= TIME_TICKANIMATION)
	{
		dwAnimTime -= TIME_TICKANIMATION;
		g_Render.GetA3DEngine()->TickAnimation();
	}

	float t = g_Render.GetA3DEngine()->GetRenderTicks() * 0.1f;
	A3DVECTOR3 vecForce;
	g_noise.GetValue(t, (float *) &vecForce, 3);

	if( g_pSoftObject0 )
	{
		g_pSoftObject0->Update(dwTickTime);
		g_pSoftObject0->AddForceToSuppleMesh(-vecForce * 40.0f, Magnitude(vecForce) * 10.0f);

		t += dwTickTime * 0.001f;
		g_noise.GetValue(t + 1000.0f, (float *) &vecForce, 3);
		g_pSoftObject1->Update(dwTickTime);
		g_pSoftObject1->AddForceToSuppleMesh(-vecForce * 50.0f, Magnitude(vecForce) * 10.0f);

		t += dwTickTime * 0.001f;
		g_noise.GetValue(t + 100.0f, (float *) &vecForce, 3);
		g_pSoftObject2->Update(dwTickTime);
		g_pSoftObject2->AddForceToSuppleMesh(-vecForce * 30.0f, Magnitude(vecForce) * 10.0f);

		t += dwTickTime * 0.001f;
		g_noise.GetValue(t + 100.0f, (float *) &vecForce, 3);
		g_pSoftObject3->Update(dwTickTime);
		g_pSoftObject3->AddForceToSuppleMesh(-vecForce * 40.0f, Magnitude(vecForce) * 10.0f);
	}
	
#define MAT_STEP 0.01f
	if (g_pSkinModel)
	{
		A3DVECTOR3 vecModelPos = g_pSkinModel->GetPos();
		vecModelPos.y = 0.5f;
		float vSpeed = 0.1f;

		if( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
			vSpeed = 0.01f;
		
		A3DHSVCOLORVALUE hsv;
		a3d_RGBToHSV(g_BaseColor.ToRGBAColor(), &hsv);
		if( GetAsyncKeyState(VK_UP) & 0x8000 )
		{
			A3DVECTOR3 vecCamDir = g_Render.GetA3DCamera()->GetDirH();
			g_pSkinModel->Move(vecCamDir * vSpeed);
			//Trail_Add(vecModelPos + g_vAxisY * 0.3f, vecModelPos - g_vAxisY * 0.1f);
		}
		else if( GetAsyncKeyState(VK_DOWN) & 0x8000 )
		{
			A3DVECTOR3 vecCamDir = g_Render.GetA3DCamera()->GetDirH();
			g_pSkinModel->Move(-vecCamDir * vSpeed);
			//Trail_Add(vecModelPos + g_vAxisY * 0.3f, vecModelPos - g_vAxisY * 0.1f);
		}
		else if( GetAsyncKeyState(VK_LEFT) & 0x8000 )
		{
			A3DVECTOR3 vecCamDir = g_Render.GetA3DCamera()->GetRightH();
			if( GetAsyncKeyState(VK_MENU) & 0x8000 )
				g_pSkinModel->RotateY(vSpeed * A3D_PI * 0.1f);
			else
				g_pSkinModel->Move(-vecCamDir * vSpeed);
			//Trail_Add(vecModelPos + g_vAxisY * 0.3f, vecModelPos - g_vAxisY * 0.1f);
		}
		else if( GetAsyncKeyState(VK_RIGHT) & 0x8000 )
		{
			A3DVECTOR3 vecCamDir = g_Render.GetA3DCamera()->GetRightH();
			if( GetAsyncKeyState(VK_MENU) & 0x8000 )
				g_pSkinModel->RotateY(-vSpeed * A3D_PI * 0.1f);
			else
				g_pSkinModel->Move(vecCamDir * vSpeed);
			
			//Trail_Add(vecModelPos + g_vAxisY * 0.3f, vecModelPos - g_vAxisY * 0.1f);
		}
		else if( GetAsyncKeyState(VK_PRIOR) & 0x8000 )
		{
			g_pSkinModel->Move(g_vAxisY * vSpeed);
		}
		else if( GetAsyncKeyState(VK_NEXT) & 0x8000 )
		{
			g_pSkinModel->Move(-g_vAxisY * vSpeed);
		}
		else if( GetAsyncKeyState(VK_NUMPAD1) & 0x8000 )
		{
			g_BaseColor.r -= MAT_STEP;
			if( g_BaseColor.r < 0.0f )
				g_BaseColor.r = 0.0f;
		}
		else if( GetAsyncKeyState(VK_NUMPAD4) & 0x8000 )
		{
			g_BaseColor.r += MAT_STEP;
			if( g_BaseColor.r > 1.0f )
				g_BaseColor.r = 1.0f;
		}
		else if( GetAsyncKeyState(VK_NUMPAD2) & 0x8000 )
		{
			g_BaseColor.g -= MAT_STEP;
			if( g_BaseColor.g < 0.0f )
				g_BaseColor.g = 0.0f;
		}
		else if( GetAsyncKeyState(VK_NUMPAD5) & 0x8000 )
		{
			g_BaseColor.g += MAT_STEP;
			if( g_BaseColor.g > 1.0f )
				g_BaseColor.g = 1.0f;
		}
		else if( GetAsyncKeyState(VK_NUMPAD3) & 0x8000 )
		{
			g_BaseColor.b -= MAT_STEP;
			if( g_BaseColor.b < 0.0f )
				g_BaseColor.b = 0.0f;
		}
		else if( GetAsyncKeyState(VK_NUMPAD6) & 0x8000 )
		{
			g_BaseColor.b += MAT_STEP;
			if( g_BaseColor.b > 1.0f )
				g_BaseColor.b = 1.0f;
		}
		else if( GetAsyncKeyState(VK_NUMPAD7) & 0x8000 )
		{
			hsv.h -= MAT_STEP;
			if( hsv.h < 0.0f )
				hsv.h = 0.0f;
			A3DCOLOR diffuse;
			a3d_HSVToRGB(hsv, &diffuse);
			g_BaseColor = diffuse;
		}
		else if( GetAsyncKeyState(VK_NUMPAD8) & 0x8000 )
		{
			hsv.h += MAT_STEP;
			if( hsv.h > 0.999f )
				hsv.h = 0.999f;
			A3DCOLOR diffuse;
			a3d_HSVToRGB(hsv, &diffuse);
			g_BaseColor = diffuse;
		}

		g_pSkinModel->AddForceToSuppleMesh(-vecForce * 30.0f, Magnitude(vecForce) * 10.0f);
		
		if( !g_bPaused )
			g_pSkinModel->Update(dwTickTime);

		int index;
		A3DBone * pBone = g_pSkinModel->GetSkeleton()->GetBone("Blade_west", &index);

		if( pBone )
			Trail_Add(pBone->GetAbsoluteTM());

		if( g_bFighting && g_bPlayDone )
		{
			g_bFighting = true;
			g_bPlayDone = false;
			g_pSkinModel->PlayActionByName("FUCKALL", 1.0f);
		}
	}

	if( g_pTreeModel )
	{
		g_pTreeModel->Update(dwTickTime);
	}
	
	if( g_pHorseModel )
	{
		g_pHorseModel->Update(dwTickTime);
	}

	if( g_pModelDog )
	{
		g_pModelDog->TickAnimation();
	}

	A3DCamera * pCamera = g_Render.GetA3DCamera();
	A3DMATRIX4 matCamera = TransformMatrix(pCamera->GetDir(), pCamera->GetUp(), pCamera->GetPos());

	A3DVECTOR3 vecPos = A3DVECTOR3(0.2f, -1.64999f, 1.0f);
	vecPos = vecPos * matCamera;
	g_pModelWeapon->SetPos(vecPos);

	matCamera._41 = matCamera._42 = matCamera._43 = 0.0f;
	A3DVECTOR3		vecDir, vecUp, vecRight, vecRightNew;
	vecDir = Normalize(A3DVECTOR3(-0.2f, 0.0f, 0.9f)) * matCamera;
	vecRight = Normalize(CrossProduct(pCamera->GetUp(), vecDir));
	vecUp = Normalize(CrossProduct(vecDir, vecRight));
	g_pModelWeapon->SetDirAndUp(vecDir, vecUp);
	g_pModelWeapon->TickAnimation();

//	g_pSkinModel->SetPos(vecPos - vecRight * 0.5f);
//	g_pSkinModel->SetDirAndUp(vecDir, vecUp);
//	g_pSkinModel->Update(0);

	if( !g_bPaused )
		Trail_Tick(dwTickTime);
	return true;
}

bool _Render()
{
	//	Begin render
	if (!g_Render.BeginRender())
		return false;

	//if( !FullGlow_BeginRender() )
	//	return false;

	A3DViewport* pA3DViewport = g_Render.GetA3DViewport();

	//	Activate viewport and clear it
	pA3DViewport->Active();
	pA3DViewport->ClearDevice();

	//	Render sky first
	if (g_pA3DSky)
	{
		g_pA3DSky->SetCamera(pA3DViewport->GetCamera());
		g_pA3DSky->Render();
	}

	RenderTerrain();

	if( GetAsyncKeyState(VK_INSERT) & 0x8000 )
	{
		if( g_pModelMox )
		{
			g_Render.GetA3DDevice()->SetTextureColorOP(0, A3DTOP_MODULATE2X);
			g_pModelMox->Render(pA3DViewport, false, false);
			g_Render.GetA3DDevice()->SetTextureColorOP(0, A3DTOP_MODULATE);
		}
	}
	else
	{
		if (g_pA3DIBLScene)
		{
			//	Render solid faces in scene
			g_pA3DIBLScene->Render(pA3DViewport, A3DSCENE_RENDER_SOLID);
		
			//	Render alpha faces in scene
			g_pA3DIBLScene->Render(pA3DViewport, A3DSCENE_RENDER_ALPHA);
		}
	}

	if( g_pSoftObject0 )
	{
		g_pSoftObject0->Render(pA3DViewport, false);
		g_pSoftObject1->Render(pA3DViewport, false);
		g_pSoftObject2->Render(pA3DViewport, false);
		g_pSoftObject3->Render(pA3DViewport, false);
	}
	
	//	Register all skin models which need rendering
	if (g_pSkinModel)
	{
	/*	const A3DLIGHTPARAM& LightParams = g_Render.GetDirLight()->GetLightparam();
		A3DCOLORVALUE colAmbient = g_Render.GetA3DDevice()->GetAmbientValue();

		A3DSkinModel::LIGHTINFO LightInfo;

		LightInfo.colAmbient	= colAmbient;	//	Ambient color
		LightInfo.vLightDir		= Normalize(A3DVECTOR3(0.5f, -0.7f, 0.707f));//LightParams.Direction;	//	Direction of directional light
		LightInfo.colDirDiff	= LightParams.Diffuse;		//	Directional light's diffuse color
		LightInfo.colDirSpec	= LightParams.Specular;		//	Directional light's specular color
		LightInfo.bPtLight		= false;		//	false, disable dynamic point light,
		g_pSkinModel->SetLightInfo(LightInfo);
	*/	
		g_pSkinModel->SetLightInfo(g_pA3DIBLScene, true);
		g_pSkinModel->Render(pA3DViewport, false);
	}

	//	Really render all skin models.
	g_Render.GetA3DEngine()->RenderSkinModels(pA3DViewport);
	g_Render.GetA3DEngine()->GetA3DSkinMan()->ResetRenderInfo();

	if (g_pTreeModel)
	{
		const A3DLIGHTPARAM& LightParams = g_Render.GetDirLight()->GetLightparam();
		A3DCOLORVALUE colAmbient = g_Render.GetA3DDevice()->GetAmbientValue();

		A3DSkinModel::LIGHTINFO LightInfo;

		LightInfo.colAmbient	= colAmbient;	//	Ambient color
		LightInfo.vLightDir		= Normalize(A3DVECTOR3(-0.5f, 0.7f, -0.707f));//LightParams.Direction;	//	Direction of directional light
		LightInfo.colDirDiff	= LightParams.Diffuse;		//	Directional light's diffuse color
		LightInfo.colDirSpec	= LightParams.Specular;		//	Directional light's specular color
		LightInfo.bPtLight		= false;		//	false, disable dynamic point light,
		g_pTreeModel->SetLightInfo(LightInfo);
		
		g_pTreeModel->Render(pA3DViewport, false);
		g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
		g_Render.GetA3DEngine()->RenderSkinModels(pA3DViewport);
		g_Render.GetA3DEngine()->GetA3DSkinMan()->ResetRenderInfo();
		g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
	}

	if( g_pPlants )
		g_pPlants->Render(pA3DViewport);

	if( g_pHorseModel )
	{
		const A3DLIGHTPARAM& LightParams = g_Render.GetDirLight()->GetLightparam();
		A3DCOLORVALUE colAmbient = g_Render.GetA3DDevice()->GetAmbientValue();

		A3DSkinModel::LIGHTINFO LightInfo;

		LightInfo.colAmbient	= colAmbient;	//	Ambient color
		LightInfo.vLightDir		= Normalize(A3DVECTOR3(0.5f, -0.7f, 0.707f));//LightParams.Direction;	//	Direction of directional light
		LightInfo.colDirDiff	= LightParams.Diffuse;		//	Directional light's diffuse color
		LightInfo.colDirSpec	= LightParams.Specular;		//	Directional light's specular color
		LightInfo.bPtLight		= false;		//	false, disable dynamic point light,
		g_pHorseModel->SetLightInfo(LightInfo);
		g_pHorseModel->Render(pA3DViewport, false);
	}

	if( g_pModelDog )
	{
		A3DCOLOR color = g_BaseColor.ToRGBAColor();
		g_Render.GetA3DDevice()->GetD3DDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, color);
		g_pModelDog->Render(pA3DViewport);
	}

	//	Really render all skin models.
	g_Render.GetA3DEngine()->RenderSkinModels(pA3DViewport);
	g_Render.GetA3DEngine()->GetA3DSkinMan()->ResetRenderInfo();

	//	Render all GFX
	g_Render.GetA3DEngine()->RenderGFX(pA3DViewport, 0xffffffff);

//	_RenderMainWeapon(pA3DViewport);

  	Trail_Render(pA3DViewport);

	g_Render.GetA3DEngine()->EndRender();
	g_Render.GetA3DEngine()->BeginRender();

	//if( !FullGlow_EndRender() )
	//	return false;
	//if( !FullGlow_GlowPresent() )
	//	return false;

	Shadow_Render(g_pSkinModel, pA3DViewport, 100.0f, DEG2RAD(1.6f), "1XiaNV_Bip01 Pelvis");
	Shadow_Render(g_pTreeModel, pA3DViewport, 800.0f, DEG2RAD(1.0f), "Tree_Bone12");
	Shadow_Render(g_pHorseModel, pA3DViewport, 100.0f, DEG2RAD(1.6f), "Horse_Bip01 Spine1");
	
	if (g_bDrawPrompt)
		_DrawPrompt();

	//g_testString.DrawText(1, 701, A3DCOLORRGBA(0, 0, 0, 255));
	g_testString.DrawText(0, 700, A3DCOLORRGBA(255, 255, 0, 255));
	//	End render
	g_Render.EndRender();

	return true;
}

//	Move camera
void _MoveCamera(DWORD dwTickTime)
{
	A3DCamera* pA3DCamera = g_Render.GetA3DCamera();

	A3DVECTOR3 vDir = pA3DCamera->GetDir();
	A3DVECTOR3 vRight = pA3DCamera->GetRight();
	A3DVECTOR3 vDelta(0.0f);

	if (GetAsyncKeyState('W') & 0x8000)
		vDelta = vDir;
	else if (GetAsyncKeyState('S') & 0x8000)
		vDelta = -vDir;
	
	if (GetAsyncKeyState('A') & 0x8000)
		vDelta = vDelta - vRight;
	else if (GetAsyncKeyState('D') & 0x8000)
		vDelta = vDelta + vRight;
	
	if (GetAsyncKeyState(VK_HOME) & 0x8000)
		vDelta = vDelta + g_vAxisY;
	else if (GetAsyncKeyState(VK_END) & 0x8000)
		vDelta = vDelta - g_vAxisY;

	float fSpeed = 16.0f;
	if( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
		fSpeed *= 0.1f;

	vDelta = Normalize(vDelta) * (fSpeed * (float)dwTickTime * 0.001f);
	
	pA3DCamera->Move(vDelta);

	A3DVECTOR3 vecPos = pA3DCamera->GetPos();
	if( vecPos.y < 0.3f )
		vecPos.y = 0.3f;
	pA3DCamera->SetPos(vecPos);
}

//	Rotate camera
void _RotateCamera(DWORD dwTickTime)
{
	A3DCamera* pA3DCamera = g_Render.GetA3DCamera();

	float fSpeed = 5.0f;
	fSpeed *= (float)dwTickTime * 0.001f;

	float fPitch = pA3DCamera->GetPitch();;
	float fYaw = pA3DCamera->GetDeg();

	POINT ptNew;
	GetCursorPos(&ptNew);
	SetCursorPos(_ptCenter.x, _ptCenter.y);
	
	fPitch += -(ptNew.y - _ptOld.y) * fSpeed;

	if( fPitch > 89.0f )
		fPitch = 89.0f;
	if( fPitch < -89.0f )
		fPitch = -89.0f;

	fYaw += (ptNew.x - _ptOld.x) * fSpeed;
	
	pA3DCamera->SetPitch(fPitch);
	pA3DCamera->SetDeg(fYaw);
}

//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
/*	case WM_ACTIVATEAPP:

		break;
*/
	case WM_DESTROY:
	
		PostQuitMessage(0);
		return 0;

	case WM_SETCURSOR:
		
		SetCursor(NULL);
		break;

	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:

			PostMessage(hWnd, WM_CLOSE, 0, 0);
			return 0;

		case VK_RETURN:
			g_pSkinModel->PlayActionByName("FUCKALL", 1.0f);
			g_bFighting = true;
			g_bPlayDone = false;
			return 0;

		case VK_SPACE:
			g_bPaused = !g_bPaused;
			return 0;

		case VK_F1:
			Camera_AddTrackKey(g_Render.GetA3DCamera());
			return 0;

		case VK_F4:
			Camera_ResetTrack();
			return 0;

		case VK_F3:
			g_iGameMode = GAMEMODE_GHOST;
			return 0;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				char szFile[MAX_PATH];
				sprintf(szFile, "%s\\CameraTrack%d.txt", g_szWorkDir, wParam - '0');
				Camera_SaveTrackToFile(szFile);
			}
			else
			{
				g_iGameMode = GAMEMODE_CAMERATRACK;
				Camera_BeginTrack(wParam - '0');
			}

			return 0;
		}

		case VK_F8:
			g_bDrawPrompt = !g_bDrawPrompt;
			break;

		case VK_F2:

			g_pSkinModel->StopAllActions();
			g_iGameMode = GAMEMODE_CAMERAACT;
			Camera_BeginActions();
			break;
		}
	
		break;

	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED || wParam == SIZE_MAXHIDE || wParam == SIZE_MAXSHOW ||
			wParam == SIZE_RESTORED)
			break;

		if (!g_Render.IsEngineOK())
			break;

		int cx = lParam & 0x0000ffff;
		int cy = (lParam & 0xffff0000) >> 16;
		g_Render.ResizeDevice(cx, cy);

		break;
	}
	}

   return DefWindowProc(hWnd, message, wParam, lParam);
}

//	Load IBL scene
bool _LoadIBLScene(const char* szSceneFile, const char* szKDTFile)
{
	//	Create scene object
	if (!(g_pA3DIBLScene = new A3DIBLScene))
		return false;

	//	Load scene file
	if (!(g_pA3DIBLScene->Load(g_Render.GetA3DEngine(), szSceneFile)))
	{
		g_Log.Log("Failed to load IBL scene file %s", szSceneFile);
		return false;
	}

	//	Create KDT object
	if (szKDTFile)
	{
	/*	if (!(m_pA3DKDTree = new A3DKDTree))
			return false;

		if (!(m_pA3DKDTree->Load(szKDTFile)))
		{
			g_Log.Log("Failed to load KDT file %s", szKDTFile);
			return false;
		}

		m_pA3DKDTree->SetA3DEngine(g_Render.GetA3DEngine());
	*/
	}

	g_Render.GetA3DEngine()->GetA3DSkinMan()->SetColorOP(g_pA3DIBLScene->GetLightGrid()->GetColorOP());
	return true;
}

//	Load sky
bool _LoadSky(int iSkyType)
{
	if (iSkyType == 0)
	{
		//	Sphere type sky
		A3DSkySphere* pSky = new A3DSkySphere;
		if (!pSky)
			return false;

		if (!pSky->Init(g_Render.GetA3DDevice(), NULL, "Demo\\Roof.bmp", 
						"Demo\\01.bmp", "Demo\\02.bmp"))
		{
			g_Log.Log("Failed to create sphere sky !");
			return false;
		}

		g_pA3DSky = pSky;
	}
	else if (iSkyType == 1)
	{
		//	Sky box
		A3DSkyBox* pSky = new A3DSkyBox;
		if (!pSky)
			return false;

		if (!pSky->Init(g_Render.GetA3DDevice(), NULL, "sky_space00.bmp", 
						"sky_space01.bmp", "sky_space02.bmp", "sky_space03.bmp", 
						"sky_space04.bmp", "sky_space05.bmp"))
		{
			g_Log.Log("Failed to create box sky !");
			return false;
		}

		g_pA3DSky = pSky;
	}

	//	This enable sky can do animation when A3DEngine::TickAnimation is called
	if (g_pA3DSky)
		g_Render.GetA3DEngine()->SetSky(g_pA3DSky);

	return true;
}

bool _LoadSkinModel(const char* szFile, const char* szTreeFile)
{
	A3DSkinModel* pModel = new A3DSkinModel;
	if (!pModel)
		return false;

	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		return false;
	}

	if (!pModel->Load(szFile))
	{
		g_Log.Log("Failed to load skin model %s !", szFile);
		delete pModel;
		return false;
	}

	g_pSkinModel = pModel;
	g_pSkinModel->SetActionStopCallback(ActionStopCallBack, 0);

	//	In a real game, skin model should get environment's light infromation every
	//	frame from IBL scene according to it's position in the scene. But in a
	//	tutorial, we simply set the light information here and never change it.
	const A3DLIGHTPARAM& LightParams = g_Render.GetDirLight()->GetLightparam();
	A3DCOLORVALUE colAmbient = g_Render.GetA3DDevice()->GetAmbientValue();

	A3DSkinModel::LIGHTINFO LightInfo;

	LightInfo.colAmbient	= colAmbient;	//	Ambient color
	LightInfo.vLightDir		= Normalize(A3DVECTOR3(0.5f, -0.707f, 0.707f));//LightParams.Direction;	//	Direction of directional light
	LightInfo.colDirDiff	= LightParams.Diffuse;		//	Directional light's diffuse color
	LightInfo.colDirSpec	= LightParams.Specular;		//	Directional light's specular color
	LightInfo.bPtLight		= false;		//	false, disable dynamic point light,
	
	g_pSkinModel->SetLightInfo(LightInfo);

	//	Erase the force generated by position changing.
//	m_pSkinModel->ErasePosEffectOnSuppleMeshes();

	//	Set model's position
	g_pSkinModel->SetPos(A3DVECTOR3(1.0f, 0.0f, -4.0f));
	//	Set model's direction
	g_pSkinModel->SetDirAndUp(g_vAxisZ, g_vAxisY);
	g_pSkinModel->RotateY(-A3D_PI * 3 / 4.0f, true);
	//	Update model
	g_pSkinModel->Update(0);

	// Now load tree
	pModel = new A3DSkinModel;
	if (!pModel)
		return false;

	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		return false;
	}

	if (!pModel->Load(szTreeFile))
	{
		g_Log.Log("Failed to load skin model %s !", szTreeFile);
		delete pModel;
		return false;
	}

	g_pTreeModel = pModel;

	//	Set model's position
	g_pTreeModel->SetPos(g_vOrigin);//A3DVECTOR3(-10.1f, 0.0f, 3.326f));
	//	Set model's direction
	g_pTreeModel->SetDirAndUp(g_vAxisZ, g_vAxisY);
	
	//	Update model
	g_pTreeModel->Update(0);
	g_pTreeModel->PlayActionByName("²¥·Å¶¯×÷", 1.0f);
	return true;
}

bool _LoadPlants()
{
	g_pPlants = new A3DPlants();
	if( !g_pPlants->Init(g_Render.GetA3DDevice()) )
		return false;

	DWORD dwGrass1;
	if( !g_pPlants->AddGrassType("plants\\grass2.mox", &dwGrass1) )
		return false;

	for(int i=0; i<1000; i++)
	{
		GRASSINFO info;
		info.vecPos = A3DVECTOR3(100.0f, 0.0f, -15.0f) + RandDirH() * (rand() % 500) / 10.0f;
		info.vecDir = RandDirH();
		info.vecUp = g_vAxisY;
		info.vecScale = A3DVECTOR3(2.5f) * (50 + rand() % 100) / 100.0f;

		info.vecPos.y = g_pA3DTerrain->GetHeight(info.vecPos);
		g_pPlants->AddGrass(dwGrass1, &info);
	}

	g_pPlants->SetGrassDis1(150.0f);
	g_pPlants->SetGrassDis2(200.0f);

	return true;
}

bool _LoadMusic(const char * szFile)
{
	if (!(g_pBackMusic = new AMSoundStream))
		return false;

	if (!g_pBackMusic->Init(g_Render.GetA3DEngine()->GetAMEngine()->GetAMSoundEngine(), szFile))
	{
		g_Log.Log("Failed to load background music %s.", szFile);
		return false;
	}

	g_pBackMusic->Play();

	return true;
}

void _DrawPrompt()
{
	DWORD dwCol = 0xffffff00;

	g_Render.TextOut(10, 10, _AL("F1: Add camera key"), dwCol);
	g_Render.TextOut(10, 26, _AL("F4: Reset camera key"), dwCol);
	g_Render.TextOut(10, 42, _AL("'0'-'9': Play camera track"), dwCol);
	g_Render.TextOut(10, 58, _AL("CTRL + '0'-'9': Save camera track"), dwCol);
	g_Render.TextOut(10, 74, _AL("F8: Trigger prompt"), dwCol);
	g_Render.TextOut(10, 90, _AL("F2: Camera action"), dwCol);
	g_Render.TextOut(10, 106, _AL("F3: Ghost mode"), dwCol);
}

bool _LoadSoftObjects()
{
	const A3DLIGHTPARAM& LightParams = g_Render.GetDirLight()->GetLightparam();
	A3DCOLORVALUE colAmbient = g_Render.GetA3DDevice()->GetAmbientValue();

	A3DSkinModel::LIGHTINFO LightInfo;

	LightInfo.colAmbient	= colAmbient;	//	Ambient color
	LightInfo.vLightDir		= Normalize(A3DVECTOR3(0.5f, -0.707f, 0.707f));//LightParams.Direction;	//	Direction of directional light
	LightInfo.colDirDiff	= LightParams.Diffuse;		//	Directional light's diffuse color
	LightInfo.colDirSpec	= LightParams.Specular;		//	Directional light's specular color
	LightInfo.bPtLight		= false;		//	false, disable dynamic point light,
	
	A3DSkinModel * pModel;

	// Now load soft objects
	pModel = new A3DSkinModel;
	if (!pModel)
		return false;
	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		return false;
	}
	if (!pModel->Load("Models\\ÑÝÎä³¡\\flag2.SMD"))
	{
		g_Log.Log("Failed to load skin model!");
		delete pModel;
		return false;
	}
	g_pSoftObject0 = pModel;
	g_pSoftObject0->SetPos(g_vOrigin);
	g_pSoftObject0->SetDirAndUp(g_vAxisZ, g_vAxisY);
	g_pSoftObject0->Update(0);
	g_pSoftObject0->SetLightInfo(LightInfo);

	pModel = new A3DSkinModel;
	if (!pModel)
		return false;
	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		return false;
	}
	if (!pModel->Load("Models\\ÑÝÎä³¡\\flag3.SMD"))
	{
		g_Log.Log("Failed to load skin model!");
		delete pModel;
		return false;
	}
	g_pSoftObject1 = pModel;
	g_pSoftObject1->SetPos(g_vOrigin);
	g_pSoftObject1->SetDirAndUp(g_vAxisZ, g_vAxisY);
	g_pSoftObject1->Update(0);
	g_pSoftObject1->SetLightInfo(LightInfo);

	pModel = new A3DSkinModel;
	if (!pModel)
		return false;
	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		return false;
	}
	if (!pModel->Load("Models\\ÑÝÎä³¡\\flag4.SMD"))
	{
		g_Log.Log("Failed to load skin model!");
		delete pModel;
		return false;
	}
	g_pSoftObject2 = pModel;
	g_pSoftObject2->SetPos(g_vOrigin);
	g_pSoftObject2->SetDirAndUp(g_vAxisZ, g_vAxisY);
	g_pSoftObject2->Update(0);
	g_pSoftObject2->SetLightInfo(LightInfo);

	pModel = new A3DSkinModel;
	if (!pModel)
		return false;
	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		return false;
	}
	if (!pModel->Load("Models\\ÑÝÎä³¡\\flag1.SMD"))
	{
		g_Log.Log("Failed to load skin model!");
		delete pModel;
		return false;
	}
	g_pSoftObject3 = pModel;
	g_pSoftObject3->SetPos(g_vOrigin);
	g_pSoftObject3->SetDirAndUp(g_vAxisZ, g_vAxisY);
	g_pSoftObject3->Update(0);
	g_pSoftObject3->SetLightInfo(LightInfo);

	return true;
}

bool _ReleaseSoftObjects()
{
	if( g_pSoftObject0 )
	{
		g_pSoftObject0->Release();
		delete g_pSoftObject0;
		g_pSoftObject0 = NULL;
	}

	if( g_pSoftObject1 )
	{
		g_pSoftObject1->Release();
		delete g_pSoftObject1;
		g_pSoftObject1 = NULL;
	}

	if( g_pSoftObject2)
	{
		g_pSoftObject2->Release();
		delete g_pSoftObject2;
		g_pSoftObject2= NULL;
	}

	if( g_pSoftObject3)
	{
		g_pSoftObject3->Release();
		delete g_pSoftObject3;
		g_pSoftObject3 = NULL;
	}
	return true;
}

bool _LoadHorse()
{
	const A3DLIGHTPARAM& LightParams = g_Render.GetDirLight()->GetLightparam();
	A3DCOLORVALUE colAmbient = g_Render.GetA3DDevice()->GetAmbientValue();

	A3DSkinModel::LIGHTINFO LightInfo;

	LightInfo.colAmbient	= colAmbient;	//	Ambient color
	LightInfo.vLightDir		= Normalize(A3DVECTOR3(0.5f, -0.707f, 0.707f));//LightParams.Direction;	//	Direction of directional light
	LightInfo.colDirDiff	= LightParams.Diffuse;		//	Directional light's diffuse color
	LightInfo.colDirSpec	= LightParams.Specular;		//	Directional light's specular color
	LightInfo.bPtLight		= false;		//	false, disable dynamic point light,

	A3DSkinModel * pModel;

	pModel = new A3DSkinModel;
	if (!pModel)
		return false;
	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		return false;
	}
	if (!pModel->Load("Models\\horse\\horse.SMD"))
	{
		g_Log.Log("Failed to load skin model!");
		delete pModel;
		return false;
	}
	g_pHorseModel = pModel;
	g_pHorseModel->SetPos(A3DVECTOR3(-1.0f, 0.0f, 6.0f));
	g_pHorseModel->SetDirAndUp(g_vAxisZ, g_vAxisY);
	g_pHorseModel->Update(0);
	g_pHorseModel->SetLightInfo(LightInfo);	
	g_pHorseModel->PlayActionByName("Âí¶¯×÷", 1.0f);

	return true;
}

bool _ReleaseHorse()
{
	if( g_pHorseModel )
	{
		g_pHorseModel->Release();
		delete g_pHorseModel;
	
		g_pHorseModel = NULL;
	}

	return true;
}

bool _RenderMainWeapon(A3DViewport * pCurrentViewport)
{
	A3DLIGHTPARAM	dirParamOld = g_Render.GetDirLight()->GetLightparam();
	A3DCOLOR		ambientOld = g_Render.GetA3DDevice()->GetAmbientColor();

	if( g_pA3DIBLScene )
	{
		A3DVECTOR3		vecLightDir;
		A3DCOLOR		clAmbient;
		A3DCOLORVALUE	clDirectional;

		A3DVECTOR3 vecPos = g_pModelWeapon->GetModelAABB().Center;
		A3DLIGHTPARAM dynamicLightParam;
		g_pA3DIBLScene->GetLightGrid()->GetEquivalentLightInfo(vecPos, &vecLightDir, &clDirectional, &clAmbient, &dynamicLightParam);

		A3DLIGHTPARAM dirParam = g_Render.GetDirLight()->GetLightparam();
		dirParam.Direction = vecLightDir;
		dirParam.Diffuse = clDirectional;
		g_Render.GetDirLight()->SetLightParam(dirParam);

		g_Render.GetA3DDevice()->SetAmbient(clAmbient);
		g_Render.GetA3DDevice()->SetTextureColorOP(0, g_pA3DIBLScene->GetLightGrid()->GetColorOP());

		/*
		if( dynamicLightParam.Type != A3DLIGHT_FORCE_DWORD )
		{
			g_Render.GetPointLight()->SetLightParam(dynamicLightParam);
			g_Render.GetPointLight()->TurnOn();
		}
		else
			g_Render.GetPointLight()->TurnOff();
			*/
	}

	g_pModelWeapon->Render(pCurrentViewport, true, false);
	g_Render.GetA3DDevice()->SetTextureColorOP(0, A3DTOP_MODULATE);

	// restore
	g_Render.GetDirLight()->SetLightParam(dirParamOld);
	g_Render.GetA3DDevice()->SetAmbient(ambientOld);

	g_Render.GetA3DEngine()->FlushMeshCollector(pCurrentViewport);
	return true;
}