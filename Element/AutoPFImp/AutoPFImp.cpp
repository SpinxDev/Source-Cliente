// AutoPFImp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AutoPFImp.h"

#include "MainFrm.h"
#include "Global.h"
#include "Render.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoPFImpApp

BEGIN_MESSAGE_MAP(CAutoPFImpApp, CWinApp)
	//{{AFX_MSG_MAP(CAutoPFImpApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoPFImpApp construction

CAutoPFImpApp::CAutoPFImpApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_dwLastFrame = 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAutoPFImpApp object

CAutoPFImpApp theApp;

bool InitApp(HINSTANCE hInst, HWND hWnd)
{

	af_Initialize();

	char szWorkDir[MAX_PATH];
	GetModuleFileNameA(NULL, szWorkDir, MAX_PATH);
	AUX_SetWorkDir(szWorkDir);

	//	Open log file
	g_Log.Init("AutoPFImp.log", "auto pathfinding log");

	//	Create A3DEngine
	if (!g_Render.Init(hInst, hWnd))
	{
		g_Log.Log("InitApp, Failed to create render");
		return false;
	}

	return true;
}

void ExitApp()
{

	g_AutoMoveAgent.Release();

	//	Release A3DEngine
	g_Render.Release();

	//	Close app's log file
	g_Log.Release();

	//	Finalize AF system
	af_Finalize();

}


/////////////////////////////////////////////////////////////////////////////
// CAutoPFImpApp initialization

BOOL CAutoPFImpApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);




	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pFrame->UpdateWindow();

	if (!InitApp(m_hInstance, pFrame->GetView()->GetSafeHwnd()))
	{
		return false;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAutoPFImpApp message handlers





/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAutoPFImpApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAutoPFImpApp message handlers


int CAutoPFImpApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	ExitApp();
	return CWinApp::ExitInstance();
}


BOOL CAutoPFImpApp::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
	CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
	ASSERT(pMainFrame);

	DWORD dwTickTime = m_dwLastFrame ? a_GetTime() - m_dwLastFrame : 0;
	m_dwLastFrame = a_GetTime();

	if (dwTickTime)
	{
		//	Clamp tick time in 80 ms, too large tick time isn't a good thing
		a_ClampRoof(dwTickTime, (DWORD)80);

		//	Do frame move
		pMainFrame->GetView()->Tick(dwTickTime);
	}

	//	Render
	pMainFrame->GetView()->Render();

	CWinApp::OnIdle(lCount);

	return TRUE;
}
