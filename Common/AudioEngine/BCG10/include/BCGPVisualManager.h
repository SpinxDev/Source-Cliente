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
// BCGPVisualManager.h: interface for the CBCGPPVisualManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPVISUALMANAGER_H__8070C021_60CE_11D5_A304_8B7130518C10__INCLUDED_)
#define AFX_BCGPVISUALMANAGER_H__8070C021_60CE_11D5_A304_8B7130518C10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGCBPro.h"

#define CBCGPRibbonComboEdit CBCGPRibbonEditCtrl

class CBCGPBaseControlBar;
class CBCGPPopupMenu;
class CBCGPBaseToolBar;
class CBCGPToolBar;
class CBCGPToolbarButton;
class CBCGPToolbarMenuButton;
class CBCGPCaptionButton;
class CBCGPCaptionMenuButton;
class CBCGPBaseTabWnd;
class CBCGPToolbarComboBoxButton;
class CBCGPStatusBar;
class CBCGStatusBarPaneInfo;
class CBCGPButton;
class CBCGPMiniFrameWnd;
class CBCGPCaptionBar;
class CBCGPHeaderCtrl;
class CBCGPToolbarEditBoxButton;
class CBCGPDockingControlBar;
class CBCGPTask;
class CBCGPTasksGroup;
class CBCGPSlider;
class CBCGPSplitterWnd;
class CBCGPAutoHideButton;
class CBCGPCalculator;
class CBCGPEdit;
class CBCGPSpinButtonCtrl;
class CBCGPAppBarWnd;
class CBCGPPopupWindow;
class CBCGPPopupWndButton;
class CBCGPPlannerView;
class CBCGPGridCtrl;
class CBCGPCalendar;
class CBCGPCalendarColors;
class CBCGPToolBoxButton;
class CBCGPPropList;
class CBCGPRibbonBar;
class CBCGPRibbonCategory;
class CBCGPRibbonCategoryScroll;
class CBCGPRibbonPanel;
class CBCGPRibbonButton;
class CBCGPRibbonTab;
class CBCGPRibbonCaptionButton;
class CBCGPRibbonButtonsGroup;
class CBCGPRibbonLaunchButton;
class CBCGPRibbonEditCtrl;
class CBCGPRibbonMainPanel;
class CBCGPRibbonLabel;
class CBCGPRibbonPaletteButton;
class CBCGPRibbonPaletteIcon;
class CBCGPRibbonContextCaption;
class CBCGPToolTipParams;
enum BCGPRibbonCategoryColor;
class CBCGPRibbonStatusBar;
class CBCGPRibbonStatusBarPane;
class CBCGPRibbonSlider;
class CBCGPRibbonProgressBar;
class CBCGPRibbonPanelMenuBar;
class CBCGPRibbonPanelMenu;
class CBCGPRibbonSeparator;
class CBCGPBaseRibbonElement;
class CBCGPPropertySheet;
class CBCGPRibbonCheckBox;
class CBCGPRibbonHyperlink;
class CBCGPRibbonColorButton;
class CBCGPRibbonRadioButton;
class CBCGPScrollBar;
class CBCGPToolTipCtrl;
class CBCGPGroup;
class CBCGPSliderCtrl;
class CBCGPURLLinkButton;
class CBCGPListBox;
class CBCGPGanttChart;
class CBCGPGanttItem;
struct BCGP_GANTT_CHART_COLORS;
struct BCGP_GANTT_CHART_HEADER_CELL_INFO;
struct BCGP_GRID_COLOR_DATA;

#define	SPIN_PRESSEDUP			0x0001
#define	SPIN_PRESSEDDOWN		0x0002
#define	SPIN_HIGHLIGHTEDUP		0x0004
#define	SPIN_HIGHLIGHTEDDOWN	0x0008
#define	SPIN_DISABLED			0x0010

#ifndef _UXTHEME_H_

// From uxtheme.h:
typedef HANDLE HTHEME;          // handle to a section of theme data for class

#endif // THEMEAPI

