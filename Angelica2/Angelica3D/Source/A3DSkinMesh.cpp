/*
 * FILE: A3DSkinMesh.cpp
 *
 * DESCRIPTION: A3D skin mesh class
 *
 * CREATED BY: duyuxin, 2003/9/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DSkinMesh.h"
#include "A3DPI.h"
#include "A3DVSDef.h"
#include "A3DStream.h"
#include "A3DFuncs.h"
#include "A3DSkin.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DConfig.h"
#include "A3DViewport.h"
#include "A3DSkinModel.h"
#include "A3DCameraBase.h"
#include "A3DSkinMan.h"
#include "AFile.h"
#include "AMemory.h"
#include "ACSWrapper.h"

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

struct s_INDEXMAP
{
	int		iNewIndex;
	int		iOldIndex;
};

//	Processed sub mesh
struct s_PROCSUBMESH
{
	s_PROCSUBMESH() : aVerts(0, 500), aIdxMaps(0, 500) {}

	int*	aMatIndices;	//	Matrix indices
	int		iMatCount;		//	Number of valid matrix in aMatIndices
	
	AArray<SKIN_VERTEX, SKIN_VERTEX&>	aVerts;		//	Vertices
	AArray<s_INDEXMAP, s_INDEXMAP&>		aIdxMaps;	//	Index maps
};

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement of A3DSkinMesh
//
///////////////////////////////////////////////////////////////////////////

A3DSkinMesh::A3DSkinMesh()
{
	m_dwClassID			= A3D_CID_SKINMESH;
	m_aVerts			= NULL;
	m_aIndices			= NULL;
	m_pVSStream			= NULL;
	m_pSRStream			= NULL;
	m_iNumVSBlendMat	= 0;
	m_bSplited			= false;
	m_iRenderMethod		= A3DDevice::SMRD_VERTEXSHADER;
}

A3DSkinMesh::~A3DSkinMesh()
{
}

//	Initialize object
bool A3DSkinMesh::Init(A3DEngine* pA3DEngine)
{
	if (!A3DMeshBase::Init(pA3DEngine))
		return false;

	return true;
}

//	Release object
void A3DSkinMesh::Release()
{
	A3DRELEASE(m_pVSStream);
	A3DRELEASE(m_pSRStream);

	//	Release all sub meshes
	ReleaseSubMeshes();

	//	Release original data buffers
	ReleaseOriginalBuffers();

	m_iNumVert	= 0;
	m_iNumIdx	= 0;
	m_bSplited	= false;

	A3DMeshBase::Release();
}

//	Load skin mesh data form file
bool A3DSkinMesh::Load(AFile* pFile, A3DSkin* pSkin)
{
	ASSERT(m_pA3DEngine);

	if (!m_pA3DEngine->GetA3DDevice())
		m_iRenderMethod = A3DDevice::SMRD_SOFTWARE;
	else
		m_iRenderMethod = m_pA3DEngine->GetA3DDevice()->GetSkinModelRenderMethod();

	//	Load data
	if (!LoadData(pFile))
	{
		g_A3DErrLog.Log("A3DSkinMesh::Load, Failed to load data!");
		return false;
	}

	//	Create sub meshes
	if (!CreateSubMeshes(pSkin))
	{
		g_A3DErrLog.Log("A3DSkinMesh::Load, Failed to create sub meshes!");
		return false;
	}

	return true;
}

//	Load skin mesh data from file
bool A3DSkinMesh::LoadData(AFile* pFile)
{
	//	Load mesh name
	if (!pFile->ReadString(m_strName))
	{
		g_A3DErrLog.Log("A3DSkinMesh::LoadData, Failed to read mesh name!");
		return false;
	}

	SKINMESHDATA Data;
	DWORD dwRead;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead) || dwRead != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DSkinMesh::LoadData, Failed to load skin mesh data!");
		return false;
	}
	
	m_iNumVert	= Data.iNumVert;
	m_iNumIdx	= Data.iNumIdx;
	m_iTexture	= Data.iTexture;
	m_iMaterial	= Data.iMaterial;

	//	Create original data buffers
	if (!CreateOriginalBuffers(m_iNumVert, m_iNumIdx))
	{
		g_A3DErrLog.Log("A3DSkinMesh::LoadData, Failed to create buffers!");
		return false;
	}

	//	Load vertex data
	if (m_iNumVert > 0)
	{
		DWORD dwSize = sizeof (A3DBVERTEX3) * m_iNumVert;

		A3DBVERTEX3* pVerts = (A3DBVERTEX3*)a_malloctemp(m_iNumVert * sizeof (A3DBVERTEX3));

		if (!pFile->Read(pVerts, dwSize, &dwRead) || dwRead != dwSize)
		{
			a_freetemp(pVerts);
			g_A3DErrLog.Log("A3DSkinMesh::LoadData, Failed to load vertex data!");
			return false;
		}

		for(int i=0; i<m_iNumVert; i++)
		{
			memcpy(&m_aVerts[i], &pVerts[i], sizeof(A3DBVERTEX3));
		}

		a_freetemp(pVerts);
	}

	//	Load index data
	if (m_iNumIdx > 0)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;

		if (!pFile->Read(m_aIndices, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DSkinMesh::LoadData, Failed to load index data!");
			return false;
		}
	}

#ifdef SKIN_BUMP_ENABLE
	BuildTangents();
#endif

	//	Calculate initial mesh aabb
	CalcInitMeshAABB();

	return true;
}

//	Save skin mesh data from file
bool A3DSkinMesh::Save(AFile* pFile)
{
	//	Write mesh name
	if (!pFile->WriteString(m_strName))
	{
		g_A3DErrLog.Log("A3DSkinMesh::Save(), Failed to write mesh name!");
		return false;
	}

	SKINMESHDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.iNumVert	= m_iNumVert;
	Data.iNumIdx	= m_iNumIdx;
	Data.iTexture	= m_iTexture;
	Data.iMaterial	= m_iMaterial;

	DWORD dwWrite;

	//	Write data
	if (!pFile->Write(&Data, sizeof (Data), &dwWrite) || dwWrite != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DSkinMesh::Save, Failed to write skin mesh data!");
		return false;
	}

	//	Write vertex data
	if (m_aVerts && m_iNumVert)
	{
		DWORD dwSize = sizeof (A3DBVERTEX3) * m_iNumVert;
		A3DBVERTEX3 * pVerts = new A3DBVERTEX3[m_iNumVert];
		for(int i=0; i<m_iNumVert; i++)
		{
			memcpy(&pVerts[i], &m_aVerts[i], sizeof(A3DBVERTEX3));
		}
		if (!pFile->Write(pVerts, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			delete [] pVerts;
			g_A3DErrLog.Log("A3DSkinMesh::Save, Failed to write vertex data!");
			return false;
		}

		delete [] pVerts;
	}

	//	Write index data
	if (m_aIndices && m_iNumIdx)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;

		if (!pFile->Write(m_aIndices, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DSkinMesh::Save, Failed to write index data!");
			return false;
		}
	}

	return true;
}

//	Calculate number of blend matrix can be used in vertex shader
void A3DSkinMesh::CalcVSBlendMatNum()
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	if (pA3DDevice)
		m_iNumVSBlendMat = (pA3DDevice->GetVSConstantNum() - SMVSC_BLENDMAT0) / 3;
	else
		m_iNumVSBlendMat = 0;
}

//	Calculate mesh center
void A3DSkinMesh::CalcInitMeshAABB()
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
		SKIN_VERTEX* pv = &m_aVerts[i];
		v.Set(pv->vPos[0], pv->vPos[1], pv->vPos[2]);
		m_aabbInitMesh.AddVertex(v);
	}

	m_aabbInitMesh.CompleteCenterExts();
}

//	Create original data buffers
bool A3DSkinMesh::CreateOriginalBuffers(int iNumVert, int iNumIdx)
{
	//	Release old buffers
	ReleaseOriginalBuffers();

	if (iNumVert > 0)
	{
		m_aVerts = new SKIN_VERTEX[iNumVert];

		if (!m_aVerts)
		{
			g_A3DErrLog.Log("A3DSkinMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	if (iNumIdx > 0)
	{
		m_aIndices = new WORD[iNumIdx];

		if (!m_aIndices)
		{
			g_A3DErrLog.Log("A3DSkinMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	m_iNumVert	= iNumVert;
	m_iNumIdx	= iNumIdx;

	return true;
}

//	Release original data buffers
void A3DSkinMesh::ReleaseOriginalBuffers()
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
}

//	Create sub meshes
bool A3DSkinMesh::CreateSubMeshes(A3DSkin* pSkin)
{
	if (!m_pA3DEngine->GetA3DDevice() || !m_iNumVert || !m_iNumIdx)
		return true;

	ASSERT(!(m_iNumIdx % 3));

	//	Calculate VS blend matrix number at first
	CalcVSBlendMatNum();

	bool bRet;

	switch (m_iRenderMethod)
	{
	case A3DDevice::SMRD_VERTEXSHADER:

		bRet = CreateVertexShaderSubMeshes(pSkin);
		break;

	case A3DDevice::SMRD_INDEXEDVERTMATRIX:

		bRet = CreateIndexedBlendMatSubMeshes();
		break;

	case A3DDevice::SMRD_SOFTWARE:

		bRet = CreateSoftwareSubMeshes();
		break;

	default:
		ASSERT(0);
		return false;
	}

	return bRet;
}

//	Create sub meshes for indexed blend matrix rendering
bool A3DSkinMesh::CreateIndexedBlendMatSubMeshes()
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	if (!(m_pSRStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DSkinMesh::CreateSoftwareSubMeshes, Not Enough Memory!");
		return false;
	}

	if (!m_pSRStream->Init(pA3DDevice, A3DVT_BVERTEX3, m_iNumVert, m_iNumIdx, 
				A3DSTRM_STATIC, A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR))
	{
		delete m_pSRStream;
		g_A3DErrLog.Log("A3DSkinMesh::CreateIndexedBlendMatSubMeshes, Failed to initialize A3DStream !");
		return false;
	}

	m_pSRStream->SetIndexRef(m_aIndices);

	A3DBVERTEX3 * pVerts = NULL;
	if( !m_pSRStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
	{
		g_A3DErrLog.Log("A3DSkinMesh::CreateIndexedBlendMatSubMeshes(), lock vertex buffer failed!");
		return false;
	}

	for(int i=0; i<m_iNumVert; i++)
	{
		memcpy(&pVerts[i], &m_aVerts[i], sizeof(A3DBVERTEX3));
	}

	m_pSRStream->UnlockVertexBuffer();

	//	Fill index buffer
	if (!m_pSRStream->SetIndices(m_aIndices, m_iNumIdx))
	{
		g_A3DErrLog.Log("A3DSkinMesh::CreateIndexedBlendMatSubMeshes, Failed to fill index buffer!");
		return false;
	}

	return true;
}

//	Create the only one stream for this skin mesh
bool A3DSkinMesh::CreateVSStream()
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	if (!(m_pVSStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DSkinMesh::CreateStream(), Not Enough Memory!");
		return false;
	}

	if (!m_pVSStream->Init(pA3DDevice, sizeof(SKIN_VERTEX), SKIN_FVF_VERTEX, m_iNumVert, m_iNumIdx, 
				A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR))
	{
		delete m_pVSStream;
		g_A3DErrLog.Log("A3DSkinMesh::CreateStream(), Failed to initialize A3DStream !");
		return false;
	}

	m_pVSStream->SetVertexRef((BYTE*)m_aVerts);
	m_pVSStream->SetIndexRef(m_aIndices);

	//	Fill vertex buffer
	if (!m_pVSStream->SetVerts((BYTE*)m_aVerts, m_iNumVert))
	{
		g_A3DErrLog.Log("A3DSkinMesh::CreateStream(), Failed to fill vertex buffer!");
		return false;
	}

	//	Fill index buffer
	if (!m_pVSStream->SetIndices(m_aIndices, m_iNumIdx))
	{
		g_A3DErrLog.Log("A3DSkinMesh::CreateStream(), Failed to fill index buffer!");
		return false;
	}
	return true;
}

//	Create sub meshes for vertex shader rendering
bool A3DSkinMesh::CreateVertexShaderSubMeshes(A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Need to split meshes ? Note: we must use '>' here other than '>=',
	//	because the blending matrices should have another more matrix
	//	represents skeleton's state
	if (m_iNumVSBlendMat > pSkin->GetSkinBoneNum())
	{
		m_bSplited = false;
		return CreateVSStream();
	}
	
	m_bSplited = true;

	int i, j, n, m, iFaceMatCnt;
	int aFaceMats[12];
	APtrArray<s_PROCSUBMESH*> aProcSubMeshes;

	for (i=0; i < m_iNumIdx; i+=3)
	{
		iFaceMatCnt = 0;

		//	Every face has 3 vertices
		for (j=0; j < 3; j++)
		{
			SKIN_VERTEX* pVert = &m_aVerts[m_aIndices[i+j]];
			DWORD dwIndices = pVert->dwMatIndices;

			//	Every vertex can have 4 matrix index
			for (n=0; n < 4; n++)
			{
				int iIndex = (dwIndices >> (n * 8)) & 0x000000ff;

				//	Don't add index repeatedly
				for (m=0; m < iFaceMatCnt; m++)
				{
					if (aFaceMats[m] == iIndex)
						break;
				}

				if (m >= iFaceMatCnt)
					aFaceMats[iFaceMatCnt++] = iIndex;
			}
		}

		//	Add this face to proper sub mesh
		AddFaceToProcessedSubMesh(aProcSubMeshes, i / 3, aFaceMats, iFaceMatCnt);
	}

	//	Convert processed sub meshes to real sub meshes
	for (i=0; i < aProcSubMeshes.GetSize(); i++)
	{
		s_PROCSUBMESH* pProcSubMesh = aProcSubMeshes[i];
		
		if (!ConvertProcessedSubMesh(pProcSubMesh))
		{
			g_A3DErrLog.Log("A3DSkinMesh::CreateVertexShaderSubMeshes, Failed to convert processed sub mesh!");
			return false;
		}
		
		//	Release processed sub mesh
		if (pProcSubMesh->aMatIndices)
			delete [] pProcSubMesh->aMatIndices;

		delete pProcSubMesh;
	}

	return true;
}

//	Add face to processed sub-mesh
bool A3DSkinMesh::AddFaceToProcessedSubMesh(APtrArray<s_PROCSUBMESH*>& aSubMeshes, 
								int iFace, int* aFaceMats, int iNumFaceMat)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	int i, j, n, iMissCnt, aMissedMats[12];
	WORD aIndices[3];

	aIndices[0] = m_aIndices[iFace * 3];
	aIndices[1] = m_aIndices[iFace * 3 + 1];
	aIndices[2] = m_aIndices[iFace * 3 + 2];

	for (i=0; i < aSubMeshes.GetSize(); i++)
	{
		s_PROCSUBMESH* pProcSubMesh = aSubMeshes[i];

		iMissCnt = 0;

		//	The sub mesh contains all blending matrix needed by the face ?
		for (n=0; n < iNumFaceMat; n++)
		{
			for (j=0; j < pProcSubMesh->iMatCount; j++)
			{
				if (pProcSubMesh->aMatIndices[j] == aFaceMats[n])
					break;
			}

			//	Don't find this matrix
			if (j >= pProcSubMesh->iMatCount)
				aMissedMats[iMissCnt++] = aFaceMats[n];
		}

		if (pProcSubMesh->iMatCount + iMissCnt <= m_iNumVSBlendMat)
		{
			//	Add face to this sub mesh
			for (j=0; j < iMissCnt; j++)
				pProcSubMesh->aMatIndices[pProcSubMesh->iMatCount++] = aMissedMats[j];

			//	Add vertices to sub mesh
			for (j=0; j < 3; j++)
				AddVertexToProcessedSubMesh(pProcSubMesh, aIndices[j]);
		
			return true;
		}
	}

	//	Create a new sub mesh
	s_PROCSUBMESH* pProcSubMesh = new s_PROCSUBMESH;
	if (!pProcSubMesh)
		return false;

	pProcSubMesh->iMatCount		= 0;
	pProcSubMesh->aMatIndices	= new int [m_iNumVSBlendMat];

	if (!pProcSubMesh->aMatIndices)
	{
		delete pProcSubMesh;
		return false;
	}

	//	Copy matrix indices
	for (i=0; i < iNumFaceMat; i++)
		pProcSubMesh->aMatIndices[i] = aFaceMats[i];

	pProcSubMesh->iMatCount = iNumFaceMat;

	//	Add vertices to mesh
	for (i=0; i < 3; i++)
		AddVertexToProcessedSubMesh(pProcSubMesh, aIndices[i]);

	aSubMeshes.Add(pProcSubMesh);

	return true;
}

/*	Add vertex to processed sub-mesh

	pProcSubMesh: processed sub mesh
	iIndex; vertex's index.
*/
bool A3DSkinMesh::AddVertexToProcessedSubMesh(s_PROCSUBMESH* pProcSubMesh, int iIndex)
{
	s_INDEXMAP IndexMap;
	IndexMap.iNewIndex = 0;
	IndexMap.iOldIndex = iIndex;

	int m, n;

	//	Check whether the vertex has been added to the mesh
	for (n=0; n < pProcSubMesh->aIdxMaps.GetSize(); n++)
	{
		if (pProcSubMesh->aIdxMaps[n].iOldIndex == IndexMap.iOldIndex)
		{
			IndexMap.iNewIndex = pProcSubMesh->aIdxMaps[n].iNewIndex;
			break;
		}
	}

	if (n < pProcSubMesh->aIdxMaps.GetSize())
	{
		//	Ok, the vertex has existed, just add new index
		pProcSubMesh->aIdxMaps.Add(IndexMap);
		return true;
	}

	//	Add a new vertex to the mesh
	SKIN_VERTEX Vert = m_aVerts[iIndex];

	//	Remap matrix indices
	int aMats[4];

	for (n=0; n < 4; n++)
	{
		int iIndex = (Vert.dwMatIndices >> (n * 8)) & 0x000000ff;

		for (m=0; m < pProcSubMesh->iMatCount; m++)
		{
			if (iIndex == pProcSubMesh->aMatIndices[m])
			{
				iIndex = m;
				break;
			}
		}

		ASSERT(m < pProcSubMesh->iMatCount);

		aMats[n] = iIndex;
	}

	Vert.dwMatIndices = 0;
	for (n=0; n < 4; n++)
		Vert.dwMatIndices |= (aMats[n] << (n * 8));

	IndexMap.iNewIndex = pProcSubMesh->aVerts.Add(Vert);
	pProcSubMesh->aIdxMaps.Add(IndexMap);

	return true;
}

