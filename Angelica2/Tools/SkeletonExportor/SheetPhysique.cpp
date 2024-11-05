// SheetPhysique.cpp : implementation file
//

#include "stdafx.h"
#include "SkeletonExportor.h"
#include "SheetPhysique.h"

/////////////////////////////////////////////////////////////////////////////
// CSheetPhysique

IMPLEMENT_DYNAMIC(CSheetPhysique, CPropertySheet)

CSheetPhysique::CSheetPhysique(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CSheetPhysique::CSheetPhysique(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	FillPages();
}

CSheetPhysique::~CSheetPhysique()
{
	FillPages();
}

BEGIN_MESSAGE_MAP(CSheetPhysique, CPropertySheet)
	//{{AFX_MSG_MAP(CSheetPhysique)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSheetPhysique message handlers

void CSheetPhysique::SetSkeletonCandidate(CSkeletonCandidate* p)
{
	m_SkeletonFilePage.SetSkeletonCandidate(p);
}

void CSheetPhysique::SetPhysiqueCandidate(CPhysiqueCandidate* p)
{
}

void CSheetPhysique::FillPages()
{
	m_SkeletonFilePage.SetStep(1, 1);
	AddPage(&m_SkeletonFilePage);
}

