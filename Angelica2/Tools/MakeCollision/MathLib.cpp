/*
 * FILE: MathLib.cpp
 *
 * DESCRIPTION: Math routines often used in BSP tree operations
 *
 * CREATED BY: Duyuxin, 2001/8/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "MathLib.h"

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

//	Clear bound box
void ClearBound(FLOAT vMins[3], FLOAT vMaxs[3])
{
	vMins[0] = vMins[1] = vMins[2] = 999999;
	vMaxs[0] = vMaxs[1] = vMaxs[2] = -999999;
}

//	Add a point to bound box
void AddPointToBound(FLOAT vMins[3], FLOAT vMaxs[3], A3DVECTOR3& v)
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
bool BoundHasIntersection(FLOAT vMins1[3], FLOAT vMaxs1[3], FLOAT vMins2[3], FLOAT vMaxs2[3])
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
void MergeBound(FLOAT vMins1[3], FLOAT vMaxs1[3], FLOAT vMins2[3], FLOAT vMaxs2[3])
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
	fEpsilon: epsilon
*/
int PointOnPlaneSide(A3DVECTOR3& v, A3DVECTOR3& vNormal, float fDist, float fEpsilon)
{
	float fDelta = DotProduct(v, vNormal) - fDist;
	if (fDelta > fEpsilon)
		return SIDE_FRONT;
	else if (fDelta < -fEpsilon)
		return SIDE_BACK;
	else
		return SIDE_ON;
}



