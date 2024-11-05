/*
 * FILE: MeshCandidate.cpp
 *
 * DESCRIPTION: Mesh candidate class
 *
 * CREATED BY: duyuxin, 2003/9/16
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "wm3.h"
#include "MeshCandidate.h"
#include "VertexCandidate.h"
#include "BoneCandidate.h"
#include "SkeletonCandidate.h"
#include "MAXInterface.h"
#include "MorphChannel.h"
#include "MAXMesh.h"
#include "AAssist.h"

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

APtrArray<CMorphChannel*>*	l_aChannels = NULL;		//	Used when sorting channels

int		l_iCurFrame = 0;

//	Vertex struct used to calculate normal
struct TEMPVERT
{
	A3DVECTOR3	vPos;
	A3DVECTOR3	vNormal;
	int			iNormalCnt;
};

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

static int _ChannelCompare(const void* arg1, const void* arg2);
static int _SpringCompare(const void* arg1, const void* arg2);

///////////////////////////////////////////////////////////////////////////
//
//	Implement CMeshCandidate
//
///////////////////////////////////////////////////////////////////////////

CMeshCandidate::CMeshCandidate() : m_aVertices(0, 300), m_aFaces(0, 300), m_aSprings(0, 300)
{
	m_pMAXMesh		= NULL;
	m_iTexIndex		= -1;
	m_iMtlIndex		= -1;
	m_iBoneIndex	= -1;
	m_bAlphaMesh	= false;
	
	m_bSuppleMesh			= false;
	m_fSpringCoefficient	= 0.8f;
	m_fGravityFactor		= 1.0f;
	m_fAirResist			= 0.3f;
	m_fVertWeight			= 1.0f;
	
	m_vMeshCenter.Clear();
}

//	Initialize object
bool CMeshCandidate::Init(CMAXMesh* pMAXMesh, int iMtlIndex, int iTexIndex, int iRigidBone,
						  bool bAlphaMesh)
{
	m_pMAXMesh		= pMAXMesh;
	m_iTexIndex		= iTexIndex;
	m_iMtlIndex		= iMtlIndex;
	m_iBoneIndex	= iRigidBone;
	m_bAlphaMesh	= bAlphaMesh;

	//	Decide whether this is a supple mesh
	INode* pINode = m_pMAXMesh->GetINode();

	BOOL bVal;
	if (pINode->GetUserPropBool("柔体", bVal))
		m_bSuppleMesh = bVal ? true : false;

	if (m_bSuppleMesh)
	{
		//	Get spring coefficient
		float fVal;
		if (pINode->GetUserPropFloat("柔体连接系数", fVal))
		{
			m_fSpringCoefficient = fVal;
			a_ClampFloor(m_fSpringCoefficient, 0.0f);
		}
		
		if (pINode->GetUserPropFloat("柔体重力系数", fVal))
		{
			m_fGravityFactor = fVal;
			a_ClampFloor(m_fGravityFactor, 0.0f);
		}

		if (pINode->GetUserPropFloat("柔体空气阻力", fVal))
		{
			m_fAirResist = fVal;
			a_ClampFloor(m_fAirResist, 0.0f);
		}

		if (pINode->GetUserPropFloat("柔体顶点质量", fVal))
		{
			m_fVertWeight = fVal;
			a_ClampFloor(m_fVertWeight, 0.00001f);
		}
	}

	return true;
}

//	Release object
void CMeshCandidate::Release()
{
	int i;

	//	Release all vertices
	for (i=0; i < m_aVertices.GetSize(); i++)
	{
		CVertexCandidate* pVert = m_aVertices[i];
		delete pVert;
	}

	m_aVertices.RemoveAll();

	//	Release all spring vertices
	for (i=0; i < m_aSpringVerts.GetSize(); i++)
	{
		CSpringVertex* pVert = m_aSpringVerts[i];
		delete pVert;
	}

	m_aSpringVerts.RemoveAll();

	//	Release all morph channels
	for (i=0; i < m_aMorphChannels.GetSize(); i++)
	{
		CMorphChannel* pChannel = m_aMorphChannels[i];
		pChannel->Release();
		delete pChannel;
	}

	m_aMorphChannels.RemoveAll();
}

//	Is morph mesh ?
bool CMeshCandidate::IsMorphMesh()
{ 
	return m_pMAXMesh->GetMorphFlag();
}

//	Is muscle mesh ?
bool CMeshCandidate::IsMuscleMesh()
{
	return m_pMAXMesh->GetMuscleFlag();
}

//	Add vertex candidate
int CMeshCandidate::AddVertexCandidate(CVertexCandidate* pVertexCandidate)
{
	//	For morph mesh, we must keep vertex information coincident with MAX's mesh
/*	if (IsMorphMesh())
	{
		int iVertex = pVertexCandidate->GetVertexID();

		for (int i=0; i < m_aVertices.GetSize(); i++)
		{
			CVertexCandidate* pVert = m_aVertices[i];

			if (pVert->GetVertexID() == iVertex)
			{
				//	The vertex has existed
				delete pVertexCandidate;
				return i;
			}
		}
	}
	else
*/	{
		//	Loop through the existing vertex candidates
		for (int i=0; i < m_aVertices.GetSize(); i++)
		{
			CVertexCandidate* pVert = m_aVertices[i];

			//	Check if we have an equal vertex candidate
			bool bMatch = false;

			if (m_bSuppleMesh)
				bMatch = pVertexCandidate->CompareNoNormal(*pVert);
			else
				bMatch = (*pVertexCandidate == *pVert);

			if (bMatch)
			{
				//	We found one, delete the submitted one
				delete pVertexCandidate;
				return i;
			}
		}
	}

	return m_aVertices.Add(pVertexCandidate);
}

