 /*
 * FILE: A3DSkinRender.cpp
 *
 * DESCRIPTION: A3D skin model render class
 *
 * CREATED BY: duyuxin, 2005/3/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DSkinRender.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DVSDef.h"
#include "A3DSkin.h"
#include "A3DSkinModel.h"
#include "A3DSkinMesh.h"
#include "A3DRigidMesh.h"
#include "A3DMorphRigidMesh.h"
#include "A3DMorphSkinMesh.h"
#include "A3DMuscleMesh.h"
#include "A3DClothMesh.h"
#include "A3DTexture.h"
#include "A3DMaterial.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DVertexShader.h"
#include "A3DCameraBase.h"
#include "A3DLight.h"
#include "A3DViewport.h"
#include "A3DShaderMan.h"
#include "A3DShader.h"
#include "A3DSkeleton.h"
#include "A3DSkinMan.h"
#include "AFileImage.h"
#include "AIniFile.h"
#include "AFI.h"
#include "AAssist.h"
#include "AMemory.h"
#include "A3DPixelShader.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement of A3DSkinRender
//
///////////////////////////////////////////////////////////////////////////

A3DSkinRender::A3DSkinRender() : 
m_aFreeMeshNodes(0, 200), 
m_aFreeAlphaNodes(0, 200),
m_BloomMeshes(0, 200),
m_vVSConst0(1.0f, 0.0f, 0.0f, 765.01f)
{
	m_pA3DEngine		= NULL;
	m_pA3DDevice		= NULL;
	m_psm		= NULL;
	m_iRenderSlotCnt	= 0;
	m_iFreeMNCnt		= 0;
	m_iFreeAMNCnt		= 0;
	m_ColorOP			= A3DTOP_MODULATE;
	m_pBloomMeshPS		= NULL;
}

A3DSkinRender::~A3DSkinRender()
{
}

//	Initialize object
bool A3DSkinRender::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);
	m_pA3DEngine	= pA3DEngine;
	m_pA3DDevice	= pA3DEngine->GetA3DDevice();
	m_psm	= pA3DEngine->GetA3DSkinMan();

	A3DShaderMan* pShaderMan = pA3DEngine->GetA3DShaderMan();

	const char* szShader = "shaders\\ps\\bloom_mesh.txt";
	if(af_IsFileExist(szShader))
	{
		m_pBloomMeshPS = pShaderMan->LoadPixelShader(szShader, false);
		if( m_pBloomMeshPS == NULL)
		{
			g_A3DErrLog.Log("A3DSkinRender::Init(), Failed to create bloom mesh pixel shader!");
			return false;
		}
	}
		

	return true;
}

//	Release object
void A3DSkinRender::Release()
{
	int i;

	//	Release render slots
	for (i=0; i < m_aRenderSlots.GetSize(); i++)
		delete m_aRenderSlots[i];

	m_aRenderSlots.RemoveAll();

	//	Release free mesh node pool
	for (i=0; i < m_aFreeMeshNodes.GetSize(); i++)
		delete m_aFreeMeshNodes[i];

	m_aFreeMeshNodes.RemoveAll();

	//	Release free alpha node pool
	for (i=0; i < m_aFreeAlphaNodes.GetSize(); i++)
		delete m_aFreeAlphaNodes[i];

	m_aFreeAlphaNodes.RemoveAll();

	m_iRenderSlotCnt	= 0;
	m_iFreeMNCnt		= 0;
	m_iFreeAMNCnt		= 0;

	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();
	if( pShaderMan )
	{
		if( m_pBloomMeshPS != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pBloomMeshPS);
			m_pBloomMeshPS = NULL;
		}
	}

	ResetBloomMeshes();

	m_psm			= NULL;
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
}

//	Allocate a new mesh node
A3DSkinRender::MESHNODE* A3DSkinRender::AllocMeshNode(bool bAlpha)
{
	MESHNODE* pMeshNode;

	if (bAlpha)
	{
		if (m_iFreeAMNCnt >= m_aFreeAlphaNodes.GetSize())
		{
			//	Allocate a new mesh node
			if (!(pMeshNode = new MESHNODE))
			{
				g_A3DErrLog.Log("A3DSkinRender::AllocMeshNode, Not enough memory !");
				return NULL;
			}

			m_aFreeAlphaNodes.Add(pMeshNode);
		}
		else
			pMeshNode = m_aFreeAlphaNodes[m_iFreeAMNCnt];

		m_iFreeAMNCnt++;
	}
	else
	{
		if (m_iFreeMNCnt >= m_aFreeMeshNodes.GetSize())
		{
			//	Allocate a new mesh node
			if (!(pMeshNode = new MESHNODE))
			{
				g_A3DErrLog.Log("A3DSkinRender::AllocMeshNode, Not enough memory !");
				return NULL;
			}

			m_aFreeMeshNodes.Add(pMeshNode);
		}
		else
			pMeshNode = m_aFreeMeshNodes[m_iFreeMNCnt];

		m_iFreeMNCnt++;
	}

	return pMeshNode;
}

//	Reset render information, this function should be called every frame
void A3DSkinRender::ResetRenderInfo(bool bAlpha)
{
	if (bAlpha)
	{
		//	Reset alpha mesh list
		m_AlphaMeshList.RemoveAll();
		m_iFreeAMNCnt = 0;
	}
	else
	{
		for (int i=0; i < m_iRenderSlotCnt; i++)
		{
			RENDERSLOT* pSlot = m_aRenderSlots[i];

			pSlot->MorphMeshList.RemoveAll();
			pSlot->RigidMeshList.RemoveAll();
			pSlot->SuppleMeshList.RemoveAll();
			pSlot->SkinMeshList.RemoveAll();
			pSlot->MuscleMeshList.RemoveAll();
			pSlot->ClothMeshList.RemoveAll();
		}

		m_iRenderSlotCnt = 0;
		m_iFreeMNCnt	 = 0;
	}
}

void A3DSkinRender::ResetBloomMeshes()
{
	for( int i = 0; i < m_BloomMeshes.GetSize(); i++)
	{
		delete m_BloomMeshes[i];
	}
	m_BloomMeshes.RemoveAll();
}

float GetNearestDistToAABB(const A3DAABB& aabb, const A3DMATRIX4& mat, const A3DVECTOR3& pos)
{
	float vMinDist = 999999.9f;
	float vDist = 0.0f;
	A3DVECTOR3 v;

	v = A3DVECTOR3(aabb.Mins.x, aabb.Mins.y, aabb.Mins.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Mins.x, aabb.Mins.y, aabb.Maxs.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Mins.x, aabb.Maxs.y, aabb.Mins.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Mins.x, aabb.Maxs.y, aabb.Maxs.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Maxs.x, aabb.Mins.y, aabb.Mins.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Maxs.x, aabb.Mins.y, aabb.Maxs.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Maxs.x, aabb.Maxs.y, aabb.Mins.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Maxs.x, aabb.Maxs.y, aabb.Maxs.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	return vMinDist;
}

//	Register mesh which is ready to be rendered.
bool A3DSkinRender::RegisterRenderMesh(A3DViewport* pViewport, A3DSkinModel* pSkinModel,
						A3DSkin* pSkin, A3DSkinMeshRef* pMeshRef)
{
	// see if totally transparent or invisible flag is set
	if (pMeshRef->fTrans >= 1.0f || pMeshRef->bInvisible)
		return true;

	//Add Bloom Mesh
	if( pSkin->IsBloomMesh())
	{
		MESHNODE* pNewNode = new MESHNODE;
		pNewNode->pSkinModel = pSkinModel;
		pNewNode->pSkin = pSkin;
		pNewNode->pMeshRef = pMeshRef;
		pNewNode->fDist = 0.0f;

		m_BloomMeshes.Add(pNewNode);
	}

	bool bAlphaShader = false;
	A3DTexture * pTexture = pMeshRef->pMesh->GetTexturePtr(pSkin);
	if( pTexture && pTexture->IsShaderTexture() && pTexture->IsAlphaTexture() )
		bAlphaShader = true;

	if (pMeshRef->fTrans >= 0.0f || pSkinModel->GetSrcBlend() != A3DBLEND_SRCALPHA ||
		pSkinModel->GetDstBlend() != A3DBLEND_INVSRCALPHA || bAlphaShader)
	{
		//	Allocate a new mesh node
		MESHNODE* pNewNode = AllocMeshNode(true);
		if (!pNewNode)
			return false;

		pNewNode->pSkinModel	= pSkinModel;
		pNewNode->pSkin			= pSkin;
		pNewNode->pMeshRef		= pMeshRef;

		//	Register alpha mesh
		A3DMATRIX4 mat = pSkinModel->GetAbsoluteTM();
		pNewNode->fDist = GetNearestDistToAABB(pMeshRef->pMesh->GetInitMeshAABB(), mat, pViewport->GetCamera()->GetPos());

		ALISTPOSITION pos = m_AlphaMeshList.GetHeadPosition();
		while (pos)
		{
			MESHNODE* pTempNode = m_AlphaMeshList.GetAt(pos);

			// first see if the skin are belonging to the same skin model
			if( pTempNode->pSkinModel == pNewNode->pSkinModel )
			{
				int iWeight1 = (pTempNode->pMeshRef->iAlphaWeight == 0) ? pTempNode->pSkin->GetAlphaSortWeight() : pTempNode->pMeshRef->iAlphaWeight;
				int iWeight2 = (pNewNode->pMeshRef->iAlphaWeight == 0) ? pNewNode->pSkin->GetAlphaSortWeight() : pNewNode->pMeshRef->iAlphaWeight;


				//	Compare alpha sort weight first
				if(iWeight1 != iWeight2)
				{
					if (iWeight2 < iWeight1)
					{
							m_AlphaMeshList.InsertBefore(pos, pNewNode);
							break;
						
					}
				}
				else
				{
					// now the situation is simple, we just need to compare the aabb along main axis, which is z axis in fact
					const A3DAABB& aabb1 = pTempNode->pMeshRef->pMesh->GetInitMeshAABB();
					const A3DAABB& aabb2 = pNewNode->pMeshRef->pMesh->GetInitMeshAABB();

					if( aabb1.Maxs.y > aabb2.Maxs.y || aabb1.Maxs.z > aabb2.Maxs.z || aabb1.Mins.z < aabb2.Mins.z )
					{
						m_AlphaMeshList.InsertBefore(pos, pNewNode);
						break;
					}
				}
			}
			else if (pTempNode->pSkinModel->GetAlphaSortID() != 0 &&
					pTempNode->pSkinModel->GetAlphaSortID() == pNewNode->pSkinModel->GetAlphaSortID())
			{
				if (pNewNode->pSkinModel->GetAlphaSortWeight() < pTempNode->pSkinModel->GetAlphaSortWeight())
				{
					m_AlphaMeshList.InsertBefore(pos, pNewNode);
					break;
				}
			}
			else // coming from different models, so we need to compare the distance
			{
				if (pTempNode->fDist <= pNewNode->fDist)
				{
					m_AlphaMeshList.InsertBefore(pos, pNewNode);
					break;
				}
			}

			m_AlphaMeshList.GetNext(pos);
		}

		if (!pos)
			m_AlphaMeshList.AddTail(pNewNode);

		return true;
	}

	//	Allocate a new mesh node
	MESHNODE* pNewNode = AllocMeshNode(false);
	if (!pNewNode)
		return false;

	pNewNode->pSkinModel	= pSkinModel;
	pNewNode->pSkin			= pSkin;
	pNewNode->pMeshRef		= pMeshRef;

	RENDERSLOT* pSlot = SelectRenderSlot(pSkin, pMeshRef->pMesh);

	switch (pMeshRef->pMesh->GetClassID())
	{
	case A3D_CID_SKINMESH:

		pSlot->SkinMeshList.AddTail(pNewNode);
		break;

	case A3D_CID_RIGIDMESH:

		pSlot->RigidMeshList.AddTail(pNewNode);
		break;

	case A3D_CID_SUPPLEMESHIMP:

		pSlot->SuppleMeshList.AddTail(pNewNode);
		break;

	case A3D_CID_MORPHSKINMESH:
	case A3D_CID_MORPHRIGIDMESH:

		pSlot->MorphMeshList.AddTail(pNewNode);
		break;

	case A3D_CID_MUSCLEMESHIMP:

		pSlot->MuscleMeshList.AddTail(pNewNode);
		break;

	case A3D_CID_CLOTHMESHIMP:

		pSlot->ClothMeshList.AddTail(pNewNode);
		break;

	default:
		ASSERT(0);
		return false;
	}

	return true;
}

//	Register skin which is ready to be rendered. This function don't actually render
//	meshes, it just register them and sort them base on texture, material and skin.
bool A3DSkinRender::RegisterRenderSkin(A3DViewport* pViewport, A3DSkin* pSkin, A3DSkinModel* pSkinModel)
{
	int i, iNumMesh;

	//	Register cloth meshes
	if (pSkin->GetClothMeshNum() && pSkin->IsClothesEnable())
	{
		iNumMesh = pSkin->GetClothMeshNum();
		for (i=0; i < iNumMesh; i++)
		{
			A3DSkinMeshRef& Ref = pSkin->GetClothMeshRef(i);
			RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
		}
	}

	//	Register all skin meshes
	iNumMesh = pSkin->GetSkinMeshNum();
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinMeshRef& Ref = pSkin->GetSkinMeshRef(i);
		if (!Ref.pClothImp || !pSkin->IsClothesEnable())
			RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
	}

	//	Register all rigid meshes
	iNumMesh = pSkin->GetRigidMeshNum();
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinMeshRef& Ref = pSkin->GetRigidMeshRef(i);
		if (!Ref.pClothImp || !pSkin->IsClothesEnable())
			RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
	}

	//	Register all morph skin meshes
	iNumMesh = pSkin->GetMorphSkinMeshNum();
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinMeshRef& Ref = pSkin->GetMorphSkinMeshRef(i);
		RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
	}

	//	Register all morph rigid meshes
	iNumMesh = pSkin->GetMorphRigidMeshNum();
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinMeshRef& Ref = pSkin->GetMorphRigidMeshRef(i);
		RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
	}

	return true;
}

//	Select a proper render slot for specified mesh
A3DSkinRender::RENDERSLOT* A3DSkinRender::SelectRenderSlot(A3DSkin* pSkin, A3DMeshBase* pMesh)
{
	DWORD dwTexture = 0;

	if (pMesh->GetTextureIndex() >= 0)
	{
		A3DTexture* pTexture = pSkin->GetTexture(pMesh->GetTextureIndex());
		if (pTexture)
			dwTexture = pTexture->GetTextureID();
	}
	
	//	Search proper render slot
	RENDERSLOT* pSlot = NULL;
	int i;

	for (i=0; i < m_iRenderSlotCnt; i++)
	{
		pSlot = m_aRenderSlots[i];
		if (pSlot->dwTexture == dwTexture)
			break;
	}

	//	Cann't find a slot, create a new one
	if (i == m_iRenderSlotCnt)
	{
		if (m_iRenderSlotCnt >= m_aRenderSlots.GetSize())
		{
			//	Allocate a new slot
			if (!(pSlot = new RENDERSLOT))
			{
				g_A3DErrLog.Log("A3DSkinRender::SelectRenderSlot, Not enough memory !");
				return false;
			}
		}
		else
			pSlot = m_aRenderSlots[m_iRenderSlotCnt];

		pSlot->dwTexture = dwTexture;

		if (m_iRenderSlotCnt >= m_aRenderSlots.GetSize())
			m_aRenderSlots.Add(pSlot);

		m_iRenderSlotCnt++;
	}

	return pSlot;
}

//	Apply mesh material (traditional version)
void A3DSkinRender::ApplyMaterial(const MESHNODE* pNode)
{
	if (!m_pA3DDevice)
		return;

	int iMethod = pNode->pSkinModel->GetMaterialMethod();
	float fTransparent = pNode->pMeshRef->fTrans;
	float fAlpha = 1.0f - fTransparent;

	if (iMethod == A3DSkinModel::MTL_SCALED)
	{
		A3DCOLORVALUE vScale = pNode->pSkinModel->GetMaterialScale();
		if (fTransparent > 0.0f)
			vScale.a = fAlpha;

		A3DMaterial mtl = *pNode->pMeshRef->pMesh->GetMaterialPtr(pNode->pSkin);
		mtl.ScaleValues(vScale, true);
		mtl.SetEmissive(mtl.GetEmissive() + pNode->pSkinModel->GetExtraEmissive());
		mtl.Appear();
	}
	else
	{
		A3DMaterial* pSrcMtl;
		if (iMethod == A3DSkinModel::MTL_ORIGINAL)
			pSrcMtl = (A3DMaterial*)pNode->pMeshRef->pMesh->GetMaterialPtr(pNode->pSkin);
		else
			pSrcMtl = &pNode->pSkinModel->GetUniformMaterial();

		A3DMaterial mtl = *pSrcMtl;
		if (fTransparent > 0.0f)
			mtl.ScaleValues(A3DCOLORVALUE(1.0f, 1.0f, 1.0f, fAlpha), true);
		
		mtl.SetEmissive(mtl.GetEmissive() + pNode->pSkinModel->GetExtraEmissive());
		mtl.Appear();
	}

	if (pNode->pMeshRef->pMesh->GetMaterialPtr(pNode->pSkin)->Is2Sided())
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	else
		m_pA3DDevice->SetFaceCull(m_CurCull);
}

//	Apply mesh material (Vertex Shader version)
void A3DSkinRender::ApplyVSMaterial(const MESHNODE* pNode)
{
	if (!m_pA3DDevice)
		return;

	A3DMaterial* pMtl, MtlTemp;
	A3DMaterial* pOriginMtl = (A3DMaterial*)pNode->pMeshRef->pMesh->GetMaterialPtr(pNode->pSkin);

	int iMethod = pNode->pSkinModel->GetMaterialMethod();
	float fTransparent = pNode->pMeshRef->fTrans;
	float fAlpha = 1.0f - fTransparent;

	if (iMethod == A3DSkinModel::MTL_SCALED)
	{
		MtlTemp = *pOriginMtl;
		MtlTemp.ScaleValues(pNode->pSkinModel->GetMaterialScale(), true);
		pMtl = &MtlTemp;
	}
	else
	{
		if (iMethod == A3DSkinModel::MTL_ORIGINAL)
			pMtl = pOriginMtl;
		else if (iMethod == A3DSkinModel::MTL_UNIFORM)
			pMtl = &pNode->pSkinModel->GetUniformMaterial();
	}

	//	Get material parameters
	const A3DMATERIALPARAM& mp = pMtl->GetMaterialParam();
	const A3DSkinModel::LIGHTINFO& LightInfo = pNode->pSkinModel->GetLightInfo();

	//	Set specular power value
	m_vVSConst0.y = mp.Power;
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_CONSTANT, &m_vVSConst0, 1);

	//	Set ambient value
	A3DCOLORVALUE col = mp.Ambient * LightInfo.colAmbient + mp.Emissive + pNode->pSkinModel->GetExtraEmissive();
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_AMBIENT, &col, 1);

	//	Set diffuse value
	col = mp.Diffuse * LightInfo.colDirDiff;
	col.a = fTransparent > 0.0f ? fAlpha : col.a;
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRDIFFUSE, &col, 1);

	//	Set specular value
	col = mp.Specular * LightInfo.colDirSpec;
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRSPECULAR, &col, 1);

	if (LightInfo.bPtLight)
	{
		//	Set dynamic lgiht's ambient component
		col = mp.Ambient * LightInfo.colPtAmb;
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PTAMBIENT, &col, 1);

		//	Set dynamic light's diffuse component
		col = mp.Diffuse * LightInfo.colPtDiff;
		col.a = 0.0f;	//	This will effect model's transparence
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PTDIFFUSE, &col, 1);
	}

	//	2 sides property always get from original material
//	if (pMtl->Is2Sided())
	if (pOriginMtl->Is2Sided())
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	else
		m_pA3DDevice->SetFaceCull(m_CurCull);

	if (iMethod == A3DSkinModel::MTL_SCALED)
		MtlTemp.Release();
}

//	Apply default mesh material (traditional version)
void A3DSkinRender::ApplyDefMaterial(float fTransparent)
{
	if (!m_pA3DDevice)
		return;

	static A3DMaterial mtlDef;
	static bool bInit = false;

	if (!bInit)
	{
		mtlDef.Init(m_pA3DDevice);
		bInit = true;
	}

	if (fTransparent > 0.0f)
	{
		A3DMaterial mtl = mtlDef;
		A3DCOLORVALUE c = mtl.GetDiffuse();
		c.a = 1.0f - fTransparent;
		mtl.SetDiffuse(c);
		mtl.Appear();
	}
	else
		mtlDef.Appear();
}

//	Apply default mesh material (Vertex Shader version)
void A3DSkinRender::ApplyDefVSMaterial(float fTransparent)
{
	if (!m_pA3DDevice)
		return;

	static A3DMaterial mtlDef;
	static bool bInit = false;

	if (!bInit)
	{
		mtlDef.Init(m_pA3DDevice);
		bInit = true;
	}

	//	Get material parameters
	const A3DMATERIALPARAM& mp = mtlDef.GetMaterialParam();

	//	Set specular power value
	m_vVSConst0.y = mp.Power;
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_CONSTANT, &m_vVSConst0, 1);
	//	Set ambient value
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_AMBIENT, &mp.Ambient, 1);
	//	Set specular value
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRSPECULAR, &mp.Specular, 1);
	//	Set dynamic lgiht's ambient component
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_PTAMBIENT, &mp.Ambient, 1);
	//	Set dynamic light's diffuse component
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_PTDIFFUSE, &mp.Diffuse, 1);
	//	Set cull mode
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	//	Set diffuse value
	if (fTransparent > 0.0f)
	{
		A3DCOLORVALUE c = mp.Diffuse;
		c.a = 1.0f - fTransparent;
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRDIFFUSE, &c, 1);
	}
	else
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRDIFFUSE, &mp.Diffuse, 1);
}

//	Render skins
bool A3DSkinRender::Render(A3DViewport* pViewport, bool bRenderAlpha/* true */)
{
	if (!m_pA3DDevice)
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	//	Save light information
	DWORD dwAmbient = m_pA3DDevice->GetAmbientColor();
	A3DLIGHTPARAM DirLightParams;
	if (m_psm->m_pDirLight)
		DirLightParams = m_psm->m_pDirLight->GetLightparam();

	if (m_psm->m_pDPointLight)
		m_psm->m_pDPointLight->TurnOn();

	if (m_ColorOP != A3DTOP_MODULATE)
		m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);

	//	Set some vertex shader constants
	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
	{
#ifdef SKIN_BUMP_ENABLE
		if( m_psm->GetSpecularMap() )
		{
			m_psm->GetSpecularMap()->Appear(3);
			// set pixel shader constants for normal map
			// now set shader's constants here
			A3DCOLORVALUE c5 = m_pA3DDevice->GetAmbientValue();
			m_pA3DDevice->SetPixelShaderConstants(5, &c5, 1);
			A3DCOLORVALUE c6;
			c6 = DirLightParams.Diffuse;
			m_pA3DDevice->SetPixelShaderConstants(6, &c6, 1);
			m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			m_pA3DDevice->SetDeviceTextureStageState(3, D3DTSS_MIPMAPLODBIAS, 0);
		}
#endif

		A3DCameraBase* pCamera = pViewport->GetCamera();

		//	Set project matrix
		A3DMATRIX4 mat = pCamera->GetProjectionTM() * pCamera->GetPostProjectTM();
		mat.Transpose();

		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PROJECT, &mat, 4);

		//	Set eye's direction in camera space, it is the z axis in fact
		A3DVECTOR4 vEyeDir(0.0f, 0.0f, 1.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_EYEDIR, &vEyeDir, 1);
	}

	//	Save current cull mode
	m_CurCull = m_pA3DDevice->GetFaceCull();

	int i;

	//	Prepare a transformed clip plane
	m_bUserClip = false;
	DWORD		dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
	if( dwState == D3DCLIPPLANE0 ) // only one clip plane supported now
	{
		m_bUserClip = true;
		m_pA3DDevice->GetD3DDevice()->GetClipPlane(0, (float *)&m_cp);
		A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
		matVP.InverseTM();
		matVP.Transpose();
		D3DXPlaneTransform(&m_hcp, &m_cp, (D3DXMATRIX *) &matVP);
	}

	//	Render all slots
	for (i=0; i < m_iRenderSlotCnt; i++)
	{
		RENDERSLOT* pSlot = m_aRenderSlots[i];

		if (pSlot->SkinMeshList.GetCount())
		{
			//	Render skin meshes
			switch (iRenderMethod)
			{
			case A3DDevice::SMRD_VERTEXSHADER:
				
				RenderSkinMeshes_VS(pViewport, pSlot);
				break;

			case A3DDevice::SMRD_INDEXEDVERTMATRIX:
				
				RenderSkinMeshes_IVM(pViewport, pSlot);
				break;

			case A3DDevice::SMRD_SOFTWARE:	
				
				RenderSkinMeshes_SW(pViewport, pSlot);
				break;
			}
		}

		//	Render rigid meshes
		if (pSlot->RigidMeshList.GetCount())
			RenderRigidMeshes(pViewport, pSlot);

		//	Render supple meshes
		if (pSlot->SuppleMeshList.GetCount())
			RenderSuppleMeshes(pViewport, pSlot);

		if (pSlot->MorphMeshList.GetCount())
		{
			//	Render morph meshes
			if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
			{
				RenderMorphMeshes_VS(pViewport, pSlot);
			}
			else
				RenderMorphMeshes_SW(pViewport, pSlot);
		}

		if (pSlot->MuscleMeshList.GetCount())
		{
			//	Render muscle meshes
		//	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
			if (0)
				RenderMuscleMeshes_VS(pViewport, pSlot);
			else
				RenderMuscleMeshes_SW(pViewport, pSlot);
		}

		//	Render cloth meshes
		if (pSlot->ClothMeshList.GetCount())
			RenderClothMeshes(pViewport, pSlot);
	}

