/*
 * FILE: WndXY.cpp
 *
 * DESCRIPTION: XY orthogonal Viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "WndXY.h"
#include "OrthoCamera.h"
#include "OrthoViewport.h"
#include "OrthoGrid.h"
#include "Utility.h"
#include "Render.h"
#include "MainFrm.h"
#include "SectorEditorDoc.h"
#include "Sector.h"

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
//	Implement CXYWnd
//
///////////////////////////////////////////////////////////////////////////

CXYWnd::CXYWnd()
{
}

//	Initialize viewport
bool CXYWnd::Init(CRender* pRender, int x, int y, int iWid, int iHei)
{
	if (!COrthogonalWnd::Init(pRender, x, y, iWid, iHei))
		return false;

	if (!m_pGrid->Init(pRender, 0, 1, 2))
	{
		g_Log.Log("CXYWnd::Init, Failed to initialize COrthoGrid");
		return false;
	}

	m_pCamera->SetPos(g_vOrigin);
	m_pCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);

	return true;
}

//	Release viewport
void CXYWnd::Release()
{
	COrthogonalWnd::Release();
}

//	Render viewport
bool CXYWnd::Render()
{
	if (!m_bRender)
		return true;

	if (!COrthogonalWnd::Render())
		return false;

	return true;
}

//	Mouse action handler
bool CXYWnd::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnMouseMove(x, y, dwFlags))
		return true;

	return false;
}

bool CXYWnd::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnLButtonDown(x, y, dwFlags))
		return true;

	return false;
}

bool CXYWnd::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnRButtonDown(x, y, dwFlags))
		return true;

	return false;
}

bool CXYWnd::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnLButtonUp(x, y, dwFlags))
		return true;
		
	return false;
}

bool CXYWnd::OnRButtonUp(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnRButtonUp(x, y, dwFlags))
		return true;

	return false;
}

bool CXYWnd::OnLButtonDbClk(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnLButtonDbClk(x, y, dwFlags))
		return true;

	return false;
}

bool CXYWnd::OnRButtonDbClk(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnRButtonDbClk(x, y, dwFlags))
		return true;

	return false;
}

void CXYWnd::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
{
}

void CXYWnd::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
{
}

void CXYWnd::DrawCoordinatesChar(int x1, int y1, int x2, int y2)
{
	m_pRender->TextOut(PSTR_X, x2, y2, g_aColTable[COL_COORDINATES]);
	m_pRender->TextOut(PSTR_Y, x1, y1, g_aColTable[COL_COORDINATES]);
}

//	Get camera's position represented by horizontal and vertical measures
void CXYWnd::GetCameraPos(float* pfH, float* pfV)
{
	A3DVECTOR3 vPos = m_pCamera->GetPos();
	*pfH = vPos.x;
	*pfV = vPos.y;
}

//	Set camera's position represented by horizontal and vertical measures
void CXYWnd::SetCameraPos(float fH, float fV)
{
	A3DVECTOR3 vPos(fH, fV, 0.0f);
	m_pCamera->SetPos(vPos);
}

