 /*
 * FILE: A3DLitModelRender.cpp
 *
 * DESCRIPTION: A3D lit model renderer class
 *
 * CREATED BY: Hedi, 2005/4/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DTexture.h"
#include "A3DFrame.h"
#include "A3DLitModelRender.h"
#include "A3DLitModel.h"
#include "A3DStream.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DShaderMan.h"
#include "A3DTextureMan.h"
#include "A3DPixelShader.h"
#include "A3DVertexShader.h"
#include "A3DCameraBase.h"
#include "A3DOrthoCamera.h"
#include "A3DCamera.h"
#include "A3DRenderTarget.h"
#include "A3DShaderMan.h"
#include "A3DLight.h"
#include "A3DShadowMap.h"
#include "AFI.h"

A3DLitModelRender::A3DLitModelRender()
: m_aRenderSlots(128, 32),
  m_aAlphaMeshes(128, 32),
  m_aAlphaMeshes2(128, 32),
  m_aShadowMapMeshes(128, 32),
  m_aShadowMapSlots(128, 32),
  m_aLightmapSlots(128,32),
  m_aLMVertLitRenderSlots(128,32)
{
	m_pStream			= NULL;
	m_pLMStream			= NULL;
	m_nMaxVerts			= 0;
	m_nMaxIndices		= 0;
	m_nVertsCount		= 0;
	m_nIndicesCount		= 0;
	m_idAlphaMeshArray	= 0;
	m_fDNFactor			= 0.0f;

	m_bIsRenderForRefract = false;
	m_vRefractSurfaceHeight = 0.0f;
	m_pRefractVertexShader = NULL;


	//shadow map
	m_bUseShadowMap		= false;
	m_pLookupMap		= NULL;

	m_pShadowMapVS		= NULL;
	m_pShadowMapPS		= NULL;
	m_pShadowMapPSNoAlpha = NULL;
	m_pLitModelShadowVS = NULL;
	m_pLitModelShadowPS = NULL;
	m_fZBias			= 0.0f;
	m_fShadowLum		= 0.0f;

	m_pAlphaTextureVS	= NULL;
	m_pAlphaTexturePS	= NULL;

	m_pLightMapVS		= NULL;
	m_pLightMapTexPassVS = NULL;
	m_pLightMapPS		= NULL;
	m_pLightMapHDRPS	= NULL;
	m_pLightMapLookup	= NULL;


#ifdef LM_BUMP_ENABLE
	m_pSpecularMap		= NULL;
	m_pBumpPixelShader	= NULL;
	m_pBumpVertexShader	= NULL;
#endif
}

A3DLitModelRender::~A3DLitModelRender()
{
}

//	Initialize object
bool A3DLitModelRender::Init(A3DEngine* pA3DEngine, int nMaxVerts, int nMaxIndices)
{
	m_pA3DEngine		= pA3DEngine;
	m_pA3DDevice		= m_pA3DEngine->GetA3DDevice();

	if( !CreateStream(nMaxVerts, nMaxIndices) )
	{
		g_A3DErrLog.Log("A3DLitModelRender::Init(), failed to create stream!");
		return false;
	}

	DWORD	dwVSDecl[20];
	int		n = 0;

	n = 0;
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in local space
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_FLOAT3);		// normal x, y, z in local space
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_D3DCOLOR);	// diffuse color of precalculated
	dwVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_FLOAT2);		// u, v coords.
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.
	m_pRefractVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("shaders\\vs\\lm_refract_vs.txt", false, dwVSDecl);


	char* szFile;
	szFile = "shaders\\vs\\litmodel_texpass_vs.txt";
	//load light map shaders
	if( af_IsFileExist(szFile))
	{
		m_pLightMapTexPassVS = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(szFile, false, dwVSDecl);
		
	}

	DWORD	dwLMVSDecl[20];	
	n = 0;
	dwLMVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwLMVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in local space
	dwLMVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_FLOAT3);		// normal x, y, z in local space
	dwLMVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_D3DCOLOR);	// diffuse color of precalculated
	dwLMVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_FLOAT2);		// u, v coords.
	dwLMVSDecl[n ++] = D3DVSD_REG(4, D3DVSDT_FLOAT2);		// u, v coords.
	dwLMVSDecl[n ++] = D3DVSD_END();

	// end tokens.
	szFile = "shaders\\vs\\litmodel_lightmap_vs.txt";
	//load light map shaders
	if( af_IsFileExist(szFile))
	{
		m_pLightMapVS = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(szFile, false, dwLMVSDecl);
		
	}

	szFile = "shaders\\ps\\litmodel_lightmap_ps.txt";
	if(af_IsFileExist(szFile))
	{
		m_pLightMapPS = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(szFile, false);
	}
	
	szFile = "shaders\\ps\\litmodel_lightmap_hdr_ps.txt";
	if(af_IsFileExist(szFile))
	{
		m_pLightMapHDRPS = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(szFile, false);
	}

	m_pLightMapLookup = new A3DTexture();
	if( !m_pLightMapLookup->Create(m_pA3DDevice, LIGHTMAP_LOOKUP_SIZE, 1, A3DFMT_A8R8G8B8))
	{
		g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create lightmap lookup texture!");
		return false;
	}

	void* pData = NULL;
	int nPitch = 0;
	if(!m_pLightMapLookup->LockRect(NULL, &pData, &nPitch, 0, 0))
		return false;
	
	DWORD* pMap = (DWORD*)pData;
	for( INT i = 0; i < LIGHTMAP_LOOKUP_SIZE; i++ )
	{
		float fD =float(i)/LIGHTMAP_LOOKUP_SIZE;
		float fC = fD/ max(0.001f, 1.0f - fD);
		if(fC >= 9.0f)
			fC = 8.99f;
		
		int nR = 0;
		int nG = 0;
		int nB = 0;
		float fR  = 0.0f;
		float fG = 0.0f;
		float fB = 0.0f;
		
		fR = fC - int(fC);
		nR = int(fR *255);
		nG = (int)(int(fC)/8.0f * 255);
		*pMap++ = D3DCOLOR_RGBA( nR , nG, nB, 0 );	
	}
	
	if(!m_pLightMapLookup->UnlockRect())
		return false;
	


	InitShadowResource();	

#ifdef LM_BUMP_ENABLE
	// now load bump shaders.
	m_pBumpPixelShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("shaders\\ps\\lmbump.txt", false);

	n = 0;
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in local space
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_D3DCOLOR);	// diffuse color of precalculated
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_FLOAT2);		// u, v coords.
	dwVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_FLOAT3);		// nx, ny, nz in local space
	dwVSDecl[n ++] = D3DVSD_REG(4, D3DVSDT_FLOAT3);		// tangent coords in local space.
	dwVSDecl[n ++] = D3DVSD_REG(5, D3DVSDT_FLOAT3);		// binormal coords in local space.
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.
	m_pBumpVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("shaders\\vs\\lmbumpvs.txt", false, dwVSDecl);

	// now we create a specular map here
	m_pSpecularMap = new A3DTexture();
	if( !m_pSpecularMap->Create(m_pA3DDevice, 256, 1, A3DFMT_A8R8G8B8, 1) )
	{
		g_A3DErrLog.Log("A3DLitModelRender::Init(), failed to create specular map!");
		return false;
	}

	void * pData = NULL;
	int iPitch;
	if( !m_pSpecularMap->LockRect(NULL, &pData, &iPitch, 0) )
	{
		g_A3DErrLog.Log("A3DLitModelRender::Init(), failed to lock specular map!");
		return false;
	}

	DWORD * pColor = (DWORD *) pData;
	float p = 20.0f;
	for(int i=0; i<256; i++)
	{
		float v = i / 255.0f;
		v = (float)pow(v, p);
		BYTE c = BYTE(v * 255.0f);
		pColor[i] = A3DCOLORRGBA(c, c, c, 255);
	}

	m_pSpecularMap->UnlockRect();
#endif

	return true;
}

void A3DLitModelRender::CaculateShadowMapRadius(float& fRadius, A3DVECTOR3& vCenter)
{
	int nSlot = 0;
	A3DAABB aabb;
	aabb.Clear();

	int nMeshCount = 0;
	for( nSlot = 0; nSlot < m_aRenderSlots.GetSize(); nSlot++)
	{
		RENDERSLOT * pSlot = m_aRenderSlots[nSlot];
		if( pSlot->pTexture == NULL )
			break; // meet empty slot here, so there is no more after this
		
		for( int nMesh = 0; nMesh < pSlot->LitMeshes.GetSize(); nMesh++)
		{
			A3DLitMesh* pLitMesh = pSlot->LitMeshes[nMesh];
			aabb.Merge(pLitMesh->GetAABB());

			//A3DLitModel* pLitModel = pLitMesh->GetParentModel();
			//aabb.Merge(pLitModel->GetModelAABB());
			

			nMeshCount++;
		}
	}

	
	fRadius = aabb.Extents.Magnitude();
	vCenter = aabb.Center;
	return;
}

bool A3DLitModelRender::InitShadowResource()
{
	
	//release
	ReleaseShadowResource();
	
	//Create look up map
	m_pLookupMap = new A3DTexture();
	if( !m_pLookupMap->Create(m_pA3DDevice, LOOKUP_MAP_SIZE, 1, A3DFMT_A8R8G8B8))
	{
		g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create lookup texture!");
		return false;
	}
	
	void* pData = NULL;
	int nPitch = 0;
	if(!m_pLookupMap->LockRect(NULL, &pData, &nPitch, 0, 0))
		return false;

	DWORD* pMap = (DWORD*)pData;
	for( INT i = 0; i < LOOKUP_MAP_SIZE; i++ )
	{
		*pMap++ = D3DCOLOR_RGBA( i & 0xFF, (i & 0xFF00) >> 3, 0, 0 );
	}

	if(!m_pLookupMap->UnlockRect())
		return false;

	//Create vert decl
	DWORD	dwVSDecl[20];
	int		n = 0;				
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in world
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_FLOAT3);		// nx, ny, nz in world
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_D3DCOLOR);	// diffuse color
	dwVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_FLOAT2);		// u, v coords.
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.


	//Load shadow map vertex shader
	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();
	if( pShaderMan == NULL)
		return false;

	//load shadow map vertex shader
	const char* szShader = "shaders\\vs\\shadowmap_vs.txt";
	if(af_IsFileExist(szShader))
	{
 		m_pShadowMapVS = pShaderMan->LoadVertexShader(szShader, false, dwVSDecl);
 		if( m_pShadowMapVS == NULL)
 		{
 			g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create shadowmap vertex shader!");
 			return false;
 		}
	}
	//Load shadow map pixel shader
	szShader = "shaders\\ps\\shadowmap_ps.txt";
	if( af_IsFileExist(szShader))
	{
		m_pShadowMapPS = pShaderMan->LoadPixelShader(szShader, false);
		if( m_pShadowMapPS == NULL)
		{
			g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create shadowmap pixel shader!");
			return false;
		}
	}

	//Load shadow map pixel shader no alpha
	szShader = "shaders\\ps\\shadowmap_ps_na.txt";
	if( af_IsFileExist(szShader))
	{
		m_pShadowMapPSNoAlpha = pShaderMan->LoadPixelShader(szShader, false);
		if( m_pShadowMapPSNoAlpha == NULL)
		{
			g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create shadowmap non alpha pixel shader!");
			return false;
		}
	}

	//load litmodel shadow vertex shader
	szShader = "shaders\\vs\\litmodel_shadow_vs.txt";
	if( af_IsFileExist(szShader))
	{
		m_pLitModelShadowVS = pShaderMan->LoadVertexShader( szShader, false, dwVSDecl);
		if( m_pLitModelShadowVS == NULL)
		{
			g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create litmodel shadow vertex shader!");
			return false;

		}
	}

	//load litmodel shadow pixel shader
	szShader = "shaders\\ps\\litmodel_shadow_ps.txt";
	if( af_IsFileExist(szShader))
	{
		m_pLitModelShadowPS = pShaderMan->LoadPixelShader( szShader, false);
		if( m_pLitModelShadowPS == NULL)
		{
			g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create litmodel shadow pixel shader!");
			return false;
		}
	}

	szShader = "shaders\\vs\\litmodel_shadow_alpha_vs.txt";
	if( af_IsFileExist(szShader))
	{
		m_pAlphaTextureVS = pShaderMan->LoadVertexShader(szShader,false, dwVSDecl);
		if( m_pAlphaTextureVS == NULL)
		{
			g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create litmodel shadow alpha vertex shader!");
			return false;		
		}
	}
	
	szShader = "shaders\\ps\\litmodel_shadow_alpha_ps.txt";
	if( af_IsFileExist(szShader))
	{
		m_pAlphaTexturePS = pShaderMan->LoadPixelShader(szShader,false);
		if( m_pAlphaTexturePS == NULL)
		{
			g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create litmodel shadow alpha pixel shader!");
			return false;		
		}
	}	
	return true;
}


void A3DLitModelRender::ReleaseShadowResource()
{
	A3DRELEASE( m_pLookupMap);

	m_aShadowMapMeshes.RemoveAll();
	for( int i = 0; i < m_aShadowMapSlots.GetSize(); i++)
	{
		delete m_aShadowMapSlots[i];
	}
	m_aShadowMapSlots.RemoveAll();

	//Release shaders
	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();
	if( pShaderMan != NULL)
	{
		if( m_pShadowMapVS != NULL)
		{
			pShaderMan->ReleaseVertexShader(&m_pShadowMapVS);
			m_pShadowMapVS = NULL;
		}

		if( m_pShadowMapPS != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pShadowMapPS);
			m_pShadowMapPS = NULL;
		}

		if( m_pShadowMapPSNoAlpha != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pShadowMapPSNoAlpha);
			m_pShadowMapPSNoAlpha = NULL;
		}

		if( m_pLitModelShadowVS != NULL)
		{
			pShaderMan->ReleaseVertexShader(&m_pLitModelShadowVS);
			m_pLitModelShadowVS = NULL;
		}

		if( m_pLitModelShadowPS != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pLitModelShadowPS);
			m_pLitModelShadowPS = NULL;
		}

		if( m_pAlphaTextureVS != NULL)
		{
			pShaderMan->ReleaseVertexShader( &m_pAlphaTextureVS);
			m_pAlphaTextureVS = NULL;
		}
		
		if( m_pAlphaTexturePS != NULL)
		{
			pShaderMan->ReleasePixelShader( &m_pAlphaTexturePS);
			m_pAlphaTexturePS = NULL;
		}
	}

}

bool A3DLitModelRender::RenderShadowMap(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius, const float fShadowRealRadius)
{
	using namespace A3D;
	if(!g_pShadowMap->IsInit())
		return false;

	//shadow map set
	g_pShadowMap->Set(pShadowCenter, pSunLightDir, fShadowRadius, fShadowRealRadius);
	A3DOrthoCamera* pShadowCamera = g_pShadowMap->GetShadowCamera();
	ClipShadowMapMeshes(pShadowCamera);

	//clear device
	g_pShadowMap->ClearDevice();

	//set render state
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	bool bAlphaTest = m_pA3DDevice->GetAlphaTestEnable();

	//set texture
	m_pLookupMap->Appear(0);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);

	//set vertex shader and pixel shader
	m_pShadowMapVS->Appear();
	m_pShadowMapPS->Appear();
	
	//set vertex constants
	A3DMATRIX4 matShadowMVP;
	matShadowMVP = pShadowCamera->GetVPTM();
	matShadowMVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, (void*)&matShadowMVP, 4);
	
	//set pixel const
	A3DVECTOR4 c0(0.0f, 0.0f, 0.0f, 1.0f);
	m_pA3DDevice->SetPixelShaderConstants(0, (void*)&c0, 1);

	//set stream
	m_pStream->Appear(0, false);

	for( int i = 0; i < m_aShadowMapSlots.GetSize(); i++)
	{
		ShadowSlot* pSlot = m_aShadowMapSlots[i];
		A3DTexture* pTex = pSlot->pTexture;

		if (pTex && !pTex->IsAlphaTexture())
			continue;

		int nNumVerts = 0;
		int nNumIndices = 0;

		if( FillShadowCasterStream(nNumVerts, nNumIndices, i))
		{
			pTex->Appear(1);
			m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
			m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
			//render
			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nNumVerts, 0, nNumIndices / 3);
			pTex->Disappear(1);	
		}
	}

	m_pShadowMapPS->Disappear();
	m_pShadowMapPSNoAlpha->Appear();

	for( i = 0; i < m_aShadowMapSlots.GetSize(); i++)
	{
		ShadowSlot* pSlot = m_aShadowMapSlots[i];
		A3DTexture* pTex = pSlot->pTexture;

		if (pTex && pTex->IsAlphaTexture())
			continue;

		int nNumVerts = 0;
		int nNumIndices = 0;

		if( FillShadowCasterStream(nNumVerts, nNumIndices, i))
		{
			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nNumVerts, 0, nNumIndices / 3);
		}
	}
	
	//restore render target
	g_pShadowMap->Restore();	

	//restore vertex shader and pixel shader
	m_pShadowMapVS->Disappear();
	m_pShadowMapPSNoAlpha->Disappear();

	//restore render state
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFogEnable(true);
	m_pA3DDevice->SetAlphaTestEnable(bAlphaTest);
	
	//restore 
	m_pLookupMap->Disappear(0);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);


	return true;
}

void A3DLitModelRender::SetLitModelShadowVSConst(A3DViewport* pViewport, A3DVECTOR3* pLightDir, A3DCOLORVALUE* pLightColor, float fZBias)
{
	using namespace A3D;

	//get model mvp
	A3DCameraBase* pCurCamera = pViewport->GetCamera();
	A3DMATRIX4 matLitModelMVP = pCurCamera->GetVPTM();
	matLitModelMVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, (void*)&matLitModelMVP, 4);

	//get shadow mvp
	A3DMATRIX4 matShadowMVP;
	if( g_pShadowMap->IsOptimize())
		matShadowMVP = g_pShadowMap->GetShadowTextureCamera()->GetVPTM();
	else
		matShadowMVP = g_pShadowMap->GetShadowCamera()->GetVPTM();
		
	// Texture adjustment matrix
	A3DMATRIX4 matTexAdj(A3DMATRIX4::IDENTITY);
	matTexAdj._11 = 0.5f;
	matTexAdj._22 = -0.5f;
	matTexAdj._33 = 0.0f;
	matTexAdj._43 = 1.0f;
	matTexAdj._44 = 1.0f;

	// Jitter 4 samples

	//tex1
	int nShadowMapSize = g_pShadowMap->GetShadowMapSize();
 	FLOAT fC1 = 0.1f / nShadowMapSize;
 	FLOAT fC2 = 0.9f / nShadowMapSize;
	//FLOAT fC1 = 0.0f;
	//FLOAT fC2 = 0.0f;

	matTexAdj._41 = 0.5f + fC1;
	matTexAdj._42 = 0.5f + fC1;
	
	A3DMATRIX4 matShadowTex1 = matShadowMVP * matTexAdj;
	matShadowTex1.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(8, (void*)&matShadowTex1, 4);

	//tex2
	matTexAdj._41 = 0.5f + fC1;
	matTexAdj._42 = 0.5f + fC2;
	A3DMATRIX4 matShadowTex2 = matShadowMVP * matTexAdj;
	matShadowTex2.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(12, (void*)&matShadowTex2, 4);

	//tex3
	matTexAdj._41 = 0.5f + fC2;
	matTexAdj._42 = 0.5f + fC1;
	A3DMATRIX4 matShadowTex3 = matShadowMVP * matTexAdj;
	matShadowTex3.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(16, (void*)&matShadowTex3, 4);

	//tex4
	matTexAdj._41 = 0.5f + fC2;
	matTexAdj._42 = 0.5f + fC2;
	A3DMATRIX4 matShadowTex4 = matShadowMVP * matTexAdj;
	matShadowTex4.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(20, (void*)&matShadowTex4, 4);

	//shadow map mvp
	matShadowMVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(4, (void*)&matShadowMVP, 4);

	//light dir
	m_pA3DDevice->SetVertexShaderConstants( 30, (void*)pLightDir, 1);
	
	//light color
	m_pA3DDevice->SetVertexShaderConstants( 31, (void*)pLightColor, 1);

	//z-bais
	A3DVECTOR4 vZBais(fZBias, 0.0f, 0.0f, 0.0f);
	m_pA3DDevice->SetVertexShaderConstants(32, (void*)&vZBais, 1);

	return;
}

//render lit model with shadow
bool A3DLitModelRender::RenderLitModelWithShadow(A3DViewport* pViewport, 
												 A3DLight* pSunLight,
												 float fZBias, 
												 float fShadowLum)
{
	using namespace A3D;

	if( !g_pShadowMap->IsInit())
		return false;

	m_fZBias = fZBias;
	m_fShadowLum = fShadowLum;

	A3DLIGHTPARAM lightParam = pSunLight->GetLightparam();
	A3DVECTOR3 vSunLightDir = -lightParam.Direction;

	//set viewport
	pViewport->Active();

	//set shadow map textures
	g_pShadowMap->AppearAsTexture(0);
	g_pShadowMap->AppearAsTexture(1);
	g_pShadowMap->AppearAsTexture(2);
	g_pShadowMap->AppearAsTexture(3);
	
	//set texture address
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_BORDER, A3DTADDR_BORDER);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_BORDER, A3DTADDR_BORDER);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_BORDER, A3DTADDR_BORDER);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_BORDER, A3DTADDR_BORDER);

// 	m_pA3DDevice->SetDeviceTextureStageState(0, D3DTSS_BORDERCOLOR, 0xff000000);
// 	m_pA3DDevice->SetDeviceTextureStageState(1, D3DTSS_BORDERCOLOR,  0xff000000);
// 	m_pA3DDevice->SetDeviceTextureStageState(2, D3DTSS_BORDERCOLOR,  0xff000000);
// 	m_pA3DDevice->SetDeviceTextureStageState(3, D3DTSS_BORDERCOLOR,  0xff000000);

	m_pA3DDevice->SetDeviceTextureStageState(0, D3DTSS_BORDERCOLOR, 0x00ffffff);
	m_pA3DDevice->SetDeviceTextureStageState(1, D3DTSS_BORDERCOLOR,  0x00ffffff);
	m_pA3DDevice->SetDeviceTextureStageState(2, D3DTSS_BORDERCOLOR,  0x00ffffff);
	m_pA3DDevice->SetDeviceTextureStageState(3, D3DTSS_BORDERCOLOR,  0x00ffffff);



	//set texture filter
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);

	//set vertex shader and pixel shader
	m_pLitModelShadowVS->Appear();
	m_pLitModelShadowPS->Appear();
	

	//set vs const
	SetLitModelShadowVSConst(pViewport, &vSunLightDir, &lightParam.Diffuse, m_fZBias);
	
	//set shadow lum
	A3DVECTOR4 vShadowLum(0.0f, 0.0f, 0.0f, fShadowLum);
	m_pA3DDevice->SetPixelShaderConstants(5, &vShadowLum, 1);
	
	//set stream 
	m_pStream->Appear(0, false);

	//set render state
	m_pA3DDevice->SetAlphaBlendEnable(false);

	for( int i = 0; i < m_aRenderSlots.GetSize(); i++)
	{
		int nNumVerts = 0;
		int nNumIndices = 0;
		RENDERSLOT * pSlot = m_aRenderSlots[i];

		if( pSlot->pTexture && !pSlot->pTexture->IsAlphaTexture())
		{
			if( FillShadowReceiverStream(nNumVerts, nNumIndices, i))
			{
				A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();
				if( pSlot->b2Sided )
					m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

				//set diffuse texture
				pSlot->pTexture->Appear(4);	
				m_pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
				m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);

				//render
				m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nNumVerts, 0, nNumIndices / 3);
				
				pSlot->pTexture->Disappear(4);
				if( pSlot->b2Sided )
					m_pA3DDevice->SetFaceCull(cullOld);
			}
		}
	}

	//restore vertex shader and pixel shader
	m_pLitModelShadowVS->Disappear();
	m_pLitModelShadowPS->Disappear();
	

	//restore textures
	g_pShadowMap->DisappearAsTexture(0);
	g_pShadowMap->DisappearAsTexture(1);
	g_pShadowMap->DisappearAsTexture(2);
	g_pShadowMap->DisappearAsTexture(3);
	
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);

	m_pA3DDevice->SetDeviceTextureStageState(0, D3DTSS_BORDERCOLOR,  0);
	m_pA3DDevice->SetDeviceTextureStageState(1, D3DTSS_BORDERCOLOR,  0);
	m_pA3DDevice->SetDeviceTextureStageState(2, D3DTSS_BORDERCOLOR,  0);
	m_pA3DDevice->SetDeviceTextureStageState(3, D3DTSS_BORDERCOLOR,  0);	

	//restore render state
	m_pA3DDevice->SetAlphaBlendEnable(true);

	return true;
}

bool A3DLitModelRender::RenderLitModelAlphaMeshes(A3DViewport* pViewport)
{
	
	//set viewport
	pViewport->Active();
	
	//set stream 
	m_pStream->Appear(0, false);
	
	//set render state
	m_pA3DDevice->SetAlphaBlendEnable(false);
	
	m_pAlphaTextureVS->Appear();
	m_pAlphaTexturePS->Appear();
	
	//Set vertex shader const
	A3DCameraBase* pCurCamera = pViewport->GetCamera();
	A3DMATRIX4 matLitModelMVP = pCurCamera->GetVPTM();
	matLitModelMVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, (void*)&matLitModelMVP, 4);
	
	for( int i = 0; i < m_aRenderSlots.GetSize(); i++)
	{
		int nNumVerts = 0;
		int nNumIndices = 0;
		RENDERSLOT * pSlot = m_aRenderSlots[i];
		
		if( pSlot->pTexture && pSlot->pTexture->IsAlphaTexture())
		{
			if( FillShadowReceiverStream(nNumVerts, nNumIndices, i))
			{
				A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();
				if( pSlot->b2Sided )
					m_pA3DDevice->SetFaceCull(A3DCULL_NONE);				
				
				//set diffuse texture
				pSlot->pTexture->Appear(0);	
				m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
				m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
				
				//render
				m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nNumVerts, 0, nNumIndices / 3);
				
				pSlot->pTexture->Disappear(0);
				
				if( pSlot->b2Sided )
					m_pA3DDevice->SetFaceCull(cullOld);				
			}
		}
	}
	
	m_pAlphaTextureVS->Disappear();
	m_pAlphaTexturePS->Disappear();
	
	m_pA3DDevice->SetAlphaBlendEnable(true);
	return true;
}

bool A3DLitModelRender::FillShadowReceiverStream(int& nNumVerts, int& nNumIndices, int nSlot)
{
	A3DLMVERTEX*	pVerts = NULL;
	WORD*			pIndices = NULL;
	int				nVertsStart = 0;
	

	RENDERSLOT * pSlot = m_aRenderSlots[nSlot];
	if( pSlot->pTexture == NULL )
		return false; // meet empty slot here, so there is no more after this
	
	nNumVerts = pSlot->nNumVerts;
	nNumIndices = pSlot->nNumIndices;
	
	if( nNumVerts == 0 || nNumIndices == 0)
		return false;
	
	//Lock Stream
	ASSERT(nNumVerts <= m_nMaxVerts && nNumIndices <= m_nMaxIndices);
	
	if( !m_pStream->LockVertexBuffer(0, nNumVerts * sizeof(A3DLMVERTEX), (BYTE **) &pVerts, 0) )
	{
		return false;
	}
	
	if( !m_pStream->LockIndexBuffer(0, nNumIndices * sizeof(WORD), (BYTE **) &pIndices, 0) )
	{
		return false;
	}
	
	//Fill Stream	
	for(int n=0; n<pSlot->LitMeshes.GetSize(); n++)
	{
		A3DLitMesh * pLitMesh = pSlot->LitMeshes[n];
		
		int nNumVerts, nNumIndices;
		nNumVerts = pLitMesh->GetNumVerts();
		nNumIndices = pLitMesh->GetNumFaces() * 3;
		
		pLitMesh->DrawToBuffer(pVerts, nVertsStart, pIndices);
		
		nVertsStart += nNumVerts;
		pVerts += nNumVerts;
		pIndices += nNumIndices;
	}

	
	//unlock stream
	if( !m_pStream->UnlockVertexBuffer())
	{
		return false;
	}
	if( !m_pStream->UnlockIndexBuffer())
	{
		return false;
	}
	
	return true;
}

bool A3DLitModelRender::FillShadowCasterStream(int& nNumVerts, int& nNumIndices, int nSlot)
{
	A3DLMVERTEX*	pVerts = NULL;
	WORD*			pIndices = NULL;
	int				nVertsStart = 0;

	ShadowSlot * pSlot = m_aShadowMapSlots[nSlot];
	if( pSlot->pTexture == NULL )
		return false; // meet empty slot here, so there is no more after this
	
	nNumVerts = pSlot->nNumVerts;
	nNumIndices = pSlot->nNumIndices;
	
	if( nNumVerts == 0 || nNumIndices == 0)
		return false;
	
	//Lock Stream
	ASSERT(nNumVerts <= m_nMaxVerts && nNumIndices <= m_nMaxIndices);


	if( !m_pStream->LockVertexBuffer(0, nNumVerts * sizeof(A3DLMVERTEX), (BYTE **) &pVerts, 0) )
	{
		return false;
	}
	
	if( !m_pStream->LockIndexBuffer(0, nNumIndices * sizeof(WORD), (BYTE **) &pIndices, 0) )
	{
		return false;
	}

	for(int n=0; n<pSlot->LitMeshes.GetSize(); n++)
	{
		A3DLitMesh * pLitMesh = pSlot->LitMeshes[n];
		
		int nNumVerts, nNumIndices;
		nNumVerts = pLitMesh->GetNumVerts();
		nNumIndices = pLitMesh->GetNumFaces() * 3;
		
		pLitMesh->DrawToBuffer(pVerts, nVertsStart, pIndices);
		
		nVertsStart += nNumVerts;
		pVerts += nNumVerts;
		pIndices += nNumIndices;
	}
	
	//unlock stream
	if( !m_pStream->UnlockVertexBuffer())
	{
		return false;
	}
	if( !m_pStream->UnlockIndexBuffer())
	{
		return false;
	}

	return true;

}

//	Initialize stream object
bool A3DLitModelRender::CreateStream(int nMaxVerts, int nMaxIndices)
{
	// first release old ones.
	ReleaseStream();

	m_nMaxVerts = nMaxVerts;
	m_nMaxIndices = nMaxIndices;

	m_pStream = new A3DStream();
	if( !m_pStream->Init(m_pA3DDevice, sizeof(A3DLMVERTEX), A3DLMVERTEX_FVF, m_nMaxVerts, m_nMaxIndices, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) )
	{
		g_A3DErrLog.Log("A3DLitModelRender::CreateStream(), failed to initialize stream buffer");
		return false;
	}

	m_pLMStream = new A3DStream();
	if( !m_pLMStream->Init(m_pA3DDevice, sizeof(A3DLMLMVERTEX), A3DLMLMVERTEX_FVF, m_nMaxVerts, m_nMaxIndices, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) )
	{
		g_A3DErrLog.Log("A3DLitModelRender::CreateStream(), failed to initialize stream buffer");
		return false;
	}

	m_nVertsCount = 0;
	m_nIndicesCount = 0;
	return true;
}

//	Finalize stream object
bool A3DLitModelRender::ReleaseStream()
{
	if( m_pStream )
	{
		m_pStream->Release();
		delete m_pStream;
		m_pStream = NULL;
	}

	if( m_pLMStream)
	{
		m_pLMStream->Release();
		delete m_pLMStream;
		m_pLMStream = NULL;
	}

	m_nMaxVerts		= 0;
	m_nMaxIndices	= 0;
	m_nVertsCount	= 0;
	m_nIndicesCount	= 0;
	return true;
}

//	Release object
void A3DLitModelRender::Release()
{
	ReleaseShadowResource();

	int i;

	for(i=0; i<m_aRenderSlots.GetSize(); i++)
	{
		delete m_aRenderSlots[i];
	}

	m_aRenderSlots.RemoveAll();

	//release light map slot
	for( i = 0; i < m_aLightmapSlots.GetSize(); i++)
	{
		delete m_aLightmapSlots[i];
	}
	m_aLightmapSlots.RemoveAll();

	for( i = 0; i < m_aLMVertLitRenderSlots.GetSize(); i++)
	{
		delete m_aLMVertLitRenderSlots[i];
	}
	m_aLMVertLitRenderSlots.RemoveAll();


	ReleaseStream();

	if( m_pRefractVertexShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pRefractVertexShader);
		m_pRefractVertexShader = NULL;
	}

	if( m_pLightMapVS)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pLightMapVS);
		m_pLightMapVS = NULL;
	}

	if( m_pLightMapTexPassVS)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pLightMapTexPassVS);
		m_pLightMapTexPassVS = NULL;
	}

	if( m_pLightMapPS)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pLightMapPS);
		m_pLightMapPS = NULL;
	}
	if( m_pLightMapHDRPS)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pLightMapHDRPS);
		m_pLightMapHDRPS = NULL;
	}

	A3DRELEASE( m_pLightMapLookup);

#ifdef LM_BUMP_ENABLE
	if( m_pBumpPixelShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pBumpPixelShader);
		m_pBumpPixelShader = NULL;
	}
	if( m_pBumpVertexShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pBumpVertexShader);
		m_pBumpVertexShader = NULL;
	}

	if( m_pSpecularMap )
	{
		m_pSpecularMap->Release();
		delete m_pSpecularMap;
		m_pSpecularMap = NULL;
	}
#endif
}


bool A3DLitModelRender::RegisterRenderShadowMapMesh(A3DLitMesh* pLitMesh)
{
	
	if( pLitMesh->GetTexturePtr()->IsShaderTexture()/* || pLitMesh->GetAlphaValue() < 255*/)
	{
		return true;
	}

