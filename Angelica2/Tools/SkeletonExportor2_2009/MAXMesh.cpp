 /*
 * FILE: MAXMesh.cpp
 *
 * DESCRIPTION: 3DMAX mesh class
 *
 * CREATED BY: duyuxin, 2003/9/15
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "MAXMesh.h"
#include "MAXInterface.h"
#include "SkeletonCandidate.h"
#include "VertexCandidate.h"
#include "BoneCandidate.h"
#include "MorphChannel.h"

#include "modstack.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Angelica muscle modifier class ID
#define MUSCLESETUP_CLASS_ID	Class_ID(0x40bb834e, 0x6da119a0)

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
//	Implement CMAXMesh
//
///////////////////////////////////////////////////////////////////////////

CMAXMesh::CMAXMesh()
{
	m_pINode		= NULL;
	m_pTriObject	= NULL;
	m_pModifier		= NULL;
	m_iModifier		= MODIFIER_NONE;
	m_bDelete		= false;
	m_bDefaultMtl	= false;
	m_bSelDefMtl	= true;
	m_bCullFlip		= false;
	m_bNormalFlip	= false;
	m_iBoneIndex	= -1;
	m_bMorph		= false;
	m_bMuscle		= false;
//	m_pMorphR3		= NULL;
	m_iNumFace		= 0;
	m_iNumVert		= 0;
	
	m_pPhysiqueExport	= NULL;
	m_pPhyContextExport	= NULL;
	m_pSkinExport		= NULL;
	m_pSkinContextData	= NULL;
}

CMAXMesh::~CMAXMesh()
{
	if (m_bDelete) 
		delete m_pTriObject;

	int i;

	//	Release all material node
	for (i=0; i < m_aStdMat.GetSize(); i++)
		delete m_aStdMat[i];

	m_aStdMat.RemoveAll();

	//	Release all morph channels
	for (i=0; i < m_aMorphChannels.GetSize(); i++)
	{
		CMorphChannel* pChannel = m_aMorphChannels[i];
		pChannel->Release();
		delete pChannel;
	}

	m_aMorphChannels.RemoveAll();
}

bool CMAXMesh::Create(INode* pINode)
{
	m_pINode = pINode;

	//	Try to get morph modifier
/*	if ((m_pMorphR3 = (MorphR3*)FindMorphModifier(pINode)) != NULL)
		m_bMorph = true;
	else if (FindMuscleModifier(pINode))
		m_bMuscle = true;
*/
	if (FindMuscleModifier(pINode))
		m_bMuscle = true;

	//	Try to get the physique modifier
	if (!(m_pModifier = FindPhysiqueModifier(pINode)))
	{
		//	Try to get the skin modifier
		if (!(m_pModifier = FindSkinModifier(pINode)))
		{
			m_iModifier = MODIFIER_NONE;
			a_SendLogToDebugWnd(0, "Node %s: No physique/skin modifier found.", m_pINode->GetName());
		}
		else
			m_iModifier = MODIFIER_SKIN;
	}
	else
	{
		m_iModifier = MODIFIER_PHYSIQUE;

		//	Physique and modifier cannot exist at same time
		m_bMorph = false;
	}

	//	Create mesh object
//	TimeValue time = SecToTicks(g_pMAXInterface->GetCurrentTime());
	TimeValue time = g_pMAXInterface->GetStartTime();
	ObjectState os = m_pINode->EvalWorldState(time);

	m_pTriObject = (TriObject*)os.obj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));
	if (!m_pTriObject)
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::Create, Couldn't convert to triangle object.");
		return false;
	}

	Mesh* pIMesh = &m_pTriObject->mesh;
	m_bDelete	= (m_pTriObject != os.obj) ? true : false;
	m_iNumFace	= pIMesh->getNumFaces();
	m_iNumVert	= pIMesh->getNumVerts();

	//	Precalculate the object transformation matrix
	m_tmObject	= m_pINode->GetObjectTM(time);
//	m_tmObject	= m_pINode->GetObjTMAfterWSM(time);
	m_tmNode	= m_pINode->GetNodeTM(time);

	if (m_bMorph)
	{
		//	Create morph channels
		if (!CreateMorphChannels())
		{
			a_SendLogToDebugWnd(1, "CMAXMesh::Create, Couldn't to create morph vertex buffer.");
			m_bMorph = false;
		}
	}

	//	Create initial mesh
	if (m_iModifier == MODIFIER_PHYSIQUE)
	{
		if (!CreatePhysiqueInitMesh())
		{
			a_SendLogToDebugWnd(1, "CMAXMesh::Create, Failed to call CreatePhysiqueInitMesh. mesh: %s", m_pINode->GetName());
			return false;
		}
	}
	else if (m_iModifier == MODIFIER_SKIN)
	{
		if (!CreateSkinInitMesh())
		{
			a_SendLogToDebugWnd(1, "CMAXMesh::Create, Failed to call CreateSkinInitMesh. mesh: %s", m_pINode->GetName());
			return false;
		}
	}
	else	//	m_iModifier == MODIFIER_NONE
	{
		if (!CreateRigidInitMesh(pIMesh))
		{
			a_SendLogToDebugWnd(1, "CMAXMesh::Create, Failed to call CreateRigidInitMesh. mesh: %s", m_pINode->GetName());
			return false;
		}
	}

	//	Create materials
	if (!m_pINode->GetMtl() || !CreateMaterial(m_pINode->GetMtl(), -1))
	{
		a_SendLogToDebugWnd(0, "Node %s: use default material", m_pINode->GetName());
		m_bDefaultMtl = true;
	}
	else
		m_bDefaultMtl = false;

	//	Build all normals if necessary
	pIMesh->checkNormals(TRUE);

	m_bCullFlip = true;
	if (DotProd(CrossProd(m_tmObject.GetRow(0), m_tmObject.GetRow(1)), m_tmObject.GetRow(2)) > 0)
		m_bCullFlip = false;

	return true;
}

