// Render.h : interface of the CRender class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class A3DEngine;
class A3DDevice;
class A3DCamera;
class A3DViewport;
class A3DLight;
class A3DFont;

///////////////////////////////////////////////////////////////////////////
//
//	Class CRender
//
///////////////////////////////////////////////////////////////////////////

class CRender
{
public:		//	Types

public:		//	Constructor and Destructor

	CRender();
	virtual ~CRender() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize game
	bool Init(HINSTANCE hInst, HWND hWnd, bool bFullScreen);
	//	Release game
	void Release();

	//	Resize device
	bool ResizeDevice(int iWid, int iHei);
	//	Begin render
	bool BeginRender();
	//	End render
	bool EndRender();
	//	Clear viewport
	void ClearViewport();

	//	Output text
	void TextOut(int x, int y, const char* szText, DWORD color);

	//	Get interface
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	A3DDevice* GetA3DDevice() {	return m_pA3DDevice; }
	A3DCamera* GetCamera() { return m_pCamera; }
	A3DViewport* GetViewport() { return m_pViewport; }
	A3DLight* GetDirectionalLight() { return m_pDirLight; }
	A3DFont* GetSysFont() { return m_pSysFont; }

	//	Get render size
	int GetRenderWidth() { return m_iRenderWid; }
	int GetRenderHeight() { return m_iRenderHei; }
	//	Get fullscreen flag
	bool IsFullScreen() { return m_bFullScreen; }
	
	//	Engine is ready ?
	bool EngineIsReady() { return m_bEngineOK; }

private:	//	Attributes

	A3DEngine*		m_pA3DEngine;		//	A3D engine
	A3DDevice*		m_pA3DDevice;		//	A3D device
	A3DCamera*		m_pCamera;			//	Camera object
	A3DViewport*	m_pViewport;		//	Viewport object
	A3DLight*		m_pDirLight;		//	Directional light
	A3DFont*		m_pSysFont;			//	System Font object

	HINSTANCE		m_hInstance;		//	Handle of instance
	HWND			m_hRenderWnd;		//	Render window's handle
	int				m_iRenderWid;		//	Render size
	int				m_iRenderHei;
	bool			m_bFullScreen;		//	Full screen flag

	bool			m_bEngineOK;		//	Engine is ready

protected:	//	Operations

	//	Initialize A3D engine
	bool InitA3DEngine();
	//	Release A3D eigine
	void ReleaseA3DEngine();
};

extern CRender	g_Render;

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

#endif // $$FILE_NAME_SYMBOL$$_INCLUDED_
