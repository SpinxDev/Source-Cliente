// PhysDemoDoc.h : interface of the CPhysDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSDEMODOC_H__74D83712_F9E0_4DD5_B647_667FC37A732E__INCLUDED_)
#define AFX_PHYSDEMODOC_H__74D83712_F9E0_4DD5_B647_667FC37A732E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPhysDemoDoc : public CDocument
{
protected: // create from serialization only
	CPhysDemoDoc();
	DECLARE_DYNCREATE(CPhysDemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhysDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPhysDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPhysDemoDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHYSDEMODOC_H__74D83712_F9E0_4DD5_B647_667FC37A732E__INCLUDED_)
