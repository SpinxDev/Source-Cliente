/*
 * FILE: A3DTerrain2Wrapper.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/7/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Terrain2View.h"
#include "A3DTerrain2Wrapper.h"
#include "Render.h"
#include "Settings.h"

#include <A3DMacros.h>
#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DTerrain2Env.h>
#include <A3DTerrain2LoaderB.h>
#include <A3DViewport.h>
#include <A3DCameraBase.h>
#include <A3DTerrain2Blk.h>
#include <A3DWireCollector.h>
#include <A3DTerrain2CullHC.h>
#include <A3DHorizonCull.h>
#include <afi.h>

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2Wrapper
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrain2Wrapper::A3DTerrain2Wrapper()
{
	m_pHCCuller	= NULL;
}

A3DTerrain2Wrapper::~A3DTerrain2Wrapper()
{
}

//	Initialize terrain object
bool A3DTerrain2Wrapper::Init(A3DEngine* pA3DEngine, float fViewRadius, float fActRadius)
{
	if (!A3DTerrain2::Init(pA3DEngine, fViewRadius, fActRadius))
		return false;

	return true;
}

//	Release terrain object
void A3DTerrain2Wrapper::Release()
{
	A3DRELEASE(m_pHCCuller);

	m_aTLVerts.RemoveAll();

	A3DTerrain2::Release();
}

//	Load terrain data from file
bool A3DTerrain2Wrapper::Load(const char* szFileName, float cx, float cz)
{
	if (!A3DTerrain2::Load(szFileName, cx, cz, false))
		return false;

	//	Create terrain culler
	if ((m_pHCCuller = new A3DTerrain2CullHC))
	{
		//	Try to load horizon culling data
		char szFile[MAX_PATH];
		strcpy(szFile, szFileName);
		af_ChangeFileExt(szFile, MAX_PATH, ".t2hc");

		if (m_pHCCuller->Init(szFile))
		{
			AttachTerrainCuller(m_pHCCuller);

			bool bEnable = glb_GetApp()->GetSettings().GetHoriLineUse();
			m_pHCCuller->Enable(bEnable);
		}
		else
		{
			A3DRELEASE(m_pHCCuller);
			a_LogOutput(1, "A3DTerrain2Wrapper::Load, Failed to create horizon cull object");
		}
	}

	return true;
}

//	Render routine
bool A3DTerrain2Wrapper::Render(A3DViewport* pViewport)
{
	if (!m_bDataLoaded || !m_pA3DDevice || m_bNoRender)
		return true;

	ASSERT(m_pA3DEngine);
	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	//	Do blocks cull and LOD
	DoBlocksCullAndLOD(pViewport);

	//	render normal scene
	m_bUnderWaterRender = false;

	if (glb_GetApp()->GetSettings().GetTerrainRender())
	{
		//	Do render
		if (!m_TerrainRender.Render(pViewport))
		{
			m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);
			return false;
		}
	}

	m_bForceUpdate = false;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	return true;
}

//	Render block quads
bool A3DTerrain2Wrapper::RenderBlockQuads(A3DViewport* pViewport, int r_hl, int c_hl)
{
	if (!m_pHCCuller)
		return true;

	A3DWireCollector* pWC = g_Render.GetA3DEngine()->GetA3DWireCollector();
	BlockArray& aBlocks = m_pCurActBlocks->aBlocks;
	A3DCameraBase* pCamera = pViewport->GetCamera();
	const A3DVECTOR3& vEyePos = pCamera->GetPos();

	//	Calculate visible area in blocks, the area will be clamped in
	//	active area.
	ARectI rcView;
	CalcAreaInBlocks(vEyePos, m_fViewRadius, rcView);

	a_ClampFloor(rcView.left, m_pCurActBlocks->rcArea.left);
	a_ClampFloor(rcView.top, m_pCurActBlocks->rcArea.top);
	a_ClampRoof(rcView.right, m_pCurActBlocks->rcArea.right);
	a_ClampRoof(rcView.bottom, m_pCurActBlocks->rcArea.bottom);

	if (rcView.Width() <= 0 || rcView.Height() <= 0)
		return true;

	//	Index in aBlocks
	int iBaseIdx = m_pCurActBlocks->GetBlockIndex(rcView.top, rcView.left);
	int iBlkPitch = m_pCurActBlocks->rcArea.Width();
	int iDrawType = glb_GetApp()->GetSettings().GetBlockQuadShow();
	const int iEdgePitch = m_iNumAllBlockCol * 2 + 1;

	A3DVECTOR3 v1, v2, v3, v4, v5, v6, v7, v8;
	DWORD dwTestCol = 0xff0000ff;
	DWORD dwOcclCol = 0xff00ffff;
	DWORD dwHLCol = 0xffff00ff;
	DWORD dwVertCol = 0xff808080;

	for (int r=rcView.top; r < rcView.bottom; r++)
	{
		int iIndex = iBaseIdx;
		iBaseIdx += iBlkPitch;

		for (int c=rcView.left; c < rcView.right; c++, iIndex++)
		{
			A3DTerrain2Block* pBlock = aBlocks[iIndex];
			if (!pBlock || !pBlock->IsDataLoaded())
				continue;

			bool bPicked = (pBlock->GetRowInTerrain() == r_hl && pBlock->GetColInTerrain() == c_hl);

			const A3DAABB& aabb = pBlock->GetBlockAABB();
			if (!bPicked && !pCamera->AABBInViewFrustum(aabb.Mins, aabb.Maxs))
				continue;

			if (bPicked || (iDrawType & CSettings::BQS_OCCLUSION))
			{
				DWORD dwCol = bPicked ? dwHLCol : dwOcclCol;

				//	Calculate block's top edge index
				int iTop = pBlock->GetRowInTerrain() * iEdgePitch + pBlock->GetColInTerrain();
				v1.Set(aabb.Mins.x, m_pHCCuller->GetEdge(iTop).y1, aabb.Maxs.z);
				v2.Set(aabb.Maxs.x, m_pHCCuller->GetEdge(iTop).y2, aabb.Maxs.z);
				pWC->Add3DLine(v1, v2, dwCol);

				//	Left edge
				int iLeft = iTop + m_iNumAllBlockCol;
				v1.Set(aabb.Mins.x, m_pHCCuller->GetEdge(iLeft).y1, aabb.Maxs.z);
				v2.Set(aabb.Mins.x, m_pHCCuller->GetEdge(iLeft).y2, aabb.Mins.z);
				pWC->Add3DLine(v1, v2, dwCol);

				//	Right edge
				int iRight = iTop + m_iNumAllBlockCol + 1;
				v1.Set(aabb.Maxs.x, m_pHCCuller->GetEdge(iRight).y1, aabb.Maxs.z);
				v2.Set(aabb.Maxs.x, m_pHCCuller->GetEdge(iRight).y2, aabb.Mins.z);
				pWC->Add3DLine(v1, v2, dwCol);

				//	Bottom edge
				int iBottom = iTop + iEdgePitch;
				v1.Set(aabb.Mins.x, m_pHCCuller->GetEdge(iBottom).y1, aabb.Mins.z);
				v2.Set(aabb.Maxs.x, m_pHCCuller->GetEdge(iBottom).y2, aabb.Mins.z);
				pWC->Add3DLine(v1, v2, dwCol);
			}

			if (/*bPicked || */(iDrawType & CSettings::BQS_TEST))
			{
				v1.Set(aabb.Mins.x, aabb.Mins.y, aabb.Maxs.z);
				v2.Set(aabb.Maxs.x, aabb.Mins.y, aabb.Maxs.z);
				v3.Set(aabb.Maxs.x, aabb.Mins.y, aabb.Mins.z);
				v4.Set(aabb.Mins.x, aabb.Mins.y, aabb.Mins.z);

				v5.Set(aabb.Mins.x, aabb.Maxs.y, aabb.Maxs.z);
				v6.Set(aabb.Maxs.x, aabb.Maxs.y, aabb.Maxs.z);
				v7.Set(aabb.Maxs.x, aabb.Maxs.y, aabb.Mins.z);
				v8.Set(aabb.Mins.x, aabb.Maxs.y, aabb.Mins.z);

				DWORD dwCol = bPicked ? dwHLCol : dwTestCol;
				pWC->Add3DLine(v5, v6, dwCol);
				pWC->Add3DLine(v6, v7, dwCol);
				pWC->Add3DLine(v7, v8, dwCol);
				pWC->Add3DLine(v8, v5, dwCol);

				pWC->Add3DLine(v1, v5, dwVertCol);
				pWC->Add3DLine(v2, v6, dwVertCol);
				pWC->Add3DLine(v3, v7, dwVertCol);
				pWC->Add3DLine(v4, v8, dwVertCol);
			}
		}
	}

	pWC->Flush();

	return true;
}

