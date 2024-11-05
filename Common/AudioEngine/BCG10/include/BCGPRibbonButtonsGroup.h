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
// BCGPRibbonButtonsGroup.h: interface for the CBCGPRibbonButtonsGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPRIBBONBUTTONSGROUP_H__4052EF58_937E_4371_BEED_65659F35FD28__INCLUDED_)
#define AFX_BCGPRIBBONBUTTONSGROUP_H__4052EF58_937E_4371_BEED_65659F35FD28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bcgcbpro.h"

#ifndef BCGP_EXCLUDE_RIBBON

#include "BCGPBaseRibbonElement.h"
#include "BCGPToolBarImages.h"

class BCGCBPRODLLEXPORT CBCGPRibbonButtonsGroup : public CBCGPBaseRibbonElement  
{
	friend class CBCGPRibbonBar;
	friend class CBCGPRibbonStatusBar;

	DECLARE_DYNCREATE(CBCGPRibbonButtonsGroup)

// Construction
public:
	CBCGPRibbonButtonsGroup();
	CBCGPRibbonButtonsGroup(CBCGPBaseRibbonElement* pButton);

	virtual ~CBCGPRibbonButtonsGroup();

// Attributes
public:
	void SetImages (CBCGPToolBarImages* pImages,
					CBCGPToolBarImages* pHotImages,
					CBCGPToolBarImages* pDisabledImages);

	BOOL HasImages () const
	{
		return m_Images.GetCount () > 0;
	}

	const CSize GetImageSize () const;

	int GetCount () const
	{
		return (int) m_arButtons.GetSize ();
	}

	CBCGPBaseRibbonElement* GetButton (int i) const
	{
		ASSERT_VALID (m_arButtons [i]);
		return m_arButtons [i];
	}

// Operations
public:
	void AddButton (CBCGPBaseRibbonElement* pButton);
	void AddButtons (
		const CList<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& lstButtons);

	void RemoveAll ();

// Overrides
public:
	virtual CSize GetRegularSize (CDC* pDC);

	virtual void OnDrawImage (CDC* pDC, CRect rectImage, 
		CBCGPBaseRibbonElement* pButton, int nImageIndex);

	virtual void SetParentCategory (CBCGPRibbonCategory* pCategory);

protected:
	virtual void OnDraw (CDC* pDC);
	virtual void OnUpdateCmdUI (CBCGPRibbonCmdUI* pCmdUI, CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void OnAfterChangeRect (CDC* pDC);
	virtual void OnShow (BOOL bShow);

	virtual CBCGPBaseRibbonElement* HitTest (CPoint point);

	virtual BOOL CanBeStretched ()
	{
		return FALSE;
	}

	virtual BOOL IsAlignByColumn () const
	{
		return FALSE;
	}

	virtual BOOL IsQAT () const
	{
		return FALSE;
	}

	virtual CBCGPBaseRibbonElement* Find (const CBCGPBaseRibbonElement* pElement);
	virtual CBCGPBaseRibbonElement* FindByID (UINT uiCmdID);
	virtual CBCGPBaseRibbonElement* FindByData (DWORD_PTR dwData);
	virtual CBCGPBaseRibbonElement* FindByOriginal (CBCGPBaseRibbonElement* pOriginal);
	virtual CBCGPBaseRibbonElement* GetPressed ();
	virtual CBCGPBaseRibbonElement* GetDroppedDown ();
	virtual CBCGPBaseRibbonElement* GetHighlighted ();
	virtual CBCGPBaseRibbonElement* GetFocused ();

	virtual BOOL ReplaceByID (UINT uiCmdID, CBCGPBaseRibbonElement* pElem);
	virtual void CopyFrom (const CBCGPBaseRibbonElement& src);
	virtual void SetParentMenu (CBCGPRibbonPanelMenuBar* pMenuBar);
	virtual void SetOriginal (CBCGPBaseRibbonElement* pOriginal);

	virtual void GetElementsByID (UINT uiCmdID, 
		CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arButtons);

	virtual void GetVisibleElements (
		CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arButtons);

	virtual void GetItemIDsList (CList<UINT,UINT>& lstItems) const;

	virtual int AddToListBox (CBCGPRibbonCommandsListBox* pWndListBox, BOOL bDeep);
	virtual void AddToKeyList (CArray<CBCGPRibbonKeyTip*,CBCGPRibbonKeyTip*>& arElems);

	virtual void OnRTLChanged(BOOL bIsRTL);

	virtual void CleanUpSizes ();

	virtual void SetParentRibbonBar (CBCGPRibbonBar* pRibbonBar);

	virtual CBCGPBaseRibbonElement* GetFirstTabStop ();
	virtual CBCGPBaseRibbonElement* GetLastTabStop ();

// Attributes
protected:
	CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>	m_arButtons;

	CBCGPToolBarImages		m_Images;
	CBCGPToolBarImages		m_HotImages;
	CBCGPToolBarImages		m_DisabledImages;
};

#endif // BCGP_EXCLUDE_RIBBON

#endif // !defined(AFX_BCGPRIBBONBUTTONSGROUP_H__4052EF58_937E_4371_BEED_65659F35FD28__INCLUDED_)
