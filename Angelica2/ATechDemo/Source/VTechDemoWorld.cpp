/*
 * FILE: VTechDemoWorld.cpp
 *
 * DESCRIPTION: class representing the technique demo world
 *
 * CREATED BY: Hedi, 2003/1/10
 *
 * HISTORY:
 *
 * Copyright (c) 2002-2008 Archosaur Studio, All Rights Reserved.	
 */

#include "VTechDemoWorld.h"
#include "VTechDemo.h"
#include "VTechDemoMsg.h"

VTechDemoWorld::VTechDemoWorld()
{
	m_pA3DEngine		= NULL;
	m_pA3DWorld			= NULL;
	m_pIBLScene			= NULL;

	m_pModelWeapon		= NULL;
	m_pModelGrenade		= NULL;
	m_pSpriteBuffers	= NULL;
	m_pSpriteBullets	= NULL;
}

VTechDemoWorld::~VTechDemoWorld()
{
}

bool VTechDemoWorld::Init(VGame * pGame)
{
	VWorld::Init(pGame);

	m_pA3DEngine = m_pGame->GetA3DEngine();

	m_pA3DWorld = new A3DWorld();
	if( !m_pA3DWorld->Init(m_pA3DEngine->GetA3DDevice(), "shanghai.scn") )
		return false;

	A3DSkySphere * pSphereSky;
	pSphereSky = new A3DSkySphere();
	if( !pSphereSky->Init(m_pA3DEngine->GetA3DDevice(), pGame->GetA3DCamera(), 
		//"SphereSky\\Cap.bmp", "SphereSky\\SurroundF.bmp", "SphereSky\\SurroundB.bmp") )
		"shanghai\\1\\roof.bmp", "shanghai\\1\\01.bmp", "shanghai\\1\\02.bmp") )
		//"shanghai\\2\\roof.bmp", "shanghai\\2\\01.bmp", "shanghai\\2\\02.bmp") )
		//"shanghai\\3\\roof.bmp", "shanghai\\3\\01.bmp", "shanghai\\3\\02.bmp") )
		//"shanghai\\4\\roof.bmp", "shanghai\\4\\01.bmp", "shanghai\\4\\02.bmp") )
		//"shanghai\\5\\roof.bmp", "shanghai\\5\\01.bmp", "shanghai\\5\\02.bmp") )
		return false;

	m_pA3DWorld->SetA3DSky(pSphereSky);

	// Load ibl scene here;
	m_pIBLScene = new A3DIBLScene();
	AFileImage fileImage;

	if( !fileImage.Open("models\\maps\\shanghai\\shanghai.ils", AFILE_OPENEXIST) )
		return false;

	if( !m_pIBLScene->Load(m_pA3DEngine->GetA3DDevice(), &fileImage) )
		return false;

	fileImage.Close();

	// Load a weapon
	if( !m_pA3DEngine->GetA3DModelMan()->LoadModelFile("weapon\\ÊÖÇ¹\\ÊÖÇ¹.mod", &m_pModelWeapon) )
		return false;

	if( !m_pA3DEngine->GetA3DModelMan()->LoadModelFile("weapon\\ÊÖÁñµ¯\\ÊÖÁñµ¯.mod", &m_pModelGrenade) )
		return false;

	m_pModelGrenade->SetDirAndUp(A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pModelGrenade->SetPos(A3DVECTOR3(10.0f, 2.0f, 10.0f));

	// Load bullets sprite here;
	A3DRECT rect;
	rect.left = 0;
	rect.right = 30;
	rect.top = 0;
	rect.bottom = 9;
	m_pSpriteBullets = new A2DSprite();
	if( NULL == m_pSpriteBullets )
		return false;
	if( !m_pSpriteBullets->Init(m_pA3DEngine->GetA3DDevice(), "³å·æÇ¹×Óµ¯.tga", 30, 9, 0, 1, &rect) )
		return false;

	m_pSpriteBuffers = new A2DSpriteBuffer();
	if( !m_pSpriteBuffers->Init(m_pA3DEngine->GetA3DDevice(), m_pSpriteBullets, 30) )
		return false;

	// Now load a robot here;
	if( !m_theRobot.Init(m_pA3DEngine->GetA3DDevice(), "Character\\Ö÷ÈË¹«_Ó²¶¯»­.mod") )
		return false;

	m_theRobot.SetPos(A3DVECTOR3(1.0f, 0.0f, 2.0f));
	g_pGame->GetA3DCamera()->SetPos(m_theRobot.GetEyePos());
	//m_pA3DEngine->GetA3DDevice()->SetGammaLevel(200);
	return true;
}

bool VTechDemoWorld::Release()
{
	m_theRobot.Release();

	//m_pA3DEngine->GetA3DDevice()->SetGammaLevel(100);
	if( m_pSpriteBuffers )
	{
		m_pSpriteBuffers->Release();
		delete m_pSpriteBuffers;
		m_pSpriteBuffers = NULL;
	}
	if( m_pSpriteBullets )
	{
		m_pSpriteBullets->Release();
		delete m_pSpriteBullets;
		m_pSpriteBullets = NULL;
	}

	if( m_pModelWeapon )
	{
		m_pA3DEngine->GetA3DModelMan()->ReleaseModel(m_pModelWeapon);
		m_pModelWeapon = NULL;
	}

	if( m_pModelGrenade )
	{
		m_pA3DEngine->GetA3DModelMan()->ReleaseModel(m_pModelGrenade);
		m_pModelGrenade = NULL;
	}

	if( m_pIBLScene )
	{
		m_pIBLScene->Release();
		delete m_pIBLScene;
		m_pIBLScene = NULL;
	}

	if( m_pA3DWorld )
	{
		m_pA3DWorld->Release();
		delete m_pA3DWorld;
		m_pA3DWorld = NULL;
	}

	VWorld::Release();
	return true;
}

bool VTechDemoWorld::Tick()
{
	VWorld::Tick();

	// Now tick controls on these bicycles;
	if( !TickControls() )
		return false;

	return true;
}

bool VTechDemoWorld::RenderBeforeEngineScene(A3DViewport * pCurrentViewport)
{
	return true;
}

bool VTechDemoWorld::RenderAfterEngineScene(A3DViewport * pCurrentViewport)
{
	if( m_pIBLScene )
		m_pIBLScene->Render(pCurrentViewport, A3DSCENE_RENDER_SOLID | A3DSCENE_RENDER_ALPHA);

	A3DCamera * pCamera = pCurrentViewport->GetCamera();
	A3DMATRIX4 matCamera = TransformMatrix(pCamera->GetDir(), pCamera->GetUp(), pCamera->GetPos());

	A3DVECTOR3 vecPos = A3DVECTOR3(0.2f, -1.64999f, 0.0f);
	vecPos = vecPos * matCamera;
	m_pModelWeapon->SetPos(vecPos);

	matCamera._41 = matCamera._42 = matCamera._43 = 0.0f;
	A3DVECTOR3		vecDir, vecUp, vecRight, vecRightNew;
	vecDir = Normalize(A3DVECTOR3(-0.2f, 0.0f, 0.9f)) * matCamera;
	vecRight = Normalize(CrossProduct(pCamera->GetUp(), vecDir));
	vecUp = Normalize(CrossProduct(vecDir, vecRight));

	m_pModelWeapon->SetDirAndUp(vecDir, vecUp);

	m_pModelWeapon->TickAnimation();
	RenderMainWeapon(pCurrentViewport);

	m_pModelGrenade->SetPos(A3DVECTOR3(10.f, 2.f, 10.f) + A3DVECTOR3(1.0f, 0.0, 1.0f) * (float)sin(m_pGame->GetA3DEngine()->GetEngineTicks() * 0.01f));
	m_pModelGrenade->RotateXRelative(0.1f);
	m_pModelGrenade->TickAnimation();
	m_pModelGrenade->Render(pCurrentViewport);

	// Now draw bullets here;
	for(int i=0; i<30; i++)
	{
		int x = i / 10;
		int y = i % 10;
		m_pSpriteBullets->SetPosition(m_pGame->GetDeviceWidth() - 60 + x * 5, m_pGame->GetDeviceHeight() - (x % 2) * 6 - (10 - y) * 10);
		m_pSpriteBullets->DrawToBuffer(m_pSpriteBuffers);
		//m_pSpriteBullets->DrawToBack();
	}

	m_pSpriteBuffers->FlushBuffer(pCurrentViewport);

	return true;
}

bool VTechDemoWorld::OnMsg(const GAMEMSG& msg, DWORD dwUserData)
{
	float vParam;
	A3DVECTOR3 vecCamDir = g_pGame->GetA3DCamera()->GetDirH();
	A3DVECTOR3 vecCamRight = g_pGame->GetA3DCamera()->GetRightH();
	A3DVECTOR3 vecCamUp = g_pGame->GetA3DCamera()->GetUp();

	switch(msg.dwMsgID)
	{
	case GAMEMSG_DEGDELTA:
		vParam = *(float *)&msg.dwParam1;
		g_pGame->GetA3DCamera()->DegDelta(vParam);
		break;

	case GAMEMSG_PITCHDELTA:
		vParam = *(float *)&msg.dwParam1;
		g_pGame->GetA3DCamera()->PitchDelta(vParam);
		break;

	case GAMEMSG_FORWARD:
		vParam = *(float *)&msg.dwParam1;
		Move(vParam * vecCamDir); 
		//g_pGame->GetA3DCamera()->MoveFront(vParam);
		break;

	case GAMEMSG_BACK:
		vParam = *(float *)&msg.dwParam1;
		Move(-vParam * vecCamDir);
		//g_pGame->GetA3DCamera()->MoveBack(vParam);
		break;

	case GAMEMSG_LEFT:
		vParam = *(float *)&msg.dwParam1;
		Move(-vParam * vecCamRight);
		//g_pGame->GetA3DCamera()->MoveLeft(vParam);
		break;

	case GAMEMSG_RIGHT:
		vParam = *(float *)&msg.dwParam1;
		Move(vParam * vecCamRight);
		//g_pGame->GetA3DCamera()->MoveRight(vParam);
		break;

	case GAMEMSG_CLIBM:
		vParam = *(float *)&msg.dwParam1;
		g_pGame->GetA3DCamera()->Move(A3DVECTOR3(0.0f, vParam, 0.0f));
		break;

	case GAMEMSG_DIVE:
		vParam = *(float *)&msg.dwParam1;
		g_pGame->GetA3DCamera()->Move(A3DVECTOR3(0.0f, -vParam, 0.0f));
		break;

	case GAMEMSG_JUMP:
		m_theRobot.SetVelocityY(5.0f);
		m_pModelWeapon->PauseAllSFX(true);
		break;

	case GAMEMSG_FIRE:
		m_pModelGrenade->SetDirAndUp(m_pGame->GetA3DEngine()->GetActiveCamera()->GetDir(), m_pGame->GetA3DEngine()->GetActiveCamera()->GetUp());
		m_pModelWeapon->PlayActionByName("[¿ª»ð]", true);
		break;
	case GAMEMSG_ZOOMIN:
		break;
	case GAMEMSG_ZOOMOUT:
		break;

	case GAMEMSG_EFFECT_EXPLODE:
		A3DGraphicsFX * pGFX;
		m_pA3DEngine->GetA3DGFXMan()->LoadGFXFromFile("»÷»Ù_µ¯Ò©Ïä.gfx", NULL, NULL, false, &pGFX);
		pGFX->SetPos(g_pGame->GetA3DCamera()->GetPos());
		pGFX->Start(true);
		m_pModelWeapon->PauseAllSFX(false);
		break;

	default:
		return VWorld::OnMsg(msg, dwUserData);
	}

	return true;
}

bool VTechDemoWorld::TickAnimation()
{
	return true;
}

bool VTechDemoWorld::Restart()
{
	return VWorld::Restart();
}

bool VTechDemoWorld::TickControls()
{
	m_theRobot.Move(GetTickTimeSpan());	
	g_pGame->GetA3DCamera()->SetPos(m_theRobot.GetEyePos());
	return true;
}

bool VTechDemoWorld::RenderMainWeapon(A3DViewport * pCurrentViewport)
{
	if( m_pIBLScene )
	{
		A3DVECTOR3		vecLightDir;
		A3DCOLOR		clAmbient;
		A3DCOLORVALUE	clDirectional;

		A3DVECTOR3 vecPos = m_pModelWeapon->GetModelAABB().Center;
		A3DLIGHTPARAM dynamicLightParam;
		m_pIBLScene->GetLightGrid()->GetEquivalentLightInfo(vecPos, &vecLightDir, &clDirectional, &clAmbient, &dynamicLightParam);

		A3DLIGHTPARAM dirParam = m_pGame->GetA3DDirLight()->GetLightparam();
		dirParam.Direction = vecLightDir;
		dirParam.Diffuse = clDirectional;
		m_pGame->GetA3DDirLight()->SetLightParam(&dirParam);

		m_pGame->GetA3DDevice()->SetAmbient(clAmbient);
		m_pGame->GetA3DDevice()->SetTextureColorOP(0, m_pIBLScene->GetLightGrid()->GetColorOP());

		if( dynamicLightParam.Type != A3DLIGHT_FORCE_DWORD )
		{
			m_pGame->GetA3DDynamicLight()->SetLightParam(&dynamicLightParam);
			m_pGame->GetA3DDynamicLight()->TurnOn();
		}
		else
			m_pGame->GetA3DDynamicLight()->TurnOff();
	}

	m_pModelWeapon->Render(pCurrentViewport, true, false);
	m_pGame->GetA3DDevice()->SetTextureColorOP(0, A3DTOP_MODULATE);

	//m_pA3DEngine->FlushMeshCollector(pCurrentViewport);
	return true;
}

bool VTechDemoWorld::Move(const A3DVECTOR3& vecDelta)
{	
	m_theRobot.SetVelocityXZ(vecDelta.x, vecDelta.z);
	return true;
}