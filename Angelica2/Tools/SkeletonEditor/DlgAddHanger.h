#if !defined(AFX_DLGADDHANGER_H__10B0CCDC_B0E7_45B3_845A_B71F566DAA7E__INCLUDED_)
#define AFX_DLGADDHANGER_H__10B0CCDC_B0E7_45B3_845A_B71F566DAA7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddHanger.h : header file
//

class CModel;

/////////////////////////////////////////////////////////////////////////////
// CDlgAddHanger dialog

class CDlgAddHanger : public CDialog
{
// Construction
public:
	CDlgAddHanger(CModel* pModel, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAddHanger)
	enum { IDD = IDD_ADDHANGER };
	CComboBox	m_CCCombo;
	CComboBox	m_HangPtCombo;
	CString	m_strFileName;
	CString	m_strName;
	//}}AFX_DATA

public:
	
	bool IsOnBone() { return m_bOnBone; }
	const char* GetBindTo() { return m_strBindTo; }
	int GetHangerType() { return m_iHangerType; } 
	const char* GetFileName() { return m_strFileName; }
	const char* GetName() { return m_strName; }
	const char* GetCCName() { return m_strCC; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddHanger)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CModel*		m_pModel;
	bool		m_bOnBone;		//	true, on bone; false, on hook
	int			m_iHangerType;	//	hanger type: model, gfx
	CString		m_strBindTo;	//	Bind to what ?
	CString		m_strCC;		//	Conection Center name

	//	Reset connection center list
	void ResetCCList();

	// Generated message map functions
	//{{AFX_MSG(CDlgAddHanger)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtnGfx();
	afx_msg void OnBtnModel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDHANGER_H__10B0CCDC_B0E7_45B3_845A_B71F566DAA7E__INCLUDED_)
