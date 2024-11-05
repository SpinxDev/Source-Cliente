#if !defined(AFX_NEWSHADERDLG_H__09AA7E2A_4E7C_4B57_A9B7_0DB5412CA0FB__INCLUDED_)
#define AFX_NEWSHADERDLG_H__09AA7E2A_4E7C_4B57_A9B7_0DB5412CA0FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewShaderDlg.h : header file
//

///////////////////////////////////////////////////////////////////////////
//
//	CNewShaderDlg dialog
//
///////////////////////////////////////////////////////////////////////////

class CNewShaderDlg : public CDialog
{
// Construction
public:
	CNewShaderDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewShaderDlg)
	enum { IDD = IDD_NEWSDRDLG };
	CString	m_strFile;
	CString	m_strName;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Attributes
protected:

// Operations
protected:

// Implementation
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewShaderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewShaderDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSHADERDLG_H__09AA7E2A_4E7C_4B57_A9B7_0DB5412CA0FB__INCLUDED_)
