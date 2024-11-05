/*
 * FILE: OptSizeSector.cpp
 *
 * DESCRIPTION: Size sector operation class
 *
 * CREATED BY: Duyuxin, 2003/7/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "OptSizeSector.h"
#include "Sector.h"
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
//	Implement COptSizeSector
//
///////////////////////////////////////////////////////////////////////////

COptSizeSector::COptSizeSector(int iID) : COperation(iID)
{
	m_pSector	= NULL;
	m_pViewWnd	= NULL;
	m_iAxisH	= 0;
	m_iAxisV	= 1;
	m_iAxisZ	= 2;
	m_bDrag		= false;
}

//	Begin operation
bool COptSizeSector::Begin()
{
	if (!COperation::Begin())
		return false;

	return true;
}

//	End operation
bool COptSizeSector::End(bool bCancel)
{
	if (bCancel && m_pSector)
	{
		//	Restore old size
		m_pSector->SetMinsAndMaxs(m_vOldMins, m_vOldMaxs);
		m_pSector = NULL;
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptSizeSector::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptSizeSector::Render(CViewportWnd* pView)
{
	return true;
}

//	Mouse operations
void COptSizeSector::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	UpdateSector(x, y);
}

void COptSizeSector::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_pSector)
		return;

	CSectorEditorView* pView = AUX_GetMainFrame()->GetView();
	int iActiveView = pView->GetActiveViewport();
	if (iActiveView == CSectorEditorView::VIEW_PERSPECTIVE)
		return;

	m_pViewWnd	= (COrthogonalWnd*)pView->GetViewport(iActiveView);
	m_iDragSide = m_pViewWnd->ClickSector(x, y, m_pSector);

	if (m_iDragSide == SIDE_NONE)
		return;

	pView->GetOrthoAxis(iActiveView, &m_iAxisH, &m_iAxisV, &m_iAxisZ);

	//	Convert position from screen to world
	m_vStart.Set((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(m_vStart, m_vStart);

	m_bDrag		= true;
	m_ptStart.x = x;
	m_ptStart.y = y;
	m_vOldMins	= m_pSector->GetMins();
	m_vOldMaxs	= m_pSector->GetMaxs();

	if (m_iDragSide == SIDE_LEFT || m_iDragSide == SIDE_RIGHT)
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
	else
		SetCursor(LoadCursor(NULL, IDC_SIZENS));

	m_bFixView	= true;
}

void COptSizeSector::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	UpdateSector(x, y);

	m_bDrag		= false;
	m_bFixView	= false;

	AUX_SetCursor(CURSOR_OPERATION);
}

void COptSizeSector::UpdateSector(int x, int y)
{
	//	Convert position from screen to world
	A3DVECTOR3 vPos((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(vPos, vPos);

	SnapVector(&vPos, &vPos, MIN_GRID);

	A3DVECTOR3 vMins = m_pSector->GetMins();
	A3DVECTOR3 vMaxs = m_pSector->GetMaxs();

	int iAxis, iSign;

	switch (m_iDragSide)
	{
	case SIDE_LEFT:
		
		if (vPos.m[m_iAxisH] == m_vOldMaxs.m[m_iAxisH])
			return;
		
		if (vPos.m[m_iAxisH] > m_vOldMaxs.m[m_iAxisH])
		{
			vMins.m[m_iAxisH] = m_vOldMaxs.m[m_iAxisH];
			vMaxs.m[m_iAxisH] = vPos.m[m_iAxisH];
			iSign = 1;
		}
		else
		{
			vMins.m[m_iAxisH] = vPos.m[m_iAxisH];
			vMaxs.m[m_iAxisH] = m_vOldMaxs.m[m_iAxisH];
			iSign = -1;
		}

		iAxis = m_iAxisH;
		break;

	case SIDE_RIGHT:
		
		if (vPos.m[m_iAxisH] == m_vOldMins.m[m_iAxisH])
			return;
		
		if (vPos.m[m_iAxisH] < m_vOldMins.m[m_iAxisH])
		{
			vMins.m[m_iAxisH] = vPos.m[m_iAxisH];
			vMaxs.m[m_iAxisH] = m_vOldMins.m[m_iAxisH];
			iSign = -1;
		}
		else
		{
			vMins.m[m_iAxisH] = m_vOldMins.m[m_iAxisH];
			vMaxs.m[m_iAxisH] = vPos.m[m_iAxisH];
			iSign = 1;
		}

		iAxis = m_iAxisH;
		break;

	case SIDE_TOP:	
		
		if (vPos.m[m_iAxisV] == m_vOldMins.m[m_iAxisV])
			return;
		
		if (vPos.m[m_iAxisV] < m_vOldMins.m[m_iAxisV])
		{
			vMins.m[m_iAxisV] = vPos.m[m_iAxisV];
			vMaxs.m[m_iAxisV] = m_vOldMins.m[m_iAxisV];
			iSign = -1;
		}
		else
		{
			vMins.m[m_iAxisV] = m_vOldMins.m[m_iAxisV];
			vMaxs.m[m_iAxisV] = vPos.m[m_iAxisV];
			iSign = 1;
		}

		iAxis = m_iAxisV;
		break;

	case SIDE_BOTTOM:
		
		if (vPos.m[m_iAxisV] == m_vOldMaxs.m[m_iAxisV])
			return;
		
		if (vPos.m[m_iAxisV] > m_vOldMaxs.m[m_iAxisV])
		{
			vMins.m[m_iAxisV] = m_vOldMaxs.m[m_iAxisV];
			vMaxs.m[m_iAxisV] = vPos.m[m_iAxisV];
			iSign = 1;
		}
		else
		{
			vMins.m[m_iAxisV] = vPos.m[m_iAxisV];
			vMaxs.m[m_iAxisV] = m_vOldMaxs.m[m_iAxisV];
			iSign = -1;
		}

		iAxis = m_iAxisV;
		break;
	}

	//	Sector collision ?
	AUX_GetMainFrame()->GetDocument()->SizeSectorCollision(vMins, vMaxs, m_pSector, iAxis, iSign);
	if (vMins.x == vMaxs.x || vMins.y == vMaxs.y || vMins.z == vMaxs.z)
		return;

	m_pSector->SetMinsAndMaxs(vMins, vMaxs);
}

void COptSizeSector::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	//	Record sector's volume for next one
	g_vLastSectorMins = m_pSector->GetMins();
	g_vLastSectorMaxs = m_pSector->GetMaxs();

	//	Finish operations
	AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
}

