/*
 * FILE: SkinModelExport.cpp
 *
 * DESCRIPTION: Skin model export class
 *
 * CREATED BY: duyuxin, 2003/10/14
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "SkinModelExportDesc.h"
#include "SkinModelExport.h"
#include "MAXInterface.h"
#include "SheetSkinModel.h"
#include "SkeletonCandidate.h"
#include "PhysiqueCandidate.h"
#include "SkinCandidate.h"
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
//	Implement CSkinModelExport
//
///////////////////////////////////////////////////////////////////////////

void* CSkinModelExportDesc::Create(BOOL loadin/* FALSE */)
{
	return new CSkinModelExport();
}

CSkinModelExport::CSkinModelExport()
{
	m_ei = NULL;
	m_i	 = NULL;
}

const TCHAR* CSkinModelExport::AuthorName()
{
	return _T("Archosaur Studio");
}

const TCHAR* CSkinModelExport::CopyrightMessage()
{
	return _T("Copyright (C) 2003, Archosaur Studio");
}

const TCHAR* CSkinModelExport::Ext(int i)
{
	switch(i)
	{
	case 0:
		return _T("smd");
	default:
		return _T("");
	}
}

int CSkinModelExport::ExtCount()
{
	return 1;
}

const TCHAR* CSkinModelExport::LongDesc()
{
	return _T("Angelica 3D 骨骼模型文件");
}

const TCHAR* CSkinModelExport::OtherMessage1()
{
	return _T("");
}

const TCHAR* CSkinModelExport::OtherMessage2()
{
	return _T("");
}

const TCHAR* CSkinModelExport::ShortDesc()
{
	return _T("Angelica 3D 骨骼模型文件");
}

void CSkinModelExport::ShowAbout(HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

//	CDialog dlg(IDD_ABOUT);
//	dlg.DoModal();
}

unsigned int CSkinModelExport::Version()
{
	return 100;
}

int CSkinModelExport::DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options)
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
		AfxMessageBox("没有被选择的节点", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	//	Create skeleton candidate
	CSkeletonCandidate SkeletonCandidate;
	if (!SkeletonCandidate.Init())
	{
		AfxMessageBox("Failed to create skeleton candidate", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	CSkinCandidate SkinCandidate;
	if (!SkinCandidate.Init())
	{
		AfxMessageBox("Failed to initialize skin candidate", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	CPhysiqueCandidate PhyCandidate;
	if (!PhyCandidate.Init())
	{
		AfxMessageBox("Failed to initialize physique candidate", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	//	Show export wizard sheet
	CSheetSkinModel Sheet("Angelica 3D Skin Model Export", MAXInterface.GetMainWnd());
	Sheet.SetSkeletonCandidate(&SkeletonCandidate);
	Sheet.SetSkinCandidate(&SkinCandidate);
	Sheet.SetWizardMode();
	if (Sheet.DoModal() != ID_WIZFINISH)
		return 1;

	SkeletonCandidate.SetUpdateFileFlag(false);

	//	Do export
	CExportor Exportor;

	if (!Exportor.Init(m_ei, m_i))
		return 0;

	char szSkeleton[256], szSkin[256], szPhysique[256];
	strcpy(szSkeleton, name);
	strcpy(szSkin, name);
	strcpy(szPhysique, name);

	char* pTemp = strrchr(szSkeleton, '.');
	strcpy(pTemp+1, "bon");

	pTemp = strrchr(szSkin, '.');
	strcpy(pTemp+1, "ski");

	pTemp = strrchr(szPhysique, '.');
	strcpy(pTemp+1, "phy");

	//	Export skeleton
	if (!Exportor.ExportSkeleton(szSkeleton, &SkeletonCandidate))
		return 0;

	//	Export skin
	if (!Exportor.ExportSkin(szSkin, &SkinCandidate, &SkeletonCandidate))
		return 0;

	//	Export physique
	if (!Exportor.ExportPhysique(szPhysique, &PhyCandidate, &SkeletonCandidate))
		return 0;

	//	Export skin model
	if (!Exportor.ExportSkinModel(name, szSkeleton, szSkin, szPhysique))
		return 0;

	return 1;
}

