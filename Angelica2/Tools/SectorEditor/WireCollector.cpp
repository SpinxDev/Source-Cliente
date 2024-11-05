/*
 * FILE: WireCollector.cpp
 *
 * DESCRIPTION: Wire collector class
 *
 * CREATED BY: Duyuxin, 2003/7/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "A3D.h"
#include "Render.h"
#include "WireCollector.h"

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
//	Implement CWireCollector
//
///////////////////////////////////////////////////////////////////////////

CWireCollector::CWireCollector()
{
	m_pRender	= NULL;
	m_aVertBuf	= NULL;
	m_aIdxBuf	= NULL;
	m_iVertCnt	= 0;
	m_iIdxCnt	= 0;
}

//	Initialize wire collector
bool CWireCollector::Init(CRender* pRender)
{
	m_pRender = pRender;

	//	Create stream
	if (!(m_pStream = new A3DStream))
	{
		g_Log.Log("CWireCollector::Init, Failed to create stream object.");
		return false;
	}

	if (!(m_pStream->Init(pRender->GetA3DDevice(), A3DVT_LVERTEX, SIZE_VERTEXBUF, 
			SIZE_INDEXBUF, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR)))
	{
		g_Log.Log("CWireCollector::Init, Failed to create stream object.");
		return false;
	}
	
	return true;
}

//	Release wire collector
void CWireCollector::Release()
{
	if (m_pStream)
	{
		if (m_aVertBuf)		//	Stream still be locked ! this shouldn't happen
			LockStream(false);
	
		m_pStream->Release();
		delete m_pStream;
		m_pStream = NULL;
	}

	m_pRender	= NULL;
	m_aVertBuf	= NULL;
	m_aIdxBuf	= NULL;
}

//	Lock/Unlock stream
bool CWireCollector::LockStream(bool bLock)
{
	if (bLock)
	{
		if (!m_pStream->LockVertexBuffer(0, 0, (BYTE**)&m_aVertBuf, 0))
			return false;

		if (!m_pStream->LockIndexBuffer(0, 0, (BYTE**)&m_aIdxBuf, 0))
			return false;

		m_iVertCnt	= 0;
		m_iIdxCnt	= 0;
	}
	else
	{
		if (!m_pStream->UnlockVertexBuffer())
			return false;

		if (!m_pStream->UnlockIndexBuffer())
			return false;

		m_aVertBuf	= NULL;
		m_aIdxBuf	= NULL;
	}

	return true;
}

//	Add vertex and index to render buffer
bool CWireCollector::AddRenderData(A3DVECTOR3* aVerts, int iNumVert, WORD* aIndices, 
								   int iNumIdx, DWORD dwCol)
{
	ASSERT(m_pRender && m_pStream);

	//	Vertex buffer has been full ?
	if (m_aVertBuf)
	{
		if (m_iVertCnt+iNumVert > SIZE_VERTEXBUF || m_iIdxCnt+iNumIdx > SIZE_INDEXBUF)
		{
			g_Log.Log("A3DStringSet::AddRenderData(), Buffer is full!");
			return false;
		}
	}

	//	Stream hasn't been locked, lock it.
	if (!m_aVertBuf)
	{
		if (!LockStream(true))
		{
			g_Log.Log("A3DStringSet::AddRenderData(), Failed to lock stream!");
			return false;
		}
	}

	A3DLVERTEX* pVert;
	WORD* pIndex;
	int i;
	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	pVert	= &m_aVertBuf[m_iVertCnt];
	pIndex	= &m_aIdxBuf[m_iIdxCnt];

	//	Fill vertex buffer
	for (i=0; i < iNumVert; i++, pVert++)
	{
		pVert->x	= aVerts[i].x;
		pVert->y	= aVerts[i].y;
		pVert->z	= aVerts[i].z;
		pVert->tu	= 0.0f;
		pVert->tv	= 0.0f;

		pVert->specular	= Specular;
		pVert->diffuse	= dwCol;
	}

	//	Fill index buffer
	for (i=0; i < iNumIdx; i++, pIndex++)
		*pIndex = m_iVertCnt + aIndices[i];

	m_iVertCnt	+= iNumVert;
	m_iIdxCnt	+= iNumIdx;

	return true;
}

//	Flush sector
bool CWireCollector::Flush()
{
	if (m_aVertBuf)
		LockStream(false);

	if (!m_iVertCnt || !m_iIdxCnt)
		return true;

	//	Set render states
//	m_pRender->SetWireRenderStates(true);
	m_pRender->GetA3DDevice()->SetWorldMatrix(IdentityMatrix());

	m_pStream->Appear();

	//	Render
	if (!m_pRender->GetA3DDevice()->DrawIndexedPrimitive(A3DPT_LINELIST, 0, m_iVertCnt,
											0, m_iIdxCnt >> 1))
		return false;

//	m_pRender->SetWireRenderStates(false);

	return true;
}

void CWireCollector::ResetBuffer()
{
	//	Stream still be locked! Unlock it.
	if (m_aVertBuf)
		LockStream(false);

	m_iVertCnt	= 0;
	m_iIdxCnt	= 0;
}