//	Convert processed sub mesh to real sub mesh
bool A3DSkinMesh::ConvertProcessedSubMesh(s_PROCSUBMESH* pProcSubMesh)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Create one sub meshes
	SUBMESH* pSubMesh = new SUBMESH;
	if (!pSubMesh)
		return false;

	pSubMesh->iMatCount	  = pProcSubMesh->iMatCount;
	pSubMesh->aMatIndices = new int [pProcSubMesh->iMatCount];

	if (!pSubMesh->aMatIndices)
	{
		delete pSubMesh;
		g_A3DErrLog.Log("A3DSkinMesh::ConvertProcessedSubMesh, Not Enough Memory!");
		return false;
	}

	memcpy(pSubMesh->aMatIndices, pProcSubMesh->aMatIndices, pProcSubMesh->iMatCount * sizeof (int));

	//	Create stream
	pSubMesh->pA3DStream = new A3DStream;
	if (!pSubMesh->pA3DStream)
	{
		delete pSubMesh;
		g_A3DErrLog.Log("A3DSkinMesh::ConvertProcessedSubMesh, Not Enough Memory!");
		return false;
	}

	A3DStream* pA3DStream = pSubMesh->pA3DStream;

	int iNumVert = pProcSubMesh->aVerts.GetSize();
	int iNumIdx = pProcSubMesh->aIdxMaps.GetSize();

	if (!pA3DStream->Init(pA3DDevice, sizeof(SKIN_VERTEX), SKIN_FVF_VERTEX, iNumVert, iNumIdx, A3DSTRM_STATIC, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DSkinMesh::ConvertProcessedSubMesh, Failed to initialize A3DStream !");
		return false;
	}

	//	Fill vertex buffer
	if (!pA3DStream->SetVerts((BYTE*)pProcSubMesh->aVerts.GetData(), iNumVert))
	{
		g_A3DErrLog.Log("A3DSkinMesh::ConvertProcessedSubMesh, Failed to fill vertex buffer!");
		return false;
	}

	//	Fill index buffer
	WORD* pVBIndices;
	if (!pA3DStream->LockIndexBuffer(0, 0, (LPBYTE*) &pVBIndices, NULL))
	{
		g_A3DErrLog.Log("A3DSkinMesh::ConvertProcessedSubMesh, Failed to lock index buffer!");
		return false;
	}

	for (int i=0; i < iNumIdx; i++)
		pVBIndices[i] = pProcSubMesh->aIdxMaps[i].iNewIndex;

	if (!pA3DStream->UnlockIndexBuffer())
	{
		g_A3DErrLog.Log("A3DSkinMesh::ConvertProcessedSubMesh, Failed to unlock index buffer");
		return false;
	}

	//	Add to sub meshes list
	m_aSubMeshes.Add(pSubMesh);

	return true;
}

