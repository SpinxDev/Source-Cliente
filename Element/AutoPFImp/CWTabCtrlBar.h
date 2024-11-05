// CWTabCtrlBar.h : header file
#if !defined(CWTABCTRLBAR_H_INCLUDED)
#define CWTABCTRLBAR_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "CWControlBar.h"

typedef struct
{
	CWnd *pWnd;
	char szLabel[32];
}TCB_ITEM;

#define   WM_USER_TABCHANGE  (WM_USER + 5000)

/////////////////////////////////////////////////////////////////////////////
// CCWTabCtrlBar tab control bar

class CCWTabCtrlBar : public CCWControlBar
{
// Construction
public:
	CCWTabCtrlBar();

// Attributes
public:
	CTabCtrl m_tabctrl;
	CToolTipCtrl* m_ToolTip;

protected:
	int m_nActiveTab;
	CView* m_pActiveView;
	CList <TCB_ITEM *,TCB_ITEM *> m_views;

// Operations
public:
	void SetActiveView(int nNewTab);
	void SetActiveView(CRuntimeClass *pViewClass);
	CImageList* SetTabImageList(CImageList *pImageList);
	BOOL ModifyTabStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags);

// Overrides
public:
    // ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCWTabCtrlBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	CView* GetActiveView();
	CView* GetView(int nView);
	CView* GetView(CRuntimeClass *pViewClass);
	BOOL AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CCreateContext *pContext = NULL);
	void RemoveView(int nView);
	virtual ~CCWTabCtrlBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CCWTabCtrlBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // CWTABCTRLBAR_H_INCLUDED
