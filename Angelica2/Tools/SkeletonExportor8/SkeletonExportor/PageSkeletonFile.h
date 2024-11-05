#if !defined(AFX_PAGESKELETONFILE_H__927153EC_FD74_4138_AB7D_73676C767124__INCLUDED_)
#define AFX_PAGESKELETONFILE_H__927153EC_FD74_4138_AB7D_73676C767124__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageSkeletonFile.h : header file
//

class CSkeletonCandidate;

/////////////////////////////////////////////////////////////////////////////
// CPageSkeletonFile dialog

class CPageSkeletonFile : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageSkeletonFile)

// Construction
public:
	CPageSkeletonFile();
	~CPageSkeletonFile();

	void SetStep(int iIndex, int iTotal);
	void SetSkeletonCandidate(CSkeletonCandidate* p) { m_pSkeletonCandidate = p; }

	CString GetSkeletonFile() { return m_strSkeletonFile; }

// Dialog Data
	//{{AFX_DATA(CPageSkeletonFile)
	enum { IDD = IDD_SKELETONFILE };
	CString	m_strSkeletonFile;
	CString	m_strStep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageSkeletonFile)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	int		m_iStepIndex;
	int		m_iStepTotal;

	CSkeletonCandidate*	m_pSkeletonCandidate;

protected:	//	Operations

	bool BeginPage();
	bool EndPage();
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageSkeletonFile)
	afx_msg void OnBtnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESKELETONFILE_H__927153EC_FD74_4138_AB7D_73676C767124__INCLUDED_)
