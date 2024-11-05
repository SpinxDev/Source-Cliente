#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <d3d8.h>
#include <d3dx8math.h>

#include "A3DVertex.h"
#include "A3DFrame.h"
#include "A3DFont.h"

#ifndef _D3DRENDERER_H_
#define _D3DRENDERER_H_

class D3DRenderer
{
public:
	HINSTANCE				m_hInstance;
	HWND					m_hWnd;
	HACCEL					m_hAccel;
	IDirect3D8 *			m_pD3D;
	IDirect3DDevice8 *		m_pD3DDevice;
	IDirect3DSurface8 *		m_pBackBuffer;
	A3DFrame *				m_pScene;			
	A3DMesh *				m_pMeshGround;
	A3DFont					m_Font;
	
	DWORD					m_dwTicks;
	DWORD					m_dwFPSTicks;
	DWORD					m_dwTimeLast;
	DWORD					m_dwLastRenderTime;
	int						m_nFrameRate;
	int						m_nTimePerFrame;

	int						m_nMeshCount;
	int						m_nFrameCount;
	int						m_nVertCount;
	int						m_nIndexCount;

	A3DMATRIX4				m_matWorld;
	float					m_fCameraX, m_fCameraY, m_fCameraZ;
	float					m_fCameraDelta;

	float					m_fFPS;
	
	D3DRenderer();
	~D3DRenderer();

	bool InitD3D(HINSTANCE hInstance, HWND hwnd, HACCEL hAccel = NULL);
	bool CreateGridGround();
	bool ReleaseD3D();

	bool ConfigDevice();

	bool SetScene(A3DFrame * pScene);
	bool Run(int nFrameRate);
	bool Tick();
	bool MessageTick();
	bool Render();

	bool RotateWorld();
	bool DragWorld(int nDX, int nDY);
	bool MoveWorld(int nDX, int nDY);
	void SetupLights();

	void ErrorMsg(char * szMsg);

	inline IDirect3DDevice8 * GetDevice() { return m_pD3DDevice; }
};

#endif