// MainFrm.cpp : implementation of the CMainFrame class
//

#include "Global.h"
#include "PhysDemo.h"
#include "MainFrm.h"
#include "PhysDemoView.h"
#include "Game.h"

#include <ATime.h>
#include <AAssist.h>

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_ENTERIDLE()
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_FILE_RESET, OnFileReset)
	ON_COMMAND(ID_VIEW_WIRE, OnViewWire)
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
	m_bActive = false;
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
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
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

CPhysDemoDoc* CMainFrame::GetDocument()
{
	CPhysDemoDoc* pDoc = (CPhysDemoDoc*)GetActiveDocument();
	ASSERT(pDoc);
	return pDoc;
}

CPhysDemoView* CMainFrame::GetView()
{
	CPhysDemoView* pView = (CPhysDemoView*)GetActiveView();
	ASSERT(pView);
	return pView;
}

void CMainFrame::OnEnterIdle(UINT nWhy, CWnd* pWho) 
{
	CFrameWnd::OnEnterIdle(nWhy, pWho);

	Tick();
}

void CMainFrame::Tick()
{
	static DWORD dwLastFrame = 0;
	DWORD dwTickTime = dwLastFrame ? a_GetTime() - dwLastFrame : 1;

	if (dwTickTime)
	{
		dwLastFrame = a_GetTime();

		//	Limit time of a logic tick
		a_ClampRoof(dwTickTime, (DWORD)100);
			
		//	Do logic tick
		GetView()->Tick(dwTickTime);

		//	Render scene
		GetView()->Render();
	}
}

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	
	m_bActive = bActive ? true : false;
}

void CMainFrame::OnFileReset() 
{
	if (MessageBox("确定要重置系统么？", "提示", MB_YESNO) == IDNO)
		return;

	g_Game.Reset();	
}

void CMainFrame::OnViewWire() 
{
	g_Game.SetWireModeFlag(!g_Game.GetWireModeFlag());
}
