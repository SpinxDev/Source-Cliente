/*
 * FILE: Brush.cpp
 *
 * DESCRIPTION: Routines for bursh
 *
 * CREATED BY: Duyuxin, 2002/4/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "Brush.h"
#include "Plane.h"
#include "MathLib.h"
#include "Winding.h"
#include "AArray.h"
#include "AMemory.h"
#include "A3DSceneHull.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

#define MAXNUM_BRUSHEDGE	24
#define MAXNUM_BRUSHVERT	8

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

struct BUILDBRUSH
{
	AArray<int, int>	aPlanes;

	A3DVECTOR3	aEdges[MAXNUM_BRUSHEDGE];
	A3DVECTOR3	aEdgeVerts[MAXNUM_BRUSHEDGE];
	int			iNumEdge;
	A3DVECTOR3	aVerts[MAXNUM_BRUSHVERT];
	int			iNumVert;
	float		vMins[3];
	float		vMaxs[3];
};

BUILDBRUSH	l_BuildBrush;

/////////////////////////////////////////////////////////////////////////
//
//	Declaration of local functions
//
/////////////////////////////////////////////////////////////////////////

s_BRUSH* _AllocBrush(int iNumPlane);
void _DestroyBrush(s_BRUSH* pBrush);
void _ClearBuildBrush();
void _AddBrushBevels();

/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

//	Allocate a brush
s_BRUSH* _AllocBrush(int iNumPlane)
{
	s_BRUSH* pBrush = new s_BRUSH;
	memset(pBrush, 0, sizeof (s_BRUSH));

	pBrush->aPlanes = (int*)a_malloc(iNumPlane * sizeof (int));
	if (!pBrush->aPlanes)
	{
		AddMessageToList("! _AllocBrush, No memory for planes !");
		return NULL;
	}

	return pBrush;
}

//	Release a brush
void _DestroyBrush(s_BRUSH* pBrush)
{
	if (pBrush->aPlanes)
	{
		a_free(pBrush->aPlanes);
		pBrush->aPlanes		= NULL;
		pBrush->iNumPlane	= 0;
	}

	delete pBrush;
}

//	Clear building brush
void _ClearBuildBrush()
{
	l_BuildBrush.iNumEdge	= 0;
	l_BuildBrush.iNumVert	= 0;

	l_BuildBrush.aPlanes.RemoveAll();

	ClearBound(l_BuildBrush.vMins, l_BuildBrush.vMaxs);
}

//	Release all brushes
void ReleaseAllBrushes()
{
	for (int i=0; i < g_aBrushes.GetSize(); i++)
		_DestroyBrush(g_aBrushes[i]);
	
	g_aBrushes.RemoveAll();
}

/*	Generate a brush form a side. Side must be a triangle or quadrangle

	Return brush's index for this side if success, otherwise return -1

	pSide: side's data
*/
int GenerateBrushForSide(s_SIDE* pSide)
{
	if (pSide->iPlane == -1)
		return -1;

	//	Clear building brush
	_ClearBuildBrush();

	s_PLANE Plane = g_aPlanes[pSide->iPlane];
	s_PLANE BackPlane;
	s_SIDE Side;
	int i, j, iPlane;

	//	If side has SIDEFLAG_TWOSIDES flag, we will add a thickness to the side 
	if (pSide->MeshProps.Get2SidesFlag())
	{
		A3DVECTOR3 vNormal	= Plane.vNormal;
		float fDist			= Plane.fDist + g_Configs.fBrushThick * 0.5f;
		A3DVECTOR3 vOff		= vNormal * (g_Configs.fBrushThick * 0.5f);

		Side.pWinding = AllocWinding(pSide->pWinding->iNumPts);
		for (i=0; i < pSide->pWinding->iNumPts; i++)
			Side.pWinding->pVerts[i] = pSide->pWinding->pVerts[i] + vOff;

		Side.pWinding->iNumPts = pSide->pWinding->iNumPts;

		Side.iPlane		= SearchPlane(vNormal, fDist);
		Side.MeshProps  = pSide->MeshProps;
		Side.iBrush		= -1;

		pSide	= &Side;
		Plane	= g_aPlanes[Side.iPlane];
	}

	BackPlane.vNormal = A3DVECTOR3(0.0f) - Plane.vNormal;
	BackPlane.fDist	  = -(Plane.fDist - g_Configs.fBrushThick);

	//	Add front and back planes
	l_BuildBrush.aPlanes.Add(pSide->iPlane);
	l_BuildBrush.aPlanes.Add(SearchPlane(BackPlane.vNormal, BackPlane.fDist));

	A3DVECTOR3 v0, v1, v2, v3, vOff1, vOff2;

	//	Push backward new winding
	vOff1 = Plane.vNormal * (-5.0f);
	vOff2 = Plane.vNormal * (-g_Configs.fBrushThick);

	//	Add border planes
	for (i=0; i < pSide->pWinding->iNumPts; i++)
	{
		j = (i + 1) % pSide->pWinding->iNumPts;

		v0 = pSide->pWinding->pVerts[i];
		v1 = pSide->pWinding->pVerts[j];
		v2 = v0 + vOff1;
		v3 = v1 + vOff1;

		if ((iPlane = MakePlaneFromPoints(v0, v2, v3)) == -1)
		{
			AddMessageToList("! GenerateBrushFromSide, Invalid brush plane");
			return -1;
		}

		l_BuildBrush.aPlanes.Add(iPlane);

		//	Add edges to brush
		v2 = v0 + vOff2;
		v3 = v1 + vOff2;

		l_BuildBrush.aEdges[l_BuildBrush.iNumEdge]		 = v1 - v0;
		l_BuildBrush.aEdgeVerts[l_BuildBrush.iNumEdge++] = v0;
		l_BuildBrush.aEdges[l_BuildBrush.iNumEdge]		 = v0 - v1;
		l_BuildBrush.aEdgeVerts[l_BuildBrush.iNumEdge++] = v1;
		l_BuildBrush.aEdges[l_BuildBrush.iNumEdge]		 = v3 - v2;
		l_BuildBrush.aEdgeVerts[l_BuildBrush.iNumEdge++] = v2;
		l_BuildBrush.aEdges[l_BuildBrush.iNumEdge]		 = v2 - v3;
		l_BuildBrush.aEdgeVerts[l_BuildBrush.iNumEdge++] = v3;
		l_BuildBrush.aEdges[l_BuildBrush.iNumEdge]		 = v2 - v0;
		l_BuildBrush.aEdgeVerts[l_BuildBrush.iNumEdge++] = v0;
		l_BuildBrush.aEdges[l_BuildBrush.iNumEdge]		 = v0 - v2;
		l_BuildBrush.aEdgeVerts[l_BuildBrush.iNumEdge++] = v2;

		//	Build brush's bounding box
		AddPointToBound(l_BuildBrush.vMins, l_BuildBrush.vMaxs, v0);
		AddPointToBound(l_BuildBrush.vMins, l_BuildBrush.vMaxs, v2);

		//	Add vertices
		l_BuildBrush.aVerts[l_BuildBrush.iNumVert++] = v0;
		l_BuildBrush.aVerts[l_BuildBrush.iNumVert++] = v2;
	}

	_AddBrushBevels();

	s_BRUSH* pBrush = _AllocBrush(l_BuildBrush.aPlanes.GetSize());
	pBrush->iNumPlane = l_BuildBrush.aPlanes.GetSize();

	for (i=0; i < l_BuildBrush.aPlanes.GetSize(); i++)
		pBrush->aPlanes[i] = l_BuildBrush.aPlanes[i];

	int iRet = g_aBrushes.GetSize();
	g_aBrushes.Add(pBrush);

	//	If side has SIDEFLAG_TWOSIDES flag
	if (pSide->MeshProps.Get2SidesFlag())
	{
		//	Create another brush follow
/*		ESPSIDE Side;

		BackPlane.vNormal = -pPlane->vNormal;
		BackPlane.fDist	  = -pPlane->fDist;

		Side.pWinding = ReverseWinding(pSide->pWinding);
		Side.iPlane	  = SearchPlane(BackPlane.vNormal, BackPlane.fDist);
		Side.iBrush	  = -1;
		Side.dwFlags  = 0;

		GenerateBrushForSide(&Side);

		DestroyWinding(Side.pWinding);
*/
		DestroyWinding(Side.pWinding);
	}

	return iRet;
}

