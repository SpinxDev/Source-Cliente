/* 
 * FILE: ESP_Types.h
 *
 * DESCRIPTION: ESP types and structures
 *
 * CREATED BY: Duyuxin, 2003/8/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_ESP_TYPES_H_
#define _ESP_TYPES_H_

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

//	Struct for leaf
struct s_ESPLEAF
{
	float		aDists[6];		//	Parameter d of planes surround this leaf
	s_SIDEREF*	pSideRef;		//	Side reference
};

//	ESP world infomation
struct s_ESPWORLD
{
	float		vMins[3];		//	Bounding box
	float		vMaxs[3];
	float		vAlignMins[3];	//	Align bounding box
	float		vAlignMaxs[3];

	s_ESPLEAF*	aLeaves;		//	Leaves
	int			vLength[3];		//	Length of each border of leaf
	int			vSize[3];		//	Size of cluster in leaves
};

/////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
/////////////////////////////////////////////////////////////////////////


#endif	//	_ESP_TYPES_H_


