#include <Windows.h>
#include <StdIO.h>
#include "D3DRenderer.h"
#include "A3DMesh.h"
#include "A3DVertex.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"

#pragma comment(lib, "d3d8.lib")

bool g_bRotateWorld = true;

D3DRenderer::D3DRenderer()
{
	m_hWnd = NULL;
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
	m_pBackBuffer = NULL;
	m_pMeshGround = NULL;
}

D3DRenderer::~D3DRenderer()
{
}

void D3DRenderer::ErrorMsg(char * msg)
{
	MessageBox(m_hWnd, msg, "ERROR", MB_ICONSTOP | MB_OK);
}

void D3DRenderer::SetupLights()
{
    D3DXVECTOR3 vecDir;
    D3DLIGHT8 light;
    ZeroMemory(&light, sizeof(D3DLIGHT8));
    light.Type       = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.a  = 1.0f;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
	light.Specular.r = 1.0f;
	light.Specular.g = 1.0f;
	light.Specular.b = 1.0f;
	light.Specular.a = 1.0f;
	
	vecDir = D3DXVECTOR3(cosf(135.0f * D3DX_PI / 180.0f),
                         -1.0f,
                         sinf(135.0f * D3DX_PI / 180.0f) );
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );

    light.Range       = 1000.0f;
    m_pD3DDevice->SetLight(0, &light);
    m_pD3DDevice->LightEnable(0, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

    // Finally, turn on some ambient light.
    m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xff202020);
}

bool D3DRenderer::InitD3D(HINSTANCE hInstance, HWND hwnd, HACCEL hAccel)
{
	D3DDISPLAYMODE	d3ddm;
	HRESULT			hval;

	m_hInstance = hInstance;
	m_hWnd = hwnd;
	m_hAccel = hAccel;

	MessageTick();

	m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
	if( NULL == m_pD3D )
	{
		ErrorMsg("Error Create IDirect3D8");
		return false;
	}

	hval = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if( D3D_OK != hval )
	{
		ErrorMsg("Can not get adapter display mode");
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth	= 800;
	d3dpp.BackBufferHeight	= 600;
	if( strstr(GetCommandLine(), "-FULLSCREEN") )
	{
		d3dpp.Windowed		= FALSE;
		d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	else
		d3dpp.Windowed		= TRUE;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat	= d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

	hval = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat);
	if FAILED(hval)
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	else
	{
		hval = m_pD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.BackBufferFormat, 
			d3dpp.BackBufferFormat, d3dpp.AutoDepthStencilFormat);
		if FAILED(hval)
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	
	}

	hval = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice);
	if( D3D_OK != hval )
	{
		ErrorMsg("Can not create device");
		return false;
	}

	//We use depth buffer;
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	hval = m_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
	if( D3D_OK != hval )
	{
		ErrorMsg("Can not get backbuffer pointer!");
		return false;
	}

	//Initialize the device variables;
	m_matWorld = IdentityMatrix();
	m_pD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&m_matWorld);

	CreateGridGround();

	m_fCameraX = 0.0f;
	m_fCameraY = 5.0f;
	m_fCameraZ = -15.0f;
	m_fCameraDelta = 0.1f;
		
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0.0f, m_fCameraY, m_fCameraZ),
								 &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
								 &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 1.333f, 1.0f, 6000.0f);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	ConfigDevice();

	SetupLights();

	if( !m_Font.Init(m_hInstance, m_pD3DDevice, "ConsoleFont.bmp", 10, 20, 12, 24) )
	{
		MessageBox(hwnd, "Missing font file ConsoleFont.bmp\n\nNo Text will be available!", "FAIL", MB_ICONSTOP | MB_OK);
	}

	//Init time control variables;
	m_nFrameRate = 30;
	m_nTimePerFrame = 1000 / m_nFrameRate;
	m_dwLastRenderTime = 0;
	g_bRotateWorld = true;

	return true;
}

bool D3DRenderer::ReleaseD3D()
{
	m_Font.Release();

	if( m_pMeshGround )
	{
		m_pMeshGround->Release();
		delete m_pMeshGround;
		m_pMeshGround = NULL;
	}
	if( m_pBackBuffer )
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}
	if( m_pD3DDevice )
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
	if( m_pD3D )
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
	return true;
}

