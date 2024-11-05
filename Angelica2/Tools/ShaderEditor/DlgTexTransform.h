#if !defined(AFX_DLGTEXTRANSFORM_H__78C6DAA2_CF01_49B3_95EF_6A92A992D835__INCLUDED_)
#define AFX_DLGTEXTRANSFORM_H__78C6DAA2_CF01_49B3_95EF_6A92A992D835__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTexTransform.h : header file
//

#include "A3DMatrix.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTexTransform dialog

class CDlgTexTransform : public CDialog
{
// Construction
public:
	CDlgTexTransform(const A3DMATRIX4& mat, DWORD dwFlag, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTexTransform)
	enum { IDD = IDD_TEXTRANSFORM };
	CComboBox	m_FlagCombo;
	float	m_11;
	float	m_12;
	float	m_13;
	float	m_14;
	float	m_21;
	float	m_22;
	float	m_23;
	float	m_24;
	float	m_31;
	float	m_32;
	float	m_33;
	float	m_34;
	float	m_41;
	float	m_42;
	float	m_43;
	float	m_44;
	BOOL	m_bProjected;
	//}}AFX_DATA

	//	Get flag
	DWORD GetFlag() { return m_dwFlag; }
	//	Get matrix
	const A3DMATRIX4& GetMatrix() { return m_matTM; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTexTransform)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	A3DMATRIX4	m_matTM;
	DWORD		m_dwFlag;

	// Generated message map functions
	//{{AFX_MSG(CDlgTexTransform)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTEXTRANSFORM_H__78C6DAA2_CF01_49B3_95EF_6A92A992D835__INCLUDED_)
