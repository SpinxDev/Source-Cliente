// DebugInfo.h : main header file for the DEBUGINFO application
//

#if !defined(AFX_DEBUGINFO_H__D7EED3A3_A5F9_46EA_87FB_A3246B42D8F5__INCLUDED_)
#define AFX_DEBUGINFO_H__D7EED3A3_A5F9_46EA_87FB_A3246B42D8F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDebugInfoApp:
// See DebugInfo.cpp for the implementation of this class
//

class CDebugInfoApp : public CWinApp
{
public:
	CDebugInfoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugInfoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDebugInfoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGINFO_H__D7EED3A3_A5F9_46EA_87FB_A3246B42D8F5__INCLUDED_)
