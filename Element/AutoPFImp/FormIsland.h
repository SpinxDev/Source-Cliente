#if !defined(AFX_FORMISLAND_H__0525FAF3_1B87_4717_91A7_BED248F5AC60__INCLUDED_)
#define AFX_FORMISLAND_H__0525FAF3_1B87_4717_91A7_BED248F5AC60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormIsland.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormIsland form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include <A3DTypes.h>

class CFormIsland : public CFormView
{
protected:
	CFormIsland();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFormIsland)

// Form Data
public:
	//{{AFX_DATA(CFormIsland)
	enum { IDD = IDD_FORMVIEW_ISLAND };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	bool AddBorderLine(const A3DPOINT2& pt1, const A3DPOINT2& pt2, bool &bStillInvalid);
	void SelectIsland(const A3DPOINT2& pt);
	void DelBorderPt(const A3DPOINT2& pt);
	void SetPassThrough(const A3DPOINT2& pt);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormIsland)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFormIsland();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CFormIsland)
	afx_msg void OnIslandNew();
	afx_msg void OnIslandSelect();
	afx_msg void OnIslandAddborder();
	afx_msg void OnIslandDelborder();
	afx_msg void OnIslandDel();
	afx_msg void OnIslandCheckpass();
	afx_msg void OnIslandCreatepass();
	afx_msg void OnIslandPassthrough();
	afx_msg void OnKillfocusEditPassthrough();
	afx_msg void OnSelchangeComboPasses();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void _RestoreBtn();
	bool _LeaveEditMode();
	void _ResetPassList();	
	void _SetEditThrough(const A3DPOINT2& pt);
	bool _CheckAllPasses();
	A3DPOINT2 _GetEditThrough();
private:
	bool m_bEditMode;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMISLAND_H__0525FAF3_1B87_4717_91A7_BED248F5AC60__INCLUDED_)
