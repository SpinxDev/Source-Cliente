// AFileDialogDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <process.h>
#include "AFileDialogDll.h"
#include "Render.h"

#include "AFilePreviewWnd.h"
#include "AImagePreviewWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

ALog g_AFDllLog;
extern CAFileDialogDllApp theApp;


//	Default log debug function
void _AFDllLogOutput(const char* szMsg)
{
	g_AFDllLog.Log(szMsg);	
}

//zjy#ifdef _ANGELICA31
extern LRESULT CALLBACK PreviewWndMsgProc(HWND, UINT, WPARAM, LPARAM);
//zjy#endif
/////////////////////////////////////////////////////////////////////////////
//
//	Local Functions
//
/////////////////////////////////////////////////////////////////////////////

BOOL RegisterMyWndClass(HINSTANCE hInstance, const TCHAR* szClassName, WNDPROC lpWndProc = DefWindowProc)
{
	WNDCLASSEX wcx;
	ZeroMemory(&wcx, sizeof(wcx));

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW | 
		CS_VREDRAW;                    // redraw if size changes 
	wcx.lpfnWndProc = lpWndProc;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = hInstance;         // handle to instance 
	wcx.hIcon = LoadIcon(NULL, 
		IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, 
		IDC_ARROW);                    // predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject( 
		WHITE_BRUSH);                  // white background brush 
	wcx.lpszMenuName =  NULL;			// name of menu resource 
	wcx.lpszClassName = szClassName;  // name of window class 
	wcx.hIconSm = NULL;

	return RegisterClassEx(&wcx);
}

HWND CreateHiddenWnd(HINSTANCE hInstance)
{
	if (!RegisterMyWndClass(hInstance, TEXT("HidenWndClass")))
		return NULL;

	return CreateWindow( 
		TEXT("HidenWndClass"),        // name of window class 
		TEXT(""),						// title-bar string 
		WS_OVERLAPPEDWINDOW, // top-level window 
		CW_USEDEFAULT,       // default horizontal position 
		CW_USEDEFAULT,       // default vertical position 
		CW_USEDEFAULT,       // default width 
		CW_USEDEFAULT,       // default height 
		(HWND) NULL,         // no owner window 
		(HMENU) NULL,        // use class menu 
		hInstance,           // handle to application instance 
		(LPVOID) NULL);      // no window-creation data 
}

HWND CreatePreviewWnd(HINSTANCE hInstance)
{
//zjy#ifdef _ANGELICA31
	if (!RegisterMyWndClass(hInstance, TEXT("PreviewWndClass"), PreviewWndMsgProc))
		return NULL;
/*zjy#elif defined _ANGELICA2
	if (!RegisterMyWndClass(hInstance, TEXT("PreviewWndClass")))
		return NULL;
#endif
zjy*/

	return CreateWindow( 
		TEXT("PreviewWndClass"),        // name of window class 
		TEXT(""),						// title-bar string 
		WS_CAPTION|WS_SIZEBOX, // top-level window 
		0,       // default horizontal position 
		0,       // default vertical position 
		256,       // default width 
		256,       // default height 
		(HWND) NULL,         // no owner window 
		(HMENU) NULL,        // use class menu 
		hInstance,           // handle to application instance 
		(LPVOID) NULL);      // no window-creation data 
}

// Thread Functions
extern void _OutPreviewProc(PVOID pParam);
extern void _RequestAndWaitOutPreviewProcEnd();
extern void _RequestOutPreviewProcStart();

static LONG _PreviewWndLoopThreadEndRequest = 0;
static unsigned __int64 _PreviewWndLoopThreadId;
static void _RequestAndWaitPreviewProcEnd()
{
	InterlockedDecrement(&_PreviewWndLoopThreadEndRequest);
	ASSERT( _PreviewWndLoopThreadEndRequest == 0 );
	WaitForSingleObject((HANDLE)_PreviewWndLoopThreadId, INFINITE);
}

//zjy#ifdef _ANGELICA31

