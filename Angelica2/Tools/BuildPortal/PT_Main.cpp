/*
 * FILE: PT_Main.cpp
 *
 * DESCRIPTION: Main file of build portal module
 *
 * CREATED BY: Duyuxin, 2003/6/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "PT_Internal.h"
#include "PT_Sector.h"
#include "PT_Winding.h"
#include "PT_Write.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

namespace PORTAL
{
	ALog			g_Log;				//	Log tool
	s_PTSTATISTICS	g_Statistics;		//	Statistics

	A3DVECTOR3*		g_aSurfVerts	= NULL;		//	Surface vertices
	int*			g_aSurfIndices	= NULL;		//	Surface indices
	s_DRAWSURF*		g_aDrawSurfs	= NULL;		//	Draw surfaces
	int				g_iNumDrawSurf	= 0;		//	Number of draw surfaces

	AArray<s_PTPLANE, s_PTPLANE&>		g_aPlanes(100, 20);		//	Planes
	AArray<s_PTSECTOR*, s_PTSECTOR*>	g_aSectors(100, 20);	//	Sectors
	AArray<s_PTPORTAL*, s_PTPORTAL*>	g_aPortals(100, 20);	//	Portals
}

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

namespace PORTAL
{

void _ReleaseResources()
{
	if (g_aSurfVerts)
	{
		delete [] g_aSurfVerts;
		g_aSurfVerts = NULL;
	}

	if (g_aSurfIndices)
	{
		delete [] g_aSurfIndices;
		g_aSurfIndices = NULL;
	}

	if (g_aDrawSurfs)
	{
		delete [] g_aDrawSurfs;
		g_aDrawSurfs = NULL;
	}

	g_iNumDrawSurf = 0;

	g_aPlanes.RemoveAll();

	int i;
	for (i=0; i < g_aSectors.GetSize(); i++)
		ReleaseSector(g_aSectors[i]);
	
	for (i=0; i < g_aPortals.GetSize(); i++)
		ReleasePortal(g_aPortals[i]);

	g_aSectors.RemoveAll();
	g_aPortals.RemoveAll();
}

}

///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

using namespace PORTAL;

bool PT_Init()
{
	//	Open log file
	g_Log.Init("BuildPortal.log", "Build portal logs");

	memset(&g_Statistics, 0, sizeof (g_Statistics));

	return true;
}

bool PT_Reset()
{
	g_Log.Log("\nBuildPortal reset ...");

	memset(&g_Statistics, 0, sizeof (g_Statistics));

	_ReleaseResources();

	return true;
}

void PT_Release()
{
	g_Log.Log("\nBuildPortal release ...");

	_ReleaseResources();

	g_Log.Release();
}

/*	Add surfaces will be rendered.

	aVerts: vertices array
	aIndices: indices array
	iNumSurf: number of surfaces (triangles)
*/
bool PT_AddRenderSurfs(A3DVECTOR3* aVerts, WORD* aIndices, int iNumSurf)
{
	if (!(g_aDrawSurfs = new s_DRAWSURF[iNumSurf]))
	{
		g_Log.Log("PT_AddRenderSurfs, Not enough memory !");
		return false;
	}

	g_iNumDrawSurf = iNumSurf;

	int i, iIndex;

	for (i=0, iIndex=0; i < iNumSurf; i++, iIndex += 3)
	{
		s_DRAWSURF* pSurf = &g_aDrawSurfs[i];
		pSurf->pWinding = AllocWinding(3);

		pSurf->pWinding->pVerts[0] = aVerts[aIndices[iIndex]];
		pSurf->pWinding->pVerts[1] = aVerts[aIndices[iIndex+1]];
		pSurf->pWinding->pVerts[2] = aVerts[aIndices[iIndex+2]];
	}

	return true;
}

bool PT_AddRenderSurfs(A3DVECTOR3* aVerts, int iNumSurf)
{
	if (!(g_aDrawSurfs = new s_DRAWSURF[iNumSurf]))
	{
		g_Log.Log("PT_AddRenderSurfs, Not enough memory !");
		return false;
	}

	g_iNumDrawSurf = iNumSurf;

	int i;
	A3DVECTOR3* pVert = aVerts;

	for (i=0; i < iNumSurf; i++, pVert+=3)
	{
		s_DRAWSURF* pSurf = &g_aDrawSurfs[i];
		pSurf->pWinding = AllocWinding(3);

		pSurf->pWinding->pVerts[0] = pVert[0];
		pSurf->pWinding->pVerts[1] = pVert[1];
		pSurf->pWinding->pVerts[2] = pVert[2];
	}

	return true;
}

/*	Add sector.

	aVerts: vertices array
	aIndices: indices array
	iNumFace: number of face (triangles)
*/
bool PT_AddSector(A3DVECTOR3* aVerts, WORD* aIndices, int iNumFace)
{
	s_PTSECTOR* pSector = CreateSector(aVerts, aIndices, iNumFace);
	if (!pSector)
	{
		g_Log.Log("PT_AddSector, Failed to create sector !");
		return false;
	}

	g_aSectors.Add(pSector);

	return true;
}

/*	Add portal.

	aVerts: vertices array
	iNumVert: number of vertices
	iFront: index of front sector.
	iBack: index of back sector
	iType: portal type
*/
bool PT_AddPortal(A3DVECTOR3* aVerts, int iNumVert, int iFront, int iBack, int iType)
{
	s_PTPORTAL* pPortal = CreatePortal(aVerts, iNumVert, iFront, iBack, iType);
	if (!pPortal)
	{
		g_Log.Log("PT_AddPortal, Failed to create portal !");
		return false;
	}

	g_aPortals.Add(pPortal);

	return true;
}

//	Build sectors and portals
bool PT_Build()
{
	//	Link parent and child sectors 
/*	if (!LinkSectors())
	{
		g_Log.Log("PT_Build, Failed to call LinkSectors!");
		return false;
	}
*/
	//	Dispatch all draw surfaces to sectors
	if (!DispatchDrawSurfaces())
	{
		g_Log.Log("PT_Build, Failed to call DispatchDrawSurfaces!");
		return false;
	}

	//	Part sector into parts
	if (!PartSectors())
	{
		g_Log.Log("PT_Build, Failed to call PartSectors!");
		return false;
	}

	return true;
}

//	Save data to portal file
bool PT_Save(const char* szFile)
{
	FILE* fp = fopen(szFile, "wb+");
	if (!fp)
	{
		g_Log.Log("PT_Save, Failed to create file %s", szFile);
		return false;
	}

	PTFHEADER Header;
	memset(&Header, 0, sizeof (Header));
	Header.dwIdentify	= PTFILE_IDENTIFY;
	Header.dwVersion	= PTFILE_VERSION;

	//	Write header
	fwrite(&Header, 1, sizeof (Header), fp);

	WritePlanes(fp, &Header);
	WritePortals(fp, &Header);
	WriteSectors(fp, &Header);

	//	Rewrite header for lumps' information
	fseek(fp, 0, SEEK_SET);
	fwrite(&Header, 1, sizeof (Header), fp);

	fclose(fp);
	return true;
}



