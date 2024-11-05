// SectorEditorView.h : interface of the CSectorEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTOREDITORVIEW_H__37DC817D_1D91_4295_9DF0_B8407D987E43__INCLUDED_)
#define AFX_SECTOREDITORVIEW_H__37DC817D_1D91_4295_9DF0_B8407D987E43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Render.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CViewportWnd;
class CSectorEditorDoc;
class CBound;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSectorEditorView
//
///////////////////////////////////////////////////////////////////////////

class CSectorEditorView : public CView
{
protected:	//	Create from serialization only
	CSectorEditorView();
	DECLARE_DYNCREATE(CSectorEditorView)

public:		//	Types

	//	Viewports identification
	enum
	{
		VIEW_XZ = 0,
		VIEW_XY,
		VIEW_YZ,
		VIEW_PERSPECTIVE,
		NUM_VIEWPORT,
	};

	//	Hit result
	enum
	{
		HIT_VIEW0 = 0,		//	Viewport 0 -- 3	
		HIT_VIEW1,
		HIT_VIEW2,
		HIT_VIEW3,
		HIT_HSPLITLINE,		//	Horizontal split line
		HIT_VSPLITLINE,		//	Vertical split line
		HIT_SPLITCROSS,		//	Cross point of split lines
		HIT_NONE,			//	None
	};

	//	Mouse drag mode
	enum
	{
		DRAG_NONE = 0,		//	Don't drag
		DRAG_INVIEW,		//	In viewport
		DRAG_HSPLIT,		//	Drag horizontal split line
		DRAG_VSPLIT,		//	Drag vertical split line
		DRAG_SPLITCROSS,	//	Drag cross point of split lines
	};

	enum
	{
		SPLITBARWIDTH = 5,	//	Width of split bar
		MINVIEWSIZE = 50,	//	Minimal size of viewport's width and height
	};

	//	Mouse drag infomation
	struct MOUSEDRAG
	{
		int		iMode;		//	Drag mode
		int		iLastX;		//	Last x
		int		iLastY;		//	Last y
	};

public:		//	Attributes

public:		//	Operations

	CSectorEditorDoc* GetDocument();

	//	Frame move
	bool FrameMove(DWORD dwTime);
	//	Render 
	bool Render();

	//	Get active viewport
	int GetActiveViewport() { return m_iActiveView; }
	//	Get viewport
	CViewportWnd* GetViewport(int iIndex)
	{
		ASSERT(iIndex >= 0 && iIndex < NUM_VIEWPORT);
		return m_aViews[iIndex];
	}

	//	Get orthogonal viewport axis
	void GetOrthoAxis(int iViewport, int* piAxisH, int* piAxisV, int* piAxisZ);
	//	Get bound box
	CBound* GetBound() { return m_pBound; }
	//	Get eye's position and direction
	void GetEyePosAndDir(A3DVECTOR3* pvPos, A3DVECTOR3* pvDir);

	//	On new document
	bool OnNewDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSectorEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSectorEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:	//	Attributes

	CViewportWnd*	m_aViews[NUM_VIEWPORT];
	CBound*			m_pBound;	//	World bound

	int		m_iActiveView;		//	Currently active view
	bool	m_bFullMode;		//	Full mode flag
	RECT	m_rcSaveArea;		//	Used to save viewport area when switch fullmode
	bool	m_bInitialized;		//	Initialized flag

	MOUSEDRAG	m_LeftDrag;		//	Mouse left button drag information
	MOUSEDRAG	m_RightDrag;	//	Mouse right button drag information

protected:	//	Operatinos

	//	Layout viewports
	void LayoutViewports(bool bFullMode);
	//	Switch full mode
	void SwitchFullMode();
	//	Adjust viewports size base on their ratio
	void AdjustViewportSize(int iRenderWid, int iRenderHei);
	//	Set viewports size
	void SetViewportSize(int w1, int h1, int w2, int h2);
	//	Hit test
	int HitTest(int x, int y);
	//	Activate viewport
	void ActivateViewport(int iView);
	//	Convert position from client to viewport
	void ClientToViewport(POINT* pPt, int iViewport);

// Generated message map functions
protected:
	//{{AFX_MSG(CSectorEditorView)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPopupCancelbound();
	afx_msg void OnViewFullclip();
	//}}AFX_MSG

	afx_msg void OnMaxViewport(UINT nID);
	afx_msg void OnShowHideFlag(UINT nID);
	afx_msg void OnMoveBound(UINT nID);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SectorEditorView.cpp
inline CSectorEditorDoc* CSectorEditorView::GetDocument()
   { return (CSectorEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECTOREDITORVIEW_H__37DC817D_1D91_4295_9DF0_B8407D987E43__INCLUDED_)