//	Create sub meshes for software rendering
bool A3DSkinMesh::CreateSoftwareSubMeshes()
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	if (!(m_pSRStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DSkinMesh::CreateSoftwareSubMeshes, Not Enough Memory!");
		return false;
	}

	if (!m_pSRStream->Init(pA3DDevice, A3DVT_VERTEX, m_iNumVert, m_iNumIdx, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR))
	{
		delete m_pSRStream;
		g_A3DErrLog.Log("A3DSkinMesh::CreateSoftwareSubMeshes, Failed to initialize A3DStream !");
		return false;
	}

	//	Fill index data
	m_pSRStream->SetIndices(m_aIndices, m_iNumIdx);
	m_pSRStream->SetIndexRef(m_aIndices);

	return true;
}

//	Release all sub meshes
void A3DSkinMesh::ReleaseSubMeshes()
{
	for (int i=0; i < m_aSubMeshes.GetSize(); i++)
	{
		SUBMESH* pSubMesh = m_aSubMeshes[i];
		if (pSubMesh->aMatIndices)
			delete [] pSubMesh->aMatIndices;

		if (pSubMesh->pA3DStream)
		{
			pSubMesh->pA3DStream->Release();
			delete pSubMesh->pA3DStream;
		}

		delete pSubMesh;
	}

	m_aSubMeshes.RemoveAll();
}

