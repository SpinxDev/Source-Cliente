// Winmain.cpp : Defines the entry point for the application.
//

#include <Windows.h>
#include <StdIO.h>
#include <StdLib.h>

#include "VTechDemo.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	g_pGame = new VTechDemo();

	bool bReadFromPck = true;
	char szCommandLine[1024];

	strncpy(szCommandLine, GetCommandLine(), 1024);
	strlwr(szCommandLine);
	if( strstr(szCommandLine, "-nopck") )
		bReadFromPck = false;

	if( !g_pGame->Init(hInstance, bReadFromPck) )
		return -1;

	g_pGame->Run();

	g_pGame->Release();
	return 0;
}