//	Create mesh object, this function only get modifer
bool CMAXMesh::CreateSimple(INode* pINode)
{
	m_pINode = pINode;

	//	Try to get the physique modifier
	if (!(m_pModifier = FindPhysiqueModifier(pINode)))
	{
		//	Try to get the skin modifier
		if (!(m_pModifier = FindSkinModifier(pINode)))
		{
			m_iModifier = MODIFIER_NONE;
			a_SendLogToDebugWnd(0, "Node %s: No physique/skin modifier found.", m_pINode->GetName());
		}
		else
			m_iModifier = MODIFIER_SKIN;
	}
	else
		m_iModifier = MODIFIER_PHYSIQUE;

	return true;
}

Mesh* CMAXMesh::GetIMesh()
{
	if (!m_pINode)
		return NULL;

	TimeValue time = g_pMAXInterface->GetStartTime();
	ObjectState os = m_pINode->EvalWorldState(time);

	m_pTriObject = (TriObject*)os.obj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));
	if (!m_pTriObject)
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::GetIMesh, Couldn't convert to triangle object.");
		return NULL;
	}

	return &m_pTriObject->mesh;
}

bool CMAXMesh::CreateMaterial(Mtl* pMtl, int iParent)
{
	//	Check for valid material
	if (!pMtl)
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::CreateMaterial, Invalid material handle.");
		return false;
	}

	//	Check if we have a standard material
	if (pMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
	{
		MATERIAL* pMaterial = new MATERIAL;

		pMaterial->iParent		= iParent;
		pMaterial->bSelected	= true;
		pMaterial->pStdMtl		= (StdMat*)pMtl;

		iParent = m_aStdMat.Add(pMaterial);
	}

	//	Handle all submaterials
	for (int i=0; i < pMtl->NumSubMtls(); i++)
		CreateMaterial((StdMat*)pMtl->GetSubMtl(i), iParent);

	return true;
}

Modifier* CMAXMesh::FindPhysiqueModifier(INode* pINode)
{
	//	Get the object reference of the node
	Object* pObject = pINode->GetObjectRef();
	if (!pObject)
		return NULL;

	//	Loop through all derived objects
	while (pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* pDerivedObject = (IDerivedObject*)pObject;

		//	Loop through all modifiers
		int iNumMod = pDerivedObject->NumModifiers();
		for (int i=0; i < iNumMod; i++)
		{
			//	Get the modifier
			Modifier* pModifier = pDerivedObject->GetModifier(i);

			//	Check if we found the physique modifier
			if (pModifier->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
				return pModifier;
		}

		//	Continue with next derived object
		pObject = pDerivedObject->GetObjRef();
	}

	return NULL;
}

Modifier* CMAXMesh::FindSkinModifier(INode* pINode)
{
#if MAX_RELEASE >= 4000

	//	Get the object reference of the node
	Object* pObject = pINode->GetObjectRef();
	if (!pObject)
		return NULL;

	const char* szName = pINode->GetName();

	bool bGetEval = false;
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
				//	Is last one ?
				if (i == iNumMod-1)
				{
					bGetEval = true;
					break;
				}
				else
				{
					m_Initos = pDerivedObject->Eval(0, i+1);
					return pModifier;
				}
			}
		}

		// continue with next derived object
		pObject = pDerivedObject->GetObjRef();

		if (bGetEval && pObject)
		{
			if (pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
				m_Initos = ((IDerivedObject*)pObject)->Eval(0, 0);
			else
				m_Initos = pObject->Eval(0);

			return pModifier;
		}
	}

#endif

	return NULL;
}

//	Find morph modifier
Modifier* CMAXMesh::FindMorphModifier(INode* pINode)
{
	//	Get the object reference of the node
/*	Object* pObject = pINode->GetObjectRef();
	if (!pObject)
		return NULL;

	//	Loop through all derived objects
	while (pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* pDerivedObject = (IDerivedObject*)pObject;

		//	Loop through all modifiers
		int iNumMod = pDerivedObject->NumModifiers();
		for (int i=0; i < iNumMod; i++)
		{
			//	Get the modifier
			Modifier* pModifier = pDerivedObject->GetModifier(i);

			//	Check if we found the morph modifier
			if (pModifier->ClassID() == MR3_CLASS_ID)
				return pModifier;
		}

		//	Continue with next derived object
		pObject = pDerivedObject->GetObjRef();
	}
*/
	return NULL;
}

