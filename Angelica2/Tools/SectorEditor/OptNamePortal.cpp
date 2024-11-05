/*
 * FILE: OptNamePortal.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/1/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "OptNamePortal.h"
#include "MainFrm.h"
#include "Portal.h"
#include "SectorEditorView.h"
#include "SectorEditorDoc.h"
#include "OrthoViewport.h"
#include "WndOrthogonal.h"
#include "Utility.h"

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
//	Implement COptNamePortal
//	
///////////////////////////////////////////////////////////////////////////

COptNamePortal::COptNamePortal(int iID) : COperation(iID)
{
	m_pViewWnd	= NULL;
}

//	Begin operation
bool COptNamePortal::Begin()
{
	if (!COperation::Begin())
		return false;

	return true;
}

//	End operation
bool COptNamePortal::End(bool bCancel)
{
	if (bCancel)
	{
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptNamePortal::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptNamePortal::Render(CViewportWnd* pView)
{
	return true;
}

void COptNamePortal::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING)
		return;

	CSectorEditorView* pView = AUX_GetMainFrame()->GetView();
	int iActiveView = pView->GetActiveViewport();
	if (iActiveView == CSectorEditorView::VIEW_PERSPECTIVE)
		return;

	m_pViewWnd = (COrthogonalWnd*)pView->GetViewport(iActiveView);

	int iAxisH, iAxisV, iAxisZ;
	pView->GetOrthoAxis(iActiveView, &iAxisH, &iAxisV, &iAxisZ);

	//	Pick portal
	int iSide;
	CPortal* pPortal = m_pViewWnd->PickPortal(x, y, &iSide);
	if (!pPortal)
		return;

	if (pPortal->GetName()[0])	//	Don't name portal twice
		return;

	//	Generate a random portal name
	char szName[32];
	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();
	pDoc->GeneratePortalName(szName);

	pPortal->SetName(szName);
	pDoc->SelectEditObject(pPortal);

	pDoc->SetModifiedFlag(TRUE);
}

void COptNamePortal::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	//	Finish operations
	AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
}


