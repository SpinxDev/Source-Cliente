/*
 * FILE: SceneTexture.cpp
 *
 * DESCRIPTION: Texture scene class
 *
 * CREATED BY: Duyuxin, 2003/7/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "SceneTexture.h"

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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
//	Implement CTextureScene
//
///////////////////////////////////////////////////////////////////////////

CTextureScene::CTextureScene()
{
	m_aVisSurfs1	= NULL;
	m_iNumVisSurf1	= 0;
	m_iRenderMode	= RENDER_ALL;
	m_pPortal		= NULL;
	m_bFillStream	= false;
}

//	Initialize object
bool CTextureScene::Init(A3DEngine* pA3DEngine)
{
	if (!A3DIBLScene::Init(pA3DEngine))
		return false;

	if (!(m_pPortal = new A3DPortal))
		return false;

	SetVisibleHandler(m_pPortal);

	return true;
}

//	Release objects
bool CTextureScene::Release()
{
	if (m_pPortal)
	{
		m_pPortal->Release();
		delete m_pPortal;
		m_pPortal = NULL;
	}

	m_aVisSurfs1	= NULL;
	m_iNumVisSurf1	= 0;

	return A3DIBLScene::Release();
}

//	Load portal file
bool CTextureScene::LoadPortalFile(const char* szFile)
{
	ASSERT(m_pPortal);
	return m_pPortal->Load(szFile);
}

//	Update visible set
bool CTextureScene::UpdateVisibleSets(A3DViewport* pCurrentViewport)
{
	if (!m_nNumTextures)
		return true;

	if (m_iRenderMode == RENDER_ALL || m_iRenderMode == RENDER_PORTAL)
	{
		m_bFillStream = true;
		return A3DIBLScene::UpdateVisibleSets(pCurrentViewport);
	}
	else if (!m_aVisSurfs1)
	{
		bool bRenderAll = GetRenderAllFlag();
		ForceToRenderAll(true);
		bool bRet = A3DIBLScene::UpdateVisibleSets(pCurrentViewport);
		ForceToRenderAll(bRenderAll);

		return bRet;
	}

	if (m_bFillStream)
	{
		int i;

		for (i=0; i < m_nNumTextures; i++)
			m_pTextureRecords[i].nFaceVisible = 0;

		for (i=0; i < m_iNumVisSurf1; i++)
		{
			int iFace = m_aVisSurfs1[i];
			int idTex = m_pFaceRecords[iFace].nTexID;
			int nFaceVisible = m_pTextureRecords[idTex].nFaceVisible++;
		
			memcpy(m_pTextureRecords[idTex].pVerts + nFaceVisible * 3, m_pAllFaces + iFace * 3, sizeof (A3DIBLVERTEX) * 3);
		}
	}
	
	m_nNumVisibleFaces = 0;

	for (int i=0; i < m_nNumTextures; i++)
	{
	#ifdef A3DSCENE_USESTREAM

		if (m_bFillStream && m_pTextureRecords[i].nFaceVisible > 0)
			m_aStreams[i]->SetVerts((LPBYTE)m_pTextureRecords[i].pVerts, m_pTextureRecords[i].nFaceVisible * 3);

	#endif

		m_nNumVisibleFaces += m_pTextureRecords[i].nFaceVisible;
	}

	m_bFillStream = false;

	return true;
}

//	Set/Get render mode
void CTextureScene::SetRenderMode(int iMode)
{ 
	m_iRenderMode = iMode;

	if (m_pPortal)
		m_pPortal->RecalcVisibleSet();

	if (m_iRenderMode == RENDER_ALL)
		ForceToRenderAll(true);
	else
		ForceToRenderAll(false);
}

//	Before device reset
bool CTextureScene::BeforeDeviceReset()
{
	A3DIBLScene::BeforeDeviceReset();
	return true;
}

//	After device reset
bool CTextureScene::AfterDeviceReset()
{
	A3DIBLScene::AfterDeviceReset();
	m_bFillStream = true;
	return true;
}


