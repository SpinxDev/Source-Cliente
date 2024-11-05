#include <Windows.h>

#include "../D3DWindow.h"
#include "../A3DFile.h"
#include "../A3DFrame.h"
#include "../A3DFuncs.h"

static bool OnOpen();

A3DFrame	g_frameScene;
HWND		g_hWnd;
HINSTANCE	g_hInstance;
D3DWindow 	g_d3dWindow;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	A3DFile		file;

	g_hInstance = hInstance;
	if( !g_d3dWindow.D3DInitWindow(hInstance, NULL, &g_hWnd, false) )
		return -1L;

	g_frameScene.Init(g_d3dWindow.GetRenderer().GetDevice(), 1);
	g_frameScene.SetTM(IdentityMatrix());
	g_frameScene.SetTM(IdentityMatrix(), false);

		
	if( file.Open("mox.mox", A3DFILE_OPENEXIST | A3DFILE_BINARY) )
	{
		if( !g_frameScene.Load(g_d3dWindow.GetRenderer().GetDevice(), &file) )
		{
			MessageBox(NULL, "Fail to load the model file d:\\1.mox", "FAIL", MB_ICONSTOP | MB_OK);
			return -1;
		}
		file.Close();
	}

	g_d3dWindow.SetOpenCallBack(OnOpen);

	g_d3dWindow.D3DRunWindow(&g_frameScene, 30);

	g_frameScene.Release();

	g_d3dWindow.D3DReleaseWindow();

	return 1L;
}

bool OnOpen()
{
	char	szFilename[MAX_PATH];
	A3DFile file;

	OPENFILENAME	ofn;
	szFilename[0] = '\0';
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWnd;
	ofn.hInstance = g_hInstance;
	ofn.lpstrFilter = "Model File(*.mox)\0*.mox\0";
	ofn.lpstrFile = szFilename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select the model file to open";
	ofn.Flags = OFN_FILEMUSTEXIST;
	if( !GetOpenFileName(&ofn) )
	{
		return true;
	}

	if( !file.Open(szFilename, A3DFILE_OPENEXIST | A3DFILE_BINARY) )
	{
		MessageBox(g_hWnd, "Can not open the specified file!", "FAIL", MB_OK | MB_ICONSTOP);
		return false;
	}

	g_frameScene.Release();

	g_frameScene.Init(g_d3dWindow.GetRenderer().GetDevice(), 1);
	g_frameScene.SetTM(IdentityMatrix());
	g_frameScene.SetTM(IdentityMatrix(), false);
	
	if( !g_frameScene.Load(g_d3dWindow.GetRenderer().GetDevice(), &file) )
	{
		MessageBox(g_hWnd, "Load the model file fail", "FAIL", MB_ICONSTOP | MB_OK);
		file.Close();
		g_frameScene.Release();
		g_frameScene.Init(g_d3dWindow.GetRenderer().GetDevice(), 1);
		g_frameScene.SetTM(IdentityMatrix());
		g_frameScene.SetTM(IdentityMatrix(), false);
		g_d3dWindow.GetRenderer().SetScene(&g_frameScene);
		return false;
	}

	file.Close();

	/*
	A3DFrame * pHead;
	if( g_frameScene.FindChildByName("Bone09", &pHead) )
	{
		A3DFile fileTest;
		A3DFrame * pFrameSphere = new A3DFrame();

		if( !fileTest.Open("D:\\MAX Models\\Binary\\SphereTest.mox", A3DFILE_OPENEXIST) )
			return false;

		if( !pFrameSphere->Load(g_d3dWindow.GetRenderer().GetDevice(), &fileTest) )
			return false;

		fileTest.Close();

		pHead->AddChild(pFrameSphere);
	}*/

	g_d3dWindow.GetRenderer().SetScene(&g_frameScene);
	
	return true;
}
 