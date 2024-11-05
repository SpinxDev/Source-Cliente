// PhysDemo.cpp : Defines the class behaviors for the application.
//

#include "Global.h"
#include "PhysDemo.h"

#include "MainFrm.h"
#include "PhysDemoDoc.h"
#include "PhysDemoView.h"
#include "Render.h"
#include "Game.h"

#include <AFI.h>
#include <AC.h>

static ALog	l_Log;

static void _LogOutput(const char* szMsg)
{
	l_Log.Log(szMsg);
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

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoApp

BEGIN_MESSAGE_MAP(CPhysDemoApp, CWinApp)
	//{{AFX_MSG_MAP(CPhysDemoApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoApp construction

CPhysDemoApp::CPhysDemoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPhysDemoApp object

CPhysDemoApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoApp initialization

BOOL CPhysDemoApp::InitInstance()
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

	//	Do some initial work
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
		RUNTIME_CLASS(CPhysDemoDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CPhysDemoView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	//	Initialize game object
	if (!g_Game.Init())
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CPhysDemoApp::ExitInstance() 
{
	//	Release game
	g_Game.Release();

	//	Release render
	g_Render.Release();

	l_Log.Release();
	a_RedirectDefLogOutput(NULL);

	//	Finalize af module
	af_Finalize();

	return CWinApp::ExitInstance();
}

//	Do some initial work
bool CPhysDemoApp::InitApp()
{
	//	Get current directory as angelica's work path
	char szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);

	strcpy(g_szWorkDir, szCurPath);

	//	Initialize af module
	af_Initialize();
	af_SetBaseDir(szCurPath);

	//	Create log file and redirect log output
	if (!l_Log.Init("PhysDemo.log", "Angelica PhysDemo log file"))
		return false;

	a_RedirectDefLogOutput(_LogOutput);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoApp message handlers

// App command to run the dialog
void CPhysDemoApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CPhysDemoApp::OnIdle(LONG lCount) 
{
	CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
	ASSERT(pMainFrame);

	if (!pMainFrame->AppIsActive())
		return CWinApp::OnIdle(lCount);

	pMainFrame->Tick();

	//	Let window update menu item states
	CWinApp::OnIdle(lCount);
	
	return TRUE;
}
