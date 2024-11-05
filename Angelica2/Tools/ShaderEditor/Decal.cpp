/*
 * FILE: Decal.cpp
 *
 * DESCRIPTION: A simple 2D decal using shader
 *
 * CREATED BY: duyuxin, 2002/2/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "CommonDef.h"
#include "Decal.h"
#include "AC.h"
#include "A3D.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

struct DECALVERTEX 
{
	float		x, y, z;
	A3DCOLOR	diffuse;
	A3DCOLOR	specular;
	float		u0, v0;
	float		u1, v1;
};

#define FVF_DECALVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)

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

CDecal::CDecal()
{
	m_pA3DDevice	= NULL;
	m_pA3DStream	= NULL;
	m_pShader		= NULL;
}

CDecal::~CDecal()
{
}

/*	Initialize object

	Return true for success, otherwise return false.
*/
bool CDecal::Init(A3DDevice* pDevice)
{
	m_pA3DDevice = pDevice;

	//	Create vertex and index stream
	if (!(m_pA3DStream = new A3DStream))
	{
		a_LogOutput(1, "CDecal::Init() not enough memory for stream!");
		return false;
	}
	
	if (!m_pA3DStream->Init(pDevice, sizeof(DECALVERTEX), FVF_DECALVERTEX, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
	{
		a_LogOutput(1, "CDecal::Init(), Failed to initialize stream!");
		return false;
	}
	
	//	fill indices
	WORD* pIndices;
	if (!m_pA3DStream->LockIndexBuffer(0, 0, (BYTE**)&pIndices, 0))
	{
		a_LogOutput(1, "CDecal::Init(), Failed to lock index buffer!");
		return false;
	}
	
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 1;
	pIndices[4] = 3;
	pIndices[5] = 2;
	
	if (!m_pA3DStream->UnlockIndexBuffer())
	{
		a_LogOutput(1, "CDecal::Init(), Failed to unlock index buffer!");
		return false;
	}

	return true;
}

//	Release object
void CDecal::Release()
{
	if (m_pA3DStream)
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}

	m_pShader	 = NULL;
	m_pA3DDevice = NULL;
}

//	Update decal and prepare to render
bool CDecal::Render(A3DViewport* pView)
{
	//	Set vertices' position on screen
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR3 v(0.0f);
	A3DVIEWPORTPARAM* pViewParams = pView->GetParam();

	DECALVERTEX* pVerts;
	if (!m_pA3DStream->LockVertexBuffer(0, 0, (BYTE**)&pVerts, 0))
		return false;

	//	Adjust color and alpha
	DWORD dwCol;

	if (m_pShader)
	{
		float fVal;
		int r = A3DCOLOR_GETRED(m_dwCol);
		int g = A3DCOLOR_GETGREEN(m_dwCol);
		int b = A3DCOLOR_GETBLUE(m_dwCol);
		int a = A3DCOLOR_GETALPHA(m_dwCol);

		if (m_pShader->GetCurGenColorGenerate(&fVal))
		{
			r = (int)(r * fVal);
			g = (int)(g * fVal);
			b = (int)(b * fVal);

			a_Clamp(r, 0, 255);
			a_Clamp(g, 0, 255);
			a_Clamp(b, 0, 255);
		}

		if (m_pShader->GetCurGenAlphaGenerate(&fVal))
		{
			a = (int)(a * fVal);
			a_Clamp(a, 0, 255);
		}

		dwCol = A3DCOLORRGBA(r, g, b, a);
	}
	else
		dwCol = m_dwCol;

	//	Left-up vertex
	v.Set((float)pViewParams->X, (float)pViewParams->Y, 0.5f);
	pView->InvTransform(v, v);

	pVerts[0].x			= v.x;
	pVerts[0].y			= v.y;
	pVerts[0].z			= v.z;
	pVerts[0].u0		= m_fTexU;
	pVerts[0].v0		= m_fTexV;
	pVerts[0].u1		= 1.0f - pVerts[0].u0;
	pVerts[0].v1		= 1.0f - pVerts[0].v0;
	pVerts[0].diffuse	= dwCol;
	pVerts[0].specular	= Specular;
	
	//	Right-up vertex
	v.Set((float)(pViewParams->X + pViewParams->Width), (float)pViewParams->Y, 0.5f);
	pView->InvTransform(v, v);

	pVerts[1].x			= v.x;
	pVerts[1].y			= v.y;
	pVerts[1].z			= v.z;
	pVerts[1].u0		= m_fTexU + m_fTexWid;
	pVerts[1].v0		= m_fTexV;
	pVerts[1].u1		= 1.0f - pVerts[1].u0;
	pVerts[1].v1		= 1.0f - pVerts[1].v0;
	pVerts[1].diffuse	= dwCol;
	pVerts[1].specular	= Specular;
	
	//	Left-bottom vertex
	v.Set((float)pViewParams->X, (float)(pViewParams->Y + pViewParams->Height), 0.5f);
	pView->InvTransform(v, v);

	pVerts[2].x			= v.x;
	pVerts[2].y			= v.y;
	pVerts[2].z			= v.z;
	pVerts[2].u0		= m_fTexU;
	pVerts[2].v0		= m_fTexV + m_fTexHei;
	pVerts[2].u1		= 1.0f - pVerts[2].u0;
	pVerts[2].v1		= 1.0f - pVerts[2].v0;
	pVerts[2].diffuse	= dwCol;
	pVerts[2].specular	= Specular;
	
	//	Right-bottom vertex
	v.Set((float)(pViewParams->X + pViewParams->Width), (float)(pViewParams->Y + pViewParams->Height), 0.5f);
	pView->InvTransform(v, v);

	pVerts[3].x			= v.x;
	pVerts[3].y			= v.y;
	pVerts[3].z			= v.z;
	pVerts[3].u0		= m_fTexU + m_fTexWid;
	pVerts[3].v0		= m_fTexV + m_fTexHei;
	pVerts[3].u1		= 1.0f - pVerts[3].u0;
	pVerts[3].v1		= 1.0f - pVerts[3].v0;
	pVerts[3].diffuse	= dwCol;
	pVerts[3].specular	= Specular;
	
	if (!m_pA3DStream->UnlockVertexBuffer())
		return false;
	
	m_pA3DStream->Appear();

	//	Must close lighting here
	g_pA3DEngine->GetA3DDevice()->SetLightingEnable(false);

	//	Render
	if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2))
		return false;
		
	return true;
}

