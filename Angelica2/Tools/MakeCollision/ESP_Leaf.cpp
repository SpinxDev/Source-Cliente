/*
 * FILE: ESP_Leaf.cpp
 *
 * DESCRIPTION: Separate and distribute sides, manage leaves
 *
 * CREATED BY: Duyuxin, 2003/8/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "MathLib.h"
#include "Plane.h"
#include "Winding.h"
#include "Side.h"
#include "ESP_Leaf.h"
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

bool _CreateESPWorld();
void _ReleaseESPWorld();
int _FilterSide(int iSide, int x, int y, int z);
bool _InitializeLeaves();
void _FilterSidesIntoLeaves();

/////////////////////////////////////////////////////////////////////////
//
//	Implement
//
/////////////////////////////////////////////////////////////////////////

bool ESP_Build()
{
	if (!_CreateESPWorld())
	{
		AddMessageToList("! ESP_Build, failed to call _CreateESPWorld()");
		return false;
	}

	_FilterSidesIntoLeaves();

	return true;
}

void ESP_Release()
{
	_ReleaseESPWorld();
}

bool _CreateESPWorld()
{
	if (!g_aSides.GetSize())
	{
		AddMessageToList("! _CreateESPWorld, No side was found");
		return false;
	}

	int i, j;

	//	Calculate bounding box of scene
	ClearBound(g_ESPWorld.vMins, g_ESPWorld.vMaxs);
	
	for (i=0; i < g_aSides.GetSize(); i++)
	{
		s_SIDE* pSide = g_aSides[i];
		for (j=0; j < pSide->pWinding->iNumPts; j++)
			AddPointToBound(g_ESPWorld.vMins, g_ESPWorld.vMaxs, pSide->pWinding->pVerts[j]);
	}

	//	Extend cluster's bound, this can prevent error occures in 3DDDA algorithm
	//	when calculate the last step on major axis
	int iDefLeafSize = g_Configs.iESPLeafSize;
	float fBorderErr = (float)iDefLeafSize;

	//	Adjust cluster's bounding box so that leaf number in cluster is integral
	for (i=0; i < 3; i++)
	{
		g_ESPWorld.vAlignMins[i] = (float)(floor((g_ESPWorld.vMins[i] - fBorderErr) / iDefLeafSize) * iDefLeafSize);
		g_ESPWorld.vAlignMaxs[i] = (float)(ceil((g_ESPWorld.vMaxs[i] + fBorderErr) / iDefLeafSize) * iDefLeafSize);

		g_ESPWorld.vLength[i]	= iDefLeafSize;
		g_ESPWorld.vSize[i]		= (int)((g_ESPWorld.vAlignMaxs[i] - g_ESPWorld.vAlignMins[i]) / iDefLeafSize + 0.5f);
	}

	int iNumLeaf = g_ESPWorld.vSize[0] * g_ESPWorld.vSize[1] * g_ESPWorld.vSize[2];

	//	Allocate memory for leaves
	g_ESPWorld.aLeaves = new s_ESPLEAF[iNumLeaf];
	if (!g_ESPWorld.aLeaves)
	{
		AddMessageToList("! _CreateESPWorld, Failed to allocate leaves memory");
		return false;
	}

	if (!_InitializeLeaves())
	{
		AddMessageToList("! _CreateESPWorld, Failed to call _InitializeLeaves()");
		return false;
	}

	return true;
}

void _ReleaseESPWorld()
{
	if (g_ESPWorld.aLeaves)
	{
		int iNumLeaf = g_ESPWorld.vSize[0] * g_ESPWorld.vSize[1] * g_ESPWorld.vSize[2];
		for (int i=0; i < iNumLeaf; i++)
			DestroySideRefList(g_ESPWorld.aLeaves[i].pSideRef);

		delete [] g_ESPWorld.aLeaves;
	}

	memset(&g_ESPWorld, 0, sizeof (g_ESPWorld));
}

/*	Initialize leaves. Build separating planes which are also surrounding planes of leaves.
	All separating planes are axial, so only record their d parameters.
*/
bool _InitializeLeaves()
{
	int x, y, z;
	s_ESPLEAF* pLeaf = g_ESPWorld.aLeaves;

	for (y=0; y < g_ESPWorld.vSize[1]; y++)
	{
		for (z=0; z < g_ESPWorld.vSize[2]; z++)
		{
			for (x=0; x < g_ESPWorld.vSize[0]; x++)
			{
				//	Planes parallel x axis
				pLeaf->aDists[0] = g_ESPWorld.vAlignMins[0] + g_ESPWorld.vLength[0] * x;
				pLeaf->aDists[1] = g_ESPWorld.vAlignMins[0] + g_ESPWorld.vLength[0] * (x+1);

				//	Planes parallel y axis
				pLeaf->aDists[2] = g_ESPWorld.vAlignMins[1] + g_ESPWorld.vLength[1] * y;
				pLeaf->aDists[3] = g_ESPWorld.vAlignMins[1] + g_ESPWorld.vLength[1] * (y+1);

				//	Planes parallel z axis
				pLeaf->aDists[4] = g_ESPWorld.vAlignMins[2] + g_ESPWorld.vLength[2] * z;
				pLeaf->aDists[5] = g_ESPWorld.vAlignMins[2] + g_ESPWorld.vLength[2] * (z+1);

				pLeaf->pSideRef = NULL;

				pLeaf++;
			}
		}
	}

	return true;
}