typedef HTHEME (__stdcall * OPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (__stdcall * CLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT (__stdcall * DRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, 
						int iPartId, int iStateId, const RECT *pRect, 
						OPTIONAL const RECT *pClipRect);
typedef HRESULT (__stdcall * GETTHEMECOLOR)(HTHEME hTheme, int iPartId, 
						int iStateId, int iPropId, OUT COLORREF *pColor);
typedef COLORREF (__stdcall * GETTHEMESYSCOLOR)(HTHEME hTheme, int iColorId);

typedef HRESULT (__stdcall * GETCURRENTTHEMENAME)(
    OUT LPWSTR pszThemeFileName, int cchMaxNameChars, 
    OUT OPTIONAL LPWSTR pszColorBuff, int cchMaxColorChars,
    OUT OPTIONAL LPWSTR pszSizeBuff, int cchMaxSizeChars);

typedef HTHEME (__stdcall * GETWINDOWTHEME)(HWND hWnd);

class BCGCBPRODLLEXPORT CBCGPWinXPThemeManager : public CObject
{
public:
	CBCGPWinXPThemeManager ();
	virtual ~CBCGPWinXPThemeManager ();

	enum WinXpTheme
	{
		WinXpTheme_None = -1,
		WinXpTheme_NonStandard,
		WinXpTheme_Blue,
		WinXpTheme_Olive,
		WinXpTheme_Silver
	};

	virtual BOOL DrawPushButton (CDC* pDC, CRect rect, CBCGPButton* pButton, UINT uiState);

	virtual BOOL DrawStatusBarProgress (CDC* pDC, CBCGPStatusBar* pStatusBar,
			CRect rectProgress, int nProgressTotal, int nProgressCurr,
			COLORREF clrBar, COLORREF clrProgressBarDest, COLORREF clrProgressText,
			BOOL bProgressText);

	virtual BOOL DrawComboDropButton (CDC* pDC, CRect rect,
										BOOL bDisabled,
										BOOL bIsDropped,
										BOOL bIsHighlighted);
	virtual BOOL DrawComboBorder	(CDC* pDC, CRect rect,
										BOOL bDisabled,
										BOOL bIsDropped,
										BOOL bIsHighlighted);

	virtual void FillRebarPane		(CDC* pDC, 
									CBCGPBaseControlBar* pBar, 
									CRect rectClient);

	virtual BOOL DrawCheckBox (CDC *pDC, CRect rect, 
										 BOOL bHighlighted, 
										 int nState,
										 BOOL bEnabled,
										 BOOL bPressed);

	virtual BOOL DrawRadioButton (CDC *pDC, CRect rect, 
										 BOOL bHighlighted, 
										 BOOL bChecked,
										 BOOL bEnabled,
										 BOOL bPressed);

	virtual WinXpTheme GetStandardWinXPTheme ();

protected:
	void UpdateSystemColors ();
	void CleanUpThemes ();

	HTHEME				m_hThemeWindow;
	HTHEME				m_hThemeToolBar;
	HTHEME				m_hThemeRebar;
	HTHEME				m_hThemeButton;
	HTHEME				m_hThemeStatusBar;
	HTHEME				m_hThemeComboBox;
	HTHEME				m_hThemeProgress;
	HTHEME				m_hThemeHeader;
	HTHEME				m_hThemeScrollBar;
	HTHEME				m_hThemeExplorerBar;
	HTHEME				m_hThemeTree;
	HTHEME				m_hThemeStartPanel;
	HTHEME				m_hThemeTaskBand;
	HTHEME				m_hThemeTaskBar;
	HTHEME				m_hThemeSpin;
	HTHEME				m_hThemeTab;
	HTHEME				m_hThemeToolTip;
	HTHEME				m_hThemeTrack;
	HTHEME				m_hThemeMenu;
	HTHEME				m_hThemeNavigation;

	HINSTANCE			m_hinstUXDLL;

	OPENTHEMEDATA		m_pfOpenThemeData;
	CLOSETHEMEDATA		m_pfCloseThemeData;
	DRAWTHEMEBACKGROUND	m_pfDrawThemeBackground;
	GETTHEMECOLOR		m_pfGetThemeColor;
	GETTHEMESYSCOLOR	m_pfGetThemeSysColor;
	GETCURRENTTHEMENAME	m_pfGetCurrentThemeName;
	GETWINDOWTHEME		m_pfGetWindowTheme;
};

class BCGCBPRODLLEXPORT CBCGPVisualManager : public CBCGPWinXPThemeManager
{
	friend class CBCGPSkinManager;
	friend struct BCGPGLOBAL_DATA;

	DECLARE_DYNCREATE (CBCGPVisualManager)

public:
	CBCGPVisualManager(BOOL bTemporary = FALSE);
	virtual ~CBCGPVisualManager();

	static void SetDefaultManager (CRuntimeClass* pRTI);

	virtual BOOL IsWinXPThemeSupported () const	{	return FALSE;	}
	static void DestroyInstance (BOOL bAutoDestroyOnly = FALSE);

// Operations:
public:
	static void RedrawAll ();
	static void AdjustToolbars ();
	static void AdjustFrames ();

// Overrides:
public:
	virtual void OnUpdateSystemColors ();

	virtual void OnFillBarBackground (CDC* pDC, CBCGPBaseControlBar* pBar,
									CRect rectClient, CRect rectClip,
									BOOL bNCArea = FALSE);
	virtual BOOL OnFillParentBarBackground (CWnd* pWnd, CDC* pDC, LPRECT rectClip = NULL);

	virtual void OnDrawBarBorder (CDC* pDC, CBCGPBaseControlBar* pBar, CRect& rect);
	virtual void OnDrawMenuBorder (CDC* pDC, CBCGPPopupMenu* pMenu, CRect rect);
	virtual void OnDrawMenuShadow (CDC* pDC, const CRect& rectClient, const CRect& rectExclude,
									int nDepth,  int iMinBrightness,  int iMaxBrightness,  
									CBitmap* pBmpSaveBottom,  CBitmap* pBmpSaveRight, BOOL bRTL);
	virtual void OnDrawBarGripper (CDC* pDC, CRect rectGripper, BOOL bHorz, CBCGPBaseControlBar* pBar);
	virtual void OnDrawSeparator (CDC* pDC, CBCGPBaseControlBar* pBar, CRect rect, BOOL bIsHoriz);
	virtual COLORREF OnDrawMenuLabel (CDC* pDC, CRect rect);
	virtual COLORREF OnDrawControlBarCaption (CDC* pDC, CBCGPDockingControlBar* pBar, 
		BOOL bActive, CRect rectCaption, CRect rectButtons);
	virtual void OnDrawCaptionButton (CDC* pDC, CBCGPCaptionButton* pButton, BOOL bActive, BOOL bHorz,
										BOOL bMaximized, BOOL bDisabled, int nImageID = -1);
	virtual void OnDrawMenuSystemButton (CDC* pDC, CRect rect, UINT uiSystemCommand, 
										UINT nStyle, BOOL bHighlight);
	virtual void OnDrawStatusBarPaneBorder (CDC* pDC, CBCGPStatusBar* pBar,
					CRect rectPane, UINT uiID, UINT nStyle);
	virtual COLORREF GetStatusBarPaneTextColor (CBCGPStatusBar* pStatusBar, 
									CBCGStatusBarPaneInfo* pPane);

	virtual void OnDrawStatusBarProgress (CDC* pDC, CBCGPStatusBar* /*pStatusBar*/,
										CRect rectProgress, int nProgressTotal, 
										int nProgressCurr,
										COLORREF clrBar, 
										COLORREF clrProgressBarDest, 
										COLORREF clrProgressText,
										BOOL bProgressText);
	virtual void OnDrawStatusBarSizeBox (CDC* pDC, CBCGPStatusBar* pStatBar,
				CRect rectSizeBox);

	virtual void OnDrawComboDropButton (CDC* pDC, CRect rect,
										BOOL bDisabled,
										BOOL bIsDropped,
										BOOL bIsHighlighted,
										CBCGPToolbarComboBoxButton* pButton);
	virtual void OnDrawComboBorder (CDC* pDC, CRect rect,
										BOOL bDisabled,
										BOOL bIsDropped,
										BOOL bIsHighlighted,
										CBCGPToolbarComboBoxButton* pButton);
	virtual void OnDrawEditBorder (CDC* pDC, CRect rect,
										BOOL bDisabled,
										BOOL bIsHighlighted,
										CBCGPToolbarEditBoxButton* pButton);
	virtual void OnDrawTearOffCaption (CDC* pDC, CRect rect, BOOL bIsActive);

	virtual void OnDrawMenuResizeBar (CDC* pDC, CRect rect, int nResizeFlags);
	virtual void OnDrawMenuScrollButton (CDC* pDC, CRect rect, BOOL bIsScrollDown, 
										BOOL bIsHighlited, BOOL bIsPressed, BOOL bIsDisabled);

	enum BCGBUTTON_STATE
	{
		ButtonsIsRegular,
		ButtonsIsPressed,
		ButtonsIsHighlighted,
	};

	virtual void OnFillButtonInterior (CDC* pDC,
		CBCGPToolbarButton* pButton, CRect rect, CBCGPVisualManager::BCGBUTTON_STATE state);

	virtual void OnDrawButtonBorder (CDC* pDC,
		CBCGPToolbarButton* pButton, CRect rect, CBCGPVisualManager::BCGBUTTON_STATE state);

	virtual void OnDrawButtonSeparator (CDC* pDC,
		CBCGPToolbarButton* pButton, CRect rect, CBCGPVisualManager::BCGBUTTON_STATE state,
		BOOL bHorz);

	virtual void OnHighlightMenuItem (CDC *pDC, CBCGPToolbarMenuButton* pButton,
		CRect rect, COLORREF& clrText);
	virtual COLORREF GetHighlightedMenuItemTextColor (CBCGPToolbarMenuButton* pButton);
	virtual BOOL IsHighlightWholeMenuItem ()	{	return FALSE;	}
	
	virtual COLORREF GetMenuItemTextColor (CBCGPToolbarMenuButton* pButton, BOOL bHighlighted, BOOL bDisabled);
	virtual void OnHighlightRarelyUsedMenuItems (CDC* pDC, CRect rectRarelyUsed);

	virtual void OnFillMenuImageRect (CDC* pDC,
		CBCGPToolbarButton* pButton, CRect rect, CBCGPVisualManager::BCGBUTTON_STATE state)
	{
		OnFillButtonInterior (pDC, pButton, rect, state);
	}
		
	virtual void OnDrawMenuImageRectBorder (CDC* pDC,
		CBCGPToolbarButton* pButton, CRect rect, CBCGPVisualManager::BCGBUTTON_STATE state)
	{
		OnDrawButtonBorder (pDC, pButton, rect, state);
	}

	virtual void OnDrawMenuCheck (CDC* pDC, CBCGPToolbarMenuButton* pButton, 
		CRect rect, BOOL bHighlight, BOOL bIsRadio);

	virtual void OnDrawMenuItemButton (CDC* pDC, CBCGPToolbarMenuButton* pButton,
				CRect rectButton, BOOL bHighlight, BOOL bDisabled);

	virtual BOOL IsOwnerDrawMenuCheck ()	{	return FALSE;	}

	virtual COLORREF GetToolbarButtonTextColor (CBCGPToolbarButton* pButton,
												CBCGPVisualManager::BCGBUTTON_STATE state);

	virtual BOOL IsToolBarButtonDefaultBackground (CBCGPToolbarButton* pButton,
												CBCGPVisualManager::BCGBUTTON_STATE state);

	virtual COLORREF GetToolbarDisabledColor () const
	{
		return (COLORREF)-1;
	}

	virtual BOOL IsToolbarRoundShape (CBCGPToolBar* /*pToolBar*/)
	{
		return FALSE;
	}

	virtual COLORREF GetToolbarHighlightColor ();

	virtual COLORREF GetToolbarDisabledTextColor ();

	virtual int GetDockingBarCaptionExtraHeight () const
	{
		return 0;
	}

	// Caption bar:
	virtual COLORREF GetCaptionBarTextColor (CBCGPCaptionBar* pBar);
	virtual void OnDrawCaptionBarInfoArea (CDC* pDC, CBCGPCaptionBar* pBar, CRect rect);
	virtual void OnDrawCaptionBarBorder (CDC* pDC, CBCGPCaptionBar* pBar, CRect rect, COLORREF clrBarBorder, BOOL bFlatBorder);
	virtual COLORREF OnFillCaptionBarButton (CDC* pDC, CBCGPCaptionBar* pBar,
											CRect rect, BOOL bIsPressed, BOOL bIsHighlighted, 
											BOOL bIsDisabled, BOOL bHasDropDownArrow,
											BOOL bIsSysButton);
	virtual void OnDrawCaptionBarButtonBorder (CDC* pDC, CBCGPCaptionBar* pBar,
											CRect rect, BOOL bIsPressed, BOOL bIsHighlighted, 
											BOOL bIsDisabled, BOOL bHasDropDownArrow,
											BOOL bIsSysButton);

	// Outlook bar:
	virtual void OnFillOutlookPageButton (CDC* pDC, const CRect& rect,
										BOOL bIsHighlighted, BOOL bIsPressed,
										COLORREF& clrText);
	virtual void OnDrawOutlookPageButtonBorder (CDC* pDC, CRect& rectBtn, BOOL bIsHighlighted, BOOL bIsPressed);
	virtual void OnDrawOutlookBarSplitter (CDC* pDC, CRect rectSplitter);
	virtual void OnFillOutlookBarCaption (CDC* pDC, CRect rectCaption, COLORREF& clrText);

	// Tab overrides:
	virtual void OnEraseTabsArea (CDC* pDC, CRect rect, const CBCGPBaseTabWnd* pTabWnd);
	virtual void OnDrawTab (CDC* pDC, CRect rectTab,
							int iTab, BOOL bIsActive, const CBCGPBaseTabWnd* pTabWnd);
	virtual void OnFillTab (CDC* pDC, CRect rectFill, CBrush* pbrFill, int iTab, BOOL bIsActive, const CBCGPBaseTabWnd* pTabWnd);
	virtual void OnDrawTabContent (CDC* pDC, CRect rectTab,
							int iTab, BOOL bIsActive, const CBCGPBaseTabWnd* pTabWnd,
							COLORREF clrText);
	virtual void OnDrawTabCloseButton (CDC* pDC, CRect rect, 
											   const CBCGPBaseTabWnd* pTabWnd, 
											   BOOL bIsHighlighted,
											   BOOL bIsPressed,
											   BOOL bIsDisabled);
	virtual void OnEraseTabsButton (CDC* pDC, CRect rect, CBCGPButton* pButton,
									CBCGPBaseTabWnd* pWndTab);
	virtual void OnDrawTabsButtonBorder (CDC* pDC, CRect& rect, 
									CBCGPButton* pButton, UINT uiState, CBCGPBaseTabWnd* pWndTab);
	virtual void OnDrawTabResizeBar (CDC* pDC, CBCGPBaseTabWnd* pWndTab, 
									BOOL bIsVert, CRect rect,
									CBrush* pbrFace, CPen* pPen);
	virtual void GetTabFrameColors (const CBCGPBaseTabWnd* pTabWnd,
				   COLORREF& clrDark,
				   COLORREF& clrBlack,
				   COLORREF& clrHighlight,
				   COLORREF& clrFace,
				   COLORREF& clrDarkShadow,
				   COLORREF& clrLight,
				   CBrush*& pbrFace,
				   CBrush*& pbrBlack);
	virtual BOOL OnEraseTabsFrame (CDC* pDC, CRect rect, const CBCGPBaseTabWnd* pTabWnd);
	virtual BOOL IsHighlightOneNoteTabs () const	{	return FALSE;	}
	virtual BOOL AlwaysHighlight3DTabs () const		{	return FALSE;	}
	virtual COLORREF GetTabTextColor (const CBCGPBaseTabWnd* /*pTabWnd*/, int /*iTab*/, BOOL /*bIsActive*/)	{	return (COLORREF)-1;	}
	virtual int GetTabHorzMargin (const CBCGPBaseTabWnd* /*pTabWnd*/)
	{
		return 0;
	}

	virtual int GetMDITabsBordersSize ()		{	return -1;	/* Default */	}
	virtual BOOL IsMDITabsTopEdge ()			{	return TRUE;				}
	virtual int GetDockingTabsBordersSize ()	{	return -1;	/* Default */	}
	virtual BOOL IsDockingTabHasBorder ()		{	return TRUE;				}

	// Customization dialog:
	virtual COLORREF OnFillCommandsListBackground (CDC* pDC, CRect rect, BOOL bIsSelected = FALSE);
	virtual void OnDrawMenuArrowOnCustomizeList (CDC* pDC, CRect rectCommand, BOOL bSelected);

	virtual CSize GetButtonExtraBorder () const
	{
		return CSize (0, 0);
	}

	virtual CSize GetCaptionButtonExtraBorder () const
	{
		return CSize (0, 0);
	}

	virtual COLORREF OnFillMiniFrameCaption (CDC* pDC, CRect rectCaption, 
											CBCGPMiniFrameWnd* pFrameWnd,
											BOOL bActive);
	virtual void OnDrawMiniFrameBorder (CDC* pDC, CBCGPMiniFrameWnd* pFrameWnd,
										CRect rectBorder, CRect rectBorderSize);
	virtual void OnDrawFloatingToolbarBorder (	CDC* pDC, CBCGPBaseToolBar* pToolBar, 
												CRect rectBorder, CRect rectBorderSize);
	// Header control:
	virtual void OnFillHeaderCtrlBackground (CBCGPHeaderCtrl* pCtrl, CDC* pDC, CRect rect);
	virtual void OnDrawHeaderCtrlBorder (CBCGPHeaderCtrl* pCtrl, CDC* pDC,
								CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnDrawHeaderCtrlSortArrow (CBCGPHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsUp);
	void DoDrawHeaderSortArrow (CDC* pDC, CRect rect, BOOL bIsUp, BOOL bDlgCtrl);

	virtual COLORREF OnFillListBoxItem (CDC* pDC, CBCGPListBox* pListBox, int nItem, CRect rect, BOOL bIsHighlihted, BOOL bIsSelected);

	// Obsolete
	virtual COLORREF OnDrawPropSheetListItem (CDC*, CBCGPPropertySheet*, CRect, BOOL, BOOL) {	return (COLORREF)0;	}

	// Tasks pane:
#ifndef BCGP_EXCLUDE_TASK_PANE
	virtual void OnFillTasksPaneBackground(CDC* pDC, CRect rectWorkArea);

	virtual void OnDrawTasksGroupCaption(CDC* pDC, CBCGPTasksGroup* pGroup, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE, BOOL bCanCollapse = FALSE);
	virtual void OnDrawTasksGroupIcon(CDC* pDC, CBCGPTasksGroup* pGroup, int nIconHOffset = 5,
		BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE, BOOL bCanCollapse = FALSE);

	virtual void OnFillTasksGroupInterior(CDC* pDC, CRect rect, BOOL bSpecial = FALSE);
	virtual void OnDrawTasksGroupAreaBorder(CDC* pDC, CRect rect, BOOL bSpecial = FALSE, BOOL bNoTitle = FALSE);
	virtual void OnDrawTask(CDC* pDC, CBCGPTask* pTask, CImageList* pIcons, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE);

	virtual void OnDrawScrollButtons(CDC* pDC, const CRect& rect, const int nBorderSize,
									int iImage, BOOL bHilited);
	virtual void OnDrawToolBoxFrame (CDC* pDC, const CRect& rect);
#endif

	// Slider
	virtual void OnDrawSlider (CDC* pDC, CBCGPSlider* pSlider, CRect rect, BOOL bAutoHideMode);

	// Property list:
	virtual void OnDrawExpandingBox (CDC* pDC, CRect rect, BOOL bIsOpened, COLORREF colorBox);

#ifndef BCGP_EXCLUDE_PROP_LIST
	virtual COLORREF GetPropListGroupColor (CBCGPPropList* pPropList);
	virtual COLORREF GetPropListGroupTextColor (CBCGPPropList* pPropList);
#endif

	// Splitter:
	virtual void OnDrawSplitterBorder (CDC* pDC, CBCGPSplitterWnd* pSplitterWnd, CRect rect);
	virtual void OnDrawSplitterBox (CDC* pDC, CBCGPSplitterWnd* pSplitterWnd, CRect& rect);
	virtual void OnFillSplitterBackground (CDC* pDC, CBCGPSplitterWnd* pSplitterWnd, CRect rect);

	// Date/time controls:
	virtual COLORREF OnFillCalendarBarNavigationRow (CDC* pDC, CRect rect);

	virtual void GetCalendarColors (const CBCGPCalendar* pCalendar,
				   CBCGPCalendarColors& colors);

	virtual void OnDrawCheckBox (CDC *pDC, CRect rect, 
										 BOOL bHighlighted, 
										 BOOL bChecked,
										 BOOL bEnabled);

	virtual void OnDrawCheckBoxEx (CDC *pDC, CRect rect, 
										 int nState,
										 BOOL bHighlighted, 
										 BOOL bPressed,
										 BOOL bEnabled);

	virtual void OnDrawRadioButton (CDC *pDC, CRect rect, 
										 BOOL bOn,
										 BOOL bHighlighted, 
										 BOOL bPressed,
										 BOOL bEnabled);

	virtual void OnDrawControlBorder (CWnd* pWndCtrl);
	virtual void OnDrawControlBorder (CDC* pDC, CRect rect, CWnd* pWndCtrl, BOOL bDrawOnGlass);

	// Auto-hide buttons:
	virtual BOOL HasOverlappedAutoHideButtons () const
	{
		return FALSE;
	}

	virtual void OnFillAutoHideButtonBackground (CDC* pDC, CRect rect, CBCGPAutoHideButton* pButton);
	virtual void OnDrawAutoHideButtonBorder (CDC* pDC, CRect rectBounds, CRect rectBorderSize, CBCGPAutoHideButton* pButton);
	virtual COLORREF GetAutoHideButtonTextColor (CBCGPAutoHideButton* pButton);

	// Calculator:
	virtual BOOL OnDrawCalculatorButton (CDC* pDC, CRect rect, CBCGPToolbarButton* pButton, CBCGPVisualManager::BCGBUTTON_STATE state, int cmd /* CBCGPCalculator::CalculatorCommands */, CBCGPCalculator* pCalculator);
	virtual BOOL OnDrawCalculatorDisplay (CDC* pDC, CRect rect, const CString& strText, BOOL bMem, CBCGPCalculator* pCalculator);

	// Edit box:
	virtual BOOL OnDrawBrowseButton (CDC* pDC, CRect rect, CBCGPEdit* pEdit, CBCGPVisualManager::BCGBUTTON_STATE state, COLORREF& clrText);

	// Spin control:
	virtual void OnDrawSpinButtons(CDC* pDC, CRect rectSpin, int nState, BOOL bOrientation, CBCGPSpinButtonCtrl* pSpinCtrl);

	// Appbar window:
	virtual void OnDrawAppBarBorder (CDC* pDC, CBCGPAppBarWnd* pAppBarWnd,
									CRect rectBorder, CRect rectBorderSize);

	virtual void OnDrawAppBarCaption (	CDC* pDC, CBCGPAppBarWnd* pAppBarWnd, 
										CRect rectCaption, CString strCaption);

	// Windows XP drawing methods:
	virtual BOOL DrawPushButtonWinXP (CDC* /*pDC*/, CRect /*rect*/, CBCGPButton* /*pButton*/, UINT /*uiState*/)	{	return FALSE;	}
	virtual BOOL DrawComboDropButtonWinXP (CDC* /*pDC*/, CRect /*rect*/,
										BOOL /*bDisabled*/,
										BOOL /*bIsDropped*/,
										BOOL /*bIsHighlighted*/)	{	return FALSE;	}
	virtual BOOL DrawComboBorderWinXP (CDC* /*pDC*/, CRect /*rect*/,
										BOOL /*bDisabled*/,
										BOOL /*bIsDropped*/,
										BOOL /*bIsHighlighted*/)	{	return FALSE;	}

	// Toolbox:
#ifndef BCGP_EXCLUDE_TOOLBOX
	virtual BOOL OnEraseToolBoxButton (CDC* pDC, CRect rect, CBCGPToolBoxButton* pButton);
	virtual BOOL OnDrawToolBoxButtonBorder (CDC* pDC, CRect& rect, CBCGPToolBoxButton* pButton, UINT uiState);
#endif

	// Smart docking marker colors:
	virtual void GetSmartDockingBaseMarkerColors (
		COLORREF& clrBaseGroupBackground,
		COLORREF& clrBaseGroupBorder);

	virtual COLORREF GetSmartDockingMarkerToneColor ();

	// Popup window:
#ifndef BCGP_EXCLUDE_POPUP_WINDOW
	virtual void OnFillPopupWindowBackground (CDC* pDC, CRect rect);
	virtual void OnDrawPopupWindowBorder (CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPopupWindowCaption (CDC* pDC, CRect rectCaption, CBCGPPopupWindow* pPopupWnd);
	virtual void OnErasePopupWindowButton (CDC* pDC, CRect rectClient, CBCGPPopupWndButton* pButton);
	virtual void OnDrawPopupWindowButtonBorder (CDC* pDC, CRect rectClient, CBCGPPopupWndButton* pButton);
	virtual BOOL IsDefaultWinXPPopupButton (CBCGPPopupWndButton* /*pButton*/) const	{	return FALSE;	}
#endif

	// Day planner:
#ifndef BCGP_EXCLUDE_PLANNER
	virtual COLORREF OnFillPlannerCaption (CDC* pDC, CBCGPPlannerView* pView,
		CRect rect, BOOL bIsToday, BOOL bIsSelected, BOOL bNoBorder = FALSE, BOOL bHorz = TRUE);

	virtual void OnDrawPlannerCaptionText (CDC* pDC, 
		CBCGPPlannerView* pView, CRect rect, const CString& strText, 
		COLORREF clrText, int nAlign, BOOL bHighlight);

	virtual void OnFillPlanner (CDC* pDC, CBCGPPlannerView* pView, 
		CRect rect, BOOL bWorkingArea);

	virtual void GetPlannerAppointmentColors (CBCGPPlannerView* pView,
		BOOL bSelected, BOOL bSimple, DWORD dwDrawFlags, 
		COLORREF& clrBack1, COLORREF& clrBack2,
		COLORREF& clrFrame1, COLORREF& clrFrame2, COLORREF& clrText);

	virtual COLORREF GetPlannerAppointmentTimeColor (CBCGPPlannerView* pView,
		BOOL bSelected, BOOL bSimple, DWORD dwDrawFlags);

	virtual COLORREF GetPlannerHourLineColor (CBCGPPlannerView* pView,
		BOOL bWorkingHours, BOOL bHour);

	virtual COLORREF OnFillPlannerTimeBar (CDC* pDC, 
		CBCGPPlannerView* pView, CRect rect, COLORREF& clrLine);

	virtual void OnDrawPlannerTimeLine (CDC* pDC, CBCGPPlannerView* pView, CRect rect);

	virtual void OnFillPlannerWeekBar (CDC* pDC, 
		CBCGPPlannerView* pView, CRect rect);

	virtual DWORD GetPlannerDrawFlags () const
	{
		return 0;
	}

	virtual int GetPlannerRowExtraHeight () const
	{
		return 0;
	}

	virtual COLORREF GetPlannerWorkColor () const
	{
		return m_clrPlannerWork;
	}

	virtual void OnDrawPlannerHeader (CDC* pDC, 
		CBCGPPlannerView* pView, CRect rect);

	virtual void OnDrawPlannerHeaderPane (CDC* pDC, 
		CBCGPPlannerView* pView, CRect rect);

	virtual void OnFillPlannerHeaderAllDay (CDC* pDC, 
		CBCGPPlannerView* pView, CRect rect);
	virtual void OnDrawPlannerHeaderAllDayItem (CDC* pDC, 
		CBCGPPlannerView* pView, CRect rect, BOOL bIsToday, BOOL bIsSelected);

	virtual COLORREF GetPlannerSelectionColor (CBCGPPlannerView* pView);
	virtual COLORREF GetPlannerSeparatorColor (CBCGPPlannerView* pView);

	virtual void PreparePlannerBackItem (BOOL bIsToday, BOOL bIsSelected);
	virtual void PreparePlannerCaptionBackItem (BOOL bIsHeader);
	
#endif // BCGP_EXCLUDE_PLANNER

	// Grid control:
#ifndef BCGP_EXCLUDE_GRID_CTRL
	virtual void OnFillGridBackground (CDC* pDC, CRect rect);
	virtual void OnDrawGridExpandingBox (CDC* pDC, CRect rect, BOOL bIsOpened, COLORREF colorBox);
	virtual void OnFillGridHeaderBackground (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect);
	virtual BOOL OnDrawGridHeaderItemBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, BOOL bPressed);
	virtual void OnDrawGridSortArrow (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectArrow, BOOL bIsUp);
	virtual COLORREF OnFillGridGroupByBoxBackground (CDC* pDC, CRect rect);
	virtual COLORREF GetGridGroupByBoxLineColor () const;
	virtual void OnDrawGridGroupByBoxItemBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect);
	virtual COLORREF GetGridLeftOffsetColor (CBCGPGridCtrl* pCtrl);
	virtual COLORREF GetGridItemSortedColor (CBCGPGridCtrl* pCtrl);
	virtual void OnFillGridGroupBackground (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectFill);
	virtual void OnDrawGridGroupUnderline (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectFill);
	virtual COLORREF OnFillGridRowBackground (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectFill, BOOL bSelected);
	virtual COLORREF OnFillGridItem (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rectFill,
		BOOL bSelected, BOOL bActiveItem, BOOL bSortedColumn);
	virtual void OnDrawGridHeaderMenuButton (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect, 
		BOOL bHighlighted, BOOL bPressed, BOOL bDisabled);
	virtual void OnDrawGridSelectionBorder (CBCGPGridCtrl* pCtrl, CDC* pDC, CRect rect);

	virtual BOOL OnSetGridColorTheme (CBCGPGridCtrl* pCtrl, BCGP_GRID_COLOR_DATA& theme);

	// Report control:
	virtual COLORREF GetReportCtrlGroupBackgoundColor ();
	virtual COLORREF OnFillReportCtrlRowBackground (CBCGPGridCtrl* pCtrl, CDC* pDC,
		CRect rectFill, BOOL bSelected, BOOL bGroup);
#endif // BCGP_EXCLUDE_GRID_CTRL

	// Gantt control:
#ifndef BCGP_EXCLUDE_GANTT
	virtual void GetGanttColors (const CBCGPGanttChart* pChart, BCGP_GANTT_CHART_COLORS& colors, COLORREF clrBack = CLR_DEFAULT) const;
	virtual void DrawGanttChartBackground (const CBCGPGanttChart* pChart, CDC& dc, const CRect& rectChart, COLORREF clrFill);
	virtual void DrawGanttItemBackgroundCell (const CBCGPGanttChart* pChart, CDC& dc, const CRect& rectItem, const CRect& rectClip, const BCGP_GANTT_CHART_COLORS& colors, BOOL bDayOff);
	virtual void DrawGanttHeaderCell (const CBCGPGanttChart* pChart, CDC& dc, const BCGP_GANTT_CHART_HEADER_CELL_INFO& cellInfo, BOOL bHilite);
	virtual void DrawGanttHeaderText (const CBCGPGanttChart* pChart, CDC& dc, const BCGP_GANTT_CHART_HEADER_CELL_INFO& cellInfo, const CString& sCellText, BOOL bHilite) const;

	virtual void FillGanttBar (const CBCGPGanttItem* pItem, CDC& dc, const CRect& rectFill, COLORREF color, double dGlowLine);

#endif //BCGP_EXCLUDE_GANTT

	// Window apperance:
	virtual BOOL IsOwnerDrawCaption ()	{	return FALSE;	}
	virtual BOOL OnSetWindowRegion (CWnd* pWnd, CSize sizeWindow);
	virtual void OnActivateApp(CWnd* pWnd, BOOL bActive);
	virtual BOOL OnNcPaint (CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);
	virtual BOOL OnNcActivate (CWnd* pWnd, BOOL bActive);
	virtual CSize GetNcBtnSize (BOOL bSmall) const;

	// Dialog:
	virtual BOOL OnFillDialog (CDC* pDC, CWnd* pDlg, CRect rect);
	virtual CBrush& GetDlgBackBrush (CWnd* pDlg);

#ifndef BCGP_EXCLUDE_RIBBON
	// Ribbon control:
	virtual COLORREF OnDrawRibbonTabsFrame (
					CDC* pDC, 
					CBCGPRibbonBar* pWndRibbonBar, 
					CRect rectTab);
	
	virtual void OnDrawRibbonMainButton (
					CDC* pDC, 
					CBCGPRibbonButton* pButton);

	virtual void OnDrawRibbonCategory (
					CDC* pDC, 
					CBCGPRibbonCategory* pCategory, 
					CRect rectCategory);

	virtual void OnDrawRibbonCategoryScroll (
					CDC* pDC, 
					CBCGPRibbonCategoryScroll* pScroll);

	virtual COLORREF OnDrawRibbonCategoryTab (
					CDC* pDC, 
					CBCGPRibbonTab* pTab, 
					BOOL bIsActive);

	virtual COLORREF OnDrawRibbonPanel (
					CDC* pDC,
					CBCGPRibbonPanel* pPanel, 
					CRect rectPanel,
					CRect rectCaption);

	virtual void OnDrawRibbonPanelCaption (
					CDC* pDC,
					CBCGPRibbonPanel* pPanel, 
					CRect rectCaption);

	virtual COLORREF OnFillRibbonPanelCaption (
					CDC* pDC,
					CBCGPRibbonPanel* pPanel, 
					CRect rectCaption);

	virtual void OnDrawRibbonLaunchButton (
					CDC* pDC,
					CBCGPRibbonLaunchButton* pButton,
					CBCGPRibbonPanel* pPanel);

	virtual void OnDrawRibbonDefaultPaneButton (
					CDC* pDC,
					CBCGPRibbonButton* pButton);

	virtual void OnDrawRibbonDefaultPaneButtonContext (
					CDC* pDC,
					CBCGPRibbonButton* pButton);

	virtual void OnDrawRibbonDefaultPaneButtonIndicator (
					CDC* pDC, 
					CBCGPRibbonButton* pButton,
					CRect rect, 
					BOOL bIsSelected, 
					BOOL bHighlighted);

	virtual COLORREF OnFillRibbonButton (
					CDC* pDC, 
					CBCGPRibbonButton* pButton);

	virtual void OnFillRibbonEdit (
					CDC* pDC, 
					CBCGPRibbonEditCtrl* pEdit,
					CRect rect,
					BOOL bIsHighlighted,
					BOOL bIsPaneHighlighted,
					BOOL bIsDisabled,
					COLORREF& clrText, COLORREF& clrSelBackground, COLORREF& clrSelText);

	virtual COLORREF GetRibbonEditBackgroundColor (
					CBCGPRibbonEditCtrl* pEdit,
					BOOL bIsHighlighted,
					BOOL bIsPaneHighlighted,
					BOOL bIsDisabled);

	virtual void OnDrawRibbonButtonBorder (
					CDC* pDC, 
					CBCGPRibbonButton* pButton);

	virtual void OnDrawRibbonMenuCheckFrame (
					CDC* pDC,
					CBCGPRibbonButton* pButton, 
					CRect rect);

	virtual COLORREF OnFillRibbonMainPanelButton (
					CDC* pDC, 
					CBCGPRibbonButton* pButton);

	virtual void OnDrawRibbonMainPanelButtonBorder (
					CDC* pDC, 
					CBCGPRibbonButton* pButton);

	virtual void OnDrawRibbonCaption (
					CDC* pDC, CBCGPRibbonBar* pBar, CRect rect,
					CRect rectText);

	virtual int GetRibbonQATChevronOffset ()	{	return 0;	}

	virtual int GetRibbonQATRightMargin ()		{	return 0;	}

	virtual COLORREF GetRibbonQATTextColor (BOOL bDisabled = FALSE)	
	{	
		return bDisabled ? GetToolbarDisabledTextColor () : (COLORREF)-1;
	}

	virtual void OnDrawRibbonCaptionButton (
					CDC* pDC, CBCGPRibbonCaptionButton* pButton);

	virtual COLORREF OnDrawRibbonButtonsGroup (
					CDC* pDC,
					CBCGPRibbonButtonsGroup* pGroup,
					CRect rectGroup);

	virtual void OnDrawDefaultRibbonImage (
					CDC* pDC, 
					CRect rectImage,
					BOOL bIsDisabled = FALSE,
					BOOL bIsPressed = FALSE,
					BOOL bIsHighlighted = FALSE);

	virtual void OnDrawRibbonMainPanelFrame (
					CDC* pDC, 
					CBCGPRibbonMainPanel* pPanel, 
					CRect rect);

	virtual void OnFillRibbonMenuFrame (
					CDC* pDC, 
					CBCGPRibbonMainPanel* pPanel, 
					CRect rect);

	virtual void OnDrawRibbonRecentFilesFrame (
					CDC* pDC, 
					CBCGPRibbonMainPanel* pPanel, 
					CRect rect);

	virtual void OnDrawRibbonLabel (
					CDC* pDC, 
					CBCGPRibbonLabel* pLabel,
					CRect rect);

	virtual void OnDrawRibbonPaletteButton (
					CDC* pDC,
					CBCGPRibbonPaletteIcon* pButton);

	virtual void OnDrawRibbonPaletteBorder (
					CDC* pDC, 
					CBCGPRibbonPaletteButton* pButton, 
					CRect rectBorder);

	virtual COLORREF RibbonCategoryColorToRGB (BCGPRibbonCategoryColor color);

	virtual COLORREF OnDrawRibbonCategoryCaption (
					CDC* pDC, 
					CBCGPRibbonContextCaption* pContextCaption);

	virtual COLORREF OnDrawRibbonStatusBarPane (CDC* pDC, CBCGPRibbonStatusBar* pBar,
					CBCGPRibbonStatusBarPane* pPane);

	virtual void GetRibbonSliderColors (CBCGPRibbonSlider* pSlider, 
					BOOL bIsHighlighted, 
					BOOL bIsPressed,
					BOOL bIsDisabled,
					COLORREF& clrLine,
					COLORREF& clrFill);

	virtual void OnDrawRibbonSliderZoomButton (
			CDC* pDC, CBCGPRibbonSlider* pSlider, 
			CRect rect, BOOL bIsZoomOut, 
			BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);

	virtual void OnDrawRibbonSliderChannel (
			CDC* pDC, CBCGPRibbonSlider* pSlider, 
			CRect rect);

	virtual void OnDrawRibbonSliderThumb (
			CDC* pDC, CBCGPRibbonSlider* pSlider, 
			CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);

	virtual void OnDrawRibbonProgressBar (
			CDC* pDC, CBCGPRibbonProgressBar* pProgress, 
			CRect rectProgress, CRect rectChunk, BOOL bInfiniteMode);

	virtual void OnFillRibbonQATPopup (
				CDC* pDC, CBCGPRibbonPanelMenuBar* pMenuBar, CRect rect);

	virtual int GetRibbonPopupBorderSize (const CBCGPRibbonPanelMenu* /*pPopup*/) const
	{
		return m_nMenuBorderSize;
	}

	virtual void OnDrawRibbonQATSeparator (CDC* pDC, CBCGPRibbonSeparator* pSeparator, CRect rect);

	virtual BOOL IsLayeredRibbonKeyTip () const	{	return FALSE;	}
	virtual void OnDrawRibbonKeyTip (CDC* pDC, CBCGPBaseRibbonElement* pElement, CRect rect, CString str);

	virtual void OnDrawRibbonCheckBoxOnList (CDC* pDC, CBCGPRibbonCheckBox* pCheckBox, CRect rect, BOOL bIsSelected, BOOL bHighlighted);
	virtual void OnDrawRibbonRadioButtonOnList (CDC* pDC, CBCGPRibbonRadioButton* pRadioButton, CRect rect, BOOL bIsSelected, BOOL bHighlighted);

	virtual COLORREF GetRibbonHyperlinkTextColor (CBCGPRibbonHyperlink* pHyperLink);
	virtual COLORREF GetRibbonStatusBarTextColor (CBCGPRibbonStatusBar* pStatusBar);

	virtual void OnDrawRibbonColorPaletteBox (CDC* pDC, CBCGPRibbonColorButton* pColorButton,
		CBCGPRibbonPaletteIcon* pIcon,
		COLORREF color, CRect rect, BOOL bDrawTopEdge, BOOL bDrawBottomEdge,
		BOOL bIsHighlighted, BOOL bIsChecked, BOOL bIsDisabled);

#endif

	// Vista support:
	virtual BOOL DrawTextOnGlass (CDC* pDC, CString strText, CRect rect, DWORD dwFlags, int nGlowSize = 0,
		COLORREF clrText = (COLORREF)-1);

	// MDI Client area
	virtual BOOL OnEraseMDIClientArea (CDC* pDC, CRect rectClient);

	// ToolTip
	virtual BOOL GetToolTipParams (CBCGPToolTipParams& params, UINT nType = (UINT)(-1));
	virtual void OnFillToolTip (CDC* pDC, CBCGPToolTipCtrl* pToolTip, CRect rect, COLORREF& clrText, COLORREF& clrLine);

	// Scrollbar
	virtual BOOL IsOwnerDrawScrollBar () const
	{
		return FALSE;
	}

	virtual void OnScrollBarDrawThumb (CDC* /*pDC*/, CBCGPScrollBar* /*pScrollBar*/, CRect /*rect*/, 
		BOOL /*bHorz*/, BOOL /*bHighlighted*/, BOOL /*bPressed*/, BOOL /*bDisabled*/)
	{
		ASSERT (FALSE);
	}
					
	virtual void OnScrollBarDrawButton (CDC* /*pDC*/, CBCGPScrollBar* /*pScrollBar*/, CRect /*rect*/, 
		BOOL /*bHorz*/, BOOL /*bHighlighted*/, BOOL /*bPressed*/, BOOL /*bFirst*/, BOOL /*bDisabled*/)
	{
		ASSERT (FALSE);
	}

	virtual void OnScrollBarFillBackground (CDC* /*pDC*/, CBCGPScrollBar* /*pScrollBar*/, CRect /*rect*/, 
		BOOL /*bHorz*/, BOOL /*bHighlighted*/, BOOL /*bPressed*/, BOOL /*bFirst*/, BOOL /*bDisabled*/)
	{
		ASSERT (FALSE);
	}

	// Push button:
	virtual BOOL OnDrawPushButton (CDC* /*pDC*/, CRect /*rect*/, CBCGPButton* /*pButton*/, COLORREF& /*clrText*/)	{	return FALSE;	}
	virtual COLORREF GetURLLinkColor (CBCGPURLLinkButton* pButton, BOOL bHover);

	// Group:
	virtual void OnDrawGroup (CDC* pDC, CBCGPGroup* pGroup, CRect rect, const CString& strName);

	// Slider:
	virtual void OnDrawSliderChannel (CDC* pDC, CBCGPSliderCtrl* pSlider, BOOL bVert, CRect rect, BOOL bDrawOnGlass);

	virtual void OnDrawSliderThumb (CDC* pDC, CBCGPSliderCtrl* pSlider, 
			CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled,
			BOOL bVert, BOOL bLeftTop, BOOL bRightBottom,
			BOOL bDrawOnGlass);

// Attributes:
public:

	void SetMenuShadowDepth (int nDepth)	// Pixels
	{
		m_nMenuShadowDepth = nDepth;
	}

	int GetMenuShadowDepth () const
	{
		return m_nMenuShadowDepth;
	}
	
	static CBCGPVisualManager* GetInstance ()
	{
		if (m_pVisManager != NULL)
		{
			ASSERT_VALID (m_pVisManager);
			return m_pVisManager;
		}

		if (m_pRTIDefault == NULL)
		{
			m_pVisManager = new CBCGPVisualManager;
		}
		else
		{
			m_pVisManager = (CBCGPVisualManager*) m_pRTIDefault->CreateObject ();
			ASSERT_VALID (m_pVisManager);
		}

		m_pVisManager->m_bAutoDestroy = TRUE;
		m_pVisManager->OnUpdateSystemColors ();

		return m_pVisManager;
	}

	BOOL IsLook2000 () const			{	return m_bLook2000; }
	void SetLook2000 (BOOL bLook2000 = TRUE);

	BOOL IsMenuFlatLook () const			{	return m_bMenuFlatLook; }
	void SetMenuFlatLook (BOOL bMenuFlatLook = TRUE)
	{
		m_bMenuFlatLook = bMenuFlatLook;
		RedrawAll ();
	}

	BOOL IsAutoDestroy () const
	{
		return m_bAutoDestroy;
	}

	void SetShadowHighlightedImage (BOOL bShadow = TRUE)
	{
		m_bShadowHighlightedImage = bShadow;
	}

	BOOL IsShadowHighlightedImage () const
	{
		return m_bShadowHighlightedImage;
	}

	void EnableToolbarButtonFill (BOOL bEnable = TRUE)
	{
		m_bEnableToolbarButtonFill = bEnable;
	}

	BOOL IsToolbarButtonFillEnabled () const
	{
		return m_bEnableToolbarButtonFill;
	}

	BOOL IsEmbossDisabledImage () const
	{
		return m_bEmbossDisabledImage;
	}

	void SetEmbossDisabledImage (BOOL bEmboss = TRUE)
	{
		m_bEmbossDisabledImage = bEmboss;
	}

	BOOL IsFadeInactiveImage () const
	{
		return m_bFadeInactiveImage;
	}

	void SetFadeInactiveImage (BOOL bFade = TRUE)
	{
		m_bFadeInactiveImage = bFade;
	}

	virtual int GetMenuImageMargin () const
	{
		return 2;
	}

	virtual COLORREF GetMenuShadowBaseColor ()
	{
		return m_clrMenuShadowBase;
	}

	virtual CRect GetMenuImageFrameOffset () const
	{
		return CRect (2, 1, 0, 2);
	}

	virtual int GetPopupMenuGap () const
	{
		return 1;
	}

	virtual BOOL IsLook2000Allowed () const
	// Allows choose "Look 2000" in the customization dialog
	{
		return TRUE;
	}

	// TasksPane:
	int GetTasksPaneVertMargin() const
	{
		return m_nVertMargin;
	}

	int GetTasksPaneHorzMargin() const
	{
		return m_nHorzMargin;
	}

	int GetTasksPaneGroupVertOffset() const
	{
		return m_nGroupVertOffset;
	}

	int GetTasksPaneGroupCaptionHeight() const
	{
		return m_nGroupCaptionHeight;
	}

	int GetTasksPaneGroupCaptionHorzOffset() const
	{
		return m_nGroupCaptionHorzOffset;
	}

	int GetTasksPaneGroupCaptionVertOffset() const
	{
		return m_nGroupCaptionVertOffset;
	}

	int GetTasksPaneTaskHorzOffset() const
	{
		return m_nTasksHorzOffset;
	}

	int GetTasksPaneIconHorzOffset() const
	{
		return m_nTasksIconHorzOffset;
	}

	int GetTasksPaneIconVertOffset() const
	{
		return m_nTasksIconVertOffset;
	}

	virtual int GetToolBarCustomizeButtonMargin () const
	{
		return 2;
	}

	virtual BOOL IsOffsetPressedButton () const
	{
		return TRUE;
	}

	virtual BOOL IsOfficeXPStyleMenus () const
	{
		return m_bOfficeXPStyleMenus;
	}

	virtual BOOL GetPopupMenuBorderSize () const
	{
		return m_nMenuBorderSize;
	}

	BOOL IsFrameMenuCheckedItems () const
	{
		return m_bFrameMenuCheckedItems;
	}

	// "Show all menu items" methods:
	virtual int GetShowAllMenuItemsHeight (CDC* pDC, const CSize& sizeDefault);
	virtual void OnDrawShowAllMenuItems (CDC* pDC, CRect rect, CBCGPVisualManager::BCGBUTTON_STATE state);

protected:
	static CBCGPVisualManager* CreateVisualManager (CRuntimeClass* pVisualManager);

// Attributes:
protected:
	static CRuntimeClass*		m_pRTIDefault;
	static CBCGPVisualManager*	m_pVisManager;

	BOOL	m_bLook2000;				// Single grippers
	int		m_nMenuShadowDepth;
	BOOL	m_bMenuFlatLook;			// Menu item is always stil unpressed
	BOOL	m_bShadowHighlightedImage;
	BOOL	m_bEmbossDisabledImage;
	BOOL	m_bFadeInactiveImage;
	BOOL	m_bEnableToolbarButtonFill;
	BOOL	m_bFrameMenuCheckedItems;

	BOOL	m_bIsTemporary;

	int		m_nVertMargin;
	int		m_nHorzMargin;
	int		m_nGroupVertOffset;
	int		m_nGroupCaptionHeight;
	int		m_nGroupCaptionHorzOffset;
	int		m_nGroupCaptionVertOffset;
	int		m_nTasksHorzOffset;
	int		m_nTasksIconHorzOffset;
	int		m_nTasksIconVertOffset;
	BOOL	m_bActiveCaptions;

	COLORREF	m_clrReportGroupText;

	BOOL	m_bOfficeXPStyleMenus;
	int		m_nMenuBorderSize;

	BOOL	m_bAlwaysFillTab;
	BOOL	m_b3DTabWideBorder;

	COLORREF	m_clrMenuShadowBase;
	COLORREF	m_clrPlannerWork;
	COLORREF	m_clrPalennerLine;

	CBrush		m_brPlanner;

	BOOL		m_bPlannerBackItemToday;
	BOOL		m_bPlannerBackItemSelected;
	BOOL		m_bPlannerCaptionBackItemHeader;

private:
	BOOL	m_bAutoDestroy;
};

BCGCBPRODLLEXPORT extern UINT BCGM_CHANGEVISUALMANAGER;

#endif // !defined(AFX_BCGPVISUALMANAGER_H__8070C021_60CE_11D5_A304_8B7130518C10__INCLUDED_)
