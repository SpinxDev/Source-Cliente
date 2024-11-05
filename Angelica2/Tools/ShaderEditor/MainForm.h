#if !defined(AFX_MAINFORM_H__89E3E539_179A_4B99_8483_CF64DC4DBF3D__INCLUDED_)
#define AFX_MAINFORM_H__89E3E539_179A_4B99_8483_CF64DC4DBF3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainForm.h : header file
//

#include "ShaderForm.h"
#include "PropertyForm.h"
#include "RenderWnd.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// 
//	CMainForm frame
//
/////////////////////////////////////////////////////////////////////////////

class CMainForm : public CFrameWnd
{
	DECLARE_DYNCREATE(CMainForm)
public:
	CMainForm();           // protected constructor used by dynamic creation

// Types
public:

// Attributes
public:

// Operations
public:

	CRenderWnd*		GetRenderWnd()		{	return &m_RenderWnd;	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainForm)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainForm();

	CShaderForm*	GetShaderForm();		//	Get shader form
	CPropertyForm*	GetPropertyForm();		//	Get property form

	// Generated message map functions
	//{{AFX_MSG(CMainForm)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateFileExit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnFileExit();
	afx_msg void OnDestroy();
	afx_msg void OnFileSetworkpath();
	afx_msg void OnUpdateFileSaveall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSetworkpath(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveall();
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnOptBgcolor();
	afx_msg void OnOptBgtexture();
	afx_msg void OnOptVertcolor();
	afx_msg void OnOptD3dtexture();
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void			OnSysCommand(WPARAM wParam, LPARAM lParam);

	//	Own message handler
	void			OnSelectChange(WPARAM wParam, LPARAM lParam);
	void			OnAddOneStage(WPARAM wParam, LPARAM lParam);

protected:	//	Attributes

	CSplitterWnd	m_SplitWnd;			//	Splitter window
	CShaderForm*	m_pShaderForm;		//	Shader list from
	CPropertyForm*	m_pPropForm;		//	Shader properties form
	CRenderWnd		m_RenderWnd;		//	Render window

protected:	//	Operations

	void			InitMainMenuState();	//	Initialize main menu's state
	bool			InitRenderWnd();		//	Initialize render window
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFORM_H__89E3E539_179A_4B99_8483_CF64DC4DBF3D__INCLUDED_)
