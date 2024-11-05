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

#include "PT_Interface.h"
#include "PT_Internal.h"
#include "PT_Sector.h"
#include "PT_Math.h"
#include "PT_Winding.h"
#include "PT_Plane.h"
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
	int _FilterDrawSurfToPart(s_PTSECTORPART* pPart, int iDrawSurf);
	void _PartSector(s_PTSECTOR* pSector);
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

		//	Create side
		s_PTSECTORSIDE* pSide = new s_PTSECTORSIDE;
		pSide->pWinding = AllocWinding(3);
		pSide->pWinding->pVerts[0] = v0;
		pSide->pWinding->pVerts[1] = v1;
		pSide->pWinding->pVerts[2] = v2;

		pSide->iPlane = iPlane;

		pSector->aSides.Add(pSide);

		//	Build bounding box
		AddPointToBound(vMin.m, vMax.m, v0);
		AddPointToBound(vMin.m, vMax.m, v1);
		AddPointToBound(vMin.m, vMax.m, v2);
	}

	pSector->vMins	= vMin;
	pSector->vMaxs	= vMax;

	return pSector;
}

bool CreatePortal(const PTADDPORTALDATA& Data, s_PTPORTAL** ppPortal1, s_PTPORTAL** ppPortal2)
{
	if (Data.iType == PTTYPE_BIDIRECTIONAL)
	{
		s_PTPORTAL *p1, *p2;

		p1 = new s_PTPORTAL;
		p2 = new s_PTPORTAL;

		ASSERT(p1 && p2);

		//	Portal 1
		p1->pWinding = AllocWinding(Data.iNumVert);
		for (int i=0; i < Data.iNumVert; i++)
			p1->pWinding->pVerts[i] = Data.aVerts[i];
		
		p1->iPlane		= MakePlaneFromPoints(Data.aVerts[0], Data.aVerts[1], Data.aVerts[2]);
		p1->iFront		= Data.iFront;
		p1->iBack		= Data.iBack;
		p1->pOpposite	= p2;

		//	Portal 2
		p2->pWinding	= ReverseWinding(p1->pWinding);
		p2->iPlane		= MakePlaneFromPoints(Data.aVerts[2], Data.aVerts[1], Data.aVerts[0]);
		p2->iFront		= Data.iBack;
		p2->iBack		= Data.iFront;
		p2->pOpposite	= p1;

		strcpy(p1->szName, Data.szName);
		strcpy(p2->szName, Data.szName);

		g_aSectors[Data.iFront]->aPortals.Add(p2);
		g_aSectors[Data.iBack]->aPortals.Add(p1);

		*ppPortal1 = p1;
		*ppPortal2 = p2;
	}
	else if (Data.iType == PTTYPE_BACKTOFRONT)
	{
		s_PTPORTAL *p = new s_PTPORTAL;
		ASSERT(p);

		//	Portal
		p->pWinding = AllocWinding(Data.iNumVert);
		for (int i=0; i < Data.iNumVert; i++)
			p->pWinding->pVerts[i] = Data.aVerts[i];
		
		p->iPlane		= MakePlaneFromPoints(Data.aVerts[0], Data.aVerts[1], Data.aVerts[2]);
		p->iFront		= Data.iFront;
		p->iBack		= Data.iBack;
		p->pOpposite	= NULL;

		strcpy(p->szName, Data.szName);

		g_aSectors[Data.iBack]->aPortals.Add(p);

		*ppPortal1 = p;
		*ppPortal2 = NULL;
	}
	else	//	iType == PTTYPE_FRONTTOBACK
	{
		s_PTPORTAL *p = new s_PTPORTAL;
		ASSERT(p);

		//	Portal
		p->pWinding = AllocWinding(Data.iNumVert);
		for (int i=0; i < Data.iNumVert; i++)
			p->pWinding->pVerts[i] = Data.aVerts[Data.iNumVert-i-1];
		
		p->iPlane		= MakePlaneFromPoints(Data.aVerts[2], Data.aVerts[0], Data.aVerts[1]);
		p->iFront		= Data.iBack;
		p->iBack		= Data.iFront;
		p->pOpposite	= NULL;

		strcpy(p->szName, Data.szName);

		g_aSectors[Data.iFront]->aPortals.Add(p);

		*ppPortal1 = p;
		*ppPortal2 = NULL;
	}

	return true;
}

void ReleaseSector(s_PTSECTOR* pSector)
{
	pSector->aPlaneRefs.RemoveAll();
	pSector->aChilds.RemoveAll();
	pSector->aDrawSurfs.RemoveAll();
	pSector->aPortals.RemoveAll();

	int i;
	for (i=0; i < pSector->aParts.GetSize(); i++)
		_ReleaseSectorPart(pSector->aParts[i]);
	
	pSector->aParts.RemoveAll();

	for (i=0; i < pSector->aSides.GetSize(); i++)
	{
		s_PTSECTORSIDE* pSide = pSector->aSides[i];
		DestroyWinding(pSide->pWinding);
		delete pSide;
	}

	pSector->aSides.RemoveAll();

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

		int iRet = WindingOnPlaneSide(pSurf->pWinding, pPlane->vNormal, pPlane->fDist, 0.1f);
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
	for (int i=0; i < g_aSectors.GetSize(); i++)
	{
		s_PTSECTOR* pSector = g_aSectors[i];
		_PartSector(pSector);
	}

	return true;
}

