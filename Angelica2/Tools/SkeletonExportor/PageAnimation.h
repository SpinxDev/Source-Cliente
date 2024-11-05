#if !defined(AFX_PAGEANIMATION_H__3AD2BBBE_6D98_4DAA_A31A_FB5A05B8F175__INCLUDED_)
#define AFX_PAGEANIMATION_H__3AD2BBBE_6D98_4DAA_A31A_FB5A05B8F175__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageAnimation.h : header file
//

class CSkeletonCandidate;

/////////////////////////////////////////////////////////////////////////////
// CPageAnimation dialog

class CPageAnimation : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageAnimation)

// Construction
public:
	CPageAnimation();
	~CPageAnimation();

	void SetStep(int iIndex, int iTotal);
	void SetSkeletonCandidate(CSkeletonCandidate* p) { m_pSkeletonCandidate = p; }

	void SetFrames(int iStartFrame, int iEndFrame)
	{
		m_iStartFrame	= iStartFrame;
		m_iEndFrame		= iEndFrame;
	}

	int GetStartFrame() { return m_iStartFrame; }
	int GetEndFrame() { return m_iEndFrame; }

// Dialog Data
	//{{AFX_DATA(CPageAnimation)
	enum { IDD = IDD_ANIMATION };
	int		m_iEndFrame;
	int		m_iStartFrame;
	CString	m_strStep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageAnimation)
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
	//{{AFX_MSG(CPageAnimation)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEANIMATION_H__3AD2BBBE_6D98_4DAA_A31A_FB5A05B8F175__INCLUDED_)