//	Apply vertex shader blend matrices
bool A3DSkinMesh::ApplyVSBlendMatrices(A3DSkinModel* pSkinModel, A3DSkin* pSkin, 
								A3DViewport* pViewport, int iStartConst)
{
	//	Get transposed blend matrices
	const A3DMATRIX4* aTBlendMats = pSkinModel->GetTVSBlendMatrices();
	const A3DVECTOR3& vc = m_pA3DEngine->GetA3DSkinMan()->GetWorldCenter();

	A3DMATRIX4 matView = pViewport->GetCamera()->GetViewTM();
	A3DMATRIX4 matOffset = Translate(DotProduct(matView.GetCol(0), vc), DotProduct(matView.GetCol(1), vc), DotProduct(matView.GetCol(2), vc));
	matView = matView * matOffset;
	matView.Transpose();

	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	int i, iNumBone = pSkin->GetSkinBoneNum();
	for (i=0; i <= iNumBone; i++)
	{
		int iBlendIdx = pSkin->MapBoneIndex(i);
		A3DMATRIX4 matBoneView = matView * aTBlendMats[iBlendIdx];
		if (!pA3DDevice->SetVertexShaderConstants(iStartConst+i*3, &matBoneView, 3))
			return false;
	}

	return true;
}

//	Apply blend matrices
bool A3DSkinMesh::ApplyIndexedBlendMatrices(A3DSkinModel* pSkinModel, A3DSkin* pSkin,
									A3DViewport* pViewport)
{
	//	Get transposed blend matrices
	const A3DMATRIX4* aBlendMats = pSkinModel->GetSWBlendMatrices();
	const A3DVECTOR3& vc = m_pA3DEngine->GetA3DSkinMan()->GetWorldCenter();

	A3DMATRIX4 matView = pViewport->GetCamera()->GetViewTM();
	A3DMATRIX4 matOffset = Translate(DotProduct(matView.GetCol(0), vc), DotProduct(matView.GetCol(1), vc), DotProduct(matView.GetCol(2), vc));
	matView = matView * matOffset;

	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);
	pA3DDevice->SetViewMatrix(matView);

	int i, iNumBone = pSkin->GetSkinBoneNum();
	for (i=0; i <= iNumBone; i++)
	{
		int iBlendIdx = pSkin->MapBoneIndex(i);
		A3DMATRIX4 matBoneView = aBlendMats[iBlendIdx];
		if (!pA3DDevice->SetIndexedVertexBlendMatrix(i, matBoneView))
			return false;
	}

	return true;
}

