/*
 * FILE: Box3D.cpp
 *
 * DESCRIPTION: 3D box class
 *
 * CREATED BY: Duyuxin, 2003/7/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "Render.h"
#include "WireCollector.h"
#include "Utility.h"
#include "MainFrm.h"
#include "Box3D.h"

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

//	Indices
WORD CBox3D::m_aIndices[24] =
{
	0, 1, 1, 2, 2, 3, 3, 0,
	4, 5, 5, 6, 6, 7, 7, 4,
	0, 4, 1, 5, 2, 6, 3, 7,
};

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CBox3D
//
///////////////////////////////////////////////////////////////////////////

CBox3D::CBox3D()
{
	m_pRender	= NULL;
	m_dwColor	= 0;
}

//	Initialize sector
bool CBox3D::Init(CRender* pRender, const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	m_pRender = pRender;

	SetMinsAndMaxs(vMins, vMaxs);

	return true;
}

//	Release sector
void CBox3D::Release()
{
	m_pRender = NULL;
}

//	Render sector
bool CBox3D::Render()
{
	ASSERT(m_pRender);

	A3DVECTOR3 aVerts[8];
	aVerts[0].Set(m_vMins.x, m_vMaxs.y, m_vMaxs.z);
	aVerts[1].Set(m_vMaxs.x, m_vMaxs.y, m_vMaxs.z);
	aVerts[2].Set(m_vMaxs.x, m_vMaxs.y, m_vMins.z);
	aVerts[3].Set(m_vMins.x, m_vMaxs.y, m_vMins.z);
	aVerts[4].Set(m_vMins.x, m_vMins.y, m_vMaxs.z);
	aVerts[5].Set(m_vMaxs.x, m_vMins.y, m_vMaxs.z);
	aVerts[6].Set(m_vMaxs.x, m_vMins.y, m_vMins.z);
	aVerts[7].Set(m_vMins.x, m_vMins.y, m_vMins.z);

	//	Push to render buffer
	m_pRender->GetWireCollector()->AddRenderData(aVerts, 8, m_aIndices, 24, m_dwColor);

	return true;
}

void CBox3D::SetMinsAndMaxs(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	m_vMins = vMins;
	m_vMaxs = vMaxs;
}



