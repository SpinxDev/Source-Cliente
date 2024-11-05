#if !defined(AFX_DLGACTIONPROP_H__FF57B31E_15BB_416B_A92D_7ACEEF7A6F2B__INCLUDED_)
#define AFX_DLGACTIONPROP_H__FF57B31E_15BB_416B_A92D_7ACEEF7A6F2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgActionProp.h : header file
//

class CModel;
class A3DSkinModelActionCore;

/////////////////////////////////////////////////////////////////////////////
// CDlgActionProp dialog

class CDlgActionProp : public CDialog
{
// Construction
public:
	CDlgActionProp(CModel* pModel, A3DSkinModelActionCore* pAction, bool bNewAction, CWnd* pParent = NULL);   // standard constructor

	//	Get track file name
	const char* GetTrackFile() { return m_strTrackFile; }

// Dialog Data
	//{{AFX_DATA(CDlgActionProp)
	enum { IDD = IDD_ACTIONPROP };
	CString	m_strName;
	CString	m_strTrackFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgActionProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CModel*	m_pModel;
	A3DSkinModelActionCore*	m_pAction;
	bool m_bNewAction;

	// Generated message map functions
	//{{AFX_MSG(CDlgActionProp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGACTIONPROP_H__FF57B31E_15BB_416B_A92D_7ACEEF7A6F2B__INCLUDED_)