#ifdef SKIN_BUMP_ENABLE
	if( m_psm->GetSpecularMap() )
	{
		m_psm->GetSpecularMap()->Disappear(3);
		m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	}
#endif

	if (bRenderAlpha && m_AlphaMeshList.GetCount())
		RenderAlphaMeshes(pViewport);

	//	Clear all vertex streams
	for (i=0; i <= MAXNUM_HWMORPHCHANNEL; i++)
		m_pA3DDevice->ClearStreamSource(i);

	//	Restore light information
	m_pA3DDevice->SetAmbient(dwAmbient);
	if (m_psm->m_pDirLight)
		m_psm->m_pDirLight->SetLightParam(DirLightParams);

	if (m_psm->m_pDPointLight)
		m_psm->m_pDPointLight->TurnOff();

	if (m_ColorOP != A3DTOP_MODULATE)
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);

	//	Restore cull mode
	m_pA3DDevice->SetFaceCull(m_CurCull);

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	return true;
}

//	Render alpha skin models
bool A3DSkinRender::RenderAlpha(A3DViewport* pViewport)
{
	if (!m_pA3DDevice || !m_AlphaMeshList.GetCount())
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	//	Save light information
	DWORD dwAmbient = m_pA3DDevice->GetAmbientColor();
	A3DLIGHTPARAM DirLightParams;
	if (m_psm->m_pDirLight)
		DirLightParams = m_psm->m_pDirLight->GetLightparam();

	if (m_psm->m_pDPointLight)
		m_psm->m_pDPointLight->TurnOn();

	if (m_ColorOP != A3DTOP_MODULATE)
		m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);

	//	Set some vertex shader constants
	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
	{
#ifdef SKIN_BUMP_ENABLE
		if( m_psm->GetSpecularMap() )
		{
			m_psm->GetSpecularMap()->Appear(3);
			// set pixel shader constants for normal map
			// now set shader's constants here
			A3DCOLORVALUE c5 = m_pA3DDevice->GetAmbientValue();
			m_pA3DDevice->SetPixelShaderConstants(5, &c5, 1);
			A3DCOLORVALUE c6;
			c6 = DirLightParams.Diffuse;
			m_pA3DDevice->SetPixelShaderConstants(6, &c6, 1);
			m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			m_pA3DDevice->SetDeviceTextureStageState(3, D3DTSS_MIPMAPLODBIAS, 0);
		}
#endif
		A3DCameraBase* pCamera = pViewport->GetCamera();

		//	Set project matrix
		A3DMATRIX4 mat = pCamera->GetProjectionTM() * pCamera->GetPostProjectTM();
		mat.Transpose();

		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PROJECT, &mat, 4);

		//	Set eye's direction in camera space, it is the z axis in fact
		A3DVECTOR4 vEyeDir(0.0f, 0.0f, 1.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_EYEDIR, &vEyeDir, 1);
	}

	//	Save current cull mode
	m_CurCull = m_pA3DDevice->GetFaceCull();

	int i;

	//	Prepare a transformed clip plane
	m_bUserClip = false;
	DWORD dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
	if( dwState == D3DCLIPPLANE0 ) // only one clip plane supported now
	{
		m_bUserClip = true;
		m_pA3DDevice->GetD3DDevice()->GetClipPlane(0, (float *)&m_cp);
		A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
		matVP = a3d_InverseTM(matVP);
		matVP.Transpose();
		D3DXPlaneTransform(&m_hcp, &m_cp, (D3DXMATRIX *) &matVP);
	}

	//	Render alpha meshes
	RenderAlphaMeshes(pViewport);

	//	Clear all vertex streams
	for (i=0; i <= MAXNUM_HWMORPHCHANNEL; i++)
		m_pA3DDevice->ClearStreamSource(i);

	//	Restore light information
	m_pA3DDevice->SetAmbient(dwAmbient);
	if (m_psm->m_pDirLight)
		m_psm->m_pDirLight->SetLightParam(DirLightParams);

	if (m_psm->m_pDPointLight)
		m_psm->m_pDPointLight->TurnOff();

	if (m_ColorOP != A3DTOP_MODULATE)
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);

