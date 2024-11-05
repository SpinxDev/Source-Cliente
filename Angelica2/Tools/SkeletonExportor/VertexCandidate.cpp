/*
 * FILE: VertexCandidate.cpp
 *
 * DESCRIPTION: Vertex candidate class
 *
 * CREATED BY: duyuxin, 2003/9/16
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "VertexCandidate.h"
#include "MAXInterface.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define	COMP_EPSILON	0.00001f

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
//	Implement CVertexCandidate
//
///////////////////////////////////////////////////////////////////////////

CVertexCandidate::CVertexCandidate()
{
	m_vPos.Clear();
	m_vNormal.Clear();

	m_ftu		= 0.0f;
	m_ftv		= 0.0f;
	m_bRigid	= false;
	m_iVertexID	= 0;
}

CVertexCandidate::~CVertexCandidate()
{
}

//	Add bone influence
void CVertexCandidate::AddInfluence(int iBone, float fWeight)
{
	int i;

	//	Check if there is already an influence for this bone (weird 3ds max behaviour =P)
	for (i=0; i < m_aInfluences.GetSize(); i++)
	{
		if (m_aInfluences[i].iBone == iBone)
		{
			m_aInfluences[i].fWeight += fWeight;
			break;
		}
	}

	//	Create an influence object if there is none for the given bone
	if (i == m_aInfluences.GetSize())
	{
		INFLUENCE Influence = {iBone, fWeight};
		m_aInfluences.Add(Influence);
	}

	//	Sort all the influences by weight in descend order
	for (i=0; i < m_aInfluences.GetSize()-1; i++)
	{
		float fMaxWeight = m_aInfluences[i].fWeight;
		int iExchange = i;

		for (int j=i+1; j < m_aInfluences.GetSize(); j++)
		{
			if (m_aInfluences[j].fWeight > fMaxWeight)
			{
				fMaxWeight  = m_aInfluences[j].fWeight;
				iExchange	= j;
			}
		}

		if (iExchange != i)
		{
			INFLUENCE Temp = m_aInfluences[i];
			m_aInfluences[i] = m_aInfluences[iExchange];
			m_aInfluences[iExchange] = Temp;
		}
	}
}

//	Compare function
bool CVertexCandidate::operator == (const CVertexCandidate& Vertex)
{
	//	Compare the position
	if (fabs(m_vPos.x - Vertex.m_vPos.x) >= COMP_EPSILON ||
		fabs(m_vPos.y - Vertex.m_vPos.y) >= COMP_EPSILON ||
		fabs(m_vPos.z - Vertex.m_vPos.z) >= COMP_EPSILON)
		return false;

	//	Compare the normal
	if (fabs(m_vNormal.x - Vertex.m_vNormal.x) >= COMP_EPSILON ||
		fabs(m_vNormal.y - Vertex.m_vNormal.y) >= COMP_EPSILON ||
		fabs(m_vNormal.z - Vertex.m_vNormal.z) >= COMP_EPSILON)
		return false;

	if (fabs(m_ftu - Vertex.m_ftu) >= COMP_EPSILON ||
		fabs(m_ftu - Vertex.m_ftu) >= COMP_EPSILON)
		return false;

	return true;
}

//	Compare function don't consider normal
bool CVertexCandidate::CompareNoNormal(const CVertexCandidate& Vertex)
{
	//	Compare the position
	if (fabs(m_vPos.x - Vertex.m_vPos.x) >= COMP_EPSILON ||
		fabs(m_vPos.y - Vertex.m_vPos.y) >= COMP_EPSILON ||
		fabs(m_vPos.z - Vertex.m_vPos.z) >= COMP_EPSILON)
		return false;

	if (fabs(m_ftu - Vertex.m_ftu) >= COMP_EPSILON ||
		fabs(m_ftu - Vertex.m_ftu) >= COMP_EPSILON)
		return false;

	return true;
}

//	Adjust bone assignment
void CVertexCandidate::AdjustBoneAssignment(int iMaxBone, float fMinWeight)
{
	int i;

	//	Erase all influences below the weight threshold
	for (i=0; i < m_aInfluences.GetSize(); i++)
	{
		//	Check against the weight threshold
		if (m_aInfluences[i].fWeight < fMinWeight)
		{
			//	Because we have sort all weights in descend order, so remove
			//	all following weights
			m_aInfluences.RemoveAt(i, m_aInfluences.GetSize() - i);
			break;
		}
	}

	//	Erase all but the first few influences specified by max bone count
	if (m_aInfluences.GetSize() > iMaxBone)
	{
		int iErase = m_aInfluences.GetSize() - iMaxBone;
		m_aInfluences.RemoveAt(m_aInfluences.GetSize() - iErase, iErase);
	}

	if (!m_aInfluences.GetSize())
	{
		//	No bone effect this vertex
		return;
	}

	//	Get the total weight of the influence
	float fWeight = 0.0f;

	for (i=0; i < m_aInfluences.GetSize(); i++)
		fWeight += m_aInfluences[i].fWeight;

	//	Weight is too small
	if (fWeight < COMP_EPSILON)
	{
		m_aInfluences.RemoveAll();
		return;
	}
	else
	{
		//	Normalize all influence weights
		for (i=0; i < m_aInfluences.GetSize(); i++)
			m_aInfluences[i].fWeight /= fWeight;
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CSpringVertex
//
///////////////////////////////////////////////////////////////////////////

CSpringVertex::CSpringVertex()
{
	m_PhysicalProp.fBoneFactor		= 0.0f;
	m_PhysicalProp.iSpringCnt		= 0;
	m_PhysicalProp.iConstraintDist	= -1;
}

CSpringVertex::~CSpringVertex()
{
}

//	Set Physical property
void CSpringVertex::SetPhysicalProperty(float fBoneFactor, int iSpringCnt, int iConstraintDist)
{
	m_PhysicalProp.fBoneFactor		= fBoneFactor;
	m_PhysicalProp.iSpringCnt		= iSpringCnt;
	m_PhysicalProp.iConstraintDist	= iConstraintDist;
}

