#if !defined(AFX_DLGBUILDHORICULL_H__8F61DC02_E8F6_49B4_9F97_1EDFDFDA1253__INCLUDED_)
#define AFX_DLGBUILDHORICULL_H__8F61DC02_E8F6_49B4_9F97_1EDFDFDA1253__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBuildHoriCull.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildHoriCull dialog

class CDlgBuildHoriCull : public CDialog
{
// Construction
public:
	CDlgBuildHoriCull(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBuildHoriCull)
	enum { IDD = IDD_BUILDHORICULL };
	CProgressCtrl	m_Progress;
	CString	m_strTrnFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBuildHoriCull)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBuildHoriCull)
	afx_msg void OnBtnBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnBuild();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBUILDHORICULL_H__8F61DC02_E8F6_49B4_9F97_1EDFDFDA1253__INCLUDED_)
