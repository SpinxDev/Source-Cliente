//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of BCGControlBar Library Professional Edition
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPRibbonCustomizePage.h : header file
//

#if !defined(AFX_BCGPRIBBONCUSTOMIZEPAGE_H__F0EB6A7D_D21D_47B5_99AF_D55DBC42F199__INCLUDED_)
#define AFX_BCGPRIBBONCUSTOMIZEPAGE_H__F0EB6A7D_D21D_47B5_99AF_D55DBC42F199__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGCBPro.h"

#ifndef BCGP_EXCLUDE_RIBBON

#include "BCGPPropertySheet.h"
#include "BCGPPropertyPage.h"
#include "BCGPButton.h"
#include "BCGPRibbonCommandsListBox.h"
#include "bcgprores.h"

class CBCGPRibbonBar;
class CBCGPLocalResource;
class CBCGPRibbonCustomCategory;

/////////////////////////////////////////////////////////////////////////////
// CBCGPRibbonCustomizePage dialog

class BCGCBPRODLLEXPORT CBCGPRibbonCustomizePage : public CBCGPPropertyPage
{
	friend class CBCGPRibbonCustomize;

	DECLARE_DYNCREATE(CBCGPRibbonCustomizePage)

// Construction
public:
	CBCGPRibbonCustomizePage(CBCGPRibbonBar* pRibbonBar = NULL);
	~CBCGPRibbonCustomizePage();

	void AddCustomCategory (LPCTSTR lpszName, const CList<UINT, UINT>& lstIDS);
	void EnableKeyboradCustomization (BOOL bEnable = TRUE);

// Dialog Data
	//{{AFX_DATA(CBCGPRibbonCustomizePage)
	enum { IDD = IDD_BCGBARRES_PROPPAGE8 };
	CButton	m_wndKbdCustomize;
	CButton	m_wndAdd;
	CButton	m_wndRemove;
	CBCGPRibbonCommandsListBox	m_wndCommandsList;
	CComboBox	m_wndCategoryCombo;
	CBCGPRibbonCommandsListBox	m_wndQATList;
	CBCGPButton	m_wndUp;
	CBCGPButton	m_wndDown;
	int		m_nCategory;
	BOOL	m_bQAToolbarOnBottom;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBCGPRibbonCustomizePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBCGPRibbonCustomizePage)
	afx_msg void OnSelendokCategoryCombo();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnToolbarReset();
	afx_msg void OnSelchangeQATCommands();
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomizeKeyboard();
	afx_msg void OnSelchangeCommandsList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void MoveItem (BOOL bMoveUp);

	CBCGPRibbonBar*													m_pRibbonBar;
	CList<CBCGPRibbonCustomCategory*,CBCGPRibbonCustomCategory*>	m_lstCustomCategories;
	BOOL															m_bIsCustomizeKeyboard;
};

class BCGCBPRODLLEXPORT CBCGPRibbonCustomize : public CBCGPPropertySheet
{
	DECLARE_DYNAMIC(CBCGPRibbonCustomize)

public:
	CBCGPRibbonCustomize (CWnd* pWndParent, CBCGPRibbonBar* pRibbon);
	virtual ~CBCGPRibbonCustomize ();

	void EnableKeyboradCustomization (BOOL bEnable = TRUE);

protected:
	CBCGPRibbonCustomizePage* m_pPage;
};

#endif // BCGP_EXCLUDE_RIBBON

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPRIBBONCUSTOMIZEPAGE_H__F0EB6A7D_D21D_47B5_99AF_D55DBC42F199__INCLUDED_)
