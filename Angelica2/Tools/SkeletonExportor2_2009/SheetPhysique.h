#if !defined(AFX_SHEETPHYSIQUE_H__08C1494A_D1D9_469B_81EE_E18F51E3E85B__INCLUDED_)
#define AFX_SHEETPHYSIQUE_H__08C1494A_D1D9_469B_81EE_E18F51E3E85B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SheetPhysique.h : header file
//

#include "PageSkeletonFile.h"

class CSkeletonCandidate;
class CPhysiqueCandidate;

/////////////////////////////////////////////////////////////////////////////
// CSheetPhysique

class CSheetPhysique : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetPhysique)

// Construction
public:
	CSheetPhysique(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSheetPhysique(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

	void SetSkeletonCandidate(CSkeletonCandidate* p);
	void SetPhysiqueCandidate(CPhysiqueCandidate* p);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetPhysique)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSheetPhysique();

protected:	//	Attributes

	CPageSkeletonFile	m_SkeletonFilePage;

protected:	//	Operations

	void FillPages();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSheetPhysique)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEETPHYSIQUE_H__08C1494A_D1D9_469B_81EE_E18F51E3E85B__INCLUDED_)
