/*
 * FILE: Winding.cpp
 *
 * DESCRIPTION: Operations for polygons
 *
 * CREATED BY: Duyuxin, 2001/8/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "Winding.h"
#include "MathLib.h"
#include "AMemory.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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

/*	Create a winding.

	Return new winding's address for success, otherwise return NULL.

	iNumPts: Number of vertices of the winding, shouldn't < 0.
*/
s_WINDING* AllocWinding(int iNumPts)
{
	s_WINDING* pWinding = new s_WINDING;
	
	if (!pWinding)
		return NULL;

	if (iNumPts)
	{
		pWinding->pVerts = new A3DVECTOR3[iNumPts];
		if (!pWinding->pVerts)
			return NULL;
	}
	else
		pWinding->pVerts = NULL;

	pWinding->iNumPts = iNumPts;
	return pWinding;
}

//	Release a winding
void DestroyWinding(s_WINDING* pWinding)
{
	if (pWinding)
	{
		if (pWinding->pVerts)
			delete pWinding->pVerts;

		delete pWinding;
	}
}

/*	Create a new winding and duplicate data from specified winding for it.

	Return new winding's address for success, otherwise return NULL.

	pWinding: source winding.
*/
s_WINDING* CopyWinding(s_WINDING* pWinding)
{
	if (!pWinding)
		return NULL;

	s_WINDING* pDest = AllocWinding(pWinding->iNumPts);
	if (!pDest)
		return NULL;

	for (int i=0; i < pWinding->iNumPts; i++)
		pDest->pVerts[i] = pWinding->pVerts[i];

	return pDest;
}

/*	Create a new winding and set it's data as specified winding's reversed vertices.

	Return new winding's address for success, otherwise return NULL.

	pWinding: source winding.
*/
s_WINDING* ReverseWinding(s_WINDING* pWinding)
{
	if (!pWinding)
		return NULL;

	s_WINDING* pDest = AllocWinding(pWinding->iNumPts);
	if (!pDest)
		return NULL;

	for (int i=0; i < pWinding->iNumPts; i++)
		pDest->pVerts[i] = pWinding->pVerts[pWinding->iNumPts - 1 - i];

	return pDest;
}

/*	Judge specified winding's relative position to a plane.

	Return relative position, front, back, on or cross.

	pWinding: specified winding
	vNormal: plane's normal.
	fDist: parameter d of plane.
	fEpsilon: epsilon
*/
int WindingOnPlaneSide(s_WINDING* pWinding, A3DVECTOR3& vNormal, FLOAT fDist, FLOAT fEpsilon)
{
	bool bFront = false, bBack = false;
	int iSide;

	for (int i=0; i < pWinding->iNumPts; i++)
	{
		if ((iSide = PointOnPlaneSide(pWinding->pVerts[i], vNormal, fDist, fEpsilon)) == SIDE_FRONT)
		{
			if (bBack)
				return SIDE_CROSS;

			bFront = true;
		}
		else if (iSide == SIDE_BACK)
		{
			if (bFront)
				return SIDE_CROSS;

			bBack = true;
		}
	}

	if (bFront)
		return SIDE_FRONT;
	else if (bBack)
		return SIDE_BACK;
	else
		return SIDE_ON;
}

