/*
 * FILE: OrthoGrid.cpp
 *
 * DESCRIPTION: Grid used in orthogonal window
 *
 * CREATED BY: Duyuxin, 2003/7/14
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "OrthoGrid.h"
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
//	Implement COrthoGrid
//
///////////////////////////////////////////////////////////////////////////

COrthoGrid::COrthoGrid()
{
	m_pRender	= NULL;
	m_pStream	= NULL;
	m_fGridSize = 1.0f;
	m_iNumLineH	= 0;
	m_iNumLineV	= 0;
	m_iAxisH	= 0;
	m_iAxisV	= 1;
	m_iAxisZ	= 2;

	m_AlignArea.Set(0.0f, 0.0f, 0.0f, 0.0f);
	m_Area.Set(0.0f, 0.0f, 0.0f, 0.0f);
}

//	Initialize viewport
bool COrthoGrid::Init(CRender* pRender, int iAxisH, int iAxisV, int iAxisZ)
{
	m_pRender	= pRender;
	m_iAxisH	= iAxisH;
	m_iAxisV	= iAxisV;
	m_iAxisZ	= iAxisZ;

	if (!(m_pStream = new A3DStream))
	{
		g_Log.Log("COrthoGrid::Init, Failed to new A3DStream");
		return false;
	}

	return true;
}

//	Release viewport
void COrthoGrid::Release()
{
	if (m_pStream)
	{
		m_pStream->Release();
		delete m_pStream;
		m_pStream = NULL;
	}

	m_pRender = NULL;
}

//	Render viewport
bool COrthoGrid::Render()
{
	if (!m_pStream)
		return true;

	A3DDevice* pDevice = m_pRender->GetA3DDevice();

	//	Set render states
	pDevice->SetWorldMatrix(IdentityMatrix());
	pDevice->GetD3DDevice()->SetTexture(0, NULL);
	m_pStream->Appear();

	//	Render scene
	pDevice->DrawPrimitive(A3DPT_LINELIST, 0, m_iNumLineH + m_iNumLineV);

	//	Restore render states

	return true;
}

/*	Resize grid

	l, r, b, t: new grid area (needn't be aligned)
	fGridSize: grid size (interval between two grid line)
*/
bool COrthoGrid::Resize(float l, float r, float b, float t, float fGridSize)
{
	ASSERT(m_pStream);

	//	Non-align area
	m_Area.Set(l, t, r, b);

	//	Align grid
	m_AlignArea.left	= (float)floor(l / fGridSize) * fGridSize;
	m_AlignArea.bottom	= (float)floor(b / fGridSize) * fGridSize;
	m_AlignArea.right	= (float)ceil(r / fGridSize) * fGridSize;
	m_AlignArea.top		= (float)ceil(t / fGridSize) * fGridSize;
	
	int iNumLineH	= (int)((m_AlignArea.top - m_AlignArea.bottom) / fGridSize + 0.5f) + 3;
	int iNumLineV	= (int)((m_AlignArea.right - m_AlignArea.left) / fGridSize + 0.5f) + 3;

	if (iNumLineH * iNumLineV != m_iNumLineH * m_iNumLineV)
	{
		//	Create new stream
		m_pStream->Release();

		int iNumVert = (iNumLineH + iNumLineV) * 2;
		if (!m_pStream->Init(m_pRender->GetA3DDevice(), A3DVT_LVERTEX, iNumVert, 0, 0, 0))
		{
			g_Log.Log("COrthoGrid::Resize, Failed to new A3DStream");
			return false;
		}
	}

	m_fGridSize = fGridSize;
	m_iNumLineH = iNumLineH;
	m_iNumLineV = iNumLineV;

	A3DLVERTEX* pVerts;
	if (!m_pStream->LockVertexBuffer(0, 0, (LPBYTE*) &pVerts, NULL))
	{
		g_Log.Log("COrthoGrid::Resize, Failed to lock vertex buffer");
		return false;
	}

	int i, iCount=0;
	A3DVECTOR3 vPos;
	DWORD dwSpecular = 0xff000000;

	//	Make horizontal lines
	for (i=0; i < m_iNumLineH; i++)
	{
		vPos.m[m_iAxisH] = m_AlignArea.left; 
		vPos.m[m_iAxisV] = m_AlignArea.bottom + i * m_fGridSize;
		vPos.m[m_iAxisZ] = 0.0f;

		pVerts[iCount].x = vPos.x;
		pVerts[iCount].y = vPos.y;
		pVerts[iCount].z = vPos.z;

		pVerts[iCount].tu		= 0.0f;
		pVerts[iCount].tv		= 0.0f;
		pVerts[iCount].specular	= dwSpecular;
		pVerts[iCount].diffuse	= g_aColTable[COL_GRID];

		iCount++;

		vPos.m[m_iAxisH] = m_AlignArea.right; 
		pVerts[iCount].x = vPos.x;
		pVerts[iCount].y = vPos.y;
		pVerts[iCount].z = vPos.z;

		pVerts[iCount].tu		= 0.0f;
		pVerts[iCount].tv		= 0.0f;
		pVerts[iCount].specular	= dwSpecular;
		pVerts[iCount].diffuse	= g_aColTable[COL_GRID];

		iCount++;
	}

	//	Make vertical lines
	for (i=0; i < m_iNumLineV; i++)
	{
		vPos.m[m_iAxisH] = m_AlignArea.left + i * m_fGridSize; 
		vPos.m[m_iAxisV] = m_AlignArea.bottom;
		vPos.m[m_iAxisZ] = 0.0f;

		pVerts[iCount].x = vPos.x;
		pVerts[iCount].y = vPos.y;
		pVerts[iCount].z = vPos.z;

		pVerts[iCount].tu		= 0.0f;
		pVerts[iCount].tv		= 0.0f;
		pVerts[iCount].specular	= dwSpecular;
		pVerts[iCount].diffuse	= g_aColTable[COL_GRID];

		iCount++;

		vPos.m[m_iAxisV] = m_AlignArea.top; 
		pVerts[iCount].x = vPos.x;
		pVerts[iCount].y = vPos.y;
		pVerts[iCount].z = vPos.z;

		pVerts[iCount].tu		= 0.0f;
		pVerts[iCount].tv		= 0.0f;
		pVerts[iCount].specular	= dwSpecular;
		pVerts[iCount].diffuse	= g_aColTable[COL_GRID];

		iCount++;
	}

	if (!m_pStream->UnlockVertexBuffer())
	{
		g_Log.Log("COrthoGrid::Resize, Failed to unlock vertex buffer");
		return false;
	}

	return true;
}

