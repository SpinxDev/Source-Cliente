#if !defined(AFX_PAGEMESH_H__1FE1B2F9_4BB0_4130_A89A_7ABE09DDD6AC__INCLUDED_)
#define AFX_PAGEMESH_H__1FE1B2F9_4BB0_4130_A89A_7ABE09DDD6AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMesh.h : header file
//

class CSkeletonCandidate;
class CSkinCandidate;

/////////////////////////////////////////////////////////////////////////////
// CPageMesh dialog

class CPageMesh : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMesh)

// Construction
public:
	CPageMesh();
	~CPageMesh();

	void SetStep(int iIndex, int iTotal);
	void SetSkeletonCandidate(CSkeletonCandidate* p) { m_pSkeletonCandidate = p; }
	void SetSkinCandidate(CSkinCandidate* p) { m_pSkinCandidate = p; }

	void SetMaxBoneNum(int iNum) { m_iMaxBoneNum = iNum; }
	int GetMaxBoneNum() { return m_iMaxBoneNum; }

	void SetMinWeight(float fWeight) { m_fMinWeight = fWeight; }
	float GetMinWeight() { return m_fMinWeight; }

	bool GetUpdateSkeletonFlag() { return m_bUpdateSkeleton ? true : false; }

// Dialog Data
	//{{AFX_DATA(CPageMesh)
	enum { IDD = IDD_MESH };
	int		m_iMaxBoneNum;
	float	m_fMinWeight;
	CString	m_strStep;
	BOOL	m_bUpdateSkeleton;
	BOOL	m_bExtraTexDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageMesh)
	public:
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	int		m_iStepIndex;
	int		m_iStepTotal;

	CSkeletonCandidate*	m_pSkeletonCandidate;
	CSkinCandidate*		m_pSkinCandidate;

protected:	//	Operations

	bool BeginPage();
	bool EndPage();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageMesh)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEMESH_H__1FE1B2F9_4BB0_4130_A89A_7ABE09DDD6AC__INCLUDED_)
