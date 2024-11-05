#ifndef _ENGINE3DWINDOW_H_
#define _ENGINE3DWINDOW_H_

#include <Windows.h>
#include <StdIO.h>
#include <StdLib.h>

#include <A3D.h>
#include <AF.h>
#include <AC.h>

/*
#if (!defined (_A_FORBID_NEWDELETE) || !defined (_A_FORBID_MALLOC))
#pragma message ("To ensure plugin work correctly, _A_FORBID_NEWDELETE and _A_FORBID_MALLOC flags in AMemory.h are supposed to be defined !")
#endif
*/

class Engine3DWindow
{
protected:
	HWND				m_hWndParent;
	HWND				m_hWnd;	
	HACCEL				m_hAccel;
	HINSTANCE			m_hInstance;

	A3DEngine			* m_pA3DEngine;
	A3DDevice			* m_pA3DDevice;
	A3DCamera			* m_pA3DCamera;
	A3DLight			* m_pA3DDirLight;
	A3DViewport			* m_pA3DViewport;

	A3DSky				* m_pA3DSky;

protected:
	bool InitInstance(HINSTANCE hInstance, HWND hParentWnd, int nCmdShow);

	virtual bool InitEngine();
	virtual bool ReleaseEngine();

	bool TickControl();
	bool Render();

protected:
	virtual bool BeforeRenderScene(A3DViewport * pCurrentViewport);
	virtual bool AfterRenderScene(A3DViewport * pCurrentViewport);

public:
	Engine3DWindow();
	~Engine3DWindow();

	bool Init(HINSTANCE hInstance, HWND hWndParent, HWND * pHWnd, bool bInMAX=true, bool bCreateSky=false);
	bool Release();

	bool MessageTick();

	bool Run(int nFrameRate=30);

	inline A3DDevice * GetA3DDevice() { return m_pA3DDevice; }
	inline A3DEngine * GetA3DEngine() { return m_pA3DEngine; }
	inline A3DViewport * GetA3DViewport() { return m_pA3DViewport; }
};

#endif//_ENGINE3DWINDOW_H_