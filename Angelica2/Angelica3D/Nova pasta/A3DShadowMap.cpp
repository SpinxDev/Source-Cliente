/*
 * FILE: A3DShadowMap.cpp
 *
 * DESCRIPTION: shadow map
 *
 * CREATED BY: Liyi, 2010/4/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */
#include "A3DShadowMap.h"

#include "A3DDevice.h"
#include "A3DRenderTarget.h"
#include "A3DOrthoCamera.h"
#include "A3DViewport.h"
#include "A3DMacros.h"
#include "A3DPI.h"
#include "A3DTypes.h"
#include "A3DTexture.h"
#include "A3DSurface.h"

namespace A3D
{
	A3DShadowMap g_ShadowMap;
	A3DShadowMap* g_pShadowMap = &g_ShadowMap;
}

A3DShadowMap::A3DShadowMap()
:m_pShadowMap(NULL),
m_pShadowCamera(NULL),
m_pShadowViewport(NULL),
m_nShadowMapSize(2048),
m_pA3DDevice(NULL),
m_bInit(false),
m_bOptimize(false),
m_bTerrainShadowOnePass(false),
m_pShadowTexture(NULL),
m_pShadowTextureCamera(NULL),
m_pTerrainShadowMapL8(NULL),
m_pTerrainShadowViewport(NULL),
m_pTerrainShadowCamera(NULL),
m_nTerrainShadowMapDivision(2),
m_nTerrainShadowMapTileCount(4),
m_nTerrainShadowMapCurTile(-1),
m_nTerrainShadowMapTileSize(512)
{
	memset(m_ppShadowCaches, 0, sizeof(A3DSurface*) * SHADOW_CACHE_NUM);
}

A3DShadowMap::~A3DShadowMap()
{
}

bool A3DShadowMap::Init(A3DDevice* pA3DDevice, int nShadowMapSize, bool bOptimize)
{
	Release();

	m_pA3DDevice = pA3DDevice;
	m_nShadowMapSize = nShadowMapSize;
	m_bOptimize = bOptimize;

	HRESULT hr;

	//Create shadow map
	m_pShadowMap = new A3DRenderTarget();
	A3DDEVFMT devFmt;
	devFmt.nHeight = m_nShadowMapSize;
	devFmt.nWidth  = m_nShadowMapSize;
	devFmt.fmtTarget = A3DFMT_A8R8G8B8;
	devFmt.fmtDepth = A3DFMT_D24X8;
	if(!m_pShadowMap->Init(m_pA3DDevice, devFmt, true, true))
	{
		g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create shadow map rendertarget!");
		goto FAILED;
	}

	//Create Viewport
	if( !m_pA3DDevice->CreateViewport(&m_pShadowViewport, 0, 0, m_nShadowMapSize, m_nShadowMapSize, 
		0.0f, 1.0f, true, true, 0xFF000000) )
	{
		g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create shadow map viewport!");
		goto FAILED;
	}
	
	//Create shadow camera
	m_pShadowCamera = new A3DOrthoCamera();
	if( !m_pShadowCamera->Init(m_pA3DDevice, -1.0f, 1.0f, -1.0f, 1.0f, 0.2f, 2000.0f))
	{
		g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create shadow map camera!");
		goto FAILED;
	}
	m_pShadowViewport->SetCamera(m_pShadowCamera);
	
	// many cards do not support l8, so change to r5g6b5
	hr = m_pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		(D3DFORMAT)m_pA3DDevice->GetDevFormat().fmtAdapter,
		D3DUSAGE_RENDERTARGET,
		D3DRTYPE_TEXTURE,
		D3DFMT_R5G6B5);

	if (SUCCEEDED(hr))
	{
		int nSize = GetShadowMapSize();
		m_pTerrainShadowMapL8 = new A3DRenderTarget();
		devFmt.nWidth = nSize;
		devFmt.nHeight = nSize;
		devFmt.fmtTarget = A3DFMT_R5G6B5;
		devFmt.fmtDepth = A3DFMT_D24X8;

		if (!m_pTerrainShadowMapL8->Init(m_pA3DDevice, devFmt, true, false))
		{
			g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create terrain shadow map rendertarget!");
			goto FAILED;
		}

		// Create Viewport
		if (!m_pA3DDevice->CreateViewport(&m_pTerrainShadowViewport, 0, 0, nSize, nSize,
			0.0f, 1.0f, true, true, 0xFF000000))
		{
			g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create terrain shadow map viewport!");
			goto FAILED;
		}
		
		// Create shadow camera
		m_pTerrainShadowCamera = new A3DOrthoCamera();

		if (!m_pTerrainShadowCamera->Init(m_pA3DDevice, -1.0f, 1.0f, -1.0f, 1.0f, 0.2f, 2000.0f))
		{
			g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create terrain shadow map camera!");
			goto FAILED;
		}

		m_pTerrainShadowViewport->SetCamera(m_pTerrainShadowCamera);
		m_nTerrainShadowMapTileSize = nSize / m_nTerrainShadowMapDivision;
		m_bTerrainShadowOnePass = true;
		g_A3DErrLog.Log("A3DShadowMap::Init(), Chosen rendering terrain shadow in one pass");
	}

	//create optimize resource
	if( bOptimize)
	{
		if(!InitOptimize())
			goto FAILED;
	}

	m_bInit = true;

	return true;