#ifdef _DEBUG
	for( int i = 0; i < m_aShadowMapMeshes.GetSize(); i++)
	{
		A3DLitMesh* pCurMesh = m_aShadowMapMeshes[i];
		assert( pCurMesh != pLitMesh);
	}
#endif
	
	m_aShadowMapMeshes.Add(pLitMesh);
	return true;
}

bool A3DLitModelRender::FillShadowMapSlots(A3DLitMesh* pLitMesh)
{
	A3DTexture* pTex;
	
	if (pLitMesh->GetReflectTexturePtr())
		pTex = pLitMesh->GetReflectTexturePtr();
	else
		pTex = pLitMesh->GetTexturePtr();
	
	for( int i = 0; i < m_aShadowMapSlots.GetSize(); i++)
	{
		ShadowSlot* pSlot = m_aShadowMapSlots[i];
		if( pTex == pSlot->pTexture)
		{
			int nNumVerts = pSlot->nNumVerts + pLitMesh->GetNumVerts();
			int nNumIndices = pSlot->nNumIndices + pLitMesh->GetNumFaces() * 3;
			
			if( nNumVerts < m_nMaxVerts && nNumIndices < m_nMaxIndices)
			{
				pSlot->nNumVerts = nNumVerts;
				pSlot->nNumIndices = nNumIndices;
				pSlot->LitMeshes.Add(pLitMesh);
				return true;
			}
		}
		else if( pSlot->pTexture == NULL)
		{
			pSlot->pTexture = pTex;
			pSlot->nNumVerts = pLitMesh->GetNumVerts();
			pSlot->nNumIndices = pLitMesh->GetNumFaces() * 3;
			pSlot->LitMeshes.Add(pLitMesh);
			return true;
		}
	}
	
	ShadowSlot* pNewSlot = new ShadowSlot;
	pNewSlot->pTexture = pTex;
	pNewSlot->nNumVerts = pLitMesh->GetNumVerts();
	pNewSlot->nNumIndices = pLitMesh->GetNumFaces() * 3;
	pNewSlot->LitMeshes.Add(pLitMesh);

	m_aShadowMapSlots.Add(pNewSlot);
	return true;
}

