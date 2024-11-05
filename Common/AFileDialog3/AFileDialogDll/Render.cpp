/*
* FILE: CRender.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/08/27
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"

#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
#include "A3DGFXExMan.h"
#endif

#if defined _ANGELICA31
#include "AResFuncs.h"
#include "AResInterfaceImpl.h"
#endif

#include "Render.h"
#include "SyncWrapper.h"
#include "AFileGfxInterface.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
#ifdef _ANGELICA22 
#define SHADERS_ROOT_DIR		"shaders\\2.2\\"
#else
#define SHADERS_ROOT_DIR		"shaders\\"
#endif

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

CRender* CRender::s_pRenderInst = NULL;
CCriticalSection CRender::s_cs;

A3DVECTOR3 g_vAxisX(1.f, 0.f, 0.f);
A3DVECTOR3 g_vAxisY(0.f, 1.f, 0.f);
A3DVECTOR3 g_vAxisZ(0.f, 0.f, 1.f);
A3DVECTOR3 g_vOrigin(0.f, 0.f, 0.f);

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

CRender* CRender::GetInstance()
{
	if (s_pRenderInst)
		return s_pRenderInst;

	{
		SyncWrapper<CCriticalSection> _locker(s_cs);
		if (s_pRenderInst == NULL)
		{
			static CRender _instance;
			s_pRenderInst = &_instance;
		}
	}

	return s_pRenderInst;
}

CRender::CRender(void)
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_pA3DCamera	= NULL;
	m_pA3DViewport	= NULL;
	m_pFrameTarget	= NULL;
	m_pTLWarpStream	= NULL;
	m_pTLWarpShader	= NULL;
	m_pWarpStream	= NULL;
	m_pWarpShader	= NULL;	
	m_pDirLight		= NULL;
	m_pDynamicLight	= NULL;
	m_pFont			= NULL;
	m_hInstance		= NULL;
	m_hRenderWnd	= NULL;
	m_pA3DGfxEngine = NULL;
	m_pGfxInterface	= NULL;
	m_pA3DGfxExMan	= NULL;
	m_pResImpl		= NULL;
	m_pPhysXShell	= NULL;
	m_bEngineOK		= false;
}

CRender::~CRender(void)
{
}

bool CRender::Init(HINSTANCE hInst, HWND hWnd)
{
	m_hInstance		= hInst;
	m_hRenderWnd	= hWnd;

	if (!InitA3DEngine())
	{
		a_LogOutput(1, "CRender::Init, Failed to initialize A3D engine!");
		return false;
	}

	if (!InitA3DGfxEngine())
	{
		a_LogOutput(1, "CRender::Init, Failed to initialize A3D GFX engine.");
		return false;
	}

	InitAResModule();
	return true;
}


//	Release game
void CRender::Release()
{
	ReleaseAResModule();
	ReleaseA3DGfxEngine();
	ReleaseA3DEngine();
}

//	Initialize ARes module
bool CRender::InitAResModule()
{
#ifdef _ANGELICA31
	m_pResImpl = new AResInterfaceImpl();
	if (!m_pResImpl->Init(m_pA3DEngine
		, m_pA3DGfxEngine
		, m_pPhysXShell
		, m_pGfxInterface->GetSkillGfxEventMan()))
	{
		a_LogOutput(1, "%s, Failed to initialize ARes interface.", __FUNCTION__);
		return false;
	}

	AResInitializeModule(m_pResImpl);
#endif
	return true;
}

//	Release ARes module
void CRender::ReleaseAResModule()
{
#ifdef _ANGELICA31
	AResFinalizeModule();

	A3DRELEASE(m_pResImpl);
#endif
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

#ifdef _ANGELICA21
	if (strstr(::GetCommandLineA(), "shader_model_2"))
		g_pA3DConfig->SetUserShaderModelVersion(2);

	if (strstr(::GetCommandLineA(), "noshadercache") == NULL)
		g_pA3DConfig->SetFlagIgnoreShaderCache(false);
#endif

	RECT Rect;
	GetClientRect(m_hRenderWnd, &Rect);

	m_iRenderWid = Rect.right;
	m_iRenderHei = Rect.bottom;

	A3DDEVFMT devFmt;
	memset(&devFmt, 0, sizeof(devFmt));
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_iRenderWid;
	devFmt.nHeight		= m_iRenderHei;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD;

	if (!m_pA3DEngine->Init(m_hInstance, m_hRenderWnd, &devFmt, dwDevFlags))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to initialize A3D engine. (%s)", af_GetBaseDir());
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

#if defined(_ANGELICA22)
	#define CONFIGS_ROOT_DIR		"configs\\2.2\\"
#elif defined(_ANGELICA2)
	#define CONFIGS_ROOT_DIR		"configs\\"
#endif

#if defined _ANGELICA2 || defined _ANGELICA22

	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	if (!pSkinMan->LoadSkinModelVertexShaders(CONFIGS_ROOT_DIR"skinmodelvs.cfg"))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to load skinmodelvs.cfg");
		::MessageBox(NULL, TEXT("¼ÓÔØConfigs\\skinmodelvs.cfgÊ§°Ü"), NULL, MB_OK);
		return false;
	}

#endif

	//	Add a directional light into scene
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pDirLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
		Normalize(A3DVECTOR3(0.0f, -0.7f, -1.0f)), A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f), 
		A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));

	m_pA3DEngine->GetA3DLightMan()->CreatePointLight(2, &m_pDynamicLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
		A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), 10.0f, 0);

	m_pDynamicLight->TurnOff();

#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21

	m_pA3DEngine->GetA3DSkinMan()->SetDPointLight(m_pDynamicLight);

#endif

	//	Set some values
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(100, 100, 100));
	m_pA3DDevice->SetFogStart(5000.0f);
	m_pA3DDevice->SetFogEnd(5000.0f);
	m_pA3DDevice->SetFogEnable(false);

	//	Get system font
	m_pFont = m_pA3DEngine->GetSystemFont();

	//	Create camera
	if (!(m_pA3DCamera = new A3DCamera))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to create camera object!");
		return false;
	}

	if (!m_pA3DCamera->Init(m_pA3DDevice, DEG2RAD(56.0f), 0.1f, 2000.0f))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to initialize camera.");
		return false;
	}

	m_pA3DCamera->SetPos(g_vOrigin - g_vAxisZ * 10.0f);
	m_pA3DCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);

	//	Create main Viewport
	RECT rc;
	::GetClientRect(m_hRenderWnd, &rc);

#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21

	if (!m_pA3DDevice->CreateViewport(&m_pA3DViewport, 0, 0, rc.right, rc.bottom,
		0.0f, 1.0f, true, true, 0xff404040))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to create viewport.");
		return false;
	}

#elif defined(_ANGELICA31)

	A3DViewport* pViewport = new A3DViewport;
	if (!pViewport) {
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to create viewport instance.");
		return false;
	}

	if (!pViewport->Init(m_pA3DDevice, 0, 0, rc.right, rc.bottom,
		0.0f, 1.0f, true, true, 0xff404040))
	{
		a_LogOutput(1, "CRender::InitA3DEngine: Failed to init viewport.");
		return false;
	}

	m_pA3DViewport = pViewport;

#endif	
	m_pA3DDevice->SetZFunc(A3DCMP_LESSEQUAL);

	m_pA3DViewport->SetCamera(m_pA3DCamera);

	//	we need a frame target to be used as texture in gfx editor
	if( !CreateFrameTarget() )
	{
		a_LogOutput(1, "CRender::InitA3DEngine(), Failed to create frame target!");
		return false;
	}

	//// now load resource for TL space warps.
	//m_nMaxTLWarpVerts = 1000;
	//m_nMaxTLWarpIndices = 1500;
	//m_pTLWarpStream = new A3DStream();
	//if( !m_pTLWarpStream->Init(m_pA3DDevice, sizeof(A3DTLWARPVERTEX), A3DFVF_A3DTLWARPVERT, m_nMaxTLWarpVerts, m_nMaxTLWarpIndices, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC) )
	//{
	//	a_LogOutput(1, "CRender::InitA3DEngine(), failed to create warp stream!");
	//	return false;
	//}


	//WORD * pIndices;
	//if( !m_pTLWarpStream->LockIndexBuffer(0, 0, (BYTE **)&pIndices, 0) )
	//{
	//	a_LogOutput(1, "CRender::InitA3DEngine(), failed to lock warp stream's index buffer!");
	//	return false;
	//}
	//int i;
	//for(i=0; i<250; i++)
	//{
	//	pIndices[i * 6 + 0] = i * 4 + 0;		
	//	pIndices[i * 6 + 1] = i * 4 + 1;		
	//	pIndices[i * 6 + 2] = i * 4 + 2;		
	//	pIndices[i * 6 + 3] = i * 4 + 2;		
	//	pIndices[i * 6 + 4] = i * 4 + 1;		
	//	pIndices[i * 6 + 5] = i * 4 + 3;		
	//}
	//m_pTLWarpStream->UnlockIndexBuffer();

	//m_pTLWarpShader = m_pA3DEngine->GetA3DShaderMan()->LoadPixelShader(SHADERS_ROOT_DIR"ps\\spacewarp_tl.txt", false);
	//if( NULL == m_pTLWarpShader )
	//{
	//	a_LogOutput(1, "CRender::InitA3DEngine(), failed to load spacewarp_tl.txt");
	//	CString strErrMsg;
	//	strErrMsg.Format(TEXT("¼ÓÔØ")TEXT(SHADERS_ROOT_DIR)TEXT("ps\\spacewarp_tl.txtÊ§°Ü, Engine Work Path: %s"), af_GetBaseDir());
	//	::MessageBox(NULL, strErrMsg, NULL, MB_OK);
	//	return false;
	//}


	//m_nMaxWarpVerts = 1000;
	//m_nMaxWarpIndices = 1500;
	//m_pWarpStream = new A3DStream();
	//if( !m_pWarpStream->Init(m_pA3DDevice, sizeof(A3DWARPVERTEX), A3DFVF_A3DWARPVERT, m_nMaxWarpVerts, m_nMaxWarpIndices, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC) )
	//{
	//	a_LogOutput(1, "CRender::InitA3DEngine(), failed to create warp stream!");
	//	return false;
	//}

	//if( !m_pWarpStream->LockIndexBuffer(0, 0, (BYTE **)&pIndices, 0) )
	//{
	//	a_LogOutput(1, "CRender::InitA3DEngine(), failed to lock warp stream's index buffer!");
	//	return false;
	//}
	//for(i=0; i<250; i++)
	//{
	//	pIndices[i * 6 + 0] = i * 4 + 0;		
	//	pIndices[i * 6 + 1] = i * 4 + 1;		
	//	pIndices[i * 6 + 2] = i * 4 + 2;		
	//	pIndices[i * 6 + 3] = i * 4 + 2;		
	//	pIndices[i * 6 + 4] = i * 4 + 1;		
	//	pIndices[i * 6 + 5] = i * 4 + 3;		
	//}
	//m_pWarpStream->UnlockIndexBuffer();

	//m_pWarpShader = m_pA3DEngine->GetA3DShaderMan()->LoadPixelShader(SHADERS_ROOT_DIR"ps\\spacewarp.txt", false);
	//if( NULL == m_pWarpShader )
	//{
	//	a_LogOutput(1, "CRender::InitA3DEngine(), failed to load spacewarp.txt");
	//	::MessageBox(NULL, TEXT("¼ÓÔØ")TEXT(SHADERS_ROOT_DIR)TEXT("ps\\spacewarp.txtÊ§°Ü"), NULL, MB_OK);
	//	return false;
	//}

	//	Below line ensure the positions of 3D sounds in world are correct.
	m_pA3DCamera->SetAM3DSoundDevice(m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice());

	m_bEngineOK = true;
	return true;
}

//	Release A3D engine
void CRender::ReleaseA3DEngine()
{
	SyncWrapper<CCriticalSection> _locker(s_cs);

	A3DRELEASE(m_pDirLight);
	A3DRELEASE(m_pDynamicLight);
	A3DRELEASE(m_pA3DCamera);
	A3DRELEASE(m_pA3DViewport);

	//if (m_pA3DEngine)
	//	m_pA3DEngine->GetA3DShaderMan()->ReleasePixelShader(&m_pTLWarpShader);

	//A3DRELEASE(m_pTLWarpStream);

	//if (m_pA3DEngine)
	//	m_pA3DEngine->GetA3DShaderMan()->ReleasePixelShader(&m_pWarpShader);

	//A3DRELEASE(m_pWarpStream);
	A3DRELEASE(m_pFrameTarget);
	A3DRELEASE(m_pA3DEngine);

	m_bEngineOK = false;
}


//	Initialize Gfx engine
bool CRender::InitA3DGfxEngine()
{
#ifdef _ANGELICA31

	m_pPhysXShell = new GFXPhysXSceneShell;

	if (!(m_pA3DGfxEngine = new A3DGfxEngine())) 
	{
		a_LogOutput(1, "CRender::InitA3DGfxEngine(), not enough memory");
		return false;
	}

	if (!(m_pGfxInterface = new AFileGfxInterface())) 
	{
		a_LogOutput(1, "CRender::InitA3DGfxEngine(), not enough memory");
		return false;
	}

	A3DGfxEngine::GfxEngineInit init;
	init.pEngine = m_pA3DEngine;
	init.pEventSystem = NULL;
	init.pInterface = m_pGfxInterface;
	if (!m_pA3DGfxEngine->Init(init)) 
	{
		a_LogOutput(1, "CRender::InitA3DGfxEngine(), failed to initialize A3DGfxEngine.");
		return false;
	}

	if (!m_pGfxInterface->Init(m_pA3DGfxEngine))
	{
		a_LogOutput(1, "CRender::InitA3DGfxEngine(), failed to initialize A3DGfxEngine.");
		return false;
	}

	m_pA3DGfxExMan = m_pA3DGfxEngine->GetA3DGfxExMan();

#elif defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21

	m_pA3DGfxExMan = AfxGetGFXExMan();
	if(!m_pA3DGfxExMan->Init(m_pA3DDevice))
		return false;

#endif

	return true;
}

//	Release Gfx engine
void CRender::ReleaseA3DGfxEngine()
{
#ifdef _ANGELICA31
	A3DRELEASE(m_pGfxInterface);
	A3DRELEASE(m_pA3DGfxEngine);
	delete m_pPhysXShell;
	m_pPhysXShell = NULL;
#elif defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
	if(m_pA3DGfxExMan)
		AfxGetGFXExMan()->Release();
#endif
}

/*	Resize device

Return true for success, otherwise return false.

iWid, iHei: new size of render area
*/
bool CRender::ResizeDevice(int iWid, int iHei)
{
	if (!m_bEngineOK)
		return true;

	m_pA3DEngine->SetDisplayMode(iWid, iHei, A3DFMT_UNKNOWN, 0, 0, SDM_WIDTH | SDM_HEIGHT);

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

	if( !CreateFrameTarget() )
	{
		a_LogOutput(1, "CRender::ResizeDevice(), failed to resize frame target!");
		return false;
	}

	//	Adjust camera
	float fRatio = (float)iWid / iHei;
	m_pA3DCamera->SetProjectionParam(DEG2RAD(65.0f), 0.1f, 2000.0f, fRatio);

	return true;
}

