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
// BCGPComboBox.h : header file
//

#if !defined(AFX_BCGPCOMBOBOX_H__B809A10B_3085_419E_8ADA_6AA9A852CA73__INCLUDED_)
#define AFX_BCGPCOMBOBOX_H__B809A10B_3085_419E_8ADA_6AA9A852CA73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGCBPro.h"
#include "BCGPEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGPComboBox window

class BCGCBPRODLLEXPORT CBCGPComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CBCGPComboBox)

// Construction
public:
	CBCGPComboBox();

// Attributes
public:
	BOOL		m_bOnGlass;
	BOOL		m_bVisualManagerStyle;

protected:
	CBCGPEdit	m_wndEdit;
	BOOL		m_bIsDroppedDown;
	CRect		m_rectBtn;
	BOOL		m_bIsButtonHighlighted;
	BOOL		m_bTracked;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBCGPComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPComboBox)
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnCloseup();
	afx_msg void OnDropdown();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnBCGSetControlVMMode (WPARAM, LPARAM);
	afx_msg LRESULT OnBCGSetControlAero (WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPCOMBOBOX_H__B809A10B_3085_419E_8ADA_6AA9A852CA73__INCLUDED_)
