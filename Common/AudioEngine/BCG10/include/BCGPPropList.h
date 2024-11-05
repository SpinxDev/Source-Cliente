#if !defined(AFX_BCGPPROPLIST_H__F6054FED_0317_4829_B7BF_4EBBDC27DF01__INCLUDED_)
#define AFX_BCGPPROPLIST_H__F6054FED_0317_4829_B7BF_4EBBDC27DF01__INCLUDED_

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
// BCGPPropList.h : header file
//

#ifndef __AFXTEMPL_H__
	#include "afxtempl.h"
#endif

#include "comdef.h"
#include "BCGCBPro.h"

#ifndef BCGP_EXCLUDE_PROP_LIST

#include "BCGGlobals.h"

#ifndef _BCGPGRID_STANDALONE
	#include "ColorPopup.h"
	#include "BCGPHeaderCtrl.h"
	#include "BCGPDateTimeCtrl.h"
	#define PROP_HEADER_CTRL_CLASS	CBCGPHeaderCtrl
#else
	#define PROP_HEADER_CTRL_CLASS	CHeaderCtrl
#endif

#include "BCGPInplaceToolTipCtrl.h"
#include "BCGPScrollBar.h"

#define AFILE_ALLOWMULFILES		1
#define AFILE_NOALLOWMULFILES	0


/////////////////////////////////////////////////////////////////////////////
// CBCGPProp object

class BCGCBPRODLLEXPORT CBCGPProp : public CObject
{
	DECLARE_DYNAMIC(CBCGPProp)

	friend class CBCGPPropList;

// Construction
public:
	// Group constructor
	CBCGPProp(const CString& strGroupName, DWORD_PTR dwData = 0,
		BOOL bIsValueList = FALSE);

	// Simple property
	CBCGPProp(const CString& strName, const _variant_t& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);

	virtual ~CBCGPProp();

	enum ClickArea
	{
		ClickExpandBox,
		ClickName,
		ClickValue,
		ClickDescription
	};

// Operations:
public:
	int GetExpandedSubItems (BOOL bIncludeHidden = TRUE) const;
	BOOL AddSubItem (CBCGPProp* pProp);
	BOOL RemoveSubItem (CBCGPProp*& pProp, BOOL bDelete = TRUE);

	BOOL AddOption (LPCTSTR lpszOption, BOOL bInsertUnique = TRUE);
	void RemoveAllOptions ();

	int GetOptionCount () const;
	LPCTSTR GetOption (int nIndex) const;

	CBCGPProp* HitTest (CPoint point, CBCGPProp::ClickArea* pnArea = NULL);

	void Expand (BOOL bExpand = TRUE);
	void Redraw ();

	void EnableSpinControl (BOOL bEnable = TRUE, int nMin = 0, int nMax = 0);

	virtual void ResetOriginalValue ();
	virtual void CommitModifiedValue ();

	void Show (BOOL bShow = TRUE, BOOL bAdjustLayout = TRUE);

protected:
	void Init ();
	void SetFlags ();
	void SetOwnerList (CBCGPPropList* pWndList);
	void Repos (int& y);
	void AddTerminalProp (CList<CBCGPProp*, CBCGPProp*>& lstProps);

	BOOL IsSubItem (CBCGPProp* pProp) const;
	CBCGPProp* FindSubItemByData (DWORD_PTR dwData) const;

	void ExpandDeep (BOOL bExpand = TRUE);
	void SetModifiedFlag ();

// Overrides
public:
	virtual void OnDrawName (CDC* pDC, CRect rect);
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual void OnDrawExpandBox (CDC* pDC, CRect rectExpand);
	virtual void OnDrawButton (CDC* pDC, CRect rectButton);
	virtual void OnDrawDescription (CDC* pDC, CRect rect);

	virtual CString FormatProperty ();

