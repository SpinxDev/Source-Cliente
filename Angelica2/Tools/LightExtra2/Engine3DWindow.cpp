#include "resource.h"
#include "Engine3DWindow.h"
				
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define DEV_WIDTH			800
#define DEV_HEIGHT			600

Engine3DWindow * g_pEngine3DWindow = NULL;
bool			 g_bFreeMouse = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDM_FILEOPEN:
				//g_pEnigine3DWindow->OnFileOpen();				
				break;
			case IDM_FILEEXIT:
				PostMessage(hWnd, WM_QUIT, 0, 0);
				break;
			}
			break;
		case WM_ACTIVATE:
			if( wParam != WA_INACTIVE )
				g_bFreeMouse = false;
			else
				g_bFreeMouse = true;
			break;
		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hWnd, &ps);
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(191, 255, 191));
			TextOut(hdc, 300, 200, "Intializing, please wait...", strlen("Intializing, please wait..."));
			EndPaint(hWnd, &ps);
			break;
		case WM_SETCURSOR:
			/*
			if( !g_bFreeMouse )
			{
				SetCursor(NULL);
			}*/
			break;
		case WM_DESTROY:
			//If the window is just a child window, do not post quit message for that will cause
			//the whole thread exit;
			//PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if( wParam == VK_RETURN )
			{
				g_bFreeMouse = !g_bFreeMouse;
				SetCursorPos(1024 / 2, 768 / 2);
			}
			break;
		case WM_MOUSEMOVE:
			return DefWindowProc(hWnd, message, wParam, lParam);
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

Engine3DWindow::Engine3DWindow()
{
	m_pA3DEngine		= NULL;
	m_pA3DDevice		= NULL;
	m_pA3DCamera		= NULL;
	m_pA3DViewport		= NULL;
	m_pA3DDirLight		= NULL;

	m_pA3DSky			= NULL;
}

Engine3DWindow::~Engine3DWindow()
{
}

bool Engine3DWindow::Init(HINSTANCE hInstance, HWND hWndParent, HWND * pHWnd, bool bInMAX, bool bCreateSky)
{
	g_pEngine3DWindow	= this;

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style				= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc		= (WNDPROC)WndProc;
	wcex.cbClsExtra			= 0;
	wcex.cbWndExtra			= 0;
	wcex.hInstance			= hInstance;
	wcex.hIcon				= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground		= CreateSolidBrush(RGB(0, 0, 0));
	if( bInMAX )
		wcex.lpszMenuName	= NULL;
	else
		wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_MAINMENU);
	wcex.lpszClassName		= "ENGINE3DWINDOW";
	wcex.hIconSm			= NULL;
	
	RegisterClassEx(&wcex);

	m_hWndParent = hWndParent;

	// Perform application initialization:
	if( !InitInstance(hInstance, hWndParent, SW_SHOW) ) 
		return false;

	m_hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL));

	if( !InitEngine() )
		return false;

	// Now create a sky here;
	if( bCreateSky )
	{
		A3DSky * pSky;
		pSky = new A3DSky();
		if( !pSky->Init(m_pA3DDevice, m_pA3DCamera, "c:\\3dsmax4\\plugcfg\\A3DSky\\Cap.jpg", "c:\\3dsmax4\\plugcfg\\A3DSky\\SurroundF.jpg", 
			"c:\\3dsmax4\\plugcfg\\A3DSky\\SurroundB.jpg", "c:\\3dsmax4\\plugcfg\\A3DSky\\Flat.tga") )
			return false;
		m_pA3DSky = pSky;
	}
	*pHWnd = m_hWnd;
	return true;
}

bool Engine3DWindow::Release()
{
	if( m_pA3DSky )
	{
		m_pA3DSky->Release();
		delete m_pA3DSky;
		m_pA3DSky = NULL;
	}

	ReleaseEngine();

	DestroyWindow(m_hWnd);
	g_pEngine3DWindow = false;
	return true;
}

bool Engine3DWindow::MessageTick()
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

bool Engine3DWindow::Run(int nFrameRate)
{
	SetFocus(m_hWnd);
	SetCursorPos(1024/2, 768/2);

	while(true)
	{
		if( !MessageTick() )
			break;

		if( !TickControl() )
			break;

		if( !Render() )
			return false;
	}

	return true;
}

bool Engine3DWindow::Render()
{
	if( !m_pA3DEngine )
		return true;
	
	if( !m_pA3DEngine->BeginRender() )
		return false;

	m_pA3DViewport->Active();
	m_pA3DViewport->ClearDevice();
	
	if( !BeforeRenderScene(m_pA3DViewport) )
		return false;

	if( !m_pA3DEngine->RenderScene(m_pA3DViewport) )
		return false;

	if( !AfterRenderScene(m_pA3DViewport) )
		return false;

	if( !m_pA3DEngine->EndRender() )
		return false;

	if( !m_pA3DEngine->Present() )
		return false;

	return true;
}

