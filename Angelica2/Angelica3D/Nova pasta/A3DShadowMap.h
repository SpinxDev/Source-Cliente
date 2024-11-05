/*
 * FILE: A3DShadowMap.h
 *
 * DESCRIPTION: shadow map
 *
 * CREATED BY: Liyi, 2010/4/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3D_SHADOW_MAP_H_
#define _A3D_SHADOW_MAP_H_

#include "A3DTypes.h"

class A3DDevice;
class A3DRenderTarget;
class A3DVECTOR3;
class A3DViewport;
class A3DOrthoCamera;
class A3DTexture;
class A3DSurface;

#define SHADOW_CACHE_NUM 9
#define SHADOW_CACHE_ROW 3
#define SHADOW_CACHE_COL 3

class A3DShadowMap
{
public:
	A3DShadowMap();
	~A3DShadowMap();

	bool Init(A3DDevice* pA3DDevice, int nShadowMapSize, bool bOptimize = false);
	void Release();

	bool Set(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius,const float fShadowRealRadius);
	bool Restore();
	bool Cache(int nCurCache);

	bool ClearDevice( A3DCOLOR clearColor = 0xFFFFE000);

	void AppearAsTexture(int nLayer);
	void DisappearAsTexture(int nLayer);

	inline bool				IsInit() { return m_bInit;}
	inline bool				IsOptimize() { return m_bOptimize;}

	inline A3DRenderTarget*	GetShadowMap(){ return m_pShadowMap;}
	inline A3DTexture*		GetShadowTexture() { return m_pShadowTexture;}
	inline A3DViewport*		GetShadowViewport() { return m_pShadowViewport;}
	inline A3DOrthoCamera*	GetShadowCamera() { return m_pShadowCamera;}
	inline A3DOrthoCamera*  GetShadowTextureCamera() { return m_pShadowTextureCamera;}

	inline A3DRenderTarget* GetTerrainShadowMap() { return m_pTerrainShadowMapL8; }
	inline A3DViewport*		GetTerrainShadowViewport() { return m_pTerrainShadowViewport; }
	inline A3DOrthoCamera*	GetTerrainShadowCamera() { return m_pTerrainShadowCamera; }
	inline bool				IsTerrainShadowMapReady() const { return m_nTerrainShadowMapCurTile >= m_nTerrainShadowMapTileCount - 1; }
	inline void				SetTerrainShadowMapReady() { m_nTerrainShadowMapCurTile = m_nTerrainShadowMapTileCount - 1; }
	inline void				SetTerrainShadowMapDirty() { m_nTerrainShadowMapCurTile = -2; }
	inline bool				CanRenderTerrainShadowOnePass() const { return m_bTerrainShadowOnePass; }

	inline bool IncTerrainShadowMapTileIndex()
	{
		if (m_nTerrainShadowMapCurTile >= m_nTerrainShadowMapTileCount - 1)
			return false;

		m_nTerrainShadowMapCurTile++;
		return m_nTerrainShadowMapCurTile >= 0;
	}

	void SetupTerrainShadowMapViewport(const A3DVECTOR3& vShadowCenter, float fShadowRealRadius, const A3DVECTOR3& vSunLightDir, bool bUpdateTotal, bool bActive);

	bool SetShadowTextureCamera(const A3DVECTOR3* pShadowRealCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRealRadius);
	int GetShadowMapSize();

	void SetShadowCamera(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius, const float fShadowRealRadius );
	bool IncorporateShadowTexture();
	
protected:

	bool InitOptimize();

protected:
	A3DDevice*			m_pA3DDevice;

	A3DRenderTarget*	m_pShadowMap;
	A3DSurface*			m_ppShadowCaches[SHADOW_CACHE_NUM];
	A3DTexture*			m_pShadowTexture;
	
	A3DViewport*		m_pShadowViewport;
	A3DOrthoCamera*		m_pShadowCamera;
	A3DOrthoCamera*		m_pShadowTextureCamera;

	A3DRenderTarget*	m_pTerrainShadowMapL8;
	A3DViewport*		m_pTerrainShadowViewport;
	A3DOrthoCamera*		m_pTerrainShadowCamera;
	int					m_nTerrainShadowMapDivision;
	int					m_nTerrainShadowMapTileCount;
	int					m_nTerrainShadowMapCurTile;
	int					m_nTerrainShadowMapTileSize;

	int					m_nShadowMapSize;
	bool				m_bInit;
	bool				m_bOptimize;
	bool				m_bTerrainShadowOnePass;
};

namespace A3D
{
	extern A3DShadowMap g_ShadowMap;
	extern A3DShadowMap* g_pShadowMap;
}

#endif //_A3D_SHADOW_MAP_H_