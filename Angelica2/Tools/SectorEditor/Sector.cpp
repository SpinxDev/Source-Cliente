/*
 * FILE: Sector.cpp
 *
 * DESCRIPTION: Sector class
 *
 * CREATED BY: Duyuxin, 2003/7/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "Render.h"
#include "Sector.h"
#include "Utility.h"
#include "SectorEditorDoc.h"
#include "MainFrm.h"

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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
//	Implement CSector
//
///////////////////////////////////////////////////////////////////////////

CSector::CSector()
{
	m_iID			= EOID_SECTOR;
	m_bDelayRender	= false;
	m_bSelected		= false;
}

//	Initialize sector
bool CSector::Init(CRender* pRender, const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	if (!m_Box3D.Init(pRender, vMins, vMaxs))
		return false;

	SetMinsAndMaxs(vMins, vMaxs);

	m_Box3D.SetColor(g_aColTable[COL_SECTOR]);

	return true;
}

//	Release sector
void CSector::Release()
{
	m_Box3D.Release();

	m_bSelected	= false;
}

void CSector::Select(bool bSel) 
{
	CEditObject::Select(bSel);
	SetColor(bSel ? g_aColTable[COL_SELSECTOR] : g_aColTable[COL_SECTOR]);
}

//	Render sector
bool CSector::Render()
{
	if (!IsVisible())
		return true;

	m_Box3D.Render();
	return true;
}

void CSector::SetMinsAndMaxs(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	SnapVector((A3DVECTOR3*)&vMins, &m_vMins, MIN_GRID);
	SnapVector((A3DVECTOR3*)&vMaxs, &m_vMaxs, MIN_GRID);
	m_Box3D.SetMinsAndMaxs(m_vMins, m_vMaxs);
}

bool CSector::Collision(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	A3DVECTOR3 vCheckMins = vMins + MIN_GRID * 0.5f;
	A3DVECTOR3 vCheckMaxs = vMaxs - MIN_GRID * 0.5f;

	return AABBToAABB(m_vMins, m_vMaxs, vCheckMins, vCheckMaxs);
}

//	Move object
void CSector::Move(const A3DVECTOR3& vDelta)
{
	AUX_GetMainFrame()->GetDocument()->MoveSectorCollision(m_vMins, m_vMaxs, this, vDelta);
	m_Box3D.SetMinsAndMaxs(m_vMins, m_vMaxs);
}

/*	Bound sector

	vMins, vMaxs: bound's volume
	bWindow: true, window mode; false, crossing mode
*/
void CSector::Bound(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, bool bWindow)
{
	bool bIn;

	if (bWindow)
		bIn = AABBInAABB(m_vMins, m_vMaxs, vMins, vMaxs);
	else
		bIn = AABBToAABB(m_vMins, m_vMaxs, vMins, vMaxs);

	SetInBoundFlag(bIn);
}

//	Load data from file
bool CSector::Load(FILE* fp)
{
	A3DVECTOR3 vMins, vMaxs;

	fread(&vMins, 1, sizeof (vMins), fp);
	fread(&vMaxs, 1, sizeof (vMaxs), fp);

	SetMinsAndMaxs(vMins, vMaxs);

	return true;
}

//	Save data from file
bool CSector::Save(FILE* fp)
{
	fwrite(&m_vMins, 1, sizeof (m_vMins), fp);
	fwrite(&m_vMaxs, 1, sizeof (m_vMaxs), fp);
	return true;
}

//	Check sector is visible
bool CSector::IsVisible()
{
	//	Selected object always is visible
	if (m_bSelected)
		return true;

	if (!g_Configs.bShowSector)
		return false;

	if (!g_Configs.bForceRender && !m_bInBound)
		return false;

	return true;
}


