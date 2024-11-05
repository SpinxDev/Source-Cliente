#ifndef _PROPERTY_SLIDER_BOX_H_
#define _PROPERTY_SLIDER_BOX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyColorBox.h : header file
//
#include "PropertyBox.h"
#include "Avariant.h"

class CXSliderDlg;
/////////////////////////////////////////////////////////////////////////////
// CPropertySliderBox window

class CPropertySliderBox : public CPropertyBox
{
// Construction
public:
	CPropertySliderBox();

// Attributes
public:
	void TestUpdateSliderData(AVariant s);
	void UpdateSliderData(AVariant s, bool bUpdate = true);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertySliderBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);
	virtual void SetReadOnly(BOOL bReadOnly);
	virtual ~CPropertySliderBox();
	// Generated message map functions
protected:
	afx_msg void OnEditChanged();
	afx_msg void OnBnClicked();
	CButton		m_wnd_button;
	CEdit		m_wnd_edit;
	BOOL		m_bool_modified;
	AVariant	m_value;
	CXSliderDlg *m_pSliderDialog;
	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertySliderBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYCOLORBOX_H__A8E4B5B2_E145_4095_A80F_53EF10DEB432__INCLUDED_)
