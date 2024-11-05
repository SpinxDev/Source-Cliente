// SectorEditorView.cpp : implementation of the CSectorEditorView class
//

#include "Global.h"
#include "SectorEditor.h"
#include "SectorEditorView.h"
#include "SectorEditorDoc.h"
#include "MainFrm.h"
#include "A3DCamera.h"

#include "Utility.h"
#include "WndXY.h"
#include "WndXZ.h"
#include "WndYZ.h"
#include "WndPerspective.h"
#include "Bound.h"
#include "OptBase.h"
#include "SceneFrame.h"
#include "WireCollector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
//	Implement CSectorEditorView
//
///////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSectorEditorView, CView)

BEGIN_MESSAGE_MAP(CSectorEditorView, CView)
	//{{AFX_MSG_MAP(CSectorEditorView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_POPUP_CANCELBOUND, OnPopupCancelbound)
	ON_COMMAND(ID_VIEW_FULLCLIP, OnViewFullclip)
	//}}AFX_MSG_MAP

	ON_COMMAND_RANGE(ID_VIEW_MAXVIEW0, ID_VIEW_RESTORE4, OnMaxViewport)
	ON_COMMAND_RANGE(ID_VIEW_SHOWGRID, ID_VIEW_SHOWPORTAL, OnShowHideFlag)
	ON_COMMAND_RANGE(ID_POPUP_LEFTBOUND, ID_POPUP_BOTTOMBOUND, OnMoveBound)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorView construction/destruction

CSectorEditorView::CSectorEditorView()
{
	m_iActiveView	= -1;
	m_bFullMode		= false;
	m_bInitialized	= false;
	m_pBound		= NULL;

	m_LeftDrag.iMode	= DRAG_NONE;
	m_RightDrag.iMode	= DRAG_NONE;
}

CSectorEditorView::~CSectorEditorView()
{
}

BOOL CSectorEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorView drawing

