      /*
 * FILE: ModelShadow.cpp
 *
 * DESCRIPTION: Shadow map for demo
 *
 * CREATED BY: Hedi, 2004/4/19
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Render.h"
#include "ModelShadow.h"

A3DRenderTarget *		g_pShadowTarget = NULL;
A3DViewport *			g_pShadowViewport = NULL;
A3DCamera *				g_pShadowCamera = NULL;

A3DVECTOR3				g_vecLightDir;

#define SHADOW_SIZE		512

bool Shadow_Init(const A3DVECTOR3& vecLightDir)
{
	g_vecLightDir = Normalize(A3DVECTOR3(0.5f, -0.707f, 0.707f));//vecLightDir;

	//	Create camera
	if (!(g_pShadowCamera = new A3DCamera))
	{
		g_Log.Log("Shadow_Init:: Create camera fail");
		return false;
	}

	if (!g_pShadowCamera->Init(g_Render.GetA3DDevice(), DEG2RAD(1.6f), 0.3f, 1000.0f, 1.0f))
	{
		g_Log.Log("Sahdow_Init:: Camera Init fail");
		return false;
	}
						    
	g_pShadowCamera->SetPos(g_vOrigin);
	g_pShadowCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);

	//	Create main Viewport
	RECT rc;
	rc.left = 0;
	rc.right = SHADOW_SIZE;
	rc.top = 0;
	rc.bottom = SHADOW_SIZE;
	
	if (!g_Render.GetA3DDevice()->CreateViewport(&g_pShadowViewport, 0, 0, rc.right, rc.bottom,
									0.0f, 1.0f, true, true, 0x00000000))
	{
		g_Log.Log("Shadow_Init::Create viewport fail");
		return false;
	}

	g_pShadowViewport->SetCamera(g_pShadowCamera);

	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= SHADOW_SIZE;
	devFmt.nHeight		= SHADOW_SIZE;
	devFmt.fmtTarget	= A3DFMT_A8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	g_pShadowTarget = new A3DRenderTarget();
	if( !g_pShadowTarget->Init(g_Render.GetA3DDevice(), devFmt, true, true) )
		return false;
	
	return true;
}

bool Shadow_Release()
{		  
	if( g_pShadowViewport )
	{
		g_pShadowViewport->Release();
		delete g_pShadowViewport;
		g_pShadowViewport = NULL;
	}

	if( g_pShadowCamera )
	{
		g_pShadowCamera->Release();
		delete g_pShadowCamera;
		g_pShadowCamera = NULL;
	}

	if( g_pShadowTarget )
	{
		g_pShadowTarget->Release();
		delete g_pShadowTarget;
		g_pShadowTarget = NULL;
	}

	return true;
}

A3DVECTOR3 GetIntersect(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecRay)
{
	float t = -vecStart.y / vecRay.y;
	return vecStart + vecRay * t;
}

bool Shadow_Render(A3DSkinModel * pModel, A3DViewport * pViewport, float vDis, float vFov, const char * szFrameName)
{
	g_Render.GetA3DDevice()->EndRender();
	g_Render.GetA3DDevice()->BeginRender();

	g_Render.GetA3DDevice()->SetRenderTarget(g_pShadowTarget);

	int index;
	A3DBone * pBone = pModel->GetSkeleton()->GetBone(szFrameName, &index);
	A3DMATRIX4 mat = IdentityMatrix();
	if( pBone )
		mat = pBone->GetAbsoluteTM();
	
	g_pShadowCamera->SetFOV(vFov);
	g_pShadowCamera->SetFOVSet(vFov);

	g_pShadowCamera->SetPos(mat.GetRow(3) - g_vecLightDir * vDis);//- g_vecLightDir * 5.0f);
	g_pShadowCamera->SetDirAndUp(g_vecLightDir, g_vAxisY);

	g_pShadowViewport->Active();
	g_pShadowViewport->ClearDevice();
	
	pModel->Render(g_pShadowViewport, false);

	g_Render.GetA3DDevice()->SetZWriteEnable(false);
	g_Render.GetA3DDevice()->SetLightingEnable(false);
	g_Render.GetA3DEngine()->RenderSkinModels(g_pShadowViewport);
	g_Render.GetA3DEngine()->GetA3DSkinMan()->ResetRenderInfo();
	g_Render.GetA3DDevice()->SetLightingEnable(true);
	g_Render.GetA3DDevice()->EndRender();

	g_Render.GetA3DDevice()->RestoreRenderTarget();
	pViewport->Active();

	g_Render.GetA3DDevice()->BeginRender();

	A3DDevice * pDevice = g_Render.GetA3DDevice();
	
	A3DVECTOR3 vecRay0, vecRay1, vecRay2, vecRay3;
	A3DVECTOR3 vecScreen;
	vecScreen.x = 0;
	vecScreen.y = 0;
	vecScreen.z = 1.0f;
	g_pShadowViewport->InvTransform(vecScreen, vecRay0);
	vecRay0 = Normalize(vecRay0 - g_pShadowCamera->GetPos());

	vecScreen.x = SHADOW_SIZE;
	vecScreen.y = 0;
	vecScreen.z = 1.0f;
	g_pShadowViewport->InvTransform(vecScreen, vecRay1);
	vecRay1 = Normalize(vecRay1 - g_pShadowCamera->GetPos());

	vecScreen.x = 0;
	vecScreen.y = SHADOW_SIZE;
	vecScreen.z = 1.0f;
	g_pShadowViewport->InvTransform(vecScreen, vecRay2);
	vecRay2 = Normalize(vecRay2 - g_pShadowCamera->GetPos());

	vecScreen.x = SHADOW_SIZE;
	vecScreen.y = SHADOW_SIZE;
	vecScreen.z = 1.0f;
	g_pShadowViewport->InvTransform(vecScreen, vecRay3);
	vecRay3 = Normalize(vecRay3 - g_pShadowCamera->GetPos());

	struct A3DPLVERTEX 
	{
		FLOAT x, y, z;
		DWORD diffuse;
		DWORD specular;
		FLOAT tu, tv, trhw;

		A3DPLVERTEX() {}
		A3DPLVERTEX(const A3DVECTOR3& _vecPos, A3DCOLOR _diffuse, A3DCOLOR _specular, FLOAT _tu, FLOAT _tv, FLOAT _trhw) 
		{ 
			x = _vecPos.x; y = _vecPos.y; z = _vecPos.z; diffuse = _diffuse; specular = _specular; tu = _tu; tv = _tv; trhw = _trhw;
		}

	};

#define A3DFVF_A3DPLVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0))
                                         
	A3DPLVERTEX verts[4];
	static WORD indices[6] = {0, 1, 2, 2, 1, 3};

	A3DCOLOR colorShadow = A3DCOLORRGBA(0, 0, 0, 32);
	A3DVECTOR3 vecPos;

	A3DMATRIX4	matShadowView = g_pShadowCamera->GetVPTM();
	A3DVECTOR4	vecPos4;
	A3DVECTOR4	vecScreen4;
	float		rhw;

	vecPos = GetIntersect(g_pShadowCamera->GetPos(), vecRay0) + A3DVECTOR3(0.0f, 0.02f, 0.0f);
	vecPos4 = vecPos;
	vecScreen4 = vecPos4 * matShadowView;
	rhw = vecScreen4.w;
	verts[0] = A3DPLVERTEX(vecPos, colorShadow, A3DCOLORRGB(0, 0, 0), 0.0f * rhw, 0.0f * rhw, rhw);

	vecPos = GetIntersect(g_pShadowCamera->GetPos(), vecRay1) + A3DVECTOR3(0.0f, 0.02f, 0.0f);
	vecPos4 = vecPos;
	vecScreen4 = vecPos4 * matShadowView;
	rhw = vecScreen4.w;
	verts[1] = A3DPLVERTEX(vecPos, colorShadow, A3DCOLORRGB(0, 0, 0), 1.0f * rhw, 0.0f * rhw, rhw);

	vecPos = GetIntersect(g_pShadowCamera->GetPos(), vecRay2) + A3DVECTOR3(0.0f, 0.02f, 0.0f);
	vecPos4 = vecPos;
	vecScreen4 = vecPos4 * matShadowView;
	rhw = vecScreen4.w;
	verts[2] = A3DPLVERTEX(vecPos, colorShadow, A3DCOLORRGB(0, 0, 0), 0.0f * rhw, 1.0f * rhw, rhw);

	vecPos = GetIntersect(g_pShadowCamera->GetPos(), vecRay3) + A3DVECTOR3(0.0f, 0.02f, 0.0f);
	vecPos4 = vecPos;
	vecScreen4 = vecPos4 * matShadowView;
	rhw = vecScreen4.w;
	verts[3] = A3DPLVERTEX(vecPos, colorShadow, A3DCOLORRGB(0, 0, 0), 1.0f * rhw, 1.0f * rhw, rhw);
	
	pDevice->GetD3DDevice()->SetTexture(0, g_pShadowTarget->GetTargetTexture());

	pDevice->SetWorldMatrix(IdentityMatrix());
	pDevice->SetLightingEnable(FALSE);
	pDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DPLVERTEX);
	pDevice->SetFaceCull(A3DCULL_NONE);
	pDevice->SetZWriteEnable(false);
	pDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	pDevice->SetTextureColorOP(0, A3DTOP_SELECTARG2);
	pDevice->SetTextureMatrix(0, IdentityMatrix());
	pDevice->SetTextureTransformFlags(0, A3DTEXTURETRANSFLAGS(A3DTTFF_PROJECTED | A3DTTFF_COUNT3));
	
	pDevice->SetWorldMatrix(IdentityMatrix());
	pDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				4, 2, indices, A3DFMT_INDEX16, 
				verts, sizeof(A3DPLVERTEX));
	vDis = Magnitude(A3DVECTOR3(verts[0].x, verts[0].y, verts[0].z) - A3DVECTOR3(verts[1].x, verts[1].y, verts[1].z));
	const A3DVECTOR3 offsets0[4] = {
		A3DVECTOR3(0.0f, 0.0f, -1.0f),
		A3DVECTOR3(-1.0f, 0.0f, 0.0f),
		A3DVECTOR3(1.0f, 0.0f, 0.0f),
		A3DVECTOR3(0.0f, 0.0f, 1.0f)
	};

	const A3DVECTOR3 offsets1[4] = {
		A3DVECTOR3(-1.0f, 0.0f, -1.0f),
		A3DVECTOR3(-1.0f, 0.0f, 1.0f),
		A3DVECTOR3(1.0f, 0.0f, -1.0f),
		A3DVECTOR3(1.0f, 0.0f, 1.0f)
	};

	A3DCOLOR colorLight0 = A3DCOLORRGBA(0, 0, 0, 64);
	verts[0].diffuse = colorLight0;
	verts[1].diffuse = colorLight0;
	verts[2].diffuse = colorLight0;
	verts[3].diffuse = colorLight0;

	pDevice->SetWorldMatrix(IdentityMatrix());
	pDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				4, 2, indices, A3DFMT_INDEX16, 
				verts, sizeof(A3DPLVERTEX));
	
	/*
	int i;
	A3DCOLOR colorLight1 = A3DCOLORRGBA(0, 0, 0, 10);
	verts[0].diffuse = colorLight1;
	verts[1].diffuse = colorLight1;
	verts[2].diffuse = colorLight1;
	verts[3].diffuse = colorLight1;
	for(i=0; i<4; i++)
	{
		A3DVECTOR3 vecDelta = offsets1[i] * (vDis * 1.0f / SHADOW_SIZE);
		pDevice->SetWorldMatrix(Translate(vecDelta.x, vecDelta.y, vecDelta.z));
		pDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
					4, 2, indices, A3DFMT_INDEX16, 
					verts, sizeof(A3DPLVERTEX));
	}
	*/

	pDevice->SetFaceCull(A3DCULL_CCW);
	pDevice->SetLightingEnable(true);
	pDevice->SetZWriteEnable(true);
	pDevice->SetZTestEnable(true);
	pDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	pDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);

	pDevice->GetD3DDevice()->SetTexture(0, NULL);
	return true;
}

