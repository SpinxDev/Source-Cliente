// PhysDemo.h : main header file for the MFCTEMP application
//

#if !defined(AFX_PHYSDEMO_H__BAA025BC_0ADD_491E_955D_A2F315D8901D__INCLUDED_)
#define AFX_PHYSDEMO_H__BAA025BC_0ADD_491E_955D_A2F315D8901D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CMainFrame;

/////////////////////////////////////////////////////////////////////////////
// CPhysDemoApp:
// See PhysDemo.cpp for the implementation of this class
//

class CPhysDemoApp : public CWinApp
{
public:
	CPhysDemoApp();

	//	Get main frame handle
	CMainFrame* GetMainFrame() { return (CMainFrame*)m_pMainWnd; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhysDemoApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CPhysDemoApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	//	Do some initial work
	bool InitApp();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHYSDEMO_H__BAA025BC_0ADD_491E_955D_A2F315D8901D__INCLUDED_)
