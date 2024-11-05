// FTDriverTestDoc.h : interface of the CFTDriverTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTDRIVERTESTDOC_H__0894D71B_5B7D_4950_BD33_7F5C56BB4254__INCLUDED_)
#define AFX_FTDRIVERTESTDOC_H__0894D71B_5B7D_4950_BD33_7F5C56BB4254__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFTDriverTestDoc : public CDocument
{
protected: // create from serialization only
	CFTDriverTestDoc();
	DECLARE_DYNCREATE(CFTDriverTestDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFTDriverTestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFTDriverTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFTDriverTestDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTDRIVERTESTDOC_H__0894D71B_5B7D_4950_BD33_7F5C56BB4254__INCLUDED_)
