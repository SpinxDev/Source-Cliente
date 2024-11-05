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
// BCGPListBox.h : header file
//

#if !defined(AFX_BCGPLISTBOX_H__7E778AEF_FFBB_486D_A7B3_D25D7868F5D9__INCLUDED_)
#define AFX_BCGPLISTBOX_H__7E778AEF_FFBB_486D_A7B3_D25D7868F5D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bcgcbpro.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGPListBox window

class BCGCBPRODLLEXPORT CBCGPListBox : public CListBox
{
// Construction
public:
	CBCGPListBox();

// Attributes
public:
	BOOL	m_bOnGlass;
	BOOL	m_bVisualManagerStyle;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBCGPListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPListBox)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnBCGSetControlVMMode (WPARAM, LPARAM);
	afx_msg LRESULT OnBCGSetControlAero (WPARAM, LPARAM);
	afx_msg LRESULT OnMouseLeave(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()

	int		m_nHighlightedItem;
	BOOL	m_bTracked;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPLISTBOX_H__7E778AEF_FFBB_486D_A7B3_D25D7868F5D9__INCLUDED_)
