#if !defined(AFX_DLGMODELINFO_H__3F4A8B94_54F5_4F3D_A1A2_1759A6B7FBFC__INCLUDED_)
#define AFX_DLGMODELINFO_H__3F4A8B94_54F5_4F3D_A1A2_1759A6B7FBFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgModelProp.h : header file
//

class CModel;

/////////////////////////////////////////////////////////////////////////////
// CDlgModelInfo dialog

class CDlgModelInfo : public CDialog
{
// Construction
public:
	CDlgModelInfo(CModel* pModel, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgModelInfo)
	enum { IDD = IDD_MODELINFO };
	CEdit	m_PropEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgModelInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CModel*		m_pModel;

	void AddLine(const char* szMsg);

	// Generated message map functions
	//{{AFX_MSG(CDlgModelInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMODELINFO_H__3F4A8B94_54F5_4F3D_A1A2_1759A6B7FBFC__INCLUDED_)
