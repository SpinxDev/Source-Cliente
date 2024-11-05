// Terrain2View.cpp : Defines the class behaviors for the application.
//

#include "Global.h"
#include "Terrain2View.h"

#include "MainFrm.h"
#include "Terrain2ViewDoc.h"
#include "Terrain2ViewView.h"
#include "Render.h"
#include "CommonFileName.h"
#include "Settings.h"
#include "A3DTerrain2Wrapper.h"

#include "DlgInputFlot.h"
#include "DlgPosition.h"
#include "DlgTerrainLOD.h"

#include "AFI.h"
#include "ALog.h"

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
// CTerrain2ViewApp

BEGIN_MESSAGE_MAP(CTerrain2ViewApp, CWinApp)
	//{{AFX_MSG_MAP(CTerrain2ViewApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_VIEW_BACKCOL, OnViewBackcol)
	ON_COMMAND(ID_VIEW_WIREFRAME, OnViewWireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIREFRAME, OnUpdateViewWireframe)
	ON_COMMAND(ID_VIEW_OCCLUSIONLINE, OnViewOcclusionline)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OCCLUSIONLINE, OnUpdateViewOcclusionline)
	ON_COMMAND(ID_VIEW_RENDERTRN, OnViewRendertrn)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RENDERTRN, OnUpdateViewRendertrn)
	ON_COMMAND(ID_VIEW_CAMERASPEED, OnViewCameraspeed)
	ON_COMMAND(ID_VIEW_USEHORILINE, OnViewUsehoriline)
	ON_UPDATE_COMMAND_UI(ID_VIEW_USEHORILINE, OnUpdateViewUsehoriline)
	ON_COMMAND(ID_VIEW_BQ_OCCLUSION, OnViewBqOcclusion)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BQ_OCCLUSION, OnUpdateViewBqOcclusion)
	ON_COMMAND(ID_VIEW_BQ_TEST, OnViewBqTest)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BQ_TEST, OnUpdateViewBqTest)
	ON_COMMAND(ID_VIEW_BQ_HIGHLIGHT, OnViewBqHighlight)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BQ_HIGHLIGHT, OnUpdateViewBqHighlight)
	ON_COMMAND(ID_VIEW_PROMPT, OnViewPrompt)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROMPT, OnUpdateViewPrompt)
	ON_COMMAND(ID_VIEW_CAMERAPOS, OnViewCamerapos)
	ON_COMMAND(ID_VIEW_VIEWRADIUS, OnViewViewradius)
	ON_COMMAND(ID_VIEW_TERRAINLOD, OnViewTerrainlod)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewApp construction

CTerrain2ViewApp::CTerrain2ViewApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTerrain2ViewApp object

CTerrain2ViewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewApp initialization

BOOL CTerrain2ViewApp::InitInstance()
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
		RUNTIME_CLASS(CTerrain2ViewDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTerrain2ViewView));
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

int CTerrain2ViewApp::ExitInstance() 
{
	//	Release render
	g_Render.Release();

	l_Log.Release();
	a_RedirectDefLogOutput(NULL);

	//	Finalize af module
	af_Finalize();

	return CWinApp::ExitInstance();
}

//	Do some initial work
bool CTerrain2ViewApp::InitApp()
{
	//	Initialize af module
	af_Initialize();

	//	Get current directory as angelica's work path
	char szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	af_SetBaseDir(szCurPath);

	//	Create log file and redirect log output
	if (!l_Log.Init("Terrain2View.log", "Angelica Terrain2View log file"))
		return false;

	a_RedirectDefLogOutput(_LogOutput);

	//	Try to open texture package if it exist
	if (g_AFilePackMan.OpenFilePackage("Textures.pck"))
	{
		a_LogOutput(0, "Read textures from package: textures.pck");
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CTerrain2ViewApp message handlers

// App command to run the dialog
void CTerrain2ViewApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CTerrain2ViewApp::OnIdle(LONG lCount) 
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
		pMainFrame->GetDocument()->Tick(dwTickTime);
		pMainFrame->GetView()->Tick(dwTickTime);

		//	Render scene
		pMainFrame->GetView()->Render();
	}

	//	Let window update menu item states
	CWinApp::OnIdle(lCount);
	
	return TRUE;
}

void CTerrain2ViewApp::OnFileOpen() 
{
	CCommonFileName FileDlg(g_Render.GetA3DEngine(), 0);
	FileDlg.SetFileExtension("trn2");
	if (FileDlg.DoModal() != IDOK)
		return;

	//	Open document
	CWinApp::OpenDocumentFile(FileDlg.GetFullFileName());
}

void CTerrain2ViewApp::OnViewBackcol() 
{
	DWORD dwCol = m_Settings.GetBackColor();
	BYTE r = A3DCOLOR_GETRED(dwCol);
	BYTE g = A3DCOLOR_GETGREEN(dwCol);
	BYTE b = A3DCOLOR_GETBLUE(dwCol);

	CColorDialog clrdlg(A3DCOLORRGB(b, g, r), NULL, NULL);
	if (clrdlg.DoModal() == IDOK)
	{
		DWORD dwCol = clrdlg.GetColor() | 0xff000000;
		BYTE r = A3DCOLOR_GETRED(dwCol);
		BYTE g = A3DCOLOR_GETGREEN(dwCol);
		BYTE b = A3DCOLOR_GETBLUE(dwCol);

		m_Settings.SetBackColor(A3DCOLORRGB(b, g, r));
	}
}

void CTerrain2ViewApp::OnViewWireframe() 
{
	bool bWireFrame = m_Settings.GetWireFrame();
	m_Settings.SetWireFrame(!bWireFrame);
}

