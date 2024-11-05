// SkeletonEditorView.h : interface of the CSkeletonEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKELETONEDITORVIEW_H__DBB0E40E_DD1F_41DF_A1AD_DF6A41775BD8__INCLUDED_)
#define AFX_SKELETONEDITORVIEW_H__DBB0E40E_DD1F_41DF_A1AD_DF6A41775BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

class A3DViewport;
class A3DCamera;
class CSkeletonEditorDoc;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSkeletonEditorView
//
///////////////////////////////////////////////////////////////////////////

class CSkeletonEditorView : public CView
{
protected: // create from serialization only
	CSkeletonEditorView();
	DECLARE_DYNCREATE(CSkeletonEditorView)

public:	//	Attributes

public:	//	Operations

	CSkeletonEditorDoc* GetDocument();

	//	Frame move
	bool FrameMove(DWORD dwTime);
	//	Render 
	bool Render();

	//	Set camera position basing on model's position and size
	void SetCameraPos(const A3DVECTOR3& vModelCenter, float fModelRadius);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkeletonEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkeletonEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:	//	Attributes

	A3DCamera*		m_pCamera;
	A3DViewport*	m_pViewport;

	bool	m_bInitialized;		//	Initialized flag

	bool	m_bLeftPressed;		//	Left button pressed flag
	bool	m_bRightPressed;	//	Right button pressed flag
	POINT	m_ptMouseLast;		//	Last mouse position

	bool	m_bDoSelect;		//	Be selecting flag
	bool	m_bAddSelect;		//	Add selection flag
	RECT	m_rcSelected;		//	Selected rectangle
	POINT	m_ptSelStart;		//	Select starting point

protected:	//	Operatinos

	//	Move camera
	void MoveCamera(DWORD dwTime);
	//	Print prompt
	void DrawPrompt();
	//	Draw ground grid
	void DrawGroundGrid();

// Generated message map functions
protected:
	//{{AFX_MSG(CSkeletonEditorView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SkeletonEditorView.cpp
inline CSkeletonEditorDoc* CSkeletonEditorView::GetDocument()
   { return (CSkeletonEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKELETONEDITORVIEW_H__DBB0E40E_DD1F_41DF_A1AD_DF6A41775BD8__INCLUDED_)
