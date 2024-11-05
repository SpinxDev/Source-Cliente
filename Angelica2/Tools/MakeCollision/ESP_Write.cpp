/*
 * FILE: ESP_Write.cpp
 *
 * DESCRIPTION: Functions used to write ESP file
 *
 * CREATED BY: Duyuxin, 2001/10/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "ESP_Write.h"
#include "Side.h"

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

void _WriteESPLump(FILE* fp, EFLUMP* aLumps, LONG lLump, void* pData, LONG lSize);
void _WriteESPWorld(FILE* fp, EFLUMP* aLumps);
void _WriteESPLeaves(FILE* fp, EFLUMP* aLumps);
void _WriteESPSides(FILE* fp, EFLUMP* aLumps);
void _WriteESPPlanes(FILE* fp, EFLUMP* aLumps);
void _WriteESPBrushes(FILE* fp, EFLUMP* aLumps);

/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

/*	Write lump data into ESP file.

	fp: ESP file's pointer.
	aLumps: ESP file lump information.
	lLump: lump will be written.
	pData: lump's data.
	lSize: total size of this lump.
*/
void _WriteESPLump(FILE* fp, EFLUMP* aLumps, LONG lLump, void* pData, LONG lSize)
{
	EFLUMP* pLump = &aLumps[lLump];

	pLump->lOff  = ftell(fp);
	pLump->lSize = lSize;

	if (pData && lSize)
		fwrite(pData, 1, lSize, fp);
}

/*	Create a new ESP file and Write data into it.

	Return TRUE for success, otherwise return FALSE.

	szFileName: ESP file's name.
*/
bool ESP_WriteFile(const char* szFileName)
{
	AddMessageToList("+ Write ESP file ...");
	SetCurProgressSeg(ESPPROG_WRITETOFILE);

	FILE* fp = fopen(szFileName, "wb+");
	if (!fp)
		return false;

	//	Write file header
	ESPFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));
	Header.dwIdentify	= ESP_IDENTIFY;
	Header.dwVersion	= ESP_VERSION;
	Header.iNumLump		= NUM_ESPLUMP;

	//	Write header
	fwrite(&Header, 1, sizeof (Header), fp);

	EFLUMP aLumps[NUM_ESPLUMP];
	memset(aLumps, 0, sizeof (aLumps));

	//	Hold lump information position
	fwrite(aLumps, 1, sizeof (aLumps), fp);

	//	Write lump data
	_WriteESPWorld(fp, aLumps);
	_WriteESPSides(fp, aLumps);
	_WriteESPPlanes(fp, aLumps);
	_WriteESPBrushes(fp, aLumps);
	_WriteESPLeaves(fp, aLumps);

	//	Rewrite lumps' information
	fseek(fp, sizeof (ESPFILEHEADER), SEEK_SET);
	fwrite(aLumps, 1, sizeof (aLumps), fp);

	fclose(fp);

	StepProgressBar(1, 1);

	return true;
}

void _WriteESPWorld(FILE* fp, EFLUMP* aLumps)
{
	EFWORLDINFO World;

	for (int i=0; i < 3; i++)
	{
		World.vAlignMins[i]	= g_ESPWorld.vAlignMins[i];
		World.vAlignMaxs[i]	= g_ESPWorld.vAlignMaxs[i];
		World.vLength[i]	= g_ESPWorld.vLength[i];
		World.vMaxs[i]		= g_ESPWorld.vMaxs[i];
		World.vMins[i]		= g_ESPWorld.vMins[i];
		World.vSize[i]		= g_ESPWorld.vSize[i];
	}

	//	Write world information lump
	_WriteESPLump(fp, aLumps, EFLUMP_WORLDINFO, &World, sizeof (EFWORLDINFO));
}

void _WriteESPSides(FILE* fp, EFLUMP* aLumps)
{
	int iNumSide = g_aSides.GetSize();
	if (!iNumSide)
	{
		_WriteESPLump(fp, aLumps, EFLUMP_SIDES, NULL, 0);
		_WriteESPLump(fp, aLumps, EFLUMP_VERTICES, NULL, 0);
		return;
	}

	EFSIDE *aSides = new EFSIDE[iNumSide];
	ASSERT(aSides);

	AArray<A3DVECTOR3, A3DVECTOR3&> aVerts(iNumSide * 3, 100);
	int i, j;

	for (i=0; i < iNumSide; i++)
	{
		EFSIDE* pDestSide = &aSides[i];
		s_SIDE* pSrcSide = g_aSides[i];

		pDestSide->lPlane		= pSrcSide->iPlane;
		pDestSide->lFirstVert	= aVerts.GetSize();
		pDestSide->lNumVert		= pSrcSide->pWinding->iNumPts;
		pDestSide->dwMeshProps	= pSrcSide->MeshProps.dwProps;
		pDestSide->iMtlType		= pSrcSide->MeshProps.MtlType;
		pDestSide->lBrush		= pSrcSide->iBrush;

		s_WINDING* pWinding = pSrcSide->pWinding;
		for (j=0; j < pWinding->iNumPts; j++)
			aVerts.Add(pWinding->pVerts[j]);
	}

	//	Write side lump
	_WriteESPLump(fp, aLumps, EFLUMP_SIDES, aSides, iNumSide * sizeof (EFSIDE));
	
	//	Write vertex lump
	_WriteESPLump(fp, aLumps, EFLUMP_VERTICES, aVerts.GetData(), aVerts.GetSize() * sizeof (A3DVECTOR3));

	delete [] aSides;
}

