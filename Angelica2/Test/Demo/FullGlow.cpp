/*
 * FILE: FullGlow.cpp
 *
 * DESCRIPTION: Fullscreen glow for the demo
 *
 * CREATED BY: Hedi, 2004/5/28
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Render.h"
#include "FullGlow.h"

A3DRenderTarget *		g_pGlowTarget = NULL;
A3DRenderTarget *		g_pSmallTarget = NULL;

#define SMALL_WIDTH		(RENDER_WIDTH/2)
#define SMALL_HEIGHT	(RENDER_HEIGHT/2)

bool FullGlow_Init()
{
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= RENDER_WIDTH;
	devFmt.nHeight		= RENDER_HEIGHT;
	devFmt.fmtTarget	= A3DFMT_A8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	g_pGlowTarget = new A3DRenderTarget();
	if( !g_pGlowTarget->Init(g_Render.GetA3DDevice(), devFmt, true, false) )
		return false;

	devFmt.bWindowed	= true;
	devFmt.nWidth		= SMALL_WIDTH;
	devFmt.nHeight		= SMALL_HEIGHT;
	devFmt.fmtTarget	= A3DFMT_A8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	g_pSmallTarget = new A3DRenderTarget();
	if( !g_pSmallTarget->Init(g_Render.GetA3DDevice(), devFmt, true, false) )
		return false;
	
	return true;
}

bool FullGlow_Release()
{		  
	if( g_pGlowTarget )
	{
		g_pGlowTarget->Release();
		delete g_pGlowTarget;
		g_pGlowTarget = NULL;
	}

	if( g_pSmallTarget )
	{
		g_pSmallTarget->Release();
		delete g_pSmallTarget;
		g_pSmallTarget = NULL;
	}

	return true;
}

bool FullGlow_BeginRender()
{
	g_Render.GetA3DDevice()->EndRender();
	g_Render.GetA3DDevice()->BeginRender();
	g_Render.GetA3DDevice()->SetRenderTarget(g_pGlowTarget);
	return true;
}

bool FullGlow_EndRender()
{
	g_Render.GetA3DDevice()->EndRender();
	g_Render.GetA3DDevice()->BeginRender();

	g_Render.GetA3DDevice()->RestoreRenderTarget();
	return true;
}

bool FullGlow_GlowPresent()
{
	g_Render.GetA3DDevice()->EndRender();
	g_Render.GetA3DDevice()->BeginRender();
	g_Render.GetA3DDevice()->SetRenderTarget(g_pSmallTarget);

	// first sample down into a small texture
	A3DTLVERTEX verts[4];
	static WORD indices[6] = {0, 1, 2, 2, 1, 3};
	static int glowlevel = 60;

	A3DCOLOR colorGlow = A3DCOLORRGBA(255, 255, 255, 255);
	A3DVECTOR3 vecPos;
	verts[0] = A3DTLVERTEX(A3DVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 0.0f, 0.0f);
	verts[1] = A3DTLVERTEX(A3DVECTOR4(SMALL_WIDTH, 0.0f, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 1.0f, 0.0f);
	verts[2] = A3DTLVERTEX(A3DVECTOR4(0.0f, SMALL_HEIGHT, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 0.0f, 1.0f);
	verts[3] = A3DTLVERTEX(A3DVECTOR4(SMALL_WIDTH, SMALL_HEIGHT, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 1.0f, 1.0f);
	
	A3DDevice * pDevice = g_Render.GetA3DDevice();

	pDevice->GetD3DDevice()->SetTexture(0, g_pGlowTarget->GetTargetTexture());
	pDevice->SetLightingEnable(false);
	pDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DTLVERTEX);
	pDevice->SetFaceCull(A3DCULL_NONE);
	pDevice->SetZWriteEnable(false);
	pDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				4, 2, indices, A3DFMT_INDEX16, 
				verts, sizeof(A3DTLVERTEX));
	pDevice->GetD3DDevice()->SetTexture(0, NULL);

	pDevice->RestoreRenderTarget();

	pDevice->GetD3DDevice()->SetTexture(0, g_pGlowTarget->GetTargetTexture());
	
	// then draw background layer
	colorGlow = A3DCOLORRGBA(200, 200, 200, 255);
	verts[0] = A3DTLVERTEX(A3DVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 0.0f, 0.0f);
	verts[1] = A3DTLVERTEX(A3DVECTOR4(RENDER_WIDTH, 0.0f, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 1.0f, 0.0f);
	verts[2] = A3DTLVERTEX(A3DVECTOR4(0.0f, RENDER_HEIGHT, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 0.0f, 1.0f);
	verts[3] = A3DTLVERTEX(A3DVECTOR4(RENDER_WIDTH, RENDER_HEIGHT, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 1.0f, 1.0f);

	pDevice->SetTextureFilterType(0, A3DTEXF_POINT);
	pDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				4, 2, indices, A3DFMT_INDEX16, 
				verts, sizeof(A3DTLVERTEX));
	pDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	
	if( GetAsyncKeyState('O') & 0x8000 )
	{
		glowlevel ++;
		if( glowlevel > 255 )
			glowlevel = 255;
	}
	if( GetAsyncKeyState('L') & 0x8000 )
	{
		glowlevel --;
		if( glowlevel < 0 )
			glowlevel = 0;
	}
	// last draw glow layer
	pDevice->GetD3DDevice()->SetTexture(0, g_pSmallTarget->GetTargetTexture());
	colorGlow = A3DCOLORRGBA(255, 255, 255, glowlevel);
	verts[0] = A3DTLVERTEX(A3DVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 0.0f, 0.0f);
	verts[1] = A3DTLVERTEX(A3DVECTOR4(RENDER_WIDTH, 0.0f, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 1.0f, 0.0f);
	verts[2] = A3DTLVERTEX(A3DVECTOR4(0.0f, RENDER_HEIGHT, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 0.0f, 1.0f);
	verts[3] = A3DTLVERTEX(A3DVECTOR4(RENDER_WIDTH, RENDER_HEIGHT, 0.0f, 1.0f), colorGlow, A3DCOLORRGB(0, 0, 0), 1.0f, 1.0f);
	pDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pDevice->SetDestAlpha(A3DBLEND_ONE);

	pDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				4, 2, indices, A3DFMT_INDEX16, 
				verts, sizeof(A3DTLVERTEX));
	
	pDevice->GetD3DDevice()->SetTexture(0, NULL);
	pDevice->SetFaceCull(A3DCULL_CCW);
	pDevice->SetLightingEnable(true);
	pDevice->SetZWriteEnable(true);
	pDevice->SetZTestEnable(true);
	
	pDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	return true;
}
