#if !defined(AFX_DLGBUILDESP_H__A7E1DC00_4540_40FA_8D19_AD8510E9AB77__INCLUDED_)
#define AFX_DLGBUILDESP_H__A7E1DC00_4540_40FA_8D19_AD8510E9AB77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBuildESP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildESP dialog
struct s_CONFIGS;

class CDlgBuildESP : public CDialog
{
// Construction
public:
	CDlgBuildESP(s_CONFIGS* pConfigs, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBuildESP)
	enum { IDD = IDD_BUILDESP };
	int		m_iLeafSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBuildESP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	s_CONFIGS*	m_pConfigs;

	// Generated message map functions
	//{{AFX_MSG(CDlgBuildESP)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBUILDESP_H__A7E1DC00_4540_40FA_8D19_AD8510E9AB77__INCLUDED_)
