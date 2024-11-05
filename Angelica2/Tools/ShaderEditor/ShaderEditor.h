#if !defined(AFX_SHADEREDITOR_H__AD371078_0687_4D5B_820E_283AFA6BAA63__INCLUDED_)
#define AFX_SHADEREDITOR_H__AD371078_0687_4D5B_820E_283AFA6BAA63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShaderEditor.h : header file
//

///////////////////////////////////////////////////////////////////////////
// 
//	CShaderEditor thread
//
///////////////////////////////////////////////////////////////////////////

class CShaderEditor : public CWinApp
{
	DECLARE_DYNCREATE(CShaderEditor)
public:
	CShaderEditor();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShaderEditor)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShaderEditor();

	// Generated message map functions
	//{{AFX_MSG(CShaderEditor)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:	//	Attributes

	CString		m_strClassName;			//	Mainform window's class

protected:	//	Operations

	void		GetInitialInfo();		//	Get initial information from .ini file
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHADEREDITOR_H__AD371078_0687_4D5B_820E_283AFA6BAA63__INCLUDED_)
