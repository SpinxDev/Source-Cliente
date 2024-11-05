// Render.cpp : implementation of the CRender class
//

#include <windows.h>

#include "Render.h"

#include <ALog.h>
#include <A3DShaderMan.h>
#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DViewport.h>
#include <A3DCamera.h>
#include <A3DFont.h>
#include <A3DLight.h>
#include <A3DLightMan.h>
#include <A3DSkinMan.h>

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
	m_pCamera		= NULL;
	m_pViewport		= NULL;
	m_pDirLight		= NULL;
	m_pSysFont		= NULL;
	m_hInstance		= NULL;
	m_hRenderWnd	= NULL;
	m_pvsSMEdge		= NULL;
	m_pvsRMEdge		= NULL;
	m_iRenderWid	= 800;
	m_iRenderHei	= 600;
	m_bFullScreen	= false;
	m_bEngineOK		= false;
}

/*	Initialize game

	Return true for success, otherwise return false.

	hInst: handle of instance.
	hWnd: handle of render window.
*/
bool CRender::Init(HINSTANCE hInst, HWND hWnd, bool bFullScreen)
{
	m_hInstance		= hInst;
	m_hRenderWnd	= hWnd;
	m_bFullScreen	= bFullScreen;

	if (!InitA3DEngine())
	{
		a_LogOutput(1, "CRender::Init, Failed to initialize A3D engine!");
		return false;
	}

	m_pvsSMEdge = m_pA3DEngine->GetA3DShaderMan()->LoadVertexShader("Shaders\\vs\\skin_mesh_m4_edge_detect.txt", false, A3DVT_BVERTEX3);
	m_pvsRMEdge = m_pA3DEngine->GetA3DShaderMan()->LoadVertexShader("Shaders\\vs\\rigid_mesh_edge_detect.txt", false, A3DVT_VERTEX);

	return true;
}

//	Release game
void CRender::Release()
{
	if (m_pvsSMEdge)
	{
		m_pA3DEngine->GetA3DShaderMan()->ReleaseVertexShader(&m_pvsSMEdge);
		m_pvsSMEdge = NULL;
	}

	if (m_pvsRMEdge)
	{
		m_pA3DEngine->GetA3DShaderMan()->ReleaseVertexShader(&m_pvsRMEdge);
		m_pvsRMEdge = NULL;
	}

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
		a_LogOutput(1, "CRender::InitA3DEngine: Not enough memory!");
		return false;
	}
	
	RECT Rect;
	GetClientRect(m_hRenderWnd, &Rect);

	m_iRenderWid = Rect.right;
	m_iRenderHei = Rect.bottom;
	
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= !m_bFullScreen;
	devFmt.nWidth		= m_iRenderWid;
	devFmt.nHeight		= m_iRenderHei;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

//	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD | A3DDEV_FORCESOFTWARETL;	//	Force software T&L
	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD;	//	Hardware render

	if (!m_pA3DEngine->Init(m_hInstance, m_hRenderWnd, &devFmt, dwDevFlags))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to initialize A3D engine.");
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Add a directional light into scene
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pDirLight, 
				A3DVECTOR3(0.0f, 0.0f, 0.0f), A3DVECTOR3(-1.0f, -1.0f, -1.0f),
				A3DCOLORVALUE(0.5f, 0.5f, 0.5f, 1.0f), 
				A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), 
				A3DCOLORVALUE(0.4f, 0.4f, 0.4f, 1.0f));

	//	Set some values
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(100, 100, 100));
	m_pA3DDevice->SetFogStart(5000.0f);
	m_pA3DDevice->SetFogEnd(5000.0f);
	m_pA3DDevice->SetFogEnable(true);

	m_pSysFont = m_pA3DEngine->GetSystemFont();
	m_pA3DEngine->GetA3DSkinMan()->SetDirLight(m_pDirLight);

	//	Create camera
	if (!(m_pCamera = new A3DCamera()))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to create camera object!");
		return false;
	}

	if (!m_pCamera->Init(g_Render.GetA3DDevice(), DEG2RAD(56.0f), 0.1f, 1200.0f))
	{
		a_LogOutput(1, 0, "CRender::InitA3DEngine: Failed to initialize camera.");
		return false;
	}
							
	//	Move camera to original point and set it's direction as Z axis
	m_pCamera->SetPos(A3DVECTOR3(0.0F) - A3DVECTOR3(0.0f, 0.0f, 1.0f) * 10.0f);
	m_pCamera->SetDirAndUp(A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));

	//	Create viewport
	if (!g_Render.GetA3DDevice()->CreateViewport(&m_pViewport, 0, 0, 
				m_iRenderWid, m_iRenderHei,
				0.0f, 1.0f, true, true, A3DCOLORRGB(64, 64, 64)))
	{
		a_LogOutput(1, 0, "CRender::InitA3DEngine: Failed to create viewport.");
		return false;
	}

	m_pViewport->SetCamera(m_pCamera);
	m_pViewport->Active();

	m_bEngineOK = true;

	return true;
}

//	Release A3D engine
void CRender::ReleaseA3DEngine()
{
	A3DRELEASE(m_pCamera);
	A3DRELEASE(m_pViewport);
	A3DRELEASE(m_pDirLight);
	A3DRELEASE(m_pA3DEngine);

	m_pSysFont	 = NULL;
	m_pA3DDevice = NULL;
}

/*	Resize device

	Return true for success, otherwise return false.

	iWid, iHei: new size of render area
*/
bool CRender::ResizeDevice(int iWid, int iHei)
{
	if (!m_bEngineOK || !iWid || !iHei)
		return false;

	m_pA3DEngine->SetDisplayMode(iWid, iHei, A3DFMT_UNKNOWN, 0, true, SDM_WIDTH | SDM_HEIGHT);

	m_iRenderWid = iWid;
	m_iRenderHei = iHei;

	//	Adjust camera and viewport
	A3DVIEWPORTPARAM Params;
	Params.X		= 0;
	Params.Y		= 0;
	Params.Width	= iWid;
	Params.Height	= iHei;
	Params.MinZ		= 0.0f;
	Params.MaxZ		= 1.0f;

	m_pViewport->SetParam(&Params);

	//	Adjust camera
	float fRatio = (float)iWid / iHei;
	m_pCamera->SetProjectionParam(m_pCamera->GetFOV(), 0.1f, 2000.0f, fRatio);

	return true;
}

//	Begin render
bool CRender::BeginRender()
{
	if (!m_bEngineOK)
		return false;

	return m_pA3DEngine->BeginRender();

	return true;
}

//	End render
bool CRender::EndRender()
{
	if (!m_bEngineOK)
		return false;

	if (!m_pA3DEngine->EndRender())
		return false;

	return m_pA3DEngine->Present();
}

//	Output text
void CRender::TextOut(int x, int y, const char* szText, DWORD color)
{
	if (m_pSysFont)
		m_pSysFont->TextOut(x, y, szText, color);
}

//	Clear viewport
void CRender::ClearViewport()
{
	if (!m_bEngineOK)
		return;

	m_pViewport->Active();
	m_pViewport->ClearDevice();
}


