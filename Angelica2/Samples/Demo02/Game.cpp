//	Game.cpp

#include <windows.h>
#include "Game.h"
#include "Render.h"
#include "DemoMenu.h"
#include "NPC.h"

#include "A3D.h"
#include "AM.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Implement GAMESETTING
//	
///////////////////////////////////////////////////////////////////////////

void WorldRenderForReflected(A3DViewport* pViewport, void * pArg)
{
	CGame* pGame = (CGame*)pArg;
	pGame->RenderForReflected(pViewport);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CGame
//	
///////////////////////////////////////////////////////////////////////////

A3DVECTOR3 CGame::m_vOrigin(0.0f);
A3DVECTOR3 CGame::m_vAxisX(1.0f, 0.0f, 0.0f);
A3DVECTOR3 CGame::m_vAxisY(0.0f, 1.0f, 0.0f);
A3DVECTOR3 CGame::m_vAxisZ(0.0f, 0.0f, 1.0f);

CGame::CGame()
{
	m_pA3DTerrain		= NULL;
	m_pA3DTerrainWater	= NULL;
	m_pA3DSky			= NULL;
	m_pRain				= NULL;
	m_pSnow				= NULL;
	m_pRootMenu			= NULL;
	m_hInstance			= NULL;
	m_hWnd				= NULL;
	m_bInit				= false;
	m_bRightPressed		= false;
	m_dwTickTime		= 0;
}

CGame::~CGame()
{
}

//	Initialize game
bool CGame::Init(HINSTANCE hInst, HWND hWnd)
{
	m_hInstance	= hInst;
	m_hWnd		= hWnd;

	//	Load vertex shaders which are needed by skin models
	A3DSkinMan* pA3DSkinMan = g_Render.GetA3DEngine()->GetA3DSkinMan();
	pA3DSkinMan->LoadSkinModelVertexShaders("Configs\\skinmodelvs.cfg");

	//	Set camera's initialize position
	g_Render.GetCamera()->SetPos(A3DVECTOR3(262.0f, 44.0f, 178.0f));

	//	Load terrain
	if (!LoadTerrain("Maps\\1\\1Render.trn2"))
		return false;

	//	Load terrain water
	if (!LoadTerrainWater("Maps\\1\\1.water"))
		return false;

	//	Load sky
	if (!LoadSky())
		return false;

	//	Load background music
	if (!LoadMusic("Music\\mainmenu.mp3"))
		return false;

	//	Create nature objects
	if (!CreateNatureObjects())
		return false;

	//	Create menu
	if (!CreateMenu())
		return false;

	//	Apply default settings
	m_Settings.ApplyCurSettings();

	//	Load terrain blocks around camera
	m_pA3DTerrain->SetLoadCenter(g_Render.GetCamera()->GetPos());

	//	Create NPC man
	if (!(m_pNPCMan = new CNPCMan))
		return false;

	if (!m_pNPCMan->Init())
	{
		a_LogOutput(1, "CGame::Init, failed to create NPC man");
		return false;
	}

	m_bInit = true;

	return true;
}

//	Release game
void CGame::Release()
{
	if (m_pRootMenu)
	{
		delete m_pRootMenu;
		m_pRootMenu = NULL;
	}

	A3DRELEASE(m_pNPCMan);

	if (m_pBackMusic)
	{
		m_pBackMusic->Stop();
		m_pBackMusic->Release();
		delete m_pBackMusic;
		m_pBackMusic = NULL;
	}

	A3DRELEASE(m_pRain);
	A3DRELEASE(m_pSnow);
	A3DRELEASE(m_pA3DSky);
	A3DRELEASE(m_pA3DTerrainWater);
	A3DRELEASE(m_pA3DTerrain);

	m_bInit = false;
}

//	Load terrain
bool CGame::LoadTerrain(const char* szFile)
{
	//	Load terrain
	if (!(m_pA3DTerrain = new A3DTerrain2))
	{
		a_LogOutput(1, "CGame::LoadTerrain", "Not enough memory");
		return false;
	}

	if (!m_pA3DTerrain->Init(g_Render.GetA3DEngine(), 100.0f, 100.0f))
	{
		a_LogOutput(1, "CGame::LoadTerrain", "Failed to initialize terrain");
		return false;
	}

	if (!m_pA3DTerrain->Load(szFile, 0.0f, 0.0f, false))
	{
		a_LogOutput(1, "CGame::LoadTerrain, Failed to load A3DTerrain2.");
		return false;
	}

	m_pA3DTerrain->SetLODType(A3DTerrain2::LOD_NOHEIGHT);

	return true;
}

//	Load terrain water
bool CGame::LoadTerrainWater(const char* szFile)
{
	//	Load terrain
	if (!(m_pA3DTerrainWater = new A3DTerrainWater))
	{
		a_LogOutput(1, "CGame::LoadTerrainWater", "Not enough memory");
		return false;
	}

	if (!m_pA3DTerrainWater->Init(g_Render.GetA3DDevice(), g_Render.GetViewport(), g_Render.GetCamera(),false, 4000, 4000, true))
	{
		a_LogOutput(1, "CGame::LoadTerrainWater", "Failed to initialize terrain water");
		return false;
	}

	if (!m_pA3DTerrainWater->LoadWaterAreas(szFile))
	{
		a_LogOutput(1, "CGame::LoadTerrainWater", "Failed to load terrain water");
		return false;
	}

//	m_pA3DTerrainWater->SetWaterType(A3DTerrainWater::WATER_TYPE_ALPHA_SIMPLE);
//	m_pA3DTerrainWater->SetCurEdgeHeight(2.0f);

//	m_pA3DTerrainWater->SetWaterType(A3DTerrainWater::WATER_TYPE_GROUND);
//	m_pA3DTerrainWater->SetGroundProp("Textures\\»ÒÉ«ÑÒÊ¯.dds", 0.1f);
//	m_pA3DTerrainWater->SetGroundProp("Textures\\½ðÊôÎèÌ¨µØÃæ04.DDS", 1.0f);
	
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

//	Load music
bool CGame::LoadMusic(const char* szFile)
{
	if (!(m_pBackMusic = new AMSoundStream))
		return false;

	if (!m_pBackMusic->Init(g_Render.GetA3DEngine()->GetAMEngine()->GetAMSoundEngine(), szFile))
	{
		a_LogOutput(1, "CGame::LoadMusic, Failed to load background music %s.", szFile);
		return false;
	}

	m_pBackMusic->Play();

	return true;
}

//	Create nature objects
bool CGame::CreateNatureObjects()
{
	m_pRain = new A3DRain();

	if (!m_pRain || !m_pRain->Init(g_Render.GetA3DDevice(), 1.0f))
	{
		a_LogOutput(1, "CGame::CreateNatureObjects, failed to init rain");
		return false;
	}

	m_pRain->SetHostCamera(g_Render.GetCamera());
	m_pRain->SetEmitterWidth(20.0f);
	m_pRain->SetEmitterLength(20.0f);
	m_pRain->SetEmitterHeight(8.0f);
	m_pRain->SetUseRateMode(20, true);
	m_pRain->SetSize(0.04f);
	m_pRain->SetSizeVariation(30.0f);
	m_pRain->SetSpeed(0.5f);
	m_pRain->SetSpeedVariation(50.0f);
	m_pRain->SetInheritInfluence(0.0f);
	m_pRain->SetLife(20);
	m_pRain->SetTextureMap("rain.tga");
	m_pRain->SetSrcBlend(A3DBLEND_SRCALPHA);
	m_pRain->SetDestBlend(A3DBLEND_INVSRCALPHA);
	m_pRain->CreateRain();
	m_pRain->SetColorNum(1);
	m_pRain->SetColor(0, A3DCOLORRGBA(255, 255, 255, 160));

	m_pSnow = new A3DSnow();
	if (!m_pSnow || !m_pSnow->Init(g_Render.GetA3DDevice(), 1.8f))
	{
		a_LogOutput(1, "CGame::CreateNatureObjects, failed to init snow");
		return false;
	}

	m_pSnow->SetHostCamera(g_Render.GetCamera());
	m_pSnow->SetEmitterWidth(80.0f);
	m_pSnow->SetEmitterLength(80.0f);
	m_pSnow->SetEmitterHeight(40.0f);
	m_pSnow->SetUseRateMode(20, true);
	m_pSnow->SetSize(0.3f);
	m_pSnow->SetSizeVariation(20.0f);
	m_pSnow->SetSpeed(0.5f);
	m_pSnow->SetSpeedVariation(20.0f);
	m_pSnow->SetLife(100);
	m_pSnow->SetTextureMap("snow.bmp");
	m_pSnow->SetSrcBlend(A3DBLEND_ONE);
	m_pSnow->SetDestBlend(A3DBLEND_INVSRCCOLOR);
	m_pSnow->SetSpinTime(100);
	m_pSnow->SetSpinTimeVariation(50);
	m_pSnow->CreateSnow();
	m_pSnow->SetColor(0, A3DCOLORRGBA(160, 160, 160, 160));

	return true;
}

//	Create menu
bool CGame::CreateMenu()
{
	//	Terrain menu
	CDemoMenu* pTrnMenu = new CDemoMenu("[+] Terrain operations:");
	pTrnMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "1 - Water quality", pTrnMenu, (DWORD)menu_trn_OnWaterQuality));
	pTrnMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "2 - Render water", pTrnMenu, (DWORD)menu_trn_OnRenderWater));
	pTrnMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "3 - Far distance", pTrnMenu, (DWORD)menu_trn_OnFarDist));
	pTrnMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "4 - Middle distance", pTrnMenu, (DWORD)menu_trn_OnMiddleDist));
	pTrnMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "5 - Near distance", pTrnMenu, (DWORD)menu_trn_OnNearDist));

	//	Model menu
	CDemoMenu* pModelMenu = new CDemoMenu("[+] NPC operations:");
	pModelMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "1 - Create NPC (1)", pModelMenu, (DWORD)menu_mod_OnCreateCat));
	pModelMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "2 - Create NPC (2)", pModelMenu, (DWORD)menu_mod_OnCreateFox));
	pModelMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "3 - Create NPC (3)", pModelMenu, (DWORD)menu_mod_OnCreateRabbit));

	//	GFX menu
	CDemoMenu* pGFXMenu = new CDemoMenu("[+] GFX operations:");
	pGFXMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "1 - Explode (1)", pGFXMenu, (DWORD)menu_gfx_OnCreateGFX1));
	pGFXMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "2 - Explode (2)", pGFXMenu, (DWORD)menu_gfx_OnCreateGFX2));

	//	Weather menu
	CDemoMenu* pWeatherMenu = new CDemoMenu("[+] Weather operations:");
	pWeatherMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "1 - Cloudy", pWeatherMenu, (DWORD)menu_wea_OnCloudy));
	pWeatherMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "2 - Rain", pWeatherMenu, (DWORD)menu_wea_OnRain));
	pWeatherMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "3 - Snow", pWeatherMenu, (DWORD)menu_wea_OnSnow));

	//	Camera menu
	CDemoMenu* pCameraMenu = new CDemoMenu("[+] Camera operations:");
	pCameraMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "1 - Move fast", pCameraMenu, (DWORD)menu_cam_OnMoveFast));
	pCameraMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "2 - Move normal", pCameraMenu, (DWORD)menu_cam_OnMoveNormal));
	pCameraMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "3 - Move slow", pCameraMenu, (DWORD)menu_cam_OnMoveSlow));
	pCameraMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "4 - Telescope", pCameraMenu, (DWORD)menu_cam_OnTelescope));

	CDemoMenu* pMenu = new CDemoMenu("Main menu:");
	pMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_SUBMENU, "1 - Terrain", pMenu, (DWORD)pTrnMenu));
	pMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_SUBMENU, "2 - NPC", pMenu, (DWORD)pModelMenu));
	pMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_SUBMENU, "3 - GFX", pMenu, (DWORD)pGFXMenu));
	pMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_SUBMENU, "4 - Weather", pMenu, (DWORD)pWeatherMenu));
	pMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_SUBMENU, "5 - Camera", pMenu, (DWORD)pCameraMenu));
	pMenu->AddItem(new CDemoMenuItem(CDemoMenuItem::TYPE_ITEM, "6 - Exit game", pMenu, (DWORD)menu_OnExitGame));

	m_pRootMenu = pMenu;
	CDemoMenu::m_pCurMenu = pMenu;

	return true;
}

