/*
 * FILE: Bound.cpp
 *
 * DESCRIPTION: Bound box class
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
#include "Bound.h"
#include "Utility.h"

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
//	Implement CBound
//
///////////////////////////////////////////////////////////////////////////

CBound::CBound()
{
}

//	Initialize sector
bool CBound::Init(CRender* pRender, const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	if (!m_Box3D.Init(pRender, vMins, vMaxs))
		return false;

	SetMinsAndMaxs(vMins, vMaxs);

	m_Box3D.SetColor(g_aColTable[COL_BOUNDBOX]);

	return true;
}

//	Release sector
void CBound::Release()
{
	m_Box3D.Release();
}

//	Render sector
bool CBound::Render()
{
	m_Box3D.Render();
	return true;
}

void CBound::SetBound(int iAxis, bool bMin, float fVal)
{
	A3DVECTOR3 vMins = m_Box3D.GetMins();
	A3DVECTOR3 vMaxs = m_Box3D.GetMaxs();

	if (bMin)
	{
		if (fVal >= vMaxs.m[iAxis])
			return;

		vMins.m[iAxis] = fVal;
	}
	else
	{
		if (fVal <= vMins.m[iAxis])
			return;

		vMaxs.m[iAxis] = fVal;
	}

	m_Box3D.SetMinsAndMaxs(vMins, vMaxs);
}

