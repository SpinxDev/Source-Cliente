#if !defined(AFX_DLGPOSITION_H__E254CC55_A300_473E_A366_ED3ABE3EEB74__INCLUDED_)
#define AFX_DLGPOSITION_H__E254CC55_A300_473E_A366_ED3ABE3EEB74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPosition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPosition dialog

class CDlgPosition : public CDialog
{
// Construction
public:
	CDlgPosition(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPosition)
	enum { IDD = IDD_POSITION };
	float	m_fx;
	float	m_fy;
	float	m_fz;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPosition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPosition)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPOSITION_H__E254CC55_A300_473E_A366_ED3ABE3EEB74__INCLUDED_)
