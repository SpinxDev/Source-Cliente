#if !defined(AFX_BCGPGRIDCTRL_H__573C359C_D1FB_446F_B60D_B5E331228237__INCLUDED_)
#define AFX_BCGPGRIDCTRL_H__573C359C_D1FB_446F_B60D_B5E331228237__INCLUDED_

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
// BCGPGridCtrl.h : header file
//

#ifndef __AFXTEMPL_H__
	#include "afxtempl.h"
#endif

#include "comdef.h"
#include "BCGCBPro.h"

#ifndef BCGP_EXCLUDE_GRID_CTRL

#include "BCGGlobals.h"

#ifndef _BCGPGRID_STANDALONE
	#include "BCGPScrollBar.h"
#if (!defined _BCGSUITE_) && (!defined _BCGSUITE_INC_)
	#include "ColorPopup.h"
#endif
	#include "BCGPDateTimeCtrl.h"
#endif

#include "BCGPInplaceToolTipCtrl.h"

// Selection modes:
#define SM_NONE					0x0000
#define SM_ALL					0x0001
#define SM_ROW					0x0002
#define SM_COLUMN				0x0004
#define SM_SINGLE_ITEM			0x0008
#define SM_SET_ACTIVE_ITEM		0x0010
#define SM_FIRST_CLICK			0x0020
#define SM_SECOND_CLICK			0x0040
#define SM_SINGE_SEL_GROUP		0x0080
#define SM_CONTINUE_SEL_GROUP	0x0100
#define SM_ADD_SEL_GROUP		0x0200
#define SM_INVERT_SEL			0x0400

// Header flags:
#define BCGP_GRID_HEADER_MOVE_ITEMS	0x0001
#define BCGP_GRID_HEADER_HIDE_ITEMS	0x0002
#define BCGP_GRID_HEADER_SORT		0x0004

// Grid item flags:
#define BCGP_GRID_ITEM_HAS_LIST		0x0001
#define BCGP_GRID_ITEM_HAS_BUTTON	0x0002
#define BCGP_GRID_ITEM_HAS_SPIN		0x0004
#define BCGP_GRID_ITEM_VTOP			0x0010
#define BCGP_GRID_ITEM_VCENTER		0x0020
#define BCGP_GRID_ITEM_VBOTTOM		0x0040
#define BCGP_GRID_ITEM_AUTOGROUP	0x0100
#define BCGP_GRID_ITEM_MULTILINE	0x0200
#define BCGP_GRID_ITEM_WORDWRAP		0x0400

class CBCGPGridItem;

/////////////////////////////////////////////////////////////////////////////
// BCGPGRID_ITEM_INFO struct, used for BCGM_GRID_ITEM_CHANGED message

struct BCGCBPRODLLEXPORT BCGPGRID_ITEM_INFO
{
	CBCGPGridItem* pItem;	// An item
    int nRow;				// Row of an item
	int nCol;				// Column of an item
};

/////////////////////////////////////////////////////////////////////////////
//

struct BCGCBPRODLLEXPORT BCGPGRID_NOTIFICATION	// used for BCGPGN_SELCHANGING, BCGPGN_SELCHANGED
{
	NMHDR	hdr;
    int		nRow;				// Row of the first item
	int		nCol;				// Column of the first item
    int		nRowTo;				// Row of the last item
	int		nColTo;				// Column of the last item
	LPARAM	lParam;				// Message specific
};

struct BCGCBPRODLLEXPORT BCGPGRID_ITEM	// used for BCGPGN_GETDISPINFO
{
    int nRow;				// Row of an item
	int nCol;				// Column of an item

    _variant_t	varValue;         // Item value
	int			iImage;			  // Item image index
    DWORD_PTR	dwData;           // User-defined data

	COLORREF	clrBackground;    // Background color
	COLORREF	clrText;          // Text color
    LOGFONT		lfFont;           // Cell font
};

struct BCGCBPRODLLEXPORT BCGPGRID_DISPINFO
{
	NMHDR				hdr;
	BCGPGRID_ITEM		item;
};

// Notification messages
#define BCGPGN_SELCHANGING		LVN_ITEMCHANGING
#define BCGPGN_SELCHANGED		LVN_ITEMCHANGED
#define BCGPGN_GETDISPINFO		LVN_GETDISPINFO 

typedef BOOL (CALLBACK* BCGPGRID_CALLBACK)(BCGPGRID_DISPINFO*, LPARAM);

