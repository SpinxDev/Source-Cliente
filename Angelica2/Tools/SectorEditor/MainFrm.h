// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__46CFEB00_8E5F_4830_A350_3F44BD228C22__INCLUDED_)
#define AFX_MAINFRM_H__46CFEB00_8E5F_4830_A350_3F44BD228C22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "A3DTypes.h"
#include "DlgMessage.h"

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

class CSectorEditorDoc;
class CSectorEditorView;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

void UF_Initialize();

///////////////////////////////////////////////////////////////////////////
//
//	Class CMainFrame
//
///////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:		//	Types

	//	Minimal size of frame window
	enum
	{
		MIN_WIDTH = 300,
		MIN_HEIGHT = 300,
	};

public:		//	Attributes

public:		//	Operations

	//	Get document and view
	CSectorEditorDoc* GetDocument();
	CSectorEditorView* GetView();

	CDlgMessage* GetMessageWnd() { return &m_MessageWnd; }

	//	Show/Hide message window
	void ShowMessageWnd(bool bShow);

	//	Application is active ?
	bool AppIsActive() { return m_bActive; }

	//	Update mouse position on status bar
	void UpdateMousePos(A3DVECTOR3* pvPos);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  //	Attributes

	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CDlgMessage	m_MessageWnd;

	bool		m_bActive;		//	App active flag

protected:	//	Operations

// Generated message map functions
protected:

	//	Initialize message dialog bar
	bool InitMessageDlgBar();

	afx_msg void OnEndOperation(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCameraMoveLevel(UINT nID);
	afx_msg void OnCameraRotateLevel(UINT nID);
	afx_msg void OnUpdateCameraMoveLevel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCameraRotateLevel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowHideFlag(CCmdUI* pCmdUI);

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnUpdateViewForcerender(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFullclip(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTest(CCmdUI* pCmdUI);
	afx_msg void OnEditViewinfo();
	afx_msg void OnViewMessagewnd();
	afx_msg void OnUpdateViewMessagewnd(CCmdUI* pCmdUI);
	afx_msg void OnEditOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__46CFEB00_8E5F_4830_A350_3F44BD228C22__INCLUDED_)