FAILED:
	Release();
	m_bInit =false;
	return false;
}

bool A3DShadowMap::InitOptimize()
{
	//create shadow caches
	for(int i = 0; i < SHADOW_CACHE_NUM; i++)
	{
		m_ppShadowCaches[i] = new A3DSurface;
		if( !m_ppShadowCaches[i]->Create(m_pA3DDevice, m_nShadowMapSize, m_nShadowMapSize, A3DFMT_A8R8G8B8))
		{
			g_A3DErrLog.Log("A3DShadowMap::InitOptimize(), Failed to create shadow map caches!");
			return false;
		}
	}

	//create shadow texture
	m_pShadowTexture = new A3DTexture;
	if(!m_pShadowTexture->Create(m_pA3DDevice, m_nShadowMapSize * SHADOW_CACHE_COL, m_nShadowMapSize * SHADOW_CACHE_ROW,  A3DFMT_A8R8G8B8, 1))
	{
		g_A3DErrLog.Log("A3DShadowMap::c(), Failed to create shadow texture!");
		return false;
	}

		//Create shadow  texture camera
	m_pShadowTextureCamera = new A3DOrthoCamera();
	if( !m_pShadowTextureCamera->Init(m_pA3DDevice, -1.0f, 1.0f, -1.0f, 1.0f, 0.2f, 2000.0f))
	{
		g_A3DErrLog.Log("A3DShadowMap::m_pShadowTextureCamera(), Failed to create shadow texture camera!");
		return false;
	}

	return true;
}

void A3DShadowMap::Release()
{
	A3DRELEASE(m_pShadowMap);
	A3DRELEASE(m_pShadowCamera);
	A3DRELEASE(m_pShadowViewport);

	A3DRELEASE(m_pTerrainShadowMapL8);
	A3DRELEASE(m_pTerrainShadowCamera);
	A3DRELEASE(m_pTerrainShadowViewport);

	for( int i = 0; i < SHADOW_CACHE_NUM; i++)
	{
		A3DRELEASE(m_ppShadowCaches[i]);
	}
	A3DRELEASE(m_pShadowTexture);
	A3DRELEASE(m_pShadowTextureCamera);

	m_bInit = false;
	m_bOptimize = false;
	m_bTerrainShadowOnePass = false;
}

void A3DShadowMap::SetShadowCamera(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius,const float fShadowRealRadius)
{
	//m_pShadowCamera->SetProjectionParam(-fShadowRadius, fShadowRadius, -fShadowRadius, fShadowRadius, 0.2f, fShadowRadius * 4.0f);
	m_pShadowCamera->SetProjectionParam(-fShadowRadius, fShadowRadius, -fShadowRadius, fShadowRadius, 0.2f, fShadowRealRadius * 4.0f);

	A3DVECTOR3 vOffset = (*pSunLightDir) * fShadowRealRadius * 2.0f;
	m_pShadowCamera->SetPos(*pShadowCenter +  vOffset);
	A3DVECTOR3 vCameraDir = -(*pSunLightDir);
	m_pShadowCamera->SetDirAndUp(vCameraDir , A3D::g_vAxisY);
}

void A3DShadowMap::SetupTerrainShadowMapViewport(const A3DVECTOR3& vShadowCenter, float fShadowRealRadius, const A3DVECTOR3& vSunLightDir, bool bUpdateTotal, bool bActive)
{
	if (bUpdateTotal)
	{
		m_pTerrainShadowCamera->SetProjectionParam(-fShadowRealRadius, fShadowRealRadius, -fShadowRealRadius, fShadowRealRadius, 0.2f, fShadowRealRadius * 6.0f);
		m_pTerrainShadowCamera->SetPos(vShadowCenter + fShadowRealRadius * 3.0f * A3D::g_vAxisY);
		m_pTerrainShadowCamera->SetDirAndUp(-A3D::g_vAxisY, vSunLightDir);

		int nSize = GetShadowMapSize();
		A3DVIEWPORTPARAM param;
		param.X = 0;
		param.Y = 0;
		param.Width = nSize;
		param.Height = nSize;
		param.MinZ = 0;
		param.MaxZ = 1.0f;
		m_pTerrainShadowViewport->SetParam(&param);

		if (bActive)
		{
			m_pTerrainShadowViewport->Active();
			m_pA3DDevice->Clear(D3DCLEAR_TARGET, 0xffffffff, 0, 0);
		}
	}
	else
	{
		m_pTerrainShadowCamera->SetPos(vShadowCenter + fShadowRealRadius * 3.0f * A3D::g_vAxisY);
		m_pTerrainShadowCamera->SetDirAndUp(-A3D::g_vAxisY, vSunLightDir);

		int row = m_nTerrainShadowMapCurTile / m_nTerrainShadowMapDivision;
		int col = m_nTerrainShadowMapCurTile % m_nTerrainShadowMapDivision;
		float fDivisionSize = fShadowRealRadius * 2.f / m_nTerrainShadowMapDivision;

		float cl = (col - 1) * fDivisionSize;
		float cr = cl + fDivisionSize;
		float ct = (1 - row) * fDivisionSize;
		float cb = ct - fDivisionSize;

		m_pTerrainShadowCamera->SetProjectionParam(cl, cr, cb, ct, 0.2f, fShadowRealRadius * 6.0f);

		A3DVIEWPORTPARAM param;
		param.X = col * m_nTerrainShadowMapTileSize;
		param.Y = row * m_nTerrainShadowMapTileSize;
		param.Width = m_nTerrainShadowMapTileSize;
		param.Height = m_nTerrainShadowMapTileSize;
		param.MinZ = 0;
		param.MaxZ = 1.0f;
		m_pTerrainShadowViewport->SetParam(&param);

		if (bActive)
		{
			m_pTerrainShadowViewport->Active();

			if (m_nTerrainShadowMapCurTile == 0)
				m_pA3DDevice->Clear(D3DCLEAR_TARGET, 0xffffffff, 0, 0);
		}
	}
}

