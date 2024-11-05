/*
 * FILE: OrthoViewport.cpp
 *
 * DESCRIPTION: Orthogonal viewport class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3D.h"
#include "OrthoViewport.h"
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
//	Implement COrthoViewport
//
///////////////////////////////////////////////////////////////////////////

COrthoViewport::COrthoViewport()
{
	m_pA3DDevice	= NULL;
	m_pCamera		= NULL;
	m_bClearColor	= false;
	m_bClearZ		= false;
	m_colClear		= A3DCOLORRGB(0, 0, 0);
}

bool COrthoViewport::Init(A3DDevice* pA3DDevice, A3DVIEWPORTPARAM* pParam, bool bClearColor, bool bClearZ, A3DCOLOR colClear)
{
	m_pA3DDevice	= pA3DDevice;
	m_pCamera		= NULL;
	m_bClearColor	= bClearColor;
	m_bClearZ		= bClearZ;
	m_colClear		= colClear;

	return SetParam(pParam);
}

bool COrthoViewport::SetParam(A3DVIEWPORTPARAM* pParam)
{
	m_ViewportParam = *pParam;

	//	Construct the view scale transform matrix;
	m_matViewScale.Identity();
	m_matViewScale._11 = 1.0f * m_ViewportParam.Width / 2;
	m_matViewScale._22 = -1.0f * m_ViewportParam.Height / 2;
	m_matViewScale._33 = m_ViewportParam.MaxZ - m_ViewportParam.MinZ;
	m_matViewScale._41 = 1.0f * m_ViewportParam.X + m_ViewportParam.Width / 2;
	m_matViewScale._42 = 1.0f * m_ViewportParam.Y + m_ViewportParam.Height / 2;
	m_matViewScale._43 = m_ViewportParam.MinZ;
	
	m_matInvViewScale = InverseTM(m_matViewScale);
//	m_matInvViewScale = m_matViewScale.GetInverse();

	return true;
}

bool COrthoViewport::Release()
{
	return true;
}

bool COrthoViewport::Active()
{
	if (m_pCamera)
		m_pCamera->Active();

	m_pA3DDevice->SetViewport(&m_ViewportParam);

	return true;
}

bool COrthoViewport::SetCamera(COrthoCamera* pCamera)
{
	m_pCamera = pCamera;
	return true;
}

bool COrthoViewport::ClearDevice()
{
	DWORD dwFlags = 0;
	
	if (m_bClearColor)
		dwFlags |= D3DCLEAR_TARGET;
	
	if (m_bClearZ)
		dwFlags |= D3DCLEAR_ZBUFFER;
	
	if (!dwFlags)
		return true;

	if (!m_pA3DDevice->Clear(dwFlags, m_colClear, 1.0f, 0))
		return false;

	return true;
}

/*	Transform a 3D point from world to screen coordinates;
	return true if that point should be clipped;
	else return false;

	the vOut is x, y, z, w, and the real x', y', z' should be x/w, y/w, z/w;
*/
bool COrthoViewport::Transform(A3DVECTOR3& vIn, A3DVECTOR3& vOut)
{
	bool bClipped;
	A3DVECTOR3 vView;
								 
	bClipped = m_pCamera->Transform(vIn, vView);

	if (vView.z < m_ViewportParam.MinZ || vView.z > m_ViewportParam.MaxZ)
		bClipped = true;

	vOut = vView * m_matViewScale;

	return bClipped;
}

//	Transform a screen point to world space coordinates;
bool COrthoViewport::InvTransform(A3DVECTOR3& vIn, A3DVECTOR3& vOut)
{
	A3DVECTOR3 vView = vIn * m_matInvViewScale;
	m_pCamera->InvTransform(vView, vOut);
	return true;
}
