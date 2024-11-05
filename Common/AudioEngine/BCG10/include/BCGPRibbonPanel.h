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
// BCGPRibbonPanel.h: interface for the BCGPRibbonPanel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPRIBBONPANEL_H__6B396E69_CBB1_42C9_9FF1_A627BB025992__INCLUDED_)
#define AFX_BCGPRIBBONPANEL_H__6B396E69_CBB1_42C9_9FF1_A627BB025992__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGCBPro.h"

#ifndef BCGP_EXCLUDE_RIBBON

#include "bcgglobals.h"
#include "BCGPRibbonButton.h"

class CBCGPRibbonCategory;
class CBCGPBaseRibbonElement;
class CBCGPRibbonPanel;
class CBCGPRibbonPaletteButton;
class CBCGPRibbonPaletteIcon;

//////////////////////////////////////////////////////////////////////
// CBCGPRibbonLaunchButton

class BCGCBPRODLLEXPORT CBCGPRibbonLaunchButton : public CBCGPRibbonButton
{
	DECLARE_DYNCREATE(CBCGPRibbonLaunchButton)

	friend class CBCGPRibbonPanel;

	CBCGPRibbonLaunchButton ();

	virtual void OnDraw (CDC* pDC);
	virtual CSize GetRegularSize (CDC* pDC);
	virtual void OnClick (CPoint point);
	virtual CRect GetKeyTipRect (CDC* pDC, BOOL bIsMenu);
	virtual CBCGPRibbonPanel* GetParentPanel () const
	{
		return m_pParentPanel;
	}

	CBCGPRibbonPanel*	m_pParentPanel;
};

//////////////////////////////////////////////////////////////////////
// CBCGPRibbonDefaultPanelButton

class BCGCBPRODLLEXPORT CBCGPRibbonDefaultPanelButton : public CBCGPRibbonButton
{
	DECLARE_DYNCREATE(CBCGPRibbonDefaultPanelButton)

	friend class CBCGPRibbonPanel;
	friend class CBCGPRibbonCategory;

	CBCGPRibbonDefaultPanelButton (CBCGPRibbonPanel* pPanel = NULL);
	virtual ~CBCGPRibbonDefaultPanelButton();

	virtual void OnDraw (CDC* pDC);
	virtual void OnLButtonDown (CPoint point);
	virtual void OnShowPopupMenu ();
	
	virtual CString GetToolTipText () const
	{
		return m_rect.IsRectEmpty () ? _T("") : m_strText;
	}

	virtual void OnClick (CPoint /*point*/)
	{
		OnShowPopupMenu ();
	}

	virtual CSize GetImageSize (RibbonImageType /*type*/) const
	{
		CSize sizeIcon (16, 16);

		if (globalData.GetRibbonImageScale () != 1.)
		{
			sizeIcon.cx = (int) (.5 + globalData.GetRibbonImageScale () * sizeIcon.cx);
			sizeIcon.cy = (int) (.5 + globalData.GetRibbonImageScale () * sizeIcon.cy);
		}

		return sizeIcon;
	}

	virtual BOOL IsDefaultPanelButton () const
	{
		return TRUE;
	}

	virtual void DrawImage (CDC* pDC, RibbonImageType type, CRect rectImage);
	virtual BOOL CanBeAddedToQAT () const
	{
		return TRUE;
	}

	virtual void CopyFrom (const CBCGPBaseRibbonElement& s);
	virtual void OnDrawOnList (CDC* pDC, CString strText, int nTextOffset, CRect rect, BOOL bIsSelected, BOOL bHighlighted);
	virtual BOOL OnKey (BOOL bIsMenuKey);
	virtual CBCGPRibbonPanel* GetParentPanel () const
	{
		return m_pPanel;
	}

	virtual BOOL SetACCData (CWnd* pParent, CBCGPAccessibilityData& data);

	HICON				m_hIcon;
	CBCGPRibbonPanel*	m_pPanel;
};

//////////////////////////////////////////////////////////////////////
// CBCGPRibbonPanel

class BCGCBPRODLLEXPORT CBCGPRibbonPanel : public CObject  
{
	DECLARE_DYNCREATE(CBCGPRibbonPanel)

	friend class CBCGPRibbonCategory;
	friend class CBCGPRibbonPanelMenuBar;
	friend class CBCGPBaseRibbonElement;
	friend class CBCGPRibbonFloaty;
	friend class CBCGPRibbonDefaultPanelButton;
	friend class CBCGPRibbonPanelMenu;
	friend class CBCGPRibbonBar;

// Construction
protected:
	CBCGPRibbonPanel(LPCTSTR lpszName = NULL, HICON hIcon = NULL, BOOL bAutoDestroyIcon = TRUE);
	CBCGPRibbonPanel(CBCGPRibbonPaletteButton* pPaletteButton);

	virtual void CopyFrom (CBCGPRibbonPanel& src);

