/*
* FILE: TrailList.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/04/23
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "TrailList.h"
#include <A3DHermiteSpline.h>

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement ITrailList
//	
///////////////////////////////////////////////////////////////////////////

TRAIL_DATA& TRAIL_DATA::operator = (const TRAIL_DATA& src)
{
	if (&src == this)
		return *this;
	
	m_vSeg1		= src.m_vSeg1;
	m_vSeg2		= src.m_vSeg2;
	m_nSegLife	= src.m_nSegLife;
	m_Color		= src.m_Color;
	return *this;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement ITrailList
//	
///////////////////////////////////////////////////////////////////////////


ITrailList* ITrailList::CreateTrailList(bool bIsSplineMode)
{
	if (bIsSplineMode)
		return new SplineTrailList();
	else
		return new LineTrailList();
}

void ITrailList::DestroyTrailList(ITrailList* pTrailImpl)
{
	if (!pTrailImpl)
		return;
	
	delete pTrailImpl;
}



//////////////////////////////////////////////////////////////////////////
//
//	Implement SplineTrailList
//
//////////////////////////////////////////////////////////////////////////

INT SplineTrailList::DETAIL_POINTFACTOR = 10;
FLOAT SplineTrailList::DETAIL_INVPOINTFACTOR = 1 / 10.f;

SplineTrailList::SplineTrailList()
{
	m_pTrailSpline[0] = new A3DHermiteSpline;
	m_pTrailSpline[1] = new A3DHermiteSpline;
	m_nStartIdx = 0;
}

SplineTrailList::~SplineTrailList()
{
	delete m_pTrailSpline[0];
	delete m_pTrailSpline[1];
	m_pTrailSpline[0] = m_pTrailSpline[1] = NULL;
}

int SplineTrailList::GetCount() const
{
	return GetKPNum();
}

void SplineTrailList::AddKeyData(const TRAIL_DATA& keyData)
{
	m_aKPData.push_back(KPData(keyData.m_nSegLife, keyData.m_Color));
	m_pTrailSpline[0]->AddPoint(keyData.m_vSeg1);
	m_pTrailSpline[1]->AddPoint(keyData.m_vSeg2);
}

void SplineTrailList::UpdateRecentKeyData(const TRAIL_DATA& keyData)
{
	int nIdx = static_cast<int>(m_aKPData.size() - 1);
	if (nIdx < 0)
	{
		AddKeyData(keyData);
		return;
	}

	KPData& kpData = m_aKPData[nIdx];
	kpData.m_nSegLife = keyData.m_nSegLife;
	m_pTrailSpline[0]->UpdatePoint(nIdx, keyData.m_vSeg1);
	m_pTrailSpline[1]->UpdatePoint(nIdx, keyData.m_vSeg2);
}

int SplineTrailList::GetRenderCount() const
{
	return GetKPNum() * DETAIL_POINTFACTOR;
}

void SplineTrailList::GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const
{
	int iKPIdx = iRenderIdx / DETAIL_POINTFACTOR;
	float t = (iRenderIdx % DETAIL_POINTFACTOR) * DETAIL_INVPOINTFACTOR;
	td.m_vSeg1 = InterpPos(0, iKPIdx, t);
	td.m_vSeg2 = InterpPos(1, iKPIdx, t);
	td.m_nSegLife = InterpSegLife(iKPIdx, t);
	td.m_Color = InterpColor(iKPIdx, t);
}

void SplineTrailList::Reset()
{
	m_nStartIdx = 0;
	m_pTrailSpline[0]->Clear();
	m_pTrailSpline[1]->Clear();
	m_aKPData.clear();
}

void SplineTrailList::Tick(DWORD dwTickDelta)
{
	for (size_t nIdx = 0; nIdx < m_aKPData.size(); ++nIdx)
	{
		m_aKPData[nIdx].m_nSegLife -= dwTickDelta;

		// 更新 m_nStartIdx, 是第一个仍然活着的关键点的Index
		if (m_aKPData[nIdx].m_nSegLife <= 0)
			m_nStartIdx = nIdx + 1;
	}

	if (m_aKPData.size() > 256 && m_nStartIdx > m_aKPData.size() / 2)
	{
		m_aKPData.erase(m_aKPData.begin(), m_aKPData.begin() + m_nStartIdx);
		m_pTrailSpline[0]->Erase(0, m_nStartIdx);
		m_pTrailSpline[1]->Erase(0, m_nStartIdx);
		m_nStartIdx = 0;
	}
}

size_t SplineTrailList::GetKPNum() const
{
	return m_aKPData.size() - m_nStartIdx;
}

A3DVECTOR3 SplineTrailList::GetKPPosition(int nSpline, size_t nKPIndex) const
{
	unsigned short nMappedIndex = unsigned short(nKPIndex + m_nStartIdx);
	assert(nMappedIndex < m_pTrailSpline[nSpline]->GetNumPoints() && "nKPIndex is out of bound!!");
	return m_pTrailSpline[nSpline]->GetPoint(nMappedIndex);
}

A3DVECTOR3 SplineTrailList::InterpPos(int nSpline, size_t nKPIndex, FLOAT t) const
{
	unsigned short nMappedIndex = unsigned short(nKPIndex + m_nStartIdx);
	assert(nMappedIndex < m_pTrailSpline[nSpline]->GetNumPoints() && "nKPIndex is out of bound!!");
	return m_pTrailSpline[nSpline]->Interpolate(nMappedIndex, t);
}

A3DCOLOR SplineTrailList::InterpColor(size_t nKPIndex, FLOAT t) const
{
	size_t nMappedIndex = nKPIndex + m_nStartIdx;
	assert(nMappedIndex < m_aKPData.size() && "nKPIndex is out of bound!!");
	if (nMappedIndex < m_aKPData.size() - 1)
		return Interp_Color(m_aKPData[nMappedIndex].m_Color, m_aKPData[nMappedIndex + 1].m_Color, t);

	return m_aKPData[nMappedIndex].m_Color;
}

int SplineTrailList::InterpSegLife(size_t nKPIndex, FLOAT t) const 
{
	size_t nMappedIndex = nKPIndex + m_nStartIdx;
	assert(nMappedIndex < m_aKPData.size() && "nKPIndex is out of bound!!");
	if (nMappedIndex < m_aKPData.size() - 1)
		return m_aKPData[nMappedIndex].m_nSegLife + int((m_aKPData[nMappedIndex + 1].m_nSegLife - m_aKPData[nMappedIndex].m_nSegLife) * t);
	
	return m_aKPData[nMappedIndex].m_nSegLife;
}

//////////////////////////////////////////////////////////////////////////
//
//	LineTrailList
//
//////////////////////////////////////////////////////////////////////////

LineTrailList::LineTrailList()
: m_TrailLst(SEG_SIZE_DEFAULT)
{
}

int LineTrailList::GetCount() const
{
	return m_TrailLst.GetDataCount();
}

void LineTrailList::AddKeyData(const TRAIL_DATA& keyData)
{
	m_TrailLst.AddData(keyData);
}

void LineTrailList::UpdateRecentKeyData(const TRAIL_DATA& keyData)
{
	int nIdx = m_TrailLst.GetDataCount() - 1;
	if (nIdx < 0)
		return;

	m_TrailLst[nIdx].m_nSegLife = keyData.m_nSegLife;
	m_TrailLst[nIdx].m_vSeg1 = keyData.m_vSeg1;
	m_TrailLst[nIdx].m_vSeg2 = keyData.m_vSeg2;
	m_TrailLst[nIdx].m_Color = keyData.m_Color;
}

void LineTrailList::Tick(DWORD dwTickDelta)
{
	int nCount = m_TrailLst.GetDataCount();
	for (int i = nCount-1; i >= 0; i--)
	{
		TRAIL_DATA& td = m_TrailLst[i];
		td.m_nSegLife -= dwTickDelta;

		if (td.m_nSegLife <= 0)
		{
			m_TrailLst.RemoveOldData(i+1);
			break;
		}
	}
}

void LineTrailList::Reset()
{
	m_TrailLst.RemoveAll();
}

int LineTrailList::GetRenderCount() const
{
	return m_TrailLst.GetDataCount();
}

void LineTrailList::GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const
{
	td = m_TrailLst[iRenderIdx];
}

LineTrailGravityList::LineTrailGravityList()
{
	memset(&m_GravityData,0,sizeof(m_GravityData));
	m_bGravity = false;
}

LineTrailGravityList::~LineTrailGravityList()
{
	
}

void LineTrailGravityList::AddKeyData( const TRAIL_DATA& keyData )
{
	if (m_bGravity)
	{
		float fNoise = m_GravityData.m_fAmplitude * sin(GetTickCount() * m_GravityData.m_fHSpeed * 0.001f);
		float x = m_GravityData.m_bXNoise ? fNoise : 0;
		float z = m_GravityData.m_bZNoise ? fNoise : 0;
		A3DVECTOR3 vOffset = A3DVECTOR3(x,0,z);
		TRAIL_DATA data = keyData;
		data.m_vSeg1 += vOffset;
		data.m_vSeg2 += vOffset;
		m_TrailLst.AddData(data);
	}
	else
	{
		m_TrailLst.AddData(keyData);
	}
}

void LineTrailGravityList::Tick( DWORD dwTickDelta )
{
	if (m_bGravity)
		TickGravity(dwTickDelta);
	else
		LineTrailList::Tick(dwTickDelta);
}

void LineTrailGravityList::SetGravityData( const GRAVITY_DATA& gravityData )
{
	m_GravityData = gravityData;
	if (m_GravityData.m_fVerticalNoise < 0.0f)
		m_GravityData.m_fVerticalNoise = 0.0f;
	if (m_GravityData.m_fVerticalNoise > 1.0f)
		m_GravityData.m_fVerticalNoise = 1.0f;
}

void LineTrailGravityList::UseGravity( bool bUse )
{
	m_bGravity=bUse;
}

void LineTrailGravityList::TickGravity( DWORD dwTickDelta )
{
	int nCount = m_TrailLst.GetDataCount();
	for (int i = nCount-1; i >= 0; i--)
	{
		TRAIL_DATA& td = m_TrailLst[i];
		td.m_nSegLife -= dwTickDelta;
		
		if (td.m_nSegLife <= 0)
		{
			m_TrailLst.RemoveOldData(i+1);
			break;
		}
		
		AffectGravity(i, m_GravityData.m_fGravity);
	}
}

void LineTrailGravityList::AffectGravity( int nIdx,float fGravity )
{
	float fNoise = fGravity * (1-m_GravityData.m_fVerticalNoise + m_GravityData.m_fVerticalNoise*fabs(sin(GetTickCount() * m_GravityData.m_fVSpeed * 0.001f)));
	
	A3DVECTOR3 v0 = m_TrailLst[nIdx].m_vSeg1;
	A3DVECTOR3 v1 = m_TrailLst[nIdx].m_vSeg2;
	
	v0.y -= fNoise;
	v1.y -= fNoise;
	
	m_TrailLst[nIdx].m_vSeg1 = v0;
	m_TrailLst[nIdx].m_vSeg2 = v1;
}