bool D3DRenderer::MessageTick()
{
	MSG msg;

	while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	{
		GetMessage(&msg, NULL, 0, 0);

		if( TranslateAccelerator(m_hWnd, m_hAccel, &msg) )
			continue;

		if( msg.message == WM_QUIT || msg.message == WM_CLOSE || msg.message == WM_DESTROY )
			return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

bool D3DRenderer::SetScene(A3DFrame * pScene)
{
	m_pScene = pScene;
	m_pScene->AddMesh(m_pMeshGround);

	m_pScene->GetPolygonInfo(&m_nMeshCount, &m_nVertCount, &m_nIndexCount, &m_nFrameCount);
	m_nFrameCount ++;

	m_matWorld = IdentityMatrix();
	m_pD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&m_matWorld);
	return true;
}

bool D3DRenderer::Run(int nFrameRate)
{
	DWORD dwTime;
	m_dwTicks = 0;
	m_dwFPSTicks = 0;
	m_dwTimeLast = m_dwLastRenderTime = GetTickCount();

	m_nFrameRate = nFrameRate;
	m_nTimePerFrame = 1000 / m_nFrameRate;
	while(true)
	{
		if( !MessageTick() )
			break;

		if( !Tick() )
			return false;

		if( !Render() )
			return false;

		m_dwTicks ++;
		m_dwFPSTicks ++;

		dwTime = GetTickCount();
		if( dwTime > m_dwTimeLast + 1000 )
		{
			m_fFPS = m_dwFPSTicks * 1000.0f / (dwTime - m_dwTimeLast);
			m_dwTimeLast = dwTime;
			m_dwFPSTicks = 0;

			char szFPS[256];
			sprintf(szFPS, "MESH:%d, FRAME %d, VERT: %d, INDEX: %d, FPS: %5.1f     ", m_nMeshCount, m_nFrameCount, m_nVertCount, m_nIndexCount, m_fFPS);
			m_Font.TextOut(0, 0, szFPS, A3DCOLORRGB(255, 0, 0));
		}
	}

	return true;
}

bool D3DRenderer::Render()
{
	HRESULT hval;

	bool	bNeedUpdate = false;

	if( ((GetAsyncKeyState('R') & 0x8000) == 0) && GetTickCount() - m_dwLastRenderTime < (DWORD)m_nTimePerFrame )
		bNeedUpdate = false;
	else
	{
		bNeedUpdate = true;
		m_dwLastRenderTime = GetTickCount();
	}

	hval = m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	if( D3D_OK != hval )
		return false;

	hval = m_pD3DDevice->BeginScene();
	if( D3D_OK != hval )
		return false;

	RotateWorld();

	if( GetAsyncKeyState('Z') & 0x8000 )
	{
		if( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
		{
			m_fCameraZ += m_fCameraDelta * 3 * 0.2f;
			m_fCameraY -= m_fCameraDelta * 0.2f;
		}
		else
		{
			m_fCameraZ += m_fCameraDelta * 3;
			m_fCameraY -= m_fCameraDelta;
		}
	}
	if( GetAsyncKeyState('A') & 0x8000 )
	{
		if( GetAsyncKeyState(VK_SHIFT) & 0x8000 )
		{
			m_fCameraZ -= m_fCameraDelta * 3 * 0.2f;
			m_fCameraY += m_fCameraDelta * 0.2f;
		}
		else
		{
			m_fCameraZ -= m_fCameraDelta * 3;
			m_fCameraY += m_fCameraDelta;
		}
	}

	if( GetAsyncKeyState('H') & 0x8000 )
	{
		//Return to home pos;
		m_fCameraX = 0.0f;
		m_fCameraY = 5.0f;
		m_fCameraZ = -15.0f;
		m_fCameraDelta = 0.1f;
		m_matWorld = IdentityMatrix();
		m_pD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&m_matWorld);
	}

	if( m_fCameraZ > -0.3f )
	{
		m_fCameraZ = -0.3f;
		m_fCameraY = 0.1f;
	}

	m_pScene->Render();
	if( bNeedUpdate )
		m_pScene->TickAnimation();
	
	m_Font.Render();

	hval = m_pD3DDevice->EndScene();
	if( D3D_OK != hval ) return false;

	hval = m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
	if( D3D_OK != hval ) return false;

	m_pD3DDevice->SetStreamSource(0, NULL, 0);
    m_pD3DDevice->SetTexture(0, NULL);
	return true;
}

bool D3DRenderer::Tick()
{
	return true;
}

bool D3DRenderer::RotateWorld()
{
	if( g_bRotateWorld )
	{
	}

	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(m_fCameraX, m_fCameraY, m_fCameraZ),
								 &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
								 &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
	return true;
}

bool D3DRenderer::ConfigDevice()
{
	//Set Render State;
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);	
	
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	return true;
}