//	Find muscle modifier
Modifier* CMAXMesh::FindMuscleModifier(INode* pINode)
{
	//	Get the object reference of the node
	Object* pObject = pINode->GetObjectRef();
	if (!pObject)
		return NULL;

	//	Loop through all derived objects
	while (pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* pDerivedObject = (IDerivedObject*)pObject;

		//	Loop through all modifiers
		int iNumMod = pDerivedObject->NumModifiers();
		for (int i=0; i < iNumMod; i++)
		{
			//	Get the modifier
			Modifier* pModifier = pDerivedObject->GetModifier(i);

			//	Check if we found the Angelica muscle modifier
			if (pModifier->ClassID() == MUSCLESETUP_CLASS_ID)
				return pModifier;
		}

		//	Continue with next derived object
		pObject = pDerivedObject->GetObjRef();
	}

	return NULL;
}

int CMAXMesh::GetFaceMaterialId(Mesh* pIMesh, int iFace)
{
	if (!pIMesh && !(pIMesh = GetIMesh()))
		return -1;

	//	Get the material id of the face
	int iMaterial =	pIMesh->getFaceMtlIndex(iFace);

	//	Make sure we return a valid material id in all cases:
	//	3ds max assigns predefined material ids to primitives. these material
	//	ids can be higher than the actual material count of the mesh itself.
	//	the official workaround is to calculate the modulo of the value to the
	//	number of existing materials.
	if (m_aStdMat.GetSize() > 0)
		iMaterial %= m_aStdMat.GetSize();
	else
		iMaterial = 0;

	return iMaterial;
}

Point3 CMAXMesh::GetVertexNormal(int iFace, int iVertex, Mesh* pIMesh)
{
	if (!pIMesh && !(pIMesh = GetIMesh()))
		return Point3(0.0f, 0.0f, 0.0f);

	//	Get the "rendered" vertex
	RVertex* pRVertex = pIMesh->getRVertPtr(iVertex);

	//	Get the face
	Face* pFace = &pIMesh->faces[iFace];

	//	Get the smoothing group of the face
	DWORD smGroup = pFace->smGroup;

	//	Get the number of normals
	int iNormalCount = pRVertex->rFlags & NORCT_MASK;

	//	Gheck if the normal is specified ...
	if (pRVertex->rFlags & SPECIFIED_NORMAL)
		return pRVertex->rn.getNormal();
	else if (iNormalCount > 0 && smGroup != 0)	//	... otherwise, Check for a smoothing group
	{
		//	If there is only one vertex is found in the rn member.
		if (iNormalCount == 1)
			return pRVertex->rn.getNormal();
		else
		{
			for (int i=0; i < iNormalCount; i++)
			{
				if (pRVertex->ern[i].getSmGroup() & smGroup)
					return pRVertex->ern[i].getNormal();
			}
		}
	}

	//	If all fails, return the face normal
	return pIMesh->getFaceNormal(iFace);
}

Matrix3 CMAXMesh::Transpose(Matrix3& mat)
{
	float Trans[4][3];

	int iRow, iCol;

	for (iRow=0; iRow < 3; iRow++)
	{
		for (iCol=0; iCol < 3; iCol++)
			Trans[iCol][iRow] = mat.GetAddr()[iRow][iCol];
	}

	for (iCol=0; iCol < 3; iCol++)
		Trans[3][iCol] = 0;
	
	Matrix3 matTranspose(Trans);
	matTranspose.NoTrans();

	return matTranspose;
}

StdMat* CMAXMesh::GetMAXMaterial(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_aStdMat.GetSize())
		return NULL;

	return m_aStdMat[iIndex]->pStdMtl;
}

CMAXMesh::MATERIAL* CMAXMesh::GetMaterial(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_aStdMat.GetSize())
		return NULL;

	return m_aStdMat[iIndex];
}

//	Get material's parent
int CMAXMesh::GetMaterialParent(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_aStdMat.GetSize())
		return -2;

	return m_aStdMat[iIndex]->iParent;
}

bool CMAXMesh::IsMaterialSelected(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_aStdMat.GetSize())
		return false;

	return m_aStdMat[iIndex]->bSelected;
}

//	Select / Deselect a material
void CMAXMesh::SelectMaterial(int iIndex, bool bSelect)
{
	if (iIndex >= 0 && iIndex < m_aStdMat.GetSize())
		m_aStdMat[iIndex]->bSelected = bSelect;
}

//	Select / Deselect all materials
void CMAXMesh::SelectAllMaterial(bool bSelect)
{
	for (int i=0; i < m_aStdMat.GetSize(); i++)
		m_aStdMat[i]->bSelected = bSelect;
}

//	Inverse select materials
void CMAXMesh::InverseSelectMaterial()
{
	for (int i=0; i < m_aStdMat.GetSize(); i++)
		m_aStdMat[i]->bSelected = !m_aStdMat[i]->bSelected;
}