	void CommonInit (LPCTSTR lpszName = NULL, HICON hIcon = NULL, BOOL bAutoDestroyIcon = TRUE);

public:
	virtual ~CBCGPRibbonPanel();

// Attributes:
public:
	LPCTSTR GetName () const
	{
		return m_strName;
	}

	CBCGPRibbonButton& GetDefaultButton ()
	{
		return m_btnDefault;
	}

	void EnableLaunchButton (UINT uiCmdID, int nIconIndex = -1, LPCTSTR lpszKeys = NULL);

	CBCGPRibbonButton& GetLaunchButton ()
	{
		return m_btnLaunch;
	}

	BOOL IsHighlighted () const
	{
		return m_bIsHighlighted;
	}

	BOOL IsCollapsed () const;

	BOOL IsMenuMode () const
	{
		return m_bMenuMode;
	}

	virtual BOOL IsMainPanel () const
	{
		return FALSE;
	}

	const CRect& GetRect () const
	{
		return m_rect;
	}

	DWORD_PTR GetData () const
	{
		return m_dwData;
	}

	void SetData (DWORD_PTR dwData)
	{
		m_dwData = dwData;
	}

	CBCGPRibbonCategory* GetParentCategory () const
	{
		return m_pParent;
	}

	CBCGPRibbonPanelMenuBar* GetParentMenuBar () const
	{
		return m_pParentMenuBar;
	}

	CBCGPBaseRibbonElement* GetParentButton () const;

	BOOL IsJustifyColumns () const
	{
		return m_bJustifyColumns;
	}

	void SetJustifyColumns (BOOL bSet = TRUE)
	{
		m_bJustifyColumns = bSet;
	}

	void SetKeys (LPCTSTR lpszKeys);

	int GetCaptionHeight () const
	{
		return m_rectCaption.Height ();
	}

	BOOL IsCenterColumnVert () const
	{
		return m_bCenterColumnVert;
	}

	void SetCenterColumnVert (BOOL bSet = TRUE)
	{
		m_bCenterColumnVert = bSet;
	}

	BOOL IsPreserveElementOrder () const
	{
		return m_bPreserveElementOrder;
	}

	void SetPreserveElementOrder (BOOL bSet = TRUE)
	{
		m_bPreserveElementOrder = bSet;
	}

// Operations
public:
	virtual CBCGPBaseRibbonElement* HitTest (CPoint point, BOOL bCheckPanelCaption = FALSE);
	virtual int HitTestEx (CPoint point);
	virtual int GetIndex (CBCGPBaseRibbonElement* pElem);
	virtual BOOL GetPreferedMenuLocation (CRect& /*rect*/)	{	return FALSE;	}

	BOOL HasElement (const CBCGPBaseRibbonElement* pElem) const;

	virtual void RecalcWidths (CDC* pDC, int nHeight);

	virtual void Add (CBCGPBaseRibbonElement* pElem);
	virtual void AddSeparator ();

	virtual BOOL Insert (CBCGPBaseRibbonElement* pElem, int nIndex);
	virtual BOOL InsertSeparator (int nIndex);

	CBCGPRibbonButtonsGroup* AddToolBar (
							UINT uiToolbarResID, UINT uiColdResID = 0, 
							UINT uiHotResID = 0, UINT uiDisabledResID = 0);

	CBCGPBaseRibbonElement* GetElement (int nIndex) const;
	CBCGPBaseRibbonElement* FindByID (UINT uiCmdID) const;
	CBCGPBaseRibbonElement* FindByData (DWORD_PTR dwData) const;

	BOOL SetElementMenu (	UINT uiCmdID, HMENU hMenu, 
							BOOL bIsDefautCommand = FALSE, 
							BOOL bRightAlign = FALSE);

	BOOL SetElementMenu (	UINT uiCmdID, UINT uiMenuResID,
							BOOL bIsDefautCommand = FALSE, 
							BOOL bRightAlign = FALSE);

	BOOL Replace (int nIndex, CBCGPBaseRibbonElement* pElem);
	BOOL ReplaceByID (UINT uiCmdID, CBCGPBaseRibbonElement* pElem);

	CBCGPBaseRibbonElement* SetElementRTC (int nIndex, CRuntimeClass* pRTC);
	CBCGPBaseRibbonElement* SetElementRTCByID (UINT uiCmdID, CRuntimeClass* pRTC);

	int GetCount () const;

	BOOL Remove (int nIndex, BOOL bDelete = TRUE);
	void RemoveAll ();

	void GetElements (
		CArray <CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arElements);

	void GetItemIDsList (CList<UINT,UINT>& lstItems) const;

	void GetElementsByID (UINT uiCmdID, 
		CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arElements);

	void GetVisibleElements (
		CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arElements);

	CBCGPBaseRibbonElement* GetDroppedDown () const;
	CBCGPBaseRibbonElement* GetHighlighted () const;
	CBCGPBaseRibbonElement* GetPressed () const;
	CBCGPBaseRibbonElement* GetFocused () const;

