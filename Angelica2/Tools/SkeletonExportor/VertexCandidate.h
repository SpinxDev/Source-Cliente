/*
 * FILE: VertexCandidate.h
 *
 * DESCRIPTION: Vertex candidate class
 *
 * CREATED BY: duyuxin, 2003/9/16
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _VERTEXCANDIDATE_H_
#define _VERTEXCANDIDATE_H_

#include "max.h"
#include "A3DTypes.h"
#include "AArray.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CVertexCandidate
//
///////////////////////////////////////////////////////////////////////////

class CVertexCandidate
{
public:		//	Types

	//	Bone influence
	struct INFLUENCE
	{
		int		iBone;
		float	fWeight;
	};
	
public:		//	Constructions and Destructions

	CVertexCandidate();
	virtual ~CVertexCandidate();

public:		//	Attributes

	//	m_vSmoothNormal and m_iNormalCnt are used to calculate smooth normal
	A3DVECTOR3	m_vSmoothNormal;	
	int			m_iNormalCnt;

public:		//	Operaitons

	//	Adjust bone assignment
	void AdjustBoneAssignment(int iMaxBone, float fMinWeight);

	//	Compare function
	bool operator == (const CVertexCandidate& Vertex);
	//	Compare function don't consider normal
	bool CompareNoNormal(const CVertexCandidate& Vertex);

	//	Add bone influence
	void AddInfluence(int iBone, float fWeight);
	//	Get influence number
	int GetInfluenceNum() { return m_aInfluences.GetSize(); }
	//	Get influence
	const INFLUENCE& GetInfluence(int n) { return m_aInfluences[n]; }

	//	Set / Get vertex position
	void SetPos(float x, float y, float z) { m_vPos.Set(x, y, z); }
	A3DVECTOR3 GetPos() { return m_vPos; }

	//	Set / Get vertex normal
	void SetNormal(float x, float y, float z) { m_vNormal.Set(x, y, z); }
	A3DVECTOR3 GetNormal() { return m_vNormal; }

	//	Set / Get normal texture coordinates
	void SetNormalTextureCoord(float u, float v) { m_ftu = u; m_ftv = v; }
	void GetNormalTextureCoord(float* pu, float* pv) { *pu = m_ftu; *pv = m_ftv; }

	//	Set / Get rigid flag
	void SetRigidFlag(bool bRigid) { m_bRigid = bRigid; }
	bool GetRigidFlag() { return m_bRigid; }

	//	Set / Get vertex ID
	void SetVertexID(int iID) { m_iVertexID = iID; }
	int GetVertexID() { return m_iVertexID; }

protected:	//	Attributes

	A3DVECTOR3	m_vPos;			//	Vertex position
	A3DVECTOR3	m_vNormal;		//	Vertex normal

	float		m_ftu;			//	Normal texture coordinates
	float		m_ftv;

	bool		m_bRigid;		//	This vertex belong to a rigid mesh
	int			m_iVertexID;	//	Vertex ID in MAX mesh

	AArray<INFLUENCE, INFLUENCE&>	m_aInfluences;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class CSpringVertex
//
///////////////////////////////////////////////////////////////////////////

class CSpringVertex : public CVertexCandidate
{
public:		//	Types

	//	Physical property
	struct PHYSICALPROP
	{
		float	fBoneFactor;		//	Bone effect factor. 0.0: constraint vertex.
		int		iSpringCnt;			//	Number of sping contected to this vertex
		int		iConstraintDist;	//	Step number to a constraint vertex
	};

public:		//	Constructions and Destructions

	CSpringVertex();
	virtual ~CSpringVertex();

public:		//	Attributes

public:		//	Operaitons

	//	Add vertex neighbour
	void AddNeighbour(int iNeighbour) { m_aNeighbours.UniquelyAdd(iNeighbour); }
	//	Add bone influence
	void AddInfluence(const INFLUENCE& Influence) { m_aInfluences.Add((INFLUENCE&)Influence); }

	//	Set / Get Physical property
	void SetPhysicalProperty(float fBoneFactor, int iSpringCnt, int iConstraintDist);
	void SetPhysicalProperty(const PHYSICALPROP& Props) { m_PhysicalProp = Props; }
	const PHYSICALPROP& GetPhysicalProperty() { return m_PhysicalProp; }

	//	Get neighbour number
	int GetNeighbourNum() { return m_aNeighbours.GetSize(); }
	//	Get #n neighbour 
	int GetNeighbour(int n) { return m_aNeighbours[n]; }
	//	Get neighbour array
	AArray<int, int>& GetNeighbourArray() { return m_aNeighbours; }

protected:	//	Attributes

	PHYSICALPROP		m_PhysicalProp;		//	Physical property
	AArray<int, int>	m_aNeighbours;		//	Vertex neighbours

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_VERTEXCANDIDATE_H_