	virtual BOOL OnUpdateValue ();
	virtual BOOL OnEdit (LPPOINT lptClick);
	virtual CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);
	virtual CSpinButtonCtrl* CreateSpinControl (CRect rectSpin);

	virtual BOOL OnEndEdit ();

	virtual void OnClickButton (CPoint point);
	virtual BOOL OnClickValue (UINT uiMsg, CPoint point);
	virtual BOOL OnDblClick (CPoint point);

	virtual void OnSelectCombo ();
	virtual void OnCloseCombo();

	virtual BOOL OnSetCursor () const;
	virtual BOOL PushChar (UINT nChar);

	virtual CString GetNameTooltip ();
	virtual CString GetValueTooltip ();

	virtual void OnClickName (CPoint /*point*/) {}
	virtual void OnRClickName (CPoint /*point*/) {}
	virtual void OnRClickValue (CPoint /*point*/, BOOL /*bSelChanged*/) {}

	virtual void OnPosSizeChanged (CRect /*rectOld*/) {}

	virtual void OnSetSelection (CBCGPProp* /*pOldSel*/) {}
	virtual void OnKillSelection (CBCGPProp* /*pNewSel*/) {}

	virtual void AdjustButtonRect ();
	virtual void AdjustInPlaceEditRect (CRect& rectEdit, CRect& rectSpin);

	virtual BOOL SetACCData (CWnd* pParent, CBCGPAccessibilityData& data);

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

	virtual BOOL IsProcessFirstClick () const
	{
		return TRUE;
	}

	virtual BOOL HasValueField () const
	{
		return TRUE;
	}

	virtual BOOL TextToVar (const CString& strText);
	virtual BOOL IsValueChanged () const;

	virtual BOOL OnActivateByTab ();
	virtual BOOL OnRotateListValue (BOOL bForward);

	virtual BOOL OnCommand (WPARAM /*wParam*/)
	{
		return FALSE;
	}

