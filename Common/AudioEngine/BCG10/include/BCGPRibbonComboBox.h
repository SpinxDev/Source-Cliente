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
// BCGPRibbonComboBox.h: interface for the CBCGPRibbonComboBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPRIBBONCOMBOBOX_H__27A9CEFE_1C67_4345_B4EB_DD1E3FA5AD19__INCLUDED_)
#define AFX_BCGPRIBBONCOMBOBOX_H__27A9CEFE_1C67_4345_B4EB_DD1E3FA5AD19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef BCGP_EXCLUDE_RIBBON

#include "BCGPRibbonEdit.h"
#include "BCGPToolbarFontCombo.h"

class CBCGPRibbonEditCtrl;

/////////////////////////////////////////////////////////////////////////////
// CBCGPRibbonComboBox

class BCGCBPRODLLEXPORT CBCGPRibbonComboBox : public CBCGPRibbonEdit  
{
	friend class CBCGPRibbonEditCtrl;
	friend class CBCGPDropDownList;

	DECLARE_DYNCREATE(CBCGPRibbonComboBox)

// Construction:
public:
	CBCGPRibbonComboBox (UINT nID, BOOL bHasEditBox = TRUE, int nWidth = -1,
		LPCTSTR lpszLabel = NULL, int nImage = -1);
	virtual ~CBCGPRibbonComboBox();

protected:
	CBCGPRibbonComboBox();

// Attributes:
public:
	void EnableDropDownListResize (BOOL bEnable = FALSE)
	{
		m_bResizeDropDownList = bEnable;
	}

	BOOL IsResizeDropDownList () const
	{
		return m_bResizeDropDownList;
	}

// Operations:
public:
	virtual INT_PTR AddItem (LPCTSTR lpszItem, DWORD_PTR dwData = 0);
	INT_PTR GetCount () const
	{
		return m_lstItems.GetCount ();
	}

	LPCTSTR GetItem (int iIndex) const;
	DWORD_PTR GetItemData (int iIndex) const;

	int GetCurSel () const
	{
		return m_iSelIndex;
	}

	void RemoveAllItems ();

	BOOL SelectItem (int iIndex);
	BOOL SelectItem (DWORD_PTR dwData);
	BOOL SelectItem (LPCTSTR lpszText);

	BOOL DeleteItem (int iIndex);
	BOOL DeleteItem (DWORD_PTR dwData);
	BOOL DeleteItem (LPCTSTR lpszText);

	int FindItem (LPCTSTR lpszText) const;

	int GetDropDownHeight () const
	{
		return m_nDropDownHeight;
	}
	void SetDropDownHeight (int nHeight);

	BOOL HasEditBox () const
	{
		return m_bHasEditBox;
	}

// Overrides
public:
	virtual CSize GetIntermediateSize (CDC* pDC);
	virtual void OnSelectItem (int nItem);

protected:
	virtual void OnDraw (CDC* pDC);

	virtual void OnLButtonDown (CPoint point);
	virtual void OnLButtonUp (CPoint point);

	virtual void CopyFrom (const CBCGPBaseRibbonElement& src);
	virtual BOOL HasMenu () const
	{
		return TRUE;
	}

	virtual BOOL CanBeStretched ()
	{
		return FALSE;
	}

	virtual void OnAfterChangeRect (CDC* pDC);

	virtual void DropDownList ();
	void CommonInit ();

	virtual void ClearData ();

	virtual BOOL OnDrawDropListItem (CDC* pDC, int nIndex, CBCGPToolbarMenuButton* pItem, 
									BOOL bHighlight);
	virtual CSize OnGetDropListItemSize (CDC* pDC, int nIndex, CBCGPToolbarMenuButton* pItem,
									CSize sizeDefault);

// Attributes:
protected:
	CStringList					m_lstItems;
	CList<DWORD_PTR, DWORD_PTR>	m_lstItemData;
	int							m_iSelIndex;
	
	BOOL						m_bHasEditBox;
	int							m_nDropDownHeight;

	BOOL						m_bResizeDropDownList;
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPRibbonFontComboBox

class BCGCBPRODLLEXPORT CBCGPRibbonFontComboBox : public CBCGPRibbonComboBox  
{
	DECLARE_DYNCREATE(CBCGPRibbonFontComboBox)

// Construction:
public:
	CBCGPRibbonFontComboBox (
			UINT nID, 
			int nFontType = DEVICE_FONTTYPE | RASTER_FONTTYPE | TRUETYPE_FONTTYPE,
			BYTE nCharSet = DEFAULT_CHARSET,
			BYTE nPitchAndFamily = DEFAULT_PITCH,
			int nWidth = -1);

	virtual ~CBCGPRibbonFontComboBox();

protected:
	CBCGPRibbonFontComboBox ();

// Operations:
public:
	BOOL SetFont (LPCTSTR lpszName, BYTE nCharSet = DEFAULT_CHARSET, BOOL bExact = FALSE);
	const CBCGPFontDesc* GetFontDesc (int iIndex = -1) const
	{
		return (CBCGPFontDesc*) GetItemData (iIndex);
	}

	void BuildFonts (
			int nFontType = DEVICE_FONTTYPE | RASTER_FONTTYPE | TRUETYPE_FONTTYPE,
			BYTE nCharSet = DEFAULT_CHARSET,
			BYTE nPitchAndFamily = DEFAULT_PITCH);
	void RebuildFonts ();

protected:
	int GetFontsCount (LPCTSTR lpszName, const CObList& lstFonts);

// Overrides
protected:
	virtual void CopyFrom (const CBCGPBaseRibbonElement& src);
	virtual void ClearData ();
	virtual void DropDownList ();

	virtual BOOL OnDrawDropListItem (CDC* pDC, int nIndex, CBCGPToolbarMenuButton* pItem, 
									BOOL bHighlight);
	virtual CSize OnGetDropListItemSize (CDC* pDC, int nIndex, CBCGPToolbarMenuButton* pItem,
									CSize sizeDefault);
// Attributes:
public:
	static BOOL	m_bDrawUsingFont;

protected:
	int			m_nFontType;
	BYTE		m_nCharSet;
	BYTE		m_nPitchAndFamily;

	CImageList	m_Images;
};

#endif // BCGP_EXCLUDE_RIBBON

#endif // !defined(AFX_BCGPRIBBONCOMBOBOX_H__27A9CEFE_1C67_4345_B4EB_DD1E3FA5AD19__INCLUDED_)