//	Begin render
bool CRender::BeginRender(bool bDoWarps)
{
	ASSERT(m_pA3DEngine);
	if( !m_pA3DEngine->BeginRender() )
		return false;

//#if defined _ANGELICA2 || defined _ANGELICA22 || defined _ANGELICA21
//
//	if (bDoWarps) m_pA3DEngine->GetA3DDevice()->SetRenderTarget(m_pFrameTarget);
//
//#elif defined(_ANGELICA31)
//
//	if (bDoWarps) m_pFrameTarget->ApplyToDevice();
//
//#endif



	return true;
}

//	End render
bool CRender::EndRender(bool bDoWarps)
{
	ASSERT(m_pA3DEngine);

//	// we should move content from frame target to back target and then do warps.
//	if (bDoWarps)
//	{
//#if defined _ANGELICA2
//
//		m_pA3DEngine->GetA3DDevice()->RestoreRenderTarget();
//		m_pA3DViewport->Active();
//
//#if	defined _ANGELICA22 || defined _ANGELICA21
//		HRESULT hval = m_pA3DDevice->GetD3DDevice()->StretchRect(m_pFrameTarget->GetTargetSurface()->m_pD3DSurface, NULL,
//			m_pA3DDevice->GetBackBuffer()->m_pD3DSurface, NULL, D3DTEXF_NONE);
//#else
//		HRESULT hval = m_pA3DDevice->GetD3DDevice()->CopyRects(m_pFrameTarget->GetTargetSurface(), NULL, 0,
//			m_pA3DDevice->GetBackBuffer(), NULL);
//#endif
//
//#elif defined(_ANGELICA31)
//
//		m_pFrameTarget->WithdrawFromDevice();
//		m_pA3DViewport->Active();
//		HRESULT hval = m_pA3DDevice->GetD3DDevice()->StretchRect(m_pFrameTarget->GetTargetSurface()->m_pD3DSurface, NULL,
//			m_pA3DDevice->GetBackBuffer()->m_pD3DSurface, NULL, D3DTEXF_NONE);
//
//#endif
//
//		m_pA3DViewport->Active();
//
//
//		DoWarps();
//	}

	if (!m_pA3DEngine->EndRender())
		return false;

	return m_pA3DEngine->Present();
}

