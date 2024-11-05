// SheetSkinModel.cpp : implementation file
//

#include "stdafx.h"
#include "SkeletonExportor.h"
#include "SheetSkinModel.h"

/////////////////////////////////////////////////////////////////////////////
// CSheetSkinModel

IMPLEMENT_DYNAMIC(CSheetSkinModel, CPropertySheet)

CSheetSkinModel::CSheetSkinModel(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	FillPages();
}

CSheetSkinModel::CSheetSkinModel(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	FillPages();
}

CSheetSkinModel::~CSheetSkinModel()
{
}


BEGIN_MESSAGE_MAP(CSheetSkinModel, CPropertySheet)
	//{{AFX_MSG_MAP(CSheetSkinModel)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSheetSkinModel message handlers

void CSheetSkinModel::SetSkeletonCandidate(CSkeletonCandidate* p)
{
	m_SkeletonTreePage.SetSkeletonCandidate(p);
	m_AnimationPage.SetSkeletonCandidate(p);
	m_MeshPage.SetSkeletonCandidate(p);
}

void CSheetSkinModel::SetSkinCandidate(CSkinCandidate* p)
{
	m_MeshPage.SetSkinCandidate(p);
}

void CSheetSkinModel::SetPhysiqueCandidate(CPhysiqueCandidate* p)
{
}

void CSheetSkinModel::FillPages()
{
	m_SkeletonTreePage.SetStep(1, 3);
	AddPage(&m_SkeletonTreePage);

	m_AnimationPage.SetStep(2, 3);
	AddPage(&m_AnimationPage);
	
	m_MeshPage.SetStep(3, 3);
	AddPage(&m_MeshPage);
}


