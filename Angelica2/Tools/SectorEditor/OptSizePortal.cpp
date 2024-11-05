/*
 * FILE: OptSizePortal.cpp
 *
 * DESCRIPTION: Size portal operation class
 *
 * CREATED BY: Duyuxin, 2003/7/24
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "OptSizePortal.h"
#include "Portal.h"
#include "Render.h"
#include "SectorEditorView.h"
#include "SectorEditorDoc.h"
#include "MainFrm.h"
#include "Utility.h"
#include "OrthoViewport.h"
#include "OrthoGrid.h"
#include "WndOrthogonal.h"
#include "A3DFuncs.h"

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
//	Implement COptSizePortal
//
///////////////////////////////////////////////////////////////////////////

COptSizePortal::COptSizePortal(int iID) : COperation(iID)
{
	m_pPortal	= NULL;
	m_pViewWnd	= NULL;
	m_iAxisH	= 0;
	m_iAxisV	= 1;
	m_iAxisZ	= 2;
	m_bDrag		= false;
}

//	Begin operation
bool COptSizePortal::Begin()
{
	if (!COperation::Begin())
		return false;

	return true;
}

//	End operation
bool COptSizePortal::End(bool bCancel)
{
	if (bCancel && m_pPortal)
	{
		//	Restore old size
		m_pPortal->SetVertices(m_aOldVerts);
		m_pPortal = NULL;
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptSizePortal::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptSizePortal::Render(CViewportWnd* pView)
{
	return true;
}

//	Mouse operations
void COptSizePortal::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	UpdatePortal(x, y);
}

void COptSizePortal::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_pPortal)
		return;

	CSectorEditorView* pView = AUX_GetMainFrame()->GetView();
	int iActiveView = pView->GetActiveViewport();
	if (iActiveView == CSectorEditorView::VIEW_PERSPECTIVE)
		return;

	m_pViewWnd	= (COrthogonalWnd*)pView->GetViewport(iActiveView);
	m_iDragSide = m_pViewWnd->ClickPortal(x, y, m_pPortal);

	if (m_iDragSide == SIDE_NONE)
		return;

	pView->GetOrthoAxis(iActiveView, &m_iAxisH, &m_iAxisV, &m_iAxisZ);

	//	Convert position from screen to world
	m_vStart.Set((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(m_vStart, m_vStart);

	m_bDrag		= true;
	m_ptStart.x = x;
	m_ptStart.y = y;

	m_pPortal->GetVertices(m_aOldVerts);
	GetPortalAABB(m_vOldMins, m_vOldMaxs);

	if (m_iDragSide == SIDE_LEFT || m_iDragSide == SIDE_RIGHT)
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
	else
		SetCursor(LoadCursor(NULL, IDC_SIZENS));

	m_bFixView	= true;
}

void COptSizePortal::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	UpdatePortal(x, y);

	m_bDrag		= false;
	m_bFixView	= false;

	AUX_SetCursor(CURSOR_OPERATION);
}

void COptSizePortal::UpdatePortal(int x, int y)
{
	//	Convert position from screen to world
	A3DVECTOR3 vPos((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(vPos, vPos);

	SnapVector(&vPos, &vPos, MIN_GRID);

	A3DVECTOR3 vMins, vMaxs;
	GetPortalAABB(vMins, vMaxs);

	switch (m_iDragSide)
	{
	case SIDE_LEFT:
		
		if (vPos.m[m_iAxisH] == m_vOldMaxs.m[m_iAxisH])
			return;
		
		if (vPos.m[m_iAxisH] > m_vOldMaxs.m[m_iAxisH])
		{
			vMins.m[m_iAxisH] = m_vOldMaxs.m[m_iAxisH];
			vMaxs.m[m_iAxisH] = vPos.m[m_iAxisH];
		}
		else
		{
			vMins.m[m_iAxisH] = vPos.m[m_iAxisH];
			vMaxs.m[m_iAxisH] = m_vOldMaxs.m[m_iAxisH];
		}

		break;

	case SIDE_RIGHT:
		
		if (vPos.m[m_iAxisH] == m_vOldMins.m[m_iAxisH])
			return;
		
		if (vPos.m[m_iAxisH] < m_vOldMins.m[m_iAxisH])
		{
			vMins.m[m_iAxisH] = vPos.m[m_iAxisH];
			vMaxs.m[m_iAxisH] = m_vOldMins.m[m_iAxisH];
		}
		else
		{
			vMins.m[m_iAxisH] = m_vOldMins.m[m_iAxisH];
			vMaxs.m[m_iAxisH] = vPos.m[m_iAxisH];
		}

		break;

	case SIDE_TOP:	
		
		if (vPos.m[m_iAxisV] == m_vOldMins.m[m_iAxisV])
			return;
		
		if (vPos.m[m_iAxisV] < m_vOldMins.m[m_iAxisV])
		{
			vMins.m[m_iAxisV] = vPos.m[m_iAxisV];
			vMaxs.m[m_iAxisV] = m_vOldMins.m[m_iAxisV];
		}
		else
		{
			vMins.m[m_iAxisV] = m_vOldMins.m[m_iAxisV];
			vMaxs.m[m_iAxisV] = vPos.m[m_iAxisV];
		}

		break;

	case SIDE_BOTTOM:
		
		if (vPos.m[m_iAxisV] == m_vOldMaxs.m[m_iAxisV])
			return;
		
		if (vPos.m[m_iAxisV] > m_vOldMaxs.m[m_iAxisV])
		{
			vMins.m[m_iAxisV] = m_vOldMaxs.m[m_iAxisV];
			vMaxs.m[m_iAxisV] = vPos.m[m_iAxisV];
		}
		else
		{
			vMins.m[m_iAxisV] = vPos.m[m_iAxisV];
			vMaxs.m[m_iAxisV] = m_vOldMaxs.m[m_iAxisV];
		}

		break;
	}

	if (vMins.m[m_iAxisH] == vMaxs.m[m_iAxisH] && vMins.m[m_iAxisV] == vMaxs.m[m_iAxisV])
		return;

	A3DVECTOR3 aVerts[4];
	BuildPortalVerts(vMins, vMaxs, aVerts);

	m_pPortal->SetVertices(aVerts);
}

void COptSizePortal::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	//	Finish operations
	AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
}

void COptSizePortal::BuildPortalVerts(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, A3DVECTOR3* aVerts)
{
	aVerts[0].m[m_iAxisH] = vMins.m[m_iAxisH];
	aVerts[0].m[m_iAxisV] = vMins.m[m_iAxisV];

	aVerts[1].m[m_iAxisH] = vMins.m[m_iAxisH];
	aVerts[1].m[m_iAxisV] = vMaxs.m[m_iAxisV];

	aVerts[2].m[m_iAxisH] = vMaxs.m[m_iAxisH];
	aVerts[2].m[m_iAxisV] = vMaxs.m[m_iAxisV];

	aVerts[3].m[m_iAxisH] = vMaxs.m[m_iAxisH];
	aVerts[3].m[m_iAxisV] = vMins.m[m_iAxisV];

	if (vMins.m[m_iAxisH] == vMaxs.m[m_iAxisH])
	{
		aVerts[0].m[m_iAxisZ] = vMins.m[m_iAxisZ];
		aVerts[1].m[m_iAxisZ] = vMins.m[m_iAxisZ];
		aVerts[2].m[m_iAxisZ] = vMaxs.m[m_iAxisZ];
		aVerts[3].m[m_iAxisZ] = vMaxs.m[m_iAxisZ];
	}
	else
	{
		aVerts[0].m[m_iAxisZ] = vMins.m[m_iAxisZ];
		aVerts[1].m[m_iAxisZ] = vMaxs.m[m_iAxisZ];
		aVerts[2].m[m_iAxisZ] = vMaxs.m[m_iAxisZ];
		aVerts[3].m[m_iAxisZ] = vMins.m[m_iAxisZ];
	}
}

void COptSizePortal::GetPortalAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs)
{
	A3DVECTOR3 aVerts[4];
	m_pPortal->GetVertices(aVerts);

	ClearAABB(vMins, vMaxs);
	AddVertexToAABB(vMins, vMaxs, aVerts[0]);
	AddVertexToAABB(vMins, vMaxs, aVerts[1]);
	AddVertexToAABB(vMins, vMaxs, aVerts[2]);
	AddVertexToAABB(vMins, vMaxs, aVerts[3]);
}


