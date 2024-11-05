// SkeletonEditor.h : main header file for the SKELETONEDITOR application
//

#if !defined(AFX_SKELETONEDITOR_H__6E5110B1_1011_4247_B774_F53CB6990433__INCLUDED_)
#define AFX_SKELETONEDITOR_H__6E5110B1_1011_4247_B774_F53CB6990433__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CMainFrame;

/////////////////////////////////////////////////////////////////////////////
// CSkeletonEditorApp:
// See SkeletonEditor.cpp for the implementation of this class
//

class CSkeletonEditorApp : public CWinApp
{
public:
	CSkeletonEditorApp();

public:

	//	Discard frame
	void DiscardFrame();
	//	Get tick time
	DWORD GetTickTime() { return m_dwTickTime; }
	//	Get main frame handle
	CMainFrame* GetMainFrame() { return (CMainFrame*)m_pMainWnd; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkeletonEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSkeletonEditorApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG

	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();

	DECLARE_MESSAGE_MAP()

protected:

	DWORD	m_dwLastFrame;
	DWORD	m_dwTickTime;

protected:

	//	Do some initial work
	bool InitApp();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKELETONEDITOR_H__6E5110B1_1011_4247_B774_F53CB6990433__INCLUDED_)
