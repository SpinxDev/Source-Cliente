// DX8Test.cpp : Defines the entry point for the application.
//

#include "D3DWindow.h"
#include "D3DRenderer.h"
#include "resource.h"

extern bool g_bRotateWorld;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

D3DWindow * g_pD3DWindow = NULL;

bool D3DWindow::D3DInitWindow(HINSTANCE hInstance, HWND hWndParent, HWND * pHWnd, bool bInMAX)
{
	m_bDragEnabled = false;
	g_pD3DWindow = this;

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= CreateSolidBrush(RGB(0, 0, 0));
	if( bInMAX )
		wcex.lpszMenuName	= NULL;
	else
		wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_MAINMENU);
	wcex.lpszClassName	= "D3DWINDOW";
	wcex.hIconSm		= NULL;
	
	RegisterClassEx(&wcex);

	m_hWndParent = hWndParent;

	// Perform application initialization:
	if( !InitInstance(hInstance, hWndParent, SW_SHOW) ) 
		return false;

	m_hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL));
	if( !m_Renderer.InitD3D(hInstance, m_hWnd, m_hAccel) )
		return false;

	*pHWnd = m_hWnd;
	
	m_pfnOpenCallBack = NULL;
	m_bDragEnabled = true;
	return true;
}

bool D3DWindow::D3DReleaseWindow()
{
	m_Renderer.ReleaseD3D();
	DestroyWindow(m_hWnd);
	g_pD3DWindow = false;
	m_pfnOpenCallBack = NULL;
	return true;
}

bool D3DWindow::D3DRunWindow(A3DFrame * pScene, int nFrameRate)
{
	m_Renderer.SetScene(pScene);

	return m_Renderer.Run(nFrameRate);
}

bool D3DWindow::InitInstance(HINSTANCE hInstance, HWND hParentWnd, int nCmdShow)
{
   HWND hWnd;

   m_hInstance = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow("D3DWINDOW", "MOX Renderer (Implemented with Direct 3D)", WS_OVERLAPPEDWINDOW,
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

   SetWindowPos(hWnd, NULL, 0, 0, 800 + rectWnd.right - rectWnd.left - rectClient.right + rectClient.left, 
	   600 + rectWnd.bottom - rectWnd.top - rectClient.bottom + rectClient.top, SWP_NOMOVE | SWP_NOZORDER);

   GetWindowRect(hWnd, &rectWnd);
   SetWindowPos(hWnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - rectWnd.right + rectWnd.left) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rectWnd.bottom + rectWnd.top) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDM_OPEN:
				g_pD3DWindow->OnFileOpen();				
				break;
			case IDM_EXIT:
				PostMessage(hWnd, WM_QUIT, 0, 0);
				break;
			}
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
		case WM_DESTROY:
			//If the window is just a child window, do not post quit message for that will cause
			//the whole thread exit;
			//PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_RETURN:
				g_bRotateWorld = !g_bRotateWorld;
				break;
			default:
				break;
			}
			break;
		case WM_MOUSEMOVE:
			int nX, nY;
			nX = LOWORD(lParam);
			nY = HIWORD(lParam);
			if( wParam == MK_LBUTTON )
			{
				g_pD3DWindow->OnMouseDrag(nX, nY, MK_LBUTTON);
			}
			else if (wParam == MK_RBUTTON)
			{
				g_pD3DWindow->OnMouseDrag(nX, nY, MK_RBUTTON);				
			}
			else
				g_pD3DWindow->OnMouseMove(nX, nY);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

bool D3DWindow::OnFileOpen()
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
}

bool D3DWindow::OnMouseMove(int nX, int nY)
{
	m_nMX = nX;
	m_nMY = nY;

	return true;
}

bool D3DWindow::OnMouseDrag(int nX, int nY, WPARAM wParam)
{
	int nDX = nX - m_nMX;
	int nDY = nY - m_nMY;

	m_nMX = nX;
	m_nMY = nY;

	if( m_bDragEnabled )
	{
		if( wParam == MK_LBUTTON )
			m_Renderer.DragWorld(nDX, nDY);
		else if( wParam == MK_RBUTTON )
			m_Renderer.MoveWorld(nDX, nDY);
	}

	return true;
}