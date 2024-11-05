#if !defined(AFX_DLGEXPORTPTFILE_H__BAE15A8C_54B6_4735_A311_144697B7274F__INCLUDED_)
#define AFX_DLGEXPORTPTFILE_H__BAE15A8C_54B6_4735_A311_144697B7274F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportPtFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportPtFile dialog

class CDlgExportPtFile : public CDialog
{
// Construction
public:
	CDlgExportPtFile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportPtFile)
	enum { IDD = IDD_EXPORTPORTAL };
	int		m_iDivX;
	int		m_iDivY;
	int		m_iDivZ;
	float	m_fMaxLimit;
	float	m_fMinLimit;
	CString	m_strPortalFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportPtFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportPtFile)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTPTFILE_H__BAE15A8C_54B6_4735_A311_144697B7274F__INCLUDED_)