#ifdef SKIN_BUMP_ENABLE
	if( m_psm->GetSpecularMap() )
	{
		m_psm->GetSpecularMap()->Disappear(3);
		m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	}
#endif
	
	//	Restore cull mode
	m_pA3DDevice->SetFaceCull(m_CurCull);

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	return true;
}

//	Render skin meshes of specified slot using indexed vertex matrix
bool A3DSkinRender::RenderSkinMeshes_IVM(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Set render state
	m_pA3DDevice->SetIndexedVertexBlendEnable(true);
	m_pA3DDevice->SetVertexBlendFlag(A3DVBF_3WEIGHTS);

	//	Change texture
	ALISTPOSITION pos = pSlot->SkinMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->SkinMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear(0);
	// no normal map in indexed mode

	while (pos)
	{
		pMeshNode = pSlot->SkinMeshList.GetNext(pos);

		A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		//	Apply model's light
		pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
		//	Set material
		ApplyMaterial(pMeshNode);
		//	Render mesh
		pMesh->Render(pViewport, pModel, pSkin);
	}

	//	Restore render state
	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if( pTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	m_pA3DDevice->SetIndexedVertexBlendEnable(false);
	m_pA3DDevice->SetVertexBlendFlag(A3DVBF_DISABLE);

	return true;
}

//	Render skin meshes of specified slot using vertex shader
bool A3DSkinRender::RenderSkinMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Set render state
//	m_pA3DDevice->SetLightingEnable(false);

	//	Change texture
	ALISTPOSITION pos = pSlot->SkinMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->SkinMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear();

#ifdef SKIN_BUMP_ENABLE
	// using normal map in vs mode
	A3DTexture* pNormalMap = pMeshNode->pMeshRef->pMesh->GetNormalMapPtr(pMeshNode->pSkin);
	if (pNormalMap)
		pNormalMap->Appear(2);
#endif

	while (pos)
	{
		pMeshNode = pSlot->SkinMeshList.GetNext(pos);

		A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		if (pMeshNode->pMeshRef->bSWRender)
		{
			//	Apply model's light
			pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
			//	Set material
			ApplyMaterial(pMeshNode);
			//	Render mesh
			pMesh->RenderSoftware(pViewport, pModel, pSkin);
		}
		else
		{
			if (m_bUserClip)
			{
				// set clip plane to the one in homegeneous space
				m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_hcp);
			}

			//	Get vertex shader
			A3DVertexShader* pvs = pMeshNode->pMeshRef->pvs;
			if (!pvs)
			{
				if (!(pvs = m_psm->GetSkinMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
					continue;
			}

			pvs->Appear();

			//	Apply light parameters to vertex shader
			pModel->ApplyVSLightParams(pViewport);
			//	Set material
			ApplyVSMaterial(pMeshNode);

			//	Render mesh
			pMesh->Render(pViewport, pModel, pSkin);

			if (m_bUserClip)
			{
				// restore clip plane to the one in world space
				m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_cp);
			}
		}
	}

	//	Restore render state
#ifdef SKIN_BUMP_ENABLE
	if( pNormalMap )
		pNormalMap->Disappear(2);
#endif

	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if( pTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

//	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->ClearVertexShader();
	
	return true;
}

//	Render skin meshes of specified slot using software
bool A3DSkinRender::RenderSkinMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Set render state
	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	//	Change texture
	ALISTPOSITION pos = pSlot->SkinMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->SkinMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear();
	// no normal map in sw

	while (pos)
	{
		pMeshNode = pSlot->SkinMeshList.GetNext(pos);

		A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		//	Apply model's light
		pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
		//	Set material
		ApplyMaterial(pMeshNode);
		//	Render mesh
		pMesh->Render(pViewport, pModel, pSkin);
	}

	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if( pTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	return true;
}

//	Render rigid meshes of specified slot
bool A3DSkinRender::RenderRigidMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Change texture
	ALISTPOSITION pos = pSlot->RigidMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->RigidMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear();

	if (m_pA3DDevice->GetSkinModelRenderMethod() == A3DDevice::SMRD_VERTEXSHADER)
	{
#ifdef SKIN_BUMP_ENABLE
		// using normal map in vs mode
		A3DTexture* pNormalMap = pMeshNode->pMeshRef->pMesh->GetNormalMapPtr(pMeshNode->pSkin);
		if (pNormalMap)
			pNormalMap->Appear(2);
#endif
		while (pos)
		{
			pMeshNode = pSlot->RigidMeshList.GetNext(pos);

			A3DRigidMesh* pMesh = (A3DRigidMesh*)pMeshNode->pMeshRef->pMesh;
			A3DSkin* pSkin = pMeshNode->pSkin;
			A3DSkinModel* pModel = pMeshNode->pSkinModel;

			if (m_bUserClip)
			{
				// set clip plane to the one in homegeneous space
				m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_hcp);
			}

			//	Get vertex shader
			A3DVertexShader* pvs = pMeshNode->pMeshRef->pvs;
			if (!pvs)
			{
				if (!(pvs = m_psm->GetRigidMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
					continue;
			}

			pvs->Appear();

			//	Apply light parameters to vertex shader
			pModel->ApplyVSLightParams(pViewport);
			//	Set material
			ApplyVSMaterial(pMeshNode);

			//	Set blending matrices as vertex constants
			pModel->ApplyVSRigidMeshMat(pViewport, pMesh->GetBoneIndex(), SMVSC_BLENDMAT0);

			//	Render mesh
			pMesh->Render(pViewport, pModel, pSkin);

			if (m_bUserClip)
			{
				// restore clip plane to the one in world space
				m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_cp);
			}
		}

#ifdef SKIN_BUMP_ENABLE
		if( pNormalMap )
			pNormalMap->Disappear(2);
#endif
	}
	else
	{
		while (pos)
		{
			pMeshNode = pSlot->RigidMeshList.GetNext(pos);

			A3DRigidMesh* pMesh = (A3DRigidMesh*)pMeshNode->pMeshRef->pMesh;
			A3DSkin* pSkin = pMeshNode->pSkin;
			A3DSkinModel* pModel = pMeshNode->pSkinModel;

			//	Apply model's light
			pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
			//	Set material
			ApplyMaterial(pMeshNode);
			//	Render mesh
			pMesh->Render(pViewport, pModel, pSkin);
		}
	}

	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if( pTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	return true;
}

//	Render supple meshes of specified slot
bool A3DSkinRender::RenderSuppleMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Change texture
	ALISTPOSITION pos = pSlot->SuppleMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->SuppleMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear();
	// no normal map supported for supple meshes

	while (pos)
	{
		pMeshNode = pSlot->SuppleMeshList.GetNext(pos);

		A3DRigidMesh* pMesh = (A3DRigidMesh*)pMeshNode->pMeshRef->pMesh;
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		//	Apply model's light
		pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
		//	Set material
		ApplyMaterial(pMeshNode);
		//	Render mesh
		pMesh->Render(pViewport, pModel, pSkin);
	}

	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if( pTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	return true;
}

//	Render cloth meshes
bool A3DSkinRender::RenderClothMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Change texture
	ALISTPOSITION pos = pSlot->ClothMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->ClothMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear();
	
	while (pos)
	{
		pMeshNode = pSlot->ClothMeshList.GetNext(pos);

		A3DClothMeshImp* pMesh = (A3DClothMeshImp*)pMeshNode->pMeshRef->pMesh;
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		//	Apply model's light
		pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
		//	Set material
		ApplyMaterial(pMeshNode);
		//	Render mesh
		pMesh->Render(pViewport, pModel, pSkin);
	}

	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if (pTexture->IsShaderTexture())
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	return true;
}

