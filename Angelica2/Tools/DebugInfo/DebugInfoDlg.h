// DebugInfoDlg.h : header file
//

#if !defined(AFX_DEBUGINFODLG_H__44669090_CEB6_4FAB_B941_9B9145904808__INCLUDED_)
#define AFX_DEBUGINFODLG_H__44669090_CEB6_4FAB_B941_9B9145904808__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDebugInfoDlg dialog

class CDebugInfoDlg : public CDialog
{
// Construction
public:
	CDebugInfoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDebugInfoDlg)
	enum { IDD = IDD_DEBUGINFO_DIALOG };
	CButton	m_TopMostCheck;
	CEdit	m_MsgEdit;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDebugInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnBtnClear();
	afx_msg void OnCheckTopmost();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGINFODLG_H__44669090_CEB6_4FAB_B941_9B9145904808__INCLUDED_)
