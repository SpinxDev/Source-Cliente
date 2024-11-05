// MainFrm.cpp : implementation of the CMainFrame class
//

#include "Global.h"
#include "SectorEditor.h"
#include "MainFrm.h"
#include "SectorEditorView.h"
#include "SectorEditorDoc.h"

#include "EditObject.h"
#include "DlgPortalSector.h"
#include "DlgOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define WNDID_MESSAGEDIALOG		100

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
	ON_WM_SIZING()
	ON_UPDATE_COMMAND_UI(ID_VIEW_FORCERENDER, OnUpdateViewForcerender)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLCLIP, OnUpdateViewFullclip)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEST, OnUpdateViewTest)
	ON_COMMAND(ID_EDIT_VIEWINFO, OnEditViewinfo)
	ON_COMMAND(ID_VIEW_MESSAGEWND, OnViewMessagewnd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MESSAGEWND, OnUpdateViewMessagewnd)
	ON_COMMAND(ID_EDIT_OPTIONS, OnEditOptions)
	//}}AFX_MSG_MAP

	ON_MESSAGE(MSG_ENDOPERATION, OnEndOperation)

	ON_COMMAND_RANGE(ID_VIEW_CAMERASPEED0, ID_VIEW_CAMERASPEED4, OnCameraMoveLevel)
	ON_COMMAND_RANGE(ID_VIEW_CAMERAROTATE0, ID_VIEW_CAMERAROTATE4, OnCameraRotateLevel)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_CAMERASPEED0, ID_VIEW_CAMERASPEED4, OnUpdateCameraMoveLevel)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_CAMERAROTATE0, ID_VIEW_CAMERAROTATE4, OnUpdateCameraRotateLevel)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_SHOWGRID, ID_VIEW_SHOWPORTAL, OnUpdateShowHideFlag)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           //	Status line indicator
	ID_INDICATOR_MOUSEPOS,	//	Mouse position
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bActive	= false;
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
		!m_wndStatusBar.SetIndicators(indicators, sizeof (indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_MOUSEPOS, 0, 200);
	m_wndStatusBar.SetPaneText(1, "", TRUE);

	if (!InitMessageDlgBar())
	{
		g_Log.Log("CMainFrame::OnCreate, Failed to create message dialog bar");
		return -1;
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

//	Initialize message dialog bar
bool CMainFrame::InitMessageDlgBar()
{
	//	Create picture dialog bar
	if (!m_MessageWnd.Create(this, IDD_MESSAGE, CBRS_BOTTOM | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED, WNDID_MESSAGEDIALOG))
	{
		g_Log.Log("CMainFrame::InitPictureDlgBar, Failed to create picture dialog bar");
		return false;
	}

	EnableDocking(CBRS_FLOAT_MULTI);

	m_MessageWnd.EnableDocking(CBRS_FLOAT_MULTI);
	FloatControlBar(&m_MessageWnd, CPoint(500, 200));
	m_MessageWnd.SetWindowText("ÏûÏ¢");

	ShowMessageWnd(false);

	return true;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.x  = 0;
	cs.y  = 0;
	cs.cx = GetSystemMetrics(SM_CXFULLSCREEN);
	cs.cy = GetSystemMetrics(SM_CYFULLSCREEN);

	if (cs.cx < MIN_WIDTH || cs.cy < MIN_HEIGHT)
		return FALSE;

	return TRUE;
}

CSectorEditorDoc* CMainFrame::GetDocument()
{
	CSectorEditorDoc* pDoc = (CSectorEditorDoc*)GetActiveDocument();
	ASSERT(pDoc);
	return pDoc;
}

CSectorEditorView* CMainFrame::GetView()
{
	CSectorEditorView* pView = (CSectorEditorView*)GetActiveView();
	ASSERT(pView);
	return pView;
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

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect) 
{
	if (MIN_WIDTH > pRect->right - pRect->left)
	{
		if (fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_LEFT || fwSide == WMSZ_TOPLEFT)
			pRect->left = pRect->right - MIN_WIDTH;
		else
			pRect->right = pRect->left + MIN_WIDTH;
	}

	if (MIN_HEIGHT > pRect->bottom - pRect->top)
	{
		if (fwSide == WMSZ_TOP || fwSide == WMSZ_TOPLEFT || fwSide == WMSZ_TOPRIGHT)
			pRect->top = pRect->bottom - MIN_HEIGHT;
		else
			pRect->bottom = pRect->top + MIN_HEIGHT;
	}

	CFrameWnd::OnSizing(fwSide, pRect);
}

//	Update mouse position
void CMainFrame::UpdateMousePos(A3DVECTOR3* pvPos)
{
	if (!pvPos)
	{
		m_wndStatusBar.SetPaneText(1, "", TRUE);
		return;
	}

	char szText[100];
	sprintf(szText, "x:%.4f, y:%.4f, z:%.4f", pvPos->x, pvPos->y, pvPos->z);
	m_wndStatusBar.SetPaneText(1, szText, TRUE);
}

//	End current operation message
void CMainFrame::OnEndOperation(WPARAM wParam, LPARAM lParam)
{
	GetDocument()->EndCurOperation(wParam ? true : false);
}

void CMainFrame::OnEditViewinfo() 
{
	CDlgPortalSector InfoDlg(GetDocument());
	if (InfoDlg.DoModal() == IDOK)
	{
		CEditObject* pSelObj = InfoDlg.GetSelObject();
		if (pSelObj)
			GetDocument()->SelectEditObject(pSelObj);
	}
}

void CMainFrame::OnViewMessagewnd() 
{
	if (m_MessageWnd.IsWindowVisible()) 
		ShowControlBar(&m_MessageWnd, FALSE, FALSE);
	else 
		ShowControlBar(&m_MessageWnd, TRUE, FALSE);
}

//	Show/Hide message window
void CMainFrame::ShowMessageWnd(bool bShow)
{
	if (bShow)
		ShowControlBar(&m_MessageWnd, TRUE, FALSE);
	else
		ShowControlBar(&m_MessageWnd, FALSE, FALSE);
}

void CMainFrame::OnCameraMoveLevel(UINT nID)
{
	g_Configs.iCameraMove = nID - ID_VIEW_CAMERASPEED0;
}

void CMainFrame::OnCameraRotateLevel(UINT nID)
{
	g_Configs.iCameraRotate = nID - ID_VIEW_CAMERAROTATE0;
}

void CMainFrame::OnEditOptions() 
{
	CDlgOptions OptDlg;
	
	if (OptDlg.DoModal() == IDOK)
	{
	}
}


