#if !defined(AFX_SAVEASKDLG_H__891332B6_2B24_4430_B7E5_ADDD846DC632__INCLUDED_)
#define AFX_SAVEASKDLG_H__891332B6_2B24_4430_B7E5_ADDD846DC632__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveAskDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSaveAskDlg dialog

class CSaveAskDlg : public CDialog
{
// Construction
public:
	CSaveAskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveAskDlg)
	enum { IDD = IDD_SAVEASKDLG };
	CString	m_strPrompt;
	//}}AFX_DATA

// Attributes
public:

	bool		Init(const char* szFile);		//	Initialize data

// Operations
public:

// Attributes
protected:

	char		m_szFileName[MAX_PATH];

// Operations
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveAskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveAskDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNotoall();
	afx_msg void OnYes();
	afx_msg void OnYestoall();
	afx_msg void OnNot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVEASKDLG_H__891332B6_2B24_4430_B7E5_ADDD846DC632__INCLUDED_)
