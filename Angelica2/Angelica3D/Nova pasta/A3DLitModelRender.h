/*
 * FILE: A3DLitModelRender.h
 *
 * DESCRIPTION: A3D lit model renderer class
 *
 * CREATED BY: Hedi, 2005/4/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLITMODELRENDER_H_
#define _A3DLITMODELRENDER_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "AList2.h"
#include "AArray.h"
#include "A3DLitModel.h"
#include "A3DDevObject.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////
#define LOOKUP_MAP_SIZE 2048
#define LIGHTMAP_LOOKUP_SIZE 1024

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DDevice;
class A3DLitModel;
class A3DLitMesh;
class A3DViewport;
class A3DTexture;
class A3DStream;
class A3DPixelShader;
class A3DVertexShader;
class A3DOrthoCamera;
class A3DCamera;
class A3DCameraBase;
class A3DLight;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DLitModelRender
//
///////////////////////////////////////////////////////////////////////////

class A3DLitModelRender 
{
public:		//	Types

	//	Render slot
	struct RENDERSLOT
	{
		A3DTexture*				pTexture;		//	Texture Ptr
		A3DTexture*				pReflectTexture;//	Fake reflection ptr
		BOOL					b2Sided;		//	is two sided material or not
		A3DTexture*				pNormalMap;		//	Normal map texture
		A3DLIGHTPARAM			paramDay;		//	light param in day
		A3DLIGHTPARAM			paramNight;		//	night param at night
		int						nNumVerts;		//	number of verts of all meshes
		int						nNumIndices;	//	number of indices of all meshes
		APtrArray<A3DLitMesh*>	LitMeshes;		//	Lit mesh array
	};
	struct ShadowSlot
	{
		A3DTexture*				pTexture;
		int						nNumVerts;
		int						nNumIndices;
		APtrArray<A3DLitMesh*>	LitMeshes;
	};

	struct LightMapSlot
	{
		A3DTexture*				pLightMap;
		A3DTexture*				pNightLightMap;
		A3DTexture*				pTexture;
		int						nNumVerts;
		int						nNumIndices;
		APtrArray<A3DLitMesh*>	LitMeshes;
	};

public:		//	Constructors and Destructors

	A3DLitModelRender();
	virtual ~A3DLitModelRender();

protected:		//	Attributes
	int							m_nMaxVerts;	// max verts in stream
	int							m_nMaxIndices;	// max indices in stream
	int							m_nVertsCount;	// current verts in stream
	int							m_nIndicesCount;// current indices in stream
	A3DStream *					m_pStream;		// stream used to do the render
	A3DStream*					m_pLMStream;

	bool						m_bIsRenderForRefract;	// flag indicates we are rendering for refract now
	float						m_vRefractSurfaceHeight;// current water surface height
	A3DVertexShader *			m_pRefractVertexShader;	// vertex shader used to do refract rendering

	//shadowmap
	bool						m_bUseShadowMap;
	A3DTexture*					m_pLookupMap;	
	A3DVertexShader*			m_pShadowMapVS;
	A3DPixelShader*				m_pShadowMapPS;
	A3DPixelShader*				m_pShadowMapPSNoAlpha;
	A3DVertexShader*			m_pLitModelShadowVS;
	A3DPixelShader*				m_pLitModelShadowPS;
	//z-bias
	float						m_fZBias;
	float						m_fShadowLum;
	//alpha texture mesh
	A3DVertexShader*			m_pAlphaTextureVS;
	A3DPixelShader*				m_pAlphaTexturePS;
	
	
#ifdef LM_BUMP_ENABLE
	A3DPixelShader *			m_pBumpPixelShader;		// pixel shader used to do per pixel lighting
	A3DVertexShader *			m_pBumpVertexShader;	// vertex shader used to do per pixel lighting
	A3DTexture *				m_pSpecularMap;			// 1D texture used for specular value look up
#endif

	bool CreateStream(int nMaxVerts, int nMaxIndices);
	bool ReleaseStream();

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine, int nMaxVerts, int nMaxIndices);
	
	//	Release object
	void Release();

	//	Register mesh which is ready to be rendered.
	bool RegisterRenderMesh(A3DViewport* pViewport, A3DLitMesh * pLitMesh, bool bAlphaFlag=false);

	//	Render all lit models
	bool Render(A3DViewport* pViewport, 
		bool bRenderAlpha=true, 
		bool bUseShadowMap = false,
		A3DLight* pSunLight = NULL,
		float fZBias = 0.0005f, 
		float fShadowLum = 0.2f);
	//	Render all lit models for refract 
	bool RenderForRefract(A3DViewport* pViewport, float vRefractSurfaceHeight);
	//	Render alpha lit models
	bool RenderAlpha(A3DViewport* pViewport);
	//	Reset render information, this function should be called every frame
	void ResetRenderInfo(bool bAlpha);

	// render shadow map
	bool InitShadowResource();
	void ReleaseShadowResource();
	bool RegisterRenderShadowMapMesh(A3DLitMesh* pLitMesh);
	bool ClipShadowMapMeshes(A3DOrthoCamera* pOrthoCamera);
	bool FillShadowMapSlots(A3DLitMesh* pLitMesh);

	bool RenderShadowMap(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius, const float fShadowRealRadius);
	bool RenderLitModelWithShadow(  A3DViewport* pViewport, 
									A3DLight* pSunLight,
									float fZBias, 
									float fShadowLum);
	bool RenderLitModelAlphaMeshes(A3DViewport* pViewport);
	inline bool IsUseShadowMap() { return m_bUseShadowMap;}
	float GetZBias() { return m_fZBias;}
	float GetShadowLum() { return m_fShadowLum;}

	//light map
	bool RegisterLightMapMesh(A3DViewport* pViewport, A3DLitMesh* pLitMesh);
	bool RegisterVertLitMesh(A3DViewport* pViewport, A3DLitMesh* pLitMesh);
	bool RenderWithLightMap(A3DViewport* pViewport, bool bHDR = true, bool bRenderAlpha = false);
	bool RenderLightPass(A3DViewport* pViewport, bool bHDR = true);
	bool RenderVertLitPass(A3DViewport* pViewport);

protected:

	//render shadow
	bool FillShadowCasterStream(int& nNumVerts, int& nNumIndices, int nSlot);
	bool FillShadowReceiverStream(int& nNumVerts, int& nNumIndices, int nSlot);
	void CaculateShadowMapRadius(float& fRadius, A3DVECTOR3& vCenter);
	void SetLitModelShadowVSConst(A3DViewport* pViewport, A3DVECTOR3* pLightDir, A3DCOLORVALUE* pLightColor, float fZBias);


protected:	//	Attributes
	
	A3DEngine*		m_pA3DEngine;		//	A3D engine object
	A3DDevice*		m_pA3DDevice;		//	A3D device object

	float						m_fDNFactor;		// day or night factor, 0.0f is day; 1.0f is night, 0.0f~1.0f is just the transition

	APtrArray<RENDERSLOT*>		m_aRenderSlots;		// Render slot
	APtrArray<A3DLitMesh *>		m_aAlphaMeshes;		// alpha meshes
	APtrArray<A3DLitMesh *>		m_aAlphaMeshes2;	// alpha meshes in another pass (eg. reflect or refract)
	int							m_idAlphaMeshArray;	// id of alpha meshes array

	//shadow map
	APtrArray<ShadowSlot*>      m_aShadowMapSlots;	// shadow map slots
	APtrArray<A3DLitMesh*>		m_aShadowMapMeshes; // shadow map meshes

	//light map
	APtrArray<LightMapSlot*>	m_aLightmapSlots;
	APtrArray<RENDERSLOT*>		m_aLMVertLitRenderSlots;
	A3DVertexShader*			m_pLightMapVS;
	A3DVertexShader*			m_pLightMapTexPassVS;
	A3DPixelShader*				m_pLightMapPS;
	A3DPixelShader*				m_pLightMapHDRPS;
	A3DTexture*					m_pLightMapLookup;

public:
	void SetAlphaMeshesArray(int id)	{ m_idAlphaMeshArray = id; }
	inline float GetDNFactor()			{ return m_fDNFactor; }
	void SetDNFactor(float f);

#ifdef LM_BUMP_ENABLE
	A3DPixelShader * GetBumpPixelShader() { return m_pBumpPixelShader; }
	A3DVertexShader * GetBumpVertexShader() { return m_pBumpVertexShader; }
#endif
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif//_A3DLITMODELRENDER_H_

