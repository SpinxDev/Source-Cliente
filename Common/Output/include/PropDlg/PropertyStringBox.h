#if !defined(AFX_PROPERTYSTRINGBOX_H__075AFFFB_469A_4524_BF80_D98DA6710FB7__INCLUDED_)
#define AFX_PROPERTYSTRINGBOX_H__075AFFFB_469A_4524_BF80_D98DA6710FB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyStringBox.h : header file
//
#include "PropertyBox.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyStringBox window

class CPropertyStringBox : public CPropertyBox
{
// Construction
public:
	CPropertyStringBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyStringBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);
	virtual void SetReadOnly(BOOL bReadOnly);
	virtual ~CPropertyStringBox();

	// Generated message map functions
protected:
	afx_msg void OnEditChanged();
	CString m_str_value;
	CComboBox m_wnd_list;	
	CEdit m_wnd_edit;
	bool m_bool_modified;
	//{{AFX_MSG(CPropertyStringBox)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYSTRINGBOX_H__075AFFFB_469A_4524_BF80_D98DA6710FB7__INCLUDED_)
