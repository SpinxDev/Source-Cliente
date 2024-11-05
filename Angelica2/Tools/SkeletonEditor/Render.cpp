/*
 * FILE: Render.cpp
 *
 * DESCRIPTION: Render class
 *
 * CREATED BY: Duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
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
	m_pDirLight		= NULL;
	m_pFont			= NULL;
	m_pStrings		= NULL;
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
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_iRenderWid;
	devFmt.nHeight		= m_iRenderHei;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

//	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD | A3DDEV_FORCESOFTWARETL;	//	Force software T&L
	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD;	//	Hardware render

	if (!m_pA3DEngine->Init(m_hInstance, m_hRenderWnd, &devFmt, dwDevFlags))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to initialize A3D engine.");
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Add a directional light into scene
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pDirLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
					A3DVECTOR3(1.0f, -1.0f, 1.0f), A3DCOLORVALUE(0.5f, 0.5f, 0.5f, 1.0f), 
					A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), A3DCOLORVALUE(0.4f, 0.4f, 0.4f, 1.0f));

	//	Set some values
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(50, 50, 50));
	m_pA3DDevice->SetFogStart(5000.0f);
	m_pA3DDevice->SetFogEnd(5000.0f);
	m_pA3DDevice->SetFogEnable(false);

	m_pFont = m_pA3DEngine->GetSystemFont();
//	m_pFont->EnableFilter(false);

	HA3DFONT hFont = m_pA3DEngine->GetA3DFontMan()->RegisterFontType(_AL("Arial"), 12, 0, 'W');

	//	Create pre-created text strings
/*	if (!CreatePreText(hFont))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to call CreatePreText().");
		return false;
	}
*/
	//	Load skin model shaders
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	if (!pSkinMan->LoadSkinModelVertexShaders("Configs\\skinmodelvs.cfg"))
	{
		a_LogOutput(1, "CECGame::InitA3DEngine, Failed to load skinmodelvs.cfg");
	//	return false;
	}

	m_pA3DEngine->GetA3DSkinMan()->SetDirLight(m_pDirLight);

	m_bEngineOK = true;

	return true;
}

//	Release A3D engine
void CRender::ReleaseA3DEngine()
{
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

	m_pA3DEngine->SetDisplayMode(iWid, iHei, A3DFMT_UNKNOWN, 0, true, SDM_WIDTH | SDM_HEIGHT);

	m_iRenderWid = iWid;
	m_iRenderHei = iHei;

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

//	m_pA3DEngine->DisplayPerformanceRecord();

	if (!m_pA3DEngine->EndRender())
		return false;

	return m_pA3DEngine->Present();
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

/*	m_pStrings->AddString("X", g_aColTable[COL_COORDINATES]);
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
*/
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

//	Draw 2D rectangle
void CRender::Draw2DRect(const RECT& rc, DWORD dwCol, bool bFrame)
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


