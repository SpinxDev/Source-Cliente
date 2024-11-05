/*
* FILE: Render.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/08/27
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _CRender_H_
#define _CRender_H_


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DGfxEngine;
class A3DGFXExMan;
class AFileGfxInterface;
class APhysXScene;
class A3DRenderTarget;
class A3DCamera;
class A3DViewport;
class GFXPhysXSceneShell;
class AResInterfaceImpl;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CRender
//	
///////////////////////////////////////////////////////////////////////////

class CRender
{

public:		//	Types

public:		//	Constructor and Destructor

	CRender(void);
	virtual ~CRender(void);

public:		//	Attributes

public:		//	Operations

	//	Initialize game
	bool Init(HINSTANCE hInst, HWND hWnd);
	//	Release game
	void Release();

	//	Test the engine state
	bool IsInitialized() const { return m_bEngineOK; }

	//	Resize device
	bool ResizeDevice(int iWid, int iHei);
	//	Begin render
	bool BeginRender(bool bDoWarps);
	//	End render
	bool EndRender(bool bDoWarps);
	//	Do warps
	bool DoWarps();

	//	Output text
	void TextOut(int x, int y, const ACHAR* szText, DWORD color);
	
	//	Render window's width / height
	int GetRenderWndHei() const { return m_iRenderHei; }
	int GetRenderWndWid() const { return m_iRenderWid; }

	//	Draw 2D rectangle
	void Draw2DRect(const A3DRECT & rc, DWORD dwCol, bool bFrame);

	//	Get interface
	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	A3DGfxEngine* GetA3DGfxEngine() const { return m_pA3DGfxEngine; }
	A3DDevice* GetA3DDevice() const {	return m_pA3DDevice; }
	A3DCamera* GetA3DCamera() const { return m_pA3DCamera; }
	A3DViewport* GetA3DViewport() const { return m_pA3DViewport; }
	A3DLight* GetDirLight() const { return m_pDirLight; }
	A3DRenderTarget* GetFrameTarget() const { return m_pFrameTarget; }

	static CRender* GetInstance();

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;		//	A3D engine
	A3DDevice*		m_pA3DDevice;		//	A3D device
	A3DCamera*		m_pA3DCamera;		//	A3D camera
	A3DViewport*	m_pA3DViewport;		//	A3D viewport
	A3DRenderTarget* m_pFrameTarget;		//	A3D frame target which may be used as texture
	A3DLight*		m_pDirLight;		//	Directional light
	A3DLight*		m_pDynamicLight;
	A3DFont*		m_pFont;			//	Font object
	A3DGfxEngine*	m_pA3DGfxEngine;	//	A3D GFX engine
	A3DGFXExMan*	m_pA3DGfxExMan;		//	A3D GFX man
	AFileGfxInterface* m_pGfxInterface;	//	GFX interface
	GFXPhysXSceneShell* m_pPhysXShell;	//	PhysX Shell
	AResInterfaceImpl* m_pResImpl;

	HINSTANCE		m_hInstance;		//	Handle of instance
	HWND			m_hRenderWnd;		//	Render window's handle
	int				m_iRenderWid;		//	Render size
	int				m_iRenderHei;

	// space warp objects.
	A3DPixelShader *		m_pTLWarpShader;		// TL warp render pixel shader
	A3DStream *				m_pTLWarpStream;		// stream used to show space warps using TL verts
	int						m_nMaxTLWarpVerts;		// max verts number of TL warp stream
	int						m_nMaxTLWarpIndices;	// max indices number of TL warp stream
	A3DPixelShader *		m_pWarpShader;
	A3DStream *				m_pWarpStream;
	int						m_nMaxWarpVerts;
	int						m_nMaxWarpIndices;
	bool			m_bEngineOK;		//	Engine is ready

	static CRender*			s_pRenderInst;
	static CCriticalSection	s_cs;

protected:	//	Operations

	//	Initialize A3D engine
	bool InitA3DEngine();
	//	Release A3D eigine
	void ReleaseA3DEngine();
	//	Initialize Gfx engine
	bool InitA3DGfxEngine();
	//	Release Gfx engine
	void ReleaseA3DGfxEngine();
	//	Initialize physical engine
	bool InitPhysXEngine();
	//	Initialize ares module
	bool InitAResModule();
	//	Release ares module
	void ReleaseAResModule();

	//	Create frame target
	bool CreateFrameTarget();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

extern APhysXScene* gGetAPhysXScene();

#endif	//	_CRender_H_


