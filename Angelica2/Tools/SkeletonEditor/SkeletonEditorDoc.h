// SkeletonEditorDoc.h : interface of the CSkeletonEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKELETONEDITORDOC_H__1DAE305E_EB75_4E45_9186_BC3595D801D2__INCLUDED_)
#define AFX_SKELETONEDITORDOC_H__1DAE305E_EB75_4E45_9186_BC3595D801D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CModel;
class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSkeletonEditorView
//
///////////////////////////////////////////////////////////////////////////

class CSkeletonEditorDoc : public CDocument
{
protected: // create from serialization only
	CSkeletonEditorDoc();
	DECLARE_DYNCREATE(CSkeletonEditorDoc)

public:	//	Attributes

public:	//	Operations

	//	Frame move
	bool FrameMove(DWORD dwTime);
	//	Render 
	bool Render(A3DViewport* pViewport);

	//	Create model object
	bool CreateModel();
	//	Get model object
	CModel* GetModel() { return m_pModel; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkeletonEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkeletonEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:	//	Attributes

	CModel*		m_pModel;

protected:	//	Operatinos

	//	Release model object
	void ReleaseModel();

// Generated message map functions
protected:
	//{{AFX_MSG(CSkeletonEditorDoc)
	afx_msg void OnEditOption();
	afx_msg void OnEditChangeskin();
	afx_msg void OnViewModelinfo();
	afx_msg void OnEditModelprop();
	afx_msg void OnEditAddphysique();
	afx_msg void OnEditRemovephysique();
	afx_msg void OnFileClosedoc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKELETONEDITORDOC_H__1DAE305E_EB75_4E45_9186_BC3595D801D2__INCLUDED_)
