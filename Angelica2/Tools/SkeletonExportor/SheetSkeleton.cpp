// SheetSkeleton.cpp : implementation file
//

#include "stdafx.h"
#include "SkeletonExportor.h"
#include "SheetSkeleton.h"

/////////////////////////////////////////////////////////////////////////////
// CSheetSkeleton

IMPLEMENT_DYNAMIC(CSheetSkeleton, CPropertySheet)

CSheetSkeleton::CSheetSkeleton(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	FillPages();
}

CSheetSkeleton::CSheetSkeleton(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	FillPages();
}

CSheetSkeleton::~CSheetSkeleton()
{
}


BEGIN_MESSAGE_MAP(CSheetSkeleton, CPropertySheet)
	//{{AFX_MSG_MAP(CSheetSkeleton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSheetSkeleton message handlers

void CSheetSkeleton::FillPages()
{
	m_SkeletonTreePage.SetStep(1, 2);
	AddPage(&m_SkeletonTreePage);

	m_AnimationPage.SetStep(2, 2);
	AddPage(&m_AnimationPage);
}

void CSheetSkeleton::SetSkeletonCandidate(CSkeletonCandidate* p)
{
	m_SkeletonTreePage.SetSkeletonCandidate(p);
	m_AnimationPage.SetSkeletonCandidate(p);
}

