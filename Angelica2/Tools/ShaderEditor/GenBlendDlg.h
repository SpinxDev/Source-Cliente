#if !defined(AFX_GENBLENDDLG_H__96AD930F_0F97_4387_A38F_D54012B9C47C__INCLUDED_)
#define AFX_GENBLENDDLG_H__96AD930F_0F97_4387_A38F_D54012B9C47C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenBlendDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGenBlendDlg dialog

class CGenBlendDlg : public CDialog
{
// Construction
public:
	CGenBlendDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGenBlendDlg)
	enum { IDD = IDD_GENBLENDDLG };
	CComboBox	m_SrcBlend;
	CComboBox	m_DestBlend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenBlendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
public:

	void		Init(int iSrcBlend, int iDstBlend);		//	Initialize

	int			GetSrcBlend()		{	return m_iSrcBlend;		}
	int			GetDstBlend()		{	return m_iDstBlend;		}

// Operations
public:

// Attributes
protected:

	int			m_iSrcBlend;		//	Selected source blend factor
	int			m_iDstBlend;		//	Selected destiantion blend factor

// Operations
protected:

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGenBlendDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENBLENDDLG_H__96AD930F_0F97_4387_A38F_D54012B9C47C__INCLUDED_)