//	Render morph meshes of specified slot using vertex shader
bool A3DSkinRender::RenderMorphMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Change texture
	ALISTPOSITION pos = pSlot->MorphMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->MorphMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear();
	// no normal map supported for morph meshes

	while (pos)
	{
		pMeshNode = pSlot->MorphMeshList.GetNext(pos);

		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		//	Select proper vertex shader
		int iVSType, iVSIndex;

		iVSType = pModel->IsSpecularEnable() ? 0 : 1;
		if (pModel->HasDynamicLight())
			iVSType += 2;

		if (pMeshNode->pMeshRef->pMesh->GetClassID() == A3D_CID_MORPHSKINMESH)
		{
			A3DMorphSkinMesh* pMesh = (A3DMorphSkinMesh*)pMeshNode->pMeshRef->pMesh;
			iVSIndex = A3DSkinMan::VS_MORPHSKINMESH0 + pMesh->m_MorphData.GetActiveMorphChannelNum();
		}
		else	//	A3D_CID_MORPHRIGIDMESH
		{
			A3DMorphRigidMesh* pMesh = (A3DMorphRigidMesh*)pMeshNode->pMeshRef->pMesh;
			iVSIndex = A3DSkinMan::VS_MORPHRIGIDMESH0 + pMesh->m_MorphData.GetActiveMorphChannelNum();
		}

		if (!m_psm->m_aVS[iVSType][iVSIndex])
			continue;

		m_psm->m_aVS[iVSType][iVSIndex]->Appear();

		//	Apply light parameters to vertex shader
		pModel->ApplyVSLightParams(pViewport);
		//	Set material
		ApplyVSMaterial(pMeshNode);
		//	Render mesh
		pMeshNode->pMeshRef->pMesh->Render(pViewport, pModel, pSkin);
	}

	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if( pTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	m_pA3DDevice->ClearVertexShader();

	return true;
}

