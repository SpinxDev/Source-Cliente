/*
 * FILE: SkinCandidate.h
 *
 * DESCRIPTION: Skin candidate class
 *
 * CREATED BY: duyuxin, 2003/9/16
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _SKINCANDIDATE_H_
#define _SKINCANDIDATE_H_

#include "max.h"
#include "AString.h"
#include "AArray.h"
#include "A3DTypes.h"
#include "A3DGeometry.h"

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

class CSkeletonCandidate;
class A3DMaterial;
class CMAXMesh;
class CMAXNode;
class CMeshCandidate;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CSkinCandidate
//
///////////////////////////////////////////////////////////////////////////

class CSkinCandidate
{
public:		//	Types

	//	Bone's bounding box
	struct BONEBOX
	{
		bool		bValid;		//	Valid flag
		int			iBone;		//	Bone index
		A3DAABB		aabb;		//	aabb in bone space
	};

public:		//	Constructions and Destructions

	CSkinCandidate();
	virtual ~CSkinCandidate();

public:		//	Attributes

public:		//	Operaitons

	//	Initialize object
	bool Init();
	//	Release object
	void Release();

	//	Process all MAX mesh nodes
	bool ProcessAllMeshNodes(CSkeletonCandidate* pSkeletonCandidate, int iMaxBone, float fMinWeight);

	//	Get mesh number
	int GetMeshNum() { return m_aMeshes.GetSize(); }
	//	Get texture number
	int GetTextureNum() { return m_aTexNames.GetSize(); }
	//	Get material number
	int GetMaterialNum() { return m_aMaterials.GetSize(); }
	//	Get mesh by index
	CMeshCandidate* GetMesh(int n) { return m_aMeshes[n]; }
	//	Get texture name by index
	const char* GetTextureName(int n) { return *m_aTexNames[n]; }
	//	Get material by index
	A3DMaterial* GetMaterial(int n) { return m_aMaterials[n]; }

	//	Get maximum bone counter
	int GetMaxBone() { return m_iMaxBone; }
	//	Get minimum weight
	float GetMinWeight() { return m_fMinWeight; }
	//	Set / Get extra texture directory flag
	void SetExtraTexDirFlag(bool bTrue) { m_bExtraTexDir = bTrue; }
	bool GetExtraTexDirFlag() { return m_bExtraTexDir; }

	//	Get bone bounding box number
	int GetBoneBoxNum() { return m_aBoneBoxes.GetSize(); }
	//	Get bone bounding box information
	const BONEBOX& GetBoneBox(int iBone) { return m_aBoneBoxes[iBone]; }

	//	Get MAX mesh number
	int GetMAXMeshNum() { return m_aMAXMeshes.GetSize(); }
	//	Get MAX mesh object
	CMAXMesh* GetMAXMesh(int n) { return m_aMAXMeshes[n]; }

protected:	//	Attributes

	APtrArray<CMAXMesh*>		m_aMAXMeshes;	//	MAX meshes array
	APtrArray<CMeshCandidate*>	m_aMeshes;		//	Mesh candidates
	APtrArray<AString*>			m_aTexNames;	//	Texture names
	APtrArray<A3DMaterial*>		m_aMaterials;	//	Materials

	AArray<BONEBOX, BONEBOX&>	m_aBoneBoxes;	//	Bone bounding bones

	int		m_iMaxBone;
	float	m_fMinWeight;
	bool	m_bExtraTexDir;		//	Create extra texture directory

	CSkeletonCandidate*		m_pSkeletonCandidate;

	//	Used for debug
	CMAXMesh*	m_pMaxMesh;

protected:	//	Operations

	//	Add a texture
	int AddTexture(const char* szTexName);
	//	Add a material
	int AddMaterial(A3DMaterial* pMaterial);
	//	Create mesh candidates from MAX node
	bool CreateMeshCandidates(CMAXMesh* pMAXMesh);
	//	Calculate AABBs for bones
	bool CalculateAABBs(CSkeletonCandidate* pSkeletonCandidate);
	//	Merge mesh
	bool MergeMesh(CMeshCandidate* pNewMesh);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SKINCANDIDATE_H_

