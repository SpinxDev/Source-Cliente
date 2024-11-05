// SheetSkinMesh.cpp : implementation file
//

#include "stdafx.h"
#include "SkeletonExportor.h"
#include "SheetSkinMesh.h"


/////////////////////////////////////////////////////////////////////////////
// CSheetSkinMesh

IMPLEMENT_DYNAMIC(CSheetSkinMesh, CPropertySheet)

CSheetSkinMesh::CSheetSkinMesh(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	FillPages();
}

CSheetSkinMesh::CSheetSkinMesh(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	FillPages();
}

CSheetSkinMesh::~CSheetSkinMesh()
{
}


BEGIN_MESSAGE_MAP(CSheetSkinMesh, CPropertySheet)
	//{{AFX_MSG_MAP(CSheetSkinMesh)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSheetSkinMesh message handlers

void CSheetSkinMesh::SetSkeletonCandidate(CSkeletonCandidate* p)
{
	m_SkeletonFilePage.SetSkeletonCandidate(p);
	m_MeshPage.SetSkeletonCandidate(p);
}

void CSheetSkinMesh::SetSkinCandidate(CSkinCandidate* p)
{
	m_MeshPage.SetSkinCandidate(p);
	m_MaterialTreePage.SetSkinCandidate(p);
}

void CSheetSkinMesh::FillPages()
{
	m_SkeletonFilePage.SetStep(1, 3);
	AddPage(&m_SkeletonFilePage);

	m_MaterialTreePage.SetStep(2, 3);
	AddPage(&m_MaterialTreePage);

	m_MeshPage.SetStep(3, 3);
	AddPage(&m_MeshPage);
}

