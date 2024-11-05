/*
 * FILE: Terrain.h
 *
 * DESCRIPTION: terrain object
 *
 * CREATED BY: Hedi, 2004/4/17
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "Global.h"
#include "Render.h"

extern A3DTerrain*		g_pA3DTerrain;			//	Terrain object

//	Load terrain
bool LoadTerrain();
//	Release terrain
bool ReleaseTerrain();
//	Render terrain
bool RenderTerrain();

#endif//_TERRAIN_H_
