/*
 * FILE: MainFrmUpdate.cpp
 *
 * DESCRIPTION: Update routines for menu item and dialog controls
 *
 * CREATED BY: Duyuxin, 2003/9/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "MainFrm.h"
#include "SkeletonEditorDoc.h"
#include "Model.h"

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

MAINFRAMEUPDATE	g_MainFrmUpdate;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

//	Initialize update flags
void UF_Initialize()
{
	memset(&g_MainFrmUpdate, 1, sizeof (g_MainFrmUpdate));
}

void CMainFrame::OnUpdateViewBoundbox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Configs.bShowBoundBox ? 1 : 0);
}

void CMainFrame::OnUpdateViewSkeletononly(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Configs.bSkeletonOnly ? 1 : 0);
}

void CMainFrame::OnUpdateViewPhysiqueshape(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Configs.bShowPhyShape ? 1 : 0);
}

void CMainFrame::OnUpdateViewGround(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Configs.bShowGround ? 1 : 0);
}

void CMainFrame::OnUpdateViewNormal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Configs.bShowNormal ? 1 : 0);
}

void CMainFrame::OnUpdateEditChangeskin(CCmdUI* pCmdUI) 
{
	CModel* pModel = GetDocument()->GetModel();
	if (!pModel)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(pModel->GetSkeleton() ? TRUE : FALSE);
}

void CMainFrame::OnUpdateEditAddphysique(CCmdUI* pCmdUI) 
{
	CModel* pModel = GetDocument()->GetModel();
	if (!pModel)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(pModel->GetSkeleton() ? TRUE : FALSE);
}

void CMainFrame::OnUpdateEditRemovephysique(CCmdUI* pCmdUI) 
{
	CModel* pModel = GetDocument()->GetModel();
	if (!pModel)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(pModel->GetSkeleton() ? TRUE : FALSE);
}

