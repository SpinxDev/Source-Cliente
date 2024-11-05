/*
 * FILE: MAXMesh.h
 *
 * DESCRIPTION: 3DMAX mesh class
 *
 * CREATED BY: duyuxin, 2003/9/15
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _MAXMESH_H_
#define _MAXMESH_H_

#include "max.h"
#include "stdmat.h"
//	#include "Morpher\\wm3.h"
#include "AString.h"
#include "AArray.h"
#include "A3DTypes.h"

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
class CBoneCandidate;
class MorphR3;
class CMorphChannel;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CMAXMesh
//
///////////////////////////////////////////////////////////////////////////

class CMAXMesh
{
public:		//	Types

	enum
	{
		MODIFIER_NONE,
		MODIFIER_SKIN,
		MODIFIER_PHYSIQUE,
	};

	struct MATERIAL
	{
		int		iParent;		//	Parent material's index
		StdMat*	pStdMtl;		//	Material object
		bool	bSelected;		//	Selected flag
	};

	//	Vertex normal
	struct VERTEXNORMAL
	{
		Point3			vNormal;	//	Normal
		DWORD			dwSmooth;	//	Smooth group flag
		VERTEXNORMAL*	pNext;

		VERTEXNORMAL(const Point3& n, DWORD s);
		//	Add a face normal
		void AddNormal(const Point3& n, DWORD s);
		//	Normalize
		void Normalize();
	};

public:		//	Constructions and Destructions

	CMAXMesh();
	virtual ~CMAXMesh();

public:		//	Attributes

public:		//	Operaitons

	//	Create mesh object
	bool Create(INode* pINode);
	//	Create mesh object, this function only get modifer
	bool CreateSimple(INode* pINode);

	int GetFaceCount() { return m_iNumFace; }
	int GetVertexCount() { return m_iNumVert; }
	int GetFaceMaterialId(Mesh* pIMesh, int iFace);
	StdMat* GetMAXMaterial(int iIndex);
	MATERIAL* GetMaterial(int iIndex);
	const Point3& GetInitPos(int n) { return m_aInitVerts[n]; }

	//	Get material's parent
	int GetMaterialParent(int iIndex);
	//	Check whether a material is selected ?
	bool IsMaterialSelected(int iIndex);
	//	Select / Deselect a material
	void SelectMaterial(int iIndex, bool bSelect);
	//	Select / Deselect all materials
	void SelectAllMaterial(bool bSelect);
	//	Inverse select materials
	void InverseSelectMaterial();
	//	Select / Deselect default material
	void SelectDefMaterial(bool bSelect) { m_bSelDefMtl = bSelect; }
	//	Is default material selected ?
	bool IsDefMaterialSelected() { return m_bSelDefMtl; }
	//	Is one material mesh ?
	bool IsOneMaterialMesh() { return (UseDefaultMaterial() || GetMaterialCount() == 1) ? true : false; }

	//	Get normal vertex candidate
	CVertexCandidate* GetVertexCandidate(CSkeletonCandidate* pSkeletonCandidate, int iFace, int iCorner, Mesh* pIMesh);
	//	Get spring vertex candidate
	CSpringVertex* GetSpringVertex(int iVertex, Mesh* pIMesh);

	//	Set all associated bones' initialize TM of this mesh
	void SetBoneInitTM(CSkeletonCandidate* pSkeletonCandidate);
	//	Is rigid mesh ?
	bool IsRigidMesh() { return m_iModifier == MODIFIER_NONE ? true : false; }
	//	Set / Get Index of bone which this mesh linked to. For rigid mesh only
	void SetBoneIndex(int iIndex) { m_iBoneIndex = iIndex; }
	int GetBoneIndex() { return m_iBoneIndex; }

	//	Get morph channel number
	int GetMorphChannelNum() { m_aMorphChannels.GetSize(); }
	//	get morph channel
	CMorphChannel* GetMorphChannel(int n) { m_aMorphChannels[n]; }
	//	Get morph channel array
	APtrArray<CMorphChannel*>& GetMorphChannelArray() { return m_aMorphChannels; }
	
	//	Prepare physique or skin modifier
	bool PrepareModifiers();
	//	Release physique or skin modifier
	void ReleaseModifiers();

	INode* GetINode() { return m_pINode; }
	Mesh* GetIMesh();
	int GetMaterialCount() { return m_aStdMat.GetSize(); }
	bool UseDefaultMaterial() { return m_bDefaultMtl; }
	bool GetCullFlipFlag() { return m_bCullFlip; }
	bool GetNormalFlipFlag() { return m_bNormalFlip; }
	bool GetMorphFlag() { return m_bMorph; }
	bool GetMuscleFlag() { return m_bMuscle; } 

protected:	//	Attributes

	INode*		m_pINode;
	TriObject*	m_pTriObject;
	Modifier*	m_pModifier;		//	Modifier pointer
//	MorphR3*	m_pMorphR3;			//	Morph modifier
	int			m_iModifier;		//	Modifier type
	bool		m_bDelete;
	bool		m_bDefaultMtl;		//	true, use default material
	bool		m_bSelDefMtl;		//	Default material selected flag
	Matrix3		m_tmObject;
	Matrix3		m_tmNode;
	Matrix3		m_tmInitObject;		//	TM from object space to world space in initial binding state
	Matrix3		m_tmSkinInit;
	bool		m_bCullFlip;		//	Cull flip flag
	bool		m_bNormalFlip;		//	Normal flip flag
	bool		m_bMorph;			//	Morph mesh
	bool		m_bMuscle;			//	Muscle mesh
	int			m_iBoneIndex;		//	Used by rigid bone
	bool		m_bNormalReady;		//	true, all vertex normals have been calculated

	int			m_iNumFace;			//	Number mesh face
	int			m_iNumVert;			//	Number of mesh vertex
	
	ObjectState	m_Initos;			//	Initialize os state

	IPhysiqueExport*	m_pPhysiqueExport;
	IPhyContextExport*	m_pPhyContextExport;
	ISkin*				m_pSkinExport;
	ISkinContextData*	m_pSkinContextData;

	APtrArray<MATERIAL*>		m_aStdMat;
	APtrArray<CMorphChannel*>	m_aMorphChannels;	//	Morph channels
	AArray<Point3>				m_aInitVerts;		//	Mesh initial vertices
	APtrArray<VERTEXNORMAL*>	m_aVertNormals;		//	Vertex normals

protected:	//	Operations

	//	Add bone influence
	bool AddBoneInfluence(CSkeletonCandidate* pSkeletonCandidate, CVertexCandidate* pVertexCandidate, INode* pNode, float fWeight);
	//	Set one bone's initialize TM of this mesh
	bool SetOneBoneInitTM(CSkeletonCandidate* pSkeletonCandidate, CBoneCandidate* pBoneCandidate);
	//	Create material
	bool CreateMaterial(Mtl* pMtl, int iParent);
	//	Create physique initial mesh
	bool CreatePhysiqueInitMesh();
	//	Create skin initial mesh
	bool CreateSkinInitMesh();
	//	Create rigid mesh's (non-skin and non-physique) initial mesh
	bool CreateRigidInitMesh(Mesh* pIMesh);
	//	Create morph channels
	bool CreateMorphChannels();
	//	Calculate specified morph vertex position delta
	bool CalcMorphVertexPosDelta(int iVert, const Point3& vOrigin, const Matrix3& matTM);
	//	Create vertex normals
	bool CreateVertexNormals();
	//	Calculate angles of a face's 3 corners
	bool CalcFaceAngles(const Point3& v0, const Point3& v1, const Point3& v2, float* aAngles);

	//	Find physique modifier
	Modifier* FindPhysiqueModifier(INode* pINode);
	//	Find skin modifier
	Modifier* FindSkinModifier(INode* pINode);
	//	Find morph modifier
	Modifier* FindMorphModifier(INode* pINode);
	//	Find muscle modifier
	Modifier* FindMuscleModifier(INode* pINode);

	Point3 GetVertexNormal(int iFace, int iCorner, int idVertex, Mesh* pIMesh);
	Matrix3 Transpose(Matrix3& mat);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_MAXMESH_H_

