#if !defined(AFX_BCGPPOPUPWINDOW_H__4DE5ED2B_6089_4A54_BF22_113C2C5FA4C0__INCLUDED_)
#define AFX_BCGPPOPUPWINDOW_H__4DE5ED2B_6089_4A54_BF22_113C2C5FA4C0__INCLUDED_

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
// BCGPPopupWindow.h : header file
//

#include "BCGCBPro.h"

#ifndef BCGP_EXCLUDE_POPUP_WINDOW

#include "BCGPButton.h"
#include "BCGPPopupDlg.h"
#include "BCGPPopupMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGPPopupWndButton window

extern BCGCBPRODLLEXPORT UINT BCGM_ON_CLOSEPOPUPWINDOW;

class BCGCBPRODLLEXPORT CBCGPPopupWndButton : public CBCGPButton
{
	friend class CBCGPPopupWindow;

public:
	CBCGPPopupWndButton ()
	{
		m_bIsCaptionButton = FALSE;
		m_bIsCloseButton = FALSE;
	}

	BOOL IsCaptionButton () const
	{
		return m_bIsCaptionButton;
	}

	BOOL IsCloseButton () const
	{
		return m_bIsCloseButton;
	}

protected:
	virtual void OnFillBackground (CDC* pDC, const CRect& rectClient);
	virtual void OnDrawBorder (CDC* pDC, CRect& rectClient, UINT uiState);
	virtual void OnDraw (CDC* pDC, const CRect& rect, UINT uiState);

	BOOL	m_bIsCaptionButton;
	BOOL	m_bIsCloseButton;
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPPopupWindow window

class BCGCBPRODLLEXPORT CBCGPPopupWindow : public CWnd
{
	friend CBCGPPopupDlg;

	DECLARE_DYNAMIC(CBCGPPopupWindow)

// Construction
public:
	CBCGPPopupWindow();

// Attributes
public:

	CBCGPPopupMenu::ANIMATION_TYPE GetAnimationType ()
	{
		return m_AnimationType;
	}

	void SetAnimationType (CBCGPPopupMenu::ANIMATION_TYPE type)
	{
		m_AnimationType = type;
	}

	void SetAnimationSpeed (UINT nSpeed)
	{
		if (nSpeed == 0 || nSpeed > 200)
		{
			ASSERT (FALSE);
			return;
		}

		m_AnimationSpeed = max (2, nSpeed);
	}

	UINT GetAnimationSpeed () const
	{
		return m_AnimationSpeed;
	}

	void SetTransparency (BYTE nTransparency)
	{
		if (nTransparency < 0 || nTransparency > 255)
		{
			ASSERT (FALSE);
			return;
		}

		m_nTransparency = nTransparency;
	}

	BYTE GetTransparency () const
	{
		return m_nTransparency;
	}

	void SetSmallCaption (BOOL bSmallCaption = TRUE)
	{
		m_bSmallCaption = bSmallCaption;
	}

	BOOL HasSmallCaption () const
	{
		return m_bSmallCaption;
	}

	void SetAutoCloseTime (int nTime)	// ms
	{
		m_nAutoCloseTime = nTime;
	}

	int GetAutoCloseTime () const		// ms
	{
		return m_nAutoCloseTime;
	}

	CPoint GetLastPos () const
	{
		return 	m_ptLastPos;
	}

	CBCGPPopupDlg* GetDlg ()
	{
		return m_pWndDlg;
	}

protected:
	CWnd*				m_pWndOwner;
	BOOL				m_bIsActive;
	UINT				m_uiDlgResID;
	CBCGPPopupDlg*		m_pWndDlg;
	BOOL				m_bHasCloseButton;
	HMENU				m_hMenu;
	int					m_nAutoCloseTime;	// ms
	BOOL				m_bSmallCaption;
	BYTE				m_nTransparency;	// (0..255)

	CBCGPPopupWndButton	m_btnClose;
	CBCGPPopupWndButton	m_btnMenu;

	int					m_nBtnMarginVert;
	int					m_nBtnMarginHorz;

	CPoint				m_ptLastPos;
	BOOL				m_bMoving;
	CPoint				m_ptStartMove;

	//----------------------
	// Animation attributes:
	//----------------------
	CBCGPPopupMenu::ANIMATION_TYPE	m_AnimationType;
	UINT					m_AnimationSpeed;
	UINT					m_nAnimationAlpha;
	BOOL					m_bAnimationIsDone;
	CSize					m_AnimSize;
	CSize					m_FinalSize;
	BOOL					m_bIsAnimRight;
	BOOL					m_bIsAnimDown;
	int						m_iFadePercent;
	int						m_iFadeStep;
	int						m_nAnimStepX;
	int						m_nAnimStepY;
	BOOL					m_bFadeOutAnimation;

	CBitmap					m_bmpScreenSrc;
	CBitmap					m_bmpScreenDst;
	CBitmap					m_bmpScreenTmp;

	COLORREF*				m_cFadeSrcBits;
	COLORREF*				m_cFadeDstBits;
	COLORREF*				m_cFadeTmpBits;

// Operations
public:
	BOOL ProcessCommand (HWND hwnd);

protected:
	CRect GetCaptionRect ();
	void StartAnimation (BOOL bShow = TRUE);
	void DrawAnimation (CDC* pPaintDC);

	BOOL CommonCreate (CPoint ptPos, CBCGPPopupWndParams* pParams = NULL);

	void StartWindowMove ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPPopupWindow)
	public:
	virtual BOOL Create(CWnd* pWndOwner, UINT uiDlgResID, HMENU hMenu = NULL, CPoint ptPos = CPoint (-1, -1), CRuntimeClass* pRTIDlgBar = RUNTIME_CLASS(CBCGPPopupDlg));
	virtual BOOL Create(CWnd* pWndOwner, CBCGPPopupWndParams& params, HMENU hMenu = NULL, CPoint ptPos = CPoint (-1, -1));
	//}}AFX_VIRTUAL
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	virtual CSize GetDialogSize ();
	virtual BOOL OnBeforeShow (CPoint& /*ptPos*/)	{	return TRUE;	}
	virtual void OnDraw (CDC* pDC);
	virtual int GetCaptionHeight ();

	virtual BOOL OnClickLinkButton (UINT /*uiCmdID*/)	{	return FALSE;	}

// Implementation
public:
	virtual ~CBCGPPopupWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPPopupWindow)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnPrintClient(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

#endif // BCGP_EXCLUDE_POPUP_WINDOW

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPPOPUPWINDOW_H__4DE5ED2B_6089_4A54_BF22_113C2C5FA4C0__INCLUDED_)
