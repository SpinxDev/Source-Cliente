/*
 * FILE: WndXZ.cpp
 *
 * DESCRIPTION: XZ orthogonal Viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "WndXZ.h"
#include "OrthoCamera.h"
#include "OrthoViewport.h"
#include "OrthoGrid.h"
#include "Utility.h"
#include "Render.h"

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
//	Implement CXZWnd
//
///////////////////////////////////////////////////////////////////////////

CXZWnd::CXZWnd()
{
}

//	Initialize viewport
bool CXZWnd::Init(CRender* pRender, int x, int y, int iWid, int iHei)
{
	if (!COrthogonalWnd::Init(pRender, x, y, iWid, iHei))
		return false;

	if (!m_pGrid->Init(pRender, 0, 2, 1))
	{
		g_Log.Log("CXZWnd::Init, Failed to initialize COrthoGrid");
		return false;
	}

	m_pCamera->SetPos(g_vOrigin);
	m_pCamera->SetDirAndUp(-g_vAxisY, g_vAxisZ);

	return true;
}

//	Release viewport
void CXZWnd::Release()
{
	COrthogonalWnd::Release();
}

//	Render viewport
bool CXZWnd::Render()
{
	if (!m_bRender)
		return true;

	if (!COrthogonalWnd::Render())
		return false;

	return true;
}

//	Mouse action handler
bool CXZWnd::OnMouseMove(int x, int y, DWORD dwFlags)
{
	return COrthogonalWnd::OnMouseMove(x, y, dwFlags);
}

bool CXZWnd::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	return COrthogonalWnd::OnLButtonDown(x, y, dwFlags);
}

bool CXZWnd::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	return COrthogonalWnd::OnRButtonDown(x, y, dwFlags);
}

bool CXZWnd::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	return COrthogonalWnd::OnLButtonUp(x, y, dwFlags);
}

bool CXZWnd::OnRButtonUp(int x, int y, DWORD dwFlags)
{
	return COrthogonalWnd::OnRButtonUp(x, y, dwFlags);
}

bool CXZWnd::OnLButtonDbClk(int x, int y, DWORD dwFlags)
{
	return COrthogonalWnd::OnLButtonDbClk(x, y, dwFlags);
}

bool CXZWnd::OnRButtonDbClk(int x, int y, DWORD dwFlags)
{
	return COrthogonalWnd::OnRButtonDbClk(x, y, dwFlags);
}

void CXZWnd::DrawCoordinatesChar(int x1, int y1, int x2, int y2)
{
	m_pRender->TextOut(PSTR_X, x2, y2, g_aColTable[COL_COORDINATES]);
	m_pRender->TextOut(PSTR_Z, x1, y1, g_aColTable[COL_COORDINATES]);
}

//	Get camera's position represented by horizontal and vertical measures
void CXZWnd::GetCameraPos(float* pfH, float* pfV)
{
	A3DVECTOR3 vPos = m_pCamera->GetPos();
	*pfH = vPos.x;
	*pfV = vPos.z;
}

//	Set camera's position represented by horizontal and vertical measures
void CXZWnd::SetCameraPos(float fH, float fV)
{
	A3DVECTOR3 vPos(fH, 0.0f, fV);
	m_pCamera->SetPos(vPos);
}