bool A3DLitModelRender::ClipShadowMapMeshes(A3DOrthoCamera* pOrthoCamera)
{
	for( int nMesh = 0; nMesh < m_aShadowMapMeshes.GetSize(); nMesh++)
	{
		A3DLitMesh* pLitMesh = m_aShadowMapMeshes[nMesh];

		A3DAABB aabb = pLitMesh->GetAABB();
		A3DLitModel* pLitModel = pLitMesh->GetParentModel();

	//	if( !pOrthoCamera->AABBInViewFrustum(pLitModel->GetModelAABB()))
	//		continue;

		if( !pOrthoCamera->AABBInViewFrustum(aabb.Mins, aabb.Maxs))
			continue;

		FillShadowMapSlots(pLitMesh);
	}

	return true;
}

#define ISSAMELIGHT(a, b)  ((a).Type == (b).Type && (a).Diffuse == (b).Diffuse && (a).Direction == (b).Direction)
//	Register mesh which is ready to be rendered.
bool A3DLitModelRender::RegisterRenderMesh(A3DViewport* pViewport, A3DLitMesh * pLitMesh, bool bAlphaFlag/*false*/)
{
	// if is a shader mesh or alpha flag mesh
	if( pLitMesh->GetTexturePtr()->IsShaderTexture() || pLitMesh->GetAlphaValue() < 255 || bAlphaFlag )
	{
		APtrArray<A3DLitMesh *> *	pAlphaMeshes;

		if( m_idAlphaMeshArray == 0 )
			pAlphaMeshes = &m_aAlphaMeshes;
		else
			pAlphaMeshes = &m_aAlphaMeshes2;
		
		// insert from far to near
		for(int i=0; i<pAlphaMeshes->GetSize(); i++)
		{
			A3DLitMesh * pNextMesh = (*pAlphaMeshes)[i];

			if( pLitMesh->GetDisToCam() > pNextMesh->GetDisToCam() )
			{
				// insert it before pNextMesh;
				pAlphaMeshes->InsertAt(i, pLitMesh);
				break;
			}
		}
		if( i == pAlphaMeshes->GetSize() )
		{
			pAlphaMeshes->Add(pLitMesh);
		}
	}
	else
	{
		// first determine which slot to use
		A3DTexture *	pTexture = pLitMesh->GetTexturePtr();
		A3DTexture *	pReflectTexture = pLitMesh->GetReflectTexturePtr();
		BOOL			b2Sided = pLitMesh->GetMaterial().Is2Sided();
#ifdef LM_BUMP_ENABLE
		A3DTexture * pNormalMap = pLitMesh->GetNormalMap();
		A3DLIGHTPARAM paramDay = pLitMesh->m_paramDay;
		A3DLIGHTPARAM paramNight = pLitMesh->m_paramNight;
#endif
		
		for(int i=0; i<m_aRenderSlots.GetSize(); i++)
		{
			RENDERSLOT * pSlot = m_aRenderSlots[i];

#ifdef LM_BUMP_ENABLE
			if( pTexture == pSlot->pTexture && pNormalMap == pSlot->pNormalMap && 
				ISSAMELIGHT(paramDay, pSlot->paramDay) && ISSAMELIGHT(paramNight, pSlot->paramNight) &&
				b2Sided == pSlot->b2Sided )
#else
			if( pTexture == pSlot->pTexture && b2Sided == pSlot->b2Sided )
#endif
			{
				// found the same texture slot, put it here
				int nNumVerts = m_aRenderSlots[i]->nNumVerts + pLitMesh->GetNumVerts();
				int nNumIndices = m_aRenderSlots[i]->nNumIndices + pLitMesh->GetNumFaces() * 3;

				if( nNumVerts <= m_nMaxVerts && nNumIndices <= m_nMaxIndices )
				{
					m_aRenderSlots[i]->nNumVerts = nNumVerts;;
					m_aRenderSlots[i]->nNumIndices = nNumIndices;
					m_aRenderSlots[i]->LitMeshes.Add(pLitMesh);
					return true;
				}
			}
			else if( pSlot->pTexture == NULL )
			{
				// found an empty slot, so there is no same texture and put it here
				pSlot->pTexture = pTexture;
				pSlot->pReflectTexture = pReflectTexture;
				pSlot->b2Sided = b2Sided;
#ifdef LM_BUMP_ENABLE
				pSlot->pNormalMap = pNormalMap;
				pSlot->paramDay = paramDay;
				pSlot->paramNight = paramNight;
#endif
				pSlot->nNumVerts = pLitMesh->GetNumVerts();
				pSlot->nNumIndices = pLitMesh->GetNumFaces() * 3;
				pSlot->LitMeshes.Add(pLitMesh);
				return true;
			}
		}

		// not found in current slots, so add a new one slot here
		RENDERSLOT * pNewSlot = new RENDERSLOT;
		pNewSlot->pTexture = pTexture;
		pNewSlot->pReflectTexture = pReflectTexture;
		pNewSlot->b2Sided = b2Sided;
#ifdef LM_BUMP_ENABLE
		pNewSlot->pNormalMap = pNormalMap;
		pNewSlot->paramDay = paramDay;
		pNewSlot->paramNight = paramNight;
#endif
		pNewSlot->nNumVerts = pLitMesh->GetNumVerts();
		pNewSlot->nNumIndices = pLitMesh->GetNumFaces() * 3;
		pNewSlot->LitMeshes.Add(pLitMesh);
		m_aRenderSlots.Add(pNewSlot);
	}

	return true;
}