//	Render mesh
bool A3DSkinMesh::Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	if (!pA3DDevice || !m_iNumVert || !m_iNumIdx)
		return true;

	if (m_iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
	{
		// if we use vertex shader, the texture coord index must be equal to the stage number, here
		// we restore it because it may be changed to 0 by A3DShader
		pA3DDevice->SetTextureCoordIndex(0, 0);
		pA3DDevice->SetTextureCoordIndex(1, 1);
		pA3DDevice->SetTextureCoordIndex(2, 2);

		if (!m_bSplited) 
		{
			if (!m_pVSStream)
				return false;

			//	Apply blend matrices
			ApplyVSBlendMatrices(pSkinModel, pSkin, pViewport, SMVSC_BLENDMAT0);

			m_pVSStream->Appear(0, false);
			if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
				return false;
		}
		else
		{
			ASSERT(pSkinModel);

			int i, j;

			//	Get transposed blend matrices
			const A3DMATRIX4* aTBlendMats = pSkinModel->GetTVSBlendMatrices();
			A3DSkinMan* pSkinMan = pSkinModel->GetA3DEngine()->GetA3DSkinMan();
			const A3DVECTOR3& vc = pSkinMan->GetWorldCenter();

			A3DMATRIX4 matView = pViewport->GetCamera()->GetViewTM();
			A3DMATRIX4 matOffset = Translate(DotProduct(matView.GetCol(0), vc), DotProduct(matView.GetCol(1), vc), DotProduct(matView.GetCol(2), vc));
			matView = matView * matOffset;
			matView.Transpose();

			//	Render all sub meshes
			for (i=0; i < m_aSubMeshes.GetSize(); i++)
			{
				SUBMESH* pSubMesh = m_aSubMeshes[i];

				//	Apply blend matrices
				for (j=0; j < pSubMesh->iMatCount; j++)
				{
					int iMat = pSubMesh->aMatIndices[j];
					iMat = pSkin->MapBoneIndex(iMat);
					A3DMATRIX4 matBoneView = matView * aTBlendMats[iMat];
					pA3DDevice->SetVertexShaderConstants(SMVSC_BLENDMAT0+j*3, &matBoneView, 3);
				}

				pSubMesh->pA3DStream->Appear(0, false);

				int iNumVert = pSubMesh->pA3DStream->GetVertCount();
				int iNumFace = pSubMesh->pA3DStream->GetIndexCount() / 3;

				if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, iNumVert, 0, iNumFace))
					return false;
			}
		}
	}
	else if (m_iRenderMethod == A3DDevice::SMRD_INDEXEDVERTMATRIX)
	{
		if (!m_pSRStream)
			return false;

		//	Apply blend matrices
		ApplyIndexedBlendMatrices(pSkinModel, pSkin, pViewport);

		m_pSRStream->Appear();

		if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
		{
			//	View matrix was changed in ApplyIndexedBlendMatrices(), restore it
			pViewport->GetCamera()->Active();
			return false;
		}

		//	View matrix was changed in ApplyIndexedBlendMatrices(), restore it
		pViewport->GetCamera()->Active();
	}
	else	//	Software rendering
	{
		if (!RenderSoftware(pViewport, pSkinModel, pSkin))
			return false;
	}

	return true;
}