//	Adjust bone assignment
void CMeshCandidate::AdjustBoneAssignment(int iMaxBone, float fMinWeight)
{
	for (int i=0; i < m_aVertices.GetSize(); i++)
	{
		CVertexCandidate* pVert = m_aVertices[i];
		pVert->AdjustBoneAssignment(iMaxBone, fMinWeight);
	}
}

//	Add a face to mesh
bool CMeshCandidate::AddFace(WORD v1, WORD v2, WORD v3)
{
	FACE Face;
	Face.aVertIdx[0] = v1;
	Face.aVertIdx[1] = v2;
	Face.aVertIdx[2] = v3;

	m_aFaces.Add(Face);

	return true;
}

//	Calcualte vertex normal
bool CMeshCandidate::CalculateNormals()
{
/*	int i;

	//	Initialize data
	for (i=0; i < m_aVertices.GetSize(); i++)
	{
		CVertexCandidate* pVert = m_aVertices[i];
		pVert->m_vSmoothNormal.Set(0.0f, 0.0f, 0.0f);
		pVert->m_iNormalCnt = 0;
	}

	//	Note: We have converted MAX's anti-clockwise faces to D3D's clockwise faces
	for (i=0; i < m_aFaces.GetSize(); i++)
	{
		A3DVECTOR3 vEdge1, vEdge2, vNormal;
		const FACE& Face = m_aFaces[i];

		CVertexCandidate* pVert0 = m_aVertices[Face.aVertIdx[0]];
		CVertexCandidate* pVert1 = m_aVertices[Face.aVertIdx[1]];
		CVertexCandidate* pVert2 = m_aVertices[Face.aVertIdx[2]];

		vEdge1 = pVert1->GetPos() - pVert0->GetPos();
		vEdge2 = pVert2->GetPos() - pVert0->GetPos();
		vNormal.CrossProduct(vEdge1, vEdge2);
		vNormal.Normalize();
		
		pVert0->m_vSmoothNormal += vNormal;
		pVert1->m_vSmoothNormal += vNormal;
		pVert2->m_vSmoothNormal += vNormal;
		pVert0->m_iNormalCnt++;
		pVert1->m_iNormalCnt++;
		pVert2->m_iNormalCnt++;
	}

	//	Average the face normals to create proper vertex normal;
	for (i=0; i < m_aVertices.GetSize(); i++)
	{
		CVertexCandidate* pVert = m_aVertices[i];

		if (!pVert->m_iNormalCnt)
			continue;	//	Isoleted vertex;

		pVert->m_vSmoothNormal /= pVert->m_iNormalCnt;
		pVert->m_vSmoothNormal.Normalize();
	}

	return true;
*/
	Mesh* pIMesh = m_pMAXMesh->GetIMesh();
	int i, iNumVert = m_pMAXMesh->GetVertexCount();

	AArray<TEMPVERT, TEMPVERT&>	aVerts;
	aVerts.SetSize(iNumVert, 100);

	//	Initialize data
	for (i=0; i < iNumVert; i++)
	{
		TEMPVERT& Vert = aVerts[i];
		const Point3& vPos = m_pMAXMesh->GetInitPos(i);

		Vert.vPos.Set(vPos.x, vPos.z, vPos.y);
		Vert.vNormal.Clear();
		Vert.iNormalCnt = 0;
	}

	int iNumFace = pIMesh->getNumFaces();

	for (i=0; i < iNumFace; i++)
	{
		//	Note: We have converted MAX's anti-clockwise faces to D3D's clockwise faces
		int v1 = pIMesh->faces[i].v[0];
		int v2 = pIMesh->faces[i].v[2];
		int v3 = pIMesh->faces[i].v[1];

		TEMPVERT& Vert0 = aVerts[v1];
		TEMPVERT& Vert1 = aVerts[v2];
		TEMPVERT& Vert2 = aVerts[v3];

		A3DVECTOR3 vEdge1 = Vert1.vPos - Vert0.vPos;
		A3DVECTOR3 vEdge2 = Vert2.vPos - Vert0.vPos;

		A3DVECTOR3 vNormal;
		vNormal.CrossProduct(vEdge1, vEdge2);
		vNormal.Normalize();
		
		Vert0.vNormal += vNormal;
		Vert1.vNormal += vNormal;
		Vert2.vNormal += vNormal;
		Vert0.iNormalCnt++;
		Vert1.iNormalCnt++;
		Vert2.iNormalCnt++;
	}

	bool bNormalFlip = m_pMAXMesh->GetNormalFlipFlag();

	for (i=0; i < iNumVert; i++)
	{
		TEMPVERT& v = aVerts[i];

		if (!v.iNormalCnt)
			continue;	//	Isoleted vertex

		v.vNormal /= v.iNormalCnt;
		v.vNormal.Normalize();

		if (bNormalFlip)
			v.vNormal = -v.vNormal;
	}

	//	Average the face normals to create proper vertex normal;
	for (i=0; i < m_aVertices.GetSize(); i++)
	{
		CVertexCandidate* pVert = m_aVertices[i];
		TEMPVERT& v = aVerts[pVert->GetVertexID()];

		pVert->m_vSmoothNormal = v.vNormal;
	}

	return true;
}

