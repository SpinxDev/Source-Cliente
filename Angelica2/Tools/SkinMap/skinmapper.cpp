/*
 * FILE: SkinMapper.cpp
 *
 * DESCRIPTION: Class to do skin mapping
 *
 * CREATED BY: Hedi, 2005/1/20
 *
 * HISTORY:
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.	
 */

#include "skinmapper.h"

#include "modstack.h"
#include "boneinterface.h"

Modifier* FindSkinModifier(INode* pINode)
{
#if MAX_RELEASE >= 4000
	//	Get the object reference of the node
	Object* pObject = pINode->GetObjectRef();
	if (!pObject)
		return NULL;

	Modifier* pModifier = NULL;

	//	Loop through all derived objects
	while (pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* pDerivedObject = (IDerivedObject*)pObject;

		int iNumMod = pDerivedObject->NumModifiers();

		//	Loop through all modifiers
		for (int i=0; i < iNumMod; i++)
		{
			//	Get the modifier
			pModifier = pDerivedObject->GetModifier(i);

			//	Check if we found the skin modifier
			if (pModifier->ClassID() == SKIN_CLASSID)
			{
				return pModifier;
			}
		}
	}

#endif

	return NULL;
}

CSkinMapper::CSkinMapper()
{
	m_eMappingStrategy = CLOSEST_VERTEX;
}

CSkinMapper::~CSkinMapper()
{
	RemoveAllSourceNodes();
}

CSkinedMesh * CSkinMapper::CreateMeshFromNode(HWND hPanel, INode * pNode)
{
	int i;
	CSkinedMesh * pNewMesh = new CSkinedMesh;

	Modifier * pModifier = FindSkinModifier(pNode);
	if( NULL == pModifier )
		return NULL;

	//	then get the triangle mesh in world space from the node.
	TimeValue time = GetCOREInterface()->GetTime();
	ObjectState os = pNode->EvalWorldState(time);

	TriObject * pTriObject = (TriObject*)os.obj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));
	if (!pTriObject)
		return NULL;
	Mesh& mesh = pTriObject->mesh;
	Matrix3 tm = pNode->GetObjectTM(time);

	//	First add all verts into this mesh
	for(i=0; i<mesh.numVerts; i++)
	{
		Point3  pos = mesh.verts[i] * tm;
		pNewMesh->AddVertex(pos);
	}
	for(i=0; i<mesh.numFaces; i++)
		pNewMesh->AddFace(mesh.faces[i].v[0], mesh.faces[i].v[1], mesh.faces[i].v[2]);
	
	//	Create a skin interface
	ISkin* pSkin = (ISkin*)pModifier->GetInterface(I_SKIN);
	if (!pSkin)
		return NULL;

	//	Add all bones in this node's skin modifier
	int nNumBones = pSkin->GetNumBones();
	for(i=0; i<nNumBones; i++)
	{
		INode * pBone = pSkin->GetBone(i);
		pNewMesh->AddBone(pBone, pBone->GetName(), pBone->GetNodeTM(time));
	}

	//	Create a skin context data interface
	ISkinContextData* pSkinContextData = (ISkinContextData*)pSkin->GetContextInterface(pNode);
	if (!pSkinContextData)
	{
		pModifier->ReleaseInterface(I_SKIN, pSkin);
		return NULL;
	}

	int iMaxVert = pSkinContextData->GetNumPoints();
	if( iMaxVert != mesh.numVerts )
	{
		char szInfo[256];
		sprintf(szInfo, "Please add bones into the skin modifier of node [%s]", pNode->GetName());
		MessageBox(hPanel, szInfo, "ERROR", MB_ICONWARNING | MB_OK);
		return NULL;
	}

	//	Set all verts weights
	for(i=0; i<iMaxVert; i++)
	{
		for(int j=0; j<pSkinContextData->GetNumAssignedBones(i); j++)
		{
			//	Get the bone id
			int idBone = pSkinContextData->GetAssignedBone(i, j);
			if( idBone < 0 ) 
				continue;

			//	Set the influence to the vertex
			pNewMesh->SetVertexWeight(i, idBone, pSkinContextData->GetBoneWeight(i, j));
		}
	}

	//	Release all interfaces
	pModifier->ReleaseInterface(I_SKIN, pSkin);

	if( pTriObject != os.obj ) 
		delete pTriObject;

	return pNewMesh;
}