bool A3DShadowMap::SetShadowTextureCamera(const A3DVECTOR3* pShadowRealCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRealRadius)
{
	if(m_pShadowTextureCamera)
	{
		//shadow texture camera
		m_pShadowTextureCamera->SetProjectionParam(-fShadowRealRadius, fShadowRealRadius, -fShadowRealRadius, fShadowRealRadius, 0.2f, fShadowRealRadius * 4.0f);

		A3DVECTOR3 vOffset = (*pSunLightDir) * fShadowRealRadius * 2.0f;
		m_pShadowTextureCamera->SetPos(*pShadowRealCenter +  vOffset);
		A3DVECTOR3 vCameraDir = -(*pSunLightDir);
		m_pShadowTextureCamera->SetDirAndUp(vCameraDir , A3D::g_vAxisY);
		return true;
	}
	return false;

}

void A3DShadowMap::AppearAsTexture(int nLayer)
{
	if(m_bOptimize)
		m_pShadowTexture->Appear(nLayer);
	else
		m_pShadowMap->AppearAsTexture(nLayer);	
}

void A3DShadowMap::DisappearAsTexture(int nLayer)
{
	if(m_bOptimize)
		m_pShadowTexture->Disappear(nLayer);
	else
		m_pShadowMap->DisappearAsTexture(nLayer);	
}

bool A3DShadowMap::IncorporateShadowTexture()
{
	if( !m_bOptimize)
		return false;

	IDirect3DSurface8* pShadowSurface = NULL;
	if( D3D_OK != m_pShadowTexture->GetD3DTexture()->GetSurfaceLevel(0, &pShadowSurface))
		return false;

	POINT destPoint;
	RECT rc;
	rc.left = 0;
	rc.right = m_nShadowMapSize;
	rc.top = 0;
	rc.bottom = m_nShadowMapSize;

	for( int nRow = 0; nRow < SHADOW_CACHE_ROW; nRow++)
	{
		for( int nCol = 0; nCol < SHADOW_CACHE_COL; nCol++)
		{

			destPoint.x = nCol * m_nShadowMapSize;
			destPoint.y = nRow * m_nShadowMapSize;

			if (D3D_OK != m_pA3DDevice->GetD3DDevice()->CopyRects(m_ppShadowCaches[nRow* SHADOW_CACHE_COL + nCol]->GetDXSurface(), &rc, 1,pShadowSurface, &destPoint))
			{
				assert(false);
			}
		}
	}

	pShadowSurface->Release();
	return true;
}

bool A3DShadowMap::Set(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir,const float fShadowRadius, const float fShadowRealRadius)
{
	m_pA3DDevice->SetRenderTarget(m_pShadowMap);

	SetShadowCamera(pShadowCenter, pSunLightDir, fShadowRadius,fShadowRealRadius);

	m_pShadowViewport->Active();

	return true;
}

bool A3DShadowMap::Restore()
{
	m_pA3DDevice->RestoreRenderTarget();

	return true;
}

bool A3DShadowMap::ClearDevice( A3DCOLOR clearColor/* = 0xFFFFE000 */)
{
	m_pShadowViewport->SetClearColor(clearColor);
	m_pShadowViewport->ClearDevice();

	return true;
}

bool A3DShadowMap::Cache(int nCurCache)
{
	if( m_bOptimize)
	{
		if( D3D_OK != m_pA3DDevice->GetD3DDevice()->CopyRects( m_pShadowMap->GetTargetSurface(), NULL, 0, m_ppShadowCaches[nCurCache]->GetDXSurface(), NULL))
			return false;
	}
	return true;
}

int A3DShadowMap::GetShadowMapSize()
{
	if( m_bOptimize)
		return m_nShadowMapSize * SHADOW_CACHE_COL;
	else
		return m_nShadowMapSize;
}