bool A3DLitModelRender::RegisterLightMapMesh(A3DViewport* pViewport, A3DLitMesh* pLitMesh)
{
	A3DTexture* pLightMap = pLitMesh->GetLightMap();
	A3DTexture* pNightLightMap =pLitMesh->GetNightLightMap();
	A3DTexture *	pTexture = pLitMesh->GetTexturePtr();

	for( int i = 0; i < m_aLightmapSlots.GetSize(); i++)
	{
		LightMapSlot* pSlot = m_aLightmapSlots[i];

		if( pLightMap == pSlot->pLightMap && pNightLightMap == pSlot->pNightLightMap && pTexture == pSlot->pTexture)
		{
			int nNumVerts = pSlot->nNumVerts + pLitMesh->GetNumVerts();
			int nNumIndices = pSlot->nNumIndices + pLitMesh->GetNumFaces() * 3;
			
			if( nNumVerts <= m_nMaxVerts && nNumIndices <= m_nMaxIndices )
			{
				pSlot->nNumVerts = nNumVerts;
				pSlot->nNumIndices = nNumIndices;
				pSlot->LitMeshes.Add(pLitMesh);
				return true;
			}
		}
		else if( pSlot->pLightMap == NULL && pSlot->pNightLightMap == NULL && pSlot->pTexture == NULL)
		{
			pSlot->pTexture = pTexture;
			pSlot->pLightMap = pLightMap;
			pSlot->pNightLightMap = pNightLightMap;
			pSlot->nNumVerts = pLitMesh->GetNumVerts();
			pSlot->nNumIndices = pLitMesh->GetNumFaces() * 3;
			pSlot->LitMeshes.Add(pLitMesh);
			return true;
		}
	}

	LightMapSlot* pNewSlot = new LightMapSlot;
	pNewSlot->pLightMap = pLightMap;
	pNewSlot->pNightLightMap = pNightLightMap;
	pNewSlot->pTexture = pTexture;
	pNewSlot->nNumVerts = pLitMesh->GetNumVerts();
	pNewSlot->nNumIndices = pLitMesh->GetNumFaces() * 3;
	pNewSlot->LitMeshes.Add(pLitMesh);
	m_aLightmapSlots.Add(pNewSlot);
	
	return true;
}

