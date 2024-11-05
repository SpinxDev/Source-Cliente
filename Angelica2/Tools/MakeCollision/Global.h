/*
 * FILE: Global.h
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2003/8/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stdafx.h"
#include "Resource.h"

#include "AMemory.h"
#include "AArray.h"

#include "ESP_Types.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define MAXSIZE_WORLD		100000

//	Progress segments
#define PROG_BUILDSIDES		0	//	Load scene and build sides (brushes)

#define ESPPROG_FILTERSIDES	1	//	ESP: Filter sides to leaves
#define ESPPROG_WRITETOFILE	2	//	ESP: Writer to file
#define NUM_ESPPROG			3	//	ESP progress number

#define KDTPROG_BUILDTREE	1	//	KDT: Build KDT tree
#define KDTPROG_WRITETOFILE	2	//	KDT: Writer to file
#define NUM_KDTPROG			3	//	KDT progress number

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	Editing config
struct s_CONFIGS
{
	//	Common configs
	bool	bMergeSide;			//	true, merge sides
	bool	bNoBrush;			//	true, don't create brushes
	float	fBrushThick;		//	Brush thick

	//	ESP configs
	int		iESPLeafSize;		//	Leaf size

	//	KDT configs
	float	fKDTMinLeafSize;	//	Minimal leaf size
	int		iKDTMinSideNum;		//	Minimal side number in a leaf
};

extern s_CONFIGS	g_Configs;

extern AArray<s_PLANE, s_PLANE&>	g_aPlanes;		//	Planes
extern AArray<s_SIDE*, s_SIDE*>		g_aSides;		//	Planes
extern AArray<s_BRUSH*, s_BRUSH*>	g_aBrushes;		//	Brushes

extern s_ESPWORLD	g_ESPWorld;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

void AddMessageToList(const char* szMsg, ...);
void UpdateLastMessage(const char* szMsg, ...);
void SetCurProgressSeg(int iSeg);
void StepProgressBar(int iCur, int iTotal);
void StripFileTail(char* szName);

#endif	//	_GLOBAL_H_

