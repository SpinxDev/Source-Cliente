#if !defined(AFX_COMBOLISTDLG_H__D4B63CFD_ECEB_4B1D_967F_9D3CE4FF09F3__INCLUDED_)
#define AFX_COMBOLISTDLG_H__D4B63CFD_ECEB_4B1D_967F_9D3CE4FF09F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboListDlg.h : header file
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
//	CComboListDlg dialog
//
///////////////////////////////////////////////////////////////////////////

class CComboListDlg : public CDialog
{
// Construction
public:
	CComboListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CComboListDlg)
	enum { IDD = IDD_COMBOLISTDLG };
	CComboBox	m_ValueList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
public:

	bool		Init(int iType, int iInitValue);

	int			GetSelectedItem()	{	return m_iSelItem;	}
	int			GetType()			{	return m_iType;		}	

// Operations
public:

// Attributes
protected:

	int			m_iType;		//	Type
	int			m_iInitValue;	//	Initial value
	int			m_iSelItem;		//	Selected item

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComboListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOLISTDLG_H__D4B63CFD_ECEB_4B1D_967F_9D3CE4FF09F3__INCLUDED_)
