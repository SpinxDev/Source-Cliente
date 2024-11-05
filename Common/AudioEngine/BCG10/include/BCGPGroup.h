#if !defined(AFX_BCGPGROUP_H__917CCAFD_2ED2_44EC_8297_0C3A5C62F28F__INCLUDED_)
#define AFX_BCGPGROUP_H__917CCAFD_2ED2_44EC_8297_0C3A5C62F28F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPGroup.h : header file
//

#include "BCGCBPro.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGPGroup window

class BCGCBPRODLLEXPORT CBCGPGroup : public CButton
{
	DECLARE_DYNAMIC(CBCGPGroup)

// Construction
public:
	CBCGPGroup();

// Attributes
public:
	BOOL	m_bOnGlass;
	BOOL	m_bVisualManagerStyle;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPGroup)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBCGPGroup();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPGroup)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LRESULT OnBCGSetControlVMMode (WPARAM, LPARAM);
	afx_msg LRESULT OnBCGSetControlAero (WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPGROUP_H__917CCAFD_2ED2_44EC_8297_0C3A5C62F28F__INCLUDED_)
