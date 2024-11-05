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
// BCGPGanttControl.h: interface for the CBCGPGanttControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPGANTTCONTROL_H__2F17CFFE_D0FB_4A23_8AB5_6AA767BF13B7__INCLUDED_)
#define AFX_BCGPGANTTCONTROL_H__2F17CFFE_D0FB_4A23_8AB5_6AA767BF13B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGPWnd.h"
#include "BCGPGanttGrid.h"

#if (!defined _BCGSUITE_) && !defined (_BCGSUITE_INC_)
#include "BCGPSplitterWnd.h"
#endif

#include "BCGPGanttChart.h"

class BCGCBPRODLLEXPORT CBCGPGanttControl : public CBCGPWnd
{
    friend class CBCGPGanttGrid;
    friend class CBCGPGanttChart;
	friend class CBCGPGanttView;

	DECLARE_DYNCREATE (CBCGPGanttControl)

public:
	CBCGPGanttControl();
	virtual ~CBCGPGanttControl();

    CBCGPGanttGrid*         GetGrid ();
    CBCGPGanttChart*        GetChart ();
    CBCGPSplitterWnd*       GetSplitter ();

    const CBCGPGanttGrid*   GetGrid () const;
    const CBCGPGanttChart*  GetChart () const;
    const CBCGPSplitterWnd* GetSplitter () const;

    virtual BOOL Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual int  CreateControls ();

	void SetScrollBarsStyle (CBCGPScrollBar::BCGPSB_STYLE style);

    void SetNameColumnIndex (int nColumn);
    int GetNameColumnIndex () const
    {
        return m_nItemNameColumn;
    }

    void SetStartDateColumnIndex (int nColumn);
    int GetStartDateColumnIndex () const
    {
        return m_nItemStartDateColumn;
    }

    void SetFinishDateColumnIndex (int nColumn);
    int GetFinishDateColumnIndex () const
    {
        return m_nItemFinishDateColumn;
    }

    void SetProgressColumnIndex (int nColumn);
    int GetProgressColumnIndex () const
    {
        return m_nItemProgressColumn;
    }

    virtual CBCGPGanttItem* ItemFromRow (int nRow) const;
    virtual int RowFromItem (CBCGPGanttItem* pItem) const;

// Operations

    void    IndentItem (CBCGPGanttItem* pItem);
    void    OutdentItem (CBCGPGanttItem* pItem);

    BOOL    CanIndentItem (CBCGPGanttItem* pItem) const;
    BOOL    CanOutdentItem (CBCGPGanttItem* pItem) const;

    // Synchronizes grid with gantt item storage
    virtual void UpdateGrid ();

    BOOL IsProcessingGridNotification () const;
    BOOL IsProcessingChartNotification () const;

    virtual void AdjustLayout ();

    virtual BOOL LoadState (LPCTSTR lpszProfileName, int nIndex = 0);
    virtual BOOL SaveState (LPCTSTR lpszProfileName, int nIndex = 0);

protected:
    virtual void DoVerticalResize  (CWnd* pSender);
    virtual void DoVerticalScroll  (CWnd* pSender, int nVerticalOffset);
    virtual void DoSelectionChanged(CWnd* pSender, CBCGPGanttItem* pItem, BOOL bSelected);
    virtual void DoItemShow        (CWnd* pSender, CBCGPGanttItem* pItem, BOOL bVisible);
    virtual void DoItemsChange     (CWnd* pSender, const BCGP_GANTT_STORAGE_UPDATE_INFO& updateInfo);
    virtual void DoExpandGroup     (CWnd* pSender, CBCGPGridRow* pRow, BOOL bExpand);


    void UpdateGridItem (CBCGPGanttItem* pItem, CBCGPGridRow* pRow);
    void CollapseGroup (CBCGPGridRow* pParentRow);
    void ExpandGroup (CBCGPGridRow* pParentRow);

    BOOL StartProcessNotification (CWnd* pSender);
    void FinishProcessNotification ();

    virtual void OnGridItemChanged (CBCGPGridItem* pGridItem, int nRow, int nColumn);

    // Generated message map functions
protected:
    //{{AFX_MSG(CBCGPGanttControl)
    afx_msg int  OnCreate (LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd (CDC* pDC);
    afx_msg void OnPaint ();
    afx_msg void OnSize (UINT nType, int cx, int cy);
    afx_msg void OnSetFocus (CWnd* pOldWnd);
    afx_msg LRESULT OnGetFont (WPARAM, LPARAM);
    afx_msg LRESULT OnSetFont (WPARAM, LPARAM);
    afx_msg LRESULT OnNotifyStorageChanged (WPARAM, LPARAM);
    afx_msg void OnDestroy ();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    LRESULT WindowProc (UINT message, WPARAM wParam, LPARAM lParam);

protected:
    CBCGPSplitterWnd    m_wndSplitter;
    CBCGPGanttGrid      m_wndGrid;
    CBCGPGanttChart     m_wndChart;

    // Bound column id's
    int                 m_nItemNameColumn;
    int                 m_nItemStartDateColumn;
    int                 m_nItemFinishDateColumn;
    int                 m_nItemProgressColumn;

private:
    int                 m_nState;
};


class CBCGPGridRow;
class CBCGPGanttItem;

struct BCGP_GANTT_CONTROL_ROW 
{
    CBCGPGridRow*       pGridRow;
    CBCGPGanttItem*     pItem;
};

// Sent when the grid data must be synchronized with chart item.
// Fill the the grid cells in message handler.
//  wParam: not used, must be 0
//  lParam: pointer to BCGP_GANTT_CONTROL_ROW struct
//  return value: TRUE if message has been handled
BCGCBPRODLLEXPORT extern const UINT BCGM_GANTT_CONTROL_READ_ITEM_DATA_FROM_GRID;

// Sent when the chart item data must be synchronized with grid.
// Read from grid cells and write to specified Gantt item in message handler.
//  wParam: not used, must be 0
//  lParam: pointer to BCGP_GANTT_CONTROL_ROW struct
//  return value: TRUE if message has been handled
BCGCBPRODLLEXPORT extern const UINT BCGM_GANTT_CONTROL_WRITE_ITEM_DATA_TO_GRID;


#endif // !defined(AFX_BCGPGANTTCONTROL_H__2F17CFFE_D0FB_4A23_8AB5_6AA767BF13B7__INCLUDED_)