/*	Clip a winding with a plane.

	pWinding: specified winding
	vNormal: plane's normal.
	fDist: parameter d of plane.
	fEpsilon: cliping epsilon
	ppFront (out): receive the pointer of clipped front winding.
	ppBack (out): receive the pointer of clipped back winding.
*/
void ClipWindingWithPlane(s_WINDING* pWinding, A3DVECTOR3& vNormal, FLOAT fDist,
						  float fEpsilon, s_WINDING** ppFront, s_WINDING** ppBack)
{
	int* aSides = new int [pWinding->iNumPts];
	float* aDeltas = new float [pWinding->iNumPts];
	A3DVECTOR3* aVerts = pWinding->pVerts;
	s_WINDING* pFront, *pBack;
	A3DVECTOR3 vDivide;		//	Divide point

	float fDelta;
	int iNumFront=0, iNumBack=0, iNumOn=0, j;

	for (int i=0; i < pWinding->iNumPts; i++)
	{
		fDelta = DotProduct(aVerts[i], vNormal) - fDist;
		if (fDelta < -fEpsilon)
		{
			aSides[i] = SIDE_BACK;
			iNumBack++;
		}
		else if (fDelta > fEpsilon)
		{
			aSides[i] = SIDE_FRONT;
			iNumFront++;
		}
		else
		{
			aSides[i] = SIDE_ON;
			iNumOn++;
		}

		aDeltas[i] = fDelta;
	}

	if (!iNumBack)
	{
		*ppFront = CopyWinding(pWinding);
		*ppBack	 = NULL;
		goto End;
	}
	
	if (!iNumFront)
	{
		*ppBack  = CopyWinding(pWinding);
		*ppFront = NULL;
		goto End;
	}

	pFront = AllocWinding(iNumFront + iNumOn + 4);
	pBack  = AllocWinding(iNumBack + iNumOn + 4);

	iNumFront = iNumBack = 0;

	for (i=0; i < pWinding->iNumPts; i++)
	{
		j = (i+1) % pWinding->iNumPts;

		if (aSides[i] == SIDE_ON)
		{
			//	Copy this vertex to both side
			pFront->pVerts[iNumFront++] = aVerts[i];
			pBack->pVerts[iNumBack++]	= aVerts[i];
			continue;
		}
		else if (aSides[i] == SIDE_FRONT)
		//	Copy this vertex to front side
			pFront->pVerts[iNumFront++] = aVerts[i];
		else	//	Copy this vertex to back side
			pBack->pVerts[iNumBack++] = aVerts[i];

		if (aSides[j] == SIDE_ON || aSides[j] == aSides[i])
			continue;
	
		//	Next vertex isn't on the same side as current vertex
		//	Generate a divide point
		fDelta  = aDeltas[i] / (aDeltas[i] - aDeltas[j]);
		vDivide = aVerts[i] + fDelta * (aVerts[j] - aVerts[i]);

		//	Add the new point to both sides
		pFront->pVerts[iNumFront++] = vDivide;
		pBack->pVerts[iNumBack++]	= vDivide;
	}

	pFront->iNumPts = iNumFront;
	pBack->iNumPts	= iNumBack;

	*ppFront = pFront;
	*ppBack  = pBack;

End:

	delete aSides;
	delete aDeltas;
}

/*	Cut a winding with specified plane, all vertice behide this plane will be discarded.

	Return a the winding which is in front of specified plane if it exists, otherwise 
	return NULL.

	pWinding: specified winding
	vNormal: plane's normal.
	fDist: parameter d of plane.
	fEpsilon: cliping epsilon
*/
s_WINDING* CutBackWindingWithPlane(s_WINDING* pWinding, A3DVECTOR3& vNormal, 
								 FLOAT fDist, float fEpsilon)
{
	int* aSides = new int [pWinding->iNumPts];
	float* aDeltas = new float [pWinding->iNumPts];
	A3DVECTOR3* aVerts = pWinding->pVerts;
	s_WINDING* pFront;
	A3DVECTOR3 vDivide;		//	Divide point

	float fDelta;
	int iNumFront=0, iNumBack=0, iNumOn=0, j;

	for (int i=0; i < pWinding->iNumPts; i++)
	{
		fDelta = DotProduct(aVerts[i], vNormal) - fDist;
		if (fDelta < -fEpsilon)
		{
			aSides[i] = SIDE_BACK;
			iNumBack++;
		}
		else if (fDelta > fEpsilon)
		{
			aSides[i] = SIDE_FRONT;
			iNumFront++;
		}
		else
		{
			aSides[i] = SIDE_ON;
			iNumOn++;
		}

		aDeltas[i] = fDelta;
	}

	if (!iNumBack)	//	All vertices are in front of plane
	{
		pFront = CopyWinding(pWinding);
		goto End;
	}
	
	if (!iNumFront)
	{
		pFront = NULL;
		goto End;
	}

	pFront = AllocWinding(iNumFront + iNumOn + 4);
	iNumFront = 0;

	for (i=0; i < pWinding->iNumPts; i++)
	{
		j = (i+1) % pWinding->iNumPts;

		if (aSides[i] == SIDE_ON)
		{
			//	Copy this vertex to both side
			pFront->pVerts[iNumFront++] = aVerts[i];
			continue;
		}
		else if (aSides[i] == SIDE_FRONT)
		//	Copy this vertex to front side
			pFront->pVerts[iNumFront++] = aVerts[i];

		if (aSides[j] == SIDE_ON || aSides[j] == aSides[i])
			continue;
	
		//	Next vertex isn't on the same side as current vertex
		//	Generate a divide point
		fDelta  = aDeltas[i] / (aDeltas[i] - aDeltas[j]);
		vDivide = aVerts[i] + fDelta * (aVerts[j] - aVerts[i]);

		//	Add the new point to front sides
		pFront->pVerts[iNumFront++] = vDivide;
	}

	pFront->iNumPts = iNumFront;

End:

	delete aSides;
	delete aDeltas;

	return pFront;
}