void CSectorEditorView::OnDraw(CDC* pDC)
{
	CSectorEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorView diagnostics

#ifdef _DEBUG
void CSectorEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CSectorEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSectorEditorDoc* CSectorEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSectorEditorDoc)));
	return (CSectorEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSectorEditorView message handlers

void CSectorEditorView::OnInitialUpdate() 
{
	if (!m_bInitialized)
	{
		//	Initialize engine...
		m_bInitialized = true;

		//	Create render
		if (!g_Render.Init(AfxGetInstanceHandle(), m_hWnd))
		{
			g_Log.Log("CSectorEditorView::OnInitialUpdate, Failed to create render");
			return;
		}

		m_aViews[VIEW_XY] = new CXYWnd;
		m_aViews[VIEW_YZ] = new CYZWnd;
		m_aViews[VIEW_XZ] = new CXZWnd;
		m_aViews[VIEW_PERSPECTIVE] = new CPerspectiveWnd;

		//	Create viewports
		for (int i=0; i < NUM_VIEWPORT; i++)
		{
			if (!m_aViews[i])
			{
				g_Log.Log("CSectorEditorView::OnInitialUpdate, Failed to new viewport %d", i);
				return;
			}

			if (!m_aViews[i]->Init(&g_Render, 0, 0, 100, 100))
			{
				g_Log.Log("CSectorEditorView::OnInitialUpdate, Failed to create viewport %d", i);
				return;
			}
		}

		//	Create bound box
		m_pBound = new CBound;

		if (!m_pBound->Init(&g_Render, A3DVECTOR3(-WORLD_BOUND), A3DVECTOR3(WORLD_BOUND)))
		{
			g_Log.Log("CSectorEditorView::OnInitialUpdate, Failed to bound box");
			return;
		}
	}

	OnNewDocument();

	CView::OnInitialUpdate();
}

//	On new document
bool CSectorEditorView::OnNewDocument()
{
	m_iActiveView = 0;
	for (int i=0; i < NUM_VIEWPORT; i++)
	{
		m_aViews[i]->Reset();
		m_aViews[i]->Activate(i==m_iActiveView ? true : false);
	}

	//	Layout viewport
	LayoutViewports(false);

	//	Cancel bound limitation
	m_pBound->SetMinsAndMaxs(A3DVECTOR3(-WORLD_BOUND), A3DVECTOR3(WORLD_BOUND));

	return true;
}

void CSectorEditorView::OnDestroy() 
{
	//	Release bound box
	if (m_pBound)
	{
		m_pBound->Release();
		delete m_pBound;
		m_pBound = NULL;
	}

	//	Release viewports
	for (int i=0; i < NUM_VIEWPORT; i++)
	{
		if (m_aViews[i])
		{
			m_aViews[i]->Release();
			delete m_aViews[i];
			m_aViews[i] = NULL;
		}
	}

	m_bInitialized = false;

	CView::OnDestroy();
}

//	Layout viewports
void CSectorEditorView::LayoutViewports(bool bFullMode)
{
	m_bFullMode = bFullMode;

	//	Get render area size
	RECT rcClient;
	GetClientRect(&rcClient);

	if (bFullMode)
	{
		for (int i=0; i < NUM_VIEWPORT; i++)
			m_aViews[i]->EnableRender(i==m_iActiveView ? true : false);

		m_aViews[m_iActiveView]->Move(0, 0, rcClient.right, rcClient.bottom);
	}
	else
	{
		for (int i=0; i < NUM_VIEWPORT; i++)
			m_aViews[i]->EnableRender(true);

		int iWid1, iHei1, iWid2, iHei2;
		iWid1 = (rcClient.right - SPLITBARWIDTH) / 2;
		iHei1 = (rcClient.bottom - SPLITBARWIDTH) / 2;
		iWid2 = rcClient.right - iWid1 - SPLITBARWIDTH;
		iHei2 = rcClient.bottom - iHei1 - SPLITBARWIDTH;

		m_aViews[0]->Move(0, 0, iWid1, iHei1);
		m_aViews[1]->Move(iWid1+SPLITBARWIDTH, 0, iWid2, iHei1);
		m_aViews[2]->Move(0, iHei1+SPLITBARWIDTH, iWid1, iHei2);
		m_aViews[3]->Move(iWid1+SPLITBARWIDTH, iHei1+SPLITBARWIDTH, iWid2, iHei2);
	}
}

//	Switch full mode
void CSectorEditorView::SwitchFullMode()
{
	m_bFullMode = !m_bFullMode;

	RECT rcClient;
	GetClientRect(&rcClient);

	if (m_bFullMode)
	{
		//	Save viewport's size
		m_rcSaveArea.left	= m_aViews[m_iActiveView]->GetX();
		m_rcSaveArea.top	= m_aViews[m_iActiveView]->GetY();
		m_rcSaveArea.right	= m_aViews[m_iActiveView]->GetWidth();
		m_rcSaveArea.bottom	= m_aViews[m_iActiveView]->GetHeight();

		for (int i=0; i < NUM_VIEWPORT; i++)
		{
			if (i == m_iActiveView)
				m_aViews[i]->Move(0, 0, rcClient.right, rcClient.bottom);
			else
				m_aViews[i]->EnableRender(false);
		}
	}
	else
	{
		m_aViews[m_iActiveView]->Move(m_rcSaveArea.left, m_rcSaveArea.top, m_rcSaveArea.right, m_rcSaveArea.bottom);
		AdjustViewportSize(rcClient.right, rcClient.bottom);

		for (int i=0; i < NUM_VIEWPORT; i++)
			m_aViews[i]->EnableRender(true);
	}
}

//	Render 
bool CSectorEditorView::Render()
{
	if (!g_Render.BeginRender(g_aColTable[COL_SPLITBAR]))
		return false;

	CSectorEditorDoc* pDoc = GetDocument();

	//	Render orthogonal viewports -------------
	//	Push sectors and portals to wire collector
	pDoc->Render();

	//	Push bound to wire collector
	CBound* pBound = AUX_GetMainFrame()->GetView()->GetBound();
	if (pBound)
		pBound->Render();

	if (!m_bFullMode)
	{
		//	Update scene's visibility
		CFrameScene* pScene1 = pDoc->GetFrameScene();
		if (pScene1 && g_Configs.bShowScene)
			pScene1->UpdateVisibleSets();
	}

	for (int i=0; i < VIEW_PERSPECTIVE; i++)
		m_aViews[i]->Render();

	m_aViews[0]->Render();

	//	Render perspective viewport -------------
	//	Reset wire collector and re-render selected objects
	if (m_aViews[VIEW_PERSPECTIVE]->GetRenderFlag())
	{
		g_Render.GetWireCollector()->ResetBuffer();
		pDoc->Render(true);

		m_aViews[VIEW_PERSPECTIVE]->Render();
	}
	
	g_Render.EndRender();

	return true;
}

void CSectorEditorView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (!m_bInitialized)
		return;

	g_Render.ResizeDevice(cx, cy);

	if (m_bFullMode)
	{
		//	Only ajust active viewport's size
		m_aViews[m_iActiveView]->Move(0, 0, cx, cy);
	}
	else
	{
		AdjustViewportSize(cx, cy);
	}
}

