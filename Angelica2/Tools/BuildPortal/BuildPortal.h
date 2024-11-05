// BuildPortal.h : main header file for the BUILDPORTAL application
//

#if !defined(AFX_BUILDPORTAL_H__5C100CC1_5648_4BB1_ADEA_4A16BEF86944__INCLUDED_)
#define AFX_BUILDPORTAL_H__5C100CC1_5648_4BB1_ADEA_4A16BEF86944__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBuildPortalApp:
// See BuildPortal.cpp for the implementation of this class
//

class CBuildPortalApp : public CWinApp
{
public:
	CBuildPortalApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuildPortalApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBuildPortalApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDPORTAL_H__5C100CC1_5648_4BB1_ADEA_4A16BEF86944__INCLUDED_)
