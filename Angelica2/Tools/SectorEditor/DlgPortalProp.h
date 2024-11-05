#if !defined(AFX_DLGPORTALPROP_H__E120E297_D0E0_4DF9_93FE_3B6F60169861__INCLUDED_)
#define AFX_DLGPORTALPROP_H__E120E297_D0E0_4DF9_93FE_3B6F60169861__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPortalProp.h : header file
//

class CPortal;

/////////////////////////////////////////////////////////////////////////////
// CDlgPortalProp dialog

class CDlgPortalProp : public CDialog
{
// Construction
public:
	CDlgPortalProp(CPortal* pPortal, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPortalProp)
	enum { IDD = IDD_PORTALPROP };
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPortalProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CPortal*	m_pPortal;

	// Generated message map functions
	//{{AFX_MSG(CDlgPortalProp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPORTALPROP_H__E120E297_D0E0_4DF9_93FE_3B6F60169861__INCLUDED_)