//	Move camera
void CGame::MoveCamera(DWORD dwTime)
{
	A3DCamera* pA3DCamera = g_Render.GetCamera();

	A3DVECTOR3 vDirH = pA3DCamera->GetDirH();
	A3DVECTOR3 vRightH = pA3DCamera->GetRightH();
	A3DVECTOR3 vDelta(0.0f);

	if (GetAsyncKeyState('W') & 0x8000)
		vDelta = vDirH;
	else if (GetAsyncKeyState('S') & 0x8000)
		vDelta = -vDirH;
	
	if (GetAsyncKeyState('A') & 0x8000)
		vDelta = vDelta - vRightH;
	else if (GetAsyncKeyState('D') & 0x8000)
		vDelta = vDelta + vRightH;
	
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		vDelta = vDelta + m_vAxisY;
	else if (GetAsyncKeyState('Z') & 0x8000)
		vDelta = vDelta - m_vAxisY;

	float fSpeed = m_Settings.GetCameraSpeed();
	vDelta = Normalize(vDelta) * (fSpeed * (float)dwTime * 0.001f);
	
	pA3DCamera->Move(vDelta);

	//	Adjust camera position so that it won't move into some wrong place
	A3DVECTOR3 vPos = pA3DCamera->GetPos();
	if (m_pA3DTerrain)
	{
		float fHeight = m_pA3DTerrain->GetPosHeight(vPos);
		a_ClampFloor(vPos.y, fHeight + 1.0f);
	}

	if (m_pA3DTerrainWater)
	{
		float fHeight = m_pA3DTerrainWater->GetWaterHeight(vPos);
		float fError = 0.1f;
		if (fabs(vPos.y - fHeight) < fError)
		{
			if (vDelta.y >= 0.0f)
				vPos.y = fHeight + fError;
			else
				vPos.y = fHeight - fError;
		}
	}

	pA3DCamera->SetPos(vPos);
}