s_WINDING* CutFrontWindingWithPlane(s_WINDING* pWinding, A3DVECTOR3& vNormal, 
								 FLOAT fDist, float fEpsilon)
{
	//	Inverse plane
	A3DVECTOR3 n = A3DVECTOR3(0, 0, 0) - vNormal;
	FLOAT d = -fDist;

	return CutBackWindingWithPlane(pWinding, n, d, fEpsilon);
}

/*	Calculate a plane's base winding which is a large enough winding so that it
	can represent the whole plane.

	Return base winding's address for success, otherwise return NULL.

	vNormal: plane's normal.
	fDist: parameter d of plane.
*/
s_WINDING* BaseWindingForPlane(A3DVECTOR3& vNormal, FLOAT fDist)
{
	//	Search major axis of normal
	int iMajor = 0;
	float fMax = (float)fabs(vNormal.x);

	if (fMax < (float)fabs(vNormal.y))
	{
		iMajor = 1;
		fMax   = (float)fabs(vNormal.y);
	}

	if (fMax < (float)fabs(vNormal.z))
	{
		iMajor = 2;
		fMax   = (float)fabs(vNormal.z);
	}

	//	Decide the fake up axis
	A3DVECTOR3 vUp;

	switch (iMajor)
	{
	case 0:
	case 2:
		//	Select y as fake up axis
		vUp = A3DVECTOR3(0.0f, 1.0f, 0.0f);
		break;

	case 1:
		//	Select x as fake up axis
		vUp = A3DVECTOR3(1.0f, 0.0f, 0.0f);
		break;
	}

	//	Build up, right axis of specified plane
	float v = DotProduct(vUp, vNormal);
	vUp = Normalize(vUp - v * vNormal);

	A3DVECTOR3 vOrigin = vNormal * fDist;
	
	//	Get right axis through up and normal
	A3DVECTOR3 vRight = CrossProduct(vUp, vNormal);

	vUp		= vUp * MAXSIZE_WORLD;
	vRight	= vRight * MAXSIZE_WORLD;

	//	Project a big enough box to this plane
	s_WINDING* pWinding = AllocWinding(4);

	pWinding->pVerts[0] = vOrigin + vUp + vRight;
	pWinding->pVerts[1] = vOrigin + vUp - vRight;
	pWinding->pVerts[2] = vOrigin - vUp - vRight;
	pWinding->pVerts[3] = vOrigin - vUp + vRight;

	return pWinding;
}

//	Judge whether specified winding is too small or not accroding to it's edge length
bool WindingIsTiny(s_WINDING* pWinding)
{
	int i, j, iEdge;
	A3DVECTOR3 vEdge;

	for (i=0, iEdge=0; i < pWinding->iNumPts; i++)
	{
		j = (i == pWinding->iNumPts-1) ? 0 : i+1;
		vEdge = pWinding->pVerts[j] - pWinding->pVerts[i];

		if (Magnitude(vEdge) > 0.03f)
		{
			if (++iEdge >= 3)
				return false;
		}
	}

	return true;
}

//	Calculate winding's area and return it
float WindingArea(s_WINDING* pWinding)
{
	float fArea = 0.0f;
	A3DVECTOR3 v1, v2, d;

	//	Separate winding to several triangles and sum their area
	for (int i=2; i < pWinding->iNumPts; i++)
	{
		v1 = pWinding->pVerts[i-1] - pWinding->pVerts[0];
		v2 = pWinding->pVerts[i] - pWinding->pVerts[0];
		d  = CrossProduct(v1, v2);

		fArea += 0.5f * Magnitude(d);
	}

	return fArea;
}

