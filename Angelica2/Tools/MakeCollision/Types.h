/* 
 * FILE: Types.h
 *
 * DESCRIPTION: Types and structures
 *
 * CREATED BY: Duyuxin, 2003/8/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_TYPES_H_
#define _TYPES_H_

#include "A3DTypes.h"

/////////////////////////////////////////////////////////////////////////
//
//	Constants and Macros
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
/////////////////////////////////////////////////////////////////////////

struct s_WINDING
{
	int			iNumPts;		//	Number of points
	A3DVECTOR3*	pVerts;			//	Vertices
};

//	Side data
struct s_SIDE
{
	int			iPlane;			//	Plane number
	s_WINDING*	pWinding;		//	Vertice position
	A3DMESHPROP	MeshProps;		//	Mesh properties
	int			iBrush;			//	Brush bind with this side
};

struct s_SIDEREF
{
	int			iSide;			//	Side
	s_SIDEREF*	pNext;			//	Next side reference
};

struct s_PLANE
{
	A3DVECTOR3	vNormal;		//	Normal
	float		fDist;			//	Distance
};

struct s_BRUSH
{
	int*		aPlanes;		//	Planes array
	int			iNumPlane;		//	Number of plane
};

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////


#endif	//	_TYPES_H_


