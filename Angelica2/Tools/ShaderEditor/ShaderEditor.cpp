// ShaderEditor.cpp : implementation file
//

#include "CommonDef.h"
#include "shadereditor.h"
#include "MainForm.h"
#include <AF.h>

#define new A_DEBUG_NEW

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


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

CShaderEditor	g_theApp;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
//	CShaderEditor
//
///////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CShaderEditor, CWinApp)

CShaderEditor::CShaderEditor()
{
}

CShaderEditor::~CShaderEditor()
{
}

//	Initialize instance
BOOL CShaderEditor::InitInstance()
{
	CoInitialize(NULL);

	//	Get initial information
	GetInitialInfo();

	//	Create main window
	CMainForm* pMainForm = new CMainForm;
	if (!pMainForm)
		return FALSE;

	m_strClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, LoadCursor(IDC_ARROW),
										(HBRUSH)GetStockObject(BLACK_BRUSH), LoadIcon(MAKEINTRESOURCE(IDI_APPICON)));

	//	Create window at center of screen
	RECT rcWnd;
	int iScrWid = GetSystemMetrics(SM_CXFULLSCREEN);
	int iScrHei = GetSystemMetrics(SM_CYFULLSCREEN);

	rcWnd.left	 = (iScrWid - 640) / 2;
	rcWnd.top	 = (iScrHei - 480) / 2;
	rcWnd.right	 = rcWnd.left + 640;
	rcWnd.bottom = rcWnd.top + 480;

	if (!pMainForm->Create((LPCTSTR)m_strClassName, "Shader Editor", WS_OVERLAPPEDWINDOW,
							rcWnd, NULL, MAKEINTRESOURCE(IDR_MAINMENU), 0, NULL))
	{
		TRACE("Failed to create main form!\n");
		return FALSE;
	}

	m_pMainWnd = pMainForm;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CShaderEditor::ExitInstance()
{
	CoUninitialize();

	af_Finalize();

	return CWinApp::ExitInstance();
}

BEGIN_MESSAGE_MAP(CShaderEditor, CWinApp)
	//{{AFX_MSG_MAP(CShaderEditor)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//	CShaderEditor message handlers
//
/////////////////////////////////////////////////////////////////////////////

BOOL CShaderEditor::OnIdle(LONG lCount) 
{
	if (!m_pMainWnd || !g_bActive)
		return CWinApp::OnIdle(lCount);

	((CMainForm*)m_pMainWnd)->GetRenderWnd()->Render();

	return TRUE;
}

//	Get initial information from .ini file
void CShaderEditor::GetInitialInfo()
{
	//	Get app's path
	char szAppPath[MAX_PATH], *pTemp;

	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	pTemp = strrchr(szAppPath, '\\');
	*(pTemp + 1) = '\0';

	strcpy(g_szIniFile, szAppPath);
	strcat(g_szIniFile, "ShaderEditor.ini");

	//	Get work path
	GetPrivateProfileString("Path", "WorkPath", szAppPath, g_szWorkDir, MAX_PATH, g_szIniFile);

	//	Set work path
	af_Initialize();
	af_SetBaseDir(g_szWorkDir);
}