//	Render morph meshes of specified slot using software
bool A3DSkinRender::RenderMorphMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Change texture
	ALISTPOSITION pos = pSlot->MorphMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->MorphMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear();
	// no normal map supported for morph meshes

	while (pos)
	{
		pMeshNode = pSlot->MorphMeshList.GetNext(pos);

		A3DMeshBase* pMesh = pMeshNode->pMeshRef->pMesh;
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		//	Apply model's light
		pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
		//	Set material
		ApplyMaterial(pMeshNode);
		//	Render mesh
		pMesh->Render(pViewport, pModel, pSkin);
	}

	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if( pTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	return true;
}

//	Render muscle meshes of specified slot using vertex shader
bool A3DSkinRender::RenderMuscleMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	return true;
}

//	Render muscle meshes of specified slot using software
bool A3DSkinRender::RenderMuscleMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Set render state
	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	//	Change texture
	ALISTPOSITION pos = pSlot->MuscleMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->MuscleMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear();
	// no normal map supported for muslce meshes

	while (pos)
	{
		pMeshNode = pSlot->MuscleMeshList.GetNext(pos);

		A3DMuscleMeshImp* pMesh = (A3DMuscleMeshImp*)pMeshNode->pMeshRef->pMesh;
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		//	Apply model's light
		pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
		//	Set material
		ApplyMaterial(pMeshNode);
		//	Render mesh
		pMesh->Render(pViewport, pModel, pSkin);
	}

	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if( pTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	return true;
}

