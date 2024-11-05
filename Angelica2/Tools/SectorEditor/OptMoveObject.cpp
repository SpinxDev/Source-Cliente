/*
 * FILE: OptMoveObject.cpp
 *
 * DESCRIPTION: Move object operation class
 *
 * CREATED BY: Duyuxin, 2003/7/25
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "OptMoveObject.h"
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
//	Implement COptMoveObject
//
///////////////////////////////////////////////////////////////////////////

COptMoveObject::COptMoveObject(int iID) : COperation(iID)
{
	m_pObject	= NULL;
	m_pViewWnd	= NULL;
	m_iAxisH	= 0;
	m_iAxisV	= 1;
	m_iAxisZ	= 2;
	m_bDrag		= false;
}

//	Begin operation
bool COptMoveObject::Begin()
{
	if (!COperation::Begin())
		return false;

	return true;
}

//	End operation
bool COptMoveObject::End(bool bCancel)
{
	if (bCancel && m_pObject)
	{
		//	Restore object
		if (m_pObject->GetID() == EOID_SECTOR)
			((CSector*)m_pObject)->SetMinsAndMaxs(m_vOldMins, m_vOldMaxs);
		else if (m_pObject->GetID() == EOID_PORTAL)
		{
			CPortal* pPortal = (CPortal*)m_pObject;
			pPortal->SetVertices(m_aOldVerts);
		}

		m_pObject = NULL;
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptMoveObject::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptMoveObject::Render(CViewportWnd* pView)
{
	return true;
}

//	Mouse operations
void COptMoveObject::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	UpdateObject(x, y);
}

void COptMoveObject::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_pObject)
		return;

	CSectorEditorView* pView = AUX_GetMainFrame()->GetView();
	int iActiveView = pView->GetActiveViewport();
	if (iActiveView == CSectorEditorView::VIEW_PERSPECTIVE)
		return;

	m_pViewWnd = (COrthogonalWnd*)pView->GetViewport(iActiveView);
	pView->GetOrthoAxis(iActiveView, &m_iAxisH, &m_iAxisV, &m_iAxisZ);

	//	Convert position from screen to world
	m_vLast.Set((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(m_vLast, m_vLast);
	SnapVector(&m_vLast, &m_vLast, MIN_GRID);

	if (m_pObject->GetID() == EOID_SECTOR)
	{
		m_vOldMins = ((CSector*)m_pObject)->GetMins();
		m_vOldMaxs = ((CSector*)m_pObject)->GetMaxs();
	}
	else if (m_pObject->GetID() == EOID_PORTAL)
		((CPortal*)m_pObject)->GetVertices(m_aOldVerts);

	m_bDrag		= true;
	m_bFixView	= true;
	m_ptLast.x	= x;
	m_ptLast.y	= y;

	SetCursor(LoadCursor(NULL, IDC_SIZEALL));
}

void COptMoveObject::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	UpdateObject(x, y);

	m_bDrag		= false;
	m_bFixView	= false;

	AUX_SetCursor(CURSOR_OPERATION);
}

void COptMoveObject::UpdateObject(int x, int y)
{
	if (x == m_ptLast.x && y == m_ptLast.y)
		return;

	A3DVECTOR3 vDelta, vPos((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(vPos, vPos);

	vDelta = vPos - m_vLast;
	vDelta.m[m_iAxisZ] = 0.0f;

	m_ptLast.x	= x;
	m_ptLast.y	= y;
	SnapVector(&vPos, &m_vLast, MIN_GRID);

	m_pObject->Move(vDelta);
}

void COptMoveObject::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_pObject->GetID() == EOID_SECTOR)
	{
		//	Record sector's volume for next one
		CSector* pSector = (CSector*)m_pObject;
		g_vLastSectorMins = pSector->GetMins();
		g_vLastSectorMaxs = pSector->GetMaxs();
	}

	//	Finish operations
	AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
}



