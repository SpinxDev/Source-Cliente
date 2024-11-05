// SkeletonExportor.h : main header file for the SKELETONEXPORTOR DLL
//

#if !defined(AFX_SKELETONEXPORTOR_H__50C9007F_9F8C_485A_A311_E38C938153B1__INCLUDED_)
#define AFX_SKELETONEXPORTOR_H__50C9007F_9F8C_485A_A311_E38C938153B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

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

class ClassDesc;
class CSkeletonExportDesc;
class CSkinMeshExportDesc;
class CSkinModelExportDesc;
class CPhysiqueExportDesc;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSkeletonExportorApp
//
///////////////////////////////////////////////////////////////////////////

class CSkeletonExportorApp : public CWinApp
{
public:
	CSkeletonExportorApp();

	ClassDesc* GetClassDesc(int id);
	int GetClassDescCount();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkeletonExportorApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSkeletonExportorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	static CSkeletonExportDesc	m_SkeletonExportDesc;
	static CSkinMeshExportDesc	m_SkinMeshExportDesc;
	static CSkinModelExportDesc	m_SkinModelExportDesc;
	static CPhysiqueExportDesc	m_PhysiqueExportDesc;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKELETONEXPORTOR_H__50C9007F_9F8C_485A_A311_E38C938153B1__INCLUDED_)