/*	Adjust viewports size base on their ratio

	iRenderWid, iRenderHei: size of whole render area.
*/
void CSectorEditorView::AdjustViewportSize(int iRenderWid, int iRenderHei)
{
	int w1, h1, w2, h2;

	w1 = m_aViews[0]->GetWidth();
	h1 = m_aViews[0]->GetHeight();
	w2 = m_aViews[1]->GetWidth();
	h2 = m_aViews[2]->GetHeight();

	float fw = (float)w1 / (w1 + w2);
	float fh = (float)h1 / (h1 + h2);

	w1 = (int)((iRenderWid - SPLITBARWIDTH) * fw);
	w2 = iRenderWid - SPLITBARWIDTH - w1;

	if (w1 < MINVIEWSIZE)
	{
		w1 = MINVIEWSIZE;
		w2 = iRenderWid - SPLITBARWIDTH - w1;
	}
	else if (w2 < MINVIEWSIZE)
	{
		w2 = MINVIEWSIZE;
		w1 = iRenderWid - SPLITBARWIDTH - w2;
	}

	h1 = (int)((iRenderHei - SPLITBARWIDTH) * fh);
	h2 = iRenderHei - SPLITBARWIDTH - h1;

	if (h1 < MINVIEWSIZE)
	{
		h1 = MINVIEWSIZE;
		h2 = iRenderHei - SPLITBARWIDTH - h1;
	}
	else if (h2 < MINVIEWSIZE)
	{
		h2 = MINVIEWSIZE;
		h1 = iRenderHei - SPLITBARWIDTH - h2;
	}

	m_aViews[0]->Move(0, 0, w1, h1);
	m_aViews[1]->Move(iRenderWid-w2, 0, w2, h1);
	m_aViews[2]->Move(0, iRenderHei-h2, w1, h2);
	m_aViews[3]->Move(iRenderWid-w2, iRenderHei-h2, w2, h2);
}

//	Set viewports size
void CSectorEditorView::SetViewportSize(int w1, int h1, int w2, int h2)
{
	RECT rcClient;
	GetClientRect(&rcClient);

	m_aViews[0]->Move(0, 0, w1, h1);
	m_aViews[1]->Move(rcClient.right-w2, 0, w2, h1);
	m_aViews[2]->Move(0, rcClient.bottom-h2, w1, h2);
	m_aViews[3]->Move(rcClient.right-w2, rcClient.bottom-h2, w2, h2);

	if (w1 < MINVIEWSIZE || w2 < MINVIEWSIZE ||	h1 < MINVIEWSIZE || h2 < MINVIEWSIZE)
		AdjustViewportSize(rcClient.right, rcClient.bottom);
}