//	Software rendering
bool A3DSkinMesh::RenderSoftware(A3DViewport* pViewport, A3DSkinModel* pSkinModel,
							A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	if (!pA3DDevice || !m_pSRStream)
		return true;

	pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	// software rendering need shift world center to eliminate z-fighting
	// we need to adjust camera position relative to the world center
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	const A3DVECTOR3& vc = pSkinMan->GetWorldCenter();
	A3DCameraBase* pCamera = pViewport->GetCamera();

	A3DMATRIX4 matView = pCamera->GetViewTM();
	A3DMATRIX4 matOffset = Translate(DotProduct(matView.GetCol(0), vc), DotProduct(matView.GetCol(1), vc), DotProduct(matView.GetCol(2), vc));
	matView = matView * matOffset;

	const A3DMATRIX4* aBlendMats = pSkinModel->GetSWBlendMatrices();
	A3DVERTEX* aVerts;

	//	Lock vertex buffer
	if (!m_pSRStream->LockVertexBuffer(0, 0, (BYTE**) &aVerts, 0))
		return false;

	A3DVECTOR3 vPos, vNormal, vSrcPos, vSrcNormal;
	float fTotalWeight;

	for (int i=0; i < m_iNumVert; i++)
	{
		A3DVERTEX* pDest = &aVerts[i];
		SKIN_VERTEX* pSrc = &m_aVerts[i];

		vSrcPos.Set(pSrc->vPos[0], pSrc->vPos[1], pSrc->vPos[2]);
		vSrcNormal.Set(pSrc->vNormal[0], pSrc->vNormal[1], pSrc->vNormal[2]);

		vPos.Clear();
		vNormal.Clear();

		fTotalWeight = 0;

		for (int j=0; j < 3; j++)
		{
			if (!pSrc->aWeights[j])
				break;

			//	Map bone index
			int iMat = (pSrc->dwMatIndices >> (j << 3)) & 0x000000ff;
			iMat = pSkin->MapBoneIndex(iMat);

			vPos	+= (aBlendMats[iMat] * vSrcPos) * pSrc->aWeights[j];
			vNormal += a3d_VectorMatrix3x3(vSrcNormal, aBlendMats[iMat]) * pSrc->aWeights[j];

			fTotalWeight += pSrc->aWeights[j];
		}

		if (j >= 3)
		{
			fTotalWeight = 1.0f - fTotalWeight;
			if (fTotalWeight > 0.0f)
			{
				//	Map bone index
				int iMat = (pSrc->dwMatIndices & 0xff000000) >> 24;
				iMat = pSkin->MapBoneIndex(iMat);

				vPos	+= (aBlendMats[iMat] * vSrcPos) * fTotalWeight;
				vNormal += a3d_VectorMatrix3x3(vSrcNormal, aBlendMats[iMat]) * fTotalWeight;
			}
		}

		vNormal.Normalize();

		pDest->x = vPos.x;
		pDest->y = vPos.y;
		pDest->z = vPos.z;

		pDest->nx = vNormal.x;
		pDest->ny = vNormal.y;
		pDest->nz = vNormal.z;
	//	pDest->nx = vSrcNormal.x;
	//	pDest->ny = vSrcNormal.y;
	//	pDest->nz = vSrcNormal.z;

		pDest->tu = pSrc->tu;
		pDest->tv = pSrc->tv;
	}

	//	Unlock stream
	if (!m_pSRStream->UnlockVertexBuffer())
		return false;

	m_pSRStream->Appear();

	pA3DDevice->SetViewMatrix(matView);

	if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
	{
		//	Don't forget to restore current camera's view tm
		pCamera->Active();
		return false;
	}

	//	now restore current camera's view tm
	pCamera->Active();

	return true;
}