/*	Resize grid. This function assume grid area's center donesn't change, only
	change grid's size

	fExtH: horizontal extent
	fExtV: vertical extent
	fGridSize: grid size (interval between two grid line)
*/
bool COrthoGrid::Resize(float fExtH, float fExtV, float fGridSize)
{
	float ch = (m_Area.left + m_Area.right) * 0.5f;
	float cv = (m_Area.top + m_Area.bottom) * 0.5f;
	return Resize(ch-fExtH, ch+fExtH, cv-fExtV, cv+fExtV, fGridSize);
}

/*	Move grid without resizing

	l, b: new position (neendn't be aligned) of grid area's left-bottom
*/
bool COrthoGrid::MoveTo(float l, float b)
{
	ASSERT(m_pStream);

	float d1 = m_Area.right - m_Area.left;
	float d2 = m_Area.top - m_Area.bottom;
	m_Area.Set(l, b+d2, l+d1, b);

	//	Align grid
	float fLeft		= (float)floor(l / m_fGridSize) * m_fGridSize;
	float fBottom	= (float)floor(b / m_fGridSize) * m_fGridSize;

	if (m_fLeft == fLeft && m_fBottom == fBottom)
		return true;

	m_AlignArea.left	= fLeft;
	m_AlignArea.bottom	= fBottom;
	m_AlignArea.right	= fLeft + m_iNumLineV * m_fGridSize;
	m_AlignArea.top		= fBottom + m_iNumLineH * m_fGridSize;

	A3DLVERTEX* pVerts;
	if (!m_pStream->LockVertexBuffer(0, 0, (LPBYTE*) &pVerts, 0))
	{
		g_Log.Log("COrthoGrid::MoveTo, Failed to lock vertex buffer");
		return false;
	}

	int i, iCount=0;
	A3DVECTOR3 vPos;
	DWORD dwSpecular = 0xff000000;

	//	Make horizontal lines
	for (i=0; i < m_iNumLineH; i++)
	{
		vPos.m[m_iAxisH] = m_AlignArea.left; 
		vPos.m[m_iAxisV] = m_AlignArea.bottom + i * m_fGridSize;
		vPos.m[m_iAxisZ] = 0.0f;

		pVerts[iCount].x = vPos.x;
		pVerts[iCount].y = vPos.y;
		pVerts[iCount].z = vPos.z;

		pVerts[iCount].tu		= 0.0f;
		pVerts[iCount].tv		= 0.0f;
		pVerts[iCount].specular	= dwSpecular;
		pVerts[iCount].diffuse	= g_aColTable[COL_GRID];

		iCount++;

		vPos.m[m_iAxisH] = m_AlignArea.right; 
		pVerts[iCount].x = vPos.x;
		pVerts[iCount].y = vPos.y;
		pVerts[iCount].z = vPos.z;

		pVerts[iCount].tu		= 0.0f;
		pVerts[iCount].tv		= 0.0f;
		pVerts[iCount].specular	= dwSpecular;
		pVerts[iCount].diffuse	= g_aColTable[COL_GRID];

		iCount++;
	}

	//	Make vertical lines
	for (i=0; i < m_iNumLineV; i++)
	{
		vPos.m[m_iAxisH] = m_AlignArea.left + i * m_fGridSize;
		vPos.m[m_iAxisV] = m_AlignArea.bottom;
		vPos.m[m_iAxisZ] = 0.0f;

		pVerts[iCount].x = vPos.x;
		pVerts[iCount].y = vPos.y;
		pVerts[iCount].z = vPos.z;

		pVerts[iCount].tu		= 0.0f;
		pVerts[iCount].tv		= 0.0f;
		pVerts[iCount].specular	= dwSpecular;
		pVerts[iCount].diffuse	= g_aColTable[COL_GRID];

		iCount++;

		vPos.m[m_iAxisV] = m_AlignArea.top; 
		pVerts[iCount].x = vPos.x;
		pVerts[iCount].y = vPos.y;
		pVerts[iCount].z = vPos.z;

		pVerts[iCount].tu		= 0.0f;
		pVerts[iCount].tv		= 0.0f;
		pVerts[iCount].specular	= dwSpecular;
		pVerts[iCount].diffuse	= g_aColTable[COL_GRID];

		iCount++;
	}

	if (!m_pStream->UnlockVertexBuffer())
	{
		g_Log.Log("COrthoGrid::MoveTo, Failed to unlock vertex buffer");
		return false;
	}

	return true;
}

/*	Move grid without resizing

	fOffH, fOffV: moving offset of grid area in horizontal and vertical directions
*/
bool COrthoGrid::Move(float fOffH, float fOffV)
{
	float l = m_Area.left + fOffH;
	float b = m_Area.bottom + fOffV;
	return MoveTo(l, b);
}

