/*
 * FILE: Render.cpp
 *
 * DESCRIPTION: Render class
 *
 * CREATED BY: Duyuxin, 2004/4/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "Render.h"

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

CRender	g_Render;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CRender
//
///////////////////////////////////////////////////////////////////////////

CRender::CRender()
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_pA3DCamera	= NULL;
	m_pA3DViewport	= NULL;
	m_pDirLight		= NULL;
	m_pPointLight	= NULL;
	m_pFont			= NULL;
	m_hInstance		= NULL;
	m_hRenderWnd	= NULL;
	m_bEngineOK		= false;
}

/*	Initialize game

	Return true for success, otherwise return false.

	hInst: handle of instance.
	hWnd: handle of render window.
*/
bool CRender::Init(HINSTANCE hInst, HWND hWnd)
{
	m_hInstance		= hInst;
	m_hRenderWnd	= hWnd;

	if (!InitA3DEngine())
	{
		g_Log.Log("CRender::Init, Failed to initialize A3D engine!");
		return false;
	}

	return true;
}

//	Release game
void CRender::Release()
{
	ReleaseA3DEngine();
}

/*	Initialize A3D engine

	Return true for success, otherwise return false.
*/
bool CRender::InitA3DEngine()
{
	//	Init Engine
	if (!(m_pA3DEngine = new A3DEngine()))
	{
		g_Log.Log("CRender::InitA3DEngine: Not enough memory!");
		return false;
	}
	
	RECT Rect;
	GetClientRect(m_hRenderWnd, &Rect);

	m_iRenderWid = Rect.right;
	m_iRenderHei = Rect.bottom;
	
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= !g_bFullScreen;
	devFmt.nWidth		= m_iRenderWid;
	devFmt.nHeight		= m_iRenderHei;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

	DWORD dwDevFlags = 0;//A3DDEV_FORCESOFTWARETL;	//	Force software T&L

	if (!m_pA3DEngine->Init(m_hInstance, m_hRenderWnd, &devFmt, dwDevFlags))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to initialize A3D engine.");
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Add a directional light into scene
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pDirLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
					A3DVECTOR3(0.5f, -0.7f, 0.707f), 0.6f * A3DCOLORVALUE(1.0f, 214/255.0f, 149/255.0f, 1.0f), 
					A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));

	//	Add a directional light into scene
	m_pA3DEngine->GetA3DLightMan()->CreatePointLight(2, &m_pPointLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
					A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f), A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), 
					A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), 10.0f, 0.0f);
	m_pPointLight->TurnOff();

	//	Set some values
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(100, 100, 100));
	m_pA3DDevice->SetFogColor(A3DCOLORRGB(222, 182, 140));
	m_pA3DDevice->SetFogStart(100.0f);
	m_pA3DDevice->SetFogEnd(2000.0f);
	m_pA3DDevice->SetFogEnable(true);

	//	Get system font
	m_pFont = m_pA3DEngine->GetSystemFont();

	//	Create camera
	if (!(m_pA3DCamera = new A3DCamera))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to create camera object!");
		return false;
	}

	if (!m_pA3DCamera->Init(g_Render.GetA3DDevice(), /*DEG2RAD(56.0f)*/DEG2RAD(51.6f), 0.3f, 2000.0f))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to initialize camera.");
		return false;
	}
						    
	m_pA3DCamera->SetPos(g_vOrigin - g_vAxisZ * 10.0f + g_vAxisY * 1.7f);
	m_pA3DCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);

	//	Create main Viewport
	RECT rc;
	::GetClientRect(m_hRenderWnd, &rc);

	if (!m_pA3DDevice->CreateViewport(&m_pA3DViewport, 0, 0, rc.right, rc.bottom,
									0.0f, 1.0f, true, true, 0xff404040))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to create viewport.");
		return false;
	}

	m_pA3DViewport->SetCamera(m_pA3DCamera);

	//	Below line ensure the positions of 3D sounds in world are correct.
	m_pA3DCamera->SetAM3DSoundDevice(m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice());

	m_bEngineOK = true;

	return true;
}

//	Release A3D engine
void CRender::ReleaseA3DEngine()
{
	A3DRELEASE(m_pPointLight);
	A3DRELEASE(m_pDirLight);
	A3DRELEASE(m_pA3DCamera);
	A3DRELEASE(m_pA3DViewport);
	A3DRELEASE(m_pA3DEngine);

	m_bEngineOK = false;
}

/*	Resize device

	Return true for success, otherwise return false.

	iWid, iHei: new size of render area
*/
bool CRender::ResizeDevice(int iWid, int iHei)
{
	if (!m_bEngineOK)
		return true;

	m_pA3DEngine->SetDisplayMode(iWid, iHei, A3DFMT_UNKNOWN, 0, SDM_WIDTH | SDM_HEIGHT);

	m_iRenderWid = iWid;
	m_iRenderHei = iHei;

	ASSERT(m_pA3DViewport && m_pA3DCamera);

	A3DVIEWPORTPARAM Params;
	Params.X		= 0;
	Params.Y		= 0;
	Params.Width	= iWid;
	Params.Height	= iHei;
	Params.MinZ		= 0.0f;
	Params.MaxZ		= 1.0f;

	m_pA3DViewport->SetParam(&Params);

	//	Adjust camera
	float fRatio = (float)iWid / iHei;
	m_pA3DCamera->SetProjectionParam(DEG2RAD(56.0f), 0.1f, 2000.0f, fRatio);

	return true;
}

//	Begin render
bool CRender::BeginRender()
{
	ASSERT(m_pA3DEngine);
	return m_pA3DEngine->BeginRender();
}

//	End render
bool CRender::EndRender()
{
	ASSERT(m_pA3DEngine);

	if (!m_pA3DEngine->EndRender())
		return false;

	return m_pA3DEngine->Present();
}

//	Output text
void CRender::TextOut(int x, int y, const ACHAR* szText, DWORD color)
{
	if (m_pFont)
		m_pFont->TextOut(x, y, szText, color);
}

//	Draw 2D rectangle
void CRender::Draw2DRect(const ARect<int>& rc, DWORD dwCol, bool bFrame)
{
	if (bFrame)
	{
		A3DPOINT2 pt1, pt2;

		//	Top border
		pt1.Set(rc.left, rc.top);
		pt2.Set(rc.right, rc.top);
		g_pA3DGDI->Draw2DLine(pt1, pt2, dwCol);

		//	Left border
		pt1.Set(rc.left, rc.top);
		pt2.Set(rc.left, rc.bottom);
		g_pA3DGDI->Draw2DLine(pt1, pt2, dwCol);

		//	Right border
		pt1.Set(rc.right, rc.top);
		pt2.Set(rc.right, rc.bottom);
		g_pA3DGDI->Draw2DLine(pt1, pt2, dwCol);

		//	Bottom border
		pt1.Set(rc.left, rc.bottom);
		pt2.Set(rc.right, rc.bottom);
		g_pA3DGDI->Draw2DLine(pt1, pt2, dwCol);
	}
	else
	{
		A3DRECT rect(rc.left, rc.top, rc.right, rc.bottom);
		g_pA3DGDI->Draw2DRectangle(rect, dwCol);
	}
}


