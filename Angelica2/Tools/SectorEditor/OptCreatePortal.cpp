/*
 * FILE: OptCreatePortal.cpp
 *
 * DESCRIPTION: Create portal operation class
 *
 * CREATED BY: Duyuxin, 2003/7/24
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "OptCreatePortal.h"
#include "Portal.h"
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
//	Implement COptCreatePortal
//
///////////////////////////////////////////////////////////////////////////

COptCreatePortal::COptCreatePortal(int iID) : COperation(iID)
{
	m_pPortal	= NULL;
	m_pViewWnd	= NULL;
	m_iAxisH	= 0;
	m_iAxisV	= 1;
	m_iAxisZ	= 2;
	m_bDrag		= false;
}

//	Begin operation
bool COptCreatePortal::Begin()
{
	if (!COperation::Begin())
		return false;

	return true;
}

//	End operation
bool COptCreatePortal::End(bool bCancel)
{
	if (bCancel && m_pPortal)
	{
		AUX_GetMainFrame()->GetDocument()->RemovePortal(m_pPortal, false);

		m_pPortal->Release();
		delete m_pPortal;
		m_pPortal = NULL;
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptCreatePortal::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptCreatePortal::Render(CViewportWnd* pView)
{
	return true;
}

void COptCreatePortal::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	UpdatePortal(x, y);
}

void COptCreatePortal::OnLButtonDown(int x, int y, DWORD dwFlags)
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
	vMins.m[m_iAxisZ] = 0.0f;

	A3DVECTOR3 vMaxs;
	vMaxs.m[m_iAxisH] = vMins.m[m_iAxisH] + MIN_GRID;
	vMaxs.m[m_iAxisV] = vMins.m[m_iAxisV] + MIN_GRID;
	vMaxs.m[m_iAxisZ] = vMins.m[m_iAxisZ];

	if (!(m_pPortal = new CPortal))
		ASSERT(0);

	A3DVECTOR3 aVerts[4];
	BuildPortalVerts(vMins, vMaxs, aVerts);

	m_pPortal->Init(&g_Render, aVerts, m_iAxisZ);

	AUX_GetMainFrame()->GetDocument()->AddPortal(m_pPortal);

	m_bDrag		= true;
	m_ptStart.x = x;
	m_ptStart.y = y;
	m_vStart	= vMins;
	m_bFixView	= true;
}

void COptCreatePortal::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	m_bDrag		= false;
	m_bFixView	= false;

	UpdatePortal(x, y);

	//	Finish operation
	AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
}

void COptCreatePortal::UpdatePortal(int x, int y)
{
	//	Convert position from screen to world
	A3DVECTOR3 vMaxs, vPos((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(vPos, vPos);

	//	Expand horizontally first
	vMaxs.m[m_iAxisH] = SnapToGrid(vPos.m[m_iAxisH], MIN_GRID);
	vMaxs.m[m_iAxisV] = SnapToGrid(vPos.m[m_iAxisV], MIN_GRID);
	vMaxs.m[m_iAxisZ] = 0.0f;

	A3DVECTOR3 vMins = m_vStart;
	vMins.m[m_iAxisH] = a_Min(m_vStart.m[m_iAxisH], vMaxs.m[m_iAxisH]);
	vMins.m[m_iAxisV] = a_Min(m_vStart.m[m_iAxisV], vMaxs.m[m_iAxisV]);
	vMaxs.m[m_iAxisH] = a_Max(m_vStart.m[m_iAxisH], vMaxs.m[m_iAxisH]);
	vMaxs.m[m_iAxisV] = a_Max(m_vStart.m[m_iAxisV], vMaxs.m[m_iAxisV]);

	A3DVECTOR3 aVerts[4];
	BuildPortalVerts(vMins, vMaxs, aVerts);

	m_pPortal->SetVertices(aVerts);
}

void COptCreatePortal::BuildPortalVerts(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, A3DVECTOR3* aVerts)
{
	aVerts[0].m[m_iAxisH] = vMins.m[m_iAxisH];
	aVerts[0].m[m_iAxisV] = vMins.m[m_iAxisV];
	aVerts[0].m[m_iAxisZ] = vMins.m[m_iAxisZ];

	aVerts[1].m[m_iAxisH] = vMins.m[m_iAxisH];
	aVerts[1].m[m_iAxisV] = vMaxs.m[m_iAxisV];
	aVerts[1].m[m_iAxisZ] = vMins.m[m_iAxisZ];

	aVerts[2].m[m_iAxisH] = vMaxs.m[m_iAxisH];
	aVerts[2].m[m_iAxisV] = vMaxs.m[m_iAxisV];
	aVerts[2].m[m_iAxisZ] = vMins.m[m_iAxisZ];

	aVerts[3].m[m_iAxisH] = vMaxs.m[m_iAxisH];
	aVerts[3].m[m_iAxisV] = vMins.m[m_iAxisV];
	aVerts[3].m[m_iAxisZ] = vMins.m[m_iAxisZ];
}

