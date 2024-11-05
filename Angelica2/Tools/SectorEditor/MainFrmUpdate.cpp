/*
 * FILE: MainFrmUpdate.cpp
 *
 * DESCRIPTION: Update routines for menu item and dialog controls
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "MainFrm.h"

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

void CMainFrame::OnUpdateShowHideFlag(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
	case ID_VIEW_SHOWGRID:		pCmdUI->SetCheck(g_Configs.bShowGrid);		break;
	case ID_VIEW_SHOWSCENE:		pCmdUI->SetCheck(g_Configs.bShowScene);  	break;
	case ID_VIEW_SHOWSECTOR:	pCmdUI->SetCheck(g_Configs.bShowSector); 	break;
	case ID_VIEW_SHOWPORTAL:	pCmdUI->SetCheck(g_Configs.bShowPortal); 	break;
	}
}

void CMainFrame::OnUpdateViewForcerender(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Configs.bForceRender);
}

void CMainFrame::OnUpdateViewFullclip(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Configs.bFullClip);
}

void CMainFrame::OnUpdateViewTest(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Configs.bTesting);
}

void CMainFrame::OnUpdateViewMessagewnd(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_MessageWnd.IsWindowVisible() ? 1 : 0);
}

void CMainFrame::OnUpdateCameraMoveLevel(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
	case ID_VIEW_CAMERASPEED0:	pCmdUI->SetRadio(g_Configs.iCameraMove == 0 ? true : false);	break;
	case ID_VIEW_CAMERASPEED1:	pCmdUI->SetRadio(g_Configs.iCameraMove == 1 ? true : false);	break;
	case ID_VIEW_CAMERASPEED2:	pCmdUI->SetRadio(g_Configs.iCameraMove == 2 ? true : false);	break;
	case ID_VIEW_CAMERASPEED3:	pCmdUI->SetRadio(g_Configs.iCameraMove == 3 ? true : false);	break;
	case ID_VIEW_CAMERASPEED4:	pCmdUI->SetRadio(g_Configs.iCameraMove == 4 ? true : false);	break;
	}
}

void CMainFrame::OnUpdateCameraRotateLevel(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
	case ID_VIEW_CAMERAROTATE0:	pCmdUI->SetRadio(g_Configs.iCameraRotate == 0 ? true : false);	break;
	case ID_VIEW_CAMERAROTATE1:	pCmdUI->SetRadio(g_Configs.iCameraRotate == 1 ? true : false);	break;
	case ID_VIEW_CAMERAROTATE2:	pCmdUI->SetRadio(g_Configs.iCameraRotate == 2 ? true : false);	break;
	case ID_VIEW_CAMERAROTATE3:	pCmdUI->SetRadio(g_Configs.iCameraRotate == 3 ? true : false);	break;
	case ID_VIEW_CAMERAROTATE4:	pCmdUI->SetRadio(g_Configs.iCameraRotate == 4 ? true : false);	break;
	}
}