//	Build morph data
bool CMeshCandidate::BuildMorphData(CSkeletonCandidate* pSkeletonCandidate)
{
	if (!IsMorphMesh())
		return true;

	APtrArray<CMorphChannel*>& aChannels = m_pMAXMesh->GetMorphChannelArray();
	int i, iNumChannel = aChannels.GetSize();

	if (!iNumChannel)
		return true;

	m_iStartFrame	= pSkeletonCandidate->GetStartFrame();
	m_iFPS			= pSkeletonCandidate->GetAnimFPS();
	m_iNumFrame		= pSkeletonCandidate->GetEndFrame() - pSkeletonCandidate->GetStartFrame() + 1;

	for (i=0; i < iNumChannel; i++)
	{
		CMorphChannel* pChannel = aChannels[i];

		//	Translate morph channels from MAX mesh to us
		m_aMorphChannels.Add(pChannel);

		//	Build channel weights
		pChannel->BuildChannelWeights(m_iStartFrame, m_iNumFrame, m_iFPS);
	}

	//	Remove channels in MAX mesh
	aChannels.RemoveAll();

	//	Prepare to sort channels
	l_aChannels = &m_aMorphChannels;

	for (i=0; i < m_iNumFrame; i++)
	{
		BYTE* aBuf = new BYTE [iNumChannel];
		if (!aBuf)
		{
			a_SendLogToDebugWnd(1, "CMeshCandidate::BuildMorphData, Not enough memory.");
			return false;
		}

		m_aChannelSorts.Add(aBuf);

		//	Initialize indices
		for (int j=0; j < iNumChannel; j++)
			aBuf[j] = (BYTE)j;

		//	Sort channels of this frame
		l_iCurFrame = i;
		qsort(aBuf, iNumChannel, sizeof (BYTE), _ChannelCompare);
	}

	//	Rebuild morph channels position deltas
	RebuildMorphChannelPosDeltas();

	//	Calcuate normal dealtas
	CalcMorphChannelNomralDeltas();
	
	return true;
}

