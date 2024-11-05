#if !defined(AFX_DLGTEXCOORD_H__BCF0027D_A4D3_4EAF_8317_EF07A86E9987__INCLUDED_)
#define AFX_DLGTEXCOORD_H__BCF0027D_A4D3_4EAF_8317_EF07A86E9987__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTexCoord.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTexCoord dialog

class CDlgTexCoord : public CDialog
{
// Construction
public:
	CDlgTexCoord(DWORD dwTexCoord, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTexCoord)
	enum { IDD = IDD_TEXCOORDINDEX };
	CComboBox	m_FlagsCombo;
	CComboBox	m_IndexCombo;
	//}}AFX_DATA

	DWORD GetTexCoord() { return m_dwTexCoord; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTexCoord)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	DWORD		m_dwTexCoord;

	// Generated message map functions
	//{{AFX_MSG(CDlgTexCoord)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTEXCOORD_H__BCF0027D_A4D3_4EAF_8317_EF07A86E9987__INCLUDED_)
