// MakeCollision.h : main header file for the MAKECOLLISION application
//

#if !defined(AFX_MAKECOLLISION_H__0FA146AB_B5DF_4F0A_A32B_3B5A1CD4BC49__INCLUDED_)
#define AFX_MAKECOLLISION_H__0FA146AB_B5DF_4F0A_A32B_3B5A1CD4BC49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMakeCollisionApp:
// See MakeCollision.cpp for the implementation of this class
//

class CMakeCollisionApp : public CWinApp
{
public:
	CMakeCollisionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakeCollisionApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMakeCollisionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAKECOLLISION_H__0FA146AB_B5DF_4F0A_A32B_3B5A1CD4BC49__INCLUDED_)