// Filter
typedef LRESULT (CALLBACK* BCGPGRID_FILTERCALLBACK)(WPARAM wParam, LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridItemID object

class BCGCBPRODLLEXPORT CBCGPGridItemID
{
public:
	CBCGPGridItemID(int nRow = -1, int nColumn = -1)
		: m_nRow (nRow), m_nColumn (nColumn) 
	{
	}

	BOOL IsNull () const
	{
		return (m_nRow < 0 && m_nColumn < 0);
	}

	void SetNull ()
	{
		m_nRow = -1;
		m_nColumn = -1;
	}

	BOOL IsRow () const
	{
		return (m_nRow >= 0 && m_nColumn == -1);
	}

	BOOL IsColumn () const
	{
		return (m_nRow == -1 && m_nColumn >= 0);
	}

	CBCGPGridItemID& operator=(const CBCGPGridItemID& rhs)
	{
		m_nRow = rhs.m_nRow;
		m_nColumn = rhs.m_nColumn;
		return *this;
	}

	int operator==(const CBCGPGridItemID& rhs) const
	{
		return (m_nRow == rhs.m_nRow && m_nColumn == rhs.m_nColumn);
	}
    
	int operator!=(const CBCGPGridItemID& rhs) const 
	{
		return !operator==(rhs);
	}

// Attributes
	int	m_nRow;
	int	m_nColumn;
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridRange object

class BCGCBPRODLLEXPORT CBCGPGridRange
{
public:
    CBCGPGridRange (int nLeft = -1, int nTop = -1, int nRight = -1, int nBottom = -1)
    {
        Set (nLeft, nTop, nRight, nBottom);
    }

	CBCGPGridRange (const CBCGPGridItemID &id)
	{
        Set (id.m_nColumn, id.m_nRow, id.m_nColumn, id.m_nRow);
	}

	CBCGPGridRange (const CBCGPGridItemID &id1, const CBCGPGridItemID &id2)
	{
        Set (id1, id2);
	}

	void Set (int nLeft, int nTop, int nRight, int nBottom)
	{
		m_nLeft = nLeft; m_nTop = nTop; m_nRight = nRight; m_nBottom = nBottom;
	}

	void Set (const CBCGPGridItemID &id1, const CBCGPGridItemID &id2)
	{
		ASSERT (id1.m_nColumn >= 0);
		ASSERT (id1.m_nRow >= 0);
		ASSERT (id2.m_nColumn >= 0);
		ASSERT (id2.m_nRow >= 0);

		m_nLeft = min (id1.m_nColumn, id2.m_nColumn);
		m_nTop = min (id1.m_nRow, id2.m_nRow);
		m_nRight = max (id1.m_nColumn, id2.m_nColumn);
		m_nBottom = max (id1.m_nRow, id2.m_nRow);
	}

	void Set (const CBCGPGridRange &src)
	{
		m_nLeft = src.m_nLeft;
		m_nTop = src.m_nTop;
		m_nRight = src.m_nRight;
		m_nBottom = src.m_nBottom;
	}

    BOOL IsInRange (int nRow, int nColumn) const
	{
		return (m_nTop <= nRow && m_nBottom >= nRow &&
			m_nLeft <= nColumn && m_nRight >= nColumn);
	}

	BOOL IsInRange (const CBCGPGridItemID &id) const
	{
		return IsInRange (id.m_nRow, id.m_nColumn);
	}

    int GetCount () const
	{
		return (m_nBottom - m_nTop + 1) * (m_nRight - m_nLeft + 1);
	}

	BOOL IsEmpty () const
	{
		return !(m_nLeft >= 0 && m_nTop >= 0 && 
				m_nRight >= 0 && m_nBottom >= 0);
	}

	BOOL IsValid () const
	{
		return (m_nLeft >= 0 && m_nTop >= 0 && 
				m_nRight >= m_nLeft && m_nBottom >= m_nTop);
	}

	CBCGPGridItemID GetSecondId (const CBCGPGridItemID &id) const
	{
		if (!IsValid() || !IsInRange (id))
		{
			CBCGPGridItemID id2;
			return id2;
		}

		CBCGPGridItemID id2 (
			(id.m_nRow != m_nTop) ? m_nTop : m_nBottom,
			(id.m_nColumn != m_nLeft) ? m_nLeft : m_nRight);
		return id2;
	}

	int operator==(const CBCGPGridRange& rhs) const
	{
		return (m_nTop == rhs.m_nTop && m_nBottom == rhs.m_nBottom &&
			m_nLeft == rhs.m_nLeft && m_nRight == rhs.m_nRight);
	}
    
	int operator!=(const CBCGPGridRange& rhs) const 
	{
		return !operator==(rhs);
	}
	
public:
	int	m_nLeft;
	int	m_nTop;
	int	m_nRight;
	int	m_nBottom;
};

/////////////////////////////////////////////////////////////////////////////
// Grid custom colors

struct BCGCBPRODLLEXPORT BCGP_GRID_COLOR_DATA
{
	struct BCGCBPRODLLEXPORT ColorData
	{
		COLORREF	m_clrBackground;
		COLORREF	m_clrText;
		COLORREF	m_clrGradient;
		int			m_nGradientAngle;	// 0 - 360
		COLORREF	m_clrBorder;

		void InitColors ()
		{
			m_clrBackground	= (COLORREF)-1;
			m_clrText		= (COLORREF)-1;
			m_clrGradient	= (COLORREF)-1;
			m_nGradientAngle= 90;
			m_clrBorder		= (COLORREF)-1;
		}

		BOOL Draw (CDC* pDC, CRect rect, BOOL bNoBorder = FALSE);
	};

	COLORREF	m_clrBackground;	// Grid background color
	COLORREF	m_clrText;			// Grid foreground color
	COLORREF	m_clrHeader;		// H foreground color

	ColorData	m_EvenColors;		// Even rows color
	ColorData	m_OddColors;		// Odd rows color

	ColorData	m_GroupColors;		// Group colors
	ColorData	m_GroupSelColors;	// Selected group colors

	ColorData	m_SelColors;		// Selected rows color

	ColorData	m_HeaderColors;		// Header item color
	ColorData	m_HeaderSelColors;	// Selected header item color

	ColorData	m_LeftOffsetColors;	// Left offset colors
	
	COLORREF	m_clrHorzLine;		// Grid horizontal line text
	COLORREF	m_clrVertLine;		// Grid vertical line text

	COLORREF	m_clrPreviewText;	// Description text color
};

class BCGCBPRODLLEXPORT CBCGPGridColors : public BCGP_GRID_COLOR_DATA
{
public:
	CBCGPGridColors ()
	{
		m_clrBackground		= (COLORREF)-1;
		m_clrText			= (COLORREF)-1;
		m_clrHeader			= (COLORREF)-1;

		m_EvenColors.InitColors ();
		m_OddColors.InitColors ();

		m_GroupColors.InitColors ();
		m_GroupSelColors.InitColors ();

		m_SelColors.InitColors ();

		m_HeaderColors.InitColors ();
		m_HeaderSelColors.InitColors ();

		m_LeftOffsetColors.InitColors ();

		m_clrHorzLine		= (COLORREF)-1;
		m_clrVertLine		= (COLORREF)-1;

		m_clrPreviewText	= (COLORREF)-1;
	}

	CBCGPGridColors (const BCGP_GRID_COLOR_DATA& src)
	{
		(BCGP_GRID_COLOR_DATA&)*this = src;
	}

	void operator=(const BCGP_GRID_COLOR_DATA& src)
	{
		(BCGP_GRID_COLOR_DATA&)*this = src;
	}
};

/////////////////////////////////////////////////////////////////////////////
// Grid borders

#define GRID_BORDERREF	int

struct GRID_BORDERS
{
	GRID_BORDERREF	top;
	GRID_BORDERREF	bottom;
	GRID_BORDERREF	left;
	GRID_BORDERREF	right;
};

#define GRID_BORDERSTYLE_EMPTY		0
#define GRID_BORDERSTYLE_DEFAULT	1

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridRow object

class BCGCBPRODLLEXPORT CBCGPGridRow : public CObject
{
	DECLARE_DYNAMIC(CBCGPGridRow)

	friend class CBCGPGridCtrl;
	friend class CBCGPGridItem;

// Construction
public:
	// Group constructor
	CBCGPGridRow (const CString& strGroupName, DWORD_PTR dwData = 0, int nBlockSize = 10);

	// Simple item
	CBCGPGridRow (int nColumnsNum = 0, DWORD_PTR dwData = 0, int nBlockSize = 10);

	virtual ~CBCGPGridRow();

	enum ClickArea
	{
		ClickExpandBox,
		ClickName,
		ClickValue,
		ClickDescription
	};

// Operations:
public:
	void AddItem (CBCGPGridItem* pItem);
	
	CBCGPGridItem* GetItem (int nColumn) const
	{
		if (nColumn >= 0 && nColumn < m_arrRowItems.GetSize ())
		{
			return m_arrRowItems [nColumn];
		}

		return NULL;
	}
	
	int GetItemCount () const
	{
		return (int) m_arrRowItems.GetSize ();
	}

	void SetItemRTC (int nColumn, CRuntimeClass* pRuntimeClass);
	void SetDefaultItemRTC (CRuntimeClass* pRTC);
	
	int GetSubItemsCount (BOOL bRecursive = FALSE) const;
	void GetSubItems (CList<CBCGPGridRow*, CBCGPGridRow*>& lst, BOOL bRecursive = FALSE);
	void AllowSubItems (BOOL bGroup = TRUE);
	BOOL AddSubItem (CBCGPGridRow* pItem, BOOL bRedraw = TRUE);

	CBCGPGridRow* HitTest (CPoint point, CBCGPGridRow::ClickArea* pnArea = NULL)
	{
		int iColumn = -1;
		CBCGPGridItem* pItem = NULL;
		return HitTest (point, iColumn, pItem, pnArea);
	}

	CBCGPGridRow* HitTest (CPoint point, int &iColumn, 
		CBCGPGridItem*& pItem,
		CBCGPGridRow::ClickArea* pnArea = NULL);

	void Expand (BOOL bExpand = TRUE);
	void Redraw ();
	void AdjustButtonRect ();

protected:
	void Init ();
	void SetFlags ();
	void SetOwnerList (CBCGPGridCtrl* pWndList);
	void Repos (int& y);
	void Shift (int dx, int dy);
	void AddTerminalItem (CList<CBCGPGridRow*, CBCGPGridRow*>& lstItems);
	void AddGroupedItem (CList<CBCGPGridRow*, CBCGPGridRow*>& lstItems);
	POSITION InsertAutoGroupBefore (
				CList<CBCGPGridRow*, CBCGPGridRow*>& lstItems, 
				POSITION& posInsertBefore, POSITION posFirstItemInGroup, int nParentLevel,
				CBCGPGridRow* pGroupedItem);
	BOOL IsAutoGroupExpanded (CBCGPGridRow* pGroupedItem, int iLevel) const;

	BOOL IsSubItem (CBCGPGridRow* pItem) const;
	CBCGPGridRow* FindSubItemByData (DWORD_PTR dwData) const;
	CBCGPGridRow* FindSubItemById (int nIndex) const;

	void ExpandDeep (BOOL bExpand = TRUE);

	BOOL IsItemFiltered () const;

// Overrides
public:
	virtual CBCGPGridItem* CreateItem (int nRow, int nColumn);
	virtual BOOL ReplaceItem (int nColumn, CBCGPGridItem* pNewItem, BOOL bRedraw = TRUE, BOOL bRepos = FALSE);

	virtual void OnItemChanged (CBCGPGridItem* pItem, int nRow, int nColumn);

	virtual void OnDrawName (CDC* pDC, CRect rect);
	virtual void OnDrawPreview (CDC* pDC, CRect rect);
	virtual void OnDrawExpandBox (CDC* pDC, CRect rectExpand);
	virtual void OnDrawRowMarker (CDC* pDC, CRect rectMarker);
	virtual void OnDrawItems (CDC* pDC, CRect rect);

	virtual void OnPrintName (CDC* pDC, CRect rect);
	virtual void OnPrintItems (CDC* pDC, CRect rectItems);

	virtual int GetHierarchyLevel () const;
	virtual void OnExpand (BOOL /*bExpand*/) {}

	virtual CString FormatItem ();
	virtual CString GetName ();

//	virtual BOOL EditItem (CBCGPGridRow* pItem, LPPOINT lptClick = NULL);

	virtual BOOL OnUpdateValue ();
	virtual BOOL OnEdit (LPPOINT lptClick);
	virtual BOOL OnEndEdit ();

	virtual void OnClickButton (CPoint /*point*/) {} // TODO
	virtual BOOL OnClickValue (UINT uiMsg, CPoint point);
	virtual BOOL OnDblClick (CPoint point);

	virtual void OnClickName (CPoint /*point*/) {}
	virtual void OnRClickName (CPoint /*point*/) {}
	virtual void OnRClickValue (CPoint point, BOOL bSelChanged);

	virtual void OnSelectCombo ();
	virtual void OnCloseCombo();

	virtual BOOL PushChar (UINT nChar);

	virtual CString GetNameTooltip ();
	virtual CString GetValueTooltip () { return _T("");} // TODO
	virtual CRect GetNameTooltipRect ();

	virtual void OnPosSizeChanged (CRect /*rectOld*/) {}

	virtual void OnSetSelection (CBCGPGridRow* /*pOldSel*/) {}
	virtual void OnKillSelection (CBCGPGridRow* /*pNewSel*/) {}

protected:
	virtual HBRUSH OnCtlColor(CDC* pDC, UINT nCtlColor);

	virtual BOOL OnKillFocus (CWnd* pNewWnd);
	virtual BOOL OnEditKillFocus ();

	virtual BOOL HasValueField () const
	{
		return TRUE;
	}

	virtual BOOL IsItemVisible () const;

	virtual void GetPreviewText (CString& str) const;
	virtual void OnMeasureGridRowRect (CRect& rect);
	virtual void OnMeasureGridItemRect (CRect& rect, CBCGPGridItem* pItem);

// Attributes
public:
	DWORD_PTR GetData () const
	{
		return m_dwData;
	}

	void SetData (DWORD_PTR dwData)
	{
		m_dwData = dwData;
	}

	BOOL IsGroup () const
	{
		return m_bGroup;
	}

	BOOL IsExpanded () const
	{
		return m_bExpanded;
	}

	BOOL IsParentExpanded () const;

	void Select (BOOL bSelect = TRUE);
	BOOL IsSelected () const;

	void Enable (BOOL bEnable = TRUE);
	BOOL IsEnabled () const
	{
		return m_bEnabled;
	}
	
	int GetRowId () const
	{
		return m_nIdRow;
	}

//?	void AllowEdit (BOOL bAllow = TRUE)
//	{
//		ASSERT (m_varValue.vt != VT_BOOL);
//		m_bAllowEdit = bAllow;
//	}

//?	BOOL IsAllowEdit () const
//	{
//		return m_bAllowEdit;
//	}

	CRect GetRect () const
	{
		return m_Rect;
	}

	CBCGPGridRow* GetSubItem (int nIndex) const;

	CBCGPGridRow* GetParent () const
	{
		return m_pParent;
	}

	BOOL IsInPlaceEditing () const
	{
		return m_bInPlaceEdit;
	}

	CWnd*				GetInPlaceWnd () const;	// Get pointer to InPlace editing window
	CComboBox*			GetComboWnd () const;	// Get pointer to combobox
	CSpinButtonCtrl*	GetSpinWnd () const;	// Get pointer to spin button

	CBCGPGridCtrl* GetOwnerList () const
	{
		return m_pWndList;
	}


public:
	// Data formats
	static CString	m_strFormatChar;
	static CString	m_strFormatShort;
	static CString	m_strFormatLong;
	static CString	m_strFormatUShort;
	static CString	m_strFormatULong;
	static CString	m_strFormatFloat;
	static CString	m_strFormatDouble;

protected:
	CArray<CBCGPGridItem*, CBCGPGridItem*>	m_arrRowItems;	// Items in this row

	CBCGPGridCtrl*	m_pWndList;		// Pointer to the grid window
	DWORD_PTR		m_dwData;		// User-defined data

	CRect			m_Rect;			// Row rectangle (in the grid coordinates)
	
	BOOL			m_bInPlaceEdit;	// Is in InPlace editing mode
	BOOL			m_bGroup;		// Is item group?
	BOOL			m_bExpanded;	// Is item expanded (for groups only)
	BOOL			m_bSelected;	// Is item selected?
	BOOL			m_bEnabled;		// Is item enabled?
	BOOL			m_bAllowEdit;	// Is item editable?
	DWORD			m_dwFlags;		// Item flags

	BOOL			m_bNameIsTrancated;

	CBCGPGridRow*		m_pParent;		// Parent item (NULL for top-level items)
	CList<CBCGPGridRow*, CBCGPGridRow*>	m_lstSubItems;	// Sub-Items list

	int				m_nIdRow;		// Index of this row
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridItem object

class CBCGPGridMergedCells;

class BCGCBPRODLLEXPORT CBCGPGridItem : public CObject
{
	DECLARE_DYNCREATE(CBCGPGridItem)

	friend class CBCGPGridCtrl;
	friend class CBCGPGridRow;

// Construction
protected:
	CBCGPGridItem ();
	
public:
	CBCGPGridItem(const _variant_t& varValue, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);

// Attributes methods
	const _variant_t& GetValue () const
	{
		return m_varValue;
	}

	void SetValue (const _variant_t& varValue, BOOL bRedraw = TRUE);
	void SetModifiedFlag (BOOL bSet = TRUE)
	{
		m_bIsChanged = bSet;
	}

	const CString& GetLabel ()
	{
		if (m_bIsChanged)
		{
			m_strLabel = FormatItem ();
			m_bIsChanged = FALSE;
		}

		return m_strLabel;
	}

	DWORD_PTR GetData () const
	{
		return m_dwData;
	}

	void SetData (DWORD_PTR dwData)
	{
		m_dwData = dwData;
	}

	int GetImage () const
	{
		return m_iImage;
	}

	void SetImage (int iImage, BOOL bRedraw = TRUE);

	void Select (BOOL bSelect = TRUE);
	BOOL IsSelected () const;

	void Enable (BOOL bEnable = TRUE);
	BOOL IsEnabled () const
	{
		return m_bEnabled;
	}

	void AllowEdit (BOOL bAllow = TRUE)
	{
		if (m_varValue.vt == VT_BOOL && bAllow)
		{
			ASSERT (FALSE);
			return;
		}
		m_bAllowEdit = bAllow;
	}

	BOOL IsAllowEdit () const
	{
		return m_bAllowEdit;
	}

	CRect GetRect () const
	{
		return m_Rect;
	}

	CRect GetButtonRect () const
	{
		return m_rectButton;
	}

	BOOL IsInPlaceEditing () const
	{
		return m_bInPlaceEdit;
	}

	CWnd* GetInPlaceWnd () const
	{
		return IsInPlaceEditing () ? m_pWndInPlace : NULL;
	}
	
	CBCGPGridItemID GetGridItemID () const
	{
		CBCGPGridItemID id (m_pGridRow != NULL ? m_pGridRow->GetRowId () : -1,
							m_nIdColumn);
		return id;
	}
	
	int GetColumnId () const
	{
		return m_nIdColumn;
	}

	CBCGPGridRow* GetParentRow () const
	{
		return m_pGridRow;
	}

	COLORREF GetBackgroundColor () const	// -1 - default value
	{
		return m_clrBackground;
	}

	COLORREF GetTextColor () const			// -1 - default value
	{
		return m_clrText;
	}

	void SetBackgroundColor (COLORREF color, BOOL bRedraw = TRUE);
	void SetTextColor (COLORREF color, BOOL bRedraw = TRUE);

	BOOL IsMultiline () const
	{
		return ((m_dwFlags & BCGP_GRID_ITEM_MULTILINE) != 0);
	}

	BOOL IsWordWrap () const;

protected:
	CBCGPGridCtrl* GetOwnerList () const
	{
		return (m_pGridRow != NULL) ? m_pGridRow->m_pWndList : NULL;
	}

// Operations methods
public:
	BOOL AddOption (LPCTSTR lpszOption, BOOL bInsertUnique = TRUE);
	void RemoveAllOptions ();

	int GetOptionCount () const;
	LPCTSTR GetOption (int nIndex) const;

	CBCGPGridItem* HitTest (CPoint point, CBCGPGridRow::ClickArea* pnArea = NULL);

	void Redraw ();

	void EnableSpinControl (BOOL bEnable = TRUE, int nMin = 0, int nMax = 0);


// Implementation
public:
	virtual ~CBCGPGridItem();

protected:
	void Init ();
	void SetFlags ();
	void SetOwnerRow (CBCGPGridRow* pRow);

// Overrides
public:
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual void OnDrawIcon (CDC* pDC, CRect& rect);
	virtual COLORREF OnFillBackground (CDC* pDC, CRect rect);
	virtual void OnDrawButton (CDC* pDC, CRect rectButton);
	virtual void OnDrawBorders (CDC* pDC, CRect rect);

	virtual void OnPrintValue (CDC* pDC, CRect rect);

	virtual BOOL OnCreate (int /* nRow */, int /* nColumn */)	{	return TRUE;	}

	virtual CString FormatItem ();

	virtual BOOL OnUpdateValue ();
	virtual void SetItemChanged ();
	virtual BOOL OnEdit (LPPOINT lptClick);
	virtual CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);
	virtual CSpinButtonCtrl* CreateSpinControl (CRect rectSpin);

	virtual BOOL OnEndEdit ();

	virtual void DoClickButton (CPoint point);

	virtual void OnClickButton (CPoint point);
	virtual BOOL OnClickValue (UINT uiMsg, CPoint point);
	virtual BOOL OnDblClick (CPoint point);

	virtual void OnSelectCombo ();
	virtual void OnCloseCombo();

	virtual BOOL OnSetCursor () const;
	virtual BOOL PushChar (UINT nChar);

	virtual CString GetValueTooltip ();

	virtual void OnRClickValue (CPoint /*point*/, BOOL /*bSelChanged*/) {}

	virtual void OnPosSizeChanged (CRect rectOld);

	virtual void OnSetSelection (CBCGPGridItem* /*pOldSel*/) {}
	virtual void OnKillSelection (CBCGPGridItem* /*pNewSel*/) {}

	virtual void AdjustButtonRect ();
	virtual void AdjustInPlaceEditRect (CRect& rectEdit, CRect& rectSpin);

	virtual void Merge (CBCGPGridMergedCells* pMergedRange);
	virtual CBCGPGridMergedCells* GetMergedCells ();
	virtual CRect GetMergedRect ();
	virtual BOOL GetMergedRange (CBCGPGridRange& range);
	virtual CBCGPGridItem* GetMergedMainItem () const;

	virtual void OnGetBorders (GRID_BORDERS& borders);
	virtual void OnGetBorders (CRect& rect);

protected:
	virtual HBRUSH OnCtlColor(CDC* pDC, UINT nCtlColor);
	virtual CComboBox* CreateCombo (CWnd* pWndParent, CRect rect);
	virtual void OnDestroyWindow ();

	virtual BOOL OnKillFocus (CWnd* /*pNewWnd*/)
	{
		return TRUE;
	}

	virtual BOOL OnEditKillFocus ()
	{
		return TRUE;
	}

	virtual BOOL HasButton () const;

	virtual BOOL HasValueField () const
	{
		return TRUE;
	}

	virtual BOOL TextToVar (const CString& strText);

	virtual BOOL CanUpdateData () const;


// Attributes
protected:
	_variant_t		m_varValue;		// Item value
	CString			m_strLabel;		// Text, representing the item value
	BOOL			m_bIsChanged;
	CBCGPGridRow*	m_pGridRow;		// Pointer to the grid row
	DWORD_PTR		m_dwData;		// User-defined data
	CString			m_strEditMask;	// Item edit mask (see CBCGPMaskEdit for description)
	CString			m_strEditTempl;	// Item edit template (see CBCGPMaskEdit for description)
	CString			m_strValidChars;// Item edit valid chars (see CBCGPMaskEdit for description)

	CStringList		m_lstOptions;	// List of combobox items

	BOOL			m_bInPlaceEdit;	// Is in InPlace editing mode

	CWnd*			m_pWndInPlace;	// Pointer to InPlace editing window
	CComboBox*		m_pWndCombo;	// Pointer to combbox
	CSpinButtonCtrl*	m_pWndSpin;		// Pointer to spin button

	CRect			m_Rect;			// Item rectangle (in the grid coordinates)
	CRect			m_rectButton;	// Drop down/open button rectangle
	BOOL			m_bButtonIsDown;// Is button pressed?
	
	BOOL			m_bSelected;	// Is item selected?
	BOOL			m_bEnabled;		// Is item enabled?
	BOOL			m_bAllowEdit;	// Is item editable?
	DWORD			m_dwFlags;		// Item flags

	BOOL			m_bValueIsTrancated;

	int				m_nMinValue;
	int				m_nMaxValue;

	int				m_nIdColumn;	// Index of this column

	COLORREF		m_clrBackground;// Custom item background color
	COLORREF		m_clrText;		// Custom item foreground color

	int				m_iImage;		// Image index

	CBCGPGridMergedCells* m_pMerged;// Nonzero, if item is merged with others
};

#ifndef _BCGPGRID_STANDALONE

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridColorItem object

class BCGCBPRODLLEXPORT CBCGPGridColorItem : public CBCGPGridItem
{
	friend class CBCGPGridCtrl;
	friend class CBCGPGridRow;

	DECLARE_DYNCREATE(CBCGPGridColorItem)

// Construction
protected:
	CBCGPGridColorItem ();

public:
	CBCGPGridColorItem(const COLORREF& color, 
		CPalette* pPalette = NULL, DWORD_PTR dwData = 0);
	virtual ~CBCGPGridColorItem();

// Overrides
public:
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual void OnPrintValue (CDC* pDC, CRect rect);
	
	virtual void OnClickButton (CPoint point);
	virtual BOOL OnEdit (LPPOINT lptClick);
	virtual BOOL OnUpdateValue ();
	virtual CString FormatItem ();

protected:
	virtual BOOL OnKillFocus (CWnd* pNewWnd)
	{
		return pNewWnd->GetSafeHwnd () != m_pPopup->GetSafeHwnd ();
	}

	virtual BOOL OnEditKillFocus ()
	{
		return m_pPopup == NULL;
	}

	virtual void AdjustInPlaceEditRect (CRect& rectEdit, CRect& rectSpin);

// Attributes
public:
	COLORREF GetColor () const
	{
		return m_Color;
	}

	void SetColor (COLORREF color);

	// Color popup attributes:
	void EnableAutomaticButton (LPCTSTR lpszLabel, COLORREF colorAutomatic, BOOL bEnable = TRUE);
	void EnableOtherButton (LPCTSTR lpszLabel, BOOL bAltColorDlg = TRUE, BOOL bEnable = TRUE);
	void SetColumnsNumber (int nColumnsNumber);

// Attributes
protected:
	COLORREF					m_Color;			// Color value
	COLORREF					m_ColorAutomatic;	// Automatic (default) color value
	CString						m_strAutoColor;		// Atomatic color label
	BOOL						m_bStdColorDlg;		// Use standard Windows color dialog
	CString						m_strOtherColor;	// Alternative color label
	CColorPopup*				m_pPopup;
	CArray<COLORREF, COLORREF>	m_Colors;
	int							m_nColumnsNumber;	// Number of columns in dropped-down colors list
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridDateTimeItem object

class BCGCBPRODLLEXPORT CBCGPGridDateTimeItem : public CBCGPGridItem
{
	DECLARE_DYNCREATE(CBCGPGridDateTimeItem)

// Construction
protected:
	CBCGPGridDateTimeItem ();

public:
	CBCGPGridDateTimeItem(const COleDateTime& date, DWORD_PTR dwData = 0,
		UINT nFlags = CBCGPDateTimeCtrl::DTM_DATE | CBCGPDateTimeCtrl::DTM_TIME);

	virtual ~CBCGPGridDateTimeItem();

// Overrides
public:
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);
	virtual BOOL OnUpdateValue ();
	virtual CString FormatItem ();

	virtual void OnSetSelection (CBCGPGridItem* pOldSel);
	virtual void OnKillSelection (CBCGPGridItem* pNewSel);

	virtual BOOL PushChar (UINT nChar);

	virtual void AdjustInPlaceEditRect (CRect& rectEdit, CRect& rectSpin);

// Attributes
public:
	void SetDate (COleDateTime date);
	void SetDateTime (const COleDateTime& date, DWORD_PTR dwData = 0,
		UINT nFlags = CBCGPDateTimeCtrl::DTM_DATE | CBCGPDateTimeCtrl::DTM_TIME,
		BOOL bRedraw = TRUE);
	COleDateTime GetDate () const
	{
		return (COleDateTime) (DATE) m_varValue;
	}

protected:
	UINT m_nFlags;

	void SetState (CBCGPDateTimeCtrl& wnd);
};

#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridCheckItem object

class BCGCBPRODLLEXPORT CBCGPGridCheckItem : public CBCGPGridItem
{
	DECLARE_DYNCREATE(CBCGPGridCheckItem)

// Construction
protected:
	CBCGPGridCheckItem ();

public:
	CBCGPGridCheckItem(bool bVal, DWORD_PTR dwData = 0);
	virtual ~CBCGPGridCheckItem();

// Overrides
public:
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual BOOL PushChar (UINT nChar);
	virtual CString FormatItem ()	{	return _T("");	}
	virtual BOOL OnClickValue (UINT uiMsg, CPoint point);
	virtual BOOL OnDblClick (CPoint point);
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridURLItem object

class BCGCBPRODLLEXPORT CBCGPGridURLItem : public CBCGPGridItem
{
	DECLARE_DYNCREATE(CBCGPGridURLItem)

// Construction
protected:
	CBCGPGridURLItem ();

public:
	CBCGPGridURLItem(CString str, CString strURL, DWORD_PTR dwData = 0);
	virtual ~CBCGPGridURLItem();

// Attributes:
public:
	CString GetURL () const
	{
		return m_strURL;
	}

	void SetURL (LPCTSTR lpszURL)
	{
		ASSERT (lpszURL != NULL);
		m_strURL = lpszURL;
	}

protected:
	CString m_strURL;

// Overrides
public:
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual BOOL OnSetCursor () const;
	virtual BOOL OnClickValue (UINT uiMsg, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridMergedCells object

class BCGCBPRODLLEXPORT CBCGPGridMergedCells : public CObject
{
// Construction:
public:
	CBCGPGridMergedCells (BOOL bAutoDelete = TRUE);
	virtual ~CBCGPGridMergedCells ();

// Operations:
public:
	const CBCGPGridItemID GetMainItemID () const;
	const CBCGPGridItemID GetVisibleItemID () const;

	void SetRect (const CRect& rect);
	void SetRectEmpty ();
	void SetRange (const CBCGPGridRange& r);

	CRect&			GetRect ();
	const CRect&	GetRect () const;
	CBCGPGridRange&			GetRange ();
	const CBCGPGridRange&	GetRange () const;

	void MarkChanged (const CRect& rectNew, const CBCGPGridItemID& id);
	void MarkUpdated ();

	BOOL IsChanged () const
	{
		return m_bChanged;
	}

	void AddRef ();
	void Release ();
	void SetAutoDelete (BOOL bAutoDelete);

// Attributes:
protected:
	CRect	m_rectMerged;	// Position in pixels of the merged cell
	CBCGPGridRange	m_range;// Range boundaries
	CBCGPGridItemID	m_idFirstVisible; // the first visible item of the merged cell
	BOOL	m_bChanged;		// TRUE, if range should be repositioned

	int		m_nRefCount;
	BOOL	m_bAutoDelete;
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridColumnsItem object

class BCGCBPRODLLEXPORT CBCGPGridColumnsItem : public CObject
{
	friend class CBCGPGridColumnsInfo;

public:
	CBCGPGridColumnsItem (LPCTSTR lpszColumn, int nWidth, int iImage = -1)
		: m_bFixedSize (TRUE), m_nDefaultSize (nWidth), m_strName (lpszColumn),
		m_nAlignment (HDF_LEFT), m_nHeaderAlignment (HDF_LEFT),
		m_bVisible (TRUE), m_iImage (iImage), 
		m_bLocked (FALSE), m_bText (TRUE), m_nAutoSize (0)
	{
	}
	virtual ~CBCGPGridColumnsItem ()
	{
	}

	// Attributes:
	BOOL		m_bFixedSize;	// 1 - specific width, 0 - best fit
	int			m_nDefaultSize;	// Default width for each column
	CString		m_strName;		// Name for each column
	int			m_nAlignment;	// HDF_LEFT - Left, HDF_RIGHT - Right, HDF_CENTER - Center, 
	int			m_nHeaderAlignment; // HDF_LEFT - Left, HDF_RIGHT - Right, HDF_CENTER - Center, 
	BOOL		m_bVisible;		// Is this column visible?
	int			m_iImage;		// Index for an image from an image list, -1 - no image
	BOOL		m_bLocked;		// Is width for this column locked? (locked for icons etc)
	BOOL		m_bText;		// Does column have text content (false for icons etc)

protected:
	int		m_nAutoSize;	// calculated value
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridColumnsInfo object

class BCGCBPRODLLEXPORT CBCGPGridColumnsInfo : public CObject
{
	friend class CBCGPGridRow;
	friend class CBCGPGridCtrl;
	friend class CBCGPGridColumnListBox;
	friend class CBCGPDBGridCtrl;
	friend class CBCGPHeaderItemDragWnd;

	enum ClickArea
	{
		ClickHeader,
		ClickDivider,
		ClickHeaderButton
	};

public:
	// Operations methods:
	int  InsertColumn (int nPos, LPCTSTR lpszColumn, int nWidth, int iImage = -1);
	BOOL DeleteColumn (int nPos);

	void InsertColumns (int nColumnsNum, int nDefaultWidth = 10);
	void DeleteAllColumns ();

	void Resize (int nTotalWidth, int nStartColumn = -1);
	BOOL ResizeColumn (int nColumn, int nWidth);
	BOOL OnChangeColumnWidth (int nColumn, int &nWidth) const;
	BOOL GetColumnWidthMinMax (int nColumn, int& nMin, int& nMax) const;

	void SetOwnerList (CBCGPGridCtrl* pWndList);
	
	// Attributes methods:
	int GetColumnCount (BOOL bCalcVisibleOnly = FALSE) const;

	int GetTotalWidth () const
	{
		return m_nTotalWidth;
	}

	int  GetColumnWidth (int nColumn) const;
	BOOL SetColumnWidth (int nColumn, int nWidth);

	LPCTSTR GetColumnName (int nColumn) const;
	BOOL SetColumnName (int nColumn, LPCTSTR lpszColumn);

	int  GetColumnAlign (int nColumn) const;
	BOOL SetColumnAlign (int nColumn, int nAlign);

	int  GetHeaderAlign (int nColumn) const;
	BOOL SetHeaderAlign (int nColumn, int nAlign);

	int  GetColumnImage (int nColumn) const;
	BOOL SetColumnImage (int nColumn, int nImage);

	BOOL GetColumnLocked (int nColumn) const;
	BOOL SetColumnLocked (int nColumn, BOOL bLocked);
	
	BOOL GetColumnVisible (int nColumn) const;
	BOOL SetColumnVisible (int nColumn, BOOL bVisible);

	BOOL CanChangeWidth (int nColumn) const; // can't change width for icons etc
	int GetMinWidth (int nColumn) const;

	void GetColumnRect (int nColumn, CRect& rect) const;
	int HitTestColumn (CPoint point, BOOL bDelimiter = FALSE, int nDelta = 0,
		CBCGPGridColumnsInfo::ClickArea* pnArea = NULL) const;

	// ---------
	// Resizing:
	// ---------
	void EnableAutoSize (BOOL bEnable = TRUE);

	BOOL IsAutoSize () const
	{
		return m_bAutoSize;
	}

	// --------
	// Sorting:
	// --------
	int GetSortColumnCount () const;

	void SetSortColumn (int nColumn, BOOL bAscening = TRUE, BOOL bAdd = FALSE);
	BOOL RemoveSortColumn (int nColumn);
	void RemoveAllSortColumns ();

	int GetSortColumn () const;
	int GetColumnState (int nColumn) const;
		// Returns: 0 - not not sorted, -1 - descending, 1 - ascending

	void EnableMultipleSort (BOOL bEnable = TRUE);
	
	BOOL IsMultipleSort () const
	{
		ASSERT (m_bMultipleSort || GetSortColumnCount () <= 1);
		return m_bMultipleSort;
	}

	// ---------
	// Grouping:
	// ---------
	int GetGroupColumnCount () const;

	int InsertGroupColumn (int nPos, int nColumn);
	BOOL RemoveGroupColumn (int nPos);
	BOOL RemoveGroupColumnByVal (int nColumn);
	int GetGroupColumn (int nPos) const;
	int GetGroupColumnPos (int nColumn);

	BOOL IsGroupColumn (int nColumn) const;

	int GetGroupingColumnOrderArray (LPINT piArray, int iCount = -1) const;

	// --------------
	// Columns order:
	// --------------
	BOOL SetColumnOrderArray (int iCount, LPINT piArray);
	int GetColumnOrderArray (LPINT piArray, int iCount = -1) const;

	BOOL ChangeColumnOrder (int nNewPos, int nColumn);

	int IndexToOrder (int nColumn) const;
	int OrderToIndex (int nPosition) const;

	int GetFirstVisibleColumn () const;
	int GetLastVisibleColumn () const;
	
	// Implementation:
	CBCGPGridColumnsInfo ();
	virtual ~CBCGPGridColumnsInfo ();

	virtual void DrawColumn (CDC* pDC, int nColumn, CRect rect,
		int nTextMargin = 5, int nArrowMargin = 5, BOOL bIsPrinting = FALSE,
		BOOL bNoSortArrow = FALSE,
		BOOL bIsGroupBox = FALSE);

	void SetHighlightColumn (int nColumn);
	int GetHighlightColumn () const;
	void SetHighlightColumnBtn (int nColumn);
	int GetHighlightColumnBtn () const;

	BOOL IsTextColumn (int nColumn) const;
	BOOL SetTextColumn (int nColumn, BOOL bText);
	int GetLeftTextOffset () const;

protected:
	int BestFitColumn (int nColumn) const;
	void OnInsertColumn (int nPos);
	
	// Helper functions: iterator for visible columns
	int Begin () const;
	int End () const;
	int Next (int& i) const;
		// Returns next visible column, index in CBCGPGridColumnsInfo::m_lstColumns.

protected:
	// Attributes:
	CList<CBCGPGridColumnsItem*, CBCGPGridColumnsItem*>	m_lstColumns;
	CMap<int, int, int, int> m_mapSortColumn;	// sorted columns
	CList<int, int> m_lstGroupingColumns;		// grouping columns
	CArray<int, int> m_arrColumnOrder;			// groups order

	BOOL	m_bAutoSize;
	int		m_nTotalWidth;

	BOOL	m_bMultipleSort;

	BOOL	m_bDrawingDraggedColumn;// used for DrawColumn function
	int		m_nHighlightedItem;		// highlighted column
	int		m_nHighlightedItemBtn;	// column with highlighted button
	CBCGPGridCtrl*	m_pWndList;		// owner window
public:
	BOOL	m_bInvertPressedColumn; // used for DrawColumn function
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridPage - page of the grid control
class BCGCBPRODLLEXPORT CBCGPGridPage : public CObject
{
public:
	CBCGPGridPage () :	m_nFirstInPage (0), m_nItems (0), m_nGroups (0), m_nTotal (0), 
		m_nHorzOffset (0) 
	{
	}

	CBCGPGridPage (CBCGPGridPage& src) 
		: m_nFirstInPage (0), m_nItems (0), m_nGroups (0), m_nTotal (0),
		m_nHorzOffset (0)
	{
		*this = src;
	}

	CBCGPGridPage& operator= (CBCGPGridPage& src)
	{
		m_nFirstInPage = src.m_nFirstInPage;
		m_nItems = src.m_nItems;
		m_nGroups = src.m_nGroups;
		m_nTotal = src.m_nTotal;

		m_nHorzOffset = src.m_nHorzOffset;

		return *this;
	}

	// vertical pagination
	int m_nFirstInPage;	// index of the first item at the page
	int m_nItems;		// visible item count
	int m_nGroups;		// visible group count
	int m_nTotal;		// total item count

	// horizontal pagination
	int m_nHorzOffset;	// page scroll offset, pixels in print dc
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridPageInfo - implementation data for printing

struct BCGCBPRODLLEXPORT CBCGPGridPageInfo
{
	CBCGPGridPageInfo () : m_nFirstItem (0), m_nLastItem (0), m_nFirstCol (0), m_nLastCol (0),
		m_lpData (NULL),
		m_nItemsCount (0), m_nGroupsCount (0),
		m_nPage (1), m_nFirstInPage (0),
		m_nItemsInPage (0), m_nGroupsInPage (0), m_nTotalInPage (0),
		m_rectPageItems (0, 0, 0, 0), m_szOne (1, 1) {}

	void Init (int nFirstItem, int nLastItem, int nFirstCol, int nLastCol)
	{
		m_nFirstItem = nFirstItem;
		m_nLastItem = nLastItem;
		m_nFirstCol = nFirstCol;
		m_nLastCol = nLastCol;

		m_lpData = NULL;

		m_nItemsCount = 0;
		m_nGroupsCount = 0;
		m_nPage = 1;
		m_nFirstInPage = 0;
		m_nItemsInPage = 0;
		m_nGroupsInPage = 0;
		m_nTotalInPage = 0;

		m_rectPageItems.SetRectEmpty ();
		m_szOne = CSize (1, 1);
	}

	int m_nFirstItem;	//	printing range
	int m_nLastItem;	//
	int m_nFirstCol;	//	printing range
	int m_nLastCol;		//

	LPVOID m_lpData;

	int m_nItemsCount;		// items before current page
	int m_nGroupsCount;		// groups before current page
	UINT m_nPage;			// current page
	int m_nFirstInPage;		// index of the first item at current page
	int m_nItemsInPage;		// printing items at current page
	int m_nGroupsInPage;	// printing groups at current page
	int m_nTotalInPage;		// count of all items at current page

	CRect m_rectPageItems;	// rect for items at current page
	CSize m_szOne;			// logical height and width of one point
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridCache object - page cache implementation for virtual mode

typedef CArray<CBCGPGridRow*, CBCGPGridRow*> CachePageArray;
struct BCGCBPRODLLEXPORT CBCGPGridCachePageInfo
{
	int				nFirst;			// Index of the first item in cache page
	int				nSize;			// Item count in cache page
	CachePageArray*	pArrCachePage;	// Items array
	BOOL			bReferenced;	// Is page accessed?
};

class BCGCBPRODLLEXPORT CBCGPGridCache : public CObject
{
	friend class CBCGPGridCtrl;

public:
	// Operations methods:
	CBCGPGridRow* GetCachedRow (int nId);
	BOOL SetCachedRow (int nId, CBCGPGridRow* pRow);
	BOOL AddCache (int nId);
	void CleanUpCache ();

	// Implementation:
	CBCGPGridCache ();
	virtual ~CBCGPGridCache ();

protected:
	void DoAddCache (CachePageArray* pArray, int nIdFirst = 0);
	void DoFreeCachePage (CBCGPGridCachePageInfo& cpi);
	void DoSwapCache ();

public:
	// Attributes:
	int		m_nCachePageSize;	// size of each page
	int		m_nCachePageCount;	// max count of pages

protected:
	CList<CBCGPGridCachePageInfo, CBCGPGridCachePageInfo&>	m_lstCache;
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPGridCtrl window

class CBCGPHeaderItemDragWnd;
class CBCGPHeaderItemDropWnd;
class CBCGPGridColumnChooser;

#define BCGGRIDCTRL_ID_INPLACE 3

BCGCBPRODLLEXPORT extern UINT BCGM_GRID_ITEM_CHANGED;
BCGCBPRODLLEXPORT extern UINT BCGM_GRID_SEL_CHANGED;
BCGCBPRODLLEXPORT extern UINT BCGM_GRID_ITEM_DBLCLICK;
BCGCBPRODLLEXPORT extern UINT BCGM_GRID_ON_HIDE_COLUMNCHOOSER;

class BCGCBPRODLLEXPORT CBCGPGridCtrl : public CWnd
{
	DECLARE_DYNAMIC(CBCGPGridCtrl)

	friend class CBCGPGridView;
	friend class CBCGPGanttView;
	friend class CBCGPGridRow;
	friend class CBCGPGridItem;
	friend class CBCGPReportRow;
	friend class CBCGPGridDateTimeItem;
	friend class CBCGPGridColorItem;
	friend class CBCGPGridCheckItem;
	friend class CBCGPGridURLItem;
	friend class CBCGPGridColumnsInfo;
	friend class CBCGPHeaderItemDragWnd;
	friend class CBCGPGridColumnChooser;
	friend class CBCGPGridColumnListBox;
	friend class CBCGPSortableArray;

	enum AUTOGROUP_CLEANUP_MODE
	{
		AG_FULL_CLEANUP = 0, 
		AG_COPY_AUTOGROUPS = 1,
		AG_CLEANUP_OLDAUTOGROUPS_ONLY = 2
	};


// Construction
public:
	CBCGPGridCtrl();
	CBCGPGridCtrl(int nMemBlockSize);

// Attributes
public:
	// Set this member to TRUE if you wish to enable ASSERT_VALID checking in debug mode
	// (AFFECTS GRID PERFORMANCE IN DEBUG MODE!)
	static BOOL m_bEnableAssertValidInDebug;

	void EnableHeader (BOOL bEnable = TRUE, DWORD dwFlags = (DWORD)-1);
	BOOL IsHeaderEnabled () const
	{
		return m_bHeader;
	}

	DWORD GetHeaderFlags () const
	{
		return m_dwHeaderFlags;
	}

	// Grid and header images:
	void SetHeaderImageList (CImageList* pImagesHeader);
	CImageList* GetHeaderImageList () const
	{
		return m_pImagesHeader;
	}

	void SetHeaderBtnImageList (CBCGPToolBarImages* pImagesHeaderBtn);
	CBCGPToolBarImages* GetHeaderBtnImageList () const
	{
		return m_pImagesHeaderBtn;
	}

	void SetImageList (CImageList* pImages);
	CImageList* GetImageList () const
	{
		return m_pImages;
	}

	// Virtual mode:
	void EnableVirtualMode (BCGPGRID_CALLBACK pCallback = NULL, LPARAM lParam = 0);
		// If the virtual mode was enabled once, you cannot disable it

	BOOL IsVirtualMode () const
	{
		return m_bVirtualMode;
	}

    BCGPGRID_CALLBACK GetCallbackFunc ()
	{
		return m_pfnCallback;
	}
	
	void SetVirtualRows (int nRowsNum);

	// Filter:
	void EnableFilter (BCGPGRID_FILTERCALLBACK pfnCallback = NULL, LPARAM lParam = 0);
	BCGPGRID_FILTERCALLBACK GetFilterCallbackFunct ();

	BOOL IsFilterEnabled () const
	{
		return m_bFilter;
	}

	// Sorting operations:
	virtual void Sort (int nColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE);
	virtual void ToggleSortColumn (int nColumn);
	void SetSortColumn (int nColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE);
	void RemoveSortColumn (int nColumn);

	int GetSortColumn () const
	{
		return m_Columns.GetSortColumn ();
	}

	void EnableMultipleSort (BOOL bEnable = TRUE);
	BOOL IsMultipleSort () const;

	virtual BOOL IsSortingMode () const
	{
		return (m_Columns.GetSortColumnCount () > 0);
	}

	void EnableMarkSortedColumn (BOOL bMark = TRUE, BOOL bRedraw = TRUE);

	virtual BOOL IsMarkSortedColumn () const
	{
		return m_bMarkSortedColumn;
	}

	// Grouping operations:
	void EnableGroupByBox (BOOL bEnable = TRUE);

	BOOL IsGroupByBox () const
	{
		return m_bGroupByBox;
	}

	int InsertGroupColumn (int nPos, int nColumn);
	BOOL RemoveGroupColumn (int nPos);
	BOOL RemoveGroupColumnByVal (int nColumn);

	int GetGroupColumnRect (int nPos, CRect& rect, CDC* pDC);

	BOOL IsGrouping () const;

	void SetBoolLabels (LPCTSTR lpszTrue, LPCTSTR lpszFalse);
	void SetListDelimiter (TCHAR c);

	CRect GetListRect () const
	{
		return m_rectList; 
	}

	int	GetRowHeight () const
	{
		return m_nRowHeight;
	}

	int	GetLargeRowHeight () const
	{
		return m_nLargeRowHeight;
	}

	void AllowRowExtraHeight (BOOL bAllow = TRUE);
	BOOL IsRowExtraHeightAllowed () const;

	void SetGridHeaderHeight (int nHeight)
	{
		ASSERT (nHeight >= 0);
		m_nGridHeaderHeight = nHeight;
	}

	void SetGridFooterHeight (int nHeight)
	{
		ASSERT (nHeight >= 0);
		m_nGridFooterHeight = nHeight;
	}

	CRect GetGridHeaderRect () const;
	CRect GetGridFooterRect () const;

	virtual CBCGPGridColumnsInfo& GetColumnsInfo ()
	{
		return m_Columns;
	}

	BOOL IsFocused () const
	{
		return m_bFocused;
	}

	void SetReadOnly (BOOL bReadOnly = TRUE)
	{
		if (!bReadOnly)
		{
			m_bWholeRowSel = FALSE;
		}

		m_bReadOnly = bReadOnly;
	}

	BOOL IsReadOnly () const 
	{
		return m_bReadOnly;
	}
	
	void SetSelectionBorder (BOOL bEnabled = TRUE)
	{
		m_bSelectionBorder = bEnabled;
	}

	BOOL IsSelectionBorder () const
	{
		return m_bSelectionBorder;
	}

	void SetSelectionBorderForActiveItem (BOOL bEnable = TRUE)
	{
		// by default - TRUE
		m_bSelectionBorderActiveItem = bEnable;
	}

	BOOL IsSelectionBorderForActiveItem () const
	{
		return m_bSelectionBorderActiveItem;
	}

	void SetHighlightActiveItem (BOOL bEnable = TRUE)
	{
		// by default - FALSE
		m_bHighlightActiveItem = bEnable;
	}

	BOOL IsHighlightActiveItem () const
	{
		return m_bHighlightActiveItem;
	}

	int GetTextLeftOffset () const
	{
		return 0;
	}
	
	void SetHighlightGroups (BOOL bHighlight = TRUE)
	{
		m_bHighlightGroups = bHighlight;
	}

	BOOL IsHighlightGroups () const
	{
		return m_bHighlightGroups;
	}

	BOOL IsControlBarColors () const
	{
		return m_bControlBarColors;
	}

	BOOL IsShowDragContext () const
	{
		return m_bShowDragContext;
	}

	void SetShowDragContext (BOOL bShowDragContext = TRUE)
	{
		m_bShowDragContext = bShowDragContext;
	}

	void SetEditFirstClick (BOOL bEditFirstClick = TRUE)
	{
		m_bEditFirstClick = bEditFirstClick;
	}

	BOOL IsSingleSel () const
	{
		return m_bSingleSel;
	}

	void SetSingleSel (BOOL bSingleSel = TRUE)
	{
		m_bSingleSel = bSingleSel;
	}

	BOOL IsWholeRowSel () const
	{
		return m_bWholeRowSel;
	}

	BOOL SetWholeRowSel (BOOL bWholeRowSel = TRUE)
	{
		if (bWholeRowSel && !m_bReadOnly)
		{
			return FALSE; // Allow in Read-only mode only
		}
		
		m_bWholeRowSel = bWholeRowSel;
		return TRUE;
	}
	
	BOOL IsShowInPlaceToolTip () const
	{
		return m_bShowInPlaceToolTip;
	}

	void SetShowInPlaceToolTip (BOOL bShow = TRUE);

	void SetRowMarker (BOOL bShow = TRUE)
	{
		m_bRowMarker = bShow;
	}

	BOOL IsRowMarker () const
	{
		return m_bRowMarker;
	}
	
	// Colors:
	void SetColorTheme (const BCGP_GRID_COLOR_DATA& theme, BOOL bRedraw = TRUE);

	const BCGP_GRID_COLOR_DATA& GetColorTheme () const
	{
		return m_ColorData;
	}

	void SetCustomColors (			// Use (COLORREF)-1 for the default color
		COLORREF	clrBackground,
		COLORREF	clrText,
		COLORREF	clrGroupBackground,
		COLORREF	clrGroupText,
		COLORREF	clrLeftOffset,
		COLORREF	clrLine);

	void GetCustomColors (
		COLORREF&	clrBackground,
		COLORREF&	clrText,
		COLORREF&	clrGroupBackground,
		COLORREF&	clrGroupText,
		COLORREF&	clrLeftOffset,
		COLORREF&	clrLine);

	COLORREF GetTextColor () const
	{
		return m_ColorData.m_clrText == (COLORREF)-1 ? 
			globalData.clrWindowText : m_ColorData.m_clrText;
	}

	COLORREF GetGroupTextColor () const
	{
		return m_ColorData.m_GroupColors.m_clrText != (COLORREF)-1 ?
			m_ColorData.m_GroupColors.m_clrText :
		(m_bHighlightGroups ? 
			(m_bControlBarColors ? globalData.clrBarShadow : globalData.clrBtnShadow) :
			globalData.clrWindowText);
	}
	
	COLORREF GetBkColor () const
	{
		return m_ColorData.m_clrBackground == (COLORREF)-1 ? 
			globalData.clrWindow : m_ColorData.m_clrBackground;
	}

	COLORREF GetSortedColor () const
	{
		return m_clrSortedColumn;
	}

	void SetPreviewTextColor (COLORREF clr);
	COLORREF GetPreviewTextColor (BOOL bSelected = FALSE) const;

	void SetScrollBarsStyle (CBCGPScrollBar::BCGPSB_STYLE style)
	{
		m_ScrollBarStyle = style;

		m_wndScrollVert.SetVisualStyle (style);
		m_wndScrollHorz.SetVisualStyle (style);
	}

	void ShowVertScrollBar (BOOL bShow = TRUE)
	{
		m_bScrollVert = bShow;
		ASSERT (m_wndScrollVert.GetSafeHwnd () != NULL);
		m_wndScrollVert.ShowWindow (bShow ? SW_SHOW : SW_HIDE);
	}

	void ShowHorzScrollBar (BOOL bShow = TRUE)
	{
		m_bScrollHorz = bShow;
		ASSERT (m_wndScrollHorz.GetSafeHwnd () != NULL);
		m_wndScrollHorz.ShowWindow (bShow ? SW_SHOW : SW_HIDE);
	}

	CBCGPScrollBar::BCGPSB_STYLE GetScrollBarsStyle () const
	{
		return m_ScrollBarStyle;
	}

	int GetRowBlockSize () const
	{
		return m_nRowsBlockSize;
	}

	// Header items drag/drop:
	void EnableDragHeaderItems (BOOL bEnable = TRUE)
	{
		m_bDragHeaderItems = bEnable;
	}

	BOOL IsDragHeaderItemsEnabled () const
	{
		return m_bDragHeaderItems;
	}

	BOOL IsGridHeaderItemHovered () const
	{
		return m_bHeaderItemHovered;
	}

protected:
	BOOL		m_bHeader;				// Is header visible?
	DWORD		m_dwHeaderFlags;		// Header flags (BCGP_GRID_HEADER_...)
	BOOL		m_bGroupByBox;			// Is GroupByBox visible?

	BOOL		m_bVirtualMode;			// Control in virtual mode
	int			m_nVirtualRows;			// Number of rows in virtual mode

	BCGPGRID_CALLBACK   m_pfnCallback;		// Callback function in virtual mode
	LPARAM				m_lParamCallback;	// Lparam for callback function
	CBCGPGridCache		m_CachedItems;		// Page cache for virtaul mode

	CBCGPInplaceToolTipCtrl	m_ToolTip;	// Tooltip control

	CString		m_strTrue;				// Customized boolean value (e.g. "Yes")
	CString		m_strFalse;				// Customized boolean value (e.g. "No")

	TCHAR		m_cListDelimeter;		// Customized list delimeter character

	CBCGPScrollBar	m_wndScrollVert;		// Vertical scroll bar
	CBCGPScrollBar	m_wndScrollHorz;		// Horizontal scroll bar
	BOOL			m_bScrollVert;			// Show vertical scroll bar
	BOOL			m_bScrollHorz;			// Show horizontal scroll bar
	HFONT			m_hFont;				// Grid regular font
	CFont			m_fontBold;				// Grid bold font
	int				m_nEditLeftMargin;		// Edit control left margin

	CImageList*	m_pImagesHeader;			// Grid header
	CImageList*	m_pImages;					// Grid item images
	CBCGPToolBarImages*	m_pImagesHeaderBtn;	// Grid header button's images

	int			m_nGroupByBoxHeight;	// GroupByBox height
	CRect		m_rectList;				// Grid area
	CRect		m_rectClip;				// Clipping area
	CRgn		m_rgnClip;
	CRgn		m_rgnClipRow;
	CRgn		m_rgnClipItem;
	CRect		m_rectHeader;			// Header area
	int			m_nRowHeight;			// Height of the single row
	int			m_nLargeRowHeight;		// Height of the group row
	BOOL		m_bAllowRowExtraHeight;	// If FALSE, the height for all rows is fixed
	int			m_nGridHeaderHeight;	// Height of extra space at the top
	int			m_nGridFooterHeight;	// Height of extra space at the bottom

	CBCGPGridColumnsInfo m_Columns;		// Width and names of grid columns
	int			m_nTreeColumn;			// Column for expanding boxes and hierarchy offsets

	int			m_nVertScrollOffset;	// In pixels
	int			m_nVertScrollTotal;
	int			m_nVertScrollPage;
	int			m_nHorzScrollOffset;	// In pixels
	int			m_nHorzScrollTotal;
	int			m_nHorzScrollPage;
	int			m_nFirstVisibleItem;

	//-----------------------------------------------------------
	// Tracking attributes: used for the vertical and description 
	// area splitters tracking:
	//-----------------------------------------------------------
	CRect		m_rectTrackHeader;
	CRect		m_rectTrackHeaderLeft;
	BOOL		m_bTracking;
	int			m_nTrackColumn;

	//-----------------------------
	// Mouse tracking, highlighting
	//-----------------------------
	BOOL		m_bMouseTracked;
	BOOL		m_bHeaderItemHovered;

	//--------------------------
	// In-place edit attributes:
	//--------------------------
	BOOL		m_bUpdateItemData;	// FALSE to cancel edit changes

	int m_nRowsBlockSize;	// Specifies the memory-allocation granularity for internal containers

	CList<CBCGPGridRow*, CBCGPGridRow*>		m_lstItems;			// List of top-level items
	CList<CBCGPGridRow*, CBCGPGridRow*>		m_lstTerminalItems;	// List of terminal items
	CList<CBCGPGridRow*, CBCGPGridRow*>		m_lstGroupedItems;	// List of grouped items
	CList<CBCGPGridRow*, CBCGPGridRow*>		m_lstAutoGroups;
	CArray <CBCGPGridPage, CBCGPGridPage&>	m_arrPages;			// Pages, used when printing

	CMap<int, int, CRuntimeClass*, CRuntimeClass*>
					m_mapItemsRTC;			// Default item class by column
	CRuntimeClass*	m_pDefaultItemRTC;		// Default item class

	CList<CBCGPGridRange*, CBCGPGridRange*>	m_lstSel;		// Current selection
	CBCGPGridItemID							m_idActive;		// Active item
	CBCGPGridItemID							m_idLastSel;	// Last selected item
	CBCGPGridRow*							m_pSelRow;		// Current selection
	CBCGPGridItem*							m_pSelItem;		// Current selection
	CBCGPGridRow*							m_pLastSelRow;	// Last selected row
	CBCGPGridItem*							m_pLastSelItem;	// Last selected item
	CRect									m_rectTrackSel;	// Selected area

	CBCGPGridItem*		m_pSetSelItem;	// used in CBCGPGridCtrl::DoSetSelection

	//----------------------
	// Selecting attributes:
	//----------------------
	BOOL		m_bSelecting;			// Is selecting range of items?
	BOOL		m_bClickTimer;			// Used for deferred item click
	CPoint		m_ptClickOnce;			// Saved point for item click
	BOOL		m_bIsFirstClick;		// Is first click?
	BOOL		m_bIsButtonClick;		// Is button click?
	
	BOOL		m_bRebuildTerminalItems;// If FALSE - do not rebuild terminal items
	BOOL		m_bNoUpdateWindow;		// To prevent flickering
	BOOL		m_bInAdjustLayout;		

	BOOL		m_bFocused;				// Control has focus

	COLORREF	m_clrGray;				// Special gray color
	COLORREF	m_clrSortedColumn;		// Background color to mark sorted column

	CBCGPScrollBar::BCGPSB_STYLE 
				m_ScrollBarStyle;		// Scroll bars style

	BOOL		m_bReadOnly;			// Is control in read-only mode?
	BOOL		m_bSelectionBorder;		// Draw border around the selected items
	BOOL		m_bEditFirstClick;		// Create inplace edit on first click
	BOOL		m_bHighlightGroups;		// Highlight groups with grey color
	BOOL		m_bControlBarColors;	// Use colors of tye parent control bar
	BOOL		m_bShowDragContext;		// Show context while dragging spliters
	BOOL		m_bSingleSel;			// Allows only one item at a time to be selected
	BOOL		m_bWholeRowSel;			// Allows only the whole row to be selected
	BOOL		m_bMarkSortedColumn;	// Mark sorted column by background color
	BOOL		m_bDrawFocusRect;		// Draw frame rect around an active row
	BOOL		m_bShowInPlaceToolTip;	// Show in-place tooltips for truncated items
	BOOL		m_bRowMarker;			// Show marker at left of the currently active row
	BOOL		m_bHighlightActiveItem;	// If TRUE - fill active item with selected color
	BOOL		m_bSelectionBorderActiveItem; // Draw frame rect around an active item
	BOOL		m_bGridItemBorders;		// If FALSE - do not allow item to draw its borders

	//---------------
	// Custom colors:
	//---------------
	CBCGPGridColors	m_ColorData;

	CBrush		m_brBackground;
	CBrush		m_brGroupBackground;
	CBrush		m_brSelBackground;
	CPen		m_penHLine;
	CPen		m_penVLine;

	//---------------
	// Print support:
	//---------------
	HFONT		m_hPrinterFont;		// if NULL, mirror display font
	HFONT		m_hPrinterBoldFont;	// if NULL, mirror display font
	HFONT		m_hMirrorFont;		// font object used when mirroring
	HFONT		m_hMirrorBoldFont;	// bold font object used when mirroring
	BOOL		m_bIsPrinting;		// TRUE, when printing
	CDC*		m_pPrintDC;			// DC for printing
	
	COLORREF	m_clrPrintBorder;	// Color of the grid border lines
	COLORREF	m_clrPrintHeader;	// Color of the header lines
	COLORREF	m_clrPrintHeaderBG;	// Header background
	COLORREF	m_clrPrintGroup;	// Color of the group underline
	COLORREF	m_clrPrintGroupBG;	// Group background
	COLORREF	m_clrPrintLeftOffset;// Background for the left hierarchy offset area
	COLORREF	m_clrPrintBG;		// Item background
	COLORREF	m_clrPrintLine;		// Color of the grid lines
	COLORREF	m_clrPrintText;		// Color of foreground text

	// Printing parameters, used when printing:
	struct PrintParams {
		int				m_nRowHeight;		// Height of the single row
		int				m_nLargeRowHeight;	// Height of the group row
		int				m_nGroupByBoxHeight;// GroupByBox height
		CRect			m_rectHeader;		// Header area
		CRect			m_rectList;			// Grid area
		CBCGPGridCache	m_CachedPrintItems;	// Page cache for virtaul mode

		int				m_nVertScrollOffset;
		int				m_nHorzScrollOffset;// pixels in print dc

		CPrintInfo*			m_pPrintInfo;
		CBCGPGridPageInfo	m_pageInfo;

		CBCGPGridItemID	m_idCur;			// Cached index for currently processing item
	};
	PrintParams m_PrintParams;

	//------------------------
	// Header items drag/drop:
	//------------------------
	BOOL	m_bDragHeaderItems;
	int		m_nDraggedColumn;
	CRect	m_rectStartDrag;
	CPoint	m_ptStartDrag;
	BOOL	m_bDragGroupItem;
	BOOL	m_bDragFromChooser;

	CBCGPHeaderItemDragWnd*	m_pWndHeaderDrag;			// Drag indicator
	CBCGPHeaderItemDropWnd*	m_pWndHeaderDrop;			// Drop indicator

	CBCGPGridColumnChooser*	m_pColumnChooser;
	CRect					m_rectColumnChooser;
	BOOL					m_bColumnChooserVisible;

	static HCURSOR m_hcurDeleteColumn;
	static HCURSOR m_hcurNoDropColumn;

	enum {	GRID_CLICKVALUE_TIMER_ID = 1,
			GRID_CLICKVALUE_TIMER_INTERVAL = 200};

	//------------
	// Merge cells
	//------------
	CList <CBCGPGridItem*, CBCGPGridItem*>	m_lstMergedItemsToUpdate; // used in UpdateMergedItems()

	//-------
	// Filter
	//-------
	BOOL					m_bFilter;				// Is filter enabled?
	BCGPGRID_FILTERCALLBACK m_pfnFilterCallback;	// callback function
	LPARAM					m_lFilterParam;			// filter info which is used by the callback function

public:
	static BOOL	m_bUseSystemFont;

private:
	CBCGPGridItemID		m_idCur;		// Cached index for currently processing item
	CList<CBCGPGridRow*, CBCGPGridRow*>		m_lstOldAutoGroups;	// used in ReposItems()

	int* m_aSortOrder;	// Use SetSortOrder/GetSortOrder to access;
	int m_nSortCount;	// used in DoRebuildTerminalItems, DoMultiColumnCompare.
	int m_nGroupCount;	//

// Operations
public:
	int AddRow (BOOL bRedraw = TRUE);
	int AddRow (CBCGPGridRow* pItem, BOOL bRedraw = TRUE);
	int InsertRowAfter (int nPos, CBCGPGridRow* pItem, BOOL bRedraw = TRUE);
	int RemoveRow (int nPos, BOOL bRedraw = TRUE);
	void RemoveAll ();
	void RebuildIndexes (int nStartFrom = -1);

	CBCGPGridRow* GetRow (int nPos) const;
	CBCGPGridRow* GetVirtualRow (int nRow);

	int GetRowCount () const
	{
		return (int) m_lstItems.GetCount ();
	}

	CBCGPGridRow* FindRowByData (DWORD_PTR dwData, BOOL bSearchSubItems = TRUE) const;

	CBCGPGridRow* HitTest (CPoint pt, CBCGPGridRow::ClickArea* pnArea = NULL, BOOL bItemsOnly = FALSE)
	{
		CBCGPGridItemID id;
		CBCGPGridItem* pItem = NULL;
		return HitTest (pt, id, pItem, pnArea, bItemsOnly);
	}

	CBCGPGridRow* HitTest (CPoint pt, CBCGPGridItemID &id, 
							CBCGPGridItem*& pItem,
							CBCGPGridRow::ClickArea* pnArea = NULL, 
							BOOL bItemsOnly = FALSE);

	void SetCurSel (CBCGPGridRow* pItem, BOOL bRedraw = TRUE);
	BOOL SetCurSel (CBCGPGridItemID idItem,
				   DWORD dwSelMode = SM_SINGE_SEL_GROUP | SM_SINGLE_ITEM, 
				   BOOL bRedraw = TRUE);

	CBCGPGridRow* GetCurSel () const;
	CBCGPGridRow* GetCurSel (CBCGPGridItemID &id) const;
	CBCGPGridItem* GetCurSelItem (CBCGPGridRow* pCurRow = NULL) const;

	BOOL SelectColumn (int nColumn, BOOL bRedraw = TRUE);

	BOOL IsRowSelected (int nRow) const;
	BOOL IsColumnSelected (int nColumn) const;
	void GetSelectedItems (CList <CBCGPGridItem*, CBCGPGridItem*> &lstSelected);
		
	void EnsureVisible (CBCGPGridRow* pItem, BOOL bExpandParents = FALSE);
	void ExpandAll (BOOL bExpand = TRUE);

	CRect GetRect (const CBCGPGridRange &range);

#ifndef _BCGPGRID_STANDALONE
	virtual void CloseColorPopup ();
	virtual void UpdateColor (COLORREF color);
#endif

	// -------
	// Columns
	// -------
	int  InsertColumn (int nPos, LPCTSTR lpszColumn, int nWidth, int iImage = -1);
	BOOL DeleteColumn (int nPos);
	void DeleteAllColumns ();

	int GetColumnCount() const;

	int  GetColumnWidth(int nCol) const;
	BOOL SetColumnWidth(int nCol, int nWidth);

	CString GetColumnName(int nCol) const;
	BOOL SetColumnName(int nCol, LPCTSTR lpszColumn);

	int  GetColumnAlign(int nCol) const;
	BOOL SetColumnAlign(int nCol, int nAlign);

	int  GetHeaderAlign(int nCol) const;
	BOOL SetHeaderAlign(int nCol, int nAlign);

	BOOL GetColumnLocked(int nCol) const;
	BOOL SetColumnLocked(int nCol, BOOL bLockedSize = TRUE);

	BOOL GetColumnVisible (int nColumn) const;
	BOOL SetColumnVisible (int nColumn, BOOL bVisible);
	
	BOOL IsTextColumn (int nColumn) const;
	BOOL SetTextColumn (int nColumn, BOOL bText);
	
	int  GetColumnOrderArray (LPINT piArray, int iCount) const;
	BOOL SetColumnOrderArray (int iCount, LPINT piArray);

	int GetColumnOrder (int nIndex) const
	{
		return (nIndex < 0 || nIndex >= m_Columns.GetColumnCount ()) ? 
			-1 : m_Columns.IndexToOrder (nIndex);
	}

	void EnableColumnAutoSize (BOOL bEnable = TRUE);

	BOOL IsColumnAutoSizeEnabled () const 
	{
		return m_Columns.IsAutoSize ();
	}

	void ShowColumnsChooser (BOOL bShow = TRUE);
	BOOL IsColumnsChooserVisible () const;

	void UpdateColumnsChooser ();

	//------------
	// Merge cells
	//------------
	void MergeSelection (BOOL bRedraw = TRUE);
	void UnMergeSelection (BOOL bRedraw = TRUE);
	void MergeRange (const CBCGPGridRange& range, BOOL bRedraw = TRUE);
	void UnMergeRange (const CBCGPGridRange& range, BOOL bRedraw = TRUE);

	// -----
	// Print
	// -----
	void SetPrinterFont(CFont* pFont, CFont* pFontBold);
	void Print ();

// Overrides
	virtual void OnItemChanged (CBCGPGridItem* pItem, int nRow, int nColumn);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPGridCtrl)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

public:
	virtual CBCGPGridRow* CreateRow ()
	{
		return new CBCGPGridRow (0, 0, GetRowBlockSize ());
	}

	virtual CBCGPGridRow* CreateRow (int nColumns);
	virtual CBCGPGridRow* CreateRow (CString strName);
	
	virtual CBCGPGridItem* CreateItem (int nRow, int nColumn);

	void SetItemRTC (int nColumn, CRuntimeClass* pRuntimeClass);
	void SetDefaultItemRTC (CRuntimeClass* pRTC) { m_pDefaultItemRTC = pRTC;}
	CRuntimeClass* GetItemRTC (int nColumn) const;
	CRuntimeClass* GetDefaultItemRTC () const	{ return m_pDefaultItemRTC; }

	virtual void OnPosSizeChanged ();
	virtual void OnChangeSelection (CBCGPGridRow* /*pNewSel*/, CBCGPGridRow* /*pOldSel*/) {}

	virtual BOOL EditItem (CBCGPGridRow* pItem, LPPOINT lptClick = NULL);
	virtual void OnClickButton (CPoint point);
	virtual BOOL EndEditItem (BOOL bUpdateData = TRUE);
	virtual BOOL ValidateItemData (CBCGPGridRow* /*pItem*/)
	{
		return TRUE;
	}

	virtual BOOL OnDrawItem (CDC* pDC, CBCGPGridRow* pItem);
	virtual void AdjustLayout ();

	virtual BOOL IsPreviewRowEnabled () const;
	virtual CRect OnGetPreviewRowMargins (CBCGPGridRow* pRow) const;

	//------------
	// Merge cells
	//------------
	virtual void OnMergeCellsChanging (const CBCGPGridRange& /*range*/, BOOL /*bMerge*/) {}
	virtual void OnMergeCellsChanged (const CBCGPGridRange& /*range*/, BOOL /*bMerge*/) {}
	virtual BOOL CanMergeRange (const CBCGPGridRange& /*range*/, BOOL /*bMerge*/) {return TRUE;}

	// -----
	// Print
	// -----
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrintPageHeader(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrintPageFooter(CDC* pDC, CPrintInfo* pInfo);

	virtual CRect OnGetPageMargins (CDC* pDC, CPrintInfo* pInfo);

	virtual void OnPreparePrintPages (CPrintInfo* pInfo,
									int nFirstItem, int nLastItem);
	virtual int  OnCalcPrintPages (CDC* pDC, CPrintInfo* pInfo);
	virtual void PrintLayout (CDC* pDC, CPrintInfo* pInfo);
	virtual int  CalcPages (CDC* pDC, CPrintInfo* pInfo);
	virtual int  OnAddGridPageRowForPrint (CBCGPGridPage& page, CRect rectPage);

	virtual int OnPrintItem (CDC* pDC, CPrintInfo* pInfo, CBCGPGridRow* pItem) const;

	//--------------
	// Drag and drop
	//--------------
	virtual BOOL OnHideInsertedGroupColumn (int /*nColumn*/) const
	{
		return FALSE;
	}

	virtual BOOL CanHideColumn (int /*nColumn*/) const
	{
		return (m_dwHeaderFlags & BCGP_GRID_HEADER_HIDE_ITEMS) != 0;
	}
	
	virtual BOOL CanDropColumn (int /*nNewPosition*/, int /*nOldPosition*/) const
	{
		return TRUE;
	}

	//-----------------
	// Save/Load state:
	//-----------------
#ifndef _BCGPGRID_STANDALONE
	virtual BOOL LoadState (LPCTSTR lpszProfileName, int nIndex = 0);
	virtual BOOL SaveState (LPCTSTR lpszProfileName, int nIndex = 0);
	virtual BOOL ResetState ();
#endif
	
protected:
	virtual void InitConstructor ();
	virtual void Init ();
	virtual void InitColors ();
	virtual void SetRowHeight ();

	virtual void OnFillBackground (CDC* pDC, CRect rectClient);

	virtual COLORREF OnFillSelItem (CDC* pDC, CRect rectFill, CBCGPGridItem* pItem);
	virtual void OnFillLeftOffset (CDC* pDC, CRect rectFill, CBCGPGridRow* pRow,
									BOOL bDrawRightBorder = TRUE);

	virtual void OnDrawList (CDC* pDC);
	virtual void OnDrawGroupByBox (CDC* pDC, CRect rect);
	virtual void OnDrawHeader (CDC* pDC);
	virtual void OnDrawSortArrow (CDC* pDC, CRect rectArrow, BOOL bAscending);
	virtual void OnFillHeaderBackground (CDC* pDC, CRect rect);
	virtual void OnDrawHeaderItemBorder (CDC* pDC, CRect rect, int nCol);
	virtual void OnDrawGridHeader (CDC* pDC);
	virtual void OnDrawGridFooter (CDC* pDC);
	virtual void OnDrawSelectionBorder (CDC* pDC);

	virtual CRect OnGetSelectionRect ();

	virtual BOOL OnGetPageHeaderRect (CDC* pDC, CPrintInfo* pInfo, CRect& rect);
	virtual BOOL OnGetPageFooterRect (CDC* pDC, CPrintInfo* pInfo, CRect& rect);
	virtual CRect OnGetGroupByBoxRect (CDC* pDC, const CRect& rectDraw);
	virtual CRect OnGetHeaderRect (CDC* pDC, const CRect& rectDraw);

	virtual void SetPrintColors ();

	virtual void OnPrintHeader(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrintList(CDC* pDC, CPrintInfo* pInfo);

	virtual BOOL ProcessClipboardAccelerators (UINT nChar);

	virtual BOOL OnTrackHeader ()
	{
		return TRUE; // resize columns inside client area by mouse
	}

	virtual void OnHeaderDividerDblClick (int /*nColumn*/) {}
	virtual void OnHeaderColumnClick (int /*nColumn*/) {}

	virtual BOOL FilterItem (const CBCGPGridRow* pRow);

	virtual BOOL IsHeaderMenuButtonEnabled (int nColumn) const;
	virtual int GetHeaderMenuButtonImageIndex (int nColumn, BOOL bSortArrow) const;
	virtual void OnDrawHeaderMenuButton (CDC* pDC, CRect rect, int nColumn, BOOL bSortArrow);
	virtual void OnHeaderMenuButtonClick (int nColumn, CRect rectMenuButton);
	virtual CRect GetHeaderMenuButtonRect (CRect rectItem, int nColumn) const;

	virtual int CompareItems (const CBCGPGridRow* pRow1, const CBCGPGridRow* pRow2, int iColumn) const;
	virtual int CompareItems (const CBCGPGridItem* pItem1, const CBCGPGridItem* pItem2) const;
	virtual int CompareGroup (const CBCGPGridRow* pRow1, const CBCGPGridRow* pRow2, int iColumn);
	virtual int DoMultiColumnCompare (const CBCGPGridRow* pRow1, const CBCGPGridRow* pRow2);

	// Grouping:
	virtual BOOL OnDropToGroupByBox (int /*nColumn*/)
	{
		return TRUE; 
		// Return FALSE - to disable certain columns 
		// from being dropped in the group by list
	}

	virtual CString GetGroupName (int nGroupCol, CBCGPGridItem* pItem);

	virtual int OnGetGroupNumber (const CBCGPGridRow* /*pRow*/, int /*iSortedColumn*/)
	{
		return -1;
	}

	virtual CString OnGetGroupText (int /*nGroupNumber*/, int /*iSortedColumn*/) 
	{
		CString str; return str;
	}

	virtual BOOL GetRowName (CBCGPGridRow* pRow, CString& strName);
	
	virtual void OnItemUpdateFailed ()
	{
		{
			ReleaseCapture ();
		}
	}

	virtual BOOL IsEditFirstClick () const
	{
		return m_bEditFirstClick;
	}

	virtual BOOL AllowInPlaceEdit () const
	{
		return !m_bReadOnly;
	}

	virtual int GetExtraHierarchyOffset () const { return m_nRowHeight;}
	virtual int GetHierarchyOffset () const { return m_Columns.GetGroupColumnCount () * m_nRowHeight;}

    virtual void OnUpdateVScrollPos (int nVOffset, int nPrevVOffset);

	//-------------
	// Select items
	//-------------
	virtual void OnSelChanging (const CBCGPGridRange &range, BOOL bSelect);
	virtual void OnSelChanged (const CBCGPGridRange &range, BOOL bSelect);
	
	//--------
	// Columns
	//--------
	virtual void OnResizeColumns ();
	virtual void OnHideColumnChooser ();

// Implementation
public:
	virtual ~CBCGPGridCtrl();

	virtual int HitTestGroupByBox (CPoint point, LPRECT lprectItem = NULL);
	virtual int GetGroupByBoxDropIndex (CPoint point, LPPOINT lpptDrop = NULL);

	virtual BOOL IsSelectionBorderEnabled () const
	{
		return m_bSelectionBorder && !m_bWholeRowSel;
	}

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnSetFont (WPARAM, LPARAM);
	afx_msg LRESULT OnGetFont (WPARAM, LPARAM);
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnSelectCombo();
	afx_msg void OnCloseCombo();
	afx_msg void OnEditKillFocus();
	afx_msg void OnComboKillFocus();
	afx_msg LRESULT OnMouseLeave(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()

	//------------------
	// Internal helpers:
	//------------------
	static HFONT GetDefaultFont ();
	HFONT SetCurrFont (CDC* pDC);
	virtual void TrackHeader (int nOffset);
	virtual BOOL SetHeaderItemWidth (int nPos, int nWidth);
	virtual void TrackToolTip (CPoint point);

	void SetScrollSizes ();
	int CalcExtraHeightTotal ();

	int GetPageItems (int& nFirst, int& nLast, int nSearchFrom = 0) const;
	int GetTotalItems (BOOL bCalcVisibleOnly = FALSE) const;
	int GetTotalItems (int nCountFrom, int nCountTo, 
						BOOL bCalcVisibleOnly = FALSE) const;
	int GetGroupsCount (BOOL bCalcVisibleOnly = FALSE) const;
	int GetGroupsCount (int nCountFrom, int nCountTo, 
						BOOL bCalcVisibleOnly = FALSE) const;
	int OffsetVisibleRow (int nStartFrom, int nOffsetCount, BOOL bDirForward = TRUE) const;

	BOOL DoRemoveRow (CBCGPGridRow* pItemDel, BOOL bRedraw);
	BOOL DoRemoveRow (POSITION posDel, BOOL bRedraw);

	void SetRebuildTerminalItems ()
	{
		m_bRebuildTerminalItems = TRUE;
	}

	virtual void ReposItems ();
	void ShiftItems (int dx, int dy);
	virtual void DoRebuildTerminalItems ();
	void SetSortOrder (int* aSortOrder, int nSortCount, int nGroupCount);
	BOOL GetSortOrder (int*& aSortOrder, int& nSortCount, int& nGroupCount) const;
	void CleanUpAutoGroups (AUTOGROUP_CLEANUP_MODE nMode = AG_FULL_CLEANUP);

	void UpdateFonts ();
	void CleanUpFonts ();
	void CalcEditMargin ();

	CBCGPGridItemID GetGridItemID (const CBCGPGridItem* pItem) const
	{
		ASSERT_VALID (pItem);
		return pItem->GetGridItemID ();
	}

	BOOL IsValidID (const CBCGPGridItemID &id) const;
	BOOL IsValidRange (const CBCGPGridRange &range) const;

	BOOL DoSetSelection (const CBCGPGridItemID& idItem, const DWORD dwSelMode, 
		BOOL bAddSelGroup, BOOL bRedraw,
		const int nFirstColumn, const int nFirstRow,
		const int nLastColumn, const int nLastRow);
	void UnionRange (CBCGPGridRange* pRange, const CBCGPGridRange* pRange2);
	void SelectRange (const CBCGPGridRange &range, BOOL bSelect = TRUE, BOOL bRedraw = TRUE);
	void DoSelectRowInRange (CBCGPGridRow* pRow, const CBCGPGridRange &range, 
							BOOL bSelect, BOOL bRedraw);
	void InvalidateRange (const CBCGPGridRange &range);
	void DoInvalidateRowInRange (CBCGPGridRow* pRow, const CBCGPGridRange &range,
							CRect& rectUpdate) const;
	static void IncludeRect (CRect& rect, const CRect& rectNew);

	BOOL SendNotification (BCGPGRID_NOTIFICATION* pn, UINT uCode) const;

	//--------------
	// Drag and drop
	//--------------
	virtual BOOL StartDragColumn (	int nItem, CRect rect, 
									BOOL bDragGroupItem, BOOL bDragFromChooser);
	virtual BOOL DragColumn (CPoint ptScreen);
	virtual BOOL StopDragColumn (CPoint point, BOOL bUpdate);

	//-------------
	// Select items
	//-------------
	BOOL StartSelectItems ();
	BOOL SelectItems (CPoint ptClient);
	void StopSelectItems ();
	BOOL DoClickValue (CBCGPGridItem* pItem, UINT uiMsg, CPoint point, 
						BOOL bFirstClick, BOOL bButtonClick);

	//-------------
	// Merged cells
	//-------------
	void DoMergeInRange (const CBCGPGridRange& rangeNormalized, CBCGPGridMergedCells* pMergedCells);
	void DoMergeRowItemsInRange (CBCGPGridRow* pRow, const CBCGPGridRange& rangeNormalized,
								CBCGPGridMergedCells* pMergedCells);
	BOOL ExtendMergedRange (CBCGPGridRange& range);
	int GetMergedItemsInRange (const CBCGPGridRange& range, CList <CBCGPGridRange, CBCGPGridRange&> &lstRanges);
	void DoGetMergedItemsInRange (CBCGPGridRow* pRow, const CBCGPGridRange& range, CList <CBCGPGridRange, CBCGPGridRange&> &lstRanges);
	void MarkMergedItemChanged (const CRect& rectNew, CBCGPGridItem* pItem);
	void UpdateMergedItems ();
	CRect GetMergedRect (const CBCGPGridRange* pRange, const CBCGPGridItemID& id);//TODO replace the 2nd parameter
	CBCGPGridRange* GetMergedRange (CBCGPGridItemID id, CBCGPGridItem* pItem, CBCGPGridRange& range);

	void ShowDropIndicator (CPoint pt);

	//--------------
	// Virtual mode:
	//--------------
	virtual BOOL SendDispInfoRequest (BCGPGRID_DISPINFO* pdi) const;

	virtual CBCGPGridRow* CreateVirtualRow (int nIndex);
	virtual CBCGPGridRow* OnCreateVirtualRow (BCGPGRID_DISPINFO* pdi);
	virtual CBCGPGridItem* OnCreateVirtualItem (BCGPGRID_DISPINFO* pdi);

	CRect GetVirtualRowRect (int nRow) const;
	CRect GetVirtualItemRect (int nRow, int nColumn) const;

	CBCGPGridItemID HitTestVirtual (CPoint pt, CBCGPGridRow::ClickArea* pnArea = NULL) const;
};

#endif // BCGP_EXCLUDE_GRID_CTRL

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPGRIDCTRL_H__573C359C_D1FB_446F_B60D_B5E331228237__INCLUDED_)
