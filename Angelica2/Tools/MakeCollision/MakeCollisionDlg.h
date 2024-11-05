// MakeCollisionDlg.h : header file
//

#if !defined(AFX_MAKECOLLISIONDLG_H__1BCEA6DC_D719_47AC_A0ED_BED4E4E83F7A__INCLUDED_)
#define AFX_MAKECOLLISIONDLG_H__1BCEA6DC_D719_47AC_A0ED_BED4E4E83F7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMakeCollisionDlg dialog

class CMakeCollisionDlg : public CDialog
{
// Construction
public:
	
	//	Progress segment
	struct PROGSEGMENT
	{
		int		iStart;
		int		iEnd;
	};
	
	CMakeCollisionDlg(CWnd* pParent = NULL);	// standard constructor

	//	Add message
	void AddMessage(const char* szMsg);
	//	Update last message
	void UpdateLastMessage(const char* szMsg);
	//	Set current progress segment
	void SetCurProgressSeg(int iSeg) { m_iCurProgSeg = iSeg; }
	//	Step progress bar
	void StepProgressBar(int iPercent);

// Dialog Data
	//{{AFX_DATA(CMakeCollisionDlg)
	enum { IDD = IDD_MAKECOLLISION_DIALOG };
	CEdit	m_LastMsgEdit;
	CEdit	m_MsgEdit;
	CProgressCtrl	m_Progress;
	CString	m_strSceneFile;
	BOOL	m_bCreateBrush;
	BOOL	m_bMergeSide;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakeCollisionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	HICON m_hIcon;

	PROGSEGMENT	m_aProgSegs[100];
	int			m_iCurProgSeg;		//	Current progress segment
	int			m_iProgTotal;		//	Total value

	//	Check parameters
	bool CheckParameters();
	//	Set configs to default value
	void DefaultConfigs();

	//	Initialize progress segment
	void ResetProgress(int iTotal, int iNumSeg, PROGSEGMENT* aSegs);

	// Generated message map functions
	//{{AFX_MSG(CMakeCollisionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnBrowse();
	afx_msg void OnBtnMakeesp();
	afx_msg void OnBtnMakekdtree();
	afx_msg void OnBtnClearmsg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAKECOLLISIONDLG_H__1BCEA6DC_D719_47AC_A0ED_BED4E4E83F7A__INCLUDED_)
