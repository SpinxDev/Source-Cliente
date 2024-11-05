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

#if !defined(AFX_BCGPEDIT_H__A7B40464_1C05_4981_A5F2_E21D74058A09__INCLUDED_)
#define AFX_BCGPEDIT_H__A7B40464_1C05_4981_A5F2_E21D74058A09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCGPEdit.h : header file
//

#include "BCGCBPro.h"

class CBCGPCalculatorPopup;
class CBCGPCalculator;

/////////////////////////////////////////////////////////////////////////////
// CBCGPEdit window

class BCGCBPRODLLEXPORT CBCGPEdit : public CEdit
{
	DECLARE_DYNAMIC(CBCGPEdit)

	friend class CBCGPCalculator;

// Construction
public:
	CBCGPEdit();

// Attributes
public:
	enum BrowseMode
	{
		BrowseMode_None,
		BrowseMode_Default,
		BrowseMode_Calculator,
		BrowseMode_File,
		BrowseMode_Folder,
	};

	CBCGPEdit::BrowseMode GetMode () const
	{
		return m_Mode;
	}

	BOOL		m_bVisualManagerStyle;
	BOOL		m_bOnGlass;

protected:
	CRect		m_rectBtn;
	BOOL		m_bIsButtonPressed;
	BOOL		m_bIsButtonHighlighted;
	BOOL		m_bIsButtonCaptured;
	BrowseMode	m_Mode;
	CImageList	m_ImageBrowse;
	BOOL		m_bDefaultImage;
	CSize		m_sizeImage;
	CString		m_strLabel;
	CString		m_strDefFileExt;
	CString		m_strFileFilter;
	int			m_nBrowseButtonWidth;

	CBCGPCalculatorPopup*	m_pCalcPopup;
	CStringList				m_lstCalcAdditionalCommands;

// Operations
public:
	void EnableBrowseButton (BOOL bEnable = TRUE, LPCTSTR szLabel = _T("..."));
	void EnableFileBrowseButton (LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFilter = NULL);
	void EnableFolderBrowseButton ();
	void EnableCalculatorButton (const CStringList* plstAdditionalCommands = NULL);

	void SetBrowseButtonImage (HICON hIcon, BOOL bAutoDestroy = TRUE);
	void SetBrowseButtonImage (HBITMAP hBitmap, BOOL bAutoDestroy = TRUE);
	void SetBrowseButtonImage (UINT uiBmpResId);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	virtual void OnBrowse ();
	virtual void OnDrawBrowseButton (CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot);
	virtual void OnChangeLayout ();
	virtual void OnAfterUpdate ();
	virtual BOOL FilterCalcKey (int nChar);

	virtual void OnCalculatorUserCommand (CBCGPCalculator* pCalculator, UINT uiCmd);

// Implementation
public:
	virtual ~CBCGPEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPEdit)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnChange();
	//}}AFX_MSG
	afx_msg BCGNcHitTestType OnNcHitTest(CPoint point);
	afx_msg LRESULT OnBCGSetControlVMMode (WPARAM, LPARAM);
	afx_msg LRESULT OnBCGSetControlAero (WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	void SetIntenalImage ();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPEDIT_H__A7B40464_1C05_4981_A5F2_E21D74058A09__INCLUDED_)