//	Compare function used to sort channel
int _ChannelCompare(const void* arg1, const void* arg2)
{
	ASSERT(l_aChannels);

	CMorphChannel* pChannel1 = (*l_aChannels)[*(BYTE*)arg1];
	CMorphChannel* pChannel2 = (*l_aChannels)[*(BYTE*)arg2];

	float w1 = (float)fabs(pChannel1->GetWeight(l_iCurFrame));
	float w2 = (float)fabs(pChannel2->GetWeight(l_iCurFrame));

	if (w1 < w2)
		return 1;
	else if (w1 > w2)
		return -1;
	else
		return 0;
}

//	Rebuild morph channel position deltas
void CMeshCandidate::RebuildMorphChannelPosDeltas()
{
	AArray<A3DVECTOR3, A3DVECTOR3&> aPosDeltas;
	int i, j, iNumVert = m_aVertices.GetSize();
	
	aPosDeltas.SetSize(iNumVert, 100);

	//	Initialize delta
	for (i=0; i < iNumVert; i++)
		aPosDeltas[i].Clear();

	for (i=0; i < m_aMorphChannels.GetSize(); i++)
	{
		CMorphChannel* pChannel = (*l_aChannels)[i];

		for (j=0; j < iNumVert; j++)
		{
			CVertexCandidate* pVert = m_aVertices[j];
			aPosDeltas[j] = pChannel->GetPosDelta(pVert->GetVertexID());
		}

		AArray<A3DVECTOR3, A3DVECTOR3&>& aPosDest = pChannel->GetPosDeltaArray();
		aPosDest.SetSize(iNumVert, 100);

		for (j=0; j < iNumVert; j++)
			aPosDest[j]	= aPosDeltas[j];
	}
}

//	Calculate morph channel normal delta
void CMeshCandidate::CalcMorphChannelNomralDeltas()
{
	int i, j;
	AArray<A3DVECTOR3, A3DVECTOR3&> aVerts;
	A3DVECTOR3 vDelta;

	int iNumVert = m_aVertices.GetSize();
	aVerts.SetSize(iNumVert, 100);

	for (i=0; i < m_aMorphChannels.GetSize(); i++)
	{
		CMorphChannel* pChannel = m_aMorphChannels[i];
		AArray<A3DVECTOR3, A3DVECTOR3&>& aNormals = pChannel->GetNormalDeltaArray();
		aNormals.SetSize(iNumVert, 100);

		//	Build channel positions
		for (j=0; j < iNumVert; j++)
		{
			CVertexCandidate* pVert = m_aVertices[j];
			vDelta = pChannel->GetPosDelta(j);
			aVerts[j] = pVert->GetPos() + vDelta * 100.0f;

			aNormals[j].Clear();
		}

		//	Calculate channel normals
		//	Note: We have converted MAX's anti-clockwise faces to D3D's clockwise faces
		for (j=0; j < m_aFaces.GetSize(); j++)
		{
			const FACE& Face = m_aFaces[j];
			int v0 = Face.aVertIdx[0];
			int v1 = Face.aVertIdx[1];
			int v2 = Face.aVertIdx[2];

			A3DVECTOR3 vEdge1, vEdge2, vNormal;

			vEdge1 = aVerts[v1] - aVerts[v0];
			vEdge2 = aVerts[v2] - aVerts[v0];
			vNormal.CrossProduct(vEdge1, vEdge2);
			vNormal.Normalize();
		
			aNormals[v0] += vNormal;
			aNormals[v1] += vNormal;
			aNormals[v2] += vNormal;
		}

		//	Average the face normals to create proper vertex normal;
		for (j=0; j < iNumVert; j++)
		{
			aNormals[j].Normalize();

			//	Save normal delta
			CVertexCandidate* pVert = m_aVertices[j];
			aNormals[j] = (aNormals[j] - pVert->m_vSmoothNormal) * 0.01f;
		}
	}
}

