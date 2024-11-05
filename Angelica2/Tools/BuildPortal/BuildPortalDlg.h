// BuildPortalDlg.h : header file
//

#if !defined(AFX_BUILDPORTALDLG_H__E4AFBE1B_C790_463E_BF65_42658A039465__INCLUDED_)
#define AFX_BUILDPORTALDLG_H__E4AFBE1B_C790_463E_BF65_42658A039465__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBuildPortalDlg dialog

class CBuildPortalDlg : public CDialog
{
// Construction
public:
	CBuildPortalDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBuildPortalDlg)
	enum { IDD = IDD_BUILDPORTAL_DIALOG };
	CString	m_strDescFile;
	CString	m_strMoxFile;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuildPortalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	//	Load scene file
	bool LoadSceneFile(const char* szFile);
	//	Load sector and portal description file
	bool LoadDescFile(const char* szFile);

	// Generated message map functions
	//{{AFX_MSG(CBuildPortalDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnCreate();
	afx_msg void OnDestroy();
	afx_msg void OnBtnBrowse1();
	afx_msg void OnBtnBrowse2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDPORTALDLG_H__E4AFBE1B_C790_463E_BF65_42658A039465__INCLUDED_)