static AFilePreviewWnd* g_pPreviewWnd = NULL;
AFilePreviewWnd* GetPreviewWnd()
{ 
	return g_pPreviewWnd; 
}

static void _PreviewWndLoop(void * pParam)
{
	InterlockedIncrement(&_PreviewWndLoopThreadEndRequest);

	AFilePreviewWnd* pPreviewWnd = GetPreviewWnd();
	if (!pPreviewWnd || !pPreviewWnd->GetSafeHwnd())
		return;

	DWORD dwLastTick = GetTickCount();

	MSG msg;

	while (pPreviewWnd->GetSafeHwnd())
	{
		if (_PreviewWndLoopThreadEndRequest == 0)
			break;

		if (PeekMessage(&msg, pPreviewWnd->GetSafeHwnd(), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD dwDelta = GetTickCount() - dwLastTick;
		if (dwDelta < 33)
		{
			Sleep(10);
			continue;
		}

		if (pPreviewWnd->IsWShow())
		{
			pPreviewWnd->Tick(dwDelta);
			pPreviewWnd->Render();
		}

		dwLastTick = GetTickCount();
	}
}
//zjy#endif

static bool g_bDllInitialized = false;

bool AFD_IsInitialized()
{
	return g_bDllInitialized;
}

//	From 2009.8 on this Dll will initialize an engine inside of it
//	In order to render the gfx / ecm in a preview window / to a preview image
bool AFD_DllInitialize()
{
	char szCurDir[1024];
	GetCurrentDirectoryA(sizeof(szCurDir), szCurDir);
	af_SetBaseDir(szCurDir);

	a_LogOutput(1, "AFD_DllInitialize() started");

//zjy#ifdef _ANGELICA31

	if (g_bDllInitialized)
	{
		a_LogOutput(1, "Error in AFD_DllInitialize, AFileDialog Dll is already initialized.");
		return true;
	}

	if (!theApp.m_hInstance)
	{
		a_LogOutput(1, "Error in AFD_DllInitialize(), AFileDialog Dll is not valid. (hInstance is NULL).");
		return false;
	}

	HWND hHiddenWnd = CreateHiddenWnd(theApp.m_hInstance);
	if (!hHiddenWnd)
	{
		a_LogOutput(1, "Error in AFD_DllInitialize(), AFileDialog could not create a hidden window for engine.");
		return false;
	}

	CRect rcRequire(0, 0, PREVIEW_SIZE, PREVIEW_SIZE);
	AdjustWindowRect(&rcRequire, GetWindowStyle(hHiddenWnd), FALSE);
	MoveWindow(hHiddenWnd, 0, 0, rcRequire.Width(), rcRequire.Height(), FALSE);

/*//addtionalview
	if (!CRender::GetInstance()->Init(theApp.m_hInstance, hHiddenWnd))
	{
		a_LogOutput(1, "Error in AFD_DllInitialize(), AFileDialog could not init the engine.");
		return false;
	}
*///addtionalview

	HWND hPreviewWnd = CreatePreviewWnd(theApp.m_hInstance);
	if (!hPreviewWnd)
	{
		a_LogOutput(1, "Error in AFD_DllInitialize(), AFilePreviewWnd create failed.");
		return false;
	}

	rcRequire = CRect(0, 0, PREVIEW_SIZE, PREVIEW_SIZE);
	AdjustWindowRect(&rcRequire, GetWindowStyle(hPreviewWnd), FALSE);
	MoveWindow(hPreviewWnd, 0, 0, rcRequire.Width(), rcRequire.Height(), FALSE);

	ShowWindow(hPreviewWnd, SW_HIDE);

	g_pPreviewWnd = new AFilePreviewWnd();
	if (!g_pPreviewWnd || !g_pPreviewWnd->Attach(hPreviewWnd))
	{
		a_LogOutput(1, "Error in AFD_DllInitialize(), Can not attach to PreviewWnd.");
		return false;
	}
	
	
	if (!CRender::GetInstance()->Init(theApp.m_hInstance, g_pPreviewWnd->GetSafeHwnd()))
	{
		a_LogOutput(1, "Error in AFD_DllInitialize(), AFileDialog could not init the engine.");
		return false;
	}
	

	if (!g_pPreviewWnd->Init(CRender::GetInstance()->GetA3DDevice(), CRender::GetInstance()->GetA3DGfxEngine()))
	{
		a_LogOutput(1, "Error in AFD_DllInitialize(), Can not initialize Preview Window.");
		return false;
	}

	// 创建AFileDialog自用的预览窗口线程
	_PreviewWndLoopThreadId = _beginthread(_PreviewWndLoop, 0, NULL);
	
	// 创建外部预览用的线程
	_RequestOutPreviewProcStart();

	g_bDllInitialized = true;

	a_LogOutput(1, "AFD_DllInitialize() finished");

	return true;

/*zjy#else

	g_bDllInitialized = true;

	return true;

#endif*/
}

void AFD_DllFinalize()
{
//zjy#ifdef _ANGELICA31

	if (!g_bDllInitialized)
		return;

	// 请求并等待外部Preview线程结束
	_RequestAndWaitOutPreviewProcEnd();
	
	// 请求并等待AFileDialog自用Preview线程结束
	_RequestAndWaitPreviewProcEnd();

	if (g_pPreviewWnd)
	{
		g_pPreviewWnd->Release();
		g_pPreviewWnd->Detach();
		delete g_pPreviewWnd;
		g_pPreviewWnd = NULL;
	}

	CRender::GetInstance()->Release();

	g_bDllInitialized = false;

//#else
//	g_bDllInitialized = false;
//#endif
}



/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDllApp

BEGIN_MESSAGE_MAP(CAFileDialogDllApp, CWinApp)
	//{{AFX_MSG_MAP(CAFileDialogDllApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDllApp construction

CAFileDialogDllApp::CAFileDialogDllApp()
: m_hDllEngineThreadId(0)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CAFileDialogDllApp::~CAFileDialogDllApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAFileDialogDllApp object

CAFileDialogDllApp theApp;

BOOL CAFileDialogDllApp::InitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	SetRegistryKey(_T("AFileDialog"));

	CWinApp::InitInstance();

#ifdef _USE_BCGP

	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));

