#if !defined(AFX_PIXELSHADERDLG_H__55D7FB22_D07E_40AE_AA62_554A46FEF09D__INCLUDED_)
#define AFX_PIXELSHADERDLG_H__55D7FB22_D07E_40AE_AA62_554A46FEF09D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PixelShaderDlg.h : header file
//

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	CPixelShaderDlg dialog
//
///////////////////////////////////////////////////////////////////////////

class CPixelShaderDlg : public CDialog
{
// Construction
public:
	CPixelShaderDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPixelShaderDlg)
	enum { IDD = IDD_PIXELSHADERDLG };
	CEdit	m_Content;
	BOOL	m_bUsePShader;
	CString	m_strPrompt;
	CString	m_strPixelFile;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

	bool		Init(const char* szFileName);		//	Initialize data
	const char*	GetShaderFile()		{	return m_strPixelFile;	}
	bool		UsePixelShader()	{	return m_bUsePShader ? true : false;	}

protected:	//	Attributes

protected:	//	Operations

	bool		OpenPixelShaderFile(const char* szFileName);	//	Open a pixel shader file and load it's content
	void		Prompt(char* szPrompt);							//	Display prompt

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPixelShaderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPixelShaderDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnUsepixelshader();
	afx_msg void OnSave();
	afx_msg void OnCompile();
	afx_msg void OnBtnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIXELSHADERDLG_H__55D7FB22_D07E_40AE_AA62_554A46FEF09D__INCLUDED_)
