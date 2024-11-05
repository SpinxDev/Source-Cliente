/*
 * FILE: PT_Sector.cpp
 *
 * DESCRIPTION: Sector routines
 *
 * CREATED BY: Duyuxin, 2003/6/12
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "PT_Internal.h"
#include "PT_Sector.h"
#include "PT_Math.h"
#include "PT_Plane.h"
#include "PT_Winding.h"
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

namespace PORTAL
{
	int _FilterDrawSurfToSector(s_PTSECTOR* pSector, int iDrawSurf);
	s_PTSECTORPART* _CreateSectorPart();
	void _ReleaseSectorPart(s_PTSECTORPART* pPart);
}

/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

namespace PORTAL
{

s_PTSECTOR* CreateSector(A3DVECTOR3* aVerts, WORD* aIndices, int iNumFace)
{
	int i, j, iIndex, iPlane;
	A3DVECTOR3 v0, v1, v2, vMin, vMax;

	s_PTSECTOR* pSector = new s_PTSECTOR;
	ASSERT(pSector);

	ClearBound(vMin.m, vMax.m);

	//	Calculate sector's bounding planes and bounding box
	for (i=0, iIndex=0; i < iNumFace; i++, iIndex+=3)
	{
		v0 = aVerts[aIndices[iIndex]];
		v1 = aVerts[aIndices[iIndex+1]];
		v2 = aVerts[aIndices[iIndex+2]];

		iPlane = MakePlaneFromPoints(v0, v1, v2);

		//	The plane has existed ?
		for (j=0; j < pSector->aPlaneRefs.GetSize(); j++)
		{
			if (iPlane == pSector->aPlaneRefs[j])
				break;
		}

		if (iPlane >= pSector->aPlaneRefs.GetSize())
			pSector->aPlaneRefs.Add(iPlane);

		AddPointToBound(vMin.m, vMax.m, v0);
		AddPointToBound(vMin.m, vMax.m, v1);
		AddPointToBound(vMin.m, vMax.m, v2);
	}

	pSector->vMins	= vMin;
	pSector->vMaxs	= vMax;

	return pSector;
}

s_PTPORTAL* CreatePortal(A3DVECTOR3* aVerts, int iNumVert, int iFront, int iBack, int iType)
{
	s_PTPORTAL* pPortal = new s_PTPORTAL;
	ASSERT(pPortal);

	int i;

	pPortal->pWinding = AllocWinding(iNumVert);

	for (i=0; i < iNumVert; i++)
		pPortal->pWinding->pVerts[i] = aVerts[i];

	pPortal->iPlane = MakePlaneFromPoints(aVerts[0], aVerts[1], aVerts[2]);
	pPortal->iFront = iFront;
	pPortal->iBack	= iBack;
	pPortal->iType	= iType;

	//	Add portal to sector
	g_aSectors[iFront]->aPortals.Add(pPortal);
	g_aSectors[iBack]->aPortals.Add(pPortal);

	return pPortal;
}

void ReleaseSector(s_PTSECTOR* pSector)
{
	pSector->aPlaneRefs.RemoveAll();
	pSector->aChilds.RemoveAll();
	pSector->aDrawSurfs.RemoveAll();
	pSector->aPortals.RemoveAll();

	for (int i=0; i < pSector->aParts.GetSize(); i++)
		_ReleaseSectorPart(pSector->aParts[i]);
	
	pSector->aParts.RemoveAll();

	delete pSector;
}

void ReleasePortal(s_PTPORTAL* pPortal)
{
	if (pPortal->pWinding)
		DestroyWinding(pPortal->pWinding);

	delete pPortal;
}

bool LinkSectors()
{
/*	for (int i=0; i < g_aSectors.GetSize(); i++)
	{
		//	Skip root sectors
		if (g_aSectors[i]->iParent < 0)
			continue;

		g_aSectors[g_aSectors[i]->iParent]->aChilds.Add(i);
	}
*/
	return true;
}

bool DispatchDrawSurfaces()
{
	int i, j;
	s_PTSECTOR* pSector;

	for (i=0; i < g_aSectors.GetSize(); i++)
	{
		pSector = g_aSectors[i];

		for (j=0; j < g_iNumDrawSurf; j++)
			_FilterDrawSurfToSector(pSector, j);
	}

	return true;
}

/*	Return values:
	
	0: draw surface is complete out of specified sector
	1: part of draw surface belong to specified sector
	2: whole draw surface belong to specified sector.
*/
int _FilterDrawSurfToSector(s_PTSECTOR* pSector, int iDrawSurf)
{
	s_PTPLANE* pPlane;
	s_DRAWSURF* pSurf;

	bool bIn = true;	//	Complete in flag

	for (int i=0; i < pSector->aPlaneRefs.GetSize(); i++)
	{
		pPlane	= &g_aPlanes[pSector->aPlaneRefs[i]];
		pSurf	= &g_aDrawSurfs[iDrawSurf];

		int iRet = WindingOnPlaneSide(pSurf->pWinding, pPlane->vNormal, pPlane->fDist);
		if (iRet == SIDE_FRONT)
			return 0;
		else if (iRet == SIDE_CROSS)
			bIn = false;
	}

	pSector->aDrawSurfs.Add(iDrawSurf);

	return bIn ? 2 : 1;
}

//	Part sectors into regions, each region contain some draw surfaces. This is helpful
//	to accelerate the surface culling process when rendering
bool PartSectors()
{
	int i, j;

	for (i=0; i < g_aSectors.GetSize(); i++)
	{
		s_PTSECTOR* pSector = g_aSectors[i];

		//	In current version, we don't part sectors. So each sector contains only one
		//	part, it's the sector itself.
		s_PTSECTORPART* pPart = _CreateSectorPart();
		pPart->vMins = pSector->vMins;
		pPart->vMaxs = pSector->vMaxs;

		for (j=0; j < pSector->aDrawSurfs.GetSize(); j++)
			pPart->aSurfs.Add(pSector->aDrawSurfs[j]);

		pSector->aParts.Add(pPart);
	}

	return true;
}

s_PTSECTORPART* _CreateSectorPart()
{
	s_PTSECTORPART* pPart = new s_PTSECTORPART;
	ASSERT(pPart);

	return pPart;
}

void _ReleaseSectorPart(s_PTSECTORPART* pPart)
{
	pPart->aSurfs.RemoveAll();
	delete pPart;
}

}	//	namespace PORTAL

