#if !defined(AFX_ALPHACOMPDLG_H__CE2F098F_1289_41C7_8355_FE523598344C__INCLUDED_)
#define AFX_ALPHACOMPDLG_H__CE2F098F_1289_41C7_8355_FE523598344C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlphaCompDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlphaCompDlg dialog

class CAlphaCompDlg : public CDialog
{
// Construction
public:
	CAlphaCompDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlphaCompDlg)
	enum { IDD = IDD_ALPHACOMPDLG };
	CComboBox	m_CompFunc;
	int		m_iRefValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlphaCompDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
public:

	void		Init(int iFunc, int iRefValue);		//	Initialize

	int			GetSelectedFunc()		{	return m_iFunc;			}
	int			GetRefValue()			{	return m_iRefValue;		}

// Operations
public:

// Attributes
protected:

	int			m_iFunc;		//	Selected compare function

// Operations
protected:

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlphaCompDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALPHACOMPDLG_H__CE2F098F_1289_41C7_8355_FE523598344C__INCLUDED_)
