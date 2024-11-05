/*
 * FILE: SkinCandidate.cpp
 *
 * DESCRIPTION: Skin candidate class
 *
 * CREATED BY: duyuxin, 2003/9/16
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "SkinCandidate.h"
#include "MAXInterface.h"
#include "MAXNode.h"
#include "MAXMesh.h"
#include "SkeletonCandidate.h"
#include "MeshCandidate.h"
#include "VertexCandidate.h"
#include "BoneCandidate.h"
#include "AString.h"

#include "AF.h"
#include "A3D.h"

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
//	Implement CSkinCandidate
//
///////////////////////////////////////////////////////////////////////////

CSkinCandidate::CSkinCandidate()
{
	m_pSkeletonCandidate	= NULL;
	m_iMaxBone				= 4;
	m_fMinWeight			= 0.0f;
	m_bExtraTexDir			= true;

	m_pMaxMesh = NULL;
}

CSkinCandidate::~CSkinCandidate()
{
	CSkinCandidate::Release();
}

//	Initialize object
bool CSkinCandidate::Init()
{
	int iNode;
	AArray<CMAXNode*, CMAXNode*> aMeshNodes;
	
	//	Loop through all the selected nodes
	for (iNode=0; iNode < g_pMAXInterface->GetSelectedNodeCount(); iNode++)
	{
		//	Get the selected node
		CMAXNode* pNode = g_pMAXInterface->GetSelectedNode(iNode);

		int iType = pNode->GetType();

		if (iType == NODE_MESH)
			aMeshNodes.Add(pNode);
	}

	//	Check if one (and only one!) node/mesh is selected
	if (!aMeshNodes.GetSize())
	{
		AUX_OutputMessage("No mesh was selected !");
		return false;
	}

	//	Start the progress info
	g_pMAXInterface->StartProgressInfo("Prepare mesh nodes ...");

	//	Create MAX meshes
	for (iNode=0; iNode < aMeshNodes.GetSize(); iNode++)
	{
		g_pMAXInterface->SetProgressInfo(100.0f * (float)iNode / aMeshNodes.GetSize());

		//	Get the mesh
		CMAXMesh* pMesh = g_pMAXInterface->GetMesh(aMeshNodes[iNode]);
		if (!pMesh)
		{
			a_SendLogToDebugWnd(1, "Node %s: no mesh was found !", aMeshNodes[iNode]->GetName());
			return false;
		}

		m_aMAXMeshes.Add(pMesh);

		m_pMaxMesh = pMesh;
	}

	//	Stop the progress info
	g_pMAXInterface->StopProgressInfo();

	return true;
}

//	Process all MAX mesh nodes
bool CSkinCandidate::ProcessAllMeshNodes(CSkeletonCandidate* pSkeletonCandidate, int iMaxBone, float fMinWeight)
{
	m_iMaxBone		= iMaxBone;
	m_fMinWeight	= fMinWeight;

	m_pSkeletonCandidate = pSkeletonCandidate;

	a_SendLogToDebugWnd(0, "%d mesh nodes will be processed", m_aMAXMeshes.GetSize());

	//	Start the progress info
	g_pMAXInterface->StartProgressInfo("Analyzing the meshes ...");

	a_SendLogToDebugWnd(0, "Creating mesh candidates ...");

	int i;

	//	Create mesh candidates from selected nodes
	for (i=0; i < m_aMAXMeshes.GetSize(); i++)
	{
		//	Update the progress info
		g_pMAXInterface->SetProgressInfo(100.0f * (float)i / m_aMAXMeshes.GetSize());

		CMAXMesh* pMAXMesh = m_aMAXMeshes[i];

		if (!pMAXMesh->GetFaceCount())
			continue;

		CreateMeshCandidates(pMAXMesh);
	}

	//	Stop the progress info
	g_pMAXInterface->StopProgressInfo();

	//	Calcualte all meshes's center
	for (i=0; i < m_aMeshes.GetSize(); i++)
	{
		CMeshCandidate* pMesh = m_aMeshes[i];
		pMesh->CalcMeshCenter();
	}

	//	Calculate AABBs
//	CalculateAABBs(pSkeletonCandidate);

	return true;
}

//	Release object
void CSkinCandidate::Release()
{
	int i;

	//	Release all max meshes
	for (i=0; i < m_aMAXMeshes.GetSize(); i++)
	{
		CMAXMesh* pMaxMesh = m_aMAXMeshes[i];
		delete pMaxMesh;
	}

	m_aMAXMeshes.RemoveAll();

	//	Release all mesh candidates
	for (i=0; i < m_aMeshes.GetSize(); i++)
	{
		CMeshCandidate* pMesh = m_aMeshes[i];
		pMesh->Release();
		delete pMesh;
	}

	m_aMeshes.RemoveAll();

	//	Release all texture names
	for (i=0; i < m_aTexNames.GetSize(); i++)
	{
		AString* pString = m_aTexNames[i];
		delete pString;
	}

	m_aTexNames.RemoveAll();

	//	Release all materials
	for (i=0; i < m_aMaterials.GetSize(); i++)
	{
		A3DMaterial* pMtl = m_aMaterials[i];
		pMtl->Release();
		delete pMtl;
	}

	m_aMaterials.RemoveAll();
}

//	Create meshes from MAX node
bool CSkinCandidate::CreateMeshCandidates(CMAXMesh* pMAXMesh)
{
	//	Set bones' initalize TM of this mesh
	pMAXMesh->SetBoneInitTM(m_pSkeletonCandidate);

	int i, iMumMaterial = pMAXMesh->GetMaterialCount();

	//	Morph mesh can only have one material
	if (pMAXMesh->GetMorphFlag() && !pMAXMesh->IsOneMaterialMesh())
	{
		a_SendLogToDebugWnd(1, "CSkinCandidate::CreateMeshes, morph mesh can only have one material !");
		return false;
	}

	APtrArray<CMeshCandidate*> aMeshes;

	if (pMAXMesh->UseDefaultMaterial())
	{
		//	This material isn't selected
		if (!pMAXMesh->IsDefMaterialSelected())
			return true;

		CMeshCandidate* pMeshCandidate = new CMeshCandidate;
		if (!pMeshCandidate)
		{
			a_SendLogToDebugWnd(1, "CSkinCandidate::CreateMeshes, not enough memory !");
			return false;
		}

		pMeshCandidate->SetName(pMAXMesh->GetINode()->GetName());

		A3DMaterial A3DMtl;
		A3DMATERIALPARAM Params;

		Color wireColor(pMAXMesh->GetINode()->GetWireColor());
		Params.Ambient	= A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
		Params.Diffuse	= A3DCOLORVALUE(wireColor.r, wireColor.g, wireColor.b, 1.0f);
		Params.Emissive = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
		Params.Specular = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
		Params.Power	= 0.0f;

		A3DMtl.Init(NULL, Params);
		A3DMtl.Set2Sided(false);

		int iMaterial = AddMaterial(&A3DMtl);

		//	Create the new mesh candidate
		if (!pMeshCandidate->Init(pMAXMesh, iMaterial, -1, pMAXMesh->GetBoneIndex(), false))
		{
			a_SendLogToDebugWnd(1, "CSkinCandidate::CreateMeshes, Falied to initialize mesh candidate");
			return false;
		}

		aMeshes.Add(pMeshCandidate);
	}
	else
	{
		AString strName;
		int iSelMtl = 0;

		//	Create all the mesh candidates
		for (i=0; i < iMumMaterial; i++)
		{
			if (!pMAXMesh->IsMaterialSelected(i))
			{
				aMeshes.Add(NULL);
				continue;
			}

			iSelMtl++;

			CMeshCandidate* pMeshCandidate = new CMeshCandidate;
			if (!pMeshCandidate)
			{
				a_SendLogToDebugWnd(1, "CSkinCandidate::CreateMeshes, not enough memory !");
				return false;
			}

			strName.Format("%s_%d", pMAXMesh->GetINode()->GetName(), i);
			pMeshCandidate->SetName(strName);

			//	Convert MAX material to A3D material
			A3DMaterial A3DMtl;
			StdMat* pMtl = pMAXMesh->GetMAXMaterial(i);
			g_pMAXInterface->MaterialFromMAXToA3D(pMtl, &A3DMtl);

			//	Get texture name
			int iTexture;
			bool bAlphaMesh;
			Texmap* pTexMap = pMtl->GetSubTexmap(ID_DI);

			if (pTexMap && pTexMap->ClassID() == Class_ID(BMTEX_CLASS_ID,0))
			{
				//	For max 3.1 and 4.0 we must ignore the material diffuse 
				//	color if there is a maps;
				A3DMtl.SetAmbient(1.0f, 1.0f, 1.0f, A3DMtl.GetAmbient().a);
				A3DMtl.SetDiffuse(1.0f, 1.0f, 1.0f, A3DMtl.GetDiffuse().a);
	
				char szMapFile[MAX_PATH];
				strcpy(szMapFile, ((BitmapTex*)pTexMap)->GetMapName());
				iTexture = AddTexture(szMapFile);

				strlwr(szMapFile);
				bAlphaMesh = af_CheckFileExt(szMapFile, ".tga") ? true : false;
			}
			else
			{
				a_SendLogToDebugWnd(1, "CSkinCandidate::CreateMeshes, unknown texture format");
				iTexture	= -1;
				bAlphaMesh	= false;
			}

			int iMaterial = AddMaterial(&A3DMtl);

			//	Create the new mesh candidate
			if (!pMeshCandidate->Init(pMAXMesh, iMaterial, iTexture, pMAXMesh->GetBoneIndex(), bAlphaMesh))
			{
				a_SendLogToDebugWnd(1, "CSkinCandidate::CreateMeshes, Falied to initialize mesh candidate");
				return false;
			}

			aMeshes.Add(pMeshCandidate);
		}

		//	No material is selected
		if (!iSelMtl)
			return true;
	}

	//	Get the number of faces of the mesh
	int iNumFace = pMAXMesh->GetFaceCount();
	Mesh* pIMesh = pMAXMesh->GetIMesh();

	//	Ensure normals were built
	pIMesh->checkNormals(TRUE);

	//	Prepare modifers
	if (!pMAXMesh->PrepareModifiers())
	{
		AUX_OutputMessage("Failed to prepare modifers !");
		return false;
	}

	a_SendLogToDebugWnd(0, "Node %s: %d sub-meshes contain %d faces", pMAXMesh->GetINode()->GetName(), aMeshes.GetSize(), iNumFace);

	//	Loop through all faces of the mesh and put it into the corresponding submesh
	for (i=0; i < iNumFace; i++)
	{
		CMeshCandidate* pMeshCandidate;
		int iMtlId;

		if (pMAXMesh->UseDefaultMaterial())
		{
			pMeshCandidate = aMeshes[0];
			iMtlId = -1;
		}
		else
		{
			//	Get the material id of the face
			iMtlId = pMAXMesh->GetFaceMaterialId(pIMesh, i);
			if (!pMAXMesh->IsMaterialSelected(iMtlId))
				continue;

			pMeshCandidate = aMeshes[iMtlId];
		}

		if (!pMeshCandidate)
			continue;

		//	Get the first vertex of the face
		CVertexCandidate* pVertexCandidate = pMAXMesh->GetVertexCandidate(m_pSkeletonCandidate, i, 0, pIMesh);
		if (!pVertexCandidate)
			continue;

		//	Add it to the corresponding mesh
		int v1 = pMeshCandidate->AddVertexCandidate(pVertexCandidate);

		//	Get the second vertex of the face
		pVertexCandidate = pMAXMesh->GetVertexCandidate(m_pSkeletonCandidate, i, 1, pIMesh);
		if (!pVertexCandidate)
			continue;

		int v2 = pMeshCandidate->AddVertexCandidate(pVertexCandidate);

		//	Get the third vertex of the face
		pVertexCandidate = pMAXMesh->GetVertexCandidate(m_pSkeletonCandidate, i, 2, pIMesh);
		if (!pVertexCandidate)
			continue;
		
		int v3 = pMeshCandidate->AddVertexCandidate(pVertexCandidate);

		if (v1 == -1 || v2 == -1 || v3 == -1)
		{
			a_SendLogToDebugWnd(1, "CSkinCandidate::CreateMeshes, Invalid vertex indices");
			return false;
		}

		if (v1 > 0xffff || v2 > 0xffff || v3 > 0xffff)
		{
			pMAXMesh->ReleaseModifiers();
			a_SendLogToDebugWnd(1, "CSkinCandidate::CreateMeshes, Index > 0xffff");
			return false;
		}

		bool bRet = false;

		//	Add the face to the corresponding submesh. 3D MAX use right-hand coordinates,
		//	so we turn around vertices here
		if (!pMAXMesh->GetCullFlipFlag())
			bRet = pMeshCandidate->AddFace((WORD)v3, (WORD)v2, (WORD)v1);
		else
			bRet = pMeshCandidate->AddFace((WORD)v3, (WORD)v1, (WORD)v2);

		if (!bRet)
		{
			pMAXMesh->ReleaseModifiers();
			a_SendLogToDebugWnd(1, "CSkinCandidate::CreateMeshes, Failed to add face to mesh");
			return false;
		}
	}

	//	Release modifiers
	pMAXMesh->ReleaseModifiers();

	//	Adjust all bone assignments in the submesh candidates
	for (i=0; i < aMeshes.GetSize(); i++)
	{
		CMeshCandidate* pMeshCandidate = aMeshes[i];
		if (!pMeshCandidate)
			continue;

		if (!pMeshCandidate->GetVertexNum() || !pMeshCandidate->GetFaceNum())
		{
			delete pMeshCandidate;
			continue;
		}

		//	Recalculate vertex normal
		pMeshCandidate->CalculateNormals();
		//	Adjust bone assignment
		pMeshCandidate->AdjustBoneAssignment(m_iMaxBone, m_fMinWeight);

		//	Build morph data
		pMeshCandidate->BuildMorphData(m_pSkeletonCandidate);
		
		//	Build spring data
		if (!pMeshCandidate->BuildSpringData())
		{
			delete pMeshCandidate;
			continue;
		}

		//	Merge mesh into skin
		if (MergeMesh(pMeshCandidate))
		{
			pMeshCandidate->Release();
			delete pMeshCandidate;
		}
	}

	return true;
}

//	Add a texture
int CSkinCandidate::AddTexture(const char* szTexName)
{
	if (!szTexName)
		return -1;

	for (int i=0; i < m_aTexNames.GetSize(); i++)
	{
		AString* pstrName = m_aTexNames[i];
		if (!pstrName->CompareNoCase(szTexName))
			return i;
	}

	AString* pstrName = new AString(szTexName);
	if (!pstrName)
		return -1;

	return m_aTexNames.Add(pstrName);
}

//	Add a material
int CSkinCandidate::AddMaterial(A3DMaterial* pMaterial)
{
	if (!pMaterial)
		return -1;

	for (int i=0; i < m_aMaterials.GetSize(); i++)
	{
		A3DMaterial* pMtl = m_aMaterials[i];
		if (pMtl->Match(pMaterial))
			return i;
	}

	A3DMaterial* pMtl = new A3DMaterial;
	if (!pMtl)
		return -1;

	pMtl->Init(NULL, pMaterial->GetMaterialParam());
	pMtl->Set2Sided(pMaterial->Is2Sided());

	return m_aMaterials.Add(pMtl);
}

//	Calculate OBBs
bool CSkinCandidate::CalculateAABBs(CSkeletonCandidate* pSkeletonCandidate)
{
	AArray<A3DMATRIX4, A3DMATRIX4&> aInitMats;

	m_aBoneBoxes.RemoveAll();

	int i, j, n, iNumBone = pSkeletonCandidate->GetBoneNum();

	//	Do some initial work
	for (i=0; i < pSkeletonCandidate->GetBoneNum(); i++)
	{
		CBoneCandidate* pBoneCandidate = pSkeletonCandidate->GetBoneCandidate(i);
		A3DMATRIX4 mat;
		g_pMAXInterface->MatrixFromMAXToA3D(pBoneCandidate->GetInitTM(), mat);
		aInitMats.Add(mat);

		BONEBOX Box;
		Box.iBone	= i;
		Box.bValid	= true;

		ClearAABB(Box.aabb.Mins, Box.aabb.Maxs);
		m_aBoneBoxes.Add(Box);
	}

	g_pMAXInterface->StartProgressInfo("Calculate bone bounding boxes ...");

	A3DVECTOR3 v;

	for (i=0; i < m_aMeshes.GetSize(); i++)
	{
		CMeshCandidate* pMesh = m_aMeshes[i];

		g_pMAXInterface->SetProgressInfo(100.0f * (float)i / m_aMeshes.GetSize());

		for (j=0; j < pMesh->GetVertexNum(); j++)
		{
			CVertexCandidate* pVertCandidate = pMesh->GetVertexCandidate(j);

			for (n=0; n < pVertCandidate->GetInfluenceNum(); n++)
			{
				CVertexCandidate::INFLUENCE Influence = pVertCandidate->GetInfluence(n);
				int iBone = Influence.iBone;
				BONEBOX& Box = m_aBoneBoxes[iBone];

				//	Convert vertex to bone space
				v = pVertCandidate->GetPos() * aInitMats[iBone];
				AddVertexToAABB(Box.aabb.Mins, Box.aabb.Maxs, v);
			}
		}
	}

	//	Stop the progress info
	g_pMAXInterface->StopProgressInfo();

	//	Check result
	for (i=0; i < iNumBone; i++)
	{
		BONEBOX& Box = m_aBoneBoxes[i];

		for (j=0; j < 3; j++)
		{
			if (Box.aabb.Mins.m[j] > 900000 || Box.aabb.Maxs.m[j] < -900000)
				break;
		}

		if (j < 3)
			Box.bValid = false;
		else
			CompleteAABB(&Box.aabb); 
	}

	return true;
}

/*	Merge mesh

	Return true if new's vertices and faces have been merged with other meshes, 
	caller should release pNewMesh. Return false if mesh has been added to skin 
	directly, caller shouldn't release pNewMesh.

	pNewMesh: new mesh will be merge into skin
*/
bool CSkinCandidate::MergeMesh(CMeshCandidate* pNewMesh)
{
	bool bAddNew = false;
	int i;

	if (!pNewMesh->CanBeMerged())
		bAddNew = true;
	else
	{
		for (i=0; i < m_aMeshes.GetSize(); i++)
		{
			CMeshCandidate* pMesh = m_aMeshes[i];
			if (pMesh->CanBeMergedWith(pNewMesh))
				break;
		}
	
		if (i >= m_aMeshes.GetSize())
			bAddNew = true;
	}

	if (bAddNew)
	{
		//	Add a new mesh
		m_aMeshes.Add(pNewMesh);
		return false;
	}

	//	Merge mesh
	CMeshCandidate* pMesh = m_aMeshes[i];

	APtrArray<CVertexCandidate*>* pDestVerts = pMesh->GetVertices();
	APtrArray<CVertexCandidate*>* pSrcVerts = pNewMesh->GetVertices();

	if ((pMesh->GetFaceNum() + pNewMesh->GetFaceNum()) * 3 > 0xffff)
	{
		//	Too many faces, add mesh directly
		a_SendLogToDebugWnd(0, "CSkinCandidate::MergeMesh, Add mesh directly because of too many faces");
		m_aMeshes.Add(pNewMesh);
		return false;
	}

	int iBaseIdx = pDestVerts->GetSize();

	//	Merge vertices
	for (i=0; i < pSrcVerts->GetSize(); i++)
	{
		CVertexCandidate* pVert = (*pSrcVerts)[i];
		pDestVerts->Add(pVert);
	}

	//	Merge faces
	AArray<CMeshCandidate::FACE, CMeshCandidate::FACE&>* pDestFaces = pMesh->GetFaceArray();
	for (i=0; i < pNewMesh->GetFaceNum(); i++)
	{
		CMeshCandidate::FACE Face = pNewMesh->GetFace(i);
		Face.aVertIdx[0] += iBaseIdx;
		Face.aVertIdx[1] += iBaseIdx;
		Face.aVertIdx[2] += iBaseIdx;
		pDestFaces->Add(Face);
	}

	//	Merge springs
	AArray<CMeshCandidate::SPRING, CMeshCandidate::SPRING&>* pDestSprings = pMesh->GetSpringArray();
	for (i=0; i < pNewMesh->GetSpringNum(); i++)
	{
		CMeshCandidate::SPRING Spring = pNewMesh->GetSpring(i);
		Spring.aVertIdx[0] += iBaseIdx;
		Spring.aVertIdx[1] += iBaseIdx;
		pDestSprings->Add(Spring);
	}

	//	Resort destination mesh's springs
	pMesh->SortSprings();

	//	Clear source vertices and indices array
	pSrcVerts->RemoveAll();

	return true;
}


