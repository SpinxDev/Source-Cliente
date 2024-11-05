#if !defined(AFX_FORMCLU_H__902AB713_C34C_44D7_A953_67004BD54100__INCLUDED_)
#define AFX_FORMCLU_H__902AB713_C34C_44D7_A953_67004BD54100__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormClu.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormClu form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CFormClu : public CFormView
{
protected:
	CFormClu();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFormClu)

// Form Data
public:
	//{{AFX_DATA(CFormClu)
	enum { IDD = IDD_FORMVIEW_CLU };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormClu)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFormClu();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CFormClu)
	afx_msg void OnButtonInfoClu();
	afx_msg void OnButtonConnectClu();
	afx_msg void OnButtonCreateClu();
	afx_msg void OnButtonCheckClu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void    _AddInfo(const char * szTxt, bool bAppend);
	void    _Check();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMCLU_H__902AB713_C34C_44D7_A953_67004BD54100__INCLUDED_)