//	Get spring vertex candidate
CSpringVertex* CMAXMesh::GetSpringVertex(int iVertex, Mesh* pIMesh)
{
	if (!pIMesh && !(pIMesh = GetIMesh()))
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::GetSpringVertex, Failed to get IMesh object.");
		return NULL;
	}

	//	Check if vertex id is valid
	if (iVertex < 0 || iVertex >= pIMesh->getNumVerts())
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::GetSpringVertex, Invalid vertex index.");
		return NULL;
	}

	//	Allocate a new spring vertex candidate
	CSpringVertex* pSpringVertex = new CSpringVertex;
	if (!pSpringVertex)
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::GetSpringVertex, Not enough memory.");
		return NULL;
	}

	pSpringVertex->SetVertexID(iVertex);

	//	Get the absolute vertex position, don't forget to exchange y and z
	Point3 vertex = m_aInitVerts[iVertex];
//	Point3 vertex = pIMesh->getVert(iVertexId) * m_tmObject;
	pSpringVertex->SetPos(vertex.x, vertex.z, vertex.y);

	//	Get the vertex weight (if possible)
	float* aVertWeights = pIMesh->getVertexWeights();
	float fWeight;

	if (aVertWeights)
		fWeight = aVertWeights[iVertex];
	else
		fWeight = 0.0f;
	
	if (fWeight < 0.0005f) 
		fWeight = 0.0f;
	else if (fWeight > 1.0f)
		fWeight = 1.0f;

	//	Set the vertex candidate weight
	pSpringVertex->SetPhysicalProperty(fWeight, 0, -1);

	return pSpringVertex;
}

//	Prepare physique or skin modifier
bool CMAXMesh::PrepareModifiers()
{
	if (m_iModifier == MODIFIER_PHYSIQUE)
	{
		//	Create a physique export interface
		m_pPhysiqueExport = (IPhysiqueExport*)m_pModifier->GetInterface(I_PHYINTERFACE);
		if (!m_pPhysiqueExport)
		{
			a_SendLogToDebugWnd(1, "CMAXMesh::PrepareModifiers, Physique modifier interface not found.");
			return false;
		}

		//	Create a context export interface
		m_pPhyContextExport = (IPhyContextExport*)m_pPhysiqueExport->GetContextInterface(m_pINode);
		if (!m_pPhyContextExport)
		{
			m_pModifier->ReleaseInterface(I_PHYINTERFACE, m_pPhysiqueExport);
			m_pPhyContextExport = NULL;
			a_SendLogToDebugWnd(1, "CMAXMesh::PrepareModifiers, Context export interface not found.");
			return false;
		}

		//	Set the flags in the context export interface
		m_pPhyContextExport->ConvertToRigid(TRUE);
		m_pPhyContextExport->AllowBlending(TRUE);
	}

#if MAX_RELEASE >= 4000

	//	Check for skin modifier
	else if (m_iModifier == MODIFIER_SKIN)
	{
		//	Create a skin interface
		m_pSkinExport = (ISkin*)m_pModifier->GetInterface(I_SKIN);
		if (!m_pSkinExport)
		{
			a_SendLogToDebugWnd(1, "CMAXMesh::PrepareModifiers, Skin modifier interface not found.");
			return false;
		}

		//	Create a skin context data interface
		m_pSkinContextData = (ISkinContextData*)m_pSkinExport->GetContextInterface(m_pINode);
		if (!m_pSkinContextData)
		{
			m_pModifier->ReleaseInterface(I_SKIN, m_pSkinExport);
			m_pSkinExport = NULL;
			a_SendLogToDebugWnd(1, "CMAXMesh::PrepareModifiers, Skin context data interface not found.");
			return false;
		}

		//	Check vertex number
		if (GetVertexCount() != m_pSkinContextData->GetNumPoints())
		{
			m_pModifier->ReleaseInterface(I_SKIN, m_pSkinExport);
			m_pSkinExport = NULL;
			a_SendLogToDebugWnd(1, "CMAXMesh::PrepareModifiers, Vertex number don't match.");
			return false;
		}
	}

#endif	//	MAX_RELEASE

	return true;
}

//	Release physique or skin modifier
void CMAXMesh::ReleaseModifiers()
{
	if (m_iModifier == MODIFIER_PHYSIQUE)
	{
		if (m_pPhysiqueExport)
		{
			m_pPhysiqueExport->ReleaseContextInterface(m_pPhyContextExport);
			m_pModifier->ReleaseInterface(I_PHYINTERFACE, m_pPhysiqueExport);
		}

		m_pPhyContextExport	= NULL;
		m_pPhysiqueExport	= NULL;
	}

#if MAX_RELEASE >= 4000

	//	Check for skin modifier
	else if (m_iModifier == MODIFIER_SKIN)
	{
		//	Release all interfaces
		if (m_pSkinExport)
		{
			m_pModifier->ReleaseInterface(I_SKIN, m_pSkinExport);
			m_pSkinExport = NULL;
		}

		m_pSkinContextData = NULL;
	}

#endif	//	MAX_RELEASE
}

