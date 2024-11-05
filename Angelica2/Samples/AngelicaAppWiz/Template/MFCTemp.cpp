// $$root$$.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "$$root$$.h"

#include "MainFrm.h"
#include "$$root$$Doc.h"
#include "$$root$$View.h"
#include "Render.h"
#include "AFI.h"
#include "AC.h"

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
// C$$root$$App

BEGIN_MESSAGE_MAP(C$$root$$App, CWinApp)
	//{{AFX_MSG_MAP(C$$root$$App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C$$root$$App construction

C$$root$$App::C$$root$$App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only C$$root$$App object

C$$root$$App theApp;

/////////////////////////////////////////////////////////////////////////////
// C$$root$$App initialization

BOOL C$$root$$App::InitInstance()
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
		RUNTIME_CLASS(C$$root$$Doc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(C$$root$$View));
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

int C$$root$$App::ExitInstance() 
{
	//	Release render
	g_Render.Release();

	//	Finalize af module
	af_Finalize();

	return CWinApp::ExitInstance();
}

//	Do some initial work
bool C$$root$$App::InitApp()
{
	//	Initialize af module
	af_Initialize();

	//	Get current directory as angelica's work path
	char szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	af_SetBaseDir(szCurPath);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// C$$root$$App message handlers

// App command to run the dialog
void C$$root$$App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL C$$root$$App::OnIdle(LONG lCount) 
{
	CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
	ASSERT(pMainFrame);

	static DWORD dwLastFrame = 0;
	DWORD dwTickTime = dwLastFrame ? a_GetTime() - dwLastFrame : 1;

	if (dwTickTime)
	{
		dwLastFrame = a_GetTime();

		//	Limit time of a logic tick
		a_ClampRoof(dwTickTime, (DWORD)100);
			
		//	Do logic tick
		pMainFrame->GetView()->Tick(dwTickTime);

		//	Render scene
		pMainFrame->GetView()->Render();
	}

	//	Let window update menu item states
	CWinApp::OnIdle(lCount);
	
	return TRUE;
}
