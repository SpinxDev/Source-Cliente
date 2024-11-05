/*
 * FILE: Global.cpp
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2002/6/30
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

ALog		g_Log;
EDITCONFIG	g_Configs;
char		g_szAppPath[MAX_PATH];

//	Color table
DWORD g_aColTable[LEN_COLORTABLE] = 
{
	0xff606060,		//	COL_SPLITBAR0,		Split bar
	0xffffffff,		//	COL_VIEWPORTBG,		Viewport background
	0xffc0c080,		//	COL_NORMALFRAME,	Normal wire frame
	0xffd0d0d0,		//	COL_GRID,			Grid color
	0xffff8040,		//	COL_VIEWFRAME,		Active viewport frame
	0xff000000,		//	COL_COORDINATES,	Coordinates
	0xff00e000,		//	COL_SECTOR,			Normal sector
	0xff000080,		//	COL_PORTAL,			Normal portal
	0xffff0000,		//	COL_SELSECTOR,		Selected sector
	0xffff0000,		//	COL_SELPORTAL,		Selected portal
	0xffff8000,		//	COL_LINKSECTOR,		Linked sector
	0xff008000,		//	COL_HALFLINKSECTOR,	Half-linked sector
	0xff000000,		//	COL_BOUNDBOX,		Bound box
	0xff0000ff,		//	COL_SELECTAREA,		Rectangle used to select object
	0xff0080ff,		//	COL_EYEPOS,			Eye position
};

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

