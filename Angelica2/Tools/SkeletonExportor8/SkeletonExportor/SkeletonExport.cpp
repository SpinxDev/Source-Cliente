/*
 * FILE: SkeletonExport.cpp
 *
 * DESCRIPTION: Skeleton export class
 *
 * CREATED BY: duyuxin, 2003/8/25
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "SkeletonExportDesc.h"
#include "SkeletonExport.h"
#include "MAXInterface.h"
#include "SheetSkeleton.h"
#include "SkeletonCandidate.h"
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
//	Implement CSkeletonExport
//
///////////////////////////////////////////////////////////////////////////

void* CSkeletonExportDesc::Create(BOOL loadin/* FALSE */)
{
	return new CSkeletonExport();
}

CSkeletonExport::CSkeletonExport()
{
	m_ei = NULL;
	m_i	 = NULL;
}

const TCHAR* CSkeletonExport::AuthorName()
{
	return _T("Archosaur Studio");
}

const TCHAR* CSkeletonExport::CopyrightMessage()
{
	return _T("Copyright (C) 2003, Archosaur Studio");
}

const TCHAR* CSkeletonExport::Ext(int i)
{
	switch(i)
	{
	case 0:
		return _T("bon");
	default:
		return _T("");
	}
}

int CSkeletonExport::ExtCount()
{
	return 1;
}

const TCHAR* CSkeletonExport::LongDesc()
{
	return _T("Angelica 3D 骨骼文件");
}

const TCHAR* CSkeletonExport::OtherMessage1()
{
	return _T("");
}

const TCHAR* CSkeletonExport::OtherMessage2()
{
	return _T("");
}

const TCHAR* CSkeletonExport::ShortDesc()
{
	return _T("Angelica 3D 骨骼文件");
}

void CSkeletonExport::ShowAbout(HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

//	CDialog dlg(IDD_ABOUT);
//	dlg.DoModal();
}

unsigned int CSkeletonExport::Version()
{
	return 100;
}

int CSkeletonExport::DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_ei = ei;
	m_i  = i;

	a_SendLogToDebugWnd(0, "Angelica3D skeleton export ////////////////////////////////////");

	CMAXInterface MAXInterface;

	if (!MAXInterface.Init(ei, i))
	{
		AfxMessageBox("Failed to create MAX interface", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	g_pMAXInterface = &MAXInterface;

	if (!MAXInterface.GetSelectedNodeCount())
	{
		AfxMessageBox("请选择要导出的 bone", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	//	Create skeleton candidate
	CSkeletonCandidate SkeletonCandidate;
	if (!SkeletonCandidate.Init())
	{
		AfxMessageBox("Failed to create skeleton candidate", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	//	Show export wizard sheet
	CSheetSkeleton Sheet("Angelica 3D Skeleton Export", MAXInterface.GetMainWnd());
	Sheet.SetSkeletonCandidate(&SkeletonCandidate);
	Sheet.SetWizardMode();
	if (Sheet.DoModal() != ID_WIZFINISH)
		return 1;

	//	Export skeleton
	CExportor Exportor;

	if (!Exportor.Init(m_ei, m_i))
		return 0;

	if (!Exportor.ExportSkeleton(name, &SkeletonCandidate))
		return 0;

	return 1;
}

