#if !defined(AFX_SINGLEVALDLG_H__5383C5C4_27E2_40CD_8BEA_D6C4F49CAD37__INCLUDED_)
#define AFX_SINGLEVALDLG_H__5383C5C4_27E2_40CD_8BEA_D6C4F49CAD37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SingleValDlg.h : header file
//

///////////////////////////////////////////////////////////////////////////
//
//	CSingleValDlg dialog
//
///////////////////////////////////////////////////////////////////////////

class CSingleValDlg : public CDialog
{
// Construction
public:
	CSingleValDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSingleValDlg)
	enum { IDD = IDD_SINGLEVALDLG };
	CEdit	m_ValueEdit;
	CString	m_strPrompt;
	//}}AFX_DATA

// Types
public:

	//	Value type
	enum
	{
		VAL_FLOAT	= 0,
		VAL_DWORD	= 1,
		VAL_STRING	= 2,
		VAL_INT		= 3
	};

// Attributes
public:

	bool		Init(const char* szTitle, float fValue);
	bool		Init(const char* szTitle, DWORD dwValue, bool bInt, bool bHex);
	bool		Init(const char* szTitle, const char* szValue, int iLimit);

	void		GetValue(float* pfValue)	{	*pfValue = m_fValue;		}
	void		GetValue(DWORD* pdwValue)	{	*pdwValue = m_dwValue;		}
	void		GetValue(char* szValue)		{	strcpy(szValue, m_szValue);	}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSingleValDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	int			m_iType;		//	Value type
	bool		m_bHex;			//	Hex
	int			m_iLimit;		//	Limit of string length

	DWORD		m_dwValue;
	float		m_fValue;
	char		m_szValue[256];

	char		m_szTitle[256];		//	Dialog window's title

protected:	//	Operations

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSingleValDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLEVALDLG_H__5383C5C4_27E2_40CD_8BEA_D6C4F49CAD37__INCLUDED_)
