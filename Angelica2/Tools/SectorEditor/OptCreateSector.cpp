/*
 * FILE: OptCreateSector.cpp
 *
 * DESCRIPTION: Create sector operation class
 *
 * CREATED BY: Duyuxin, 2003/7/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "OptCreateSector.h"
#include "Sector.h"
#include "Render.h"
#include "SectorEditorView.h"
#include "SectorEditorDoc.h"
#include "MainFrm.h"
#include "Utility.h"
#include "OrthoViewport.h"
#include "OrthoGrid.h"
#include "WndOrthogonal.h"
#include "AAssist.h"

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
//	Implement COptCreateSector
//
///////////////////////////////////////////////////////////////////////////

COptCreateSector::COptCreateSector(int iID) : COperation(iID)
{
	m_pSector	= NULL;
	m_pViewWnd	= NULL;
	m_iAxisH	= 0;
	m_iAxisV	= 1;
	m_iAxisZ	= 2;
	m_bDrag		= false;
}

//	Begin operation
bool COptCreateSector::Begin()
{
	if (!COperation::Begin())
		return false;

	return true;
}

//	End operation
bool COptCreateSector::End(bool bCancel)
{
	if (bCancel && m_pSector)
	{
		AUX_GetMainFrame()->GetDocument()->RemoveSector(m_pSector, false);

		m_pSector->Release();
		delete m_pSector;
		m_pSector = NULL;
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptCreateSector::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptCreateSector::Render(CViewportWnd* pView)
{
	return true;
}

void COptCreateSector::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	UpdateSector(x, y);
}

void COptCreateSector::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING)
		return;

	CSectorEditorView* pView = AUX_GetMainFrame()->GetView();
	int iActiveView = pView->GetActiveViewport();
	if (iActiveView == CSectorEditorView::VIEW_PERSPECTIVE)
		return;

	m_pViewWnd = (COrthogonalWnd*)pView->GetViewport(iActiveView);
	pView->GetOrthoAxis(iActiveView, &m_iAxisH, &m_iAxisV, &m_iAxisZ);

	//	Convert position from screen to world
	A3DVECTOR3 vMins((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(vMins, vMins);

	vMins.m[m_iAxisH] = SnapToGrid(vMins.m[m_iAxisH], MIN_GRID);
	vMins.m[m_iAxisV] = SnapToGrid(vMins.m[m_iAxisV], MIN_GRID);
	vMins.m[m_iAxisZ] = g_vLastSectorMins.m[m_iAxisZ];

	A3DVECTOR3 vMaxs;
	vMaxs.m[m_iAxisH] = vMins.m[m_iAxisH] + MIN_GRID;
	vMaxs.m[m_iAxisV] = vMins.m[m_iAxisV] + MIN_GRID;
	vMaxs.m[m_iAxisZ] = g_vLastSectorMaxs.m[m_iAxisZ];

	//	Sector collision ?
	if (AUX_GetMainFrame()->GetDocument()->CheckSectorCollision(vMins, vMaxs, NULL))
		return;

	if (!(m_pSector = new CSector))
		ASSERT(0);

	m_pSector->Init(&g_Render, vMins, vMaxs);

	AUX_GetMainFrame()->GetDocument()->AddSector(m_pSector);

	m_bDrag		= true;
	m_ptStart.x = x;
	m_ptStart.y = y;
	m_vStart	= vMins;
	m_bFixView	= true;
}

void COptCreateSector::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	m_bDrag		= false;
	m_bFixView	= false;

	UpdateSector(x, y);

	//	Record sector's volume for next one
	g_vLastSectorMins = m_pSector->GetMins();
	g_vLastSectorMaxs = m_pSector->GetMaxs();

	//	Finish operation
	AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
}

void COptCreateSector::UpdateSector(int x, int y)
{
	//	Convert position from screen to world
	A3DVECTOR3 vMaxs, vPos((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(vPos, vPos);

	//	Expand horizontally first
	vMaxs.m[m_iAxisH] = SnapToGrid(vPos.m[m_iAxisH], MIN_GRID);
	vMaxs.m[m_iAxisV] = m_vStart.m[m_iAxisV];
	vMaxs.m[m_iAxisZ] = g_vLastSectorMaxs.m[m_iAxisZ];

	A3DVECTOR3 vMins = m_vStart;
	vMins.m[m_iAxisH] = a_Min(m_vStart.m[m_iAxisH], vMaxs.m[m_iAxisH]);
	vMaxs.m[m_iAxisH] = a_Max(m_vStart.m[m_iAxisH], vMaxs.m[m_iAxisH]);

	//	Sector collision ?
	int iAxis = m_iAxisH;
	int iSign = x > m_ptStart.x ? 1 : -1;
	AUX_GetMainFrame()->GetDocument()->SizeSectorCollision(vMins, vMaxs, m_pSector, iAxis, iSign);

	//	Then expand vertically
	vMaxs.m[m_iAxisV] = SnapToGrid(vPos.m[m_iAxisV], MIN_GRID);
	vMins.m[m_iAxisV] = a_Min(m_vStart.m[m_iAxisV], vMaxs.m[m_iAxisV]);
	vMaxs.m[m_iAxisV] = a_Max(m_vStart.m[m_iAxisV], vMaxs.m[m_iAxisV]);

	//	Sector collision ?
	iAxis = m_iAxisV;
	iSign = y > m_ptStart.y ? -1 : 1;
	AUX_GetMainFrame()->GetDocument()->SizeSectorCollision(vMins, vMaxs, m_pSector, iAxis, iSign);

	if (vMins.x == vMaxs.x || vMins.y == vMaxs.z || vMins.z == vMaxs.z)
		return;

	m_pSector->SetMinsAndMaxs(vMins, vMaxs);
}


