#if !defined(AFX_BUMPMATDLG_H__1586CE03_D69D_462F_BC98_DAF956840A84__INCLUDED_)
#define AFX_BUMPMATDLG_H__1586CE03_D69D_462F_BC98_DAF956840A84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BumpMatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBumpMatDlg dialog

class CBumpMatDlg : public CDialog
{
// Construction
public:
	CBumpMatDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBumpMatDlg)
	enum { IDD = IDD_BUMPMATDLG };
	float	m_fMat00;
	float	m_fMat01;
	float	m_fMat10;
	float	m_fMat11;
	//}}AFX_DATA

// Attributes
public:

	bool		Init(float f00, float f01, float f10, float f11);

	float		Get00()		{	return m_fMat00;	}
	float		Get01()		{	return m_fMat01;	}
	float		Get10()		{	return m_fMat10;	}
	float		Get11()		{	return m_fMat11;	}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBumpMatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

protected:	//	Operations

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBumpMatDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUMPMATDLG_H__1586CE03_D69D_462F_BC98_DAF956840A84__INCLUDED_)
