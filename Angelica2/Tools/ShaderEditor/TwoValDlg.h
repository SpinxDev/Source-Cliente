#if !defined(AFX_TWOVALDLG_H__6824312F_4A69_4738_8A5E_DE389ABE15D5__INCLUDED_)
#define AFX_TWOVALDLG_H__6824312F_4A69_4738_8A5E_DE389ABE15D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TwoValDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTwoValDlg dialog

class CTwoValDlg : public CDialog
{
// Construction
public:
	CTwoValDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTwoValDlg)
	enum { IDD = IDD_TWOVALUEDLG };
	CEdit	m_ValEdit2;
	CEdit	m_ValEdit1;
	CString	m_strValue1;
	CString	m_strValue2;
	//}}AFX_DATA

// Types
public:

	//	Value type
	enum
	{
		VAL_FLOAT	= 0,
		VAL_DWORD	= 1,
		VAL_INT		= 2
	};

// Attributes
public:

// Operations
public:

	bool		Init(char* szTitle, char* szPrompt1, char* szPrompt2, float fVal1, float fVal2);
	bool		Init(char* szTitle, bool bInt, char* szPrompt1, char* szPrompt2, DWORD dwVal1, DWORD dwVal2);

	void		GetValue(float* pfVal1, float* pfVal2)		{	*pfVal1 = m_fVal1; *pfVal2 = m_fVal2;		}
	void		GetValue(DWORD* pdwVal1, DWORD* pdwVal2)	{	*pdwVal1 = m_dwVal1; *pdwVal2 = m_dwVal2;	}

// Attributes
protected:

	int			m_iType;			//	Value type
	float		m_fVal1;			//	Values
	float		m_fVal2;
	DWORD		m_dwVal1;
	DWORD		m_dwVal2;

	char		m_szTitle[128];		//	Dialog's title
	char		m_szPrompt1[64];	//	Prompt1
	char		m_szPrompt2[64];	//	Prompt2

// Operations
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTwoValDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTwoValDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWOVALDLG_H__6824312F_4A69_4738_8A5E_DE389ABE15D5__INCLUDED_)
