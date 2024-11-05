  /*
 * FILE: Trail.cpp
 *
 * DESCRIPTION: Trail for knife and something like that
 *
 * CREATED BY: Hedi, 2004/4/17
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Render.h"
#include "trail.h"

#define SEG_NUM			128

A3DVECTOR3		g_vecSeg1[SEG_NUM];
A3DVECTOR3		g_vecSeg2[SEG_NUM];
int				g_nSegLife[SEG_NUM];
int				g_nSegBrightness[SEG_NUM];
int				g_nSegTail = 0;
int				g_nSegCount = 0;

A3DLVERTEX		g_verts[SEG_NUM * 2];
WORD			g_indices[(SEG_NUM - 1) * 2 * 3];

A3DTexture *	g_pSegTex = NULL;

A3DVECTOR3		g_vecPt1;
A3DVECTOR3		g_vecPt2;
A3DVECTOR3		g_vecLastCenter;
A3DQUATERNION	g_quLastOrient;

bool Trail_Init(const char * szSegTex, A3DVECTOR3 vecPt1, A3DVECTOR3 vecPt2)
{
	g_vecPt1 = vecPt1;
	g_vecPt2 = vecPt2;

	if( !g_Render.GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szSegTex, &g_pSegTex) )
		return false;

	g_nSegCount = 0;

	// now setup the indices buffer
	for(int i=0; i<SEG_NUM-1; i++)
	{
		g_indices[i * 6]	 = i * 2;
		g_indices[i * 6 + 1] = i * 2 + 1;
		g_indices[i * 6 + 2] = i * 2 + 3;

		g_indices[i * 6 + 3] = i * 2;
		g_indices[i * 6 + 4] = i * 2 + 3;
		g_indices[i * 6 + 5] = i * 2 + 2;
	}

	return true;
}

bool Trail_Release()
{
	if( g_pSegTex )
	{
		g_pSegTex->Release();
		g_pSegTex = NULL;
	}

	return true;
}

bool Trail_Add(const A3DMATRIX4& matNow)
{
	int nNum;

	A3DQUATERNION	quNow;
	quNow.ConvertFromMatrix(matNow);
	A3DVECTOR3		vecNow = matNow.GetRow(3);

	float vDot = DotProduct(g_quLastOrient, quNow);
	if( vDot < 0.0f )
		vDot = -vDot;
	float vAngle = (float) acos(vDot);

	nNum = int(vAngle / DEG2RAD(5.0f));
	if( nNum > 8 )
		nNum = 8;
	
	int nBrightness = int(nNum * 32 + (Magnitude(g_vecLastCenter - vecNow) / 0.01f));
	if( nBrightness > 200 ) 
		nBrightness = 200;

	for(int i=0; i<nNum; i++)
	{
		float t = (i + 1) * 1.0f / nNum;
		A3DVECTOR3 vecThis = g_vecLastCenter * (1.0f - t) + vecNow * t;
		A3DQUATERNION quThis = SLERPQuad(g_quLastOrient, quNow, t);
		A3DMATRIX4 matThis;
		quThis.ConvertToMatrix(matThis);
		matThis = matThis * a3d_Translate(vecThis.x, vecThis.y, vecThis.z);

		A3DVECTOR3 vec1;
		A3DVECTOR3 vec2;

		vec1 = g_vecPt1 * matThis;
		vec2 = g_vecPt2 * matThis;

		int idSeg = g_nSegTail;

		g_vecSeg1[idSeg] = vec1;
		g_vecSeg2[idSeg] = vec2;
		g_nSegLife[idSeg] = MAX_LIFE;	// millisecond
		g_nSegBrightness[idSeg] = nBrightness;

		if( g_nSegCount < SEG_NUM )
			g_nSegCount ++;

		g_nSegTail ++;
		if( g_nSegTail >= SEG_NUM )
			g_nSegTail -= SEG_NUM;
	}

	g_quLastOrient = quNow;
	g_vecLastCenter = vecNow;
	return true;
}

bool Trail_Tick(int nDeltaTime)
{
	int idSeg = g_nSegTail - 1;
	if( idSeg < 0 )
			idSeg += SEG_NUM;
	
	int nCount = 0;
	for(int i=0; i<g_nSegCount; i++)
	{
		g_nSegLife[idSeg] -= nDeltaTime;
		if( g_nSegLife[idSeg] <= 0 )
			break;

		idSeg --;
		if( idSeg < 0 )
			idSeg += SEG_NUM;

		nCount ++;
	}

	g_nSegCount = nCount;
	return true;
}

bool Trail_Render(A3DViewport * pCurrentViewport)
{
	if( g_nSegCount <= 1 )
		return true;

	int idSeg = g_nSegTail - g_nSegCount;
	int idQuad = 0;

	if( idSeg < 0 )
		idSeg += SEG_NUM;

	for(int i=0; i<g_nSegCount; i++)
	{
		int nAlpha = g_nSegLife[idSeg] * g_nSegBrightness[idSeg] / MAX_LIFE;
		g_verts[idQuad * 2] = A3DLVERTEX(g_vecSeg1[idSeg], A3DCOLORRGBA(255, 255, 255, nAlpha), A3DCOLORRGBA(0, 0, 0, 255), 0.0f, g_nSegLife[idSeg] * 1.0f / MAX_LIFE);	
		g_verts[idQuad * 2 + 1] = A3DLVERTEX(g_vecSeg2[idSeg], A3DCOLORRGBA(255, 255, 255, nAlpha), A3DCOLORRGBA(0, 0, 0, 255), 1.0f, g_nSegLife[idSeg] * 1.0f / MAX_LIFE);

		idSeg ++;
		if( idSeg >= SEG_NUM )
			idSeg -= SEG_NUM;

		idQuad ++;
	}

	A3DDevice * pDevice = g_Render.GetA3DDevice();
	
	g_pSegTex->Appear(0);

	pDevice->SetWorldMatrix(IdentityMatrix());
	pDevice->SetLightingEnable(false);
	pDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DLVERTEX);
	pDevice->SetFaceCull(A3DCULL_NONE);
	pDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	pDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	pDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pDevice->SetDestAlpha(A3DBLEND_ONE);
	pDevice->SetZWriteEnable(false);
								   
	pDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				g_nSegCount * 2, (g_nSegCount - 1) * 2, g_indices, A3DFMT_INDEX16, 
				g_verts, sizeof(A3DLVERTEX));

	pDevice->SetFaceCull(A3DCULL_CCW);
	pDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	pDevice->SetLightingEnable(true);
	pDevice->SetZWriteEnable(true);
	pDevice->SetZTestEnable(true);

	g_pSegTex->Disappear(0);
	return true;
}