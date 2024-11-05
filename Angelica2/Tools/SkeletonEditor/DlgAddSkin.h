#if !defined(AFX_DLGADDSKIN_H__2F4866D2_B620_49BE_865C_F8FA3A639443__INCLUDED_)
#define AFX_DLGADDSKIN_H__2F4866D2_B620_49BE_865C_F8FA3A639443__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddSkin.h : header file
//

class CModel;

/////////////////////////////////////////////////////////////////////////////
// CDlgAddSkin dialog

class CDlgAddSkin : public CDialog
{
// Construction
public:
	CDlgAddSkin(CModel* pModel, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAddSkin)
	enum { IDD = IDD_ADDSKIN };
	CListCtrl	m_SkinList;
	//}}AFX_DATA

	//	Get modified flag
	bool GetModifiedFlag() { return m_bModifiedFlag; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddSkin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CModel*		m_pModel;
	bool		m_bModifiedFlag;
	CString		m_strLastDir;

	//	Update whole skin list
	void UpdateSkinList();

	// Generated message map functions
	//{{AFX_MSG(CDlgAddSkin)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAddskin();
	afx_msg void OnBtnReplaceskin();
	afx_msg void OnBtnRemoveskin();
	afx_msg void OnBtnDeleteskin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDSKIN_H__2F4866D2_B620_49BE_865C_F8FA3A639443__INCLUDED_)
