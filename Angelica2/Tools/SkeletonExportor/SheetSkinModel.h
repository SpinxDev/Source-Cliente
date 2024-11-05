#if !defined(AFX_SHEETSKINMODEL_H__A93F9C27_1625_4AE1_9F74_0C4EB63A147F__INCLUDED_)
#define AFX_SHEETSKINMODEL_H__A93F9C27_1625_4AE1_9F74_0C4EB63A147F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SheetSkinModel.h : header file
//

#include "PageSkeletonTree.h"
#include "PageAnimation.h"
#include "PageMesh.h"

class CSkeletonCandidate;
class CSkinCandidate;
class CPhysiqueCandidate;
class CPageMesh;

/////////////////////////////////////////////////////////////////////////////
// CSheetSkinModel

class CSheetSkinModel : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetSkinModel)

// Construction
public:
	CSheetSkinModel(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSheetSkinModel(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

	void SetSkeletonCandidate(CSkeletonCandidate* p);
	void SetSkinCandidate(CSkinCandidate* p);
	void SetPhysiqueCandidate(CPhysiqueCandidate* p);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetSkinModel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSheetSkinModel();

protected:	//	Attributes

	CPageSkeletonTree	m_SkeletonTreePage;
	CPageAnimation		m_AnimationPage;
	CPageMesh			m_MeshPage;

protected:	//	Operations

	void FillPages();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSheetSkinModel)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEETSKINMODEL_H__A93F9C27_1625_4AE1_9F74_0C4EB63A147F__INCLUDED_)
