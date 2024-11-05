/*
 * FILE: OptSnapPortal.cpp
 *
 * DESCRIPTION: Snap portal to sector operation class
 *
 * CREATED BY: Duyuxin, 2003/7/30
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "OptSnapPortal.h"
#include "Sector.h"
#include "Portal.h"
#include "SectorEditorView.h"
#include "SectorEditorDoc.h"
#include "MainFrm.h"
#include "Utility.h"
#include "WndOrthogonal.h"

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
//	Implement COptSnapPortal
//
///////////////////////////////////////////////////////////////////////////

COptSnapPortal::COptSnapPortal(int iID) : COperation(iID)
{
	m_pPortal	= NULL;
	m_pViewWnd	= NULL;
}

//	Begin operation
bool COptSnapPortal::Begin()
{
	if (!COperation::Begin())
		return false;

	ASSERT(m_pPortal);
	m_pPortal->GetVertices(m_aOldVerts);

	return true;
}

//	End operation
bool COptSnapPortal::End(bool bCancel)
{
	if (bCancel && m_pPortal)
	{
		//	Restore old data
		m_pPortal->SetVertices(m_aOldVerts);
		m_pPortal = NULL;
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptSnapPortal::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptSnapPortal::Render(CViewportWnd* pView)
{
	return true;
}

void COptSnapPortal::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_pPortal)
		return;

	CSectorEditorView* pView = AUX_GetMainFrame()->GetView();
	int iActiveView = pView->GetActiveViewport();
	if (iActiveView == CSectorEditorView::VIEW_PERSPECTIVE)
		return;

	m_pViewWnd = (COrthogonalWnd*)pView->GetViewport(iActiveView);

	int iAxisH, iAxisV, iAxisZ;
	pView->GetOrthoAxis(iActiveView, &iAxisH, &iAxisV, &iAxisZ);

	if (iAxisZ == m_pPortal->GetMajorAxis())
		return;

	//	Select sector
	int iSide;
	CSector* pSector = m_pViewWnd->PickSector(x, y, &iSide);
	if (!pSector)
		return;

	if (iAxisH == m_pPortal->GetMajorAxis())
	{
		if (iSide == SIDE_TOP || iSide == SIDE_BOTTOM)
			return;

		A3DVECTOR3 v = (iSide == SIDE_LEFT) ? pSector->GetMins() : pSector->GetMaxs();

		A3DVECTOR3 aVerts[4];
		m_pPortal->GetVertices(aVerts);
		
		aVerts[0].m[iAxisH] = v.m[iAxisH];
		aVerts[1].m[iAxisH] = v.m[iAxisH];
		aVerts[2].m[iAxisH] = v.m[iAxisH];
		aVerts[3].m[iAxisH] = v.m[iAxisH];

		m_pPortal->SetVertices(aVerts);
	}
	else	//	iAxisV == m_pPortal->GetMajorAxis()
	{
		if (iSide == SIDE_LEFT || iSide == SIDE_RIGHT)
			return;

		A3DVECTOR3 v = (iSide == SIDE_BOTTOM) ? pSector->GetMins() : pSector->GetMaxs();

		A3DVECTOR3 aVerts[4];
		m_pPortal->GetVertices(aVerts);
		
		aVerts[0].m[iAxisV] = v.m[iAxisV];
		aVerts[1].m[iAxisV] = v.m[iAxisV];
		aVerts[2].m[iAxisV] = v.m[iAxisV];
		aVerts[3].m[iAxisV] = v.m[iAxisV];

		m_pPortal->SetVertices(aVerts);
	}
}

void COptSnapPortal::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	//	Finish operations
	AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
}




