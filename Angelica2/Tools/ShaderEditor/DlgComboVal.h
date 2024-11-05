#if !defined(AFX_DLGCOMBOVAL_H__ABA676E0_9F63_4EB9_838C_2873D12DD50E__INCLUDED_)
#define AFX_DLGCOMBOVAL_H__ABA676E0_9F63_4EB9_838C_2873D12DD50E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgComboVal.h : header file
//

#include <AArray.h>

/////////////////////////////////////////////////////////////////////////////
// CDlgComboVal dialog

class CDlgComboVal : public CDialog
{
// Construction
public:
	CDlgComboVal(CWnd* pParent = NULL);   // standard constructor

public:

	int AddCandidate(const char* szVal);
	void SetDefaultSel(int iSel) { m_iCurSel = iSel; }
	int GetCurSel() { return m_iCurSel; }
	void SetDescString(const char* szText) { m_strDesc = szText; }

// Dialog Data
	//{{AFX_DATA(CDlgComboVal)
	enum { IDD = IDD_COMBOVALDLG };
	CButton	m_DescBtn;
	CComboBox	m_ValCombo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgComboVal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	APtrArray<CString*>		m_aCandidates;
	CString					m_strDesc;
	int						m_iCurSel;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgComboVal)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOMBOVAL_H__ABA676E0_9F63_4EB9_838C_2873D12DD50E__INCLUDED_)