// Attributes
public:
	LPCTSTR GetName () const
	{
		return m_strName;
	}

	void SetName (LPCTSTR lpszName, BOOL bRedraw = TRUE);

	virtual const _variant_t& GetValue () const
	{
		return m_varValue;
	}

	virtual void SetValue (const _variant_t& varValue);

	const _variant_t& GetOrgiginalValue () const
	{
		return m_varValueOrig;
	}

	void SetOriginalValue (const _variant_t& varValue);

	const CString& GetDescription () const
	{
		return m_strDescr;
	}

	void SetDescription (const CString& strDescr)
	{
		m_strDescr = strDescr;
	}

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

	virtual BOOL IsSelected () const;
	int GetHierarchyLevel () const;

	void Enable (BOOL bEnable = TRUE);
	BOOL IsEnabled () const
	{
		return m_bEnabled;
	}

	void AllowEdit (BOOL bAllow = TRUE)
	{
		ASSERT (m_varValue.vt != VT_BOOL);
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

	int GetSubItemsCount () const
	{
		return (int) m_lstSubItems.GetCount ();
	}

	CBCGPProp* GetSubItem (int nIndex) const;

	CBCGPProp* GetParent () const
	{
		return m_pParent;
	}

	BOOL IsInPlaceEditing () const
	{
		return m_bInPlaceEdit;
	}

	BOOL IsModified () const
	{
		return m_bIsModified;
	}

	BOOL IsVisible () const
	{
		return m_bIsVisible;
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
	CString			m_strName;		// Property name
	_variant_t		m_varValue;		// Property value
	_variant_t		m_varValueOrig;	// Property original value
	CBCGPPropList*	m_pWndList;		// Pointer to the PropertyList window
	DWORD_PTR		m_dwData;		// User-defined data
	CString			m_strDescr;		// Property description
	CString			m_strEditMask;	// Property edit mask (see CBCGPMaskEdit for description)
	CString			m_strEditTempl;	// Property edit template (see CBCGPMaskEdit for description)
	CString			m_strValidChars;// Property edit valid chars (see CBCGPMaskEdit for description)

	CStringList		m_lstOptions;	// List of combobox items

	BOOL			m_bInPlaceEdit;	// Is in InPalce editing mode

	CWnd*			m_pWndInPlace;	// Pointer to InPlace editing window
	CComboBox*		m_pWndCombo;	// Pointer to combbox
	CSpinButtonCtrl*	m_pWndSpin;		// Pointer to spin button

	CRect			m_Rect;			// Property rectangle (in the prop.list coordinates)
	CRect			m_rectButton;	// Drop down/open button rectangle
	BOOL			m_bButtonIsDown;// Is button pressed?
	BOOL			m_bButtonIsFocused;	// Is button focused?
	
	BOOL			m_bGroup;		// Is property group?
	BOOL			m_bExpanded;	// Is property expanded (for groups only)
	BOOL			m_bEnabled;		// Is propperty enabled?
	BOOL			m_bAllowEdit;	// Is property editable?
	BOOL			m_bIsValueList;	// This is a value list group?
	DWORD			m_dwFlags;		// Property flags

	CBCGPProp*		m_pParent;		// Parent property (NULL for top-level properties)
	CList<CBCGPProp*, CBCGPProp*>	m_lstSubItems;	// Sub-properies list

	BOOL			m_bNameIsTrancated;
	BOOL			m_bValueIsTrancated;

	int				m_nMinValue;
	int				m_nMaxValue;

	BOOL			m_bIsModified;	// Is property modified
	BOOL			m_bIsVisible;	// Is property visible
};

#ifndef _BCGPGRID_STANDALONE

/////////////////////////////////////////////////////////////////////////////
// CBCGPColorProp object

class BCGCBPRODLLEXPORT CBCGPColorProp : public CBCGPProp
{
	friend class CBCGPPropList;

	DECLARE_DYNAMIC(CBCGPColorProp)

// Construction
public:
	CBCGPColorProp(const CString& strName, const COLORREF& color, 
		CPalette* pPalette = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CBCGPColorProp();

// Overrides
public:
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual void OnClickButton (CPoint point);
	virtual BOOL OnEdit (LPPOINT lptClick);
	virtual BOOL OnUpdateValue ();
	virtual CString FormatProperty ();

protected:
	virtual BOOL OnKillFocus (CWnd* pNewWnd)
	{
		return pNewWnd->GetSafeHwnd () != m_pPopup->GetSafeHwnd ();
	}

	virtual BOOL OnEditKillFocus ()
	{
		return m_pPopup == NULL;
	}

	virtual BOOL IsValueChanged () const
	{
		return m_Color != m_ColorOrig;
	}

	virtual void AdjustInPlaceEditRect (CRect& rectEdit, CRect& rectSpin);
	virtual void ResetOriginalValue ();
	virtual void CommitModifiedValue ();

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
	COLORREF					m_ColorOrig;		// Original color value
	COLORREF					m_ColorAutomatic;	// Automatic (default) color value
	CString						m_strAutoColor;		// Atomatic color label
	BOOL						m_bStdColorDlg;		// Use standard Windows color dialog
	CString						m_strOtherColor;	// Alternative color label
	CColorPopup*				m_pPopup;
	CArray<COLORREF, COLORREF>	m_Colors;
	int							m_nColumnsNumber;	// Number of columns in dropped-down colors list
};

#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPFileProp object

class BCGCBPRODLLEXPORT CBCGPFileProp : public CBCGPProp
{
	DECLARE_DYNAMIC(CBCGPFileProp)

// Construction
public:

#ifndef _BCGPGRID_STANDALONE
	CBCGPFileProp(const CString& strName, const CString& strFolderName, DWORD_PTR dwData = 0, LPCTSTR lpszDescr = NULL);
#endif

	CBCGPFileProp(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, 
				LPCTSTR lpszDefExt = NULL,
				DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
				LPCTSTR lpszFilter = NULL,
				LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CBCGPFileProp();

// Overrides
public:
	virtual void OnClickButton (CPoint point);

// Attributes
protected:

#ifndef _BCGPGRID_STANDALONE
	BOOL m_bIsFolder;
#endif

	// File open dialog atributes:
	BOOL	m_bOpenFileDialog;	// TRUE - use "File Open/Save" diaog; otherwise - folder selection dialog

	DWORD	m_dwFileOpenFlags;
	CString	m_strDefExt;
	CString m_strFilter;
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPFontProp object

class BCGCBPRODLLEXPORT CBCGPFontProp : public CBCGPProp
{
	DECLARE_DYNAMIC(CBCGPFontProp)

// Construction
public:
	CBCGPFontProp(	const CString& strName, LOGFONT& lf, 
					DWORD dwFontDialogFlags = CF_EFFECTS | CF_SCREENFONTS, 
					LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
					COLORREF color = (COLORREF)-1);
	virtual ~CBCGPFontProp();

// Overrides
public:
	virtual void OnClickButton (CPoint point);
	virtual CString FormatProperty ();

// Attributes
public:
	LPLOGFONT GetLogFont ()
	{
		return &m_lf;
	}

	COLORREF GetColor () const
	{
		return m_Color;
	}

protected:
	LOGFONT		m_lf;
	LOGFONT		m_lfOrig;
	DWORD_PTR	m_dwFontDialogFlags;
	COLORREF	m_Color;

	virtual BOOL IsValueChanged () const
	{
		return	memcmp (&m_lf, &m_lfOrig, sizeof (LOGFONT)) != 0 ||
				lstrcmp (m_lf.lfFaceName, m_lfOrig.lfFaceName) != 0;
	}

	virtual void ResetOriginalValue ();
	virtual void CommitModifiedValue ();
};

#ifndef _BCGPGRID_STANDALONE

/////////////////////////////////////////////////////////////////////////////
// CBCGPDateTimeProp object

class BCGCBPRODLLEXPORT CBCGPDateTimeProp : public CBCGPProp
{
	DECLARE_DYNAMIC(CBCGPDateTimeProp)

// Construction
public:
	CBCGPDateTimeProp(const CString& strName, const COleDateTime& date, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		UINT nFlags = CBCGPDateTimeCtrl::DTM_DATE | CBCGPDateTimeCtrl::DTM_TIME);

	virtual ~CBCGPDateTimeProp();

// Overrides
public:
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);
	virtual void OnPosSizeChanged (CRect rectOld);
	virtual BOOL OnUpdateValue ();

	virtual void OnSetSelection (CBCGPProp* pOldSel);
	virtual void OnKillSelection (CBCGPProp* pNewSel);

	virtual BOOL PushChar (UINT nChar);

	virtual CString FormatProperty ();
	virtual BOOL IsValueChanged () const;

	virtual void AdjustInPlaceEditRect (CRect& rectEdit, CRect& rectSpin);
	virtual BOOL OnCommand (WPARAM /*wParam*/);

	virtual void ResetOriginalValue ();

// Attributes
public:
	void SetDate (COleDateTime date);
	COleDateTime GetDate () const
	{
		return (COleDateTime) (DATE) m_varValue;
	}

protected:
	UINT				m_nFlags;
	CBCGPDateTimeCtrl	m_wndDateTime;

	void SetState (CBCGPDateTimeCtrl& wnd);
};

#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPPropList window

#define BCGPROPLIST_ID_INPLACE 3

BCGCBPRODLLEXPORT extern UINT BCGM_PROPERTY_CHANGED;

class BCGCBPRODLLEXPORT CBCGPPropList : public CBCGPWnd
{
	DECLARE_DYNAMIC(CBCGPPropList)

	friend class CBCGPProp;
	friend class CBCGPColorProp;
	friend class CBCGPDateTimeProp;

// Construction
public:
	CBCGPPropList();

// Attributes
public:
	void EnableHeaderCtrl (BOOL bEnable = TRUE,
		LPCTSTR lpszLeftColumn = _T("Property"), 
		LPCTSTR lpszRightColumn = _T("Value"));
	BOOL IsHeaderCtrl () const
	{
		return m_bHeaderCtrl;
	}

	void EnableDescriptionArea (BOOL bEnable = TRUE);
	BOOL IsDescriptionArea () const
	{
		return m_bDescriptionArea;
	}

	int GetDescriptionHeight () const
	{
		return m_nDescrHeight;
	}

	int GetDescriptionRows () const
	{
		return m_nDescrRows;
	}

	void SetDescriptionRows (int nDescRows);

	void SetAlphabeticMode (BOOL bSet = TRUE);
	BOOL IsAlphabeticMode () const
	{
		return m_bAlphabeticMode;
	}

	void SetVSDotNetLook (BOOL bSet = TRUE);
	BOOL IsVSDotNetLook () const
	{
		return m_bVSDotNetLook;
	}

	void MarkModifiedProperties (BOOL bMark = TRUE, BOOL bRedraw = TRUE);
	BOOL IsMarkModifiedProperties () const
	{
		return m_bMarkModifiedProperties;
	}

	void ResetOriginalValues (BOOL bRedraw = TRUE);
	void CommitModifiedValues (BOOL bRedraw = TRUE);

	void SetBoolLabels (LPCTSTR lpszTrue, LPCTSTR lpszFalse);
	void SetListDelimiter (TCHAR c);

	CRect GetListRect () const
	{
		return m_rectList; 
	}

	int GetPropertyColumnWidth () const 
	{ 
		return m_nLeftColumnWidth; 
	}

	void SetPropertyColumnWidth (int nWidth)
	{ 
		m_nLeftColumnWidth = nWidth;
	}

	int GetHeaderHeight () const
	{
		return m_nHeaderHeight;
	}

	int	GetRowHeight () const
	{
		return m_nRowHeight;
	}

	virtual PROP_HEADER_CTRL_CLASS& GetHeaderCtrl ()
	{
		return m_wndHeader;
	}

	int GetLeftColumnWidth () const
	{
		return m_nLeftColumnWidth;
	}

	BOOL IsGroupNameFullWidth () const
	{
		return m_bGroupNameFullWidth;
	}

	void SetGroupNameFullWidth (BOOL bGroupNameFullWidth = TRUE, BOOL bRedraw = TRUE);

	BOOL IsShowDragContext () const
	{
		return m_bShowDragContext;
	}

	void SetShowDragContext (BOOL bShowDragContext = TRUE)
	{
		m_bShowDragContext = bShowDragContext;
	}

	void SetCustomColors (			// Use (COLORREF)-1 for the default color
		COLORREF	clrBackground,
		COLORREF	clrText,
		COLORREF	clrGroupBackground,
		COLORREF	clrGroupText,
		COLORREF	clrDescriptionBackground,
		COLORREF	clrDescriptionText,
		COLORREF	clrLine);

	void GetCustomColors (
		COLORREF&	clrBackground,
		COLORREF&	clrText,
		COLORREF&	clrGroupBackground,
		COLORREF&	clrGroupText,
		COLORREF&	clrDescriptionBackground,
		COLORREF&	clrDescriptionText,
		COLORREF&	clrLine);

	COLORREF GetTextColor () const
	{
		return m_clrText == (COLORREF)-1 ? 
			globalData.clrWindowText : m_clrText;
	}

	COLORREF GetBkColor () const
	{
		return m_clrBackground == (COLORREF)-1 ? 
			globalData.clrWindow : m_clrBackground;
	}

	CFont& GetBoldFont ()
	{
		return m_fontBold;
	}

	BOOL IsAlwaysShowUserToolTip () const 
	{
		return m_bAlwaysShowUserTT;
	}

	void AlwaysShowUserToolTip (BOOL bShow = TRUE)
	{
		m_bAlwaysShowUserTT = bShow;
	}

	BOOL DrawControlBarColors () const
	{
		return m_bVisualManagerStyle || m_bControlBarColors;
	}

	void SetScrollBarsStyle (CBCGPScrollBar::BCGPSB_STYLE style)
	{
		m_ScrollBarStyle = style;
		m_wndScrollVert.SetVisualStyle (style);
	}

	CBCGPScrollBar::BCGPSB_STYLE GetScrollBarsStyle () const
	{
		return m_ScrollBarStyle;
	}

protected:
	PROP_HEADER_CTRL_CLASS	m_wndHeader;		// Property list header control
	BOOL		m_bHeaderCtrl;			// Is header control visible?

	BOOL		m_bDescriptionArea;		// Does description area enabled?
	int			m_nDescrHeight;			// Description area height
	int			m_nDescrRows;			// Number of rows in description area

	CToolTipCtrl	m_ToolTip;			// Tooltip control
	int			m_nTooltipsCount;		// Number of tooltip items

	CBCGPInplaceToolTipCtrl	m_IPToolTip;	// Inplace tooltip control

	BOOL		m_bAlwaysShowUserTT;	// Always show user tooltips, even if in-place TT should be activated

	BOOL		m_bAlphabeticMode;		// Use property list in alphabetic (non-"tree") mode
	BOOL		m_bVSDotNetLook;		// Provide MS Visual Studio.NET look (gray groups, flat look)

	CString		m_strTrue;				// Customized boolean value (e.g. "Yes")
	CString		m_strFalse;				// Customized boolean value (e.g. "No")

	TCHAR		m_cListDelimeter;		// Customized list delimeter character

	CBCGPScrollBar	m_wndScrollVert;		// Vertical scroll bar
	CBCGPScrollBar::BCGPSB_STYLE 
				m_ScrollBarStyle;		// Scroll bars style

	HFONT		m_hFont;				// Property list regular font
	CFont		m_fontBold;				// Property list bold font
	int			m_nEditLeftMargin;		// Edit control left margin
	int			m_nBoldEditLeftMargin;	// Edit control left margin (bold font)

	int			m_nBorderSize;			// Control border size

	int			m_nHeaderHeight;		// Header control height
	CRect		m_rectList;				// Properies area
	int			m_nRowHeight;			// Height of the single row
	int			m_nLeftColumnWidth;		// Width of the left ("Name") column

	int			m_nVertScrollOffset;	// In rows
	int			m_nVertScrollTotal;
	int			m_nVertScrollPage;

	BOOL		m_bMarkModifiedProperties;	// Draw modified properties with bold

	//-----------------------------------------------------------
	// Tracking attributes: used for the vertical and description 
	// area splitters tracking:
	//-----------------------------------------------------------
	CRect		m_rectTrackHeader;
	CRect		m_rectTrackDescr;
	BOOL		m_bTracking;
	BOOL		m_bTrackingDescr;

	CList<CBCGPProp*, CBCGPProp*>	m_lstProps;			// List of top-level properties
	CList<CBCGPProp*, CBCGPProp*>	m_lstTerminalProps;	// List of terminal properties

	CBCGPProp*	m_pSel;					// Current selection
	
	BOOL		m_bFocused;				// Control has focus

	COLORREF	m_clrGray;				// Special gray color

	BOOL		m_bControlBarColors;	// Use colors of tye parent control bar
	BOOL		m_bGroupNameFullWidth;	// Display group name in the whole row
	BOOL		m_bShowDragContext;		// Show context while dragging spliters

	//---------------
	// Custom colors:
	//---------------
	COLORREF	m_clrBackground;		// Control background color
	COLORREF	m_clrText;				// Control foreground color
	COLORREF	m_clrGroupBackground;	// Group background text
	COLORREF	m_clrGroupText;			// Group foreground text
	COLORREF	m_clrDescriptionBackground;	// Description background text
	COLORREF	m_clrDescriptionText;	// Description foreground text
	COLORREF	m_clrLine;				// Color of the grid lines

	CBrush		m_brBackground;

// Operations
public:
	int AddProperty (CBCGPProp* pProp, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE);
	BOOL DeleteProperty (CBCGPProp*& pProp, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE);
	void RemoveAll ();

	CBCGPProp* GetProperty (int nIndex) const;
	int GetPropertyCount () const
	{
		return (int) m_lstProps.GetCount ();
	}

	CBCGPProp* FindItemByData (DWORD_PTR dwData, BOOL bSearchSubItems = TRUE) const;

	CBCGPProp* HitTest (CPoint pt, CBCGPProp::ClickArea* pnArea = NULL, BOOL bPropsOnly = FALSE) const;

	void SetCurSel (CBCGPProp* pProp, BOOL bRedraw = TRUE);
	CBCGPProp* GetCurSel () const
	{
		return m_pSel;
	}

	void EnsureVisible (CBCGPProp* pProp, BOOL bExpandParents = FALSE);
	void ExpandAll (BOOL bExapand = TRUE);

#ifndef _BCGPGRID_STANDALONE
	virtual void CloseColorPopup ();
	virtual void UpdateColor (COLORREF color);
#endif

	virtual void AdjustLayout ();

// Overrides
	virtual void OnPropertyChanged (CBCGPProp* pProp) const;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPPropList)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	virtual void OnChangeSelection (CBCGPProp* /*pNewSel*/, CBCGPProp* /*pOldSel*/) {}

	virtual BOOL EditItem (CBCGPProp* pProp, LPPOINT lptClick = NULL);
	virtual void OnClickButton (CPoint point);
	virtual BOOL EndEditItem (BOOL bUpdateData = TRUE);
	virtual BOOL ValidateItemData (CBCGPProp* /*pProp*/)
	{
		return TRUE;
	}

	virtual int OnDrawProperty (CDC* pDC, CBCGPProp* pProp) const;
	virtual void InitHeader ();
	virtual BOOL OnSetAccData (long lVal);
	virtual BOOL IsAccessibilityCompatible () { return TRUE; }
	virtual HRESULT get_accRole(VARIANT varChild, VARIANT *pvarRole);

protected:
	virtual void Init ();

	virtual void OnFillBackground (CDC* pDC, CRect rectClient);

	virtual void OnDrawBorder (CDC* pDC);
	virtual void OnDrawList (CDC* pDC);
	virtual void OnDrawDescription (CDC* pDC, CRect rect);

	virtual BOOL ProcessClipboardAccelerators (UINT nChar);

	virtual int CompareProps (const CBCGPProp* pProp1, const CBCGPProp* pProp2) const;

	virtual void NotifyAccessibility (CBCGPProp* pProp);

// Implementation
public:
	virtual ~CBCGPPropList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPPropList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
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
	//}}AFX_MSG
	afx_msg LRESULT OnSetFont (WPARAM, LPARAM);
	afx_msg LRESULT OnGetFont (WPARAM, LPARAM);
	afx_msg void OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderEndTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinDeltaPos(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnUpdateSpin (WPARAM, LPARAM);
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnSelectCombo();
	afx_msg void OnCloseCombo();
	afx_msg void OnEditKillFocus();
	afx_msg void OnComboKillFocus();
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);
	afx_msg LRESULT OnGetObject (WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	//------------------
	// Internal helpres:
	//------------------
	HFONT SetCurrFont (CDC* pDC);
	void TrackHeader (int nOffset);
	void TrackDescr (int nOffset);
	void TrackToolTip (CPoint pt);

	void SetScrollSizes ();

	int GetTotalItems (BOOL bIncludeHidden = TRUE) const;
	void ReposProperties ();

	void CreateBoldFont ();
	void CalcEditMargin ();
};

#define GetDesciption		 GetDescription
#define SetDesciption		 SetDescription
#define m_bDesciptionArea	 m_bDescriptionArea
#define GetDesciptionHeight	 GetDescriptionHeight
#define OnDrawDesciption	 OnDrawDescription
#define EnableDesciptionArea EnableDescriptionArea
#define IsDesciptionArea	 IsDescriptionArea
#define GetDesciptionHeight	 GetDescriptionHeight

#endif // BCGP_EXCLUDE_PROP_LIST

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPPROPLIST_H__F6054FED_0317_4829_B7BF_4EBBDC27DF01__INCLUDED_)
