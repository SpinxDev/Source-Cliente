/*
 * FILE: PT_Math.cpp
 *
 * DESCRIPTION: Math routines often used in building portal
 *
 * CREATED BY: Duyuxin, 2003/6/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "PT_Internal.h"
#include "PT_Math.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Extern variables and functions
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Declaration of local functions
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

namespace PORTAL
{

//	Clear bound box
void ClearBound(float vMins[3], float vMaxs[3])
{
	vMins[0] = vMins[1] = vMins[2] = 999999;
	vMaxs[0] = vMaxs[1] = vMaxs[2] = -999999;
}

//	Add a point to bound box
void AddPointToBound(float vMins[3], float vMaxs[3], A3DVECTOR3& v)
{
	for (int i=0; i < 3; i++)
	{
		if (v.m[i] < vMins[i])
			vMins[i] = v.m[i];
		
		if (v.m[i] > vMaxs[i])
			vMaxs[i] = v.m[i];
	}
}

/*	Check whether two bounding box intersect.

	Return TRUE if they intersect, otherwise return FALSE.

	vMins1, vMaxs1: bounding box 1
	vMins2, vMaxs2: bounding box 2
*/
bool BoundHasIntersection(float vMins1[3], float vMaxs1[3], float vMins2[3], float vMaxs2[3])
{
	int i;
	float fDist;

	for (i=0; i < 3; i++)
	{
		fDist = ((vMins1[i] + vMaxs1[i]) - (vMins2[i] + vMaxs2[i])) * 0.5f;
		if (fDist < 0)
			fDist = -fDist;

		if (((vMaxs1[i] - vMins1[i]) + (vMaxs2[i] - vMins2[i])) * 0.5f < fDist)
			return false;
	}

	return true;
}

/*	Merge too bounding box.

	vMins1, vMaxs1 (in): bounding box 1
				   (out): merge result
	vMins2, vMaxs2: bounding box 2
*/
void MergeBound(float vMins1[3], float vMaxs1[3], float vMins2[3], float vMaxs2[3])
{
	for (int i=0; i < 3; i++)
	{
		if (vMins2[i] < vMins1[i])
			vMins1[i] = vMins2[i];

		if (vMaxs2[i] > vMaxs1[i])
			vMaxs1[i] = vMaxs2[i];
	}
}

/*	Judge specified point's relative position to a plane.

	Return relative position, front, back, on or cross.

	v: specified point's position.
	vNormal: normal of plane
	fDist: parameter d of plane
*/
int PointOnPlaneSide(A3DVECTOR3& v, A3DVECTOR3& vNormal, float fDist)
{
	float fDelta = DotProduct(v, vNormal) - fDist;
	if (fDelta > EPSILON_ONPLANE)
		return SIDE_FRONT;
	else if (fDelta < -EPSILON_ONPLANE)
		return SIDE_BACK;
	else
		return SIDE_ON;
}

}	//	namespace PORTAL