#endif

	//af_Initialize();
	//char lpFileName[1024];
	//GetModuleFileNameA(m_hInstance, lpFileName, sizeof(lpFileName));
	//char * pFileName = PathFindFileNameA(lpFileName);
	//if (pFileName)
	//	*pFileName = 0;

	//af_SetBaseDir(lpFileName);

	char szCurDir[1024];
	GetCurrentDirectoryA(sizeof(szCurDir), szCurDir);
	af_SetBaseDir(szCurDir);

	if (!g_AFDllLog.Init("AFDll.log", "AFile Dialog Dll module Init"))
		return false;

	g_AFDllLog.Log("AFDll Init in %s", af_GetBaseDir());
	a_RedirectDefLogOutput(_AFDllLogOutput);

#ifdef _ANGELICA2
	//LuaBind::Init();
	g_LuaStateMan.Init();
#elif defined _ANGELICA31 || defined _ANGELICA22 || defined _ANGELICA21
	g_LuaStateMan.Init();
#endif

	AfxEnableControlContainer();

	return TRUE;
}

#ifdef _ANGELICA2
class APhysXScene;
APhysXScene *gGetAPhysXScene(void)
{
	return 0;
}
#endif

int CAFileDialogDllApp::ExitInstance()
{

#if defined _ANGELICA31 || defined _ANGELICA22 || defined _ANGELICA21
	g_LuaStateMan.Release();
#elif defined _ANGELICA2
	//LuaBind::Release();
	g_LuaStateMan.Release();
#endif

	af_Finalize();

#ifdef _USE_BCGP

	BCGCBProCleanUp();

#endif

	return CWinApp::ExitInstance();
}