bool A3DSkinRender::RenderBloomMeshes(A3DViewport* pViewport)
{
	if (m_bUserClip)
	{
		// set clip plane to the one in homegeneous space
		m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float *)&m_hcp);
	}
	

	
	for( int i = 0; i < m_BloomMeshes.GetSize(); i++)
	{
		MESHNODE* pMeshNode = m_BloomMeshes[i];
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;
		A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;

	
		A3DTexture* pTex = NULL;
		for(int j =0; j < pSkin->GetTextureNum(); j++)
		{
			 pTex = pSkin->GetTexture(j);

			if (pTex->IsShaderTexture())
			{
				pTex->Appear(0);
				break;
			}
		}

		A3DVertexShader* pvs = pMeshNode->pMeshRef->pvs;

		switch (pMesh->GetClassID())
		{
			case A3D_CID_SKINMESH:
			{
				
				if (!pvs)
				{
					if (!(pvs = m_psm->GetSkinMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
						return true;
				}

				pvs->Appear();	
					
			}
			break;
			case A3D_CID_RIGIDMESH:
				{
					
					if (!pvs)
					{
						if (!(pvs = m_psm->GetRigidMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
							return true;
					}

					pvs->Appear();

					//	Set blending matrices as vertex constants
					pModel->ApplyVSRigidMeshMat(pViewport, ((A3DRigidMesh*)pMesh)->GetBoneIndex(), SMVSC_BLENDMAT0);
				}
				break;
			default:
				break;
		}

		//	Apply light parameters to vertex shader
		pModel->ApplyVSLightParams(pViewport);
		//	Set material
		ApplyVSMaterial(pMeshNode);

		A3DCameraBase* pCamera = pViewport->GetCamera();
		
		//	Set project matrix
		A3DMATRIX4 mat = pCamera->GetProjectionTM() * pCamera->GetPostProjectTM();
		mat.Transpose();
		
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PROJECT, &mat, 4);
		
		//	Set eye's direction in camera space, it is the z axis in fact
		A3DVECTOR4 vEyeDir(0.0f, 0.0f, 1.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_EYEDIR, &vEyeDir, 1);


		//	Render mesh
		m_pBloomMeshPS->Appear();
		pMesh->Render(pViewport, pModel, pSkin);
		pvs->Disappear();
		if( pTex)
			pTex->Disappear();
		m_pBloomMeshPS->Disappear();
	}
	

	
	if (m_bUserClip)
	{
		// restore clip plane to the one in world space
		m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float *)&m_cp);
	}

	ResetBloomMeshes();
	return true;
}

//	Render alpha meshes
bool A3DSkinRender::RenderAlphaMeshes(A3DViewport* pViewport)
{
	A3DTexture* pLastTexture = NULL;
	DWORD dwTexture = (DWORD)(-1);
	
	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
	bool bAlphaComp = m_pA3DDevice->GetAlphaTestEnable();

	ALISTPOSITION pos = m_AlphaMeshList.GetHeadPosition();

	while (pos)
	{
		MESHNODE* pMeshNode = m_AlphaMeshList.GetNext(pos);
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		//	Apply mesh texture
		ApplyMeshTexture(&pLastTexture, dwTexture, pMeshNode);

		//	Apply mesh render state
		m_pA3DDevice->SetSourceAlpha(pModel->GetSrcBlend());
		m_pA3DDevice->SetDestAlpha(pModel->GetDstBlend());
		if( pModel->GetAlphaCompFlag() )
			m_pA3DDevice->SetAlphaTestEnable(true);

		switch (pMeshNode->pMeshRef->pMesh->GetClassID())
		{
		case A3D_CID_SKINMESH:
		{
			A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;

			switch (iRenderMethod)
			{
			case A3DDevice::SMRD_VERTEXSHADER:
			{
				if (pMeshNode->pMeshRef->bSWRender)
				{
					//	Apply model's light
					pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
					//	Set material
					ApplyMaterial(pMeshNode);
					//	Render mesh
					pMesh->RenderSoftware(pViewport, pModel, pSkin);
				}
				else
				{
					if (m_bUserClip)
					{
						// set clip plane to the one in homegeneous space
						m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float *)&m_hcp);
					}

					A3DVertexShader* pvs = pMeshNode->pMeshRef->pvs;
					if (!pvs)
					{
						if (!(pvs = m_psm->GetSkinMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
							goto NextMesh;
					}

					pvs->Appear();
					
					//	Apply light parameters to vertex shader
					pModel->ApplyVSLightParams(pViewport);
					//	Set material
					ApplyVSMaterial(pMeshNode);
					//	Render mesh
					pMesh->Render(pViewport, pModel, pSkin);

					if (m_bUserClip)
					{
						// restore clip plane to the one in world space
						m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float *)&m_cp);
					}
				}
				break;
			}
			case A3DDevice::SMRD_INDEXEDVERTMATRIX:
			{
				//	Set render state
				m_pA3DDevice->SetIndexedVertexBlendEnable(true);
				m_pA3DDevice->SetVertexBlendFlag(A3DVBF_3WEIGHTS);

				//	Apply model's light
				pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
				//	Set material
				ApplyMaterial(pMeshNode);
				//	Render mesh
				pMesh->Render(pViewport, pModel, pSkin);

				//	Restore render state
				m_pA3DDevice->SetIndexedVertexBlendEnable(false);
				m_pA3DDevice->SetVertexBlendFlag(A3DVBF_DISABLE);

				break;
			}
			case A3DDevice::SMRD_SOFTWARE:
			{	
				//	Set render state
				m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

				//	Apply model's light
				pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
				//	Set material
				ApplyMaterial(pMeshNode);
				//	Render mesh
				pMesh->Render(pViewport, pModel, pSkin);

				break;
			}
			}

			break;
		}
		case A3D_CID_RIGIDMESH:
		{
			A3DRigidMesh* pMesh = (A3DRigidMesh*)pMeshNode->pMeshRef->pMesh;

			if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
			{
				if (m_bUserClip)
				{
					// set clip plane to the one in homegeneous space
					m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_hcp);
				}

				A3DVertexShader* pvs = pMeshNode->pMeshRef->pvs;
				if (!pvs)
				{
					if (!(pvs = m_psm->GetRigidMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
						continue;
				}

				pvs->Appear();

				//	Apply light parameters to vertex shader
				pModel->ApplyVSLightParams(pViewport);
				//	Set material
				ApplyVSMaterial(pMeshNode);
				//	Set blending matrices as vertex constants
				pModel->ApplyVSRigidMeshMat(pViewport, pMesh->GetBoneIndex(), SMVSC_BLENDMAT0);

				//	Render mesh
				pMesh->Render(pViewport, pModel, pSkin);

				if (m_bUserClip)
				{
					// restore clip plane to the one in world space
					m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_cp);
				}
			}
			else
			{
				//	Apply model's light
				pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
				//	Set material
				ApplyMaterial(pMeshNode);
				//	Render mesh
				pMesh->Render(pViewport, pModel, pSkin);
			}

			break;
		}
		case A3D_CID_SUPPLEMESHIMP:
		case A3D_CID_MUSCLEMESHIMP:
		case A3D_CID_CLOTHMESHIMP:
		{
			A3DMeshBase* pMesh = (A3DMeshBase*)pMeshNode->pMeshRef->pMesh;

			//	Apply model's light
			pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
			//	Set material
			ApplyMaterial(pMeshNode);
			//	Render mesh
			pMesh->Render(pViewport, pModel, pSkin);

			break;
		}
		case A3D_CID_MORPHSKINMESH:
		case A3D_CID_MORPHRIGIDMESH:
		{
			if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
			{
				//	Apply light parameters to vertex shader
				pModel->ApplyVSLightParams(pViewport);

				//	Select proper vertex shader
				int iVSType, iVSIndex;

				iVSType = pModel->IsSpecularEnable() ? 0 : 1;
				if (pModel->HasDynamicLight())
					iVSType += 2;

				if (pMeshNode->pMeshRef->pMesh->GetClassID() == A3D_CID_MORPHSKINMESH)
				{
					A3DMorphSkinMesh* pMesh = (A3DMorphSkinMesh*)pMeshNode->pMeshRef->pMesh;
					iVSIndex = A3DSkinMan::VS_MORPHSKINMESH0 + pMesh->m_MorphData.GetActiveMorphChannelNum();
				}
				else	//	A3D_CID_MORPHRIGIDMESH
				{
					A3DMorphRigidMesh* pMesh = (A3DMorphRigidMesh*)pMeshNode->pMeshRef->pMesh;
					iVSIndex = A3DSkinMan::VS_MORPHRIGIDMESH0 + pMesh->m_MorphData.GetActiveMorphChannelNum();
				}

				if (!m_psm->m_aVS[iVSType][iVSIndex])
					continue;

				m_psm->m_aVS[iVSType][iVSIndex]->Appear();

				//	Set material
				ApplyVSMaterial(pMeshNode);
				//	Render mesh
				pMeshNode->pMeshRef->pMesh->Render(pViewport, pModel, pSkin);

			//	m_pA3DDevice->ClearVertexShader();
			}
			else
			{
				A3DMeshBase* pMesh = pMeshNode->pMeshRef->pMesh;

				//	Apply model's light
				pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
				//	Set material
				ApplyMaterial(pMeshNode);
				//	Render mesh
				pMesh->Render(pViewport, pModel, pSkin);
			}

			break;
		}
		default:
			ASSERT(0);
			break;
		}

	NextMesh:;

	}

	if (pLastTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pLastTexture->Disappear();
		if( pLastTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	m_pA3DDevice->ClearVertexShader();

	//	Restore mesh render state
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetAlphaTestEnable(bAlphaComp);

	return true;
}

//	Render specified skin model at once
bool A3DSkinRender::RenderSkinModelAtOnce(A3DViewport* pViewport, A3DSkinModel* pSkinModel, 
									   DWORD dwFlags)
{
	if (!m_pA3DDevice)
		return true;

	//	Save light information
	DWORD dwAmbient = m_pA3DDevice->GetAmbientColor();
	A3DLIGHTPARAM DirLightParams;
	if (m_psm->m_pDirLight)
		DirLightParams = m_psm->m_pDirLight->GetLightparam();

	if (m_psm->m_pDPointLight)
		m_psm->m_pDPointLight->TurnOn();

	if (m_ColorOP != A3DTOP_MODULATE)
		m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);

	//	Set some vertex shader constants
	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
	{
#ifdef SKIN_BUMP_ENABLE
		if( m_psm->GetSpecularMap() )
		{
			m_psm->GetSpecularMap()->Appear(3);
			// set pixel shader constants for normal map
			// now set shader's constants here
			A3DCOLORVALUE c5 = m_pA3DDevice->GetAmbientValue();
			m_pA3DDevice->SetPixelShaderConstants(5, &c5, 1);
			A3DCOLORVALUE c6;
			c6 = DirLightParams.Diffuse;
			m_pA3DDevice->SetPixelShaderConstants(6, &c6, 1);
			m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			m_pA3DDevice->SetDeviceTextureStageState(3, D3DTSS_MIPMAPLODBIAS, 0);
		}
#endif

		A3DCameraBase* pCamera = pViewport->GetCamera();

		//	Set project matrix
		A3DMATRIX4 mat = pCamera->GetProjectionTM() * pCamera->GetPostProjectTM();
		mat.Transpose();

		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PROJECT, &mat, 4);

		//	Set eye's direction in camera space, it is the z axis in fact
		A3DVECTOR4 vEyeDir(0.0f, 0.0f, 1.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_EYEDIR, &vEyeDir, 1);

		//	Apply light parameters to vertex shader
		pSkinModel->ApplyVSLightParams(pViewport);
	}

	bool bAlphaModel = false;
	if (pSkinModel->GetSrcBlend() != A3DBLEND_SRCALPHA ||
		pSkinModel->GetDstBlend() != A3DBLEND_INVSRCALPHA)
		bAlphaModel = true;

	if (bAlphaModel)
	{
		m_pA3DDevice->SetSourceAlpha(pSkinModel->GetSrcBlend());
		m_pA3DDevice->SetDestAlpha(pSkinModel->GetDstBlend());
		m_pA3DDevice->SetAlphaBlendEnable(true);
	}

	//	Save current cull mode
	m_CurCull = m_pA3DDevice->GetFaceCull();

	int i;

	//	Prepare a transformed clip plane
	m_bUserClip = false;
	DWORD dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
	if (dwState == D3DCLIPPLANE0) // only one clip plane supported now
	{
		m_bUserClip = true;
		m_pA3DDevice->GetD3DDevice()->GetClipPlane(0, (float *)&m_cp);
		A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
		matVP.InverseTM();
		matVP.Transpose();
		D3DXPlaneTransform(&m_hcp, &m_cp, (D3DXMATRIX *) &matVP);
	}

	//	Apply model's light
	pSkinModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);

	if (dwFlags & A3DSkinModel::RAO_NOTEXTURE)
		m_pA3DDevice->ClearTexture(0);

	//	Render all skins
	for (i=0; i < pSkinModel->GetSkinNum(); i++)
		RenderSkinAtOnce(pViewport, pSkinModel, i, dwFlags);

	//	Clear all vertex streams
	for (i=0; i <= MAXNUM_HWMORPHCHANNEL; i++)
		m_pA3DDevice->ClearStreamSource(i);

	//	Restore light information
	m_pA3DDevice->SetAmbient(dwAmbient);
	if (m_psm->m_pDirLight)
		m_psm->m_pDirLight->SetLightParam(DirLightParams);

	if (m_psm->m_pDPointLight)
		m_psm->m_pDPointLight->TurnOff();

	if (m_ColorOP != A3DTOP_MODULATE)
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);

