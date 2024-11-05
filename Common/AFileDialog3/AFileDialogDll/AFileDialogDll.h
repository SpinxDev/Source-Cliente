// AFileDialogDll.h : main header file for the AFILEDIALOGDLL DLL
//

#if !defined(AFX_AFILEDIALOGDLL_H__72FF9271_8B0A_46FF_A6CB_A9B5D6B81EC5__INCLUDED_)
#define AFX_AFILEDIALOGDLL_H__72FF9271_8B0A_46FF_A6CB_A9B5D6B81EC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAFileDialogDllApp
// See AFileDialogDll.cpp for the implementation of this class
//

class CAFileDialogDllApp : public CWinApp
{
public:
	CAFileDialogDllApp();
	~CAFileDialogDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAFileDialogDllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAFileDialogDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	HANDLE m_hDllEngineThreadId;
	CEvent m_EngineCreateEvent;
};

extern ALog g_AFDllLog;

class AFilePreviewWnd;
AFilePreviewWnd* GetPreviewWnd();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AFILEDIALOGDLL_H__72FF9271_8B0A_46FF_A6CB_A9B5D6B81EC5__INCLUDED_)