bool A3DLitModelRender::RegisterVertLitMesh(A3DViewport* pViewport, A3DLitMesh* pLitMesh)
{
	// first determine which slot to use
	A3DTexture *	pTexture = pLitMesh->GetTexturePtr();
	A3DTexture *	pReflectTexture = pLitMesh->GetReflectTexturePtr();
	BOOL			b2Sided = pLitMesh->GetMaterial().Is2Sided();

	if( pLitMesh->GetTexturePtr()->IsShaderTexture() || pLitMesh->GetAlphaValue() < 255 )
	{
		return true;
	}

	for(int i=0; i<m_aLMVertLitRenderSlots.GetSize(); i++)
	{
		RENDERSLOT * pSlot = m_aLMVertLitRenderSlots[i];
		if( pTexture == pSlot->pTexture && b2Sided == pSlot->b2Sided )
		{
			// found the same texture slot, put it here
			int nNumVerts = m_aLMVertLitRenderSlots[i]->nNumVerts + pLitMesh->GetNumVerts();
			int nNumIndices = m_aLMVertLitRenderSlots[i]->nNumIndices + pLitMesh->GetNumFaces() * 3;
			
			if( nNumVerts <= m_nMaxVerts && nNumIndices <= m_nMaxIndices )
			{
				m_aLMVertLitRenderSlots[i]->nNumVerts = nNumVerts;;
				m_aLMVertLitRenderSlots[i]->nNumIndices = nNumIndices;
				m_aLMVertLitRenderSlots[i]->LitMeshes.Add(pLitMesh);
				return true;
			}
		}
		else if( pSlot->pTexture == NULL )
		{
			// found an empty slot, so there is no same texture and put it here
			pSlot->pTexture = pTexture;
			pSlot->pReflectTexture = pReflectTexture;
			pSlot->b2Sided = b2Sided;
			pSlot->nNumVerts = pLitMesh->GetNumVerts();
			pSlot->nNumIndices = pLitMesh->GetNumFaces() * 3;
			pSlot->LitMeshes.Add(pLitMesh);
			return true;
		}
	}
	
	// not found in current slots, so add a new one slot here
	RENDERSLOT * pNewSlot = new RENDERSLOT;
	pNewSlot->pTexture = pTexture;
	pNewSlot->pReflectTexture = pReflectTexture;
	pNewSlot->b2Sided = b2Sided;
	pNewSlot->nNumVerts = pLitMesh->GetNumVerts();
	pNewSlot->nNumIndices = pLitMesh->GetNumFaces() * 3;
	pNewSlot->LitMeshes.Add(pLitMesh);
	m_aLMVertLitRenderSlots.Add(pNewSlot);
	

	return true;
}

//	Render all lit models
bool A3DLitModelRender::Render(A3DViewport* pViewport, 
							   bool bRenderAlpha, 
							   bool bUseShadowMap, 
							   A3DLight* pSunLight,
							   float fZBias, 
							   float fShadowLum)
{
	using namespace A3D;

	m_bUseShadowMap = bUseShadowMap;

	if(g_pShadowMap->IsInit() && m_bUseShadowMap && m_pA3DDevice->TestPixelShaderVersion(1, 4))
	{
		RenderLitModelWithShadow(pViewport,pSunLight, fZBias, fShadowLum);

		//render alpha texture meshes
		RenderLitModelAlphaMeshes(pViewport);

		return true;
	}

	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);

	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG1);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);

	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();

	m_pStream->Appear();

#ifdef LM_BUMP_ENABLE
	A3DVECTOR3 vecCamPos = pViewport->GetCamera()->GetPos();
	bool bUseNormalMap = false;
	bool bUserClip = false;
	D3DXPLANE cp, hcp;
	DWORD dwOldBias = 0;
	if( m_pBumpPixelShader && m_pBumpVertexShader && !(GetKeyState(VK_F7) & 0x8000) )
	{
		m_pBumpPixelShader->Appear();
		//	Prepare a transformed clip plane
		DWORD dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
		if( dwState == D3DCLIPPLANE0 ) // only one clip plane supported now
		{
			bUserClip = true;
			m_pA3DDevice->GetD3DDevice()->GetClipPlane(0, (float *)&cp);
			A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
			matVP.InverseTM();
			matVP.Transpose();
			D3DXPlaneTransform(&hcp, &cp, (D3DXMATRIX *) &matVP);
			m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float *)&hcp);
		}
		m_pBumpVertexShader->Appear();
		bUseNormalMap = true;

		m_pA3DDevice->GetDeviceTextureStageState(1, D3DTSS_MIPMAPLODBIAS, &dwOldBias);
		m_pA3DDevice->SetDeviceTextureStageState(1, D3DTSS_MIPMAPLODBIAS, 0);
		m_pA3DDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

		if( m_pSpecularMap )
			m_pSpecularMap->Appear(2);
	}