//	Use specified blend matrices to calculate vertices' positions and normals through software rendering method
//	One of aVerts and aNormals can be NULL
bool A3DSkinMesh::GetBlendedVertices(A3DSkin* pSkin, const A3DMATRIX4* aBlendMats, A3DVECTOR3* aVerts, A3DVECTOR3* aNormals)
{
	A3DVECTOR3 vPos, vNormal, vSrcPos, vSrcNormal;
	float fTotalWeight;

	for (int i=0; i < m_iNumVert; i++)
	{
		SKIN_VERTEX* pSrc = &m_aVerts[i];

		vSrcPos.Set(pSrc->vPos[0], pSrc->vPos[1], pSrc->vPos[2]);
		vSrcNormal.Set(pSrc->vNormal[0], pSrc->vNormal[1], pSrc->vNormal[2]);

		vPos.Clear();
		vNormal.Clear();

		fTotalWeight = 0;

		for (int j=0; j < 3; j++)
		{
			if (!pSrc->aWeights[j])
				break;

			//	Map bone index
			int iMat = (pSrc->dwMatIndices >> (j << 3)) & 0x000000ff;
			iMat = pSkin->MapBoneIndex(iMat);

			vPos	+= (aBlendMats[iMat] * vSrcPos) * pSrc->aWeights[j];
			vNormal += a3d_VectorMatrix3x3(vSrcNormal, aBlendMats[iMat]) * pSrc->aWeights[j];

			fTotalWeight += pSrc->aWeights[j];
		}

		if (j >= 3)
		{
			fTotalWeight = 1.0f - fTotalWeight;
			if (fTotalWeight > 0.0f)
			{
				//	Map bone index
				int iMat = (pSrc->dwMatIndices & 0xff000000) >> 24;
				iMat = pSkin->MapBoneIndex(iMat);

				vPos	+= (aBlendMats[iMat] * vSrcPos) * fTotalWeight;
				vNormal += a3d_VectorMatrix3x3(vSrcNormal, aBlendMats[iMat]) * fTotalWeight;
			}
		}

		aVerts[i] = vPos;

		vNormal.Normalize();
		aNormals[i] = vNormal;
	}

	return true;
}

