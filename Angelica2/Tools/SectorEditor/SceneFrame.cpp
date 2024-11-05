/*
 * FILE: SceneFrame.cpp
 *
 * DESCRIPTION: Frame scene class
 *
 * CREATED BY: Duyuxin, 2003/7/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "SceneFrame.h"
#include "Render.h"
#include "A3D.h"

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
//	Implement CFrameScene
//
///////////////////////////////////////////////////////////////////////////

CFrameScene::CFrameScene()
{
	m_pRender		= NULL;
	m_aVerts		= NULL;
	m_aVisSurfs		= NULL;
	m_iNumVisSurf	= 0;
	m_iNumFace		= 0;
	m_bRenderAll	= true;
	m_bFillStream	= false;

	m_dwUnmanagedPos = 0;
}

//	Initialize viewport
bool CFrameScene::Init(CRender* pRender)
{
	m_pRender = pRender;

	if (m_pRender->GetA3DDevice())
		m_dwUnmanagedPos = m_pRender->GetA3DDevice()->AddUnmanagedDevObject(this);

	return true;
}

//	Release viewport
void CFrameScene::Release()
{
	ReleaseSceneData();

	if (m_pRender && m_pRender->GetA3DDevice())
		m_pRender->GetA3DDevice()->RemoveUnmanagedDevObject((A3DDevObject *)m_dwUnmanagedPos);
	
	m_dwUnmanagedPos = 0;
}

//	Release scene data
void CFrameScene::ReleaseSceneData()
{
	for (int i=0; i < m_aStreams.GetSize(); i++)
	{
		//	Release stream
		A3DRELEASE(m_aStreams[i].pA3DStream);
	}

	if (m_aVerts)
	{
		delete [] m_aVerts;
		m_aVerts = NULL;
	}

	m_aVisSurfs		= NULL;
	m_iNumVisSurf	= 0;
	m_iNumFace		= 0;
}

//	Load scene
bool CFrameScene::Load(A3DIBLScene* pScene)
{
	//	Release old data
	ReleaseSceneData();

	m_iNumFace = pScene->GetFaceCount();

	A3DVECTOR3* aVertPos = new A3DVECTOR3[m_iNumFace * 3];
	if (!aVertPos)
	{
		g_Log.Log("CFrameScene::Load, not enough memory");
		return false;
	}

	int iSize = m_iNumFace * 3 * sizeof (A3DVECTOR3);
	if (!pScene->GetAllFaceVerts(aVertPos, &iSize))
	{
		g_Log.Log("CFrameScene::Load, Failed to call GetAllFaceVerts");
		return false;
	}

	if (!(m_aVerts = new A3DLVERTEX[m_iNumFace * 3]))
	{
		g_Log.Log("CFrameScene::Load, not enough memory");
		return false;
	}

	for (int i=0; i < m_iNumFace * 3; i++)
	{
		m_aVerts[i].x	= aVertPos[i].x;
		m_aVerts[i].y	= aVertPos[i].y;
		m_aVerts[i].z	= aVertPos[i].z;
		m_aVerts[i].tu	= 0.0f;
		m_aVerts[i].tv	= 0.0f;

		m_aVerts[i].diffuse		= g_aColTable[COL_NORMALFRAME];
		m_aVerts[i].specular	= 0xff000000;
	}

	delete [] aVertPos;

	if (!CreateStreams())
	{
		g_Log.Log("CFrameScene::Load, Failed to create streams !");
		return false;
	}

	return true;
}

//	Create streams
bool CFrameScene::CreateStreams()
{
	int i, iNumStream = (m_iNumFace * 3 + STREAM_SIZE - 1) / STREAM_SIZE;

	for (i=0; i < iNumStream; i++)
	{
		A3DStream* pA3DStream = new A3DStream;
		if (!pA3DStream)
		{
			ASSERT(0);
			return false;
		}

		if (!pA3DStream->Init(m_pRender->GetA3DDevice(), A3DVT_LVERTEX, STREAM_SIZE, 
			0, A3DSTRM_REFERENCEPTR, 0))
		{
			g_Log.Log("CFrameScene::CreateStream, Init a stream fail!");
			return false;
		}

		STREAM Stream;
		Stream.pA3DStream	= pA3DStream;
		Stream.iNumRdVert	= 0;
		m_aStreams.Add(Stream);
	}

	return true;
}

//	Update visible set
bool CFrameScene::UpdateVisibleSets()
{
	static bool bLastRenderAll = false;

	if (m_bRenderAll || !m_aVisSurfs)
	{
		if (m_bFillStream || !bLastRenderAll)
		{
			int iNumTail = (m_iNumFace * 3) % STREAM_SIZE;
			int iNumStream = m_aStreams.GetSize();
			int iStart = 0;

			for (int i=0; i < iNumStream - 1; i++)
			{
				m_aStreams[i].pA3DStream->SetVerts((BYTE*)(m_aVerts+iStart), STREAM_SIZE);
				m_aStreams[i].iNumRdVert = STREAM_SIZE;
				iStart += STREAM_SIZE;
			}

			m_aStreams[iNumStream-1].pA3DStream->SetVerts((BYTE*)(m_aVerts+iStart), iNumTail);
			m_aStreams[iNumStream-1].iNumRdVert = iNumTail;
		}

		bLastRenderAll = true;
	}
	else
	{
		if (m_bFillStream || bLastRenderAll)
		{
			int i, j;

			for (i=0; i < m_aStreams.GetSize(); i++)
				m_aStreams[i].iNumRdVert = 0;

			int iNumLoop = m_iNumVisSurf * 3 / STREAM_SIZE;
			int iFaceCnt = 0, iFillCnt;

			for (i=0; i < iNumLoop; i++)
			{
				A3DLVERTEX* pVerts;
				m_aStreams[i].pA3DStream->LockVertexBuffer(0, 0, (BYTE**) &pVerts, 0);

				for (j=0, iFillCnt=0; j < STREAM_SIZE / 3; j++)
				{
					int iBase = m_aVisSurfs[iFaceCnt] * 3;
					pVerts[iFillCnt++] = m_aVerts[iBase];
					pVerts[iFillCnt++] = m_aVerts[iBase+1];
					pVerts[iFillCnt++] = m_aVerts[iBase+2];

					iFaceCnt++;
				}

				m_aStreams[i].pA3DStream->UnlockVertexBuffer();
				m_aStreams[i].iNumRdVert = STREAM_SIZE;
			}

			int iTail = (m_iNumVisSurf * 3) % STREAM_SIZE;
			if (iTail)
			{
				A3DLVERTEX* pVerts;
				m_aStreams[i].pA3DStream->LockVertexBuffer(0, 0, (BYTE**) &pVerts, 0);

				int iNumTailFace = iTail / 3;

				for (j=0, iFillCnt=0; j < iNumTailFace; j++)
				{
					int iBase = m_aVisSurfs[iFaceCnt] * 3;
					pVerts[iFillCnt++] = m_aVerts[iBase];
					pVerts[iFillCnt++] = m_aVerts[iBase+1];
					pVerts[iFillCnt++] = m_aVerts[iBase+2];

					iFaceCnt++;
				}

				m_aStreams[i].pA3DStream->UnlockVertexBuffer();
				m_aStreams[i].iNumRdVert = iTail;
			}
		}

		bLastRenderAll = false;
	}

	m_bFillStream = false;

	return true;
}

//	Render viewport
bool CFrameScene::Render()
{
	if (!m_aStreams.GetSize())
		return true;

	A3DDevice* pDevice = m_pRender->GetA3DDevice();

	//	Set render states
	pDevice->SetWorldMatrix(IdentityMatrix());
	pDevice->GetD3DDevice()->SetTexture(0, NULL);

	for (int i=0; i < m_aStreams.GetSize(); i++)
	{
		if (!m_aStreams[i].iNumRdVert)
			break;

		m_aStreams[i].pA3DStream->Appear();

		pDevice->DrawPrimitive(A3DPT_TRIANGLELIST, 0, m_aStreams[i].iNumRdVert / 3);
	}

	//	Restore render states
	pDevice->ClearStreamSource(0);

	return true;
}

//	Before device reset
bool CFrameScene::BeforeDeviceReset()
{
	return true;
}

//	After device reset
bool CFrameScene::AfterDeviceReset()
{
	m_bFillStream = true;
	return true;
}