#endif

	if( m_bIsRenderForRefract && m_pRefractVertexShader )
	{
		m_pRefractVertexShader->Appear();
		// set vertex shader constants here
		A3DVECTOR4 c0;
		c0.x = 1.0f;
		c0.y = m_vRefractSurfaceHeight;
		c0.z = 0.075f;	// fog density
		c0.w = 0.0f;	// fog offset
		m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
		A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
		matVP.Transpose();
		m_pA3DDevice->SetVertexShaderConstants(1, &matVP, 4);
		A3DVECTOR4 c5(pViewport->GetCamera()->GetPos());
		m_pA3DDevice->SetVertexShaderConstants(5, &c5, 1);
	}
	
	A3DLMVERTEX *		pVerts;
	WORD *				pIndices;

	int					nVertsLockStart = 0;
	int					nIndicesLockStart = 0;
	int					nVertsToLock = 0;
	int					nIndicesToLock = 0;
	
	for(int i=0; i<m_aRenderSlots.GetSize(); i++)
	{
		RENDERSLOT * pSlot = m_aRenderSlots[i];

		if( pSlot->pTexture == NULL )
			break; // meet empty slot here, so there is no more after this

		pSlot->pTexture->Appear(0);
		if( !m_bIsRenderForRefract && pSlot->pReflectTexture )
		{
			pSlot->pReflectTexture->Appear(1);
			m_pA3DDevice->SetTextureColorArgs(1, A3DTA_CURRENT, A3DTA_TEXTURE);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_MODULATEALPHA_ADDCOLOR);
			m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_CURRENT, A3DTA_TEXTURE);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_SELECTARG2);
			m_pA3DDevice->SetTextureCoordIndex(1, A3DTCI_CAMERASPACEREFLECTIONVECTOR);
			m_pA3DDevice->SetTextureTransformFlags(1, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));
			A3DMATRIX4 matCam = TransformMatrix(pViewport->GetCamera()->GetDirH(), A3DVECTOR3(0.0f, 1.0f, 0.0f), A3DVECTOR3(0.0f));
			A3DMATRIX4 matScale = IdentityMatrix();
			matScale._11 = 0.5f;
			matScale._22 = -0.5f;
			matScale._41 = 0.5f;
			matScale._42 = 0.5f;
			matScale = matCam * matScale;
			m_pA3DDevice->SetTextureMatrix(1, matScale);
		}
		else
		{
			m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
			m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
		}
		A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();
		if( pSlot->b2Sided )
			m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

#ifdef LM_BUMP_ENABLE
		if( bUseNormalMap )
		{
			if( pSlot->pNormalMap )
				pSlot->pNormalMap->Appear(1);
		}
#endif
		
		DWORD dwVertsLockFlag = 0;
		DWORD dwIndicesLockFlag = 0;

		nVertsToLock = pSlot->nNumVerts;
		if( nVertsLockStart + nVertsToLock > m_nMaxVerts )
		{
			// not enough free vertex buffer, so lock entire buffer with discard flag here
			nVertsLockStart = 0;
		}
		if( nVertsLockStart > 0 )
			dwVertsLockFlag = D3DLOCK_NOOVERWRITE;
		
		nIndicesToLock = pSlot->nNumIndices;
		if( nIndicesLockStart + nIndicesToLock > m_nMaxIndices )
		{
			// not enough free vertex buffer, so lock entire buffer with discard flag here
			nIndicesLockStart = 0;
		}
		if( nIndicesLockStart > 0 )
			dwIndicesLockFlag = D3DLOCK_NOOVERWRITE;
		
		ASSERT(nVertsLockStart + nVertsToLock <= m_nMaxVerts &&
			nIndicesLockStart + nIndicesToLock <= m_nMaxIndices);
		
		if( !m_pStream->LockVertexBuffer(nVertsLockStart * sizeof(A3DLMVERTEX), nVertsToLock * sizeof(A3DLMVERTEX), (BYTE **) &pVerts, dwVertsLockFlag) )
		{
			if( pSlot->b2Sided )
				m_pA3DDevice->SetFaceCull(cullOld);
			return false;
		}

		if( !m_pStream->LockIndexBuffer(nIndicesLockStart * sizeof(WORD), nIndicesToLock * sizeof(WORD), (BYTE **) &pIndices, dwIndicesLockFlag) )
		{
			if( pSlot->b2Sided )
				m_pA3DDevice->SetFaceCull(cullOld);
			return false;
		}

		m_nVertsCount = 0;
		m_nIndicesCount = 0;

		for(int n=0; n<pSlot->LitMeshes.GetSize(); n++)
		{
			A3DLitMesh * pLitMesh = pSlot->LitMeshes[n];
			
			int nNumVerts, nNumIndices;
			nNumVerts = pLitMesh->GetNumVerts();
			nNumIndices = pLitMesh->GetNumFaces() * 3;
			
			pLitMesh->DrawToBuffer(pVerts, nVertsLockStart + m_nVertsCount, pIndices);
			
			m_nVertsCount += nNumVerts;
			m_nIndicesCount += nNumIndices;
			
			pVerts += nNumVerts;
			pIndices += nNumIndices;
		}

		if( m_nVertsCount )
		{
			if( !m_pStream->UnlockVertexBuffer() )
			{
				if( pSlot->b2Sided )
					m_pA3DDevice->SetFaceCull(cullOld);
				return false;
			}
			if( !m_pStream->UnlockIndexBuffer() )
			{
				if( pSlot->b2Sided )
					m_pA3DDevice->SetFaceCull(cullOld);
				return false;
			}
#ifdef LM_BUMP_ENABLE
			if( bUseNormalMap )
			{
				if( m_pBumpPixelShader )
				{
					// now set shader's constants here
					A3DCOLORVALUE c1 = m_pA3DDevice->GetAmbientValue() * 0.5f;
					m_pA3DDevice->SetPixelShaderConstants(1, &c1, 1);
					A3DCOLORVALUE c2;
					c2 = pSlot->paramDay.Diffuse * 0.5f;
					m_pA3DDevice->SetPixelShaderConstants(2, &c2, 1);
				}

				if( m_pBumpVertexShader )
				{
					// set vertex shader constants here
					A3DVECTOR4 c0(0.5f, 0.5f, 0.5f, 1.0f);
					m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
					A3DVECTOR3 vecLightDir = pSlot->paramDay.Direction;
					A3DVECTOR4 c1;
					c1.x = vecLightDir.x;
					c1.y = vecLightDir.z;
					c1.z = vecLightDir.y;
					m_pA3DDevice->SetVertexShaderConstants(1, &c1, 1);
					A3DVECTOR3 vecEyePos = vecCamPos;
					A3DVECTOR4 c2;
					c2.x = vecEyePos.x;
					c2.y = vecEyePos.y;
					c2.z = vecEyePos.z;
					m_pA3DDevice->SetVertexShaderConstants(2, &c2, 1);
					A3DMATRIX4 mat = pViewport->GetCamera()->GetVPTM();
					mat.Transpose();
					m_pA3DDevice->SetVertexShaderConstants(3, &mat, 4);
				}
			}
#endif

			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, nVertsLockStart, m_nVertsCount, nIndicesLockStart, m_nIndicesCount / 3);

			pSlot->nNumVerts -= m_nVertsCount;
			pSlot->nNumIndices -= m_nIndicesCount;
		}
		else
		{
			ASSERT(0);
		}

		pSlot->pTexture->Disappear(0);	 
		if( !m_bIsRenderForRefract && pSlot->pReflectTexture )
		{
			pSlot->pReflectTexture->Disappear(1);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
			m_pA3DDevice->SetTextureCoordIndex(1, 1);
			m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
		}

		if( pSlot->b2Sided )
			m_pA3DDevice->SetFaceCull(cullOld);

#ifdef LM_BUMP_ENABLE
		if( bUseNormalMap )
		{
			if( pSlot->pNormalMap )
				pSlot->pNormalMap->Disappear(1);
		}
#endif

		nVertsLockStart += nVertsToLock;
		nIndicesLockStart += nIndicesToLock;
	}

	if( m_bIsRenderForRefract && m_pRefractVertexShader )
	{
		m_pRefractVertexShader->Disappear();
	}

#ifdef LM_BUMP_ENABLE
	if( m_pBumpPixelShader && m_pBumpVertexShader && !(GetKeyState(VK_F7) & 0x8000) )
	{
		if( bUserClip )
			m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float *)&cp);
		if( m_pSpecularMap )
			m_pSpecularMap->Disappear(2);
		m_pBumpPixelShader->Disappear();
		m_pBumpVertexShader->Disappear();
		m_pA3DDevice->SetDeviceTextureStageState(1, D3DTSS_MIPMAPLODBIAS, dwOldBias);
		m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	}
#endif

	if( bRenderAlpha )
		RenderAlpha(pViewport);

	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	return true;
}