/*	Filter all side into leaves, after this, every leaf will has a link list to
	store references to all sides in it.
*/
void _FilterSidesIntoLeaves()
{
	AddMessageToList("+ Filter sides ...");
	SetCurProgressSeg(ESPPROG_FILTERSIDES);

	int vMins[3], vMaxs[3];
	int i, j, n, vPos[3], iNumRef=0;

	for (i=0; i < g_aSides.GetSize(); i++)
	{
		s_SIDE* pSide = g_aSides[i];
		s_WINDING* pWinding = pSide->pWinding;
	
		//	Clear box
		for (j=0; j < 3; j++)
		{
			vMins[j] = 0x7fffffff;
			vMaxs[j] = -1;	
		}

		//	Calculate side's bounding box represented by leaves
		for (j=0; j < pWinding->iNumPts; j++)
		{
			//	Which leaf this vertex in ?
			for (n=0; n < 3; n++)
			{
				vPos[n] = (int)((pWinding->pVerts[j].m[n] - g_ESPWorld.vAlignMins[n]) / g_ESPWorld.vLength[n]);

				if (vPos[n] < 0)
				{
					AddMessageToList("! _FilterSidesIntoLeaves, %d side beyond world border", i);
					vPos[n] = 0;
				}
				else if (vPos[n] >= g_ESPWorld.vSize[n])
				{
					AddMessageToList("! _FilterSidesIntoLeaves, %d side beyond world border", i);
					vPos[n] = g_ESPWorld.vSize[n] - 1;
				}

				//	Add this leaf to box
				if (vPos[n] < vMins[n])
					vMins[n] = vPos[n];
				
				if (vPos[n] > vMaxs[n])
					vMaxs[n] = vPos[n];
			}
		}

		for (int x=vMins[0]; x <= vMaxs[0]; x++)
		{
			for (int y=vMins[1]; y <= vMaxs[1]; y++)
			{
				for (int z=vMins[2]; z <= vMaxs[2]; z++)
				{
					if (_FilterSide(i, x, y, z))
						iNumRef++;
				}
			}
		}

		StepProgressBar(i+1, g_aSides.GetSize());
	}

	AddMessageToList("+ %d sides were referenced %d times", g_aSides.GetSize(), iNumRef);
}

/*	Filter a side into leaf.

	Return true if side collisde with leaf.

	iSide: side's index
	x, y, z: leaf's position in ESP space.
*/
int _FilterSide(int iSide, int x, int y, int z)
{
	s_SIDE* pSide = g_aSides[iSide];

	s_ESPLEAF* pLeaf = &g_ESPWorld.aLeaves[y * g_ESPWorld.vSize[0] * g_ESPWorld.vSize[2] + 
										   z * g_ESPWorld.vSize[0] + x];

	s_PLANE aPlanes[6];

	aPlanes[0].vNormal	= A3DVECTOR3(-1.0f, 0.0f, 0.0f);
	aPlanes[0].fDist	= -pLeaf->aDists[0];
	aPlanes[1].vNormal	= A3DVECTOR3(1.0f, 0.0f, 0.0f);
	aPlanes[1].fDist	= pLeaf->aDists[1];

	aPlanes[2].vNormal	= A3DVECTOR3(0.0f, -1.0f, 0.0f);
	aPlanes[2].fDist	= -pLeaf->aDists[2];
	aPlanes[3].vNormal	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
	aPlanes[3].fDist	= pLeaf->aDists[3];

	aPlanes[4].vNormal	= A3DVECTOR3(0.0f, 0.0f, -1.0f);
	aPlanes[4].fDist	= -pLeaf->aDists[4];
	aPlanes[5].vNormal	= A3DVECTOR3(0.0f, 0.0f, 1.0f);
	aPlanes[5].fDist	= pLeaf->aDists[5];

	for (int i=0; i < 6; i++)
	{
		if (WindingOnPlaneSide(pSide->pWinding, aPlanes[i].vNormal, aPlanes[i].fDist, 0.0001f) == SIDE_FRONT)
			return false;
	}

	//	Add side to leaf
	s_SIDEREF* pRef = AllocSideRef();
	pRef->iSide		= iSide;
	pRef->pNext		= pLeaf->pSideRef;
	pLeaf->pSideRef = pRef;

	return true;
}



