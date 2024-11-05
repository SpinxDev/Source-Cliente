/*
 * FILE: PT_Internal.h
 *
 * DESCRIPTION: Internal header file
 *
 * CREATED BY: Duyuxin, 2003/6/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_PT_INTERNAL_H_
#define _PT_INTERNAL_H_

#include "AArray.h"
#include "AList2.h"
#include "A3DTypes.h"
#include "A3DPortalFile.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////

//	Size of world
#define MAXSIZE_WORLD		100000

enum
{
	SIDE_FRONT = 0,
	SIDE_BACK,
	SIDE_ON,
	SIDE_CROSS,
};

/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////

struct s_WINDING
{
	int			iNumPts;	//	Number of points
	A3DVECTOR3*	pVerts;		//	Vertices
};

struct s_DRAWSURF
{
	s_WINDING*	pWinding;	//	Winding
};

struct s_PTPLANE
{
	A3DVECTOR3	vNormal;
	float		fDist;
	int			iType;
};

struct s_PTSECTORPART
{
	A3DVECTOR3	vMins;		//	AABB of sector part
	A3DVECTOR3	vMaxs;

	AArray<int, int>	aSurfs;		//	Surface indices
};

struct s_PTSECTOR;	//	Pre-declare

struct s_PTPORTAL
{
	int			iPlane;		//	Plane's index
	int			iFront;		//	Sector this portal face to
	int			iBack;		//	Sector this portal back to
	int			iType;		//	Portal type
	s_WINDING*	pWinding;	//	Winding
};

struct s_PTSECTOR
{
	AArray<int, int>	aPlaneRefs;		//	Sector bounding plane indices
	A3DVECTOR3			vMins;			//	Bounding box of sector
	A3DVECTOR3			vMaxs;
	AArray<int, int>	aChilds;		//	Child sectors
	AArray<int, int>	aDrawSurfs;		//	Draw surfaces

	AArray<s_PTPORTAL*, s_PTPORTAL*>			aPortals;	//	Portals
	AArray<s_PTSECTORPART*, s_PTSECTORPART*>	aParts;		//	Parts
};

struct s_PTSTATISTICS
{
	int		iNumBadPlanes;		//	Number of bad planes
};

namespace PORTAL
{
	extern ALog				g_Log;				//	Log tool
	extern s_PTSTATISTICS	g_Statistics;		//	Statistics

	extern A3DVECTOR3*		g_aSurfVerts;		//	Surface vertices
	extern int*				g_aSurfIndices;		//	Surface indices
	extern s_DRAWSURF*		g_aDrawSurfs;		//	Draw surfaces
	extern int				g_iNumDrawSurf;		//	Number of draw surfaces

	extern AArray<s_PTPLANE, s_PTPLANE&>	g_aPlanes;		//	Planes
	extern AArray<s_PTSECTOR*, s_PTSECTOR*>	g_aSectors;		//	Sectors
	extern AArray<s_PTPORTAL*, s_PTPORTAL*> g_aPortals;		//	Portals
}

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////


#endif	//	_PT_INTERNAL_H_