//	Output text
void CRender::TextOut(int x, int y, const ACHAR* szText, DWORD color)
{

#ifdef _ANGELICA31
	if (m_pFont)
		m_pFont->AddText(x, y, szText, 0, color);
#endif
}

//	Draw 2D rectangle
void CRender::Draw2DRect(const A3DRECT & rc, DWORD dwCol, bool bFrame)
{
#ifdef _ANGELICA31
	A3DGDI* pA3DGDI = A3DGDI::GetInstance();
#else
	A3DGDI* pA3DGDI = g_pA3DGDI;
#endif

	if (bFrame)
	{
		A3DPOINT2 pt1, pt2;

		//	Top border
		pt1.Set(rc.left, rc.top);
		pt2.Set(rc.right, rc.top);
		pA3DGDI->Draw2DLine(pt1, pt2, dwCol);

		//	Left border
		pt1.Set(rc.left, rc.top);
		pt2.Set(rc.left, rc.bottom);
		pA3DGDI->Draw2DLine(pt1, pt2, dwCol);

		//	Right border
		pt1.Set(rc.right, rc.top);
		pt2.Set(rc.right, rc.bottom);
		pA3DGDI->Draw2DLine(pt1, pt2, dwCol);

		//	Bottom border
		pt1.Set(rc.left, rc.bottom);
		pt2.Set(rc.right, rc.bottom);
		pA3DGDI->Draw2DLine(pt1, pt2, dwCol);
	}
	else
	{
		A3DRECT rect(rc.left, rc.top, rc.right, rc.bottom);
		pA3DGDI->Draw2DRectangle(rect, dwCol);
	}
}