//	Tick routine
bool CGame::Tick(DWORD dwDeltaTime)
{
	if (!m_bInit)
		return true;

	m_dwTickTime = dwDeltaTime;

	//	Move camera
	MoveCamera(dwDeltaTime);

	//	Tick terrain
	m_pA3DTerrain->Update(dwDeltaTime, g_Render.GetCamera(), g_Render.GetCamera()->GetPos());

	//	Tick water
	if( m_pA3DTerrainWater)
		m_pA3DTerrainWater->Update(dwDeltaTime);

	//	A3DEngine::TickAnimation trigger animation of many objects.
	//	For example: A3DSky objects, GFX objects etc.
	static DWORD dwAnimTime = 0;
	if ((dwAnimTime += dwDeltaTime) >= TIME_TICKANIMATION)
	{
		dwAnimTime -= TIME_TICKANIMATION;
		g_Render.GetA3DEngine()->TickAnimation();

		m_Settings.GetCurFogCol().Shade(TIME_TICKANIMATION);

		//	Update rain/snow
		if (m_Settings.GetWeather() == CSettings::WEATHER_RAIN)
			m_pRain->TickEmitting();
		else if (m_Settings.GetWeather() == CSettings::WEATHER_SNOW)
			m_pSnow->TickEmitting();

		//	Update ear position so that all 3D sounds' positions are correct
		g_Render.GetCamera()->UpdateEar();
	}

	//	Tick NPC man
	m_pNPCMan->Tick(dwDeltaTime);

	return true;
}

