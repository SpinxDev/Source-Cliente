/*
 * FILE: SkinMeshExport.cpp
 *
 * DESCRIPTION: Skin mesh export class
 *
 * CREATED BY: duyuxin, 2003/9/15
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "SkinMeshExportDesc.h"
#include "SkinMeshExport.h"
#include "MAXInterface.h"
#include "SkeletonCandidate.h"
#include "SkinCandidate.h"
#include "SheetSkinMesh.h"
#include "Exportor.h"

#include "A3D.h"
#include "AF.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CSkinMeshExport
//
///////////////////////////////////////////////////////////////////////////

void* CSkinMeshExportDesc::Create(BOOL loadin/* FALSE */)
{
	return new CSkinMeshExport();
}

CSkinMeshExport::CSkinMeshExport()
{
	m_ei = NULL;
	m_i	 = NULL;
}

const TCHAR* CSkinMeshExport::AuthorName()
{
	return _T("Archosaur Studio");
}

const TCHAR* CSkinMeshExport::CopyrightMessage()
{
	return _T("Copyright (C) 2003, Archosaur Studio");
}

const TCHAR* CSkinMeshExport::Ext(int i)
{
	switch(i)
	{
	case 0:
		return _T("ski");
	default:
		return _T("");
	}
}

int CSkinMeshExport::ExtCount()
{
	return 1;
}

const TCHAR* CSkinMeshExport::LongDesc()
{
	return _T("Angelica 3D 皮肤文件");
}

const TCHAR* CSkinMeshExport::OtherMessage1()
{
	return _T("");
}

const TCHAR* CSkinMeshExport::OtherMessage2()
{
	return _T("");
}

const TCHAR* CSkinMeshExport::ShortDesc()
{
	return _T("Angelica 3D 皮肤文件");
}

void CSkinMeshExport::ShowAbout(HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

//	CDialog dlg(IDD_ABOUT);
//	dlg.DoModal();
}

unsigned int CSkinMeshExport::Version()
{
	return 100;
}

int CSkinMeshExport::DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_ei = ei;
	m_i  = i;

	a_SendLogToDebugWnd(0, "Angelica3D skin mesh export ////////////////////////////////////");

	CMAXInterface MAXInterface;

	if (!MAXInterface.Init(ei, i))
	{
		AfxMessageBox("Failed to create MAX interface", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	g_pMAXInterface = &MAXInterface;

	if (!MAXInterface.GetSelectedNodeCount())
	{
		AfxMessageBox("请选择要导出的 mesh", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	CSkeletonCandidate SkeletonCandidate;
	CSkinCandidate SkinCandidate;

	if (!SkinCandidate.Init())
	{
		AfxMessageBox("Failed to initialize skin candidate", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	//	Show export wizard sheet
	CSheetSkinMesh Sheet("Angelica 3D Skin Mesh Export", MAXInterface.GetMainWnd());
	Sheet.SetSkeletonCandidate(&SkeletonCandidate);
	Sheet.SetSkinCandidate(&SkinCandidate);
	Sheet.SetWizardMode();
	if (Sheet.DoModal() != ID_WIZFINISH)
		return 1;

	//	Export skin
	CExportor Exportor;

	if (!Exportor.Init(m_ei, m_i))
		return 0;

	if (!Exportor.ExportSkin(name, &SkinCandidate, &SkeletonCandidate))
		return 0;

	return 1;
}

