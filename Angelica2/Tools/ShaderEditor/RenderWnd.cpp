// RenderWnd.cpp : implementation file
//

#include "stdafx.h"
#include "CommonDef.h"
#include "RenderWnd.h"
#include "A3D.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


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

A3DEngine*			g_pA3DEngine	= NULL;
A3DCamera*			g_pA3DCamera	= NULL;
A3DViewport*		g_pA3DViewport	= NULL;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// 
//	CRenderWnd
//
/////////////////////////////////////////////////////////////////////////////

CRenderWnd::CRenderWnd()
{
	m_pCurShader	= NULL;
	m_pBackTexture	= NULL;
	m_dwBackCol		= 0xffffffff;
	m_dwVertCol		= 0xffffffff;
}

CRenderWnd::~CRenderWnd()
{
}


BEGIN_MESSAGE_MAP(CRenderWnd, CWnd)
	//{{AFX_MSG_MAP(CRenderWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// 
//	CRenderWnd message handlers
//
/////////////////////////////////////////////////////////////////////////////

/*	Initialize A3D Engine

	Return true for success, otherwise return false

	hInstance: Instance's handle.
*/
bool CRenderWnd::InitA3DEngine(HINSTANCE hInstance)
{
	//	Init Engine;
	if (!(g_pA3DEngine = new A3DEngine()))
	{
		TRACE("CRenderWnd::InitA3DEngine, Not enough memory!");
		return false;
	}
	
	A3DDEVFMT devFmt;
	memset(&devFmt, 0, sizeof(devFmt));
	devFmt.bWindowed	= true;
	devFmt.nWidth		= RENDERAREA_WIDTH;
	devFmt.nHeight		= RENDERAREA_HEIGHT;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD;

	if (!g_pA3DEngine->Init(hInstance, m_hWnd, &devFmt, A3DDEV_ALLOWMULTITHREAD))
	{
		TRACE("CRenderWnd::InitA3DEngine, Failed to initialize A3D engine.\n");
		return false;
	}

	//	Set Render Parameters
	if (!(g_pA3DCamera = new A3DCamera()))
	{
		TRACE("CRenderWnd::InitA3DEngine, Not enough memory!");
		return false;
	}

	if (!g_pA3DCamera->Init(g_pA3DEngine->GetA3DDevice(), DEG2RAD(70.0f), 0.1f, 500.0f, 1.0f))
	{
		TRACE("CRenderWnd::InitA3DEngine, Failed to initialize camera.\n");
		return false;
	}
						    
	g_pA3DCamera->SetPos(A3DVECTOR3(0.0f, 0.0f, 0.0f));
	g_pA3DCamera->SetDirAndUp(A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));

	//	Main Viewport
	if (!g_pA3DEngine->GetA3DDevice()->CreateViewport(&g_pA3DViewport, 0, 0, RENDERAREA_WIDTH,
		RENDERAREA_HEIGHT, 0.0f, 1.0f, true, true, A3DCOLORRGBA(0, 0, 0, 255)))
	{
		TRACE("CRenderWnd::InitA3DEngine, Failed to create viewport.\n");
		return false;
	}

	g_pA3DViewport->SetCamera(g_pA3DCamera);
	g_pA3DEngine->AddViewport(g_pA3DViewport, NULL);

	//	Hide FSP counter
	g_pA3DConfig->RT_SetShowFPSFlag(false);

	//	Disable lighting
	g_pA3DEngine->GetA3DDevice()->SetLightingEnable(false);

	//	Load background texture
	ChangeBackgroundTexture("Transparent.bmp");

	//	Initialize decal
	if (!m_Decal.Init(g_pA3DEngine->GetA3DDevice()))
	{
		TRACE("CRenderWnd::InitA3DEngine, Failed to create decal.\n");
		return false;
	}

	m_Decal.SetTexCoor(0.0f, 0.0f);
	m_Decal.SetTexSize(1.0f, 1.0f);
	m_Decal.SetColor(0xffffffff);

	return true;
}

//	Release A3D Engine
void CRenderWnd::ReleaseA3DEngine()
{
	if (m_pBackTexture)
	{
		m_pBackTexture->Release();
		delete m_pBackTexture;
		m_pBackTexture = NULL;
	}

	m_Decal.Release();

	if (g_pA3DViewport)
	{
		g_pA3DViewport->Release();
		delete g_pA3DViewport;
		g_pA3DViewport = NULL;
	}

	if (g_pA3DCamera)
	{
		g_pA3DCamera->Release();
		delete g_pA3DCamera;
		g_pA3DCamera = NULL;
	}

	if (g_pA3DEngine)
	{
		g_pA3DEngine->Release();
		delete g_pA3DEngine;
		g_pA3DEngine = NULL;
	}
}

//	Create window
BOOL CRenderWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	ASSERT_VALID(this);
	
	if (!CWnd::CreateEx(0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
												LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW))),
						lpszWindowName, 
						dwStyle,
						rect, 
						pParentWnd, 
						nID, 
						pContext))
		return FALSE;

	return TRUE;
}

//	Set current shader
void CRenderWnd::ChangeShader(A3DShader* pShader)
{
	m_pCurShader = pShader;

	m_Decal.SetShader(pShader);
	m_Decal.SetColor(0xffffffff);
}

//	Render background
bool CRenderWnd::RenderBackground()
{
	if (!m_pBackTexture)
		return true;

	A3DDevice* pDevice = g_pA3DEngine->GetA3DDevice();

	//	Render states
	pDevice->SetSourceAlpha(A3DBLEND_ONE);
	pDevice->SetDestAlpha(A3DBLEND_ZERO);
	pDevice->SetZWriteEnable(false);

	m_pBackTexture->Appear();

	m_Decal.SetColor(m_dwBackCol);
	m_Decal.RenderAsBackground(g_pA3DViewport);

	m_pBackTexture->Disappear();

	//	Restore render states
	pDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	pDevice->SetZWriteEnable(true);

	return true;
}

//	Render window
bool CRenderWnd::Render()
{
	if (!g_pA3DEngine->BeginRender())
		return false;

	if (!g_pA3DEngine->RenderAllRenderTarget())
		return false;

	if (!g_pA3DEngine->RenderAllViewport())
		return false;

	//	Render background
	RenderBackground();

	if (m_pCurShader)
	{
		m_pCurShader->TickAnimation();

		m_pCurShader->Appear(); 

		m_Decal.SetColor(m_dwVertCol);
		m_Decal.Render(g_pA3DViewport);

		m_pCurShader->Disappear();
	}

	if (!g_pA3DEngine->EndRender())
		return false;

	if (!g_pA3DEngine->Present())
		return false;

	return true;
}

//	Change background texture
bool CRenderWnd::ChangeBackgroundTexture(const char* szFile)
{
	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return false;

	char szFullPath[MAX_PATH];
	sprintf(szFullPath, "%s%s", g_szWorkDir, szFile);

	if (!pTexture->LoadFromFile(g_pA3DEngine->GetA3DDevice(), szFullPath, szFile))
	{
		pTexture->Release();
		delete pTexture;
		return false;
	}

	//	Release current texture
	if (m_pBackTexture)
	{
		m_pBackTexture->Release();
		delete m_pBackTexture;
		m_pBackTexture = NULL;
	}

	m_pBackTexture = pTexture;

	return true;
}

