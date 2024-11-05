/*
 * FILE: WndPerspective.cpp
 *
 * DESCRIPTION: Perspective Viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "WndPerspective.h"
#include "MainFrm.h"
#include "Utility.h"
#include "Render.h"
#include "SectorEditorDoc.h"
#include "SceneTexture.h"

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

#define DEFAULTFOV	56.0f

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CPerspectiveWnd
//
///////////////////////////////////////////////////////////////////////////

CPerspectiveWnd::CPerspectiveWnd()
{
	m_pCamera	= NULL;
	m_pViewport	= NULL;
}

//	Initialize viewport
bool CPerspectiveWnd::Init(CRender* pRender, int x, int y, int iWid, int iHei)
{
	if (!CViewportWnd::Init(pRender, x, y, iWid, iHei))
		return false;

	//	Create camera
	if (!(m_pCamera = new A3DCamera()))
	{
		g_Log.Log("CPerspectiveWnd::Init: Failed to create camera object!");
		return false;
	}

	if (!m_pCamera->Init(pRender->GetA3DDevice(), DEG2RAD(DEFAULTFOV), 0.1f, 2000.0f))
	{
		g_Log.Log(0, "CPerspectiveWnd::Init: Failed to initialize camera.");
		return false;
	}
						    
	m_pCamera->SetPos(g_vOrigin);
	m_pCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);

	//	Create main Viewport
	if (!pRender->GetA3DDevice()->CreateViewport(&m_pViewport, x, y, iWid, iHei,
									0.0f, 1.0f, true, true, 0xffffffff))
	{
		g_Log.Log(0, "CPerspectiveWnd::Init: Failed to create viewport.");
		return false;
	}

	m_pViewport->SetCamera(m_pCamera);

	return true;
}

//	Release viewport
void CPerspectiveWnd::Release()
{
	if (m_pCamera)
	{
		m_pCamera->Release();
		delete m_pCamera;
		m_pCamera = NULL;
	}

	if (m_pViewport)
	{
		m_pViewport->Release();
		delete m_pViewport;
		m_pViewport = NULL;
	}

	CViewportWnd::Release();
}

//	Reset window.
bool CPerspectiveWnd::Reset()
{
	if (m_pCamera)
	{
		m_pCamera->SetPos(g_vOrigin);
		m_pCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);
		m_pCamera->SetPitch(0.0f);
		m_pCamera->SetDeg(0.0f);
	}

	return true;
}

//	Move and resize viewport
bool CPerspectiveWnd::Move(int x, int y, int iWid, int iHei)
{
	CViewportWnd::Move(x, y, iWid, iHei);

	ASSERT(m_pViewport && m_pCamera);

	A3DVIEWPORTPARAM Params;
	Params.X		= x;
	Params.Y		= y;
	Params.Width	= iWid;
	Params.Height	= iHei;
	Params.MinZ		= 0.0f;
	Params.MaxZ		= 1.0f;

	m_pViewport->SetParam(&Params);

	//	Adjust camera
	float fRatio = (float)iWid / iHei;
	m_pCamera->SetProjectionParam(DEG2RAD(DEFAULTFOV), 0.1f, 2000.0f, fRatio);

	return true;
}

//	Render viewport
bool CPerspectiveWnd::Render()
{
	if (!m_bRender)
		return true;

	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();

	m_pViewport->Active();
	m_pViewport->ClearDevice();

	//	Render scene
	CTextureScene* pScene = pDoc->GetTextureScene();
	if (pScene && g_Configs.bShowScene)
	{
		pScene->UpdateVisibleSets(m_pViewport);
		pScene->Render(m_pViewport, A3DSCENE_RENDER_SOLID);
		pScene->Render(m_pViewport, A3DSCENE_RENDER_ALPHA);
	}

	//	Flush wire collector
	m_pRender->SetWireRenderStates(true);
	m_pRender->FlushWireCollector();
	m_pRender->SetWireRenderStates(false);

	//	Output potential visible sectors
	if (g_Configs.bTesting && pScene)
	{
		char szMsg[256];
		sprintf(szMsg, "Visible face: %d", pScene->GetNumVisibleFaces());
		
		DWORD dwCol = pScene->GetNumVisibleFaces() > g_Configs.iNumWarnFace ? 0xffff0000 : 0xff00ff00;

		TextOut(10, 10, szMsg, dwCol);
	}

	if (m_bActive)
		DrawActiveFrame();

	return true;
}

//	Mouse action handler
bool CPerspectiveWnd::OnMouseMove(int x, int y, DWORD dwFlags)
{
	return true;
}

bool CPerspectiveWnd::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	return true;
}

bool CPerspectiveWnd::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	return true;
}

bool CPerspectiveWnd::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	return true;
}

bool CPerspectiveWnd::OnRButtonUp(int x, int y, DWORD dwFlags)
{
	return true;
}

bool CPerspectiveWnd::OnLButtonDbClk(int x, int y, DWORD dwFlags)
{
	return true;
}

bool CPerspectiveWnd::OnRButtonDbClk(int x, int y, DWORD dwFlags)
{
	return true;
}

//	Frame move
bool CPerspectiveWnd::FrameMove(DWORD dwTime)
{
	//	Move camera
	MoveCamera(dwTime);
	//	Rotate camera
	RotateCamera(dwTime);

	return true;
} 

//	Move camera
void CPerspectiveWnd::MoveCamera(DWORD dwTime)
{
	A3DVECTOR3 vDirH	= m_pCamera->GetDirH();
	A3DVECTOR3 vRightH	= m_pCamera->GetRightH();
	A3DVECTOR3 vDelta(0.0f);

	if (GetAsyncKeyState('W') & 0x8000)
		vDelta = vDirH;
	else if (GetAsyncKeyState('S') & 0x8000)
		vDelta = -vDirH;
	
	if (GetAsyncKeyState('A') & 0x8000)
		vDelta = vDelta - vRightH;
	else if (GetAsyncKeyState('D') & 0x8000)
		vDelta = vDelta + vRightH;
	
	if (GetAsyncKeyState(VK_HOME) & 0x8000)
		vDelta = vDelta + g_vAxisY;
	else if (GetAsyncKeyState(VK_END) & 0x8000)
		vDelta = vDelta - g_vAxisY;

	float fSpeed = 1.0f + g_Configs.iCameraMove * 16.0f;
	vDelta = Normalize(vDelta) * (fSpeed * (float)dwTime * 0.001f);
	
	m_pCamera->Move(vDelta);
}

//	Rotate camera
void CPerspectiveWnd::RotateCamera(DWORD dwTime)
{
	float fSpeed = 10.0f + g_Configs.iCameraRotate * 50.0f;
	fSpeed *= (float)dwTime * 0.001f;

	float fPitch = 0.0f;
	float fYaw = 0.0f;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		fPitch = -1.0f;
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		fPitch = 1.0f;

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		fYaw = -1.0f;
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		fYaw = 1.0f;

	if (fYaw)
		m_pCamera->DegDelta(fYaw * fSpeed);

	if (fPitch)
		m_pCamera->PitchDelta(fPitch * fSpeed);
}


