/*
 * FILE: OptZoom.cpp
 *
 * DESCRIPTION: Zoom operation class
 *
 * CREATED BY: Duyuxin, 2003/7/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "OptZoom.h"
#include "Render.h"
#include "SectorEditorView.h"
#include "SectorEditorDoc.h"
#include "MainFrm.h"
#include "Utility.h"
#include "OrthoViewport.h"
#include "OrthoGrid.h"
#include "WndOrthogonal.h"
#include "WireCollector.h"

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
//	Implement COptZoom
//
///////////////////////////////////////////////////////////////////////////

COptZoom::COptZoom(int iID) : COperation(iID)
{
	m_pViewWnd	= NULL;
	m_iAxisH	= 0;
	m_iAxisV	= 1;
	m_iAxisZ	= 2;
	m_bDrag		= false;
	m_bMagnify	= true;
}

//	Begin operation
bool COptZoom::Begin()
{
	if (!COperation::Begin())
		return false;

	return true;
}

//	End operation
bool COptZoom::End(bool bCancel)
{
	if (bCancel)
	{
	}

	return COperation::End(bCancel);
}

//	Tick routinue
bool COptZoom::FrameMove(DWORD dwTime)
{
	return true;
}

//	Render routinue
bool COptZoom::Render(CViewportWnd* pView)
{
	if (m_pViewWnd != pView || !m_bDrag)
		return true;

	A3DVECTOR3 vPos, vMins, vMaxs;
	int i;

	for (i=0; i < 3; i++)
	{
		vMins.m[i] = min2(m_vStart.m[i], m_vEnd.m[i]);
		vMaxs.m[i] = max2(m_vStart.m[i], m_vEnd.m[i]);
	}

	A3DLVERTEX aVerts[4];
	static WORD aIndices[8] = {0, 1, 1, 2, 2, 3, 3, 0};

	aVerts[0].x = vMins.x;
	aVerts[0].y = vMins.y;
	aVerts[0].z = vMins.z;
	aVerts[2].x = vMaxs.x;
	aVerts[2].y = vMaxs.y;
	aVerts[2].z = vMaxs.z;

	vPos.m[m_iAxisH] = vMins.m[m_iAxisH];
	vPos.m[m_iAxisV] = vMaxs.m[m_iAxisV];
	vPos.m[m_iAxisZ] = vMins.m[m_iAxisZ];
	aVerts[1].x = vPos.x;
	aVerts[1].y = vPos.y;
	aVerts[1].z = vPos.z;

	vPos.m[m_iAxisH] = vMaxs.m[m_iAxisH];
	vPos.m[m_iAxisV] = vMins.m[m_iAxisV];
	vPos.m[m_iAxisZ] = vMins.m[m_iAxisZ];
	aVerts[3].x = vPos.x;
	aVerts[3].y = vPos.y;
	aVerts[3].z = vPos.z;

	for (i=0; i < 4; i++)
	{
		aVerts[i].diffuse	= g_aColTable[COL_SELECTAREA];
		aVerts[i].specular	= A3DCOLORRGBA(0, 0, 0, 255);
		aVerts[i].tu		= 0.0f;
		aVerts[i].tv		= 0.0f;
	}

	//	Render
	g_Render.GetA3DDevice()->DrawIndexedPrimitiveUP(A3DPT_LINELIST, 0, 4, 4, aIndices,
							A3DFMT_INDEX16, aVerts, sizeof (A3DLVERTEX));

	return true;
}

void COptZoom::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	UpdateSelection(x, y);
}

void COptZoom::OnLButtonDown(int x, int y, DWORD dwFlags)
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
	m_vStart.Set((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(m_vStart, m_vStart);

	m_bDrag		= true;
	m_vEnd		= m_vStart;
	m_bFixView	= true;
}

void COptZoom::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_iState != STATE_DOING || !m_bDrag)
		return;

	m_bDrag		= false;
	m_bFixView	= false;

	UpdateSelection(x, y);

	float l = min2(m_vStart.m[m_iAxisH], m_vEnd.m[m_iAxisH]);
	float r = max2(m_vStart.m[m_iAxisH], m_vEnd.m[m_iAxisH]);
	float b = min2(m_vStart.m[m_iAxisV], m_vEnd.m[m_iAxisV]);
	float t = max2(m_vStart.m[m_iAxisV], m_vEnd.m[m_iAxisV]);

	m_pViewWnd->Zoom(m_bMagnify, l, r, b, t);
}

void COptZoom::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	//	Finish operations
	AUX_GetMainFrame()->PostMessage(MSG_ENDOPERATION, 0, 0);
}

void COptZoom::UpdateSelection(int x, int y)
{
	//	Convert position from screen to world
	m_vEnd.Set((float)x, (float)y, 0.5f);
	m_pViewWnd->InvTransform(m_vEnd, m_vEnd);
}