//	Calculate mesh center
void CMeshCandidate::CalcMeshCenter()
{
	A3DVECTOR3 vCenter(0.0f);

	int iNumVert = m_aVertices.GetSize();
	if (!iNumVert)
		return;

	for (int i=0; i < iNumVert; i++)
	{
		CVertexCandidate* pVert = m_aVertices[i];
		vCenter += pVert->GetPos();
	}

	vCenter /= iNumVert;
}

//	This is a merge-able mesh ?
bool CMeshCandidate::CanBeMerged()
{
	if (IsMorphMesh() || IsAlphaMesh() || IsSuppleMesh())
		return false;

	if (IsRigidMesh() && !IsSuppleMesh())
		return false;

	return true;
}

//	Check whether mesh can be merged with specified mesh ?
bool CMeshCandidate::CanBeMergedWith(CMeshCandidate* pMesh)
{
	if (!CanBeMerged() || !pMesh->CanBeMerged())
		return false;

	if (GetTextureIndex() != pMesh->GetTextureIndex() ||
		GetMaterialIndex() != pMesh->GetMaterialIndex())
		return false;
	
	return true;
}

/*	Check whether there are isolate vertices in spring system. Isolate vertex
	is the vertex which isn't connected with constraint (weight=0.0) vertex.

	Return true if there is isolate vertex, otherwise return false.
*/
bool CMeshCandidate::FindIsolateVertex()
{
	//	Only check for supple mesh
	if (!IsSuppleMesh())
		return false;

	//	Teachable vertices
	AArray<int, int> aTeachable(m_aSpringVerts.GetSize(), 1000);

	//	Loop through all the vertex candidates of the mesh
	for (int i=0; i < m_aSpringVerts.GetSize(); i++)
	{
		if (m_aSpringVerts[i]->GetPhysicalProperty().fBoneFactor == 0.0f)
			continue;
	
		//	Clear records
		aTeachable.RemoveAll(false);

		//	Add itself
		aTeachable.Add(i);

		for (int j=0; j < aTeachable.GetSize(); j++)
		{
			CSpringVertex* pVert = m_aSpringVerts[aTeachable[j]];

			for (int n=0; n < pVert->GetNeighbourNum(); n++)
			{
				int iNeighbour = pVert->GetNeighbour(n);

				//	The neighbour is a constraint vertex ?
				if (m_aSpringVerts[iNeighbour]->GetPhysicalProperty().fBoneFactor == 0.0f)
					goto NextVert;

				aTeachable.UniquelyAdd(iNeighbour);
			}
		}

		//	Cannot reach any constraint vertex
		return true;

	NextVert:;
	}

	return false;
}

