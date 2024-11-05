// PhysDemoView.h : interface of the CPhysDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSDEMOVIEW_H__720D684B_622A_4F85_8A00_343B8F218CCC__INCLUDED_)
#define AFX_PHYSDEMOVIEW_H__720D684B_622A_4F85_8A00_343B8F218CCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class A3DVECTOR3;

class CPhysDemoView : public CView
{
protected: // create from serialization only
	CPhysDemoView();
	DECLARE_DYNCREATE(CPhysDemoView)

// Attributes
public:
	CPhysDemoDoc* GetDocument();

// Operations
public:

	//	Logic tick
	bool Tick(DWORD dwTime);
	//	Render scene
	bool Render();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhysDemoView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPhysDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	bool		m_bRightPressed;	//	Right button pressed flag
	POINT		m_ptMouseLock;		//	Mouse lock position
	POINT		m_ptMouseLast;		//	Mouse last position
	
	DWORD		m_dwRenderStart;	//	Render starting time
	DWORD		m_dwLastRender;		//	Render time of last frame

protected:

	//	Move camera
	void MoveCamera(DWORD dwTime);
	//	Render prompts
	void DrawPrompts();
	//	Get the terrain position pointed by cursor
	bool GetCurTerrainPos(int x, int y, A3DVECTOR3& vOutPos);

	//	Drop a dummy sphere with specified height
	void DropDummySphere(float fHeight);

// Generated message map functions
protected:
	//{{AFX_MSG(CPhysDemoView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEditAddstaticobj();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditPhysxtrngen();
	afx_msg void OnEditAddterrain();
	afx_msg void OnEditAdd2stick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PhysDemoView.cpp
inline CPhysDemoDoc* CPhysDemoView::GetDocument()
   { return (CPhysDemoDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHYSDEMOVIEW_H__720D684B_622A_4F85_8A00_343B8F218CCC__INCLUDED_)
