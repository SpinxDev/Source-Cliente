/*
 * FILE: WndViewport.cpp
 *
 * DESCRIPTION: Viewport window base class
 *
 * CREATED BY: Duyuxin, 2003/7/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "Render.h"
#include "WndViewport.h"

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
//	Implement CViewportWnd
//
///////////////////////////////////////////////////////////////////////////

CViewportWnd::CViewportWnd()
{
	m_pRender	= NULL;
	m_x			= 0;
	m_y			= 0;
	m_iWidth	= 100;
	m_iHeight	= 100;
	m_bRender	= true;
	m_bActive	= false;

	m_LeftDrag.bDrag	= false;
	m_RightDrag.bDrag	= false;
}

//	Initialize viewport
bool CViewportWnd::Init(CRender* pRender, int x, int y, int iWid, int iHei)
{
	m_pRender = pRender;
	return true;
}

//	Release viewport
void CViewportWnd::Release()
{
	m_pRender	= NULL;
}

//	Resize viewport
bool CViewportWnd::Move(int x, int y, int iWid, int iHei)
{
	m_x			= x;
	m_y			= y;
	m_iWidth	= iWid;
	m_iHeight	= iHei;

	return true;
}

//	Draw active frame
void CViewportWnd::DrawActiveFrame()
{
	ASSERT(m_pRender);

	int l, r, t, b;
	l = m_x + 1;
	r = m_x + m_iWidth - 2;
	t = m_y + 1;
	b = m_y + m_iHeight - 2;

	DWORD dwCol = g_aColTable[COL_VIEWFRAME];

	g_pA3DGDI->Draw2DLine(A3DPOINT2(l, t), A3DPOINT2(r, t), dwCol);
	g_pA3DGDI->Draw2DLine(A3DPOINT2(r, t), A3DPOINT2(r, b), dwCol);
	g_pA3DGDI->Draw2DLine(A3DPOINT2(l, t), A3DPOINT2(l, b), dwCol);
	g_pA3DGDI->Draw2DLine(A3DPOINT2(l, b), A3DPOINT2(r, b), dwCol);
}

//	Mouse action handler
bool CViewportWnd::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if (m_LeftDrag.bDrag)
	{
		m_LeftDrag.iLastX = x;
		m_LeftDrag.iLastY = y;
	}

	return false;
}

void CViewportWnd::Activate(bool bActivate)
{
	m_bActive = bActivate;

	if (!bActivate)
	{
		m_LeftDrag.bDrag	= false;
		m_RightDrag.bDrag	= false;
	}
}

bool CViewportWnd::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_LeftDrag.bDrag	= true;
	m_LeftDrag.iLastX	= x;
	m_LeftDrag.iLastY	= y;
	return false;
}

bool CViewportWnd::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	return false;
}

bool CViewportWnd::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_LeftDrag.bDrag)
		m_LeftDrag.bDrag = false;

	return false;
}

bool CViewportWnd::OnRButtonUp(int x, int y, DWORD dwFlags)
{
	return false;
}

bool CViewportWnd::OnLButtonDbClk(int x, int y, DWORD dwFlags)
{
	return false;
}

bool CViewportWnd::OnRButtonDbClk(int x, int y, DWORD dwFlags)
{
	return false;
}

//	Text out basing on viewport coordinates
void CViewportWnd::TextOut(int x, int y, char* szText, DWORD color)
{
	if (m_pRender)
		m_pRender->TextOut(m_x+x, m_y+y, szText, color);
}


