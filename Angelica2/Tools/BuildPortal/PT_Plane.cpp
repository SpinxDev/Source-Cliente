/*
 * FILE: PT_Plane.cpp
 *
 * DESCRIPTION: Operations for planes
 *
 * CREATED BY: Duyuxin, 2003/6/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "PT_Internal.h"
#include "PT_Plane.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define EPSILON_NORMAL		1e-7f		//	Epsilon of normal
#define EPSILON_DIST		0.0001f		//	Epsilon of distance

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

/*	Create a plane form threee clockwise points.

	v1, v2, v3: clockwise points on plane.
	pPlane (out): used to receive plane's data.
*/
void MakePlaneFromPoints(A3DVECTOR3& v1, A3DVECTOR3& v2, A3DVECTOR3& v3, s_PTPLANE* pPlane)
{
	A3DVECTOR3 d1 = v2 - v1;
	A3DVECTOR3 d2 = v3 - v1;

	pPlane->vNormal.CrossProduct(d1, d2);
	pPlane->vNormal.Normalize();
	pPlane->fDist = DotProduct(pPlane->vNormal, v1);

	SnapPlane(pPlane);

	pPlane->iType = PlaneType(pPlane->vNormal);
}

/*	Create a plane from three clockwise points.

  	Return plane's index in g_aPlanes for success, otherwise return -1.
*/
int MakePlaneFromPoints(A3DVECTOR3& v1, A3DVECTOR3& v2, A3DVECTOR3& v3)
{
	A3DVECTOR3 d1 = v2 - v1;
	A3DVECTOR3 d2 = v3 - v1;

	A3DVECTOR3 vNormal;
	float fDist;

	vNormal.CrossProduct(d1, d2);
	vNormal.Normalize();
	fDist = DotProduct(vNormal, v1);

	return SearchPlane(vNormal, fDist);
}

/*	Create a new plane.

	Return plane's index in g_aPlanes for success, otherwise return -1.

	vNormal: plane's normal, should be normalized.
	fDist: parameter d of plane.
*/
int	CreatePlane(A3DVECTOR3& vNormal, float fDist)
{
	if (vNormal.Magnitude() < 0.5f)
	{
		g_Statistics.iNumBadPlanes++;
		return -1;
	}

	s_PTPLANE Plane;
	Plane.vNormal	= vNormal;
	Plane.fDist		= fDist;
	Plane.iType		= PlaneType(Plane.vNormal);

	return g_aPlanes.Add(Plane);
}

//	Return plane type
int PlaneType(A3DVECTOR3& vNormal)
{
	if (vNormal.x == 1.0f)
		return PT_PLANE_X;
	else if (vNormal.y == 1.0f)
		return PT_PLANE_Y;
	else if (vNormal.z == 1.0f)
		return PT_PLANE_Z;
	else
		return PT_PLANE_NONAXIAL;
}

/*	Search a plane who has the specified normal and dist, if such a plane don't exist,
	then create a new one.

	Return plane's index for success, otherwise return -1.

	vNormal: normal of plane, needn't be normalized
	fDist: parameter d of plane

	TODO: use hash algorithm.
*/
int SearchPlane(A3DVECTOR3& vNormal, float fDist)
{
	s_PTPLANE Plane;
	Plane.vNormal = vNormal;
	Plane.fDist	  = fDist;	
	Plane.vNormal.Normalize();

	//	SnapPlane will cause error in collision detection
	SnapPlane(&Plane);

	for (int i=0; i < g_aPlanes.GetSize(); i++)
		if (ComparePlane(&Plane, &g_aPlanes[i]))
			return i;

	return CreatePlane(Plane.vNormal, Plane.fDist);
}

/*  Compare tow plane, 

	If they are equal return TRUE, otherwise return FALSE.
*/
BOOL ComparePlane(s_PTPLANE* p1, s_PTPLANE* p2)
{
	if (fabs(p1->vNormal.x - p2->vNormal.x) < 1e-5f &&
		fabs(p1->vNormal.y - p2->vNormal.y) < 1e-5f &&
		fabs(p1->vNormal.z - p2->vNormal.z) < 1e-5f &&
		fabs(p1->fDist - p2->fDist) < EPSILON_DIST)
		return TRUE;

	return FALSE;
}

/*  Compare tow plane, 

	If they are equal return TRUE, otherwise return FALSE.
*/
BOOL ComparePlane(s_PTPLANE* p1, A3DVECTOR3 vNormal, float fDist)
{
	if (fabs(p1->vNormal.x - vNormal.x) < 1e-5f &&
		fabs(p1->vNormal.y - vNormal.y) < 1e-5f &&
		fabs(p1->vNormal.z - vNormal.z) < 1e-5f &&
		fabs(p1->fDist - fDist) < EPSILON_DIST)
		return TRUE;

	return FALSE;
}

/*	Snap a normal vector.

	vNormal: a normalized vector.
*/
void SnapVector(A3DVECTOR3& vNormal)
{
	for (int i=0; i < 3; i++)
	{
		if (1.0f - vNormal.m[i] < EPSILON_NORMAL)
		{
			vNormal = A3DVECTOR3(0, 0, 0);
			vNormal.m[i] = 1.0f;
			break;
		}
		else if (vNormal.m[i] - (-1.0f) < EPSILON_NORMAL)
		{
			vNormal = A3DVECTOR3(0, 0, 0);
			vNormal.m[i] = -1.0f;
			break;
		}
	}
}

//	Snap a plane
void SnapPlane(s_PTPLANE* p)
{
	SnapVector(p->vNormal);

	if (fabs(p->fDist - floor(p->fDist + 0.5f)) < EPSILON_DIST)
		p->fDist = (float)floor(p->fDist + 0.5f);
}

}	//	namespace PORTAL

