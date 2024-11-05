/*
 * FILE: OrthoCamera.h
 *
 * DESCRIPTION: Orthogonal camera class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_ORTHOCAMERA_H_
#define _ORTHOCAMERA_H_

#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DDevice;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COrthoCamera
//
///////////////////////////////////////////////////////////////////////////

class COrthoCamera
{
public:		//	Types

public:		//	Constructor and Destructor

	COrthoCamera();
	virtual ~COrthoCamera() {}

public:		//	Attributes

	//	Initialize camera object
	bool Init(A3DDevice* pDevice, const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);
	//	Release object
	void Release();

	//	Set/Get position
	void SetPos(const A3DVECTOR3& vPos)
	{
		m_vPos = vPos;
		UpdateViewTM();
	}

	A3DVECTOR3 GetPos() { return m_vPos; }

	//	Set direction
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	//	Set project range
	void SetProjectRange(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);
	//	Transform point from world to view
	bool Transform(A3DVECTOR3& vIn, A3DVECTOR3& vOut);
	//	Trnasform point from view to world
	bool InvTransform(A3DVECTOR3& vIn, A3DVECTOR3& vOut);
	//	Active camera
	bool Active();

	//	Move camera
	bool MoveFront(float fDistance);
	bool MoveRight(float fDistance);
	bool MoveUp(float fDistance);
	bool Move(const A3DVECTOR3& vDelta);

	//	Get project range
	A3DVECTOR3 GetProjectRangeMins() { return m_vMins; }
	A3DVECTOR3 GetProjectRangeMaxs() { return m_vMaxs; }

public:		//	Operations

protected:	//	Attributes

	A3DDevice*	m_pA3DDevice;	//	A3D device

	A3DMATRIX4	m_matView;		//	World to camera matrix
	A3DMATRIX4	m_matProject;	//	Project matrix
	A3DMATRIX4	m_matVP;		//	m_matView * m_matProject
	A3DMATRIX4	m_matInvVP;		//	Inverse m_matVP

	A3DVECTOR3	m_vPos;
	A3DVECTOR3	m_vDir;
	A3DVECTOR3	m_vUp;
	A3DVECTOR3	m_vRight;

	A3DVECTOR3	m_vMins;		//	Project area
	A3DVECTOR3	m_vMaxs;

protected:	//	Operations

	//	Update view and project matrix
	bool UpdateViewTM();
	bool UpdateProjectTM();
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_ORTHOCAMERA_H_
