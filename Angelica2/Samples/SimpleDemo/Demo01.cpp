// Demo01.cpp : 


#include <windows.h>

#include "Resource.h"
#include "Render.h"
#include "Game.h"

#include <ALog.h>
#include <ATime.h>
#include <AFI.h>
#include <AAssist.h>

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

static HINSTANCE l_hInstance = NULL;
static HWND	l_hMainWnd = NULL;
static const char* l_szClassName = "Demo01 Class";

static int l_iRenderWid = 800;
static int l_iRenderHei = 600;
static bool l_bFullScreen = false;

static ALog	l_Log;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

static bool _InitGameApp();
static void _FinalizeGameApp();
static ATOM _RegisterWndClass(HINSTANCE hInstance);
static bool _CreateMainWnd(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static bool _Tick(DWORD dwTickTime);
static bool _Render();

static void _LogOutput(const char* szMsg)
{
	l_Log.Log(szMsg);
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement 
//
///////////////////////////////////////////////////////////////////////////

//	WinMain
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
	//	Initialize application
	if (!_InitGameApp())
	{
		a_LogOutput(1, "wWinMain, Failed to initialize application");
		return -1;
	}

	//	Register window class
	if (!_RegisterWndClass(hInstance))
	{
		a_LogOutput(1, "wWinMain, Failed to register window class");
		return -1;
	}

	//	Create main window
	if (!_CreateMainWnd(hInstance, nCmdShow))
	{
		a_LogOutput(1, "wWinMain, Failed to create main window");
		return -1;
	}

	//	Initialize render
	if (!g_Render.Init(l_hInstance, l_hMainWnd, l_bFullScreen))
	{
		a_LogOutput(1, "wWinMain: Failed to initialize render");
		return -1;
	}

	//	Initialize game
	if (!g_Game.Init(l_hInstance, l_hMainWnd))
	{
		a_LogOutput(1, "wWinMain: Failed to initialize game");
		return -1;
	}
	
	static DWORD dwLastFrame = a_GetTime();
	MSG	msg;

	while (1)
	{
		DWORD dwTickTime = a_GetTime() - dwLastFrame;

		if (dwTickTime)
		{
			dwLastFrame = a_GetTime();

			//	Limit time of a logic tick
			a_ClampRoof(dwTickTime, (DWORD)100);
				
			//	Do logic tick
			_Tick(dwTickTime);

			//	Render scene
			_Render();
		}

		//	Message tick which will prevent the application hanging up;
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0); 
			if (msg.message == WM_QUIT)
				goto Exit;
	
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//	Sleep a little time
		Sleep(5);
	}

Exit:

	//	Release game
	g_Game.Release();

	//	Release render
	g_Render.Release();

	//	Finalize game
	_FinalizeGameApp();

	return 0;
}

bool _InitGameApp()
{
	//	Initialize 
	af_Initialize();

	//	Get current directory as angelica's work path
	char szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	af_SetBaseDir(szCurPath);

	//	Create log file and redirect log output
	if (!l_Log.Init("Demo01.log", "Angelica demo01 log file"))
		return false;

	a_RedirectDefLogOutput(_LogOutput);

	//	Initialize random rand
	a_InitRandom();

	return true;
}

void _FinalizeGameApp()
{
	l_Log.Release();
	a_RedirectDefLogOutput(NULL);

	af_Finalize();
}

ATOM _RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof (WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)_WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= l_szClassName;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

bool _CreateMainWnd(HINSTANCE hInstance, int nCmdShow)
{
	l_hInstance = hInstance;

	DWORD dwStyles = WS_POPUP;
	if (!l_bFullScreen)
	{
		dwStyles |= WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
	}

	DWORD dwExStyle = 0;
	const char* l_szWndName = "Demo01 Window";
	
	HWND hWnd = CreateWindowEx(dwExStyle, l_szClassName, l_szWndName, dwStyles,
					0, 0, l_iRenderWid, l_iRenderHei, NULL, NULL, hInstance, NULL);
	if (!hWnd)						    
		return false;
	
	l_hMainWnd = hWnd;

	//	Adjust window position and size in non-fullscreen mode
	if (!l_bFullScreen)
	{
		RECT rcWnd = {0, 0, l_iRenderWid, l_iRenderHei};
		AdjustWindowRect(&rcWnd, dwStyles, FALSE);
	
		int w = rcWnd.right - rcWnd.left;
		int h = rcWnd.bottom - rcWnd.top;
		int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
		int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

		MoveWindow(l_hMainWnd, x, y, w, h, FALSE);
	}

	//	Show main window
	ShowWindow(l_hMainWnd, nCmdShow);
	UpdateWindow(l_hMainWnd);

	//	we force set foreground window to ensure our main window is activated
	SetForegroundWindow(l_hMainWnd);
	return true;
}

LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_DESTROY:
	
		PostQuitMessage(0);
		return 0;

	case WM_SETCURSOR:
		
		SetCursor(NULL);
		break;

	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED || wParam == SIZE_MAXHIDE || wParam == SIZE_MAXSHOW)
			break;

		int cx = (lParam & 0x0000ffff);
		int cy = (lParam & 0xffff0000) >> 16;
		
		if (!cx || !cy)
			return 0;
		
		g_Render.ResizeDevice(cx, cy);
		break;
	}
	}

	return g_Game.WndProc(hWnd, message, wParam, lParam);
}

bool _Tick(DWORD dwTickTime)
{
	return g_Game.Tick(dwTickTime);
}

bool _Render()
{
	if (!g_Render.BeginRender())
		return false;

	g_Game.Render();

	g_Render.EndRender();

	return true;
}

