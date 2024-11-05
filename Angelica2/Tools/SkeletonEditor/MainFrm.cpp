// MainFrm.cpp : implementation of the CMainFrame class
//

#include "Global.h"
#include "MainFrm.h"
#include "SkeletonEditorView.h"
#include "SkeletonEditorDoc.h"
#include "SkeletonEditor.h"
#include "DlgMergeSkeleton.h"

#include "AF.h"
#include "A3D.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CMainFrame
//
///////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_VIEW_SKELETONONLY, OnViewSkeletononly)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SKELETONONLY, OnUpdateViewSkeletononly)
	ON_COMMAND(ID_VIEW_PHYSIQUESHAPE, OnViewPhysiqueshape)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PHYSIQUESHAPE, OnUpdateViewPhysiqueshape)
	ON_COMMAND(ID_VIEW_BOUNDBOX, OnViewBoundbox)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BOUNDBOX, OnUpdateViewBoundbox)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CHANGESKIN, OnUpdateEditChangeskin)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDPHYSIQUE, OnUpdateEditAddphysique)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REMOVEPHYSIQUE, OnUpdateEditRemovephysique)
	ON_COMMAND(ID_EDIT_ACTFACTORY, OnEditActfactory)
	ON_COMMAND(ID_VIEW_GROUND, OnViewGround)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GROUND, OnUpdateViewGround)
	ON_COMMAND(ID_VIEW_NORMAL, OnViewNormal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NORMAL, OnUpdateViewNormal)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if (!InitToolTabWnd())
	{
		g_Log.Log("CMainFrame::OnCreate, Failed to create tool tab window");
		return -1;
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	//	TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.cx	= 605;
	cs.cy	= 700;
	cs.x	= 0;
	cs.y	= 0;

	return TRUE;
}

CSkeletonEditorDoc* CMainFrame::GetDocument()
{
	CSkeletonEditorDoc* pDoc = (CSkeletonEditorDoc*)GetActiveDocument();
	ASSERT(pDoc);
	return pDoc;
}

CSkeletonEditorView* CMainFrame::GetView()
{
	CSkeletonEditorView* pView = (CSkeletonEditorView*)GetActiveView();
	ASSERT(pView);
	return pView;
}

//	Initialize tool tab window
bool CMainFrame::InitToolTabWnd()
{
	CRect Rect(0, 0, 100, 100);

	if (!m_ToolTabWnd.Create(NULL, "工具栏", WS_POPUP | WS_CAPTION, Rect, this, NULL))
	{
		g_Log.Log("CMainForm::InitRenderWnd, Failed to create tool tab window.\n");
		return false;
	}

	//	This trigger WM_SIZE message for m_ToolTabWnd
	Rect.SetRect(605, 0, 1024, 700);
	m_ToolTabWnd.MoveWindow(&Rect, FALSE);

	m_ToolTabWnd.ShowWindow(SW_SHOW);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	
	m_bActive = bActive ? true : false;
}
/*
void CMainFrame::OnFileSetworkdir() 
{
	BROWSEINFO Info;
	LPITEMIDLIST pPIDL;
	char szPath[MAX_PATH];

	memset(&Info, 0, sizeof (Info));
	Info.hwndOwner		= m_hWnd;
	Info.pidlRoot		= NULL;
	Info.pszDisplayName	= szPath;
	Info.lpszTitle		= "请选择工作路径：";
	Info.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;

	if ((pPIDL = SHBrowseForFolder(&Info)))
	{
		SHGetPathFromIDList(pPIDL, szPath);

		//	Add a '\' at tail
		sprintf(g_szWorkDir, "%s\\", szPath);

		//	Save work path to .ini file
		WritePrivateProfileString("Path", "WorkPath", g_szWorkDir, g_szIniFile);

		//	Reset work path
		af_SetBaseDir(g_szWorkDir);
	}
}
*/
BOOL CMainFrame::DestroyWindow() 
{
	m_ToolTabWnd.Release();

	return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnViewSkeletononly() 
{
	g_Configs.bSkeletonOnly = !g_Configs.bSkeletonOnly;
}

void CMainFrame::OnViewPhysiqueshape() 
{
	g_Configs.bShowPhyShape = !g_Configs.bShowPhyShape;
}

//	Bind model
bool CMainFrame::BindModel(CModel* pModel)
{
	if (!m_ToolTabWnd.BindModel(pModel))
		return false;

	return true;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnEditActfactory() 
{
	CDlgMergeSkeleton MergeDlg;
	MergeDlg.DoModal();

	//	Discard this frame
	AUX_GetApp()->DiscardFrame();
}

void CMainFrame::OnViewBoundbox() 
{
	g_Configs.bShowBoundBox = !g_Configs.bShowBoundBox;

	if (g_pA3DConfig)
		g_pA3DConfig->RT_SetShowBoundBoxFlag(g_Configs.bShowBoundBox);
}

void CMainFrame::OnViewGround() 
{
	g_Configs.bShowGround = !g_Configs.bShowGround;
}

void CMainFrame::OnViewNormal() 
{
	g_Configs.bShowNormal = !g_Configs.bShowNormal;
}

