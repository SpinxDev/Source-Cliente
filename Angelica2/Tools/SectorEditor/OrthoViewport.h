/*
 * FILE: OrthoViewport.h
 *
 * DESCRIPTION: Orthogonal viewport class
 *
 * CREATED BY: Duyuxin, 2003/7/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_ORTHOVIEWPORT_H_
#define _ORTHOVIEWPORT_H_

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
class COrthoCamera;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class COrthoViewport
//
///////////////////////////////////////////////////////////////////////////

class COrthoViewport
{
public:		//	Types

public:		//	Constructor and Destructor

	COrthoViewport();
	virtual ~COrthoViewport() {}

public:		//	Attributes

public:		//	Operations

	bool Init(A3DDevice* pA3DDevice, A3DVIEWPORTPARAM* pParam, bool bClearColor, bool bClearZ, A3DCOLOR colClear);
	bool Release();

	//	Set Viewport's parameters;
	bool SetParam(A3DVIEWPORTPARAM* pParam);

	//	Transform a world space point to screen coordinates, vecOut is a screen point;
	bool Transform(A3DVECTOR3& vIn, A3DVECTOR3& vOut);
	//	Transform a screen point to world space coordinates, vecIn is a screen point;
	bool InvTransform(A3DVECTOR3& vIn, A3DVECTOR3& vOut);

	bool SetCamera(COrthoCamera* pCamera);
	bool Active();
	bool ClearDevice();

	inline A3DVIEWPORTPARAM* GetParam() { return &m_ViewportParam; }
	inline COrthoCamera* GetCamera() { return m_pCamera; }
	inline void SetClearColor(A3DCOLOR color) { m_colClear = color; }
	inline A3DCOLOR GetClearColor() { return m_colClear; }

protected:	//	Attributes

	A3DVIEWPORTPARAM	m_ViewportParam;

	bool			m_bClearColor;
	bool			m_bClearZ;
	A3DCOLOR		m_colClear;
	A3DDevice*		m_pA3DDevice;
	COrthoCamera*	m_pCamera;

	A3DMATRIX4		m_matViewScale;
	A3DMATRIX4		m_matInvViewScale;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_ORTHOVIEWPORT_H_
