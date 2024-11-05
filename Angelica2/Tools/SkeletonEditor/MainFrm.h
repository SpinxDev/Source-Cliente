 // MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__1B27E641_73DB_42B3_A547_4DC5D403AF4D__INCLUDED_)
#define AFX_MAINFRM_H__1B27E641_73DB_42B3_A547_4DC5D403AF4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolTabWnd.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

struct MAINFRAMEUPDATE
{
	//	File menu items
	BYTE	bFile_New;			//	File->New
	BYTE	bFile_Open;			//	File->Open
	BYTE	bFile_Save;			//	File->Save
	BYTE	bFile_SaveAs;		//	File->Save as
	BYTE	bFile_Exit;			//	File->Exit
};

extern MAINFRAMEUPDATE	g_MainFrmUpdate;

class CSkeletonEditorView;
class CSkeletonEditorDoc;
class CModel;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

void UF_Initialize();

///////////////////////////////////////////////////////////////////////////
//
//	Class CSkeletonEditorView
//
///////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:		//	Attributes

public:		//	Operations

	//	Get document and view
	CSkeletonEditorDoc* GetDocument();
	CSkeletonEditorView* GetView();

	//	Get tool tab window
	CToolTabWnd* GetToolTabWnd() { return &m_ToolTabWnd; }

	//	Bind model
	bool BindModel(CModel* pModel);

	//	Application is active ?
	bool AppIsActive() { return m_bActive; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  //	Attributes

	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CToolTabWnd		m_ToolTabWnd;

	bool	m_bActive;		//	App active flag

protected:	//	Operations

	//	Initialize tool tab window
	bool InitToolTabWnd();

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnViewSkeletononly();
	afx_msg void OnUpdateViewSkeletononly(CCmdUI* pCmdUI);
	afx_msg void OnViewPhysiqueshape();
	afx_msg void OnUpdateViewPhysiqueshape(CCmdUI* pCmdUI);
	afx_msg void OnViewBoundbox();
	afx_msg void OnUpdateViewBoundbox(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditChangeskin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditAddphysique(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRemovephysique(CCmdUI* pCmdUI);
	afx_msg void OnEditActfactory();
	afx_msg void OnViewGround();
	afx_msg void OnUpdateViewGround(CCmdUI* pCmdUI);
	afx_msg void OnViewNormal();
	afx_msg void OnUpdateViewNormal(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__1B27E641_73DB_42B3_A547_4DC5D403AF4D__INCLUDED_)
