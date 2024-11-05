/*
 * FILE: A3DRigidMesh.cpp
 *
 * DESCRIPTION: A3D rigid mesh class
 *
 * CREATED BY: duyuxin, 2003/10/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DRigidMesh.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DStream.h"
#include "A3DSkin.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DConfig.h"
#include "A3DViewport.h"
#include "A3DSkinModel.h"
#include "AFile.h"
#include "AMemory.h"

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
//	Implement of A3DRigidMesh
//
///////////////////////////////////////////////////////////////////////////

A3DRigidMesh::A3DRigidMesh()
{
	m_dwClassID		= A3D_CID_RIGIDMESH;
	m_aVerts		= NULL;
	m_aIndices		= NULL;
	m_pA3DStream	= NULL;
	m_bStaticBuf	= true;
	m_iBoneIdx		= -1;
}

A3DRigidMesh::~A3DRigidMesh()
{
}

//	Initialize object
bool A3DRigidMesh::Init(A3DEngine* pA3DEngine)
{
	if (!A3DMeshBase::Init(pA3DEngine))
		return false;

	return true;
}

//	Release object
void A3DRigidMesh::Release()
{
	A3DRELEASE(m_pA3DStream);

	//	Release original data buffers
	ReleaseOriginalBuffers();

	A3DMeshBase::Release();
}

//	Release original data buffers
void A3DRigidMesh::ReleaseOriginalBuffers()
{
	if (m_aVerts)
	{
		delete [] m_aVerts;
		m_aVerts = NULL;
	}

	if (m_aIndices)
	{
		delete [] m_aIndices;
		m_aIndices = NULL;
	}

	m_iNumVert	= 0;
	m_iNumIdx	= 0;
}

//	Load skin mesh data form file
bool A3DRigidMesh::Load(AFile* pFile, A3DSkin* pSkin)
{
	ASSERT(m_pA3DEngine);

	//	Load mesh name
	if (!pFile->ReadString(m_strName))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Load(), Failed to read mesh name!");
		return false;
	}

	RIGIDMESHDATA Data;
	DWORD dwRead;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead) || dwRead != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Load, Failed to load skin mesh data!");
		return false;
	}

	m_iBoneIdx	= Data.iBoneIdx;
	m_iNumVert	= Data.iNumVert;
	m_iNumIdx	= Data.iNumIdx;
	m_iTexture	= Data.iTexture;
	m_iMaterial	= Data.iMaterial;

	//	Create original buffers
	if (!CreateOriginalBuffers(m_iNumVert, m_iNumIdx))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Load, Failed to create original buffers!");
		return false;
	}

	//	Load vertex data
	if (m_iNumVert > 0)
	{
		DWORD dwSize = sizeof (A3DVERTEX) * m_iNumVert;
		if (!pFile->Read(m_aVerts, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DRigidMesh::Load, Failed to load vertex data!");
			return false;
		}
	}

	//	Load index data
	if (m_iNumIdx > 0)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;
		if (!pFile->Read(m_aIndices, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to load index data!");
			return false;
		}
	}

	if (m_iNumVert > 0 && m_iNumIdx > 0)
	{
		if (!CreateStream())
		{
			g_A3DErrLog.Log("A3DRigidMesh::Load, Failed to create stream!");
			return false;
		}
	}

	//	Calculate initial mesh aabb
	CalcInitMeshAABB();

	return true;
}

//	Save skin mesh data from file
bool A3DRigidMesh::Save(AFile* pFile)
{
	//	Write mesh name
	if (!pFile->WriteString(m_strName))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Save(), Failed to write mesh name!");
		return false;
	}

	RIGIDMESHDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.iBoneIdx	= m_iBoneIdx;
	Data.iNumVert	= m_iNumVert;
	Data.iNumIdx	= m_iNumIdx;
	Data.iTexture	= m_iTexture;
	Data.iMaterial	= m_iMaterial;

	DWORD dwWrite;

	//	Write data
	if (!pFile->Write(&Data, sizeof (Data), &dwWrite) || dwWrite != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to write skin mesh data!");
		return false;
	}

	//	Write vertex data
	if (m_aVerts && m_iNumVert)
	{
		DWORD dwSize = sizeof (A3DVERTEX) * m_iNumVert;
		if (!pFile->Write(m_aVerts, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to write vertex data!");
			return false;
		}
	}

	//	Write index data
	if (m_aIndices && m_iNumIdx)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;
		if (!pFile->Write(m_aIndices, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DRigidMesh::Save, Failed to write index data!");
			return false;
		}
	}

	return true;
}

//	Create original data buffers
bool A3DRigidMesh::CreateOriginalBuffers(int iNumVert, int iNumIdx)
{
	//	Release old buffers
	ReleaseOriginalBuffers();

	if (iNumVert > 0)
	{
		m_aVerts = new A3DVERTEX[iNumVert];

		if (!m_aVerts)
		{
			g_A3DErrLog.Log("A3DRigidMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	if (iNumIdx > 0)
	{
		m_aIndices = new WORD[iNumIdx];

		if (!m_aIndices)
		{
			g_A3DErrLog.Log("A3DRigidMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	m_iNumVert	= iNumVert;
	m_iNumIdx	= iNumIdx;

	return true;
}

//	Create A3D stream
bool A3DRigidMesh::CreateStream()
{
	if (!m_pA3DEngine->GetA3DDevice())
		return true;

	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Release old stream
	if (m_pA3DStream)
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
	}

	if (!(m_pA3DStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DRigidMesh::CreateStream, Not Enough Memory!");
		return false;
	}

	ASSERT(!(m_iNumIdx % 3));

	DWORD dwFlags = A3DSTRM_REFERENCEPTR;
	if (m_bStaticBuf)
		dwFlags |= A3DSTRM_STATIC;
	
	if (!m_pA3DStream->Init(pA3DDevice, A3DVT_VERTEX, m_iNumVert, m_iNumIdx, dwFlags, dwFlags))
	{
		g_A3DErrLog.Log("A3DRigidMesh::CreateStream, Failed to initialize A3DStream !");
		return false;
	}

	//	Dynamic buffer will be filled every frame, so needn't initializing
	if (m_bStaticBuf)
	{
		if (!m_pA3DStream->SetVerts((BYTE*)m_aVerts, m_iNumVert))
		{
			g_A3DErrLog.Log("A3DRigidMesh::CreateStream, Failed to set vertex stream data !");
			return false;
		}

		m_pA3DStream->SetVertexRef((BYTE*)m_aVerts);
		m_pA3DStream->SetIndexRef(m_aIndices);
	}
	
	//	Fill index data
	if (!m_pA3DStream->SetIndices(m_aIndices, m_iNumIdx))
	{
		g_A3DErrLog.Log("A3DRigidMesh::CreateStream, Failed to set index stream data !");
		return false;
	}

	return true;
}

//	Calculate initial mesh aabb
void A3DRigidMesh::CalcInitMeshAABB()
{
	if (!m_iNumVert || !m_aVerts)
	{
		memset(&m_aabbInitMesh, 0, sizeof (m_aabbInitMesh));
		return;
	}

	m_aabbInitMesh.Clear();
	A3DVECTOR3 v;

	for (int i=0; i < m_iNumVert; i++)
	{
		A3DVERTEX* pv = &m_aVerts[i];
		v.Set(pv->x, pv->y, pv->z);
		m_aabbInitMesh.AddVertex(v);
	}

	m_aabbInitMesh.CompleteCenterExts();
}

//	Render mesh
bool A3DRigidMesh::Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	if (!pA3DDevice || !m_pA3DStream || !m_iNumVert || !m_iNumIdx)
		return true;

	if (pA3DDevice->GetSkinModelRenderMethod() == A3DDevice::SMRD_VERTEXSHADER)
	{
		m_pA3DStream->Appear(0, false);
		if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
			return false;
	}
	else if (m_bStaticBuf)
	{
		//	Set transform matrix
		const A3DMATRIX4& matWorld = pSkinModel->GetBlendMatrix(m_iBoneIdx);
		pA3DDevice->SetWorldMatrix(matWorld);

		m_pA3DStream->Appear();
		if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
			return false;
	}
	else
	{
		//	TODO: Software vertex process, for example, do LOD for mesh.
		//		fill stream data and render it.
		if (!RenderSoftware(pViewport, pSkinModel))
			return false;
	}

	return true;
}

//	Software rendering
bool A3DRigidMesh::RenderSoftware(A3DViewport* pViewport, A3DSkinModel* pSkinModel)
{
	return true;
}

//	Get approximate mesh data size
int A3DRigidMesh::GetDataSize()
{
	int iSize = 0;
	int iVertSize = m_iNumVert * sizeof (A3DVERTEX);
	int iIdxSize = m_iNumIdx * sizeof (WORD);

	if (m_aVerts)
		iSize += iVertSize;

	if (m_aIndices)
		iSize += iIdxSize;

	//	There are 2 copies of vertex data: original data, software render data
	//	and hardware sub mesh render data
	iSize += iSize;

	return iSize;
}


