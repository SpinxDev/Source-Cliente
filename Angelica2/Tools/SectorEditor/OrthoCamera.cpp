/*
 * FILE: OrthoCamera.cpp
 *
 * DESCRIPTION: Orthogonal camera class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3D.h"
#include "OrthoCamera.h"

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
//	Implement COrthoCamera
//
///////////////////////////////////////////////////////////////////////////

COrthoCamera::COrthoCamera()
{
	m_pA3DDevice	= NULL;
}

//	Initialize camera object
bool COrthoCamera::Init(A3DDevice* pDevice, const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	m_pA3DDevice	= pDevice;
	m_vMins			= vMins;
	m_vMaxs			= vMaxs;

	D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)&m_matProject, vMins.x, vMaxs.x, vMins.y, vMaxs.y, vMins.z, vMaxs.z);

	return true;
}

//	Release object
void COrthoCamera::Release()
{
	m_pA3DDevice	= NULL;
}

//	Set direction
void COrthoCamera::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	m_vDir	= Normalize(vDir);
	m_vUp	= Normalize(vUp);

	m_vRight.CrossProduct(m_vUp, m_vDir);
	m_vRight.Normalize();

	m_vUp.CrossProduct(m_vDir, m_vRight);
	m_vUp.Normalize();

	UpdateViewTM();
}

//	Move camera
bool COrthoCamera::MoveFront(float fDistance)
{
	m_vPos += m_vDir * fDistance;
	UpdateViewTM();
	return true;
}

bool COrthoCamera::MoveRight(float fDistance)
{
	m_vPos += m_vRight * fDistance;
	UpdateViewTM();
	return true;
}

bool COrthoCamera::MoveUp(float fDistance)
{
	m_vPos += m_vUp * fDistance;
	UpdateViewTM();
	return true;
}

bool COrthoCamera::Move(const A3DVECTOR3& vDelta)
{
	m_vPos += vDelta;
	UpdateViewTM();
	return true;
}

//	Set project range
void COrthoCamera::SetProjectRange(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs)
{
	m_vMins = vMins;
	m_vMaxs = vMaxs;
	UpdateProjectTM();
}

//	Update view matrix
bool COrthoCamera::UpdateViewTM()
{
	m_matView	= ViewMatrix(m_vPos, m_vDir, m_vUp, 0.0f);
	m_matVP		= m_matView * m_matProject;
	m_matInvVP	= InverseTM(m_matVP);
	return true;
}

//	Update project matrix
bool COrthoCamera::UpdateProjectTM()
{
	D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)&m_matProject, m_vMins.x, m_vMaxs.x, m_vMins.y, m_vMaxs.y, m_vMins.z, m_vMaxs.z);
	m_matVP		= m_matView * m_matProject;
	m_matInvVP	= InverseTM(m_matVP);
	return true;
}

//	Transform point from world to view
bool COrthoCamera::Transform(A3DVECTOR3& vIn, A3DVECTOR3& vOut)
{
	vOut = vIn * m_matVP;
	if (vOut.x < -1.0f || vOut.y < -1.0f ||	vOut.x > 1.0f || vOut.y > 1.0f)
		return true;
	else
		return false;
}

//	Trnasform point from view to world
bool COrthoCamera::InvTransform(A3DVECTOR3& vIn, A3DVECTOR3& vOut)
{
	vOut = vIn * m_matInvVP;
	return true;
}

bool COrthoCamera::Active()
{
	m_pA3DDevice->SetViewMatrix(m_matView);
	m_pA3DDevice->SetProjectionMatrix(m_matProject);
	return true;
}