CVertexCandidate* CMAXMesh::GetVertexCandidate(CSkeletonCandidate* pSkeletonCandidate,
											   int iFace, int iVertex, Mesh* pIMesh)
{
	if (!pIMesh && !(pIMesh = GetIMesh()))
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Failed to get IMesh object.");
		return NULL;
	}

	//	Check if face id is valid
	if (iFace < 0 || iFace >= pIMesh->getNumFaces())
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Invalid face index.");
		return NULL;
	}

	//	Check if face vertex id is valid
	if (iVertex < 0 || iVertex >= 3)
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Invalid vertex index.");
		return NULL;
	}

	//	Allocate a new vertex candidate
	CVertexCandidate* pVertexCandidate = new CVertexCandidate();
	if (!pVertexCandidate)
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Not enough memory.");
		return NULL;
	}

	int iVertexId = pIMesh->faces[iFace].v[iVertex];
	pVertexCandidate->SetVertexID(iVertexId);

	//	Get the absolute vertex position, don't forget to exchange y and z
	Point3 vertex = m_aInitVerts[iVertexId];
//	Point3 vertex1 = pIMesh->getVert(iVertexId) * m_tmObject;
	pVertexCandidate->SetPos(vertex.x, vertex.z, vertex.y);

	//	Get the absolute vertex normal, don't forget to exchange y and z
	//	Note: normal got here is the normal of frame 0, later, we will re-calculate 
	//		  every vertex's normal in every initial mesh (mesh at initial time)
	//		  Calculate normal here is only used to distinguish different vertices
	Point3 normal = GetVertexNormal(iFace, iVertexId, pIMesh);
	normal = normal * Inverse(Transpose(m_tmObject));
	normal = normal.Normalize();

	pVertexCandidate->SetNormal(normal.x, normal.z, normal.y);

	//	Get texture coordinates
	if (m_bDefaultMtl)
	{
		pVertexCandidate->SetNormalTextureCoord(0.0f, 0.0f);
	}
	else
	{
	/*	Mesh* pIMesh = GetIMesh();
		if (!pIMesh)
		{
			a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Failed to get IMesh.");
			return NULL;
		}
	*/
		//	Get the material id of the face
		int iMtlId = GetFaceMaterialId(pIMesh, iFace);
		if (iMtlId < 0 || iMtlId >= m_aStdMat.GetSize())
		{
			delete pVertexCandidate;
			a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Invalid material id found.");
			return NULL;
		}

		//	Get the material of the face
		StdMat* pStdMat = m_aStdMat[iMtlId]->pStdMtl;

		//	Get texture map
		Texmap* pTexMap = pStdMat->GetSubTexmap(ID_DI);
		if (pTexMap)
		{
			bool bValidUV = false;

			//	Get the mapping channel
			int iChannel = pTexMap->GetMapChannel();

			//	Extract the texture coordinate
			UVVert uvVert;
			if (pIMesh->mapSupport(iChannel))
			{
				TVFace* pTVFace = pIMesh->mapFaces(iChannel);
				UVVert* pUVVert = pIMesh->mapVerts(iChannel);
				
				uvVert = pUVVert[pTVFace[iFace].t[iVertex]];
				bValidUV = true;
			}
			else if (pIMesh->numTVerts > 0)
			{
				uvVert = pIMesh->tVerts[pIMesh->tvFace[iFace].t[iVertex]];
				bValidUV = true;
			}

			//	If we found valid texture coordinates, add them to the vertex candidate
			if (bValidUV)
			{
				StdUVGen* pStdUVGen = (StdUVGen*)pTexMap->GetTheUVGen();
				if (pStdUVGen)
				{
					Matrix3 tmUV;
					pStdUVGen->GetUVTransform(tmUV);
					uvVert = uvVert * tmUV;
				}

				//	Set normal texture coordinate to the vertex candidate
				pVertexCandidate->SetNormalTextureCoord(uvVert.x, 1.0f - uvVert.y);
			}
			else
				pVertexCandidate->SetNormalTextureCoord(0.0f, 0.0f);
		}
		else
			pVertexCandidate->SetNormalTextureCoord(0.0f, 0.0f);
	}

	//	Check for physique modifier
	if (m_iModifier == MODIFIER_PHYSIQUE)
	{
		//	Create a physique export interface
	/*	IPhysiqueExport* pPhysiqueExport = (IPhysiqueExport*)m_pModifier->GetInterface(I_PHYINTERFACE);
		if (!pPhysiqueExport)
		{
			delete pVertexCandidate;
			a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Physique modifier interface not found.");
			return NULL;
		}

		//	Create a context export interface
		IPhyContextExport* pContextExport = (IPhyContextExport*)pPhysiqueExport->GetContextInterface(m_pINode);
		if (!pContextExport)
		{
			m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
			delete pVertexCandidate;
			a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Context export interface not found.");
			return NULL;
		}

		//	Set the flags in the context export interface
		pContextExport->ConvertToRigid(TRUE);
		pContextExport->AllowBlending(TRUE);
	*/
		if (!m_pPhysiqueExport || !m_pPhyContextExport)
			return NULL;
		
		//	Get the vertex export interface
		IPhyVertexExport* pVertexExport = (IPhyVertexExport*)m_pPhyContextExport->GetVertexInterface(iVertexId);
		if (!pVertexExport)
		{
			delete pVertexCandidate;
			a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Vertex export interface not found.");
			return NULL;
		}

		//	Get the vertex type
		int iVertexType = pVertexExport->GetVertexType();

		//	Handle the specific vertex type
		if (iVertexType == RIGID_NON_BLENDED_TYPE)
		{
			IPhyRigidVertex* pTypeVertex = (IPhyRigidVertex*)pVertexExport;

			//	Add the influence to the vertex candidate get the influencing bone
			if (!AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, pTypeVertex->GetNode(), 1.0f))
			{
				delete pVertexCandidate;
				a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Invalid bone assignment.");
				return NULL;
			}
		}
		else if (iVertexType == RIGID_BLENDED_TYPE)
		{
			IPhyBlendedRigidVertex* pTypeVertex = (IPhyBlendedRigidVertex*)pVertexExport;

			//	Loop through all influencing bones
			for (int i=0; i < pTypeVertex->GetNumberNodes(); i++)
			{
				//	Add the influence to the vertex candidate
				if (!AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, pTypeVertex->GetNode(i), pTypeVertex->GetWeight(i)))
				{
					delete pVertexCandidate;
					a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Invalid bone assignment.");
					return NULL;
				}
			}
		}

		//	Release all interfaces
		m_pPhyContextExport->ReleaseVertexInterface(pVertexExport);
	}