bool CSkinMapper::AddSourceNode(INode * pNode)
{
	Modifier * pSkinModifier = FindSkinModifier(pNode);
	if( NULL == pSkinModifier )
	{
		MessageBox(NULL, "No skin modifier found!", "ERROR", MB_ICONWARNING | MB_OK);
		return false;
	}

	m_srcNodes.push_back(pNode);
	return true;
}

bool CSkinMapper::RemoveSourceNode(int index)
{
	if( index < 0 || index >= m_srcNodes.size() )
		return false;

	m_srcNodes.erase(&m_srcNodes[index]);
	return true;
}

bool CSkinMapper::RemoveAllSourceNodes()
{
	m_srcNodes.clear();
	return true;
}
	
INode * CSkinMapper::GetSourceNode(int index)
{
	if( index < 0 || index >= m_srcNodes.size() )
		return NULL;

	return m_srcNodes[index];
}

bool CSkinMapper::DoMapping(HWND hPanel, INode * pNode)
{
	int i, j;
	
	Modifier * pSkinModifier = FindSkinModifier(pNode);
	if( NULL == pSkinModifier )
	{
		MessageBox(hPanel, "No skin modifier found!", "ERROR", MB_ICONWARNING | MB_OK);
		return false;
	}

	// get all meshes ready!
	vector<CSkinedMesh *> srcSkins;
	for(i=0; i<m_srcNodes.size(); i++)
	{
		CSkinedMesh * pMesh = CreateMeshFromNode(hPanel, m_srcNodes[i]);
		if( !pMesh )
			goto FAILURE;
		srcSkins.push_back(pMesh);
	}
	CSkinedMesh * pDestSkin;
	pDestSkin = CreateMeshFromNode(hPanel, pNode);
	if( !pDestSkin )
		goto FAILURE;
	pDestSkin->ClearAllVertsWeight();
	for(i=0; i<srcSkins.size(); i++)
	{
		srcSkins[i]->SetInfluenceFlags(pDestSkin);
	}

	if(m_eMappingStrategy == CLOSEST_VERTEX)
		NearestPointMapping(srcSkins, pDestSkin);
	if(m_eMappingStrategy == CLOSEST_EDGE)
		NearestEdgeMapping(srcSkins, pDestSkin);
	if(m_eMappingStrategy == CLOSEST_TRIANGLE)
		NearestTriangleMapping(srcSkins, pDestSkin);

	// now set the result into skin modifier
	// Create a skin interface
	ISkin* pSkin;
	pSkin = (ISkin*)pSkinModifier->GetInterface(I_SKIN);
	if (!pSkin)
		return false;
	ISkinContextData* pSkinContextData;
	pSkinContextData = (ISkinContextData*)pSkin->GetContextInterface(pNode);
	if (!pSkinContextData)
	{
		pSkinModifier->ReleaseInterface(I_SKIN, pSkin);
		goto FAILURE;
	}

	BoneModData * bmd;
	const Tab<int> * pBoneMap;
	bmd = (BoneModData *) pSkinContextData;
	
	bmd->reevaluate = FALSE;
	pBoneMap = (Tab<int> *)((char *)(bmd->mod) + 0x90);	// this offset is used to get the BoneMap in class BeneDefMod, the value of offset can be seen in the function of GetAssignedBone

	int nv;
	nv = pDestSkin->m_verts.size();
	if( bmd->VertexDataCount != nv )
	{
		bmd->VertexDataCount = nv;

		// first release current vertex datas
		for(i=0; i<bmd->VertexData.Count(); i++)
		{
			if (bmd->VertexData[i] != NULL)
				delete (bmd->VertexData[i]);
			bmd->VertexData[i] = NULL;
		}
		bmd->VertexData.ZeroCount();
		bmd->VertexData.SetCount(nv);

		for(i=0; i<nv; i++) 
		{
			VertexListClass *vc;
			vc = new VertexListClass;
			bmd->VertexData[i] = vc;
			bmd->VertexData[i]->modified = FALSE;
			bmd->VertexData[i]->selected = FALSE;
 			bmd->VertexData[i]->d.ZeroCount();
		}
	}

	for(i=0; i<nv; i++)
	{
		bmd->VertexData[i]->modified = TRUE;
		bmd->VertexData[i]->selected = FALSE;
 		bmd->VertexData[i]->d.ZeroCount();

		bmd->VertexData[i]->d.SetCount(pDestSkin->m_verts[i].m_idBones.size());

		for(j=0; j<pDestSkin->m_verts[i].m_idBones.size(); j++)
		{
			bmd->VertexData[i]->d[j].Bones = (*pBoneMap)[pDestSkin->m_verts[i].m_idBones[j]];
			bmd->VertexData[i]->d[j].Influences = pDestSkin->m_verts[i].m_weightBones[j];
			bmd->VertexData[i]->d[j].normalizedInfluences = pDestSkin->m_verts[i].m_weightBones[j];
		}
	}
	bmd->CurrentCachePiece = -1;
	pSkinModifier->ReleaseInterface(I_SKIN, pSkin);

	pSkinModifier->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());

	return true;

