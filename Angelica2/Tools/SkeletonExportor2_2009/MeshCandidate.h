/*
 * FILE: MeshCandidate.h
 *
 * DESCRIPTION: Mesh candidate class
 *
 * CREATED BY: duyuxin, 2003/9/16
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _MESHCANDIDATE_H_
#define _MESHCANDIDATE_H_

#include "max.h"
#include "AArray.h"
#include "AString.h"
#include "A3DVector.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CVertexCandidate;
class CSpringVertex;
class CSkeletonCandidate;
class CMorphChannel;
class CMAXMesh;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CMeshCandidate
//
///////////////////////////////////////////////////////////////////////////

class CMeshCandidate
{
public:		//	Types

	//	Face structure
	struct FACE
	{
		int		aVertIdx[3];
	};

	//	Spring structure
	struct SPRING
	{
		int		aVertIdx[2];
		float	fCoefficient;
		float	iIdleLen;
		int		iPriority;
	};

public:		//	Constructions and Destructions

	CMeshCandidate();
	virtual ~CMeshCandidate() {}

public:		//	Attributes

public:		//	Operaitons

	//	Initialize object
	bool Init(CMAXMesh* pMAXMesh, int iMtlIndex, int iTexIndex, int iRigidBone, bool bAlphaMesh);
	//	Release object
	void Release();

	//	Add vertex candidate
	int AddVertexCandidate(CVertexCandidate* pVertexCandidate);
	//	Adjust bone assignment
	void AdjustBoneAssignment(int iMaxBone, float fMinWeight);
	//	Add a face to mesh
	bool AddFace(WORD v1, WORD v2, WORD v3);
	//	Calcualte vertex normal
	bool CalculateNormals();

	//	Build morph data
	bool BuildMorphData(CSkeletonCandidate* pSkeletonCandidate);
	//	Calculate mesh center
	void CalcMeshCenter();
	//	Build spring data
	bool BuildSpringData();
	//	Sort springs
	void SortSprings();

	//	Get vertex number
	int GetVertexNum() { return m_aVertices.GetSize(); }
	//	Get vertex candidate
	CVertexCandidate* GetVertexCandidate(int n) { return m_aVertices[n]; }
	//	Get spring vertex number
	int GetSpringVertexNum() { return m_aSpringVerts.GetSize(); }
	//	Get spring vertex
	CSpringVertex* GetSpringVertex(int n) { return m_aSpringVerts[n]; }

	//	Set / Get Index of bone which this mesh linked to
	void SetBoneIndex(int iIndex) { m_iBoneIndex = iIndex; }
	int GetBoneIndex() { return m_iBoneIndex; }
	//	Is rigid mesh ?
	bool IsRigidMesh() { return m_iBoneIndex >= 0 ? true : false; }
	//	Is morph mesh ?
	bool IsMorphMesh();
	//	Is muscle mesh ?
	bool IsMuscleMesh();
	//	Is alpha mesh ?
	bool IsAlphaMesh() { return m_bAlphaMesh; }
	//	Get mesh center
	const A3DVECTOR3& GetMeshCenter() { return m_vMeshCenter; }
	//	Is supple mesh ?
	bool IsSuppleMesh() { return m_bSuppleMesh; }
	//	Get vertex gravity factor
	float GetGravityFactor() { return m_fGravityFactor; }
	//	Get air resistance factor
	float GetAirResistance() { return m_fAirResist; }
	//	Get vertex weight
	float GetVertWeight() { return m_fVertWeight; }

	//	The mesh can be merged with other mesh ?
	//	Some kinds of mesh counldn't be merged.
	//	rigid mesh: rigid meshes bound to different bone, so they couldn't be merged
	//	morph mesh: mesh meshes are assoicated with morph channels, so they couldn't 
	//				be merged.
	//	alpha mesh: alpha will be sorted by the distance that is between they and 
	//				camera when rendered, so they couldn't be merged
	bool CanBeMerged();
	//	Check whether mesh can be merged with specified mesh ?
	bool CanBeMergedWith(CMeshCandidate* pMesh);

	int GetMorphChannelNum() { return m_aMorphChannels.GetSize(); }
	CMorphChannel* GetMorphChannel(int n) { return m_aMorphChannels[n]; }
	BYTE* GetMorphChannelSort(int n) { return m_aChannelSorts[n]; }

	int GetAnimStartFrame() { return m_iStartFrame; }
	int GetAnimFrameNum() { return m_iNumFrame; }
	int GetAnimFPS() { return m_iFPS; }

	//	Set / Get name
	void SetName(const char* szName) { m_strName = szName; }
	const char* GetName() { return m_strName; }

	int GetTextureIndex() { return m_iTexIndex; }
	int GetMaterialIndex() { return m_iMtlIndex; }
	APtrArray<CVertexCandidate*>* GetVertices() { return &m_aVertices; }

	//	Get face number
	int GetFaceNum() { return m_aFaces.GetSize(); }
	//	Get face
	const FACE& GetFace(int n) { return m_aFaces[n]; }
	//	Get face array
	AArray<FACE, FACE&>* GetFaceArray() { return &m_aFaces; }
	//	Get spring number
	int GetSpringNum() { return m_aSprings.GetSize(); }
	//	Get spring
	const SPRING& GetSpring(int n) { return m_aSprings[n]; }
	//	Get spring array
	AArray<SPRING, SPRING&>* GetSpringArray() { return &m_aSprings; }
	//	Get MAX mesh object
	CMAXMesh* GetMAXMesh() { return m_pMAXMesh; }

protected:	//	Attributes

	CMAXMesh*	m_pMAXMesh;		//	Mesh candidate
	AString		m_strName;		//	Mesh name
	bool		m_bAlphaMesh;	//	Alpha mesh flag
	A3DVECTOR3	m_vMeshCenter;	//	Mesh center
	bool		m_bSuppleMesh;	//	Supple mesh flag

	int			m_iTexIndex;	//	Texture index
	int			m_iMtlIndex;	//	Material index
	int			m_iBoneIndex;	//	Used by rigid mesh

	int			m_iStartFrame;	//	Start frame
	int			m_iNumFrame;	//	Frame number
	int			m_iFPS;			//	Animation FPS

	float		m_fSpringCoefficient;	//	Spring coefficient if this is a supple mesh
	float		m_fGravityFactor;		//	Vertex gravity factor
	float		m_fAirResist;			//	Air resistance factor
	float		m_fVertWeight;			//	Vertex weight

	APtrArray<CVertexCandidate*>	m_aVertices;		//	Vertex candidates
	APtrArray<CMorphChannel*>		m_aMorphChannels;	//	Morph channels
	APtrArray<BYTE*>				m_aChannelSorts;	//	Sorted channel indices for every frame
	AArray<FACE, FACE&>				m_aFaces;			//	Face array
	AArray<SPRING, SPRING&>			m_aSprings;			//	Spring array
	APtrArray<CSpringVertex*>		m_aSpringVerts;		//	Spring vertices

protected:	//	Operations

	//	Rebuild morph channel position deltas
	void RebuildMorphChannelPosDeltas();
	//	Calculate morph channel normal delta
	void CalcMorphChannelNomralDeltas();
	//	Build spring vertices
	bool BuildSpringVertices();
	//	Add a spring
	bool AddSpring(int v1, int v2);
	//	Adjust spring vertex
	bool AdjustSpringVertex(int v1, int v2);
	//	Check whether there are isolate vertices in spring system. 
	bool FindIsolateVertex();
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_MESHCANDIDATE_H_