void CSectorEditorView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDblClk(nFlags, point);
}

void CSectorEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iHit = HitTest(point.x, point.y);
	switch (iHit)
	{
	case HIT_VIEW0:
	case HIT_VIEW1:
	case HIT_VIEW2:
	case HIT_VIEW3:
	{
		//	Activate viewpoint
		ActivateViewport(iHit - HIT_VIEW0);

		//	Convert point to viewport coordinates
		POINT pt = {point.x, point.y};
		ClientToViewport(&pt, m_iActiveView);
		m_aViews[m_iActiveView]->OnLButtonDown(pt.x, pt.y, nFlags);
		
		m_LeftDrag.iMode = DRAG_INVIEW;

		break;
	}
	case HIT_HSPLITLINE:
		
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		m_LeftDrag.iMode = DRAG_HSPLIT;
		break;

	case HIT_VSPLITLINE:
		
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		m_LeftDrag.iMode = DRAG_VSPLIT;
		break;

	case HIT_SPLITCROSS:
		
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		m_LeftDrag.iMode = DRAG_SPLITCROSS;
		break;
	}

	m_LeftDrag.iLastX	= point.x;
	m_LeftDrag.iLastY	= point.y;
	SetCapture();

	CView::OnLButtonDown(nFlags, point);
}

void CSectorEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_LeftDrag.iMode == DRAG_NONE)
	{
		//	Convert point to viewport coordinates
		POINT pt = {point.x, point.y};
		ClientToViewport(&pt, m_iActiveView);
		m_aViews[m_iActiveView]->OnLButtonUp(pt.x, pt.y, nFlags);

		CView::OnLButtonUp(nFlags, point);
		return;
	}

	ReleaseCapture();
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	if (m_LeftDrag.iMode == DRAG_HSPLIT ||
		m_LeftDrag.iMode == DRAG_VSPLIT ||
		m_LeftDrag.iMode == DRAG_SPLITCROSS)
	{
		int iHalf = SPLITBARWIDTH / 2;
		int w1, h1, w2, h2;

		RECT rcClient;
		GetClientRect(&rcClient);

		if (m_LeftDrag.iMode == DRAG_HSPLIT)
		{
			w1 = m_aViews[0]->GetWidth();
			h1 = point.y - iHalf;
			w2 = m_aViews[1]->GetWidth();
			h2 = rcClient.bottom - h1 - SPLITBARWIDTH;
		}
		else if (m_LeftDrag.iMode == DRAG_VSPLIT)
		{
			w1 = point.x - iHalf;
			h1 = m_aViews[0]->GetHeight();
			w2 = rcClient.right - w1 - SPLITBARWIDTH;
			h2 = m_aViews[2]->GetHeight();
		}
		else
		{
			w1 = point.x - iHalf;
			h1 = point.y - iHalf;
			w2 = rcClient.right - w1 - SPLITBARWIDTH;
			h2 = rcClient.bottom - h1 - SPLITBARWIDTH;
		}

		SetViewportSize(w1, h1, w2, h2);
	}
	else
	{
		POINT pt = {point.x, point.y};
		ClientToViewport(&pt, m_iActiveView);
		m_aViews[m_iActiveView]->OnLButtonUp(pt.x, pt.y, nFlags);
	}

	m_LeftDrag.iMode = DRAG_NONE;

	CView::OnLButtonUp(nFlags, point);
}

void CSectorEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_LeftDrag.iMode == DRAG_NONE)
	{
		if (!m_bFullMode && !GetDocument()->GetCurOperation())
		{
			int iHit = HitTest(point.x, point.y);
			switch (iHit)
			{
			case HIT_HSPLITLINE:	SetCursor(LoadCursor(NULL, IDC_SIZENS));	break;
			case HIT_VSPLITLINE:	SetCursor(LoadCursor(NULL, IDC_SIZEWE));	break;
			case HIT_SPLITCROSS:	SetCursor(LoadCursor(NULL, IDC_SIZEALL));	break;

			default:

				SetCursor(LoadCursor(NULL, IDC_ARROW));
				break;
			}
		}

		POINT pt = {point.x, point.y};
		ClientToViewport(&pt, m_iActiveView);
		m_aViews[m_iActiveView]->OnMouseMove(pt.x, pt.y, nFlags);

		CView::OnMouseMove(nFlags, point);
		return;
	}

	RECT rcClient;
	GetClientRect(&rcClient);

	if (m_LeftDrag.iMode == DRAG_HSPLIT ||
		m_LeftDrag.iMode == DRAG_VSPLIT ||
		m_LeftDrag.iMode == DRAG_SPLITCROSS)
	{
		POINT pt = point;
		bool bForceSet=false, bCheckH=true, bCheckV=true;

		if (m_LeftDrag.iMode == DRAG_HSPLIT)
			bCheckH = false;
		else if (m_LeftDrag.iMode == DRAG_VSPLIT)
			bCheckV = false;

		if (bCheckV)
		{
			if (point.y < MINVIEWSIZE)
			{
				bForceSet = true;
				pt.y = MINVIEWSIZE;
			}
			else if (point.y > rcClient.bottom - MINVIEWSIZE)
			{
				bForceSet = true;
				pt.y = rcClient.bottom - MINVIEWSIZE;
			}
		}

		if (bCheckH)
		{
			if (point.x < MINVIEWSIZE)
			{
				bForceSet = true;
				pt.x = MINVIEWSIZE;
			}
			else if (point.x > rcClient.right - MINVIEWSIZE)
			{
				bForceSet = true;
				pt.x = rcClient.right - MINVIEWSIZE;
			}
		}

		if (bForceSet)
		{
			point = pt;
			ClientToScreen(&pt);
			SetCursorPos(pt.x, pt.y);
		}
	}
	else
	{
		POINT pt = {point.x, point.y};
		ClientToViewport(&pt, m_iActiveView);
		m_aViews[m_iActiveView]->OnMouseMove(pt.x, pt.y, nFlags);
	}

	m_LeftDrag.iLastX = point.x;
	m_LeftDrag.iLastY = point.y;

	CView::OnMouseMove(nFlags, point);
}

void CSectorEditorView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CView::OnRButtonDblClk(nFlags, point);
}

void CSectorEditorView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int iHit = HitTest(point.x, point.y);
	if (iHit < HIT_VIEW0 || iHit > HIT_VIEW3)
		return;

	bool bViewChange = false;

	if (!GetDocument()->GetCurOperation())
	{
		//	Activate viewpoint
		if (m_iActiveView != iHit - HIT_VIEW0)
		{
			ActivateViewport(iHit - HIT_VIEW0);
			bViewChange = true;
		}
	}

	//	Convert point to viewport coordinates
	POINT pt = {point.x, point.y};
	ClientToViewport(&pt, m_iActiveView);
	m_aViews[m_iActiveView]->OnRButtonDown(pt.x, pt.y, nFlags);
		
	m_RightDrag.iMode	= DRAG_INVIEW;
	m_RightDrag.iLastX	= point.x;
	m_RightDrag.iLastY	= point.y;

	//	Has viewport handled this click ?
	if (m_aViews[m_iActiveView]->OnRButtonDown(pt.x, pt.y, nFlags))
	{
		CView::OnRButtonDown(nFlags, point);
		return;
	}

	//	Popup menu
	if (!bViewChange)
	{
		pt.x = point.x;
		pt.y = point.y;
		ClientToScreen(&pt);

		HMENU hPopMenu, hSubMenu;

		hPopMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINPOPUP));
		hSubMenu = GetSubMenu(hPopMenu, 0);

		::TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 
						 0, GetSafeHwnd(), NULL);

		if (m_iActiveView == VIEW_PERSPECTIVE)
		{
			EnableMenuItem(hSubMenu, ID_POPUP_LEFTBOUND, MF_GRAYED | MF_BYCOMMAND);
			EnableMenuItem(hSubMenu, ID_POPUP_RIGHTBOUND, MF_GRAYED | MF_BYCOMMAND);
			EnableMenuItem(hSubMenu, ID_POPUP_TOPBOUND, MF_GRAYED | MF_BYCOMMAND);
			EnableMenuItem(hSubMenu, ID_POPUP_BOTTOMBOUND, MF_GRAYED | MF_BYCOMMAND);
			EnableMenuItem(hSubMenu, ID_POPUP_CANCELBOUND, MF_GRAYED | MF_BYCOMMAND);
		}
	}
	
	CView::OnRButtonDown(nFlags, point);
}

