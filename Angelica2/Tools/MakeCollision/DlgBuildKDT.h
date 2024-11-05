#if !defined(AFX_DLGBUILDKDT_H__21561F82_B4AC_428D_A542_416AF410F480__INCLUDED_)
#define AFX_DLGBUILDKDT_H__21561F82_B4AC_428D_A542_416AF410F480__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBuildKDT.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildKDT dialog
struct s_CONFIGS;

class CDlgBuildKDT : public CDialog
{
// Construction
public:
	CDlgBuildKDT(s_CONFIGS* pConfigs, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBuildKDT)
	enum { IDD = IDD_BUILDKDT };
	float	m_fMinLeafSize;
	int		m_iMinSideNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBuildKDT)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	s_CONFIGS*	m_pConfigs;

	// Generated message map functions
	//{{AFX_MSG(CDlgBuildKDT)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBUILDKDT_H__21561F82_B4AC_428D_A542_416AF410F480__INCLUDED_)
