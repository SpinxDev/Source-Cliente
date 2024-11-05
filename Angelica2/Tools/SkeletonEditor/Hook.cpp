/*
 * FILE: Hook.cpp
 *
 * DESCRIPTION: Hook class
 *
 * CREATED BY: Duyuxin, 2003/10/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "Render.h"
#include "Hook.h"
#include "Box3D.h"
#include "Staff.h"

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
//	Implement CHook
//
///////////////////////////////////////////////////////////////////////////

CHook::CHook()
{
	m_pRender	= NULL;
	m_pA3DHook	= NULL;
	m_pBox		= NULL;
	m_aAxis[0]	= NULL;
	m_aAxis[1]	= NULL;
	m_aAxis[2]	= NULL;
	m_bVisible	= false;
}

//	Initialize box
bool CHook::Init(CRender* pRender, A3DSkeletonHook* pA3DHook)
{
	m_pRender	= pRender;
	m_pA3DHook	= pA3DHook;

	m_pBox = new CBox3D;
	ASSERT(m_pBox);

	m_pBox->Init(m_pRender, 1.0f);
	m_pBox->SetColor(g_aColTable[COL_HOOKBOX]);

	//	Axises
	for (int i=0; i < 3; i++)
	{
		CStaff* pStaff = new CStaff;
		ASSERT(pStaff);

		pStaff->Init(m_pRender, g_Configs.iStaffSegment, 1.0f, 1.0f);
		pStaff->SetColor(g_aColTable[COL_AXISX+i]);
		m_aAxis[i] = pStaff;
	}

	//	Update size
	SetBoxAndStaffSize(g_Configs.fBoneBoxSize, g_Configs.fStaffRadius);

	return true;
}

//	Release box
void CHook::Release()
{
	if (m_pBox)
	{
		m_pBox->Release();
		delete m_pBox;
		m_pBox = NULL;
	}

	for (int i=0; i < 3; i++)
	{
		m_aAxis[i]->Release();
		delete m_aAxis[i];
		m_aAxis[i] = NULL;
	}

	m_pRender	= NULL;
	m_pA3DHook	= NULL;
}

//	Frame move
bool CHook::FrameMove(const A3DMATRIX4& matBoneToWorld)
{
	A3DMATRIX4 mat = m_pA3DHook->GetScaledHookTM() * matBoneToWorld;

	A3DVECTOR3 vPos = mat.GetRow(3);
	A3DVECTOR3 vX = mat.GetRow(0);
	A3DVECTOR3 vY = mat.GetRow(1);
	A3DVECTOR3 vZ = mat.GetRow(2);

	m_pBox->SetPos(vPos);
	m_pBox->SetDirAndUp(vZ, vY);

	float fOffset = m_aAxis[0]->GetLength() * 0.5f;

	m_aAxis[0]->SetPos(vPos + vX * fOffset);
	m_aAxis[0]->SetDirAndUp(vX, vY);

	m_aAxis[1]->SetPos(vPos + vY * fOffset);
	m_aAxis[1]->SetDirAndUp(vY, vZ);

	m_aAxis[2]->SetPos(vPos + vZ * fOffset);
	m_aAxis[2]->SetDirAndUp(vZ, vY);

	return true;
}

//	Render routine
bool CHook::Render()
{
	if (!m_bVisible)
		return true;

	if (m_pBox)
		m_pBox->Render();

	for (int i=0; i < 3; i++)
	{
		if (m_aAxis[i])
			m_aAxis[i]->Render();
	}

	return true;
}

//	Set box size and staff radius
void CHook::SetBoxAndStaffSize(float fBoxSize, float fStaffRadius)
{
	m_pBox->SetSize(fBoxSize * 0.4f);

	//	Axises
	for (int i=0; i < 3; i++)
	{
		m_aAxis[i]->SetLength(fStaffRadius * 20.0f);
		m_aAxis[i]->SetRadius(fStaffRadius * 0.4f);
	}
}

//	Set transform matrix
void CHook::SetTM(const A3DMATRIX4& mat)
{
	m_pBox->SetTM(mat);
	m_aAxis[0]->SetTM(mat);
	m_aAxis[1]->SetTM(mat);
	m_aAxis[2]->SetTM(mat);
}