	void SetFocused (CBCGPBaseRibbonElement* pNewFocus);

	CBCGPRibbonPanelMenu* ShowPopup (CBCGPRibbonDefaultPanelButton* pButton = NULL);

	void MakePaletteItemVisible (CBCGPBaseRibbonElement* pItem);
	CRect GetPaletteRect ();

// Overrides
public:
	virtual BOOL OnKey (UINT nChar);

protected:
	virtual int GetHeight (CDC* pDC) const;

	virtual BOOL IsFixedSize () const
	{
		return FALSE;
	}

	virtual void DoPaint (CDC* pDC);
	virtual void Repos (CDC* pDC, const CRect& rect);
	virtual void ReposMenu (CDC* pDC, const CRect& rect);
	virtual void ReposPalette (CDC* pDC, const CRect& rect);
	virtual void OnUpdateCmdUI (CBCGPRibbonCmdUI* pCmdUI, CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL NotifyControlCommand (BOOL bAccelerator, int nNotifyCode, WPARAM wParam, LPARAM lParam);
	virtual void OnAfterChangeRect (CDC* pDC);
	virtual void OnShow (BOOL bShow);
	virtual void RedrawElement (CBCGPBaseRibbonElement* pElem);
	virtual void OnDrawMenuBorder (CDC* /*pDC*/, CBCGPRibbonPanelMenuBar* /*pMenuBar*/)	{}
	virtual void OnRTLChanged (BOOL bIsRTL);

// Operations
protected:
	int GetMinWidth (CDC* pDC);
	void CenterElementsInColumn (int nFirstInColumnIndex, int nLastInColumnIndex, int nCaptionHeight);
	void JustifyElementsInColumn (int nFirstInColumnIndex, int nLastInColumnIndex);

	CSize GetCaptionSize (CDC* pDC) const;
	int CalcTotalWidth ();

	void ShowDefaultButton (CDC* pDC);

	virtual void Highlight (BOOL bHighlight, CPoint point);
	virtual CBCGPBaseRibbonElement* MouseButtonDown (CPoint point);
	virtual void MouseButtonUp (CPoint point);
	void CancelMode ();

	CWnd* GetParentWnd () const;

	CBCGPBaseRibbonElement* GetFirstTabStop () const;
	CBCGPBaseRibbonElement* GetLastTabStop () const;

	void CleanUpSizes ();
	void OnDrawPaletteMenu (CDC* pDC);
	void ScrollPalette (int nScrollOffset, BOOL bIsDelta = FALSE);
	CSize GetPaletteMinSize () const;

// Attributes
protected:
	CString					m_strName;			// Panel name
	CRect					m_rect;				// Panel location
	DWORD_PTR				m_dwData;			// User-defined data
	CBCGPRibbonCategory*	m_pParent;			// Parent category
	CBCGPRibbonPanelMenuBar*
							m_pParentMenuBar;	// Parent menu bar
	CArray<int,int>			m_arWidths;			// All possible widthds
	CMap<int,int,int,int>	m_mapNonOptWidths;	// Optimized Width <-> Full Width
	int						m_nCurrWidthIndex;	// Index of the current width
	int						m_nFullWidth;		// Full width
	int						m_nRows;			// Rows number (m_bAlignByColumn == FALSE)
	int						m_nXMargin;
	int						m_nYMargin;
	CBCGPRibbonDefaultPanelButton		
							m_btnDefault;		// Default panel button
	CBCGPRibbonLaunchButton	m_btnLaunch;
	BOOL					m_bShowCaption;
	BOOL					m_bForceCollpapse;
	BOOL					m_bIsHighlighted;
	BOOL					m_bIsCalcWidth;
	CBCGPBaseRibbonElement*	m_pHighlighted;		// Highlighted 
	BOOL					m_bAlignByColumn;
	BOOL					m_bCenterColumnVert;
	BOOL					m_bMenuMode;
	BOOL					m_bIsDefaultMenuLook;
	BOOL					m_bFloatyMode;
	BOOL					m_bIsQATPopup;
	CRect					m_rectCaption;
	BOOL					m_bJustifyColumns;
	
	CBCGPRibbonPaletteButton*	m_pPaletteButton;
	CRect					m_rectMenuAreaTop;
	CRect					m_rectMenuAreaBottom;
	CScrollBar*				m_pScrollBar;
	int						m_nScrollOffset;
	BOOL					m_bSizeIsLocked;
	BOOL					m_bScrollDnAvailable;
	BOOL					m_bTrancateCaption;
	BOOL					m_bPreserveElementOrder;

	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>	m_arElements;

	static UINT				m_nNextPanelID;
};

#endif // BCGP_EXCLUDE_RIBBON

#endif // !defined(AFX_BCGPRIBBONPANEL_H__6B396E69_CBB1_42C9_9FF1_A627BB025992__INCLUDED_)