#if MAX_RELEASE >= 4000

	//	Check for skin modifier
	else if (m_iModifier == MODIFIER_SKIN)
	{
		//	Create a skin interface
	/*	ISkin* pSkin = (ISkin*)m_pModifier->GetInterface(I_SKIN);
		if (!pSkin)
		{
			delete pVertexCandidate;
			a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Skin modifier interface not found.");
			return NULL;
		}

		//	Create a skin context data interface
		ISkinContextData* pSkinContextData = (ISkinContextData*)pSkin->GetContextInterface(m_pINode);
		if (!pSkinContextData)
		{
			m_pModifier->ReleaseInterface(I_SKIN, pSkin);
			delete pVertexCandidate;
			a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Skin context data interface not found.");
			return NULL;
		}
	*/
		if (!m_pSkinExport || !m_pSkinContextData)
			return NULL;

		int iMaxVert = m_pSkinContextData->GetNumPoints();
		if (iVertexId < iMaxVert)
		{
			//	Loop through all influencing bones
			for (int i=0; i < m_pSkinContextData->GetNumAssignedBones(iVertexId); i++)
			{
				//	Get the bone id
				int iBoneId = m_pSkinContextData->GetAssignedBone(iVertexId, i);
				if (iBoneId < 0) 
					continue;

				//	Add the influence to the vertex candidate
				if (!AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, m_pSkinExport->GetBone(iBoneId), m_pSkinContextData->GetBoneWeight(iVertexId, i)))
				{
					delete pVertexCandidate;
					a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Invalid bone assignment.");
					return NULL;
				}
			}
		}

		//	Release all interfaces
	}

#endif

	else	//	m_iModifier == MODIFIER_NONE
	{
		//	Handle as rigid mesh
		pVertexCandidate->SetRigidFlag(true);

		if (!AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, m_pINode, 1.0f))
		{
			delete pVertexCandidate;
			a_SendLogToDebugWnd(1, "CMAXMesh::GetVertexCandidate, Invalid bone assignment.");
			return NULL;
		}
	}

	return pVertexCandidate;
}

bool CMAXMesh::AddBoneInfluence(CSkeletonCandidate* pSkeletonCandidate, CVertexCandidate* pVertexCandidate, INode* pNode, float fWeight)
{
	//	Get the bone id of the bone from the skeleton candidate
//	int iBoneId = pSkeletonCandidate->GetBoneId(pNode->GetName());
	int iBoneId = pSkeletonCandidate->GetBoneSelectedId(pNode->GetName());
	if (iBoneId == -1)
	{
		AString str = pNode->GetName();
		return false;
	}

	//	Add the influence to the vertex candidate
	pVertexCandidate->AddInfluence(iBoneId, fWeight);

	return true;
}

//	Set bone's initialize TM of this mesh
void CMAXMesh::SetBoneInitTM(CSkeletonCandidate* pSkeletonCandidate)
{
	//	For rigid mesh
	if (m_iModifier == MODIFIER_NONE)
		m_iBoneIndex = pSkeletonCandidate->GetBoneSelectedId(m_pINode->GetName());

	//	Get bone candidate vector
	APtrArray<CBoneCandidate*>& aBones = pSkeletonCandidate->GetBoneArray();

	for (int iBone=0; iBone < aBones.GetSize(); iBone++)
	{
		//	Get the bone candidate
		CBoneCandidate* pBoneCandidate = aBones[iBone];
		SetOneBoneInitTM(pSkeletonCandidate, pBoneCandidate);
	}
}

