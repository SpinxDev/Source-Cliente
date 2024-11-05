#if !defined(AFX_FORMMAP_H__EADC0619_C421_4992_AEBC_A7E1AD59E34B__INCLUDED_)
#define AFX_FORMMAP_H__EADC0619_C421_4992_AEBC_A7E1AD59E34B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormMap form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CFormMap : public CFormView
{
protected:
	CFormMap();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFormMap)

// Form Data
public:
	//{{AFX_DATA(CFormMap)
	enum { IDD = IDD_FORMVIEW_MAP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void AddJoint(const char * txt);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormMap)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFormMap();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CFormMap)
	afx_msg void OnButtonLoadMapcfg();
	afx_msg void OnButtonViewLayer();
	afx_msg void OnButtonAddJoint();
	afx_msg void OnButtonDelJoint();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonLoadMapbin();
	afx_msg void OnButtonLoadMapcfg2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   void	_ResetJoints();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMMAP_H__EADC0619_C421_4992_AEBC_A7E1AD59E34B__INCLUDED_)
