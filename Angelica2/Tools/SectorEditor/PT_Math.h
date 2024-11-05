/*
 * FILE: PT_Math.h
 *
 * DESCRIPTION: Math routines often used in building portal
 *
 * CREATED BY: Duyuxin, 2003/6/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_PT_MATH_H_
#define _PT_MATH_H_

#include "A3DTypes.h"
#include "A3DFuncs.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define EPSILON_ONPLANE	0.01f
#define EPSILON_CLIP	0.01f
#define EPSILON_SPLIT	0.001f

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
	void ClearBound(float vMins[3], float vMaxs[3]);
	void AddPointToBound(float vMins[3], float vMaxs[3], A3DVECTOR3& v);
	bool BoundHasIntersection(float vMins1[3], float vMaxs1[3], float vMins2[3], float vMaxs2[3]);
	void MergeBound(float vMins1[3], float vMaxs1[3], float vMins2[3], float vMaxs2[3]);
	int PointOnPlaneSide(A3DVECTOR3& v, A3DVECTOR3& vNormal, float fDist, FLOAT fEpsilon);
}

/////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
/////////////////////////////////////////////////////////////////////////

namespace PORTAL
{
	/*	Calculate distance from a point to plane and return it.
	
		v: specified point's position.
		vNormal: normal of plane
		fDist: parameter d of plane
	*/
	inline float PointToPlane(A3DVECTOR3& v, A3DVECTOR3& vNormal, float fDist)
	{
		return DotProduct(v, vNormal) - fDist;
	}
}

#endif	//	_PLANE_H_


