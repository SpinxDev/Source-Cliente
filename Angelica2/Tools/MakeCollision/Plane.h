/*
 * FILE: Plane.h
 *
 * DESCRIPTION: Operations for planes
 *
 * CREATED BY: Duyuxin, 2001/9/28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_PLANE_H_
#define _PLANE_H_

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define PLANE_X			0
#define PLANE_Y			1
#define PLANE_Z			2
#define PLANE_NONAXIAL	3

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

int MakePlaneFromPoints(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3);
void MakePlaneFromPoints(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3, s_PLANE* pPlane);
int	CreatePlane(A3DVECTOR3& vNormal, float fDist);
int PlaneType(A3DVECTOR3& vNormal);
int SearchPlane(A3DVECTOR3& vNormal, float fDist);
BOOL ComparePlane(s_PLANE* p1, s_PLANE* p2);
BOOL ComparePlane(s_PLANE* p1, A3DVECTOR3 vNormal, float fDist);

void SnapVector(A3DVECTOR3& vNormal);
void SnapPlane(s_PLANE* p);

#endif	//	_PLANE_H_