/*	Add a winding's vertices to another convex winding and the result winding is
	also a convex winding.

	pWinding: winding will be added.
	ppHull: hull winding pWinding will be added to.
	vNormal: plane's normal.

	Note: pWinding and pHull must be on the same plane.
*/
void AddWindingToConvexHull(s_WINDING* pWinding, s_WINDING** ppHull, A3DVECTOR3 vNormal)
{
	A3DVECTOR3 aHull[MAXNUM_WINDINGVERT], aNewHull[MAXNUM_WINDINGVERT];
	A3DVECTOR3 aSideDirs[MAXNUM_WINDINGVERT], v, vDir;
	int aOutside[MAXNUM_WINDINGVERT];
	int iNumVerts, iNumNewVerts, i, j, k;
	bool bOutside;
	float d;

	if (!(*ppHull))
	{
		*ppHull = CopyWinding(pWinding);
		return;
	}

	iNumVerts = (*ppHull)->iNumPts;
	for (i=0; i < iNumVerts; i++)
		aHull[i] = (*ppHull)->pVerts[i];

	for (i=0; i < pWinding->iNumPts; i++)
	{
		v = pWinding->pVerts[i];

		//	Calculate normal of each side in current hull
		for (j=0; j < iNumVerts; j++)
		{
			k = (j+1) % iNumVerts;
			vDir = aHull[k] - aHull[j];
			vDir = CrossProduct(Normalize(vDir), vNormal);
			aSideDirs[j] = vDir;
		}

		//	Check current vertex's relative position to each side of current hull
		bOutside = false;
		for (j=0; j < iNumVerts; j++)
		{
			d = DotProduct(v-aHull[j], aSideDirs[j]);
			if (d >= EPSILON_ONPLANE)
				bOutside = true;
			
			if (d >= -EPSILON_ONPLANE)
				aOutside[j] = 1;	//	Outside
			else
				aOutside[j] = 0;	//	Inside
		}

		if (!bOutside)	//	Vertex is complete inside hull
			continue;

		//	Search the back side to front side transition
		for (j=0; j < iNumVerts; j++)
		{
			if (!aOutside[j] && aOutside[(j+1) % iNumVerts])
				break;
		}

		if (j == iNumVerts)
			continue;

		//	Insert this vertex just here
		aNewHull[0]  = v;
		iNumNewVerts = 1;

		//	Copy all vertices that aren't double front
		j = (j+1) % iNumVerts;
		for (k=0; k < iNumVerts; k++)
		{
			//	Double front ?
			if (aOutside[(j+k) % iNumVerts] && aOutside[(j+k+1) % iNumVerts])
				continue;

			//	Copy vertex
			aNewHull[iNumNewVerts++] = aHull[(j+k+1) % iNumVerts];
		}

		//	Use new hull as current hull
		iNumVerts = iNumNewVerts;
		for (j=0; j < iNumVerts; j++)
			aHull[j] = aNewHull[j];
	}

	//	Free old hull and create new one
	DestroyWinding(*ppHull);
	*ppHull = AllocWinding(iNumVerts);

	for (i=0; i < iNumVerts; i++)
		(*ppHull)->pVerts[i] = aHull[i];
}

/*	Calcuate the plane which specified winding is on.

	pWinding: specified winding.
	vNormal (out): used to store plane's normal.
	pfDist (out): used to store plane's d parameter.
*/	
void WindingPlane(s_WINDING* pWinding, A3DVECTOR3& vNormal, FLOAT* pfDist)
{
	A3DVECTOR3 d1 = pWinding->pVerts[1] - pWinding->pVerts[0];
	A3DVECTOR3 d2 = pWinding->pVerts[2] - pWinding->pVerts[0];

	vNormal	= CrossProduct(d1, d2);
	vNormal	= Normalize(vNormal);
	*pfDist	= DotProduct(vNormal, pWinding->pVerts[0]);
}

/*	Calcualte the bound sphere of specified winding.

	pWinding: valid winding.
	vOrigin (out): sphere's center position
	pfRadius (out): sphere's radius.
*/
void WindingSphere(s_WINDING* pWinding, A3DVECTOR3& vOrigin, float* pfRadius)
{
	A3DVECTOR3 vSum(0, 0, 0);

	for (int i=0; i < pWinding->iNumPts; i++)
		vSum = vSum + pWinding->pVerts[i];

	vSum = vSum / (float)pWinding->iNumPts;

	float fMaxRadius = 0.0f, d;

	for (i=0; i < pWinding->iNumPts; i++)
	{
		d = Magnitude(pWinding->pVerts[i] - vSum);
		if (d > fMaxRadius)
			fMaxRadius = d;
	}

	vOrigin	  = vSum;
	*pfRadius = fMaxRadius;
}

