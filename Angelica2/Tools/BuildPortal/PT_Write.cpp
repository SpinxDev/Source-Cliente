/*
 * FILE: PT_Write.cpp
 *
 * DESCRIPTION: File writing operations
 *
 * CREATED BY: Duyuxin, 2003/6/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "PT_Internal.h"
#include "PT_Write.h"
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
	void _WriteLump(FILE* fp, PTFHEADER* pHeader, int iLump, void* pData, DWORD dwSize);
	int GetPortalIndex(s_PTPORTAL* pPortal);
}

/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

namespace PORTAL
{

int GetPortalIndex(s_PTPORTAL* pPortal)
{
	for (int i=0; i < g_aPortals.GetSize(); i++)
	{
		if (g_aPortals[i] == pPortal)
			return i;
	}

	return -1;
}

/*	Write lump data into portal file.

	fp: ESP file's pointer.
	pHeader: ESP file's header.
	lLump: lump will be written.
	pData: lump's data.
	lSize: total size of this lump.
*/
void _WriteLump(FILE* fp, PTFHEADER* pHeader, int iLump, void* pData, DWORD dwSize)
{
	PTFLUMP* pLump = &pHeader->aLumps[iLump];

	pLump->dwOff  = ftell(fp);
	pLump->dwSize = dwSize;

	fwrite(pData, 1, dwSize, fp);
}

bool WritePlanes(FILE* fp, PTFHEADER* pHeader)
{
	PTFPLANE* aPlanes = new PTFPLANE[g_aPlanes.GetSize()];
	ASSERT(aPlanes);

	for (int i=0; i < g_aPlanes.GetSize(); i++)
	{
		PTFPLANE* pDest = &aPlanes[i];
		s_PTPLANE* pSrc = &g_aPlanes[i];

		pDest->vNormal[0] = pSrc->vNormal.m[0];
		pDest->vNormal[1] = pSrc->vNormal.m[1];
		pDest->vNormal[2] = pSrc->vNormal.m[2];
		pDest->fDist	  = pSrc->fDist;
	}

	//	Write planes lump
	_WriteLump(fp, pHeader, PTLUMP_PLANES, aPlanes, g_aPlanes.GetSize() * sizeof (PTFPLANE));

	delete [] aPlanes;

	return true;
}

bool WritePortals(FILE* fp, PTFHEADER* pHeader)
{
	PTFPORTAL* aPortals = new PTFPORTAL[g_aPortals.GetSize()];
	ASSERT(aPortals);

	AArray<A3DVECTOR3, A3DVECTOR3&> aVerts;
	int i, j;

	for (i=0; i < g_aPortals.GetSize(); i++)
	{
		PTFPORTAL* pDest = &aPortals[i];
		s_PTPORTAL* pSrc = g_aPortals[i];

		pDest->iPlane		= pSrc->iPlane;
		pDest->iFront		= pSrc->iFront;
		pDest->iBack		= pSrc->iBack;
		pDest->iType		= pSrc->iType;
		pDest->iFirstVert	= aVerts.GetSize();
		pDest->iNumVert		= pSrc->pWinding->iNumPts;

		//	Copy vertiecs
		for (j=0; j < pSrc->pWinding->iNumPts; j++)
			aVerts.Add(pSrc->pWinding->pVerts[j]);
	}

	//	Write portal vertices lump
	_WriteLump(fp, pHeader, PTLUMP_VERTEX, aVerts.GetData(), aVerts.GetSize() * sizeof (A3DVECTOR3));

	//	Write portal lump
	_WriteLump(fp, pHeader, PTLUMP_PORTAL, aPortals, g_aPortals.GetSize() * sizeof (PTFPORTAL));

	delete [] aPortals;

	return true;
}