//	Set one bone's initialize TM of this mesh
bool CMAXMesh::SetOneBoneInitTM(CSkeletonCandidate* pSkeletonCandidate, CBoneCandidate* pBoneCandidate)
{
	INode* pBoneNode = pBoneCandidate->GetNode()->GetINode();

	if (m_iModifier == MODIFIER_PHYSIQUE)
	{
		IPhysiqueExport* pPhysiqueExport = (IPhysiqueExport*)m_pModifier->GetInterface(I_PHYINTERFACE);
		if (!pPhysiqueExport)
			return false;

		Matrix3 mat;
		int iRet = pPhysiqueExport->GetInitNodeTM(pBoneNode, mat);

		//	Release all interfaces
		m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);

		if (iRet == MATRIX_RETURNED)
		{
			//	Ok, We got the matrix
			Point3 vInitScale = g_pMAXInterface->GetMatrixScale(mat);
			Point3 vCurScale = pBoneCandidate->GetNodeTMScale() / vInitScale;
			mat.NoScale();
			mat.Invert();
			mat.Scale(vCurScale, TRUE);
			pBoneCandidate->SetInitTM(mat);
			return true;
		}
	}

#if MAX_RELEASE >= 4000

	//	Check for skin modifier
	else if (m_iModifier == MODIFIER_SKIN)
	{
		//	Create a skin interface
		ISkin* pSkin = (ISkin*)m_pModifier->GetInterface(I_SKIN);
		if (!pSkin)
			return false;
		
		Matrix3 mat;
		int iRet = pSkin->GetBoneInitTM(pBoneNode, mat);

		//	Release all interfaces
		m_pModifier->ReleaseInterface(I_SKIN, pSkin);

		if (iRet == SKIN_OK)
		{
			//	Ok, we got the matrix
			Point3 vInitScale = g_pMAXInterface->GetMatrixScale(mat);
			Point3 vCurScale = pBoneCandidate->GetNodeTMScale() / vInitScale;
			mat.NoScale();
			mat.Invert();
			mat.Scale(vCurScale, TRUE);
			pBoneCandidate->SetInitTM(mat);
			return true;
		}
	}

#endif	//	MAX_RELEASE

	else	//	m_iModifier == MODIFIER_NONE
	{
		//	Rigid mesh
		if (m_pINode == pBoneCandidate->GetNode()->GetINode())
		{
			Matrix3 mat;
			mat.IdentityMatrix();
			pBoneCandidate->SetInitTM(mat);
			return true;
		}
	}

	return false;
}

//	Create physique initial mesh
bool CMAXMesh::CreatePhysiqueInitMesh()
{
	IPhysiqueExport* pPhysiqueExport = (IPhysiqueExport*)m_pModifier->GetInterface(I_PHYINTERFACE);
	if (!pPhysiqueExport)
		return false;

	//	Create a ModContext Export Interface for the specific node of the Physique Modifier
	IPhyContextExport* pContextExport = (IPhyContextExport*)pPhysiqueExport->GetContextInterface(m_pINode);
	if (!pContextExport)
		return false;

	//	We convert all vertices to Rigid in this example
	pContextExport->ConvertToRigid(TRUE);
	pContextExport->AllowBlending(TRUE);

	m_aInitVerts.SetSize(m_iNumVert, 50);

	//	Compute the transformed Point3 at time t
	for (int i=0; i < m_iNumVert; i++)
	{
		IPhyVertexExport* pVertexExport = pContextExport->GetVertexInterface(i);
		if (!pVertexExport)
			return false;

		//	Get the vertex type
		int iVertexType = pVertexExport->GetVertexType();

	    //	Need to check if vertex has blending
		if (iVertexType == RIGID_BLENDED_TYPE)
		{
			IPhyBlendedRigidVertex* pBlendVertex = (IPhyBlendedRigidVertex*)pVertexExport;

			Point3 vBlendP(0.0f, 0.0f, 0.0f);
			for (int n=0; n < pBlendVertex->GetNumberNodes(); n++)
			{
				INode* pBone	= pBlendVertex->GetNode(n);
				Point3 vOffset	= pBlendVertex->GetOffsetVector(n);
				float fWeight	= pBlendVertex->GetWeight(n);

				Matrix3 matInit;
				if (pPhysiqueExport->GetInitNodeTM(pBone, matInit) == MATRIX_RETURNED)
					vBlendP += (matInit * vOffset) * fWeight;
			}
	
			m_aInitVerts[i] = vBlendP;
		}
		else if (iVertexType == RIGID_NON_BLENDED_TYPE)
		{
			IPhyRigidVertex* pRigidVertex = (IPhyRigidVertex*)pVertexExport;
			INode* pBone	= pRigidVertex->GetNode();
			Point3 vOffset	= pRigidVertex->GetOffsetVector();
            
		//	a_SendLogToDebugWnd(0, "vertex %d offset: %f, %f, %f.", i, vOffset.x, vOffset.y, vOffset.z);

			Matrix3 matInit;
			if (pPhysiqueExport->GetInitNodeTM(pBone, matInit) == MATRIX_RETURNED)
				m_aInitVerts[i] = matInit * vOffset;
			else
			{
				//	This shouldn't happen
				a_SendLogToDebugWnd(1, "CMAXMesh::CreatePhysiqueInitMesh, not assigned bone was found.");
				m_aInitVerts[i] = vOffset;
			}
		}

		pContextExport->ReleaseVertexInterface(pVertexExport);
	}
    
	pPhysiqueExport->ReleaseContextInterface(pContextExport);
	m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
	
	return true;
}

