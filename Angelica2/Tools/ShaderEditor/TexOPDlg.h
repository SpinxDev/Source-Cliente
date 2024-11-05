#if !defined(AFX_TEXOPDLG_H__5AEE22B5_3367_4B52_B791_82E2996D1188__INCLUDED_)
#define AFX_TEXOPDLG_H__5AEE22B5_3367_4B52_B791_82E2996D1188__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TexOPDlg.h : header file
//

#include <A3DShader.h>

///////////////////////////////////////////////////////////////////////////
//
//	CTexOPDlg dialog
//
///////////////////////////////////////////////////////////////////////////

class CTexOPDlg : public CDialog
{
// Construction
public:
	CTexOPDlg(CWnd* pParent=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTexOPDlg)
	enum { IDD = IDD_TEXOPDLG };
	CComboBox	m_OPCombo;
	CComboBox	m_ModCombo2;
	CComboBox	m_ModCombo1;
	CComboBox	m_ArgCombo2;
	CComboBox	m_ArgCombo1;
	//}}AFX_DATA

// Types
public:

	//	Operation type
	enum
	{
		OP_COLOR	= 0,
		OP_ALPHA	= 1
	};

// Attributes
public:

	bool SetParam(int iStage, int iType, const SHADERSTAGE& stage);

// Operations
public:

// Attributes
protected:

	int			m_iType;		//	Operation type
	int			m_iStage;		//	Stage index
	SHADERSTAGE	m_Stage;		//	Stage data
	UINT		m_hTimer;		//	Timer handler

	//	Initial settings
	DWORD		m_dwInitOP;
	DWORD		m_dwInitArg1;
	DWORD		m_dwInitArg2;
	DWORD		m_dwInitMod1;
	DWORD		m_dwInitMod2;

// Operations
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTexOPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void ApplyChanges();

	// Generated message map functions
	//{{AFX_MSG(CTexOPDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeArgcombo1();
	afx_msg void OnSelchangeArgcombo2();
	afx_msg void OnSelchangeModcombo1();
	afx_msg void OnSelchangeModcombo2();
	afx_msg void OnSelchangeOpcombo();
	afx_msg void OnDestroy();
	afx_msg void OnBtnRestore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXOPDLG_H__5AEE22B5_3367_4B52_B791_82E2996D1188__INCLUDED_)
