/*
 * FILE: MathLib.h
 *
 * DESCRIPTION: Math routines often used in BSP tree operations
 *
 * CREATED BY: Duyuxin, 2001/8/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_BSPMATH_H_
#define _BSPMATH_H_

#include "A3DTypes.h"
#include "A3DFuncs.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define EPSILON_ONPLANE	0.001f
#define EPSILON_CLIP	0.001f
#define EPSILON_SPLIT	0.001f

//	Plane type
#define PLANE_X			0
#define PLANE_Y			1
#define PLANE_Z			2
#define PLANE_NONAXIAL	3

//	Relative side
#define SIDE_FRONT		0
#define SIDE_BACK		1
#define SIDE_ON			2
#define SIDE_CROSS		3

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

void ClearBound(FLOAT vMins[3], FLOAT vMaxs[3]);
void AddPointToBound(FLOAT vMins[3], FLOAT vMaxs[3], A3DVECTOR3& v);
bool BoundHasIntersection(FLOAT vMins1[3], FLOAT vMaxs1[3], FLOAT vMins2[3], FLOAT vMaxs2[3]);
void MergeBound(FLOAT vMins1[3], FLOAT vMaxs1[3], FLOAT vMins2[3], FLOAT vMaxs2[3]);
int PointOnPlaneSide(A3DVECTOR3& v, A3DVECTOR3& vNormal, float fDist, float fEpsilon);

/////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
/////////////////////////////////////////////////////////////////////////

/*	Calculate distance from a point to plane and return it.

	v: specified point's position.
	vNormal: normal of plane
	fDist: parameter d of plane
*/
inline FLOAT PointToPlane(A3DVECTOR3& v, A3DVECTOR3& vNormal, FLOAT fDist)
{
	return DotProduct(v, vNormal) - fDist;
}


#endif	//	_PLANE_H_