//	Add bevels to current building brush
void _AddBrushBevels()
{
	int i, iAxis, iDir, iOrder;
	s_PLANE Plane;
	A3DVECTOR3 vNormal;
	float fDist;

	//	Add the axial planes
	for (iAxis=0, iOrder=0; iAxis < 3; iAxis++)
	{
		for (iDir=-1; iDir <= 1; iDir+=2, iOrder++)
		{
			//	See if the plane is allready present
			for (i=0; i < l_BuildBrush.aPlanes.GetSize(); i++)
			{
				Plane = g_aPlanes[l_BuildBrush.aPlanes[i]];
				if (FLOATISZERO(Plane.vNormal.m[iAxis] - iDir))
					break;
			}

			if (i == l_BuildBrush.aPlanes.GetSize())
			{
				vNormal = A3DVECTOR3(0.0f);
				vNormal.m[iAxis] = (float)iDir;

				if (iDir == 1)
					fDist = l_BuildBrush.vMaxs[iAxis];
				else
					fDist = -l_BuildBrush.vMins[iAxis];

				l_BuildBrush.aPlanes.Add(SearchPlane(vNormal, fDist));
			}

			//	If the plane is not in it canonical order, swap it
			if (i != iOrder)
			{
				int iTemp = l_BuildBrush.aPlanes[iOrder];
				l_BuildBrush.aPlanes[iOrder] = l_BuildBrush.aPlanes[i];
				l_BuildBrush.aPlanes[i] = iTemp;
			}
		}
	}

	//	Add the edge bevels
	if (l_BuildBrush.aPlanes.GetSize() == 6)
		return;		//	Pure axial
	else
	{
		A3DVECTOR3 vEdge, vAxis;
		int	j, k;
		float d;

		//	Test the non-axial plane edges
		for (i=0; i < l_BuildBrush.iNumEdge; i++)
		{
			vEdge = l_BuildBrush.aEdges[i];
			if (Normalize(vEdge, vEdge) < 0.5f)		//	bad vector
				continue;
			
			SnapVector(vEdge);

			//	Is a axial edge ?
			for (j=0; j < 3; j++)
			{
				if (vEdge.m[j] == -1.0f || vEdge.m[j] == 1.0f)
					break;		//	Axial
			}
			
			if (j != 3)
				continue;	//	Only test non-axial edges

			//	Try the six possible slanted axials from this edge
			for (iAxis=0; iAxis < 3; iAxis++)
			{
				for (iDir=-1; iDir <= 1; iDir+=2)
				{
					//	Construct a plane
					vAxis = A3DVECTOR3(0.0f);
					vAxis.m[iAxis] = (float)iDir;

					vNormal = CrossProduct(vEdge, vAxis);
					if (Normalize(vNormal, vNormal) < 0.5f)
						continue;

					SnapVector(vNormal);

					fDist = DotProduct(l_BuildBrush.aEdgeVerts[i], vNormal);

					//	If all the points on all the sides are
					//	behind this plane, it is a proper edge bevel
					for (j=0; j < l_BuildBrush.aPlanes.GetSize(); j++)
					{
						//	If this plane has allready been used, skip it
						if (ComparePlane(&g_aPlanes[l_BuildBrush.aPlanes[j]], vNormal, fDist))
							break;

						for (k=0; k < l_BuildBrush.iNumVert; k++)
						{
							d = DotProduct(l_BuildBrush.aVerts[k], vNormal) - fDist;
							if (d > 0.01f)
								break;		//	Point in front
						}
						
						if (k != l_BuildBrush.iNumVert)
							break;
					}

					if (j != l_BuildBrush.aPlanes.GetSize())
						continue;	//	Wasn't part of the outer hull

					l_BuildBrush.aPlanes.Add(SearchPlane(vNormal, fDist));
				}
			}
		}		  
	}
}



