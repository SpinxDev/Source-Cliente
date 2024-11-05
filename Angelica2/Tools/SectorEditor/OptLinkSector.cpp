/*
 * FILE: OptLinkSector.cpp
 *
 * DESCRIPTION: Link sector with portal operation class
 *
 * CREATED BY: Duyuxin, 2003/7/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "OptLinkSector.h"
#include "Sector.h"
#include "Portal.h"
#include "Render.h"
#include "SectorEditorView.h"
#include "SectorEditorDoc.h"
#include "MainFrm.h"
#include "Utility.h"
#include "OrthoViewport.h"
#include "OrthoGrid.h"
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
//	Implement COptLinkSector
//
///////////////////////////////////////////////////////////////////////////

COptLinkSector::COptLinkSector(int iID) : COperation(iID)
{
	m_pPortal	= NULL;
	m_pViewWnd	= NULL;
}

//	Begin operation
bool COptLinkSector::Begin()
{
	if (!COperation::Begin())
		return false;

	ASSERT(m_pPortal);

	for (int i=0; i < 2; i++)
		m_aSectors[i] = m_pPortal->GetLinkedSector(i, &m_aLinkModes[i]);

	m_pPortal->UnlinkSectors();

	return true;
}

//	End operation
bool COptLinkSector::End(bool bCancel)
{
	if (bCancel && m_pPortal)
	{
		//	Restore old data
		m_pPortal->UnlinkSectors();
		m_pPortal->LinkSector(m_aSectors[0]);
		m_pPortal->LinkSector(m_aSectors[1]);
		m_pPortal = NULL;
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptLinkSector::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptLinkSector::Render(CViewportWnd* pView)
{
	return true;
}

void COptLinkSector::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_pPortal)
		return;

	CSectorEditorView* pView = AUX_GetMainFrame()->GetView();
	int iActiveView = pView->GetActiveViewport();
	if (iActiveView == CSectorEditorView::VIEW_PERSPECTIVE)
		return;

	m_pViewWnd = (COrthogonalWnd*)pView->GetViewport(iActiveView);

	//	Select sector
	int iSide;
	CSector* pSector = m_pViewWnd->PickSector(x, y, &iSide);
	if (!pSector)
		return;

	m_pPortal->LinkSector(pSector);

	int iNumLinked = m_pPortal->GetLinkedSectorNum();
	if (iNumLinked == 2)
	{
		//	Finish operations
		AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
	}
}

void COptLinkSector::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	//	If sectors have been unlinked, then end this operation
	if (!m_pPortal->GetLinkedSectorNum())
	{
		AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
		return;
	}

	//	Re-link sectors
	m_pPortal->UnlinkSectors();
}




