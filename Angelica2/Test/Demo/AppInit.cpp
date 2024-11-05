/*
 * FILE: AppInit.cpp
 *
 * DESCRIPTION: Routinues used to initialize windows application
 *
 * CREATED BY: duyuxin, 2003/5/11
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Render.h"

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

LRESULT CALLBACK AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

HINSTANCE	g_hInstance		= NULL;
HWND		g_hWnd			= NULL;
bool		g_bFullScreen	= false;
bool		g_bActive		= false;

const ACHAR	g_szClassName[50] = _AL("Angelica Demo");

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof (WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)AppWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_szClassName;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
bool InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInstance = hInstance;

	DWORD dwStyles = WS_POPUP;
	if (!g_bFullScreen)
		dwStyles |= WS_POPUP | WS_CAPTION | WS_THICKFRAME;

	HWND hWnd = CreateWindow(g_szClassName, _AL("Angelica Demo"), dwStyles,
							 0, 0, RENDER_WIDTH, RENDER_HEIGHT, NULL, NULL, hInstance, NULL);
	if (!hWnd)						    
		return false;
	
	g_hWnd = hWnd;

	if (!g_bFullScreen)
	{
		//	Adjust window position
		RECT rcWnd = {0, 0, RENDER_WIDTH, RENDER_HEIGHT};
		AdjustWindowRect(&rcWnd, dwStyles, FALSE);
	
		int w = rcWnd.right - rcWnd.left;
		int h = rcWnd.bottom - rcWnd.top;
		int x = (GetSystemMetrics(SM_CXFULLSCREEN) - w) / 2;
		int y = (GetSystemMetrics(SM_CYFULLSCREEN) - h) / 2;

		MoveWindow(hWnd, x, y, w, h, FALSE);
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return true;
}


