/*
 * FILE: Render.cpp
 *
 * DESCRIPTION: Render class
 *
 * CREATED BY: Duyuxin, 2003/6/30
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "Render.h"
#include "WireCollector.h"

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
	m_pDirLight		= NULL;
	m_pFont			= NULL;
	m_pStrings		= NULL;
	m_hInstance		= NULL;
	m_hRenderWnd	= NULL;
	m_pWireCollect	= NULL;
	m_bEngineOK		= false;
	m_bWireRender	= false;
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
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_iRenderWid;
	devFmt.nHeight		= m_iRenderHei;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

	if (!m_pA3DEngine->Init(m_hInstance, m_hRenderWnd, &devFmt, A3DDEV_ALLOWMULTITHREAD))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to initialize A3D engine.");
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Add a directional light into scene
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pDirLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
					A3DVECTOR3(0.0f, -0.7f, 1.0f), A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f), 
					A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));

	//	Set some values
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(150, 150, 180));
	m_pA3DDevice->SetFogEnable(false);

	m_pFont = m_pA3DEngine->GetSystemFont();

	//	Create pre-created text strings
	if (!CreatePreText(m_pFont->GetA3DFont()))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to call CreatePreText().");
		return false;
	}

	//	Create wire collector
	if (!(m_pWireCollect = new CWireCollector))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to create wire collector.");
		return false;
	}

	if (!m_pWireCollect->Init(this))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to initialize wire collector.");
		return false;
	}

	m_bEngineOK = true;

	return true;
}

//	Release A3D engine
void CRender::ReleaseA3DEngine()
{
	if (m_pWireCollect)
	{
		m_pWireCollect->Release();
		delete m_pWireCollect;
		m_pWireCollect = NULL;
	}

	if (m_pStrings)
	{
		m_pStrings->Release();
		delete m_pStrings;
		m_pStrings = NULL;
	}

	if (m_pDirLight)
	{
		m_pDirLight->Release();
		delete m_pDirLight;
		m_pDirLight = NULL;
	}

	if (m_pA3DEngine)
	{
		m_pA3DEngine->Release();
		delete m_pA3DEngine;
		m_pA3DEngine = NULL;
	}

	m_pFont		 = NULL;
	m_pA3DDevice = NULL;
}

/*	Resize device

	Return true for success, otherwise return false.

	iWid, iHei: new size of render area
*/
bool CRender::ResizeDevice(int iWid, int iHei)
{
	if (!m_pA3DEngine)
		return false;

	m_pA3DEngine->SetDisplayMode(iWid, iHei, A3DFMT_UNKNOWN, false, false, SDM_WIDTH | SDM_HEIGHT);

	m_iRenderWid = iWid;
	m_iRenderHei = iHei;

	return true;
}

//	Begin render
bool CRender::BeginRender(DWORD dwClearCol)
{
	ASSERT(m_pA3DEngine);

	//	In order to clear whole screen, viewport parameters must be set
	A3DVIEWPORTPARAM View;
	
	View.X		= 0;
	View.Y		= 0;
	View.Width	= m_iRenderWid;
	View.Height	= m_iRenderHei;
	View.MinZ	= 0.0f;
	View.MaxZ	= 1.0f;

	m_pA3DDevice->SetViewport(&View);
	m_pA3DDevice->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, dwClearCol, 1.0f, 0);

	return m_pA3DEngine->BeginRender();
}

//	End render
bool CRender::EndRender()
{
	ASSERT(m_pA3DEngine);

	//	Reset wire collector buffer
	if (m_pWireCollect)
		m_pWireCollect->ResetBuffer();

	if (!m_pA3DEngine->EndRender())
		return false;

	return m_pA3DEngine->Present();
}

//	Set/Restore wire render state
void CRender::SetWireRenderStates(bool bEnable)
{
	if (m_bWireRender == bEnable)
		return;

	m_bWireRender = bEnable;

	if (bEnable)
	{
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		m_pA3DDevice->SetZTestEnable(false);
		m_pA3DDevice->SetZWriteEnable(false);
		m_pA3DDevice->SetAlphaBlendEnable(false);
		m_pA3DDevice->SetLightingEnable(false);
		m_pA3DDevice->SetFillMode(A3DFILL_WIREFRAME);

		m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DLVERTEX);
		m_pA3DDevice->GetD3DDevice()->SetTexture(0, NULL);
	}
	else
	{
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
		m_pA3DDevice->SetZTestEnable(true);
		m_pA3DDevice->SetZWriteEnable(true);
		m_pA3DDevice->SetAlphaBlendEnable(true);
		m_pA3DDevice->SetLightingEnable(true);
		m_pA3DDevice->SetFillMode(A3DFILL_SOLID);
	}
}

//	Create pre-created text string
bool CRender::CreatePreText(DWORD hFont)
{
	if (!(m_pStrings = new A3DStringSet))
	{
		g_Log.Log("CRender::CreatePreText: Failed to new A3DStringSet.");
		return false;
	}

	if (!m_pStrings->Init(m_pA3DDevice, (HA3DFONT)hFont, true))
	{
		g_Log.Log("CRender::CreatePreText: Failed to initialize A3DStringSet.");
		return false;
	}

	m_pStrings->AddString("X", g_aColTable[COL_COORDINATES]);
	m_pStrings->AddString("Y", g_aColTable[COL_COORDINATES]);
	m_pStrings->AddString("Z", g_aColTable[COL_COORDINATES]);
	m_pStrings->AddString("Grid:", g_aColTable[COL_COORDINATES]);
	m_pStrings->AddString("100.0", g_aColTable[COL_COORDINATES]);
	m_pStrings->AddString("10.0", g_aColTable[COL_COORDINATES]);
	m_pStrings->AddString("1.0", g_aColTable[COL_COORDINATES]);
	m_pStrings->AddString("0.1", g_aColTable[COL_COORDINATES]);
	m_pStrings->AddString("0.05", g_aColTable[COL_COORDINATES]);

	if (!m_pStrings->Build())
	{
		g_Log.Log("CRender::CreatePreText: Failed to build string set.");
		return false;
	}

//	int w = m_pStrings->GetTextureWid();
//	int h = m_pStrings->GetTextureHei();

	return true;
}

//	Output text
void CRender::TextOut(int x, int y, char* szText, DWORD color)
{
	if (m_pFont)
		m_pFont->TextOut(x, y, szText, color);
}

//	Output pre-created text
void CRender::TextOut(int iText, int x, int y, DWORD color)
{
	if (m_pStrings)
	{
		m_pStrings->SetColor(iText, color);
		m_pStrings->OutputString(iText, x, y);
	}
}

//	Flush pre-created text
void CRender::FlushPreText()
{
	if (m_pStrings)
		m_pStrings->Flush();
}

//	Flush wire collector
void CRender::FlushWireCollector()
{
	if (m_pWireCollect)
		m_pWireCollect->Flush();
}

