// $$root$$View.h : interface of the C$$root$$View class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class C$$root$$View : public CView
{
protected: // create from serialization only
	C$$root$$View();
	DECLARE_DYNCREATE(C$$root$$View)

// Attributes
public:
	C$$root$$Doc* GetDocument();

// Operations
public:

	//	Logic tick
	bool Tick(DWORD dwTime);
	//	Render scene
	bool Render();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C$$root$$View)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~C$$root$$View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(C$$root$$View)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in $$root$$View.cpp
inline C$$root$$Doc* C$$root$$View::GetDocument()
   { return (C$$root$$Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