void _WriteESPPlanes(FILE* fp, EFLUMP* aLumps)
{
	EFPLANE* aPlanes = new EFPLANE[g_aPlanes.GetSize()];
	ASSERT(aPlanes);

	for (int i=0; i < g_aPlanes.GetSize(); i++)
	{
		EFPLANE* pDest = &aPlanes[i];
		s_PLANE* pSrc = &g_aPlanes[i];

		pDest->vNormal[0] = pSrc->vNormal.m[0];
		pDest->vNormal[1] = pSrc->vNormal.m[1];
		pDest->vNormal[2] = pSrc->vNormal.m[2];
		pDest->fDist	  = pSrc->fDist;
	}

	//	Write planes lump
	_WriteESPLump(fp, aLumps, EFLUMP_PLANES, aPlanes, g_aPlanes.GetSize() * sizeof (EFPLANE));

	delete [] aPlanes;
}

void _WriteESPBrushes(FILE* fp, EFLUMP* aLumps)
{
	int iNumBrush = g_aBrushes.GetSize();

	if (g_Configs.bNoBrush || !iNumBrush)
	{
		_WriteESPLump(fp, aLumps, EFLUMP_BRUSHES, NULL, 0);
		return;
	}

	EFBRUSH* aBrushes = new EFBRUSH[iNumBrush];
	ASSERT(aBrushes);

	AArray<int, int> aPlaneRefs(iNumBrush*6, 100);
	int i, j;

	for (i=0; i < iNumBrush; i++)
	{
		EFBRUSH* pDest = &aBrushes[i];
		s_BRUSH* pSrc = g_aBrushes[i];

		pDest->lFirstPlane	= aPlaneRefs.GetSize();
		pDest->lNumPlane	= pSrc->iNumPlane;

		for (j=0; j < pSrc->iNumPlane; j++)
			aPlaneRefs.Add(pSrc->aPlanes[j]);
	}

	//	Write brush lump
	_WriteESPLump(fp, aLumps, EFLUMP_BRUSHES, aBrushes, iNumBrush * sizeof (EFBRUSH));

	//	Write brush plane reference lump
	_WriteESPLump(fp, aLumps, EFLUMP_BRUSHPLANES, aPlaneRefs.GetData(), aPlaneRefs.GetSize() * sizeof (int));
	
	delete [] aBrushes;
}

void _WriteESPLeaves(FILE* fp, EFLUMP* aLumps)
{
	int iNumLeaf = g_ESPWorld.vSize[0] * g_ESPWorld.vSize[1] * g_ESPWorld.vSize[2];
	if (!iNumLeaf || !g_ESPWorld.aLeaves)
	{
		_WriteESPLump(fp, aLumps, EFLUMP_LEAVES, NULL, 0);
		_WriteESPLump(fp, aLumps, EFLUMP_LEAFSIDES, NULL, 0);
		return;
	}

	EFLEAF* aLeaves = new EFLEAF[iNumLeaf];
	ASSERT(aLeaves);

	AArray<int, int> aSideRefs(iNumLeaf * 3, 200);
	int i, iCount;

	for (i=0; i < iNumLeaf; i++)
	{
		EFLEAF* pDest = &aLeaves[i];
		s_ESPLEAF* pSrc = &g_ESPWorld.aLeaves[i];

		pDest->lFirstLeafSide = aSideRefs.GetSize();

		s_SIDEREF* pRef = pSrc->pSideRef;
		iCount = 0;

		while (pRef)
		{
			aSideRefs.Add(pRef->iSide);
			pRef = pRef->pNext;
			iCount++;
		}

		pDest->lNumLeafSides = iCount;
	}

	//	Write leaf lump
	_WriteESPLump(fp, aLumps, EFLUMP_LEAVES, aLeaves, iNumLeaf * sizeof (EFLEAF));

	//	Write side reference lump
	_WriteESPLump(fp, aLumps, EFLUMP_LEAFSIDES, aSideRefs.GetData(), aSideRefs.GetSize() * sizeof (int));

	delete [] aLeaves;
}