//	Render alpha lit models
bool A3DLitModelRender::RenderAlpha(A3DViewport* pViewport)
{
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);

	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);

	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();

	A3DLMVERTEX *		pVerts;
	WORD *				pIndices;

	int					nVertsLockStart = 0;
	int					nIndicesLockStart = 0;
	int					nVertsToLock = 0;
	int					nIndicesToLock = 0;

	m_pStream->Appear();

	APtrArray<A3DLitMesh *> * pAlphaMeshes;
	if( m_idAlphaMeshArray == 0 )
		pAlphaMeshes = &m_aAlphaMeshes;
	else
		pAlphaMeshes = &m_aAlphaMeshes2;

	int nAlphaRefOld = m_pA3DDevice->GetAlphaRef();
	bool bAlphaTestOld = m_pA3DDevice->GetAlphaTestEnable();

	for(int i=0; i<pAlphaMeshes->GetSize(); i++)
	{
		A3DLitMesh * pLitMesh = (*pAlphaMeshes)[i];

		DWORD dwVertsLockFlag = 0;
		DWORD dwIndicesLockFlag = 0;

		nVertsToLock = pLitMesh->GetNumVerts();
		if( nVertsLockStart + nVertsToLock > m_nMaxVerts )
		{
			// not enough free vertex buffer, so lock entire buffer with discard flag here
			nVertsLockStart = 0;
		}
		if( nVertsLockStart > 0 )
			dwVertsLockFlag = D3DLOCK_NOOVERWRITE;
		
		nIndicesToLock = pLitMesh->GetNumFaces() * 3;
		if( nIndicesLockStart + nIndicesToLock > m_nMaxIndices )
		{
			// not enough free vertex buffer, so lock entire buffer with discard flag here
			nIndicesLockStart = 0;
		}
		if( nIndicesLockStart > 0 )
			dwIndicesLockFlag = D3DLOCK_NOOVERWRITE;
		
		ASSERT(nVertsLockStart + nVertsToLock <= m_nMaxVerts &&
			nIndicesLockStart + nIndicesToLock <= m_nMaxIndices);
		
		if( !m_pStream->LockVertexBuffer(nVertsLockStart * sizeof(A3DLMVERTEX), nVertsToLock * sizeof(A3DLMVERTEX), (BYTE **) &pVerts, dwVertsLockFlag) )
			return false;

		if( !m_pStream->LockIndexBuffer(nIndicesLockStart * sizeof(WORD), nIndicesToLock * sizeof(WORD), (BYTE **) &pIndices, dwIndicesLockFlag) )
			return false;

		pLitMesh->GetTexturePtr()->Appear(0);
		A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();
		if( pLitMesh->GetMaterial().Is2Sided() )
			m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

		pLitMesh->DrawToBuffer(pVerts, nVertsLockStart, pIndices);

		if( pLitMesh->GetAlphaValue() != 255 )
		{
			// it's a fading non alpha mesh, so alpha compare also needed
			m_pA3DDevice->SetAlphaTestEnable(true);
			m_pA3DDevice->SetAlphaRef(80 * pLitMesh->GetAlphaValue() / 255 + 1);
		}
		else
		{
			// it's a alpha mesh, so no alpha compare needed
			m_pA3DDevice->SetAlphaTestEnable(false);
		}

		if( !m_pStream->UnlockVertexBuffer() )
		{
			if( pLitMesh->GetMaterial().Is2Sided() )
				m_pA3DDevice->SetFaceCull(cullOld);
			return false;
		}
		if( !m_pStream->UnlockIndexBuffer() )
		{
			if( pLitMesh->GetMaterial().Is2Sided() )
				m_pA3DDevice->SetFaceCull(cullOld);
			return false;
		}

		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, nVertsLockStart, pLitMesh->GetNumVerts(), nIndicesLockStart, pLitMesh->GetNumFaces());

		pLitMesh->GetTexturePtr()->Disappear(0);
		if( pLitMesh->GetMaterial().Is2Sided() )
			m_pA3DDevice->SetFaceCull(cullOld);

		nVertsLockStart += pLitMesh->GetNumVerts();
		nIndicesLockStart += pLitMesh->GetNumFaces() * 3;
	}

	m_pA3DDevice->SetAlphaRef(nAlphaRefOld);
	m_pA3DDevice->SetAlphaTestEnable(bAlphaTestOld);

	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	return true;
}

//	Reset render information, this function should be called every frame
void A3DLitModelRender::ResetRenderInfo(bool bAlpha)
{
	//shadow map
	int i = 0;
	
	m_aShadowMapMeshes.RemoveAll(false);

	for( i = 0; i < m_aShadowMapSlots.GetSize(); i++)
	{
		m_aShadowMapSlots[i]->pTexture = NULL;
		m_aShadowMapSlots[i]->nNumVerts = 0;
		m_aShadowMapSlots[i]->nNumIndices = 0;
		m_aShadowMapSlots[i]->LitMeshes.RemoveAll(false);
	}

	for( i = 0; i < m_aLightmapSlots.GetSize(); i++)
	{
		m_aLightmapSlots[i]->pLightMap = NULL;
		m_aLightmapSlots[i]->pNightLightMap = NULL;
		m_aLightmapSlots[i]->pTexture = NULL;
		m_aLightmapSlots[i]->nNumVerts = 0;
		m_aLightmapSlots[i]->nNumIndices = 0;
		m_aLightmapSlots[i]->LitMeshes.RemoveAll(false);
	}

	for( i = 0; i < m_aLMVertLitRenderSlots.GetSize(); i++)
	{
		m_aLMVertLitRenderSlots[i]->pTexture = NULL;
		m_aLMVertLitRenderSlots[i]->nNumVerts = 0;
		m_aLMVertLitRenderSlots[i]->nNumIndices = 0;
		m_aLMVertLitRenderSlots[i]->LitMeshes.RemoveAll(false);
	}

	if( !bAlpha )
	{
		for( i = 0; i<m_aRenderSlots.GetSize(); i++)
		{
			m_aRenderSlots[i]->LitMeshes.RemoveAll(false);
			m_aRenderSlots[i]->pTexture = NULL;
			m_aRenderSlots[i]->pReflectTexture = NULL;
			m_aRenderSlots[i]->nNumVerts = 0;
			m_aRenderSlots[i]->nNumIndices = 0;
		}
	}
	else
	{
		if( m_idAlphaMeshArray == 0 )
			m_aAlphaMeshes.RemoveAll(false);
		else
			m_aAlphaMeshes2.RemoveAll(false);
	}
}



