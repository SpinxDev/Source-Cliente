// SectorEditor.cpp : Defines the class behaviors for the application.
//

#include "Global.h"
#include "SectorEditor.h"

#include "ATime.h"
#include "AF.h"

#include "MainFrm.h"
#include "SectorEditorDoc.h"
#include "SectorEditorView.h"
#include "Render.h"
#include "PT_Interface.h"

#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorApp

BEGIN_MESSAGE_MAP(CSectorEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CSectorEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorApp construction

CSectorEditorApp::CSectorEditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSectorEditorApp object

CSectorEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorApp initialization

BOOL CSectorEditorApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	if (!InitApp())
		return FALSE;

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSectorEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSectorEditorView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CSectorEditorApp::ExitInstance() 
{
	PT_Release();
	g_Render.Release();
	g_Log.Release();
	
	af_Finalize();

	return CWinApp::ExitInstance();
}

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
void CSectorEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorApp message handlers

BOOL CSectorEditorApp::OnIdle(LONG lCount) 
{
	static DWORD dwLastFrame = 0;

	CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
	ASSERT(pMainFrame);

	if (!pMainFrame->AppIsActive())
	{
		dwLastFrame = 0;
		return CWinApp::OnIdle(lCount);
	}

	DWORD dwTime = dwLastFrame ? a_GetTime() - dwLastFrame : 0;
	dwLastFrame = a_GetTime();
	if (dwTime)
	{
		//	Do frame move
		pMainFrame->GetView()->FrameMove(dwTime);
	}

	pMainFrame->GetView()->Render();

	//	Let window update menu item states
	CWinApp::OnIdle(lCount);
	
	return TRUE;
}

//	Do some initial work
bool CSectorEditorApp::InitApp()
{
	//	Initalize random generator
	srand((unsigned)time(NULL));

	//	Get app's path
	GetModuleFileName(NULL, g_szAppPath, MAX_PATH);
	char* pTemp = strrchr(g_szAppPath, '\\');
	*(pTemp+1) = '\0';

	//	Get work path
	af_Initialize();

	g_Log.Init("SectorEditor.log", "Sector editor log");

	//	Set default configs
	g_Configs.bLightmap		= false;
	g_Configs.bShowGrid		= true;
	g_Configs.bShowScene	= true;
	g_Configs.bShowSector	= true;
	g_Configs.bShowPortal	= true;
	g_Configs.bForceRender	= false;
	g_Configs.bFullClip		= true;
	g_Configs.iCameraMove	= 2;
	g_Configs.iCameraRotate	= 2;
	g_Configs.iNumWarnFace	= 20000;

	if (!PT_Init())
		return false;

	return true;
}



