/*
 * FILE: WndYZ.h
 *
 * DESCRIPTION: YZ orthogonal Viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "WndYZ.h"
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
//	Implement CYZWnd
//
///////////////////////////////////////////////////////////////////////////

CYZWnd::CYZWnd()
{
}

//	Initialize viewport
bool CYZWnd::Init(CRender* pRender, int x, int y, int iWid, int iHei)
{
	if (!COrthogonalWnd::Init(pRender, x, y, iWid, iHei))
		return false;

	if (!m_pGrid->Init(pRender, 2, 1, 0))
	{
		g_Log.Log("CYZWnd::Init, Failed to initialize COrthoGrid");
		return false;
	}

	m_pCamera->SetPos(g_vOrigin);
	m_pCamera->SetDirAndUp(-g_vAxisX, g_vAxisY);

	return true;
}

//	Release viewport
void CYZWnd::Release()
{
	COrthogonalWnd::Release();
}

//	Render viewport
bool CYZWnd::Render()
{
	if (!m_bRender)
		return true;

	if (!COrthogonalWnd::Render())
		return false;

	return true;
}

//	Mouse action handler
bool CYZWnd::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnMouseMove(x, y, dwFlags))
		return true;

	return false;
}

bool CYZWnd::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnLButtonDown(x, y, dwFlags))
		return true;

	return false;
}

bool CYZWnd::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnRButtonDown(x, y, dwFlags))
		return true;

	return false;
}

bool CYZWnd::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnLButtonUp(x, y, dwFlags))
		return true;

	return false;
}

bool CYZWnd::OnRButtonUp(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnRButtonUp(x, y, dwFlags))
		return true;

	return false;
}

bool CYZWnd::OnLButtonDbClk(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnLButtonDbClk(x, y, dwFlags))
		return true;

	return false;
}

bool CYZWnd::OnRButtonDbClk(int x, int y, DWORD dwFlags)
{
	if (COrthogonalWnd::OnRButtonDbClk(x, y, dwFlags))
		return true;

	return false;
}

void CYZWnd::DrawCoordinatesChar(int x1, int y1, int x2, int y2)
{
	m_pRender->TextOut(PSTR_Y, x1, y1, g_aColTable[COL_COORDINATES]);
	m_pRender->TextOut(PSTR_Z, x2, y2, g_aColTable[COL_COORDINATES]);
}

//	Get camera's position represented by horizontal and vertical measures
void CYZWnd::GetCameraPos(float* pfH, float* pfV)
{
	A3DVECTOR3 vPos = m_pCamera->GetPos();
	*pfH = vPos.z;
	*pfV = vPos.y;
}

//	Set camera's position represented by horizontal and vertical measures
void CYZWnd::SetCameraPos(float fH, float fV)
{
	A3DVECTOR3 vPos(0.0f, fV, fH);
	m_pCamera->SetPos(vPos);
}


