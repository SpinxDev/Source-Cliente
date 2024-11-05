/*
 * FILE: PT_Plane.h
 *
 * DESCRIPTION: Operations for planes
 *
 * CREATED BY: Duyuxin, 2003/6/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_PT_PLANE_H_
#define _PT_PLANE_H_

#include "PT_Internal.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define PT_PLANE_X			0
#define PT_PLANE_Y			1
#define PT_PLANE_Z			2
#define PT_PLANE_NONAXIAL	3

/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////

namespace PORTAL
{
	int MakePlaneFromPoints(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3);
	void MakePlaneFromPoints(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3, s_PTPLANE* pPlane);
	int	CreatePlane(A3DVECTOR3& vNormal, float fDist);
	int PlaneType(A3DVECTOR3& vNormal);
	int SearchPlane(A3DVECTOR3& vNormal, float fDist);
	BOOL ComparePlane(s_PTPLANE* p1, s_PTPLANE* p2);
	BOOL ComparePlane(s_PTPLANE* p1, A3DVECTOR3 vNormal, float fDist);

	void SnapVector(A3DVECTOR3& vNormal);
	void SnapPlane(s_PTPLANE* p);
}

#endif	//	_PT_PLANE_H_


