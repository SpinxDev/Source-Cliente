// $$root$$View.cpp : implementation of the C$$root$$View class
//

#include "stdafx.h"
#include "$$root$$.h"

#include "$$root$$Doc.h"
#include "$$root$$View.h"
#include "Render.h"
#include "ALog.h"

/////////////////////////////////////////////////////////////////////////////
// C$$root$$View

IMPLEMENT_DYNCREATE(C$$root$$View, CView)

BEGIN_MESSAGE_MAP(C$$root$$View, CView)
	//{{AFX_MSG_MAP(C$$root$$View)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C$$root$$View construction/destruction

C$$root$$View::C$$root$$View()
{
	// TODO: add construction code here

}

C$$root$$View::~C$$root$$View()
{
}

BOOL C$$root$$View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// C$$root$$View drawing

void C$$root$$View::OnDraw(CDC* pDC)
{
	C$$root$$Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// C$$root$$View diagnostics

#ifdef _DEBUG
void C$$root$$View::AssertValid() const
{
	CView::AssertValid();
}

void C$$root$$View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

C$$root$$Doc* C$$root$$View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C$$root$$Doc)));
	return (C$$root$$Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// C$$root$$View message handlers

void C$$root$$View::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if (!cx || !cy)
		return;
	
	if (nType == SIZE_MINIMIZED || nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;

	g_Render.ResizeDevice(cx, cy);
}

void C$$root$$View::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	if (!g_Render.EngineIsReady())
	{
		//	Create render
		if (!g_Render.Init(AfxGetInstanceHandle(), m_hWnd, false))
		{
			a_LogOutput(1, "CSkeletonEditorView::OnInitialUpdate, Failed to create render");
			return;
		}
	}
}

//	Logic tick
bool C$$root$$View::Tick(DWORD dwTime)
{
	//	TODO: do logic tick here.

	return true;
}

//	Render scene
bool C$$root$$View::Render()
{
	if (!g_Render.BeginRender())
		return false;

	//	Clear viewport
	g_Render.ClearViewport();

	//	TODO: render scene and objects here

	g_Render.EndRender();

	return true;
}