//	Get approximate mesh data size
int A3DSkinMesh::GetDataSize()
{
	int iSize = 0;
	int iVertSize = m_iNumVert * sizeof (SKIN_VERTEX);
	int iIdxSize = m_iNumIdx * sizeof (WORD);

	if (m_aVerts)
		iSize += iVertSize;

	if (m_aIndices)
		iSize += iIdxSize;

	//	There area 3 copies of vertex data: original data, software render data
	//	and hardware sub mesh render data
	iSize *= 3;

	return iSize;
}

#ifdef SKIN_BUMP_ENABLE
bool A3DSkinMesh::BuildTangents()
{
	int i;
	// first of all clear all vertex's tangent coords.
	for(i=0; i<m_iNumVert; i++)
	{
		m_aVerts[i].tangents.Clear();
		m_aVerts[i].binormals.Clear();
	}

	// now calcualte tangent space coords.
	for(i=0; i<m_iNumIdx/3; i++)
	{
		int i0, i1, i2;
		i0 = m_aIndices[i * 3];
		i1 = m_aIndices[i * 3 + 1];
		i2 = m_aIndices[i * 3 + 2];

		SKIN_VERTEX& v0 = m_aVerts[i0];
		SKIN_VERTEX& v1 = m_aVerts[i1];
		SKIN_VERTEX& v2 = m_aVerts[i2];

		A3DVECTOR3 d10 = A3DVECTOR3(v1.vPos[0] - v0.vPos[0], v1.vPos[1] - v0.vPos[1], v1.vPos[2] - v0.vPos[2]);
		A3DVECTOR3 d20 = A3DVECTOR3(v2.vPos[0] - v0.vPos[0], v2.vPos[1] - v0.vPos[1], v2.vPos[2] - v0.vPos[2]);
		float t10 = v1.tu - v0.tu;
		float t20 = v2.tu - v0.tu;
		float b10 = v0.tv - v1.tv;
		float b20 = v0.tv - v2.tv;
		
		A3DVECTOR3 faceTangents = (d10 * b20 - d20 * b10) * (1.0f / (t10 * b20 - t20 * b10));
		A3DVECTOR3 faceBinormals = (d10 * t20 - d20 * t10) * (1.0f / (b10 * t20 - b20 * t10));

		v0.tangents = v0.tangents + faceTangents;
		v0.binormals = v0.binormals + faceBinormals;

		v1.tangents = v1.tangents + faceTangents;
		v1.binormals = v1.binormals + faceBinormals;

		v2.tangents = v2.tangents + faceTangents;
		v2.binormals = v2.binormals + faceBinormals;
	}

	for(i=0; i<m_iNumVert; i++)
	{
		m_aVerts[i].tangents.Normalize();
		m_aVerts[i].binormals.Normalize();
	}

	return true;
}
#endif

