#if !defined(AFX_WAVEDLG_H__1DF9A908_8C5E_4FBA_B45F_826E4CFF95AE__INCLUDED_)
#define AFX_WAVEDLG_H__1DF9A908_8C5E_4FBA_B45F_826E4CFF95AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaveDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaveDlg dialog

class CWaveDlg : public CDialog
{
// Construction
public:
	CWaveDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaveDlg)
	enum { IDD = IDD_WAVEDLG };
	CComboBox	m_WaveCombo;
	float	m_fAmp;
	float	m_fBase;
	float	m_fPhase;
	int		m_iPeriod;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

	bool		Init(char* szTitle, SHADERWAVE& Wave);
	void		GetData(SHADERWAVE& Wave);

// Attributes
protected:

	bool		m_bTurb;			//	tcmod turbulence ?
	int			m_iFunc;			//	Wave function
	char		m_szTitle[128];		//	Dialog's title

// Operations
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaveDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVEDLG_H__1DF9A908_8C5E_4FBA_B45F_826E4CFF95AE__INCLUDED_)
