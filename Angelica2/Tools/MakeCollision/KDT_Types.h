/* 
 * FILE: KDT_Types.h
 *
 * DESCRIPTION: KD-Tree types and structures
 *
 * CREATED BY: Duyuxin, 2003/8/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_KDT_TYPES_H_
#define _KDT_TYPES_H_

#include "Types.h"

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

//	BSP node and leaf
struct s_KDTNODE
{
	int			iPlane;			//	Split plane
	s_KDTNODE*	pParent;		//	Parent node
	s_KDTNODE*	aChildren[2];	//	Children nodes
	s_SIDEREF*	pSideRef;		//	Side reference
	A3DVECTOR3	vMins;			//	Bounding box of node
	A3DVECTOR3	vMaxs;
};

//	BSP side
struct s_KDTSIDE
{
	int			iSide;			//	Side index
	s_WINDING*	pWinding;		//	Winding vertices
	s_KDTSIDE*	pNext;	
};

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////


#endif	//	_KDT_TYPES_H_