#ifdef SKIN_BUMP_ENABLE
	if( m_psm->GetSpecularMap() )
	{
		m_psm->GetSpecularMap()->Disappear(3);
		m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	}
#endif

	//	Restore cull mode
	m_pA3DDevice->SetFaceCull(m_CurCull);

	if (bAlphaModel)
	{
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		m_pA3DDevice->SetAlphaBlendEnable(false);
	}

	return true;
}

//	Render skin item in specified skin model
bool A3DSkinRender::RenderSkinAtOnce(A3DViewport* pViewport, A3DSkinModel* pModel, 
							int iSkinItem, DWORD dwFlags)
{
	A3DSkinModel::SKIN* pSkinItem = pModel->GetSkinItem(iSkinItem);
	ASSERT(pSkinItem);
	if (!pSkinItem->bRender || !pSkinItem->pA3DSkin)
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	bool bAlphaBlend = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) ? true : false;

	int i, iNumMesh;
	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
	int iNumClothMesh = 0;
	A3DClothMeshImp* aClothMeshes[128];

	A3DSkin* pSkin = pSkinItem->pA3DSkin;
	A3DTexture* pLastTexture = NULL;
	DWORD dwTexture = (DWORD)(-1);

	A3DSkinMeshRef Ref;
	MESHNODE MeshNode;
	MeshNode.pSkin		= pSkin;
	MeshNode.pSkinModel	= pModel;
	MeshNode.pMeshRef	= &Ref;

	//	Render skin meshes
	if (!(iNumMesh = pSkin->GetSkinMeshNum()))
		goto RenderRigidMesh;

	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
	{
		for (i=0; i < iNumMesh; i++)
		{
			Ref = pSkin->GetSkinMeshRef(i);

			if (Ref.bInvisible)
				continue;

			//	Is a cloth mesh ?
			if (Ref.pClothImp && pSkin->IsClothesEnable())
			{
				aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
				continue;
			}
			
			A3DSkinMesh* pMesh = (A3DSkinMesh*)Ref.pMesh;
			A3DTexture * pTexture = pMesh->GetTexturePtr(pSkin);
			if (pTexture)
			{
				if (pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
					continue;
				
				//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
				if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
				{
					if ((dwFlags & A3DSkinModel::RAO_NOPIXELSHADER) && pTexture->IsShaderTexture())
					{
						A3DTexture* pBaseTex = ((A3DShader*)pTexture)->GetBaseTexture();
						
						if (pBaseTex)
						{
							pBaseTex->Appear(0);
							pLastTexture = pBaseTex;
						}
					}
					else
						ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode);
				}
			}

			if (Ref.bSWRender)
			{
				//	Set material and transparent
				ApplyMeshMaterial(&MeshNode, dwFlags, false);
				//	Render mesh
				pMesh->RenderSoftware(pViewport, pModel, pSkin);
			}
			else
			{
				//	Set material and transparent
				ApplyMeshMaterial(&MeshNode, dwFlags, true);

				if (m_bUserClip)
				{
					//	Set clip plane to the one in homegeneous space
					m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_hcp);
				}

				A3DVertexShader* pvs = MeshNode.pMeshRef->pvs;
				if (!pvs)
				{
					if (!(pvs = m_psm->GetSkinMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
						continue;
				}

				pvs->Appear();

				//	Render mesh
				pMesh->Render(pViewport, pModel, pSkin);

				if (m_bUserClip)
				{
					// restore clip plane to the one in world space
					m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_cp);
				}
			}
		}
	}
	else if (iRenderMethod == A3DDevice::SMRD_INDEXEDVERTMATRIX)
	{
		//	Set render state
		m_pA3DDevice->SetIndexedVertexBlendEnable(true);
		m_pA3DDevice->SetVertexBlendFlag(A3DVBF_3WEIGHTS);

		for (i=0; i < pSkin->GetSkinMeshNum(); i++)
		{
			Ref = pSkin->GetSkinMeshRef(i);

			if (Ref.bInvisible)
				continue;

			//	Is a cloth mesh ?
			if (Ref.pClothImp && pSkin->IsClothesEnable())
			{
				aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
				continue;
			}

			A3DTexture * pTexture = Ref.pMesh->GetTexturePtr(pSkin);
			if(pTexture && pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
				continue;

			//	Set material and transparent
			ApplyMeshMaterial(&MeshNode, dwFlags, false);

			//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
			if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
				ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode);

			//	Render mesh
			Ref.pMesh->Render(pViewport, pModel, pSkin);
		}

		m_pA3DDevice->SetIndexedVertexBlendEnable(false);
		m_pA3DDevice->SetVertexBlendFlag(A3DVBF_DISABLE);
	}
	else if (iRenderMethod == A3DDevice::SMRD_SOFTWARE)
	{
		//	Set render state
		m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

		for (i=0; i < pSkin->GetSkinMeshNum(); i++)
		{
			Ref = pSkin->GetSkinMeshRef(i);

			if (Ref.bInvisible)
				continue;

			//	Is a cloth mesh ?
			if (Ref.pClothImp && pSkin->IsClothesEnable())
			{
				aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
				continue;
			}

			A3DTexture * pTexture = Ref.pMesh->GetTexturePtr(pSkin);
			if(pTexture && pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
				continue;

			//	Set material and transparent
			ApplyMeshMaterial(&MeshNode, dwFlags, false);

			//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
			if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
				ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode);

			//	Render mesh
			Ref.pMesh->Render(pViewport, pModel, pSkin);
		}
	}