//	Render routine
bool CGame::Render()
{
	//	Clear viewport
	g_Render.ClearViewport();

	if (!m_bInit)
		return true;


	
	//	Test code...
	
		m_pA3DTerrainWater->SetFogWater(0xff404040, 100.0f, 500.0f);


	A3DViewport* pA3DViewport = g_Render.GetViewport();
	A3DCamera* pA3DCamera = g_Render.GetCamera();

	bool bUnderWater;
	if (!m_Settings.GetRenderWaterFlag())
		bUnderWater = false;
	else
		bUnderWater = m_pA3DTerrainWater->IsUnderWater(pA3DCamera->GetPos());

	//	Update fog parameters
	m_Settings.UpdateFogParams(m_dwTickTime);

	//	Render sky first
	m_pA3DSky->SetCamera(pA3DCamera);
	if (!bUnderWater)
		m_pA3DSky->Render();

	//	Render terrain
	if (!bUnderWater)
		m_pA3DTerrain->Render(pA3DViewport);
	else
		m_pA3DTerrain->RenderUnderWater(pA3DViewport, m_pA3DTerrainWater->GetCurCaustTexture(),
						m_pA3DTerrainWater->GetCaustDU(), m_pA3DTerrainWater->GetCaustDV());

	//	Render NPCs
	m_pNPCMan->Render(pA3DViewport);

	//	Flush skin models
	g_Render.GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender()->Render(pA3DViewport);

	//	Render water
	if (m_Settings.GetRenderWaterFlag())
	{
		const A3DLIGHTPARAM& LightParam = g_Render.GetDirectionalLight()->GetLightparam();
		if (!bUnderWater)
		{
			m_pA3DTerrainWater->RenderReflect(LightParam.Direction, pA3DViewport, m_pA3DTerrain, m_pA3DSky, NULL, WorldRenderForReflected, NULL, this, NULL, this, false, m_pA3DTerrain);
			//m_pA3DTerrainWater->BlendToScreenPS(LightParam.Direction, pA3DViewport, true, 0);
		}
		else
		{
			m_pA3DTerrainWater->RenderRefract(LightParam.Direction, pA3DViewport, m_pA3DTerrain, m_pA3DSky, NULL, NULL, this, NULL, this, false);
			//m_pA3DTerrainWater->BlendToScreenPS(LightParam.Direction, pA3DViewport, false, 0);
		}
	}
	
	//	Render rain / snow
	if (!bUnderWater)
	{
		if (m_Settings.GetWeather() == CSettings::WEATHER_RAIN)
			m_pRain->RenderParticles(pA3DViewport);
		else if (m_Settings.GetWeather() == CSettings::WEATHER_SNOW)
			m_pSnow->RenderParticles(pA3DViewport);
	}

	A3DWireCollector* pWC = g_Render.GetA3DEngine()->GetA3DWireCollector();
	pWC->Add3DBox(A3DVECTOR3(235.520f, 23.5103f, 184.189f), m_vAxisZ, m_vAxisY, m_vAxisX, A3DVECTOR3(1.0f), 0xffffff00);
	pWC->Flush();

	A3DFlatCollector* pFC = g_Render.GetA3DEngine()->GetA3DFlatCollector();
	pFC->AddBox_3D(A3DVECTOR3(235.520f, 23.5103f, 184.189f), m_vAxisZ, m_vAxisY, m_vAxisX, A3DVECTOR3(1.0f), 0xffffff00);
	pFC->Flush();

	//	Render menu
	if (CDemoMenu::m_pCurMenu)
		CDemoMenu::m_pCurMenu->Render();

	//	Draw prompts
	DrawPrompts();

	return true;
}