bool D3DRenderer::CreateGridGround()
{
#define GRID_NUM	33
#define VERT_NUM	(GRID_NUM * 4 - 4)
#define INDEX_NUM	(GRID_NUM * 2 * 2)
#define CELL_SIZE	10.0f;

	bool		bval;
	int			x, y;
	A3DVERTEX	pVerts[VERT_NUM];
	WORD		pIndices[INDEX_NUM];
	int			nVert = 0;

	m_pMeshGround = new A3DMesh();
	if( NULL == m_pMeshGround )
		return false;

	m_pMeshGround->SetName("SYSTEM_RESERVED");
	bval = m_pMeshGround->Init(m_pD3DDevice, 1, VERT_NUM, INDEX_NUM, true);
	if( !bval ) 
		return false;

	nVert = 0;
	for(y=0; y<GRID_NUM; y++)
	{
		for(x=0; x<GRID_NUM; x++)
		{
			if( x == 0 || y == 0 || x == GRID_NUM - 1 || y == GRID_NUM - 1 )
			{
				pVerts[nVert].nx = 0.0f;
				pVerts[nVert].ny = 1.0f;
				pVerts[nVert].nz = 0.0f;
				pVerts[nVert].x = (x - GRID_NUM / 2) * CELL_SIZE;
				pVerts[nVert].y = 0.0f;
				pVerts[nVert].z = (GRID_NUM / 2 - y) * CELL_SIZE;
				nVert ++;
			}
		}
	}

	for(x=0; x<GRID_NUM; x++)
	{
		pIndices[x * 2 + 0] = x;
		pIndices[x * 2 + 1] = VERT_NUM - GRID_NUM + x;
	}
	for(y=0; y<GRID_NUM; y++)
	{
		if( y == 0 )
		{
			pIndices[(GRID_NUM + y) * 2 + 0] = 0;
			pIndices[(GRID_NUM + y) * 2 + 1] = GRID_NUM - 1;
		}
		else if( y == GRID_NUM - 1 )
		{
			pIndices[(GRID_NUM + y) * 2 + 0] = VERT_NUM - GRID_NUM;
			pIndices[(GRID_NUM + y) * 2 + 1] = VERT_NUM - 1;
		}
		else
		{
			pIndices[(GRID_NUM + y) * 2 + 0] = GRID_NUM + (y - 1) * 2;
			pIndices[(GRID_NUM + y) * 2 + 1] = GRID_NUM + (y - 1) * 2 + 1;
		}
	}
	m_pMeshGround->SetVerts(0, pVerts, VERT_NUM);
	m_pMeshGround->SetIndices(pIndices, INDEX_NUM);
	m_pMeshGround->GetMaterial()->SetDiffuse(0.7f, 1.0f, 0.7f, 0.5f);

	return true;
}

bool D3DRenderer::DragWorld(int nDX, int nDY)
{
	A3DMATRIX4 matRotateX;
	A3DMATRIX4 matRotateY;
	float vScale;
	if( GetAsyncKeyState(VK_MENU) & 0x8000 )
		vScale = m_fCameraY * 0.1f;
	else
		vScale = m_fCameraY * 0.02f;

	matRotateX = RotateX(-nDY / 30.0f * vScale);
	matRotateY = RotateY(-nDX / 30.0f * vScale);

	m_matWorld = m_matWorld * matRotateX * matRotateY;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&m_matWorld);
	return true;
}

bool D3DRenderer::MoveWorld(int nDX, int nDY)
{
	A3DMATRIX4 matMoveX;
	A3DMATRIX4 matMoveY;
	float vScale = m_fCameraY * 0.01f;

	matMoveX = IdentityMatrix();
	matMoveX.m[3][0] = nDX * vScale;
	matMoveY = IdentityMatrix();
	matMoveY.m[3][1] = -nDY * vScale;
	
	m_matWorld = m_matWorld * matMoveX * matMoveY;

	m_pD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&m_matWorld);
	return true;
}