RenderRigidMesh:

	//	Render rigid meshes
	if (!(iNumMesh = pSkin->GetRigidMeshNum()))
		goto RenderMorphMesh;

	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
	{
		for (i=0; i < iNumMesh; i++)
		{
			Ref = pSkin->GetRigidMeshRef(i);

			if (Ref.bInvisible)
				continue;

			//	Is a cloth mesh ?
			if (Ref.pClothImp && pSkin->IsClothesEnable())
			{
				aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
				continue;
			}

			A3DRigidMesh* pMesh = (A3DRigidMesh*)Ref.pMesh;
			A3DTexture * pTexture = pMesh->GetTexturePtr(pSkin);
			if (pTexture)
			{
				if (pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
					continue;

				//	Set material and transparent
				ApplyMeshMaterial(&MeshNode, dwFlags, true);

				//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
				if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
				{
					if ((dwFlags & A3DSkinModel::RAO_NOPIXELSHADER) && pTexture->IsShaderTexture())
					{
						A3DTexture* pBaseTex = ((A3DShader*)pTexture)->GetBaseTexture();

						if (pBaseTex)
						{
							pBaseTex->Appear(0);
							pLastTexture = pBaseTex;
						}
					}
					else
						ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode);
				}
			}

			if (m_bUserClip)
			{
				// set clip plane to the one in homegeneous space
				m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_hcp);
			}

			A3DVertexShader* pvs = MeshNode.pMeshRef->pvs;
			if (!pvs)
			{
				if (!(pvs = m_psm->GetRigidMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
					continue;
			}

			pvs->Appear();

			//	Set blending matrices as vertex constants
			pModel->ApplyVSRigidMeshMat(pViewport, pMesh->GetBoneIndex(), SMVSC_BLENDMAT0);

			//	Render mesh
			pMesh->Render(pViewport, pModel, pSkin);

			if (m_bUserClip)
			{
				// restore clip plane to the one in world space
				m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &m_cp);
			}
		}
	}
	else
	{
		for (i=0; i < iNumMesh; i++)
		{
			Ref = pSkin->GetRigidMeshRef(i);

			if (Ref.bInvisible)
				continue;

			//	Is a cloth mesh ?
			if (Ref.pClothImp && pSkin->IsClothesEnable())
			{
				aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
				continue;
			}

			A3DTexture * pTexture = Ref.pMesh->GetTexturePtr(pSkin);
			if(pTexture && pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
				continue;

			//	Set material and transparent
			ApplyMeshMaterial(&MeshNode, dwFlags, false);

			//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
			if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
				ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode);
			
			//	Render mesh
			Ref.pMesh->Render(pViewport, pModel, pSkin);
		}
	}

RenderMorphMesh:

	//	TODO: morph mesh isn't used now, so we skip it. Maybe we will
	//		handle it later

//	RenderSuppleMesh:
	
	//	TODO: supple mesh isn't used now, so we skip it. Maybe we will
	//		handle it later

//	RenderMuscleMesh:
	
	//	TODO: muscle mesh isn't used now, so we skip it. Maybe we will
	//		handle it later

	//	RenderClothMeshes:
	for (i=0; i < iNumClothMesh; i++)
	{
		A3DClothMeshImp* pMesh = aClothMeshes[i];

		//	Apply model's light
		pModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
		//	Set material
		ApplyMaterial(&MeshNode);

		//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
		if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
			ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode);

		//	Render mesh
		pMesh->Render(pViewport, pModel, pSkin);
	}

	//	Last texture may be a shader, so disappear is necessary
	if (pLastTexture)
	{
		pLastTexture->Disappear();
		if( pLastTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	return true;
}

//	Apply mesh texture
void A3DSkinRender::ApplyMeshTexture(A3DTexture** ppLastTex, DWORD& dwTexID, const MESHNODE* pNode)
{
	A3DTexture* pLastTex = *ppLastTex;
	A3DTexture* pNewTex = pNode->pMeshRef->pMesh->GetTexturePtr(pNode->pSkin);
#ifdef SKIN_BUMP_ENABLE
	A3DTexture* pNormalMap = pNode->pMeshRef->pMesh->GetNormalMapPtr(pNode->pSkin);
#endif

	if (!pNewTex)
	{
		//	Last texture may be a shader, so disappear is necessary
		if (pLastTex)
		{
			pLastTex->Disappear();
			if( pLastTex->IsShaderTexture() )
			{
				if (m_ColorOP != A3DTOP_MODULATE)
					m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);	
			}
		}

		m_pA3DDevice->ClearTexture(0);
#ifdef SKIN_BUMP_ENABLE
		// we clear normal map here, because it can't be a shader, we directly clear device's texture
		if( pNormalMap )
			m_pA3DDevice->ClearTexture(2);
#endif

		*ppLastTex = NULL;
		dwTexID = (DWORD)(-1);
	}
	else if (dwTexID != pNewTex->GetTextureID())
	{
		//	Last texture may be a shader, so disappear is necessary
		if (pLastTex)
		{
			pLastTex->Disappear();
			if( pLastTex->IsShaderTexture() )
			{
				if (m_ColorOP != A3DTOP_MODULATE)
					m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);	
			}
#ifdef SKIN_BUMP_ENABLE
			// we clear normal map here, because it can't be a shader, we directly clear device's texture
			if( pNormalMap )
				m_pA3DDevice->ClearTexture(2);
#endif
		}

		//	Change texture
		pNewTex->Appear();
#ifdef SKIN_BUMP_ENABLE
		if( pNormalMap )
			pNormalMap->Appear(2);
#endif

		*ppLastTex = pNewTex;
		dwTexID = pNewTex->GetTextureID();
	}
}

//	Apply mesh material
void A3DSkinRender::ApplyMeshMaterial(MESHNODE* pNode, DWORD dwFlags, bool bVS)
{
	//	RAO_NOALPHA flag should be checked before material is set
	if (dwFlags & A3DSkinModel::RAO_NOALPHA)
		pNode->pMeshRef->fTrans = -1.0f;

	//	Set material
	if (dwFlags & A3DSkinModel::RAO_NOMATERIAL)
	{
		if (bVS)
			ApplyDefVSMaterial(pNode->pMeshRef->fTrans);
		else
			ApplyDefMaterial(pNode->pMeshRef->fTrans);
	}
	else
	{
		if (bVS)
			ApplyVSMaterial(pNode);
		else
			ApplyMaterial(pNode);
	}
}