//	Render for reflect
bool CGame::RenderForReflected(A3DViewport* pA3DViewport)
{
	//	Clear viewport
//	g_Render.ClearViewport();

	//	Render NPCs
	m_pNPCMan->Render(pA3DViewport);

	//	Flush skin models
	g_Render.GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender()->Render(pA3DViewport);
	
	A3DWireCollector* pWC = g_Render.GetA3DEngine()->GetA3DWireCollector();
	pWC->Add3DBox(A3DVECTOR3(235.520f, 23.5103f, 184.189f), m_vAxisZ, m_vAxisY, m_vAxisX, A3DVECTOR3(1.0f), 0xffffff00);
	pWC->Flush();

	A3DFlatCollector* pFC = g_Render.GetA3DEngine()->GetA3DFlatCollector();
	pFC->AddBox_3D(A3DVECTOR3(235.520f, 23.5103f, 184.189f), m_vAxisZ, m_vAxisY, m_vAxisX, A3DVECTOR3(1.0f), 0xffffff00);
	pFC->Flush();

	return true;
}

//	Render prompts
void CGame::DrawPrompts()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	int x = rcClient.left + 10;
	int y = rcClient.bottom - 24;
	DWORD dwColor = 0xff00ff00;
	AString str;

	//	Camera position
	A3DVECTOR3 vPos = g_Render.GetCamera()->GetPos();
	str.Format("Camera: %.3f, %.3f, %.3f", vPos.x, vPos.y, vPos.z);
	g_Render.TextOut(x, y, str, dwColor);
	y -= 16;

	//	'New Object' operation
	str = "Press 'N' to create Object: ";
	
	switch (m_Settings.GetNewObjectOpt())
	{
	case CSettings::NEW_OBJ_NONE:		str += "None";		break;
	case CSettings::NEW_OBJ_NPC_CAT:	str += "NPC (1)";	break;
	case CSettings::NEW_OBJ_NPC_FOX:	str += "NPC (2)";	break;
	case CSettings::NEW_OBJ_NPC_RABBIT:	str += "NPC (3)";	break;
	case CSettings::NEW_OBJ_GFX_1:		str += "GFX (1)";	break;
	case CSettings::NEW_OBJ_GFX_2:		str += "GFX (2)";	break;
	}

	g_Render.TextOut(x, y, str, dwColor);
	y -= 16;
}

