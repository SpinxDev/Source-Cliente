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
#include "PT_Interface.h"

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
	char		szName[PTNAME_LENGTH];	//	Name
	
	int			iPlane;		//	Plane's index
	int			iFront;		//	Sector this portal face to
	int			iBack;		//	Sector this portal back to
	s_PTPORTAL*	pOpposite;	//	Opposite portal of this portal
	s_WINDING*	pWinding;	//	Winding
};

struct s_PTSECTORSIDE
{
	s_WINDING*	pWinding;	//	Winding
	int			iPlane;		//	Plane this side is on
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
	AArray<s_PTSECTORSIDE*, s_PTSECTORSIDE*>	aSides;		//	Sector sides
};

struct s_PTSTATISTICS
{
	int		iNumBadPlanes;		//	Number of bad planes
};

struct s_PTBSPNODE
{
	s_PTSECTOR*		pSector;
	int				iPlane;			//	Split plane
	s_PTBSPNODE*	pParent;		//	Parent node
	s_PTBSPNODE*	aChildren[2];	//	Children nodes
};

namespace PORTAL
{
	extern ALog				g_Log;				//	Log tool
	extern s_PTSTATISTICS	g_Statistics;		//	Statistics

	extern A3DVECTOR3*		g_aSurfVerts;		//	Surface vertices
	extern int*				g_aSurfIndices;		//	Surface indices
	extern s_DRAWSURF*		g_aDrawSurfs;		//	Draw surfaces
	extern int				g_iNumDrawSurf;		//	Number of draw surfaces

	extern PTBUILDPARAMS	g_BuildParams;		//	Building parameters

	extern AArray<s_PTPLANE, s_PTPLANE&>		g_aPlanes;		//	Planes
	extern AArray<s_PTSECTOR*, s_PTSECTOR*>		g_aSectors;		//	Sectors
	extern AArray<s_PTPORTAL*, s_PTPORTAL*>		g_aPortals;		//	Portals
	extern AArray<s_PTBSPNODE*, s_PTBSPNODE*>	g_aBSPNodes;	//	BSP nodes
}

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////


#endif	//	_PT_INTERNAL_H_