//	Render horizon culling line
bool A3DTerrain2Wrapper::RenderHorizonCull(A3DViewport* pViewport)
{
	if (!m_pHCCuller || !m_pHCCuller->IsEnabled())
		return true;

	A3DHorizonCull* pHoriCull = m_pHCCuller->GetHorzonCull();
	float* pBuf = pHoriCull->GetBufferData();
	int iWidth = pHoriCull->GetWidth();
	int iHeight = pHoriCull->GetHeight();

	if (iWidth != m_aTLVerts.GetSize())
	{
		m_aTLVerts.RemoveAll(false);
		m_aTLVerts.SetSize(iWidth, 16);
	}

	int i;
	DWORD dwCol = 0xffffff00;

	//	Note: the height infomation stored in m_pHoriCull is from bottom to top,
	//		this is inverse with scrren coordinates
	for (i=0; i < iWidth; i++)
	{
		A3DTLVERTEX& v = m_aTLVerts[i];
		v.x			= (float)i;
		v.y			= iHeight - 1 - pBuf[i];
		v.z			= 0.0f;
		v.rhw		= 1.0f;
		v.tu		= 0.0f;
		v.tv		= 0.0f;
		v.diffuse	= dwCol;
		v.specular	= 0xff000000;
	}

	//	Set render states
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetDeviceVertexShader(A3DFVF_A3DTLVERTEX);
	pA3DDevice->SetZWriteEnable(false);
	pA3DDevice->SetZTestEnable(false);
	pA3DDevice->ClearTexture(0);

	//	Render
	if (!pA3DDevice->DrawPrimitiveUP(A3DPT_LINESTRIP, iWidth-1, m_aTLVerts.GetData(), sizeof (A3DTLVERTEX)))
		return false;

	//	Restore render state
	pA3DDevice->SetLightingEnable(true);
	pA3DDevice->SetZWriteEnable(true);
	pA3DDevice->SetZTestEnable(true);

	return true;
}