//	Build spring data
bool CMeshCandidate::BuildSpringData()
{
	if (!IsSuppleMesh())
		return true;

	//	Build spring vertices
	if (!BuildSpringVertices())
	{
		a_SendLogToDebugWnd(1, "CMeshCandidate::BuildSpringData, Failed to build spring vertices.");
		return false;
	}

	//	Check whether there is isolate vertex
	if (FindIsolateVertex())
	{
		char szMsg[1024];
		sprintf(szMsg, "Mesh [%s] found isolate vertices in spring system.", m_strName);
		AfxMessageBox(szMsg, MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	AArray<int, int> aNeighbours, aNeighbourNeighbours;

	int i, iVertex;

	//	Loop through all the vertex candidates of the mesh
	for (iVertex=0; iVertex < m_aSpringVerts.GetSize(); iVertex++)
	{
		//	Start from the current vertex candidate
		aNeighbours.UniquelyAdd(iVertex);

		//	Get all the neighbours of the current vertex candidate up to the given level
		for (int iLevel=0; iLevel < 2; iLevel++)
		{
			for (i=0; i < aNeighbours.GetSize(); i++)
			{
				CSpringVertex* pVert = m_aSpringVerts[aNeighbours[i]];

				for (int j=0; j < pVert->GetNeighbourNum(); j++)
					aNeighbourNeighbours.UniquelyAdd(pVert->GetNeighbour(j));
			}

			for (i=0; i < aNeighbourNeighbours.GetSize(); i++)
				aNeighbours.UniquelyAdd(aNeighbourNeighbours[i]);
			
			aNeighbourNeighbours.RemoveAll(false);
		}

		//	Add springs to all these neighbours
		for (i=0; i < aNeighbours.GetSize(); i++)
			AddSpring(iVertex, aNeighbours[i]);

		aNeighbours.RemoveAll(false);
	}

	//	Loop until we have a stable system
	while (1)
	{
		//	Clear the modification flag
		bool bModified = false;

		//	Loop through all the springs of the submesh candidate
		for (i=0; i < m_aSprings.GetSize(); i++)
		{
			//	Get the spring
			const SPRING& Spring = m_aSprings[i];

			//	Adjust the two vertices connected to the spring
			bModified |= AdjustSpringVertex(Spring.aVertIdx[0], Spring.aVertIdx[1]);
			bModified |= AdjustSpringVertex(Spring.aVertIdx[1], Spring.aVertIdx[0]);
		}

		if (!bModified)
			break;
	}

	//	Calculate spring's priority
	for (i=0; i < m_aSprings.GetSize(); i++)
	{
		//	Get the spring
		SPRING& Spring = m_aSprings[i];

		//	Get the physical properties of the two spring vertices
		const CSpringVertex::PHYSICALPROP* p1 = &m_aSpringVerts[Spring.aVertIdx[0]]->GetPhysicalProperty();
		const CSpringVertex::PHYSICALPROP* p2 = &m_aSpringVerts[Spring.aVertIdx[1]]->GetPhysicalProperty();

		Spring.iPriority = (p1->iConstraintDist < p2->iConstraintDist) ? p1->iConstraintDist : p2->iConstraintDist;
	}

	//	Sort springs
	SortSprings();

	return true;
}

//	Build spring vertices
bool CMeshCandidate::BuildSpringVertices()
{
	int i, iNumVert = m_pMAXMesh->GetVertexCount();
	Mesh* pIMesh = m_pMAXMesh->GetIMesh();
	
	for (i=0; i < iNumVert; i++)
	{
		CSpringVertex* pSpringVertex = m_pMAXMesh->GetSpringVertex(i, pIMesh);
		if (!pSpringVertex)
		{
			a_SendLogToDebugWnd(1, "CMeshCandidate::BuildSpringVertices, Failed to get spring vertex");
			return false;
		}

		m_aSpringVerts.Add(pSpringVertex);
	}

	//	Fill spring vertex's influence information
	for (i=0; i < m_aVertices.GetSize(); i++)
	{
		CVertexCandidate* pVertexCandidate = m_aVertices[i];
		CSpringVertex* pSpringVertex = m_aSpringVerts[pVertexCandidate->GetVertexID()];

		if (pSpringVertex->GetInfluenceNum())
			continue;	//	Influence information has been set

		for (int j=0; j < pVertexCandidate->GetInfluenceNum(); j++)
		{
			const CVertexCandidate::INFLUENCE& Influence = pVertexCandidate->GetInfluence(j);
			pSpringVertex->AddInfluence(Influence);
		}
	}

	//	Build neighbours
	int iNumFace = m_pMAXMesh->GetFaceCount();

	for (i=0; i < iNumFace; i++)
	{
		int v1 = pIMesh->faces[i].v[0];
		int v2 = pIMesh->faces[i].v[1];
		int v3 = pIMesh->faces[i].v[2];

		//	Add neighbours
		m_aSpringVerts[v1]->AddNeighbour(v2);
		m_aSpringVerts[v1]->AddNeighbour(v3);
		m_aSpringVerts[v2]->AddNeighbour(v1);
		m_aSpringVerts[v2]->AddNeighbour(v3);
		m_aSpringVerts[v3]->AddNeighbour(v1);
		m_aSpringVerts[v3]->AddNeighbour(v2);
	}

	return true;
}

//	Add a spring
bool CMeshCandidate::AddSpring(int v1, int v2)
{
	//	Check for valid vertices
	if (v1 == v2)
		return false;

	//	Make the lower vertex id the first one
	if (v1 > v2)
		a_Swap(v1, v2);

	CSpringVertex* pVert1 = m_aSpringVerts[v1];
	CSpringVertex* pVert2 = m_aSpringVerts[v2];

	//	Get the physical property of each vertex
	CSpringVertex::PHYSICALPROP aPhysicalProps[2];
	aPhysicalProps[0] = pVert1->GetPhysicalProperty();
	aPhysicalProps[1] = pVert2->GetPhysicalProperty();

	//	At least on of the vertices must have a weight
	if (aPhysicalProps[0].fBoneFactor == 0.0f && aPhysicalProps[1].fBoneFactor == 0.0f)
		return false;

	//	Check if we have an equal spring
	for (int i=0; i < m_aSprings.GetSize(); i++)
	{
		if (m_aSprings[i].aVertIdx[0] == v1 && m_aSprings[i].aVertIdx[1] == v2)
			return false;
	}
	
	//	Create a spring object
	SPRING Spring;
	Spring.aVertIdx[0]	= v1;
	Spring.aVertIdx[1]	= v2;
	Spring.fCoefficient = m_fSpringCoefficient;

	//	Calculate the spring length
	A3DVECTOR3 vDelta = pVert1->GetPos() - pVert2->GetPos();
	Spring.iIdleLen = vDelta.Magnitude();

	m_aSprings.Add(Spring);

	//	Adjust the number of springs and the minimal distance to a constraint vertex of both vertices
	aPhysicalProps[0].iSpringCnt++;
	if (aPhysicalProps[0].fBoneFactor == 0.0f)
		aPhysicalProps[0].iConstraintDist = 0;
	pVert1->SetPhysicalProperty(aPhysicalProps[0]);

	aPhysicalProps[1].iSpringCnt++;
	if (aPhysicalProps[1].fBoneFactor == 0.0f) 
		aPhysicalProps[1].iConstraintDist = 0;
	pVert2->SetPhysicalProperty(aPhysicalProps[1]);

	return true;
}

/*	Adjust spring vertex

	Return true if vertex has been adjusted, otherwise return false.
*/
bool CMeshCandidate::AdjustSpringVertex(int v1, int v2)
{
	//	Get the physical property of each vertex
	CSpringVertex::PHYSICALPROP aPhysicalProps[2];
	aPhysicalProps[0] = m_aSpringVerts[v1]->GetPhysicalProperty();
	aPhysicalProps[1] = m_aSpringVerts[v2]->GetPhysicalProperty();
	
	//	Adjust the distance to the nearest constraint vertex if necessary
	if (aPhysicalProps[0].iConstraintDist == -1 || 
		aPhysicalProps[0].iConstraintDist > aPhysicalProps[1].iConstraintDist + 1)
	{
		if (aPhysicalProps[1].iConstraintDist != -1)
		{
			aPhysicalProps[0].iConstraintDist = aPhysicalProps[1].iConstraintDist + 1;
			m_aSpringVerts[v1]->SetPhysicalProperty(aPhysicalProps[0]);
		}

		return true;
	}

	return false;
}

//	Sort springs
void CMeshCandidate::SortSprings()
{
	if (!IsSuppleMesh())
		return;

	//	Sort springs
	qsort(m_aSprings.GetData(), m_aSprings.GetSize(), sizeof (SPRING), _SpringCompare);
}

//	Sort spring in nondescending order.
int _SpringCompare(const void* arg1, const void* arg2)
{
	CMeshCandidate::SPRING* pSpring1 = (CMeshCandidate::SPRING*)arg1;
	CMeshCandidate::SPRING* pSpring2 = (CMeshCandidate::SPRING*)arg2;

	if (pSpring1->iPriority < pSpring2->iPriority)
		return -1;
	else if (pSpring1->iPriority > pSpring2->iPriority)
		return 1;
	else
		return 0;
}


