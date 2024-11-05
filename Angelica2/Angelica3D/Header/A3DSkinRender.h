/*
 * FILE: A3DSkinRender.h
 *
 * DESCRIPTION: A3D skin model render class
 *
 * CREATED BY: duyuxin, 2005/3/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKINRENDER_H_
#define _A3DSKINRENDER_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "AList2.h"
#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DDevice;
class A3DSkin;
class A3DMeshBase;
class A3DSkinModel;
class A3DViewport;
class A3DTexture;
struct A3DSkinMeshRef;
class A3DVertexShader;
class A3DPixelShader;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinRender
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinRender
{
public:		//	Types

	//	Render mesh node
	struct MESHNODE
	{
		A3DSkinModel*	pSkinModel;		//	Skin model object
		A3DSkin*		pSkin;			//	Skin object
		float			fDist;			//	Distance between camera and mesh (used by alpha mesh)
		A3DSkinMeshRef*	pMeshRef;		//	Mesh reference
	};

	//	Render slot
	struct RENDERSLOT
	{
		DWORD			dwTexture;		//	Texture ID

		APtrList<MESHNODE*>	SkinMeshList;	//	Skin mesh list
		APtrList<MESHNODE*> RigidMeshList;	//	Rigid mesh list
		APtrList<MESHNODE*>	SuppleMeshList;	//	Supple mesh list
		APtrList<MESHNODE*> MorphMeshList;	//	Morph mesh list
		APtrList<MESHNODE*>	MuscleMeshList;	//	Muscle mesh list
		APtrList<MESHNODE*> ClothMeshList;	//	Cloth mesh list
	};

	friend class A3DSkinMan;

public:		//	Constructors and Destructors

	A3DSkinRender();
	virtual ~A3DSkinRender();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine);
	//	Release object
	void Release();

	//	Register skin which is ready to be rendered.
	bool RegisterRenderSkin(A3DViewport* pViewport, A3DSkin* pSkin, A3DSkinModel* pSkinModel);
	//	Render all skin models
	bool Render(A3DViewport* pViewport, bool bRenderAlpha=true);
	//	Render alpha skin models
	bool RenderAlpha(A3DViewport* pViewport);
	//	Reset render information, this function should be called every frame
	void ResetRenderInfo(bool bAlpha);
	
	//	Render specified skin model at once
	bool RenderSkinModelAtOnce(A3DViewport* pViewport, A3DSkinModel* pSkinModel, DWORD dwFlags);

	//	Set / Get color operation
	void SetColorOP(A3DTEXTUREOP op) { m_ColorOP = op; }
	A3DTEXTUREOP GetColorOP() { return m_ColorOP; }

	//	Get A3D engine object
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	//	Get A3D device object
	A3DDevice* GetA3DDevice() { return m_pA3DDevice; }

	bool RenderBloomMeshes(A3DViewport* pViewport);
	void ResetBloomMeshes();

protected:	//	Attributes
	
	A3DEngine*		m_pA3DEngine;		//	A3D engine object
	A3DDevice*		m_pA3DDevice;		//	A3D device object
	A3DSkinMan*		m_psm;				//	A3D skin model manager

	APtrArray<RENDERSLOT*>	m_aRenderSlots;		//	Render slot
	APtrArray<MESHNODE*>	m_aFreeMeshNodes;	//	Free mesh node pool
	APtrArray<MESHNODE*>	m_aFreeAlphaNodes;	//	Free alpha mesh node pool
	APtrList<MESHNODE*>		m_AlphaMeshList;	//	Alpha mesh list
	APtrArray<MESHNODE*>	m_BloomMeshes;		// bloom Meshes
	

	int				m_iRenderSlotCnt;	//	Render slot counter
	int				m_iFreeMNCnt;		//	Free mesh node counter
	int				m_iFreeAMNCnt;		//	Free alpha mesh node counter
	A3DTEXTUREOP	m_ColorOP;			//	Color operation
	A3DVECTOR4		m_vVSConst0;		//	Vertex shader const 0
	A3DCULLTYPE		m_CurCull;			//	Current cull mode

	bool			m_bUserClip;		//	flag indicates whether user clip plane has been enabled
	D3DXPLANE		m_cp;				//	user clip plane in world space
	D3DXPLANE		m_hcp;				//	user clip plane in homogeneous space
	
	A3DPixelShader*		m_pBloomMeshPS;

protected:	//	Operations

	//	Release all resources
	void ReleaseAllResources();
	//	Allocate a new mesh node
	MESHNODE* AllocMeshNode(bool bAlpha);
	//	Select a proper render slot for specified mesh
	RENDERSLOT* SelectRenderSlot(A3DSkin* pSkin, A3DMeshBase* pMesh);

	//	Apply mesh material (traditional version)
	void ApplyMaterial(const MESHNODE* pNode);
	//	Apply mesh material (Vertex Shader version)
	void ApplyVSMaterial(const MESHNODE* pNode);
	//	Apply default mesh material (traditional version)
	void ApplyDefMaterial(float fTransparent);
	//	Apply default mesh material (Vertex Shader version)
	void ApplyDefVSMaterial(float fTransparent);

	//	Register mesh which is ready to be rendered.
	bool RegisterRenderMesh(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin, A3DSkinMeshRef* pMeshRef);

	//	Render skin meshes of specified slot using vertex shader
	bool RenderSkinMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render skin meshes of specified slot using indexed vertex matrix
	bool RenderSkinMeshes_IVM(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render skin meshes of specified slot using software
	bool RenderSkinMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render rigid meshes of specified slot
	bool RenderRigidMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render supple meshes of specified slot
	bool RenderSuppleMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render morph meshes of specified slot using vertex shader
	bool RenderMorphMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render morph meshes of specified slot using software
	bool RenderMorphMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render muscle meshes of specified slot using vertex shader
	bool RenderMuscleMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render muscle meshes of specified slot using software
	bool RenderMuscleMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render cloth meshes
	bool RenderClothMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render alpha meshes
	bool RenderAlphaMeshes(A3DViewport* pViewport);

	//	Render skin item in specified skin model
	bool RenderSkinAtOnce(A3DViewport* pViewport, A3DSkinModel* pModel, int iSkinItem, DWORD dwFlags);
	//	Apply mesh texture
	void ApplyMeshTexture(A3DTexture** ppLastTex, DWORD& dwTexID, const MESHNODE* pNode);
	//	Apply mesh material
	void ApplyMeshMaterial(MESHNODE* pNode, DWORD dwFlags, bool bVS);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSKINRENDER_H_

