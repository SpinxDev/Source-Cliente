/*
 * FILE: Global.h
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2003/6/30
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stdafx.h"
#include "Resource.h"

#include "ALog.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define MSG_ENDOPERATION	WM_USER+100		//	End current operation

#define MIN_GRID			0.05f			//	Minimum grid of sector and portal
#define PICK_EPSILON		5				//	Pick epsilon (in pixels)
#define WORLD_BOUND			3000.0f

//	Sector editor file flag and version
#define SEDFILE_FLAGS		(('S'<<24) | ('E'<<16) | ('D'<<8) | '0')
#define SEDFILE_VERSION		2

//	Color table references
enum
{
	COL_SPLITBAR = 0,		//	Split bar
	COL_VIEWPORTBG,			//	Viewport background
	COL_NORMALFRAME,		//	Normal wire frame
	COL_GRID,				//	Grid color
	COL_VIEWFRAME,			//	Active viewport frame
	COL_COORDINATES,		//	Coordinates
	COL_SECTOR,				//	Normal sector
	COL_PORTAL,				//	Normal portal
	COL_SELSECTOR,			//	Selected sector
	COL_SELPORTAL,			//	Selected portal
	COL_LINKSECTOR,			//	Linked sector
	COL_HALFLINKSECTOR,		//	Half-linked sector
	COL_BOUNDBOX,			//	Bound box
	COL_SELECTAREA,			//	Rectangle used to select object
	COL_EYEPOS,				//	Eye position
	LEN_COLORTABLE,			//	Length of color table
};

//	Side identification
enum
{
	SIDE_NONE = 0,
	SIDE_LEFT,
	SIDE_TOP,
	SIDE_RIGHT,
	SIDE_BOTTOM,
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	Editing config
struct EDITCONFIG
{
	bool	bLightmap;		//	Open/Close light map
	bool	bShowGrid;		//	Show/Hide grid
	bool	bShowScene;		//	Show/Hide scene
	bool	bShowSector;	//	Show/Hide sector
	bool	bShowPortal;	//	Show/Hide portal
	bool	bFullClip;		//	Full clip flag
	bool	bForceRender;	//	Force render flag
	bool	bTesting;		//	Testing flag
	int		iCameraMove;	//	Camera move speed
	int		iCameraRotate;	//	Camera rotate speed
	int		iNumWarnFace;	//	Warning face number
};

extern DWORD		g_aColTable[];	//	Color table
extern ALog			g_Log;
extern EDITCONFIG	g_Configs;
extern char			g_szAppPath[];

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


#endif	//	_GLOBAL_H_