//	Create skin initial mesh
bool CMAXMesh::CreateSkinInitMesh()
{
#if MAX_RELEASE >= 4000

	bool bDelete = false;

	TriObject* pTriObject = (TriObject*)m_Initos.obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));
	if (!pTriObject)
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::CreateSkinInitMesh, Couldn't convert to triangle object.");
		return false;
	}

	Mesh* pIMesh = &pTriObject->mesh;
	bDelete = (pTriObject != m_Initos.obj) ? true : false;

	//	Create a skin interface
	ISkin* pSkin = (ISkin*)m_pModifier->GetInterface(I_SKIN);
	if (!pSkin)
		return false;

	//	tm is the result made by all modifiers before skin modifier in object space
	Matrix3 tm = *m_Initos.GetTM();
	Matrix3 InitTM;
	
	if (pSkin->GetSkinInitTM(m_pINode, InitTM, true) != SKIN_OK)
	{
		a_SendLogToDebugWnd(1, "CMAXMesh::CreateSkinInitMesh, Failed to get skin initTM.");
		m_pModifier->ReleaseInterface(I_SKIN, pSkin);

		if (bDelete)
			delete pTriObject;

		return false;
	}

	int iNumVert = pIMesh->getNumVerts();
	m_aInitVerts.SetSize(iNumVert, 50);

	tm = tm * InitTM;

	m_bNormalFlip = true;
	if (DotProd(CrossProd(tm.GetRow(0), tm.GetRow(1)), tm.GetRow(2)) > 0)
		m_bNormalFlip = false;

	if (m_bMorph)
	{
		for (int i=0; i < iNumVert; i++)
		{
			Point3 v = pIMesh->getVert(i);
			m_aInitVerts[i] = v * tm;

			CalcMorphVertexPosDelta(i, v, tm);
		}
	}
	else
	{
		for (int i=0; i < iNumVert; i++)
		{
			Point3 v = pIMesh->getVert(i);
			m_aInitVerts[i] = v * tm;
		}
	}

	//	Release all interfaces
	m_pModifier->ReleaseInterface(I_SKIN, pSkin);

	if (bDelete)
		delete pTriObject;

	return true;

#else
	
	return false;

#endif	//	MAX_RELEASE
}

//	Create rigid mesh's (non-skin and non-physique) initial mesh
bool CMAXMesh::CreateRigidInitMesh(Mesh* pIMesh)
{
	if (!pIMesh || !m_iNumVert)
		return false;

	m_aInitVerts.SetSize(m_iNumVert, 50);

	Matrix3 mat = g_pMAXInterface->NormalizeTM(m_tmNode);
	mat.Invert();
	mat = m_tmObject * mat;

	m_bNormalFlip = true;
	if (DotProd(CrossProd(mat.GetRow(0), mat.GetRow(1)), mat.GetRow(2)) > 0)
		m_bNormalFlip = false;

	if (m_bMorph)
	{
		for (int i=0; i < m_iNumVert; i++)
		{
			Point3 v = pIMesh->getVert(i);
			m_aInitVerts[i] = v * mat;

			CalcMorphVertexPosDelta(i, v, mat);
		}
	}
	else
	{
		for (int i=0; i < m_iNumVert; i++)
		{
			Point3 v = pIMesh->getVert(i);
			m_aInitVerts[i] = v * mat;
		}
	}

	return true;
}

//	Create morph channels
bool CMAXMesh::CreateMorphChannels()
{
/*	if (!m_bMorph || !m_pMorphR3)
		return false;

	for (int i=0; i < MR3_NUM_CHANNELS; i++) 
	{
		morphChannel* pMAXChannel = &m_pMorphR3->chanBank[i];
		if (!pMAXChannel->mActive)
			continue;
	
		CMorphChannel* pChannel = new CMorphChannel;
		if (!pChannel)
			continue;

		if (!pChannel->Init(m_pMorphR3, i, pMAXChannel->mName))
		{
			delete pChannel;
			a_SendLogToDebugWnd(1, "CMAXMesh::CreateMorphChannels, Failed to init morph channel.");
			continue;
		}

		m_aMorphChannels.Add(pChannel);
	}
*/	
	return true;
}

//	Calculate specified morph vertex position
bool CMAXMesh::CalcMorphVertexPosDelta(int iVert, const Point3& vOrigin, const Matrix3& matTM)
{
	for (int i=0; i < m_aMorphChannels.GetSize(); i++)
	{
		CMorphChannel* pChannel = m_aMorphChannels[i];
		pChannel->CalcVertexPosDelta(iVert, vOrigin, matTM);
	}

	return true;
}