void CTerrain2ViewApp::OnUpdateViewWireframe(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Settings.GetWireFrame());
}

void CTerrain2ViewApp::OnViewBqOcclusion() 
{
	int iCurType = m_Settings.GetBlockQuadShow();
	if (iCurType & CSettings::BQS_OCCLUSION)
		m_Settings.SetBlockQuadShow(iCurType & ~CSettings::BQS_OCCLUSION);
	else
		m_Settings.SetBlockQuadShow(iCurType | CSettings::BQS_OCCLUSION);
}

void CTerrain2ViewApp::OnUpdateViewBqOcclusion(CCmdUI* pCmdUI) 
{
	bool bCheck = (m_Settings.GetBlockQuadShow() & CSettings::BQS_OCCLUSION) ? true : false;
	pCmdUI->SetCheck(bCheck);
}

void CTerrain2ViewApp::OnViewBqTest() 
{
	int iCurType = m_Settings.GetBlockQuadShow();
	if (iCurType & CSettings::BQS_TEST)
		m_Settings.SetBlockQuadShow(iCurType & ~CSettings::BQS_TEST);
	else
		m_Settings.SetBlockQuadShow(iCurType | CSettings::BQS_TEST);
}

void CTerrain2ViewApp::OnUpdateViewBqTest(CCmdUI* pCmdUI) 
{
	bool bCheck = (m_Settings.GetBlockQuadShow() & CSettings::BQS_TEST) ? true : false;
	pCmdUI->SetCheck(bCheck);
}

void CTerrain2ViewApp::OnViewBqHighlight() 
{
	int iCurType = m_Settings.GetBlockQuadShow();
	if (iCurType & CSettings::BQS_PICKED)
		m_Settings.SetBlockQuadShow(iCurType & ~CSettings::BQS_PICKED);
	else
		m_Settings.SetBlockQuadShow(iCurType | CSettings::BQS_PICKED);
}

void CTerrain2ViewApp::OnUpdateViewBqHighlight(CCmdUI* pCmdUI) 
{
	bool bCheck = (m_Settings.GetBlockQuadShow() & CSettings::BQS_PICKED) ? true : false;
	pCmdUI->SetCheck(bCheck);
}

void CTerrain2ViewApp::OnViewOcclusionline() 
{
	bool bShow = m_Settings.GetHoriLineShow();
	m_Settings.SetHoriLineShow(!bShow);
}

void CTerrain2ViewApp::OnUpdateViewOcclusionline(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Settings.GetHoriLineShow());
}

void CTerrain2ViewApp::OnViewRendertrn() 
{
	bool bShow = m_Settings.GetTerrainRender();
	m_Settings.SetTerrainRender(!bShow);
}

void CTerrain2ViewApp::OnUpdateViewRendertrn(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Settings.GetTerrainRender());
}

void CTerrain2ViewApp::OnViewCameraspeed() 
{
	CDlgInputFlot dlg;
	dlg.m_fValue = m_Settings.GetCameraSpeed();
	dlg.m_strPrompt = "设置摄像机移动速度";

	if (dlg.DoModal() == IDOK)
	{
		m_Settings.SetCameraSpeed(dlg.m_fValue);
	}
}

void CTerrain2ViewApp::OnViewUsehoriline() 
{
	bool bUse = m_Settings.GetHoriLineUse();
	m_Settings.SetHoriLineUse(!bUse);
}

void CTerrain2ViewApp::OnUpdateViewUsehoriline(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Settings.GetHoriLineUse());
}

void CTerrain2ViewApp::OnViewPrompt() 
{
	bool bShow = m_Settings.GetPromptShow();
	m_Settings.SetPromptShow(!bShow);
}

void CTerrain2ViewApp::OnUpdateViewPrompt(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Settings.GetPromptShow());
}

void CTerrain2ViewApp::OnViewCamerapos() 
{
	A3DVECTOR3 vPos = g_Render.GetCamera()->GetPos();

	CDlgPosition dlg;
	dlg.m_fx = vPos.x;
	dlg.m_fy = vPos.y;
	dlg.m_fz = vPos.z;

	if (dlg.DoModal() != IDOK)
		return;

	vPos.x = dlg.m_fx;
	vPos.y = dlg.m_fy;
	vPos.z = dlg.m_fz;

	A3DTerrain2Wrapper* pTerrain = glb_GetMainFrame()->GetDocument()->GetTerrain();
	if (pTerrain)
	{
		pTerrain->SetLoadCenter(vPos);
		float fHei = pTerrain->GetPosHeight(vPos);
		if (vPos.y < fHei + 0.1f)
			vPos.y = fHei + 0.1f;
	}

	g_Render.GetCamera()->SetPos(vPos);
}

void CTerrain2ViewApp::OnViewViewradius() 
{
	CDlgInputFlot dlg;
	dlg.m_fValue = m_Settings.GetViewDist();
	dlg.m_fMin = 10.0f;
	dlg.m_fMax = 500.0f;
	dlg.m_strPrompt = "设置视野半径";

	if (dlg.DoModal() == IDOK)
	{
		m_Settings.SetViewDist(dlg.m_fValue);
	}
}

void CTerrain2ViewApp::OnViewTerrainlod() 
{
	CDlgTerrainLOD dlg;
	dlg.m_iLODType = m_Settings.GetLODType();
	dlg.m_fDist1 = m_Settings.GetLODDist1();
	dlg.m_fDist2 = m_Settings.GetLODDist2();

	if (dlg.DoModal() == IDOK)
	{
		m_Settings.SetLODType(dlg.m_iLODType);
		m_Settings.SetLODDist(dlg.m_fDist1, dlg.m_fDist2);
	}
}
