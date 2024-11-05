/*
 * FILE: OptCreateSidePortal.cpp
 *
 * DESCRIPTION: Create portal on sector's side
 *
 * CREATED BY: Duyuxin, 2003/8/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "OptCreateSidePortal.h"
#include "Sector.h"
#include "Portal.h"
#include "Render.h"
#include "SectorEditorView.h"
#include "SectorEditorDoc.h"
#include "MainFrm.h"
#include "Utility.h"
#include "OrthoViewport.h"
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
//	Implement COptCreateSidePortal
//
///////////////////////////////////////////////////////////////////////////

COptCreateSidePortal::COptCreateSidePortal(int iID) : COperation(iID)
{
	m_pViewWnd	= NULL;
}

//	Begin operation
bool COptCreateSidePortal::Begin()
{
	if (!COperation::Begin())
		return false;

	return true;
}

//	End operation
bool COptCreateSidePortal::End(bool bCancel)
{
	if (bCancel)
	{
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptCreateSidePortal::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptCreateSidePortal::Render(CViewportWnd* pView)
{
	return true;
}

void COptCreateSidePortal::OnLButtonDown(int x, int y, DWORD dwFlags)
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

	//	Select sector
	int iSide;
	CSector* pSector = m_pViewWnd->PickSector(x, y, &iSide);
	if (!pSector)
		return;

	A3DVECTOR3 aVerts[4];

	A3DVECTOR3 vMins = pSector->GetMins();
	A3DVECTOR3 vMaxs = pSector->GetMaxs();

	float l = vMins.m[iAxisH];
	float r = vMaxs.m[iAxisH];
	float t = vMaxs.m[iAxisV];
	float b = vMins.m[iAxisV];

	int iMajorAxis;

	switch (iSide)
	{
	case SIDE_LEFT:

		aVerts[0].m[iAxisH] = l;	aVerts[0].m[iAxisV] = b;	aVerts[0].m[iAxisZ] = vMins.m[iAxisZ];
		aVerts[1].m[iAxisH]	= l;	aVerts[1].m[iAxisV] = t;	aVerts[1].m[iAxisZ] = vMins.m[iAxisZ];
		aVerts[2].m[iAxisH] = l;	aVerts[2].m[iAxisV] = t;	aVerts[2].m[iAxisZ] = vMaxs.m[iAxisZ];
		aVerts[3].m[iAxisH] = l;	aVerts[3].m[iAxisV] = b;	aVerts[3].m[iAxisZ] = vMaxs.m[iAxisZ];
		iMajorAxis = iAxisH;
		break;

	case SIDE_TOP:

		aVerts[0].m[iAxisH] = l;	aVerts[0].m[iAxisV] = t;	aVerts[0].m[iAxisZ] = vMins.m[iAxisZ];
		aVerts[1].m[iAxisH]	= r;	aVerts[1].m[iAxisV] = t;	aVerts[1].m[iAxisZ] = vMins.m[iAxisZ];
		aVerts[2].m[iAxisH] = r;	aVerts[2].m[iAxisV] = t;	aVerts[2].m[iAxisZ] = vMaxs.m[iAxisZ];
		aVerts[3].m[iAxisH] = l;	aVerts[3].m[iAxisV] = t;	aVerts[3].m[iAxisZ] = vMaxs.m[iAxisZ];
		iMajorAxis = iAxisV;
		break;

	case SIDE_RIGHT:

		aVerts[0].m[iAxisH] = r;	aVerts[0].m[iAxisV] = t;	aVerts[0].m[iAxisZ] = vMins.m[iAxisZ];
		aVerts[1].m[iAxisH]	= r;	aVerts[1].m[iAxisV] = b;	aVerts[1].m[iAxisZ] = vMins.m[iAxisZ];
		aVerts[2].m[iAxisH] = r;	aVerts[2].m[iAxisV] = b;	aVerts[2].m[iAxisZ] = vMaxs.m[iAxisZ];
		aVerts[3].m[iAxisH] = r;	aVerts[3].m[iAxisV] = t;	aVerts[3].m[iAxisZ] = vMaxs.m[iAxisZ];
		iMajorAxis = iAxisH;
		break;

	case SIDE_BOTTOM:

		aVerts[0].m[iAxisH] = r;	aVerts[0].m[iAxisV] = b;	aVerts[0].m[iAxisZ] = vMins.m[iAxisZ];
		aVerts[1].m[iAxisH]	= l;	aVerts[1].m[iAxisV] = b;	aVerts[1].m[iAxisZ] = vMins.m[iAxisZ];
		aVerts[2].m[iAxisH] = l;	aVerts[2].m[iAxisV] = b;	aVerts[2].m[iAxisZ] = vMaxs.m[iAxisZ];
		aVerts[3].m[iAxisH] = r;	aVerts[3].m[iAxisV] = b;	aVerts[3].m[iAxisZ] = vMaxs.m[iAxisZ];
		iMajorAxis = iAxisV;
		break;

	default:	
		
		ASSERT(0);
		break;
	}

	//	Create a portal
	CPortal* pPortal = new CPortal;
	if (!pPortal)
		ASSERT(0);

	pPortal->Init(&g_Render, aVerts, iMajorAxis);

	CSectorEditorDoc* pDoc = AUX_GetMainFrame()->GetDocument();
	pDoc->AddPortal(pPortal);
	pDoc->SelectEditObject(pPortal);
}

void COptCreateSidePortal::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	//	Finish operations
	AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
}