FAILURE:
	for(i=0; i<srcSkins.size(); i++)
		delete srcSkins[i];
	
	if( pDestSkin )
		delete pDestSkin;

	return false;
}

void CSkinMapper::NearestPointMapping(vector<CSkinedMesh *>& srcSkins, CSkinedMesh * pDestSkin)
{
	size_t i;
	for(i=0; i<pDestSkin->m_verts.size(); i++)
	{
		Point3 pos = pDestSkin->m_verts[i].m_vertPos;

		int		srcSkinIndex = -1;
		int		vertIndex;

		float disMin = FLT_MAX;
		for(size_t k=0; k<srcSkins.size(); k++)
		{
			int vertIndexThis;
			float disThis = srcSkins[k]->InfluentialPoint(pos, &vertIndexThis);
			if(disMin > disThis)
			{
				disMin = disThis;
				srcSkinIndex = k;
				vertIndex = vertIndexThis;
			}
		}

		// get nearest intersected triagle in the src skin meshes
		// set weight according to this triangle (3 vertices and its bone id weight)
		if(disMin > FLT_MAX*0.1 || vertIndex<0)
			continue;

		for(size_t j=0; j<pDestSkin->m_bones.size(); j++)
		{
			const char * name = pDestSkin->m_bones[j].m_szName;

			size_t s;
			CWeightedVertex& vertex = srcSkins[srcSkinIndex]->m_verts[vertIndex];
			for(s=0; s<vertex.m_idBones.size(); s++)
			{
				int boneid = vertex.m_idBones[s];
				if(stricmp(name, srcSkins[srcSkinIndex]->m_bones[boneid].m_szName) == 0)
				{
					if(vertex.m_weightBones[s]>0.001)
						pDestSkin->SetVertexWeight(i, j, vertex.m_weightBones[s]);
				}
			}
		}
	}
	pDestSkin->NormalizeAllVertsWeight();
}

void CSkinMapper::NearestEdgeMapping(vector<CSkinedMesh *>& srcSkins, CSkinedMesh * pDestSkin)
{
	for(size_t i=0; i<pDestSkin->m_verts.size(); i++)
	{
		Point3 nearest;
		Point3 pos = pDestSkin->m_verts[i].m_vertPos;
		
		int		srcSkinIndex = -1;
		int		indexEdge[2] = {-1, -1};
		float	weights[2] = {0, 0};

		float disMin = FLT_MAX;
		if(srcSkinIndex<0 || indexEdge[0]<0 || indexEdge[1]<0)
		{
			for(size_t k=0; k<srcSkins.size(); k++)
			{
				float weightThis[2] = {0, 0};
				int indexThis[2] = {-1, -1};
				float disThis = srcSkins[k]->InfluentialEdge(pos, indexThis, weightThis);
				if(disMin > disThis)
				{
					disMin = disThis;
					srcSkinIndex = k;	
					indexEdge[0] = indexThis[0];
					indexEdge[1] = indexThis[1];
					weights[0] = weightThis[0];
					weights[1] = weightThis[1];
				}
			}
		}

		// get nearest intersected triagle in the src skin meshes
		// set weight according to this triangle (3 vertices and its bone id weight)
		if(disMin > FLT_MAX*0.1)
			continue;

		for(size_t j=0; j<pDestSkin->m_bones.size(); j++)
		{
			const char * name = pDestSkin->m_bones[j].m_szName;
			float w[2] = {0, 0};

			size_t s;
			CWeightedVertex* vertex = &(srcSkins[srcSkinIndex]->m_verts[indexEdge[0]]);
			for(s=0; s<vertex->m_idBones.size(); s++)
			{
				int boneid = vertex->m_idBones[s];
				if(stricmp(name, srcSkins[srcSkinIndex]->m_bones[boneid].m_szName) == 0)
				{
					w[0] = vertex->m_weightBones[s];
					break;
				}
			}

			vertex = &(srcSkins[srcSkinIndex]->m_verts[indexEdge[1]]);
			for(s=0; s<vertex->m_idBones.size(); s++)
			{
				int boneid = vertex->m_idBones[s];
				if(stricmp(name, srcSkins[srcSkinIndex]->m_bones[boneid].m_szName) == 0)
				{
					w[1] = vertex->m_weightBones[s];
					break;
				}
			}

			float weight = weights[0]*w[0]+weights[1]*w[1];
			if(weight>0.001)
				pDestSkin->SetVertexWeight(i, j, weight);
		}
	}
	pDestSkin->NormalizeAllVertsWeight();
}