//	Windows message handler
LRESULT CGame::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInit)
		return DefWindowProc(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		OnLButtonDown(hWnd, wParam, x, y);
		break;
	}
	case WM_LBUTTONUP:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		OnLButtonUp(hWnd, wParam, x, y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		OnRButtonDown(hWnd, wParam, x, y);
		break;
	}
	case WM_RBUTTONUP:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		OnRButtonUp(hWnd, wParam, x, y);
		break;
	}
	case WM_MOUSEMOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		OnMouseMove(hWnd, wParam, x, y);
		break;
	}
	case WM_CHAR:
	{
		if (wParam >= '1' && wParam <= '9')
		{
			//	Select menu item
			int iIndex = wParam - '1';
			if (CDemoMenu::m_pCurMenu && iIndex < CDemoMenu::m_pCurMenu->GetItemNum())
			{
				CDemoMenuItem* pMenuItem = CDemoMenu::m_pCurMenu->GetItem(iIndex);
				pMenuItem->OnEvent();
			}
		}

		break;
	}
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:

			//	Return to root menu
			CDemoMenu::m_pCurMenu = m_pRootMenu;
			break;
		}
	
		break;

	case WM_KEYUP:

		switch (wParam)
		{
		case 'N':
		{
			POINT pt;
			::GetCursorPos(&pt);
			::ScreenToClient(m_hWnd, &pt);

			RECT rc;
			::GetClientRect(m_hWnd, &rc);
			if (pt.x >= 0 && pt.x < rc.right && pt.y >= 0 && pt.y < rc.bottom)
				CreateNewObject(pt.x, pt.y);

			break;
		}
		}
	
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CGame::OnLButtonDown(HWND hWnd, UINT nFlags, int x, int y)
{
	//	Do trace


	return;
}

void CGame::OnLButtonUp(HWND hWnd, UINT nFlags, int x, int y)
{
	return;
}