void CSectorEditorView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnRButtonUp(nFlags, point);
}

//	Hit test
int CSectorEditorView::HitTest(int x, int y)
{
	if (m_bFullMode)
		return HIT_VIEW0 + m_iActiveView;

	int w1, h1, w2, h2;

	w1 = m_aViews[0]->GetWidth();
	h1 = m_aViews[0]->GetHeight();
	w2 = m_aViews[1]->GetWidth();
	h2 = m_aViews[2]->GetHeight();

	if (x < w1)
	{
		if (y < h1)
			return HIT_VIEW0;
		else if (y > h1 + SPLITBARWIDTH)
			return HIT_VIEW2;
		else
			return HIT_HSPLITLINE;
	}
	else if (x > w1 + SPLITBARWIDTH)
	{
		if (y < h1)
			return HIT_VIEW1;
		else if (y > h1 + SPLITBARWIDTH)
			return HIT_VIEW3;
		else
			return HIT_HSPLITLINE;
	}
	else
	{
		if (y < h1 || y > h1 + SPLITBARWIDTH)
			return HIT_VSPLITLINE;
		else
			return HIT_SPLITCROSS;
	}
}

//	Frame move
bool CSectorEditorView::FrameMove(DWORD dwTime)
{
	if (m_iActiveView < 0)
		return true;

	m_aViews[m_iActiveView]->FrameMove(dwTime);

	return true;
}

//	Activate viewport
void CSectorEditorView::ActivateViewport(int iView)
{
	if (m_iActiveView == iView)
		return;

	//	Deactivate previously active viewport
	m_aViews[m_iActiveView]->Activate(false);

	m_iActiveView = iView;
	m_aViews[iView]->Activate(true);
}

//	Maximum viewport
void CSectorEditorView::OnMaxViewport(UINT nID)
{
	//	One operation is being done, forbid to maximum viewport
	COperation* pOpt = GetDocument()->GetCurOperation();
	if (pOpt && pOpt->FixViewport())
		return;

	if (nID == ID_VIEW_RESTORE4)
	{
		if (m_bFullMode)
			SwitchFullMode();

		return;
	}

	if (m_bFullMode)
		SwitchFullMode();

	ActivateViewport(nID - ID_VIEW_MAXVIEW0);

	if (!m_bFullMode)
		SwitchFullMode();
}

//	Convert position from client to viewport
void CSectorEditorView::ClientToViewport(POINT* pPt, int iViewport)
{
	ASSERT(iViewport >= 0 && iViewport < NUM_VIEWPORT);

	pPt->x -= m_aViews[iViewport]->GetX();
	pPt->y -= m_aViews[iViewport]->GetY();
}

void CSectorEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_ESCAPE:
	{
		COperation* pOpt = GetDocument()->GetCurOperation();
		if (pOpt)	//	Cancel current operation
			AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 1, 0);
		else	//	Cancel current selection
			GetDocument()->SelectEditObject(NULL);

		break;
	}
	}
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSectorEditorView::OnShowHideFlag(UINT nID)
{
	switch (nID)
	{
	case ID_VIEW_SHOWGRID:		g_Configs.bShowGrid = !g_Configs.bShowGrid;		break;
	case ID_VIEW_SHOWSCENE:		g_Configs.bShowScene = !g_Configs.bShowScene;	break;
	case ID_VIEW_SHOWSECTOR:	g_Configs.bShowSector = !g_Configs.bShowSector;	break;
	case ID_VIEW_SHOWPORTAL:	g_Configs.bShowPortal = !g_Configs.bShowPortal;	break;
	}
}

BOOL CSectorEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	AUX_SetCursor(g_iCurCursor);
	return TRUE;
//	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CSectorEditorView::GetOrthoAxis(int iViewport, int* piAxisH, int* piAxisV, int* piAxisZ)
{
	switch (iViewport)
	{
	case VIEW_XY:	*piAxisH=0; *piAxisV=1; *piAxisZ=2;	break;
	case VIEW_YZ:	*piAxisH=2; *piAxisV=1; *piAxisZ=0;	break;
	case VIEW_XZ:	*piAxisH=0; *piAxisV=2; *piAxisZ=1;	break;
	}
}

void CSectorEditorView::OnMoveBound(UINT nID)
{
	if (m_iActiveView == VIEW_PERSPECTIVE)
		return;

	COrthogonalWnd* pViewWnd = (COrthogonalWnd*)m_aViews[m_iActiveView];

	//	Convert point to viewport coordinates
	POINT pt = {m_RightDrag.iLastX, m_RightDrag.iLastY};
	ClientToViewport(&pt, m_iActiveView);

	//	Transform ot world coordinates
	A3DVECTOR3 vPos((float)pt.x, (float)pt.y, 0.5f);
	pViewWnd->InvTransform(vPos, vPos);

	int iAxisH, iAxisV, iAxisZ;
	GetOrthoAxis(m_iActiveView, &iAxisH, &iAxisV, &iAxisZ);
	
	switch (nID)
	{
	case ID_POPUP_LEFTBOUND:

		m_pBound->SetBound(iAxisH, true, vPos.m[iAxisH]);
		break;

	case ID_POPUP_RIGHTBOUND:

		m_pBound->SetBound(iAxisH, false, vPos.m[iAxisH]);
		break;

	case ID_POPUP_TOPBOUND:

		m_pBound->SetBound(iAxisV, false, vPos.m[iAxisV]);
		break;

	case ID_POPUP_BOTTOMBOUND:

		m_pBound->SetBound(iAxisV, true, vPos.m[iAxisV]);
		break;
	}

	GetDocument()->SetBound(m_pBound->GetMins(), m_pBound->GetMaxs(), g_Configs.bFullClip);
}

void CSectorEditorView::OnPopupCancelbound() 
{
	m_pBound->SetMinsAndMaxs(A3DVECTOR3(-WORLD_BOUND), A3DVECTOR3(WORLD_BOUND));
	GetDocument()->SetBound(m_pBound->GetMins(), m_pBound->GetMaxs(), g_Configs.bFullClip);
}

void CSectorEditorView::OnViewFullclip() 
{
	g_Configs.bFullClip = !g_Configs.bFullClip;
	GetDocument()->SetBound(m_pBound->GetMins(), m_pBound->GetMaxs(), g_Configs.bFullClip);
}

//	Get eye's position and direction
void CSectorEditorView::GetEyePosAndDir(A3DVECTOR3* pvPos, A3DVECTOR3* pvDir)
{
	CPerspectiveWnd* pView = (CPerspectiveWnd*)m_aViews[VIEW_PERSPECTIVE];

	if (pvPos)
		*pvPos = pView->GetCamera()->GetPos();

	if (pvDir)
		*pvDir = pView->GetCamera()->GetDir();
}


