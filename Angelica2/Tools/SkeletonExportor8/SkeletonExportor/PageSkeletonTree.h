#if !defined(AFX_PAGESKELETONTREE_H__4348E952_D7E2_4BB1_A632_9D6EE3B0A40C__INCLUDED_)
#define AFX_PAGESKELETONTREE_H__4348E952_D7E2_4BB1_A632_9D6EE3B0A40C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageSkeletonTree.h : header file
//

class CSkeletonCandidate;

/////////////////////////////////////////////////////////////////////////////
// CPageSkeletonTree dialog

class CPageSkeletonTree : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageSkeletonTree)

	enum
	{
		SEL_SELECT = 0,
		SEL_UNSELECT,
	};

// Construction
public:
	CPageSkeletonTree();
	~CPageSkeletonTree();

	void SetStep(int iIndex, int iTotal);
	void SetSkeletonCandidate(CSkeletonCandidate* p) { m_pSkeletonCandidate = p; }
	
// Dialog Data
	//{{AFX_DATA(CPageSkeletonTree)
	enum { IDD = IDD_SKELETONTREE };
	CTreeCtrl	m_SkeletonTree;
	CString	m_strStep;
	BOOL	m_bSortBone;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageSkeletonTree)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	int			m_iStepIndex;
	int			m_iStepTotal;
	CImageList	m_ImageList;

	CSkeletonCandidate*	m_pSkeletonCandidate;

protected:	//	Operations

	bool BeginPage();
	bool EndPage();

	void InsertBoneCandidate(bool bBone, int iIndex, HTREEITEM hParentItem);
	void SelectBones(HTREEITEM hBoneItem, int iSelFlag);
	void SelectBone(HTREEITEM hBoneItem, int iSelFlag);

	void OnNoNubFlagChanged();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageSkeletonTree)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnSelectall();
	afx_msg void OnBtnUnselectall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedCheckNonub();
public:
	BOOL m_bNoNub;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESKELETONTREE_H__4348E952_D7E2_4BB1_A632_9D6EE3B0A40C__INCLUDED_)