void CSkinMapper::NearestTriangleMapping(vector<CSkinedMesh *>& srcSkins, CSkinedMesh * pDestSkin)
{
	for(size_t i=0; i<pDestSkin->m_verts.size(); i++)
	{
		Point3 nearest;
		Point3 pos = pDestSkin->m_verts[i].m_vertPos;
		
		int		srcSkinIndex = -1;
		int		indexTri[3] = {-1, -1, -1};
		float	weights[3] = {0, 0, 0};

		float disMin = FLT_MAX;
		if(srcSkinIndex<0 || indexTri[0]<0 || indexTri[1]<0 || indexTri[2]<0)
		{
			for(size_t k=0; k<srcSkins.size(); k++)
			{

				float weightThis[3] = {0, 0, 0};
				int indexTriThis[3] = {-1, -1, -1};
				float disThis = srcSkins[k]->InfluentialTriangle(pos, indexTriThis, weightThis);
				if(disMin > disThis)
				{
					disMin = disThis;
					srcSkinIndex = k;	
					indexTri[0] = indexTriThis[0];
					indexTri[1] = indexTriThis[1];
					indexTri[2] = indexTriThis[2];
					weights[0] = weightThis[0];
					weights[1] = weightThis[1];
					weights[2] = weightThis[2];
				}
			}
		}

		// get nearest intersected triagle in the src skin meshes
		// set weight according to this triangle (3 vertices and its bone id weight)
		if(disMin > FLT_MAX*0.1)
			continue;

		for(size_t j=0; j<pDestSkin->m_bones.size(); j++)
		{
			const char * name = pDestSkin->m_bones[j].m_szName;
			float w[3] = {0, 0, 0};

			size_t s;
			CWeightedVertex& vertex = srcSkins[srcSkinIndex]->m_verts[indexTri[0]];
			for(s=0; s<vertex.m_idBones.size(); s++)
			{
				int boneid = vertex.m_idBones[s];
				if(stricmp(name, srcSkins[srcSkinIndex]->m_bones[boneid].m_szName) == 0)
				{
					w[0] = vertex.m_weightBones[s];
					break;
				}
			}

			vertex = srcSkins[srcSkinIndex]->m_verts[indexTri[1]];
			for(s=0; s<vertex.m_idBones.size(); s++)
			{
				int boneid = vertex.m_idBones[s];
				if(stricmp(name, srcSkins[srcSkinIndex]->m_bones[boneid].m_szName) == 0)
				{
					w[1] = vertex.m_weightBones[s];
					break;
				}
			}

			vertex = srcSkins[srcSkinIndex]->m_verts[indexTri[2]];
			for(s=0; s<vertex.m_idBones.size(); s++)
			{
				int boneid = vertex.m_idBones[s];
				if(stricmp(name, srcSkins[srcSkinIndex]->m_bones[boneid].m_szName) == 0)
				{
					w[2] = vertex.m_weightBones[s];
					break;
				}
			}

			float weight = weights[0]*w[0]+weights[1]*w[1]+weights[2]*w[2];
			if(weight>0.001)
					pDestSkin->SetVertexWeight(i, j, weight);
		}
	}
	pDestSkin->NormalizeAllVertsWeight();
}
