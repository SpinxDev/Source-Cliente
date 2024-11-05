/*
 * FILE: PhysiqueExport.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/5/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "PhysiqueExportDesc.h"
#include "PhysiqueExport.h"
#include "MAXInterface.h"
#include "SkeletonCandidate.h"
#include "PhysiqueCandidate.h"
#include "SheetPhysique.h"
#include "Exportor.h"

#include "A3D.h"
#include "AF.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

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
//	Implement CPhysiqueExport
//	
///////////////////////////////////////////////////////////////////////////

void* CPhysiqueExportDesc::Create(BOOL loadin/* FALSE */)
{
	return new CPhysiqueExport();
}

CPhysiqueExport::CPhysiqueExport()
{
	m_ei = NULL;
	m_i	 = NULL;
}

const TCHAR* CPhysiqueExport::AuthorName()
{
	return _T("Archosaur Studio");
}

const TCHAR* CPhysiqueExport::CopyrightMessage()
{
	return _T("Copyright (C) 2003, Archosaur Studio");
}

const TCHAR* CPhysiqueExport::Ext(int i)
{
	switch(i)
	{
	case 0:
		return _T("phy");
	default:
		return _T("");
	}
}

int CPhysiqueExport::ExtCount()
{
	return 1;
}

const TCHAR* CPhysiqueExport::LongDesc()
{
	return _T("Angelica 3D 体型文件");
}

const TCHAR* CPhysiqueExport::OtherMessage1()
{
	return _T("");
}

const TCHAR* CPhysiqueExport::OtherMessage2()
{
	return _T("");
}

const TCHAR* CPhysiqueExport::ShortDesc()
{
	return _T("Angelica 3D 体型文件");
}

void CPhysiqueExport::ShowAbout(HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

//	CDialog dlg(IDD_ABOUT);
//	dlg.DoModal();
}

unsigned int CPhysiqueExport::Version()
{
	return 100;
}

int CPhysiqueExport::DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_ei = ei;
	m_i  = i;

	a_SendLogToDebugWnd(0, "Angelica3D physique export ////////////////////////////////////");

	CMAXInterface MAXInterface;

	if (!MAXInterface.Init(ei, i))
	{
		AfxMessageBox("Failed to create MAX interface", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	g_pMAXInterface = &MAXInterface;

	if (!MAXInterface.GetSelectedNodeCount())
	{
		AfxMessageBox("没有找到选中的节点", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	//	Create physique candidate
	CSkeletonCandidate SkeletonCandidate;
	CPhysiqueCandidate PhyCandidate;

	if (!PhyCandidate.Init())
	{
		AfxMessageBox("Failed to initialize physique candidate", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	//	Show export wizard sheet
	CSheetPhysique Sheet("Angelica 3D Physique Export", MAXInterface.GetMainWnd());
	Sheet.SetSkeletonCandidate(&SkeletonCandidate);
	Sheet.SetPhysiqueCandidate(&PhyCandidate);
	Sheet.SetWizardMode();
	if (Sheet.DoModal() != ID_WIZFINISH)
		return 1;

	//	Export skin
	CExportor Exportor;

	if (!Exportor.Init(m_ei, m_i))
		return 0;

	if (!Exportor.ExportPhysique(name, &PhyCandidate, &SkeletonCandidate))
		return 0;

	return 1;
}

