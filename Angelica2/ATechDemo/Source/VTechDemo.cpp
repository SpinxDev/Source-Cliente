/*
 * FILE: VTechDemo.cpp
 *
 * DESCRIPTION: An tech demo application object for Angelica Game Engine
 *
 * CREATED BY: Hedi, 2003/1/10
 *
 * HISTORY:
 *
 * Copyright (c) 2002-2008 Archosaur Studio, All Rights Reserved.	
 */

#include "VKeyboardDevice.h"
#include "VTechDemo.h"
#include "VTechDemoKeyTrans.h"
#include "VTechDemoWorld.h"

VTechDemo::VTechDemo() : VGame()
{
	strcpy(m_szClassName, "VTECHDEMO");
	strcpy(m_szWndTitle, "Technique Demo of Angelica Engine");

	m_nWidth = 1024;
	m_nHeight = 768;
	m_nDepth = 32;

	m_bWindowed = false;

	char szCommandLine[1024];

	strncpy(szCommandLine, GetCommandLine(), 1024);
	strlwr(szCommandLine);
	if( strstr(szCommandLine, "-window") )
		m_bWindowed = true;
}

VTechDemo::~VTechDemo()
{
}

bool VTechDemo::Tick()
{
	if( !VGame::Tick() )
		return false;

	//m_BackMusic.Tick();

	return true;
}

bool VTechDemo::RenderContent(A3DViewport * pCurrentViewport)
{
	if( !VGame::RenderContent(pCurrentViewport) )
		return false;

	//m_videoClip.DrawToBack();
	return true;
}

bool VTechDemo::InitWorld()
{
	// Set some engine parameters, including lights
	g_pA3DConfig->SetTextureQuality(A3DTEXTURE_QUALITY_MEDIUM);
	m_pA3DEngine->SetShowFPSFlag(true);
	m_pA3DDevice->SetSpecularEnable(true);

	//m_pA3DDevice->SetFogStart(1800.0f);
	//m_pA3DDevice->SetFogEnd(4000.0f);
	m_pA3DDevice->SetFogStart(10.0f);
	m_pA3DDevice->SetFogEnd(4000.0f);
	m_pA3DDevice->SetFogColor(A3DCOLORRGB(66, 92, 106));

	SetDirectionalColor(255, 255, 255);
	SetAmbient(128, 128, 128);
	//SetDirectionalColor(80, 80, 80);
	//SetAmbient(0, 0, 0);

	// load vworld;
	VTechDemoWorld * pDemoWorld = new VTechDemoWorld();
	if( !pDemoWorld->Init(this) )
	{
		ErrLog("VBikeGame::InitWorld(), Can not init VWorld!");
		return false;
	}
	m_pVWorld = pDemoWorld;

	// Create keyboard device;
	VKeyBoardDevice * pKeyBoard;
	pKeyBoard = new VKeyBoardDevice();

	if( !pKeyBoard->Init(m_hDeviceWnd, this, m_pMsgQueue) )
	{
		ErrLog("VGame::InitWorld(), Init the key board device fail!");
		return false;
	}

	pKeyBoard->SetCursorPosition(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
	m_pInputDevices[m_nInputDeviceNum ++] = pKeyBoard;

	VKeyBoardTranslator * pKeyTranslator;
	pKeyTranslator = new VTechDemoKeyTranslator();
	if( !pKeyTranslator->Init(this, m_pMsgQueue) )
		return false;

	pKeyBoard->ConnectTranslator(pKeyTranslator);

	/*
	if( !m_videoClip.Init(m_pA3DDevice, m_pA3DDevice->GetA3DEngine()->GetAMVideoEngine(), "F:\\Game\\欢乐亿派\\血战上海滩\\video\\intro.mpg") )
		return false;
	m_videoClip.SetAutoScaleFlags(AMVID_FIT_X | AMVID_FIT_Y | AMVID_FIT_KEEPCONSTRAINT);
	m_videoClip.Start();
	*/

	return true;
}

bool VTechDemo::ReleaseWorld()
{
	//m_videoClip.Stop();
	//m_videoClip.Release();

	m_pInputDevices[0]->Release();
	delete m_pInputDevices[0];
	m_pInputDevices[0] = NULL;

	return true;
}

bool VTechDemo::ShowSplash()
{
	return true;
}

bool VTechDemo::ShowEnd()
{
	return true;
}
