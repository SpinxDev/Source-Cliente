#if !defined(AFX_DLGMODELPROP_H__ABEE3746_69BD_4C6F_90B6_C85637A6206E__INCLUDED_)
#define AFX_DLGMODELPROP_H__ABEE3746_69BD_4C6F_90B6_C85637A6206E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgModelProp.h : header file
//

class CModel;

/////////////////////////////////////////////////////////////////////////////
// CDlgModelProp dialog

class CDlgModelProp : public CDialog
{
// Construction
public:
	CDlgModelProp(CModel* pModel, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgModelProp)
	enum { IDD = IDD_MODELPROP };
	CListCtrl	m_PropList;
	CString	m_strPropName;
	CString	m_strPropVal;
	//}}AFX_DATA

	bool GetModifiedFlag() { return m_bModified; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgModelProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CModel*		m_pModel;
	bool		m_bModified;

	// Generated message map functions
	//{{AFX_MSG(CDlgModelProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedListProp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnSubmit();
	afx_msg void OnBtnAddprop();
	afx_msg void OnBtnDelprop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMODELPROP_H__ABEE3746_69BD_4C6F_90B6_C85637A6206E__INCLUDED_)