bool CRender::CreateFrameTarget()
{
	if( m_pFrameTarget )
	{
		A3DRELEASE(m_pFrameTarget);
	}



	A3DDEVFMT devFmt = m_pA3DDevice->GetDevFormat();
	devFmt.bWindowed	= true;

#if defined _ANGELICA31 || defined _ANGELICA22 || defined _ANGELICA21

	A3DRenderTarget::RTFMT rtFmt;
	rtFmt.fmtDepth = devFmt.fmtDepth;
	rtFmt.fmtTarget = devFmt.fmtTarget;
	rtFmt.iHeight = devFmt.nHeight;
	rtFmt.iWidth = devFmt.nWidth;

#else // if defined _ANGELICA2 || defined _ANGELICA21

	A3DDEVFMT& rtFmt = devFmt;

#endif

	m_pFrameTarget = new A3DRenderTarget();

	if( !m_pFrameTarget->Init(m_pA3DDevice, rtFmt, true, false) )
	{
		a_LogOutput(1, "CRender::CreateFrameTarget(), failed to create frame target!");
		return false;
	}

	return true;
}

bool CRender::DoWarps()
{
	// first TL space warps.
	return true;
}

#if defined _ANGELICA22 || defined _ANGELICA21
APhysXScene* gGetAPhysXScene()
{
	return NULL;
}
#endif