bool A3DLitModelRender::RenderLightPass(A3DViewport* pViewport, bool bHDR)
{
	//set render state
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);
	
	//set shader
	if( m_pLightMapVS)
		m_pLightMapVS->Appear();
	if( m_pLightMapPS)
		m_pLightMapPS->Appear();

	if(bHDR)
	{
		if( m_pLightMapLookup)
		{
			m_pLightMapLookup->Appear(3);
			m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(3,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			m_pLightMapLookup->Appear(4);
			m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(4,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			m_pLightMapLookup->Appear(5);
			m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(5,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		}
		m_pLightMapHDRPS->Appear();
	}

	A3DVECTOR4 c1(0.0f, 0.0f, 0.0f, m_fDNFactor);
	m_pA3DDevice->SetPixelShaderConstants(1, &c1, 1);

	//set vs const
	A3DMATRIX4 matVP= pViewport->GetCamera()->GetVPTM();
	A3DMATRIX4 matVPTranpose = matVP.GetTranspose();
	A3DMATRIX4 matAdjust = IdentityMatrix();

	int nWidth = pViewport->GetParam()->Width;
	int nHeight = pViewport->GetParam()->Height;
	A3DDEVFMT devFmt = m_pA3DDevice->GetDevFormat();
	matAdjust._11 = 0.5f * nWidth / devFmt.nWidth;
	matAdjust._22 = -0.5f * nHeight / devFmt.nHeight;
	
	matAdjust._41 = 0.5f + 0.5f / devFmt.nWidth;
	matAdjust._42 = 0.5f + 0.5f / devFmt.nHeight;

	A3DMATRIX4 matTex = matVP * matAdjust;
	matTex.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, &matVPTranpose, 4);
	m_pA3DDevice->SetVertexShaderConstants(4, &matTex, 4);

	
	//set stream
	m_pLMStream->Appear(0, false);

	A3DLMLMVERTEX *		pVerts;
	WORD *				pIndices;

	int					nVertsLockStart = 0;
	int					nIndicesLockStart = 0;
	int					nVertsToLock = 0;
	int					nIndicesToLock = 0;
	
	for(int i=0; i< m_aLightmapSlots.GetSize(); i++)
	{
		LightMapSlot * pSlot = m_aLightmapSlots[i];

		if(pSlot->pTexture)
		{
			pSlot->pTexture->Appear(0);
			m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		}

		if( pSlot->pLightMap != NULL )
		{
			pSlot->pLightMap->Appear(1);
			m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(1,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		}
		else
			break;

		if( pSlot->pNightLightMap != NULL)
		{	
			pSlot->pNightLightMap->Appear(2);
			m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(2,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		}

		
		DWORD dwVertsLockFlag = 0;
		DWORD dwIndicesLockFlag = 0;

		nVertsToLock = pSlot->nNumVerts;
		if( nVertsLockStart + nVertsToLock > m_nMaxVerts )
		{
			// not enough free vertex buffer, so lock entire buffer with discard flag here
			nVertsLockStart = 0;
		}
		if( nVertsLockStart > 0 )
			dwVertsLockFlag = D3DLOCK_NOOVERWRITE;
		
		nIndicesToLock = pSlot->nNumIndices;
		if( nIndicesLockStart + nIndicesToLock > m_nMaxIndices )
		{
			// not enough free vertex buffer, so lock entire buffer with discard flag here
			nIndicesLockStart = 0;
		}
		if( nIndicesLockStart > 0 )
			dwIndicesLockFlag = D3DLOCK_NOOVERWRITE;
		
		ASSERT(nVertsLockStart + nVertsToLock <= m_nMaxVerts &&
			nIndicesLockStart + nIndicesToLock <= m_nMaxIndices);
		
		if( !m_pLMStream->LockVertexBuffer(nVertsLockStart * sizeof(A3DLMLMVERTEX), nVertsToLock * sizeof(A3DLMLMVERTEX), (BYTE **) &pVerts, dwVertsLockFlag) )
		{	
			return false;
		}

		if( !m_pLMStream->LockIndexBuffer(nIndicesLockStart * sizeof(WORD), nIndicesToLock * sizeof(WORD), (BYTE **) &pIndices, dwIndicesLockFlag) )
		{
			return false;
		}

		m_nVertsCount = 0;
		m_nIndicesCount = 0;

		for(int n=0; n<pSlot->LitMeshes.GetSize(); n++)
		{
			A3DLitMesh * pLitMesh = pSlot->LitMeshes[n];
			if(!pLitMesh->IsSupportLightMap())
				continue;
			
			int nNumVerts, nNumIndices;
			nNumVerts = pLitMesh->GetNumVerts();
			nNumIndices = pLitMesh->GetNumFaces() * 3;
			
			pLitMesh->DrawLMCoordToBuffer(pVerts, nVertsLockStart + m_nVertsCount, pIndices);
			
			
			m_nVertsCount += nNumVerts;
			m_nIndicesCount += nNumIndices;
			
			pVerts += nNumVerts;
			pIndices += nNumIndices;
		}

		if( m_nVertsCount )
		{
			if( !m_pLMStream->UnlockVertexBuffer() )
			{
			
				return false;
			}
			if( !m_pLMStream->UnlockIndexBuffer() )
			{
				return false;
			}

			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, nVertsLockStart, m_nVertsCount, nIndicesLockStart, m_nIndicesCount / 3);

			pSlot->nNumVerts -= m_nVertsCount;
			pSlot->nNumIndices -= m_nIndicesCount;
		}
		else
		{
			ASSERT(0);
		}

		if(pSlot->pTexture)
		{
			pSlot->pTexture->Disappear(0);
			m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
		}

		//restore texture 
		if(pSlot->pLightMap != NULL)
		{
			m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(1,A3DTADDR_WRAP, A3DTADDR_WRAP);
			pSlot->pLightMap->Disappear(1);	
		}
		if( pSlot->pNightLightMap != NULL)
		{
			pSlot->pNightLightMap->Disappear(2);
			m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(2,A3DTADDR_WRAP, A3DTADDR_WRAP);
		}
	
		nVertsLockStart += nVertsToLock;
		nIndicesLockStart += nIndicesToLock;
	}

	//clear shaders
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();

	//restore render state
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);

	if(bHDR)
	{	
		if(m_pLightMapLookup)
		{
			m_pLightMapLookup->Disappear(3);
			m_pLightMapLookup->Disappear(4);
			m_pLightMapLookup->Disappear(5);
			m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(3,A3DTADDR_WRAP, A3DTADDR_WRAP);
			m_pA3DDevice->SetTextureAddress(4,A3DTADDR_WRAP, A3DTADDR_WRAP);
			m_pA3DDevice->SetTextureAddress(5,A3DTADDR_WRAP, A3DTADDR_WRAP);
		}

	}

	return true;

}


bool A3DLitModelRender::RenderVertLitPass(A3DViewport* pViewport)
{
		//Set Render State
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);

	//Set texture state
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG1);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);

	//clear shader
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();

	//Set stream
	m_pStream->Appear();

	if( m_bIsRenderForRefract && m_pRefractVertexShader )
	{
		m_pRefractVertexShader->Appear();
		// set vertex shader constants here
		A3DVECTOR4 c0;
		c0.x = 1.0f;
		c0.y = m_vRefractSurfaceHeight;
		c0.z = 0.075f;	// fog density
		c0.w = 0.0f;	// fog offset
		m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
		A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
		matVP.Transpose();
		m_pA3DDevice->SetVertexShaderConstants(1, &matVP, 4);
		A3DVECTOR4 c5(pViewport->GetCamera()->GetPos());
		m_pA3DDevice->SetVertexShaderConstants(5, &c5, 1);
	}
	
	//Prepare fill stream
	A3DLMVERTEX *		pVerts;
	WORD *				pIndices;

	int					nVertsLockStart = 0;
	int					nIndicesLockStart = 0;
	int					nVertsToLock = 0;
	int					nIndicesToLock = 0;
	
	for(int i=0; i<m_aLMVertLitRenderSlots.GetSize(); i++)
	{
		RENDERSLOT * pSlot = m_aLMVertLitRenderSlots[i];

		if( pSlot->pTexture == NULL )
			break; // meet empty slot here, so there is no more after this

		if(pSlot->pTexture)
		{
			pSlot->pTexture->Appear(0);
		}
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

		if( !m_bIsRenderForRefract && pSlot->pReflectTexture )
		{
			pSlot->pReflectTexture->Appear(1);
			m_pA3DDevice->SetTextureColorArgs(1, A3DTA_CURRENT, A3DTA_TEXTURE);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_MODULATEALPHA_ADDCOLOR);
			m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_CURRENT, A3DTA_TEXTURE);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_SELECTARG2);
			m_pA3DDevice->SetTextureCoordIndex(1, A3DTCI_CAMERASPACEREFLECTIONVECTOR);
			m_pA3DDevice->SetTextureTransformFlags(1, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));
			A3DMATRIX4 matCam = TransformMatrix(pViewport->GetCamera()->GetDirH(), A3DVECTOR3(0.0f, 1.0f, 0.0f), A3DVECTOR3(0.0f));
			A3DMATRIX4 matScale = IdentityMatrix();
			matScale._11 = 0.5f;
			matScale._22 = -0.5f;
			matScale._41 = 0.5f;
			matScale._42 = 0.5f;
			matScale = matCam * matScale;
			m_pA3DDevice->SetTextureMatrix(1, matScale);
		}
		else
		{
			m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
			m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
		}
		A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();
		if( pSlot->b2Sided )
			m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

		DWORD dwVertsLockFlag = 0;
		DWORD dwIndicesLockFlag = 0;

		nVertsToLock = pSlot->nNumVerts;
		if( nVertsLockStart + nVertsToLock > m_nMaxVerts )
		{
			// not enough free vertex buffer, so lock entire buffer with discard flag here
			nVertsLockStart = 0;
		}
		if( nVertsLockStart > 0 )
			dwVertsLockFlag = D3DLOCK_NOOVERWRITE;
		
		nIndicesToLock = pSlot->nNumIndices;
		if( nIndicesLockStart + nIndicesToLock > m_nMaxIndices )
		{
			// not enough free vertex buffer, so lock entire buffer with discard flag here
			nIndicesLockStart = 0;
		}
		if( nIndicesLockStart > 0 )
			dwIndicesLockFlag = D3DLOCK_NOOVERWRITE;
		
		ASSERT(nVertsLockStart + nVertsToLock <= m_nMaxVerts &&
			nIndicesLockStart + nIndicesToLock <= m_nMaxIndices);
		
		if( !m_pStream->LockVertexBuffer(nVertsLockStart * sizeof(A3DLMVERTEX), nVertsToLock * sizeof(A3DLMVERTEX), (BYTE **) &pVerts, dwVertsLockFlag) )
		{
			if( pSlot->b2Sided )
				m_pA3DDevice->SetFaceCull(cullOld);

			return false;
		}

		if( !m_pStream->LockIndexBuffer(nIndicesLockStart * sizeof(WORD), nIndicesToLock * sizeof(WORD), (BYTE **) &pIndices, dwIndicesLockFlag) )
		{
			if( pSlot->b2Sided )
				m_pA3DDevice->SetFaceCull(cullOld);

			return false;
		}

		m_nVertsCount = 0;
		m_nIndicesCount = 0;

		for(int n=0; n<pSlot->LitMeshes.GetSize(); n++)
		{
			A3DLitMesh * pLitMesh = pSlot->LitMeshes[n];
			
			int nNumVerts, nNumIndices;
			nNumVerts = pLitMesh->GetNumVerts();
			nNumIndices = pLitMesh->GetNumFaces() * 3;
			
			pLitMesh->DrawToBuffer(pVerts, nVertsLockStart + m_nVertsCount, pIndices);
			
			m_nVertsCount += nNumVerts;
			m_nIndicesCount += nNumIndices;
			
			pVerts += nNumVerts;
			pIndices += nNumIndices;
		}

		if( m_nVertsCount )
		{
			if( !m_pStream->UnlockVertexBuffer() )
			{
				if( pSlot->b2Sided )
					m_pA3DDevice->SetFaceCull(cullOld);
				return false;
			}
			if( !m_pStream->UnlockIndexBuffer() )
			{
				if( pSlot->b2Sided )
					m_pA3DDevice->SetFaceCull(cullOld);
				return false;
			}


			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, nVertsLockStart, m_nVertsCount, nIndicesLockStart, m_nIndicesCount / 3);

			pSlot->nNumVerts -= m_nVertsCount;
			pSlot->nNumIndices -= m_nIndicesCount;
		}
		else
		{
			ASSERT(0);
		}
		if(pSlot->pTexture)
		{
			pSlot->pTexture->Disappear(0);	 
		}
		if( !m_bIsRenderForRefract && pSlot->pReflectTexture )
		{
			pSlot->pReflectTexture->Disappear(1);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
			m_pA3DDevice->SetTextureCoordIndex(1, 1);
			m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
		}
		if( pSlot->b2Sided )
			m_pA3DDevice->SetFaceCull(cullOld);

		nVertsLockStart += nVertsToLock;
		nIndicesLockStart += nIndicesToLock;
	}

	if( m_bIsRenderForRefract && m_pRefractVertexShader )
	{
		m_pRefractVertexShader->Disappear();
	}

	//restore texture state
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);

	//restore render state
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	return true;
}

bool A3DLitModelRender::RenderWithLightMap(A3DViewport* pViewport, bool bHDR, bool bRenderAlpha)
{
	if( !m_pA3DDevice->TestPixelShaderVersion(1, 4))
	{
		Render(pViewport, bRenderAlpha);
		return true;
	}

	if( m_pLightMapPS == NULL || m_pLightMapVS == NULL)
		return false;
	

	//Get alphaBlend state
	bool bAlphaBlend = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) ? true : false;
	m_pA3DDevice->SetAlphaBlendEnable(false);
	
	//Light Pass
	RenderLightPass(pViewport, bHDR);

	
	//vert light pass
	RenderVertLitPass(pViewport);

	//Restore alpha blending state
	m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	if( bRenderAlpha)
		RenderAlpha(pViewport);


	return true;
}
	
void A3DLitModelRender::SetDNFactor(float f)
{
	m_fDNFactor = f;
	if( m_fDNFactor < 0.0f )
		m_fDNFactor = 0.0f;
	if( m_fDNFactor > 1.0f )
		m_fDNFactor = 1.0f;
}
bool A3DLitModelRender::RenderForRefract(A3DViewport * pViewport, float vRefractSurfaceHeight)
{
	bool bFogTable = m_pA3DDevice->GetFogTableEnable();

	m_pA3DDevice->SetFogTableEnable(false);
	m_bIsRenderForRefract = true;
	m_vRefractSurfaceHeight = vRefractSurfaceHeight;

	D3DXPLANE cp, hcp;
	m_pA3DDevice->GetD3DDevice()->GetClipPlane(0, cp);
	A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
	matVP.InverseTM();
	matVP.Transpose();
	D3DXPlaneTransform(&hcp, &cp, (D3DXMATRIX *) &matVP);
	m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, hcp);

	Render(pViewport, false);

	m_pA3DDevice->SetFogTableEnable(bFogTable);
	m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, cp);
	m_bIsRenderForRefract = false;
	return true;
}

