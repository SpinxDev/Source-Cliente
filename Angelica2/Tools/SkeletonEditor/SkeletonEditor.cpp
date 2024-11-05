// SkeletonEditor.cpp : Defines the class behaviors for the application.
//

#include "Global.h"
#include "SkeletonEditor.h"
#include "MainFrm.h"
#include "SkeletonEditorView.h"
#include "SkeletonEditorDoc.h"
#include "DlgSelectFile.h"
#include "CommonFileName.h"

#include "ATime.h"
#include "Render.h"
#include "AF.h"
#include "ALog.h"
#include <time.h>

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorApp

BEGIN_MESSAGE_MAP(CSkeletonEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CSkeletonEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CSkeletonEditorApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorApp construction

CSkeletonEditorApp::CSkeletonEditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_dwLastFrame	= 0;
	m_dwTickTime	= 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSkeletonEditorApp object

CSkeletonEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorApp initialization

BOOL CSkeletonEditorApp::InitInstance()
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
		RUNTIME_CLASS(CSkeletonEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSkeletonEditorView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	AUX_GetMainFrame()->GetDocument()->OnNewDocument();

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
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
void CSkeletonEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorApp message handlers

int CSkeletonEditorApp::ExitInstance() 
{
	g_Render.Release();
	g_Log.Release();
	
	af_Finalize();
	
	return CWinApp::ExitInstance();
}

//	Do some initial work
bool CSkeletonEditorApp::InitApp()
{
	//	Get current directory
	char szAppPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szAppPath);
	strcat(szAppPath, "\\");

	strcpy(g_szIniFile, szAppPath);
	strcat(g_szIniFile, "SkeletonEditor.ini");

	//	Get work path from .ini file
//	GetPrivateProfileString("Path", "WorkPath", szAppPath, g_szWorkDir, MAX_PATH, g_szIniFile);
	strcpy(g_szWorkDir, szAppPath);

	//	Set work path
	af_Initialize();
	af_SetBaseDir(g_szWorkDir);

	//	Create temporary directory
	char szTempDir[MAX_PATH];
	sprintf(szTempDir, "%sTemp", g_szWorkDir);
	AUX_CreateDirectory(szTempDir);

	//	Create log file
	g_Log.Init("SkeletonEditor.log", "Skeleton editor log");

	//	Set default configs
	g_Configs.bSkeletonOnly		= true;
	g_Configs.bShowBoundBox		= false;
	g_Configs.iStaffSegment		= 6;
	g_Configs.bShowPhyShape		= false;
	g_Configs.iEditPhyShape		= -1;
	g_Configs.bShowBoneCoord	= true;
	g_Configs.bShowGround		= true;
	g_Configs.bShowNormal		= false;

	//	Load configs from .ini file
	GetPrivateProfileString("Configs", "BoxSize", "0.2f", szAppPath, MAX_PATH, g_szIniFile);
	g_Configs.fBoneBoxSize = (float)atof(szAppPath);

	GetPrivateProfileString("Configs", "StaffRadius", "0.1f", szAppPath, MAX_PATH, g_szIniFile);
	g_Configs.fStaffRadius = (float)atof(szAppPath);

	g_Configs.iCameraMoveSpeed = GetPrivateProfileInt("Configs", "CameraMove", 2, g_szIniFile);
	g_Configs.iCameraRotSpeed = GetPrivateProfileInt("Configs", "CameraRot", 4, g_szIniFile);
	g_Configs.iMouseDragSpeed = GetPrivateProfileInt("Configs", "MouseDrag", 4, g_szIniFile);

	//	Initialize random number generator
	srand((unsigned)time(NULL));

	m_dwLastFrame = a_GetTime();

	return true;
}

BOOL CSkeletonEditorApp::OnIdle(LONG lCount) 
{
	CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
	ASSERT(pMainFrame);

	if (!pMainFrame->AppIsActive())
		return CWinApp::OnIdle(lCount);

	m_dwTickTime = m_dwLastFrame ? a_GetTime() - m_dwLastFrame : 0;

	if (m_dwTickTime)
	{
		m_dwLastFrame = a_GetTime();

		a_ClampRoof(m_dwTickTime, (DWORD)80);
			
		//	Do frame move
		pMainFrame->GetView()->FrameMove(m_dwTickTime);

		pMainFrame->GetView()->Render();
	}

	//	Let window update menu item states
	CWinApp::OnIdle(lCount);
	
	return TRUE;
}

void CSkeletonEditorApp::OnFileNew() 
{
	CWinApp::OnFileNew();
}

void CSkeletonEditorApp::OnFileOpen()
{
/*	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= false;
	Params.szFileFilter	= "*.smd";
	Params.szPrompt		= "请选择一个模型文件";
	Params.szRootDir	= g_szWorkDir;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;

	//	Open document
	CWinApp::OpenDocumentFile(FileDlg.GetFullPathName());
*/
	CCommonFileName FileDlg(g_Render.GetA3DEngine(), 0);
	FileDlg.SetFileExtension("smd");
	if (FileDlg.DoModal() != IDOK)
		return;

	//	Open document
	CWinApp::OpenDocumentFile(FileDlg.GetFullFileName());
}

//	Discard frame
void CSkeletonEditorApp::DiscardFrame()
{
	m_dwLastFrame = a_GetTime();
}

