#if !defined(AFX_SHEETSKINMESH_H__A4F90DF8_BBF8_4212_9494_106D21958830__INCLUDED_)
#define AFX_SHEETSKINMESH_H__A4F90DF8_BBF8_4212_9494_106D21958830__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SheetSkinMesh.h : header file
//

#include "PageSkeletonFile.h"
#include "PageMesh.h"
#include "PageMaterialTree.h"

class CSkeletonCandidate;
class CSkinCandidate;

/////////////////////////////////////////////////////////////////////////////
// CSheetSkinMesh

class CSheetSkinMesh : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetSkinMesh)

// Construction
public:
	CSheetSkinMesh(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSheetSkinMesh(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

	void SetSkeletonCandidate(CSkeletonCandidate* p);
	void SetSkinCandidate(CSkinCandidate* p);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetSkinMesh)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSheetSkinMesh();

protected:	//	Attributes

	CPageSkeletonFile	m_SkeletonFilePage;
	CPageMesh			m_MeshPage;
	CPageMaterialTree	m_MaterialTreePage;

protected:	//	Operations

	void FillPages();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSheetSkinMesh)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEETSKINMESH_H__A4F90DF8_BBF8_4212_9494_106D21958830__INCLUDED_)
