#include <Windows.h>
#include <StdIO.h>
#include <StdLib.h>

#include "A3DFrame.h"
#include "A3DMesh.h"
#include "A3DTexture.h"
#include "D3DRenderer.h"

#ifndef _D3DWINDOW_H_
#define _D3DWINDOW_H

typedef bool (*OPENCALLBACK)();

class D3DWindow
{
private:
	HWND m_hWndParent;
	HWND m_hWnd;	
	HACCEL m_hAccel;
	HINSTANCE m_hInstance;
	D3DRenderer m_Renderer;
	OPENCALLBACK m_pfnOpenCallBack;

	bool m_bDragEnabled;
	int	m_nMX;
	int m_nMY;

	bool InitInstance(HINSTANCE hInstance, HWND hParentWnd, int nCmdShow);

public:
	bool D3DInitWindow(HINSTANCE hInstance, HWND hWndParent, HWND * pHWnd, bool bInMAX=true);
	bool D3DReleaseWindow();
	bool D3DRunWindow(A3DFrame * pScene, int nFrameRate=30);
	bool OnFileOpen();
	bool OnMouseMove(int nX, int nY);
	bool OnMouseDrag(int nX, int nY, WPARAM wParam);
	inline D3DRenderer& GetRenderer() { return m_Renderer; }
	inline void SetOpenCallBack(OPENCALLBACK pfn) { m_pfnOpenCallBack = pfn; }
};

typedef D3DWindow * PD3DWindow;

#endif