#if !defined(AFX_DLGINPUTFLOT_H__67579D72_0CB4_4AA7_9B8A_876E2EE7E462__INCLUDED_)
#define AFX_DLGINPUTFLOT_H__67579D72_0CB4_4AA7_9B8A_876E2EE7E462__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInputFlot.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInputFlot dialog

class CDlgInputFlot : public CDialog
{
// Construction
public:
	CDlgInputFlot(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInputFlot)
	enum { IDD = IDD_INPUTFLOAT };
	CButton	m_PromptBtn;
	float	m_fValue;
	//}}AFX_DATA

	CString	m_strPrompt;
	float	m_fMin;
	float	m_fMax;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputFlot)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputFlot)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUTFLOT_H__67579D72_0CB4_4AA7_9B8A_876E2EE7E462__INCLUDED_)
