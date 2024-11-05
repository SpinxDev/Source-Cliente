/*
 * FILE: Terrain.cpp
 *
 * DESCRIPTION: test font
 *
 * CREATED BY: Hedi, 2004/4/17
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Render.h"
#include "terrain.h"

A3DTerrain* g_pA3DTerrain = NULL;			//	Terrain object

//	Load terrain
bool LoadTerrain()
{
	g_pA3DTerrain = new A3DTerrain();

	TERRAINPARAM param;
	memset(&param, 0, sizeof(param));
	param.nHeight = 256;
	param.nWidth = 256;
	param.nNumTexture = 3;
	param.vScale = 3.0f;
	param.vCellSize = 15.0f;
	param.nTextureCover = 8;
	param.vOffset = -0.05f;
		
	g_pA3DTerrain->SetDirectionalLight(g_Render.GetDirLight());
	if( !g_pA3DTerrain->Init(g_Render.GetA3DDevice(), &param, "demo", "Terrain copy.raw", "Texture copy.raw") )
		return false;

	g_pA3DTerrain->SetCamera(g_Render.GetA3DCamera());
	return true;
}

//	Release terrain
bool ReleaseTerrain()
{
	if( g_pA3DTerrain )
	{
		g_pA3DTerrain->Release();
		delete g_pA3DTerrain;
		g_pA3DTerrain = NULL;
	}

	return true;
}

//	Render terrain
bool RenderTerrain()
{
	if( g_pA3DTerrain )
	{
		g_Render.GetA3DDevice()->SetFogEnable(false);
		g_pA3DTerrain->Render(g_Render.GetA3DViewport());
		g_Render.GetA3DDevice()->SetFogEnable(true);
	}

	return true;
}