bool Engine3DWindow::InitEngine()
{	
	int nWidth = 1024, nHeight = 768;
	af_Initialize();

	//Init Engine;	
	m_pA3DEngine = new A3DEngine();
	
	A3DDEVFMT devFmt;
	ZeroMemory(&devFmt, sizeof(devFmt));
	devFmt.bWindowed = true;
	devFmt.nWidth = nWidth;
	devFmt.nHeight = nHeight;

	if( !m_pA3DEngine->Init(m_hInstance, m_hWnd, &devFmt, A3DDEV_ALLOWMULTITHREAD) )
		return false;

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//Set Render Parameters;
	m_pA3DCamera = new A3DCamera();
	
	if( !m_pA3DCamera->Init(m_pA3DDevice, DEG2RAD(70.0f), 0.1f, 1000.0f, nWidth * 1.0f / nHeight) )
		return false;
						    
	m_pA3DCamera->SetPos(A3DVECTOR3(0.0f, 10.0f, -100.0f));
	m_pA3DCamera->SetDirAndUp(Normalize(A3DVECTOR3(0.0f, 0.0f, 1.0f)), Normalize(A3DVECTOR3(0.0f, 1.0f, 0.0f)));

	// Viewport;
	if( !m_pA3DDevice->CreateViewport(&m_pA3DViewport, 0, 0, nWidth, nHeight, 0.0f, 1.0f, true, true, A3DCOLORRGBA(0, 0, 0, 255)) )
		return false;

	m_pA3DViewport->SetCamera(m_pA3DCamera);
	m_pA3DEngine->AddViewport(m_pA3DViewport, NULL);

	// Light;
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pA3DDirLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
		A3DVECTOR3(-1.0f, -0.7f, 1.0f), A3DCOLORVALUE(0.9f, 0.9f, 0.9f, 1.0f), A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(80, 80, 80));

	m_pA3DDevice->SetFogColor(A3DCOLORRGB(255, 0, 0));
	return true;
}

bool Engine3DWindow::ReleaseEngine()
{
	if( m_pA3DDirLight )
	{
		m_pA3DDirLight->Release();
		delete m_pA3DDirLight;
		m_pA3DDirLight = NULL;
	}

	if( m_pA3DViewport )
	{
		m_pA3DViewport->Release();
		delete m_pA3DViewport;
		m_pA3DViewport = NULL;
	}

	if( m_pA3DCamera )
	{
		m_pA3DCamera->Release();
		delete m_pA3DCamera;
		m_pA3DCamera = NULL;
	}

	if( m_pA3DEngine )
	{
		m_pA3DEngine->Release();
		delete m_pA3DEngine;
		m_pA3DEngine = NULL;
	}

	af_Finalize();
	return true;
}

bool Engine3DWindow::InitInstance(HINSTANCE hInstance, HWND hParentWnd, int nCmdShow)
{
   HWND hWnd;

   m_hInstance = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow("ENGINE3DWINDOW", "Anglica Engine 3D Renderer Window", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParentWnd, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return false;
   }

   m_hWnd = hWnd;

   //Resize the window to 800 * 600;
   RECT rectWnd, rectClient;

   GetWindowRect(hWnd, &rectWnd);
   GetClientRect(hWnd, &rectClient);

   SetWindowPos(hWnd, NULL, 0, 0, 1024 + rectWnd.right - rectWnd.left - rectClient.right + rectClient.left, 
	   768 + rectWnd.bottom - rectWnd.top - rectClient.bottom + rectClient.top, SWP_NOMOVE | SWP_NOZORDER);

   GetWindowRect(hWnd, &rectWnd);
   SetWindowPos(hWnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - rectWnd.right + rectWnd.left) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rectWnd.bottom + rectWnd.top) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return true;
}

/*
bool Engine3DWindow::OnFileOpen()
{
	if( m_pfnOpenCallBack )
	{
		bool bval;
		m_bDragEnabled = false;
		bval = (*m_pfnOpenCallBack)();
		m_Renderer.MessageTick();
		m_bDragEnabled = true;
		return bval;
	}
	return true;
}*/

bool Engine3DWindow::BeforeRenderScene(A3DViewport * pCurrentViewport)
{
	if( m_pA3DSky )
	{
		m_pA3DSky->Render();
	}

	return true;
}

bool Engine3DWindow::AfterRenderScene(A3DViewport * pCurrentViewport)
{
	return true;
}

bool Engine3DWindow::TickControl()
{
	// control the camera;
	POINT ptCursor;
	GetCursorPos(&ptCursor);
		int nDX = ptCursor.x - 1024 / 2;;
		int nDY = ptCursor.y - 768 / 2;

	if( !g_bFreeMouse )
	{
		m_pA3DCamera->DegDelta(nDX * 0.1f);
		m_pA3DCamera->PitchDelta(-nDY * 0.11f);
	}

	SetCursorPos(1024 / 2, 768 / 2);

	A3DVECTOR3 vecCamPos = m_pA3DCamera->GetPos();
	A3DVECTOR3 vecCamDir = m_pA3DCamera->GetDir();
	A3DVECTOR3 vecCamRight = m_pA3DCamera->GetRight();

	float vSpeed = 1.0f;
	if( GetAsyncKeyState(VK_SHIFT) )
		vSpeed *= 0.1f;
	
	if( GetAsyncKeyState('W') & 0x8000 )
		vecCamPos = vecCamPos + vecCamDir * vSpeed;
	if( GetAsyncKeyState('S') & 0x8000 )
		vecCamPos = vecCamPos - vecCamDir * vSpeed;
	if( GetAsyncKeyState('A') & 0x8000 )
		vecCamPos = vecCamPos - vecCamRight * vSpeed;
	if( GetAsyncKeyState('D') & 0x8000 )
		vecCamPos = vecCamPos + vecCamRight * vSpeed;

	m_pA3DCamera->SetPos(vecCamPos);

	if( GetAsyncKeyState('Q') & 0x8000 )
	{
		m_pA3DCamera->DegDelta(-1.0f * vSpeed);
	}
	if( GetAsyncKeyState('E') & 0x8000 )
	{
		m_pA3DCamera->DegDelta(1.0f * vSpeed);
	}

	return true;
}