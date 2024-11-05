// AutoPFImp.h : main header file for the AUTOPFIMP application
//

#if !defined(AFX_AUTOPFIMP_H__A45FF127_21C8_452A_BB48_6B12BAB4FB2A__INCLUDED_)
#define AFX_AUTOPFIMP_H__A45FF127_21C8_452A_BB48_6B12BAB4FB2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAutoPFImpApp:
// See AutoPFImp.cpp for the implementation of this class
//

class CAutoPFImpApp : public CWinApp
{
public:
	CAutoPFImpApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoPFImpApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CAutoPFImpApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	DWORD		m_dwLastFrame;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOPFIMP_H__A45FF127_21C8_452A_BB48_6B12BAB4FB2A__INCLUDED_)
