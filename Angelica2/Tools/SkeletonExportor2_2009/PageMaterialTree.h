#if !defined(AFX_PAGEMATERIALTREE_H__FE2F482E_BF1F_4116_BBCC_70E797F8F19B__INCLUDED_)
#define AFX_PAGEMATERIALTREE_H__FE2F482E_BF1F_4116_BBCC_70E797F8F19B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageMaterialTree.h : header file
//

class CSkinCandidate;
class CMAXMesh;

/////////////////////////////////////////////////////////////////////////////
// CPageMaterialTree dialog

class CPageMaterialTree : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageMaterialTree)

	enum
	{
		SEL_SELECT = 0,
		SEL_UNSELECT,
		SEL_INVERSE,
	};

// Construction
public:
	CPageMaterialTree();
	~CPageMaterialTree();

	void SetStep(int iIndex, int iTotal);
	void SetSkinCandidate(CSkinCandidate* p) { m_pSkinCandidate = p; }

// Dialog Data
	//{{AFX_DATA(CPageMaterialTree)
	enum { IDD = IDD_MATERIALTREE };
	CTreeCtrl	m_MtlTree;
	CString	m_strStep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageMaterialTree)
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

	CSkinCandidate*		m_pSkinCandidate;

protected:	//	Operations

	bool BeginPage();
	bool EndPage();

	//	Insert a mesh
	bool InsertMesh(CMAXMesh* pMesh, HTREEITEM hParentItem);
	//	Insert a material
	bool InsertMaterial(CMAXMesh* pMesh, void* pMtl, HTREEITEM hParentItem);
	//	Set selection state of a material item and all it's children
	void SelectMaterial(HTREEITEM hMtlItem, int iSelFlag, bool bSelfSet);
	//	Set selection state of a material item
	void SelectMaterial(HTREEITEM hMtlItem, int iSelFlag);

	//	Update selection states of material and all it's children
	void UpdateMaterial(CMAXMesh* pMesh, HTREEITEM hMtlItem);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageMaterialTree)
	afx_msg void OnBtnSelectall();
	afx_msg void OnBtnUnselectall();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickTreeMaterial(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEMATERIALTREE_H__FE2F482E_BF1F_4116_BBCC_70E797F8F19B__INCLUDED_)
