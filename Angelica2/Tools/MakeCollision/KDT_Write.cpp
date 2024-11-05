/*
 * FILE: KDT_Write.h
 *
 * DESCRIPTION: Functions used to write KDT file
 *
 * CREATED BY: Duyuxin, 2003/8/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3DKDTreeFile.h"
#include "KDT_Write.h"
#include "KDT_Types.h"
#include "KDT_Main.h"
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

void _WriteKDTLump(FILE* fp, KDTFLUMP* aLumps, LONG lLump, void* pData, DWORD dwSize);
void _WriteKDTSides(FILE* fp, KDTFLUMP* aLumps);
void _WriteKDTPlanes(FILE* fp, KDTFLUMP* aLumps);
void _WriteKDTBrushes(FILE* fp, KDTFLUMP* aLumps);
bool _WriteKDTNodes(FILE* fp, KDTFLUMP* aLumps);

int _GetKDTNodeIndex(s_KDTNODE* pNode);

/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

/*	Write lump data into KDT file.

	fp: KDT file's pointer.
	aLumps: KDT file lump information.
	lLump: lump will be written.
	pData: lump's data.
	lSize: total size of this lump.
*/
void _WriteKDTLump(FILE* fp, KDTFLUMP* aLumps, LONG lLump, void* pData, DWORD dwSize)
{
	KDTFLUMP* pLump = &aLumps[lLump];

	pLump->dwOff  = ftell(fp);
	pLump->dwSize = dwSize;

	if (pData && dwSize)
		fwrite(pData, 1, dwSize, fp);
}

/*	Create a new KDT file and Write data into it.

	Return TRUE for success, otherwise return FALSE.

	szFileName: KDT file's name.
*/
bool KDT_WriteFile(const char* szFileName)
{
	SetCurProgressSeg(KDTPROG_WRITETOFILE);
	AddMessageToList("+ Write KDT file ...");

	FILE* fp = fopen(szFileName, "wb+");
	if (!fp)
		return false;

	//	Write file header
	KDTFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));
	Header.dwIdentify	= KDTFILE_IDENTIFY;
	Header.dwVersion	= KDTFILE_VERSION;
	Header.iNumLump		= NUM_KDTLUMP;

	//	Write header
	fwrite(&Header, 1, sizeof (Header), fp);

	KDTFLUMP aLumps[NUM_KDTLUMP];
	memset(aLumps, 0, sizeof (aLumps));

	//	Hold lump information position
	fwrite(aLumps, 1, sizeof (aLumps), fp);

	//	Write lump data
	_WriteKDTSides(fp, aLumps);
	_WriteKDTPlanes(fp, aLumps);
	_WriteKDTBrushes(fp, aLumps);
	_WriteKDTNodes(fp, aLumps);
	
	//	Rewrite lumps' information
	fseek(fp, sizeof (KDTFILEHEADER), SEEK_SET);
	fwrite(aLumps, 1, sizeof (aLumps), fp);

	fclose(fp);

	StepProgressBar(1, 1);

	return true;
}

void _WriteKDTSides(FILE* fp, KDTFLUMP* aLumps)
{
	int iNumSide = g_aSides.GetSize();
	if (!iNumSide)
	{
		_WriteKDTLump(fp, aLumps, KDTLUMP_SIDES, NULL, 0);
		_WriteKDTLump(fp, aLumps, KDTLUMP_VERTICES, NULL, 0);
		return;
	}

	KDTFSIDE *aSides = new KDTFSIDE[iNumSide];
	ASSERT(aSides);

	AArray<A3DVECTOR3, A3DVECTOR3&> aVerts(iNumSide * 3, 100);
	int i, j;

	for (i=0; i < iNumSide; i++)
	{
		KDTFSIDE* pDestSide = &aSides[i];
		s_SIDE* pSrcSide = g_aSides[i];

		pDestSide->iPlane		= pSrcSide->iPlane;
		pDestSide->iFirstVert	= aVerts.GetSize();
		pDestSide->iNumVert		= pSrcSide->pWinding->iNumPts;
		pDestSide->dwMeshProps	= pSrcSide->MeshProps.dwProps;
		pDestSide->iMtlType		= pSrcSide->MeshProps.MtlType;
		pDestSide->iBrush		= pSrcSide->iBrush;

		s_WINDING* pWinding = pSrcSide->pWinding;
		for (j=0; j < pWinding->iNumPts; j++)
			aVerts.Add(pWinding->pVerts[j]);
	}

	//	Write side lump
	_WriteKDTLump(fp, aLumps, KDTLUMP_SIDES, aSides, iNumSide * sizeof (KDTFSIDE));
	
	//	Write vertex lump
	_WriteKDTLump(fp, aLumps, KDTLUMP_VERTICES, aVerts.GetData(), aVerts.GetSize() * sizeof (A3DVECTOR3));

	delete [] aSides;
}

