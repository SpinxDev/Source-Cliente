#if !defined(AFX_SHEETSKELETON_H__EC77C52C_940E_47AA_BB7D_BCC1B76B7F42__INCLUDED_)
#define AFX_SHEETSKELETON_H__EC77C52C_940E_47AA_BB7D_BCC1B76B7F42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SheetSkeleton.h : header file
//

#include "PageSkeletonTree.h"
#include "PageAnimation.h"

class CSkeletonCandidate;

/////////////////////////////////////////////////////////////////////////////
// CSheetSkeleton

class CSheetSkeleton : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetSkeleton)

// Construction
public:
	CSheetSkeleton(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSheetSkeleton(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

	void SetSkeletonCandidate(CSkeletonCandidate* p);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetSkeleton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSheetSkeleton();

protected:	//	Attributes

	CPageSkeletonTree	m_SkeletonTreePage;
	CPageAnimation		m_AnimationPage;

protected:	//	Operations

	void FillPages();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSheetSkeleton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEETSKELETON_H__EC77C52C_940E_47AA_BB7D_BCC1B76B7F42__INCLUDED_)
