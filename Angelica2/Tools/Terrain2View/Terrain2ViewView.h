// Terrain2ViewView.h : interface of the CTerrain2ViewView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRAIN2VIEWVIEW_H__D12D4178_FD7B_49EE_A85E_DB07AF70EB72__INCLUDED_)
#define AFX_TERRAIN2VIEWVIEW_H__D12D4178_FD7B_49EE_A85E_DB07AF70EB72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTerrain2ViewDoc;
class A3DViewport;

class CTerrain2ViewView : public CView
{
protected: // create from serialization only
	CTerrain2ViewView();
	DECLARE_DYNCREATE(CTerrain2ViewView)

// Attributes
public:
	CTerrain2ViewDoc* GetDocument();

// Operations
public:

	//	Logic tick
	bool Tick(DWORD dwTime);
	//	Render scene
	bool Render();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrain2ViewView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTerrain2ViewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	bool	m_bRightPressed;	//	Right button pressed flag
	POINT	m_ptMouseLast;		//	Last mouse position
	POINT	m_ptPickedBlk;		//	Picked block
	POINT	m_ptCameraBlk;		//	Camera block

protected:

	//	Move camera
	void MoveCamera(DWORD dwTime);
	//	Get block info
	void GetBlockInfo(A3DViewport* pViewport);
	//	Draw prompt info
	void DrawPromptInfo(A3DViewport* pViewport);

// Generated message map functions
protected:
	//{{AFX_MSG(CTerrain2ViewView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Terrain2ViewView.cpp
inline CTerrain2ViewDoc* CTerrain2ViewView::GetDocument()
   { return (CTerrain2ViewDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAIN2VIEWVIEW_H__D12D4178_FD7B_49EE_A85E_DB07AF70EB72__INCLUDED_)