void _WriteKDTPlanes(FILE* fp, KDTFLUMP* aLumps)
{
	KDTFPLANE* aPlanes = new KDTFPLANE[g_aPlanes.GetSize()];
	ASSERT(aPlanes);

	for (int i=0; i < g_aPlanes.GetSize(); i++)
	{
		KDTFPLANE* pDest = &aPlanes[i];
		s_PLANE* pSrc = &g_aPlanes[i];

		pDest->vNormal[0] = pSrc->vNormal.m[0];
		pDest->vNormal[1] = pSrc->vNormal.m[1];
		pDest->vNormal[2] = pSrc->vNormal.m[2];
		pDest->fDist	  = pSrc->fDist;
	}

	//	Write planes lump
	_WriteKDTLump(fp, aLumps, KDTLUMP_PLANES, aPlanes, g_aPlanes.GetSize() * sizeof (KDTFPLANE));

	delete [] aPlanes;
}

void _WriteKDTBrushes(FILE* fp, KDTFLUMP* aLumps)
{
	int iNumBrush = g_aBrushes.GetSize();

	if (g_Configs.bNoBrush || !iNumBrush)
	{
		_WriteKDTLump(fp, aLumps, KDTLUMP_BRUSHES, NULL, 0);
		return;
	}

	KDTFBRUSH* aBrushes = new KDTFBRUSH[iNumBrush];
	ASSERT(aBrushes);

	AArray<int, int> aPlaneRefs(iNumBrush*6, 100);
	int i, j;

	for (i=0; i < iNumBrush; i++)
	{
		KDTFBRUSH* pDest = &aBrushes[i];
		s_BRUSH* pSrc = g_aBrushes[i];

		pDest->iFirstPlane	= aPlaneRefs.GetSize();
		pDest->iNumPlane	= pSrc->iNumPlane;

		for (j=0; j < pSrc->iNumPlane; j++)
			aPlaneRefs.Add(pSrc->aPlanes[j]);
	}

	//	Write brush lump
	_WriteKDTLump(fp, aLumps, KDTLUMP_BRUSHES, aBrushes, iNumBrush * sizeof (KDTFBRUSH));

	//	Write brush plane reference lump
	_WriteKDTLump(fp, aLumps, KDTLUMP_BRUSHPLANES, aPlaneRefs.GetData(), aPlaneRefs.GetSize() * sizeof (int));
	
	delete [] aBrushes;
}

int _GetKDTNodeIndex(s_KDTNODE* pNode)
{
	for (int i=0; i < g_aKDTNodes.GetSize(); i++)
	{
		if (pNode == g_aKDTNodes[i])
			return i;
	}

	return -1;
}

bool _WriteKDTNodes(FILE* fp, KDTFLUMP* aLumps)
{
	int iNumNode = g_aKDTNodes.GetSize();
	if (!iNumNode)
	{
		_WriteKDTLump(fp, aLumps, KDTLUMP_NODES, NULL, 0);
		_WriteKDTLump(fp, aLumps, KDTLUMP_LEAFSIDES, NULL, 0);
		return true;
	}

	KDTFNODE* aNodes = new KDTFNODE[iNumNode];
	ASSERT(aNodes);

	AArray<int, int> aSideRefs(iNumNode * 3, 200);
	int i, j, iCount;

	for (i=0; i < iNumNode; i++)
	{
		KDTFNODE* pDest = &aNodes[i];
		s_KDTNODE* pSrc = g_aKDTNodes[i];

		pDest->iPlane		= pSrc->iPlane;
		pDest->iParent		= pSrc->pParent ? _GetKDTNodeIndex(pSrc->pParent) : -1;
		pDest->aChildren[0]	= pSrc->aChildren[0] ? _GetKDTNodeIndex(pSrc->aChildren[0]) : -1;
		pDest->aChildren[1]	= pSrc->aChildren[1] ? _GetKDTNodeIndex(pSrc->aChildren[1]) : -1;
		pDest->iFirstSide	= aSideRefs.GetSize();

		for (j=0; j < 3; j++)
		{
			pDest->vMins[j] = pSrc->vMins.m[j];
			pDest->vMaxs[j] = pSrc->vMaxs.m[j];
		}

		s_SIDEREF* pRef = pSrc->pSideRef;
		iCount = 0;

		while (pRef)
		{
			aSideRefs.Add(pRef->iSide);
			pRef = pRef->pNext;
			iCount++;
		}

		pDest->iNumSide = iCount;
	}

	//	Write KDT node lump
	_WriteKDTLump(fp, aLumps, KDTLUMP_NODES, aNodes, iNumNode * sizeof (KDTFNODE));

	//	Write side reference lump
	_WriteKDTLump(fp, aLumps, KDTLUMP_LEAFSIDES, aSideRefs.GetData(), aSideRefs.GetSize() * sizeof (int));

	delete [] aNodes;

	return true;
}