//	Render as background
bool CDecal::RenderAsBackground(A3DViewport* pView)
{
	//	Set vertices' position on screen
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	A3DVECTOR3 v;
	A3DVIEWPORTPARAM* pViewParams = pView->GetParam();

	DECALVERTEX* pVerts;
	if (!m_pA3DStream->LockVertexBuffer(0, 0, (BYTE**)&pVerts, 0))
		return false;

	//	Left-up vertex
	v.Set((float)pViewParams->X, (float)pViewParams->Y, 0.5f);
	pView->InvTransform(v, v);

	pVerts[0].x			= v.x;
	pVerts[0].y			= v.y;
	pVerts[0].z			= v.z;
	pVerts[0].u0		= m_fTexU;
	pVerts[0].v0		= m_fTexV;
	pVerts[0].u1		= 1.0f - pVerts[0].u0;
	pVerts[0].v1		= 1.0f - pVerts[0].v0;
	pVerts[0].diffuse	= m_dwCol;
	pVerts[0].specular	= Specular;
	
	//	Right-up vertex
	v.Set((float)(pViewParams->X + pViewParams->Width), (float)pViewParams->Y, 0.5f);
	pView->InvTransform(v, v);

	pVerts[1].x			= v.x;
	pVerts[1].y			= v.y;
	pVerts[1].z			= v.z;
	pVerts[1].u0		= m_fTexU + m_fTexWid;
	pVerts[1].v0		= m_fTexV;
	pVerts[1].u1		= 1.0f - pVerts[1].u0;
	pVerts[1].v1		= 1.0f - pVerts[1].v0;
	pVerts[1].diffuse	= m_dwCol;
	pVerts[1].specular	= Specular;
	
	//	Left-bottom vertex
	v.Set((float)pViewParams->X, (float)(pViewParams->Y + pViewParams->Height), 0.5f);
	pView->InvTransform(v, v);

	pVerts[2].x			= v.x;
	pVerts[2].y			= v.y;
	pVerts[2].z			= v.z;
	pVerts[2].u0		= m_fTexU;
	pVerts[2].v0		= m_fTexV + m_fTexHei;
	pVerts[2].u1		= 1.0f - pVerts[2].u0;
	pVerts[2].v1		= 1.0f - pVerts[2].v0;
	pVerts[2].diffuse	= m_dwCol;
	pVerts[2].specular	= Specular;
	
	//	Right-bottom vertex
	v.Set((float)(pViewParams->X + pViewParams->Width), (float)(pViewParams->Y + pViewParams->Height), 0.5f);
	pView->InvTransform(v, v);

	pVerts[3].x			= v.x;
	pVerts[3].y			= v.y;
	pVerts[3].z			= v.z;
	pVerts[3].u0		= m_fTexU + m_fTexWid;
	pVerts[3].v0		= m_fTexV + m_fTexHei;
	pVerts[3].u1		= 1.0f - pVerts[3].u0;
	pVerts[3].v1		= 1.0f - pVerts[3].v0;
	pVerts[3].diffuse	= m_dwCol;
	pVerts[3].specular	= Specular;
	
	if (!m_pA3DStream->UnlockVertexBuffer())
		return false;
	
	m_pA3DStream->Appear();

	//	Must close lighting here
	g_pA3DEngine->GetA3DDevice()->SetLightingEnable(false);

	//	Render
	if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2))
		return false;
		
	return true;
}


