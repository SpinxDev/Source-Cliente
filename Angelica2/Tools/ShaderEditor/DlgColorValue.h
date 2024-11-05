#if !defined(AFX_DLGCOLORVALUE_H__89C56546_BBB5_47F7_A0B4_D5BA0B828357__INCLUDED_)
#define AFX_DLGCOLORVALUE_H__89C56546_BBB5_47F7_A0B4_D5BA0B828357__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColorValue.h : header file
//

#include "A3DTypes.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgColorValue dialog

class CDlgColorValue : public CDialog
{
// Construction
public:
	CDlgColorValue(const A3DCOLORVALUE& col, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgColorValue)
	enum { IDD = IDD_COLORVALUE };
	float	m_b;
	float	m_a;
	float	m_g;
	float	m_r;
	//}}AFX_DATA

	//	Get color value
	const A3DCOLORVALUE& GetColorValue() { return m_colValue; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColorValue)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	A3DCOLORVALUE	m_colValue;

	// Generated message map functions
	//{{AFX_MSG(CDlgColorValue)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLORVALUE_H__89C56546_BBB5_47F7_A0B4_D5BA0B828357__INCLUDED_)