void _PartSector(s_PTSECTOR* pSector)
{
	int i, j, k, aDivs[3];
	A3DVECTOR3 vSize;

	//	Divide sector
	for (i=0; i < 3; i++)
	{
		float fLen = pSector->vMaxs.m[i] - pSector->vMins.m[i];

		if (!g_BuildParams.aSectorDiv[i])
		{
			vSize.m[i]	= fLen;
			aDivs[i]	= 1;
		}
		else
		{
			vSize.m[i] = fLen / g_BuildParams.aSectorDiv[i];

			float fSize = 0.0f;
			if (vSize.m[i] > g_BuildParams.fPartMax)
				fSize = g_BuildParams.fPartMax;
			else if (vSize.m[i] < g_BuildParams.fPartMin)
				fSize = g_BuildParams.fPartMin;

			if (fSize)
			{
				aDivs[i]	= (int)(fLen / fSize) + 1;
				vSize.m[i]	= fLen / (float)aDivs[i];
			}
			else
				aDivs[i] = g_BuildParams.aSectorDiv[i];
		}
	}
	
	//	Create parts
	s_PTSECTORPART* pPart;

	for (i=0; i < aDivs[0]; i++)
	{
		for (j=0; j < aDivs[1]; j++)
		{
			for (k=0; k < aDivs[2]; k++)
			{
				pPart = _CreateSectorPart();

				pPart->vMins.x = pSector->vMins.x + i * vSize.x;
				pPart->vMins.y = pSector->vMins.y + j * vSize.y;
				pPart->vMins.z = pSector->vMins.z + k * vSize.z;

				pPart->vMaxs.x = pPart->vMins.x + vSize.x;
				pPart->vMaxs.y = pPart->vMins.y + vSize.y;
				pPart->vMaxs.z = pPart->vMins.z + vSize.z;

				//	Filter draw surfaces to part
				for (int n=0; n < pSector->aDrawSurfs.GetSize(); n++)
					_FilterDrawSurfToPart(pPart, pSector->aDrawSurfs[n]);

				pSector->aParts.Add(pPart);
			}
		}
	}
}

/*	Return values:
	
	0: draw surface is complete out of specified sector
	1: part of draw surface belong to specified sector
	2: whole draw surface belong to specified sector.
*/
int _FilterDrawSurfToPart(s_PTSECTORPART* pPart, int iDrawSurf)
{
	A3DPLANE aPlanes[6];

	aPlanes[0].vNormal	= A3DVECTOR3(1.0f, 0.0f, 0.0f);
	aPlanes[0].fDist	= DotProduct(aPlanes[0].vNormal, pPart->vMaxs);

	aPlanes[1].vNormal	= A3DVECTOR3(-1.0f, 0.0f, 0.0f);
	aPlanes[1].fDist	= DotProduct(aPlanes[1].vNormal, pPart->vMins);

	aPlanes[2].vNormal	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
	aPlanes[2].fDist	= DotProduct(aPlanes[2].vNormal, pPart->vMaxs);

	aPlanes[3].vNormal	= A3DVECTOR3(0.0f, -1.0f, 0.0f);
	aPlanes[3].fDist	= DotProduct(aPlanes[3].vNormal, pPart->vMins);

	aPlanes[4].vNormal	= A3DVECTOR3(0.0f, 0.0f, 1.0f);
	aPlanes[4].fDist	= DotProduct(aPlanes[4].vNormal, pPart->vMaxs);

	aPlanes[5].vNormal	= A3DVECTOR3(0.0f, 0.0f, -1.0f);
	aPlanes[5].fDist	= DotProduct(aPlanes[5].vNormal, pPart->vMins);

	bool bIn = true;	//	Complete in flag

	s_DRAWSURF* pSurf = &g_aDrawSurfs[iDrawSurf];

	for (int i=0; i < 6; i++)
	{
		int iRet = WindingOnPlaneSide(pSurf->pWinding, aPlanes[i].vNormal, aPlanes[i].fDist, 0.1f);
		if (iRet == SIDE_FRONT)
			return 0;
		else if (iRet == SIDE_CROSS)
			bIn = false;
	}

	pPart->aSurfs.Add(iDrawSurf);

	return bIn ? 2 : 1;
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

int SectorOnPlaneSide(s_PTSECTOR* pSector, A3DVECTOR3& vNormal, float fDist)
{
	bool bBack=false, bFront=false;
	int iSide;

	for (int i=0; i < pSector->aDrawSurfs.GetSize(); i++)
	{
		s_DRAWSURF* pSurf = &g_aDrawSurfs[pSector->aDrawSurfs[i]];

		iSide = WindingOnPlaneSide(pSurf->pWinding, vNormal, fDist, 0.05f);
		if (iSide == SIDE_FRONT)
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
		return SIDE_CROSS;
}

}	//	namespace PORTAL