bool WriteSectors(FILE* fp, PTFHEADER* pHeader)
{
	PTFSECTOR* aSectors = new PTFSECTOR[g_aSectors.GetSize()];
	ASSERT(aSectors);

	int i, j;
	AArray<int, int> aPlaneRefs;
	AArray<int, int> aPortalRefs;
	AArray<int, int> aPartSurfRefs;
	AArray<PTFSECTORPART, PTFSECTORPART&> aParts;

	for (i=0; i < g_aSectors.GetSize(); i++)
	{
		PTFSECTOR* pDest = &aSectors[i];
		s_PTSECTOR* pSrc = g_aSectors[i];

		pDest->vMins[0]		= pSrc->vMins.m[0];
		pDest->vMins[1]		= pSrc->vMins.m[1];
		pDest->vMins[2]		= pSrc->vMins.m[2];
		pDest->vMaxs[0]		= pSrc->vMaxs.m[0];
		pDest->vMaxs[1]		= pSrc->vMaxs.m[1];
		pDest->vMaxs[2]		= pSrc->vMaxs.m[2];
		pDest->iFirstPlane	= aPlaneRefs.GetSize();
		pDest->iNumPlane	= pSrc->aPlaneRefs.GetSize();

		//	Copy plane references
		for (j=0; j < pSrc->aPlaneRefs.GetSize(); j++)
			aPlaneRefs.Add(pSrc->aPlaneRefs[j]);

		pDest->iFirstPortal	= aPortalRefs.GetSize();
		pDest->iNumPortal	= pSrc->aPortals.GetSize();

		//	Copy portal references
		for (j=0; j < pSrc->aPortals.GetSize(); j++)
		{
			int iPortal = GetPortalIndex(pSrc->aPortals[j]);
			ASSERT(iPortal >= 0);
			aPortalRefs.Add(iPortal);
		}

		pDest->iFirstPart	= aParts.GetSize();
		pDest->iNumPart		= pSrc->aParts.GetSize();

		//	Copy parties
		for (j=0; j < pSrc->aParts.GetSize(); j++)
		{
			PTFSECTORPART DestPart;
			s_PTSECTORPART* pSrcPart = pSrc->aParts[j];

			DestPart.vMins[0]	= pSrcPart->vMins.m[0];
			DestPart.vMins[1]	= pSrcPart->vMins.m[1];
			DestPart.vMins[2]	= pSrcPart->vMins.m[2];
			DestPart.vMaxs[0]	= pSrcPart->vMaxs.m[0];
			DestPart.vMaxs[1]	= pSrcPart->vMaxs.m[1];
			DestPart.vMaxs[2]	= pSrcPart->vMaxs.m[2];
			DestPart.iFirstSurf	= aPartSurfRefs.GetSize();
			DestPart.iNumSurf	= pSrcPart->aSurfs.GetSize();

			aParts.Add(DestPart);

			for (int k=0; k < pSrcPart->aSurfs.GetSize(); k++)
				aPartSurfRefs.Add(pSrcPart->aSurfs[k]);
		}
	}

	//	Write plane reference lump
	_WriteLump(fp, pHeader, PTLUMP_SECTPLANEREF, aPlaneRefs.GetData(), aPlaneRefs.GetSize() * sizeof (int));

	//	Write portal reference lump
	_WriteLump(fp, pHeader, PTLUMP_SECTPORTALREF, aPortalRefs.GetData(), aPortalRefs.GetSize() * sizeof (int));

	//	Write part lump
	_WriteLump(fp, pHeader, PTLUMP_SECTORPART, aParts.GetData(), aParts.GetSize() * sizeof (PTFSECTORPART));

	//	Write part's draw surface reference lump
	_WriteLump(fp, pHeader, PTLUMP_PARTSURFACEREF, aPartSurfRefs.GetData(), aPartSurfRefs.GetSize() * sizeof (int));

	//	Write sector lump
	_WriteLump(fp, pHeader, PTLUMP_SECTOR, aSectors, g_aSectors.GetSize() * sizeof (PTFSECTOR));

	delete [] aSectors;

	return true;
}

}	//	namespace PORTAL