void CGame::OnMouseMove(HWND hWnd, UINT nFlags, int x, int y)
{
	POINT pt = {x, y};
	::ClientToScreen(hWnd, &pt);

	if (m_bRightPressed)
	{
		//	Rotate camera
		float f1 = 0.1f;
		A3DCamera* pCamera = g_Render.GetCamera();

		if (pt.x - m_ptMouseLast.x)
		{
			float fYaw = (pt.x - m_ptMouseLast.x) * f1;
			pCamera->DegDelta(fYaw);
		}
		
		if (pt.y - m_ptMouseLast.y)
		{
			float fPitch = -(pt.y - m_ptMouseLast.y) * f1;
			pCamera->PitchDelta(fPitch);
		}
	}

	m_ptMouseLast = pt;
}

void CGame::OnRButtonDown(HWND hWnd, UINT nFlags, int x, int y)
{
	m_bRightPressed	= true;
	m_ptMouseLast.x	= x;
	m_ptMouseLast.y = y;
	
	::ClientToScreen(hWnd, &m_ptMouseLast);
	::SetCapture(hWnd);
}

void CGame::OnRButtonUp(HWND hWnd, UINT nFlags, int x, int y)
{
	if (!m_bRightPressed)
		return;

	m_bRightPressed = false;
	::ReleaseCapture();
}

//	Create new object
void CGame::CreateNewObject(int sx, int sy)
{
	int iNewObj = m_Settings.GetNewObjectOpt();
	if (iNewObj == CSettings::NEW_OBJ_NONE)
		return;
	
	//	Do ray trace at first
	if (!m_RayTrace.DoRayTrace(sx, sy, CRayTrace::FILTER_TERRAIN))
		return;

	A3DVECTOR3 vPos = m_RayTrace.m_vHitPos;

	//	Get current new object type
	switch (iNewObj)
	{
	case CSettings::NEW_OBJ_NPC_CAT:

		m_pNPCMan->CreateNPC(vPos, "Models\\Ã¨Å®\\Ã¨Å®.smd");
		break;

	case CSettings::NEW_OBJ_NPC_FOX:

		m_pNPCMan->CreateNPC(vPos, "Models\\ÉµºüÀê\\ÉµºüÀê.smd");
		break;

	case CSettings::NEW_OBJ_NPC_RABBIT:

		m_pNPCMan->CreateNPC(vPos, "Models\\ÍÃ×ÓÂù±ø\\ÍÃ×ÓÂù±ø.smd");
		break;

	case CSettings::NEW_OBJ_GFX_1:
		
		PlayGFX(vPos, "±¬Õ¨_ÄàµØ.gfx");
		Play3DSound(vPos, "sfx\\Tank_Fire02.wav");
		break;

	case CSettings::NEW_OBJ_GFX_2:
		
		PlayGFX(vPos, "±¬Õ¨_È¼ÉÕ.gfx");
		Play3DSound(vPos, "sfx\\Tank_Fire02.wav");
		break;
	}
}


//	Player gfx at specified position
void CGame::PlayGFX(const A3DVECTOR3& vPos, const char* szFile)
{
	A3DGraphicsFX* pGFX = NULL;
	if (!g_Render.GetA3DEngine()->GetA3DGFXMan()->LoadGFXFromFile(szFile, NULL, NULL, false, &pGFX, vPos))
	{
		a_LogOutput(1, "CGame::PlayGFX, Failed to load GFX %s.", szFile);
		return;
	}

	float fScale = a_Random(0.6f, 1.3f);
	pGFX->SetScale(fScale);
	pGFX->Start(true);
}

//	Play 3D-sound at specified position
void CGame::Play3DSound(const A3DVECTOR3& vPos, const char* szFile)
{
	AMSoundBufferMan* pSoundBufMan = g_Render.GetA3DEngine()->GetAMEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
	pSoundBufMan->Play3DAutoSound(0, szFile, vPos, 10.0f, 100.0f);
}
